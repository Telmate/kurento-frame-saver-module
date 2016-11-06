################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/gst_frame_saver_plugin.c 

OBJS += \
./src/gst_frame_saver_plugin.o 

C_DEPS += \
./src/gst_frame_saver_plugin.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=c99 -D_LINUX -D_DUMMY_PLUGIN_LIB -D_USE_GSTREAMER_VER_=150 -I/usr/include/glib-2.0 -I/usr/include/gstreamer-1.5 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/lib/x86_64-linux-gnu/gstreamer-1.5/include -O3 -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


