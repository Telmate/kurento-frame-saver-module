  execute_process (COMMAND /usr/bin/kurento-module-creator -r /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/src/server/interface ;-dr;/usr/share/kurento/modules -o /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server/)

  file (READ /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server/kms_frame_saver_video_filter.kmd.json KMD_DATA)

  string (REGEX REPLACE "\n *" "" KMD_DATA ${KMD_DATA})
  string (REPLACE "\"" "\\\"" KMD_DATA ${KMD_DATA})
  string (REPLACE "\\n" "\\\\n" KMD_DATA ${KMD_DATA})
  set (KMD_DATA "\"${KMD_DATA}\"")

  file (WRITE /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server/kms_frame_saver_video_filter.kmd.json ${KMD_DATA})
