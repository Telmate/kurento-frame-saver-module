/* Autogenerated with kurento-module-creator */

#ifndef __FRAME_SAVER_MEDIA_PIPELINE_IMPL_FACTORY_HPP__
#define __FRAME_SAVER_MEDIA_PIPELINE_IMPL_FACTORY_HPP__

#include "FrameSaverMediaPipelineImpl.hpp"
#include "MediaPipelineImplFactory.hpp"
#include <Factory.hpp>
#include <MediaObjectImpl.hpp>
#include <boost/property_tree/ptree.hpp>

namespace kurento
{
namespace module
{
namespace FrameSaverMediaPipeline
{

class FrameSaverMediaPipelineImplFactory : public virtual MediaPipelineImplFactory
{
public:
  FrameSaverMediaPipelineImplFactory () {};

  virtual std::string getName () const {
    return "FrameSaverMediaPipeline";
  };

private:

  virtual MediaObjectImpl *createObjectPointer (const boost::property_tree::ptree &conf, const Json::Value &params) const;

  MediaObjectImpl *createObject (const boost::property_tree::ptree &conf) const;
};

} /* FrameSaverMediaPipeline */
} /* module */
} /* kurento */

#endif /*  __FRAME_SAVER_MEDIA_PIPELINE_IMPL_FACTORY_HPP__ */