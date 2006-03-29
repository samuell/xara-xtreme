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
//
// A file which encapsulates the zipped file streaming routines and data.
// Used to compress/uncompress the native files.

/*
*/

#include "camtypes.h"
#include "fixmem.h"
#include "errors.h"

#include "zstream.h"
#include "zdeflate.h"
#include "zinflate.h"

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision: 662 $");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_MEMDUMP(ZStream, CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(GZipStream, CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(GZipFile, CC_CLASS_MEMDUMP)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

// Some useful zip bits
#if RALPH
	#define Z_BUFSIZE 32 // was 4096
#else
	#define Z_BUFSIZE 32*1024 // was 4096
#endif

//#define ALLOC(size) zcalloc((voidp)0, 1, size)
//#define TRYFREE(p) {if (p) zcfree((voidp)0, p);}
#define ALLOC(size) CCMalloc(size)
#define TRYFREE(p) {if (p) { CCFree(p); p = NULL; } }

//#define GZ_MAGIC_1 0x1f
//#define GZ_MAGIC_2 0x8b

//static INT32 gz_magic[2] = {0x1f, 0x8b}; /* gzip magic header */

/* gzip flag byte */
#define ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
#define HEAD_CRC     0x02 /* bit 1 set: header CRC present */
#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define COMMENT      0x10 /* bit 4 set: file comment present */
#define RESERVED     0xE0 /* bits 5..7: reserved */

#ifndef SEEK_CUR
#  define SEEK_CUR 1
#endif

/* ---------------------------------- ZStream class -------------------------------------- */

/********************************************************************************************

>	ZStream::ZStream()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	34/05/95
	Purpose:	ZStream constructor.

********************************************************************************************/

ZStream::ZStream()
{
	next_in = NULL;
	avail_in = 0;
	total_in = 0;	

    next_out = NULL;
	avail_out = 0;
	total_out = 0;	

    msg = NULL;

   	In_state = NULL;
    De_state = NULL;

    zalloc = (alloc_func)0;
    zfree = (free_func)0;

	data_type = Z_BINARY;
	adler = 0;
	reserved = 0;

	zlib_version = ZLIB_VERSION;
}	

/********************************************************************************************

>	ZStream::ZStream()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	34/05/95
	Purpose:	ZStream destructor.

********************************************************************************************/

ZStream::~ZStream()
{
	if (In_state)
	{
		delete In_state;
		In_state = NULL;		
	}
#ifdef DO_EXPORT
	if (De_state)
	{
		delete De_state;
		De_state = NULL;		
	}
#endif
}	


/* ------------------------------- GZipStream class -------------------------------------- */

/********************************************************************************************

>	GZipStream::GZipStream()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	34/05/95
	Purpose:	GZipStream constructor.

********************************************************************************************/

GZipStream::GZipStream()
{
    z_err = Z_OK;
    z_eof = 0;

    file = NULL;
    inbuf = NULL;
    outbuf = NULL;

    crc = 0;
    crc = GZipFile::crc32(0L, NULL, 0);
    msg = NULL;
	path = NULL;
    transparent = 0;

	Peek = FALSE;		// no value peeked
	PeekedValue = '#';	// random value
	PeekStatus = Z_OK;	// status ok

	InitialPos = 0;

	Inited = FALSE;
}	

/********************************************************************************************

>	GZipStream::~GZipStream()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	34/05/95
	Purpose:	GZipStream destructor.

********************************************************************************************/

GZipStream::~GZipStream()
{
	// Try and free up the buffers that we have allocated 
    TRYFREE(inbuf);
    TRYFREE(outbuf);
    TRYFREE(path);
    TRYFREE(msg);
}	


/* --------------------------------- GZipFile class -------------------------------------- */

/********************************************************************************************

>	GZipFile::GZipFile()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	34/05/95
	Purpose:	GZipFile constructor.

********************************************************************************************/

GZipFile::GZipFile()
{
	deflate = NULL;   
	inflate = NULL;   
}	

/********************************************************************************************

>	GZipFile::~GZipFile()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	34/05/95
	Purpose:	GZipFile destructor.

********************************************************************************************/

GZipFile::~GZipFile()
{
#ifdef DO_EXPORT
	// Remove the deflate class, if present
	if (deflate != NULL)
	{
		delete deflate;
		deflate = NULL;   
	}
#endif

	// Remove the inflate class, if present
	if (inflate != NULL)
	{
		delete inflate;
		inflate = NULL;   
	}
}	

/********************************************************************************************

>	static double GZipFile::GetStreamVersionNo()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/05/95
	Returns:	the current version number of the stream in the form 0.92
	Purpose:	To find out the current version number of the stream and hence files which
				it can recoknise. The verison is of the form 0.92.

********************************************************************************************/

double GZipFile::GetStreamVersionNo()
{
	return ZLIB_VERSIONNO;
}

/********************************************************************************************

>	INT32 GZipFile::destroy(GZipStream *s)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Purpose:	Cleanup then free the given GZipStream. Return a zlib error code.
				Assume caller will close the file.

********************************************************************************************/

