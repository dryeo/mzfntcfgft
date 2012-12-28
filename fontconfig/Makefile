# Makefile for GNU make

DEPTH = .
OBJS  = $(DEPTH)/objs
SRC   = $(DEPTH)/src
PROJECT = fntcfg2
FONTCONFIG_NAME = $(OBJS)/fontconfig
FONTCONFIG_DLL= $(OBJS)/$(PROJECT).dll
FONTCONFIG_DLLFLAGS = -lfreetype.a -L../freetype/objs
INCLUDES = -I./include -I ../freetype/include

CFLAGS += $(INCLUDES) -D__EMX__ -DOS2  -D__OS2__ -D__ST_MT_ERRNO__ -march=pentium -mtune=pentium4

FONTCONFIG_OBJS := $(OBJS)/fontconfig.o \
	$(OBJS)/fclang.o \
	$(OBJS)/fcpat.o  \
	$(OBJS)/fcstr.o \
	$(NULL)

$(FONTCONFIG_DLL): $(FONTCONFIG_OBJS)
	@echo "Link Fontconfig into $(FONTCONFIG_DLL)..."
	@echo "LIBRARY $(PROJECT) INITINSTANCE TERMINSTANCE" \
	   > $(FONTCONFIG_NAME).def
	@echo "DESCRIPTION '$(PROJECT), a new OS/2 Fontconfig version'" \
	   >> $(FONTCONFIG_NAME).def
	cat ./exports.list >> $(FONTCONFIG_NAME).def

	$(CC) -g -Zbin-files -Zhigh-mem -Zomf -Zmap -Zdll $(FONTCONFIG_DLLFLAGS) -o $@ \
	      $(FONTCONFIG_NAME).def $^
	ar cru $(FONTCONFIG_NAME)_s.a $(FONTCONFIG_OBJS)
	emximp -o $(FONTCONFIG_NAME).a $(FONTCONFIG_NAME).def
	emximp -o $(FONTCONFIG_NAME).lib $(FONTCONFIG_NAME).def

$(OBJS)/fontconfig.o: $(SRC)/fontconfig.c $(SRC)/fcint.h
	@echo $<
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJS)/fclang.o: $(SRC)/fclang.c $(SRC)/fclang.h
	@echo $<
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJS)/fcpat.o: $(SRC)/fcpat.c
	@echo $<
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJS)/fcstr.o: $(SRC)/fcstr.c
	@echo $<
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: $(FONTCONFIG_DLL)

clean:	
	rm -f $(FONTCONFIG_OBJS)\
	      $(FONTCONFIG_OUT) \
	      $(FONTCONFIG_LIB)
