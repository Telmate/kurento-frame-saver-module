/* 
* ======================================================================================
* File:        gst_frame_saver_plugin.c
*
* Purpose:     boiler-plate plugin --- uses "Frame_Saver_Filter" for behavior.
* 
* History:     1. 2016-11-05   JBendor     Created    
*              2. 2016-11-06   JBendor     Updated 
*
* Copyright (c) 2016 TELMATE INC. All Rights Reserved. Proprietary and confidential.
*               Unauthorized copying of this file is strictly prohibited.
* ======================================================================================
*/

/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2016 jonny <<user@hostname.org>>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-frame-saver-plugin
 *
 * TODO: FrameSaverPlugin_Description
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! FrameSaverPlugin ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

 #ifndef _HAVE_PLUGIN_CONFIG_
    #ifndef VERSION
        #define VERSION "1.0.0"
    #endif
    #ifndef PACKAGE
        #define PACKAGE "frame_saver_plugin_package"     
    #endif
#else 
    #include "plugin_config.h"
#endif

#ifdef _CYGWIN     // prevent CYGWIN's compiler warnings
    int cygwin_static_assert(int is_true) { return is_true; }
    #ifdef G_STATIC_ASSERT
        #undef G_STATIC_ASSERT
    #endif
    #define G_STATIC_ASSERT(condition)  cygwin_static_assert( (condition) )
    #define __GI_SCANNER__
#endif

#include <stdio.h>
#include <gst/gst.h>
#include <glib/gmacros.h>
#include <glib/gatomic.h>


#ifdef _ALLOW_DYNAMIC_PARAMS_
    // These headers are needed for Dynamic-Controllable-Parameters
    // The controller allows changing Gobject properties while streaming
    // Applications must link to the shared library "gstreamer-controller"
    #include <gst/controller/gstargbcontrolbinding.h>
    #include <gst/controller/gstdirectcontrolbinding.h>
    #include <gst/controller/gsttimedvaluecontrolsource.h>
    #include <gst/controller/gstinterpolationcontrolsource.h>
    #include <gst/controller/gsttriggercontrolsource.h>
    #include <gst/controller/gstlfocontrolsource.h>

    #define NANOS_PER_MILLISEC  (1000L * 1000L)

    #define DYNAMIC_SYNC_NANOS  (NANOS_PER_MILLISEC * 1000)
#endif


#ifdef _DUMMY_PLUGIN_LIB
    static int Frame_Saver_Filter_Lookup(const void * pluginPtr) { return 0; }
    static int Frame_Saver_Filter_Set_Params(GstElement * pluginPtr, const GValue * valuePtr, GParamSpec * specPtr) { return 0; }
    static int Frame_Saver_Filter_Get_Params(GstElement * pluginPtr, GValue * valuePtr, GParamSpec * specPtr) { return 0; }
    static int Frame_Saver_Filter_Transition(GstElement * pluginPtr, GstStateChange aTransition) { return 0; }
    static int Frame_Saver_Filter_Attach(GstElement * pluginPtr) { return 0; }
    static int Frame_Saver_Filter_Detach(GstElement * pluginPtr) { return 0; }
#else
    #include "frame_saver_filter.h"
#endif



/* boiler-plate plugin-element-instance structure */
typedef struct _GstFrameSaverPlugin
{
  GstElement element;
  GstPad *sinkpad, *srcpad;
  gboolean silent;

} GstFrameSaverPlugin;


/* boiler-plate plugin-element-class structure */
typedef struct _GstFrameSaverPluginClass 
{
  GstElementClass parent_class;

} GstFrameSaverPluginClass;


G_DEFINE_TYPE(GstFrameSaverPlugin, gst_frame_saver_plugin, GST_TYPE_ELEMENT);   // declares several static functions

GST_DEBUG_CATEGORY_STATIC (gst_frame_saver_plugin_debug);

#define GST_CAT_DEFAULT gst_frame_saver_plugin_debug

extern GType gst_frame_saver_plugin_get_type (void);

#define GST_TYPE_OF_FRAME_SAVER_PLUGIN            (gst_frame_saver_plugin_get_type())

#define GST_FRAME_SAVER_PLUGIN(obj)               (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_OF_FRAME_SAVER_PLUGIN, GstFrameSaverPlugin))

#define GST_FRAME_SAVER_PLUGIN_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST((klass),  GST_TYPE_OF_FRAME_SAVER_PLUGIN, GstFrameSaverPluginClass))

