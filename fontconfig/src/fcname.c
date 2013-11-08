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

/* 
 * Please do not change this list, it is used to initialize the object
 * list in this order to match the FC_foo_OBJECT constants. Those 
 * constants are written into cache files.
 */

static const FcObjectType _FcBaseObjectTypes[] = {
    { FC_FAMILY,	FcTypeString, },    /* 1 */
    { FC_FAMILYLANG,	FcTypeString, },
    { FC_STYLE,		FcTypeString, },
    { FC_STYLELANG,	FcTypeString, },
    { FC_FULLNAME,	FcTypeString, },
    { FC_FULLNAMELANG,	FcTypeString, },
    { FC_SLANT,		FcTypeInteger, },
    { FC_WEIGHT,	FcTypeInteger, },
    { FC_WIDTH,		FcTypeInteger, },
    { FC_SIZE,		FcTypeDouble, },
    { FC_ASPECT,	FcTypeDouble, },
    { FC_PIXEL_SIZE,	FcTypeDouble, },
    { FC_SPACING,	FcTypeInteger, },
    { FC_FOUNDRY,	FcTypeString, },
    { FC_ANTIALIAS,	FcTypeBool, },
    { FC_HINT_STYLE,    FcTypeInteger, },
    { FC_HINTING,	FcTypeBool, },
    { FC_VERTICAL_LAYOUT,   FcTypeBool, },
    { FC_AUTOHINT,	FcTypeBool, },
    { FC_GLOBAL_ADVANCE,    FcTypeBool, },
    { FC_FILE,		FcTypeString, },
    { FC_INDEX,		FcTypeInteger, },
    { FC_RASTERIZER,	FcTypeString, },
    { FC_OUTLINE,	FcTypeBool, },
    { FC_SCALABLE,	FcTypeBool, },
    { FC_DPI,		FcTypeDouble },
    { FC_RGBA,		FcTypeInteger, },
    { FC_SCALE,		FcTypeDouble, },
    { FC_MINSPACE,	FcTypeBool, },
    { FC_CHAR_WIDTH,	FcTypeInteger },
    { FC_CHAR_HEIGHT,	FcTypeInteger },
    { FC_MATRIX,	FcTypeMatrix },
    { FC_CHARSET,	FcTypeCharSet },
    { FC_LANG,		FcTypeLangSet },
    { FC_FONTVERSION,	FcTypeInteger },
    { FC_CAPABILITY,	FcTypeString },
    { FC_FONTFORMAT,	FcTypeString },
    { FC_EMBOLDEN,	FcTypeBool },
    { FC_EMBEDDED_BITMAP,   FcTypeBool },
    { FC_DECORATIVE,	FcTypeBool },
    { FC_LCD_FILTER,	FcTypeInteger }, /* 41 */
};

#define NUM_OBJECT_TYPES    (sizeof _FcBaseObjectTypes / sizeof _FcBaseObjectTypes[0])

typedef struct _FcObjectTypeList    FcObjectTypeList;

struct _FcObjectTypeList {
    const FcObjectTypeList  *next;
    const FcObjectType	    *types;
    int			    ntypes;
};

static const FcObjectTypeList _FcBaseObjectTypesList = {
    0,
    _FcBaseObjectTypes,
    NUM_OBJECT_TYPES,
};

static const FcObjectTypeList	*_FcObjectTypes = &_FcBaseObjectTypesList;

#define OBJECT_HASH_SIZE    31

typedef struct _FcObjectBucket {
    struct _FcObjectBucket  *next;
    FcChar32		    hash;
    FcObject		    id;
} FcObjectBucket;

static FcObjectBucket	*FcObjectBuckets[OBJECT_HASH_SIZE];

static FcObjectType	*FcObjects = (FcObjectType *) _FcBaseObjectTypes;
static int		FcObjectsNumber = NUM_OBJECT_TYPES;
static int		FcObjectsSize = 0;
static FcBool		FcObjectsInited;

