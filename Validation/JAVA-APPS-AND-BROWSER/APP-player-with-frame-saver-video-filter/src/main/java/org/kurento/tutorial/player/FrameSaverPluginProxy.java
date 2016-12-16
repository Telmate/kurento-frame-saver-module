/* 
 * ======================================================================================
 * File:        FrameSaverPluginProxy.c
 * 
 * History:     1. 2016-11-28   JBendor     Created
 *              5. 2016-12-15   JBendor     Updated
 *
 * Description: Implements a proxy for a Kurento module: FrameSaverVideoFilterPlugin
 *
 * Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
 *               Unauthorized copying of this file is strictly prohibited.
 * ======================================================================================
 */

package org.kurento.tutorial.player;

import org.kurento.client.MediaElement;
import org.kurento.client.MediaPipeline;

import org.kurento.module.pointerdetectix.WindowParam;
import org.kurento.module.pointerdetectix.PointerDetectixFilter;
import org.kurento.module.framesavervideofilter.FrameSaverVideoFilter;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;


public class FrameSaverPluginProxy
{
    private String TheDefaultParams[] = { "wait=1000",             // wait 1 second before saving --- forever iff 0
                                          "snap=1000,9,2",         // 1000 ms intervals, limit 9 frames or 2 errors 
                                          "link=auto,auto,auto",   // link in pipeline of the elements to be spliced
                                          "pads=auto,auto,auto",   // pads spliced by TEE (occurs after the waiting period)
                                          "path=auto"              // working folder (frames will be stored in sub-folders)
                                        };  
    
    private static final String KNOWN_FILTERS[] = { "FrameSaver", "PointerDetectix" };
    
    private static final String SELECTED_FILTER = KNOWN_FILTERS[0];
    
    private static final Logger TheLogger = LoggerFactory.getLogger(FrameSaverPluginProxy.class);
    
    private static MediaPipeline           TheMediaPipeline = null;
    
    private static FrameSaverPluginProxy   TheFrameSaverProxy   = null;
    
    private static FrameSaverVideoFilter   TheFramesSaverFilter = null;
    
    private static PointerDetectixFilter   ThePointerDetectix = null;
    
    
    private FrameSaverPluginProxy()     // private c'tor --- singleton class
    {
        return;     
    }

    
    public static FrameSaverPluginProxy newInstance(MediaPipeline aPipeline)
    {       
        if (TheFrameSaverProxy != null)
        {
            return TheFrameSaverProxy;
        }
        
        TheMediaPipeline = aPipeline;

        TheFrameSaverProxy = new FrameSaverPluginProxy();
               
        try
        { 
            if (SELECTED_FILTER.contains("Point"))
            {            
                ThePointerDetectix = new PointerDetectixFilter.Builder(aPipeline, new WindowParam(5, 5, 30, 30)).build();

                TheLogger.info("ThePointerDetectix:    {} CREATED", (ThePointerDetectix != null)  ? "WAS" : "NOT" );
            }
            else if (SELECTED_FILTER.contains("Frame"))
            {
                TheFramesSaverFilter = new FrameSaverVideoFilter.Builder(aPipeline).build();

                TheLogger.info("TheFramesSaverFilter:    {} CREATED", (TheFramesSaverFilter != null)  ? "WAS" : "NOT" );
            }
            else
            {
                TheLogger.info("FILTER NOT CREATED --- UNKNOWN FILTER TYPE" );
            }
        }
        catch (Exception ex)
        {
            TheLogger.info( "EXCEPTION: " + ex.getMessage() );           
        }

        return TheFrameSaverProxy;
    }
    
    
    public boolean connectWith(MediaElement aFromElement, MediaElement aIntoElement)
    {
        boolean is_ok = true;
        
        try
        {
            if (TheFramesSaverFilter != null) 
            {
                aFromElement.connect(TheFramesSaverFilter);
                TheFramesSaverFilter.connect(aIntoElement);
                TheLogger.info( "CONNECTED:   aFromElement-->>--TheFramesSaverFilter-->>--aIntoElement" );           
                String element_names = TheFramesSaverFilter.getElementsNamesList().replaceAll("\t",",");
                TheLogger.info("TheFramesSaverFilter.ElementsNames: ({}) \r\n", element_names);
            }
            else if (ThePointerDetectix != null)
            {
                aFromElement.connect(ThePointerDetectix);
                ThePointerDetectix.connect(aIntoElement);
                TheLogger.info( "CONNECTED:   aFromElement-->>--ThePointerDetectix-->>--aIntoElement" );           
            }
            else
            {
                aFromElement.connect(aIntoElement);
                TheLogger.info( "CONNECTED:   aFromElement-->>--aIntoElement" );           
            }

            if (TheMediaPipeline == null)   // always false --- disabled
            {
                TheLogger.info("PipelineTopology CONNECTED \r\n {} \r\n ------ \r\n", TheMediaPipeline.getGstreamerDot());
            }
        }
        catch(Exception ex)
        {
            TheLogger.info( "EXCEPTION: " + ex.getMessage() );           
            is_ok = false;
        }            
        
        return is_ok; 
    }    

    
    public boolean isUsable()
    {
        return (TheFramesSaverFilter != null) || (ThePointerDetectix != null);
    }
    

