/* 
* ======================================================================================
* File:        save_frames_as_png_file.c
*
* Purpose:     saves image frames as PNG files
* 
* History:     1. 2016-10-17   JBendor     Created
*              2. 2016-11-01   JBendor     Updated 
*
* Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
*               Unauthorized copying of this file is strictly prohibited.
* ======================================================================================
*/

#include <png.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#ifdef _LINUX
    #define stricmp strcasecmp
#endif

typedef struct 
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} RGB24_Pix_t, *RGB24_Pix_Ptr_t;

typedef struct 
{
    uint8_t alpha;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} RGB32_Pix_t, *RGB32_Pix_Ptr_t;


static const int NUM_RGB24_PIXEL_BYTES = sizeof(RGB24_Pix_t); 
static const int NUM_RGB32_PIXEL_BYTES = sizeof(RGB32_Pix_t); 
static const int NUM_SAMPLE_BITS_R_G_B = (8); 

typedef struct  
{
    RGB24_Pix_Ptr_t pixels;
    png_uint_32     width;
    png_uint_32     height;
    png_uint_32     stride;
} RGB_Pixmap_t, *RGB_Pixmap_Ptr_t;


// returns a pointer to the pixel at the point (col_idx, row_idx)
static RGB24_Pix_Ptr_t do_get_RGB_pixel_ptr_at (RGB_Pixmap_Ptr_t bitmap_ptr, int col_idx, int row_idx)
{
    png_byte * ptr_col_in_first_row = (png_byte *) &bitmap_ptr->pixels[col_idx];

    return (RGB24_Pix_Ptr_t) (ptr_col_in_first_row + (bitmap_ptr->stride * row_idx));
}


// Write bitmap to a PNG file specified by path; returns 0 if OK, else error
static int do_save_png_image_to_file (RGB_Pixmap_Ptr_t bitmap_ptr, const char * file_path)
{
    png_uint_32 row_idx = 0;

    png_structp png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) 
    {
        return -1;
    }

    png_infop info_ptr = png_create_info_struct (png_ptr);
    if (info_ptr == NULL) 
    {
        png_destroy_write_struct (&png_ptr, &info_ptr);
        return -2;
    }

    if (setjmp(png_jmpbuf(png_ptr)) != 0) 
    {
        png_destroy_write_struct (&png_ptr, &info_ptr);
        return -3;
    }

    FILE * fp = fopen (file_path, "wb");
    if (! fp) 
    {
        png_destroy_write_struct (&png_ptr, &info_ptr);
        return -4;
    }

    png_set_IHDR (png_ptr,
                  info_ptr,
                  bitmap_ptr->width,
                  bitmap_ptr->height,
                  NUM_SAMPLE_BITS_R_G_B,
                  PNG_COLOR_TYPE_RGB,
                  PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_DEFAULT,
                  PNG_FILTER_TYPE_DEFAULT);

    png_alloc_size_t length = (bitmap_ptr->height + 1) * sizeof(png_bytep);

    png_bytep* row_pointers = png_malloc (png_ptr, length);

    for (row_idx = 0;  row_idx < bitmap_ptr->height;  ++row_idx) 
    {
        RGB24_Pix_Ptr_t row_ptr = do_get_RGB_pixel_ptr_at(bitmap_ptr, 0, row_idx);

        row_pointers[row_idx] = (png_bytep) row_ptr;
    }

    png_init_io (png_ptr, fp);

    png_set_rows (png_ptr, info_ptr, row_pointers);

    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    png_free (png_ptr, row_pointers);

    fclose (fp);

    return 0;
}

static int do_save_RGB24_frame(const char   * aPathPtr,
                               void         * aPixelsPtr,
                               int            aStrideLng,
                               int            aFrameWdt, 
                               int            aFrameHgt)
{
    RGB_Pixmap_t rgb_image;

    int min_stride = sizeof(RGB24_Pix_t) * aFrameWdt;

    if ( (aFrameWdt < 9) || (aFrameHgt < 9) || (aStrideLng < min_stride) )
    {
        return -1;
    }

    if (aPixelsPtr == NULL)
    {
        RGB24_Pix_t * pixels_ptr = calloc(aStrideLng * aFrameHgt, 1);

        for (int offset = 0, row_idx = 0; row_idx < aFrameHgt; ++row_idx)
        {
            uint8_t  red = ((row_idx & 0x3) == 1) ? 255 : 0;
            uint8_t  grn = ((row_idx & 0x3) == 2) ? 255 : 0;
            uint8_t  blu = ((row_idx & 0x3) == 3) ? 255 : 0;

            for (int col_idx = 0; col_idx < aFrameWdt; ++col_idx, ++offset)
            {
                int group = col_idx % 10;
                pixels_ptr[offset].red   = (group ? red : 127);
                pixels_ptr[offset].green = (group ? grn : 127);
                pixels_ptr[offset].blue  = (group ? blu : 127);
            }
        }

        int result = do_save_RGB24_frame(aPathPtr, pixels_ptr, aStrideLng, aFrameWdt, aFrameHgt);

        free(pixels_ptr);

        return result;
    }

    rgb_image.width  = aFrameWdt;
    rgb_image.height = aFrameHgt;
    rgb_image.stride = aStrideLng;
    rgb_image.pixels = aPixelsPtr;

    int errors = do_save_png_image_to_file ( &rgb_image, aPathPtr );

    return errors;  // 0 is OK, else error
}