static FcObjectType* FcObjectInsert (const char *name, FcType type)
{
    FcObjectType    *o;
    if (FcObjectsNumber >= FcObjectsSize)
    {
	int		newsize = FcObjectsNumber * 2;
	FcObjectType	*newobjects;
	
	if (FcObjectsSize)
	    newobjects = realloc (FcObjects, newsize * sizeof (FcObjectType));
	else
	{
	    newobjects = malloc (newsize * sizeof (FcObjectType));
	    if (newobjects)
		memcpy (newobjects, FcObjects,
			FcObjectsNumber * sizeof (FcObjectType));
	}
	if (!newobjects)
	    return NULL;
	FcObjects = newobjects;
	FcObjectsSize = newsize;
    }
    o = &FcObjects[FcObjectsNumber];
    o->object = name;
    o->type = type;
    ++FcObjectsNumber;
    return o;
}

static FcObject FcObjectId (FcObjectType *o)
{
    return o - FcObjects + 1;
}

static FcBool FcObjectHashInsert (const FcObjectType *object, FcBool copy)
{
    FcChar32	    hash = FcStringHash ((const FcChar8 *) object->object);
    FcObjectBucket  **p;
    FcObjectBucket  *b;
    FcObjectType    *o;

    if (!FcObjectsInited)
	FcObjectInit ();
    for (p = &FcObjectBuckets[hash%OBJECT_HASH_SIZE]; (b = *p); p = &(b->next))
    {
	o = FcObjects + b->id - 1;
        if (b->hash == hash && !strcmp (object->object, o->object))
            return FcFalse;
    }
    /*
     * Hook it into the hash chain
     */
    b = malloc (sizeof(FcObjectBucket));
    if (!b) 
	return FcFalse;
    if (copy)
    {
	o = FcObjectInsert (object->object, object->type);
	if (!o)
	{
	    free (b);
	    return FcFalse;
	}
    }
    else
	o = (FcObjectType *) object;
    b->next = NULL;
    b->hash = hash;
    b->id = FcObjectId (o);
    *p = b;
    return FcTrue;
}

FcBool FcObjectInit (void)
{
    int	i;

    if (FcObjectsInited)
	return FcTrue;

    FcObjectsInited = FcTrue;
    for (i = 0; i < NUM_OBJECT_TYPES; i++)
	if (!FcObjectHashInsert (&_FcBaseObjectTypes[i], FcFalse))
	    return FcFalse;
    return FcTrue;
}

fcExport static FcObjectType* FcObjectFindByName (const char *object, FcBool insert)
{
    FcChar32	    hash = FcStringHash ((const FcChar8 *) object);
    FcObjectBucket  **p;
    FcObjectBucket  *b;
    FcObjectType    *o;

    if (!FcObjectsInited)
	FcObjectInit ();
    for (p = &FcObjectBuckets[hash%OBJECT_HASH_SIZE]; (b = *p); p = &(b->next))
    {
	o = FcObjects + b->id - 1;
        if (b->hash == hash && !strcmp (object, (o->object)))
            return o;
    }
    if (!insert)
	return NULL;
    /*
     * Hook it into the hash chain
     */
    b = malloc (sizeof(FcObjectBucket));
    if (!b) 
	return NULL;
    object = (const char *) FcStrCopy ((FcChar8 *) object);
    if (!object) {
	free (b);
	return NULL;
    }
    o = FcObjectInsert (object, -1);
    b->next = NULL;
    b->hash = hash;
    b->id = FcObjectId (o);
    *p = b;
    return o;
}

const FcObjectType* FcNameGetObjectType (const char *object)
{
    return FcObjectFindByName (object, FcFalse);
}