INT32 GZipFile::destroy(GZipStream *s)
{
	TRACEUSER( "Neville", _T("GZipFile::gzflush\n"));

	// Check that a valid stream has been passed in
    if (!s) return Z_STREAM_ERROR;

    INT32 err = Z_OK;

	// Try and free up the buffers that we have allocated 
    TRYFREE(s->inbuf);
    TRYFREE(s->outbuf);
    TRYFREE(s->path);
    TRYFREE(s->msg);

	if (s->mode == 'w')
	{
#ifdef DO_EXPORT
   		if (s->stream.De_state != NULL && deflate != NULL)
			err = deflate->End(&(s->stream));

		// Now remove the deflate class, if present
		if (deflate != NULL)
		{
			delete deflate;
			deflate = NULL;   
		}
#endif
	}
	else if (s->mode == 'r')
	{
   		if (s->stream.In_state != NULL && inflate != NULL)
		   err = inflate->End(&(s->stream));
		
		// Now remove the inflate class, if present
		if (inflate != NULL)
		{
			delete inflate;
			inflate = NULL;   
		}
	}

	// If we had a file claimed then try and close it
	// Assume this is done by the caller as in the native file save we still have some
	// bits to write out to the file.
	// Assume pointer is of no use anymore so set to NULL.
	s->file = NULL;

    if (s->z_err < 0)
    	err = s->z_err;

	// free up our ZipStream object
    delete s;
	s = NULL;

    return err;
}

//static const Bytef dictionary[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//static const uInt dictionary_size = 8;
	
/********************************************************************************************

>	GZipStream *GZipFile::gz_init(iostream *pFileStream, TCHAR *mode, BOOL Header = FALSE)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/06/95
	Inputs:		pFileStream		the iostream class to use for reading/writing
				mode			rw mode for this file
				 	
	Purpose:	Initialises the compression system and tries to grab the memory required.
				gz_init returns NULL if there was insufficient memory to allocate the
				(de)compression state; errno can be checked to distinguish the two cases
				(if errno is zero, the zlib error is Z_MEM_ERROR).
				Graeme ( 15/11/99) : Added the Header parameter to the parameter list.
				This enables the GZipStream to be initialised so that it writes the ZLib
				header, which was previously supressed.

********************************************************************************************/

GZipStream *GZipFile::gz_init(iostream *pFileStream, TCHAR *mode, BOOL Header)
{
	TRACEUSER( "Neville", _T("GZipFile::gz_init\n"));

	if (pFileStream == NULL)
	{
		ERROR3("GZipFile::gz_open Expects file class to be supplied and open as of yet!");
    	return NULL;
	}

    GZipStream *s = new GZipStream;
    if (!s) return NULL;

    s->stream.zalloc = (alloc_func)0;
    s->stream.zfree = (free_func)0;
    s->stream.next_in = NULL;
    s->inbuf = NULL;
    s->stream.next_out = NULL;
    s->outbuf = NULL;
    s->stream.avail_in = 0;
    s->stream.avail_out = 0;
    s->file = pFileStream;		// will be NULL if wanting it to be opened
    s->z_err = Z_OK;
    s->z_eof = 0;
    s->crc = crc32(0L, NULL, 0);
    s->msg = NULL;
    s->transparent = 0;
	s->Peek = FALSE;		// no value peeked
	s->PeekedValue = '#';	// random value
	s->PeekStatus = 0;		// status 0 bytes present

	// Work our whether we are reading or writing from the supplied mode
    TCHAR			   *p = mode;
    s->mode = '\0';
    do
    {
        if( *p == _T('r') ) 
			s->mode = _T('r');
        if( *p == _T('w') )
			s->mode = _T('w');
    } while (*p++);
    
    if( s->mode == _T('\0') )
    {
    	destroy(s);
    	return NULL;
    }
    
    INT32 err;
    if( s->mode == _T('w') )
    {
#ifdef DO_EXPORT
		// First, create the deflate class ready for use
		deflate = new ZipDeflate;
		if (deflate == NULL)
	    {
	    	destroy(s);
	    	return NULL;
	    }

		// Now initialise it, ready for use.
		// Graeme ( 15/11/99 ) - Added the switch on Header to allow for the
		// zlib header.
		if ( Header )
		{
			// Initialise using MAX_WBITS, so that the ZLib header is created.
			err = deflate->Init(&(s->stream), Z_DEFAULT_COMPRESSION,
				               DEFLATED, MAX_WBITS, DEF_MEM_LEVEL, 0);
		}
		else
        {
			// windowBits is passed < 0 to suppress zlib header.
			err = deflate->Init(&(s->stream), Z_DEFAULT_COMPRESSION,
				               DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, 0);
		}

        s->stream.next_out = (Byte*) ALLOC(Z_BUFSIZE);
        s->outbuf = s->stream.next_out; 
	    s->stream.avail_out = Z_BUFSIZE;

        if (err != Z_OK || s->outbuf == NULL)
	    {
	    	destroy(s);
	    	return NULL;
	    }

		/* err = deflate->SetDictionary(&(s->stream), dictionary, dictionary_size);
        if (err != Z_OK)
	    {
	    	destroy(s);
	    	return NULL;
	    } */
#else
		return NULL;
#endif
    }
    else
    {
		// First, create the inflate class ready for use
		inflate = new ZipInflate;
		if (inflate == NULL)
	    {
	    	destroy(s);
	    	return NULL;
	    }

        err = inflate->Init(&(s->stream), -MAX_WBITS);
        s->inbuf = (Byte*) ALLOC(Z_BUFSIZE);
        s->stream.next_in  = s->inbuf;

        if (err != Z_OK || s->inbuf == NULL)
	    {
	    	destroy(s);
	    	return NULL;
	    }

		/* err = inflate->SetDictionary(&(s->stream), dictionary, dictionary_size);
        if (err != Z_OK)
	    {
	    	destroy(s);
	    	return NULL;
	    } */
    }

	s->Inited = TRUE;

	return s;
}

