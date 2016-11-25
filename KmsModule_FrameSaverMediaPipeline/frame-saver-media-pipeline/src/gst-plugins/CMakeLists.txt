include_directories(
  ${GSTREAMER_INCLUDE_DIRS}
  ${GSTREAMER_VIDEO_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${OPENCV_INCLUDE_DIRS}
)

set(FRAME_SAVER_SOURCES
  frame_saver/frame_saver_filter.c
  frame_saver/frame_saver_filter.c
  frame_saver/frame_saver_filter_lib.c
  frame_saver/frame_saver_filter_lib.h
  frame_saver/frame_saver_params.c
  frame_saver/frame_saver_params.h
  frame_saver/save_frames_as_png.c
  frame_saver/save_frames_as_png.h
  frame_saver/wrapped_natives.c
  frame_saver/wrapped_natives.h
)

set(FRAME_SAVER_LIBS
   z
   m
   png12
   pthread
   gstapp-1.5
)

set(FRAME_SAVER_PLUGIN_SOURCES
  gst_Frame_Saver_Plugin.c
  gst_Frame_Saver_Plugin.h
  ${FRAME_SAVER_SOURCES}
)


add_library(framesavermediapipeline MODULE ${FRAME_SAVER_PLUGIN_SOURCES})

target_link_libraries(framesavermediapipeline
  ${GSTREAMER_LIBRARIES}
  ${GSTREAMER_VIDEO_LIBRARIES}
  ${OPENCV_LIBRARIES}
  ${FRAME_SAVER_LIBS}
)

install(
  TARGETS framesavermediapipeline
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_GST_PLUGINS_DIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

