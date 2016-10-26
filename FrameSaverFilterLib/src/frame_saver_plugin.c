/* 
 * ======================================================================================
 * File:        frame_saver_plugin.c
 * 
 * History:     1. 2016-10-14   JBendor     Created
 *              2. 2016-10-25   JBendor     Updated 
 *
 * Description: Uses the Gstreamer TEE to splice one video source into two sinks.
 *
 *              The baseline TEE example code is in this URL:
 *              http://tordwessman.blogspot.com/2013/06/gstreamer-tee-code-example.html
 *
 *              The following URL describes Gstreamer streaming: 
 *              http://www.einarsundgren.se/gstreamer-basic-real-time-streaming-tutorial
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * ======================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>

#ifdef _WIN32
    #include <direct.h>
    #define GET_CWD(buf,lng)    _getcwd((buf),(lng))
    #define PATH_DELIMITER      '\\' 
#endif

#ifdef _LINUX
    #include <unistd.h>
    #define GET_CWD(buf,lng)    getcwd((buf),(lng))
    #define PATH_DELIMITER      '/'
#endif

#if (GST_VERSION_MAJOR == 0)
    #error "GST_VERSION_MAJOR must not be 0"
#endif

extern int save_image_frame_as_PNG_file(const char * aPathPtr,
                                        const char * aFormatPtr,
                                        void       * aPixmapPtr,
                                        int          aPixmapLng,
                                        int          aStrideLng,
                                        int          aFrameWdt, 
                                        int          aFrameHgt);

static const guint64    NANOS_PER_MILLISEC  = 1000L * 1000L;
static const guint32    NUM_APPSINK_BUFFERS = 2;

static const char * PIPELINE_NAME       = "THE_PIPE";
static const char * VID_SINK_1_NAME     = "VID_SNK1";
static const char * VID_SINK_2_NAME     = "VID_SNK2";
static const char * APP_SINK_2_NAME     = "APP_SNK2";
static const char * VID_SOURCE_NAME     = "VID_SRC";
static const char * VID_CVT_NAME        = "VID_CVT";
static const char * VID_TEE_NAME        = "TEE2TWO";
static const char * QUEUE_1_NAME        = "T_QUE_1";
static const char * QUEUE_2_NAME        = "T_QUE_2";
static const char * SRC_CAPS_FILTER_NAME= "SRC_CAPS_FILTER";
static const char * SNK_CAPS_FILTER_NAME= "SNK_CAPS_FILTER";

#ifdef _WIN32
	static const char * VID_SRC_ELEMENT = "videotestsrc";       // other: "ksvideosrc"
	static const char * VID_SNK_ELEMENT = "autovideosink";      // other: "d3dvideosink"
#endif

#ifdef _LINUX
	static const char * VID_SRC_ELEMENT = "videotestsrc";       // other: "v4l2src"
	static const char * VID_SNK_ELEMENT = "autovideosink";      // other: "ximagesink"
#endif

typedef struct
{
    GMainLoop   * main_loop_ptr;    // Main-Event-Loop --- manages all events

    GstBus      * bus_ptr;          // bus transfers messages from/to pipeline

    GstCaps     * source_caps_ptr,  // video-format: TEE --> Q1 --> sink-1
                * sinker_caps_ptr;  // video-fromat: TEE --> Q2 --> appsink

    GstElement  * pipeline_ptr,
                * vid_source_ptr,
                * video_sink1_ptr,
                * video_sink2_ptr,
                * src_caps_filter,
                * snk_caps_filter,
                * cvt_element_ptr,
                * tee_element_ptr,
                * Q_1_element_ptr,
                * Q_2_element_ptr;

    GstClockTime start_play_time_ns;
    GstClockTime spin_state_ends_ns;
    GstClockTime next_frame_snap_ns;

    guint   one_tick_ms,    // timer-ticks interval as milliseconds
            one_snap_ms,    // frame-snaps interval as milliseconds
            max_spin_ms,    // spin-state-timeout as milliseconds 
            max_play_ms,    // play-state-timeout as milliseconds 
            snap_signals,   // counter of pending signals to snap
            snap_counter;   // counter of frames snapped and saved

    GstAppSinkCallbacks appsink_callbacks;

    char folder_path[300];

} SnapsPlugin_t;


typedef enum 
{ 
    e_SPLICER_STATE_NONE = 0, 
    e_SPLICER_STATE_BUSY = 1,
    e_SPLICER_STATE_DONE = 2,
    e_SPLICER_STATE_USED = 3,
    e_SPLICER_STATE_FAIL = 4

} SPLICER_STATE_e;


typedef struct
{
    SPLICER_STATE_e status;

    GQueue          effects_queue;

    SnapsPlugin_t * ptr_snaps_plugin;

    GstPad        * ptr_from_pad;   // producer-out-pad feeds consumer-inp-pad --- never NULL
    GstPad        * ptr_into_pad;   // consumer-inp-pad feeds consumer-out-pad --- never NULL
    GstPad        * ptr_next_pad;   // consumer-out-pad feeds next-element-inp-pad -- NULL OK

    GstElement    * ptr_from_element;
    GstElement    * ptr_into_element;
    GstElement    * ptr_snaps_pipeline;

    GstElement    * ptr_current_effect;
    gchar*        * pp_effects_names;

    gchar           producer_element_name[30];
    gchar           producer_out_pad_name[30];

    gchar           consumer_element_name[20];
    gchar           consumer_inp_pad_name[20];
    gchar           consumer_out_pad_name[20];

} FlowSplicer_t;


static SnapsPlugin_t    myPlugin = { NULL };

static FlowSplicer_t    mySplicer = { e_SPLICER_STATE_NONE, G_QUEUE_INIT };


static int do_cleanup()
{
    if (myPlugin.main_loop_ptr != NULL)
    {
        g_main_loop_quit(myPlugin.main_loop_ptr);

        gst_object_unref(GST_OBJECT(myPlugin.pipeline_ptr));

        g_main_loop_unref(myPlugin.main_loop_ptr);

        myPlugin.pipeline_ptr = NULL;

        myPlugin.main_loop_ptr = NULL;
    }

    if (myPlugin.pipeline_ptr != NULL)
    {
        gst_object_unref(myPlugin.pipeline_ptr);

        myPlugin.pipeline_ptr = NULL;
    }

    return 0;
}


static gboolean do_bus_callback(GstBus *aBusPtr, GstMessage *aMsgPtr, void *aContextPtr)
{
    const GstStructure * gst_struct_ptr;

    GError    * ptr_err = NULL;
    gchar     * ptr_dbg = NULL;

    int msg_type = GST_MESSAGE_TYPE(aMsgPtr);

    GMainLoop * ptr_main_looper = aContextPtr;

    if (myPlugin.main_loop_ptr != ptr_main_looper)
    {
        return FALSE;   // TODO --- impossible
    }

    switch (msg_type)
    {
    case GST_MESSAGE_ERROR:
        gst_message_parse_error(aMsgPtr, &ptr_err, &ptr_dbg);
        printf("   bus_msg=ERROR --- %s", ptr_err->message);
        gst_object_default_error(aMsgPtr->src, ptr_err, ptr_dbg);
        g_clear_error(&ptr_err);
        g_error_free(ptr_err);
        g_free(ptr_dbg);
        g_main_loop_quit(ptr_main_looper);
        break;

    case GST_MESSAGE_APPLICATION:
        gst_struct_ptr = gst_message_get_structure(aMsgPtr);
        if (gst_structure_has_name(gst_struct_ptr, "turn_off"))
        {
            printf("   bus_msg=OFF \n");
            g_main_loop_quit(ptr_main_looper);
        }
        break;

    case GST_MESSAGE_EOS:
        printf("   bus_msg=EOS \n");
        g_main_loop_quit(ptr_main_looper);
        break;

    default:
        break;
    }

    if (msg_type == GST_MESSAGE_STATE_CHANGED)
    {
        GstState old_state, new_state, pending_state;

        gst_message_parse_state_changed(aMsgPtr, &old_state, &new_state, &pending_state);
        
        printf("   bus_msg=STATE --- old=%i  new=%i  pending=%i. \n", old_state, new_state, pending_state);
    }
    else
    {
        const char * psz_msg_type_name = GST_MESSAGE_TYPE_NAME(aMsgPtr); 

        int msg_time = (int) aMsgPtr->timestamp;

        printf("   bus_msg=OTHER --- name=(%s) time=%i  type=%i  \n", psz_msg_type_name, msg_time, msg_type);
    }

    return TRUE;
}


static gint do_parse_caps(const char * aCapsTextPtr, 
                          gchar * aFormatPtr, 
                          gint  * aNumColsPtr, 
                          gint  * aNumRowsPtr,
                          gint  * aNumBitsPtr)
{
    gint errors = 0;

    const char * ptr_field = strstr(aCapsTextPtr, "format=(string)");

    if (ptr_field != NULL)
    {
        for (ptr_field += 15;  (*ptr_field > ' ') && (*ptr_field != ',');  ++ptr_field)
        {
            *aFormatPtr++ = *ptr_field;
        }
    }
    *aFormatPtr = 0;

    ptr_field = strstr(aCapsTextPtr, "height=(int)");

    if (ptr_field != NULL)
    {
        errors += (sscanf(ptr_field + 12, "%d", aNumRowsPtr) == 1) ? 0 : 0x1;
    }

    ptr_field = strstr(aCapsTextPtr, "width=(int)");

    if (ptr_field != NULL)
    {
        errors += (sscanf(ptr_field + 11, "%d", aNumColsPtr) == 1) ? 0 : 0x2;
    }

    ptr_field = strstr(aCapsTextPtr, "depth=(int)");    // aka "bit-per-pixel"

    if (ptr_field != NULL)
    {
        errors += (sscanf(ptr_field + 11, "%d", aNumBitsPtr) == 1) ? 0 : 0x4;
    }

    ptr_field = strstr(aCapsTextPtr, "bpp=(int)");      // alias for "depth"

    if (ptr_field != NULL)
    {
        errors += (sscanf(ptr_field + 9, "%d", aNumBitsPtr) == 1) ? 0 : 0x8;
    }

    return errors;
}


static int do_snap_and_save_image_frame(GstAppSink * aAppSinkPtr, 
                                        GstBuffer  * aBufferPtr,
                                        GstCaps    * aCapsPtr,
                                        SnapsPlugin_t * aStatePtr)
{
    /* 
    * We set the caps on the appsink so that it can only be an rgb buffer. 
    * However, the height depends on the pixel-aspect-ratio of the source.
    * The stride of the video buffers is rounded to nearest multiple of 4.
    */
    char sz_format[100], sz_path[sizeof(aStatePtr->folder_path) + 100];

    GstMapInfo map;

    int  cols = 0, 
         rows = 0, 
         bits = 8,
         errs = do_parse_caps(gst_caps_to_string(aCapsPtr), sz_format, &cols, &rows, &bits);

    int stride = GST_ROUND_UP_4(cols * ((bits + 7) / 8));  // bytes in one row

    GstClockTime now = gst_clock_get_time (gst_system_clock_obtain());

    guint elapsed_ms = (guint) ((now - myPlugin.start_play_time_ns) / NANOS_PER_MILLISEC);

    if ( (errs != 0) || (TRUE != gst_buffer_map(aBufferPtr, &map, GST_MAP_READ)) )
    {
        return GST_FLOW_OK; // TODO --- GST_FLOW_ERROR;
    }

    sprintf(sz_path, "%s%s_%dx%dx%d.@%04u_%03u.#%u.png", 
            aStatePtr->folder_path,
            sz_format, 
            cols, 
            rows,
            bits,
            elapsed_ms / 1000,
            elapsed_ms % 1000,
            aStatePtr->snap_counter);

    errs = save_image_frame_as_PNG_file(sz_path, sz_format, map.data, (int) map.size, stride, cols, rows);

    gst_buffer_unmap (aBufferPtr, &map);

    return (errs == 0) ? GST_FLOW_OK : GST_FLOW_OK;  // TODO: GST_FLOW_ERROR;
}


