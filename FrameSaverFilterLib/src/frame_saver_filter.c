/* 
 * ======================================================================================
 * File:        frame_saver_filter.c
 * 
 * History:     1. 2016-10-14   JBendor     Created
 *              2. 2016-10-28   JBendor     Updated
 *              3. 2016-10-29   JBendor     Removed parameters code to new file
 *              4. 2016-11-02   JBendor     Support for custom pipelines
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

#include "frame_saver_filter.h"
#include "frame_saver_params.h"

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>


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


//=======================================================================================
// custom types
//=======================================================================================
typedef struct
{
    GMainLoop   * main_loop_ptr;    // Main-Event-Loop --- manages all events

    GstBus      * bus_ptr;          // bus transfers messages from/to pipeline

    GstCaps     * source_caps_ptr,  // video-format: TEE --> Q1 --> sink-1
                * sinker_caps_ptr;  // video-fromat: TEE --> Q2 --> appsink

    GstElement  * pipeline_ptr,
                * vid_sourcer_ptr,
                * video_sink1_ptr,
                * video_sink2_ptr,
                * src_caps_filter,
                * cvt_element_ptr,
                * tee_element_ptr,
                * Q_1_element_ptr,
                * Q_2_element_ptr;

    GstClock    * sys_clock_ptr;

    GstClockTime start_play_time_ns;    // timestamp of pipeline startup
    GstClockTime spin_state_ends_ns;    // timestamp for a TEE insertion
    GstClockTime frame_snap_wait_ns;    // wait time for next frame snap

    guint   num_snap_signals,           // count of signals to snap frames
            num_saved_frames,           // count of frames saved as files
            num_saver_errors,           // count of frames saver's errors
            num_appsink_frames,         // count of appsink's input frames
            num_appsink_errors;         // count of appsink's input errors

    GstAppSinkCallbacks appsink_callbacks;

    gchar   work_folder_path[PATH_MAX + 1];

} FlowSniffer_t;


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

    SplicerParams_t params;

    FlowSniffer_t * frame_saver_filter;

    GstCaps       * ptr_all_from_caps,
                  * ptr_all_into_caps;

    GstPad        * ptr_from_pad;   // producer-out-pad feeds consumer-inp-pad --- never NULL
    GstPad        * ptr_into_pad;   // consumer-inp-pad feeds consumer-out-pad --- never NULL
    GstPad        * ptr_next_pad;   // consumer-out-pad feeds next-element-inp-pad -- NULL OK

    GstElement    * ptr_from_element;
    GstElement    * ptr_into_element;
    GstElement    * ptr_snaps_pipeline;

    GstElement    * ptr_current_effect;
    gchar*        * pp_effects_names;

} FlowSplicer_t;


static FlowSniffer_t    mySniffer = { NULL };

static FlowSplicer_t    mySplicer = { e_SPLICER_STATE_NONE, G_QUEUE_INIT };


//=======================================================================================
// synopsis: result = do_appsink_snap_frame(aAppSinkPtr, aBufferPtr, aCapsPtr, aStatePtr)
//
// snaps and saves a frame --- returns GST_FLOW_OK on success, else GST_FLOW_ERROR
//=======================================================================================
static gint do_appsink_snap_frame(GstAppSink    * aAppSinkPtr, 
                                  GstBuffer     * aBufferPtr,
                                  GstCaps       * aCapsPtr,
                                  FlowSniffer_t * aStatePtr)
{
    /* 
    * NOTE-1: frame height can depend on the pixel-aspect-ratio of the source.
    *
    * NOTE-2: stride of video buffers is rounded to the nearest multiple of 4.
    */
    char sz_image_format[100], 
         sz_image_path[PATH_MAX + 100];

    GstMapInfo map;

    int  cols = 0, 
         rows = 0, 
         bits = 8,
         errs = pipeline_params_parse_caps(gst_caps_to_string(aCapsPtr), 
                                           sz_image_format, 
                                           &cols, 
                                           &rows, 
                                           &bits);

    int stride = GST_ROUND_UP_4(cols * ((bits + 7) / 8));  // bytes in one row

    GstClockTime now = gst_clock_get_time (aStatePtr->sys_clock_ptr);

    guint elapsed_ms = (guint) ((now - mySniffer.start_play_time_ns) / NANOS_PER_MILLISEC);

    if ( (errs != 0) || (TRUE != gst_buffer_map(aBufferPtr, &map, GST_MAP_READ)) )
    {
        return GST_FLOW_OK; // TODO --- GST_FLOW_ERROR;
    }

    aStatePtr->num_saved_frames += 1;

    sprintf(sz_image_path, 
            "%s%s_%dx%dx%d.@%04u_%03u.#%u.png", 
            aStatePtr->work_folder_path,
            sz_image_format, 
            cols, 
            rows,
            bits,
            elapsed_ms / 1000,
            elapsed_ms % 1000,
            aStatePtr->num_saved_frames);

    errs = save_image_frame_as_PNG_file(sz_image_path, 
                                        sz_image_format, 
                                        map.data, 
                                        (int) map.size, 
                                        stride, 
                                        cols, 
                                        rows);

    if (errs != 0)
    {
        aStatePtr->num_saver_errors += 1;
    }

    gst_buffer_unmap (aBufferPtr, &map);

    return (errs == 0) ? GST_FLOW_OK : GST_FLOW_OK;  // TODO: GST_FLOW_ERROR;
}