static const FcConstant _FcBaseConstants[] = {
    { (FcChar8 *) "thin",	    "weight",   FC_WEIGHT_THIN, },
    { (FcChar8 *) "extralight",	    "weight",   FC_WEIGHT_EXTRALIGHT, },
    { (FcChar8 *) "ultralight",	    "weight",   FC_WEIGHT_EXTRALIGHT, },
    { (FcChar8 *) "light",	    "weight",   FC_WEIGHT_LIGHT, },
    { (FcChar8 *) "book",	    "weight",	FC_WEIGHT_BOOK, },
    { (FcChar8 *) "regular",	    "weight",   FC_WEIGHT_REGULAR, },
    { (FcChar8 *) "medium",	    "weight",   FC_WEIGHT_MEDIUM, },
    { (FcChar8 *) "demibold",	    "weight",   FC_WEIGHT_DEMIBOLD, },
    { (FcChar8 *) "semibold",	    "weight",   FC_WEIGHT_DEMIBOLD, },
    { (FcChar8 *) "bold",	    "weight",   FC_WEIGHT_BOLD, },
    { (FcChar8 *) "extrabold",	    "weight",   FC_WEIGHT_EXTRABOLD, },
    { (FcChar8 *) "ultrabold",	    "weight",   FC_WEIGHT_EXTRABOLD, },
    { (FcChar8 *) "black",	    "weight",   FC_WEIGHT_BLACK, },
    { (FcChar8 *) "heavy",	    "weight",	FC_WEIGHT_HEAVY, },

    { (FcChar8 *) "roman",	    "slant",    FC_SLANT_ROMAN, },
    { (FcChar8 *) "italic",	    "slant",    FC_SLANT_ITALIC, },
    { (FcChar8 *) "oblique",	    "slant",    FC_SLANT_OBLIQUE, },

    { (FcChar8 *) "ultracondensed", "width",	FC_WIDTH_ULTRACONDENSED },
    { (FcChar8 *) "extracondensed", "width",	FC_WIDTH_EXTRACONDENSED },
    { (FcChar8 *) "condensed",	    "width",	FC_WIDTH_CONDENSED },
    { (FcChar8 *) "semicondensed", "width",	FC_WIDTH_SEMICONDENSED },
    { (FcChar8 *) "normal",	    "width",	FC_WIDTH_NORMAL },
    { (FcChar8 *) "semiexpanded",   "width",	FC_WIDTH_SEMIEXPANDED },
    { (FcChar8 *) "expanded",	    "width",	FC_WIDTH_EXPANDED },
    { (FcChar8 *) "extraexpanded",  "width",	FC_WIDTH_EXTRAEXPANDED },
    { (FcChar8 *) "ultraexpanded",  "width",	FC_WIDTH_ULTRAEXPANDED },
    
    { (FcChar8 *) "proportional",   "spacing",  FC_PROPORTIONAL, },
    { (FcChar8 *) "dual",	    "spacing",  FC_DUAL, },
    { (FcChar8 *) "mono",	    "spacing",  FC_MONO, },
    { (FcChar8 *) "charcell",	    "spacing",  FC_CHARCELL, },

    { (FcChar8 *) "unknown",	    "rgba",	    FC_RGBA_UNKNOWN },
    { (FcChar8 *) "rgb",	    "rgba",	    FC_RGBA_RGB, },
    { (FcChar8 *) "bgr",	    "rgba",	    FC_RGBA_BGR, },
    { (FcChar8 *) "vrgb",	    "rgba",	    FC_RGBA_VRGB },
    { (FcChar8 *) "vbgr",	    "rgba",	    FC_RGBA_VBGR },
    { (FcChar8 *) "none",	    "rgba",	    FC_RGBA_NONE },

    { (FcChar8 *) "hintnone",	    "hintstyle",   FC_HINT_NONE },
    { (FcChar8 *) "hintslight",	    "hintstyle",   FC_HINT_SLIGHT },
    { (FcChar8 *) "hintmedium",	    "hintstyle",   FC_HINT_MEDIUM },
    { (FcChar8 *) "hintfull",	    "hintstyle",   FC_HINT_FULL },

    { (FcChar8 *) "antialias",	    "antialias",    FcTrue },
    { (FcChar8 *) "hinting",	    "hinting",	    FcTrue },
    { (FcChar8 *) "verticallayout", "verticallayout",	FcTrue },
    { (FcChar8 *) "autohint",	    "autohint",	    FcTrue },
    { (FcChar8 *) "globaladvance",  "globaladvance",	FcTrue },
    { (FcChar8 *) "outline",	    "outline",	    FcTrue },
    { (FcChar8 *) "scalable",	    "scalable",	    FcTrue },
    { (FcChar8 *) "minspace",	    "minspace",	    FcTrue },
    { (FcChar8 *) "embolden",	    "embolden",	    FcTrue },
    { (FcChar8 *) "embeddedbitmap", "embeddedbitmap",	FcTrue },
    { (FcChar8 *) "decorative",	    "decorative",   FcTrue },
    { (FcChar8 *) "lcdnone",	    "lcdfilter",    FC_LCD_NONE },
    { (FcChar8 *) "lcddefault",	    "lcdfilter",    FC_LCD_DEFAULT },
    { (FcChar8 *) "lcdlight",	    "lcdfilter",    FC_LCD_LIGHT },
    { (FcChar8 *) "lcdlegacy",	    "lcdfilter",    FC_LCD_LEGACY },
};

