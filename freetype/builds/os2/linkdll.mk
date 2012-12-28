#
# FreeType 2 configuration rules for OS/2; gcc and dll
#


# Copyright 1996-2000 by
# David Turner, Robert Wilhelm, and Werner Lemberg.
#
# This file is part of the FreeType project, and may only be used, modified,
# and distributed under the terms of the FreeType project license,
# LICENSE.TXT.  By continuing to use, modify, or distribute this file you
# indicate that you have read the license and understand and accept it
# fully.
#
#  Link instructions for standard systems
#


ifdef BUILD_PROJECT

  .PHONY: clean_project distclean_project

  # Now include the main sub-makefile.  It contains all the rules used to
  # build the library with the previous variables defined.
  #
  include $(TOP_DIR)/builds/$(PROJECT).mk

  # The cleanup targets.
  #
  clean_project: clean_project_std
  distclean_project: distclean_project_std

  # This final rule is used to link all object files into a single library.
  # this is compiler-specific
  #
  $(PROJECT_LIBRARY): $(OBJECTS_LIST) $(OBJ_DIR)/$(OS2DLLNAME)


$(OBJ_DIR)/$(OS2DLLNAME): $(OBJECTS_LIST)
	rm -f $(OBJ_DIR)/$(PROJECT).a
	ar cru $(OBJ_DIR)/$(PROJECT)_s.a $(OBJECTS_LIST)
	gcc -g -Zbin-files -Zhigh-mem -Zomf -Zmap -Zdll $(EXPORTS_LIST) -o $@ $(OBJECTS_LIST) 
	emximp -o $(OBJ_DIR)/$(PROJECT).a $(EXPORTS_LIST)
	emximp -o $(OBJ_DIR)/$(PROJECT).lib $(EXPORTS_LIST)
#	cp $@ $(OBJ_DIR)

endif

# EOF
