// $Id$
/* @@tag:xara-cn@@ DO NOT MODIFY THIS LINE
================================XARAHEADERSTART===========================
 
               Xara LX, a vector drawing and manipulation program.
                    Copyright (C) 1993-2006 Xara Group Ltd.
       Copyright on certain contributions may be held in joint with their
              respective authors. See AUTHORS file for details.

LICENSE TO USE AND MODIFY SOFTWARE
----------------------------------

This file is part of Xara LX.

Xara LX is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as published
by the Free Software Foundation.

Xara LX and its component source files are distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with Xara LX (see the file GPL in the root directory of the
distribution); if not, write to the Free Software Foundation, Inc., 51
Franklin St, Fifth Floor, Boston, MA  02110-1301 USA


ADDITIONAL RIGHTS
-----------------

Conditional upon your continuing compliance with the GNU General Public
License described above, Xara Group Ltd grants to you certain additional
rights. 

The additional rights are to use, modify, and distribute the software
together with the wxWidgets library, the wxXtra library, and the "CDraw"
library and any other such library that any version of Xara LX relased
by Xara Group Ltd requires in order to compile and execute, including
the static linking of that library to XaraLX. In the case of the
"CDraw" library, you may satisfy obligation under the GNU General Public
License to provide source code by providing a binary copy of the library
concerned and a copy of the license accompanying it.

Nothing in this section restricts any of the rights you have under
the GNU General Public License.


SCOPE OF LICENSE
----------------

This license applies to this program (XaraLX) and its constituent source
files only, and does not necessarily apply to other Xara products which may
in part share the same code base, and are subject to their own licensing
terms.

This license does not apply to files in the wxXtra directory, which
are built into a separate library, and are subject to the wxWindows
license contained within that directory in the file "WXXTRA-LICENSE".

This license does not apply to the binary libraries (if any) within
the "libs" directory, which are subject to a separate license contained
within that directory in the file "LIBS-LICENSE".


ARRANGEMENTS FOR CONTRIBUTION OF MODIFICATIONS
----------------------------------------------

Subject to the terms of the GNU Public License (see above), you are
free to do whatever you like with your modifications. However, you may
(at your option) wish contribute them to Xara's source tree. You can
find details of how to do this at:
  http://www.xaraxtreme.org/developers/

Prior to contributing your modifications, you will need to complete our
contributor agreement. This can be found at:
  http://www.xaraxtreme.org/developers/contribute/

Please note that Xara will not accept modifications which modify any of
the text between the start and end of this header (marked
XARAHEADERSTART and XARAHEADEREND).


MARKS
-----

Xara, Xara LX, Xara X, Xara X/Xtreme, Xara Xtreme, the Xtreme and Xara
designs are registered or unregistered trademarks, design-marks, and/or
service marks of Xara Group Ltd. All rights in these marks are reserved.


      Xara Group Ltd, Gaddesden Place, Hemel Hempstead, HP2 6EX, UK.
                        http://www.xara.com/

=================================XARAHEADEREND============================
 */

// This class contains all the file deflating code

#ifndef INC_ZIPINFLATE
#define INC_ZIPINFLATE

#include "zutil.h"

class ZStream;

/* Huffman code lookup table entry--this entry is four bytes for machines
   that have 16-bit pointers (e.g. PC's in the small or medium model).
   Valid extra bits (exop) are 0..13.  exop == -64 is EOB (end of block),
   exop == 16 means that v is a literal, exop < 0 means that v is a pointer
   to the next table, which codes -exop bits, and lastly exop == -128
   indicates an unused code.  If a code with exop == -128 is looked up,
   this implies an error in the data. */

//#if defined(STDC) || defined(sgi)
//typedef signed char Char;
//#else
typedef char Char; /* just hope that char is signed */
//#endif

// ------------------------------------------------------------------------------------------
// From inftrees.h
// ------------------------------------------------------------------------------------------

