/*
 * ======================================================================================
 * File:        kms_Frame_Saver_Plugin.c
 *
 * Purpose:     Kurento+Gstreamer plugin-filter --- uses "Frame-Saver" for full behavior.
 *
 * History:     1. 2016-11-25   JBendor     Created as copy of "gst_Frame_Saver_Plugin.c"
 *              2. 2016-11-25   JBendor     Adapted to _IS_KURENTO_FILTER_ being defined
 *              3. 2016-12-13   JBendor     Updated
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * ======================================================================================
 */





#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "kms_Frame_Saver_Plugin.h"
#include <string.h>
#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/video/gstvideofilter.h>
#include <glib/gstdio.h>


GST_DEBUG_CATEGORY_STATIC       (kms_frame_saver_plugin_debug_category);
#define GST_CAT_DEFAULT         kms_frame_saver_plugin_debug_category


typedef enum
{
    e_PROP_0,

    e_PROP_WAIT,    // "wait=MillisWaitBeforeNextFrameSnap"
    e_PROP_SNAP,    // "snap=MillisIntervals,MaxNumSnaps,MaxNumFails"
    e_PROP_LINK,    // "link=PipelineName,ProducerName,ConsumerName"
    e_PROP_PADS,    // "pads=ProducerOut,ConsumerInput,ConsumerOut"
    e_PROP_PATH,    // "path=PathForWorkingFolderForSavedImageFiles"
    e_PROP_NOTE,    // "note=none or note=MostRecentError"
    e_PROP_SILENT,   // silent=0 or silent=1 --- 1 disables messages

    e_PROP_SHOW_DEBUG_INFO,
    e_PROP_WINDOWS_LAYOUT,
    e_PROP_MESSAGE,
    e_PROP_SHOW_WINDOWS_LAYOUT,
    e_PROP_CALIBRATION_AREA

} PLUGIN_PARAMS_e;


enum
{
    e_FIRST_SIGNAL = 0,
    e_FINAL_SIGNAL

} PLUGIN_SIGNALS_e;


typedef struct _KmsFrameSaverPluginPrivate
{
    gboolean     is_silent, show_debug_info, putMessage, show_windows_layout;
    guint        num_buffs;
    guint        num_drops;
    guint        num_notes;
    gchar        sz_wait[30],
                 sz_snap[30],
                 sz_link[90],
                 sz_pads[90],
                 sz_path[400],
                 sz_note[200];

} KmsFrameSaverPluginPrivate;

#define VIDEO_SRC_CAPS      GST_VIDEO_CAPS_MAKE("{ BGR }")
#define VIDEO_SINK_CAPS     GST_VIDEO_CAPS_MAKE("{ BGR }")

extern GType    kms_frame_saver_plugin_get_type(void);     // body defined by macro: G_DEFINE_TYPE
static void     kms_frame_saver_plugin_init (KmsFrameSaverPlugin * aPluginPtr);  // initialize instance


G_DEFINE_TYPE_WITH_CODE (KmsFrameSaverPlugin,                                               \
                         kms_frame_saver_plugin,                                            \
                         GST_TYPE_VIDEO_FILTER,                                             \
                         GST_DEBUG_CATEGORY_INIT (kms_frame_saver_plugin_debug_category,    \
                                                  THIS_PLUGIN_NAME, 0,                      \
                                                  "debug category for FrameSaverPlugin element"));

#define GET_PRIVATE_STRUCT_PTR(obj)    (G_TYPE_INSTANCE_GET_PRIVATE((obj),                       \
                                                                    KMS_TYPE_FRAME_SAVER_PLUGIN, \
                                                                    KmsFrameSaverPluginPrivate))


#ifdef _SAVE_IMAGE_FRAMES_

    extern int Frame_Saver_Filter_Attach(GstElement * pluginPtr);
    extern int Frame_Saver_Filter_Detach(GstElement * pluginPtr);
    extern int Frame_Saver_Filter_Receive_Buffer(GstElement * pluginPtr, GstBuffer * aBufferPtr);
    extern int Frame_Saver_Filter_Transition(GstElement * pluginPtr, GstStateChange aTransition) ;
    extern int Frame_Saver_Filter_Set_Params(GstElement * pluginPtr, const gchar * aNewValuePtr, gchar * aPrvSpecsPtr);

