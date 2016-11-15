/* 
 * ======================================================================================
 * File:        gst_frame_saver_plugin.c
 *
 * Purpose:     boiler-plate Gstreamer plugin --- uses "Frame-Saver-Filter" for behavior.
 *
 * History:     1. 2016-11-05   JBendor     Created
 *              2. 2016-11-15   JBendor     Updated
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * ======================================================================================
 */

#ifdef _IS_LIB_FOR_PLUGIN_      /* should be defined only when building the plugin */

 
#ifdef _HAVE_PLUGIN_CONFIG_H_
    #include "plugin_config.h"
#else 
    #ifdef _DEBUG
        #define BUILD_TYPE  "BUILD.DEBUG"
    #else
        #define BUILD_TYPE  "BUILD.RELEASE"
    #endif

    #ifdef _MSC_VER
        #define BUILD_TOOL  "MSC.WINDOWS"
    #else
        #define BUILD_TOOL  "GCC.LINUX"
    #endif

    #ifdef _CYGWIN
        #undef  BUILD_TOOL
        #define BUILD_TOOL  "GCC.CYGWIN"
    #endif

    #define BUILD_DATETIME  "(" __DATE__ "  " __TIME__ ")" 
    
    #define MY_VERSION ( "1.0.0"  " " BUILD_TYPE  "." BUILD_TOOL " " BUILD_DATETIME )
    
    #define PACKAGE "frame_saver_plugin_package"
#endif


#ifdef _CYGWIN
    int cygwin_static_assert(int is_true)  { return is_true; }
    #define G_STATIC_ASSERT(condition)  cygwin_static_assert( (condition) )
    #define __GI_SCANNER__           // prevent CYGWIN's compiler warnings
#endif


#include <stdio.h>
#include <string.h>
#include <gst/gst.h>


// stubs needed when gst-launch loads our plugin and appsink version is old
#ifdef _PROVIDE_APPSINK_STUBS_
    #include <gst/app/gstappsink.h>
    GType gst_app_sink_get_type(void) { return 0xFEED1BAD; }
    GstSample* gst_app_sink_pull_sample(GstAppSink *appsink) { return NULL; }
#endif



#ifdef __ALLOW_DYNAMIC_PARAMS__
    // These headers are needed for Dynamic-Controllable-Parameters
    // The controller allows changing Gobject properties while streaming
    // Applications must link to the shared library "gstreamer-controller"
    #include <gst/controller/gstargbcontrolbinding.h>
    #include <gst/controller/gstdirectcontrolbinding.h>
    #include <gst/controller/gsttimedvaluecontrolsource.h>
    #include <gst/controller/gstinterpolationcontrolsource.h>
    #include <gst/controller/gsttriggercontrolsource.h>
    #include <gst/controller/gstlfocontrolsource.h>

    #define NANOS_PER_MILLISEC  ((guint64) (1000 * 1000))
    #define DYNAMIC_SYNC_NANOS  (NANOS_PER_MILLISEC * 1000)
#endif


/* boiler-plate plugin-element-class structure */
typedef struct _GstFrameSaverPluginClass
{
    GstElementClass parent_class;

} GstFrameSaverPluginClass;


/* boiler-plate plugin-element-instance structure */
typedef struct _GstFrameSaverPlugin
{
    GstElement   element;
    GstPad     * sinkpad;
    GstPad     * srcpad;
    guint        num_buffs;
    guint        num_drops;
    gboolean     is_silent;
    gchar        sz_wait[30],
                 sz_snap[30],
                 sz_link[90],
                 sz_pads[90],
                 sz_path[400];

} GstFrameSaverPlugin;


#ifdef _MSC_VER
    #include "gst_frame_saver_plugin.ms.freeze.__c" /* parsing G_DEFINE_TYPE freezes VS-2015 */
#else
    G_DEFINE_TYPE(GstFrameSaverPlugin, gst_frame_saver_plugin, GST_TYPE_ELEMENT);
#endif

extern GType gst_frame_saver_plugin_get_type(void); /* body defined by G_DEFINE_TYPE */

GST_DEBUG_CATEGORY_STATIC(gst_frame_saver_plugin_debug);

#define GST_CAT_DEFAULT gst_frame_saver_plugin_debug

