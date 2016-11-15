################
# DEFINITIONS
################

# This will be the name of the executable
TARGET:=usbspeed

# Directories
OBJDIR:=build
SRCDIR:=src
ITRLVDIR:=interleave/src

# ANSI color codes
# Only used for fancy output
NO_COLOR=\x1b[0m
RED:=\x1b[31m
GREEN:=\x1b[32m
YELLOW:=\x1b[33m

################
# TOOLCHAIN
################

CC:=gcc
LD:=gcc

################
# FLAGS
################

LIBUSB_PATH:=/usr/local/Cellar/libusb/1.0.20

INCLUDES:=-I$(LIBUSB_PATH)/include/libusb-1.0 -I$(SRCDIR) -I$(ITRLVDIR)
LIBS:=-L$(LIBUSB_PATH)/lib

CFLAGS:=-std=c99 -Wall $(INCLUDES)
LDFLAGS:=$(LIBS) -lusb-1.0 -lpthread

################
# FILES
################

CSRC:= $(addprefix $(SRCDIR)/, \
callbacks.c \
usb_device.c \
main.c \
)

C_FILES := $(notdir $(CSRC))
C_PATHS := $(dir $(CSRC))
C_OBJS := $(addprefix $(OBJDIR)/, $(C_FILES:.c=.o))
OBJS = $(C_OBJS)

vpath %.c $(C_PATHS)

################
# BUILD RULES
################

.PHONY : mcu fpga run clean
mcu : clean
mcu : CFLAGS += -DMCU
mcu : $(TARGET)

fpga : clean
fpga : CFLAGS += -DNDEBUG
fpga : $(TARGET)

.PHONY : run
run : $(TARGET)
	./$(TARGET)

$(TARGET) : $(OBJS)
	@echo "$(YELLOW)Linking target: $@$(NO_COLOR)"
	$(LD) $(LDFLAGS) -o $@ $(OBJS)
	@echo "$(GREEN)==========================================$(NO_COLOR)"

$(OBJDIR)/%.o : %.c | $(OBJDIR)
	@echo "$(YELLOW)Compiling source: $@$(NO_COLOR)"
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR) :
	mkdir $@

clean :
	-rm -rf $(OBJDIR) $(TARGET)