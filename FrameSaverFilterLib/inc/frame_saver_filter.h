/* 
 * ======================================================================================
 * File:        frame_saver_filter.h
 *
 * Purpose:     external interface (API) for Frame_Saver_Filter
 * 
 * History:     1. 2016-10-29   JBendor     Created    
 *              2. 2016-11-01   JBendor     Updated 
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * ======================================================================================
 */

#ifndef __Frame_Saver_Filter_H__

#define __Frame_Saver_Filter_H__


#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


//=======================================================================================
// synopsis: result = frame_saver_filter_tester(argc, argv)
//
// performs a self-test of the frame_saver_filter --- returns 0 for success, else error
//=======================================================================================
int frame_saver_filter_tester(int argc, char *argv[]);


#ifdef __cplusplus
}
#endif  // __cplusplus


#endif // __Frame_Saver_Filter_H__
