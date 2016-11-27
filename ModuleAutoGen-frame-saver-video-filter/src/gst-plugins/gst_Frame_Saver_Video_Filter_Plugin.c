/*
 * ======================================================================================
 * File:        gst_Frame_Saver_Video_Filter_Plugin.c
 *
 * Purpose:     Kurento+Gstreamer plugin-filter --- uses "Frame-Saver" for full behavior.
 *
 * History:     1. 2016-11-25   JBendor     Created as copy of "gst_Frame_Saver_Plugin.c"
 *              2. 2016-11-25   JBendor     Adapted to _VIDEO_FILTER_TYPE_ being defined
 *              3. 2016-11-26   JBendor     Updated
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * ======================================================================================
 */

#include <gst_Frame_Saver_Video_Filter_Plugin.h>


#ifndef FRAME_SAVER_PLUGIN_VERSION_INFO

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

    #define FRAME_SAVER_PLUGIN_VERSION_INFO ( "1.0.0"  " " BUILD_TYPE  "." BUILD_TOOL " " BUILD_DATETIME  " source=" __FILE__ )

#endif


#ifndef PACKAGE
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


#ifdef _SAVE_IMAGE_FRAMES_

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
        return 0;
    }

#endif


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
	e_PROP_NOTE,    // "note=none or note=MostRecentError"
	e_PROP_SILENT   // silent=0 or silent=1 --- 1 disables messages

} PLUGIN_PARAMS_e;


GST_DEBUG_CATEGORY_STATIC       (gst_frame_saver_plugin_debug_category);

#define GST_CAT_DEFAULT         gst_frame_saver_plugin_debug_category

extern GType gst_frame_saver_plugin_get_type(void);     // body defined by G_DEFINE_TYPE

#ifdef _VIDEO_FILTER_TYPE_

	#include <gst/video/video.h>
	#include <gst/video/gstvideofilter.h>

	typedef struct _GstFrameSaverPluginPrivate
	{
		GstElement   element;
		GstPad     * sinkpad;
		GstPad     * srcpad;
		guint        num_buffs;
		guint        num_drops;
		guint        num_notes;
		gboolean     is_silent;
		gchar        sz_wait[30],
					 sz_snap[30],
					 sz_link[90],
					 sz_pads[90],
					 sz_path[400],
					 sz_note[200];

	} GstFrameSaverPluginPrivate;

	typedef struct _GstFrameSaverPlugin
	{
		GstVideoFilter                base;
		GstFrameSaverPluginPrivate  * priv;
	} GstFrameSaverPlugin;

	typedef struct _GstFrameSaverPluginClass
	{
		GstVideoFilterClass 	parent_class;
	} GstFrameSaverPluginClass;

	static GstFlowReturn gst_frame_saver_plugin_transform_frame_ip (GstVideoFilter *filter, GstVideoFrame *frame)
	{
		return GST_FLOW_OK;
	}

	static void gst_frame_saver_plugin_init (GstFrameSaverPlugin * aPluginPtr);  /* initializes plugin instance */

	#ifdef _MSC_VER
		#include "gst_frame_saver_plugin.ms.freeze.__c" /* parsing G_DEFINE_TYPE freezes VS-2015 */
	#else
		G_DEFINE_TYPE(GstFrameSaverPlugin, gst_frame_saver_plugin, GST_TYPE_VIDEO_FILTER);

        //? G_DEFINE_TYPE_WITH_CODE (GstFrameSaverPlugin, gst_frame_saver_plugin, GST_TYPE_VIDEO_FILTER,
        //? GST_DEBUG_CATEGORY_INIT(gst_frame_saver_plugin_debug_category, THIS_PLUGIN_NAME, 0, "debug category for this element") );

        //? unused-kurento-pad-template: ("src",  GST_PAD_SRC,  GST_PAD_ALWAYS, gst_caps_from_string(VIDEO_SRC_CAPS))
        //? unused-kurento-pad-template: ("sink", GST_PAD_SINK, GST_PAD_ALWAYS, gst_caps_from_string(VIDEO_SINK_CAPS)
	#endif

	#define GST_GET_PRIVATE(obj)    (G_TYPE_INSTANCE_GET_PRIVATE((obj), GST_TYPE_OF_FRAME_SAVER_PLUGIN, GstFrameSaverPluginPrivate))

    #define GST_TYPE_OF_FRAME_SAVER_PLUGIN            (gst_frame_saver_plugin_get_type())
    #define GST_FRAME_SAVER_PLUGIN(obj)               (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_OF_FRAME_SAVER_PLUGIN, GstFrameSaverPlugin))
    #define GST_FRAME_SAVER_PLUGIN_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST((klass),  GST_TYPE_OF_FRAME_SAVER_PLUGIN, GstFrameSaverPluginClass))
    #define GST_IS_FRAME_SAVER_PLUGIN(obj)            (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_OF_FRAME_SAVER_PLUGIN))
    #define GST_IS_FRAME_SAVER_PLUGIN_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE((klass),  GST_TYPE_OF_FRAMES_AVER_PLUGIN))