#define GST_TYPE_OF_FRAME_SAVER_PLUGIN            (gst_frame_saver_plugin_get_type())

#define GST_FRAME_SAVER_PLUGIN(obj)               (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_OF_FRAME_SAVER_PLUGIN, GstFrameSaverPlugin))

#define GST_FRAME_SAVER_PLUGIN_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST((klass),  GST_TYPE_OF_FRAME_SAVER_PLUGIN, GstFrameSaverPluginClass))

#define GST_IS_FRAME_SAVER_PLUGIN(obj)            (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_OF_FRAME_SAVER_PLUGIN))

#define GST_IS_FRAME_SAVER_PLUGIN_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE((klass),  GST_TYPE_OF_FRAMES_AVER_PLUGIN))


enum
{
    e_FIRST_SIGNAL = 0, 
    e_FINAL_SIGNAL

} PLUGIN_SIGNALS_e;


enum
{
    e_PROP_0, 
    e_PROP_WAIT,    // "wait=MillisWaitBeforeNextFrameSnap"
    e_PROP_SNAP,    // "snap=MillisIntervals,MaxNumSnaps,MaxNumFails"
    e_PROP_LINK,    // "link=PipelineName,ProducerName,ConsumerName"
    e_PROP_PADS,    // "pads=ProducerOut,ConsumerInput,ConsumerOut"
    e_PROP_PATH,    // "path=PathForWorkingFolderForSavedImageFiles"
    e_PROP_SILENT   // silent=0 or silent=1 --- 1 disables messages

} PLUGIN_PARAMS_e;


#define _USING_FRAME_SAVER_     // when undefined: not saving image frames


#ifdef _USING_FRAME_SAVER_

    extern int Frame_Saver_Filter_Attach(GstElement * pluginPtr);
    extern int Frame_Saver_Filter_Detach(GstElement * pluginPtr);
    extern int Frame_Saver_Filter_Receive(GstElement * pluginPtr, GstBuffer * aBufferPtr);
    extern int Frame_Saver_Filter_Transition(GstElement * pluginPtr, GstStateChange aTransition) ;
    extern int Frame_Saver_Filter_Set_Params(GstElement * pluginPtr, const gchar * aNewValuePtr, gchar * aPrvSpecsPtr);

#else
    static int Frame_Saver_Filter_Attach(GstElement * pluginPtr) 
    { 
        return 0;
    }
    static int Frame_Saver_Filter_Detach(GstElement * pluginPtr) 
    { 
        return 0;
    }
    static int Frame_Saver_Filter_Receive(GstElement * pluginPtr, GstBuffer * aBufferPtr) 
    { 
        return 0;
    }
    static int Frame_Saver_Filter_Transition(GstElement * pluginPtr, GstStateChange aTransition) 
    { 
        return 0;
    }
    static int Frame_Saver_Filter_Set_Params(GstElement * pluginPtr, const gchar * aNewValuePtr, gchar * aPrvSpecsPtr)
    {
        #ifdef _ALLOW_DYNAMIC_PARAMS_
            gst_object_sync_values(pluginPtr, DYNAMIC_SYNC_NANOS);
        #endif

        return 0; 
    }

#endif


/* the capabilities of the inputs and outputs. describe the real formats here. */
static GstStaticPadTemplate gst_frame_saver_plugin_sink_template =
                            GST_STATIC_PAD_TEMPLATE( "sink",
                                                     GST_PAD_SINK,
                                                     GST_PAD_ALWAYS,
                                                     GST_STATIC_CAPS ("ANY") );

static GstStaticPadTemplate gst_frame_saver_plugin_src_template =
                            GST_STATIC_PAD_TEMPLATE( "src",
                                                     GST_PAD_SRC,
                                                     GST_PAD_ALWAYS,
                                                     GST_STATIC_CAPS ("ANY") );


static void gst_frame_saver_plugin_set_property(GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);

static void gst_frame_saver_plugin_get_property(GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);

static gboolean gst_frame_saver_plugin_sink_event(GstPad * pad, GstObject * parent, GstEvent * event);

static GstFlowReturn gst_frame_saver_plugin_chain(GstPad * pad, GstObject * parent, GstBuffer * buf);

static GstStateChangeReturn gst_frame_saver_plugin_change_state(GstElement *element, GstStateChange transition);

static GstElementClass * This_Plugin_Element_Class_ptr = NULL;

