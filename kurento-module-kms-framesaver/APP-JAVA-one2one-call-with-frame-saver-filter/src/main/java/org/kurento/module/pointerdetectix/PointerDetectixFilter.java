/**
 * This file is generated with Kurento-maven-plugin.
 * Please don't edit.
 */
package org.kurento.module.pointerdetectix;

import org.kurento.client.*;

/**
 *
 * This type of {@link module:core/abstracts.Filter Filter} detects UI pointers in a video feed.
 *
 **/
@org.kurento.client.internal.RemoteClass
public interface PointerDetectixFilter extends Filter {



/**
 *
 * changes pipeline state to PLAYING
 * @return FALSE when Failed. *
 **/
  boolean startPipelinePlaying();

/**
 *
 * Asynchronous version of startPipelinePlaying:
 * {@link Continuation#onSuccess} is called when the action is
 * done. If an error occurs, {@link Continuation#onError} is called.
 * @see PointerDetectixFilter#startPipelinePlaying
 *
 **/
    void startPipelinePlaying(Continuation<Boolean> cont);

/**
 *
 * changes pipeline state to PLAYING
 * @return FALSE when Failed. *
 **/
    TFuture<Boolean> startPipelinePlaying(Transaction tx);


/**
 *
 * changes pipeline state from PLAYING to READY
 * @return FALSE when Failed. *
 **/
  boolean stopPipelinePlaying();

/**
 *
 * Asynchronous version of stopPipelinePlaying:
 * {@link Continuation#onSuccess} is called when the action is
 * done. If an error occurs, {@link Continuation#onError} is called.
 * @see PointerDetectixFilter#stopPipelinePlaying
 *
 **/
    void stopPipelinePlaying(Continuation<Boolean> cont);

/**
 *
 * changes pipeline state from PLAYING to READY
 * @return FALSE when Failed. *
 **/
    TFuture<Boolean> stopPipelinePlaying(Transaction tx);


/**
 *
 * gets a string of names of all elements separated by tabs.
 * @return names of all elements separated by tabs --- pipeline name is the first element *
 **/
  String getElementsNamesList();

/**
 *
 * Asynchronous version of getElementsNamesList:
 * {@link Continuation#onSuccess} is called when the action is
 * done. If an error occurs, {@link Continuation#onError} is called.
 * @see PointerDetectixFilter#getElementsNamesList
 *
 **/
    void getElementsNamesList(Continuation<String> cont);

/**
 *
 * gets a string of names of all elements separated by tabs.
 * @return names of all elements separated by tabs --- pipeline name is the first element *
 **/
    TFuture<String> getElementsNamesList(Transaction tx);


/**
 *
 * gets string of last error --- empty string when no error
 * @return string of last error --- empty string when no error *
 **/
  String getLastError();

/**
 *
 * Asynchronous version of getLastError:
 * {@link Continuation#onSuccess} is called when the action is
 * done. If an error occurs, {@link Continuation#onError} is called.
 * @see PointerDetectixFilter#getLastError
 *
 **/
    void getLastError(Continuation<String> cont);

/**
 *
 * gets string of last error --- empty string when no error
 * @return string of last error --- empty string when no error *
 **/
    TFuture<String> getLastError(Transaction tx);


/**
 *
 * gets a string of all parameters separated by tabs.
 * @return all parameters separated by tabs --- each one is: name=value *
 **/
  String getParamsList();

/**
 *
 * Asynchronous version of getParamsList:
 * {@link Continuation#onSuccess} is called when the action is
 * done. If an error occurs, {@link Continuation#onError} is called.
 * @see PointerDetectixFilter#getParamsList
 *
 **/
    void getParamsList(Continuation<String> cont);

/**
 *
 * gets a string of all parameters separated by tabs.
 * @return all parameters separated by tabs --- each one is: name=value *
 **/
    TFuture<String> getParamsList(Transaction tx);


/**
 *
 * gets the current string value of one parameter.
 *
 * @param aParamName
 *       string with name of parameter.
 * @return current value of named parameter --- empty if invalid name *
 **/
  String getParam(@org.kurento.client.internal.server.Param("aParamName") String aParamName);

/**
 *
 * Asynchronous version of getParam:
 * {@link Continuation#onSuccess} is called when the action is
 * done. If an error occurs, {@link Continuation#onError} is called.
 * @see PointerDetectixFilter#getParam
 *
 * @param aParamName
 *       string with name of parameter.
 *
 **/
    void getParam(@org.kurento.client.internal.server.Param("aParamName") String aParamName, Continuation<String> cont);

/**
 *
 * gets the current string value of one parameter.
 *
 * @param aParamName
 *       string with name of parameter.
 * @return current value of named parameter --- empty if invalid name *
 **/
    TFuture<String> getParam(Transaction tx, @org.kurento.client.internal.server.Param("aParamName") String aParamName);


/**
 *
 * sets the current string value of one parameter.
 *
 * @param aParamName
 *       string with name of parameter.
 * @param aNewParamValue
 *       string has the desired value of the parameter.
 * @return FALSE when Failed. *
 **/
  boolean setParam(@org.kurento.client.internal.server.Param("aParamName") String aParamName, @org.kurento.client.internal.server.Param("aNewParamValue") String aNewParamValue);

/**
 *
 * Asynchronous version of setParam:
 * {@link Continuation#onSuccess} is called when the action is
 * done. If an error occurs, {@link Continuation#onError} is called.
 * @see PointerDetectixFilter#setParam
 *
 * @param aParamName
 *       string with name of parameter.
 * @param aNewParamValue
 *       string has the desired value of the parameter.
 *
 **/
    void setParam(@org.kurento.client.internal.server.Param("aParamName") String aParamName, @org.kurento.client.internal.server.Param("aNewParamValue") String aNewParamValue, Continuation<Boolean> cont);

/**
 *
 * sets the current string value of one parameter.
 *
 * @param aParamName
 *       string with name of parameter.
 * @param aNewParamValue
 *       string has the desired value of the parameter.
 * @return FALSE when Failed. *
 **/
    TFuture<Boolean> setParam(Transaction tx, @org.kurento.client.internal.server.Param("aParamName") String aParamName, @org.kurento.client.internal.server.Param("aNewParamValue") String aNewParamValue);


/**
 *
 *  Adds a new detection window for the filter to detect pointers entering or exiting the window
 *
 * @param window
 *       The window to be added
 *
 **/
  void addWindow(@org.kurento.client.internal.server.Param("window") org.kurento.module.pointerdetectix.PointerDetectixWindowMediaParam window);

/**
 *
 * Asynchronous version of addWindow:
 * {@link Continuation#onSuccess} is called when the action is
 * done. If an error occurs, {@link Continuation#onError} is called.
 * @see PointerDetectixFilter#addWindow
 *
 * @param window
 *       The window to be added
 *
 **/
    void addWindow(@org.kurento.client.internal.server.Param("window") org.kurento.module.pointerdetectix.PointerDetectixWindowMediaParam window, Continuation<Void> cont);

/**
 *
 *  Adds a new detection window for the filter to detect pointers entering or exiting the window
 *
 * @param window
 *       The window to be added
 *
 **/
    void addWindow(Transaction tx, @org.kurento.client.internal.server.Param("window") org.kurento.module.pointerdetectix.PointerDetectixWindowMediaParam window);


/**
 *
 * Removes all pointer detectix windows
 *
 **/
  void clearWindows();

/**
 *
 * Asynchronous version of clearWindows:
 * {@link Continuation#onSuccess} is called when the action is
 * done. If an error occurs, {@link Continuation#onError} is called.
 * @see PointerDetectixFilter#clearWindows
 *
 **/
    void clearWindows(Continuation<Void> cont);

/**
 *
 * Removes all pointer detectix windows
 *
 **/
    void clearWindows(Transaction tx);


/**
 *
 * This method allows to calibrate the tracking color.
 * The new tracking color will be the color of the object in the colorCalibrationRegion.
 *
 **/
  void trackColorFromCalibrationRegion();

/**
 *
 * Asynchronous version of trackColorFromCalibrationRegion:
 * {@link Continuation#onSuccess} is called when the action is
 * done. If an error occurs, {@link Continuation#onError} is called.
 * @see PointerDetectixFilter#trackColorFromCalibrationRegion
 *
 **/
    void trackColorFromCalibrationRegion(Continuation<Void> cont);

/**
 *
 * This method allows to calibrate the tracking color.
 * The new tracking color will be the color of the object in the colorCalibrationRegion.
 *
 **/
    void trackColorFromCalibrationRegion(Transaction tx);


/**
 *
 * Removes a window from the list to be monitored
 *
 * @param windowId
 *       the id of the window to be removed
 *
 **/
  void removeWindow(@org.kurento.client.internal.server.Param("windowId") String windowId);

/**
 *
 * Asynchronous version of removeWindow:
 * {@link Continuation#onSuccess} is called when the action is
 * done. If an error occurs, {@link Continuation#onError} is called.
 * @see PointerDetectixFilter#removeWindow
 *
 * @param windowId
 *       the id of the window to be removed
 *
 **/
    void removeWindow(@org.kurento.client.internal.server.Param("windowId") String windowId, Continuation<Void> cont);

/**
 *
 * Removes a window from the list to be monitored
 *
 * @param windowId
 *       the id of the window to be removed
 *
 **/
    void removeWindow(Transaction tx, @org.kurento.client.internal.server.Param("windowId") String windowId);