/********************************************************************************************

>	INT32 GZipFile::gz_open(GZipStream *s)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/06/95
	Inputs:		s		the zip stream that we should be using
				 	
	Purpose:	The form of open if we have already called init.
				At present, this is fairly simple and just checks out a few things

********************************************************************************************/

INT32 GZipFile::gz_open(GZipStream *s)
{
	TRACEUSER( "Neville", _T("GZipFile::gz_open init already called\n"));

	// check that everyhting is valid before preceeding
    if (s == NULL) return Z_STREAM_ERROR;

	// Check if init has been called
	if (!s->Inited)
		return Z_STREAM_ERROR;
 
	// Note the current position in the file for later use
	s->InitialPos = s->file->tellg();
	
	// check if there is an actual file stream class attached
    if (s->file == NULL)
    {
    	destroy(s);
    	return 0;
    }

	// Check if reading and already at end of file
    if (s->mode == 'r' && s->file->eof())
    {
        s->z_err = Z_DATA_ERROR;
		return Z_DATA_ERROR;
    }

	// Check if reading and the next bytes are not compressed
    if (s->mode == 'r')
    {
        s->stream.avail_in = 15;
        s->file->read( (char *)s->inbuf, s->stream.avail_in );
		// Seek back to where we were before we started this test
		s->file->seekp(s->InitialPos);
        if (s->stream.avail_in == 15)
        {
			// If we find the following words then the data is not compressed  
			// We must fail but cleanly as all we need to do is read it uncomrpessed
			if( strncmp( (PCSTR)s->inbuf, "%%AWColourTable", 15 ) == 0 )
			{
        		s->stream.avail_in = 0;
	    	    s->z_err = Z_UNCOMPRESSED_ERROR;
				return Z_UNCOMPRESSED_ERROR;
			}
       		s->stream.avail_in = 0;
        }
		else
		{
    	    s->z_err = Z_DATA_ERROR;
			return Z_DATA_ERROR;
        }
    }

	// Everything went ok
	return Z_OK;
}

/********************************************************************************************

>	GZipStream *GZipFile::gz_open(iostream* pFileStream, TCHAR *mode, TCHAR *path)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		pFileStream		the iostream class to use for reading/writing
				mode			rw mode for this file
				path			pathname for this file (optional)
				 	
	Purpose:	Opens a gzip file for reading or writing. The mode parameter is as in fopen
				("rb" or "wb").
				The stream is given at present by the iostream class, the patname is
				optional and is just used for debugging.

				gz_open return NULL if the stream could not be opened or if there was
				insufficient memory to allocate the (de)compression state; errno
				can be checked to distinguish the two cases (if errno is zero, the
				zlib error is Z_MEM_ERROR).

********************************************************************************************/

GZipStream *GZipFile::gz_open(iostream* pFileStream, TCHAR *mode, TCHAR *path )
{
	TRACEUSER( "Neville", _T("GZipFile::gz_open\n"));

	if (pFileStream == NULL)
	{
		ERROR3("GZipFile::gz_open Expects file class to be supplied and open as of yet!");
    	return NULL;
	}

    INT32 err;
    TCHAR			   *p = mode;

    GZipStream *s = new GZipStream;
    if (!s) return NULL;

    s->stream.zalloc = (alloc_func) 0;
    s->stream.zfree = (free_func) 0;
    s->stream.next_in = NULL;
    s->inbuf = NULL;
    s->stream.next_out = NULL;
    s->outbuf = NULL;
    s->stream.avail_in = 0;
    s->stream.avail_out = 0;
    s->file = pFileStream;		// will be NULL if wanting it to be opened
    s->z_err = Z_OK;
    s->z_eof = 0;
    s->crc = crc32(0L, NULL, 0);
    s->msg = NULL;
    s->transparent = 0;

	// Our extra bits
	s->Peek = FALSE;		// no value peeked
	s->PeekedValue = '#';	// random value
	s->PeekStatus = 0;		// status 0 bytes present
	s->path = NULL;

	if (path != NULL)
	{
		s->path = (TCHAR*) ALLOC(cc_strlenBytes(path) + 1);
		if (s->path == NULL)
		{
    		destroy(s);
    		return NULL;
		}

		_tcscpy(s->path, path); /* do this early for debugging */
	}

	// Note the current position in the file for later use
	s->InitialPos = s->file->tellg();

	// Work our whether we are reading or writing from the supplied mode
    s->mode = '\0';
    do
    {
        if (*p == 'r') s->mode = 'r';
        if (*p == 'w') s->mode = 'w';
    } while (*p++);
    
    if (s->mode == '\0')
    {
    	destroy(s);
    	return NULL;
    }
    
    if (s->mode == 'w')
    {
#ifdef DO_EXPORT
		// First, create the deflate class ready for use
		deflate = new ZipDeflate;
		if (deflate == NULL)
	    {
	    	destroy(s);
	    	return NULL;
	    }

		// Now initialise it, ready for use
        err = deflate->Init(&(s->stream), Z_DEFAULT_COMPRESSION,
		                   DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, 0);
        /* windowBits is passed < 0 to suppress zlib header */
 
        s->stream.next_out = (Byte*) ALLOC(Z_BUFSIZE);
        s->outbuf = s->stream.next_out; 

        if (err != Z_OK || s->outbuf == NULL)
	    {
	    	destroy(s);
	    	return NULL;
	    }

		s->stream.avail_out = Z_BUFSIZE;

		/* err = deflate->SetDictionary(&(s->stream), dictionary, dictionary_size);
        if (err != Z_OK)
	    {
	    	destroy(s);
	    	return NULL;
	    } */
#else
		return NULL;
#endif
    }
    else
    {
		// First, create the inflate class ready for use
		inflate = new ZipInflate;
		if (inflate == NULL)
	    {
	    	destroy(s);
	    	return NULL;
	    }

        err = inflate->Init(&(s->stream), -MAX_WBITS);
        s->inbuf = (Byte*) ALLOC(Z_BUFSIZE);
        s->stream.next_in  = s->inbuf;

        if (err != Z_OK || s->inbuf == NULL)
	    {
	    	destroy(s);
	    	return NULL;
	    }

		/*err = inflate->SetDictionary(&(s->stream), dictionary, dictionary_size);
        if (err != Z_OK)
	    {
	    	destroy(s);
	    	return NULL;
	    } */
    }
	// Moved from inside write clause by 0.99 upgrade
	//s->stream.avail_out = Z_BUFSIZE;

    if (s->file == NULL)
    {
    	destroy(s);
    	return NULL;
    }
    
	// All simple header writing code is done by the native file class
    // All header/compression verison checking is done by the native file class
    if (s->mode == 'r')
    {
		// This is all done in the native file filter so need to do it here
		// reset buffer back to zero ready for the true data itself
        s->stream.avail_in = 0;

        if (s->file->eof())
        {
            s->z_err = Z_DATA_ERROR;
        }

		// Check if reading and the next bytes are not compressed
        s->stream.avail_in = 15;
        s->file->read( (char *)s->inbuf, s->stream.avail_in );
		// Seek back to where we were before we started this test
		s->file->seekg(s->InitialPos);
        if (s->stream.avail_in == 15)
        {
			// If we find the following words then the data is not compressed  
			// We must fail but cleanly as all we need to do is read it uncomrpessed
			if( strncmp( (char *)s->inbuf, "%%AWColourTable", 15 ) == 0 )
			{
        		s->stream.avail_in = 0;
	    	    s->z_err = Z_UNCOMPRESSED_ERROR;
				return s;
			}
       		s->stream.avail_in = 0;
        }
		else
		{
    	    s->z_err = Z_DATA_ERROR;
			return s;
        }
    }

	s->Inited = TRUE;

    return s;
}

