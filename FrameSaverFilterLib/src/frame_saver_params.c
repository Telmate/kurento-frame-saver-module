/* 
 * ======================================================================================
 * File:        frame_saver_params.c
 * 
 * History:     1. 2016-10-29   JBendor     Created
 *              2. 2016-11-02   JBendor     Updated
 *
 * Description: implements parameters used by the Frame_Saver_Filter
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * ======================================================================================
 */

#include "frame_saver_params.h"


#if defined _CYGWIN_X64

int realpath( const char *aPathPtr, char * aBuffPtr )
{
    int lng = (int) strlen( GET_CWD(aBuffPtr, PATH_MAX) );

    if (aPathPtr[0] == PATH_DELIMITER)
    {
        return sprintf(aBuffPtr, "%s", aPathPtr);
    }

    while ((lng > 0) && (aPathPtr[0] == '.') && (aPathPtr[1] == '.') && (aPathPtr[2] == PATH_DELIMITER))
    {
        while ((--lng > 0) && (aBuffPtr[lng] != PATH_DELIMITER))
        {
            ; // next step back
        }
        aPathPtr += 3;
    }

    if ((aPathPtr[0] == '.') && (aPathPtr[1] == PATH_DELIMITER))
    {
        return lng + sprintf(aBuffPtr+lng, "%s", ++aPathPtr);
    }

    return lng + sprintf(aBuffPtr+lng, "%s", aPathPtr);
}


int strcasecmp( const char *aStrPtr, const char * aPatPtr )
{
    int diff = 0;
    while ( (diff == 0) && (*aStrPtr != 0) && (*aPatPtr != 0) )
    {
        diff = toupper(*aStrPtr++) - toupper(*aPatPtr++);
    }
    return diff;
}

#endif // _CYGWIN_X64


//=======================================================================================
// synopsis: count = do_trim_spaces(aTextPtr, aKeepOne)
//
// replaces repeated spaces with on or none --- returns trimmed length
//=======================================================================================
static int do_trim_spaces(char * aTextPtr, gboolean aKeepOne)
{
    #define QUOTE ('\"')
    int    dst_idx = -1;
    char a_context = ' ';
    char * src_ptr = aTextPtr - 1;

    while ( *(++src_ptr) != 0 )
    {
        if ( (a_context == QUOTE) || (*src_ptr == QUOTE) )
        {  
            aTextPtr[ ++dst_idx ] = *src_ptr;
            a_context = (*src_ptr == a_context) ? 0 : a_context;
        }
        else if ( ! isspace(*src_ptr) )
        {
            aTextPtr[ ++dst_idx ] = a_context = *src_ptr;
        }
        else if ( aKeepOne && (a_context != ' ') )
        {
            aTextPtr[ ++dst_idx ] = a_context = ' ';
        }
    }

    aTextPtr[ ++dst_idx ] = 0;

    return dst_idx ;
}


 //=======================================================================================
 // synopsis: count = do_tokenize_text(aTextPtr, aDelimiter, aMaxTokens, aLngArray)
 //
 // finds occurences of a delimiter in a string --- returns number of occurences
 //=======================================================================================
static int do_tokenize_text(const char * aTextPtr, char aDelimiter, int aMaxTokens, int* aLngArray)
{
    const char * prior_ptr = aTextPtr - 1;
    int          token_lng = aTextPtr ? 1 : -1;
    int          token_idx = -1;

    while ( (prior_ptr != NULL) && (token_lng > 0) && (++token_idx < aMaxTokens) )
    {
        const char * token_ptr = strchr(++prior_ptr, aDelimiter);

        token_lng = (int) (token_ptr ? (token_ptr - prior_ptr) : strlen(prior_ptr) );

        aLngArray[ token_idx ] = token_lng;

        prior_ptr = token_ptr;
    }

    return (token_idx + 1);
}


