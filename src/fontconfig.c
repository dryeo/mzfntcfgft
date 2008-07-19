/*
 * This code is (C) Netlabs.org
 * Author: Doodle <doodle@scenergy.dfmk.hu>
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#define INCL_DOS
#define INCL_WIN
#define INCL_DOSERRORS
#include <os2.h>
#include <fontconfig/fontconfig.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#ifndef FC_CACHE_VERSION_STRING
# define FC_CACHE_VERSION_STRING "v1.0_with_GCC"
#endif

#ifdef FC_EXPORT_FUNCTIONS
# define fcExport __declspec(dllexport)
#else
# define fcExport
#endif

#define MAX_FONTLISTSIZE 4096

typedef struct FontDescriptionCache_s
{
  char achFileName[CCHMAXPATH];
  struct stat FileStatus;
  char achFamilyName[128];
  char achStyleName[128];

  struct FontDescriptionCache_s *pNext;
} FontDescriptionCache_t, *FontDescriptionCache_p;

struct _FcPattern
{
    char *family;
    int slant;
    int weight;
    int pixelsize;
    int spacing;

    FontDescriptionCache_p pFontDesc;
};


static FT_Library hFtLib;
static FontDescriptionCache_p pFontDescriptionCacheHead;
static FontDescriptionCache_p pFontDescriptionCacheLast;

fcExport void FcFini()
{
  FontDescriptionCache_p pToDelete;

  /* Uninitialize FreeType */
  FT_Done_FreeType(hFtLib);

  /* Destroy Font Description Cache */
  while (pFontDescriptionCacheHead)
  {
    pToDelete = pFontDescriptionCacheHead;
    pFontDescriptionCacheHead = pFontDescriptionCacheHead->pNext;
    free(pToDelete);
  }
}

static int CreateCache(FontDescriptionCache_p pFontCache, char *pchFontName, char *pchFontFileName)
{
  FT_Face ftface;
  int rc;
  ULONG ulSize;

  if (FT_New_Face(hFtLib, pchFontFileName, 0, &ftface))
  {
    /* Could not load font. */
    return 0;
  }

  strncpy(pFontCache->achFileName,
          pchFontFileName,
          sizeof(pFontCache->achFileName));
  if (stat(pchFontFileName, &(pFontCache->FileStatus))==-1)
  {
    /* Could not get status info, skip this font! */
    return 0;
  }
  if (ftface->family_name)
    strncpy(pFontCache->achFamilyName,
            ftface->family_name,
            sizeof(pFontCache->achFamilyName));
  if (ftface->style_name)
    strncpy(pFontCache->achStyleName,
            ftface->style_name,
            sizeof(pFontCache->achStyleName));

  FT_Done_Face(ftface);

  ulSize = sizeof(FontDescriptionCache_t);
  rc = PrfWriteProfileData(HINI_USER, "PM_Fonts_FontConfig_Cache_"FC_CACHE_VERSION_STRING, pchFontName,
                           pFontCache,  ulSize);

  return rc;
}

static void CacheFontDescription(char *pchFontName, char *pchFontFileName)
{
  int rc;
  FontDescriptionCache_t FontDesc;
  FontDescriptionCache_p pNewFontCacheEntry;
  ULONG ulSize;
  int iLen = strlen(pchFontFileName);

  /* Modify filename if needed */
  if (stricmp(pchFontFileName + (iLen-4), ".OFM") == 0)
  {
    pchFontFileName[iLen-3] = 'P';
    pchFontFileName[iLen-1] = 'B';
  }

  /* Check if we have a cache entry with this font name in INI file */
  memset(&FontDesc, 0, sizeof(FontDesc));
  ulSize = sizeof(FontDesc);
  rc = PrfQueryProfileData(HINI_USER, "PM_Fonts_FontConfig_Cache_"FC_CACHE_VERSION_STRING, pchFontName,
                           &FontDesc,  &ulSize);
  if ((ulSize!=sizeof(FontDesc)) || (!rc))
  {
    /* Hm, there is no cache for this file, try to create it! */
    if (!CreateCache(&FontDesc, pchFontName, pchFontFileName))
      return;
  } else
  {
    struct stat statBuf;

    /* There is cache for this file, check if it's up to date! */
    if (stat(pchFontFileName, &statBuf)==-1)
    {
      /* Could not get status info, skip this font! */
      return;
    }
    if ((statBuf.st_size != FontDesc.FileStatus.st_size) ||
        (statBuf.st_mtime != FontDesc.FileStatus.st_mtime))
    {
      /* The cache is not up to date, so recreate it! */
      if (!CreateCache(&FontDesc, pchFontName, pchFontFileName))
        return;
    }
  }

  /* Link this font to the list of available fonts */
  pNewFontCacheEntry = (FontDescriptionCache_p) malloc(sizeof(FontDescriptionCache_t));
  if (!pNewFontCacheEntry)
    return;

  memcpy(pNewFontCacheEntry, &FontDesc, sizeof(FontDesc));
  pNewFontCacheEntry->pNext = NULL;
  if (pFontDescriptionCacheLast)
  {
    pFontDescriptionCacheLast->pNext = pNewFontCacheEntry;
    pFontDescriptionCacheLast = pNewFontCacheEntry;
  } else
  {
    pFontDescriptionCacheLast = pFontDescriptionCacheHead = pNewFontCacheEntry;
  }
  // Then uppercase the names internally
  strupr(pNewFontCacheEntry->achFamilyName);
  strupr(pNewFontCacheEntry->achStyleName);

}

