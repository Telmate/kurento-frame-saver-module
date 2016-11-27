/*
 * =================================================================================================
 * File:        FrameSaverVideoFilterImpl.cpp
 *
 * History:     1. 2016-11-25   JBendor     Created as a class derived from kurento::FilterImpl
 *              2. 2016-11-27   JBendor     Updated
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * =================================================================================================
 */

#include <FrameSaverVideoFilterImpl.hpp>

#include <FrameSaverVideoFilterImplFactory.hpp>


#define GST_DEFAULT_NAME    "FrameSaverVideoFilterImpl"

#define GST_CAT_DEFAULT     kurento_frame_saver_video_filter_impl

GST_DEBUG_CATEGORY_STATIC   (GST_CAT_DEFAULT);

namespace kurento
{

namespace module
{

namespace kms_frame_saver_video_filter
{

static FrameSaverVideoFilterImpl * First_Instance_Ptr = NULL;


FrameSaverVideoFilterImpl * FrameSaverVideoFilterImpl::getFirstInstancePtr()
{
    return First_Instance_Ptr;
}


FrameSaverVideoFilterImpl::~FrameSaverVideoFilterImpl()
{
    releaseResources(true);
}


FrameSaverVideoFilterImpl::FrameSaverVideoFilterImpl (const boost::property_tree::ptree & ref_config, 
                                                      std::shared_ptr<MediaPipeline>      ptr_parent)
                          : 
                          FilterImpl ( ref_config, std::dynamic_pointer_cast<MediaPipelineImpl>(ptr_parent) )
{
    initializeInstance(true);
}


bool FrameSaverVideoFilterImpl::setPipelinePlayState(bool isEnabled)
{
    std::unique_lock <std::recursive_mutex>  locker (mRecursiveMutex);

    GstElement  * self_ptr = NULL;

    g_object_get (G_OBJECT (element), "filter", & self_ptr, NULL);

    bool is_ok = (self_ptr != NULL);

    if (is_ok)
    {
        GstElement * pipeline_ptr = (GstElement *) gst_element_get_parent( self_ptr );

        if (pipeline_ptr != NULL)
        {
            gst_element_set_state ( pipeline_ptr, isEnabled ? GST_STATE_PLAYING : GST_STATE_READY );
        }
        else
        {
            is_ok = false;
        }

        g_object_unref (self_ptr);
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
    gchar       * text_ptr = NULL;

    GstElement  * self_ptr = NULL;

    g_object_get (G_OBJECT (element), "filter", & self_ptr, NULL);

    bool is_ok = (self_ptr != NULL);

    if (is_ok)
    {
        g_object_get( G_OBJECT(self_ptr), aName.c_str(), & text_ptr, NULL );

        g_object_unref (self_ptr);

        is_ok = (text_ptr != NULL);
    }

    aPresentValue.assign(is_ok ? text_ptr : "");

    return is_ok;
}


bool FrameSaverVideoFilterImpl::setParam(const std::string aName, const std::string aNewVal)
{
    std::unique_lock <std::recursive_mutex>  locker (mRecursiveMutex);

    std::string param_text;

    bool is_ok = getParam(aName, param_text);

    if (is_ok)
    {
        GstElement * self_ptr = NULL;

        g_object_get (G_OBJECT (element), "filter", & self_ptr, NULL);

        if (self_ptr != NULL) 
        {
            g_object_set( G_OBJECT(self_ptr), aName.c_str(), aNewVal.c_str(), NULL );

            g_object_unref (self_ptr);
        }
        else
        {
            is_ok = false;
        }
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
    std::unique_lock <std::recursive_mutex>  locker (mRecursiveMutex);

    GstElement * self_ptr = NULL;

    g_object_get (G_OBJECT (element), "filter", & self_ptr, NULL);

    if (self_ptr == NULL) 
    {
        throw KurentoException (MEDIA_OBJECT_NOT_AVAILABLE, "Media Object " GST_DEFAULT_NAME " not available");
    }

    g_object_unref (self_ptr);

    g_object_set (element, "filter-factory", GST_DEFAULT_NAME, NULL);

    if (First_Instance_Ptr == NULL)
    {
        First_Instance_Ptr = this;
    }

    return true;
}


bool FrameSaverVideoFilterImpl::releaseResources(bool isDelete)
{ 
    std::unique_lock <std::recursive_mutex>  locker (mRecursiveMutex);

    if (First_Instance_Ptr == this)
    {
        First_Instance_Ptr = NULL;
    }

    return true;
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

} // ends namespace: fkms_frame_saver_video_filter

} // ends namespace: module

} // ends namespace: kurento