//=======================================================================================
// synopsis: count = do_read_params_file(aAbdPathPtr, aBufferLng, aBufferPtr, aMaxParams, aParamsArray)
//
// reads parameters from ascii file --- returns number of params added to aParamsArray
//=======================================================================================
static int  do_read_params_file(const char * aAbsPathPtr,
                                int          aBufferLng,
                                char       * aBufferPtr,
                                int          aMaxParams,
                                char      ** aParamsArray)
{
    int params_idx = -1;

    char last_symb = 0;

    gboolean is_ok = (aMaxParams > 1) && aAbsPathPtr && aBufferPtr && aParamsArray;

    FILE *file_ptr = is_ok ? fopen(aAbsPathPtr, "r") : NULL;

    if (! file_ptr)
    {
        return -1;  // invalid conditions --- or failed to open file
    }

    while ( (aBufferLng > 9) && (fgets(aBufferPtr, aBufferLng, file_ptr) != NULL) )
    {
        int  length = (int) strlen(aBufferPtr);

        while ( (length > 0) && (aBufferPtr[length - 1] <= ' ') )
        {
            aBufferPtr[ --length ] = 0;     // remove trailing white-space
        }

        // continuation lines follow lines which ended with backslash
        if ( last_symb == '\\' )
        {
            last_symb = aBufferPtr[length - 1];

            if (last_symb == '\\')
            {
                aBufferPtr[ --length ] = 0;
            }
            else
            {
                ++length;
            }

            aBufferLng -= length;

            aBufferPtr += length;

            continue;
        }

        // lines which define a parameters must start with a letter
        if ( isupper(*aBufferPtr) || islower(*aBufferPtr) )
        {
            if ( --aMaxParams <= 1 )
            {
                break;
            }

            params_idx += 1;

            aParamsArray[ params_idx ] = aBufferPtr;

            last_symb = aBufferPtr[length - 1];

            if (last_symb == '\\')
            {
                aBufferPtr[ --length ] = 0;
            }
            else
            {
                ++length;
            }

            aBufferLng -= length;

            aBufferPtr += length;

            continue;
        }

        // disable continuation --- and proceed to read another line
        *aBufferPtr = last_symb = 0;
    }

    fclose(file_ptr);

    aParamsArray[ ++params_idx ] = NULL;

    return (params_idx);
}


//=======================================================================================
// synopsis: result = pipeline_params_parse_caps(aCapsPtr,aFmtPtr,aWdtPtr,aHgtPtr,aBitsPtr)
//
// parse caps string --- returns 0 on success, else error
//=======================================================================================
gint pipeline_params_parse_caps(const char * aCapsPtr, 
                                gchar * aFormatPtr, 
                                gint  * aNumColsPtr, 
                                gint  * aNumRowsPtr,
                                gint  * aNumBitsPtr)
{
    gint errors = 0;

    const char * ptr_field = strstr(aCapsPtr, "format=(string)");

    if (ptr_field != NULL)
    {
        for (ptr_field += 15;  (*ptr_field > ' ') && (*ptr_field != ',');  ++ptr_field)
        {
            *aFormatPtr++ = *ptr_field;
        }
    }
    *aFormatPtr = 0;

    ptr_field = strstr(aCapsPtr, "height=(int)");

    if (ptr_field != NULL)
    {
        errors += (sscanf(ptr_field + 12, "%d", aNumRowsPtr) == 1) ? 0 : 0x1;
    }

    ptr_field = strstr(aCapsPtr, "width=(int)");

    if (ptr_field != NULL)
    {
        errors += (sscanf(ptr_field + 11, "%d", aNumColsPtr) == 1) ? 0 : 0x2;
    }

    ptr_field = strstr(aCapsPtr, "depth=(int)");    // aka "bit-per-pixel"

    if (ptr_field != NULL)
    {
        errors += (sscanf(ptr_field + 11, "%d", aNumBitsPtr) == 1) ? 0 : 0x4;
    }

    ptr_field = strstr(aCapsPtr, "bpp=(int)");      // alias for "depth"

    if (ptr_field != NULL)
    {
        errors += (sscanf(ptr_field + 9, "%d", aNumBitsPtr) == 1) ? 0 : 0x8;
    }

    return errors;
}


