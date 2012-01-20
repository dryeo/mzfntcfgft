/*
 * This code is (C) Netlabs.org
 * Authors:
 *    Doodle <doodle@netlabs.org>
 *    Peter Weilbacher <mozilla@weilbacher.org>
 *
 * Contributors:
 *    KO Myung-Hun <komh78@gmail.com>
 *    Alex Taylor <alex@altsan.org>
 *    Rich Walsh <rich@e-vertise.com>
 *    Silvan Scherrer <silvan.scherrer@aroa.ch>
 *
 */

#ifndef _FCINT_H_
#define _FCINT_H_

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <math.h> /* for fabs */
#include <float.h> /* for DBL_EPSILON */
#include <iconv.h>
#define INCL_DOS
#define INCL_WIN
#define INCL_DOSERRORS
#define INCL_SHLERRORS
#include <os2.h>
#include <fontconfig/fontconfig.h>
#include <fontconfig/fcfreetype.h>
#include <fontconfig/fcprivate.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_SFNT_NAMES_H
#include FT_TRUETYPE_IDS_H

#ifdef FC_CACHE_VERSION_STRING
#undef FC_CACHE_VERSION_STRING
#endif
#define FC_CACHE_VERSION_STRING "v1.3_with_GCC"

#define FC_MEM_CHARSET	    0
#define FC_MEM_CHARLEAF	    1
#define FC_MEM_FONTSET	    2
#define FC_MEM_FONTPTR	    3
#define FC_MEM_OBJECTSET    4
#define FC_MEM_OBJECTPTR    5
#define FC_MEM_MATRIX	    6
#define FC_MEM_PATTERN	    7
#define FC_MEM_PATELT	    8
#define FC_MEM_VALLIST	    9
#define FC_MEM_SUBSTATE	    10
#define FC_MEM_STRING	    11
#define FC_MEM_LISTBUCK	    12
#define FC_MEM_STRSET	    13
#define FC_MEM_STRLIST	    14
#define FC_MEM_CONFIG	    15
#define FC_MEM_LANGSET	    16
#define FC_MEM_ATOMIC	    17
#define FC_MEM_BLANKS	    18
#define FC_MEM_CACHE	    19
#define FC_MEM_STRBUF	    20
#define FC_MEM_SUBST	    21
#define FC_MEM_OBJECTTYPE   22
#define FC_MEM_CONSTANT	    23
#define FC_MEM_TEST	    24
#define FC_MEM_EXPR	    25
#define FC_MEM_VSTACK	    26
#define FC_MEM_ATTR	    27
#define FC_MEM_PSTACK	    28
#define FC_MEM_STATICSTR    29

#define FC_MEM_NUM	    30

#define FC_MIN(a,b) ((a) < (b) ? (a) : (b))
#define FC_MAX(a,b) ((a) > (b) ? (a) : (b))
#define FC_ABS(a)   ((a) < 0 ? -(a) : (a))

#ifdef FC_EXPORT_FUNCTIONS
# define fcExport __declspec(dllexport)
#else
# define fcExport
#endif

// #define LOOKUP_SFNT_NAME_DEBUG
// #define FONTCONFIG_DEBUG_PRINTF
// #define MATCH_DEBUG

#define DEFAULT_SERIF_FONT          "Times New Roman"
#define DEFAULT_SERIF_FONTJA        "Times New Roman WT J"
#define DEFAULT_SANSSERIF_FONT      "Helvetica"
#define DEFAULT_MONOSPACED_FONT     "Courier"
#define DEFAULT_SYMBOL_FONT         "Symbol Set"
#define DEFAULT_DINGBATS_FONT       "DejaVu Sans"

/* structure for the font cache in OS2.INI  */
typedef struct FontDescriptionCache_s
{
  char achFileName[CCHMAXPATH];
  struct stat FileStatus;
  char achFamilyName[128];
  char achStyleName[128];
  long lFontIndex;

  struct FontDescriptionCache_s *pNext;
} FontDescriptionCache_t, *FontDescriptionCache_p;

struct _FcPattern
{
    char *family;
    int slant;
    int weight;
    double pixelsize;
    int spacing;
    FcBool hinting;
    FcBool antialias;
    FcBool embolden;
    FcBool verticallayout;
    int hintstyle;
    FcBool autohint;
    FcBool bitmap;
    FcBool outline;
    int rgba;
    double size;
    char *style;
    FT_Face face;
    int ref;
    char *lang;
    FontDescriptionCache_p pFontDesc;
    FcLangSet *langset;
};

struct _FcCharSet {
    int		    ref;	/* reference count */
    int		    num;	/* size of leaves and numbers arrays */
    intptr_t	    leaves_offset;
    intptr_t	    numbers_offset;
};

typedef struct _FcCharLeaf {
    FcChar32	map[256/32];
} FcCharLeaf;

struct _FcStrSet {
    int		    ref;	/* reference count */
    int		    num;
    int		    size;
    FcChar8	    **strs;
};

struct _FcStrList {
    FcStrSet	    *set;
    int		    n;
};

#define FC_REF_CONSTANT	    -1

void *pConfig;

#endif /* _FCINT_H_ */
