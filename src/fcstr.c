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


fcExport int FcStrCmpIgnoreCase(const FcChar8 *s1, const FcChar8 *s2)
{
  /* It's actually wrong to use stricmp here, because this doesn't know
   * anything about UTF-8. But in the original FC package this also just
   * lowers ASCII characters for comparison, so this should be a good
   * and simple replacement.
   */
  return stricmp((char *)s1, (char *)s2);
}

fcExport FcChar8* FcStrCopy (const FcChar8 *s)
{
    int     len;
    FcChar8 *r;

    if (!s)
	return 0;
    len = strlen ((char *) s) + 1;
    r = (FcChar8 *) malloc (len);
    if (!r)
	return 0;
    memcpy (r, s, len);
    return r;
}

fcExport FcStrSet* FcStrSetCreate (void)
{
    FcStrSet	*set = malloc (sizeof (FcStrSet));
    if (!set)
	return 0;
    set->ref = 1;
    set->num = 0;
    set->size = 0;
    set->strs = 0;
    return set;
}

void FcStrSetDestroy (FcStrSet *set)
{
    if (--set->ref == 0)
    {
	int	i;
    
	for (i = 0; i < set->num; i++)
	    free(set->strs[i]);
	if (set->strs)
	{
	    free (set->strs);
	}
	free (set);
    }
}

static FcBool _FcStrSetAppend (FcStrSet *set, FcChar8 *s)
{
    if (FcStrSetMember (set, s))
    {
	free (s);
	return FcTrue;
    }
    if (set->num == set->size)
    {
	FcChar8 **strs = malloc ((set->size + 2) * sizeof (FcChar8 *));

	if (!strs)
	    return FcFalse;
	if (set->num)
	    memcpy (strs, set->strs, set->num * sizeof (FcChar8 *));
	if (set->strs)
	{
	    free (set->strs);
	}
	set->size = set->size + 1;
	set->strs = strs;
    }
    set->strs[set->num++] = s;
    set->strs[set->num] = 0;
    return FcTrue;
}

fcExport FcBool FcStrSetMember (FcStrSet *set, const FcChar8 *s)
{
    int	i;

    for (i = 0; i < set->num; i++)
	if (!strcmp((char *)set->strs[i], (char *)s)) // i don't use FcStrCmp here as strcmp does the same
	    return FcTrue;
    return FcFalse;
}

fcExport FcBool FcStrSetAdd (FcStrSet *set, const FcChar8 *s)
{
    FcChar8 *new = FcStrCopy (s);
    if (!new)
	return FcFalse;
    if (!_FcStrSetAppend (set, new))
    {
	free (new);
	return FcFalse;
    }
    return FcTrue;
}

fcExport FcStrList* FcStrListCreate (FcStrSet *set)
{
    FcStrList	*list;

    list = malloc (sizeof (FcStrList));
    if (!list)
	return 0;
    list->set = set;
    set->ref++;
    list->n = 0;
    return list;
}

fcExport FcChar8* FcStrListNext (FcStrList *list)
{
    if (list->n >= list->set->num)
	return 0;
    return list->set->strs[list->n++];
}

fcExport void FcStrListDone (FcStrList *list)
{
    FcStrSetDestroy (list->set);
    free (list);
}