#else

	typedef struct _GstFrameSaverPlugin
	{
		GstElement   element;
		GstPad     * sinkpad;
		GstPad     * srcpad;
		guint        num_buffs;
		guint        num_drops;
		guint        num_notes;
		gboolean     is_silent;
		gchar        sz_wait[30],
					 sz_snap[30],
					 sz_link[90],
					 sz_pads[90],
					 sz_path[400],
					 sz_note[200];

	} GstFrameSaverPlugin, GstFrameSaverPluginPrivate;

	typedef struct _GstFrameSaverPluginClass
	{
	    GstElementClass parent_class;
	} GstFrameSaverPluginClass;

	static void gst_frame_saver_plugin_init (GstFrameSaverPlugin * aPluginPtr);  /* initializes plugin instance */

	#ifdef _MSC_VER
		#include "gst_frame_saver_plugin.ms.freeze.__c" /* parsing G_DEFINE_TYPE freezes VS-2015 */
	#else
		G_DEFINE_TYPE(GstFrameSaverPlugin, gst_frame_saver_plugin, GST_TYPE_ELEMENT);
	#endif

    #define GST_GET_PRIVATE(obj)    ((GstFrameSaverPluginPrivate *) (obj))

    #define GST_TYPE_OF_FRAME_SAVER_PLUGIN            (gst_frame_saver_plugin_get_type())
    #define GST_FRAME_SAVER_PLUGIN(obj)               (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_OF_FRAME_SAVER_PLUGIN, GstFrameSaverPlugin))
    #define GST_FRAME_SAVER_PLUGIN_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST((klass),  GST_TYPE_OF_FRAME_SAVER_PLUGIN, GstFrameSaverPluginClass))
    #define GST_IS_FRAME_SAVER_PLUGIN(obj)            (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_OF_FRAME_SAVER_PLUGIN))
    #define GST_IS_FRAME_SAVER_PLUGIN_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE((klass),  GST_TYPE_OF_FRAMES_AVER_PLUGIN))

#endif  //  _VIDEO_FILTER_TYPE_


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
    GstStateChangeReturn             ret_val = GST_STATE_CHANGE_SUCCESS;

    GstFrameSaverPlugin             * filter = GST_FRAME_SAVER_PLUGIN(element);

    GstFrameSaverPluginPrivate * ptr_private = GST_GET_PRIVATE(filter);

    gboolean                  is_frame_saver = (ptr_private->sz_snap[5] > '0');
        
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


