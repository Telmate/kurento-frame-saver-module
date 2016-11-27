/* Autogenerated with kurento-module-creator */

#include "FrameSaverVideoFilter.hpp"
#include <mutex>

namespace kurento
{
namespace module
{
namespace kms_frame_saver_video_filter
{

static std::vector<std::string> hierarchyFrameSaverVideoFilter;
std::once_flag initFlagFrameSaverVideoFilter;

static void
initHierarchy ()
{
  hierarchyFrameSaverVideoFilter.push_back("kurento.Filter");
  hierarchyFrameSaverVideoFilter.push_back("kurento.MediaElement");
  hierarchyFrameSaverVideoFilter.push_back("kurento.MediaObject");
}

const std::vector<std::string> &
FrameSaverVideoFilter::getHierarchy () const
{
  std::call_once (initFlagFrameSaverVideoFilter,initHierarchy);
  return hierarchyFrameSaverVideoFilter;
}

const std::string&
FrameSaverVideoFilter::getType () const {
  static std::string typeName = "FrameSaverVideoFilter";

  return typeName;
}

const std::string&
FrameSaverVideoFilter::getQualifiedType () const {
  static std::string qualifiedTypeName = getModule() + "." + getType();

  return qualifiedTypeName;
}

const std::string&
FrameSaverVideoFilter::getModule () const {
  static std::string moduleName =  "kms_frame_saver_video_filter";

  return moduleName;
}

} /* kms_frame_saver_video_filter */
} /* module */
} /* kurento */
