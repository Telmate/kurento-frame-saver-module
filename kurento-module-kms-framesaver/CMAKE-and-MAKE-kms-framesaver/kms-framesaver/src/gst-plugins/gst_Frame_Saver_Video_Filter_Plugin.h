/*
 * ======================================================================================
 * File:        gst_Frame_Saver_Video_Filter_Plugin.h
 *
 * Purpose:     Kurento+Gstreamer plugin-filter --- uses "Frame-Saver" for full behavior.
 *
 * History:     1. 2016-11-25   JBendor     Created as copy of "gst_Frame_Saver_Plugin.h"
 *              2. 2016-11-28   JBendor     Updated
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * ======================================================================================
 */

#ifndef _GST_FRAME_SAVER_VIDEO_FILTER_PLUGIN_H_
#define _GST_FRAME_SAVER_VIDEO_FILTER_PLUGIN_H_

#include <config.h>

#ifdef THIS_PLUGIN_NAME
    #undef THIS_PLUGIN_NAME
#endif

#define THIS_PLUGIN_NAME  "framesavervideofilter"

#define  _SAVE_IMAGE_FRAMES_

#define  _VIDEO_FILTER_TYPE_

#define  NO___PROVIDE_APPSINK_STUBS_

#define  NO___ALLOW_DYNAMIC_PARAMS__


#endif  // _GST_FRAME_SAVER_VIDEO_FILTER_PLUGIN_H_
