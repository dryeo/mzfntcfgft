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

#include "fcint.h"

fcExport FcPattern *FcPatternCreate(void)
{
  FcPattern *pResult = malloc(sizeof(FcPattern));
  if (pResult)
  {
    memset(pResult, 0, sizeof(FcPattern));
  }
  pResult->hinting = FcTrue; // default to hinting on
  pResult->antialias = FcTrue; // default to antialias on
  pResult->embolden = FcFalse; // no emboldening by default
  pResult->verticallayout = FcFalse; // horizontal layout by default
  pResult->autohint = FcFalse; // off by default as recommended in fontconfig.h
  pResult->bitmap = FcTrue; // default to using embedded bitmaps
  pResult->outline = FcTrue; // default to using outline
  /* set the strings to NULL for easy testing */
  pResult->family = NULL;
  pResult->style = NULL;
  pResult->lang = NULL;
  pResult->langset = NULL;
  pResult->ref = 1;
  return pResult;
}

fcExport void FcPatternDestroy(FcPattern *p)
{
  if (!p)
    return;

  if (--p->ref > 0)
    return;
  if (p->family)
    free(p->family);
  if (p->style)
    free(p->style);
  if (p->lang)
    free(p->lang);
  if (p->langset)
    FcLangSetDestroy(p->langset);
  free(p);
}

fcExport FcResult FcPatternGetInteger(const FcPattern *p, const char *object, int id, int *i)
{
  if (!p)
    return FcResultNoMatch;

  if (id) // we don't support more than one property of the same type
    return FcResultNoId;

  if (strcmp(object, FC_INDEX)==0)
  {
    *i = p->pFontDesc->lFontIndex;
    return FcResultMatch;
  }
  if (strcmp(object, FC_SLANT)==0)
  {
    *i = p->slant;
    return FcResultMatch;
  }
  if (strcmp(object, FC_WEIGHT)==0)
  {
    *i = p->weight;
    return FcResultMatch;
  }
  if (strcmp(object, FC_HINT_STYLE)==0)
  {
    *i = p->hintstyle;
    return FcResultMatch;
  }
  if (strcmp(object, FC_RGBA)==0)
  {
    *i = p->rgba;
    return FcResultMatch;
  }
  return FcResultNoMatch;
}

fcExport FcResult FcPatternGetString(const FcPattern *p, const char *object, int id, FcChar8 **s)
{
  if (!p)
    return FcResultNoMatch;

  if (id) // we don't support more than one property of the same type
    return FcResultNoId;

  if (strcmp(object, FC_FILE)==0)
  {
    *s = (FcChar8 *)p->pFontDesc->achFileName;
    return FcResultMatch;
  }

  if (strcmp(object, FC_FAMILY)==0)
  {
    if (p->family)
    {
      *s = (FcChar8 *)p->family;
      return FcResultMatch;
    } else
    if (p->pFontDesc)
    {
      *s = (FcChar8 *)p->pFontDesc->achFamilyName;
      return FcResultMatch;
    }
  }

  if (strcmp(object, FC_STYLE)==0)
  {
    if (p->style)
    {
      *s = (FcChar8 *)p->style;
      return FcResultMatch;
    } else
    if (p->pFontDesc)
    {
      *s = (FcChar8 *)p->pFontDesc->achStyleName;
      return FcResultMatch;
    }
  }

  return FcResultNoMatch;
}

fcExport FcResult FcPatternGetBool(const FcPattern *p, const char *object, int id, FcBool *b)
{
  if (!p)
    return FcResultNoMatch;

  if (id) // we don't support more than one property of the same type
    return FcResultNoId;

  if (strcmp(object, FC_HINTING)==0)
  {
    *b = p->hinting;
    return FcResultMatch;
  }
  if (strcmp(object, FC_ANTIALIAS)==0)
  {
    *b = p->antialias;
    return FcResultMatch;
  }
  if (strcmp(object, FC_EMBOLDEN)==0)
  {
    *b = p->embolden;
    return FcResultMatch;
  }
  if (strcmp(object, FC_VERTICAL_LAYOUT)==0)
  {
    *b = p->verticallayout;
    return FcResultMatch;
  }
  if (strcmp(object, FC_AUTOHINT)==0)
  {
    *b = p->autohint;
    return FcResultMatch;
  }
  if (strcmp(object, FC_EMBEDDED_BITMAP)==0)
  {
    *b = p->bitmap;
    return FcResultMatch;
  }
  if (strcmp(object, FC_OUTLINE)==0)
  {
    *b = p->outline;
    return FcResultMatch;
  }
  return FcResultNoMatch;
}

fcExport FcResult FcPatternGet(const FcPattern *p, const char *object, int id, FcValue *v)
{
  if (!v)
    return FcResultNoMatch;

  if (id) // we don't support more than one property of the same type
    return FcResultNoId;

  if (strcmp(object, FC_ANTIALIAS)==0)
  {
    v->type = FcTypeBool;
    v->u.b  = FcTrue;
    return FcResultMatch;
  }
  return FcResultNoMatch;
}