//=======================================================================================
// synopsis: result = do_appsink_callback_probe_inp_pad(aAppSinkPtr, aContextPtr)
//
// appsink callback for new-frame events
//=======================================================================================
static GstFlowReturn do_appsink_callback_for_new_frame(GstAppSink * aAppSinkPtr, 
                                                       gpointer     aContextPtr) 
{
    GstFlowReturn flow_result = GST_FLOW_OK;

    FlowSniffer_t * ptr_state = (FlowSniffer_t *) aContextPtr;

    GstSample    * sample_ptr = gst_app_sink_pull_sample(aAppSinkPtr);

    GstCaps      *   caps_ptr = sample_ptr ? gst_sample_get_caps(sample_ptr) : NULL;

    GstBuffer    * buffer_ptr = sample_ptr ? gst_sample_get_buffer(sample_ptr) : NULL;

    if ( (buffer_ptr == NULL) || (ptr_state != &mySniffer) || (caps_ptr == NULL) )
    {
        ptr_state->num_appsink_errors += 1;

        gst_sample_unref(sample_ptr);

        return GST_FLOW_ERROR;
    }

    ptr_state->num_appsink_frames += 1;

    if (mySplicer.params.one_snap_ms > 0)
    {
        if (ptr_state->num_snap_signals > ptr_state->num_saved_frames)
        {
            flow_result = do_appsink_snap_frame(aAppSinkPtr, 
                                                       buffer_ptr,
                                                       caps_ptr,
                                                       ptr_state);
        }

        gst_sample_unref(sample_ptr);
    }
    else
    {
        ; // TODO --- gst_app_src_push_buffer(GST_APP_SRC(ptr_my_state->vid_sourcer_ptr), buffer_ptr);
    }

    return flow_result;
}


//=======================================================================================
// synopsis: result = do_appsink_callback_probe_inp_pad(aPadPtr, aInfoPtr, aCtxPtr)
//
// appsink callback for input-pad events
//=======================================================================================
static GstPadProbeReturn do_appsink_callback_probe_inp_pad(GstPad          * aPadPtr, 
                                                           GstPadProbeInfo * aInfoPtr, 
                                                           gpointer          aCtxPtr)
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


//=======================================================================================
// synopsis: is_ok = do_appsink_trigger_next_frame_snap(elapsedPlaytimeMillis)
//
// triggers frame snaps --- returns TRUE on success
//=======================================================================================
static gboolean do_appsink_trigger_next_frame_snap(uint32_t elapsedPlaytimeMillis)
{
    GstClockTime next_snap_nanos = NANOS_PER_MILLISEC * mySplicer.params.one_snap_ms;

    // establish a desired time for next frame snap
    mySniffer.frame_snap_wait_ns += next_snap_nanos;

    // increment the number of snap-signals --- create new folder on first snap
    if (++mySniffer.num_snap_signals == 1)
    {
        time_t now = time(NULL);

        struct tm * tm_ptr = localtime(&now);

        int length = (int) strlen(mySniffer.work_folder_path);

        sprintf(&mySniffer.work_folder_path[length],
                "%c%s%04d%02d%02d_%02d%02d%02d%c",
                PATH_DELIMITER,
                "IMAGES_",
                tm_ptr->tm_year + 1900,
                tm_ptr->tm_mon + 1,
                tm_ptr->tm_mday,
                tm_ptr->tm_hour,
                tm_ptr->tm_min,
                tm_ptr->tm_sec,
                PATH_DELIMITER);

        if (MK_RWX_DIR(mySniffer.work_folder_path) == 0)
        {
            g_print("frame_saver_filter --- playtime=%u %s (%s) \n", 
                    elapsedPlaytimeMillis,
                    "... New Folder",
                    &mySniffer.work_folder_path[length+1]);
        }
        else
        {
            g_print("frame_saver_filter --- playtime=%u %s (%s) NOT created \n", 
                    elapsedPlaytimeMillis,
                    "... New Folder",
                    &mySniffer.work_folder_path[0]);

            mySniffer.work_folder_path[length] = 0;

            mySniffer.num_snap_signals = 0;
        }

        next_snap_nanos += NANOS_PER_MILLISEC * elapsedPlaytimeMillis;

        mySniffer.frame_snap_wait_ns = next_snap_nanos;
    }

    g_print("frame_saver_filter --- playtime=%u ... snap=%u ... save=%u ... error=%u,%u ... appsink=%u\n",
            elapsedPlaytimeMillis,
            mySniffer.num_snap_signals,
            mySniffer.num_saved_frames,
            mySniffer.num_saver_errors,
            mySniffer.num_appsink_errors,
            mySniffer.num_appsink_frames);

    return TRUE;
}

//=======================================================================================
// synopsis: appsink_ptr = do_appsink_create(aNamePtr, aNumBuffers)
//
// creates & configures the appsink for frame snaps --- returns element pointer
//=======================================================================================
static GstElement* do_appsink_create(const char * aNamePtr, int aNumBuffers)
{
    GstElement * appsink_ptr = gst_element_factory_make("appsink", aNamePtr);
    
    mySniffer.appsink_callbacks.eos         = NULL;
    mySniffer.appsink_callbacks.new_preroll = NULL;
    mySniffer.appsink_callbacks.new_sample  = do_appsink_callback_for_new_frame;

    gst_app_sink_set_callbacks(GST_APP_SINK(appsink_ptr), 
                               &mySniffer.appsink_callbacks, 
                               &mySniffer, // pointer_to_context_of_the_callback
                               NULL);      // pointer_to_destroy_notify_callback

    gst_app_sink_set_drop(GST_APP_SINK(appsink_ptr), TRUE);

    gst_app_sink_set_emit_signals(GST_APP_SINK(appsink_ptr), TRUE);

    gst_app_sink_set_max_buffers(GST_APP_SINK(appsink_ptr), aNumBuffers);

    GstPad * ptr_inp_pad = gst_element_get_static_pad(GST_ELEMENT(appsink_ptr), "sink");

    // install consumer-inp-pad-probe for EOS event
    gulong probe_id = gst_pad_add_probe(ptr_inp_pad,
                                        GST_PAD_PROBE_TYPE_BLOCK | GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM,
                                        do_appsink_callback_probe_inp_pad,
                                        &mySniffer,
                                        NULL);


    return (probe_id ? appsink_ptr : appsink_ptr);
}


