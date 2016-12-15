/* 
 * ======================================================================================
 * File:        frame_saver_filter.c
 * 
 * History:     1. 2016-10-14   JBendor     Created
 *              2. 2016-10-28   JBendor     Updated
 *              3. 2016-10-29   JBendor     Removed parameters code to new file
 *              4. 2016-11-04   JBendor     Support for making custom pipelines
 *              5. 2016-12-08   JBendor     Support the actual Gstreamer plugin
 *              6. 2016-12-14   JBendor     Updated
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

#include "wrapped_natives.h"

#include "frame_saver_filter.h"
#include "frame_saver_params.h"
#include "save_frames_as_png.h"

#include <gst/gst.h>
#include <gst/app/gstappsink.h>


#if (GST_VERSION_MAJOR == 0)
    #error "GST_VERSION_MAJOR must not be 0"
#endif


#ifdef _DEBUG
    #define do_DBG_print(txt)  g_print((txt))
#else
    #define do_DBG_print(txt)  g_print((txt))
#endif


#define CAPS_FOR_AUTO_SOURCE  "video/x-raw, width=(int)500, height=(int)200" //, framerate=(fraction)1/2"
#define CAPS_FOR_VIEW_SINKER  "video/x-raw, width=(int)500, height=(int)200"
#define CAPS_FOR_SNAP_SINKER  "video/x-raw, format=(string)RGB, bpp=(int)24"
#define NUM_APP_SINK_BUFFERS  (2)


//=======================================================================================
// custom types
//=======================================================================================
typedef enum 
{ 
    e_PIPELINE_MAKER_ERROR_NONE = 0, 
    e_PIPELINE_PARSER_HAS_ERROR = 1,
    e_FAILED_MAKE_MINI_PIPELINE = e_PIPELINE_PARSER_HAS_ERROR,
    e_FAILED_MAKE_PIPELINE_NAME = 2,
    e_FAILED_SET_PIPELINE_STATE = 3,
    e_FAILED_FETCH_PIPELINE_BUS = 4,
    e_FAILED_MAKE_WORK_ELEMENTS = 5,
    e_FAILED_LINK_MINI_PIPELINE = 6,
    e_ERROR_PIPELINE_TEE_PARAMS = 7

} PIPELINE_MAKER_ERROR_e;



typedef struct
{
    GMainLoop   * main_loop_ptr;        // Main-Event-Loop --- manages all events

    GstBus      * bus_ptr;              // bus transfers messages from/to pipeline

    GstCaps     * source_caps_ptr,      // for Source-to-Sink1 in default pipeline
                * sinker_caps_ptr;      // for TEE-to-Queue2-to-Sink2 (or appsink)

    GstElement  * pipeline_ptr,
                * vid_sourcer_ptr,
                * video_sink1_ptr,
                * video_sink2_ptr,
                * vid_convert_ptr,
                * cvt_element_ptr,
                * tee_element_ptr,
                * Q_1_element_ptr,
                * Q_2_element_ptr,
                * real_plugin_ptr;

    GstClock    * sys_clock_ptr;

    GstClockTime start_play_time_ns;    // timestamp of pipeline startup
    GstClockTime wait_state_ends_ns;    // timestamp for a TEE insertion
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
    e_TEE_INSERT_ENDED = 0, 
    e_TEE_INSERT_WAITS = 1, 
    e_TEE_INSERT_ERROR = 2,
    e_FAIL_UNLINK_PADS = 3,
    e_FAIL_TEE_IN_PADS = 4,
    e_FAIL_TEE_UP_LINK = 5,
    e_FAIL_TEE_Q1_LINK = 6,
    e_FAIL_TEE_Q2_LINK = 7,
    e_FAIL_TEE_TO_FILE = 8

} e_TEE_INSERT_STATE_e;


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
// synopsis: result = do_snap_and_save_frame(aAppSinkPtr, aBufferPtr, aCapsPtr, aStatePtr)
//
// snaps and saves a frame --- returns GST_FLOW_OK on success, else GST_FLOW_ERROR
//=======================================================================================
static gint do_snap_and_save_frame(GstAppSink    * aAppSinkPtr_unused,
                                   GstBuffer     * aBufferPtr,
                                   GstCaps       * aCapsPtr,
                                   FlowSniffer_t * aStatePtr)
{
    /* 
    * NOTE-1: frame height can depend on the pixel-aspect-ratio of the source.
    *
    * NOTE-2: stride of video buffers is rounded to the nearest multiple of 4.
    */

    GstMapInfo map;

    char sz_image_format[100],
         sz_image_path[PATH_MAX + 100];

    const char * psz_caps = gst_caps_to_string(aCapsPtr);

    const char * interlace = strstr(psz_caps, "interlace-mode=");

    int     format_is_I420 = (strstr(psz_caps, "I420") != NULL);

    int  cols = 0, 
         rows = 0, 
         bits = 8,
         errs = pipeline_params_parse_caps(psz_caps,
                                           sz_image_format, 
                                           &cols, 
                                           &rows, 
                                           &bits);

    if ( (errs != 0) || (rows < 1) || (cols < 1) )
    {
        aStatePtr->num_saver_errors += 1;
        return GST_FLOW_ERROR;  // invalid attributes
    }

    if ( (interlace != NULL) && (strstr(interlace, "progressive") == NULL) )
    {
        aStatePtr->num_saver_errors += 1;
        return GST_FLOW_ERROR;  // only "progressive" is allowed
    }

    if (TRUE != gst_buffer_map(aBufferPtr, &map, GST_MAP_READ))
    {
        aStatePtr->num_saver_errors += 1;
        return GST_FLOW_ERROR;
    }

    int data_lng = (int) map.size;          // total number of frame's bytes
    int num_pixs = rows * cols;
    int pix_size = data_lng / num_pixs;
    int   stride = GST_ROUND_UP_4(cols * pix_size);         // bytes per row

    GstClockTime now = gst_clock_get_time (aStatePtr->sys_clock_ptr);

    guint elapsed_ms = (guint) ((now - mySniffer.start_play_time_ns) / NANOS_PER_MILLISEC);

    aStatePtr->num_saved_frames += 1;
    
    if ( strncmp(sz_image_format, "BGR", 3) == 0 )
    {
        convert_BGR_frame_to_RGB(map.data, pix_size * 8, stride, cols, rows);
        sz_image_format[0] = 'R';
        sz_image_format[2] = 'B';
    }

    sprintf(sz_image_path, 
            "%s%c%s_%dx%dx%d.@%04u_%03u.#%u.png",             // "f:/telmate/junk/"
            aStatePtr->work_folder_path, PATH_DELIMITER,
            (format_is_I420 ? "I420_RGB" : sz_image_format), 
            cols, 
            rows,
            (format_is_I420 ? 24 : bits),
            elapsed_ms / 1000,
            elapsed_ms % 1000,
            aStatePtr->num_saved_frames);

    errs = save_frame_as_PNG(sz_image_path, sz_image_format, map.data, data_lng, stride, cols, rows);

    gst_buffer_unmap (aBufferPtr, &map);

    if (errs != 0)
    {
        aStatePtr->num_saver_errors += 1;
    }

    return (errs == 0) ? GST_FLOW_OK : GST_FLOW_OK;
}


