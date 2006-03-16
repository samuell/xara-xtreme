// $Id: zutil.h 662 2006-03-14 21:31:49Z alex $
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

#ifndef INC_ZIPUTIL
#define INC_ZIPUTIL

// Our mapping for our classes onto the equivalents
//#define z_stream ZStream
//#define gz_stream GZipStream
//#define deflate_state DeflateState

/* 
     The 'zlib' compression library provides in-memory compression and
  decompression functions, including integrity checks of the uncompressed
  data.  This version of the library supports only one compression method
  (deflation) but other algorithms may be added later and will have the same
  stream interface.

     For compression the application must provide the output buffer and
  may optionally provide the input buffer for optimization. For decompression,
  the application must provide the input buffer and may optionally provide
  the output buffer for optimization.

     Compression can be done in a single step if the buffers are large
  enough (for example if an input file is mmap'ed), or can be done by
  repeated calls of the compression function.  In the latter case, the
  application must provide more input and/or consume the output
  (providing more output space) before each call.

     The library does not install any signal handler. It is recommended to
  add at least a handler for SIGSEGV when decompressing; the library checks
  the consistency of the input data whenever possible but may go nuts
  for some forms of corrupted input.
*/

#include <string.h>

// For the PNG stuff, change references to z_stream to ZStream
#define ZStream z_stream 

// Should really be this but version 1.1 Camelot does not seem to like different version numbers
//#define ZLIB_VERSION "1.0"
#define ZLIB_VERSION "0.92"
#define ZLIB_VERSIONNO 0.92

// We will put the real version number in these new variables.
#define ZLIB_MAJOR_VERSIONNO 0
#define ZLIB_MINOR_VERSIONNO 99

//extern const char *z_errmsg[10]; /* indexed by 2-zlib_error */
/* (size given to avoid silly warnings with Visual C++) */

typedef void FAR *voidpf;
typedef void     *voidp;

typedef unsigned char  Byte;  /* 8 bits */
typedef UINT32   uInt;  /* 16 bits or more */
typedef UINT32  uLong; /* 32 bits or more */

typedef Byte  FAR Bytef;
typedef char  FAR charf;
typedef INT32   FAR intf;
typedef uInt  FAR uIntf;
typedef uLong FAR uLongf;

#define ERR_MSG(err) (char*)z_errmsg[Z_NEED_DICT-(err)]

//#define ERR_RETURN(strm,err) return (strm->msg = ERR_MSG(err), (err))
/* To be used only when the state is known to be valid */

// My old form
//#define ERR_RETURN(strm,err) return (strm->msg = GZipFile::z_errmsg[Z_NEED_DICT-(err)], err)
///* To be used only when the state is known to be valid */
#define ERR_RETURN(strm,err) return (err)


//-------------------------------------------------------------------------------------------
// Constants from file zconf.h
//-------------------------------------------------------------------------------------------

/* common constants */

#define DEFLATED   8

#define DEF_WBITS 15
/* default windowBits for decompression. MAX_WBITS is for compression only */

/*
 * Compile with -DMAXSEG_64K if the alloc function cannot allocate more
 * than 64k bytes at a time (needed on systems with 16-bit INT32).
 */
//#  define __32BIT__
//#  define MAXSEG_64K

/* The memory requirements for deflate are (in bytes):
            1 << (windowBits+2)   +  1 << (memLevel+9)
 that is: 128K for windowBits=15  +  128K for memLevel = 8  (default values)
 plus a few kilobytes for small objects. For example, if you want to reduce
 the default memory requirements from 256K to 128K, compile with
     make CFLAGS="-O -DMAX_WBITS=14 -DMAX_MEM_LEVEL=7"
 Of course this will generally degrade compression (there's no free lunch).

   The memory requirements for inflate are (in bytes) 1 << windowBits
 that is, 32K for windowBits=15 (default value) plus a few kilobytes
 for small objects.
*/

/* Maximum value for memLevel in deflateInit2 */
#define MAX_MEM_LEVEL 9

/* Maximum value for windowBits in deflateInit2 and inflateInit2 */
#define MAX_WBITS   15 /* 32K LZ77 window */


//-------------------------------------------------------------------------------------------
// Constants from file zutil.h
//-------------------------------------------------------------------------------------------