/*
* YUV encodings have Y,U=Cb,V=Cr samples stored in packed or planar formats. 
* YUV values can have the range [0,255] or limited to [16,235] & [16,240].
* In Packed formats byte-samples are packed into one array of macropixels.
* In Planar formats Y,U,V are in consecutive arrays (U,V are sub-sampled).
* More details are provided at "http://www.fourcc.org/yuv.php".
* 
* ITUR_Convert_YUV_to_RGB(int Y, int U, int V)
*      int32_t Cb = U - 128;
*      int32_t Cr = V - 128;
*      int32_t R  = Y + (1.4065 * Cb);
*      int32_t B  = Y + (1.7790 * Cr);  
*      int32_t G  = Y - (0.3455 * Cb) - (0.7169 * Cr);
*      if (R < 0) R=0; else if (R > 255) R = 255;
*      if (B < 0) B=0; else if (B > 255) B = 255;
*      if (G < 0) G=0; else if (G > 255) G = 255;
*      int32_t RGB = 0xFF000000 | (R << 16) | (G << 8) | B;
*/

#define _VERIFY_I420_ARGS_NO_NO_NO
#define _STRICT_I420_TYPE_

#define _RGB32_PIXEL_TYPE_NO_NO_NO
#define _RGB24_PIXEL_TYPE_

static char sz_YUV_decoder_error[299] = { '!', 0 };

static int32_t do_decode_YUV420_frame(void    *output_RGB_ptr, 
                                      uint8_t *YUV_source_ptr, 
                                      int32_t  numFrameCols, 
                                      int32_t  numFrameRows,
                                      int32_t  srcFrameType) 
{
#define CLAMP_8BITS(X) { if (X < 0) { X = 0; } else if (X > 255) { X = 255; } }
#define MAKE_ARGB_8888(pRGB,R,B,G)   ( ++pRGB, pRGB->red=(uint8_t)R, pRGB->blue=(uint8_t)B, pRGB->green=(uint8_t)G ) // pRGB->alpha=0xFF
#define MAKE_RGB24_888(pRGB,R,B,G)   ( ++pRGB, pRGB->red=(uint8_t)R, pRGB->blue=(uint8_t)B, pRGB->green=(uint8_t)G )

#ifdef _RGB24_PIXEL_TYPE_
    #define MAKE_RGB_PIXEL(pRGB,R,G,B)   ( MAKE_RGB24_888(pRGB,R,B,G) )
    RGB24_Pix_t* ptr_RGB_pixel = ((RGB24_Pix_t*) output_RGB_ptr) - 1;
#endif

#ifdef _RGB32_PIXEL_TYPE_
    #define MAKE_RGB_PIXEL(pRGB,R,G,B)   ( MAKE_ARGB_8888(pRGB,R,B,G) )
    RGB32_Pix_t* ptr_RGB_pixel = ((RGB32_Pix_t*) output_RGB_ptr) - 1;
#endif

    int32_t     num_frame_cols = numFrameCols;
    int32_t     num_frame_rows = numFrameRows;
    int32_t     the_frame_size = num_frame_cols * num_frame_rows;
    int32_t     quarter_frame = the_frame_size >> 2;    
    int32_t     half_row_size = num_frame_cols >> 1;

    int32_t     col_index, row_index, R, G, B, Y1, Y2, U, V, D, E, F;

    uint8_t    *ptr_Y_encoding = YUV_source_ptr - 1;
    uint8_t    *end_Y_encoding = ptr_Y_encoding + the_frame_size;
    uint8_t    *ptr_U_encoding = NULL;    
    uint8_t    *ptr_V_encoding = NULL;

    *sz_YUV_decoder_error = 0;    


#ifdef _VERIFY_I420_ARGS_
    if ( (num_frame_cols < 0) || (num_frame_cols & 1) || (srcFrameType < 1) ||
         (num_frame_rows < 0) || (num_frame_rows & 1) || (srcFrameType > 4) )
    {        
        sprintf (sz_YUV_decoder_error, "invalid arguments");
        return -1;        
    }
#endif

#ifdef _STRICT_I420_TYPE_
    if (srcFrameType != 1)
    {
        sprintf (sz_YUV_decoder_error, "expecting I420_IYUV as Frame Type");
        return -2;        
    }
#endif

    for ( row_index = 0;  row_index < num_frame_rows;  ++row_index )
    {
        ptr_U_encoding = end_Y_encoding + ( (row_index >> 1) * half_row_size );
        ptr_V_encoding = ptr_U_encoding + quarter_frame;

#ifndef _STRICT_I420_TYPE_
        if (srcFrameType == 2) // "I420_YV12"
        {
            ptr_V_encoding  = ptr_U_encoding;
            ptr_U_encoding += quarter_frame;
        }
        else if (srcFrameType == 3) // "I420_NV12"
        {
            ptr_V_encoding = ptr_U_encoding + 1;
        }
        else if (srcFrameType == 4) // "I420_NV21"
        {
            ptr_V_encoding  = ptr_U_encoding;
            ptr_U_encoding += 1;
        }
        else if (srcFrameType != 1) // "I420_IYUV"
        {
            sprintf (sz_YUV_decoder_error, "invalid Frame Type");
            return -3;        
        }
#endif

        for ( col_index = 0;  col_index < num_frame_cols;  col_index += 2 )
        {
            Y1 = ( 0xFF & (int32_t) *(++ptr_Y_encoding) );
            Y2 = ( 0xFF & (int32_t) *(++ptr_Y_encoding) );

            U = ( 0xFF & (int32_t) *(++ptr_U_encoding) ) - 128;
            V = ( 0xFF & (int32_t) *(++ptr_V_encoding) ) - 128;

            D = ( ((88 * U) + (184 * V)) >> 8 );    // approx. ((0.3455 * U) + (0.7169 * V))
            E = ( (455 * U) >> 8 );                 // approx. (1.7790 * U)
            F = ( (360 * V) >> 8 );                 // approx. (1.4065 * V)

            G = Y1 - D;      CLAMP_8BITS( G );
            B = Y1 + E;      CLAMP_8BITS( B );
            R = Y1 + F;      CLAMP_8BITS( R );

            MAKE_RGB_PIXEL(ptr_RGB_pixel, R, G, B);

            G = Y2 - D;      CLAMP_8BITS( G );
            B = Y2 + E;      CLAMP_8BITS( B );
            R = Y2 + F;      CLAMP_8BITS( R );

            MAKE_RGB_PIXEL(ptr_RGB_pixel, R, G, B);
        }
    }

    return the_frame_size;
}