#else

    static int Frame_Saver_Filter_Attach(GstElement * pluginPtr)
    {
        g_print("%s --- %s \n", THIS_PLUGIN_NAME, __func__);
        return 0;
    }
    static int Frame_Saver_Filter_Detach(GstElement * pluginPtr)
    {
        g_print("%s --- %s \n", THIS_PLUGIN_NAME, __func__);
        return 0;
    }
    static int Frame_Saver_Filter_Receive_Buffer(GstElement * pluginPtr, GstBuffer * aBufferPtr)
    {
        g_print("%s --- %s \n", THIS_PLUGIN_NAME, __func__);
        return 0;
    }
    static int Frame_Saver_Filter_Transition(GstElement * pluginPtr, GstStateChange aTransition)
    {
        g_print("%s --- %s \n", THIS_PLUGIN_NAME, __func__);
        return 0;
    }
    static int Frame_Saver_Filter_Set_Params(GstElement * pluginPtr, const gchar * aNewValuePtr, gchar * aPrvSpecsPtr)
    {
        g_print("%s --- %s \n", THIS_PLUGIN_NAME, __func__);
        return 0;
    }

#endif


static void initialize_plugin_instance(KmsFrameSaverPlugin * aPluginPtr, KmsFrameSaverPluginPrivate * aPrivatePtr);


static GstClock      * The_Sys_Clock_Ptr = NULL;
static GstClockTime    The_Startup_Nanos = 0;


static guint64 Get_Runtime_Nanosec()
{
    if (The_Sys_Clock_Ptr == NULL)
    {
        The_Sys_Clock_Ptr = gst_system_clock_obtain();

        The_Startup_Nanos = gst_clock_get_time (The_Sys_Clock_Ptr);

        g_printf("\n%s\n", THIS_PLUGIN_NAME);
    }

    GstClockTime  now_nanos = gst_clock_get_time( The_Sys_Clock_Ptr );

    GstClockTime elapsed_ns = now_nanos -The_Startup_Nanos;

    return (guint64) elapsed_ns;
}


static guint Get_Runtime_Millisec()
{
    #define  NANOS_PER_MILLISEC  ((guint64) (1000L * 1000L))

    return (guint) (Get_Runtime_Nanosec() / NANOS_PER_MILLISEC);
}


static guint DBG_Print(const gchar * aTextPtr, gint aValue)
{
    guint elapsed_ms = Get_Runtime_Millisec();

    if (aTextPtr != NULL)
    {
        g_printf("%-7d --- %s --- %s --- (%d)", elapsed_ms, THIS_PLUGIN_NAME, aTextPtr, aValue);
    }

    g_printf("\n");

    return elapsed_ms;
}


static void kms_frame_saver_plugin_init(KmsFrameSaverPlugin * aPluginPtr)
{
    The_Sys_Clock_Ptr = NULL;
    DBG_Print( __func__, 0 );

    initialize_plugin_instance(aPluginPtr, GET_PRIVATE_STRUCT_PTR(aPluginPtr));

    return;
}