#define GST_IS_FRAME_SAVER_PLUGIN(obj)            (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_OF_FRAME_SAVER_PLUGIN))

#define GST_IS_FRAME_SAVER_PLUGIN_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE((klass),  GST_TYPE_OF_FRAMES_AVER_PLUGIN))


enum
{
  e_FIRST_SIGNAL = 0,
  e_FINAL_SIGNAL = e_FIRST_SIGNAL
  
} PLUGIN_SIGNALS_e;

enum
{
  PROP_0,
  e_PROP_SILENT,
  e_PROP_SPLICE
  
} PLUGIN_PARAMS_e;


/* the capabilities of the inputs and outputs. describe the real formats here. */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
                                                                    GST_PAD_SINK,
                                                                    GST_PAD_ALWAYS,
                                                                    GST_STATIC_CAPS ("ANY") );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
                                                                    GST_PAD_SRC,
                                                                    GST_PAD_ALWAYS,
                                                                    GST_STATIC_CAPS ("ANY") );

static void gst_frame_saver_plugin_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);

static void gst_frame_saver_plugin_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);

static gboolean gst_frame_saver_plugin_sink_event (GstPad * pad, GstObject * parent, GstEvent * event);

static GstFlowReturn gst_frame_saver_plugin_chain (GstPad * pad, GstObject * parent, GstBuffer * buf);

static GstStateChangeReturn gst_frame_saver_plugin_change_state (GstElement *element, GstStateChange transition);

static GstElementClass * This_Plugin_Element_Class_ptr = NULL;

static GstElementClass * This_Plugin_Element_Parent_Class_ptr = NULL; 


/* GObject vmethod implementations */

/* initialize the framesaverplugin's class */
static void gst_frame_saver_plugin_class_init (GstFrameSaverPluginClass * klass)
{
  GObjectClass * gobject_class = (GObjectClass *) klass;

  gobject_class->set_property  = gst_frame_saver_plugin_set_property;

  gobject_class->get_property  = gst_frame_saver_plugin_get_property;
  
  if (This_Plugin_Element_Class_ptr == NULL)
  {
      This_Plugin_Element_Class_ptr = GST_ELEMENT_CLASS (klass);

      This_Plugin_Element_Parent_Class_ptr = GST_ELEMENT_CLASS(&This_Plugin_Element_Class_ptr->parent_class);

      This_Plugin_Element_Class_ptr->change_state = gst_frame_saver_plugin_change_state;
  }
  else if (This_Plugin_Element_Class_ptr != (GstElementClass *) klass)
  {
      ; // TODO --- should be impossible
  }

  g_object_class_install_property (gobject_class, 
                                   e_PROP_SILENT,
                                   g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
                                   FALSE, 
                                   G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, 
                                   e_PROP_SPLICE,
                                   g_param_spec_string ("params", "Params", "change runtime parameters",
                                   FALSE, 
                                   G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE | G_PARAM_STATIC_STRINGS));


  gst_element_class_set_details_simple( This_Plugin_Element_Class_ptr,
                                        "FrameSaverPlugin",
                                        "TODO: Generic",
                                        "TODO: Generic Template Element",
                                        "TODO! <<user@hostname.org>>" );

  gst_element_class_add_pad_template (This_Plugin_Element_Class_ptr, gst_static_pad_template_get (&src_factory));

  gst_element_class_add_pad_template (This_Plugin_Element_Class_ptr, gst_static_pad_template_get (&sink_factory));
}


static gboolean gst_my_filter_src_query (GstPad *pad, GstObject *parent, GstQuery *query)
{
  gboolean ret;

  switch (GST_QUERY_TYPE (query)) 
  {
    case GST_QUERY_CAPS:      /* report the supported caps here */
    default:
      ret = gst_pad_query_default (pad, parent, query);
      break;
  }
  return ret;
}


static GstStateChangeReturn gst_frame_saver_plugin_change_state (GstElement *element, GstStateChange transition)
{
  GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;

  if (transition == GST_STATE_CHANGE_NULL_TO_READY)
  {
      Frame_Saver_Filter_Attach(element);
  }

  Frame_Saver_Filter_Transition( element, transition ); 

  if (This_Plugin_Element_Parent_Class_ptr->change_state != NULL)
  {
      ret = This_Plugin_Element_Parent_Class_ptr->change_state(element, transition);
  }
  
  if (ret != GST_STATE_CHANGE_FAILURE)
  {
      if (transition == GST_STATE_CHANGE_READY_TO_NULL)
      {
          Frame_Saver_Filter_Detach(element);
      }
  }

  return ret;
}