    public String getElementNames()
    {
        String element_names = "?";
        
        
        if (TheFramesSaverFilter != null) 
        {
            element_names = TheFramesSaverFilter.getElementsNamesList();
        }       
        else if (ThePointerDetectix != null) 
        {
            element_names = ThePointerDetectix.getElementsNamesList();
        }       

        return element_names;
    }
    
    
    public boolean setOneParam(String aName, String aValue)
    {
        boolean is_ok = false;
        
        if (TheFramesSaverFilter != null) 
        {
            is_ok = TheFramesSaverFilter.setParam(aName, aValue);

            TheLogger.info("FrameSaverPluginProxy.setOneParam: name={}  value={}  ok={}", aName, aValue, is_ok);
        }       
        else if (ThePointerDetectix != null) 
        {
            is_ok = ThePointerDetectix.setParam(aName, aValue);

            TheLogger.info("ThePointerDetectix.setOneParam: name={}  value={}  ok={}", aName, aValue, is_ok);
        }       

        return is_ok;
    }

        
    public boolean setParams(String aParamsArray[])
    {
        boolean is_ok = isUsable();
        
        if (! is_ok)
        {
            return false;               
        }
        
        if (aParamsArray == null)
        {
            aParamsArray = TheDefaultParams;
        }
        
        for (int index=0;  is_ok && (index < aParamsArray.length);  ++index)
        {
            String parts[] = aParamsArray[index].split("=");
            
            is_ok = (parts.length == 2);
            
            if (is_ok)
            { 
            	is_ok = setOneParam(parts[0], parts[1]);
            }
        }
        
        return is_ok;
    }

    
    public boolean setSpliceLinkAfter(MediaElement producerElement)
    {
        String producer_name = "auto";
        String consumer_name = "auto";
        
        boolean is_ok = isUsable();

        if (! is_ok)
        {
            TheLogger.info("ERROR! isUsable()");
            
            return false;
        }
        
        String element_names = getElementNames();
        
        if ( (element_names == null) || element_names.isEmpty() )
        {
            TheLogger.info("ERROR! TheFramesSaverFilter.getElementsNamesList()");
            
            return false;            
        }

        TheLogger.info("ElementsNameList: ({}) \r\n", element_names);
        
        if (is_ok)
        {
            String link_specs = String.format("auto, %s, %s", producer_name, consumer_name);
            
            is_ok = setOneParam("link", link_specs);
        }        
        
        return is_ok;        // TODO
    }
    
    
    public boolean startPlaying()
    {
        boolean is_ok = isUsable();
        
        if (is_ok)
        { 
            is_ok = TheFramesSaverFilter.startPipelinePlaying();
        }
        
        return is_ok; 
    }
}
