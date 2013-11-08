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

FcCharSet* FcCharSetCreate (void)
{
    FcCharSet	*fcs;

    fcs = (FcCharSet *) malloc (sizeof (FcCharSet));
    if (!fcs)
	return 0;
    fcs->ref = 1;
    fcs->num = 0;
    fcs->leaves_offset = 0;
    fcs->numbers_offset = 0;
    return fcs;
}
void
FcCharSetDestroy (FcCharSet *fcs)
{
    int i;
    
    if (fcs->ref == FC_REF_CONSTANT)
    {
	// FcCacheObjectDereference (fcs); because we cache different we just return
	return;
    }
    if (--fcs->ref > 0)
	return;
    for (i = 0; i < fcs->num; i++)
    {
	free (FcCharSetLeaf (fcs, i));
    }
    if (fcs->num)
    {
        /* the numbers here are estimates */
	free (FcCharSetLeaves (fcs));
	free (FcCharSetNumbers (fcs));
    }
    free (fcs);
}

/*
 * Search for the leaf containing with the specified num.
 * Return its index if it exists, otherwise return negative of
 * the (position + 1) where it should be inserted
 */


static int FcCharSetFindLeafForward (const FcCharSet *fcs, int start, FcChar16 num)
{
    FcChar16		*numbers = FcCharSetNumbers(fcs);
    FcChar16		page;
    int			low = start;
    int			high = fcs->num - 1;

    if (!numbers)
	return -1;
    while (low <= high)
    {
	int mid = (low + high) >> 1;
	page = numbers[mid];
	if (page == num)
	    return mid;
	if (page < num)
	    low = mid + 1;
	else
	    high = mid - 1;
    }
    if (high < 0 || (high < fcs->num && numbers[high] < num))
	high++;
    return -(high + 1);
}

static int FcCharSetFindLeafPos (const FcCharSet *fcs, FcChar32 ucs4)
{
    return FcCharSetFindLeafForward (fcs, 0, ucs4 >> 8);
}

#define FC_IS_ZERO_OR_POWER_OF_TWO(x) (!((x) & ((x)-1)))

static FcBool
FcCharSetPutLeaf (FcCharSet	*fcs, 
		  FcChar32	ucs4,
		  FcCharLeaf	*leaf, 
		  int		pos)
{
    intptr_t	*leaves = FcCharSetLeaves (fcs);
    FcChar16	*numbers = FcCharSetNumbers (fcs);

    ucs4 >>= 8;
    if (ucs4 >= 0x10000)
	return FcFalse;

    if (FC_IS_ZERO_OR_POWER_OF_TWO (fcs->num))
    {
      if (!fcs->num)
      {
        unsigned int alloced = 8;
	leaves = malloc (alloced * sizeof (*leaves));
	numbers = malloc (alloced * sizeof (*numbers));
      }
      else
      {
        unsigned int alloced = fcs->num;
	intptr_t *new_leaves, distance;

	alloced *= 2;
	new_leaves = realloc (leaves, alloced * sizeof (*leaves));
	numbers = realloc (numbers, alloced * sizeof (*numbers));

	distance = (intptr_t) new_leaves - (intptr_t) leaves;
	if (new_leaves && distance)
	{
	    int i;
	    for (i = 0; i < fcs->num; i++)
		new_leaves[i] -= distance;
	}
	leaves = new_leaves;
      }

      if (!leaves || !numbers)
	  return FcFalse;

      fcs->leaves_offset = FcPtrToOffset (fcs, leaves);
      fcs->numbers_offset = FcPtrToOffset (fcs, numbers);
    }
    
    memmove (leaves + pos + 1, leaves + pos, 
	     (fcs->num - pos) * sizeof (*leaves));
    memmove (numbers + pos + 1, numbers + pos,
	     (fcs->num - pos) * sizeof (*numbers));
    numbers[pos] = (FcChar16) ucs4;
    leaves[pos] = FcPtrToOffset (leaves, leaf);
    fcs->num++;
    return FcTrue;
}