//=======================================================================================
// synopsis: result = do_appsink_callback_for_new_frame(aAppSinkPtr, aContextPtr)
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
        guint total_done = ptr_state->num_saver_errors + ptr_state->num_saved_frames;

        if (ptr_state->num_snap_signals > total_done)
        {
            flow_result = do_snap_and_save_frame(aAppSinkPtr, buffer_ptr, caps_ptr, ptr_state);
        }

        gst_sample_unref(sample_ptr);
    }

    return flow_result;
}


//=======================================================================================
// synopsis: result = do_appsink_callback_probe_inp_pad_BUF(aPadPtr, aInfoPtr, aCtxPtr)
//
// appsink callback for input-pad BUFFER events
//=======================================================================================
static GstPadProbeReturn do_appsink_callback_probe_inp_pad_BUF(GstPad          * aPadPtr,
                                                               GstPadProbeInfo * aInfoPtr,
                                                               gpointer          aCtxPtr)
{
    GstBuffer   * buffer_ptr = GST_PAD_PROBE_INFO_BUFFER(aInfoPtr);
    GstEvent    *  event_ptr = GST_PAD_PROBE_INFO_DATA(aInfoPtr);
    GstEventType  event_type = GST_EVENT_TYPE(event_ptr);

    if ( (buffer_ptr == NULL) || (event_type == GST_EVENT_UNKNOWN) )
    {
        return GST_PAD_PROBE_OK;
    }

#ifdef _USE_APPSINK_CALLBACKS_
    if ( mySniffer.real_plugin_ptr == NULL )
    {
        // GST_DEBUG_OBJECT(aPadPtr, "appsink-inp-pad --- BUF event !!! ");
        return GST_PAD_PROBE_OK;
    }
#endif

    mySniffer.num_appsink_frames += 1;

    // note: "buffer" here --- "sample" in do_appsink_callback_for_new_frame()
    if ( (mySplicer.params.one_snap_ms > 0) &&
         (mySniffer.num_snap_signals > mySniffer.num_saved_frames) )
    {
            do_snap_and_save_frame( (GstAppSink*) mySniffer.video_sink2_ptr,
                                    buffer_ptr,
                                    gst_pad_get_current_caps(aPadPtr),
                                    &mySniffer );
    }

    return GST_PAD_PROBE_OK;
}


//=======================================================================================
// synopsis: result = do_appsink_callback_probe_inp_pad_EOS(aPadPtr, aInfoPtr, aCtxPtr)
//
// appsink callback for input-pad events --- se for EOS
//=======================================================================================
static GstPadProbeReturn do_appsink_callback_probe_inp_pad_EOS(GstPad          * aPadPtr,
                                                               GstPadProbeInfo * aInfoPtr,
                                                               gpointer          aCtxPtr)
{
    GstEvent* event_data_ptr = GST_PAD_PROBE_INFO_DATA(aInfoPtr);

    GstEventType  event_type = GST_EVENT_TYPE(event_data_ptr);

    if (event_type != GST_EVENT_EOS)
    {
        return GST_PAD_PROBE_PASS;
    }

    GST_DEBUG_OBJECT(aPadPtr, "appsink-inp-pad --- EOS event !!! ");

    return GST_PAD_PROBE_OK;
}

#include "glib.h"
#define MK_GLIB_RW_DIR(path)    (g_mkdir_with_parents((path), 0666))

//=======================================================================================
// synopsis: is_ok = do_appsink_trigger_next_frame_snap(elapsedPlaytimeMillis)
//
// triggers frame snaps --- returns TRUE iff more snaps are allowed
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
                "%c%s%04d%02d%02d_%02d%02d%02d",
                PATH_DELIMITER,
                "IMAGES_",
                tm_ptr->tm_year + 1900,
                tm_ptr->tm_mon + 1,
                tm_ptr->tm_mday,
                tm_ptr->tm_hour,
                tm_ptr->tm_min,
                tm_ptr->tm_sec);

        int error = MK_GLIB_RW_DIR(mySniffer.work_folder_path);
        if (error == 0)
        {
            g_print("frame_saver_filter --- playtime=%u %s (%s) \n", 
                    elapsedPlaytimeMillis,
                    "... New Folder",
                    &mySniffer.work_folder_path[length+1]);
        }
        else
        {
            g_print("frame_saver_filter --- playtime=%u %s (%s) NOT created --- error=(%d) \n", 
                    elapsedPlaytimeMillis,
                    "... New Folder",
                    &mySniffer.work_folder_path[0], error);

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

    gboolean is_more_snaps_ok = TRUE;

    if ((mySplicer.params.max_num_snaps_saved > 0) &&
        (mySplicer.params.max_num_snaps_saved <= mySniffer.num_saved_frames))
    {
        g_print("frame_saver_filter --- Reached-Limit ... %s=(%u) \n", 
                "#Saved",
                mySplicer.params.max_num_snaps_saved);

        is_more_snaps_ok = FALSE;
    }

    if ((mySplicer.params.max_num_failed_snap) &&
        (mySplicer.params.max_num_failed_snap <= mySniffer.num_saver_errors))
    {
        g_print("frame_saver_filter --- Reached-Limit ... %s=(%u) \n", 
                "#Fails",
                mySplicer.params.max_num_failed_snap);

        is_more_snaps_ok = FALSE;
    }

    return is_more_snaps_ok && (mySniffer.real_plugin_ptr != NULL);
}


