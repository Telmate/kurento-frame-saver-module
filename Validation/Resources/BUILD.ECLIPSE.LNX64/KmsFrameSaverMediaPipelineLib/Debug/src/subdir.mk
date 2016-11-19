################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/FrameSaverMediaPipeline_Impl.cpp 

OBJS += \
./src/FrameSaverMediaPipeline_Impl.o 

CPP_DEPS += \
./src/FrameSaverMediaPipeline_Impl.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D_LINUX -D_DEBUG -D_LIB -I"/home/jonny/Work/TELMATE/GST/ECLIPSE.LNX64.GST150/KmsFrameSaverMediaPipelineLib/src" -I"/home/jonny/Work/TELMATE/GST/ECLIPSE.LNX64.GST150/KmsFrameSaverMediaPipelineLib/inc" -I"/home/jonny/Work/TELMATE/GST/ECLIPSE.LNX64.GST150/KmsFrameSaverMediaPipelineLib/inc/boost" -I"/home/jonny/Work/TELMATE/GST/ECLIPSE.LNX64.GST150/KmsFrameSaverMediaPipelineLib/inc/kms-core-server-implementation" -I"/home/jonny/Work/TELMATE/GST/ECLIPSE.LNX64.GST150/KmsFrameSaverMediaPipelineLib/inc/kms-core-server-stubs-for-missing" -I"/home/jonny/Work/TELMATE/GST/ECLIPSE.LNX64.GST150/KmsFrameSaverMediaPipelineLib/inc/kms-core-server-implementation/objects" -I"/home/jonny/Work/TELMATE/GST/ECLIPSE.LNX64.GST150/KmsFrameSaverMediaPipelineLib/inc/kms-core-server-implementation/interface" -I/usr/include -I/usr/include/kurento -I/usr/include/boost/flyweight -I/usr/include/sigc++-2.0 -I/usr/lib/x86_64-linux-gnu/sigc++-2.0/include -I/usr/include/gstreamer-1.0 -I/usr/lib/x86_64-linux-gnu/gstreamer-1.0/include -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/glib-2.0 -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