/********************************************************************************************

>	INT32    GZipFile::gzpeek(GZipStream *file, TCHAR data)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		s		the zip stream that we should be using
				buf		the character that we are to return
	Returns:	number of uncompressed bytes actually read (0 for end of file, -1 for error)
	Purpose:	Reads the next byte from the input stream and returns this to the caller.
				It then remembers this value for the next read.
				gzread returns the number of uncompressed bytes actually read (0 for
				end of file, -1 for error).

********************************************************************************************/

INT32    GZipFile::gzpeek( GZipStream *s, char *buf )
{
	// check that everyhting is valid before preceeding
    if (s == NULL || s->mode != 'r' || inflate == NULL || buf == NULL) return Z_STREAM_ERROR;

    // First check for problem states from last reading/peeking
    if (s->z_err == Z_DATA_ERROR) return -1; /* bad .gz file */
    if (s->z_err == Z_STREAM_END) return 0;  /* don't read crc as data */

	// first check if we are in the peeked state already
	if (s->PeekStatus > 0)
	{
		// just return the peeked character
		// add in the values from the peeks
		ERROR3IF(s->PeekStatus > 1,"peek with status > 1");
		buf[0] = s->PeekedValue;	// set first byte to be the peeked value

		// return amount read to caller 
		return 1;	
	}

    Byte *b = (Byte*)buf;
    s->stream.next_out = b;

	// remember the current uncompressed buffer size as this will give us where
	unsigned offset = s->stream.avail_out;
	if(offset !=0)
		ERROR3IF(offset != 0,"peek offset not zero");
    s->stream.avail_out += 1;	// we want 1 character

    while (s->stream.avail_out != 0)
    {
        if (s->stream.avail_in == 0 && !s->z_eof)
        {
			s->stream.avail_in = Z_BUFSIZE;
			// Note the current file position
			FilePos Pos = s->file->tellg();
			s->file->read((char *) s->inbuf, s->stream.avail_in );
			FilePos PosAfter = s->file->tellg();
			s->stream.avail_in = PosAfter- Pos;
TRACEUSER( "Neville", _T("GZipFile::gz_peek filled buffer size %d\n"),s->stream.avail_in);
            if (s->stream.avail_in == 0)
            {
                s->z_eof = 1;
			//	s->file->setf(s->file->eofbit);

			}
          /*  else if (s->stream.avail_in == (uInt)EOF)
            {
            //    s->stream.avail_in = 0;
                s->z_eof = 1;
            //    s->z_err = Z_ERRNO;
            //    break;
            }*/
            else if (s->stream.avail_in < Z_BUFSIZE || s->file->eof())
            {
				// We are at the end of the file but have read some data in
				// We must set the goodbit to clear the eof and fail bits as otherwise
				// all the checks in the diskfile will then fail.
				s->z_eof = 1;
				s->file->clear(s->file->goodbit);
            }
            s->stream.next_in = s->inbuf;
        }
        s->z_err = inflate->inflate(&(s->stream), Z_NO_FLUSH);

	    if (s->z_err == Z_STREAM_END ||
            s->z_err != Z_OK  || s->z_eof) break;
    }

	// Update len with the bytes left in the uncompressed stream, should be 0 if everything ok
    unsigned len = 1 - s->stream.avail_out;

	s->PeekedValue = *buf;		// remember the value that was read in		
	s->PeekStatus += 1;			// increment peek count
    s->stream.avail_out += 1;	// say that we have peeked 1 character

	// return the actual amount read in back to the caller
    return len;
}	