static GstFlowReturn do_appsink_callback_for_new_data(GstAppSink * aAppSinkPtr, gpointer aUserDataPtr) 
{
    GstFlowReturn flow_result = GST_FLOW_OK;

    SnapsPlugin_t * ptr_state = (SnapsPlugin_t *) aUserDataPtr;

    GstSample  * sample_ptr = gst_app_sink_pull_sample(aAppSinkPtr);

    GstCaps    *   caps_ptr = sample_ptr ? gst_sample_get_caps(sample_ptr) : NULL;

    GstBuffer  * buffer_ptr = sample_ptr ? gst_sample_get_buffer(sample_ptr) : NULL;

    if ( (buffer_ptr == NULL) || (ptr_state == NULL) || (caps_ptr == NULL) )
    {
        gst_sample_unref(sample_ptr);
        return GST_FLOW_ERROR;
    }

    if (ptr_state->one_snap_ms > 0)
    {
        if (ptr_state->snap_signals > ptr_state->snap_counter)
        {
            ptr_state->snap_counter += 1;

            flow_result = do_snap_and_save_image_frame(aAppSinkPtr, 
                                                       buffer_ptr,
                                                       caps_ptr,
                                                       ptr_state);
        }

        gst_sample_unref(sample_ptr);
    }
    else
    {
        ; // TODO --- gst_app_src_push_buffer(GST_APP_SRC(ptr_my_state->vid_source_ptr), buffer_ptr);
    }

    return flow_result;
}