fcExport FcBool FcInit()
{
  ULONG ulBootDrive;
  char chBootDrive;
  char achFontNameList[MAX_FONTLISTSIZE];
  char *pchCurrentFont;
  char achFontFileName[CCHMAXPATH];
  char achAbsFontFileName[CCHMAXPATH];

  if (FT_Init_FreeType(&hFtLib))
  {
    /* Could not initialize FreeType */
    return FcFalse;
  }

  /* Go through all the available/installed fonts and
   * make sure we have an up-to-date description cache
   * for all of them */
  pFontDescriptionCacheHead = NULL;
  pFontDescriptionCacheLast = NULL;

  DosQuerySysInfo(QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, &ulBootDrive, sizeof(ULONG));
  chBootDrive = (char)( ulBootDrive + '@' );

  memset(achFontNameList, 0, MAX_FONTLISTSIZE);
  PrfQueryProfileString(HINI_USER, "PM_Fonts", NULL, NULL, achFontNameList, MAX_FONTLISTSIZE);

  pchCurrentFont = &(achFontNameList[0]);
  while (pchCurrentFont[0])
  {
    PrfQueryProfileString(HINI_USER, "PM_Fonts", pchCurrentFont, "", achFontFileName, CCHMAXPATH);

    if (achFontFileName[0] == '\\' )
    {
      achAbsFontFileName[0] = chBootDrive;
      achAbsFontFileName[1] = ':';
      achAbsFontFileName[2] = 0;
      strcat(achAbsFontFileName, achFontFileName);
    } else
    {
      strcpy(achAbsFontFileName, achFontFileName);
    }

    CacheFontDescription(pchCurrentFont, achAbsFontFileName);

    pchCurrentFont += strlen(pchCurrentFont)+1;
  }

  return FcTrue;
}

fcExport FcPattern *FcPatternCreate (void)
{
  FcPattern *pResult = malloc(sizeof(FcPattern));
  if (pResult)
  {
    memset(pResult, 0, sizeof(FcPattern));
  }
  return pResult;
}

fcExport void FcPatternDestroy (FcPattern *p)
{
  if (p->family)
    free(p->family);
  free(p);
}

fcExport FcBool FcConfigSubstitute (FcConfig	*config,
                           FcPattern	*p,
                           FcMatchKind	kind)
{
  // STUB
  return FcTrue;
}

fcExport void FcDefaultSubstitute (FcPattern *pattern)
{
  // STUB
}

fcExport FcResult FcPatternGetInteger (const FcPattern *p, const char *object, int n, int *i)
{
  if (strcmp(object, FC_INDEX)==0)
  {
    *i = 0;
    return FcResultMatch;
  }
  if (strcmp(object, FC_HINT_STYLE)==0)
  {
    *i = FC_HINT_FULL;
    return FcResultMatch;
  }
  return FcResultNoMatch;
}

fcExport FcResult FcPatternGetString (const FcPattern *p, const char *object, int n, FcChar8 ** s)
{
  if (strcmp(object, FC_FILE)==0)
  {
    *s = p->pFontDesc->achFileName;
    return FcResultMatch;
  }
  return FcResultNoMatch;
}

fcExport FcResult FcPatternGetBool (const FcPattern *p, const char *object, int n, FcBool *b)
{
  if (strcmp(object, FC_HINTING)==0)
  {
    *b = FcTrue;
    return FcResultMatch;
  }
  if (strcmp(object, FC_ANTIALIAS)==0)
  {
    *b = FcTrue;
    return FcResultMatch;
  }
  return FcResultNoMatch;
}

fcExport FcResult FcPatternGet (const FcPattern *p, const char *object, int id, FcValue *v)
{
  if (strcmp(object, FC_ANTIALIAS)==0)
  {
    v->type = FcTypeBool;
    v->u.b  = FcTrue;
    return FcResultMatch;
  }
  return FcResultNoMatch;
}