/* Huffman code lookup table entry--this entry is four bytes for machines
   that have 16-bit pointers (e.g. PC's in the small or medium model). */

typedef struct inflate_huft_s FAR inflate_huft;

struct inflate_huft_s {
  union
  {
    struct
	{
      Byte Exop;        /* number of extra bits or operation */
      Byte Bits;        /* number of bits in this code or subcode */
    } what;
    Bytef *pad;         /* pad structure to a power of 2 (4 bytes for */
  } word;               /*  16-bit, 8 bytes for 32-bit machines) */
  union
  {
    uInt Base;          /* literal, length base, or distance base */
    inflate_huft *Next; /* pointer to next level of table */
  } more;
};

/********************************************************************************************

********************************************************************************************/

/* And'ing with mask[n] masks the lower n bits */
extern uInt inflate_mask[];

// ------------------------------------------------------------------------------------------
// From inftrees.h
// ------------------------------------------------------------------------------------------

/********************************************************************************************
********************************************************************************************/

enum InflateCodesMode
	{   /* waiting for "i:"=input, "o:"=output, "x:"=nothing */
		START,    /* x: set up for LEN */
		LEN,      /* i: get length/literal/eob next */
		LENEXT,   /* i: getting length extra (have base) */
		DIST,     /* i: get distance next */
		DISTEXT,  /* i: getting distance extra */
		COPY,     /* o: copying bytes in window, waiting for space */
		LIT,      /* o: got literal, waiting for output space */
		WASH,     /* o: got eob, possibly still output waiting */
		END,      /* x: got eob and all data flushed */
		BADCODE   /* x: got error */
	};

/********************************************************************************************

>	class inflate_codes_state : public CCObject

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> Humprhys
	Created:	23/5/95
	Purpose:	An inflate codes state object for use by the inflater for the file compressor
				and decompressor.

********************************************************************************************/

class inflate_codes_state : public CC_CLASS_MEMDUMP
{
	// Give my name in memory dumps
	// If you need a different sort of decare, replace this one. 
	// See CCObject in the help file for info about DECLARE types
	CC_DECLARE_MEMDUMP(inflate_codes_state);

public:
	inflate_codes_state() {};
	~inflate_codes_state() {};

public:
  /* mode */
  InflateCodesMode  mode;		/* current inflate_codes mode */

  /* mode dependent information */
  uInt len;
  union
  {
    struct
    {
      inflate_huft *tree;		/* pointer into tree */
      uInt need;				/* bits needed */
    } code;						/* if LEN or DIST, where in tree */

    uInt lit;					/* if LIT, literal */

    struct
    {
      uInt get;					/* bits to get for extra */
      uInt dist;				/* distance back to copy from */
    } copy;						/* if EXT or COPY, where and how much */

  } sub;						/* submode */

  /* mode independent information */
  Byte lbits;					/* ltree bits decoded per branch */
  Byte dbits;					/* dtree bits decoder per branch */
  inflate_huft *ltree;			/* literal/length/eob tree */
  inflate_huft *dtree;			/* distance tree */

};

// ------------------------------------------------------------------------------------------
// From infutil.h
// ------------------------------------------------------------------------------------------

/********************************************************************************************
********************************************************************************************/

enum InflateBlockMode
  		{
			TYPE,     /* get type bits (3, including end bit) */
			LENS,     /* get lengths for stored */
			STORED,   /* processing stored block */
			TABLE,    /* get table lengths */
			BTREE,    /* get bit lengths tree for a dynamic block */
			DTREE,    /* get length, distance trees for a dynamic block */
			CODES,    /* processing fixed or dynamic block */
			DRY,      /* output remaining window bytes */
			BLOCKDONE,/* finished last block, done */
			BLOCKBAD  /* got a data error--stuck here */
	  	};

/********************************************************************************************

>	class inflate_blocks_state : public CCObject

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> Humprhys
	Created:	23/5/95
	Purpose:	An inflate blocks state object for use by the inflater for the file compressor
				and decompressor.

********************************************************************************************/

