/*
 * =================================================================================================
 * File:        FrameSaverMediaPipelineImpl.cpp
 *
 * History:     1. 2016-11-16   JBendor     Created as a class derived from MediaPipelineImpl
 *              2. 2016-11-30   JBendor     Updated
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * =================================================================================================
 */

#include "FrameSaverMediaPipelineImpl.hpp"
#include "FrameSaverMediaPipelineImplFactory.hpp"

#define GST_DEFAULT_NAME    THE_CLASS_NAME
#define GST_CAT_DEFAULT     frame_saver_media_pipeline_impl
GST_DEBUG_CATEGORY_STATIC   (GST_CAT_DEFAULT);


namespace kurento
{

namespace module
{

namespace FrameSaverMediaPipeline
{

static boost::property_tree::ptree   The_Default_Config;

static FrameSaverMediaPipelineImpl * The_Single_Instance_Ptr = NULL;

static std::string                   This_Class_Name(THE_CLASS_NAME);



std::string FrameSaverMediaPipelineImpl::getClassName()
{
    This_Class_Name.assign(THE_CLASS_NAME);    return This_Class_Name;
}


FrameSaverMediaPipelineImpl * FrameSaverMediaPipelineImpl::getLiveInstancePtr()
{
    return The_Single_Instance_Ptr;
}


FrameSaverMediaPipelineImpl::~FrameSaverMediaPipelineImpl()
{
    releaseResources(true);
}


FrameSaverMediaPipelineImpl::FrameSaverMediaPipelineImpl() : MediaPipelineImpl(The_Default_Config)
{
    initializeInstance(true);
}


FrameSaverMediaPipelineImpl::FrameSaverMediaPipelineImpl (const boost::property_tree::ptree & ref_cfg)
                            : MediaPipelineImpl(ref_cfg)
{
    initializeInstance(true);
}


GstElement * FrameSaverMediaPipelineImpl::getPipeline() 
{ 
    return mPipelinePtr; 
}


bool FrameSaverMediaPipelineImpl::startPipelinePlaying()
{
    std::unique_lock <std::recursive_mutex>  locker (mRecursiveMutex);

    bool is_ok = (mPipelinePtr != NULL);

    if (is_ok)
    {
        GstState pipeline_state = GST_STATE_NULL;

        gst_element_get_state(mPipelinePtr, &pipeline_state, NULL, 0);

        if (pipeline_state != GST_STATE_PLAYING)
        {
            gst_element_set_state ( mPipelinePtr, GST_STATE_PLAYING );
        }
    }

    return is_ok;
}


bool FrameSaverMediaPipelineImpl::stopPipelinePlaying()
{
    std::unique_lock <std::recursive_mutex>  locker (mRecursiveMutex);

    bool is_ok = (mPipelinePtr != NULL);

    if (is_ok)
    {
        GstState pipeline_state = GST_STATE_NULL;

        gst_element_get_state(mPipelinePtr, &pipeline_state, NULL, 0);

        if (pipeline_state == GST_STATE_PLAYING)
        {
            gst_element_set_state ( mPipelinePtr, GST_STATE_READY );
        }
    }

    return is_ok;
}


bool FrameSaverMediaPipelineImpl::addFrameSaver(const std::string aLink, const std::string aPads)
{
    std::unique_lock <std::recursive_mutex>  locker (mRecursiveMutex);

    bool is_ok = (mFrameSaverPluginPtr == NULL);

    if (is_ok)
    {
        mFrameSaverPluginPtr = gst_element_factory_make("FrameSaverPlugin", "FrameSaverPlugin_0" );

        is_ok = (mFrameSaverPluginPtr == NULL);
    }

    if (is_ok)
    {
        is_ok = addElement(mFrameSaverPluginPtr);
    }

    if (is_ok && (aLink.empty() == false))
    {
        is_ok = setParam( std::string("link"), aLink);
    }

    if (is_ok && (aPads.empty() == false))
    {
        is_ok = setParam( std::string("pads"), aPads);
    }

    return is_ok;
}


bool FrameSaverMediaPipelineImpl::getFrameSaverName(std::string aElementName)
{
    bool is_ok = (mFrameSaverPluginPtr != NULL);

    aElementName.assign(is_ok ? "FrameSaverPlugin_0" : "");

    return is_ok;

}


bool FrameSaverMediaPipelineImpl::getElementsNamesList(std::string aElementsNamesSeparatedByTabs)
{
    std::unique_lock <std::recursive_mutex>  locker(mRecursiveMutex);

    bool is_ok = (mFrameSaverPluginPtr != NULL);

    GstElement * pipeline_ptr = is_ok ? (GstElement *) gst_element_get_parent(mFrameSaverPluginPtr) : NULL;

    GstIterator * iterate_ptr = pipeline_ptr ? gst_bin_iterate_elements(GST_BIN(pipeline_ptr)) : NULL;

    is_ok = (iterate_ptr != NULL);

    if (is_ok)
    {
        gchar * name_ptr = gst_element_get_name(pipeline_ptr);

        aElementsNamesSeparatedByTabs.append(name_ptr ? name_ptr : "PipelineNameIsNull");
        aElementsNamesSeparatedByTabs.append("\t");

        g_free(name_ptr);

        GValue element_as_value = G_VALUE_INIT;

        while (gst_iterator_next(iterate_ptr, &element_as_value) == GST_ITERATOR_OK)
        {
            name_ptr = gst_element_get_name(g_value_get_object(&element_as_value));

            aElementsNamesSeparatedByTabs.append(name_ptr ? name_ptr : "ElementNameIsNull");
            aElementsNamesSeparatedByTabs.append("\t");

            g_free(name_ptr);

            g_value_reset(&element_as_value);
        }

        g_value_unset(&element_as_value);

        gst_iterator_free(iterate_ptr);
    }

    return is_ok;
}


bool FrameSaverMediaPipelineImpl::getParamsList(std::string aCurrentParamsSeparatedByTabs)
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


bool FrameSaverMediaPipelineImpl::getParam(const std::string aName, std::string aPresentValue)
{
    bool is_ok = (mFrameSaverPluginPtr != NULL);

    gchar * txt_ptr = NULL;

    if (is_ok)
    {
        g_object_get( G_OBJECT(mFrameSaverPluginPtr), aName.c_str(), & txt_ptr, NULL );

        is_ok = (txt_ptr != NULL);
    }

    aPresentValue.assign(is_ok ? txt_ptr : "");

    return is_ok;
}


bool FrameSaverMediaPipelineImpl::setParam(const std::string aName, const std::string aNewVal)
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


void FrameSaverMediaPipelineImpl::postConstructor()
{
    MediaPipelineImpl::postConstructor ();

    mPipelinePtr = MediaPipelineImpl::getPipeline();

    stopPipelinePlaying();

    return;    
}


bool FrameSaverMediaPipelineImpl::initializeInstance(bool isNewInstance)
{
    if (The_Single_Instance_Ptr)
    {
        throw KurentoException (MEDIA_OBJECT_NOT_AVAILABLE, "FrameSaverMediaPipeline is singleton");
    }

    if (! isNewInstance)
    {
        releaseResources(false);
    }

    if (! The_Single_Instance_Ptr)
    {
        The_Single_Instance_Ptr = this;
    }

    mFrameSaverPluginPtr = NULL;

    mPipelinePtr = MediaPipelineImpl::getPipeline();

    stopPipelinePlaying();

    return true;
}


bool FrameSaverMediaPipelineImpl::releaseResources(bool isDelete)
{
    std::unique_lock <std::recursive_mutex>  locker (mRecursiveMutex);

    stopPipelinePlaying();

    mFrameSaverPluginPtr = mPipelinePtr = NULL;

    if (The_Single_Instance_Ptr == this)
    {
        The_Single_Instance_Ptr = NULL;
    }

    return true;
}


FrameSaverMediaPipelineImpl::StaticConstructor FrameSaverMediaPipelineImpl::Private_Static_Constructor;

FrameSaverMediaPipelineImpl::StaticConstructor::StaticConstructor()
{
    GST_DEBUG_CATEGORY_INIT(GST_CAT_DEFAULT, GST_DEFAULT_NAME, 0, GST_DEFAULT_NAME);
}


MediaObjectImpl * FrameSaverMediaPipelineImplFactory::createObject (const boost::property_tree::ptree & config) const
{
    return new FrameSaverMediaPipelineImpl (config);
}

} // ends namespace FrameSaverMediaPipeline

} // ends namespace module

} // ends namespace kurento