static GstPadProbeReturn do_appsink_inp_pad_probe_callback(GstPad          * aPadPtr, 
                                                           GstPadProbeInfo * aInfoPtr, 
                                                           gpointer          aContextPtr)
{
    GstEvent  * event_data_ptr = GST_PAD_PROBE_INFO_DATA(aInfoPtr);

    GstEventType  e_event_type = GST_EVENT_TYPE(event_data_ptr);

    if (e_event_type != GST_EVENT_EOS)
    {
        return GST_PAD_PROBE_PASS;
    }

    GST_DEBUG_OBJECT(aPadPtr, "the 'appsink-inp-pad' is now EOS !!! ");

    return GST_PAD_PROBE_OK;
}


static int do_setup_appsink(GstAppSink * aAppSinkPtr)
{
    myPlugin.appsink_callbacks.eos         = NULL;
    myPlugin.appsink_callbacks.new_preroll = NULL;
    myPlugin.appsink_callbacks.new_sample  = do_appsink_callback_for_new_data;

    gst_app_sink_set_callbacks(GST_APP_SINK(aAppSinkPtr), 
                               &myPlugin.appsink_callbacks, 
                               &myPlugin, // pointer_to_context_of_the_callback
                               NULL);     // pointer_to_destroy_notify_callback

    gst_app_sink_set_drop(aAppSinkPtr, TRUE);

    gst_app_sink_set_emit_signals(aAppSinkPtr, TRUE);

    gst_app_sink_set_max_buffers(aAppSinkPtr, NUM_APPSINK_BUFFERS);

    GstPad * ptr_inp_pad = gst_element_get_static_pad(GST_ELEMENT(aAppSinkPtr), "sink");

    // install consumer-inp-pad-probe for EOS event
    gulong probe_id = gst_pad_add_probe(ptr_inp_pad,
                                        GST_PAD_PROBE_TYPE_BLOCK | GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM,
                                        do_appsink_inp_pad_probe_callback,
                                        &myPlugin,
                                        NULL);


    return (probe_id ? 1 : 1);
}


static GstPadProbeReturn do_consumer_inp_pad_probe_callback(GstPad          * aPadPtr, 
                                                            GstPadProbeInfo * aInfoPtr, 
                                                            gpointer          aContextPtr)
{
    FlowSplicer_t * splicer_ptr = aContextPtr;

    GstEvent  * event_data_ptr = GST_PAD_PROBE_INFO_DATA(aInfoPtr);

    GstEventType  e_event_type = GST_EVENT_TYPE(event_data_ptr);
     
    if (e_event_type != GST_EVENT_EOS)
    {
        return GST_PAD_PROBE_PASS;
    }

    GST_DEBUG_OBJECT(aPadPtr, "the 'consumer-INP-pad' is now EOS !!! ");

    gst_pad_remove_probe(aPadPtr, GST_PAD_PROBE_INFO_ID(aInfoPtr));

    GstPad * ptr_out_pad = gst_element_get_static_pad(splicer_ptr->ptr_into_element, 
                                                      splicer_ptr->consumer_out_pad_name);

    if (ptr_out_pad != NULL)
    {
        // send EOS onto the consumer OUT pad
        gst_pad_send_event( ptr_out_pad, gst_event_new_eos() );

        gst_object_unref(ptr_out_pad);
    }

    // now we can insert the TEE and change links
    splicer_ptr->status = e_SPLICER_STATE_DONE;

    return GST_PAD_PROBE_DROP;
}