//=======================================================================================
// synopsis: is_ok = do_splicer_negotiate_pads_caps(aPadPtr, aCtsPtr, aQueryPtr)
//
// establishes an acceptable CAPS for linked pads
//=======================================================================================
static gboolean do_splicer_negotiate_pads_caps(GstPad    * aPadPtr, 
                                               GstObject * aCtxPtr, 
                                               GstQuery  * aQueryPtr)
{
    gint query_id = GST_QUERY_TYPE(aQueryPtr);

    if (query_id != GST_QUERY_CAPS) 
    {
        return gst_pad_query_default(aPadPtr, aCtxPtr, aQueryPtr);
    }

    if (aCtxPtr != (GstObject *) &mySplicer)
    {
        return FALSE;
    }

    gboolean  peer_is_from = (aPadPtr == mySplicer.ptr_into_pad);

//  GstPad  * ptr_peer_pad = peer_is_from ? mySplicer.ptr_from_pad : mySplicer.ptr_into_pad;

    GstCaps * ptr_all_caps = peer_is_from ? mySplicer.ptr_all_from_caps : mySplicer.ptr_all_into_caps;

    GstCaps * ptr_query_caps;    

    GstCaps * ptr_final_caps; 

    gst_query_parse_caps(aQueryPtr, &ptr_query_caps);

    ptr_final_caps = ptr_query_caps; 

    if (ptr_all_caps != NULL)
    {
        int idx = gst_caps_get_size(ptr_all_caps);

        // discard sample-rate to allow ANY rate supported by linked elements
        while ( --idx >= 0) 
        {
            GstStructure *structure = gst_caps_get_structure(ptr_all_caps, idx);
            gst_structure_remove_field (structure, "rate");
        }

        // the returned CAPS must "intersect" with the pad-template-caps
        GstCaps * ptr_template = gst_pad_get_pad_template_caps(aPadPtr);
        if (ptr_template) 
        {
            ptr_final_caps = gst_caps_intersect(ptr_all_caps, ptr_template);
            gst_caps_unref(ptr_template);
        }

        // possibly --- match with the query filter
        if (ptr_query_caps) 
        {
            ptr_final_caps = gst_caps_intersect(ptr_all_caps, ptr_query_caps);
            gst_caps_unref (ptr_query_caps);
        }
    }

    if (ptr_final_caps)
    {
        gst_query_set_caps_result (aQueryPtr, ptr_final_caps);
        //gst_caps_unref (ptr_final_caps);
    }

    return TRUE;
}

 
//=======================================================================================
// synopsis: result = do_callback_for_consumer_INP_pad_events(aPadPtr, aInfoPtr, aCtxPtr)
//
// splicer callback for producer-element-output-pad events
//=======================================================================================
static gboolean do_callback_for_consumer_INP_pad_events(GstPad    * aPadPtr, 
                                                        GstObject * aCtxPtr, 
                                                        GstEvent  * aEventPtr)
{
    gboolean is_ok = FALSE;

    gint  event_id = GST_EVENT_TYPE(aEventPtr);

    if (event_id == GST_EVENT_CAPS)
    {
        GstQuery * ptr_query = gst_query_new_caps (mySplicer.ptr_all_from_caps);

        is_ok = do_splicer_negotiate_pads_caps(aPadPtr, (GstObject*) &mySplicer, ptr_query);

        gst_query_unref(ptr_query);
    }
    else
    {
        is_ok = gst_pad_event_default(aPadPtr, aCtxPtr, aEventPtr);
    }

    return is_ok;
}


//=======================================================================================
// synopsis: result = do_splicer_callback_for_consumer_inp_pad(aPadPtr, aInfoPtr, aCtxPtr)
//
// splicer callback for consumer-element-input-pad events
//=======================================================================================
static GstPadProbeReturn do_splicer_callback_for_consumer_inp_pad(GstPad          * aPadPtr, 
                                                                  GstPadProbeInfo * aInfoPtr, 
                                                                  gpointer          aCtxPtr)
{
    FlowSplicer_t * splicer_ptr = aCtxPtr;

    GstEvent  * event_data_ptr = GST_PAD_PROBE_INFO_DATA(aInfoPtr);

    GstEventType  e_event_type = GST_EVENT_TYPE(event_data_ptr);
     
    if (e_event_type != GST_EVENT_EOS)
    {
        return GST_PAD_PROBE_PASS;
    }

    GST_DEBUG_OBJECT(aPadPtr, "the 'consumer-INP-pad' is now EOS !!! ");

    gst_pad_remove_probe(aPadPtr, GST_PAD_PROBE_INFO_ID(aInfoPtr));

    GstPad * ptr_out_pad = gst_element_get_static_pad(splicer_ptr->ptr_into_element, 
                                                      splicer_ptr->params.consumer_out_pad_name);

    // possibly --- send EOS onto the consumer OUT pad --- possibly redundant
    if (ptr_out_pad != NULL)
    {
        // TODO--- gst_pad_send_event( ptr_out_pad, gst_event_new_eos() );

        gst_object_unref(ptr_out_pad);
    }

    // now we can insert the TEE and change links
    splicer_ptr->status = e_SPLICER_STATE_DONE;

    return GST_PAD_PROBE_DROP;
}


//=======================================================================================
// synopsis: result = do_callback_for_splicer_producer_pad(aPadPtr, aInfoPtr, aCtxPtr)
//
// splicer callback for producer-element-output-pad events
//=======================================================================================
static GstPadProbeReturn do_callback_for_splicer_producer_pad(GstPad          * aPadPtr, 
                                                              GstPadProbeInfo * aInfoPtr, 
                                                              gpointer          aCtxPtr)
{
    FlowSplicer_t * splicer_ptr = aCtxPtr;

    gulong  probe_info_id = GST_PAD_PROBE_INFO_ID(aInfoPtr);

    GST_DEBUG_OBJECT(aPadPtr, "the 'producer' pad is now blocked !!! ");

    // remove the producer-pad-probe
    gst_pad_remove_probe(aPadPtr, probe_info_id);

    GstPad * ptr_inp_pad = gst_element_get_static_pad(splicer_ptr->ptr_into_element, 
                                                      splicer_ptr->params.consumer_inp_pad_name);

    // install consumer-inp-pad-probe for EOS event
    gst_pad_add_probe(ptr_inp_pad,
                      GST_PAD_PROBE_TYPE_BLOCK | GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM,
                      do_splicer_callback_for_consumer_inp_pad,
                      aCtxPtr,
                      NULL);

    // send EOS onto the consumer INP pad
    gst_pad_send_event( ptr_inp_pad, gst_event_new_eos() );

    gst_object_unref(ptr_inp_pad);

    return GST_PAD_PROBE_OK;
}


