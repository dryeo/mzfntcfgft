# Makefile for GNU make

DEPTH = .

include $(DEPTH)/defines.mk

all:	$(LIBDIR)/$(FREETYPE_OUT) $(BINDIR)/freetype-config $(LIBDIR)/$(FONTCONFIG_OUT)

$(LIBDIR)/$(FREETYPE_OUT):
	$(MAKE) -C $(SRCDIR) $(FREETYPE_OUT)
	cp -f $(SRCDIR)/$(FREETYPE_OUT) $@

$(BINDIR)/freetype-config:	freetype-config.in
	@echo "Adapt and install freetype-config script..."
	sed -e s,@__PWD__@,`pwd`,g                 \
	    -e s,@__FTLIBNAME__@,$(FREETYPE_NAME), \
	    $< > $@

$(LIBDIR)/$(FONTCONFIG_OUT):
	$(MAKE) -C $(SRCDIR) $(FONTCONFIG_OUT)
	cp -f $(SRCDIR)/$(FONTCONFIG_OUT) $@


.PHONY: clean nuke

clean:
	@echo "Cleanup object files and targets in $(SRCDIR)..."
	$(MAKE) -C $(SRCDIR) clean

nuke:	clean
	@echo "Nuke also all targets..."
	rm -f $(LIBDIR)/$(FONTCONFIG_NAME).lib \
	      $(LIBDIR)/$(FREETYPE_OUT)        \
	      $(BINDIR)/freetype-config