static void kms_frame_saver_plugin_set_property(GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec)
{
    const gchar * psz_now = NULL;

    KmsFrameSaverPlugin        *  ptr_filter = KMS_FRAME_SAVER_PLUGIN(object);

    KmsFrameSaverPluginPrivate * ptr_private = GET_PRIVATE_STRUCT_PTR(ptr_filter);

    DBG_Print( __func__, prop_id );

    GST_OBJECT_LOCK(ptr_filter);

    switch (prop_id)
    {
    case e_PROP_SILENT:
        ptr_private->is_silent = g_value_get_boolean(value);
        psz_now = ptr_private->is_silent ? "Silent=TRUE" : "Silent=FALSE";
        break;

    case e_PROP_WAIT:
        snprintf( ptr_private->sz_wait, sizeof(ptr_private->sz_wait), "wait=%s", g_value_get_string(value) );
        psz_now = ptr_private->sz_wait;
        break;

    case e_PROP_SNAP:
        snprintf( ptr_private->sz_snap, sizeof(ptr_private->sz_snap), "snap=%s", g_value_get_string(value) );
        psz_now = ptr_private->sz_snap;
        break;

    case e_PROP_LINK:
        snprintf( ptr_private->sz_link, sizeof(ptr_private->sz_link), "link=%s", g_value_get_string(value) );
        psz_now = ptr_private->sz_link;
        break;

    case e_PROP_PADS:
        snprintf( ptr_private->sz_pads, sizeof(ptr_private->sz_pads), "pads=%s", g_value_get_string(value) );
        psz_now = ptr_private->sz_pads;
        break;

    case e_PROP_PATH:
        snprintf( ptr_private->sz_path, sizeof(ptr_private->sz_path), "path=%s", g_value_get_string(value) );
        psz_now = ptr_private->sz_path;
        break;

    case e_PROP_SHOW_DEBUG_INFO:
        ptr_private->show_debug_info = g_value_get_boolean (value);
        break;

    case e_PROP_WINDOWS_LAYOUT:
        break;

    case e_PROP_MESSAGE:
        ptr_private->putMessage = g_value_get_boolean (value);
        break;

    case e_PROP_SHOW_WINDOWS_LAYOUT:
        ptr_private->show_windows_layout = g_value_get_boolean (value);
        break;

    case e_PROP_CALIBRATION_AREA:
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }

    if (psz_now != NULL)
    {
        g_print("%s --- Property #%u --- Now: (%s) \n", THIS_PLUGIN_NAME, (guint) prop_id, psz_now);

        ptr_private->num_buffs = 0;
        ptr_private->num_drops = 0;
        ptr_private->num_notes = 0;
    }

    GST_OBJECT_UNLOCK(ptr_filter);

    return;
}


static void kms_frame_saver_plugin_get_property(GObject * object, guint prop_id, GValue * value, GParamSpec * pspec)
{
    KmsFrameSaverPlugin        * ptr_filter = KMS_FRAME_SAVER_PLUGIN(object);

    KmsFrameSaverPluginPrivate * ptr_private = GET_PRIVATE_STRUCT_PTR(ptr_filter);

    DBG_Print( __func__, prop_id );

    GST_OBJECT_LOCK(ptr_filter);

    switch (prop_id)
    {
        case e_PROP_SILENT:
            g_value_set_boolean(value, ptr_private->is_silent);
            break;

        case e_PROP_WAIT:
            g_value_set_string(value, ptr_private->sz_wait);
            break;

        case e_PROP_SNAP:
            g_value_set_string(value, ptr_private->sz_snap);
            break;

        case e_PROP_LINK:
            g_value_set_string(value, ptr_private->sz_link);
            break;

        case e_PROP_PADS:
            g_value_set_string(value, ptr_private->sz_pads);
            break;

        case e_PROP_PATH:
            g_value_set_string(value, ptr_private->sz_path);
            break;

        case e_PROP_NOTE:
            g_value_set_string(value, ptr_private->sz_note);
            strcpy(ptr_private->sz_note, "note=none");
            break;

        case e_PROP_SHOW_DEBUG_INFO:
            g_value_set_boolean (value, ptr_private->show_debug_info);
            break;

        case e_PROP_WINDOWS_LAYOUT:
            break;

        case e_PROP_MESSAGE:
            g_value_set_boolean (value, ptr_private->putMessage);
            break;

        case e_PROP_SHOW_WINDOWS_LAYOUT:
            g_value_set_boolean (value, ptr_private->show_windows_layout);
            break;

        case e_PROP_CALIBRATION_AREA:
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }

    GST_OBJECT_UNLOCK(ptr_filter);

    return;
}


static void kms_frame_saver_plugin_finalize (GObject *object)
{
    DBG_Print( __func__, 0 );    DBG_Print( NULL, 0 );

    G_OBJECT_CLASS (kms_frame_saver_plugin_parent_class)->finalize(object);

    return;
}


static gboolean KMS_frame_saver_plugin_start (GstBaseTransform * aTransPtr)
{
    KmsFrameSaverPlugin * ptr_filter = KMS_FRAME_SAVER_PLUGIN(aTransPtr);

    DBG_Print( __func__, 0 );

    GST_DEBUG_OBJECT (ptr_filter, "start");

    return TRUE;
}


static gboolean KMS_frame_saver_plugin_stop (GstBaseTransform * aTransPtr)
{
    KmsFrameSaverPlugin * ptr_filter = KMS_FRAME_SAVER_PLUGIN(aTransPtr);

    DBG_Print( __func__, 0 );

    GST_DEBUG_OBJECT (ptr_filter, "stop");

    return TRUE;
}


