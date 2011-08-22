# Makefile for GNU make

DEPTH = ..

include $(DEPTH)/defines.mk

FONTCONFIG_DLLFLAGS = -l$(FREETYPE_NAME) -L../$(LIBDIR)
FONTCONFIG_OBJS = fontconfig.o 

FREETYPE_OBJS = \
	base/ftsystem.o     \
	base/ftinit.o       \
	base/ftdebug.o      \
	base/ftbase.o       \
	base/ftglyph.o      \
	base/ftbbox.o       \
	base/ftmm.o         \
	base/fttype1.o      \
	base/ftsynth.o      \
	base/ftstroke.o     \
	base/ftxf86.o       \
	autofit/autofit.o   \
	sfnt/sfnt.o         \
	psnames/psnames.o   \
	psaux/psaux.o       \
	truetype/truetype.o \
	type1/type1.o       \
	cid/type1cid.o      \
	raster/raster.o     \
	smooth/smooth.o     \
	base/ftbitmap.o     \
	pshinter/pshinter.o \
	otvalid/otvalid.o
FREETYPE_CFLAGS = -DFT2_BUILD_LIBRARY=1

CFLAGS += -I../$(INCLUDEDIR) $(FREETYPE_CFLAGS)

.c.o:	
	@echo $<
	$(CC) $(CFLAGS) -c -o $@ $<

$(FONTCONFIG_OUT)_dynamic: $(FONTCONFIG_OBJS)
	@echo "Link Fontconfig into $(FONTCONFIG_OUT)..."
	@echo "LIBRARY $(FONTCONFIG_NAME) INITINSTANCE TERMINSTANCE" \
	   > $(FONTCONFIG_NAME).def
	@echo "DESCRIPTION '$(FONTCONFIG_NAME), a new OS/2 Fontconfig version from Netlabs, v1.0'" \
	   >> $(FONTCONFIG_NAME).def
	cat exports.list >> $(FONTCONFIG_NAME).def

	$(CC) $(DLLFLAGS) $(FONTCONFIG_DLLFLAGS) -o $@ \
	      $(FONTCONFIG_NAME).def $^
	emximp -o $(FONTCONFIG_NAME).lib $(FONTCONFIG_NAME).def

$(FONTCONFIG_OUT): $(FONTCONFIG_OBJS)
	@echo "Create Fontconfig library $(FONTCONFIG_OUT)..."
	rm -f $@
	emxomfar rc $@ $(FONTCONFIG_OBJS)

$(FREETYPE_OUT):	$(FREETYPE_OBJS)
	@echo "Create FreeType library ($(FREETYPE_OUT))..."
	rm -f $@
	emxomfar rc $@ $(FREETYPE_OBJS)

.PHONY: clean

clean:	
	rm -f $(FONTCONFIG_OBJS) $(FREETYPE_OBJS) \
	      $(FONTCONFIG_NAME).lib \
	      $(FREETYPE_OUT)
