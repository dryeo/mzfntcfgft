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

typedef struct {
    const FcChar8    	lang[8];
    const FcCharSet	charset;
} FcLangCharSet;

typedef struct {
    int begin;
    int end;
} FcLangCharSetRange;

#include "fclang.h"

struct _FcLangSet {
    FcStrSet	*extra;
    FcChar32    map_size;
    FcChar32	map[NUM_LANG_SET_MAP];
};

static void FcLangSetBitSet (FcLangSet *ls, unsigned int id)
{
  int bucket;

  id = fcLangCharSetIndices[id];
  bucket = id >> 5;
  if (bucket >= ls->map_size)
    return; /* shouldn't happen really */

  ls->map[bucket] |= ((FcChar32) 1 << (id & 0x1f));
}

static FcBool FcLangSetBitGet (const FcLangSet *ls, unsigned int id)
{
  int bucket;

  id = fcLangCharSetIndices[id];
  bucket = id >> 5;
  if (bucket >= ls->map_size)
    return FcFalse;

  return ((ls->map[bucket] >> (id & 0x1f)) & 1) ? FcTrue : FcFalse;
}

static int FcLangSetIndex (const FcChar8 *lang)
{
    int	    low, high, mid = 0;
    int	    cmp = 0;
    FcChar8 firstChar = FcToLower(lang[0]); 
    FcChar8 secondChar = firstChar ? FcToLower(lang[1]) : '\0';
    
    if (firstChar < 'a')
    {
	low = 0;
	high = fcLangCharSetRanges[0].begin;
    }
    else if(firstChar > 'z')
    {
	low = fcLangCharSetRanges[25].begin;
	high = NUM_LANG_CHAR_SET - 1;
    }
    else
    {
	low = fcLangCharSetRanges[firstChar - 'a'].begin;
	high = fcLangCharSetRanges[firstChar - 'a'].end;
	/* no matches */
	if (low > high)
	    return -low; /* next entry after where it would be */
    }

    while (low <= high)
    {
	mid = (high + low) >> 1;
	if(fcLangCharSets[mid].lang[0] != firstChar)
	    cmp = FcStrCmpIgnoreCase(fcLangCharSets[mid].lang, lang);
	else
	{   /* fast path for resolving 2-letter languages (by far the most common) after
	     * finding the first char (probably already true because of the hash table) */
	    cmp = fcLangCharSets[mid].lang[1] - secondChar;
	    if (cmp == 0 && 
		(fcLangCharSets[mid].lang[2] != '\0' || 
		 lang[2] != '\0'))
	    {
		cmp = FcStrCmpIgnoreCase(fcLangCharSets[mid].lang+2, 
					 lang+2);
	    }
	}
	if (cmp == 0)
	    return mid;
	if (cmp < 0)
	    low = mid + 1;
	else
	    high = mid - 1;
    }
    if (cmp < 0)
	mid++;
    return -(mid + 1);
}

#define FcLangEnd(c)	((c) == '-' || (c) == '\0')

FcLangResult FcLangCompare (const FcChar8 *s1, const FcChar8 *s2)
{
    FcChar8	    c1, c2;
    FcLangResult    result = FcLangDifferentLang;

    for (;;)
    {
	c1 = *s1++;
	c2 = *s2++;
	
	c1 = FcToLower (c1);
	c2 = FcToLower (c2);
	if (c1 != c2)
	{
	    if (FcLangEnd (c1) && FcLangEnd (c2))
		result = FcLangDifferentTerritory;
	    return result;
	}
	else if (!c1)
	    return FcLangEqual;
	else if (c1 == '-')
	    result = FcLangDifferentTerritory;
    }
}

/*
 * Return FcTrue when super contains sub. 
 *
 * super contains sub if super and sub have the same
 * language and either the same country or one
 * is missing the country
 */

static FcBool FcLangContains (const FcChar8 *super, const FcChar8 *sub)
{
    FcChar8	    c1, c2;

    for (;;)
    {
	c1 = *super++;
	c2 = *sub++;
	
	c1 = FcToLower (c1);
	c2 = FcToLower (c2);
	if (c1 != c2)
	{
	    /* see if super has a country while sub is mising one */
	    if (c1 == '-' && c2 == '\0')
		return FcTrue;
	    /* see if sub has a country while super is mising one */
	    if (c1 == '\0' && c2 == '-')
		return FcTrue;
	    return FcFalse;
	}
	else if (!c1)
	    return FcTrue;
    }
}

fcExport FcLangResult FcLangSetHasLang(const FcLangSet *ls, const FcChar8 *lang)
{
    int		    id;
    FcLangResult    best, r;
    int		    i;

    id = FcLangSetIndex (lang);
    if (id < 0)
	id = -id - 1;
    else if (FcLangSetBitGet (ls, id))
	return FcLangEqual;
    best = FcLangDifferentLang;
    for (i = id - 1; i >= 0; i--)
    {
	r = FcLangCompare (lang, fcLangCharSets[i].lang);
	if (r == FcLangDifferentLang)
	    break;
	if (FcLangSetBitGet (ls, i) && r < best)
	    best = r;
    }
    for (i = id; i < NUM_LANG_CHAR_SET; i++)
    {
	r = FcLangCompare (lang, fcLangCharSets[i].lang);
	if (r == FcLangDifferentLang)
	    break;
	if (FcLangSetBitGet (ls, i) && r < best)
	    best = r;
    }
    if (ls->extra)
    {
	FcStrList	*list = FcStrListCreate (ls->extra);
	FcChar8		*extra;
	
	if (list)
	{
	    while (best > FcLangEqual && (extra = FcStrListNext (list)))
	    {
		r = FcLangCompare (lang, extra);
		if (r < best)
		    best = r;
	    }
	    FcStrListDone (list);
	}
    }
    return best;
}