fcExport FcResult FcPatternGetLangSet(const FcPattern *p, const char *object, int id, FcLangSet **ls)
{

  if (id) // we don't support more than one property of the same type
    return FcResultNoId;

  if (p->langset)
  {
    *ls = p->langset;
    return FcResultMatch;
  }
  return FcResultNoMatch;
}

fcExport FcBool FcPatternAddInteger(FcPattern *p, const char *object, int i)
{
  if (!p)
    return FcFalse;

  if (strcmp(object, FC_INDEX)==0)
  {
    p->pFontDesc->lFontIndex = i;
    return FcTrue;
  }
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
  if (strcmp(object, FC_HINT_STYLE)==0)
  {
    p->hintstyle = i;
    return FcTrue;
  }
  if (strcmp(object, FC_RGBA)==0)
  {
    p->rgba = i;
    return FcTrue;
  }

  return FcFalse;
}

fcExport FcBool FcPatternAddDouble(FcPattern *p, const char *object, double d)
{
  if (!p)
    return FcFalse;

  if (strcmp(object, FC_PIXEL_SIZE)==0)
  {
    p->pixelsize = d;
    return FcTrue;
  }
  if (strcmp(object, FC_SIZE)==0)
  {
    p->size = d;
    return FcTrue;
  }

  return FcFalse;
}

fcExport FcResult FcPatternGetDouble(const FcPattern *p, const char *object, int id, double *d)
{
  if (!p)
    return FcResultNoMatch;

  if (id) // we don't support more than one property of the same type
    return FcResultNoId;

  if (strcmp(object, FC_PIXEL_SIZE)==0)
  {
    *d = p->pixelsize;
    return FcResultMatch;
  }
  if (strcmp(object, FC_SIZE)==0)
  {
    *d = p->size;
    return FcResultMatch;
  }

  return FcResultNoMatch;
}

fcExport FcBool FcPatternAddString(FcPattern *p, const char *object, const FcChar8 *s)
{
  if (!p)
    return FcFalse;

  if (strcmp(object, FC_FAMILY)==0)
  {
    if (p->family)
    {
      free(p->family); p->family = NULL;
    }
    p->family = strdup((const char *)s);
    return FcTrue;
  }

  if (strcmp(object, FC_STYLE)==0)
  {
    if (p->style)
    {
      free(p->style); p->style = NULL;
    }
    p->style = strdup((const char *)s);
    return FcTrue;
  }

  if (strcmp(object, FC_LANG)==0)
  {
    if (p->lang)
    {
      free(p->lang); p->lang = NULL;
    }
    p->lang = strdup((const char *)s);

/* as in newer fontconfig also the langset is built we need to do that here also */
    if (p->langset)
    {
      FcLangSetDestroy(p->langset); p->langset = NULL;
    }
    p->langset = FcLangSetCreate();
    if (p->langset)
    {
      if (!FcLangSetAdd (p->langset, p->lang))
         {
	    FcLangSetDestroy(p->langset);
            p->langset = NULL;
         }

    }
    return FcTrue;
  }

  return FcFalse;
}

fcExport FcBool FcPatternAddBool(FcPattern *p, const char *object, FcBool b)
{
  if (!p)
    return FcFalse;

  if (strcmp(object, FC_HINTING)==0)
  {
    p->hinting = b;
    return FcTrue;
  }
  if (strcmp(object, FC_ANTIALIAS)==0)
  {
    p->antialias = b;
    return FcTrue;
  }
  if (strcmp(object, FC_EMBOLDEN)==0)
  {
    p->embolden = b;
    return FcTrue;
  }
  if (strcmp(object, FC_VERTICAL_LAYOUT)==0)
  {
    p->verticallayout = b;
    return FcTrue;
  }
  if (strcmp(object, FC_AUTOHINT)==0)
  {
    p->autohint = b;
    return FcTrue;
  }
  if (strcmp(object, FC_EMBEDDED_BITMAP)==0)
  {
    p->bitmap = b;
    return FcTrue;
  }
  if (strcmp(object, FC_OUTLINE)==0)
  {
    p->outline = b;
    return FcTrue;
  }

  return FcFalse;
}

fcExport FcBool FcPatternAddFTFace(FcPattern *p, const char *object, const FT_Face f)
{
  if (!p)
    return FcFalse;

  if (strcmp(object, FC_FT_FACE)==0)
  {
    p->face = f;
    return FcTrue;
  }

  return FcFalse;
}

fcExport FcResult FcPatternGetFTFace(const FcPattern *p, const char *object, int id, FT_Face *f)
{
  if (!p)
    return FcResultNoMatch;

  if (id) // we don't support more than one property of the same type
    return FcResultNoId;

  if (strcmp(object, FC_FT_FACE)==0 && p && p->face)
  {
    *f = p->face;
    return FcResultMatch;
  }

  return FcResultNoMatch;
}

