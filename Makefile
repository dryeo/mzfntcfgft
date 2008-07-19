# Makefile for GNU make

DEPTH = .

include $(DEPTH)/defines.mk

all:	$(LIBDIR)/$(FREETYPE_OUT) $(BINDIR)/freetype-config $(LIBDIR)/$(EXPAT_FONTCONFIG_OUT)

$(LIBDIR)/$(FREETYPE_OUT):
	$(MAKE) -C $(SRCDIR) $(FREETYPE_OUT)
	cp -f $(SRCDIR)/$(FREETYPE_OUT) $@

$(BINDIR)/freetype-config:	freetype-config.in
	@echo "Adapt and install freetype-config script..."
	sed -e s,@__PWD__@,`pwd`,g                 \
	    -e s,@__FTLIBNAME__@,$(FREETYPE_NAME), \
	    $< > $@

$(LIBDIR)/$(EXPAT_FONTCONFIG_OUT):
	$(MAKE) -C $(SRCDIR) $(EXPAT_FONTCONFIG_OUT)
	cp -f $(SRCDIR)/$(EXPAT_FONTCONFIG_NAME).lib \
	      $(SRCDIR)/$(EXPAT_FONTCONFIG_NAME).map \
	   $(LIBDIR)
	cp -f $(SRCDIR)/$(EXPAT_FONTCONFIG_NAME).dll $@


.PHONY: clean nuke

clean:
	@echo "Cleanup object files and targets in $(SRCDIR)..."
	$(MAKE) -C $(SRCDIR) clean

nuke:	clean
	@echo "Nuke also all targets..."
	rm -f $(LIBDIR)/$(EXPAT_FONTCONFIG_NAME).lib \
	      $(LIBDIR)/$(EXPAT_FONTCONFIG_NAME).dll \
	      $(LIBDIR)/$(EXPAT_FONTCONFIG_NAME).map \
	      $(LIBDIR)/$(FREETYPE_OUT)              \
	      $(BINDIR)/freetype-config

