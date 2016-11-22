/*
 * =================================================================================================
 * File:        FrameSaverMediaPipelineImpl.hpp
 *
 * History:     1. 2016-11-16   JBendor     Created
 *              2. 2016-11-21   JBendor     Updated
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * =================================================================================================
 */

#ifndef __FRAME_SAVER_MEDIA_PIPELINE_IMPL_HPP__
#define __FRAME_SAVER_MEDIA_PIPELINE_IMPL_HPP__

#include "MediaObjectImpl.hpp"
#include "MediaPipeline.hpp"
#include <EventHandler.hpp>
#include <gst/gst.h>
#include <boost/property_tree/ptree.hpp>


namespace kurento
{

#define STRING      std::string

class FrameSaverMediaPipelineImpl;


void Serialize(std::shared_ptr<FrameSaverMediaPipelineImpl> & ref_object_ptr,
               kurento::JsonSerializer                      & ref_serializer);


class FrameSaverMediaPipelineImpl: public MediaObjectImpl, public virtual MediaPipeline
{
public:

    FrameSaverMediaPipelineImpl(const boost::property_tree::ptree &config);

    virtual ~FrameSaverMediaPipelineImpl();

    bool hasFrameSaver() { return (mFrameSaverPluginPtr ? true : false); }

    bool addFrameSaver(const STRING aLink, const STRING aPads); // returns FALSE on failure

    bool getParamsList(STRING aCurrentParamsSeparatedByTabs);   // returns FALSE on failure

    bool getParam(const STRING aName, STRING aPresentValue);    // returns FALSE on failure

    bool setParam(const STRING aName, const STRING aNewVal);    // returns FALSE on failure

    bool setPipelinePlayState(bool isEnabled);                  // sets state READY or PLAY

    bool addElement (GstElement * aElementPtr);

    GstElement * getPipeline();

    virtual STRING getGstreamerDot ();

    virtual STRING getGstreamerDot (std::shared_ptr<GstreamerDotDetails>  ptr_details);

    virtual bool getLatencyStats ();

    virtual void setLatencyStats (bool newState);

    /* Next methods are automatically implemented by code generator */
    virtual bool connect (const STRING &eventType, std::shared_ptr<EventHandler> handler);

    virtual void invoke (std::shared_ptr<MediaObjectImpl>   ptr_obj,
                         const STRING                     & ref_methodName_ref, 
                         const Json::Value                & ref_params,
                         Json::Value                      & ref_response);

    virtual void Serialize (kurento::JsonSerializer & ref_serializer);

protected:

    virtual void postConstructor();

    virtual bool releaseResources(bool isDelete);

private:
    GstElement             *mFrameSaverPluginPtr, *mPipelinePtr;
    gulong                  mBusMessageHandler;
    bool                    mLatencyStatsFlag;
    std::recursive_mutex    mRecursiveMutex;

    void handleBusMessage (GstMessage * aMessagePtr);

    class StaticConstructor
    {
        public:  StaticConstructor();
    };

    static StaticConstructor  Private_Static_Constructor;
};


} /* ends namespace kurento */


#endif /*  __FRAME_SAVER_MEDIA_PIPELINE_IMPL_HPP__ */

// ends file: "FrameSaverMediaPipelineImpl.hpp"

