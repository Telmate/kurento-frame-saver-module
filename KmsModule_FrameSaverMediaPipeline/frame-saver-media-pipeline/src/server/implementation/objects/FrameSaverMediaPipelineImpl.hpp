/*
 * =================================================================================================
 * File:        FrameSaverMediaPipelineImpl.hpp
 *
 * History:     1. 2016-11-16   JBendor     Created as a class derived from MediaPipelineImpl
 *              2. 2016-11-24   JBendor     Updated
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * =================================================================================================
 */

#ifndef __FRAME_SAVER_MEDIA_PIPELINE_IMPL_HPP__
#define __FRAME_SAVER_MEDIA_PIPELINE_IMPL_HPP__

#include "MediaObjectImpl.hpp"
#include "MediaPipelineImpl.hpp"

#include <boost/property_tree/ptree.hpp>    // also in "MediaObjectImpl.hpp"
#include <jsonrpc/JsonSerializer.hpp>       // also in "MediaObjectImpl.hpp"
#include <KurentoException.hpp>             // also in "MediaObjectImpl.hpp"
#include <EventHandler.hpp>                 // also in "MediaObjectImpl.hpp"
#include <mutex>                            // also in "MediaObjectImpl.hpp"
#include <gst/gst.h>                        // also in "MediaObjectImpl.hpp"


namespace kurento
{

class JsonSerializer;

namespace module
{

namespace FrameSaverMediaPipeline
{

class FrameSaverMediaPipelineImpl;

// This function is called by an auto-generated function
void Serialize (std::shared_ptr<FrameSaverMediaPipelineImpl> & ref_object, JsonSerializer & ref_serializer);


class FrameSaverMediaPipelineImpl: public MediaPipelineImpl
{
public:

    static FrameSaverMediaPipelineImpl * getLiveInstancePtr();              // one instance

    virtual ~FrameSaverMediaPipelineImpl();                                 // virtual d'tor

    FrameSaverMediaPipelineImpl ();                                         // default c'tor

    FrameSaverMediaPipelineImpl (const boost::property_tree::ptree & cfg);  // c'tor same as base class

    GstElement * getPipeline() { return mPipelinePtr; }                     // overrides the base class

    bool setPipelinePlayState(bool isEnabled);                              // sets state READY or PLAY

    bool hasFrameSaver() { return (mFrameSaverPluginPtr != NULL); }         // returns TRUE when exists

    bool addFrameSaver(const std::string aLink, const std::string aPads);   // returns FALSE on failure

    bool getParamsList(std::string aCurrentParamsSeparatedByTabs);          // returns FALSE on failure

    bool getParam(const std::string aName, std::string aPresentValue);      // returns FALSE on failure

    bool setParam(const std::string aName, const std::string aNewVal);      // returns FALSE on failure

    // The Next Three methods are automatically implemented by code generator
    virtual void invoke (std::shared_ptr<MediaObjectImpl> obj, const std::string &methodName, const Json::Value &params, Json::Value &response);
    virtual bool connect (const std::string &eventType, std::shared_ptr<EventHandler> handler);
    virtual void Serialize (JsonSerializer & ref_serializer);

protected:

    virtual void postConstructor();

    virtual bool initializeInstance(bool isNewInstance);

    virtual bool releaseResources(bool isDelete);

private:
    GstElement             *mFrameSaverPluginPtr, *mPipelinePtr;
    std::recursive_mutex    mRecursiveMutex;

    class StaticConstructor
    {
        public:  StaticConstructor();
    };

    static StaticConstructor  Private_Static_Constructor;
};

} // ends namespace FrameSaverMediaPipeline

} // ends namespace module

} // ends namespace kurento 


#endif /*  __FRAME_SAVER_MEDIA_PIPELINE_IMPL_HPP__ */
