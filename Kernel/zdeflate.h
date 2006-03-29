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

#ifndef INC_ZIPDEFLATE
#define INC_ZIPDEFLATE

#include "zutil.h"

//#include "zstream.h"

/* ===========================================================================
 * Internal compression state.
 */

#define LENGTH_CODES 29
/* number of length codes, not counting the special END_BLOCK code */

#define LITERALS  256
/* number of literal bytes 0..255 */

#define L_CODES (LITERALS+1+LENGTH_CODES)
/* number of Literal or Length codes, including the END_BLOCK code */

#define D_CODES   30
/* number of distance codes */

#define BL_CODES  19
/* number of codes used to transfer the bit lengths */

#define HEAP_SIZE (2*L_CODES+1)
/* maximum heap size */

#define MAX_BITS 15
/* All codes must not exceed MAX_BITS bits */

#define INIT_STATE    42
#define BUSY_STATE   113
#define FINISH_STATE 666
/* Stream status */


/* Data structure describing a single value and its code string. */
typedef struct ct_data_s {
    union {
        ush  freq;       /* frequency count */
        ush  code;       /* bit string */
    } fc;
    union {
        ush  dad;        /* father node in Huffman tree */
        ush  len;        /* length of bit string */
    } dl;
} ct_data;

typedef struct static_tree_desc_s  static_tree_desc;

typedef struct tree_desc_s {
    ct_data *dyn_tree;           /* the dynamic tree */
    INT32     max_code;            /* largest code with non zero frequency */
    static_tree_desc *stat_desc; /* the corresponding static tree */
} tree_desc;

typedef ush Pos;
typedef Pos FAR Posf;
typedef unsigned IPos;

/* A Pos is an index in the character window. We use short instead of INT32 to
 * save space in the various tables. IPos is used only for parameter passing.
 */

/* Output a byte on the stream.
 * IN assertion: there is enough room in pending_buf.
 */
#define put_byte(s, c) {s->pending_buf[s->pending++] = (c);}


#define MIN_LOOKAHEAD (MAX_MATCH+MIN_MATCH+1)
/* Minimum amount of lookahead, except at the end of the input file.
 * See deflate.c for comments about the MIN_MATCH+1.
 */

#define MAX_DIST(s)  ((s)->w_size-MIN_LOOKAHEAD)
/* In order to simplify the code, particularly on 16 bit machines, match
 * distances are limited to MAX_DIST instead of WSIZE.
 */

class ZStream;

/********************************************************************************************

>	class DeflateState : public CCObject

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> Humprhys
	Created:	23/5/95
	Purpose:	A deflate state object for the deflater for the file compressor and decompressor.

********************************************************************************************/

class DeflateState : public CC_CLASS_MEMDUMP
{
	// Give my name in memory dumps
	// If you need a different sort of decare, replace this one. 
	// See CCObject in the help file for info about DECLARE types
	CC_DECLARE_MEMDUMP(DeflateState);

public:
	DeflateState();
	~DeflateState();

public:
    ZStream *strm;      /* pointer back to this zlib stream */
    INT32   status;        /* as the name implies */
    Bytef *pending_buf;   /* output still pending */
    Bytef *pending_out;   /* next pending byte to output to the stream */
    INT32   pending;       /* nb of bytes in the pending buffer */
 // removed verison 0.99
 //   uLong adler;         /* adler32 of uncompressed data */
    INT32   noheader;      /* suppress zlib header and adler32 */
    Byte  data_type;     /* UNKNOWN, BINARY or ASCII */
    Byte  method;        /* STORED (for zip only) or DEFLATED */
    INT32   last_flush;    /* value of flush param for previous deflate call */

                /* used by deflate.c: */

    uInt  w_size;        /* LZ77 window size (32K by default) */
    uInt  w_bits;        /* log2(w_size)  (8..16) */
    uInt  w_mask;        /* w_size - 1 */

    Byte *window;
    /* Sliding window. Input bytes are read into the second half of the window,
     * and move to the first half later to keep a dictionary of at least wSize
     * bytes. With this organization, matches are limited to a distance of
     * wSize-MAX_MATCH bytes, but this ensures that IO is always
     * performed with a length multiple of the block size. Also, it limits
     * the window size to 64K, which is quite useful on MSDOS.
     * To do: use the user input buffer as sliding window.
     */

    ulg window_size;
    /* Actual size of window: 2*wSize, except when the user input buffer
     * is directly used as sliding window.
     */

    Pos *prev;
    /* Link to older string with same hash index. To limit the size of this
     * array to 64K, this link is maintained only for the last 32K strings.
     * An index in this array is thus a window index modulo 32K.
     */

    Pos *head; /* Heads of the hash chains or NIL. */