static GstPadProbeReturn do_producer_pad_probe_callback(GstPad          * aPadPtr, 
                                                        GstPadProbeInfo * aInfoPtr, 
                                                        gpointer          aContextPtr)
{
    FlowSplicer_t * splicer_ptr = aContextPtr;

    gulong  probe_info_id = GST_PAD_PROBE_INFO_ID(aInfoPtr);

    GST_DEBUG_OBJECT(aPadPtr, "the 'producer' pad is now blocked !!! ");

    // remove the producer-pad-probe
    gst_pad_remove_probe(aPadPtr, probe_info_id);

    GstPad * ptr_inp_pad = gst_element_get_static_pad(splicer_ptr->ptr_into_element, 
                                                      splicer_ptr->consumer_inp_pad_name);

    // install consumer-inp-pad-probe for EOS event
    gst_pad_add_probe(ptr_inp_pad,
                      GST_PAD_PROBE_TYPE_BLOCK | GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM,
                      do_consumer_inp_pad_probe_callback,
                      aContextPtr,
                      NULL);

    // send EOS onto the consumer INP pad
    gst_pad_send_event( ptr_inp_pad, gst_event_new_eos() );

    gst_object_unref(ptr_inp_pad);

    return GST_PAD_PROBE_OK;
}


static int do_unlink_two_linked_elements()
{
    #define DESIRED_EFFECTS  "videoflip, edgetv" // TODO --- maybe

    static gchar * Options_Effects_Ptr = NULL;

    static GOptionEntry Options_Effects_Array[] =
    {
        { "Options_Effects", 'e', 0, G_OPTION_ARG_STRING, &Options_Effects_Ptr, DESIRED_EFFECTS, NULL },
        { NULL }
    };

    mySplicer.status = e_SPLICER_STATE_FAIL;

    mySplicer.ptr_from_element = gst_bin_get_by_name(GST_BIN(myPlugin.pipeline_ptr), 
                                                     mySplicer.producer_element_name);

    mySplicer.ptr_into_element = gst_bin_get_by_name(GST_BIN(myPlugin.pipeline_ptr), 
                                                     mySplicer.consumer_element_name);

    // verify existence of the elements to be spliced by a TEE
    if ( (mySplicer.ptr_from_element == NULL) || (mySplicer.ptr_from_element == NULL) )
    {
        g_critical("pipeline missing one or both elements to be spliced by TEE \n");        
        return -1;
    }

    // a source pad produces data consumed by a sink pad of the next downstream element
    mySplicer.ptr_from_pad = gst_element_get_static_pad(mySplicer.ptr_from_element, 
                                                        mySplicer.producer_out_pad_name);

    mySplicer.ptr_into_pad = gst_element_get_static_pad(mySplicer.ptr_into_element, 
                                                        mySplicer.consumer_inp_pad_name);

    mySplicer.ptr_next_pad = gst_element_get_static_pad(mySplicer.ptr_into_element, 
                                                        mySplicer.consumer_out_pad_name);

    if (mySplicer.ptr_next_pad == NULL)
    {
        mySplicer.ptr_next_pad = mySplicer.ptr_into_pad;    // non-producers OK
    }

    // verify existence of the desired pads
    if ( (mySplicer.ptr_from_pad == NULL) || (mySplicer.ptr_into_pad == NULL) )
    {
        g_critical("desired pads missing in the elements to be spliced by TEE \n");        
        return -2;
    }

    // get desired effects/elements for probes --- TODO maybe someday
    if (mySplicer.pp_effects_names != NULL)
    {
        GOptionContext * ptr_options_ctx = g_option_context_new("");

        g_option_context_add_main_entries( ptr_options_ctx, Options_Effects_Array, NULL );

        g_option_context_add_group( ptr_options_ctx, gst_init_get_option_group() );

        g_option_context_free( ptr_options_ctx );

        if (Options_Effects_Ptr == NULL)
        {
            Options_Effects_Ptr = DESIRED_EFFECTS;
        }

        mySplicer.pp_effects_names = g_strsplit( Options_Effects_Ptr, ",", -1 );
    }

    if (mySplicer.pp_effects_names != NULL)
    {
        for (int index=0;  (mySplicer.pp_effects_names[index] != NULL);  ++index)
        {
            const gchar * psz_effect_name = mySplicer.pp_effects_names[index];

            GstElement  * ptr_new_effect = gst_element_factory_make(psz_effect_name, NULL);

            if (ptr_new_effect != NULL)
            {
                g_queue_push_tail( &mySplicer.effects_queue, ptr_new_effect );

                g_print("Added (%s) to Unlinker-Effects-Queue \n", *ptr_new_effect);
            }
            else
            {
                g_print("Failed to make Unlinker-Effect (%s) \n", psz_effect_name);
            }
        }
    }

    // start the safe unlinking procedure
    mySplicer.status             = e_SPLICER_STATE_BUSY;
    mySplicer.ptr_snaps_plugin   = &myPlugin; 
    mySplicer.ptr_snaps_pipeline = myPlugin.pipeline_ptr;

    // get the first queued effect --- possibly none
    mySplicer.ptr_current_effect = g_queue_pop_head( &mySplicer.effects_queue );

    // apply probe to 'producer' pad --- stop data flow into consumer element
    gst_pad_add_probe(mySplicer.ptr_from_pad, 
                      GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, 
                      do_producer_pad_probe_callback, 
                      &mySplicer, 
                      NULL);

    return 1;   // success --- safe unlinking procedure is now in progress
}


