   Dependent lib for Mozilla Cairo builds
  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Setting it up
To make use of this package for compilation of a Mozilla build that makes use
of the cairo library, you need a GCC, and various other libraries and
environment variables set. Please follow the guide at
   https://developer.mozilla.org/En/Developer_Guide/Build_Instructions/OS//2_Build_Prerequisites
   https://developer.mozilla.org/En/Developer_Guide/Build_Instructions/OS//2_Build_Prerequisites/Building_on_OS//2_using_Mercurial
how to do this.

Move the whole directory containing this package where you want it installed.
First Freetype has to be built by cding to the freetype directory and doing make twice.
The first make will set things up for using the OS/2 make files in freetype\build and the second make will build the libraries.
Subsequant builds will only need make invoked once.
Then go to freetype\objs and copy either freetype_s.a for static building or freetype.lib to lib\mozft.a or moft.lib depending on the original library name.
Then go to the fontconfig directory and do make to build the fontconfig libraries. Once again copy the correct fontconfig library to lib\mzfntcfg.a or lib\mzfntcfg.lib

For using in other products double check the lib\pkgconfig pc files for
correct prefix and set PKG_CONFIG_PATH to point to the location of the
pc files. eg SET PKG_CONFIG_PATH=g:/mzfntcfgft/lib/pkgconfig;%PKG_CONFIG_PATH%

Now be sure to also adapt the MZFNTCFGFT variable in your setmozenv.cmd, so
that this package can be used by the Mozilla build process. In case your
setmozenv.cmd does _not_ yet contain these variables add the
following lines:
  'set MZFTCFGFT=%ROOT%\mzfntcfgft'
  'set MZFTCFGFT2='backtoforward(value('MZFTCFGFT',,'OS2ENVIRONMENT'));


- Sources
The required source code is contained in this package. It is compiled from bits
of the packages
   fontconfig           (the Netlabs version by ErOs2 and Doodle, enhanced by
                        Peter Weilbacher in collaboration with Doodle)
   fontconfig-2.3.2-os2 (only the header files of the original Fontconfig package)
Both are part of the *dependencies* packages on <ftp://ftp.netlabs.org/pub/Cairo/>.
   freetype-2.4.9.tar.bz2 (from http://sourceforge.net/projects/freetype/files/freetype2/2.4.9/)


- Using the library
If you built a Mozilla application with SVG support or if you compile a current
trunk build, the above configuration will automatically link this library
statically into one of the DLLs or the static EXE (depending on the build
configuration). With newer then Gecko 4 it will be statically linked to XUL.DLl


- Copyright
See mzfntcfg.COPYING for copyright information. That file contains the required
copyright notes from both base packages.
(If you want to ship a program linked against this library, you should not
forget to package mzfntcfg.COPYING, too, unless you built the program from
current Mozilla trunk, which already has the necessary info in about:license.)


- History
20121227  - Various fixes to use the latest for building Mozilla apps
20121221  - Update to Freetype 2.4.11
20120522  - Updated Fontconfig to build nicely static and dynamic in one go
20120522  - Seperate the Fontconfig part
20120521  - Updated Freetype to build nicely static and dynamic in one go
20120310  - Update to Freetype 2.4.9
20120306  - Separate the src directories into fontconfig and freetype and temporarily remove Peters fixes in anticapation of Silvans fixes from Lucide
20120306  - Add tag Gecko_10_Release for the revision built against Firefox 10.x etc
20111116  - Update to Freetype 2.4.8
20110918  - Add support for FC_OUTLINE. Patch by KO Myung-Hun
20110804  - More Japanese font fixes and some italic fixes. Patch by Alex Taylor
20110804  - Update to Freetype 2.4.5
20110804  - Add a stub for FcPatternAddWeak to fix a Mozilla build break
20110407  - Updates for Poppler, add (Japanese) language support, add additional fonts. Patch from Silvan Scherrer
20110407  - Bug fix from Rich Walsh
20101204  - Misc fixes. Thanks to Rich Walsh for some of these.
20101203  - Update to Freetyoe 2.4.4
20101125  - Update to Freetype 2.4.3
20101125  - DBCS fixes and Wingdings and Symbol font fixes. Patch by KO Myung-Hun
20101122  - Add more font faces and misc font faces fixes. Patch by Alex Taylor
20090811  - Update to FreeType 2.3.9.
          - Improve emboldened font display
20080806  - Implement FcPatternAddFTFace() and FcPatternGetFTFace() to be able
            to build newer post-1.6 cairo and Mozilla from mozilla-central
          - Add FcFreeTypeCharIndex()
          - Use CVS version of T1_Face_Init() in FreeType to properly represent
            OS/2 Type1 fonts
20080617  - fix problem with font styles introduced with FT 2.3.6
          - update FontConfig to write its cache to a seperate INI file instead
            of OS2.INI (by Doodle)
          - make font cache more robust by removing obsolete fonts (by Doodle)
          - This is the version used to build Firefox 3.0.
20080615  - update to FreeType 2.3.6
          + remove unused Type42 driver from FreeType
          - improve emboldening
          - don't use preferred font family and font style to be in line with
            OS/2 font handling
          - improve FcFontList()
20080315  - Compile FT stroking code, too
          - Implement FcPatternEqual()
20080307  Lots of improvements to the FontConfig port with help from Doodle
          - Improve matching of installed fonts (exact font name matched first)
          - Add support for oblique fonts
          - More FontConfig properties supported (hint style, RGBA, style, etc.)
          - Add function FcFreeTypeQuery()
          - Support multiple font faces in one font file
          . Because of the latter, has to cache fonts in a new OS2.INI key
            (PM_Fonts_FontConfig_Cache_v1.1)
20071125  - Upgrade to FT 2.3.5
          - Add a few features and fix a few bugs of the OS/2 Fontconfig
            implementation
20070528  - Change from DLL and DLL import library to static import library
          + Fix a small mistake when returning the matched font name
20070518  - Upgrade to FT 2.3.4 and switch from standard FC to Netlabs
            version


- Contact person
If you want to complain please post to the group mozilla.dev.ports.os2 on the
newsserver news.mozilla.org.

Peter Weilbacher <mozilla@Weilbacher.org>, 2009-08-11
Dave Yeo <dave.r.yeo@gmail.com>, current maintainer