static void gst_frame_saver_plugin_set_property(GObject      * object,
                                                guint          prop_id,
                                                const GValue * value,
                                                GParamSpec   * pspec)
{
    const gchar * psz_now = NULL;

    GstFrameSaverPlugin * filter = GST_FRAME_SAVER_PLUGIN(object);

    GstFrameSaverPluginPrivate * ptr_private = GST_GET_PRIVATE(filter);

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

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }

    if (psz_now != NULL)
    {
        g_print("FrameSaverPlugin --- Property #%u --- Now: (%s) \n", (guint) prop_id, psz_now);

        ptr_private->num_buffs = 0;
        ptr_private->num_drops = 0;
        ptr_private->num_notes = 0;
    }

    return;
}


static void gst_frame_saver_plugin_get_property(GObject * object, guint prop_id, GValue * value, GParamSpec * pspec)
{
    GstFrameSaverPlugin * filter = GST_FRAME_SAVER_PLUGIN(object);

    GstFrameSaverPluginPrivate * ptr_private = GST_GET_PRIVATE(filter);

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

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }

    return;
}


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
    GstFrameSaverPlugin             * filter = GST_FRAME_SAVER_PLUGIN(parent);

    GstFrameSaverPluginPrivate * ptr_private = GST_GET_PRIVATE(filter);

    GstFlowReturn                     result = gst_pad_push(ptr_private->srcpad, buf);
    
    ptr_private->num_buffs += 1;

    ptr_private->num_drops += (result == GST_FLOW_OK) ? 0 : 1;

    if (ptr_private->is_silent == FALSE)
    {
        g_print("FrameSaverPlugin --- Push --- (%u/%u) \n", ptr_private->num_buffs, ptr_private->num_drops);
    }    
    
    if ( ptr_private->sz_snap[5] > '0')  // is_frame_saver)
    {
        Frame_Saver_Filter_Receive(GST_ELEMENT(filter), buf);
    }
    
    return result;  // anythin except GST_FLOW_OK could halt the flow in the pipeline
}


static void gst_frame_saver_plugin_finalize (GObject *object)
{
    return;
}


/* initialize the new element
 * instantiate pads and add them to element
 * set pad callback functions
 * initialize instance structure
 */
static void gst_frame_saver_plugin_init(GstFrameSaverPlugin * filter)
{
    GstFrameSaverPluginPrivate * ptr_private = GST_GET_PRIVATE(filter);

    ptr_private->sinkpad = gst_pad_new_from_static_template(&gst_frame_saver_plugin_sink_template, "sink");
    ptr_private->srcpad = gst_pad_new_from_static_template(&gst_frame_saver_plugin_src_template, "src");

    gst_pad_set_event_function(ptr_private->sinkpad, GST_DEBUG_FUNCPTR(gst_frame_saver_plugin_sink_event));
    gst_pad_set_chain_function(ptr_private->sinkpad, GST_DEBUG_FUNCPTR(gst_frame_saver_plugin_chain));
    gst_pad_set_query_function(ptr_private->sinkpad, gst_my_filter_src_query);

    GST_PAD_SET_PROXY_CAPS(ptr_private->sinkpad);
    GST_PAD_SET_PROXY_CAPS(ptr_private->srcpad);

    gst_element_add_pad(GST_ELEMENT(filter), ptr_private->sinkpad);
    gst_element_add_pad(GST_ELEMENT(filter), ptr_private->srcpad);

    strcpy(ptr_private->sz_wait, "wait=2000");
    strcpy(ptr_private->sz_snap, "snap=0,0,0");
    strcpy(ptr_private->sz_link, "link=Live,auto,auto");
    strcpy(ptr_private->sz_pads, "pads=auto,auto,auto");
    strcpy(ptr_private->sz_path, "path=auto");
    strcpy(ptr_private->sz_note, "note=none");
    
    ptr_private->is_silent = TRUE;

    ptr_private->num_buffs = 0;
    ptr_private->num_drops = 0;    
    ptr_private->num_notes = 0;

    return;
}


