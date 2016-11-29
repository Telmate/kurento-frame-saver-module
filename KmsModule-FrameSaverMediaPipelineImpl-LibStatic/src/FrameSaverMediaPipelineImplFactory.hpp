/*
 * =================================================================================================
 * File:        FrameSaverMediaPipelineImplFactory.hpp
 *
 * History:     1. 2016-11-23   JBendor     Created
 *              2. 2016-11-28   JBendor     Updated
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * =================================================================================================
 */

#ifndef __FRAME_SAVER_MEDIA_PIPELINE_IMPL_FACTORY_HPP__
#define __FRAME_SAVER_MEDIA_PIPELINE_IMPL_FACTORY_HPP__

#include "FrameSaverMediaPipelineImpl.hpp"
#include "MediaPipelineImplFactory.hpp"
#include <Factory.hpp>


namespace kurento
{

namespace module
{

namespace FrameSaverMediaPipeline
{

class FrameSaverMediaPipelineImplFactory : public virtual MediaPipelineImplFactory
{
public:
    FrameSaverMediaPipelineImplFactory () {};    // default empty c'tor

    virtual std::string getName () const { return FrameSaverMediaPipelineImpl::getClassName(); ; };

private:
    // The (factory) function "createObject" is implemeted in file "FrameSaverMediaPipelineImpl.cpp"
    MediaObjectImpl *createObject (const boost::property_tree::ptree & ref_config) const;

    // The (factory) function "createObjectPointer" is implementd by auto-generated code
    virtual MediaObjectImpl *createObjectPointer (const boost::property_tree::ptree & ref_config,
                                                  const Json::Value                 & ref_params) const;

};


} // ends namespace FrameSaverMediaPipeline

} // ends namespace module

} // ends namespace kurento


#endif /*  __FRAME_SAVER_MEDIA_PIPELINE_IMPL_FACTORY_HPP__ */