    uInt  ins_h;          /* hash index of string to be inserted */
    uInt  hash_size;      /* number of elements in hash table */
    uInt  hash_bits;      /* log2(hash_size) */
    uInt  hash_mask;      /* hash_size-1 */

    uInt  hash_shift;
    /* Number of bits by which ins_h must be shifted at each input
     * step. It must be such that after MIN_MATCH steps, the oldest
     * byte no longer takes part in the hash key, that is:
     *   hash_shift * MIN_MATCH >= hash_bits
     */

    INT32 block_start;
    /* Window position at the beginning of the current output block. Gets
     * negative when the window is moved backwards.
     */

    uInt match_length;           /* length of best match */
    IPos prev_match;             /* previous match */
    INT32 match_available;         /* set if previous match exists */
    uInt strstart;               /* start of string to insert */
    uInt match_start;            /* start of matching string */
    uInt lookahead;              /* number of valid bytes ahead in window */

    uInt prev_length;
    /* Length of the best match at previous step. Matches not greater than this
     * are discarded. This is used in the lazy match evaluation.
     */

    uInt max_chain_length;
    /* To speed up deflation, hash chains are never searched beyond this
     * length.  A higher limit improves compression ratio but degrades the
     * speed.
     */

    uInt max_lazy_match;
    /* Attempt to find a better match only when the current match is strictly
     * smaller than this value. This mechanism is used only for compression
     * levels >= 4.
     */
#define max_insert_length  max_lazy_match
    /* Insert new strings in the hash table only if the match length is not
     * greater than this length. This saves time but degrades compression.
     * max_insert_length is used only for compression levels <= 3.
     */

    INT32 level;    /* compression level (1..9) */
    INT32 strategy; /* favor or force Huffman coding*/

    uInt good_match;
    /* Use a faster search when the previous match is longer than this */

    INT32 nice_match; /* Stop searching when current match exceeds this */



    /* used by trees.c: */

    ct_data dyn_ltree[HEAP_SIZE];   /* literal and length tree */
    ct_data dyn_dtree[2*D_CODES+1]; /* distance tree */
    ct_data bl_tree[2*BL_CODES+1];  /* Huffman tree for the bit lengths */

    tree_desc l_desc;               /* descriptor for literal tree */
    tree_desc d_desc;               /* descriptor for distance tree */
    tree_desc bl_desc;              /* descriptor for bit length tree */

    ush bl_count[MAX_BITS+1];
    /* number of codes at each bit length for an optimal tree */

    INT32 heap[2*L_CODES+1];      /* heap used to build the Huffman trees */
    INT32 heap_len;               /* number of elements in the heap */
    INT32 heap_max;               /* element of largest frequency */
    /* The sons of heap[n] are heap[2*n] and heap[2*n+1]. heap[0] is not used.
     * The same heap array is used to build all trees.
     */

    uch depth[2*L_CODES+1];
    /* Depth of each subtree used as tie breaker for trees of equal frequency
     */

    uch *l_buf;           /* buffer for literals or lengths */

    /* Size of match buffer for literals/lengths.  There are 4 reasons for
     * limiting lit_bufsize to 64K:
     *   - frequencies can be kept in 16 bit counters
     *   - if compression is not successful for the first block, all input
     *     data is still in the window so we can still emit a stored block even
     *     when input comes from standard input.  (This can also be done for
     *     all blocks if lit_bufsize is not greater than 32K.)
     *   - if compression is not successful for a file smaller than 64K, we can
     *     even emit a stored file instead of a stored block (saving 5 bytes).
     *     This is applicable only for zip (not gzip or zlib).
     *   - creating new Huffman trees less frequently may not provide fast
     *     adaptation to changes in the input data statistics. (Take for
     *     example a binary file with poorly compressible code followed by
     *     a highly compressible string table.) Smaller buffer sizes give
     *     fast adaptation but have of course the overhead of transmitting
     *     trees more frequently.
     *   - I can't count above 4
     */
    uInt  lit_bufsize;

    uInt last_lit;      /* running index in l_buf */

    // Buffer for distances. To simplify the code, d_buf and l_buf have
    // the same number of elements. To use different lengths, an extra flag
    // array would be necessary.
    ush *d_buf;

    ulg opt_len;        /* bit length of current block with optimal trees */
    ulg static_len;     /* bit length of current block with static trees */
    ulg compressed_len; /* total bit length of compressed file */
    uInt matches;       /* number of string matches in current block */
// New version 0.99
    INT32 last_eob_len;   /* bit length of EOB code for last block */

#ifdef DEBUG
    ulg bits_sent;      /* bit length of the compressed data */
#endif

    // Output buffer. bits are inserted starting at the bottom (least
    // significant bits).
    ush bi_buf;

    // Number of valid bits in bi_buf.  All bits above the last valid bit
    // are always zero.
    INT32 bi_valid;
};


// New for verison 0.99 

/* Output a byte on the stream.
 * IN assertion: there is enough room in pending_buf.
 */
