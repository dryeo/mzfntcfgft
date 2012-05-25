#
# FreeType 2 configuration rules for the OS/2 + gcc
#


# Copyright 1996-2000 by
# David Turner, Robert Wilhelm, and Werner Lemberg.
#
# This file is part of the FreeType project, and may only be used, modified,
# and distributed under the terms of the FreeType project license,
# LICENSE.TXT.  By continuing to use, modify, or distribute this file you
# indicate that you have read the license and understand and accept it
# fully.

# default definitions of the export list
#
OS2DLLNAME        = freetyp2.dll
EXPORTS_LIST      = $(OBJ_DIR)/freetype.def
EXPORTS_OPTIONS   = $(EXPORTS_LIST)
APINAMES_OPTIONS := -d$(OS2DLLNAME) -wO

# include OS/2-specific definitions
include $(TOP_DIR)/builds/os2/os2-def.mk

# include gcc-specific definitions
include $(TOP_DIR)/builds/compiler/gcc.mk
CFLAGS +=-D__EMX__ -DOS2  -D__OS2__ -D__ST_MT_ERRNO__ -march=pentium -mtune=pentium4

# include linking instructions
ifeq ($(DLL),yes)
  include $(TOP_DIR)/builds/os2/linkdll.mk
else
include $(TOP_DIR)/builds/link_dos.mk
endif

# EOF