static GstElementClass * This_Plugin_Element_Parent_Class_ptr = NULL;


/* GObject vmethod implementations */


/* initialize the frame-saver-plugin-class */
static void gst_frame_saver_plugin_class_init(GstFrameSaverPluginClass * klass)
{
    GParamFlags param_flags = G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | GST_PARAM_MUTABLE_READY;

    GObjectClass * gobject_class = (GObjectClass *) klass;

    gobject_class->set_property = gst_frame_saver_plugin_set_property;

    gobject_class->get_property = gst_frame_saver_plugin_get_property;

    if (This_Plugin_Element_Class_ptr == NULL)
    {
        This_Plugin_Element_Class_ptr = GST_ELEMENT_CLASS(klass);

        This_Plugin_Element_Parent_Class_ptr = GST_ELEMENT_CLASS(gst_frame_saver_plugin_parent_class);

        This_Plugin_Element_Class_ptr->change_state = gst_frame_saver_plugin_change_state;
    }
    else if (This_Plugin_Element_Class_ptr != (GstElementClass *) klass)
    {
        // should be impossible to reach here ... just suppress "unused_static" warnings
        param_flags |= (gobject_class == (GObjectClass *) gst_frame_saver_plugin_get_instance_private);
    }

    #ifdef _ALLOW_DYNAMIC_PARAMS_
        param_flags |= GST_PARAM_CONTROLLABLE;
    #endif

    g_object_class_install_property(gobject_class,
                                    e_PROP_WAIT,
                                    g_param_spec_string("wait",
                                                        "wait=MillisWaitBeforeNextFrameSnap",
                                                        "wait before snapping another frame",
                                                        "3000",
                                                        param_flags));

    g_object_class_install_property(gobject_class,
                                    e_PROP_SNAP,
                                    g_param_spec_string("snap",
                                                        "snap=millisecInterval,maxNumSnaps,maxNumFails",
                                                        "snap and save frames as RGB images in PNG files",
                                                        "1000,0,0",
                                                        param_flags));

    g_object_class_install_property(gobject_class,
                                    e_PROP_LINK,
                                    g_param_spec_string("link",
                                                        "link=pipelineName,producerName,consumerName",
                                                        "insert TEE between producer and consumer elements",
                                                        "auto,auto,auto",
                                                        param_flags));

    g_object_class_install_property(gobject_class,
                                    e_PROP_PADS,
                                    g_param_spec_string("pads",
                                                        "pads=producerOut,consumerInput,consumerOut",
                                                        "pads used by the producer and consumer elements",
                                                        "src,sink,src",
                                                        param_flags));

    g_object_class_install_property(gobject_class,
                                    e_PROP_PATH,
                                    g_param_spec_string("path",
                                                        "path=path-of-working-folder-for-saved-images",
                                                        "path of working folder for saved image files",
                                                        "auto",
                                                        param_flags));

    gst_element_class_set_details_simple(GST_ELEMENT_CLASS(klass),
                                         "FrameSaverPlugin",                // name to launch
                                         "Pipeline-Splicer",                // classification
                                         "Inserts TEE and Saves Frames",    // description
                                         "Author <<author@hostname.org>>"); // author info

    g_object_class_install_property(gobject_class,
                                    e_PROP_SILENT,
                                    g_param_spec_boolean("silent",
                                                         "Silent or Verbose",
                                                         "Silent is 1/True --- Verbose is 0/False",
                                                         FALSE,
                                                         G_PARAM_READWRITE));

    gst_element_class_add_pad_template(GST_ELEMENT_CLASS(klass),
                                       gst_static_pad_template_get(&gst_frame_saver_plugin_src_template));

    gst_element_class_add_pad_template(GST_ELEMENT_CLASS(klass),
                                       gst_static_pad_template_get(&gst_frame_saver_plugin_sink_template));

    return;
}


static gboolean gst_my_filter_src_query(GstPad *pad, GstObject *parent, GstQuery *query)
{
    gboolean ret;

    switch (GST_QUERY_TYPE(query))
    {
    case GST_QUERY_CAPS: /* report the supported caps here */
    default:
        ret = gst_pad_query_default(pad, parent, query);
        break;
    }
    return ret;
}


