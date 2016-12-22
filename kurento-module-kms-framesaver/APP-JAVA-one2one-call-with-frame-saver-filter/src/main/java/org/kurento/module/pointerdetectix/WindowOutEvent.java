/**
 * This file is generated with Kurento ktool-rom-processor.
 * Please don't edit. Changes should go to kms-interface-rom and
 * ktool-rom-processor templates.
 */
package org.kurento.module.pointerdetectix;

import org.kurento.client.*;

/**
 *
 * Event generated when an object exits a window.
 *
 **/
public class WindowOutEvent extends MediaEvent {

/**
 *
 * Opaque String indicating the id of the window entered
 *
 **/
	private String windowId;

/**
 *
 * Event generated when an object exits a window.
 *
 * @param source
 *       Object that raised the event
 * @param timestamp
 *       
 * @param tags
 *       
 * @param type
 *       Type of event that was raised
 * @param windowId
 *       Opaque String indicating the id of the window entered
 *
 **/
  public WindowOutEvent(@org.kurento.client.internal.server.Param("source") org.kurento.client.MediaObject source, @org.kurento.client.internal.server.Param("timestamp") String timestamp, @org.kurento.client.internal.server.Param("tags") java.util.List<org.kurento.client.Tag> tags, @org.kurento.client.internal.server.Param("type") String type, @org.kurento.client.internal.server.Param("windowId") String windowId) {
    super(source, timestamp, tags, type);
    this.windowId = windowId;
  }

/**
 *
 * Getter for the windowId property
 * @return Opaque String indicating the id of the window entered *
 **/
	public String getWindowId() {
		return windowId;
	}

/**
 *
 * Setter for the windowId property
 *
 * @param windowId
 *       Opaque String indicating the id of the window entered
 *
 **/
	public void setWindowId(String windowId) {
		this.windowId = windowId;
	}

}