/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void gst_frame_saver_plugin_init (GstFrameSaverPlugin * filter)
{
  filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");

  gst_pad_set_event_function (filter->sinkpad, GST_DEBUG_FUNCPTR(gst_frame_saver_plugin_sink_event));

  gst_pad_set_chain_function (filter->sinkpad, GST_DEBUG_FUNCPTR(gst_frame_saver_plugin_chain));

  gst_pad_set_query_function (filter->srcpad, gst_my_filter_src_query);

  GST_PAD_SET_PROXY_CAPS (filter->sinkpad);

  gst_element_add_pad (GST_ELEMENT(filter), filter->sinkpad);

  filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");

  GST_PAD_SET_PROXY_CAPS (filter->srcpad);

  gst_element_add_pad (GST_ELEMENT(filter), filter->srcpad);

  filter->silent = FALSE;
  
  Frame_Saver_Filter_Attach( GST_ELEMENT(filter) );
}


static void gst_frame_saver_plugin_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec)
{
  GstFrameSaverPlugin * filter = GST_FRAME_SAVER_PLUGIN (object);
    
#ifdef _ALLOW_DYNAMIC_PARAMS_
    gst_object_sync_values(filter, DYNAMIC_SYNC_NANOS);
#endif

  switch (prop_id) 
  {
    case e_PROP_SPLICE:
        Frame_Saver_Filter_Set_Params(GST_ELEMENT(filter), value, pspec);
      break;

    case e_PROP_SILENT:
      filter->silent = g_value_get_boolean (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}


static void gst_frame_saver_plugin_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec)
{
  GstFrameSaverPlugin *filter = GST_FRAME_SAVER_PLUGIN (object);

  switch (prop_id) 
  {
    case e_PROP_SPLICE:
      Frame_Saver_Filter_Get_Params(GST_ELEMENT(filter), value, pspec);
      break;

    case e_PROP_SILENT:
      g_value_set_boolean (value, filter->silent);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean gst_frame_saver_plugin_sink_event (GstPad * pad, GstObject * parent, GstEvent * event)
{
  gboolean  ret;
  GstCaps * caps;
  GstFrameSaverPlugin * filter = GST_FRAME_SAVER_PLUGIN (parent);

  GST_LOG_OBJECT (filter, "Received %s event: %" GST_PTR_FORMAT, GST_EVENT_TYPE_NAME (event), event);

  switch (GST_EVENT_TYPE (event)) 
  {
    case GST_EVENT_CAPS: /* do something with the caps --- forward */
      gst_event_parse_caps (event, &caps);
      ret = gst_pad_event_default (pad, parent, event);
      break;

    case GST_EVENT_EOS: /* end-of-stream, close down all stream leftovers here */
      ret = gst_pad_event_default (pad, parent, event);
      break;

    default:
      ret = gst_pad_event_default (pad, parent, event);
      break;
  }
  return ret;
}

/* chain function --- this function does the actual processing */
static GstFlowReturn gst_frame_saver_plugin_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
  GstFrameSaverPlugin * filter = GST_FRAME_SAVER_PLUGIN (parent);

  if (filter->silent == FALSE)
    g_print ("FrameSaverPlugin is plugged in. \n");

  /* just push out the incoming buffer without touching it */
  return gst_pad_push (filter->srcpad, buf);
}


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean frame_saver_plugin_init (GstPlugin * framesaverplugin)
{
#ifdef _ALLOW_DYNAMIC_PARAMS_
  gst_controller_init (NULL, NULL);
#endif

  /* debug category for filtering log messages   */
  GST_DEBUG_CATEGORY_INIT (gst_frame_saver_plugin_debug, "FrameSaverPlugin", 0, "FrameSaverPlugin_Description");

  return gst_element_register (framesaverplugin, "FrameSaverPlugin", GST_RANK_NONE, GST_TYPE_OF_FRAME_SAVER_PLUGIN);
}


#if 1
/* gstreamer looks for this structure to register plugins */
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    FrameSaverPlugin,
    "FrameSaverPlugin_Description",
    frame_saver_plugin_init,
    VERSION,
    "LGPL",
    "GStreamer",
    "http://gstreamer.net/"
)
#endif
