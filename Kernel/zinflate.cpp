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

/*
*/

#include "camtypes.h"
#include "fixmem.h"

#include "zinflate.h"
#include "zstream.h"

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_MEMDUMP(InflateState, CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(ZipInflate, CC_CLASS_MEMDUMP)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

uInt inflate_mask[] = {
    0x0000,
    0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff,
    0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff
};

// ------------------------------------------------------------------------------------------
// From inflate.c
// ------------------------------------------------------------------------------------------

/* ---------------------------------- ZipInflate class -------------------------------------- */

/********************************************************************************************

>	ZipInflate::ZipInflate()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Purpose:	ZipInflate constructor.

********************************************************************************************/

ZipInflate::ZipInflate()
{
}	

/********************************************************************************************

>	ZipInflate::ZipInflate()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Purpose:	ZipInflate destructor.

********************************************************************************************/

ZipInflate::~ZipInflate()
{
	
}	

/********************************************************************************************

>	INT32 ZipInflate::Reset(ZStream *z)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		s	the zip strema to use
	Purpose:	This function is equivalent to inflateEnd followed by inflateInit,
				but does not free and reallocate all the internal decompression state.
				The stream will keep attributes that may have been set by inflateInit2.

				inflate Reset returns Z_OK if success, or Z_STREAM_ERROR if the source
				stream state was inconsistent (such as zalloc or state being NULL).

********************************************************************************************/

INT32 ZipInflate::Reset(ZStream *z)
{
	uLong c;

	if (z == NULL || z->In_state == NULL)
		return Z_STREAM_ERROR;

	z->total_in = z->total_out = 0;
	z->msg = Z_NULL;
	z->In_state->mode = z->In_state->nowrap ? BLOCKS : METHOD;
	inflate_blocks_reset(z->In_state->blocks, z, &c);
	Trace((stderr, "inflate: reset\n"));
	return Z_OK;
}


/********************************************************************************************

>	INT32 ZipInflate::End(ZStream *z)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		s	the zip strema to use
	Purpose:	All dynamically allocated data structures for this stream are freed.
				This function discards any unprocessed input and does not flush any
				pending output.

				inflate End returns Z_OK if success, Z_STREAM_ERROR if the stream state
				was inconsistent. In the error case, msg may be set but then points to a
				static string (which must not be deallocated).

********************************************************************************************/

INT32 ZipInflate::End(ZStream *z)
{
	uLong c;

	if (z == NULL || z->In_state == NULL || z->zfree == Z_NULL) // || z->zfree == NULL)
		return Z_STREAM_ERROR;

	if (z->In_state->blocks != NULL)
		inflate_blocks_free(z->In_state->blocks, z, &c);

	delete z->In_state;
	z->In_state = NULL;
	Trace((stderr, "inflate: end\n"));

	return Z_OK;
}


/********************************************************************************************

>	INT32 ZipInflate::Init(ZStream *z, INT32 w)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		s	the zip strema to use
				w	base two logarithm of the maximum window size, in the range 8..15
	Purpose:	This is another version of inflate Init with more compression options. The
				fields next_out, zalloc and zfree must be initialized before by the caller.

				 The windowBits parameter is the base two logarithm of the maximum window
				size (the size of the history buffer).  It should be in the range 8..15 for
				this version of the library (the value 16 will be allowed soon). The
				default value is 15 if inflateInit is used instead. If a compressed stream
				with a larger window size is given as input, inflate() will return with
				the error code Z_DATA_ERROR instead of trying to allocate a larger window.

				 If next_out is not null, the library will use this buffer for the history
				buffer; the buffer must either be large enough to hold the entire output
				data, or have at least 1<<windowBits bytes.  If next_out is null, the
				library will allocate its own buffer (and leave next_out null). next_in
				need not be provided here but must be provided by the application for the
				next call of inflate().

				 If the history buffer is provided by the application, next_out must
				never be changed by the application since the decompressor maintains
				history information inside this buffer from call to call; the application
				can only reset next_out to the beginning of the history buffer when
				avail_out is zero and all output has been consumed.

				  inflateInit2 returns Z_OK if success, Z_MEM_ERROR if there was
				not enough memory, Z_STREAM_ERROR if a parameter is invalid (such as
				windowBits < 8). msg is set to null if there is no error message.
				inflateInit2 does not perform any compression: this will be done by
				inflate().

********************************************************************************************/

INT32 ZipInflate::Init(ZStream *z, INT32 w)
{
	//if (version == Z_NULL || version[0] != ZLIB_VERSION[0] ||
	//	stream_size != sizeof(z_stream))
	//	return Z_VERSION_ERROR;

	/* initialize state */
	if (z == NULL)
		return Z_STREAM_ERROR;

	z->msg = Z_NULL;

	if (z->zalloc == NULL)
	{
		z->zalloc = GZipFile::zcalloc;
	    z->opaque = 0;
	}
	if (z->zfree == NULL)
		z->zfree = GZipFile::zcfree;
//  if ((z->In_state = (struct InflateState *)
//       ZALLOC(z,1,sizeof(struct InflateState))) == NULL)
//    return Z_MEM_ERROR;
	z->In_state = new InflateState;
	if ((z->In_state) == NULL)
		return Z_MEM_ERROR;
	z->In_state->blocks = NULL;

	/* handle undocumented nowrap option (no zlib header or check) */
	z->In_state->nowrap = 0;
	if (w < 0)
	{
		w = - w;
		z->In_state->nowrap = 1;
	}

	/* set window size */
	if (w < 8 || w > 15)
	{
		End(z);
		return Z_STREAM_ERROR;
	}
	z->In_state->wbits = (uInt)w;

	/* create inflate_blocks state */
	z->In_state->blocks = inflate_blocks_new(z, z->In_state->nowrap ? Z_NULL : GZipFile::adler32, (uInt)1 << w);
	if (z->In_state->blocks == NULL)
	{
		End(z);
		return Z_MEM_ERROR;
	}
	Trace((stderr, "inflate: allocated\n"));

	/* reset state */
	Reset(z);

	return Z_OK;
}


/********************************************************************************************

>	INT32 ZipInflate::Init(ZStream *z)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		s	the zip strema to use
	Purpose:	Initializes the internal stream state for decompression. The fields
				zalloc and zfree must be initialized before by the caller.  If zalloc and
				zfree are set to Z_NULL, deflateInit updates them to use default allocation
				functions.

				inflate Init returns Z_OK if success, Z_MEM_ERROR if there was not
				enough memory.  msg is set to null if there is no error message.
				inflateInit does not perform any decompression: this will be done by
				inflate().

********************************************************************************************/

INT32 ZipInflate::Init(ZStream *z)
{
  return Init(z, DEF_WBITS);
}

/********************************************************************************************

>	INT32 ZipInflate::inflate(ZStream *z, INT32 f)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		s	the zip strema to use
				f	dictates the flush type to use
	Purpose:	Performs one or both of the following actions:

				- Decompress more input starting at next_in and update next_in and avail_in
				accordingly. If not all input can be processed (because there is not
				enough room in the output buffer), next_in is updated and processing
				will resume at this point for the next call of inflate().

				 - Provide more output starting at next_out and update next_out and avail_out
				accordingly.  inflate() provides as much output as possible, until there
				is no more input data or no more space in the output buffer (see below
				about the flush parameter).

				Before the call of inflate(), the application should ensure that at least
				one of the actions is possible, by providing more input and/or consuming
				more output, and updating the next_* and avail_* values accordingly.
				The application can consume the uncompressed output when it wants, for
				example when the output buffer is full (avail_out == 0), or after each
				call of inflate(). If inflate returns Z_OK and with zero avail_out, it
				must be called again after making room in the output buffer because there
				might be more output pending.

				If the parameter flush is set to Z_PARTIAL_FLUSH, inflate flushes as much
				output as possible to the output buffer. The flushing behavior of inflate is
				not specified for values of the flush parameter other than Z_PARTIAL_FLUSH
				and Z_FINISH, but the current implementation actually flushes as much output
				as possible anyway.

				inflate() should normally be called until it returns Z_STREAM_END or an
				error. However if all decompression is to be performed in a single step
				(a single call of inflate), the parameter flush should be set to
				Z_FINISH. In this case all pending input is processed and all pending
				output is flushed; avail_out must be large enough to hold all the
				uncompressed data. (The size of the uncompressed data may have been saved
				by the compressor for this purpose.) The next operation on this stream must
				be inflateEnd to deallocate the decompression state.

				inflate() returns Z_OK if some progress has been made (more input
				processed or more output produced), ZStream_END if the end of the
				compressed data has been reached and all uncompressed output has been
				produced, Z_DATA_ERROR if the input data was corrupted, Z_STREAM_ERROR if
				the stream structure was inconsistent (for example if next_in or next_out
				was NULL), Z_MEM_ERROR if there was not enough memory, Z_BUF_ERROR if no
				progress is possible or if there was not enough room in the output buffer
				when Z_FINISH is used. In the Z_DATA_ERROR case, the application may then
				call inflateSync to look for a good compression block.

********************************************************************************************/

#define NEEDBYTE {if(z->avail_in==0)return r;r=Z_OK;}
#define NEXTBYTE (z->avail_in--,z->total_in++,*z->next_in++)

INT32 ZipInflate::inflate(ZStream *z, INT32 f)
{
  INT32 r;
  uInt b;

  if (z == Z_NULL || z->In_state == Z_NULL || z->next_in == Z_NULL || f < 0)
    return Z_STREAM_ERROR;
  r = Z_BUF_ERROR;
  while (1) switch (z->In_state->mode)
  {
    case METHOD:
      NEEDBYTE
      if (((z->In_state->sub.method = NEXTBYTE) & 0xf) != Z_DEFLATED)
      {
        z->In_state->mode = BAD;
        z->msg = (char*)"unknown compression method";
        z->In_state->sub.marker = 5;       /* can't try inflateSync */
        break;
      }
      if ((z->In_state->sub.method >> 4) + 8 > z->In_state->wbits)
      {
        z->In_state->mode = BAD;
        z->msg = (char*)"invalid window size";
        z->In_state->sub.marker = 5;       /* can't try inflateSync */
        break;
      }
      z->In_state->mode = FLAG;
    case FLAG:
      NEEDBYTE
      b = NEXTBYTE;
      if (((z->In_state->sub.method << 8) + b) % 31)
      {
        z->In_state->mode = BAD;
        z->msg = (char*)"incorrect header check";
        z->In_state->sub.marker = 5;       /* can't try inflateSync */
        break;
      }
      Trace((stderr, "inflate: zlib header ok\n"));
      if (!(b & PRESET_DICT))
      {
        z->In_state->mode = BLOCKS;
	break;
      }
      z->In_state->mode = DICT4;
    case DICT4:
      NEEDBYTE
      z->In_state->sub.check.need = (uLong)NEXTBYTE << 24;
      z->In_state->mode = DICT3;
    case DICT3:
      NEEDBYTE
      z->In_state->sub.check.need += (uLong)NEXTBYTE << 16;
      z->In_state->mode = DICT2;
    case DICT2:
      NEEDBYTE
      z->In_state->sub.check.need += (uLong)NEXTBYTE << 8;
      z->In_state->mode = DICT1;
    case DICT1:
      NEEDBYTE
      z->In_state->sub.check.need += (uLong)NEXTBYTE;
      z->adler = z->In_state->sub.check.need;
      z->In_state->mode = DICT0;
      return Z_NEED_DICT;
    case DICT0:
      z->In_state->mode = BAD;
      z->msg = (char*)"need dictionary";
      z->In_state->sub.marker = 0;       /* can try inflateSync */
      return Z_STREAM_ERROR;
    case BLOCKS:
      r = inflate_blocks(z->In_state->blocks, z, r);
      if (r == Z_DATA_ERROR)
      {
        z->In_state->mode = BAD;
        z->In_state->sub.marker = 0;       /* can try inflateSync */
        break;
      }
      if (r != Z_STREAM_END)
        return r;
      r = Z_OK;
      inflate_blocks_reset(z->In_state->blocks, z, &z->In_state->sub.check.was);
      if (z->In_state->nowrap)
      {
        z->In_state->mode = DONE;
        break;
      }
      z->In_state->mode = CHECK4;
    case CHECK4:
      NEEDBYTE
      z->In_state->sub.check.need = (uLong)NEXTBYTE << 24;
      z->In_state->mode = CHECK3;
    case CHECK3:
      NEEDBYTE
      z->In_state->sub.check.need += (uLong)NEXTBYTE << 16;
      z->In_state->mode = CHECK2;
    case CHECK2:
      NEEDBYTE
      z->In_state->sub.check.need += (uLong)NEXTBYTE << 8;
      z->In_state->mode = CHECK1;
    case CHECK1:
      NEEDBYTE
      z->In_state->sub.check.need += (uLong)NEXTBYTE;

      if (z->In_state->sub.check.was != z->In_state->sub.check.need)
      {
        z->In_state->mode = BAD;
        z->msg = (char*)"incorrect data check";
        z->In_state->sub.marker = 5;       /* can't try inflateSync */
        break;
      }
      Trace((stderr, "inflate: zlib check ok\n"));
      z->In_state->mode = DONE;
    case DONE:
      return Z_STREAM_END;
    case BAD:
      return Z_DATA_ERROR;
    default:
      return Z_STREAM_ERROR;
  }
}

/********************************************************************************************

>	INT32 ZipInflate::SetDictionary(ZStream *z, const Bytef *dictionary, uInt dictLength)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		z	the zip stream to use
				dictionary
				dictLength
	Purpose:	Set the same dictionary on loading that was used on saving. For comments see:-
	SeeAlso:	ZipDeflate::SetDictionary

********************************************************************************************/

INT32 ZipInflate::SetDictionary(ZStream *z, const Bytef *dictionary, uInt dictLength)
{
  uInt length = dictLength;

  if (z == Z_NULL || z->In_state == Z_NULL || z->In_state->mode != DICT0)
    return Z_STREAM_ERROR;
  if (GZipFile::adler32(1L, dictionary, dictLength) != z->adler) return Z_DATA_ERROR;
  z->adler = 1L;

  if (length >= ((uInt)1<<z->In_state->wbits))
  {
    length = (1<<z->In_state->wbits)-1;
    dictionary += dictLength - length;
  }
  inflate_set_dictionary(z->In_state->blocks, dictionary, length);
  z->In_state->mode = BLOCKS;
  return Z_OK;
}


/********************************************************************************************

>	INT32 ZipInflate::Sync(ZStream *z)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		s	the zip stream to use
	Purpose:	Skips invalid compressed data until the special marker (see deflate()
				above) can be found, or until all available input is skipped. No output
				is provided.

				inflate Sync returns Z_OK if the special marker has been found, Z_BUF_ERROR
				if no more input was provided, Z_DATA_ERROR if no marker has been found,
				or Z_STREAM_ERROR if the stream structure was inconsistent. In the success
				case, the application may save the current current value of total_in which
				indicates where valid compressed data was found. In the error case, the
				application may repeatedly call inflateSync, providing more input each time,
				until success or end of the input data.

********************************************************************************************/

INT32 ZipInflate::Sync(ZStream *z)
{
  uInt n;       /* number of bytes to look at */
  Bytef *p;     /* pointer to bytes */
  uInt m;       /* number of marker bytes found in a row */
  uLong r, w;   /* temporaries to save total_in and total_out */

  /* set up */
  if (z == Z_NULL || z->In_state == Z_NULL)
    return Z_STREAM_ERROR;
  if (z->In_state->mode != BAD)
  {
    z->In_state->mode = BAD;
    z->In_state->sub.marker = 0;
  }
  if ((n = z->avail_in) == 0)
    return Z_BUF_ERROR;
  p = z->next_in;
  m = z->In_state->sub.marker;

  /* search */
  while (n && m < 4)
  {
    if (*p == (Byte)(m < 2 ? 0 : 0xff))
      m++;
    else if (*p)
      m = 0;
    else
      m = 4 - m;
    p++, n--;
  }

  /* restore */
  z->total_in += p - z->next_in;
  z->next_in = p;
  z->avail_in = n;
  z->In_state->sub.marker = m;

  /* return no joy or set up to restart on a new block */
  if (m != 4)
    return Z_DATA_ERROR;
  r = z->total_in;  w = z->total_out;
  Reset(z);
  z->total_in = r;  z->total_out = w;
  z->In_state->mode = BLOCKS;
  return Z_OK;
}

// ------------------------------------------------------------------------------------------
// From infutil.c
// ------------------------------------------------------------------------------------------

/********************************************************************************************

>	INT32 ZipInflate::inflate_flush(ZStream *z)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		s	current inflate state
				z	zip stream we are to use
				r
	Purpose:	copy as much as possible from the sliding window to the output area

********************************************************************************************/

INT32 ZipInflate::inflate_flush(inflate_blocks_state *s, ZStream *z, INT32 r)
{
  uInt n;
  Bytef *p, *q;

  /* local copies of source and destination pointers */
  p = z->next_out;
  q = s->read;

  /* compute number of bytes to copy as far as end of window */
  n = (uInt)((q <= s->write ? s->write : s->end) - q);
  if (n > z->avail_out) n = z->avail_out;
  if (n && r == Z_BUF_ERROR) r = Z_OK;

  /* update counters */
  z->avail_out -= n;
  z->total_out += n;

  /* update check information */
  if (s->checkfn != Z_NULL)
    z->adler = s->check = (*s->checkfn)(s->check, q, n);

  /* copy as far as end of window */
  zmemcpy(p, q, n);
  p += n;
  q += n;

  /* see if more to copy at beginning of window */
  if (q == s->end)
  {
    /* wrap pointers */
    q = s->window;
    if (s->write == s->end)
      s->write = s->window;

    /* compute bytes to copy */
    n = (uInt)(s->write - q);
    if (n > z->avail_out) n = z->avail_out;
    if (n && r == Z_BUF_ERROR) r = Z_OK;

    /* update counters */
    z->avail_out -= n;
    z->total_out += n;

    /* update check information */
    if (s->checkfn != Z_NULL)
      z->adler = s->check = (*s->checkfn)(s->check, q, n);

    /* copy */
    zmemcpy(p, q, n);
    p += n;
    q += n;
  }

  /* update pointers */
  z->next_out = p;
  s->read = q;

  /* done */
  return r;
}