/* initialize the frame-saver-plugin-class */
static void gst_frame_saver_plugin_class_init(GstFrameSaverPluginClass * klass)
{
    int  params_attributes_flags = G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | GST_PARAM_MUTABLE_READY;

    GObjectClass * gobject_class = (GObjectClass *) klass;

    gobject_class->set_property = gst_frame_saver_plugin_set_property;

    gobject_class->get_property = gst_frame_saver_plugin_get_property;

    gobject_class->finalize     = gst_frame_saver_plugin_finalize;

    // possibly --- first time here
    if (This_Plugin_Element_Class_ptr == NULL)
    {
        GST_DEBUG_CATEGORY_INIT (GST_CAT_DEFAULT, THIS_PLUGIN_NAME, 0, THIS_PLUGIN_NAME);

        This_Plugin_Element_Class_ptr = GST_ELEMENT_CLASS(klass);

        This_Plugin_Element_Parent_Class_ptr = GST_ELEMENT_CLASS(gst_frame_saver_plugin_parent_class);

        This_Plugin_Element_Class_ptr->change_state = gst_frame_saver_plugin_change_state;

		#ifdef _VIDEO_FILTER_TYPE_
			gst_element_class_set_static_metadata (GST_ELEMENT_CLASS (klass),
												   "element definition",
												   "Video/Filter",
												   "Filter doc",
												   "Developer");

			GstVideoFilterClass *video_filter_class = GST_VIDEO_FILTER_CLASS (klass);

			video_filter_class->transform_frame_ip = GST_DEBUG_FUNCPTR (gst_frame_saver_plugin_transform_frame_ip);
		#endif
    }
    else if (This_Plugin_Element_Class_ptr != (GstElementClass *) klass)
    {
        // should be impossible to reach here ... just suppress "unused_static" warnings
    	params_attributes_flags += gobject_class  ? 0 : 0;
    }

    gst_element_class_add_pad_template(GST_ELEMENT_CLASS(klass),
                                       gst_static_pad_template_get(&gst_frame_saver_plugin_src_template));

    gst_element_class_add_pad_template(GST_ELEMENT_CLASS(klass),
                                       gst_static_pad_template_get(&gst_frame_saver_plugin_sink_template));

    GParamFlags param_flags = (GParamFlags) params_attributes_flags;

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

    g_object_class_install_property(gobject_class,
                                    e_PROP_NOTE,
                                    g_param_spec_string("note",
                                                        "note=MostRecentErrorCodition",
                                                        "most recent error",
                                                        "none",
                                                        G_PARAM_READABLE));

    g_object_class_install_property(gobject_class,
                                    e_PROP_SILENT,
                                    g_param_spec_boolean("silent",
                                                         "Silent or Verbose",
                                                         "Silent is 1/True --- Verbose is 0/False",
                                                         FALSE,
                                                         G_PARAM_READWRITE));

    gst_element_class_set_details_simple(GST_ELEMENT_CLASS(klass),
                                         "FrameSaverPlugin",                // name to launch
                                         "Pipeline-Splicer",                // classification
                                         "Inserts TEE and Saves Frames",    // description
                                         "Author <<author@hostname.org>>"); // author info

    return;
}


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean frame_saver_plugin_register(GstPlugin * aPluginPtr)
{
    #ifdef _ALLOW_DYNAMIC_PARAMS_
        gst_controller_init (NULL, NULL);
    #endif

    return gst_element_register (aPluginPtr, THIS_PLUGIN_NAME, GST_RANK_NONE, GST_TYPE_OF_FRAME_SAVER_PLUGIN);
}


/* gstreamer looks for this structure to register plugins */
GST_PLUGIN_DEFINE(GST_VERSION_MAJOR,
                  GST_VERSION_MINOR,
                  FrameSaverPlugin,
                  "FrameSaverPlugin --- Inserts TEE and saves image frames",
				  frame_saver_plugin_register,
                  FRAME_SAVER_PLUGIN_VERSION_INFO,
                  "LGPL",
                  "GStreamer",
                  "http://gstreamer.net/")

// ends file:  "gst_frame_saver_plugin_video_filter.c"