//=======================================================================================
// synopsis: result = do_splicer_unlink_two_elements()
//
// safely unlinks two inter-linked elements --- returns 1 on success, else rrror
//=======================================================================================
static gint do_splicer_unlink_two_elements()
{
    #define DESIRED_EFFECTS  "videoflip, edgetv" // TODO --- maybe

    static gchar * Options_Effects_Ptr = NULL;

    static GOptionEntry Options_Effects_Array[] =
    {
        { "Options_Effects", 'e', 0, G_OPTION_ARG_STRING, &Options_Effects_Ptr, DESIRED_EFFECTS, NULL },
        { NULL }
    };

    mySplicer.status = e_SPLICER_STATE_FAIL;

    // verify existence of the elements to be spliced by a TEE
    if ( (mySplicer.ptr_from_element == NULL) || (mySplicer.ptr_into_element == NULL) )
    {
        return -1;
    }

    // verify existence of the desired pads
    if ( (mySplicer.ptr_from_pad == NULL) || (mySplicer.ptr_into_pad == NULL) )
    {
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

                g_print("Added (%s) to Unlinker-Effects-Queue \n", psz_effect_name);
            }
            else
            {
                g_print("Failed to make Unlinker-Effect (%s) \n", psz_effect_name);
            }
        }
    }

    // start the safe unlinking procedure
    mySplicer.status             = e_SPLICER_STATE_BUSY;
    mySplicer.frame_saver_filter = &mySniffer; 
    mySplicer.ptr_snaps_pipeline = mySniffer.pipeline_ptr;

    // get the first queued effect --- possibly none
    mySplicer.ptr_current_effect = g_queue_pop_head( &mySplicer.effects_queue );

    // apply probe to 'producer' pad --- stop data flow into consumer element
    gst_pad_add_probe(mySplicer.ptr_from_pad, 
                      GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, 
                      do_callback_for_splicer_producer_pad, 
                      &mySplicer, 
                      NULL);

    return 1;   // success --- safe unlinking procedure is now in progress
}


//=======================================================================================
// synopsis: result = do_splicer_configure_TEE_pads()
//
// obtains the internal pads of a TEE --- returns 1 on success, else rrror
//=======================================================================================
static gint do_splicer_configure_TEE_pads()
{
    GstPad  * tee_Q1_pad_ptr, 
            * tee_Q2_pad_ptr,
            * queue1_pad_ptr, 
            * queue2_pad_ptr;

    GstElementClass * class_ptr = GST_ELEMENT_GET_CLASS(mySniffer.tee_element_ptr);

    GstPadTemplate  * pad_template_ptr = gst_element_class_get_pad_template(class_ptr, "src_%u");

    if (! pad_template_ptr)
    {
        g_warning("Unable to get pad template");        
        return 0;
    }

    tee_Q1_pad_ptr = gst_element_request_pad(mySniffer.tee_element_ptr, 
                                             pad_template_ptr, 
                                             NULL, 
                                             NULL);

    // g_print("Got request pad (%s) for tee-branch-Q1 \n", gst_pad_get_name(tee_Q1_pad_ptr));

    queue1_pad_ptr = gst_element_get_static_pad(mySniffer.Q_1_element_ptr, "sink");

    tee_Q2_pad_ptr = gst_element_request_pad(mySniffer.tee_element_ptr, 
                                             pad_template_ptr, 
                                             NULL, 
                                             NULL);

    // g_print("Got request pad (%s) for tee-branch-Q2 \n", gst_pad_get_name(tee_Q2_pad_ptr));

    queue2_pad_ptr = gst_element_get_static_pad(mySniffer.Q_2_element_ptr, "sink");

    if (gst_pad_link(tee_Q1_pad_ptr, queue1_pad_ptr) != GST_PAD_LINK_OK)
    {
        g_warning("Tee for q1 could not be linked.\n");
        return 0;
    }

    if (gst_pad_link(tee_Q2_pad_ptr, queue2_pad_ptr) != GST_PAD_LINK_OK)
    {
        g_warning("Tee for q2 could not be linked.\n");
        return 0;
    }

    gst_object_unref(queue1_pad_ptr);
    gst_object_unref(queue2_pad_ptr);

    return 1;
}


//=======================================================================================
// synopsis: result = do_pipeline_cleanup()
//
// inserts TEE into the pipeline --- returns 0 on always
//=======================================================================================
static gint do_pipeline_cleanup()
{
    if (mySniffer.main_loop_ptr != NULL)
    {
        g_main_loop_quit(mySniffer.main_loop_ptr);

        gst_object_unref(GST_OBJECT(mySniffer.pipeline_ptr));

        g_main_loop_unref(mySniffer.main_loop_ptr);

        mySniffer.pipeline_ptr = NULL;

        mySniffer.main_loop_ptr = NULL;
    }

    if (mySniffer.pipeline_ptr != NULL)
    {
        gst_object_unref(mySniffer.pipeline_ptr);

        mySniffer.pipeline_ptr = NULL;
    }

    return 0;
}