class inflate_blocks_state : public CC_CLASS_MEMDUMP
{
	// Give my name in memory dumps
	// If you need a different sort of decare, replace this one. 
	// See CCObject in the help file for info about DECLARE types
	CC_DECLARE_MEMDUMP(inflate_blocks_state);

public:
	inflate_blocks_state() {};
	~inflate_blocks_state() {};

public:
	/* inflate blocks semi-private state */
    InflateBlockMode mode;	/* current inflate_block mode */

	/* mode dependent information */
	union {
		uInt left;			/* if STORED, bytes left to copy */
		struct
		{
		  uInt table;				/* table lengths (14 bits) */
		  uInt index;				/* index into blens (or border) */
		  uInt *blens;				/* bit lengths of codes */
		  uInt bb;					/* bit length tree depth */
		  inflate_huft *tb;			/* bit length decoding tree */
		} trees;			/* if DTREE, decoding info for trees */
		struct
		{
		  inflate_huft *tl, *td;    /* trees to free */
		  inflate_codes_state *codes;
		} decode;           /* if CODES, current state */
	} sub;					/* submode */
	uInt last;				/* true if this block is the last block */

  /* mode independent information */
  uInt bitk;            /* bits in bit buffer */
  uLong bitb;           /* bit buffer */
  Bytef *window;        /* sliding window */
  Bytef *end;           /* one byte after sliding window */
  Bytef *read;          /* window read pointer */
  Bytef *write;         /* window write pointer */
  check_func checkfn;   /* check function */
  uLong check;          /* check on output */
};

// ------------------------------------------------------------------------------------------
// From ?.c
// ------------------------------------------------------------------------------------------

/********************************************************************************************
********************************************************************************************/

/* current inflate mode */

enum InflateMode
  		{
			METHOD,   /* waiting for method byte */
			FLAG,     /* waiting for flag byte */
			DICT4,    /* four dictionary check bytes to go */
			DICT3,    /* three dictionary check bytes to go */
			DICT2,    /* two dictionary check bytes to go */
			DICT1,    /* one dictionary check byte to go */
			DICT0,    /* waiting for inflateSetDictionary */
			BLOCKS,   /* decompressing blocks */
			CHECK4,   /* four check bytes to go */
			CHECK3,   /* three check bytes to go */
			CHECK2,   /* two check bytes to go */
			CHECK1,   /* one check byte to go */
			DONE,     /* finished check, done */
			BAD       /* got an error--stay here */
		};

/********************************************************************************************

>	class InflateState : public CCObject

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> Humprhys
	Created:	23/5/95
	Purpose:	An inflate state object for use by the inflater for the file compressor
				and decompressor.

********************************************************************************************/

class InflateState : public CC_CLASS_MEMDUMP
{
	// Give my name in memory dumps
	// If you need a different sort of decare, replace this one. 
	// See CCObject in the help file for info about DECLARE types
	CC_DECLARE_MEMDUMP(InflateState);

public:
	InflateState() {};
	~InflateState() {};

public:
	InflateMode    mode;	/* current inflate mode */

  /* mode dependent information */
  union {
    uInt method;		/* if FLAGS, method byte */
    struct
    {
      uLong was;		/* computed check value */
      uLong need;		/* stream check value */
    } check;			/* if CHECK, check values to compare */
    uInt marker;		/* if BAD, inflateSync's marker bytes count */
  } sub;				/* submode */

  /* mode independent information */
  INT32  nowrap;						/* flag for no wrapper */
  uInt wbits;						/* log2(window size)  (8..15, defaults to 15) */
  inflate_blocks_state *blocks;		/* current inflate_blocks state */

protected:

};

/********************************************************************************************

>	class ZipInflate : public CCObject

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> Humprhys
	Created:	23/5/95
	Purpose:	The inflater for the file compressor and decompressor.
				Allows the CCDiskFile to offer compression of files. At present just
				used to compress the native file format.

********************************************************************************************/

