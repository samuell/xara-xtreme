// $Id$
/* @@tag:xara-cn-tp@@ THIRD PARTY COPYRIGHT */
// This class contains all the streaming file zipping code

#ifndef INC_ZSTREAM
#define INC_ZSTREAM

#include <fstream>
#include "ccfile.h"		// FilePos

#include "zutil.h"
#include "zdeflate.h"
#include "zinflate.h"

// This is what the original code is based upon. Converted to C++.
 
/* interface of the 'zlib' general purpose compression library

  version 0.92 May 3rd, 1995.

  Copyright (C) 1995 Jean-loup Gailly and Mark Adler

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Jean-loup Gailly        Mark Adler
  gzip@prep.ai.mit.edu    madler@cco.caltech.edu
 */

// Some useful bits and pieces

// These are from zconf.h

/*
  The library does not install any signal handler. It is recommended to
  add at least a handler for SIGSEGV when decompressing; the library checks
  the consistency of the input data whenever possible but may go nuts
  for some forms of corrupted input.
 */

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
*/

/*
   The application must update next_in and avail_in when avail_in has
   dropped to zero. It must update next_out and avail_out when avail_out
   has dropped to zero. The application must initialize zalloc, zfree and
   opaque before calling the init function. All other fields are set by the
   compression library and must not be updated by the application.

   The opaque value provided by the application will be passed as the first
   parameter for calls of zalloc and zfree. This can be useful for custom
   memory management. The compression library attaches no meaning to the
   opaque value.

   zalloc must return NULL if there is not enough memory for the object.
   On 16-bit systems, the functions zalloc and zfree must be able to allocate
   exactly 65536 bytes, but will not be required to allocate more than this
   if the symbol MAXSEG_64K is defined (see zconf.h). WARNING: On MSDOS,
   pointers returned by zalloc for objects of exactly 65536 bytes *must*
   have their offset normalized to zero. The default allocation function
   provided by this library ensures this (see zutil.c). To reduce memory
   requirements and avoid any allocation of 64K objects, at the expense of
   compression ratio, compile the library with -DMAX_WBITS=14 (see zconf.h).

   The fields total_in and total_out can be used for statistics or
   progress reports. After compression, total_in holds the total size of
   the uncompressed data and may be saved for use in the decompressor
   (particularly if the decompressor wants to decompress everything in
   a single step).
*/

typedef void* (*alloc_func)(void *opaque, uInt items, uInt size);
typedef void  (*free_func)(void *opaque, void* address);

//class InflateState;
//class DeflateState;

typedef ZStream FAR *z_streamp;



/********************************************************************************************

>	class ZStream : public CCObject

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> Humprhys
	Created:	23/5/95
	Purpose:	A file stream compressor and decompressor.
				Allows the CCDiskFile to offer compression of files. At present just
				used to compress the native file format.

********************************************************************************************/

class ZStream : public CC_CLASS_MEMDUMP
{
	// Give my name in memory dumps
	// If you need a different sort of decare, replace this one. 
	// See CCObject in the help file for info about DECLARE types
	CC_DECLARE_MEMDUMP(ZStream);

public:
	ZStream();
	~ZStream();

	//Init();

public:
	// The application can compare zlib_version and ZLIB_VERSION for consistency.
   	// If the first character differs, the library code actually used is
   	// not compatible with the zlib.h header file used by the application.
	char *zlib_version;

	// ZStream object
    Byte     *next_in;  	/* next input byte */
    uInt     avail_in;  	/* number of bytes available at next_in */
    uLong    total_in;  	/* total nb of input bytes read so far */

    Byte     *next_out; 	/* next output byte should be put there */
    uInt     avail_out; 	/* remaining free space at next_out */
    uLong    total_out; 	/* total nb of bytes output so far */

    char     *msg;      	/* last error message, NULL if no error */
    InflateState *In_state;	/* not visible by applications */
    DeflateState *De_state;	/* not visible by applications */

    alloc_func zalloc;  	/* used to allocate the internal state */
    free_func  zfree;   	/* used to free the internal state */
    void     *opaque;  		/* private data object passed to zalloc and zfree */

