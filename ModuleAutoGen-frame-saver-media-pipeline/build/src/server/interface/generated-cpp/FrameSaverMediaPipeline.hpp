/* Autogenerated with kurento-module-creator */

#ifndef __FRAME_SAVER_MEDIA_PIPELINE_HPP__
#define __FRAME_SAVER_MEDIA_PIPELINE_HPP__

#include <json/json.h>
#include <jsonrpc/JsonRpcException.hpp>
#include <memory>
#include <vector>
#include "MediaPipeline.hpp"

namespace kurento
{
namespace module
{
namespace FrameSaverMediaPipeline
{
class FrameSaverMediaPipeline;
} /* FrameSaverMediaPipeline */
} /* module */
} /* kurento */

namespace kurento
{
class JsonSerializer;
void Serialize (std::shared_ptr<kurento::module::FrameSaverMediaPipeline::FrameSaverMediaPipeline> &object, JsonSerializer &serializer);
}

namespace kurento
{
namespace module
{
namespace FrameSaverMediaPipeline
{
class FrameSaverMediaPipeline : public virtual MediaPipeline
{

public:
  FrameSaverMediaPipeline () {};
  virtual ~FrameSaverMediaPipeline () {};

  virtual const std::string& getType () const;
  virtual const std::string& getQualifiedType () const;
  virtual const std::string& getModule () const;
  virtual const std::vector<std::string>& getHierarchy () const;

  virtual void Serialize (JsonSerializer &serializer) = 0;

};

} /* FrameSaverMediaPipeline */
} /* module */
} /* kurento */

#endif /*  __FRAME_SAVER_MEDIA_PIPELINE_HPP__ */