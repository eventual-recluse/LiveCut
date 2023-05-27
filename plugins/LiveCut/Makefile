#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = LiveCut

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	LiveCut.cpp \
	lib/BBCutter.cpp \
	lib/BitCrusher.cpp \
	lib/Comb.cpp \
	lib/DelayLine.cpp \
	lib/FirstOrderLowpass.cpp
	

FILES_UI  = \
	LiveCutUI.cpp \
	../../dpf-widgets/opengl/DearImGui.cpp

# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk
BUILD_CXX_FLAGS += -I./lib
BUILD_CXX_FLAGS += -I../../dpf-widgets/generic
BUILD_CXX_FLAGS += -I../../dpf-widgets/opengl

# --------------------------------------------------------------
# Enable all possible plugin types

TARGETS += jack
TARGETS += clap
TARGETS += lv2_sep
TARGETS += vst2
TARGETS += vst3

all: $(TARGETS)

# --------------------------------------------------------------
