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
	gcc -std=c99 -D_LINUX -D_DEBUG -D_LIB_SHARED -I"/home/jonny/Work/TELMATE/GST/ECLIPSE.LNX64.GST150/FrameSaverFilterLibStatic/inc" -I"/home/jonny/Work/TELMATE/GST/ECLIPSE.LNX64.GST150/FrameSaverPlugin/src" -I/usr/include/gstreamer-1.0 -I/usr/lib/x86_64-linux-gnu/gstreamer-1.0/include -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/glib-2.0 -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


