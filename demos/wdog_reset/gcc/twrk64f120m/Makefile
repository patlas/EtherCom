#-----------------------------------------------
# Make command:
# mingw32-make build=<build> target=<target> entry=main stack=0x400 heap=0x1000
# <build>: debug or release, release by default.
# <target>: sram or flash, flash by default.
#-----------------------------------------------

#-----------------------------------------------
# setup variables
# ----------------------------------------------

SDK_ROOT := $(abspath ../../../..)

#-----------------------------------------------
# set board infomation
# The CHIP must be set correctly, otherwise the
# platform_lib cannot be linked.
#-----------------------------------------------
BOARD := twrk64f120m
CHIP := K64F12
CPU := cortex-m4

APP_NAME = wdog_reset
#-----------------------------------------------
# Debug or Release
# Release by default
#-----------------------------------------------
build ?= release

#-----------------------------------------------
# Sram target or flash target
# flash target by defult.
#-----------------------------------------------
target ?= flash

#-----------------------------------------------
# Choose the float point method.
# K64F12 can use: NO_FP, SOFT_FP and HARD_FP.
# Application's float point configuration must
# be the same with the libraries it linked.
#-----------------------------------------------
CHOOSE_FLOAT = HARD_FP

include $(SDK_ROOT)/mk/common.mk

#-----------------------------------------------
# Link libraries
#-----------------------------------------------
ifeq "$(build)" "debug"
LINK_LIB_NAME := $(SDK_ROOT)/lib/ksdk_platform_lib/gcc/$(CHIP)/Debug/ksdk_platform_lib.a
else
LINK_LIB_NAME := $(SDK_ROOT)/lib/ksdk_platform_lib/gcc/$(CHIP)/Release/ksdk_platform_lib.a
endif

#-----------------------------------------------
# Include path. Add the include paths like this:
# INCLUDES += ./include/
#-----------------------------------------------
INCLUDES += $(SDK_ROOT)/platform/startup/M$(CHIP)     \
            $(SDK_ROOT)/boards/$(BOARD)               \
			../../src

include $(SDK_ROOT)/demos/common_include.mk

#-----------------------------------------------
# Startup files
#-----------------------------------------------
STARTUP_ROOT := $(SDK_ROOT)/platform/startup
STARTUP_SOURCES := $(STARTUP_ROOT)/startup.c \
				   $(STARTUP_ROOT)/M$(CHIP)/system_M$(CHIP).c \
				   $(STARTUP_ROOT)/M$(CHIP)/gcc/startup_M$(CHIP).S

#-----------------------------------------------
# Application files
#-----------------------------------------------
APP_SOURCES  := $(wildcard $(APPS_ROOT)/$(APP_NAME)/src/*.c)

#-----------------------------------------------
# Board related files, e.g. platform_pins.c
#-----------------------------------------------
BOARD_SOURCES := $(wildcard $(BOARD_ROOT)/*.c)

#-----------------------------------------------
# Add all sources needed here
#-----------------------------------------------
SOURCES = $(STARTUP_SOURCES) $(APP_SOURCES) $(BOARD_SOURCES) \
		  $(SDK_ROOT)/platform/utilities/src/fsl_debug_console.c \
		  $(SDK_ROOT)/platform/utilities/src/fsl_misc_utilities.c

#-----------------------------------------------
# Add all pre-defined symbols here
#-----------------------------------------------
DEFINES += -DCPU_MK64FN1M0VMD12 \
		   -DTWR_K64F120M \
           -DTOWER

#-----------------------------------------------
# Link file
# Input only the name of the link file here,
# the targets.mk will convert it to
# platform/linker/gcc/<CHIP>/$(LD_FILE_NAME)
# e.g, if LD_FILE_NAME is set to K22FN512xxx12_flash.ld
# the link file platform/linker/gcc/K22F51212/K22FN512xxx12_flash.ld
# will be used
#-----------------------------------------------
ifeq "$(target)" "sram"
LD_FILE_NAME := $(LD_FILE_ROOT)/$(CHIP)/K64FN1Mxxx12_ram.ld
else
LD_FILE_NAME := $(LD_FILE_ROOT)/$(CHIP)/K64FN1Mxxx12_flash.ld
endif


#-----------------------------------------------
# Define the stack size and heap size. If not defined,
# the default velue defined in the startup_M<CHIP>.S
# will be used.
#-----------------------------------------------
stack ?= 0x1000
heap  ?= 0x1000
DEFINES += -D__STACK_SIZE=$(stack) \
           -D__HEAP_SIZE=$(heap)
		   
include $(SDK_ROOT)/mk/targets.mk