class ZipInflate : public CC_CLASS_MEMDUMP
{
	// Give my name in memory dumps
	// If you need a different sort of decare, replace this one. 
	// See CCObject in the help file for info about DECLARE types
	CC_DECLARE_MEMDUMP(ZipInflate);

public:
	ZipInflate();
	~ZipInflate();

public:
	INT32 Init(ZStream *strm);

	INT32 inflate(ZStream *strm, INT32 flush);

	INT32 SetDictionary(ZStream *z, const Bytef *dictionary, uInt dictLength);

	INT32 End(ZStream *strm);

	INT32 Init(ZStream *strm, INT32  windowBits);

	INT32 Sync(ZStream *strm);

	INT32 Reset(ZStream *strm);

protected:
	// from infutil.h
	// copy as much as possible from the sliding window to the output area
	INT32 inflate_flush(inflate_blocks_state *, ZStream *, INT32);

	// from infblocks.h
	inflate_blocks_state *inflate_blocks_new(ZStream *, check_func, uInt);

	INT32 inflate_blocks(inflate_blocks_state *, ZStream *, INT32);

	void inflate_blocks_reset(inflate_blocks_state *, ZStream *, uLongf *);

	INT32 inflate_blocks_free(inflate_blocks_state *, ZStream *, uLongf *);

	// New version 0.99
	void inflate_set_dictionary(
								inflate_blocks_state *s,
								const Bytef *d,  /* dictionary */
								uInt  n);       /* dictionary length */

	// from inftrees.h
#ifdef DEBUG
	  uInt inflate_hufts;
#endif

	INT32 inflate_trees_bits(
	    uIntf *,                    /* 19 code lengths */
		uIntf *,                    /* bits tree desired/actual depth */
		inflate_huft * FAR *,       /* bits tree result */
	    ZStream *);					/* for zalloc, zfree functions */

	INT32 inflate_trees_dynamic(
		uInt,                       /* number of literal/length codes */
		uInt,                       /* number of distance codes */
		uIntf *,                    /* that many (total) code lengths */
		uIntf *,                    /* literal desired/actual bit depth */
		uIntf *,                    /* distance desired/actual bit depth */
		inflate_huft * FAR *,       /* literal/length tree result */
		inflate_huft * FAR *,       /* distance tree result */
	    ZStream *);					/* for zalloc, zfree functions */

	INT32 inflate_trees_fixed(
		uIntf *,                    /* literal desired/actual bit depth */
		uIntf *,                    /* distance desired/actual bit depth */
		inflate_huft * FAR *,       /* literal/length tree result */
		inflate_huft * FAR *);		/* distance tree result */

	INT32 inflate_trees_free(
	    inflate_huft *,             /* tables to free */
	    ZStream *);					/* for zfree function */

	// inftrees.c
	INT32 huft_build(
					uIntf *,            /* code lengths in bits */
					uInt,               /* number of codes */
					uInt,               /* number of "simple" codes */
					uIntf *,            /* list of base values for non-simple codes */
					uIntf *,            /* list of extra bits for non-simple codes */
					inflate_huft * FAR*,/* result: starting table */
					uIntf *,            /* maximum lookup bits (returns actual) */
				    ZStream *);        /* for zalloc function */

	//voidpf falloc(
	//				voidpf,         /* opaque pointer (not used) */
	//				uInt,               /* number of items */
	//				uInt);             /* size of item */


	// from inffast.h
	INT32 inflate_fast(uInt, uInt, inflate_huft *, inflate_huft *,
				     inflate_blocks_state *, ZStream *);

	// from infcodes.h
	inflate_codes_state *inflate_codes_new( uInt, uInt,
										    inflate_huft *, inflate_huft *,
										    ZStream *);

	INT32 inflate_codes(inflate_blocks_state *, ZStream *, INT32);

	void inflate_codes_free(inflate_codes_state *, ZStream *);

};

#endif  // INC_ZIPINFLATE