static int do_link_TEE_request_pads()
{
    GstPad  * tee_Q1_pad_ptr, 
            * tee_Q2_pad_ptr,
            * queue1_pad_ptr, 
            * queue2_pad_ptr;

    GstElementClass * class_ptr = GST_ELEMENT_GET_CLASS(myPlugin.tee_element_ptr);

    GstPadTemplate  * pad_template_ptr = gst_element_class_get_pad_template(class_ptr, "src_%u");

    if (! pad_template_ptr)
    {
        g_critical("Unable to get pad template");        
        return 0;
    }

    tee_Q1_pad_ptr = gst_element_request_pad(myPlugin.tee_element_ptr, 
                                             pad_template_ptr, 
                                             NULL, 
                                             NULL);

    g_print("Got request pad (%s) for tee-branch-Q1 \n", gst_pad_get_name(tee_Q1_pad_ptr));

    queue1_pad_ptr = gst_element_get_static_pad(myPlugin.Q_1_element_ptr, "sink");

    tee_Q2_pad_ptr = gst_element_request_pad(myPlugin.tee_element_ptr, 
                                             pad_template_ptr, 
                                             NULL, 
                                             NULL);

    g_print("Got request pad (%s) for tee-branch-Q2 \n", gst_pad_get_name(tee_Q2_pad_ptr));

    queue2_pad_ptr = gst_element_get_static_pad(myPlugin.Q_2_element_ptr, "sink");

    if (gst_pad_link(tee_Q1_pad_ptr, queue1_pad_ptr) != GST_PAD_LINK_OK)
    {
        g_critical("Tee for q1 could not be linked.\n");
        return 0;
    }

    if (gst_pad_link(tee_Q2_pad_ptr, queue2_pad_ptr) != GST_PAD_LINK_OK)
    {
        g_critical("Tee for q2 could not be linked.\n");
        return 0;
    }

    gst_object_unref(queue1_pad_ptr);
    gst_object_unref(queue2_pad_ptr);

    return 1;
}


static int do_insert_TEE_into_pipeline()
{
    // possibly --- our TEE is already in the pipeline --- nothing more to do
    if ( gst_element_get_parent(myPlugin.tee_element_ptr) != NULL )
    {
        return 1;
    }

    GstState  pipeline_state = GST_STATE_NULL;

    gst_element_get_state(myPlugin.pipeline_ptr, &pipeline_state, NULL, 0);

    // dynamic unlinking is needed when the pipeline is PLAYING
    if (pipeline_state != GST_STATE_NULL)
    {
        // possibly --- start new unlinking procedure
        if ((mySplicer.status == e_SPLICER_STATE_NONE) ||
            (mySplicer.status == e_SPLICER_STATE_USED) )
        {
            do_unlink_two_linked_elements();
        }

        // possibly --- wait for end of unlinking procedure
        if (mySplicer.status == e_SPLICER_STATE_BUSY)
        {
            return 0;
        }

        // suspend the producer and consumer elements
        GstStateChangeReturn rv1 = gst_element_set_state(mySplicer.ptr_from_element, GST_STATE_PAUSED);
        GstStateChangeReturn rv2 = gst_element_set_state(mySplicer.ptr_from_element, GST_STATE_PAUSED);
        GstStateChangeReturn rv3 = gst_element_set_state(myPlugin.pipeline_ptr,      GST_STATE_PAUSED);

        // possibly --- the unlinking procedure failed
        if (mySplicer.status != e_SPLICER_STATE_DONE)
        {
            return ((rv1 != rv2) || (rv1 != rv3)) ? -1 : -1;  // only for DBG
        }
    }

    mySplicer.status = e_SPLICER_STATE_USED;

    // put necessary elements in pipeline --- elements are not linked yet
    gst_bin_add_many(GST_BIN(myPlugin.pipeline_ptr),
                     myPlugin.tee_element_ptr,
                     myPlugin.Q_1_element_ptr,
                     myPlugin.Q_2_element_ptr,
                     myPlugin.snk_caps_filter,
                     myPlugin.video_sink2_ptr,
                     NULL);

    // now we can safely unlink the pads where the TEE must be inserted
    if (pipeline_state != GST_STATE_NULL)
    {
        gst_pad_unlink(mySplicer.ptr_from_pad, mySplicer.ptr_into_pad);
    }
    else
    {
        gst_element_unlink(myPlugin.vid_source_ptr, myPlugin.cvt_element_ptr);
    }

    // link elements: video-source to TEE --- T-QUE-2 flows into sink-2 via snk_caps_filter 
    if (FALSE == gst_element_link_many(myPlugin.vid_source_ptr,
                                       myPlugin.tee_element_ptr, 
                                       NULL))
    {
        g_critical("Unable to link elements: SRC --> TEE \n");       
        return -2;
    }

    // link elements: T-QUE-1 to CVT --- CVT flows into sink-1 via src_caps_filter
    if (FALSE == gst_element_link_many(myPlugin.Q_1_element_ptr,
                                       myPlugin.cvt_element_ptr, 
                                       NULL))
    {
        g_critical("Unable to link elements: T-Q1 --> CVT \n"); 
        return -3;
    }

    // link elements: queue-2 to snk_caps_filter to sink-2
    if (FALSE == gst_element_link_filtered(myPlugin.Q_2_element_ptr,
                                           myPlugin.video_sink2_ptr,
                                           myPlugin.sinker_caps_ptr )) 
    {
        g_critical("Unable to link elements: T-Q2 --> snk_caps_filter --> SINK2 \n"); 
        return -4;
    }    

    if (do_link_TEE_request_pads() <= 0)
    {
        return -5;
    }

    return 1;
}


