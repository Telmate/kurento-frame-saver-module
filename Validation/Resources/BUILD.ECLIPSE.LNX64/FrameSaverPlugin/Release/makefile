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
all: libFrameSaverPluginLibShared-Release.so

# Tool invocations
libFrameSaverPluginLibShared-Release.so: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/jonny/Work/TELMATE/GST/ECLIPSE.LNX64.GST150/FrameSaverFilterLibStatic/Release" -L/usr/lib/x86_64-linux-gnu -L/usr/lib/x86_64-linux-gnu/gstreamer-1.0 -L/usr/lib/x86_64-linux-gnu/pix/extensions -shared -o "libFrameSaverPluginLibShared-Release.so" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(LIBRARIES)$(OBJS)$(C_DEPS) libFrameSaverPluginLibShared-Release.so
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
