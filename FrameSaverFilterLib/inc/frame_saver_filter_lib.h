/* 
 * ======================================================================================
 * File:        frame_saver_filter_lib.h
 *
 * Purpose:     external interface (API) for Frame_Saver_Filter_Library (aka FSL)
 * 
 * History:     1. 2016-10-14   JBendor     Created    
 *              2. 2016-10-16   JBendor     Updated 
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * ======================================================================================
 */

#ifndef __Frame_Saver_Filter_Lib_H__

#define __Frame_Saver_Filter_Lib_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


//=======================================================================================
// custom elementary types
//=======================================================================================
typedef void    *fsl_handle_t;
typedef int8_t   fsl_bool_t;
typedef int32_t  fsl_error_t;


//=======================================================================================
// useful macros (i.e inlines or constants)
//=======================================================================================
#define FSL_TRUE                        (1)
#define FSL_FALSE                       (0)
#define FSL_SUCCESS                     (0)
#define FSL_IS_SUCCESS(fsl_error)       ((fsl_error) == 0)
#define FSL_IS_HANDLE_VALID(tsv_handle) ((tsv_handle) != NULL)


//=======================================================================================
// examples for configuration parameters
//=======================================================================================
#define FSL_PARAM_PARTS_SPLITTER    ";"
#define FSL_EXAMPLE_CAMERA_FAR1     "type=GRAB;name=/dev/FAR1"  // capture from a named camera
#define FSL_EXAMPLE_CAMERA_AUTO     "type=GRAB;name=@AUTO"      // capture from default camera
#define FSL_EXAMPLE_CAMERA_IDX3     "type=GRAB;name=@3"         // capture from indexed device
#define FSL_EXAMPLE_SCREEN_AUTO     "type=SHOW;name=@AUTO"      // display on default screen
#define FSL_EXAMPLE_SCREEN_LCD1     "type=SHOW;name=/dev/LCD1"  // display on a named screen
#define FSL_EXAMPLE_ENCODE_H264     "type=H264;mux=TS"          // h264 muxed transport-stream
#define FSL_EXAMPLE_ENCODE_NONE     "type=NONE;mux=NONE"        // no encoder, no muxer
#define FSL_EXAMPLE_SOURCE_TEST     "type=TEST;pattern=0"       // videotestsrc, pattern=0 (max=20)
#define FSL_EXAMPLE_STREAM_5SEC     "time=5000;priority=0"      // 5000 ms, lowest priority (max=9)
#define FSL_EXAMPLE_OUTPUT_SEND     "type=SEND;mode=H264;host=192.168.1.110;port=1234"
#define FSL_FMT_VID_RAW             "video/x-raw,"
#define FSL_FMT_RAW_I420            "format=I420"
#define FSL_FMT_RAW_YUY2            "format=YUY2"

#if defined (_USE_GSTREAMER_VER_) && (_USE_GSTREAMER_VER_ < 100)       
    #undef  FSL_FMT_VID_RAW
    #define FSL_FMT_VID_RAW     "video/x-raw-yuv,"

    #undef  FSL_FMT_RAW_I420
    #define FSL_FMT_RAW_I420     "format=(fourcc)I420"

    #undef  FSL_FMT_RAW_YUY2
    #define FSL_FMT_RAW_YUY2     "format=(fourcc)YUY2"
#endif

#define FSL_FORMAT_I420_352x288     FSL_FMT_VID_RAW "width=352,height=288,framerate=30/1," FSL_FMT_RAW_I420
#define FSL_FORMAT_I420_800x400     FSL_FMT_VID_RAW "width=800,height=400,framerate=30/1," FSL_FMT_RAW_I420
#define FSL_FORMAT_YUY2_640x480     FSL_FMT_VID_RAW "width=640,height=480,framerate=30/1," FSL_FMT_RAW_YUY2

#if defined _WIN32
    #define FSL_EXAMPLE_OUTPUT_264  "type=FILE;name=E:/vid264.ts"       // filesink location=E:/vid264.ts
    #define FSL_EXAMPLE_OUTPUT_YUV  "type=FILE;name=E:/vid420.yuv"      // filesink location=E:/vid420.yuv
    #define FSL_EXAMPLE_SOURCE_MP4  "type=FILE;name=E:/bunny.mp4"       // filesrc  location=E:/bunny.mp4
    #define FSL_EXAMPLE_SOURCE_TOY  "type=FILE;name=E:/352x288-TOY.yuv;blocksize=152064;"  FSL_FORMAT_I420_352x288
    #define FSL_EXAMPLE_SOURCE_BUS  "type=FILE;name=E:/352x288-BUS.yuv;blocksize=152064;"  FSL_FORMAT_I420_352x288
#elif defined _LINUX
    #define FSL_EXAMPLE_OUTPUT_264  "type=FILE;name=/tmp/vid264.ts"     // filesink location=/tmp/vid264.ts
    #define FSL_EXAMPLE_OUTPUT_YUV  "type=FILE;name=/tmp/vid420.yuv"    // filesink location=/tmp/vid420.yuv
    #define FSL_EXAMPLE_SOURCE_MP4  "type=FILE;name=/tmp/bunny.mp4"     // filesrc  location=/tmp/bunny.mp4
    #define FSL_EXAMPLE_SOURCE_TOY  "type=FILE;name=/tmp/352x288-TOY.yuv;blocksize=152064;" FSL_FORMAT_I420_352x288
    #define FSL_EXAMPLE_SOURCE_BUS  "type=FILE;name=/tmp/352x288-BUS.yuv;blocksize=152064;" FSL_FORMAT_I420_352x288
#endif


//=======================================================================================
// synopsis: psz_version_text = fsl_get_version()
//
// gets version info about Gstreamer and FSL
//=======================================================================================
extern const char * fsl_get_version();


//=======================================================================================
// synopsis: result = fsl_initialize()
//
// initializes the library --- returns 0 on success, else error
//=======================================================================================
extern int fsl_initialize();


//=======================================================================================
// synopsis: result = fsl_main_test(argc,argv)
//
// performs a self-test on the library --- returns 0
//=======================================================================================
extern int fsl_main_test(int argc, char *argv[]);


#ifdef __cplusplus
}
#endif  // __cplusplus


#endif // __Frame_Saver_Filter_Lib_H__