static int do_frame_saver_plugin_make_base_pipeline()
{
    gst_init(NULL, NULL);

    myPlugin.pipeline_ptr = gst_pipeline_new(PIPELINE_NAME);

    myPlugin.bus_ptr = gst_pipeline_get_bus(GST_PIPELINE(myPlugin.pipeline_ptr));

    if ((! myPlugin.pipeline_ptr) || (! myPlugin.bus_ptr))
    {
        g_critical("Unable to create: pipeline or bus");
        return do_cleanup();
    }

    gst_element_set_state( GST_ELEMENT(myPlugin.pipeline_ptr), GST_STATE_NULL );

    gst_bus_add_watch(myPlugin.bus_ptr, do_bus_callback, NULL);
    gst_object_unref(myPlugin.bus_ptr);
    myPlugin.bus_ptr = NULL;

    // create the pipeline's elements (aka: plugins)
    myPlugin.vid_source_ptr  = gst_element_factory_make(VID_SRC_ELEMENT, VID_SOURCE_NAME);
    myPlugin.cvt_element_ptr = gst_element_factory_make("videoconvert",  VID_CVT_NAME);
    myPlugin.tee_element_ptr = gst_element_factory_make("tee",           VID_TEE_NAME);
    myPlugin.Q_1_element_ptr = gst_element_factory_make("queue",         QUEUE_1_NAME);
    myPlugin.Q_2_element_ptr = gst_element_factory_make("queue",         QUEUE_2_NAME);
    myPlugin.src_caps_filter = gst_element_factory_make("capsfilter",    SRC_CAPS_FILTER_NAME);
    myPlugin.snk_caps_filter = gst_element_factory_make("capsfilter",    SNK_CAPS_FILTER_NAME);

    if (myPlugin.one_snap_ms > 0)
    {
        myPlugin.video_sink1_ptr = gst_element_factory_make(VID_SNK_ELEMENT, VID_SINK_1_NAME);
        myPlugin.video_sink2_ptr = gst_element_factory_make("appsink",       APP_SINK_2_NAME);
        myPlugin.source_caps_ptr = gst_caps_from_string("video/x-raw, width=(int)500, height=(int)200");
        myPlugin.sinker_caps_ptr = gst_caps_from_string("video/x-raw, format=(string)RGB, bpp=(int)24");
    }
    else
    {
        myPlugin.video_sink1_ptr = gst_element_factory_make(VID_SNK_ELEMENT, VID_SINK_1_NAME);
        myPlugin.video_sink2_ptr = gst_element_factory_make(VID_SNK_ELEMENT, VID_SINK_2_NAME);
        myPlugin.source_caps_ptr = gst_caps_from_string("video/x-raw, width=(int)500, height=(int)200");
        myPlugin.sinker_caps_ptr = gst_caps_from_string("video/x-raw, width=(int)500, height=(int)200");
    }

    int flags = (myPlugin.vid_source_ptr  ? 0 : 0x001) + 
                (myPlugin.cvt_element_ptr ? 0 : 0x002) +
                (myPlugin.video_sink1_ptr ? 0 : 0x004) + 
                (myPlugin.video_sink2_ptr ? 0 : 0x008) +
                (myPlugin.Q_1_element_ptr ? 0 : 0x010) +
                (myPlugin.Q_2_element_ptr ? 0 : 0x020) +
                (myPlugin.tee_element_ptr ? 0 : 0x040) +
                (myPlugin.source_caps_ptr ? 0 : 0x100) +
                (myPlugin.sinker_caps_ptr ? 0 : 0x200) +
                (myPlugin.src_caps_filter ? 0 : 0x400) +
                (myPlugin.snk_caps_filter ? 0 : 0x800) ;


    if (flags != 0)
    {
        g_critical("Failed making some pipeline elements (flags=0x%0X)", flags);
        return do_cleanup();
    }

    g_object_set(G_OBJECT(myPlugin.src_caps_filter), "caps", myPlugin.source_caps_ptr, NULL);
    g_object_set(G_OBJECT(myPlugin.snk_caps_filter), "caps", myPlugin.sinker_caps_ptr, NULL);

    gst_bin_add_many(GST_BIN(myPlugin.pipeline_ptr), 
                     myPlugin.vid_source_ptr,
                     myPlugin.cvt_element_ptr,
                     myPlugin.src_caps_filter,
                     myPlugin.video_sink1_ptr,  
                     NULL);

    // link elements: video-source to video-convert to src_caps_filter to video-sink-1 
    if (FALSE == gst_element_link_many(myPlugin.vid_source_ptr,
                                       myPlugin.cvt_element_ptr,
                                       myPlugin.src_caps_filter,
                                       myPlugin.video_sink1_ptr, 
                                       NULL))
    {
        g_critical("Failed linking elements: SRC --> SRC_CAPS_FILTER --> SINK1 \n");        
        return do_cleanup();
    }

    // possibly --- setup the app-sink callback to handle newly arriving frames
    if (myPlugin.one_snap_ms > 0)
    {
        do_setup_appsink( (GstAppSink*) myPlugin.video_sink2_ptr );
    }

    if (myPlugin.max_spin_ms == 0)
    {
        if (do_insert_TEE_into_pipeline() <= 0)
        {
            return do_cleanup();
        }
    }

    return 1;
}

static gboolean do_frame_saver_plugin_timer_callback(gpointer aCtxPtr); // forward

