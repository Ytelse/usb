################
# DEFINITIONS
################

# This will be the name of the executable
TARGET:=interleave_test

# Directories
OBJDIR:=build
SRCDIR:=src

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

INCLUDES:= -Isrc
LIBS:=

CFLAGS:=-std=c99 -Wall $(INCLUDES)
LDFLAGS:=$(LIBS)

################
# FILES
################

CSRC:= $(addprefix $(SRCDIR)/, \
test.c \
interleave.c \
deinterleave.c \
printimg.c \
)

C_FILES := $(notdir $(CSRC))
C_PATHS := $(dir $(CSRC))
C_OBJS := $(addprefix $(OBJDIR)/, $(C_FILES:.c=.o))
OBJS = $(C_OBJS)

vpath %.c $(C_PATHS)

################
# BUILD RULES
################

.PHONY : all run clean

all : $(TARGET)

run : $(TARGET)
	./$(TARGET)

$(TARGET) : $(OBJS)
	@echo "$(YELLOW)Linking target: $@$(NO_COLOR)"
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

$(OBJDIR)/%.o : %.c | $(OBJDIR)
	@echo "$(YELLOW)Compiling source: $@$(NO_COLOR)"
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR) :
	mkdir $@

clean :
	-rm -rf $(OBJDIR) $(TARGET)