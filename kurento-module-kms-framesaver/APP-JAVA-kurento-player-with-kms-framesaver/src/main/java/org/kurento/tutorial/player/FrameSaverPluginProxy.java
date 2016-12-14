/* 
 * ======================================================================================
 * File:        FrameSaverPluginProxy.c
 * 
 * History:     1. 2016-11-28   JBendor     Created
 *              5. 2016-12-05   JBendor     Updated
 *
 * Description: Implements a proxy for a Kurento module: FrameSaverVideoFilterPlugin
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * ======================================================================================
 */

package org.kurento.tutorial.player;

import org.kurento.client.MediaPipeline;
import org.kurento.module.framesavervideofilter.*;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;


public class FrameSaverPluginProxy
{
    private static final Logger TheLogger = LoggerFactory.getLogger(FrameSaverPluginProxy.class);
    
    private static FrameSaverVideoFilter   TheFrameSaverFilter = null;
    
    private static FrameSaverPluginProxy   TheFrameSaverProxy = null;
    
    private FrameSaverPluginProxy()     // private c'tor --- singleton class
    {
        return;     
    }
    
    public static FrameSaverPluginProxy newInstance(MediaPipeline aPipeline)
    {
        if (aPipeline == null)
        {
            return null;
        }

        if (TheFrameSaverProxy != null)
        {
            return TheFrameSaverProxy;
        }

        TheLogger.info("Calling FrameSaverVideoFilter.Builder(aPipeline).build()");
        
        try
        {
            TheFrameSaverFilter = new FrameSaverVideoFilter.Builder(aPipeline).build();
        }
        catch (Exception ex)
        {
            TheLogger.info( "EXCEPTION: " + ex.getMessage() );           
        }
        
        if (TheFrameSaverFilter == null)
        {
            TheLogger.info("Failed Using Factory.Build for new FrameSaverVideoFilter" );
        }
        
        TheFrameSaverProxy = new FrameSaverPluginProxy();      
               
        return TheFrameSaverProxy;
    }


    public boolean isUsable()
    {
        return (TheFrameSaverFilter != null);
    }
    
    public boolean setParams(String aParamsArray[])
    {
        boolean is_ok = (TheFrameSaverFilter != null) && (aParamsArray != null);
        
        if (! is_ok)
        {
            return false;               
        }
        
        for (int index=0;  is_ok && (index < aParamsArray.length);  ++index)
        {
            String parts[] = aParamsArray[index].split("=");
            
            is_ok = (parts.length == 2);
            
            if (is_ok)
            { 
            	is_ok = TheFrameSaverFilter.setParam(parts[0], parts[1]);
            }
        }
        
        return is_ok;
    }
    
    public boolean setElementsToSplice(String aProducerName, String aConsumerName)
    {
        boolean is_ok = (TheFrameSaverFilter != null);
        
        if (is_ok)
        {
            String link_specs = String.format("%s%s%s", "auto", aProducerName, aConsumerName);
            
            is_ok = TheFrameSaverFilter.setParam("link", link_specs);
        }        
        
        return is_ok;        
    }
    
    public boolean startPlaying()
    {
        boolean is_ok = (TheFrameSaverFilter != null);
        
        if (is_ok)
        { 
        	is_ok = TheFrameSaverFilter.startPipelinePlaying();
        }
        
        return is_ok; 
    }    
}