static gboolean do_frame_saver_plugin_idle_callback(gpointer aCtxPtr)
{
    GstClockTime present_ns = gst_clock_get_time (gst_system_clock_obtain());

    GstClockTime  future_ns = present_ns + (NANOS_PER_MILLISEC * myPlugin.one_tick_ms);

    GstClockTime elapsed_ns = present_ns - myPlugin.start_play_time_ns;

    uint32_t        play_ms = (uint32_t) (elapsed_ns / NANOS_PER_MILLISEC);

    gboolean  is_TEE_needed = (myPlugin.spin_state_ends_ns < future_ns);

    gboolean  is_TEE_linked = (gst_element_get_parent(myPlugin.tee_element_ptr) != NULL);

    if ( is_TEE_needed && (! is_TEE_linked) )
    {
        if ((mySplicer.status == e_SPLICER_STATE_NONE) || (mySplicer.status == e_SPLICER_STATE_USED) )
        {
            printf("frame_saver_plugin --- elapsed: %u %s ", play_ms, "... TEE insertion is STARTING \n");
        }

        int status = do_insert_TEE_into_pipeline();

        if (status > 0)    // success --- the TEE was inserted into pipeline
        {
            gst_element_set_state( myPlugin.pipeline_ptr, GST_STATE_PLAYING );
            gst_element_set_state( myPlugin.vid_source_ptr, GST_STATE_PLAYING );
            gst_element_set_state( myPlugin.video_sink1_ptr, GST_STATE_PLAYING );
            gst_element_set_state( myPlugin.video_sink2_ptr, GST_STATE_PLAYING );

            printf("frame_saver_plugin --- elapsed: %u %s", play_ms, "... TEE insertion is COMPLETE \n");
        }
        else if (status < 0)    // insertion or unlinking failed --- No-Go
        {
            printf("frame_saver_plugin --- elapsed: %u %s ", play_ms, "... TEE insertion has FAILED \n");

            gst_element_set_state( myPlugin.pipeline_ptr, GST_STATE_READY );

            myPlugin.spin_state_ends_ns = myPlugin.spin_state_ends_ns + (NANOS_PER_MILLISEC * 1000);

            g_timeout_add ( (guint) myPlugin.one_tick_ms, do_frame_saver_plugin_timer_callback, NULL );
        }
        else // (status == 0)
        {
            //printf("frame_saver_plugin --- elapsed: %u %s", play_ms, " ... TEE insertion is ONGOING \n");
        }
    }

    return TRUE;
}


static gboolean do_frame_saver_plugin_timer_callback(gpointer aCtxPtr)
{
    GstClockTime present_ns = gst_clock_get_time (gst_system_clock_obtain());

    GstClockTime elapsed_ns = present_ns - myPlugin.start_play_time_ns;

    uint32_t        play_ms = (uint32_t) (elapsed_ns / NANOS_PER_MILLISEC);

    gboolean        is_idle = (myPlugin.spin_state_ends_ns > present_ns);

    printf("frame_saver_plugin --- elapsed: %u %s ", play_ms, (is_idle ? "... idle" : ""));

    // possibly --- trigger the next frame snap
    if ( elapsed_ns >= myPlugin.next_frame_snap_ns )
    {
        myPlugin.next_frame_snap_ns += NANOS_PER_MILLISEC * myPlugin.one_snap_ms;

        if ( (myPlugin.one_snap_ms > 0) && (! is_idle) )
        {
            myPlugin.snap_signals += 1;

            printf("... snaps=%u ... saves=%u ", myPlugin.snap_signals, myPlugin.snap_counter );
        }
    }

    printf("\n");

    if ( play_ms > myPlugin.max_play_ms )
    {
        gst_element_send_event ( myPlugin.vid_source_ptr, gst_event_new_eos() );

        gst_element_set_state( myPlugin.vid_source_ptr, GST_STATE_READY );

        gst_element_set_state( myPlugin.pipeline_ptr, GST_STATE_NULL );

        do_cleanup();

        return FALSE;
    }    

    return TRUE;
}


static void do_frame_saver_plugin_run()
{
    myPlugin.main_loop_ptr = g_main_loop_new(NULL, FALSE);

    g_idle_add( do_frame_saver_plugin_idle_callback, &myPlugin );

    g_timeout_add ( (guint) myPlugin.one_tick_ms, do_frame_saver_plugin_timer_callback, NULL );

    gst_element_set_state(myPlugin.pipeline_ptr, GST_STATE_PLAYING);

    myPlugin.start_play_time_ns = gst_clock_get_time (gst_system_clock_obtain());

    myPlugin.spin_state_ends_ns = (NANOS_PER_MILLISEC * myPlugin.max_spin_ms) + myPlugin.start_play_time_ns;

    myPlugin.next_frame_snap_ns = (NANOS_PER_MILLISEC * myPlugin.one_snap_ms);

    myPlugin.snap_counter = 0;

    myPlugin.snap_signals = 0;

#ifdef xxxxxxxxxxxxxxxx_DEBUG
    if ( (strstr(myPlugin.folder_path, "WIN64.GST") != NULL) ||
        ( strstr(myPlugin.folder_path, "LNX64.GST") != NULL) )
    {
        char file_path[sizeof(myPlugin.folder_path)];
        int result = sprintf(file_path, "%s_%u.png", myPlugin.folder_path, (int) time(NULL));
        result = save_image_frame_as_PNG_file(file_path, "RGB24", NULL, 0, 80*3, 80, 40);
        printf("created PNG file (%s) (result=%d) \n", file_path, result);
    }
#endif

    printf("frame_saver_plugin --- active --- %s \n", myPlugin.folder_path);

    g_main_loop_run(myPlugin.main_loop_ptr);

    printf("frame_saver_plugin --- ended \n");
}


