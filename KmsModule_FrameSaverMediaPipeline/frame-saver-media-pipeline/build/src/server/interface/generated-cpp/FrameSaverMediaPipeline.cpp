/* Autogenerated with kurento-module-creator */

#include "FrameSaverMediaPipeline.hpp"
#include <mutex>

namespace kurento
{
namespace module
{
namespace FrameSaverMediaPipeline
{

static std::vector<std::string> hierarchyFrameSaverMediaPipeline;
std::once_flag initFlagFrameSaverMediaPipeline;

static void
initHierarchy ()
{
  hierarchyFrameSaverMediaPipeline.push_back("kurento.MediaPipeline");
  hierarchyFrameSaverMediaPipeline.push_back("kurento.MediaObject");
}

const std::vector<std::string> &
FrameSaverMediaPipeline::getHierarchy () const
{
  std::call_once (initFlagFrameSaverMediaPipeline,initHierarchy);
  return hierarchyFrameSaverMediaPipeline;
}

const std::string&
FrameSaverMediaPipeline::getType () const {
  static std::string typeName = "FrameSaverMediaPipeline";

  return typeName;
}

const std::string&
FrameSaverMediaPipeline::getQualifiedType () const {
  static std::string qualifiedTypeName = getModule() + "." + getType();

  return qualifiedTypeName;
}

const std::string&
FrameSaverMediaPipeline::getModule () const {
  static std::string moduleName =  "FrameSaverMediaPipeline";

  return moduleName;
}

} /* FrameSaverMediaPipeline */
} /* module */
} /* kurento */
