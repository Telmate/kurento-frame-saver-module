/*
 * =================================================================================================
 * File:        FrameSaverMediaPipeline_Impl.c
 *
 * History:     1. 2016-11-16   JBendor     Created
 *              2. 2016-11-20   JBendor     Updated
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * =================================================================================================
 */

#ifndef __FRAME_SAVER_MEDIA_PIPELINE_IMPL_HPP__
#define __FRAME_SAVER_MEDIA_PIPELINE_IMPL_HPP__

#include "MediaPipelineImpl.hpp"    // also includes: gst.h, ptree.hpp, JsonSerializer.hpp


namespace kurento
{

#define STRING      std::string

class FrameSaverMediaPipeline_Impl;

// This function does not define a body --- it's declared for TBD future uses
void Serialize(std::shared_ptr<FrameSaverMediaPipeline_Impl> & aSomeObjectRef,
               kurento::JsonSerializer                       & aSerializerRef);


class FrameSaverMediaPipeline_Impl: public MediaPipelineImpl
{
public:

    FrameSaverMediaPipeline_Impl(const boost::property_tree::ptree &config);

    virtual ~FrameSaverMediaPipeline_Impl() { releaseResources(true); }

    virtual bool hasFrameSaver() { return (mFrameSaverPluginPtr ? true : false); }

    virtual bool addFrameSaver(const STRING aLink, const STRING aPads); // returns FALSE on failure

    virtual bool getParamsList(STRING aCurrentParamsSeparatedByTabs);   // returns FALSE on failure

    virtual bool getParam(const STRING aName, STRING aPresentValue);    // returns FALSE on failure

    virtual bool setParam(const STRING aName, const STRING aNewVal);    // returns FALSE on failure

    virtual void Serialize(kurento::JsonSerializer & aSerializerRef);	// no-body (abstract in base)

protected:

    virtual void postConstructor();

    virtual bool releaseResources(bool isDelete);

private:
    GstElement * mFrameSaverPluginPtr;

    class StaticConstructor
    {
    public:
        StaticConstructor();
    };

    static StaticConstructor s_StaticConstructor;
};


} /* ends namespace kurento */


#endif /*  __FRAME_SAVER_MEDIA_PIPELINE_IMPL_HPP__ */