//=======================================================================================
// synopsis: is_ok = pipeline_params_parse_one(aSpecsPtr, aParamsPtr)
//
// parses one pipeline parameter for the frame_saver_filter --- returns TRUE for success
//=======================================================================================
gboolean pipeline_params_parse_one(const char * aSpecsPtr, SplicerParams_t * aParamsPtr)
{
    gboolean is_ok = (aSpecsPtr != NULL);

    if (! is_ok)
    {
        return FALSE;
    }

    if ( strncmp(aSpecsPtr, "poke=", 5) == 0 )
    {
        int lengths[4] = { 0, 0, 0, 0 };

        int num_tokens = do_tokenize_text(&aSpecsPtr[5], ',', 4, lengths);

        is_ok = (num_tokens == 3) && 
                (lengths[0] >= 1) && (lengths[0] <= MAX_ELEMENT_NAME_LNG) &&
                (lengths[1] >= 1) && (lengths[1] <= MAX_ELEMENT_NAME_LNG) &&                   
                (lengths[2] >= 1) && (lengths[2] <= MAX_ELEMENT_NAME_LNG);

        if (is_ok)
        {
            const char *token_ptr = &aSpecsPtr[5];
            int         token_lng = lengths[0];
            
            strncpy( aParamsPtr->pipeline_name, token_ptr, token_lng);
            aParamsPtr->pipeline_name[token_lng] = 0;

            token_ptr += token_lng;
            token_lng = lengths[1];

            strncpy( aParamsPtr->producer_name, ++token_ptr, token_lng );
            aParamsPtr->producer_name[token_lng] = 0;

            token_ptr += token_lng;
            token_lng = lengths[2];
            
            strncpy( aParamsPtr->consumer_name, ++token_ptr, token_lng );
            aParamsPtr->consumer_name[token_lng] = 0;

            do_trim_spaces(aParamsPtr->pipeline_name, FALSE);
            do_trim_spaces(aParamsPtr->producer_name, FALSE);
            do_trim_spaces(aParamsPtr->consumer_name, FALSE);
        }

        return is_ok;
    }

    if ( strncmp(aSpecsPtr, "pads=", 5) == 0 )
    {
        int lengths[4] = { 0, 0, 0, 0 };

        int num_tokens = do_tokenize_text(&aSpecsPtr[5], ',', 4, lengths);

        is_ok = (num_tokens == 3) && 
                (lengths[0] >= 1) && (lengths[0] <= MAX_PAD_NAME_LNG) &&
                (lengths[1] >= 1) && (lengths[1] <= MAX_PAD_NAME_LNG) &&                   
                (lengths[2] >= 1) && (lengths[2] <= MAX_PAD_NAME_LNG);

        if (is_ok)
        {
            const char *token_ptr = &aSpecsPtr[5];
            int         token_lng = lengths[0];

            strncpy( aParamsPtr->producer_out_pad_name, token_ptr, token_lng );
            aParamsPtr->producer_out_pad_name[token_lng] = 0;

            token_ptr += token_lng;
            token_lng = lengths[1];

            strncpy( aParamsPtr->consumer_inp_pad_name, ++token_ptr, token_lng );
            aParamsPtr->consumer_inp_pad_name[token_lng] = 0;

            token_ptr += token_lng;
            token_lng = lengths[2];
            
            strncpy( aParamsPtr->consumer_out_pad_name, ++token_ptr, token_lng );
            aParamsPtr->consumer_out_pad_name[token_lng] = 0;

            do_trim_spaces(aParamsPtr->producer_out_pad_name, FALSE);
            do_trim_spaces(aParamsPtr->consumer_inp_pad_name, FALSE);
            do_trim_spaces(aParamsPtr->consumer_out_pad_name, FALSE);
        }

        return is_ok;
    }

    if ( strncmp(aSpecsPtr, "pipe=", 5) == 0 )
    {
        is_ok = (strchr(aSpecsPtr, '!') != NULL);

        if (is_ok)
        {
            strcpy(aParamsPtr->pipeline_spec, &aSpecsPtr[5]);

            do_trim_spaces(aParamsPtr->pipeline_spec, TRUE);
        }

        return is_ok;
    }

    return FALSE;
}