//=======================================================================================
// synopsis: is_ok = do_pipeline_callback_for_bus_messages()
//
// handle pipeline's bus messages --- returns TRUE on success, else rrror
//=======================================================================================
static gboolean do_pipeline_callback_for_bus_messages(GstBus     * aBusPtr,
                                                      GstMessage * aMsgPtr, 
                                                      gpointer     aCtxPtr)
{
    const GstStructure * gst_struct_ptr;

    GError    * ptr_err = NULL;
    gchar     * ptr_dbg = NULL;

    int msg_type = GST_MESSAGE_TYPE(aMsgPtr);

    GMainLoop * ptr_main_looper = aCtxPtr;

    if (mySniffer.main_loop_ptr != ptr_main_looper)
    {
        return FALSE;   // TODO --- impossible
    }

    switch (msg_type)
    {
    case GST_MESSAGE_ERROR:
        gst_message_parse_error(aMsgPtr, &ptr_err, &ptr_dbg);
        g_print("   bus_msg=ERROR --- %s", ptr_err->message);
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
            g_print("   bus_msg=OFF \n");
            g_main_loop_quit(ptr_main_looper);
        }
        break;

    case GST_MESSAGE_EOS:
        g_print("   bus_msg=EOS \n");
        g_main_loop_quit(ptr_main_looper);
        break;

    default:
        break;
    }

    if (msg_type == GST_MESSAGE_STATE_CHANGED)
    {
        GstState old_state, new_state, pending_state;

        gst_message_parse_state_changed(aMsgPtr, &old_state, &new_state, &pending_state);
        
        g_print("   bus_msg=STATE --- old=%i  new=%i  pending=%i. \n", old_state, new_state, pending_state);
    }
    else
    {
        const char * psz_msg_type_name = GST_MESSAGE_TYPE_NAME(aMsgPtr); 

        int msg_time = (int) aMsgPtr->timestamp;

        g_print("   bus_msg=OTHER --- name=(%s) time=%i  type=%i  \n", psz_msg_type_name, msg_time, msg_type);
    }

    return TRUE;
}


//=======================================================================================
// synopsis: result = do_pipeline_insert_TEE_splicer()
//
// inserts TEE into the pipeline --- returns 1 on success, else rrror
//=======================================================================================
static gint do_pipeline_insert_TEE_splicer()
{
    gboolean pipeline_has_TEE = (gst_element_get_parent(mySniffer.tee_element_ptr) != NULL);

    GstState   pipeline_state = GST_STATE_NULL;

    // possibly --- our TEE is already in the pipeline --- nothing more to do
    if ( pipeline_has_TEE )
    {
        return 1;
    }

    gst_element_get_state(mySniffer.pipeline_ptr, &pipeline_state, NULL, 0);

    // dynamic unlinking is needed when the pipeline is PLAYING
    if (pipeline_state != GST_STATE_NULL)
    {
        // possibly --- start new unlinking procedure
        if ((mySplicer.status == e_SPLICER_STATE_NONE) ||
            (mySplicer.status == e_SPLICER_STATE_USED) )
        {
            do_splicer_unlink_two_elements();
        }

        // possibly --- failed to unlink elements
        if (mySplicer.status == e_SPLICER_STATE_FAIL)
        {
            return -1;
        }

        // possibly --- wait for end of unlinking procedure
        if (mySplicer.status == e_SPLICER_STATE_BUSY)
        {
            return 0;
        }

        // suspend the producer and consumer elements
        GstStateChangeReturn rv1 = gst_element_set_state(mySplicer.ptr_from_element, GST_STATE_PAUSED);
        GstStateChangeReturn rv2 = gst_element_set_state(mySplicer.ptr_into_element, GST_STATE_PAUSED);
        GstStateChangeReturn rv3 = gst_element_set_state(mySniffer.pipeline_ptr,     GST_STATE_PAUSED);

        // possibly --- the unlinking procedure failed
        if (mySplicer.status != e_SPLICER_STATE_DONE)
        {
            return ((rv1 != rv2) || (rv1 != rv3)) ? -1 : -1;  // only for DBG
        }
    }

    if (pipeline_state != GST_STATE_NULL)
    {
        if (! gst_pad_unlink(mySplicer.ptr_from_pad, mySplicer.ptr_into_pad))
        {
            g_warning("Unable to unlink pads of connected pipeline elements \n");       
            return -2;
        }
    }

    // now we can safely unlink the pipe where the TEE must be inserted
    gst_element_unlink(mySplicer.ptr_from_element, mySplicer.ptr_into_element);

    // add necessary splicing elements to pipeline
    gst_bin_add_many(GST_BIN(mySniffer.pipeline_ptr),
                     mySniffer.tee_element_ptr,
                     mySniffer.Q_1_element_ptr,
                     mySniffer.Q_2_element_ptr,
                     mySniffer.video_sink2_ptr,
                     NULL);

    if (do_splicer_configure_TEE_pads() <= 0)
    {
        return -3;
    }

    // link elements pads: PRODUCER.OUT pad to TEE.INP pad
    if ( TRUE != gst_element_link_pads(mySplicer.ptr_from_element, 
                                       mySplicer.params.producer_out_pad_name,
                                       mySniffer.tee_element_ptr, 
                                       "sink") )
    {
        g_warning("Unable to link pads: PRODUCER.OUT --> TEE.SINK \n");       
        return -4;
    }

    GstCaps * ptr_producer_caps = gst_pad_query_caps(mySplicer.ptr_from_pad, 
                                                     mySplicer.ptr_all_from_caps);

    if ( ptr_producer_caps != NULL )
    {
        // link elements: T-QUE-1 to CONSUMER (with producer caps)
        if (TRUE != gst_element_link_filtered(mySniffer.Q_1_element_ptr,
                                              mySplicer.ptr_into_element,
                                              ptr_producer_caps)) 
        {
            g_warning("Unable to link elements: T-QUE-2 --> SINK2 (with caps) \n"); 
            return -6;
        }    
    }
    else 
    {
        gst_pad_set_event_function(mySplicer.ptr_into_pad, do_callback_for_consumer_INP_pad_events);

        g_warning("linking pads with negotiated CAPS: T-QUE-1.OUT --> CONSUMER.INP \n"); 

        // link elements pads: T-QUE-1.OUT to CONSUMER.INP
        if ( TRUE != gst_element_link_pads(mySniffer.Q_1_element_ptr, 
                                            "src",
                                            mySplicer.ptr_into_element,
                                            mySplicer.params.consumer_inp_pad_name))
        {
            g_warning("Unable to link pads: T-QUE-1.OUT --> CONSUMER.INP \n"); 
            return -5;
        }
    }

    if ( ptr_producer_caps != NULL )
    {
        // link elements: T-QUE-2 to SINK2 (with producer caps)
        if (TRUE != gst_element_link_filtered(mySniffer.Q_2_element_ptr,
                                              mySniffer.video_sink2_ptr,
                                              ptr_producer_caps)) 
        {
            g_warning("Unable to link elements: T-QUE-2 --> SINK2 (with producer caps) \n"); 
            return -6;
        }
    }
    else
    {
        // link elements: T-QUE-2 to SINK2 (with sinker caps)
        if (TRUE != gst_element_link_many(mySniffer.Q_2_element_ptr,
                                          mySniffer.video_sink2_ptr,
                                          mySniffer.sinker_caps_ptr)) 
        {
            g_warning("Unable to link elements: T-QUE-2 --> SINK2 (with sinker caps) \n"); 
            return -6;
        }
    }

    return 1;
}


