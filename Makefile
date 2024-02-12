# configure
DEBUG      := 1
PIPEWIRE   := 0
PULSEAUDIO := 0
ALSA       := 0

# try to be constistent
TZ        := UTC
LC_ALL    := C.UTF-8
SHELL     := /bin/sh
MAKEFLAGS += --warn-undefined-variables
MAKEFLAGS += --no-builtin-rules

.ONESHELL:
.SHELLFLAGS := -eu -c
.DELETE_ON_ERROR:
.RECIPEPREFIX = >

# prep var
CFLAGS_1  :=
LDFLAGS_1 :=

# Third party
# <LIB>_INC ?= $(shell pkg-config --cflags <library> 2>/dev/null || echo -I<library>)
# <LIB>_LIB ?= $(shell pkg-config --libs <library> 2>/dev/null || echo -l<library>)

# INCS = <LIB>_INC
LIBS = 
###

NASM   := nasm -f elf64
CC      = clang
CFLAGS += -std=c99
CFLAGS += -Werror -Wall -Wpedantic -Wconversion
CFLAGS += -ffreestanding -nostdlib -nostartfiles
CFLAGS += -fno-stack-protector
CFLAGS += -Iinclude
LDFLAGS += -Wl,--as-needed

# debug flags
CFLAGS_$(DEBUG) += -g -O0
CFLAGS += -DDEBUG=$(DEBUG)

# pipewire flags
CFLAGS += -DPIPEWIRE=$(PIPEWIRE)
#CFLAGS_$(PIPEWIRE) += -I/usr/include/pipewire-0.3 -I/usr/include/spa-0.2 -D_REENTRANT
LDFLAGS_$(PIPEWIRE) += -lpipewire-0.3

# pulseaudio flags
CFLAGS += -DPULSEAUDIO=$(PULSEAUDIO)

# alsa flags
CFLAGS += -DALSA=$(ALSA)
LDFLAGS_$(ALSA) += -lasound

# merge enabled flags
CFLAGS  += $(CFLAGS_1)
LDFLAGS += $(LDFLAGS_1)

BINARY  := status

SRC_DIR := src
OBJ_DIR := obj

SOURCES := $(SRC_DIR)/main.c
OBJECTS := $(OBJ_DIR)/main.o

ASM_SOURCES := $(SRC_DIR)/init.asm
ASM_OBJECTS := $(OBJ_DIR)/init.o

DESTDIR ?=
PREFIX  ?= /usr/local
BINDIR  ?= $(PREFIX)/bin
MKDIR   := mkdir -p
LN      := ln -sf
RM      := rm -rf
STRIP   := strip
INSTALL := install
INSTALL_DIR := $(INSTALL) -d -m 755
INSTALL_EXE := $(INSTALL) -m 755

all: dirs $(BINARY)

dirs:
> @test -d $(OBJ_DIR) || $(MKDIR) $(OBJ_DIR)

$(BINARY): $(OBJECTS) $(ASM_OBJECTS)
> $(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS)

strip: $(BINARY)
> $(STRIP) $(BINARY)

obj/init.o: src/init.asm
> $(NASM) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
> $(CC) $(CFLAGS) -c $< -o $@

install:
> $(INSTALL_DIR) $(DESTDIR)$(BINDIR)
> $(INSTALL_EXE) $(BINARY) $(DESTDIR)$(BINDIR)

uninstall:
> $(RM) $(DESTDIR)$(BINDIR)/$(BINARY)

clean:
> $(RM) $(OBJ_DIR)/ $(BINARY)

tests: test_types test_utils
test_types: dirs obj/init.o 
> $(CC) $(CFLAGS) -o obj/types_test test/types_test.c obj/init.o $(LDFLAGS) $(LIBS) && ./obj/types_test
test_utils: dirs obj/init.o 
> $(CC) $(CFLAGS) -o obj/utils_test test/utils_test.c obj/init.o $(LDFLAGS) $(LIBS) && ./obj/utils_test

.PHONY: all install uninstall clean tests