//=======================================================================================
// synopsis: is_ok = frame_saver_params_report_all(aParamsPtr, aOutFilePtr)
//
// reports the parameters for the frame_saver_filter --- returns TRUE for success
//=======================================================================================
gboolean frame_saver_params_report_all(SplicerParams_t * aParamsPtr, FILE * aOutFilePtr)
{
    char * bangs_ptr = strchr(aParamsPtr->pipeline_spec, '!');

    if (bangs_ptr == NULL)
    {
        sprintf(aParamsPtr->pipeline_spec, "%s", "auto");
    }

    if ((*aParamsPtr->pipeline_name == 0) || 
        (strcmp(aParamsPtr->pipeline_name, "auto") == 0))
    {
        sprintf(aParamsPtr->pipeline_name, "%s", DEFAULT_PIPELINE_NAME);
    }

    if ((*aParamsPtr->producer_name == 0) || 
        (strcmp(aParamsPtr->producer_name, "auto") == 0))
    {
        sprintf(aParamsPtr->producer_name, "%s", DEFAULT_VID_SOURCE_NAME);
    }

    if ((*aParamsPtr->consumer_name == 0) || 
        (strcmp(aParamsPtr->consumer_name, "auto") == 0))
    {
        sprintf(aParamsPtr->consumer_name, "%s", DEFAULT_VID_CVT_NAME);
    }

    if ((*aParamsPtr->producer_out_pad_name == 0) || 
        (strcmp(aParamsPtr->producer_out_pad_name, "auto") == 0))
    {
        sprintf(aParamsPtr->producer_out_pad_name, "%s", "src");
    }

    if ((*aParamsPtr->consumer_inp_pad_name == 0) || 
        (strcmp(aParamsPtr->consumer_inp_pad_name, "auto") == 0))
    {
        sprintf(aParamsPtr->consumer_inp_pad_name, "%s", "sink");
    }

    if ((*aParamsPtr->consumer_out_pad_name == 0) || 
        (strcmp(aParamsPtr->consumer_out_pad_name, "auto") == 0))
    {
        sprintf(aParamsPtr->consumer_out_pad_name, "%s", "src");
    }

    fprintf(aOutFilePtr,
            "\nPARAMETERS: %s=%d %s=%d %s=%d %s=%d %s=(%s) %s=(%s) %s=(%s,%s,%s) %s=(%s,%s,%s) \n\n",
            "\n          tick", aParamsPtr->one_tick_ms,
            "\n          snap", aParamsPtr->one_snap_ms,
            "\n          spin", aParamsPtr->max_spin_ms,
            "\n          play", aParamsPtr->max_play_ms,
            "\n          path", aParamsPtr->folder_path,
            "\n          pipe", bangs_ptr ? "custom-pipeline" : "default-pipeline",
            "\n          poke", aParamsPtr->pipeline_name,
                                aParamsPtr->producer_name, 
                                aParamsPtr->consumer_name,
            "\n          pads", aParamsPtr->producer_out_pad_name, 
                                aParamsPtr->consumer_inp_pad_name, 
                                aParamsPtr->consumer_out_pad_name);

    if (bangs_ptr != NULL)
    {
        char pipe_specs[sizeof(aParamsPtr->pipeline_spec)];
        char* specs_ptr = pipe_specs;

        strcpy(specs_ptr, aParamsPtr->pipeline_spec);
        bangs_ptr = strchr(specs_ptr, '!');

        *bangs_ptr = 0;
        fprintf(aOutFilePtr, "PIPELINE: %s ! \n", specs_ptr);

        for ( ; ; )
        {
            specs_ptr = bangs_ptr + 1;
            bangs_ptr = strchr(specs_ptr, '!');
            if (bangs_ptr != NULL)
            {
                *bangs_ptr = 0;
                fprintf(aOutFilePtr, ".........%s ! \n", specs_ptr);            
                continue;
            }
            break;
        }

        fprintf(aOutFilePtr, ".........%s \n\n", specs_ptr);
    }
    return TRUE;
}


//=======================================================================================
// synopsis: is_ok = frame_saver_params_initialize(aParamsPtr)
//
// sets default parameters for the frame_saver_filter --- returns TRUE for success
//=======================================================================================
gboolean frame_saver_params_initialize(SplicerParams_t * aParamsPtr)
{
    memset(aParamsPtr, 0, sizeof(*aParamsPtr));

    aParamsPtr->one_tick_ms = 1000;
    aParamsPtr->one_snap_ms = 2000;
    aParamsPtr->max_spin_ms = 4000;
    aParamsPtr->max_play_ms = 9000;

    GET_CWD(aParamsPtr->folder_path, sizeof(aParamsPtr->folder_path));

    return TRUE;
}


