################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/FrameSaverVideoFilterImpl.cpp 

OBJS += \
./src/FrameSaverVideoFilterImpl.o 

CPP_DEPS += \
./src/FrameSaverVideoFilterImpl.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D_LINUX -D_DEBUG -D_LIB -I"/home/jonny/Work/TELMATE/GST/ECLIPSE.LNX64.GST150/KmsModuleFrameSaverVideoFilterimpl/src" -I/usr/include/kmsjsoncpp -I/usr/include/kurento -I/usr/include/kurento/commons -I/usr/include/kurento/modules/core -I/usr/include -I/usr/include/kurento -I/usr/include/boost/flyweight -I/usr/include/sigc++-2.0 -I/usr/lib/x86_64-linux-gnu/sigc++-2.0/include -I/usr/include/gstreamer-1.5 -I/usr/lib/x86_64-linux-gnu/gstreamer-1.5/include -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/glib-2.0 -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