    Byte     data_type; 	/* best guess about the data type: ascii or binary */
    uLong   adler;      /* adler32 value of the uncompressed data */
    uLong   reserved;   /* reserved for future use */
protected:

};



/********************************************************************************************

>	class GZipStream : public CCObject

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> Humprhys
	Created:	23/5/95
	Purpose:	A file stream compressor and decompressor.
				Allows the CCDiskFile to offer compression of files. At present just
				used to compress the native file format.

Note:	z_stream == to our ZStream
		gz_stream == to our GZipStream

********************************************************************************************/

class GZipStream : public CC_CLASS_MEMDUMP
{
	// Give my name in memory dumps
	// If you need a different sort of decare, replace this one. 
	// See CCObject in the help file for info about DECLARE types
	CC_DECLARE_MEMDUMP(GZipStream);

public:
	GZipStream();
	~GZipStream();

	//Init();

public:
    ZStream		stream;		// The stream that we will be using
    INT32      	z_err;   	// error code for last stream operation
    INT32      	z_eof;   	// set if end of input file
	iostream 	*file;    	// Underlying implementation is an fstream
    Byte     	*inbuf;  	// input buffer
    Byte     	*outbuf; 	// output buffer
    uLong    	crc;     	// crc32 of uncompressed data
    char     	*msg;    	// error message
    TCHAR     	*path;   	// path name for debugging only
    INT32      	transparent; // 1 if input file is not a .gz file
    char     	mode;    	// 'w' or 'r' 

	// Peek variables
	BOOL 		Peek;		// flag to say if we have peeked at all
	char 		PeekedValue;// if peek is true then use this as the next value		
	INT32			PeekStatus;	// the value returned from the peek

	FilePos		InitialPos;	// position in the file that we start at
	
	BOOL Inited;			// Flag to say whether Init has been called.

protected:
};



//class ZipDeflate;
//class ZipInflate;


/********************************************************************************************

>	class GZipFile : public CCObject

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> Humprhys
	Created:	23/5/95
	Purpose:	A file stream compressor and decompressor.
				Allows the CCDiskFile to offer compression of files. At present just
				used to compress the native file format.

********************************************************************************************/

class GZipFile : public CC_CLASS_MEMDUMP
{
	// Give my name in memory dumps
	// If you need a different sort of decare, replace this one. 
	// See CCObject in the help file for info about DECLARE types
	CC_DECLARE_MEMDUMP(GZipFile);

public:
	GZipFile();
	~GZipFile();

	//Init();

protected:
	// The classes we use for the compression/decompression 
	ZipDeflate *deflate;
	ZipInflate *inflate;

public:
	// Function to find out the current file version of the stream 
	static double GetStreamVersionNo();

	// Useful function to try and claim all the memory for the compression system
	GZipStream *gz_init(iostream *pFileStream, TCHAR *mode, BOOL Header = FALSE);

	// Useful stream functions for opening/closing,reading and writing
	GZipStream *gz_open(iostream *pFileStream, TCHAR *mode, TCHAR *path);

	// The form of open if we have already called init
	INT32 gz_open(GZipStream *s);

	INT32 gzpeek(GZipStream *s, char *data);

	INT32 gzread(GZipStream *s, char *data, unsigned len);

	INT32 gzwrite(GZipStream *s, const char *data, unsigned len);

	INT32 gzflush(GZipStream *s, INT32 flush);

	INT32 gzclose(GZipStream *s);

	//char* gzerror(GZipStream *s, INT32 *errnum);

	INT32 destroy(GZipStream *file);


	// Checksum functions

	static uLong crc32(uLong crc, Bytef *buf, uInt len);
	static uLong adler32(uLong adler, const Bytef *buf, uInt len);
	static uLongf* get_crc_table();

	// These are wrappers around the memory allocation functions
	static void* zcalloc (void *opaque, unsigned items, unsigned size);
	static void  zcfree(void *opaque, void *ptr);

protected:
	void   putLong(iostream* file, uLong x);
	uLong  getLong(Byte *buf);

public:
	FilePos GetCurrentFilePos(GZipStream *s);
};

#endif  // INC_ZSTREAM
