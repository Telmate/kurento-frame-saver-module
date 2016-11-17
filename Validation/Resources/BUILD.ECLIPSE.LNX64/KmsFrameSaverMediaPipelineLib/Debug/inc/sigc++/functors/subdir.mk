################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../inc/sigc++/functors/slot_base.cc 

CC_DEPS += \
./inc/sigc++/functors/slot_base.d 

OBJS += \
./inc/sigc++/functors/slot_base.o 


# Each subdirectory must supply rules for building sources it contributes
inc/sigc++/functors/%.o: ../inc/sigc++/functors/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D_LINUX -D_DEBUG -D_LIB_SHARED -I"/home/jonny/Work/TELMATE/GST/ECLIPSE.LNX64.GST150/KmsFrameSaverMediaPipelineLib/src" -I"/home/jonny/Work/TELMATE/GST/ECLIPSE.LNX64.GST150/KmsFrameSaverMediaPipelineLib/inc" -I"/home/jonny/Work/TELMATE/GST/ECLIPSE.LNX64.GST150/KmsFrameSaverMediaPipelineLib/inc/kms-core-server-implementation" -I"/home/jonny/Work/TELMATE/GST/ECLIPSE.LNX64.GST150/KmsFrameSaverMediaPipelineLib/inc/kms-core-server-implementation/objects" -I"/home/jonny/Work/TELMATE/GST/ECLIPSE.LNX64.GST150/KmsFrameSaverMediaPipelineLib/inc/kms-core-server-implementation/interface" -I"/home/jonny/Work/TELMATE/GST/ECLIPSE.LNX64.GST150/KmsFrameSaverMediaPipelineLib/inc/kms-core-server-implementation/stubs" -I/usr/include -I/usr/include/kurento -I/usr/include/boost/flyweight -I/usr/include/gstreamer-1.5 -I/usr/lib/x86_64-linux-gnu/gstreamer-1.5/include -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/glib-2.0 -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


