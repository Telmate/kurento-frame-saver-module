/* 
 * ======================================================================================
 * File:        frame_saver_filter_lib.c
 *
 * Purpose:     implements the API for the Frame_Saver_Filter_Library (aka FSL)
 * 
 * History:     1. 2016-10-14   JBendor     Created
 *              2. 2016-10-16   JBendor     Updated 
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * ======================================================================================
 */

#include "frame_saver_filter_lib.h"
#include "wrapped_native_platforms.h"

#include <gst/gst.h>

static guint s_GST_ver_major, s_GST_ver_minor, s_GST_ver_micro, s_GST_ver_nano;

static char s_FSL_Version[200] = { 0 };

extern int tee_to_two_example(int argc, char *argv[]);


//=======================================================================================
// synopsis: psz_version_text = fsl_get_version()
//
// gets version info about Gstreamer and FSL
//=======================================================================================
const char * fsl_get_version()
{
    return s_FSL_Version;
}


//=======================================================================================
// synopsis: result = fsl_initialize()
//
// initializes the library --- returns 0 on success, else error
//=======================================================================================
int fsl_initialize()
{
    gst_init(NULL, NULL);

    gst_version(&s_GST_ver_major, &s_GST_ver_minor, &s_GST_ver_micro, &s_GST_ver_nano);

    snprintf(s_FSL_Version, 
             ARRAY_CAPACITY(s_FSL_Version),
             "GStreamer version %d.%d.%d.%d --- _USE_GSTREAMER_VER_=%d \n", 
             s_GST_ver_major, 
             s_GST_ver_minor, 
             s_GST_ver_micro, 
             s_GST_ver_nano, 
             _USE_GSTREAMER_VER_);

    return 0;
}


//=======================================================================================
// synopsis: test_result = fsl_main_test(argc, argv)
//
// performs a test on the FSL library --- returns 0
//=======================================================================================
int fsl_main_test(int argc, char *argv[])
{
    printf( "fsl_main_test() started \n" );

    fsl_initialize();

    printf( fsl_get_version() );

    printf( "tee_2_two_test_result = (0x%X) \n", tee_to_two_example(argc, argv) );

    printf( "fsl_main_test() ended \n\n" );

    nativeWaitForKeypress();
    
    return 0;
}