    /**
     * Add a {@link EventListener} for event {@link WindowInEvent}. Synchronous call.
     *
     * @param  listener Listener to be called on WindowInEvent
     * @return ListenerSubscription for the given Listener
     *
     **/
    @org.kurento.client.internal.server.EventSubscription(WindowInEvent.class)
    ListenerSubscription addWindowInListener(EventListener<WindowInEvent> listener);
    /**
     * Add a {@link EventListener} for event {@link WindowInEvent}. Asynchronous call.
     * Calls Continuation&lt;ListenerSubscription&gt; when it has been added.
     *
     * @param listener Listener to be called on WindowInEvent
     * @param cont     Continuation to be called when the listener is registered
     *
     **/
    @org.kurento.client.internal.server.EventSubscription(WindowInEvent.class)
    void addWindowInListener(EventListener<WindowInEvent> listener, Continuation<ListenerSubscription> cont);
    
	/**
     * Remove a {@link ListenerSubscription} for event {@link WindowInEvent}. Synchronous call.
     *
     * @param listenerSubscription Listener subscription to be removed
     *
     **/
    @org.kurento.client.internal.server.EventSubscription(WindowInEvent.class)
    void removeWindowInListener(ListenerSubscription listenerSubscription);
    /**
     * Remove a {@link ListenerSubscription} for event {@link WindowInEvent}. Asynchronous call.
     * Calls Continuation&lt;Void&gt; when it has been removed.
     *
     * @param listenerSubscription Listener subscription to be removed
     * @param cont                 Continuation to be called when the listener is removed
     *
     **/
    @org.kurento.client.internal.server.EventSubscription(WindowInEvent.class)
    void removeWindowInListener(ListenerSubscription listenerSubscription, Continuation<Void> cont);
    /**
     * Add a {@link EventListener} for event {@link WindowOutEvent}. Synchronous call.
     *
     * @param  listener Listener to be called on WindowOutEvent
     * @return ListenerSubscription for the given Listener
     *
     **/
    @org.kurento.client.internal.server.EventSubscription(WindowOutEvent.class)
    ListenerSubscription addWindowOutListener(EventListener<WindowOutEvent> listener);
    /**
     * Add a {@link EventListener} for event {@link WindowOutEvent}. Asynchronous call.
     * Calls Continuation&lt;ListenerSubscription&gt; when it has been added.
     *
     * @param listener Listener to be called on WindowOutEvent
     * @param cont     Continuation to be called when the listener is registered
     *
     **/
    @org.kurento.client.internal.server.EventSubscription(WindowOutEvent.class)
    void addWindowOutListener(EventListener<WindowOutEvent> listener, Continuation<ListenerSubscription> cont);
    