static int do_frame_saver_plugin_set_work_folder(const char * aFolderPathPtr)
{
    int length = 0;

    time_t now = time(NULL);

    struct tm * tm_ptr = localtime(&now);

    if (aFolderPathPtr == NULL)
    {
        GET_CWD(myPlugin.folder_path, sizeof(myPlugin.folder_path));

        length = (int) strlen(myPlugin.folder_path);
    }
    else
    {
        length = snprintf(myPlugin.folder_path, sizeof(myPlugin.folder_path), "%s", aFolderPathPtr);
    }

    if ( (length < 2) || (sizeof(myPlugin.folder_path) < length + 30) )
    {
        return ~length;     // error: need space for names of folder and files
    }

    length += sprintf(myPlugin.folder_path + length, "%c%04d%02d%02d_%02d%02d%02d%c", 
                      PATH_DELIMITER,
                      tm_ptr->tm_year + 1900, 
                      tm_ptr->tm_mon + 1, 
                      tm_ptr->tm_mday, 
                      tm_ptr->tm_hour, 
                      tm_ptr->tm_min, 
                      tm_ptr->tm_sec,
                      PATH_DELIMITER);

    return (mkdir(myPlugin.folder_path) ? 0 : length); // returns 0 iff failed
}


static gboolean do_frame_saver_plugin_parse_args(int argc, char *argv[])
{
    gboolean is_ok = TRUE;

    myPlugin.one_tick_ms = 1000;
    myPlugin.one_snap_ms = 3000;
    myPlugin.max_spin_ms = 4000;
    myPlugin.max_play_ms = 11000;

    do_frame_saver_plugin_set_work_folder(NULL);

    sprintf(mySplicer.producer_element_name, "%s", VID_SOURCE_NAME);
    sprintf(mySplicer.consumer_element_name, "%s", VID_CVT_NAME );

    sprintf(mySplicer.producer_out_pad_name, "%s", "src");
    sprintf(mySplicer.consumer_inp_pad_name, "%s", "sink");
    sprintf(mySplicer.consumer_out_pad_name, "%s", "src");

    if ( (argc < 2) || (! argv) )
    {
        printf("\n using: %s=%d %s=%d %s=%d %s=%d %s=(%s) %s=%s,%s,%s %s=%s,%s,%s  \n\n",
               "\n tick", myPlugin.one_tick_ms,
               "\n snap", myPlugin.one_snap_ms,
               "\n spin", myPlugin.max_spin_ms,
               "\n play", myPlugin.max_play_ms,
               "\n path", myPlugin.folder_path,
               "\n poke", PIPELINE_NAME,
                          mySplicer.producer_element_name, 
                          mySplicer.consumer_element_name,
               "\n pads", mySplicer.producer_out_pad_name, 
                          mySplicer.consumer_inp_pad_name, 
                          mySplicer.consumer_out_pad_name);

        return TRUE;
    }

    while ( is_ok && (--argc > 0) )
    {
        const char * psz_param = argv[argc];

        if ( strncmp(psz_param, "tick=", 5) == 0 )
        {
            is_ok = (sscanf(&psz_param[5], "%d", &myPlugin.one_tick_ms) == 1) && (myPlugin.one_tick_ms >= 0);
            continue;
        }

        if ( strncmp(psz_param, "snap=", 5) == 0 )
        {
            is_ok = (sscanf(&psz_param[5], "%d", &myPlugin.one_snap_ms) == 1) && (myPlugin.one_snap_ms >= 0);
            continue;
        }

        if ( strncmp(psz_param, "spin=", 5) == 0 )
        {
            is_ok = (sscanf(&psz_param[5], "%d", &myPlugin.max_spin_ms) == 1) && (myPlugin.max_spin_ms >= 0);
            continue;
        }

        if ( strncmp(psz_param, "play=", 5) == 0 )
        {
            is_ok = (sscanf(&psz_param[5], "%d", &myPlugin.max_play_ms) == 1) && (myPlugin.max_play_ms >= 0);
            continue;
        }

        if ( strncmp(psz_param, "path=", 5) == 0 )
        {
            is_ok = (do_frame_saver_plugin_set_work_folder( &psz_param[5] ) > 0);
            continue;
        }
    }

    if (is_ok)
    {
        if (myPlugin.one_tick_ms < 100)
        {
            printf("minimum 'tick' time interval is 100 miliseconds \n");
            is_ok = FALSE;
        }
        else if (myPlugin.one_tick_ms < myPlugin.max_play_ms)
        {
            printf("minimum 'play' milliseconds is one 'tick' \n");      
            is_ok = FALSE;
        }
        else if (myPlugin.max_play_ms < myPlugin.max_spin_ms)
        {
            printf("maximum 'spin' milliseconds is 'play' \n");      
            is_ok = FALSE;
        }
    }
    else
    {
        printf("invalid arg: (%s) \n", argv[argc]);
    }

    return is_ok;
}


int frame_saver_plugin_tester( int argc, char *argv[] )
{
    int result = 0;

    if (do_frame_saver_plugin_parse_args(argc, argv) == FALSE)
    {
        printf("\n");
        result = -1;
    }
    else if (do_frame_saver_plugin_make_base_pipeline() == 0)
    {
        printf("frame_saver_plugin_tester --- failed creating pipeline \n");
        result = -2;
    }
    else
    {
        do_frame_saver_plugin_run();
    }

    return result;
}
