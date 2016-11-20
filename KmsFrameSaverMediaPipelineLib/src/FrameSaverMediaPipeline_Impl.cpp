/*
 * =================================================================================================
 * File:        FrameSaverMediaPipeline_Impl.c
 *
 * History:     1. 2016-11-16   JBendor     Created
 *              2. 2016-11-19   JBendor     Updated
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * =================================================================================================
 */

#include "FrameSaverMediaPipeline_Impl.hpp"


#define GST_DEFAULT_NAME    "KurentoFrameSaverMediaPipeline_Impl"

#define GST_CAT_DEFAULT     kurento_farem_saver_media_pipeline_impl

GST_DEBUG_CATEGORY_STATIC   (GST_CAT_DEFAULT);


namespace kurento
{

FrameSaverMediaPipeline_Impl::FrameSaverMediaPipeline_Impl(const boost::property_tree::ptree &config)
                            : MediaPipelineImpl(config)
{
    mFrameSaverPluginPtr = NULL;
}


bool FrameSaverMediaPipeline_Impl::addFrameSaver(const STRING aLink, const STRING aPads)
{
    #define PLUGIN_TYPE   "FrameSaverPlugin"
    #define PLUGIN_NAME   (PLUGIN_TYPE "_0")

    bool is_ok = (mFrameSaverPluginPtr == NULL);

    if (is_ok)
    {
        mFrameSaverPluginPtr = gst_element_factory_make(PLUGIN_TYPE, PLUGIN_NAME);

        is_ok = (mFrameSaverPluginPtr == NULL);
    }

    if (is_ok)
    {
        is_ok = addElement(mFrameSaverPluginPtr);
    }

    if (is_ok && (aLink.empty() == false))
    {
        is_ok = setParam( STRING("link"), aLink);
    }

    if (is_ok && (aPads.empty() == false))
    {
        is_ok = setParam( STRING("pads"), aPads);
    }

    return is_ok;
}

bool FrameSaverMediaPipeline_Impl::getParamsList(STRING aCurrentParamsSeparatedByTabs)
{
    static const char * names[] = {"wait", "snap", "link", "pads", "path", "note", NULL};

    STRING param_text;

    int index = -1;

    while ( names[++index] != NULL )
    {
        if (getParam(STRING(names[index]), param_text) == true)
        {
            aCurrentParamsSeparatedByTabs.append( names[index] ); 
            aCurrentParamsSeparatedByTabs.append( "=" );
            aCurrentParamsSeparatedByTabs.append( param_text.c_str() );
            aCurrentParamsSeparatedByTabs.append( "\t" );
            continue;
        }
        break;
    }

    return (names[index] ? false : true);
}


bool FrameSaverMediaPipeline_Impl::getParam(const STRING aName, STRING aPresentValue)
{
    bool is_ok = (mFrameSaverPluginPtr != NULL);

    gchar * txt_ptr = NULL;

    if (is_ok)
    {
        g_object_get(G_OBJECT(mFrameSaverPluginPtr), aName.c_str(), & txt_ptr, NULL);

        is_ok = (txt_ptr != NULL);
    }

    aPresentValue.assign(is_ok ? txt_ptr : "");

    return is_ok;
}


bool FrameSaverMediaPipeline_Impl::setParam(const STRING aName, const STRING aNewVal)
{
    STRING param_text;

    bool is_ok = getParam(aName, param_text);

    if (is_ok)
    {
        g_object_set(G_OBJECT(mFrameSaverPluginPtr), aName.c_str(), aNewVal.c_str(), NULL);
    }

    return is_ok;
}


void FrameSaverMediaPipeline_Impl::postConstructor()
{
    MediaPipelineImpl::postConstructor();
}


bool FrameSaverMediaPipeline_Impl::releaseResources(bool isDelete)
{
    delete(mFrameSaverPluginPtr);
    mFrameSaverPluginPtr = NULL;
    return true;
}


FrameSaverMediaPipeline_Impl::StaticConstructor FrameSaverMediaPipeline_Impl::s_StaticConstructor;

FrameSaverMediaPipeline_Impl::StaticConstructor::StaticConstructor()
{
    GST_DEBUG_CATEGORY_INIT(GST_CAT_DEFAULT, GST_DEFAULT_NAME, 0, GST_DEFAULT_NAME);
}


} /* ends namespace kurento */