/********************************************************************************************

>	INT32 GZipFile::gzread(GZipStream *s, Byte *buf, unsigned len)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		s		the zip stream that we should be using
				buf		the buffer that needs the data reading into
	Returns:	number of uncompressed bytes actually read (0 for end of file, -1 for error)
	Purpose:	Reads the given number of uncompressed bytes from the compressed file.
				If the input file was not in gzip format, gzread copies the given number
				of bytes into the buffer.
				gzread returns the number of uncompressed bytes actually read (0 for
				end of file, -1 for error).

********************************************************************************************/

INT32 GZipFile::gzread( GZipStream *s, char *buf, unsigned len )
{
	//TRACEUSER( "Neville", _T("GZipFile::gz_read\n"));
	// check that everyhting is valid before preceeding
    if (s == NULL || s->mode != 'r' || inflate == NULL || buf == NULL) return Z_STREAM_ERROR;

    Byte *b = (Byte*)buf;

    // Section added by version 0.99 update
	if (s->transparent)
	{
		TRACEUSER( "Neville", _T("GZipFile::gz_read trying transparent!\n"));
		return 0;
	}

    // First check for problem states from last reading/peeking
    if (s->z_err == Z_DATA_ERROR) return -1; /* bad .gz file */
    if (s->z_err == Z_STREAM_END) return 0;  /* don't read crc as data */

	// use the supplied buffer to read the characters into
    s->stream.next_out = b;

	// if we have peeked then avail_out will be equal to the number of peeks made 
	if (s->stream.avail_out > 0)
	{
		// add in the values from the peeks
		ERROR3IF(s->stream.avail_out > 1,"read with peek > 1");
		buf[0] = s->PeekedValue;		// set first byte to be the peeked value
		s->PeekStatus -= 1;				// decrement peek count 
	    s->stream.avail_out -= 1;		// need one less byte for input

		if (len == 1)
		{
			// only 1 byte wanted and we have just supplied this
			// checksum the byte that we have just read in
    		s->crc = crc32(s->crc, b, len);
			return len; 			
		}
		// otherwise we require some more input
	    s->stream.avail_out = len - 1;	// need one less byte for input
	    s->stream.next_out = b + 1;		// move buffer pointer on by one
	}
	else
	    s->stream.avail_out = len;

    while (s->stream.avail_out != 0)
    {
        if (s->stream.avail_in == 0 && !s->z_eof)
        {
			s->stream.avail_in = Z_BUFSIZE;
			// Note the current file position
			FilePos Pos = s->file->tellg();
			s->file->read((char *) s->inbuf, s->stream.avail_in );
			FilePos PosAfter = s->file->rdbuf()->pubseekoff( 0, iostream::cur, iostream::in );
			s->stream.avail_in = PosAfter- Pos;
TRACEUSER( "Neville", _T("GZipFile::gz_read filled buffer size %d\n"),s->stream.avail_in);
            if (s->stream.avail_in == 0)
            {
               	//s->file->setf(s->file->eofbit);
				s->z_eof = 1;
            }
            /*else if (s->stream.avail_in == (uInt)EOF)
            {
            //    s->stream.avail_in = 0;
                s->z_eof = 1;
            //    s->z_err = Z_ERRNO;
            //    break;
            } */
            else if (s->stream.avail_in < Z_BUFSIZE || s->file->eof())
            {
				// We are at the end of the file but have read some data in
				// We must set the goodbit to clear the eof and fail bits as otherwise
				// all the checks in the diskfile will then fail.
				s->z_eof = 1;
				s->file->clear(s->file->goodbit);
            }
            s->stream.next_in = s->inbuf;
        }
        s->z_err = inflate->inflate(&(s->stream), Z_NO_FLUSH);

        if (s->z_err == Z_STREAM_END ||
            s->z_err != Z_OK  || s->z_eof) break;
    }

	// Update len with the bytes left in the uncompressed stream, should be 0 if everything ok
    len -= s->stream.avail_out;

	// checksum the data that we have just read in
    s->crc = crc32(s->crc, b, len);

	// return the actual amount read in back to the caller
    return len;
}

/********************************************************************************************

>	FilePos GZipFile::GetCurrentFilePos(GZipStream *s)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/05/95
	Inputs:		s		the zip stream that we should be using
	Returns:	the current file position
	Purpose:	Allows the progress bar to be updated with the current position in
				the compressed file.

********************************************************************************************/

FilePos GZipFile::GetCurrentFilePos(GZipStream *s)
{
    if (s == NULL || s->file == NULL) return 0;
	
	// Get the current file position, this will be ready to read in the next chunk of data
	// into the input buffer.
	// Take off the position that we started at
	FilePos				Pos = FilePos(s->file->tellg()) - s->InitialPos;

	// Take off the current amount left in the input buffer. This is our current position.
	Pos -= (FilePos)s->stream.avail_in;

	return Pos;
}



/********************************************************************************************

>	INT32 GZipFile::gzwrite(GZipStream *s, const TCHAR *buf, unsigned len)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		s		the zip stream that we should be using
				buf		the buffer that needs exporting
				len		the length of the buffer 
	Returns:	the number of uncompressed bytes actually written (0 in case of error)
	Purpose:	Writes the given number of uncompressed bytes into the compressed file.
				gzwrite returns the number of bytes actually written (0 in case of error).

********************************************************************************************/

