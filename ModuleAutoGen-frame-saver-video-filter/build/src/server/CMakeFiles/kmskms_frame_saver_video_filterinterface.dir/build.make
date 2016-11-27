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
CMAKE_SOURCE_DIR = /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build

# Include any dependencies generated for this target.
include src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/depend.make

# Include the progress variables for this target.
include src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/progress.make

# Include the compile flags for this target's objects.
include src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/flags.make

src/server/cpp_interface_internal.generated: ../src/server/interface/kms_frame_saver_video_filter.kmd.json
src/server/cpp_interface_internal.generated: ../src/server/interface/kms_frame_saver_video_filter.kms_frame_saver_video_filter.kmd.json
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating cpp_interface_internal.generated, interface/generated-cpp/FrameSaverVideoFilterInternal.cpp, interface/generated-cpp/FrameSaverVideoFilterInternal.hpp"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server && /usr/bin/cmake -E touch cpp_interface_internal.generated
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server && /usr/bin/kurento-module-creator -c /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server/interface/generated-cpp -r /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/src/server/interface -dr /usr/share/kurento/modules -dr /usr/share/kurento/modules -it cpp_interface_internal

src/server/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp: src/server/cpp_interface_internal.generated

src/server/interface/generated-cpp/FrameSaverVideoFilterInternal.hpp: src/server/cpp_interface_internal.generated

src/server/cpp_interface.generated: ../src/server/interface/kms_frame_saver_video_filter.kmd.json
src/server/cpp_interface.generated: ../src/server/interface/kms_frame_saver_video_filter.kms_frame_saver_video_filter.kmd.json
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating cpp_interface.generated, interface/generated-cpp/FrameSaverVideoFilter.cpp, interface/generated-cpp/FrameSaverVideoFilter.hpp"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server && /usr/bin/cmake -E touch cpp_interface.generated
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server && /usr/bin/kurento-module-creator -c /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server/interface/generated-cpp -r /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/src/server/interface -dr /usr/share/kurento/modules -dr /usr/share/kurento/modules -it cpp_interface

src/server/interface/generated-cpp/FrameSaverVideoFilter.cpp: src/server/cpp_interface.generated

src/server/interface/generated-cpp/FrameSaverVideoFilter.hpp: src/server/cpp_interface.generated

src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp.o: src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/flags.make
src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp.o: src/server/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp.o"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp.o -c /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp

src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp.i"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp > CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp.i

src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp.s"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp -o CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp.s

src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp.o.requires:
.PHONY : src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp.o.requires

src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp.o.provides: src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp.o.requires
	$(MAKE) -f src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/build.make src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp.o.provides.build
.PHONY : src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp.o.provides

src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp.o.provides.build: src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp.o

src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilter.cpp.o: src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/flags.make
src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilter.cpp.o: src/server/interface/generated-cpp/FrameSaverVideoFilter.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilter.cpp.o"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilter.cpp.o -c /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server/interface/generated-cpp/FrameSaverVideoFilter.cpp

src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilter.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilter.cpp.i"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server/interface/generated-cpp/FrameSaverVideoFilter.cpp > CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilter.cpp.i

src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilter.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilter.cpp.s"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server/interface/generated-cpp/FrameSaverVideoFilter.cpp -o CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilter.cpp.s

src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilter.cpp.o.requires:
.PHONY : src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilter.cpp.o.requires

src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilter.cpp.o.provides: src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilter.cpp.o.requires
	$(MAKE) -f src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/build.make src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilter.cpp.o.provides.build
.PHONY : src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilter.cpp.o.provides

src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilter.cpp.o.provides.build: src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilter.cpp.o

# Object files for target kmskms_frame_saver_video_filterinterface
kmskms_frame_saver_video_filterinterface_OBJECTS = \
"CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp.o" \
"CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilter.cpp.o"

# External object files for target kmskms_frame_saver_video_filterinterface
kmskms_frame_saver_video_filterinterface_EXTERNAL_OBJECTS =

src/server/libkmskms_frame_saver_video_filterinterface.a: src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp.o
src/server/libkmskms_frame_saver_video_filterinterface.a: src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilter.cpp.o
src/server/libkmskms_frame_saver_video_filterinterface.a: src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/build.make
src/server/libkmskms_frame_saver_video_filterinterface.a: src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX static library libkmskms_frame_saver_video_filterinterface.a"
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server && $(CMAKE_COMMAND) -P CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/cmake_clean_target.cmake
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/build: src/server/libkmskms_frame_saver_video_filterinterface.a
.PHONY : src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/build

src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/requires: src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp.o.requires
src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/requires: src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/interface/generated-cpp/FrameSaverVideoFilter.cpp.o.requires
.PHONY : src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/requires

src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/clean:
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server && $(CMAKE_COMMAND) -P CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/cmake_clean.cmake
.PHONY : src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/clean

src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/depend: src/server/cpp_interface_internal.generated
src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/depend: src/server/interface/generated-cpp/FrameSaverVideoFilterInternal.cpp
src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/depend: src/server/interface/generated-cpp/FrameSaverVideoFilterInternal.hpp
src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/depend: src/server/cpp_interface.generated
src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/depend: src/server/interface/generated-cpp/FrameSaverVideoFilter.cpp
src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/depend: src/server/interface/generated-cpp/FrameSaverVideoFilter.hpp
	cd /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/src/server /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server /home/jonny/Work/TELMATE/KmsModule_FrameSaverMediaPipelineAndVideoFilter/kms_frame_saver_video_filter/build/src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/server/CMakeFiles/kmskms_frame_saver_video_filterinterface.dir/depend

