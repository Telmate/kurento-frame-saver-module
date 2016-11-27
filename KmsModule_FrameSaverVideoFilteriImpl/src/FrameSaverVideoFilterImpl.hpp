/*
 * =================================================================================================
 * File:        FrameSaverVideoFilterImpl.hpp
 *
 * History:     1. 2016-11-25   JBendor     Created as a class derived from kurento::FilterImpl
 *              2. 2016-11-27   JBendor     Updated
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * =================================================================================================
 */

#ifndef __FRAME_SAVER_VIDEO_FILTER_IMPL_HPP__
#define __FRAME_SAVER_VIDEO_FILTER_IMPL_HPP__


#include "FilterImpl.hpp"
#include "MediaPipeline.hpp"
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
    namespace module
    {
        namespace kms_frame_saver_video_filter
        {
            class FrameSaverVideoFilterImpl;
        }
    }
}


namespace kurento
{
    // This function is called by an auto-generated function
    void Serialize (std::shared_ptr<kurento::module::kms_frame_saver_video_filter::FrameSaverVideoFilterImpl> & object, JsonSerializer & serializer);
}


namespace kurento
{
    class MediaPipelineImpl;
}


namespace kurento
{

namespace module
{

namespace kms_frame_saver_video_filter
{

class FrameSaverVideoFilterImpl : public FilterImpl
{

public:

    static FrameSaverVideoFilterImpl * getFirstInstancePtr();               // the first instance

    FrameSaverVideoFilterImpl (const boost::property_tree::ptree & ref_cfg, std::shared_ptr<MediaPipeline> ptr_parent);

    virtual ~FrameSaverVideoFilterImpl();                                   // virtual d'tor

    bool setPipelinePlayState(bool isEnabled);                              // sets state READY or PLAY

    bool addFrameSaver(const std::string aLink, const std::string aPads);   // returns FALSE on failure

    bool getParamsList(std::string aCurrentParamsSeparatedByTabs);          // returns FALSE on failure

    bool getParam(const std::string aName, std::string aPresentValue);      // returns FALSE on failure

    bool setParam(const std::string aName, const std::string aNewVal);      // returns FALSE on failure

    /* Next methods are automatically implemented by code generator */
    virtual bool connect (const std::string &eventType,  std::shared_ptr<EventHandler> handler);
    virtual void invoke (std::shared_ptr<MediaObjectImpl> obj, const std::string &methodName, const Json::Value &params, Json::Value &response);
    virtual void Serialize (JsonSerializer &serializer);

protected:
    virtual void postConstructor ();

    virtual bool initializeInstance(bool isNewInstance);

    virtual bool releaseResources(bool isDelete);

private:
    std::recursive_mutex    mRecursiveMutex;

    class StaticConstructor
    {
        public:  StaticConstructor();
    };

    static StaticConstructor  Private_Static_Constructor;

};


} // ends namespace: kms_frame_saver_video_filter

} // ends namespace: module

} // ends namespace: kurento

#endif //  __FRAME_SAVER_VIDEO_FILTER_IMPL_HPP__

