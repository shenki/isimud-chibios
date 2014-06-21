# Makefile for ChibiOS

USE_OPT ?= -std=gnu99 -O2 -ggdb -fomit-frame-pointer -falign-functions=16

# C specific options here (added to USE_OPT).
USE_COPT ?= 

# C++ specific options here (added to USE_OPT).
USE_CPPOPT ?= -fno-rtti

# Enable this if you want the linker to remove unused code and data
USE_LINK_GC ?= yes

# Linker extra options here.
USE_LDOPT ?= 

# Enable this if you want link time optimizations (LTO)
USE_LTO ?= no

# If enabled, this option allows to compile the application in THUMB mode.
USE_THUMB ?= yes

# Enable this if you want to see the full log while compiling.
USE_VERBOSE_COMPILE ?= no

# Enable for semihosting support. Project will not run standalone.
USE_SEMIHOSTING ?= no

# Define project name here
PROJECT = isimud

# Imported source files and paths
CHIBIOS = ./chibios/
include $(CHIBIOS)/boards/ST_STM32F0_DISCOVERY/board.mk
include $(CHIBIOS)/os/hal/platforms/STM32F0xx/platform.mk
include $(CHIBIOS)/os/hal/hal.mk
include $(CHIBIOS)/os/ports/GCC/ARMCMx/STM32F0xx/port.mk
include $(CHIBIOS)/os/kernel/kernel.mk

# Define linker script file here
LDSCRIPT= $(PORTLD)/STM32F051x8.ld

# C sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CSRC = $(PORTSRC) \
       $(KERNSRC) \
       $(TESTSRC) \
       $(HALSRC) \
       $(PLATFORMSRC) \
       $(BOARDSRC) \
       main.c

# C++ sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CPPSRC =

# List ASM source files here
ASMSRC = $(PORTASM)

INCDIR = $(PORTINC) $(KERNINC) $(TESTINC) \
         $(HALINC) $(PLATFORMINC) $(BOARDINC) \
         $(CHIBIOS)/os/various

MCU  = cortex-m0

TRGT = arm-none-eabi-
CC   = $(TRGT)gcc
CPPC = $(TRGT)g++
# Enable loading with g++ only if you need C++ runtime support.
# NOTE: You can use C++ even without C++ support if you are careful. C++
#       runtime support makes code size explode.
LD   = $(TRGT)gcc
#LD   = $(TRGT)g++
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
OD   = $(TRGT)objdump
SZ   = $(TRGT)size
HEX  = $(CP) -O ihex
BIN  = $(CP) -O binary

# THUMB-specific options here
TOPT = -mthumb -DTHUMB

# Define C warning options here
CWARN = -O2 -ggdb -fomit-frame-pointer -falign-functions=16 -std=gnu99

# Define C++ warning options here
CPPWARN = -Wall -Wextra

# List all default C defines here, like -D_DEBUG=1
DDEFS =

# List all default ASM defines here, like -D_DEBUG=1
DADEFS =

# List all default directories to look for include files here
DINCDIR =

# List the default directory to look for the libraries here
DLIBDIR =

# List all default libraries here
DLIBS =

ifneq ($(USE_SEMIHOSTING),no)
  DLIBS += --specs=rdimon.specs -lc -lrdimon
  DDEFS += -DSEMIHOSTING=1
endif

RULESPATH = $(CHIBIOS)/os/ports/GCC/ARMCMx
include $(RULESPATH)/rules.mk
