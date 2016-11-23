/*
 * =================================================================================================
 * File:        FrameSaverMediaPipelineImpl.cpp
 *
 * History:     1. 2016-11-16   JBendor     Created
 *              2. 2016-11-21   JBendor     Updated
 *              3. 2016-11-22   JBendor     Methods for class FrameSaverMediaPipelineImplFactory
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * =================================================================================================
 */

#include "FrameSaverMediaPipelineImpl.hpp"    // also defines class FrameSaverMediaPipelineImplFactory

#include <DotGraph.hpp>

#include <GstreamerDotDetails.hpp>

#include <SignalHandler.hpp>

#include "kmselement.h"


#define GST_DEFAULT_NAME    "KurentoFrameSaverMediaPipelineImpl"

#define GST_CAT_DEFAULT     kurento_farem_saver_media_pipeline_impl

GST_DEBUG_CATEGORY_STATIC   (GST_CAT_DEFAULT);


namespace kurento
{

static boost::property_tree::ptree  The_Default_Config; // = new boost::property_tree::ptree();


FrameSaverMediaPipelineImpl::FrameSaverMediaPipelineImpl() : MediaObjectImpl(The_Default_Config)
{
    initializeInstance(true);
}


FrameSaverMediaPipelineImpl::FrameSaverMediaPipelineImpl (std::shared_ptr <MediaObject> parent)
                            : MediaObjectImpl(The_Default_Config, parent)
{
    initializeInstance(true);
}


FrameSaverMediaPipelineImpl::FrameSaverMediaPipelineImpl (const boost::property_tree::ptree & config)
                            : MediaObjectImpl(config)
{
    initializeInstance(true);
}


FrameSaverMediaPipelineImpl::~FrameSaverMediaPipelineImpl()
{ 
    releaseResources(true); 
}


bool FrameSaverMediaPipelineImpl::addFrameSaver(const STRING aLink, const STRING aPads)
{
    std::unique_lock <std::recursive_mutex>  locker (mRecursiveMutex);

    #define PLUGIN_TYPE   "FrameSaverPlugin"
    #define PLUGIN_NAME   "FrameSaverPlugin_0"

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


bool FrameSaverMediaPipelineImpl::getParamsList(STRING aCurrentParamsSeparatedByTabs)
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


bool FrameSaverMediaPipelineImpl::getParam(const STRING aName, STRING aPresentValue)
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


bool FrameSaverMediaPipelineImpl::setParam(const STRING aName, const STRING aNewVal)
{
    std::unique_lock <std::recursive_mutex>  locker (mRecursiveMutex);

    STRING param_text;

    bool is_ok = getParam(aName, param_text);

    if (is_ok)
    {
        g_object_set( G_OBJECT(mFrameSaverPluginPtr), aName.c_str(), aNewVal.c_str(), NULL );
    }

    return is_ok;
}


bool FrameSaverMediaPipelineImpl::setPipelinePlayState(bool isEnabled)
{
    std::unique_lock <std::recursive_mutex>  locker (mRecursiveMutex);

    gst_element_set_state (mPipelinePtr, isEnabled ? GST_STATE_PLAYING : GST_STATE_READY);

    return true;
}



bool FrameSaverMediaPipelineImpl::addElement (GstElement * aElementPtr)
{
    std::unique_lock <std::recursive_mutex> locker (mRecursiveMutex);

    if ( KMS_IS_ELEMENT (aElementPtr) ) 
    {
        g_object_set (aElementPtr, "media-stats", mLatencyStatsFlag, NULL);
    }

    bool is_ok = gst_bin_add ( GST_BIN(mPipelinePtr), aElementPtr );

    if (is_ok) 
    {
        gst_element_sync_state_with_parent ( aElementPtr );
    }

    return is_ok;
}


GstElement * FrameSaverMediaPipelineImpl::getPipeline() 
{ 
    return mPipelinePtr; 
}


STRING FrameSaverMediaPipelineImpl::getGstreamerDot (std::shared_ptr<GstreamerDotDetails>  ptr_details)
{
    STRING result = generateDotGraph (GST_BIN(mPipelinePtr), ptr_details);

    return result;
}


STRING FrameSaverMediaPipelineImpl::getGstreamerDot()
{
    std::shared_ptr <GstreamerDotDetails> dot_ptr( new GstreamerDotDetails(GstreamerDotDetails::SHOW_VERBOSE) );

    STRING result = generateDotGraph ( GST_BIN(mPipelinePtr), dot_ptr );

    return result;
}


bool FrameSaverMediaPipelineImpl::getLatencyStats ()
{
    std::unique_lock <std::recursive_mutex>  locker (mRecursiveMutex);

    return mLatencyStatsFlag;
}


void FrameSaverMediaPipelineImpl::setLatencyStats (bool newState)
{
    std::unique_lock <std::recursive_mutex>  locker (mRecursiveMutex);

    if (mLatencyStatsFlag == newState) 
    {
        return;
    }

    mLatencyStatsFlag = newState;

    GstIterator * iter_ptr = gst_bin_iterate_elements ( GST_BIN(mPipelinePtr) );

    GValue      item_value = G_VALUE_INIT;

    gboolean      is_ended = FALSE;

    while (! is_ended) 
    {
        switch (gst_iterator_next (iter_ptr, &item_value) ) 
        {
            case GST_ITERATOR_OK: 
            {
                GstElement * element_ptr = GST_ELEMENT (g_value_get_object (&item_value) );

                if (KMS_IS_ELEMENT (element_ptr) ) 
                {
                    g_object_set (element_ptr, "media-stats", mLatencyStatsFlag, NULL);
                }

                g_value_reset ( &item_value );
                break;
            }

            case GST_ITERATOR_RESYNC:
                gst_iterator_resync (iter_ptr);
                break;

            case GST_ITERATOR_ERROR:
            case GST_ITERATOR_DONE:
                is_ended = TRUE;
                break;

            default:
                break;
        }
    }

    g_value_unset ( &item_value );

    gst_iterator_free (iter_ptr);

    return;
}


void FrameSaverMediaPipelineImpl::postConstructor()
{
    MediaObjectImpl::postConstructor ();

    GstBus * bus_ptr = gst_pipeline_get_bus( GST_PIPELINE(mPipelinePtr) );

    gst_bus_add_signal_watch (bus_ptr); 

    mBusMessageHandler = register_signal_handler( G_OBJECT(bus_ptr), 
                                                  "message",
                                                  std::function < void (GstBus *, GstMessage *) > 
                                                  ( std::bind (&FrameSaverMediaPipelineImpl::handleBusMessage, this, std::placeholders::_2) ),
                                                  std::dynamic_pointer_cast<FrameSaverMediaPipelineImpl> (shared_from_this() ) );
    g_object_unref (bus_ptr);

    return;    
}


bool FrameSaverMediaPipelineImpl::initializeInstance(bool isNewInstance)
{
    if (! isNewInstance)
    {
        releaseResources(false);
    }

    mFrameSaverPluginPtr = NULL;

    mBusMessageHandler = 0;

    mLatencyStatsFlag = false;

    mPipelinePtr = gst_pipeline_new (NULL);

    if (mPipelinePtr == NULL)
    {
        throw KurentoException (MEDIA_OBJECT_NOT_AVAILABLE, "Cannot create gstreamer pipeline");
    }

    GstClock * clock_ptr = gst_system_clock_obtain ();

    gst_pipeline_use_clock ( GST_PIPELINE(mPipelinePtr), clock_ptr );

    g_object_unref ( clock_ptr );

    setPipelinePlayState ( false );

    return true;
}


bool FrameSaverMediaPipelineImpl::releaseResources(bool isDelete)
{
    std::unique_lock <std::recursive_mutex>  locker (mRecursiveMutex);

    if (isDelete && mPipelinePtr)
    {
        GstBus * bus_ptr = gst_pipeline_get_bus ( GST_PIPELINE(mPipelinePtr) );

        gst_element_set_state (mPipelinePtr, GST_STATE_NULL);

        if (mBusMessageHandler > 0) 
        {
            unregister_signal_handler (bus_ptr, mBusMessageHandler);
        }

        gst_bus_remove_signal_watch (bus_ptr);

        g_object_unref (bus_ptr);

        g_object_unref (mPipelinePtr);

        g_object_unref (mFrameSaverPluginPtr);

        mFrameSaverPluginPtr = mPipelinePtr = NULL;
    }

    return true;
}


void FrameSaverMediaPipelineImpl::handleBusMessage (GstMessage * aMessagePtr)
{
    if (aMessagePtr->type != GST_MESSAGE_ERROR) 
    {
        return;
    }

    GError * err_ptr = NULL;

    gchar  * dbg_ptr = NULL;

    GST_ERROR ("Error on bus: %" GST_PTR_FORMAT, aMessagePtr);

    gst_debug_bin_to_dot_file_with_ts ( GST_BIN(mPipelinePtr), GST_DEBUG_GRAPH_SHOW_ALL, "error" );

    gst_message_parse_error ( aMessagePtr, &err_ptr, &dbg_ptr );

    STRING error_string;

    if (err_ptr) 
    {
        error_string = STRING (err_ptr->message);
    }

    if (dbg_ptr != NULL) 
    {
        error_string += " -> " + STRING (dbg_ptr);
    }

    try 
    {
        gint code = err_ptr ? err_ptr->code : 0;

        Error err_val (shared_from_this(), error_string , code, "UNEXPECTED_PIPELINE_ERROR");

        signalError (err_val);
    } 
    catch (std::bad_weak_ptr &ex)
    {
        ; // exception ignored
    }

    g_error_free (err_ptr);

    g_free (dbg_ptr);

    return;
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


} /* ends namespace kurento */

// ends file: "FrameSaverMediaPipelineImpl.cpp"