	/**
     * Remove a {@link ListenerSubscription} for event {@link WindowOutEvent}. Synchronous call.
     *
     * @param listenerSubscription Listener subscription to be removed
     *
     **/
    @org.kurento.client.internal.server.EventSubscription(WindowOutEvent.class)
    void removeWindowOutListener(ListenerSubscription listenerSubscription);
    /**
     * Remove a {@link ListenerSubscription} for event {@link WindowOutEvent}. Asynchronous call.
     * Calls Continuation&lt;Void&gt; when it has been removed.
     *
     * @param listenerSubscription Listener subscription to be removed
     * @param cont                 Continuation to be called when the listener is removed
     *
     **/
    @org.kurento.client.internal.server.EventSubscription(WindowOutEvent.class)
    void removeWindowOutListener(ListenerSubscription listenerSubscription, Continuation<Void> cont);
    



    public class Builder extends AbstractBuilder<PointerDetectixFilter> {

/**
 *
 * Creates a Builder for PointerDetectixFilter
 *
 **/
    public Builder(org.kurento.client.MediaPipeline mediaPipeline, org.kurento.module.pointerdetectix.WindowParam calibrationRegion){

      super(PointerDetectixFilter.class,mediaPipeline);

      props.add("mediaPipeline",mediaPipeline);
      props.add("calibrationRegion",calibrationRegion);
    }

	public Builder withProperties(Properties properties) {
    	return (Builder)super.withProperties(properties);
  	}

	public Builder with(String name, Object value) {
		return (Builder)super.with(name, value);
	}
	
/**
 *
 * Sets a value for windows in Builder for PointerDetectixFilter.
 *
 * @param windows
 *       list of detection windows for the filter.
 *
 **/
    public Builder withWindows(java.util.List<org.kurento.module.pointerdetectix.PointerDetectixWindowMediaParam> windows){
      props.add("windows",windows);
      return this;
    }
    }


}