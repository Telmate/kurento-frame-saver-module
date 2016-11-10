/* 
 * ======================================================================================
 * File:        gst_frame_saver_plugin.c
 *
 * Purpose:     boiler-plate Gstreamer plugin --- uses "Frame-Saver-Filter" for behavior.
 *
 * History:     1. 2016-11-05   JBendor     Created
 *              2. 2016-11-09   JBendor     Updated
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * ======================================================================================
 */

#ifdef _HAVE_PLUGIN_CONFIG_H_
    #include "plugin_config.h"
#else 
    #define VERSION "1.0.0"
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
    GstPad     * sinkpad,
               * srcpad;
    gboolean     is_silent,
                 is_pusher;
    guint        num_buffs;
    gchar        sz_wait[30],
                 sz_snap[30],
                 sz_poke[90],
                 sz_pads[90];

} GstFrameSaverPlugin;


#ifdef _MSC_VER
    #include "gst_frame_saver_plugin.ms" // parsing G_DEFINE_TYPE freezes VS-2015
#else
    G_DEFINE_TYPE(GstFrameSaverPlugin, gst_frame_saver_plugin, GST_TYPE_ELEMENT);
#endif


extern GType gst_frame_saver_plugin_get_type(void);    // defined by G_DEFINE_TYPE

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
    e_PROP_POKE,    // "poke=PipelineName,ProducerName,ConsumerName"
    e_PROP_PADS,    // "pads=ProducerOut,ConsumerInput,ConsumerOut"
    e_PROP_SILENT   // silent=0 or silent=1 --- 1 suppresses messages

} PLUGIN_PARAMS_e;


#ifdef _DUMMY_PLUGIN_LIB
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
    static int Frame_Saver_Filter_Set_Params(GstElement * pluginPtr, const gchar * aNewValuePtr, gchar * aParamSpecPtr)
    {
        g_print("FrameSaverPlugin_Set_Param --- IS (%s)  --- NEW (%s) \n", aParamSpecPtr, aNewValuePtr);

        strcpy( &aParamSpecPtr[5], aNewValuePtr );

        #ifdef _ALLOW_DYNAMIC_PARAMS_
            gst_object_sync_values(pluginPtr, DYNAMIC_SYNC_NANOS);
        #endif

        return 0; 
    }

#else

    extern int Frame_Saver_Filter_Attach(GstElement * pluginPtr);
    extern int Frame_Saver_Filter_Detach(GstElement * pluginPtr); 
    extern int Frame_Saver_Filter_Receive(GstElement * pluginPtr, GstBuffer * aBufferPtr);
    extern int Frame_Saver_Filter_Transition(GstElement * pluginPtr, GstStateChange aTransition) ;
    extern int Frame_Saver_Filter_Set_Params(GstElement * pluginPtr, const gchar * aNewValuePtr, gchar * aParamSpecPtr);

#endif


/* the capabilities of the inputs and outputs. describe the real formats here. */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE( "sink",
                                                                    GST_PAD_SINK,
                                                                    GST_PAD_ALWAYS,
                                                                    GST_STATIC_CAPS ("ANY") );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE( "src",
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
                                    e_PROP_POKE,
                                    g_param_spec_string("poke",
                                                        "poke=pipelineName,producerName,consumerName",
                                                        "poke TEE between producer and consumer elements",
                                                        "auto,auto,auto",
                                                        param_flags));

    g_object_class_install_property(gobject_class,
                                    e_PROP_PADS,
                                    g_param_spec_string("pads",
                                                        "pads=producerOut,consumerInput,consumerOut",
                                                        "pads used by the producer and consumer elements",
                                                        "src,sink,src",
                                                        param_flags));

    gst_element_class_set_details_simple(This_Plugin_Element_Class_ptr,
                                         "FrameSaverPlugin",                // name to launch
                                         "Pipeline-Splicer",                // classification
                                         "Inserts-TEE-and-Saves-Frames",    // description
                                         "Author <<author@hostname.org>>"); // author info

    g_object_class_install_property(gobject_class,
                                    e_PROP_SILENT,
                                    g_param_spec_boolean("silent",
                                                         "Silent or Verbose",
                                                         "Silent is 0/False --- Verbose is 1/True",
                                                         FALSE,
                                                         G_PARAM_READWRITE));

    gst_element_class_add_pad_template(This_Plugin_Element_Class_ptr, gst_static_pad_template_get(&src_factory));

    gst_element_class_add_pad_template(This_Plugin_Element_Class_ptr, gst_static_pad_template_get(&sink_factory));

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
    GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;

    if (transition == GST_STATE_CHANGE_NULL_TO_READY)
    {
        Frame_Saver_Filter_Attach(element);
    }

    Frame_Saver_Filter_Transition(element, transition);

    if (This_Plugin_Element_Parent_Class_ptr->change_state != NULL)
    {
        ret = This_Plugin_Element_Parent_Class_ptr->change_state(element, transition);
    }

    if (ret != GST_STATE_CHANGE_FAILURE)
    {
        if (transition == GST_STATE_CHANGE_READY_TO_NULL)
        {
            Frame_Saver_Filter_Detach(element);
        }
    }

    return ret;
}


