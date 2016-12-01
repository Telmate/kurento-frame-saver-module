/**
 * This file is generated with Kurento-maven-plugin.
 * Please don't edit.
 */
package org.kurento.module.kms_frame_saver_video_filter;

import org.kurento.client.*;

/**
 *
 * kms_frame_saver_video_filter interface. Documentation about the module
 *
 **/
@org.kurento.client.internal.RemoteClass
public interface FrameSaverVideoFilter extends Filter {



/**
 *
 * changes pipeline state to PLAYING
 *
 **/
  void startPipelinePlaying();

/**
 *
 * Asynchronous version of startPipelinePlaying:
 * {@link Continuation#onSuccess} is called when the action is
 * done. If an error occurs, {@link Continuation#onError} is called.
 * @see FrameSaverVideoFilter#startPipelinePlaying
 *
 **/
    void startPipelinePlaying(Continuation<Void> cont);

/**
 *
 * changes pipeline state to PLAYING
 *
 **/
    void startPipelinePlaying(Transaction tx);


/**
 *
 * changes pipeline state from PLAYING to READY
 *
 **/
  void stopPipelinePlaying();

/**
 *
 * Asynchronous version of stopPipelinePlaying:
 * {@link Continuation#onSuccess} is called when the action is
 * done. If an error occurs, {@link Continuation#onError} is called.
 * @see FrameSaverVideoFilter#stopPipelinePlaying
 *
 **/
    void stopPipelinePlaying(Continuation<Void> cont);

/**
 *
 * changes pipeline state from PLAYING to READY
 *
 **/
    void stopPipelinePlaying(Transaction tx);


/**
 *
 * gets a string of names of all elements separated by tabs.
 *
 * @param aElementsNamesSeparatedByTabs
 *       string of names of all elements separated by tabs.
 *
 **/
  void getElementsNamesList(@org.kurento.client.internal.server.Param("aElementsNamesSeparatedByTabs") String aElementsNamesSeparatedByTabs);

/**
 *
 * Asynchronous version of getElementsNamesList:
 * {@link Continuation#onSuccess} is called when the action is
 * done. If an error occurs, {@link Continuation#onError} is called.
 * @see FrameSaverVideoFilter#getElementsNamesList
 *
 * @param aElementsNamesSeparatedByTabs
 *       string of names of all elements separated by tabs.
 *
 **/
    void getElementsNamesList(@org.kurento.client.internal.server.Param("aElementsNamesSeparatedByTabs") String aElementsNamesSeparatedByTabs, Continuation<Void> cont);

/**
 *
 * gets a string of names of all elements separated by tabs.
 *
 * @param aElementsNamesSeparatedByTabs
 *       string of names of all elements separated by tabs.
 *
 **/
    void getElementsNamesList(Transaction tx, @org.kurento.client.internal.server.Param("aElementsNamesSeparatedByTabs") String aElementsNamesSeparatedByTabs);


/**
 *
 * gets a string of all parameters separated by tabs.
 *
 * @param aCurrentParamsSeparatedByTabs
 *       string of all parameters separated by tabs.
 *
 **/
  void getParamsList(@org.kurento.client.internal.server.Param("aCurrentParamsSeparatedByTabs") String aCurrentParamsSeparatedByTabs);

/**
 *
 * Asynchronous version of getParamsList:
 * {@link Continuation#onSuccess} is called when the action is
 * done. If an error occurs, {@link Continuation#onError} is called.
 * @see FrameSaverVideoFilter#getParamsList
 *
 * @param aCurrentParamsSeparatedByTabs
 *       string of all parameters separated by tabs.
 *
 **/
    void getParamsList(@org.kurento.client.internal.server.Param("aCurrentParamsSeparatedByTabs") String aCurrentParamsSeparatedByTabs, Continuation<Void> cont);

/**
 *
 * gets a string of all parameters separated by tabs.
 *
 * @param aCurrentParamsSeparatedByTabs
 *       string of all parameters separated by tabs.
 *
 **/
    void getParamsList(Transaction tx, @org.kurento.client.internal.server.Param("aCurrentParamsSeparatedByTabs") String aCurrentParamsSeparatedByTabs);


/**
 *
 * gets the current string value of one parameter.
 *
 * @param aParamName
 *       string with name of parameter.
 * @param aParamValue
 *       string for the current value of the parameter.
 *
 **/
  void getParam(@org.kurento.client.internal.server.Param("aParamName") String aParamName, @org.kurento.client.internal.server.Param("aParamValue") String aParamValue);

/**
 *
 * Asynchronous version of getParam:
 * {@link Continuation#onSuccess} is called when the action is
 * done. If an error occurs, {@link Continuation#onError} is called.
 * @see FrameSaverVideoFilter#getParam
 *
 * @param aParamName
 *       string with name of parameter.
 * @param aParamValue
 *       string for the current value of the parameter.
 *
 **/
    void getParam(@org.kurento.client.internal.server.Param("aParamName") String aParamName, @org.kurento.client.internal.server.Param("aParamValue") String aParamValue, Continuation<Void> cont);

/**
 *
 * gets the current string value of one parameter.
 *
 * @param aParamName
 *       string with name of parameter.
 * @param aParamValue
 *       string for the current value of the parameter.
 *
 **/
    void getParam(Transaction tx, @org.kurento.client.internal.server.Param("aParamName") String aParamName, @org.kurento.client.internal.server.Param("aParamValue") String aParamValue);


/**
 *
 * sets the current string value of one parameter.
 *
 * @param aParamName
 *       string with name of parameter.
 * @param aNewParamValue
 *       string has the desired value of the parameter.
 *
 **/
  void setParam(@org.kurento.client.internal.server.Param("aParamName") String aParamName, @org.kurento.client.internal.server.Param("aNewParamValue") String aNewParamValue);

/**
 *
 * Asynchronous version of setParam:
 * {@link Continuation#onSuccess} is called when the action is
 * done. If an error occurs, {@link Continuation#onError} is called.
 * @see FrameSaverVideoFilter#setParam
 *
 * @param aParamName
 *       string with name of parameter.
 * @param aNewParamValue
 *       string has the desired value of the parameter.
 *
 **/
    void setParam(@org.kurento.client.internal.server.Param("aParamName") String aParamName, @org.kurento.client.internal.server.Param("aNewParamValue") String aNewParamValue, Continuation<Void> cont);

/**
 *
 * sets the current string value of one parameter.
 *
 * @param aParamName
 *       string with name of parameter.
 * @param aNewParamValue
 *       string has the desired value of the parameter.
 *
 **/
    void setParam(Transaction tx, @org.kurento.client.internal.server.Param("aParamName") String aParamName, @org.kurento.client.internal.server.Param("aNewParamValue") String aNewParamValue);

    



    public class Builder extends AbstractBuilder<FrameSaverVideoFilter> {

/**
 *
 * Creates a Builder for FrameSaverVideoFilter
 *
 **/
    public Builder(org.kurento.client.MediaPipeline mediaPipelineParent){

      super(FrameSaverVideoFilter.class,mediaPipelineParent);

      props.add("mediaPipelineParent",mediaPipelineParent);
    }

	public Builder withProperties(Properties properties) {
    	return (Builder)super.withProperties(properties);
  	}

	public Builder with(String name, Object value) {
		return (Builder)super.with(name, value);
	}
	
    }


}