################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: FrameSaverFilterTesterApp-Debug

# Tool invocations
FrameSaverFilterTesterApp-Debug: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/jonny/Work/TELMATE/GST/ECLIPSE.LNX64.GST150/FrameSaverFilterLib/Debug" -L/usr/lib/x86_64-linux-gnu -L/usr/lib/x86_64-linux-gnu/gstreamer-1.5 -L/usr/lib/x86_64-linux-gnu/pix/extensions -o "FrameSaverFilterTesterApp-Debug" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) FrameSaverFilterTesterApp-Debug
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