#define put_byte(s, c) {s->pending_buf[s->pending++] = (c);}


#define MIN_LOOKAHEAD (MAX_MATCH+MIN_MATCH+1)
/* Minimum amount of lookahead, except at the end of the input file.
 * See deflate.c for comments about the MIN_MATCH+1.
 */

#define MAX_DIST(s)  ((s)->w_size-MIN_LOOKAHEAD)
/* In order to simplify the code, particularly on 16 bit machines, match
 * distances are limited to MAX_DIST instead of WSIZE.
 */


/********************************************************************************************

>	class ZipDeflate : public CCObject

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> Humprhys
	Created:	23/5/95
	Purpose:	The deflater for the file compressor and decompressor.
				Allows the CCDiskFile to offer compression of files. At present just
				used to compress the native file format.

********************************************************************************************/

class ZipDeflate : public CC_CLASS_MEMDUMP
{
	// Give my name in memory dumps
	// If you need a different sort of decare, replace this one. 
	// See CCObject in the help file for info about DECLARE types
	CC_DECLARE_MEMDUMP(ZipDeflate);

public:
	ZipDeflate();
	~ZipDeflate();

public:
	INT32 Init(ZStream *strm, INT32 level);

	INT32 deflate(ZStream *strm, INT32 flush);

	INT32 End(ZStream *strm);

	INT32 Init(ZStream *strm, INT32  level, INT32  method, INT32  windowBits,
             INT32  memLevel, INT32  strategy);
			 //const char *version = ZLIB_VERSION, INT32 stream_size = sizeof(ZStream));

	//INT32 Copy(ZStream *dest, ZStream *source);

	INT32 Reset(ZStream *strm);
	
	// New for version 1.1
	INT32 SetDictionary(ZStream *strm, const Bytef *dictionary, uInt dictLength);
	INT32 Params(ZStream *strm, INT32 level, INT32 strategy);

private:
	// these are accessed from configuration_table
	INT32  deflate_stored( DeflateState *s, INT32 flush); // New for version 1.1
	INT32  deflate_fast( DeflateState *s, INT32 flush);
	INT32  deflate_slow( DeflateState *s, INT32 flush);
	
	void fill_window( DeflateState *s);
	void lm_init( DeflateState *s);
	uInt longest_match( DeflateState *s, IPos cur_match);
	void putShortMSB( DeflateState *s, uInt b);
	void flush_pending(ZStream *strm);
	INT32 read_buf(ZStream *strm, charf *buf, unsigned size);
#ifdef ASMV
      void match_init(void); /* asm code initialization */
#endif

#ifdef DEBUG
	void check_match( DeflateState *s, IPos start, IPos match, INT32 length);
	void send_bits(DeflateState *s, INT32 value, INT32 length);
#endif

protected:
    // functions this calls in ztrees.c
	void ct_init( DeflateState *s);
	INT32  ct_tally( DeflateState *s, INT32 dist, INT32 lc);
	ulg ct_flush_block( DeflateState *s, char *buf, ulg stored_len, INT32 eof);
	void ct_stored_block( DeflateState *s, char *buf, ulg stored_len, INT32 eof);


	// New for version 0.99
	void _tr_init(DeflateState *s);
	INT32  _tr_tally(DeflateState *s, unsigned dist, unsigned lc);
	ulg  _tr_flush_block(DeflateState *s, charf *buf, ulg stored_len, INT32 eof);
	void _tr_align(DeflateState *s);
	void _tr_stored_block(DeflateState *s, charf *buf, ulg stored_len, INT32 eof);

private:
	// functions local to ztrees.c
	void tr_static_init(void);
	void init_block( DeflateState *s);
	void pqdownheap( DeflateState *s, ct_data *tree, INT32 k);
	void gen_bitlen( DeflateState *s, tree_desc *desc);
	void gen_codes(ct_data *tree, INT32 max_code, ushf bl_count[]);
	void build_tree( DeflateState *s, tree_desc *desc);
	void scan_tree( DeflateState *s, ct_data *tree, INT32 max_code);
	void send_tree( DeflateState *s, ct_data *tree, INT32 max_code);
	INT32  build_bl_tree( DeflateState *s);
	void send_all_trees( DeflateState *s, INT32 lcodes, INT32 dcodes, INT32 blcodes);
	void compress_block( DeflateState *s, ct_data *ltree, ct_data *dtree);
	void set_data_type( DeflateState *s);
//	void send_bits( DeflateState *s, INT32 value, INT32 length);
	unsigned bi_reverse(unsigned value, INT32 length);
	void bi_windup( DeflateState *s);
	void bi_flush(DeflateState *s);	// new version 0.99
	void copy_block( DeflateState *s, char *buf, unsigned len, INT32 header);

};

#endif  // INC_ZIPDEFLATE


