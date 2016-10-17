/* 
 * ======================================================================================
 * File:        tee_to_two_example.c
 * 
 * History:     1. 2016-10-14   JBendor     Created
 *              2. 2016-10-16   JBendor     Updated 
 *
 * Description: Uses the Gstreamer TEE to split one video source into two sinks.
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

// #include <opencv2/imgproc/imgproc.hpp>   // TODO

#define _USE_APP_SINK_FOR_FRAME_SAVE________TODO

#define _TEE_ONE_SOURCE_TO_TWO_SINKS_


const guint64 NANOS_PER_MILLISEC = 1000L * 1000L;


typedef struct
{
    gpointer frame_ptr;   // TODO: (IplImageFrame *)
} ImageFrame_t;


typedef struct
{
    GMainLoop   * main_loop_ptr;    // Main-Event-Loop --- manages all events

    GstBus      * bus_ptr;          // bus transfers messages from/to pipeline

    GstElement  * pipeline_ptr,
                * vid_source_ptr,
                * video_sink1_ptr,
                * video_sink2_ptr,
                * cvt_element_ptr,
                * tee_element_ptr,
                * Q_1_element_ptr,
                * Q_2_element_ptr;

    GstCaps     * video_caps_ptr;

    GstClockTime start_run_time_ns;
    GstClockTime next_snap_time_ns;

    guint   one_tick_ms,    // timer-ticks interval as milliseconds
            one_snap_ms,    // frame-snaps interval as milliseconds
            max_play_ms,    // run-timeout as milliseconds 
            snaps_count;    // frame-snaps counter

    GstAppSinkCallbacks appsink_callbacks;

    ImageFrame_t image_frame;

} TeeState_t;

static TeeState_t   myT;


static int do_cleanup()
{
    if (myT.main_loop_ptr != NULL)
    {
        g_main_loop_quit(myT.main_loop_ptr);

        gst_object_unref(GST_OBJECT(myT.pipeline_ptr));

        g_main_loop_unref(myT.main_loop_ptr);

        myT.pipeline_ptr = NULL;

        myT.main_loop_ptr = NULL;
    }

    if (myT.pipeline_ptr != NULL)
    {
        gst_object_unref(myT.pipeline_ptr);

        myT.pipeline_ptr = NULL;
    }

    return 0;
}


static gboolean do_bus_callback(GstBus *aBusPtr, GstMessage *aMsgPtr, void *aDataPtr)
{
    GError * err_ptr;

    const GstStructure * gst_struct_ptr;

    int msg_time = (int) aMsgPtr->timestamp;

    int msg_type = GST_MESSAGE_TYPE(aMsgPtr);

    switch (msg_type)
    {
    case GST_MESSAGE_EOS:
        g_main_loop_quit(myT.main_loop_ptr);
        break;

    case GST_MESSAGE_ERROR:
        gst_message_parse_error(aMsgPtr, &err_ptr, NULL);
        printf("   bus_msg --- ERROR: %s", err_ptr->message);
        g_error_free(err_ptr);
        g_main_loop_quit(myT.main_loop_ptr);
        break;

    case GST_MESSAGE_APPLICATION:
        gst_struct_ptr = gst_message_get_structure(aMsgPtr);
        if (gst_structure_has_name(gst_struct_ptr, "turn_off"))
        {
            g_main_loop_quit(myT.main_loop_ptr);
        }
        break;

    default:
        break;
    }

    if (msg_type == GST_MESSAGE_STATE_CHANGED)
    {
        GstState old_state, new_state, pending_state;

        gst_message_parse_state_changed(aMsgPtr, &old_state, &new_state, &pending_state);
        
        printf("   bus_msg --- State changed. Old: %i New: %i Pending: %i. \n", old_state, new_state, pending_state);
    }
    else
    {
        const char * psz_msg_type_name = GST_MESSAGE_TYPE_NAME(aMsgPtr); 

        printf("   bus_msg --- time=%i (%s) type=%i \n", msg_time, psz_msg_type_name, msg_type);
    }

    return TRUE;
}


static void do_unref_sample_or_buffer(gpointer aSomePtr)
{
    #if (GST_VERSION_MAJOR == 0)
        gst_buffer_unref( (GstBuffer *) aSomePtr );
    #else
        gst_sample_unref( (GstSample *) aSomePtr );
    #endif

    return;
}


static GstFlowReturn do_appsink_callback_for_new_data(GstAppSink * aAppSinkPtr, gpointer aUserDataPtr) 
{
    #if (GST_VERSION_MAJOR == 0)
        GstBuffer *data_buf_ptr = gst_app_sink_pull_buffer (aAppSinkPtr);
    #else
        GstSample *data_buf_ptr = gst_app_sink_pull_sample (aAppSinkPtr);
    #endif

    TeeState_t * ptr_my_state = (TeeState_t *) aUserDataPtr;

    ImageFrame_t  * image_ptr = &ptr_my_state->image_frame;

    if ( (data_buf_ptr == NULL) || (ptr_my_state == NULL) || (image_ptr == NULL) )
    {
        do_unref_sample_or_buffer(data_buf_ptr);
        return GST_FLOW_ERROR;
    }

#ifdef _USE_APP_SINK_FOR_FRAME_SAVE_

    #define CV_CREATE_IMAGE(frame_size, num_pix_bits, num_planes)   (NULL)
    #define CV_SAVE_IMAGE(psz_path, frame_ptr)                      ((psz_path != NULL) && (frame_ptr != NULL))
    #define CV_SIZE(wdt, hgt)                                       (0L)
    #define SET_IMAGE_PIXMAP(frame_ptr, pixbuf_ptr)                 ((frame_ptr != NULL) && (pixbuf_ptr != NULL))

    if (image_ptr->frame_ptr == NULL)
    {
        image_ptr->frame_ptr = CV_CREATE_IMAGE( CV_SIZE(0, 0), IPL_DEPTH_8U, 1 );   // TODO
    }
    
    if (image_ptr->frame_ptr == NULL)
    {
        do_unref_sample_or_buffer(data_buf_ptr);
        return GST_FLOW_ERROR;
    }

    SET_IMAGE_PIXMAP(image_ptr->frame_ptr, GST_BUFFER_DATA(data_buf_ptr));  // TODO

    CV_SAVE_IMAGE("SomeFilePath", image_ptr->frame_ptr);

    do_unref_sample_or_buffer(data_buf_ptr);

#else

    gst_app_src_push_buffer( GST_APP_SRC(ptr_my_state->vid_source_ptr), (GstBuffer *) data_buf_ptr ); // TODO

#endif

    return GST_FLOW_OK;
}


static int do_setup_appsink(GstAppSink * aAppSinkPtr)
{
    if (! aAppSinkPtr)
    {
        return 0;
    }

#ifdef _USE_APP_SINK_FOR_FRAME_SAVE_

    myT.appsink_callbacks.eos          = NULL;
    myT.appsink_callbacks.new_preroll  = NULL;
    myT.appsink_callbacks.new_sample   = do_appsink_callback_for_new_data;

    gst_app_sink_set_callbacks(GST_APP_SINK(aAppSinkPtr), 
                               &myT.appsink_callbacks, 
                               &myT,    // pointer_to_data_passed_to_callbacks
                               NULL);   // pointer_to_destroy_notify_callback

    gst_app_sink_set_drop(aAppSinkPtr, TRUE);

    gst_app_sink_set_max_buffers(aAppSinkPtr, 1);

    gst_app_sink_set_emit_signals(aAppSinkPtr, TRUE);

#endif

    return 1;
}


static int do_link_TEE_request_pads()
{
    GstPad  * tee_Q1_pad_ptr, 
            * tee_Q2_pad_ptr,
            * queue1_pad_ptr, 
            * queue2_pad_ptr;

    GstElementClass * class_ptr = GST_ELEMENT_GET_CLASS(myT.tee_element_ptr);

    GstPadTemplate  * pad_template_ptr = gst_element_class_get_pad_template(class_ptr, "src_%u");

    if (! pad_template_ptr)
    {
        g_critical("Unable to get pad template");        
        return 0;
    }

    tee_Q1_pad_ptr = gst_element_request_pad(myT.tee_element_ptr, 
                                             pad_template_ptr, 
                                             NULL, 
                                             NULL);

    g_print("Got request pad (%s) for tee-branch-Q1 \n", gst_pad_get_name(tee_Q1_pad_ptr));

    queue1_pad_ptr = gst_element_get_static_pad(myT.Q_1_element_ptr, "sink");

    tee_Q2_pad_ptr = gst_element_request_pad(myT.tee_element_ptr, 
                                             pad_template_ptr, 
                                             NULL, 
                                             NULL);

    g_print("Got request pad (%s) for tee-branch-Q2 \n", gst_pad_get_name(tee_Q2_pad_ptr));

    queue2_pad_ptr = gst_element_get_static_pad(myT.Q_2_element_ptr, "sink");

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


static int do_tee2two_make_pipeline()
{
    static const char * PIPELINE_NAME   = "tee2two_pipeline";
    static const char * VID_SOURCE_NAME = "video_source";
    static const char * VID_SINK_1_NAME = "video_sink_1";
    static const char * VID_SINK_2_NAME = "video_sink_2";
    static const char * VID_CVT_NAME 	= "cvt_element";
    static const char * VID_TEE_NAME 	= "tee_element";
    static const char * QUEUE_1_NAME 	= "tee_queue_1";
    static const char * QUEUE_2_NAME 	= "tee_queue_2";

	#ifdef _WIN32
		static const char * VID_SRC_ELEMENT = "videotestsrc";       // other: "ksvideosrc"
		static const char * VID_SNK_ELEMENT = "autovideosink";      // other: "directdrawsink"
    #endif

	#ifdef _LINUX
		static const char * VID_SRC_ELEMENT = "videotestsrc";       // other: "v4l2src"
		static const char * VID_SNK_ELEMENT = "autovideosink";      // other: "ximagesink"
	#endif

    gst_init(NULL, NULL);

    myT.pipeline_ptr = gst_pipeline_new(PIPELINE_NAME);

    myT.bus_ptr = gst_pipeline_get_bus(GST_PIPELINE(myT.pipeline_ptr));

    if ((! myT.pipeline_ptr) || (! myT.bus_ptr))
    {
        g_critical("Unable to create: pipeline or bus");
        return do_cleanup();
    }

    gst_bus_add_watch(myT.bus_ptr, do_bus_callback, NULL);
    gst_object_unref(myT.bus_ptr);
    myT.bus_ptr = NULL;

    // create the pipeline's elements (aka: plugins)
    myT.vid_source_ptr  = gst_element_factory_make(VID_SRC_ELEMENT, VID_SOURCE_NAME);
    myT.cvt_element_ptr = gst_element_factory_make("videoconvert",  VID_CVT_NAME);
    myT.tee_element_ptr = gst_element_factory_make("tee",           VID_TEE_NAME);
    myT.Q_1_element_ptr = gst_element_factory_make("queue",         QUEUE_1_NAME);
    myT.Q_2_element_ptr = gst_element_factory_make("queue",         QUEUE_2_NAME);

    #ifdef _USE_APP_SINK_FOR_FRAME_SAVE_
        myT.video_sink1_ptr = gst_element_factory_make(VID_SNK_ELEMENT, VID_SINK_1_NAME);
        myT.video_sink2_ptr = gst_element_factory_make("appsink",       VID_SINK_2_NAME);
    #else
        myT.video_sink1_ptr = gst_element_factory_make(VID_SNK_ELEMENT, VID_SINK_1_NAME);
        myT.video_sink2_ptr = gst_element_factory_make(VID_SNK_ELEMENT, VID_SINK_2_NAME);
    #endif

    // create the video format (aka: capabilities)
    myT.video_caps_ptr = gst_caps_new_simple("video/x-raw",
                                             "width",  G_TYPE_INT, 400,
                                             "height", G_TYPE_INT, 200,
                                             NULL);

    int flags = (myT.vid_source_ptr  ? 0 : 0x001) + 
                (myT.cvt_element_ptr ? 0 : 0x002) +
                (myT.video_sink1_ptr ? 0 : 0x004) + 
                (myT.video_sink2_ptr ? 0 : 0x008) +
                (myT.Q_1_element_ptr ? 0 : 0x010) +
                (myT.Q_2_element_ptr ? 0 : 0x020) +
                (myT.tee_element_ptr ? 0 : 0x040) +
                (myT.video_caps_ptr  ? 0 : 0x080) ;

    if (flags != 0)
    {
        g_critical("Unable to create some pipeline elements (0x%0X)", flags);
        return do_cleanup();
    }

#ifdef _TEE_ONE_SOURCE_TO_TWO_SINKS_

    // put all the elements in the pipeline
    gst_bin_add_many(GST_BIN(myT.pipeline_ptr),
                     myT.vid_source_ptr,
                     myT.cvt_element_ptr,
                     myT.tee_element_ptr,
                     myT.Q_1_element_ptr,
                     myT.video_sink1_ptr,
                     myT.Q_2_element_ptr,
                     myT.video_sink2_ptr,
                     NULL);

    // link elements: video-source to video-convert
    if (FALSE == gst_element_link_many(myT.vid_source_ptr, myT.cvt_element_ptr, NULL))
    {
        g_critical("Unable to link elements: SRC --> CVT \n");       
        return do_cleanup();
    }

    // link elements: video-convert to tee --- with the desired video-format
    if (FALSE == gst_element_link_filtered(myT.cvt_element_ptr, myT.tee_element_ptr, myT.video_caps_ptr))
    {
        g_critical("Unable to link filtered: CVT --> TEE \n"); 
        return do_cleanup();
    }

    // link elements: queue-1 to video-sink-1
    if (FALSE == gst_element_link_many(myT.Q_1_element_ptr, myT.video_sink1_ptr, NULL))
    {
        g_critical("Unable to link elements: CVT --> TEE --> Q1 --> SINK1");        
        return do_cleanup();
    }

    // link elements: queue-2 to video-sink-2
    if (FALSE == gst_element_link_many(myT.Q_2_element_ptr, myT.video_sink2_ptr, NULL))
    {
        g_critical("Unable to link elements: CVT --> TEE --> Q2 --> SINK2");
        return do_cleanup();
    }

    if (do_link_TEE_request_pads() == 0)
    {
        return do_cleanup();
    }

    if (do_setup_appsink((GstAppSink*) myT.video_sink2_ptr) == 0)
    {
        return do_cleanup();
    }

#else   // undefined _TEE_ONE_SOURCE_TO_TWO_SINKS_

    gst_bin_add_many(GST_BIN(myT.pipeline_ptr), myT.vid_source_ptr, myT.video_sink1_ptr,  NULL);

    if (FALSE == gst_element_link_filtered(myT.vid_source_ptr, myT.video_sink1_ptr, myT.video_caps_ptr))
    {
        g_critical("Unable to link filtered: SRC --> SINK1 \n"); 
        return do_cleanup();
    }

#endif // _TEE_ONE_SOURCE_TO_TWO_SINKS_

    return 1;
}


static gboolean do_timer_callback(gpointer aDataPtr)
{
    GstClockTime present_ns = gst_clock_get_time (gst_system_clock_obtain());

    GstClockTime elapsed_ns = present_ns - myT.start_run_time_ns;

    GstClockTime play_ms = elapsed_ns / NANOS_PER_MILLISEC;

    printf("tee2two --- elapsed: %u ", (uint32_t) play_ms);

    if ( elapsed_ns >= myT.next_snap_time_ns )
    {
        myT.next_snap_time_ns += NANOS_PER_MILLISEC * myT.one_snap_ms;

        printf(" ... snaps=%u ", (uint32_t) ++myT.snaps_count);
    }

    printf("\n");

    if ( (myT.pipeline_ptr == NULL) || (play_ms > myT.max_play_ms) )
    {
        gst_element_send_event ( myT.vid_source_ptr, gst_event_new_eos() );

        return FALSE;
    }    

    return TRUE;
}


static void do_tee2two_run()
{
    printf("tee2two --- active \n");

    myT.main_loop_ptr = g_main_loop_new(NULL, FALSE);

    gst_element_set_state(GST_ELEMENT(myT.pipeline_ptr), GST_STATE_PLAYING);

    g_timeout_add ( (guint) myT.one_tick_ms, do_timer_callback, NULL );

    myT.start_run_time_ns = gst_clock_get_time (gst_system_clock_obtain());

    myT.next_snap_time_ns = NANOS_PER_MILLISEC * myT.one_snap_ms;

    myT.snaps_count = 0;

    g_main_loop_run(myT.main_loop_ptr);

    gst_element_set_state( GST_ELEMENT(myT.pipeline_ptr), GST_STATE_NULL );

    do_cleanup();

    printf("tee2two --- ended \n");
}


static gboolean do_tee2two_parse_args(int argc, char *argv[])
{
    gboolean is_ok = TRUE;

    myT.one_tick_ms = 1000;
    myT.one_snap_ms = 5000;
    myT.max_play_ms = 22000;

    if ( (argc < 2) || (! argv) )
    {
        printf("args used:  [tick=%d]  [snap=%d]  [play=%d] \n\n",
                myT.one_tick_ms,
                myT.one_snap_ms,
                myT.max_play_ms );

        return TRUE;
    }

    while ( is_ok && (--argc > 0) )
    {
        const char * psz_param = argv[argc];

        if ( strncmp(psz_param, "tick=", 5) == 0 )
        {
            is_ok = (sscanf(&psz_param[5], "%d", &myT.one_tick_ms) == 1) && (myT.one_tick_ms > 0);
            continue;
        }

        if ( strncmp(psz_param, "snap=", 5) == 0 )
        {
            is_ok = (sscanf(&psz_param[5], "%d", &myT.one_snap_ms) == 1) && (myT.one_snap_ms > 0);
            continue;
        }

        if ( strncmp(psz_param, "play=", 5) == 0 )
        {
            is_ok = (sscanf(&psz_param[5], "%d", &myT.max_play_ms) == 1) && (myT.max_play_ms > 0);
            continue;
        }
    }

    if (is_ok)
    {
        if (myT.one_tick_ms < 10)
        {
            printf("minimum 'tick' is 10 ms \n");
            is_ok = FALSE;
        }
        else if (myT.one_tick_ms < myT.one_snap_ms)
        {
            printf("minimum 'snap' is one 'tick' \n");      
            is_ok = FALSE;
        }
        else if (myT.one_tick_ms < myT.max_play_ms)
        {
            printf("minimum 'play' is one 'tick' \n");      
            is_ok = FALSE;
        }
    }
    else
    {
        printf("invalid arg: (%s) \n", argv[argc]);
    }

    return is_ok;
}


int tee_to_two_example(int argc, char *argv[])
{
    int result = 0;

    if (do_tee2two_parse_args(argc, argv) == FALSE)
    {
        printf("\n");
        result = -1;
    }
    else if (do_tee2two_make_pipeline() == 0)
    {
        printf("tee2two --- unable to initialize pipeline \n");
        result = -2;
    }
    else
    {
        do_tee2two_run();
    }

    return result;
}