//=======================================================================================
// synopsis: appsink_ptr = do_appsink_create(aNamePtr, aNumBuffers)
//
// creates & configures the appsink for frame snaps --- returns element pointer
//=======================================================================================
static GstElement* do_appsink_create(const char * aNamePtr, int aNumBuffers)
{
    do_DBG_print("@@  do_appsink_create \n");

    GstElement * appsink_ptr = gst_element_factory_make("appsink", aNamePtr);

    mySniffer.appsink_callbacks.eos         = NULL;
    mySniffer.appsink_callbacks.new_preroll = NULL;
    mySniffer.appsink_callbacks.new_sample  = do_appsink_callback_for_new_frame;

#ifdef _USE_APPSINK_CALLBACKS_

    gst_app_sink_set_callbacks(GST_APP_SINK(appsink_ptr),
                               &mySniffer.appsink_callbacks,
                               &mySniffer, // pointer_to_context_of_the_callback
                               NULL);      // pointer_to_destroy_notify_callback

    gst_app_sink_set_drop(GST_APP_SINK(appsink_ptr), TRUE);

    gst_app_sink_set_emit_signals(GST_APP_SINK(appsink_ptr), TRUE);

    gst_app_sink_set_max_buffers(GST_APP_SINK(appsink_ptr), aNumBuffers);

#endif // _USE_APPSINK_CALLBACKS_

    GstPad *  ptr_inp_pad = gst_element_get_static_pad(GST_ELEMENT(appsink_ptr), "sink");

    GstPadProbeType flags = GST_PAD_PROBE_TYPE_BLOCK | GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM;

    // install consumer-inp-pad-probe for EOS events
    gulong probe_EOS = gst_pad_add_probe( ptr_inp_pad,
                                          flags,
                                          do_appsink_callback_probe_inp_pad_EOS,
                                          &mySniffer,
                                          NULL );

    // install consumer-inp-pad-probe for BUFFER event
    gulong probe_BUF = gst_pad_add_probe( ptr_inp_pad,
                                          GST_PAD_PROBE_TYPE_BUFFER,
                                          do_appsink_callback_probe_inp_pad_BUF,
                                          &mySniffer,
                                          NULL );

    return ( (probe_EOS && probe_BUF) ? appsink_ptr : appsink_ptr );
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

    GstEvent* event_data_ptr = GST_PAD_PROBE_INFO_DATA(aInfoPtr);

    GstEventType  event_type = GST_EVENT_TYPE(event_data_ptr);
     
    if (event_type != GST_EVENT_EOS)
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
        // gst_pad_send_event( ptr_out_pad, gst_event_new_eos() );
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
// synopsis: is_ok = do_splicer_unlink_two_elements()
//
// safely unlinks two inter-linked elements --- returns TRUE on success, else rrror
//=======================================================================================
static gboolean do_splicer_unlink_two_elements()
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
        return FALSE;
    }

    // verify existence of the desired pads
    if ( (mySplicer.ptr_from_pad == NULL) || (mySplicer.ptr_into_pad == NULL) )
    {
        return FALSE;
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
        int index = -1;
        while ( mySplicer.pp_effects_names[++index] != NULL )
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

    return TRUE;   // success --- safe unlinking procedure is now in progress
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
        return FALSE;   // impossible
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
// inserts TEE into the pipeline --- returns 0 on done, 1 on busy, else error
//=======================================================================================
static e_TEE_INSERT_STATE_e  do_pipeline_insert_TEE_splicer()
{
    GstState pipeline_state = GST_STATE_NULL;

    gboolean is_TEE_in_pipe = (gst_element_get_parent(mySniffer.tee_element_ptr) != NULL);

    gboolean   is_linked_ok = FALSE;

    // possibly --- our TEE is already in the pipeline --- nothing more to do
    if (is_TEE_in_pipe)
    {
        return e_TEE_INSERT_ENDED;
    }

    gst_element_get_state(mySniffer.pipeline_ptr, &pipeline_state, NULL, 0);

    // dynamic unlinking is needed when the pipeline is PLAYING
    if (pipeline_state != GST_STATE_NULL)
    {
        // possibly --- start new unlinking procedure
        if ((mySplicer.status == e_SPLICER_STATE_NONE) ||
            (mySplicer.status == e_SPLICER_STATE_USED) )
        {
            do_splicer_unlink_two_elements();   // affects "mySplicer.status"
        }

        // possibly --- failed to unlink elements
        if (mySplicer.status == e_SPLICER_STATE_FAIL)
        {
            return e_TEE_INSERT_ERROR;
        }

        // possibly --- wait for end of unlinking procedure
        if (mySplicer.status == e_SPLICER_STATE_BUSY)
        {
            return e_TEE_INSERT_WAITS;
        }

        // suspend the producer and consumer elements
        GstStateChangeReturn rv1 = gst_element_set_state(mySplicer.ptr_from_element, GST_STATE_PAUSED);
        GstStateChangeReturn rv2 = gst_element_set_state(mySplicer.ptr_into_element, GST_STATE_PAUSED);
        GstStateChangeReturn rv3 = gst_element_set_state(mySniffer.pipeline_ptr,     GST_STATE_PAUSED);

        // possibly --- the unlinking procedure failed
        if (mySplicer.status != e_SPLICER_STATE_DONE)
        {
            return e_TEE_INSERT_ERROR + ((rv1 && rv2 && rv3) ? 0 : 0);  // rv's only for Debug
        }
    }

    if (pipeline_state != GST_STATE_NULL)
    {
        if (! gst_pad_unlink(mySplicer.ptr_from_pad, mySplicer.ptr_into_pad))
        {
            g_warning("Unable to unlink pads of connected pipeline elements \n");       
            return e_FAIL_UNLINK_PADS;
        }
    }

    // now we can safely unlink the pipe where the TEE must be inserted
    gst_element_unlink(mySplicer.ptr_from_element, mySplicer.ptr_into_element);

    // add necessary splicing elements to pipeline
    gst_bin_add_many(GST_BIN(mySniffer.pipeline_ptr),
                     mySniffer.tee_element_ptr,
                     mySniffer.Q_1_element_ptr,
                     mySniffer.Q_2_element_ptr,
                     mySniffer.cvt_element_ptr,
                     mySniffer.video_sink2_ptr,
                     NULL);

    if (do_splicer_configure_TEE_pads() != 1)
    {
        return e_FAIL_TEE_IN_PADS;
    }

    // link elements pads: PRODUCER.OUT pad to TEE.INP pad
    if ( TRUE != gst_element_link_pads(mySplicer.ptr_from_element, 
                                       mySplicer.params.producer_out_pad_name,
                                       mySniffer.tee_element_ptr, 
                                       "sink") )
    {
        g_warning("Unable to link pads: PRODUCER.OUT --> TEE.SINK \n");       
        return e_FAIL_TEE_UP_LINK;
    }

    // get the producer's caps
    GstCaps * ptr_linker_caps = gst_pad_query_caps(mySplicer.ptr_from_pad, 
                                                   mySplicer.ptr_all_from_caps);

    // link elements: T-QUE-1 to CONSUMER
    if ( ptr_linker_caps != NULL )
    {
        is_linked_ok = gst_element_link_filtered(mySniffer.Q_1_element_ptr,
                                                 mySplicer.ptr_into_element,
                                                 ptr_linker_caps); 
        if (! is_linked_ok)
        {
            g_warning("Unable to link elements: T-QUE-1 --> CONSUMER (with producer caps) \n"); 
            return e_FAIL_TEE_Q1_LINK;
        }    
    }
    else    // link T-QUE-1 using negotiated caps when producer caps is null
    {
        gst_pad_set_event_function(mySplicer.ptr_into_pad, do_callback_for_consumer_INP_pad_events);

        g_warning("linking pads with negotiated CAPS: T-QUE-1.OUT --> CONSUMER.INP \n"); 

        is_linked_ok = gst_element_link_pads(mySniffer.Q_1_element_ptr, 
                                             "src",
                                             mySplicer.ptr_into_element,
                                             mySplicer.params.consumer_inp_pad_name);
        if (! is_linked_ok)
        {
            g_warning("Unable to link pads: T-QUE-1.OUT --> CONSUMER.INP \n"); 
            return e_FAIL_TEE_Q1_LINK;
        }
    }

#ifndef _NOT_USING_SINKER_CAPS_
    ptr_linker_caps = mySniffer.sinker_caps_ptr;
#endif

    if (ptr_linker_caps == NULL)
    {
        ptr_linker_caps = mySniffer.sinker_caps_ptr;
    }

    // link elements: T-QUE-2 to SINK2 
    is_linked_ok = gst_element_link_many(mySniffer.Q_2_element_ptr, 
                                         mySniffer.cvt_element_ptr,
                                         NULL);

    is_linked_ok &= gst_element_link_filtered(mySniffer.cvt_element_ptr,
                                              mySniffer.video_sink2_ptr, 
                                              ptr_linker_caps);

    if (! is_linked_ok)
    {
        g_warning("Unable to link elements: T-QUE-2 --> SINK2 \n"); 
        return e_FAIL_TEE_Q2_LINK;
    }

    return e_TEE_INSERT_ENDED;
}


//=======================================================================================
// synopsis: is_ok = do_pipeline_validate_splicer_parameters()
//
// runs the pipeline --- return TRUE on success, else error
//=======================================================================================
gboolean do_pipeline_validate_splicer_parameters()
{
    do_DBG_print("@@  do_pipeline_validate_splicer_parameters \n");

    mySplicer.ptr_from_element = gst_bin_get_by_name(GST_BIN(mySniffer.pipeline_ptr), 
                                                     mySplicer.params.producer_name);

    if ( mySplicer.ptr_from_element == NULL )
    {
        g_warning("pipeline missing PRODUCER element (%s) \n", mySplicer.params.producer_name);
        return FALSE;
    }

    mySplicer.ptr_into_element = gst_bin_get_by_name(GST_BIN(mySniffer.pipeline_ptr), 
                                                     mySplicer.params.consumer_name);

    if ( mySplicer.ptr_into_element == NULL )
    {
        g_warning("pipeline missing CONSUMER element (%s) \n", mySplicer.params.consumer_name);
        return FALSE;
    }

    // a source pad produces data consumed by a sink pad of the next downstream element
    mySplicer.ptr_from_pad = gst_element_get_static_pad(mySplicer.ptr_from_element, 
                                                        mySplicer.params.producer_out_pad_name);

    if ( mySplicer.ptr_from_pad == NULL )
    {
        g_warning("pipeline missing PRODUCER output pad (%s) \n", mySplicer.params.producer_out_pad_name);
        return FALSE;
    }

    mySplicer.ptr_into_pad = gst_element_get_static_pad(mySplicer.ptr_into_element, 
                                                        mySplicer.params.consumer_inp_pad_name);

    if ( mySplicer.ptr_into_pad == NULL )
    {
        g_warning("pipeline missing CONSUMER input pad (%s) \n", mySplicer.params.consumer_inp_pad_name);
        return FALSE;
    }

    mySplicer.ptr_all_from_caps = gst_pad_get_allowed_caps(mySplicer.ptr_from_pad);

    mySplicer.ptr_all_into_caps = gst_pad_get_allowed_caps(mySplicer.ptr_into_pad);

    return TRUE;
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
    if ( (mySniffer.wait_state_ends_ns == 0) || (mySniffer.tee_element_ptr == NULL) )
    {
        if (elapsed_ns > mySniffer.frame_snap_wait_ns)
        {
            gboolean more_ok = do_appsink_trigger_next_frame_snap(playtime_ms);

            // possibly --- disable snaps --- effectively "infinit" wait time
            if (! more_ok)
            {
                mySniffer.num_snap_signals = mySniffer.num_saved_frames;

                mySniffer.frame_snap_wait_ns += NANOS_PER_DAY;
            }
        }

        return TRUE;
    }

    // possibly --- try-or-retry to insert TEE into pipeline
    if ( mySniffer.wait_state_ends_ns < now_nanos )
    {
        if ((mySplicer.status == e_SPLICER_STATE_NONE) || (mySplicer.status == e_SPLICER_STATE_USED) )
        {
            g_print("frame_saver_filter --- playtime=%u %s", playtime_ms, "... Starting TEE insertion \n");
        }

        e_TEE_INSERT_STATE_e  status = do_pipeline_insert_TEE_splicer();

        if (status == e_TEE_INSERT_ENDED)    // TEE inserted into pipeline
        {
            mySniffer.wait_state_ends_ns = 0;

            gst_element_set_state( mySniffer.pipeline_ptr,     GST_STATE_PLAYING );
            gst_element_set_state( mySniffer.video_sink2_ptr,  GST_STATE_PLAYING );
            gst_element_set_state( mySplicer.ptr_from_element, GST_STATE_PLAYING );
            gst_element_set_state( mySplicer.ptr_into_element, GST_STATE_PLAYING );

            g_print("frame_saver_filter --- playtime=%u %s", playtime_ms, "... Finished TEE insertion\n");
        }
        else if (status != e_TEE_INSERT_WAITS)    // failed --- Retry later
        {
            mySniffer.wait_state_ends_ns = now_nanos + (NANOS_PER_MILLISEC * mySplicer.params.one_tick_ms * 10);

            g_print("frame_saver_filter --- playtime=%u %s", playtime_ms, "... Failed TEE insertion\n");

            gst_element_set_state( mySniffer.pipeline_ptr, GST_STATE_READY );
        }
        else // NOTE: here we could report the typical intervals between "idle" callbacks
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


    if (now_nanos < mySniffer.wait_state_ends_ns)   // idle waiting state
    {
        g_print("frame_saver_filter --- playtime=%u ... idle-wait \n", playtime_ms);
    }
    else if (mySplicer.params.one_snap_ms == 0)     // not doing frame snaps
    {
        g_print("frame_saver_filter --- playtime=%u \n", playtime_ms);
    }

    // possibly --- it's time to shutdown the pipeline being tested
    if ( playtime_ms > mySplicer.params.max_play_ms )
    {
        g_print("frame_saver_filter --- playtime=%u ... play-ends \n", playtime_ms);

        if (gst_element_get_parent(mySniffer.vid_sourcer_ptr) != NULL)  // default pipeline
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
// synopsis: result = do_pipeline_create_splicer_elements()
//
// creates the pipeline's spicer elements --- return 0 on success, else error
//=======================================================================================
static PIPELINE_MAKER_ERROR_e do_pipeline_create_splicer_elements()
{
    do_DBG_print("@@  do_pipeline_create_splicer_elements \n");

    if (do_pipeline_validate_splicer_parameters() != TRUE)
    {
        return e_ERROR_PIPELINE_TEE_PARAMS;
    }

    // create the splicer's elements
    mySniffer.tee_element_ptr = gst_element_factory_make("tee",          "FSL_TEE");
    mySniffer.Q_1_element_ptr = gst_element_factory_make("queue",        "FSL_T_QUEUE_1");
    mySniffer.Q_2_element_ptr = gst_element_factory_make("queue",        "FSL_T_QUEUE_2");
    mySniffer.cvt_element_ptr = gst_element_factory_make("videoconvert", "FSL_T_VID_CVT");

    // possibly --- create the appsink to snap and save frames
    if (mySplicer.params.one_snap_ms > 0)
    {
        mySniffer.sinker_caps_ptr = gst_caps_from_string(CAPS_FOR_SNAP_SINKER);
        mySniffer.video_sink2_ptr = do_appsink_create("FSL_T_APP_SINK", NUM_APP_SINK_BUFFERS);
    }
    else
    {
        mySniffer.sinker_caps_ptr = gst_caps_from_string(CAPS_FOR_VIEW_SINKER);
        mySniffer.video_sink2_ptr = gst_element_factory_make("autovideosink", "FSL_T_VID_SINK");
    }

    int flags = (mySniffer.tee_element_ptr ? 0 : 0x0010) +
                (mySniffer.Q_1_element_ptr ? 0 : 0x0020) +
                (mySniffer.Q_2_element_ptr ? 0 : 0x0040) +
                (mySniffer.cvt_element_ptr ? 0 : 0x0100) +
                (mySniffer.video_sink2_ptr ? 0 : 0x0200) +
                (mySniffer.sinker_caps_ptr ? 0 : 0x0400) ;

    if (flags != 0)
    {
        g_warning("Failed creating some elements for splicing (flags=0x%0X) \n", flags);
        return e_FAILED_MAKE_WORK_ELEMENTS;
    }

    return e_PIPELINE_MAKER_ERROR_NONE;
}


//=======================================================================================
// synopsis: result = do_pipeline_create_default_elements()
//
// creates elements for default pipeline --- return 0 on success, else error
//=======================================================================================
static PIPELINE_MAKER_ERROR_e do_pipeline_create_default_elements()
{
    do_DBG_print("@@  do_pipeline_create_default_elements \n");

    // create the elements of the default pipeline
    mySniffer.source_caps_ptr = gst_caps_from_string(CAPS_FOR_AUTO_SOURCE);
    mySniffer.vid_sourcer_ptr = gst_element_factory_make("videotestsrc",  DEFAULT_VID_SRC_NAME);
    mySniffer.vid_convert_ptr = gst_element_factory_make("videoconvert",  DEFAULT_VID_CVT_NAME);
    mySniffer.video_sink1_ptr = gst_element_factory_make("autovideosink", "auto_video_sink_0");

    int flags = (mySniffer.vid_sourcer_ptr ? 0 : 0x001) +   // only used for default pipeline
                (mySniffer.video_sink1_ptr ? 0 : 0x002) +   // only used for default pipeline
                (mySniffer.vid_convert_ptr ? 0 : 0x004) +
                (mySniffer.source_caps_ptr ? 0 : 0x008) ;

    if (flags != 0)
    {
        g_warning("Failed creating basic pipeline elements (flags=0x%0X) \n", flags);
        return e_FAILED_MAKE_WORK_ELEMENTS;
    }

    g_object_set(G_OBJECT(mySniffer.vid_sourcer_ptr), "is-live", TRUE, NULL);

    gst_bin_add_many(GST_BIN(mySniffer.pipeline_ptr),
                     mySniffer.vid_sourcer_ptr ,
                     mySniffer.vid_convert_ptr,
                     mySniffer.video_sink1_ptr,
                     NULL);

    // link elements: SRC to CVT
    gboolean is_link_1_ok = gst_element_link_many(mySniffer.vid_sourcer_ptr,
                                                  mySniffer.vid_convert_ptr,
                                                  NULL);

    // link elements: CVT with CAPS to SINK1
    gboolean is_link_2_ok = gst_element_link_filtered(mySniffer.vid_convert_ptr,
                                                      mySniffer.video_sink1_ptr,
                                                      mySniffer.source_caps_ptr);

    if ( (! is_link_1_ok) || (! is_link_2_ok) )
    {
        g_warning("Failed linking basic elements: SRC --> CVT --> CAPS --> SINK1 \n");
        return e_FAILED_LINK_MINI_PIPELINE;
    }

    return e_PIPELINE_MAKER_ERROR_NONE;
}


//=======================================================================================
// synopsis: result = do_pipeline_create_instance()
//
// creates the pipeline --- return 0 on success, else error
//=======================================================================================
static PIPELINE_MAKER_ERROR_e do_pipeline_create_instance()
{
    do_DBG_print("@@  do_pipeline_create_instance \n");

    PIPELINE_MAKER_ERROR_e e_result = e_PIPELINE_MAKER_ERROR_NONE;

    gboolean is_custom = (strchr(mySplicer.params.pipeline_spec, '!') != NULL);

    if ( is_custom )
    {
        GError * error_ptr = NULL;

        mySniffer.pipeline_ptr = gst_parse_launch(mySplicer.params.pipeline_spec, &error_ptr);

        if (error_ptr != NULL)
        {
            g_warning("Failed creating custom pipeline --- (%s) \n", error_ptr->message);
            return e_PIPELINE_PARSER_HAS_ERROR;
        }

        if (gst_object_set_name(GST_OBJECT(mySniffer.pipeline_ptr), mySplicer.params.pipeline_name) != TRUE)
        {
            g_warning("Failed naming custom pipeline (%s) \n", mySplicer.params.pipeline_name);
            return e_FAILED_MAKE_PIPELINE_NAME;
        }
    }
    else
    {
        mySniffer.pipeline_ptr = gst_pipeline_new(mySplicer.params.pipeline_name);

        if (! mySniffer.pipeline_ptr)
        {
            g_warning("Failed creating pipeline named (%s) \n", mySplicer.params.pipeline_name);
            return e_FAILED_MAKE_MINI_PIPELINE;
        }
    }

    if (gst_element_set_state(mySniffer.pipeline_ptr, GST_STATE_NULL) != GST_STATE_CHANGE_SUCCESS)
    {
        g_warning("Failed setting new pipeline state to NULL \n");
        return e_FAILED_SET_PIPELINE_STATE;
    }

    mySniffer.bus_ptr = gst_pipeline_get_bus(GST_PIPELINE(mySniffer.pipeline_ptr));

    if (! mySniffer.bus_ptr)
    {
        g_warning("Failed obtaining the pipeline's bus \n");
        return e_FAILED_FETCH_PIPELINE_BUS;
    }

    gst_bus_add_watch(mySniffer.bus_ptr, do_pipeline_callback_for_bus_messages, NULL);
    gst_object_unref(mySniffer.bus_ptr);
    mySniffer.bus_ptr = NULL;

    // possibly --- configure the default pipeline
    if (! is_custom)
    {
        e_result = do_pipeline_create_default_elements();
    }

    if (e_result == e_PIPELINE_MAKER_ERROR_NONE)
    {
        e_result = do_pipeline_create_splicer_elements();
    }

    return e_result;
}



//=======================================================================================
// synopsis: is_ok = do_prepare_to_play( canSplicePipeline )
//
// prepares to run the main loop the pipeline --- return TRUE on success, else error
//=======================================================================================
static gboolean do_prepare_to_play( gboolean canSplicePipeline )
{
    do_DBG_print("@@  do_prepare_to_play \n");

    GstClockTime some_nanos = (NANOS_PER_MILLISEC * MIN_TICKS_MILLISEC) / 10;
    GstClockTime play_nanos = (NANOS_PER_MILLISEC * mySplicer.params.max_play_ms) + some_nanos;
    GstClockTime wait_nanos = (NANOS_PER_MILLISEC * mySplicer.params.max_wait_ms) + some_nanos;
    GstClockTime now_nanos;

    strcpy(mySniffer.work_folder_path, mySplicer.params.folder_path);

    mySniffer.sys_clock_ptr = gst_system_clock_obtain();

    now_nanos = gst_clock_get_time(mySniffer.sys_clock_ptr);

    mySniffer.start_play_time_ns = now_nanos;
    mySniffer.wait_state_ends_ns = (mySplicer.params.max_wait_ms < 1) ? 0 : wait_nanos + now_nanos;
    mySniffer.frame_snap_wait_ns = (mySplicer.params.one_snap_ms > 0) ? 0 : play_nanos + NANOS_PER_DAY;

    mySniffer.num_appsink_frames = 0;
    mySniffer.num_appsink_errors = 0;

    mySniffer.num_saver_errors = 0;
    mySniffer.num_saved_frames = 0;
    mySniffer.num_snap_signals = 0;

    if ( canSplicePipeline )
    {
        if (mySniffer.tee_element_ptr == NULL) 
        {
            do_pipeline_create_splicer_elements();
        }
    }

    g_idle_add( do_pipeline_callback_for_idle_time, &mySniffer );

    return TRUE;
}


//=======================================================================================
// synopsis: is_ok = do_pipeline_test_run_main_loop()
//
// runs the main-loop and plays the pipeline --- return TRUE on success, else error
//=======================================================================================
static gboolean do_pipeline_test_run_main_loop()
{
    if (mySniffer.sys_clock_ptr == NULL)
    {
        do_prepare_to_play( TRUE );

        // possibly --- insert the TEE now, before pipeline starts playing
        if (mySplicer.params.max_wait_ms <= 0)
        {
            if (do_pipeline_insert_TEE_splicer() != e_TEE_INSERT_ENDED)
            {
                return FALSE;
            }
        }
    }

    do_DBG_print("@@  do_pipeline_test_run_main_loop \n");

    mySniffer.main_loop_ptr = g_main_loop_new(NULL, FALSE);

    if (gst_element_set_state(mySniffer.pipeline_ptr, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE)
    {
        g_warning("Failed setting pipeline state to PLAYING \n");
        return FALSE;
    }

    g_print("frame_saver_filter --- PLAYING \n");

    g_timeout_add(mySplicer.params.one_tick_ms, do_pipeline_callback_for_timer_tick, &mySniffer);

    g_main_loop_run(mySniffer.main_loop_ptr);

    g_print("frame_saver_filter --- EXITING \n");

    return TRUE;
}


static void *  myMutexHandlePtr = NULL;
static void *  myKnownPlugins[100];
static int     myPluginsCount = -1;

#define MAX_NUM_KNOWN_PLUGINS  ( sizeof(myKnownPlugins) / sizeof(gpointer) )
#define LOCK_MUTEX_TIMEOUT_MS  (10)


//=======================================================================================
// synopsis: result = Frame_Saver_Filter_Lookup(aPluginPtr)
//
// returns index in array of known plugins --- returns -1 iff not in the array
//=======================================================================================
int Frame_Saver_Filter_Lookup(const void * aPluginPtr)
{
    int index = -1;

    // possibly --- first time here --- initalize mutex and array
    if (myMutexHandlePtr == NULL)
    {
        if (nativeCreateMutex(&myMutexHandlePtr) != 0)
        {
            myMutexHandlePtr = NULL;    // failed to created mutex
        }

        memset( myKnownPlugins, 0, sizeof(myKnownPlugins) );

        myPluginsCount = 0;

        return -1;
    }

    // array always has NULL sentinel --- it's optimized for lookup
    while ( (myKnownPlugins[++index] != NULL) && (myKnownPlugins[index] != aPluginPtr) )
    {
        ; // loop to next
    }

    return (myKnownPlugins[index] == NULL) ? -1 : index;
}


//=======================================================================================
// synopsis: result = Frame_Saver_Filter_Attach(aPluginPtr)
//
// should be called when plugin is created --- returns 0 on success, else error
//=======================================================================================
int Frame_Saver_Filter_Attach(GstElement * aPluginPtr)
{
    int index = Frame_Saver_Filter_Lookup(aPluginPtr);     // -1 if not found

    // verify validity of plugin element
    if (aPluginPtr == NULL)
    {
        do_DBG_print("@@  Frame_Saver_Filter_Attach --- ERROR ---- aPluginPtr \n");
        return -1;
    }

    // possibly --- plugin is not new --- unexpected, but allowed
    if (index >= 0)
    {
        do_DBG_print("@@  Frame_Saver_Filter_Attach --- KNOWN \n");
        return 0;
    }
    
    if (! myMutexHandlePtr)
    {
        do_DBG_print("@@  Frame_Saver_Filter_Attach --- ERROR ---- myMutexHandlePtr \n");
        return -3;  // mutex does not exist
    }

    if (nativeTryLockMutex(myMutexHandlePtr, LOCK_MUTEX_TIMEOUT_MS) != 0)
    {
        do_DBG_print("@@  Frame_Saver_Filter_Attach --- ERROR ---- LOCK_MUTEX_TIMEOUT_MS \n");
        return -2;  // failed to acquire mutex
    }

    // find the NULL sentinel ---- it always exists
    for (index = 0;  myKnownPlugins[index] != NULL;  ++index)
    {
        ; // loop to next
    }

    if (mySniffer.real_plugin_ptr != NULL)
    {
        do_DBG_print("@@  Frame_Saver_Filter_Attach --- ERROR --- MULTI \n");   // TODO: allow many
    }
    else if (index >= MAX_NUM_KNOWN_PLUGINS - 1)
    {
        do_DBG_print("@@  Frame_Saver_Filter_Attach --- ERROR --- LIMIT \n");
    }
    else
    {
        ++myPluginsCount;

        myKnownPlugins[index] = aPluginPtr;

        mySniffer.real_plugin_ptr = aPluginPtr;

        mySniffer.pipeline_ptr = (GstElement *) gst_element_get_parent(aPluginPtr); // TODO: find-root

        frame_saver_params_initialize( &mySplicer.params );

        do_DBG_print("@@  Frame_Saver_Filter_Attach --- SUCCESS \n");
    }

    nativeReleaseMutex(myMutexHandlePtr);

    return (myKnownPlugins[index] == NULL) ? -1 : 0;
}


//=======================================================================================
// synopsis: result = Frame_Saver_Filter_Detach(aPluginPtr)
//
// should be called when plugin gets EOS --- returns 0 on success, else error
//=======================================================================================
int Frame_Saver_Filter_Detach(GstElement * aPluginPtr)
{
    if (! myMutexHandlePtr)
    {
        do_DBG_print("@@  Frame_Saver_Filter_Detach --- ERROR ---- myMutexHandlePtr \n");
        return -3;  // mutex does not exist
    }

    if (nativeTryLockMutex(myMutexHandlePtr, LOCK_MUTEX_TIMEOUT_MS) != 0)
    {
        do_DBG_print("@@  Frame_Saver_Filter_Detach --- ERROR ---- LOCK_MUTEX_TIMEOUT_MS \n");
        return -2;  // failed to acquire mutex
    }

    int index = Frame_Saver_Filter_Lookup(aPluginPtr);     // -1 if not found

    // possibly --- plugin is unknown
    if (index < 0)
    {
        nativeReleaseMutex(myMutexHandlePtr);

        do_DBG_print("@@  Frame_Saver_Filter_Detach --- UNKNOWN \n");

        return -1;
    }

    index = Frame_Saver_Filter_Lookup(aPluginPtr); 

    do_DBG_print("@@  Frame_Saver_Filter_Detach --- SUCCESS \n");

    // keep array items compacted --- the sentinel is always the first NULL
    while (myKnownPlugins[++index] != NULL)
    {
        myKnownPlugins[index - 1] = myKnownPlugins[index]; 
    }

    myKnownPlugins[index] = NULL;

    mySniffer.real_plugin_ptr = NULL;

    if ( --myPluginsCount == 0 )
    {
        void * ptr_mutex = myMutexHandlePtr;

        myMutexHandlePtr = NULL;

        nativeDeleteMutex(ptr_mutex);
    }
    else
    {
        nativeReleaseMutex(myMutexHandlePtr);
    }

    return 0;
}


//=======================================================================================
// synopsis: result = Frame_Saver_Filter_Receive_Buffer(aPluginPtr, aBufferPtr)
//
// called at by the actual plugin upon buffer arrival --- returns GST_FLOW_OK
//=======================================================================================
int Frame_Saver_Filter_Receive_Buffer(GstElement * aPluginPtr, gpointer aBufferPtr)
{
    int result = (int) GST_FLOW_ERROR;

    int  index = Frame_Saver_Filter_Lookup(aPluginPtr);     // -1 if not found

    if (index < 0)
    {
        return result;
    }

    mySniffer.num_appsink_frames += 1;

    if (mySplicer.params.one_snap_ms > 0)
    {
        if (mySniffer.num_snap_signals > mySniffer.num_saved_frames)
        {
            result = do_snap_and_save_frame( (GstAppSink*) mySniffer.video_sink2_ptr,
                                             (GstBuffer *) aBufferPtr,
                                              mySniffer.sinker_caps_ptr,
                                              &mySniffer );
        }
    }

    return result;
}


//=======================================================================================
// synopsis: result = Frame_Saver_Filter_Transition(aPluginPtr)
//
// called at by the actual plugin upon state change --- returns 0 on success, else error
//=======================================================================================
int Frame_Saver_Filter_Transition(GstElement * aPluginPtr, GstStateChange aTransition)
{
    int index = Frame_Saver_Filter_Lookup(aPluginPtr);     // -1 if not found

    if (index >= 0)
    {
        do_DBG_print("@@  Frame_Saver_Filter_Transition \n");

        if (aTransition == GST_STATE_CHANGE_NULL_TO_READY)
        {
            char report[MAX_PARAMS_SPECS_LNG];

            frame_saver_params_write_to_buffer( &mySplicer.params, report, sizeof(report) );

            do_prepare_to_play(FALSE);

            g_print(report);
        }
    }

    return 0;
}


//=======================================================================================
// synopsis: result = Frame_Saver_Filter_Set_Params(aPluginPtr, aNewValuePtr, aDstValuePtr)
//
// called at by the actual plugin to change params --- returns 0 on success, else error
//=======================================================================================
int Frame_Saver_Filter_Set_Params(GstElement  * aPluginPtr, 
                                  const gchar * aNewValuePtr, 
                                  gchar       * aDstValuePtr)
{
    char params_specs[MAX_PARAMS_SPECS_LNG];

    int index = Frame_Saver_Filter_Lookup(aPluginPtr);     // -1 if not found

    int error = 0;

    // possibly --- plugin is unknown
    if (index < 0)
    {
        return -1;
    }

    g_print("@@  Frame_Saver_Filter_Set_Params --- NEW=(%s) --- OLD=(%s) \n", aNewValuePtr, aDstValuePtr);

    // possibly --- invalid length
    if ( sprintf(params_specs, "%s", aNewValuePtr) != (int) strlen(aNewValuePtr) )
    {
        return -2;
    }

    if (strncmp(aNewValuePtr, "wait=", 5) == 0)
    {
        if (frame_saver_params_parse_from_text(&mySplicer.params, params_specs) == TRUE)
        {
            sprintf(aDstValuePtr, "wait=%u", mySplicer.params.max_wait_ms);
        }
        else
        {
            error = 1;
        }
    }
    else if (strncmp(aNewValuePtr, "path=", 5) == 0)
    {
        if (frame_saver_params_parse_from_text(&mySplicer.params, params_specs) == TRUE)
        {
            sprintf(aDstValuePtr, "path=%s", mySplicer.params.folder_path);
        }
        else
        {
            error = 2;
        }
    }
    else if (strncmp(aNewValuePtr, "snap=", 5) == 0)
    {
        if (frame_saver_params_parse_from_text(&mySplicer.params, params_specs) == TRUE)
        {
            sprintf(aDstValuePtr, "snap=%u,%u,%u",
                    mySplicer.params.one_snap_ms,
                    mySplicer.params.max_num_snaps_saved,
                    mySplicer.params.max_num_failed_snap);
        }
        else
        {
            error = 3;
        }
    }
    else if (strncmp(aNewValuePtr, "link=", 5) == 0)
    {
        if (frame_saver_params_parse_from_text(&mySplicer.params, params_specs) == TRUE)
        {
            sprintf(aDstValuePtr, "link=%s,%s,%s",
                    mySplicer.params.pipeline_name,
                    mySplicer.params.producer_name,
                    mySplicer.params.consumer_name);
        }
        else
        {
            error = 4;
        }
    }
    else if (strncmp(aNewValuePtr, "pads=", 5) == 0)
    {
        if (frame_saver_params_parse_from_text(&mySplicer.params, params_specs) == TRUE)
        {
            sprintf(aDstValuePtr, "pads=%s,%s,%s",
                    mySplicer.params.producer_out_pad_name,
                    mySplicer.params.consumer_inp_pad_name,
                    mySplicer.params.consumer_out_pad_name);
        }
        else
        {
            error = 5;
        }
    }

    g_print("@@  Frame_Saver_Filter_Set_Params --- NOW=(%s) --- Error=%d \n", aDstValuePtr, error);

    return error;   // 0 is success
}


//=======================================================================================
// synopsis: result = frame_saver_filter_tester(argc, argv)
//
// performs a self-test of the frame-saver-filter --- returns 0 on success, else error
//=======================================================================================
int frame_saver_filter_tester( int argc, char ** argv )
{
    int result = 0;

    gst_init(NULL, NULL);

    frame_saver_params_initialize( &mySplicer.params );

    if (frame_saver_params_parse_from_array(&mySplicer.params, ++argv, --argc) != TRUE)
    {
        g_print("\n");
        result = 1;
    }
    else if (frame_saver_params_write_to_file(&mySplicer.params, stdout) != TRUE)
    {
        g_print("frame_saver_filter_tester --- failed reporting parameters used \n");
        result = 2;
    }
    else
    {
        result = (int) do_pipeline_create_instance();  // 0 is success

        if (result != 0)
        {
            g_print("frame_saver_filter_tester --- error (%d) creating pipeline \n", result);
        }
        else if (do_pipeline_test_run_main_loop() != TRUE)
        {
            result = 3;
        }
        
        do_pipeline_cleanup();
    }

    return result;   // returns 0 on success
}

