/*
 * ======================================================================================
 * File:        kms_Frame_Saver_Plugin.h
 *
 * Purpose:     Kurento+Gstreamer plugin-filter --- uses "Frame-Saver" for full behavior.
 *
 * History:     1. 2016-11-25   JBendor     Created as copy of "gst_Frame_Saver_Plugin.h"
 *              2. 2016-12-13   JBendor     Updated
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * ======================================================================================
 */

#ifndef _GST_FRAME_SAVER_VIDEO_FILTER_PLUGIN_H_
#define _GST_FRAME_SAVER_VIDEO_FILTER_PLUGIN_H_

#include <gst/video/video.h>
#include <gst/video/gstvideofilter.h>

#define      _IS_KURENTO_FILTER_
#define  NO___SAVE_IMAGE_FRAMES_
#define  NO___NEED_APPSINK_STUBS_
#define  NO___ALLOW_DYNAMIC_PARAMS_
#define  NO___ALLOW_BUTTON_ACTIONS_

G_BEGIN_DECLS

#define KMS_TYPE_FRAME_SAVER_PLUGIN             (kms_frame_saver_plugin_get_type())
#define KMS_FRAME_SAVER_PLUGIN(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), KMS_TYPE_FRAME_SAVER_PLUGIN, KmsFrameSaverPlugin))
#define KMS_FRAME_SAVER_PLUGIN_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass),  KMS_TYPE_FRAME_SAVER_PLUGIN, KmsFrameSaverPluginClass))
#define KMS_IS_FRAME_SAVER_PLUGIN(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), KMS_TYPE_FRAME_SAVER_PLUGIN))
#define KMS_IS_FRAME_SAVER_PLUGIN_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass),  KMS_TYPE_FRAME_SAVER_PLUGIN))


typedef struct _KmsFrameSaverPlugin             KmsFrameSaverPlugin;
typedef struct _KmsFrameSaverPluginClass        KmsFrameSaverPluginClass;
typedef struct _KmsFrameSaverPluginPrivate      KmsFrameSaverPluginPrivate;


#ifdef THIS_PLUGIN_NAME
    #undef THIS_PLUGIN_NAME
#endif


#ifdef _IS_KURENTO_FILTER_

    #define THIS_PLUGIN_NAME  "framesavervideofilter"   // KMS needs lower-case plugin names

    struct _KmsFrameSaverPluginClass
    {
        GstVideoFilterClass     base_videofilter_class;

    };

    struct _KmsFrameSaverPlugin
    {
        GstVideoFilter                       base;

        struct _KmsFrameSaverPluginPrivate * priv;

    };

#else   // regular Gstreamer plugin

    // TODO: #define THIS_PLUGIN_NAME  "FrameSaverPlugin"        // GStreamer allows mixed-case names

    struct _KmsFrameSaverPluginClass
    {
        GstElementClass     base_element_class;

    };

    struct _KmsFrameSaverPlugin
    {
        // empty --- all is private

    };

    static GstStateChangeReturn kms_frame_saver_plugin_change_state(GstElement *element, GstStateChange transition);
    static GstFlowReturn        kms_frame_saver_plugin_chain(GstPad * pad, GstObject * parent, GstBuffer * buf);
    static gboolean             kms_frame_saver_plugin_src_query(GstPad *pad, GstObject *parent, GstQuery *query);
    static gboolean             kms_frame_saver_plugin_sink_event(GstPad * pad, GstObject * parent, GstEvent * aEventPtr);
#endif


G_END_DECLS


#ifndef FRAME_SAVER_VERSION

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

    #define PLUGIN_VERSION ( "1.0.0"  " " BUILD_TYPE  "." BUILD_TOOL " " BUILD_DATETIME  " name=" THIS_PLUGIN_NAME )

#endif


#ifndef PACKAGE
    #define PACKAGE     "frame_saver_plugin_package"
#endif

#ifndef VERSION
    #define VERSION     PLUGIN_VERSION
#endif


#endif  // _GST_FRAME_SAVER_VIDEO_FILTER_PLUGIN_H_

