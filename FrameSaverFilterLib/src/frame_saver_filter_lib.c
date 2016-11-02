/* 
 * ======================================================================================
 * File:        frame_saver_filter_lib.c
 *
 * Purpose:     implements the API for the Frame_Saver_Filter_Library (aka FSL)
 * 
 * History:     1. 2016-10-14   JBendor     Created
 *              2. 2016-11-01   JBendor     Updated 
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * ======================================================================================
 */

#include "frame_saver_filter_lib.h"
#include "frame_saver_filter.h"
#include <gst/gst.h>


static guint s_GST_ver_major, s_GST_ver_minor, s_GST_ver_micro, s_GST_ver_nano;

static char s_FSL_Version[200] = { 0 };


//=======================================================================================
// synopsis: (void) do_wait_for_keypress()
//
// WIN32: waits for any keypess. UNIX: waits only for Ctrl-C 
//=======================================================================================
static void do_wait_for_keypress()
{
#if defined _LINUX || defined __GNUC__
    printf("please press Ctrl-C ... ");
    while ( getchar() != EOF )
    {
        continue;
    }
#else
    extern int _kbhit(void);
    printf("please press any key ... ");
    while ( !_kbhit() )
    {
        continue;
    }
#endif
    return;
}


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
             sizeof(s_FSL_Version) / sizeof(*s_FSL_Version),
             "GStreamer version is: %d.%d.%d.%d \n", 
             s_GST_ver_major, 
             s_GST_ver_minor, 
             s_GST_ver_micro, 
             s_GST_ver_nano);

    return 0;
}


//=======================================================================================
// synopsis: test_result = fsl_main_test(argc, argv)
//
// performs a test on the FSL library --- returns 0
//=======================================================================================
int fsl_main_test(int argc, char *argv[])
{
    printf( "%s%s", "\n", "fsl_main_test() started \n" );

    fsl_initialize();

    printf( "%s", fsl_get_version() );

    printf( "fsl_main_test() result=(0x%X) \n", frame_saver_filter_tester(argc, argv) );

    printf( "fsl_main_test() ended \n\n" );

    do_wait_for_keypress();
    
    return 0;
}