fcExport FcBool FcPatternAddInteger (FcPattern *p, const char *object, int i)
{
  if (strcmp(object, FC_SLANT)==0)
  {
    p->slant = i;
    return FcTrue;
  }
  if (strcmp(object, FC_WEIGHT)==0)
  {
    p->weight = i;
    return FcTrue;
  }
  if (strcmp(object, FC_PIXEL_SIZE)==0)
  {
    p->pixelsize = i;
    return FcTrue;
  }

  return FcFalse;
}

fcExport FcBool FcPatternAddString (FcPattern *p, const char *object, const FcChar8 *s)
{
  if (strcmp(object, FC_FAMILY)==0)
  {
    if (p->family)
    {
      free(p->family); p->family = NULL;
    }
    p->family = strdup(s);
    strupr(p->family);
    return FcTrue;
  }

  return FcFalse;
}

fcExport FcBool FcPatternAddBool (FcPattern *p, const char *object, FcBool b)
{
  // STUB
  return FcTrue;
}

#define DEFAULT_SERIF_FONT          "TIMES NEW ROMAN"
#define DEFAULT_SANSSERIF_FONT      "HELVETICA"
#define DEFAULT_MONOSPACED_FONT     "COURIER"

fcExport FcPattern *FcFontMatch (FcConfig	*config,
                                 FcPattern	*p,
                                 FcResult	*result)
{
  FontDescriptionCache_p pFont, pBestMatch;
  int iBestMatchScore;
  char achKey[512];
  int bWeightOk;
  int bSlantOk;

  pFont = pFontDescriptionCacheHead;
  pBestMatch = NULL;
  iBestMatchScore = -1;
  while (pFont)
  {
    if ((p->family) && (strstr(pFont->achFamilyName, p->family)))
    {
      // Family found, calculate score for it!
      if ( p->weight > FC_WEIGHT_NORMAL )
      {
        bWeightOk = (strstr(pFont->achStyleName, "BOLD")!=NULL);
      } else
        bWeightOk = 1;
  
      if ( p->slant > FC_SLANT_ROMAN )
      {
        bSlantOk = (strstr(pFont->achStyleName, "ITALIC")!=NULL);
      } else
        bSlantOk = 1;

      // Check if this score is better than the previous best one
      if (iBestMatchScore < bWeightOk*2 + bSlantOk)
      {
        pBestMatch = pFont;
        iBestMatchScore = bWeightOk*2 + bSlantOk;

        // Check if it's a perfect match!
        if ((bWeightOk) && (bSlantOk))
        {
          // Fount an exact match!
          break;
        }
      }
    }
    pFont = pFont->pNext;
  }
  // Use the one if we've found something
  if (pBestMatch)
    pFont = pBestMatch;

  if (!pFont)
  {
    // Did not find a good one by family name match,
    // search now with default font families!
    if ( p->spacing == FC_MONO )
    {
      strncpy(achKey, DEFAULT_MONOSPACED_FONT, sizeof(achKey));
    }
    else
    {
      if (
          (p->family) &&
          ((strstr( p->family, "SWISS" ) != NULL ) ||
           (strstr( p->family, "SANS" ) != NULL )
          )
         )
      {
        strncpy(achKey, DEFAULT_SANSSERIF_FONT, sizeof(achKey));
      }
      else
      {
        strncpy(achKey, DEFAULT_SERIF_FONT, sizeof(achKey));
      }
    }

    pFont = pFontDescriptionCacheHead;
    pBestMatch = NULL;
    iBestMatchScore = -1;
    while (pFont)
    {
      if (strstr(pFont->achFamilyName, achKey))
      {
        // Family found, calculate score for it!
        if ( p->weight > FC_WEIGHT_NORMAL )
        {
          bWeightOk = (strstr(pFont->achStyleName, "BOLD")!=NULL);
        } else
          bWeightOk = 1;

        if ( p->slant > FC_SLANT_ROMAN )
        {
          bSlantOk = (strstr(pFont->achStyleName, "ITALIC")!=NULL);
        } else
          bSlantOk = 1;

        // Check if this score is better than the previous best one
        if (iBestMatchScore < bWeightOk*2 + bSlantOk)
        {
          pBestMatch = pFont;
          iBestMatchScore = bWeightOk*2 + bSlantOk;

          // Check if it's a perfect match!
          if ((bWeightOk) && (bSlantOk))
          {
            // Fount an exact match!
            break;
          }
        }
      }
      pFont = pFont->pNext;
    }
    // Use the one if we've found something
    if (pBestMatch)
      pFont = pBestMatch;
  }

  if (pFont)
  {
    // If a font is found, then return with it!
    FcPattern *pResult = malloc(sizeof(FcPattern));
    if (pResult)
    {
      memcpy(pResult, p, sizeof(FcPattern));
      if (p->family)
        pResult->family = strdup(p->family);

      pResult->pFontDesc = pFont;
    }
    if (result)
      *result = FcResultMatch;
    return pResult;
  } else
  {
    if (result)
      *result = FcResultNoMatch;
    return NULL;
  }
}
