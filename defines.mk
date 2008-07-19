# Defines for Makefile for GNU make

FONTCONFIG_NAME = mzfntcfg
FONTCONFIG_OUT = $(FONTCONFIG_NAME).lib

FREETYPE_NAME = mozft
FREETYPE_OUT = $(FREETYPE_NAME).lib

BINDIR = bin
LIBDIR = lib
INCLUDEDIR = include
SRCDIR = src

CC = gcc
CFLAGS = -Wall -O2 -Zomf -pipe 
#CFLAGS += -g
DLLFLAGS = -Zomf -Zdll -Zmap -pipe
DLLFLAGS += -Zlinker /EXEPACK:2 -Zlinker /PACKCODE -Zlinker /PACKCODE -s -O2 
#DLLFLAGS += -g 

