#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

# Ensure devkitARM is set
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. Run from devkitPro Command Prompt.")
endif

# Toolchain binaries
CC := arm-none-eabi-gcc
CXX := arm-none-eabi-g++
AS := arm-none-eabi-as
AR := arm-none-eabi-ar
LD := arm-none-eabi-g++

# Include devkitPro 3DS rules
include $(DEVKITPRO)/devkitARM/3ds_rules

#---------------------------------------------------------------------------------
# Project Settings
#---------------------------------------------------------------------------------
TARGET      := mp3_player
BUILD       := build
SOURCES     := source
INCLUDES    := include
ROMFS       := romfs

APP_TITLE       := 3DS MP3 Player
APP_DESCRIPTION := MP3 playback for 3DS
APP_AUTHOR      := Sanichthestrong
ICON	        := icon.png

#---------------------------------------------------------------------------------
# Flags
#---------------------------------------------------------------------------------
ARCH     := -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft

# Include paths (hardcoded to avoid lookup issues)
INCLUDE_DIRS := -IC:/devkitPro/libctru/include -I$(CURDIR)/$(INCLUDES)

CFLAGS   := -g -Wall -O2 -mword-relocations -ffunction-sections $(ARCH) $(INCLUDE_DIRS)
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++11 -fexceptions
ASFLAGS  := -g $(ARCH)
LDFLAGS  := -specs=3dsx.specs -g $(ARCH) -Wl,-Map,$(TARGET).map

LIBS := -lctru -lcitro2d -lcitro3d -lm
LIBDIRS := "$(DEVKITPRO)/libctru/lib"


#---------------------------------------------------------------------------------
# Source Files
#---------------------------------------------------------------------------------
CFILES   := $(wildcard $(SOURCES)/*.c)
CPPFILES := $(wildcard $(SOURCES)/*.cpp)
SFILES   := $(wildcard $(SOURCES)/*.s)

OFILES   := $(addprefix $(BUILD)/, $(notdir $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)))

#---------------------------------------------------------------------------------
# Build Rules
#---------------------------------------------------------------------------------
.PHONY: all clean

all: $(TARGET).3dsx

$(BUILD):
	@mkdir -p $@

$(BUILD)/%.o: $(SOURCES)/%.cpp | $(BUILD)
	@echo Compiling $<
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: $(SOURCES)/%.c | $(BUILD)
	@echo Compiling $<
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: $(SOURCES)/%.s | $(BUILD)
	@echo Assembling $<
	@$(AS) $(ASFLAGS) -c $< -o $@

$(TARGET).elf: $(OFILES)
	@echo Linking $@
	@$(LD) $(LDFLAGS) -o $@ $^ -L$(LIBDIRS) $(LIBS)

$(TARGET).smdh: $(ICON)
	@echo Generating SMDH
	@smdhtool --create "$(APP_TITLE)" "$(APP_DESCRIPTION)" "$(APP_AUTHOR)" $(ICON) $@

$(TARGET).3dsx: $(TARGET).elf $(TARGET).smdh
	@echo Creating 3dsx file
	@3dsxtool $< $@ --smdh=$(TARGET).smdh --romfs=$(ROMFS)

clean:
	@echo Cleaning...
	@rm -rf $(BUILD) *.elf *.3dsx *.smdh *.map