#define NUM_FC_CONSTANTS   (sizeof _FcBaseConstants/sizeof _FcBaseConstants[0])

typedef struct _FcConstantList FcConstantList;

struct _FcConstantList {
    const FcConstantList    *next;
    const FcConstant	    *consts;
    int			    nconsts;
};

static const FcConstantList _FcBaseConstantList = {
    0,
    _FcBaseConstants,
    NUM_FC_CONSTANTS
};

static const FcConstantList	*_FcConstants = &_FcBaseConstantList;

fcExport const FcConstant * FcNameGetConstant (FcChar8 *string)
{
    const FcConstantList    *l;
    int			    i;

    for (l = _FcConstants; l; l = l->next)
    {
	for (i = 0; i < l->nconsts; i++)
	    if (!FcStrCmpIgnoreCase (string, l->consts[i].name))
		return &l->consts[i];
    }
    return 0;
}

FcBool FcNameConstant (FcChar8 *string, int *result)
{
    const FcConstant	*c;

    if ((c = FcNameGetConstant(string)))
    {
	*result = c->value;
	return FcTrue;
    }
    return FcFalse;
}

FcBool FcNameBool (const FcChar8 *v, FcBool *result)
{
    char    c0, c1;

    c0 = *v;
    c0 = FcToLower (c0);
    if (c0 == 't' || c0 == 'y' || c0 == '1')
    {
	*result = FcTrue;
	return FcTrue;
    }
    if (c0 == 'f' || c0 == 'n' || c0 == '0')
    {
	*result = FcFalse;
	return FcTrue;
    }
    if (c0 == 'o')
    {
	c1 = v[1];
	c1 = FcToLower (c1);
	if (c1 == 'n')
	{
	    *result = FcTrue;
	    return FcTrue;
	}
	if (c1 == 'f')
	{
	    *result = FcFalse;
	    return FcTrue;
	}
    }
    return FcFalse;
}

static FcValue FcNameConvert (FcType type, FcChar8 *string, FcMatrix *m)
{
    FcValue	v;

    v.type = type;
    switch (v.type) {
    case FcTypeInteger:
	if (!FcNameConstant (string, &v.u.i))
	    v.u.i = atoi ((char *) string);
	break;
    case FcTypeString:
	v.u.s = FcStrStaticName(string);
	if (!v.u.s)
	    v.type = FcTypeVoid;
	break;
    case FcTypeBool:
	if (!FcNameBool (string, &v.u.b))
	    v.u.b = FcFalse;
	break;
    case FcTypeDouble:
	v.u.d = strtod ((char *) string, 0);
	break;
    case FcTypeMatrix:
	v.u.m = m;
	sscanf ((char *) string, "%lg %lg %lg %lg", &m->xx, &m->xy, &m->yx, &m->yy);
	break;
    case FcTypeCharSet:
	v.u.c = FcNameParseCharSet (string);
	if (!v.u.c)
	    v.type = FcTypeVoid;
	break;
    case FcTypeLangSet:
	v.u.l = FcNameParseLangSet (string);
	if (!v.u.l)
	    v.type = FcTypeVoid;
	break;
    default:
	break;
    }
    return v;
}

