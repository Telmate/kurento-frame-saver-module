/*
 * =================================================================================================
 * File:        FrameSaverVideoFilterImplFactory.hpp
 *
 * History:     1. 2016-11-25   JBendor     Created
 *              2. 2016-11-28   JBendor     Updated
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * =================================================================================================
 */

#ifndef __FRAME_SAVER_VIDEO_FILTER_IMPL_FACTORY_HPP__
#define __FRAME_SAVER_VIDEO_FILTER_IMPL_FACTORY_HPP__

#include <FrameSaverVideoFilterImpl.hpp>
#include "FilterImplFactory.hpp"


namespace kurento
{

namespace module
{

namespace kms_frame_saver_video_filter
{

class FrameSaverVideoFilterImplFactory : public virtual FilterImplFactory
{
public:
    FrameSaverVideoFilterImplFactory () {};    // default empty c'tor

    virtual std::string getName () const { return FrameSaverVideoFilterImpl::getClassName(); };

private:
    // The (factory) function "createObject" is implemeted in file "FrameSaverVideoFilterImpl.cpp"
    MediaObjectImpl * createObject (const boost::property_tree::ptree & config, std::shared_ptr<MediaPipeline> parent) const;

    // The (factory) function "createObjectPointer" is implementd by auto-generated code
    virtual MediaObjectImpl *createObjectPointer (const boost::property_tree::ptree & ref_config,
                                                  const Json::Value                 & ref_params) const;

};


} // ends namespace: frame_saver_video_filter

} // ends namespace: module

} // ends namespace: kurento


#endif /*  __FRAME_SAVER_VIDEO_FILTER_IMPL_FACTORY_HPP__ */