INT32 GZipFile::gzwrite(GZipStream *s, const char *buf, unsigned len)
{
#ifdef DO_EXPORT
	//TRACEUSER( "Neville", _T("GZipFile::gz_write\n"));
	
    if (s == NULL || s->mode != 'w' || deflate == NULL || buf == NULL)
    	return Z_STREAM_ERROR;

    Byte *b = (Byte*)buf;

    s->stream.next_in = b;
    s->stream.avail_in = len;

    while (s->stream.avail_in != 0)
    {
        if (s->stream.avail_out == 0)
        {
            s->stream.next_out = s->outbuf;
            INT32 len = Z_BUFSIZE;
			s->file->write(s->outbuf, len);
            if (len != Z_BUFSIZE)
            {
                s->z_err = Z_ERRNO;
                break;
            }
            s->stream.avail_out = Z_BUFSIZE;
        }
        s->z_err = deflate->deflate(&(s->stream), Z_NO_FLUSH);
        
        // Check if we had a problem or not 
        if (s->z_err != Z_OK)
        	break;
    }

	// Update our checksum of the data
    s->crc = crc32(s->crc, b, len);

	// Return the number of bytes that we actually wrote
    return len - s->stream.avail_in;
#else
   	return Z_STREAM_ERROR;
#endif
}

/********************************************************************************************

>	INT32 GZipFile::gzflush(GZipStream *s, INT32 flush)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		s 		the zip stream that is to be used
				flush	
	Purpose:	Flushes all pending output into the compressed file. The parameter
				flush is as in the deflate() function. The return value is the zlib
			    error number (see function gzerror below). gzflush returns Z_OK if the
			    flush parameter is Z_FINISH and all output could be flushed.
				gzflush should be called only when strictly necessary because it can
				degrade compression.

********************************************************************************************/

INT32 GZipFile::gzflush(GZipStream *s, INT32 flush)
{
#ifdef DO_EXPORT
	TRACEUSER( "Neville", _T("GZipFile::gzflush\n"));

    uInt len;
    INT32 done = 0;

    if (s == NULL || s->mode != 'w' || deflate == NULL)
		 return Z_STREAM_ERROR;

    s->stream.avail_in = 0; /* should be zero already anyway */

    for (;;)
    {
        len = Z_BUFSIZE - s->stream.avail_out;

        if (len != 0)
        {
//            if (fwrite(s->outbuf, 1, len, s->file) != len)
            uInt len2 = len;
            s->file->write(s->outbuf, len2);
            if ( len2 != len)
            {
                s->z_err = Z_ERRNO;
                return Z_ERRNO;
            }
            s->stream.next_out = s->outbuf;
            s->stream.avail_out = Z_BUFSIZE;
        }
        if (done) break;
        s->z_err = deflate->deflate(&(s->stream), flush);

        /* deflate has finished flushing only when it hasn't used up
         * all the available space in the output buffer: 
         */
        done = (s->stream.avail_out != 0 || s->z_err == Z_STREAM_END);
 
        if (s->z_err != Z_OK && s->z_err != Z_STREAM_END) break;
    }
    return  s->z_err == Z_STREAM_END ? Z_OK : s->z_err;
#else
	return Z_STREAM_ERROR;
#endif
}

/********************************************************************************************

>	void GZipFile::putLong(iostream *file, uLong x)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Purpose:	Outputs an INT32 in LSB order to the given file

********************************************************************************************/

void GZipFile::putLong(iostream *file, uLong x)
{
#ifdef DO_EXPORT
    INT32 n;
    for (n = 0; n < 4; n++)
    {
//        _fputtc((INT32)(x & 0xff), file);
        file->put((TCHAR)(x & 0xff));
        x >>= 8;
    }
#endif
}

/********************************************************************************************

>	uLong GZipFile::getLong (Byte *buf)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Purpose:	Reads an INT32 in LSB order from the given buffer

********************************************************************************************/

uLong GZipFile::getLong (Byte *buf)
{
    uLong x = 0;
    Byte *p = buf+4;

    do
    {
        x <<= 8;
        x |= *--p; 
    } while (p != buf);

    return x;
}

/********************************************************************************************

>	INT32 GZipFile::gzclose(GZipStream *s)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		s	the z stream to be used  
	Returns:	Z_OK or an error number if there has been a problem
	Purpose:	Flushes all pending output if necessary, closes the compressed file
				and deallocates all the (de)compression state.
				The return value is the zlib error number (see function gzerror below).

********************************************************************************************/

