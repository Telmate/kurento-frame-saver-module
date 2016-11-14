################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/frame_saver_filter.c \
../src/frame_saver_filter_lib.c \
../src/frame_saver_params.c \
../src/save_frames_as_png.c \
../src/win32api-dll-main.c \
../src/wrapped_natives.c 

OBJS += \
./src/frame_saver_filter.o \
./src/frame_saver_filter_lib.o \
./src/frame_saver_params.o \
./src/save_frames_as_png.o \
./src/win32api-dll-main.o \
./src/wrapped_natives.o 

C_DEPS += \
./src/frame_saver_filter.d \
./src/frame_saver_filter_lib.d \
./src/frame_saver_params.d \
./src/save_frames_as_png.d \
./src/win32api-dll-main.d \
./src/wrapped_natives.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=c99 -D_LINUX -D_LIB_SHARED -D_USE_GSTREAMER_VER_=150 -D_USING_PNG_LIBRARY_ -D_USING_SINKER_CAPS_ -D_IS_LIB_FOR_PLUGIN_ -I"/home/jonny/Work/TELMATE/GST/ECLIPSE.LNX64.GST150/FrameSaverPluginLib/inc" -I"/home/jonny/Work/TELMATE/GST/ECLIPSE.LNX64.GST150/FrameSaverPluginLib/png-api" -I/usr/include/gstreamer-1.5 -I/usr/lib/x86_64-linux-gnu/gstreamer-1.5/include -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/glib-2.0 -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