#if MAX_MEM_LEVEL >= 8
#  define DEF_MEM_LEVEL 8
#else
#  define DEF_MEM_LEVEL  MAX_MEM_LEVEL
#endif
/* default memLevel */

#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES    2
/* The three kinds of block type */

#define MIN_MATCH  3
#define MAX_MATCH  258
/* The minimum and maximum match lengths */

#define PRESET_DICT 0x20 /* preset dictionary flag in zlib header */

typedef unsigned char  uch;
typedef uch FAR uchf;
typedef unsigned short ush;
typedef ush FAR ushf;
typedef UINT32  ulg;

//-------------------------------------------------------------------------------------------
// Constants from file zlib.h
//-------------------------------------------------------------------------------------------

/* constants */

#define Z_NO_FLUSH      0
#define Z_PARTIAL_FLUSH 1
#define Z_SYNC_FLUSH    2
#define Z_FULL_FLUSH    3
#define Z_FINISH        4
/* Allowed flush values; see deflate() below for details */

#define Z_OK            0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
#define Z_VERSION_ERROR (-6)
// This is our special no compressed section found error
#define Z_UNCOMPRESSED_ERROR (-7)
/* Return codes for the compression/decompression functions. Negative
 * values are errors, positive values are used for special but normal events.
 */

#define Z_NO_COMPRESSION         0
#define Z_BEST_SPEED             1
#define Z_BEST_COMPRESSION       9
#define Z_DEFAULT_COMPRESSION  (-1)
/* compression levels */

#define Z_FILTERED            1
#define Z_HUFFMAN_ONLY        2
#define Z_DEFAULT_STRATEGY    0
/* compression strategy; see deflateInit2() below for details */

#define Z_BINARY   0
#define Z_ASCII    1
#define Z_UNKNOWN  2
/* Possible values of the data_type field */

#define Z_DEFLATED   8
/* The deflate compression method (the only one supported in this version) */

#define Z_NULL  0  /* for initializing zalloc, zfree, opaque */



//-------------------------------------------------------------------------------------------
// Constants from file zutil.h
//-------------------------------------------------------------------------------------------

/* target dependencies */

/* Windows NT */
#define OS_CODE  0x0b
#define zmemcpy memcpy
#define zmemcmp memcmp
#define zmemzero(dest, len) memset(dest, 0, len)

/* Diagnostic functions */

#define _FTPRINTF TRACEUSER( "Neville", _T("%s"),x) 

//#ifdef _DEBUG
//	#define DEBUG 1
//#else
//	#undef DEBUG
//#endif 

#ifdef DEBUG
#  include <stdio.h>
#  ifndef verbose
#    define verbose 0
#  endif
#  define Assert(cond,msg) ERROR3IF(!(cond),msg); //{if(!(cond)) z_error(msg);}
#  define Assert3(msg) ERROR3(msg); //{if(!(cond)) z_error(msg);}
#  define Trace(x) {TRACEUSER( "Neville", _T("%s"),x); }
#  define Tracev(x) {if (verbose) TRACEUSER( "Neville", _T("%s"),x) ;}
#  define Tracevv(x) {if (verbose>1) TRACEUSER( "Neville", _T("%s"),x) ;}
#  define Tracec(c,x) {if (verbose && (c)) TRACEUSER( "Neville", _T("%s"),x) ;}
#  define Tracecv(c,x) {if (verbose>1 && (c)) TRACEUSER( "Neville", _T("%s"),x) ;}
#else
#  define Assert(cond,msg)
#  define Trace(x)
#  define Tracev(x)
#  define Tracevv(x)
#  define Tracec(c,x)
#  define Tracecv(c,x)
#endif

typedef uLong (*check_func)(uLong check, const Bytef *buf, uInt len);

//extern void z_error(char *m);
#define z_error(m) ERROR3(m); 

//#define ZALLOC(strm, items, size) CCMalloc(size)
//#define ZFREE(strm, addr)  {CCFree(addr); addr = NULL; }
//#define TRY_FREE(s, p) {if (p) ZFREE(s, p)}

#define ZALLOC(strm, items, size) \
           (*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE(strm, addr)  (*((strm)->zfree))((strm)->opaque, (void*)(addr))
#define TRY_FREE(s, p) {if (p) ZFREE(s, p);}


#endif  // INC_ZIPUTIL