INT32 GZipFile::gzclose(GZipStream *s)
{
	TRACEUSER( "Neville", _T("GZipFile::gzclose\n"));

    uInt n;

    if (s == NULL) return Z_STREAM_ERROR;

    if (s->mode == 'w')
    {
#ifdef DO_EXPORT
	    INT32 err;
        err = gzflush(s, Z_FINISH);
        if (err != Z_OK) return destroy(s);

        putLong (s->file, s->crc);
        putLong (s->file, s->stream.total_in);

		TRACEUSER( "Neville", _T("gzclose calc'd crc = %d\n"),s->crc);
		TRACEUSER( "Neville", _T("gzclose bytes written = %d\n"),s->stream.total_out);
#endif
    }
    else if (s->mode == 'r' && s->z_err == Z_STREAM_END)
    {

        /* slide CRC and original size if they are at the end of inbuf */
        if ((n = s->stream.avail_in) < 8  && !s->z_eof)
        {
            Byte *p = s->inbuf;
            Byte *q = s->stream.next_in;
            while (n--)
            {
            	*p++ = *q++;
           	};

            n = s->stream.avail_in;
//            n += fread(p, 1, 8, s->file);
            INT32 len = 8;
            s->file->read( (char *)p, len );
			n += len; 
            s->stream.next_in = s->inbuf;
        }

        // check CRC and original size
		uLong FileCrc  = getLong(s->stream.next_in);
		uLong FileSize = getLong(s->stream.next_in + 4);
		TRACEUSER( "Neville", _T("check stored crc %d, against calc'd crc = %d\n"), FileCrc, s->crc);
		TRACEUSER( "Neville", _T("check stored bytes written %d, against count = %d\n"), FileSize, s->stream.total_out);
		TRACEUSER( "Neville", _T("bytes left in buffer %d\n"), n);
        if (n < 8 || FileCrc != s->crc || FileSize != s->stream.total_out)
        {

            s->z_err = Z_DATA_ERROR;
        }

		// Quick bodge here. We now have n bytes left in the buffer, this will include 8 for the CRC etc.
		// If we close down compression then the file pointer will be sitting n - 8 bytes on from the
		// position where we actually want it. Therefore, we must skip back to the correct position.
		FilePos Pos = s->file->tellg();
		Pos = Pos - (n - 8);
		s->file->seekg(Pos);
    }

    return destroy(s);
}

/********************************************************************************************
********************************************************************************************/
//static char *z_errmsg[] =
//	{
//		"stream end",          /* Z_STREAM_END    1 */
//		"",                    /* Z_OK            0 */
//		"file error",          /* Z_ERRNO        (-1) */
//		"stream error",        /* Z_STREAM_ERROR (-2) */
//		"data error",          /* Z_DATA_ERROR   (-3) */
//		"insufficient memory", /* Z_MEM_ERROR    (-4) */
//		"buffer error",        /* Z_BUF_ERROR    (-5) */
//		""
//	};

/********************************************************************************************

>	char*  GZipFile::gzerror(GZipStream *s, INT32 *errnum)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Purpose:	Returns the error message for the last error which occured on the
				given compressed file. errnum is set to zlib error number. If an
				error occured in the file system and not in the compression library,
				errnum is set to Z_ERRNO and the application may consult errno
				to get the exact error code.

********************************************************************************************/

//char*  GZipFile::gzerror(GZipStream *s, INT32 *errnum)
//{
//	TRACEUSER( "Neville", _T("GZipFile::gzerror\n"));
//
//    char *m;
//
//    if (s == NULL)
//    {
//        *errnum = Z_STREAM_ERROR;
//        return z_errmsg[1-Z_STREAM_ERROR];
//    }
//
//    *errnum = s->z_err;
//    if (*errnum == Z_OK) return "";
//
//    if (*errnum == Z_ERRNO)
//    {
//    	_stprintf(m, "Disc error number %d", errno);  //zstrerror(errno); 	
//    }
//	else
//	{
//		m = s->stream.msg;	
//	}
//
//    if (m == NULL || *m == '\0') m = z_errmsg[1-s->z_err];
//
//    TRYFREE(s->msg);
//    s->msg = (char*)ALLOC(_tcsclen(s->path) + strlen(m) + 3);
//	if (s->msg)
//	{
//	    _tcscpy(s->msg, s->path);
//	    strcat(s->msg, ": ");
//	    strcat(s->msg, m);
//	}
//
//    return s->msg;
//}

//-------------------------------------------------------------------------------------------
// Contents of file CRC32.c
//-------------------------------------------------------------------------------------------

/********************************************************************************************

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Purpose:	Table of CRC-32's of all single-byte values (made by make_crc_table)

********************************************************************************************/
#if 0 //def DYNAMIC_CRC_TABLE

static INT32 crc_table_empty = 1;
static uLongf crc_table[256];
static void make_crc_table OF((void));

/*
  Generate a table for a byte-wise 32-bit CRC calculation on the polynomial:
  x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1.

  Polynomials over GF(2) are represented in binary, one bit per coefficient,
  with the lowest powers in the most significant bit.  Then adding polynomials
  is just exclusive-or, and multiplying a polynomial by x is a right shift by
  one.  If we call the above polynomial p, and represent a byte as the
  polynomial q, also with the lowest power in the most significant bit (so the
  byte 0xb1 is the polynomial x^7+x^3+x+1), then the CRC is (q*x^32) mod p,
  where a mod b means the remainder after dividing a by b.

  This calculation is done using the shift-register method of multiplying and
  taking the remainder.  The register is initialized to zero, and for each
  incoming bit, x^32 is added mod p to the register if the bit is a one (where
  x^32 mod p is p+x^32 = x^26+...+1), and the register is multiplied mod p by
  x (which is shifting right by one and adding x^32 mod p if the bit shifted
  out is a one).  We start with the highest power (least significant bit) of
  q and repeat for all eight bits of q.

  The table is simply the CRC of all possible eight bit values.  This is all
  the information needed to generate CRC's on data a byte at a time for all
  combinations of CRC register values and incoming bytes.
*/

/* =========================================================================
 * Make the crc table. This function is needed only if you want to compute
 * the table dynamically.
*/

