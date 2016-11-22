/* Autogenerated with kurento-module-creator */

#ifndef __FRAME_SAVER_MEDIA_PIPELINE_IMPL_HPP__
#define __FRAME_SAVER_MEDIA_PIPELINE_IMPL_HPP__

#include "FilterImpl.hpp"
#include "FrameSaverMediaPipeline.hpp"
#include <EventHandler.hpp>
#include <boost/property_tree/ptree.hpp>

namespace kurento
{
namespace module
{
namespace FrameSaverMediaPipeline
{
class FrameSaverMediaPipelineImpl;
} /* FrameSaverMediaPipeline */
} /* module */
} /* kurento */

namespace kurento
{
void Serialize (std::shared_ptr<kurento::module::FrameSaverMediaPipeline::FrameSaverMediaPipelineImpl> &object, JsonSerializer &serializer);
} /* kurento */

namespace kurento
{
class MediaPipelineImpl;
} /* kurento */

namespace kurento
{
namespace module
{
namespace FrameSaverMediaPipeline
{

class FrameSaverMediaPipelineImpl : public FilterImpl, public virtual FrameSaverMediaPipeline
{

public:

  FrameSaverMediaPipelineImpl (const boost::property_tree::ptree &config, std::shared_ptr<MediaPipeline> FrameSaverMediaPipeline);

  virtual ~FrameSaverMediaPipelineImpl () {};

  /* Next methods are automatically implemented by code generator */
  virtual bool connect (const std::string &eventType, std::shared_ptr<EventHandler> handler);
  virtual void invoke (std::shared_ptr<MediaObjectImpl> obj,
                       const std::string &methodName, const Json::Value &params,
                       Json::Value &response);

  virtual void Serialize (JsonSerializer &serializer);

private:

  class StaticConstructor
  {
  public:
    StaticConstructor();
  };

  static StaticConstructor staticConstructor;

};

} /* FrameSaverMediaPipeline */
} /* module */
} /* kurento */

#endif /*  __FRAME_SAVER_MEDIA_PIPELINE_IMPL_HPP__ */
