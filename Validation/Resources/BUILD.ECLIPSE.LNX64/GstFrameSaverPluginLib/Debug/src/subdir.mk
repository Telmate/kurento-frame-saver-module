################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/gst_Frame_Saver_Plugin.c 

OBJS += \
./src/gst_Frame_Saver_Plugin.o 

C_DEPS += \
./src/gst_Frame_Saver_Plugin.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=c99 -D_LINUX -D_DEBUG -D_LIB_SHARED -D_IS_LIB_FOR_PLUGIN_ -D_USING_PNG_LIBRARY_ -D_USING_SINKER_CAPS_ -I"/home/jonny/Work/TELMATE/GST/ECLIPSE.LNX64.GST150/GstFrameSaverPluginLib/src" -I/usr/include/gstreamer-1.5 -I/usr/lib/x86_64-linux-gnu/gstreamer-1.5/include -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/glib-2.0 -O0 -g3 -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


