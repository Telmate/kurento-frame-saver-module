# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build

# Include any dependencies generated for this target.
include src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/depend.make

# Include the progress variables for this target.
include src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/progress.make

# Include the compile flags for this target's objects.
include src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/flags.make

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/gst_Frame_Saver_Plugin.c.o: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/flags.make
src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/gst_Frame_Saver_Plugin.c.o: ../src/gst-plugins/gst_Frame_Saver_Plugin.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/gst_Frame_Saver_Plugin.c.o"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/gst-plugins && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/framesavermediapipeline.dir/gst_Frame_Saver_Plugin.c.o   -c /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/src/gst-plugins/gst_Frame_Saver_Plugin.c

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/gst_Frame_Saver_Plugin.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/framesavermediapipeline.dir/gst_Frame_Saver_Plugin.c.i"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/gst-plugins && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/src/gst-plugins/gst_Frame_Saver_Plugin.c > CMakeFiles/framesavermediapipeline.dir/gst_Frame_Saver_Plugin.c.i

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/gst_Frame_Saver_Plugin.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/framesavermediapipeline.dir/gst_Frame_Saver_Plugin.c.s"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/gst-plugins && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/src/gst-plugins/gst_Frame_Saver_Plugin.c -o CMakeFiles/framesavermediapipeline.dir/gst_Frame_Saver_Plugin.c.s

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/gst_Frame_Saver_Plugin.c.o.requires:
.PHONY : src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/gst_Frame_Saver_Plugin.c.o.requires

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/gst_Frame_Saver_Plugin.c.o.provides: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/gst_Frame_Saver_Plugin.c.o.requires
	$(MAKE) -f src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/build.make src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/gst_Frame_Saver_Plugin.c.o.provides.build
.PHONY : src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/gst_Frame_Saver_Plugin.c.o.provides

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/gst_Frame_Saver_Plugin.c.o.provides.build: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/gst_Frame_Saver_Plugin.c.o

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter.c.o: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/flags.make
src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter.c.o: ../src/gst-plugins/frame_saver/frame_saver_filter.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter.c.o"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/gst-plugins && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter.c.o   -c /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/src/gst-plugins/frame_saver/frame_saver_filter.c

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter.c.i"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/gst-plugins && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/src/gst-plugins/frame_saver/frame_saver_filter.c > CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter.c.i

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter.c.s"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/gst-plugins && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/src/gst-plugins/frame_saver/frame_saver_filter.c -o CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter.c.s

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter.c.o.requires:
.PHONY : src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter.c.o.requires

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter.c.o.provides: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter.c.o.requires
	$(MAKE) -f src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/build.make src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter.c.o.provides.build
.PHONY : src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter.c.o.provides

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter.c.o.provides.build: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter.c.o

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter_lib.c.o: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/flags.make
src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter_lib.c.o: ../src/gst-plugins/frame_saver/frame_saver_filter_lib.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter_lib.c.o"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/gst-plugins && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter_lib.c.o   -c /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/src/gst-plugins/frame_saver/frame_saver_filter_lib.c

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter_lib.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter_lib.c.i"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/gst-plugins && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/src/gst-plugins/frame_saver/frame_saver_filter_lib.c > CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter_lib.c.i

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter_lib.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter_lib.c.s"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/gst-plugins && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/src/gst-plugins/frame_saver/frame_saver_filter_lib.c -o CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter_lib.c.s

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter_lib.c.o.requires:
.PHONY : src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter_lib.c.o.requires

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter_lib.c.o.provides: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter_lib.c.o.requires
	$(MAKE) -f src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/build.make src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter_lib.c.o.provides.build
.PHONY : src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter_lib.c.o.provides

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter_lib.c.o.provides.build: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter_lib.c.o

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_params.c.o: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/flags.make
src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_params.c.o: ../src/gst-plugins/frame_saver/frame_saver_params.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_params.c.o"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/gst-plugins && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_params.c.o   -c /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/src/gst-plugins/frame_saver/frame_saver_params.c

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_params.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_params.c.i"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/gst-plugins && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/src/gst-plugins/frame_saver/frame_saver_params.c > CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_params.c.i

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_params.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_params.c.s"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/gst-plugins && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/src/gst-plugins/frame_saver/frame_saver_params.c -o CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_params.c.s

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_params.c.o.requires:
.PHONY : src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_params.c.o.requires

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_params.c.o.provides: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_params.c.o.requires
	$(MAKE) -f src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/build.make src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_params.c.o.provides.build