static const FcChar8* FcNameFindNext (const FcChar8 *cur, const char *delim, FcChar8 *save, FcChar8 *last)
{
    FcChar8    c;
    
    while ((c = *cur))
    {
	if (c == '\\')
	{
	    ++cur;
	    if (!(c = *cur))
		break;
	}
	else if (strchr (delim, c))
	    break;
	++cur;
	*save++ = c;
    }
    *save = 0;
    *last = *cur;
    if (*cur)
	cur++;
    return cur;
}

fcExport FcPattern* FcNameParse (const FcChar8 *name)
{
    FcChar8		*save;
    FcPattern		*pat;
    double		d;
    FcChar8		*e;
    FcChar8		delim;
    FcValue		v;
    FcMatrix		m;
    const FcObjectType	*t;
    const FcConstant	*c;

    /* freed below */
    save = malloc (strlen ((char *) name) + 1);
    if (!save)
	goto bail0;
    pat = FcPatternCreate ();
    if (!pat)
	goto bail1;

    for (;;)
    {
	name = FcNameFindNext (name, "-,:", save, &delim);
	if (save[0])
	{
	    if (!FcPatternAddString (pat, FC_FAMILY, save))
		goto bail2;
	}
	if (delim != ',')
	    break;
    }
    if (delim == '-')
    {
	for (;;)
	{
	    name = FcNameFindNext (name, "-,:", save, &delim);
	    d = strtod ((char *) save, (char **) &e);
	    if (e != save)
	    {
		if (!FcPatternAddDouble (pat, FC_SIZE, d))
		    goto bail2;
	    }
	    if (delim != ',')
		break;
	}
    }
    while (delim == ':')
    {
	name = FcNameFindNext (name, "=_:", save, &delim);
	if (save[0])
	{
	    if (delim == '=' || delim == '_')
	    {
		t = FcNameGetObjectType ((char *) save);
		for (;;)
		{
		    name = FcNameFindNext (name, ":,", save, &delim);
		    if (t)
		    {
			v = FcNameConvert (t->type, save, &m);
			if (!FcPatternAdd (pat, t->object, v, FcTrue))
			{
			    switch (v.type) {
			    case FcTypeCharSet:
				FcCharSetDestroy ((FcCharSet *) v.u.c);
				break;
			    case FcTypeLangSet:
				FcLangSetDestroy ((FcLangSet *) v.u.l);
				break;
			    default:
				break;
			    }
			    goto bail2;
			}
			switch (v.type) {
			case FcTypeCharSet:
			    FcCharSetDestroy ((FcCharSet *) v.u.c);
			    break;
			case FcTypeLangSet:
			    FcLangSetDestroy ((FcLangSet *) v.u.l);
			    break;
			default:
			    break;
			}
		    }
		    if (delim != ',')
			break;
		}
	    }
	    else
	    {
		if ((c = FcNameGetConstant (save)))
		{
		    t = FcNameGetObjectType ((char *) c->object);
		    switch (t->type) {
		    case FcTypeInteger:
		    case FcTypeDouble:
			if (!FcPatternAddInteger (pat, c->object, c->value))
			    goto bail2;
			break;
		    case FcTypeBool:
			if (!FcPatternAddBool (pat, c->object, c->value))
			    goto bail2;
			break;
		    default:
			break;
		    }
		}
	    }
	}
    }

    free (save);
    return pat;

bail2:
    FcPatternDestroy (pat);
bail1:
    free (save);
bail0:
    return 0;
}
