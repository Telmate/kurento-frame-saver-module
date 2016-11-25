/* JB-FIXED --- Autogenerated with kurento-module-creator */

#include <gst/gst.h>
#include "FrameSaverMediaPipelineImpl.hpp"
#include "FrameSaverMediaPipelineImplFactory.hpp"
#include "FrameSaverMediaPipelineInternal.hpp"
#include <jsonrpc/JsonSerializer.hpp>
#include <KurentoException.hpp>

using kurento::KurentoException;

namespace kurento
{
namespace module
{
namespace FrameSaverMediaPipeline
{

MediaObjectImpl *FrameSaverMediaPipelineImplFactory::createObjectPointer (const boost::property_tree::ptree &conf, const Json::Value &params) const
{
  kurento::JsonSerializer s (false);
  FrameSaverMediaPipelineConstructor constructor;

  s.JsonValue = params;
  constructor.Serialize (s);

  return createObject (conf);
}

void
FrameSaverMediaPipelineImpl::invoke (std::shared_ptr<MediaObjectImpl> obj, const std::string &methodName, const Json::Value &params, Json::Value &response)
{
  MediaPipelineImpl::invoke (obj, methodName, params, response);
}

bool
FrameSaverMediaPipelineImpl::connect (const std::string &eventType, std::shared_ptr<EventHandler> handler)
{

  return MediaPipelineImpl::connect (eventType, handler);
}

void
FrameSaverMediaPipelineImpl::Serialize (JsonSerializer &serializer)
{
  if (serializer.IsWriter) {
    try {
      Json::Value v (getId() );

      serializer.JsonValue = v;
    } catch (std::bad_cast &e) {
    }
  } else {
    throw KurentoException (MARSHALL_ERROR,
                            "'FrameSaverMediaPipelineImpl' cannot be deserialized as an object");
  }
}
} /* FrameSaverMediaPipeline */
} /* module */
} /* kurento */

namespace kurento
{

void
Serialize (std::shared_ptr<kurento::module::FrameSaverMediaPipeline::FrameSaverMediaPipelineImpl> &object, JsonSerializer &serializer)
{
  if (serializer.IsWriter) {
    if (object) {
      object->Serialize (serializer);
    }
  } else {
    std::shared_ptr<kurento::MediaObjectImpl> aux;
    aux = kurento::module::FrameSaverMediaPipeline::FrameSaverMediaPipelineImplFactory::getObject (JsonFixes::getString(serializer.JsonValue) );
    object = std::dynamic_pointer_cast<kurento::module::FrameSaverMediaPipeline::FrameSaverMediaPipelineImpl> (aux);
  }
}

void
Serialize (std::shared_ptr<kurento::module::FrameSaverMediaPipeline::FrameSaverMediaPipeline> &object, JsonSerializer &serializer)
{
  std::shared_ptr<kurento::module::FrameSaverMediaPipeline::FrameSaverMediaPipelineImpl> aux = std::dynamic_pointer_cast<kurento::module::FrameSaverMediaPipeline::FrameSaverMediaPipelineImpl> (object);

  Serialize(object, serializer);            //? TODO-FIXED-JB --- was: Serialize (aux, serializer);

  object = std::dynamic_pointer_cast <kurento::module::FrameSaverMediaPipeline::FrameSaverMediaPipeline> (aux);
}

} /* kurento */