//=======================================================================================
// synopsis: is_ok = do_pipeline_validate_splicer_parameters()
//
// runs the pipeline --- return TRUE on success, else error
//=======================================================================================
gboolean do_pipeline_validate_splicer_parameters()
{
    mySplicer.ptr_from_element = gst_bin_get_by_name(GST_BIN(mySniffer.pipeline_ptr), 
                                                     mySplicer.params.producer_name);

    mySplicer.ptr_into_element = gst_bin_get_by_name(GST_BIN(mySniffer.pipeline_ptr), 
                                                     mySplicer.params.consumer_name);

    if ( ! (mySplicer.ptr_from_element && mySplicer.ptr_into_element) )
    {
        g_warning("pipeline missing one or both elements to be spliced by TEE \n");        
        return FALSE;
    }

    // a source pad produces data consumed by a sink pad of the next downstream element
    mySplicer.ptr_from_pad = gst_element_get_static_pad(mySplicer.ptr_from_element, 
                                                        mySplicer.params.producer_out_pad_name);

    mySplicer.ptr_into_pad = gst_element_get_static_pad(mySplicer.ptr_into_element, 
                                                        mySplicer.params.consumer_inp_pad_name);

    // verify existence of the desired pads
    if ( (mySplicer.ptr_from_pad == NULL) || (mySplicer.ptr_into_pad == NULL) )
    {
        g_warning("desired pads missing in the elements to be spliced by TEE \n");        
        return FALSE;
    }

    mySplicer.ptr_next_pad = gst_element_get_static_pad(mySplicer.ptr_into_element, 
                                                        mySplicer.params.consumer_out_pad_name);

    if (mySplicer.ptr_next_pad == NULL)
    {
        mySplicer.ptr_next_pad = mySplicer.ptr_into_pad;    // non-producers OK
    }

    mySplicer.ptr_all_from_caps = gst_pad_get_allowed_caps(mySplicer.ptr_from_pad);

    mySplicer.ptr_all_into_caps = gst_pad_get_allowed_caps(mySplicer.ptr_into_pad);

    return TRUE;
}


