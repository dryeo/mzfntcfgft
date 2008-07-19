# Makefile for GNU make

DEPTH = ..

include $(DEPTH)/defines.mk

EXPAT_CFLAGS = -DHAVE_EXPAT_CONFIG_H
# EXPAT_SOURCES = \
# 	xmlparse.c \
# 	xmlrole.c  \
# 	xmltok.c
EXPAT_OBJS = \
	xmlparse.o \
	xmlrole.o  \
	xmltok.o

FONTCONFIG_CFLAGS = \
	-DFONTCONFIG_FILE='"~/fonts_cairo.conf"' \
	-DHAVE_FT_GET_BDF_PROPERTY               \
	-DHAVE_FT_GET_PS_FONT_INFO               \
	-DHAVE_FT_GET_NEXT_CHAR
# FONTCONFIG_SOURCES = \
# 	fcatomic.c     \
# 	fcblanks.c     \
# 	fccache.c      \
# 	fccfg.c        \
# 	fccharset.c    \
# 	fcdbg.c        \
# 	fcdefault.c    \
# 	fcdefaultcfg.c \
# 	fcdir.c        \
# 	fcfreetype.c   \
# 	fcfs.c         \
# 	fcinit.c       \
# 	fclang.c       \
# 	fclist.c       \
# 	fcmatch.c      \
# 	fcmatrix.c     \
# 	fcname.c       \
# 	fcpat.c        \
# 	fcstr.c        \
# 	fcxml.c
FONTCONFIG_OBJS = \
	fcatomic.o     \
	fcblanks.o     \
	fccache.o      \
	fccfg.o        \
	fccharset.o    \
	fcdbg.o        \
	fcdefault.o    \
	fcdefaultcfg.o \
	fcdir.o        \
	fcfreetype.o   \
	fcfs.o         \
	fcinit.o       \
	fclang.o       \
	fclist.o       \
	fcmatch.o      \
	fcmatrix.o     \
	fcname.o       \
	fcpat.o        \
	fcstr.o        \
	fcxml.o

# FREETYPE_SOURCES = \
# 	base/ftsystem.c     \
# 	base/ftinit.c       \
# 	base/ftdebug.c      \
# 	base/ftbase.c       \
# 	base/ftglyph.c      \
# 	base/ftbbox.c       \
# 	base/ftmm.c         \
# 	base/ftpfr.c        \
# 	base/ftbdf.c        \
# 	base/ftwinfnt.c     \
# 	base/fttype1.c      \
# 	autofit/autofit.c   \
# 	sfnt/sfnt.c         \
# 	cff/cff.c           \
# 	pfr/pfr.c           \
# 	bdf/bdf.c           \
# 	pcf/pcf.c           \
# 	psnames/psnames.c   \
# 	psaux/psaux.c       \
# 	truetype/truetype.c \
# 	type1/type1.c       \
# 	cid/type1cid.c      \
# 	winfonts/winfnt.c   \
# 	raster/raster.c     \
# 	smooth/smooth.c     \
# 	base/ftbitmap.c     \
# 	gzip/ftgzip.c       \
# 	lzw/ftlzw.c         \
# 	type42/type42.c     \
# 	pshinter/pshinter.c \
# 	otvalid/otvalid.c
FREETYPE_OBJS = \
	base/ftsystem.o     \
	base/ftinit.o       \
	base/ftdebug.o      \
	base/ftbase.o       \
	base/ftglyph.o      \
	base/ftbbox.o       \
	base/ftmm.o         \
	base/ftpfr.o        \
	base/ftbdf.o        \
	base/ftwinfnt.o     \
	base/fttype1.o      \
	autofit/autofit.o   \
	sfnt/sfnt.o         \
	cff/cff.o           \
	pfr/pfr.o           \
	bdf/bdf.o           \
	pcf/pcf.o           \
	psnames/psnames.o   \
	psaux/psaux.o       \
	truetype/truetype.o \
	type1/type1.o       \
	cid/type1cid.o      \
	winfonts/winfnt.o   \
	raster/raster.o     \
	smooth/smooth.o     \
	base/ftbitmap.o     \
	gzip/ftgzip.o       \
	lzw/ftlzw.o         \
	type42/type42.o     \
	pshinter/pshinter.o \
	otvalid/otvalid.o

CFLAGS += -I../$(INCLUDEDIR) $(EXPAT_CFLAGS) $(FONTCONFIG_CFLAGS)

.c.o:	
	@echo $<
	$(CC) $(CFLAGS) -c -o $@ $<

$(EXPAT_FONTCONFIG_OUT): $(EXPAT_OBJS) $(FONTCONFIG_OBJS)
	@echo "Create Expat and Fontconfig library ($(FREETYPE_OUT))..."
	emxomfar rc $@ $^

$(FREETYPE_OUT):	$(FREETYPE_OBJS)
	@echo "Create FreeType library ($(FREETYPE_OUT))..."
	emxomfar rc $@ $(FREETYPE_OBJS)

#   ftbase.o ftbbox.o ftdebug.o ftglyph.o ftinit.o ftmm.o ftsystem.o        \
#   ftpfr.o ftbdf.o ftwinfnt.o fttype1.o autofit.o sfnt.o cff.o pfr.o bdf.o \
#   pcf.o psnames.o psaux.o truetype.o type1.o type1cid.o winfnt.o raster.o \
#   smooth.o ftbitmap.o ftgzip.o ftlzw.o type42.o pshinter.o otvalid.o
#
#   xmlparse.o xmlrole.o xmltok.o                                           \
#   fcatomic.o fcblanks.o fccache.o fccfg.o fccharset.o fcdbg.o fcdefault.o \
#   fcdefaultcfg.o fcdir.o fcfreetype.o fcfs.o fcinit.o fclang.o fclist.o   \
#   fcmatch.o fcmatrix.o fcname.o fcpat.o fcstr.o fcxml.o
  

.PHONY: clean

clean:	
	rm -f $(EXPAT_OBJS) $(FONTCONFIG_OBJS) $(FREETYPE_OBJS) \
	      $(EXPAT_FONTCONFIG_NAME).lib \
	      $(FREETYPE_OUT)

