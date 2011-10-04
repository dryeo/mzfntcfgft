# Defines for Makefile for GNU make

FONTCONFIG_NAME = mzfntcfg
FONTCONFIG_LIB = $(FONTCONFIG_NAME).lib
FONTCONFIG_OUT = $(FONTCONFIG_NAME).a

FREETYPE_NAME = mozft
FREETYPE_LIB = $(FREETYPE_NAME).lib
FREETYPE_OUT = $(FREETYPE_NAME).a

BINDIR = bin
LIBDIR = lib
INCLUDEDIR = include
SRCDIR = src

CC = gcc
CFLAGS = -Wall -O2 -pipe
#CFLAGS += -g
DLLFLAGS = -Zomf -Zdll -Zmap -pipe
DLLFLAGS += -Zlinker /EXEPACK:2 -Zlinker /PACKCODE -Zlinker /PACKCODE -s -O2
#DLLFLAGS += -g