//=======================================================================================
// synopsis: result = do_pipeline_create()
//
// creates the pipeline --- return 0 on success, else error
//=======================================================================================
static int do_pipeline_create()
{
    gboolean is_custom = (strchr(mySplicer.params.pipeline_spec, '!') != NULL);

    gst_init(NULL, NULL);

    if ( is_custom )
    {
        GError * error_ptr = NULL;

        mySniffer.pipeline_ptr = gst_parse_launch(mySplicer.params.pipeline_spec, &error_ptr);

        if (error_ptr != NULL)
        {
            g_warning("Failed creating custom pipeline --- (%s) \n", error_ptr->message);
            return 1;
        }

        if (gst_object_set_name(GST_OBJECT(mySniffer.pipeline_ptr), mySplicer.params.pipeline_name) != TRUE)
        {
            g_warning("Failed naming custom pipeline (%s) \n", mySplicer.params.pipeline_name);
            return 2;
        }
    }
    else
    {
        mySniffer.pipeline_ptr = gst_pipeline_new(mySplicer.params.pipeline_name);

        if (! mySniffer.pipeline_ptr)
        {
            g_warning("Failed creating pipeline named (%s) \n", mySplicer.params.pipeline_name);
            return 1;
        }
    }

    if (gst_element_set_state(mySniffer.pipeline_ptr, GST_STATE_NULL) != GST_STATE_CHANGE_SUCCESS)
    {
        g_warning("Failed setting new pipeline state to NULL \n");
        return 3;
    }

    mySniffer.bus_ptr = gst_pipeline_get_bus(GST_PIPELINE(mySniffer.pipeline_ptr));

    if (! mySniffer.bus_ptr)
    {
        g_warning("Failed obtaining the pipeline's bus \n");
        return 4;
    }

    gst_bus_add_watch(mySniffer.bus_ptr, do_pipeline_callback_for_bus_messages, NULL);
    gst_object_unref(mySniffer.bus_ptr);
    mySniffer.bus_ptr = NULL;

    // create the pipeline's elements
    mySniffer.vid_sourcer_ptr = gst_element_factory_make("videotestsrc",  DEFAULT_VID_SOURCE_NAME);
    mySniffer.cvt_element_ptr = gst_element_factory_make("videoconvert",  DEFAULT_VID_CVT_NAME);
    mySniffer.tee_element_ptr = gst_element_factory_make("tee",           DEFAULT_VID_TEE_NAME);
    mySniffer.Q_1_element_ptr = gst_element_factory_make("queue",         DEFAULT_QUEUE_1_NAME);
    mySniffer.Q_2_element_ptr = gst_element_factory_make("queue",         DEFAULT_QUEUE_2_NAME);
    mySniffer.video_sink1_ptr = gst_element_factory_make("autovideosink", DEFAULT_VID_SINK_1_NAME);
    mySniffer.src_caps_filter = gst_element_factory_make("capsfilter",    DEFAULT_SRC_CAPS_FILTER_NAME);

    // possibly --- create the appsink to snap and save frames
    if (mySplicer.params.one_snap_ms > 0)
    {
        mySniffer.source_caps_ptr = gst_caps_from_string("video/x-raw, width=(int)500, height=(int)200");
        mySniffer.sinker_caps_ptr = gst_caps_from_string("video/x-raw, format=(string)RGB, bpp=(int)24");
        mySniffer.video_sink2_ptr = do_appsink_create(DEFAULT_APP_SINK_2_NAME, 3);
    }
    else
    {
        mySniffer.source_caps_ptr = gst_caps_from_string("video/x-raw, width=(int)500, height=(int)200");
        mySniffer.sinker_caps_ptr = gst_caps_from_string("video/x-raw, width=(int)500, height=(int)200");
        mySniffer.video_sink2_ptr = gst_element_factory_make("autovideosink", DEFAULT_VID_SINK_2_NAME);
    }

    int flags = (mySniffer.vid_sourcer_ptr ? 0 : 0x001) +   // only used for default pipeline
                (mySniffer.video_sink1_ptr ? 0 : 0x002) +   // only used for default pipeline 
                (mySniffer.video_sink2_ptr ? 0 : 0x004) +
                (mySniffer.cvt_element_ptr ? 0 : 0x008) +
                (mySniffer.Q_1_element_ptr ? 0 : 0x010) +
                (mySniffer.Q_2_element_ptr ? 0 : 0x020) +
                (mySniffer.tee_element_ptr ? 0 : 0x040) +
                (mySniffer.source_caps_ptr ? 0 : 0x100) +
                (mySniffer.sinker_caps_ptr ? 0 : 0x200) +
                (mySniffer.src_caps_filter ? 0 : 0x400) ;

    if (flags != 0)
    {
        g_warning("Failed creating some pipeline elements (flags=0x%0X) \n", flags);
        return 5;
    }

    g_object_set(G_OBJECT(mySniffer.src_caps_filter), "caps", mySniffer.source_caps_ptr, NULL);

    // possibly --- configure the default pipeline
    if (! is_custom)
    {
        gst_bin_add_many(GST_BIN(mySniffer.pipeline_ptr),
                         mySniffer.vid_sourcer_ptr ,
                         mySniffer.cvt_element_ptr,
                         mySniffer.src_caps_filter,
                         mySniffer.video_sink1_ptr,
                         NULL);

        // link elements: SRC to CVT to CAPS to SINK1
        if (TRUE != gst_element_link_many(mySniffer.vid_sourcer_ptr,
                                          mySniffer.cvt_element_ptr, 
                                          mySniffer.src_caps_filter,
                                          mySniffer.video_sink1_ptr,
                                          NULL)) 
        {
            g_warning("Failed linking elements: SRC --> CVT --> CAPS --> SINK1 \n");
            return 6;
        }

        // gst_pad_create_stream_id(gst_element_get_static_pad(mySniffer.vid_sourcer_ptr, "src"),
        //                          mySniffer.vid_sourcer_ptr, DEFAULT_PIPELINE_NAME);
    }

    if (do_pipeline_validate_splicer_parameters() != TRUE)
    {
        g_print("invalid parameters for splicing the pipeline (TEE insertion) \n");
        return 7;
    }

    // possibly --- insert the TEE now, before pipeline start playing
    if (mySplicer.params.max_spin_ms <= 0)
    {
        if (do_pipeline_insert_TEE_splicer() <= 0)
        {
            return 8;
        }
    }

    return 0;
}


//=======================================================================================
// synopsis: is_ok = do_pipeline_callback_for_idle_time()
//
// callback when the pipeline's main-loop-thread is idle --- return TRUE always
//=======================================================================================
static gboolean do_pipeline_callback_for_idle_time(gpointer aCtxPtr)
{
    GstClockTime  now_nanos = gst_clock_get_time( mySniffer.sys_clock_ptr );

    GstClockTime elapsed_ns = now_nanos - mySniffer.start_play_time_ns;

    uint32_t    playtime_ms = (uint32_t) (elapsed_ns / NANOS_PER_MILLISEC);


    // possibly --- TEE was inserted or is not wanted
    if (mySniffer.spin_state_ends_ns == 0)
    {
        if (elapsed_ns > mySniffer.frame_snap_wait_ns)
        {
            do_appsink_trigger_next_frame_snap(playtime_ms);
        }

        return TRUE;
    }

    // possibly --- try-or-retry to insert TEE into pipeline
    if ( mySniffer.spin_state_ends_ns < now_nanos )
    {
        if ((mySplicer.status == e_SPLICER_STATE_NONE) || (mySplicer.status == e_SPLICER_STATE_USED) )
        {
            g_print("frame_saver_filter --- playtime=%u %s", playtime_ms, "... Starting TEE insertion \n");
        }

        int status = do_pipeline_insert_TEE_splicer();

        if (status > 0)    // success --- the TEE was inserted into pipeline
        {
            mySniffer.spin_state_ends_ns = 0;

            gst_element_set_state( mySniffer.pipeline_ptr,     GST_STATE_PLAYING );
            gst_element_set_state( mySniffer.video_sink2_ptr,  GST_STATE_PLAYING );
            gst_element_set_state( mySplicer.ptr_from_element, GST_STATE_PLAYING );
            gst_element_set_state( mySplicer.ptr_into_element, GST_STATE_PLAYING );

            g_print("frame_saver_filter --- playtime=%u %s", playtime_ms, "... Finished TEE insertion\n");
        }
        else if (status < 0)    // insertion or unlinking failed --- Retry after 10 more ticks
        {
            mySniffer.spin_state_ends_ns = now_nanos + (NANOS_PER_MILLISEC * mySplicer.params.one_tick_ms * 10);

            g_print("frame_saver_filter --- playtime=%u %s", playtime_ms, "... Failed TEE insertion\n");

            gst_element_set_state( mySniffer.pipeline_ptr, GST_STATE_READY );
        }
        else // (status == 0) --- NOTE: here we can show the typical intervals between "idle" callbacks
        {
            // g_print("frame_saver_filter --- playtime=%u %s", playtime_ms, " ... TEE insertion is ONGOING \n");
        }

        return TRUE;
    }

    return TRUE;
}


