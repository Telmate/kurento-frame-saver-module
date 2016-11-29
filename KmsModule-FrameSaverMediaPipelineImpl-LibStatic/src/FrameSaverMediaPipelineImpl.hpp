/*
 * =================================================================================================
 * File:        FrameSaverMediaPipelineImpl.hpp
 *
 * History:     1. 2016-11-16   JBendor     Created as a class derived from MediaPipelineImpl
 *              2. 2016-11-28   JBendor     Updated
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


#ifdef THE_CLASS_NAME
    #undef THE_CLASS_NAME
#endif

#define THE_CLASS_NAME "FrameSaverMediaPipelineImpl"


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

    FrameSaverMediaPipelineImpl (const boost::property_tree::ptree & cfg);  // c'tor same as base class

    FrameSaverMediaPipelineImpl ();                                         // default c'tor

    virtual ~FrameSaverMediaPipelineImpl();                                 // virtual d'tor

    static std::string getClassName();                                      // the class name

    static FrameSaverMediaPipelineImpl * getLiveInstancePtr();              // the single instance

    GstElement * getPipeline();                                             // overrides base class

    bool startPipelinePlaying();                                            // sets state to PLAYING

    bool stopPipelinePlaying();                                             // changes PLAYING to READY

    bool getFrameSaverName(std::string aElementName);                       // returns FALSE on failure

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

