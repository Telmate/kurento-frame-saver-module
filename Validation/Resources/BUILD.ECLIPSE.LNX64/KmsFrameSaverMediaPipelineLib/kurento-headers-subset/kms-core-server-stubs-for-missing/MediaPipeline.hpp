// MediaPipeline.hpp

#ifndef _MEDIA_PIPELINE_HPP_

#define _MEDIA_PIPELINE_HPP_

#include "MediaObject.hpp"


class MediaPipeline
{
    public:    MediaPipeline(){}  ;
};


typedef enum 
{
    SHOW_ALL,
    SHOW_CAPS_DETAILS,
    SHOW_FULL_PARAMS,
    SHOW_MEDIA_TYPE,
    SHOW_NON_DEFAULT_PARAMS,
    SHOW_STATES,
    SHOW_VERBOSE 
} GstreamerDotDetails;


#endif
