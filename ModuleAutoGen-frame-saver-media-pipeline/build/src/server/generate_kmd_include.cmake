  execute_process (COMMAND /usr/bin/kurento-module-creator -r /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/src/server/interface ;-dr;/usr/share/kurento/modules -o /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/server/)

  file (READ /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/server/FrameSaverMediaPipeline.kmd.json KMD_DATA)

  string (REGEX REPLACE "\n *" "" KMD_DATA ${KMD_DATA})
  string (REPLACE "\"" "\\\"" KMD_DATA ${KMD_DATA})
  string (REPLACE "\\n" "\\\\n" KMD_DATA ${KMD_DATA})
  set (KMD_DATA "\"${KMD_DATA}\"")

  file (WRITE /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/server/FrameSaverMediaPipeline.kmd.json ${KMD_DATA})
