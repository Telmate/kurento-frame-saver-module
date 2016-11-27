/*
 * =================================================================================================
 * File:        FrameSaverVideoFilterImpl.cpp
 *
 * History:     1. 2016-11-25   JBendor     Created as a class derived from kurento::FilterImpl
 *              2. 2016-11-26   JBendor     Updated
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * =================================================================================================
 */

#include <FrameSaverVideoFilterImpl.hpp>

#include <FrameSaverVideoFilterImplFactory.hpp>


#define GST_DEFAULT_NAME    "KurentoFrameSaverVideoFilterImpl"

#define GST_CAT_DEFAULT     kurento_frame_saver_media_pipeline_impl

GST_DEBUG_CATEGORY_STATIC   (GST_CAT_DEFAULT);


namespace kurento
{

namespace module
{

namespace FrameSaverVideoFilter
{

FrameSaverVideoFilterImpl::~FrameSaverVideoFilterImpl()
{
    releaseResources(true);
}


FrameSaverVideoFilterImpl::FrameSaverVideoFilterImpl (const boost::property_tree::ptree & ref_config, 
                                                      std::shared_ptr<MediaPipeline>      ptr_parent)
                         : FilterImpl ( ref_config, std::dynamic_pointer_cast<MediaPipelineImpl>(ptr_parent) )
{
    mFrameSaverPluginPtr = (GstElement *) FilterImpl::element;

    mPipelineParentPtr = (GstElement *) gst_element_get_parent( mFrameSaverPluginPtr );

    initializeInstance(true);
}


bool FrameSaverVideoFilterImpl::setPipelinePlayState(bool isEnabled)
{
    std::unique_lock <std::recursive_mutex>  locker (mRecursiveMutex);

    bool is_ok = (mPipelineParentPtr != NULL) && (mFrameSaverPluginPtr != NULL);

    if (is_ok)
    {
        gst_element_set_state ( mPipelineParentPtr, isEnabled ? GST_STATE_PLAYING : GST_STATE_READY );
    }

    return is_ok;
}


bool FrameSaverVideoFilterImpl::getParamsList(std::string aCurrentParamsSeparatedByTabs)
{
    static const char * names[] = { "wait", "snap", "link", "pads", "path", "note", NULL };

    std::string param_text;

    int index = -1;

    while ( names[++index] != NULL )
    {
        if (getParam(std::string(names[index]), param_text) == true)
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


bool FrameSaverVideoFilterImpl::getParam(const std::string aName, std::string aPresentValue)
{
    bool is_ok = (mPipelineParentPtr != NULL) && (mFrameSaverPluginPtr != NULL);

    gchar * txt_ptr = NULL;

    if (is_ok)
    {
        g_object_get( G_OBJECT(mFrameSaverPluginPtr), aName.c_str(), & txt_ptr, NULL );

        is_ok = (txt_ptr != NULL);
    }

    aPresentValue.assign(is_ok ? txt_ptr : "");

    return is_ok;
}


bool FrameSaverVideoFilterImpl::setParam(const std::string aName, const std::string aNewVal)
{
    std::unique_lock <std::recursive_mutex>  locker (mRecursiveMutex);

    std::string param_text;

    bool is_ok = getParam(aName, param_text);

    if (is_ok)
    {
        g_object_set( G_OBJECT(mFrameSaverPluginPtr), aName.c_str(), aNewVal.c_str(), NULL );
    }

    return is_ok;
}


void FrameSaverVideoFilterImpl::postConstructor()
{
    FilterImpl::postConstructor ();

    setPipelinePlayState(false);

    return;    
}


bool FrameSaverVideoFilterImpl::initializeInstance(bool isNewInstance)
{
    bool is_ok = setPipelinePlayState ( false );

    return is_ok;
}


bool FrameSaverVideoFilterImpl::releaseResources(bool isDelete)
{
    std::unique_lock <std::recursive_mutex>  locker (mRecursiveMutex);

    bool is_ok = setPipelinePlayState(false);

    return is_ok;
}


FrameSaverVideoFilterImpl::StaticConstructor FrameSaverVideoFilterImpl::Private_Static_Constructor;

FrameSaverVideoFilterImpl::StaticConstructor::StaticConstructor()
{
    GST_DEBUG_CATEGORY_INIT(GST_CAT_DEFAULT, GST_DEFAULT_NAME, 0, GST_DEFAULT_NAME);
}


MediaObjectImpl * FrameSaverVideoFilterImplFactory::createObject (const boost::property_tree::ptree & config, std::shared_ptr<MediaPipeline> parent) const
{
    return new FrameSaverVideoFilterImpl (config, parent);
}

} // ends namespace FrameSaverVideoFilter

} // ends namespace module

} // ends namespace kurento

