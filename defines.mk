# Defines for Makefile for GNU make

EXPAT_FONTCONFIG_NAME = mzfntcfg
EXPAT_FONTCONFIG_OUT = $(EXPAT_FONTCONFIG_NAME).dll

FREETYPE_NAME = mozft
FREETYPE_OUT = $(FREETYPE_NAME).lib

BINDIR = bin
LIBDIR = lib
INCLUDEDIR = include
SRCDIR = src

CC = gcc
CFLAGS = -Wall -O2 -Zomf -pipe
DLLFLAGS = -O2 -Zomf -Zdll -Zmap -s -pipe \
	-Zlinker /EXEPACK:2 -Zlinker /PACKCODE -Zlinker /PACKCODE

