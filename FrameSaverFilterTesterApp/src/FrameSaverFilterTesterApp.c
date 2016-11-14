/* 
 * =================================================================================================
 * File:        FrameSaverFilterTesterApp.c
 * 
 * History:     1. 2016-10-14   JBendor     Created
 *              2. 2016-11-13   JBendor     Updated
 * 
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * =================================================================================================
 */

#include "frame_saver_filter_lib.h"


int main(int argc, char** argv)
{
    int fsl_test_result = fsl_main_test(argc, argv);

    return fsl_test_result;
}