int save_image_frame_as_PNG_file(const char * aPathPtr,
                                 const char * aFormatPtr,
                                 void       * aPixmapPtr,
                                 int          aPixmapLng,
                                 int          aStrideLng,
                                 int          aFrameWdt,
                                 int          aFrameHgt)
{
    int num_frame_pixels = (aFrameHgt < 1) ? 0 : (aFrameHgt * aFrameWdt);

    int  num_pixel_bytes = (aFrameWdt < 1) ? 0 : (aStrideLng / aFrameWdt);

    if ( (aPathPtr == NULL)     || 
         (aFormatPtr == NULL)   || 
         (num_pixel_bytes < 1)  || 
         (num_pixel_bytes > 4)  ||
         (num_frame_pixels < 0) )
    {
        return -10;
    }

    if ( strstr(aPathPtr, ".RAW.") != NULL )
    {
        FILE * fp = (aPixmapPtr == NULL) ? NULL : fopen (aPathPtr, "wb");

        if (fp != NULL) 
        {
            fwrite(aPixmapPtr, aPixmapLng, 1, fp);
            fclose(fp);
        }

        return (fp ? 0 : -20);
    }


    if ( stricmp(aFormatPtr, "RGB") == 0 )
    {
        int result = -1;

        if (num_pixel_bytes == 3)       // RGB_24
        {
            result = do_save_RGB24_frame(aPathPtr, aPixmapPtr, aStrideLng, aFrameWdt, aFrameHgt);
        }
        else if (num_pixel_bytes == 3)  // RGB_32
        {
            ;   // TODO --- MAYBE
        }
        else if (num_pixel_bytes == 2)  // RGB_16
        {
            ;   // TODO --- MAYBE
        }

        return (result ? -30 : 0);
    }

    if (stricmp(aFormatPtr, "I420") == 0)   // YUV
    {
        RGB24_Pix_t * ptr_RGB24_pixels = malloc(sizeof(RGB24_Pix_t) * num_frame_pixels);

        int result = do_decode_YUV420_frame(ptr_RGB24_pixels, aPixmapPtr, aFrameWdt, aFrameHgt, 1);

        if (result == num_frame_pixels)
        {
            result = do_save_RGB24_frame(aPathPtr, ptr_RGB24_pixels, aFrameWdt * 3, aFrameWdt, aFrameHgt);
        }

        free(ptr_RGB24_pixels);

        return (result ? -60 : 0);
    }

    return -100;
}