fcExport FcBool FcPatternDel(FcPattern *p, const char *object)
{
  // This is a stub.
  return FcTrue;
}

fcExport FcBool FcPatternEqual(const FcPattern *pa, const FcPattern *pb)
{
  /* point to the same pattern */
  if (pa == pb) {
    return FcTrue;
  }

  /* one of the patterns is invalid */
  if (!pa || !pb) {
    return FcFalse;
  }

  /* check string properties */
  /* If the string have the same address or they are both NULL it would mean
   * we had equal strings. If that is not the case we have to test if only
   * one of them is NULL and finally how they compare.
   */
  if (!(pa->family == pb->family || (!pa->family && !pb->family))
      && (!pa->family || !pb->family || stricmp(pa->family, pb->family) != 0))
  {
    return FcFalse;
  }

  if (!(pa->style == pb->style || (!pa->style && !pb->style))
      && (!pa->style || !pb->style || stricmp(pa->style, pb->style) != 0))
  {
    return FcFalse;
  }

  /* check int properties */
  if (pa->weight != pb->weight ||
      pa->slant != pb->slant ||
      pa->spacing != pb->spacing ||
      pa->hintstyle != pb->hintstyle ||
      pa->rgba != pb->rgba)
  {
    return FcFalse;
  }

  /* check double properties, better not compare directly */
  if (fabs(pa->pixelsize-pb->pixelsize) > DBL_EPSILON ||
      fabs(pa->size-pb->size) > DBL_EPSILON)
  {
    return FcFalse;
  }

  /* check bool properties, just direct comparison as for ints */
  if (pa->hinting != pb->hinting ||
      pa->antialias != pb->antialias ||
      pa->embolden != pb->embolden ||
      pa->verticallayout != pb->verticallayout ||
      pa->autohint != pb->autohint ||
      pa->bitmap != pb->bitmap ||
      pa->outline != pb->outline)
  {
    return FcFalse;
  }

  /* if we haven't returned by now then everything is equal */
  return FcTrue;
}

/*
 * Increment pattern reference count
 * Add another reference to p. Patterns are freed only when the reference
 * count reaches zero.
 */
fcExport void FcPatternReference(FcPattern *p)
{
  if (!p)
    return;

  p->ref++;
}

/*
 * Copy a pattern
 * Copy a pattern, returning a new pattern that matches p. Each pattern may be
 * modified without affecting the other.
 */
fcExport FcPattern *FcPatternDuplicate(const FcPattern *p)
{
  FcPattern *pResult = malloc(sizeof(FcPattern));
  if (pResult)
  {
    /* for a start, copy all entries */
    memcpy(pResult, p, sizeof(FcPattern));

    /* now correct the pointers */
    if (p->family)
      pResult->family = strdup(p->family);
    if (p->style)
      pResult->style = strdup(p->style);
    if (p->lang)
      pResult->lang = strdup(p->lang);
    if (p->langset)
      pResult->langset = FcLangSetCopy(p->langset);

    /* this is doubtful, but for now set the reference to 1,
     * so that the duplicate pattern is treated like a new one
     */
    pResult->ref = 1;
  }
  return pResult;
}

fcExport FcPattern *FcPatternBuild (FcPattern *p, ...)
{
  va_list  va;

  // according to fontconfig FcInit() is not needed in a app, so make sure its called
  if (!pConfig)
  {
    FcInit();
  }

  va_start (va, p);
  FcPatternVapBuild (p, p, va);
  va_end (va);
  return p;
}

fcExport FcBool FcPatternAdd (FcPattern *p, const char *object,
                              FcValue value, FcBool append)
{
  FcBool rc = FcTrue;

  switch (value.type)
  {
    case FcTypeInteger:
      rc = FcPatternAddInteger(p,  object, value.u.i);
      break;
    case FcTypeDouble:
      rc = FcPatternAddDouble(p,  object, value.u.d);
      break;
    case FcTypeString:
      rc = FcPatternAddString(p,  object, value.u.s);
      break;
    case FcTypeBool:
      rc = FcPatternAddBool(p,  object, value.u.b);
      break;
    case FcTypeFTFace:
      rc = FcPatternAddFTFace(p,  object, value.u.f);
      break;
    default:
      break;
  }

  return rc;
}

fcExport FcBool FcPatternAddWeak (FcPattern *p, const char *object,
                              FcValue value, FcBool append)
{
  FcBool rc = FcTrue;

  switch (value.type)
  {
    case FcTypeInteger:
      rc = FcPatternAddInteger(p,  object, value.u.i);
      break;
    case FcTypeDouble:
      rc = FcPatternAddDouble(p,  object, value.u.d);
      break;
    case FcTypeString:
      rc = FcPatternAddString(p,  object, value.u.s);
      break;
    case FcTypeBool:
      rc = FcPatternAddBool(p,  object, value.u.b);
      break;
    case FcTypeFTFace:
      rc = FcPatternAddFTFace(p,  object, value.u.f);
      break;
    default:
      break;
  }

  return rc;
}

