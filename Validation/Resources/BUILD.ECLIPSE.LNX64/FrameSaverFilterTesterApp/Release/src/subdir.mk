################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/FrameSaverFilterTesterApp.c 

OBJS += \
./src/FrameSaverFilterTesterApp.o 

C_DEPS += \
./src/FrameSaverFilterTesterApp.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=c99 -D_LINUX -I"/home/jonny/Work/TELMATE/GST/ECLIPSE.LNX64.GST150/FrameSaverFilterLibStatic/inc" -O3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