//=======================================================================================
// synopsis: is_ok = frame_saver_params_parse_many(argc, argv, aParamsPtr)
//
// parses parameters for the frame_saver_filter --- returns TRUE for success
//=======================================================================================
gboolean frame_saver_params_parse_many(int argc, char *argv[], SplicerParams_t * aParamsPtr)
{
    gboolean is_ok = TRUE;

    if (argc < 0)
    {
        return FALSE;
    }

    if ( (argc < 2) || (! argv) )
    {
        return TRUE;
    }

    while ( is_ok && (--argc > 0) )
    {
        const char * psz_param = argv[argc];

        if ( (psz_param == NULL) || (*psz_param == 0) )
        {
            continue;
        }

        if ( strncmp(psz_param, "tick=", 5) == 0 )
        {
            is_ok = (sscanf(&psz_param[5], "%d", &aParamsPtr->one_tick_ms) == 1) && (aParamsPtr->one_tick_ms >= 0);
            continue;
        }

        if ( strncmp(psz_param, "snap=", 5) == 0 )
        {
            is_ok = (sscanf(&psz_param[5], "%d", &aParamsPtr->one_snap_ms) == 1) && (aParamsPtr->one_snap_ms >= 0);
            continue;
        }

        if ( strncmp(psz_param, "spin=", 5) == 0 )
        {
            is_ok = (sscanf(&psz_param[5], "%d", &aParamsPtr->max_spin_ms) == 1) && (aParamsPtr->max_spin_ms >= 0);
            continue;
        }

        if ( strncmp(psz_param, "play=", 5) == 0 )
        {
            is_ok = (sscanf(&psz_param[5], "%d", &aParamsPtr->max_play_ms) == 1) && (aParamsPtr->max_play_ms >= 0);
            continue;
        }

        if ( strncmp(psz_param, "path=", 5) == 0 )
        {
            int lng = snprintf(aParamsPtr->folder_path, sizeof(aParamsPtr->folder_path), "%s", &psz_param[5]);

            is_ok = (lng > 3) && ((lng + 30) < sizeof(aParamsPtr->folder_path));

            continue;
        }

        // possibly --- parse parameters related to the pipeline
        if ( (strncmp(psz_param, "poke=", 5) == 0) ||
             (strncmp(psz_param, "pads=", 5) == 0) ||
             (strncmp(psz_param, "pipe=", 5) == 0) )
        {
            is_ok = pipeline_params_parse_one(psz_param, aParamsPtr);
            continue;
        }

        // possibly --- read and parse parameters from a designated file
        if ( strncmp(psz_param, "args=", 5) == 0 )
        {
            char  abs_path[PATH_MAX]; 
            char  params_ascii[4000];
            char* params_array[30] = { "", NULL };

            ABS_PATH( &psz_param[5], abs_path, PATH_MAX ); 

            int  max_params = (sizeof(params_array) / sizeof(char*));

            int  num_params = do_read_params_file(abs_path, 
                                                  sizeof(params_ascii),
                                                  params_ascii,
                                                  max_params - 1,
                                                  params_array + 1);

            // recursive call
            is_ok = (num_params > 0) &&
                    frame_saver_params_parse_many(num_params+1, params_array, aParamsPtr);

            continue;
        }

        g_print("WARNING:  Unknown Arg #%d: (%s) \n", argc, argv[argc]);

    } // ends while ( is_ok && (--argc > 0) )

    if (is_ok)
    {
        if (aParamsPtr->one_tick_ms < MIN_TICKS_MILLISEC)
        {
            g_print("minimum 'tick' time interval is %u miliseconds \n", MIN_TICKS_MILLISEC);
            is_ok = FALSE;
        }
        else if (aParamsPtr->max_play_ms < aParamsPtr->one_tick_ms)
        {
            g_print("minimum 'play' milliseconds is one 'tick' \n");      
            is_ok = FALSE;
        }
        else if (aParamsPtr->max_play_ms < aParamsPtr->one_snap_ms)
        {
            g_print("maximum 'snap' milliseconds is 'play' time \n");      
            is_ok = FALSE;
        }
    }
    else
    {
        g_print("ERROR: Invalid Arg #%d: (%s) \n", argc, argv[argc]);
    }

    return is_ok;
}
