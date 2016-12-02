/* Autogenerated with kurento-module-creator */

#include <iostream>
#include "FrameSaverVideoFilterInternal.hpp"
#include <KurentoException.hpp>
#include <jsonrpc/JsonSerializer.hpp>
#include "MediaPipeline.hpp"

namespace kurento
{
namespace module
{
namespace framesaver
{

bool FrameSaverVideoFilterMethodStartPipelinePlaying::invoke (std::shared_ptr<FrameSaverVideoFilter> obj)
{
  if (!obj) {
    throw KurentoException (MEDIA_OBJECT_NOT_FOUND, "Invalid object while invoking method FrameSaverVideoFilter::startPipelinePlaying");
  }

  return obj->startPipelinePlaying ();
}

void FrameSaverVideoFilterMethodStartPipelinePlaying::Serialize (kurento::JsonSerializer &s)
{
  if (s.IsWriter) {
  } else {
    if (!s.JsonValue.isNull () && !s.JsonValue.isObject ()) {
      throw KurentoException (MARSHALL_ERROR,
                              "'operationParams' should be an object");
    }

  }
}

bool FrameSaverVideoFilterMethodStopPipelinePlaying::invoke (std::shared_ptr<FrameSaverVideoFilter> obj)
{
  if (!obj) {
    throw KurentoException (MEDIA_OBJECT_NOT_FOUND, "Invalid object while invoking method FrameSaverVideoFilter::stopPipelinePlaying");
  }

  return obj->stopPipelinePlaying ();
}

void FrameSaverVideoFilterMethodStopPipelinePlaying::Serialize (kurento::JsonSerializer &s)
{
  if (s.IsWriter) {
  } else {
    if (!s.JsonValue.isNull () && !s.JsonValue.isObject ()) {
      throw KurentoException (MARSHALL_ERROR,
                              "'operationParams' should be an object");
    }

  }
}

std::string FrameSaverVideoFilterMethodGetElementsNamesList::invoke (std::shared_ptr<FrameSaverVideoFilter> obj)
{
  if (!obj) {
    throw KurentoException (MEDIA_OBJECT_NOT_FOUND, "Invalid object while invoking method FrameSaverVideoFilter::getElementsNamesList");
  }

  return obj->getElementsNamesList ();
}

void FrameSaverVideoFilterMethodGetElementsNamesList::Serialize (kurento::JsonSerializer &s)
{
  if (s.IsWriter) {
  } else {
    if (!s.JsonValue.isNull () && !s.JsonValue.isObject ()) {
      throw KurentoException (MARSHALL_ERROR,
                              "'operationParams' should be an object");
    }

  }
}

std::string FrameSaverVideoFilterMethodGetLastError::invoke (std::shared_ptr<FrameSaverVideoFilter> obj)
{
  if (!obj) {
    throw KurentoException (MEDIA_OBJECT_NOT_FOUND, "Invalid object while invoking method FrameSaverVideoFilter::getLastError");
  }

  return obj->getLastError ();
}

void FrameSaverVideoFilterMethodGetLastError::Serialize (kurento::JsonSerializer &s)
{
  if (s.IsWriter) {
  } else {
    if (!s.JsonValue.isNull () && !s.JsonValue.isObject ()) {
      throw KurentoException (MARSHALL_ERROR,
                              "'operationParams' should be an object");
    }

  }
}

std::string FrameSaverVideoFilterMethodGetParamsList::invoke (std::shared_ptr<FrameSaverVideoFilter> obj)
{
  if (!obj) {
    throw KurentoException (MEDIA_OBJECT_NOT_FOUND, "Invalid object while invoking method FrameSaverVideoFilter::getParamsList");
  }

  return obj->getParamsList ();
}

void FrameSaverVideoFilterMethodGetParamsList::Serialize (kurento::JsonSerializer &s)
{
  if (s.IsWriter) {
  } else {
    if (!s.JsonValue.isNull () && !s.JsonValue.isObject ()) {
      throw KurentoException (MARSHALL_ERROR,
                              "'operationParams' should be an object");
    }

  }
}

std::string FrameSaverVideoFilterMethodGetParam::invoke (std::shared_ptr<FrameSaverVideoFilter> obj)
{
  if (!obj) {
    throw KurentoException (MEDIA_OBJECT_NOT_FOUND, "Invalid object while invoking method FrameSaverVideoFilter::getParam");
  }

  return obj->getParam (aParamName);
}

void FrameSaverVideoFilterMethodGetParam::Serialize (kurento::JsonSerializer &s)
{
  if (s.IsWriter) {
    s.SerializeNVP (aParamName);

  } else {
    if (s.JsonValue.isNull ()) {
      throw KurentoException (MARSHALL_ERROR,
                              "'operationParams' is required");
    } else if (!s.JsonValue.isObject ()){
      throw KurentoException (MARSHALL_ERROR,
                              "'operationParams' should be an object");
    }

    if (!s.JsonValue.isMember ("aParamName") || !s.JsonValue["aParamName"].isConvertibleTo (Json::ValueType::stringValue) ) {
      throw KurentoException (MARSHALL_ERROR,
                              "'aParamName' parameter should be a string");
    }

    s.SerializeNVP (aParamName);

  }
}

bool FrameSaverVideoFilterMethodSetParam::invoke (std::shared_ptr<FrameSaverVideoFilter> obj)
{
  if (!obj) {
    throw KurentoException (MEDIA_OBJECT_NOT_FOUND, "Invalid object while invoking method FrameSaverVideoFilter::setParam");
  }

  return obj->setParam (aParamName, aNewParamValue);
}

void FrameSaverVideoFilterMethodSetParam::Serialize (kurento::JsonSerializer &s)
{
  if (s.IsWriter) {
    s.SerializeNVP (aParamName);

    s.SerializeNVP (aNewParamValue);

  } else {
    if (s.JsonValue.isNull ()) {
      throw KurentoException (MARSHALL_ERROR,
                              "'operationParams' is required");
    } else if (!s.JsonValue.isObject ()){
      throw KurentoException (MARSHALL_ERROR,
                              "'operationParams' should be an object");
    }

    if (!s.JsonValue.isMember ("aParamName") || !s.JsonValue["aParamName"].isConvertibleTo (Json::ValueType::stringValue) ) {
      throw KurentoException (MARSHALL_ERROR,
                              "'aParamName' parameter should be a string");
    }

    s.SerializeNVP (aParamName);

    if (!s.JsonValue.isMember ("aNewParamValue") || !s.JsonValue["aNewParamValue"].isConvertibleTo (Json::ValueType::stringValue) ) {
      throw KurentoException (MARSHALL_ERROR,
                              "'aNewParamValue' parameter should be a string");
    }

    s.SerializeNVP (aNewParamValue);

  }
}

std::shared_ptr<MediaPipeline> FrameSaverVideoFilterConstructor::getMediaPipelineParent ()
{
  return mediaPipelineParent;
}

void FrameSaverVideoFilterConstructor::Serialize (kurento::JsonSerializer &s)
{
  if (s.IsWriter) {
    s.SerializeNVP (mediaPipelineParent);

  } else {
    if (s.JsonValue.isNull ()) {
      throw KurentoException (MARSHALL_ERROR,
                              "'constructorParams' is required");
    } else if (!s.JsonValue.isObject ()){
      throw KurentoException (MARSHALL_ERROR,
                              "'constructorParams' should be an object");
    }

    if (!s.JsonValue.isMember ("mediaPipelineParent") || !s.JsonValue["mediaPipelineParent"].isConvertibleTo (Json::ValueType::stringValue) ) {
      throw KurentoException (MARSHALL_ERROR,
                              "'mediaPipelineParent' parameter should be a string");
    }

    s.SerializeNVP (mediaPipelineParent);

  }
}

} /* framesaver */
} /* module */
} /* kurento */