static gboolean KMS_frame_saver_plugin_set_info ( GstVideoFilter  * aFilterPtr, 
                                                  GstCaps         * in_caps_ptr,  
                                                  GstVideoInfo    * in_info_ptr, 
                                                  GstCaps         * out_caps_ptr, 
                                                  GstVideoInfo    * out_info_ptr )
{
    KmsFrameSaverPlugin * ptr_filter = KMS_FRAME_SAVER_PLUGIN(aFilterPtr);

    DBG_Print( __func__, 0 );

    GST_DEBUG_OBJECT (ptr_filter, "set_info");

    return TRUE;
}


static GstFlowReturn KMS_frame_saver_plugin_transform_frame_ip(GstVideoFilter * aFilterPtr, GstVideoFrame * aFramePtr)
{
    static gint  num_frames = 0;

    DBG_Print( __func__, (aFramePtr == NULL) ? 0 : ++num_frames );

    return GST_FLOW_OK;
}


static void kms_frame_saver_plugin_class_init(KmsFrameSaverPluginClass * klass)
{
    #define PARAM_ATTRIBUTES (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | GST_PARAM_MUTABLE_READY)

    #ifdef _ALLOW_DYNAMIC_PARAMS_
        GParamFlags param_flags = (GParamFlags) (PARAM_ATTRIBUTES | GST_PARAM_CONTROLLABLE);
    #else
        GParamFlags param_flags = (GParamFlags) (PARAM_ATTRIBUTES);
    #endif

    GObjectClass            *        gobject_class_ptr = G_OBJECT_CLASS(klass);
    GstVideoFilterClass     *   video_filter_class_ptr = GST_VIDEO_FILTER_CLASS(klass);
    GstBaseTransformClass   * base_transform_class_ptr = GST_BASE_TRANSFORM_CLASS(klass);

    The_Sys_Clock_Ptr = NULL;    DBG_Print( __func__, 0 );

    gobject_class_ptr->set_property = kms_frame_saver_plugin_set_property;
    gobject_class_ptr->get_property = kms_frame_saver_plugin_get_property;
    gobject_class_ptr->finalize     = kms_frame_saver_plugin_finalize;

    base_transform_class_ptr->start = GST_DEBUG_FUNCPTR (KMS_frame_saver_plugin_start);
    base_transform_class_ptr->stop  = GST_DEBUG_FUNCPTR (KMS_frame_saver_plugin_stop);

    video_filter_class_ptr->set_info = GST_DEBUG_FUNCPTR (KMS_frame_saver_plugin_set_info);
    video_filter_class_ptr->transform_frame_ip = GST_DEBUG_FUNCPTR (KMS_frame_saver_plugin_transform_frame_ip);

    gst_element_class_add_pad_template (GST_ELEMENT_CLASS (klass),
                                        gst_pad_template_new ("src", 
                                                              GST_PAD_SRC, 
                                                              GST_PAD_ALWAYS,
                                                              gst_caps_from_string (VIDEO_SRC_CAPS)));

    gst_element_class_add_pad_template (GST_ELEMENT_CLASS (klass),
                                        gst_pad_template_new ("sink", 
                                                              GST_PAD_SINK, 
                                                              GST_PAD_ALWAYS,
                                                              gst_caps_from_string (VIDEO_SINK_CAPS)));

    g_object_class_install_property(gobject_class_ptr,
                                    e_PROP_WAIT,
                                    g_param_spec_string("wait",
                                                        "wait=MillisWaitBeforeNextFrameSnap",
                                                        "wait before snapping another frame",
                                                        "3000",
                                                        param_flags));

    g_object_class_install_property(gobject_class_ptr,
                                    e_PROP_SNAP,
                                    g_param_spec_string("snap",
                                                        "snap=millisecInterval,maxNumSnaps,maxNumFails",
                                                        "snap and save frames as RGB images in PNG files",
                                                        "1000,0,0",
                                                        param_flags));

    g_object_class_install_property(gobject_class_ptr,
                                    e_PROP_LINK,
                                    g_param_spec_string("link",
                                                        "link=pipelineName,producerName,consumerName",
                                                        "insert TEE between producer and consumer elements",
                                                        "auto,auto,auto",
                                                        param_flags));

    g_object_class_install_property(gobject_class_ptr,
                                    e_PROP_PADS,
                                    g_param_spec_string("pads",
                                                        "pads=producerOut,consumerInput,consumerOut",
                                                        "pads used by the producer and consumer elements",
                                                        "src,sink,src",
                                                        param_flags));

    g_object_class_install_property(gobject_class_ptr,
                                    e_PROP_PATH,
                                    g_param_spec_string("path",
                                                        "path=path-of-working-folder-for-saved-images",
                                                        "path of working folder for saved image files",
                                                        "auto",
                                                        param_flags));

    g_object_class_install_property(gobject_class_ptr,
                                    e_PROP_NOTE,
                                    g_param_spec_string("note",
                                                        "note=MostRecentErrorCodition",
                                                        "most recent error",
                                                        "none",
                                                        G_PARAM_READABLE));

    g_object_class_install_property(gobject_class_ptr,
                                    e_PROP_SILENT,
                                    g_param_spec_boolean("silent",
                                                         "Silent or Verbose",
                                                         "Silent is 1/True --- Verbose is 0/False",
                                                         FALSE,
                                                         G_PARAM_READWRITE));

    g_object_class_install_property (gobject_class_ptr, 
                                     e_PROP_SHOW_DEBUG_INFO,
                                     g_param_spec_boolean ("show-debug-region", 
                                                           "show debug region",
                                                           "show evaluation regions over the image", 
                                                           FALSE, 
                                                           G_PARAM_READWRITE));

    g_object_class_install_property (gobject_class_ptr, 
                                     e_PROP_WINDOWS_LAYOUT,
                                     g_param_spec_boxed ("windows-layout", 
                                                         "windows layout",
                                                         "supply the positions and dimensions of windows into the main window",
                                                         GST_TYPE_STRUCTURE, 
                                                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property (gobject_class_ptr, 
                                     e_PROP_MESSAGE,
                                     g_param_spec_boolean ("message", 
                                                           "message",
                                                           "Put a window-in or window-out message in the bus if " "an object enters o leaves a window", 
                                                           TRUE, 
                                                           G_PARAM_READWRITE));

    g_object_class_install_property (gobject_class_ptr, 
                                     e_PROP_SHOW_WINDOWS_LAYOUT,
                                     g_param_spec_boolean ("show-windows-layout", 
                                                           "show windows layout",
                                                           "show windows layout over the image", 
                                                           TRUE, 
                                                           G_PARAM_READWRITE));

    g_object_class_install_property (gobject_class_ptr, 
                                     e_PROP_CALIBRATION_AREA,
                                     g_param_spec_boxed ("calibration-area", 
                                                         "calibration area",
                                                         "define the window used to calibrate the color to track",
                                                         GST_TYPE_STRUCTURE, 
                                                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
      
    gst_element_class_set_details_simple(GST_ELEMENT_CLASS(klass),
                                         THIS_PLUGIN_NAME,                  // name to launch
                                         "Frame-Saver-Video-Filter",        // classification
                                         "Saves Frames (Can Insert TEE)",   // description
                                         "Author <<a.TODO@hostname.org>>"); // author info

    #ifdef _IS_KURENTO_FILTER_

        g_type_class_add_private (klass, sizeof (KmsFrameSaverPluginPrivate));
        
    #else   // regular Gstreamer plugin --- create static pads --- set callbacks

        gst_element_class_add_pad_template(GST_ELEMENT_CLASS(klass), gst_static_pad_template_get(&The_Src_Pad_Template));

        gst_element_class_add_pad_template(GST_ELEMENT_CLASS(klass), gst_static_pad_template_get(&The_Sink_Pad_Template));

        GST_ELEMENT_CLASS(kms_frame_saver_plugin_parent_class)->change_state = kms_frame_saver_plugin_change_state;

        GST_ELEMENT_CLASS(klass)->change_state = kms_frame_saver_plugin_change_state;
        
    #endif  // _IS_KURENTO_FILTER_

    return;
}


/*
 * entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean register_this_plugin(GstPlugin * aPluginPtr)
{
    The_Sys_Clock_Ptr = NULL;
    DBG_Print( __func__, 0 );

    #ifdef _ALLOW_DYNAMIC_PARAMS_
        gst_controller_init (NULL, NULL);
    #endif

    return gst_element_register (aPluginPtr, THIS_PLUGIN_NAME, GST_RANK_NONE, KMS_TYPE_FRAME_SAVER_PLUGIN);
}


// gstreamer looks for this structure to register plugins
GST_PLUGIN_DEFINE ( GST_VERSION_MAJOR, 
                    GST_VERSION_MINOR,
                    FrameSaverPlugin, 
                    "saves-image-frames", 
                    register_this_plugin,
                    PLUGIN_VERSION, "LGPL", "GStreamer", "http://gstreamer.net/")


static void initialize_plugin_instance(KmsFrameSaverPlugin * aPluginPtr, KmsFrameSaverPluginPrivate * aPrivatePtr)
{
    strcpy(aPrivatePtr->sz_wait, "wait=2000");
    strcpy(aPrivatePtr->sz_snap, "snap=0,0,0");
    strcpy(aPrivatePtr->sz_link, "link=Live,auto,auto");
    strcpy(aPrivatePtr->sz_pads, "pads=auto,auto,auto");
    strcpy(aPrivatePtr->sz_path, "path=auto");
    strcpy(aPrivatePtr->sz_note, "note=none");
    
    aPrivatePtr->num_buffs = 0;
    aPrivatePtr->num_drops = 0;    
    aPrivatePtr->num_notes = 0;
    aPrivatePtr->is_silent = TRUE;

    aPrivatePtr->show_debug_info    = FALSE;
    aPrivatePtr->putMessage         = TRUE;
    aPrivatePtr->show_windows_layout= TRUE;

    #ifdef _IS_KURENTO_FILTER_

        aPluginPtr->priv = aPrivatePtr;

    #else   // regular Gstreamer plugin --- create pads --- set callbacks --- set video properties

        ptr_private->srcpad  = gst_pad_new_from_static_template( &The_Src_Pad_Template, "src" );        
        ptr_private->sinkpad = gst_pad_new_from_static_template( &The_Sink_Pad_Template, "sink" );

        gst_pad_set_event_function(ptr_private->sinkpad, GST_DEBUG_FUNCPTR(kms_frame_saver_plugin_sink_event));
        gst_pad_set_chain_function(ptr_private->sinkpad, GST_DEBUG_FUNCPTR(kms_frame_saver_plugin_chain));
        gst_pad_set_query_function(ptr_private->sinkpad, GST_DEBUG_FUNCPTR(kms_frame_saver_plugin_src_query));

        GST_PAD_SET_PROXY_CAPS(ptr_private->sinkpad);
        GST_PAD_SET_PROXY_CAPS(ptr_private->srcpad);

    #endif

    if (aPluginPtr == NULL)    // always FALSE, suppress warnings on unused statics
    {
        Frame_Saver_Filter_Attach(NULL);
        Frame_Saver_Filter_Detach(NULL);
        Frame_Saver_Filter_Transition(NULL, 0) ;
        Frame_Saver_Filter_Receive_Buffer(NULL, NULL);
        Frame_Saver_Filter_Set_Params(NULL, NULL, NULL);
        
        KMS_frame_saver_plugin_transform_frame_ip(NULL, NULL);

        #ifndef _IS_KURENTO_FILTER_
            kms_frame_saver_plugin_src_query(NULL, NULL, NULL);
            kms_frame_saver_plugin_chain(NULL, NULL, NULL);
            kms_frame_saver_plugin_sink_event(NULL, NULL, NULL);
            kms_frame_saver_plugin_change_state(NULL, GST_STATE_CHANGE_READY_TO_NULL);
        #endif
    }  

    return;
}


#ifndef _IS_KURENTO_FILTER_

static gboolean kms_frame_saver_plugin_sink_event(GstPad * pad, GstObject * parent, GstEvent * aEventPtr)
{
    gboolean is_ok;
    
    GstCaps * ptr_caps;

    DBG_Print( __func__, (gint) GST_EVENT_TYPE(aEventPtr) );

    switch (GST_EVENT_TYPE(aEventPtr))
    {
    case GST_EVENT_CAPS: // do something with the caps --- forward
        gst_event_parse_caps(aEventPtr, & ptr_caps);
        is_ok = gst_pad_event_default(pad, parent, aEventPtr);
        break;

    case GST_EVENT_EOS: // end-of-stream, close down all stream leftovers here
        is_ok = gst_pad_event_default(pad, parent, aEventPtr);
        break;

    default:
        is_ok = gst_pad_event_default(pad, parent, aEventPtr);
        break;
    }

    return is_ok;
}


// Gstreamer chain function --- this function does the actual processing
static GstFlowReturn kms_frame_saver_plugin_chain(GstPad * pad, GstObject * parent, GstBuffer * buf)
{
    KmsFrameSaverPlugin        *  ptr_filter = KMS_FRAME_SAVER_PLUGIN(parent);

    KmsFrameSaverPluginPrivate * ptr_private = GET_PRIVATE_STRUCT_PTR(ptr_filter);

    #ifdef _IS_KURENTO_FILTER_
        GstFlowReturn  result = GST_FLOW_OK;
    #else
        GstFlowReturn  result = gst_pad_push(ptr_private->srcpad, buf);
    #endif

    ptr_private->num_drops += (result == GST_FLOW_OK) ? 0 : 1;    
    ptr_private->num_buffs += 1;

    DBG_Print( __func__, (gint) ptr_private->num_buffs );

    if (ptr_private->is_silent == FALSE)
    {
        g_print("%s --- Push --- (%u/%u) \n", THIS_PLUGIN_NAME, ptr_private->num_buffs, ptr_private->num_drops);
    }    
    
    if ( ptr_private->sz_snap[5] > '0')  // is_frame_saver)
    {
        Frame_Saver_Filter_Receive_Buffer(GST_ELEMENT(ptr_filter), buf);
    }
    
    return result;  // anythin except GST_FLOW_OK could halt flow in the pipeline
}


static gboolean kms_frame_saver_plugin_src_query(GstPad *pad, GstObject *parent, GstQuery *query)
{
    gboolean ret;

    DBG_Print( __func__, (gint) GST_QUERY_TYPE(query) );

    switch (GST_QUERY_TYPE(query))
    {
    case GST_QUERY_CAPS: /* report the supported caps here */
    default:
        ret = gst_pad_query_default(pad, parent, query);
        break;
    }
    return ret;
}


static GstStateChangeReturn kms_frame_saver_plugin_change_state(GstElement *element, GstStateChange transition)
{
    GstStateChangeReturn             ret_val = GST_STATE_CHANGE_SUCCESS;

    KmsFrameSaverPlugin         * ptr_filter = KMS_FRAME_SAVER_PLUGIN(element);

    KmsFrameSaverPluginPrivate * ptr_private = GET_PRIVATE_STRUCT_PTR(ptr_filter);

    gboolean                  is_frame_saver = (ptr_private->sz_snap[5] > '0');

    DBG_Print( __func__ , (guint) transition );
        
    if (is_frame_saver)
    {
        if (transition == GST_STATE_CHANGE_NULL_TO_READY)
        {
            if ( Frame_Saver_Filter_Attach(element) == 0 )
            {
                Frame_Saver_Filter_Set_Params(element, ptr_private->sz_wait, ptr_private->sz_wait);
                Frame_Saver_Filter_Set_Params(element, ptr_private->sz_snap, ptr_private->sz_snap);
                Frame_Saver_Filter_Set_Params(element, ptr_private->sz_link, ptr_private->sz_link);
                Frame_Saver_Filter_Set_Params(element, ptr_private->sz_pads, ptr_private->sz_pads);
                Frame_Saver_Filter_Set_Params(element, ptr_private->sz_path, ptr_private->sz_path);                
            }
        }

        Frame_Saver_Filter_Transition(element, transition);
    }

    if (GST_ELEMENT_CLASS(kms_frame_saver_plugin_parent_class)->change_state != NULL)
    {
        ret_val = GST_ELEMENT_CLASS(kms_frame_saver_plugin_parent_class)->change_state(element, transition);
    }

    if (ret_val != GST_STATE_CHANGE_FAILURE)
    {
        if (transition == GST_STATE_CHANGE_READY_TO_NULL)
        {
            Frame_Saver_Filter_Detach(element);
        }
    }

    return ret_val;
}

#endif // _IS_KURENTO_FILTER_

// ends file:  "kms_frame_saver_plugin_video_filter.c"