static GstStateChangeReturn gst_frame_saver_plugin_change_state(GstElement *element, GstStateChange transition)
{
    GstStateChangeReturn ret_val = GST_STATE_CHANGE_SUCCESS;

    GstFrameSaverPlugin * filter = GST_FRAME_SAVER_PLUGIN(element);

    gboolean      is_frame_saver = (filter->sz_snap[5] > '0');
        
    if (is_frame_saver)
    {
        if (transition == GST_STATE_CHANGE_NULL_TO_READY)
        {
            if ( Frame_Saver_Filter_Attach(element) == 0 )
            {
                Frame_Saver_Filter_Set_Params(element, filter->sz_wait, filter->sz_wait);
                Frame_Saver_Filter_Set_Params(element, filter->sz_snap, filter->sz_snap);
                Frame_Saver_Filter_Set_Params(element, filter->sz_link, filter->sz_link);
                Frame_Saver_Filter_Set_Params(element, filter->sz_pads, filter->sz_pads);
                Frame_Saver_Filter_Set_Params(element, filter->sz_path, filter->sz_path);                
            }
        }

        Frame_Saver_Filter_Transition(element, transition);
    }

    if (This_Plugin_Element_Parent_Class_ptr->change_state != NULL)
    {
        ret_val = This_Plugin_Element_Parent_Class_ptr->change_state(element, transition);
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


/* initialize the new element
 * instantiate pads and add them to element
 * set pad callback functions
 * initialize instance structure
 */
static void gst_frame_saver_plugin_init(GstFrameSaverPlugin * filter)
{
    filter->sinkpad = gst_pad_new_from_static_template(&gst_frame_saver_plugin_sink_template, "sink");
    filter->srcpad = gst_pad_new_from_static_template(&gst_frame_saver_plugin_src_template, "src");

    gst_pad_set_event_function(filter->sinkpad, GST_DEBUG_FUNCPTR(gst_frame_saver_plugin_sink_event));
    gst_pad_set_chain_function(filter->sinkpad, GST_DEBUG_FUNCPTR(gst_frame_saver_plugin_chain));
    gst_pad_set_query_function(filter->sinkpad, gst_my_filter_src_query);

    GST_PAD_SET_PROXY_CAPS(filter->sinkpad);
    GST_PAD_SET_PROXY_CAPS(filter->srcpad);

    gst_element_add_pad(GST_ELEMENT(filter), filter->sinkpad);
    gst_element_add_pad(GST_ELEMENT(filter), filter->srcpad);

    strcpy(filter->sz_wait, "wait=2000");
    strcpy(filter->sz_snap, "snap=0,0,0");
    strcpy(filter->sz_link, "link=Live,auto,auto");
    strcpy(filter->sz_pads, "pads=auto,auto,auto");
    strcpy(filter->sz_path, "path=auto");
    
    filter->is_silent = TRUE;

    filter->num_buffs = 0;

    filter->num_drops = 0;

    return;
}


static void gst_frame_saver_plugin_set_property(GObject      * object,
                                                guint          prop_id,
                                                const GValue * value,
                                                GParamSpec   * pspec)
{
    const gchar * psz_now = NULL;

    GstFrameSaverPlugin * filter = GST_FRAME_SAVER_PLUGIN(object);

    switch (prop_id)
    {
    case e_PROP_SILENT:
        filter->is_silent = g_value_get_boolean(value);
        psz_now = filter->is_silent ? "Silent=TRUE" : "Silent=FALSE";
        break;

    case e_PROP_WAIT:
        snprintf( filter->sz_wait, sizeof(filter->sz_wait), "wait=%s", g_value_get_string(value) );
        psz_now = filter->sz_wait;
        break;

    case e_PROP_SNAP:
        snprintf( filter->sz_snap, sizeof(filter->sz_snap), "snap=%s", g_value_get_string(value) );
        psz_now = filter->sz_snap;
        break;

    case e_PROP_LINK:
        snprintf( filter->sz_link, sizeof(filter->sz_link), "link=%s", g_value_get_string(value) );
        psz_now = filter->sz_link;
        break;

    case e_PROP_PADS:
        snprintf( filter->sz_pads, sizeof(filter->sz_pads), "pads=%s", g_value_get_string(value) );
        psz_now = filter->sz_pads;
        break;

    case e_PROP_PATH:
        snprintf( filter->sz_path, sizeof(filter->sz_path), "path=%s", g_value_get_string(value) );
        psz_now = filter->sz_path;
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }

    if (psz_now != NULL)
    {
        g_print("FrameSaverPlugin --- Property #%u --- Now: (%s) \n", (guint) prop_id, psz_now);

        filter->num_buffs = 0;
        filter->num_drops = 0;
    }

    return;
}


static void gst_frame_saver_plugin_get_property(GObject * object, guint prop_id, GValue * value, GParamSpec * pspec)
{
    GstFrameSaverPlugin * filter = GST_FRAME_SAVER_PLUGIN(object);

    switch (prop_id)
    {
    case e_PROP_SILENT:
        g_value_set_boolean(value, filter->is_silent);
        break;

    case e_PROP_WAIT:
        g_value_set_string(value, filter->sz_wait);
        break;

    case e_PROP_SNAP:
        g_value_set_string(value, filter->sz_snap);
        break;

    case e_PROP_LINK:
        g_value_set_string(value, filter->sz_link);
        break;

    case e_PROP_PADS:
        g_value_set_string(value, filter->sz_pads);
        break;

    case e_PROP_PATH:
        g_value_set_string(value, filter->sz_path);
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }

    return;
}


/* GstElement vmethod implementations */


/* this function handles sink events */
static gboolean gst_frame_saver_plugin_sink_event(GstPad * pad, GstObject * parent, GstEvent * event)
{
    gboolean ret;
    GstCaps * caps;
    GstFrameSaverPlugin * filter = GST_FRAME_SAVER_PLUGIN(parent);

    GST_LOG_OBJECT(filter, "Received Event (%s) : %" GST_PTR_FORMAT, GST_EVENT_TYPE_NAME (event), event);

    switch (GST_EVENT_TYPE(event))
    {
    case GST_EVENT_CAPS: /* do something with the caps --- forward */
        gst_event_parse_caps(event, &caps);
        ret = gst_pad_event_default(pad, parent, event);
        break;

    case GST_EVENT_EOS: /* end-of-stream, close down all stream leftovers here */
        ret = gst_pad_event_default(pad, parent, event);
        break;

    default:
        ret = gst_pad_event_default(pad, parent, event);
        break;
    }
    return ret;
}


/* chain function --- this function does the actual processing */
static GstFlowReturn gst_frame_saver_plugin_chain(GstPad * pad, GstObject * parent, GstBuffer * buf)
{
    GstFrameSaverPlugin * filter = GST_FRAME_SAVER_PLUGIN(parent);

    GstFlowReturn         result = gst_pad_push(filter->srcpad, buf);
    
    filter->num_buffs += 1;

    filter->num_drops += (result == GST_FLOW_OK) ? 0 : 1;

    if (filter->is_silent == FALSE)
    {
        g_print("FrameSaverPlugin --- Push --- (%u/%u) \n", filter->num_buffs, filter->num_drops);
    }    
    
    if ( filter->sz_snap[5] > '0')  // is_frame_saver)
    {
        Frame_Saver_Filter_Receive(GST_ELEMENT(filter), buf);
    }
    
    return result;  // anythin except GST_FLOW_OK could halt the flow in the pipeline
}


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean frame_saver_plugin_init(GstPlugin * filter)
{
    #ifdef _ALLOW_DYNAMIC_PARAMS_
        gst_controller_init (NULL, NULL);
    #endif

    /* debug category for filtering log messages   */
    GST_DEBUG_CATEGORY_INIT(gst_frame_saver_plugin_debug, "FrameSaverPlugin", 0, "FrameSaverPlugin_Description");

    return gst_element_register(filter, "FrameSaverPlugin", GST_RANK_NONE, GST_TYPE_OF_FRAME_SAVER_PLUGIN);
}


/* gstreamer looks for this structure to register plugins */
GST_PLUGIN_DEFINE(GST_VERSION_MAJOR,
                  GST_VERSION_MINOR,
                  FrameSaverPlugin,
                  "FrameSaverPlugin --- Inserts TEE and saves image frames",
                  frame_saver_plugin_init,
                  MY_VERSION,
                  "LGPL",
                  "GStreamer",
                  "http://gstreamer.net/")

                  
#endif  /* _IS_LIB_FOR_PLUGIN_ */