static FcBool FcCharSetInsertLeaf (FcCharSet *fcs, FcChar32 ucs4, FcCharLeaf *leaf)
{
    int		    pos;

    pos = FcCharSetFindLeafPos (fcs, ucs4);
    if (pos >= 0)
    {
	free (FcCharSetLeaf (fcs, pos));
	FcCharSetLeaves(fcs)[pos] = FcPtrToOffset (FcCharSetLeaves(fcs),
						   leaf);
	return FcTrue;
    }
    pos = -pos - 1;
    return FcCharSetPutLeaf (fcs, ucs4, leaf, pos);
}

static const unsigned char	charToValue[256] = {
    /*     "" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /*   "\b" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\020" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\030" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /*    " " */ 0xff,  0x00,  0xff,  0x01,  0x02,  0x03,  0x04,  0xff, 
    /*    "(" */ 0x05,  0x06,  0x07,  0x08,  0xff,  0xff,  0x09,  0x0a, 
    /*    "0" */ 0x0b,  0x0c,  0x0d,  0x0e,  0x0f,  0x10,  0x11,  0x12, 
    /*    "8" */ 0x13,  0x14,  0xff,  0x15,  0x16,  0xff,  0x17,  0x18, 
    /*    "@" */ 0x19,  0x1a,  0x1b,  0x1c,  0x1d,  0x1e,  0x1f,  0x20, 
    /*    "H" */ 0x21,  0x22,  0x23,  0x24,  0x25,  0x26,  0x27,  0x28, 
    /*    "P" */ 0x29,  0x2a,  0x2b,  0x2c,  0x2d,  0x2e,  0x2f,  0x30, 
    /*    "X" */ 0x31,  0x32,  0x33,  0x34,  0xff,  0x35,  0x36,  0xff, 
    /*    "`" */ 0xff,  0x37,  0x38,  0x39,  0x3a,  0x3b,  0x3c,  0x3d, 
    /*    "h" */ 0x3e,  0x3f,  0x40,  0x41,  0x42,  0x43,  0x44,  0x45, 
    /*    "p" */ 0x46,  0x47,  0x48,  0x49,  0x4a,  0x4b,  0x4c,  0x4d, 
    /*    "x" */ 0x4e,  0x4f,  0x50,  0x51,  0x52,  0x53,  0x54,  0xff, 
    /* "\200" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\210" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\220" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\230" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\240" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\250" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\260" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\270" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\300" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\310" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\320" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\330" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\340" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\350" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\360" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
    /* "\370" */ 0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 
};

static FcChar8 *
FcCharSetParseValue (FcChar8 *string, FcChar32 *value)
{
    int		i;
    FcChar32	v;
    FcChar32	c;
    
    if (*string == ' ')
    {
	v = 0;
	string++;
    }
    else
    {
	v = 0;
	for (i = 0; i < 5; i++)
	{
	    if (!(c = (FcChar32) (unsigned char) *string++))
		return 0;
	    c = charToValue[c];
	    if (c == 0xff)
		return 0;
	    v = v * 85 + c;
	}
    }
    *value = v;
    return string;
}

FcCharSet* FcNameParseCharSet (FcChar8 *string)
{
    FcCharSet	*c;
    FcChar32	ucs4;
    FcCharLeaf	*leaf;
    FcCharLeaf	temp;
    FcChar32	bits;
    int		i;

    c = FcCharSetCreate ();
    if (!c)
	goto bail0;
    while (*string)
    {
	string = FcCharSetParseValue (string, &ucs4);
	if (!string)
	    goto bail1;
	bits = 0;
	for (i = 0; i < 256/32; i++)
	{
	    string = FcCharSetParseValue (string, &temp.map[i]);
	    if (!string)
		goto bail1;
	    bits |= temp.map[i];
	}
	if (bits)
	{
	    leaf = malloc (sizeof (FcCharLeaf));
	    if (!leaf)
		goto bail1;
	    *leaf = temp;
	    if (!FcCharSetInsertLeaf (c, ucs4, leaf))
		goto bail1;
	}
    }
    return c;
bail1:
    if (c->num)
    {
	free (FcCharSetLeaves (c));
    }
    if (c->num)
    {
	free (FcCharSetNumbers (c));
    }
    free (c);
bail0:
    return NULL;
}