//=======================================================================================
// synopsis: is_ok = do_pipeline_callback_for_timer_tick()
//
// callback for the pipeline's timer-tick event --- return TRUE always
//=======================================================================================
static gboolean do_pipeline_callback_for_timer_tick(gpointer aCtxPtr)
{
    GstClockTime  now_nanos = gst_clock_get_time( mySniffer.sys_clock_ptr );

    GstClockTime elapsed_ns = now_nanos - mySniffer.start_play_time_ns;

    uint32_t    playtime_ms = (uint32_t) (elapsed_ns / NANOS_PER_MILLISEC);


    if (now_nanos < mySniffer.spin_state_ends_ns)   // idle spinning state
    {
        g_print("frame_saver_filter --- playtime=%u ... idle-spin \n", playtime_ms);
    }
    else if (mySplicer.params.one_snap_ms == 0)     // no doing frame snaps
    {
        g_print("frame_saver_filter --- playtime=%u \n", playtime_ms);
    }

    // possibly --- it's time to shutdown
    if ( playtime_ms > mySplicer.params.max_play_ms )
    {
        g_print("frame_saver_filter --- playtime=%u ... play-ends \n", playtime_ms);

        if (mySniffer.vid_sourcer_ptr != NULL)   // it's the default pipeline
        {
            gst_element_send_event ( mySniffer.vid_sourcer_ptr, gst_event_new_eos() );

            gst_element_set_state( mySniffer.vid_sourcer_ptr, GST_STATE_READY );
        }

        gst_element_set_state( mySniffer.pipeline_ptr, GST_STATE_NULL );

        do_pipeline_cleanup();

        return FALSE;
    }    

    return TRUE;
}


//=======================================================================================
// synopsis: result = do_pipeline_run_main_loop()
//
// runs the pipeline --- return 0 on success, else error
//=======================================================================================
static int do_pipeline_run_main_loop()
{
    GstClockTime some_nanos = (NANOS_PER_MILLISEC * MIN_TICKS_MILLISEC) / 10;
    GstClockTime play_nanos = (NANOS_PER_MILLISEC * mySplicer.params.max_play_ms) + some_nanos;
    GstClockTime spin_nanos = (NANOS_PER_MILLISEC * mySplicer.params.max_spin_ms) + some_nanos;
    GstClockTime tick_nanos = (NANOS_PER_MILLISEC * mySplicer.params.one_tick_ms);
    GstClockTime now_nanos;

    strcpy(mySniffer.work_folder_path, mySplicer.params.folder_path);

    mySniffer.main_loop_ptr = g_main_loop_new(NULL, FALSE);

    mySniffer.sys_clock_ptr = gst_system_clock_obtain();

    now_nanos = gst_clock_get_time(mySniffer.sys_clock_ptr);

    mySniffer.start_play_time_ns = now_nanos;
    mySniffer.spin_state_ends_ns = (mySplicer.params.max_spin_ms < 1) ? 0 : spin_nanos + now_nanos;
    mySniffer.frame_snap_wait_ns = (mySplicer.params.one_snap_ms > 0) ? 0 : play_nanos * tick_nanos;

    mySniffer.num_appsink_frames = 0;
    mySniffer.num_appsink_errors = 0;

    mySniffer.num_saver_errors = 0;
    mySniffer.num_saved_frames = 0;
    mySniffer.num_snap_signals = 0;

    if (gst_element_set_state(mySniffer.pipeline_ptr, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE)
    {
        g_warning("Failed setting pipeline state to PLAYING \n");
        return 1;
    }

    g_print("frame_saver_filter --- PLAYING \n");

    g_timeout_add(mySplicer.params.one_tick_ms, do_pipeline_callback_for_timer_tick, &mySniffer);

    g_idle_add(do_pipeline_callback_for_idle_time, &mySniffer);

    g_main_loop_run(mySniffer.main_loop_ptr);

    g_print("frame_saver_filter --- EXITING \n");

    return 0;
}


//=======================================================================================
// synopsis: result = frame_saver_filter_tester(argc, argv)
//
// performs a self-test of the frame_saver_filter --- returns 0 on success
//=======================================================================================
int frame_saver_filter_tester( int argc, char *argv[] )
{
    int result = 0;

    frame_saver_params_initialize( &mySplicer.params );

    if (frame_saver_params_parse_many(argc, argv, &mySplicer.params) != TRUE)
    {
        g_print("\n");
        result = 1;
    }
    else if (frame_saver_params_report_all(&mySplicer.params, stdout) != TRUE)
    {
        g_print("frame_saver_filter_tester --- failed reporting parameters used \n");
        result = 2;
    }
    else
    {
        result = do_pipeline_create();  // returns 0 on success

        if (result != 0)
        {
            g_print("frame_saver_filter_tester --- error (%d) creating pipeline \n", result);
            do_pipeline_cleanup();
        }
        else
        {
            result = (do_pipeline_run_main_loop() == 0) ? 0 : 3;
            do_pipeline_cleanup();
        }
    }

    return result;   // returns 0 on success
}
