
#ifndef __FRAME_SAVER_VIDEO_FILTER_HPP__
#define __FRAME_SAVER_VIDEO_FILTER_HPP__

#include "Filter.hpp"


namespace kurento
{

class FrameSaverVideoFilter;

class JsonSerializer;

void Serialize (std::shared_ptr<kurento::FrameSaverVideoFilter> & object, JsonSerializer & serializer);


class FrameSaverVideoFilter : public Filter
{
public:
    FrameSaverVideoFilter() { };

    virtual ~FrameSaverVideoFilter () {};

    virtual const std::string& getType () const;

    virtual const std::string& getQualifiedType () const;

    virtual const std::string& getModule () const;

    virtual const std::vector<std::string>& getHierarchy () const;

    virtual void Serialize (JsonSerializer &serializer) = 0;

};


} // ends namespace kurento

#endif // __FRAME_SAVER_VIDEO_FILTER_HPP__