.PHONY : src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_params.c.o.provides

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_params.c.o.provides.build: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_params.c.o

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/save_frames_as_png.c.o: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/flags.make
src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/save_frames_as_png.c.o: ../src/gst-plugins/frame_saver/save_frames_as_png.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/save_frames_as_png.c.o"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/gst-plugins && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/framesavermediapipeline.dir/frame_saver/save_frames_as_png.c.o   -c /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/src/gst-plugins/frame_saver/save_frames_as_png.c

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/save_frames_as_png.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/framesavermediapipeline.dir/frame_saver/save_frames_as_png.c.i"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/gst-plugins && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/src/gst-plugins/frame_saver/save_frames_as_png.c > CMakeFiles/framesavermediapipeline.dir/frame_saver/save_frames_as_png.c.i

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/save_frames_as_png.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/framesavermediapipeline.dir/frame_saver/save_frames_as_png.c.s"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/gst-plugins && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/src/gst-plugins/frame_saver/save_frames_as_png.c -o CMakeFiles/framesavermediapipeline.dir/frame_saver/save_frames_as_png.c.s

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/save_frames_as_png.c.o.requires:
.PHONY : src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/save_frames_as_png.c.o.requires

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/save_frames_as_png.c.o.provides: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/save_frames_as_png.c.o.requires
	$(MAKE) -f src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/build.make src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/save_frames_as_png.c.o.provides.build
.PHONY : src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/save_frames_as_png.c.o.provides

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/save_frames_as_png.c.o.provides.build: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/save_frames_as_png.c.o

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/wrapped_natives.c.o: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/flags.make
src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/wrapped_natives.c.o: ../src/gst-plugins/frame_saver/wrapped_natives.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/CMakeFiles $(CMAKE_PROGRESS_6)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/wrapped_natives.c.o"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/gst-plugins && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/framesavermediapipeline.dir/frame_saver/wrapped_natives.c.o   -c /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/src/gst-plugins/frame_saver/wrapped_natives.c

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/wrapped_natives.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/framesavermediapipeline.dir/frame_saver/wrapped_natives.c.i"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/gst-plugins && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/src/gst-plugins/frame_saver/wrapped_natives.c > CMakeFiles/framesavermediapipeline.dir/frame_saver/wrapped_natives.c.i

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/wrapped_natives.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/framesavermediapipeline.dir/frame_saver/wrapped_natives.c.s"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/gst-plugins && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/src/gst-plugins/frame_saver/wrapped_natives.c -o CMakeFiles/framesavermediapipeline.dir/frame_saver/wrapped_natives.c.s

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/wrapped_natives.c.o.requires:
.PHONY : src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/wrapped_natives.c.o.requires

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/wrapped_natives.c.o.provides: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/wrapped_natives.c.o.requires
	$(MAKE) -f src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/build.make src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/wrapped_natives.c.o.provides.build
.PHONY : src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/wrapped_natives.c.o.provides

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/wrapped_natives.c.o.provides.build: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/wrapped_natives.c.o

# Object files for target framesavermediapipeline
framesavermediapipeline_OBJECTS = \
"CMakeFiles/framesavermediapipeline.dir/gst_Frame_Saver_Plugin.c.o" \
"CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter.c.o" \
"CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter_lib.c.o" \
"CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_params.c.o" \
"CMakeFiles/framesavermediapipeline.dir/frame_saver/save_frames_as_png.c.o" \
"CMakeFiles/framesavermediapipeline.dir/frame_saver/wrapped_natives.c.o"

# External object files for target framesavermediapipeline
framesavermediapipeline_EXTERNAL_OBJECTS =

src/gst-plugins/libframesavermediapipeline.so: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/gst_Frame_Saver_Plugin.c.o
src/gst-plugins/libframesavermediapipeline.so: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter.c.o
src/gst-plugins/libframesavermediapipeline.so: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter_lib.c.o
src/gst-plugins/libframesavermediapipeline.so: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_params.c.o
src/gst-plugins/libframesavermediapipeline.so: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/save_frames_as_png.c.o
src/gst-plugins/libframesavermediapipeline.so: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/wrapped_natives.c.o
src/gst-plugins/libframesavermediapipeline.so: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/build.make
src/gst-plugins/libframesavermediapipeline.so: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C shared module libframesavermediapipeline.so"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/gst-plugins && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/framesavermediapipeline.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/build: src/gst-plugins/libframesavermediapipeline.so
.PHONY : src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/build

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/requires: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/gst_Frame_Saver_Plugin.c.o.requires
src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/requires: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter.c.o.requires
src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/requires: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_filter_lib.c.o.requires
src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/requires: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/frame_saver_params.c.o.requires
src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/requires: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/save_frames_as_png.c.o.requires
src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/requires: src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/frame_saver/wrapped_natives.c.o.requires
.PHONY : src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/requires

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/clean:
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/gst-plugins && $(CMAKE_COMMAND) -P CMakeFiles/framesavermediapipeline.dir/cmake_clean.cmake
.PHONY : src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/clean

src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/depend:
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/src/gst-plugins /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/gst-plugins /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipeline/frame-saver-media-pipeline/build/src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/gst-plugins/CMakeFiles/framesavermediapipeline.dir/depend