void make_crc_table()
{
  uLong c;
  INT32 n, k;
  uLong poly;            /* polynomial exclusive-or pattern */
  /* terms of polynomial defining this crc (except x^32): */
  static Byte p[] = {0,1,2,4,5,7,8,10,11,12,16,22,23,26};

  /* make exclusive-or pattern from polynomial (0xedb88320L) */
  poly = 0L;
  for (n = 0; n < sizeof(p)/sizeof(Byte); n++)
    poly |= 1L << (31 - p[n]);
 
  for (n = 0; n < 256; n++)
  {
    c = (uLong)n;
    for (k = 0; k < 8; k++)
      c = c & 1 ? poly ^ (c >> 1) : c >> 1;
    crc_table[n] = c;
  }
  crc_table_empty = 0;
}
#else
/* ========================================================================
 * Table of CRC-32's of all single-byte values (made by make_crc_table)
 */
static uLongf crc_table[256] = {
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};
#endif

/* ========================================================================= */
#define DO1(buf) crc = crc_table[((INT32)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

/* ========================================================================= */

/********************************************************************************************

>	uLong GZipFile::crc32(uLong crc, Bytef *buf, uInt len)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		crc		previous checksum value???
				buf		pointer to the buffer to check
				len 	length of the buffer
	Returns:	the checksum value
	Purpose:	Update a running crc with the bytes buf[0..len-1] and return the updated
				crc. If buf is NULL, this function returns the required initial value
				for the crc. Pre- and post-conditioning (one's complement) is performed
				within this function so it shouldn't be done by the application.
				Usage example:

				uLong crc = crc32(0L, Z_NULL, 0);

				while (read_buffer(buffer, length) != EOF)
				{
					crc = crc32(crc, buffer, length);
				}
				if (crc != original_crc) error();

********************************************************************************************/
uLong GZipFile::crc32(uLong crc, Bytef *buf, uInt len)
{
    if (buf == Z_NULL) return 0L;
#ifdef DYNAMIC_CRC_TABLE
    if (crc_table_empty)
      make_crc_table();
#endif
    crc = crc ^ 0xffffffffL;
    while (len >= 8)
    {
      DO8(buf);
      len -= 8;
    }
    if (len) do {
      DO1(buf);
    } while (--len);
    return crc ^ 0xffffffffL;
}

/* =========================================================================
 * This function can be used by asm versions of crc32()
 */
uLongf *GZipFile::get_crc_table()
{
#ifdef DYNAMIC_CRC_TABLE
  if (crc_table_empty) make_crc_table();
#endif
  return (uLongf *)crc_table;
}

/********************************************************************************************
********************************************************************************************/

//-------------------------------------------------------------------------------------------
// Contents of file CRC32.c
//-------------------------------------------------------------------------------------------

#define BASE 65521L /* largest prime smaller than 65536 */
#define NMAX 5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define A_DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define A_DO2(buf,i)  A_DO1(buf,i); A_DO1(buf,i+1);
#define A_DO4(buf,i)  A_DO2(buf,i); A_DO2(buf,i+2);
#define A_DO8(buf,i)  A_DO4(buf,i); A_DO4(buf,i+4);
#define A_DO16(buf)   A_DO8(buf,0); A_DO8(buf,8);

/********************************************************************************************

>	uLong GZipFile::adler32(uLong adler, const Bytef *buf, uInt len)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		crc		previous checksum value???
				buf		pointer to the buffer to check
				len 	length of the buffer
	Returns:	the checksum value
	Purpose:	Update a running Adler-32 checksum with the bytes buf[0..len-1] and
				return the updated checksum. If buf is NULL, this function returns
				the required initial value for the checksum.
				An Adler-32 checksum is almost as reliable as a CRC32 but can be computed
				much faster. Usage example:

				uLong adler = adler32(0L, Z_NULL, 0);

				while (read_buffer(buffer, length) != EOF)
				{
					adler = adler32(adler, buffer, length);
				}
				if (adler != original_adler) error();

********************************************************************************************/

/* ========================================================================= */
uLong GZipFile::adler32(uLong adler, const Bytef *buf, uInt len)
{
    UINT32 s1 = adler & 0xffff;
    UINT32 s2 = (adler >> 16) & 0xffff;
    INT32 k;

    if (buf == Z_NULL) return 1L;

    while (len > 0) {
        k = len < NMAX ? len : NMAX;
        len -= k;
        while (k >= 16) {
            A_DO16(buf);
	    buf += 16;
            k -= 16;
        }
        if (k != 0) do {
            s1 += *buf++;
	    s2 += s1;
        } while (--k);
        s1 %= BASE;
        s2 %= BASE;
    }
    return (s2 << 16) | s1;}


/********************************************************************************************

>	void *GZipFile::zcalloc (opaque, items, size)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		opaque	random memory handle/checksum value, never used
				items 	number of items required
				size 	size of each buffer item
	Returns:	the checksum value
	Purpose:	Allocate memory of size items and with size bytes per item

********************************************************************************************/

void *GZipFile::zcalloc(void *opaque, unsigned items, unsigned size)
{
	// allocate memory of size items and with size bytes per item
    //return calloc(items, size);
	return CCMalloc(items * size);
}

/********************************************************************************************

>	void  GZipFile::zcfree(opaque, ptr)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		opaque	random memory handle/checksum value, never used
				ptr		memory pointer to free, must have been allocated by zcalloc
	Purpose:	A wrapper round the free function

********************************************************************************************/

void  GZipFile::zcfree(void *opaque, void *ptr)
{
    //free(ptr);
	if (ptr)
		CCFree(ptr);
}