/* initialize the new element
 * instantiate pads and add them to element
 * set pad callback functions
 * initialize instance structure
 */
static void gst_frame_saver_plugin_init(GstFrameSaverPlugin * filter)
{
    filter->sinkpad = gst_pad_new_from_static_template(&sink_factory, "sink");

    gst_pad_set_event_function(filter->sinkpad, GST_DEBUG_FUNCPTR(gst_frame_saver_plugin_sink_event));

    gst_pad_set_chain_function(filter->sinkpad, GST_DEBUG_FUNCPTR(gst_frame_saver_plugin_chain));

    gst_pad_set_query_function(filter->srcpad, gst_my_filter_src_query);

    GST_PAD_SET_PROXY_CAPS(filter->sinkpad);

    gst_element_add_pad(GST_ELEMENT(filter), filter->sinkpad);

    filter->srcpad = gst_pad_new_from_static_template(&src_factory, "src");

    GST_PAD_SET_PROXY_CAPS(filter->srcpad);

    gst_element_add_pad(GST_ELEMENT(filter), filter->srcpad);

    strcpy(filter->sz_wait, "wait=1000");
    strcpy(filter->sz_snap, "snap=1000,0,0");
    strcpy(filter->sz_poke, "poke=auto,auto,auto");
    strcpy(filter->sz_pads, "pads=auto,auto,auto");
    
    filter->is_pusher = ((filter->sz_snap[5] < '1') || (filter->sz_snap[5] > '9'));
    filter->is_silent = FALSE;
    filter->num_buffs = 0;

    Frame_Saver_Filter_Attach(GST_ELEMENT(filter));
    
    return;
}


static void gst_frame_saver_plugin_set_property(GObject      * object,
                                                guint          prop_id,
                                                const GValue * value,
                                                GParamSpec   * pspec)
{
    GstFrameSaverPlugin * filter = GST_FRAME_SAVER_PLUGIN(object);

    switch (prop_id)
    {
    case e_PROP_SILENT:
        filter->is_silent = g_value_get_boolean(value);
        break;

    case e_PROP_WAIT:
        Frame_Saver_Filter_Set_Params(GST_ELEMENT(filter), g_value_get_string(value), filter->sz_wait);
        break;

    case e_PROP_SNAP:
        Frame_Saver_Filter_Set_Params(GST_ELEMENT(filter), g_value_get_string(value), filter->sz_snap);
        filter->is_pusher = ((filter->sz_snap[5] < '1') || (filter->sz_snap[5] > '9'));        
        break;

    case e_PROP_POKE:
        Frame_Saver_Filter_Set_Params(GST_ELEMENT(filter), g_value_get_string(value), filter->sz_poke);
        break;

    case e_PROP_PADS:
        Frame_Saver_Filter_Set_Params(GST_ELEMENT(filter), g_value_get_string(value), filter->sz_pads);
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
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

    case e_PROP_POKE:
        g_value_set_string(value, filter->sz_poke);
        break;

    case e_PROP_PADS:
        g_value_set_string(value, filter->sz_pads);
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

    GST_LOG_OBJECT(filter, "Received %s event: %" GST_PTR_FORMAT, GST_EVENT_TYPE_NAME (event), event);

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
    GstFlowReturn result = GST_FLOW_OK;
    
    GstFrameSaverPlugin * filter = GST_FRAME_SAVER_PLUGIN(parent);

    if (filter->is_pusher)
    {
        result = gst_pad_push(filter->srcpad, buf); /* push out the buffer untouched */
        
        filter->num_buffs += (result == GST_FLOW_OK) ? 1 : 0;

        if (filter->is_silent == FALSE)
        {
            g_print("FrameSaverPlugin --- pushed: #%u \n", filter->num_buffs);
        }    
    }
    else
    {
        result = (GstFlowReturn) Frame_Saver_Filter_Receive(GST_ELEMENT(filter), buf);

        if (filter->is_silent == FALSE)
        {
            g_print("FrameSaverPlugin --- Received: #%u \n", ++filter->num_buffs);
        }    
    }
    
    return result;
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
                  "FrameSaverPlugin_Description",
                  frame_saver_plugin_init,
                  VERSION,
                  "LGPL",
                  "GStreamer",
                  "http://gstreamer.net/")