fcExport FcLangSet* FcLangSetCreate (void)
{
    FcLangSet	*ls;

    ls = malloc (sizeof (FcLangSet));
    if (!ls)
	return 0;
    memset (ls->map, '\0', sizeof (ls->map));
    ls->map_size = NUM_LANG_SET_MAP;
    ls->extra = 0;
    return ls;
}

fcExport void FcLangSetDestroy (FcLangSet *ls)
{
    if (ls->extra)
	FcStrSetDestroy (ls->extra);
    free (ls);
}

fcExport FcLangSet* FcLangSetCopy (const FcLangSet *ls)
{
    FcLangSet	*new;

    new = FcLangSetCreate ();
    if (!new)
	goto bail0;
    memset (new->map, '\0', sizeof (new->map));
    memcpy (new->map, ls->map, FC_MIN (sizeof (new->map), ls->map_size * sizeof (ls->map[0])));
    if (ls->extra)
    {
	FcStrList	*list;
	FcChar8		*extra;
	
	new->extra = FcStrSetCreate ();
	if (!new->extra)
	    goto bail1;

	list = FcStrListCreate (ls->extra);	
	if (!list)
	    goto bail1;
	
	while ((extra = FcStrListNext (list)))
	    if (!FcStrSetAdd (new->extra, extra))
	    {
		FcStrListDone (list);
		goto bail1;
	    }
	FcStrListDone (list);
    }
    return new;
bail1:
    FcLangSetDestroy (new);
bail0:
    return 0;
}

FcBool FcLangSetAdd (FcLangSet *ls, const FcChar8 *lang)
{
    int	    id;

    id = FcLangSetIndex (lang);
    if (id >= 0)
    {
	FcLangSetBitSet (ls, id);
	return FcTrue;
    }
    if (!ls->extra)
    {
	ls->extra = FcStrSetCreate ();
	if (!ls->extra)
	    return FcFalse;
    }
    return FcStrSetAdd (ls->extra, lang);
}

static FcBool FcLangSetContainsLang (const FcLangSet *ls, const FcChar8 *lang)
{
    int		    id;
    int		    i;

    id = FcLangSetIndex (lang);
    if (id < 0)
	id = -id - 1;
    else if (FcLangSetBitGet (ls, id))
	return FcTrue;
    /*
     * search up and down among equal languages for a match
     */
    for (i = id - 1; i >= 0; i--)
    {
	if (FcLangCompare (fcLangCharSets[i].lang, lang) == FcLangDifferentLang)
	    break;
	if (FcLangSetBitGet (ls, i) &&
	    FcLangContains (fcLangCharSets[i].lang, lang))
	    return FcTrue;
    }
    for (i = id; i < NUM_LANG_CHAR_SET; i++)
    {
	if (FcLangCompare (fcLangCharSets[i].lang, lang) == FcLangDifferentLang)
	    break;
	if (FcLangSetBitGet (ls, i) &&
	    FcLangContains (fcLangCharSets[i].lang, lang))
	    return FcTrue;
    }
    if (ls->extra)
    {
	FcStrList	*list = FcStrListCreate (ls->extra);
	FcChar8		*extra;
	
	if (list)
	{
	    while ((extra = FcStrListNext (list)))
	    {
		if (FcLangContains (extra, lang))
		    break;
	    }
	    FcStrListDone (list);
    	    if (extra)
		return FcTrue;
	}
    }
    return FcFalse;
}

/*
 * return FcTrue if lsa contains every language in lsb
 */
FcBool FcLangSetContains (const FcLangSet *lsa, const FcLangSet *lsb)
{
    int		    i, j, count;
    FcChar32	    missing;

    /*
     * check bitmaps for missing language support
     */
    count = FC_MIN (lsa->map_size, lsb->map_size);
    count = FC_MIN (NUM_LANG_SET_MAP, count);
    for (i = 0; i < count; i++)
    {
	missing = lsb->map[i] & ~lsa->map[i];
	if (missing)
	{
	    for (j = 0; j < 32; j++)
		if (missing & (1 << j)) 
		{
		    if (!FcLangSetContainsLang (lsa,
						fcLangCharSets[fcLangCharSetIndicesInv[i*32 + j]].lang))
		    {
			return FcFalse;
		    }
		}
	}
    }
    if (lsb->extra)
    {
	FcStrList   *list = FcStrListCreate (lsb->extra);
	FcChar8	    *extra;

	if (list)
	{
	    while ((extra = FcStrListNext (list)))
	    {
		if (!FcLangSetContainsLang (lsa, extra))
		{
		    break;
		}
	    }
	    FcStrListDone (list);
	    if (extra)
		return FcFalse;
	}
    }
    return FcTrue;
}

