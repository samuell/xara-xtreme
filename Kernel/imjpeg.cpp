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

// A JPEG import filter 

#include "camtypes.h"
//#include "filters.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "progress.h"
//#include "oilfltrs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "oilbitmap.h"
//#include "dibutil.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "filtrres.h"
//#include "andy.h"			// for _R(IDE_FORMATNOTSUPPORTED)
//#include "resource.h"		// for _R(IDS_OUT_OF_MEMORY)
//#include "accures.h"		// for _R(IDW_CANCELLEDBMPIMPORT)
#include "bmpsrc.h"			// for (JPEG)BitmapSource
//#include "camfiltr.h"		// for GetCurrentRecordSize() - in camtypes.h [AUTOMATICALLY REMOVED]

#include "imjpeg.h"
#include "jpgsrc.h"
#include "jpgprgrs.h"
#include "jpgermgr.h"

/********************************************************************************************

>	class JPEGImportOptions : public BitmapImportOptions

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/06/96
	Purpose:   	Import options for the JPEG import filter. Allows a clean method of passing
				assorted parameters for the JPEG import filter. Specifically, that the
				calling class should give us a progress bar.
	Scope:		public
	See Also:	BitmapImportOptions

********************************************************************************************/
class JPEGImportOptions : public BitmapImportOptions
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(JPEGImportOptions);
public:
	JPEGImportOptions() : BitmapImportOptions() {m_bProvideProgress = TRUE;}

	BOOL m_bProvideProgress;
	
};
/*
class JPEGImportResults
{
public:
	LPBITMAPINFO	*m_ppInfo;
	LPBYTE			*m_ppBytes;
};
*/

// Place any IMPLEMENT type statements here
CC_IMPLEMENT_MEMDUMP(JPEGImportFilter, BaseBitmapFilter)
CC_IMPLEMENT_MEMDUMP(JPEGImportOptions,  BitmapImportOptions)



// We want better memory tracking
#define new CAM_DEBUG_NEW

// Functions follow



/********************************************************************************************

>	JPEGImportFilter::JPEGImportFilter()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Purpose:	Default constructor for a JPEGFilter object.

********************************************************************************************/
JPEGImportFilter::JPEGImportFilter() : BaseBitmapFilter()
{
	// Initialize members for class Filter
	ImportMsgID = _R(IDS_IMPORTMSG_JPEG);

	Flags.CanImport					= TRUE;
	Flags.CanExport					= FALSE;
	Flags.CanExportMultipleImages	= FALSE;
	Flags.ShowFilter				= TRUE;

	FilterID = FILTERID_IMPORT_JPEG;

	// Initialize our class members
	m_pFile					= NULL;
	m_bOldReportErrors		= FALSE;
	m_bOldThrowExceptions	= FALSE;
	m_uStartOffset			= 0;		// Offset from start at which header appears, maybe...

	m_pErrorHandler			= NULL;
	m_pSourceHandler		= NULL;
	m_pProgressMonitor		= NULL;

	m_pFilterForProgress	= NULL;
	m_pFileForProgress		= NULL;
	m_uImportSize			= 0;

	m_ppInfo				= NULL;
	m_ppBytes				= NULL;
	m_uWidth				= 0;
	m_uHeight				= 0;
	m_uBitsPerPixel			= 0;
}


/********************************************************************************************

>	JPEGImportFilter::~JPEGImportFilter()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Purpose:	Default constructor for a JPEGFilter object.

********************************************************************************************/
JPEGImportFilter::~JPEGImportFilter()
{
	if (m_pErrorHandler != NULL)
		delete m_pErrorHandler;

	if (m_pSourceHandler != NULL)
		delete m_pSourceHandler;

	if (m_pProgressMonitor != NULL)
		delete m_pProgressMonitor;
}

/********************************************************************************************

>	BOOL JPEGImportFilter::Init()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Returns:	TRUE if the filter was initialized ok
				FALSE otherwise
	Purpose:	Initializes the JPEGImportFilter class.

********************************************************************************************/
BOOL JPEGImportFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new JPEGImportOILFilter(this);
	if (pOILFilter==NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(_R(IDS_JPG_IMP_FILTERNAME));
	FilterInfo.Load(_R(IDS_JPG_IMP_FILTERINFO));

	// All ok
	return TRUE;
}


/********************************************************************************************

>	ADDR FindBytes(ADDR pStart, UINT32 BufferSize, ADDR pBytes, UINT32 BytesSize)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Inputs:		pStart - The address of the first byte in memory from which to start
				BufferSize - The size of the block starting at pStart
				pBytes : the start address of the block of bytes to search for
				BytesSize : The number of bytes in the block pointed to by pBytes
	Returns:	The address of the first matching block of memory
	Purpose:	Finds BytesSize bytes starting at pBytes in pStart, which is BufferSize long
				Searches for a string of bytes
	Scope:		static
	Notes:		This should be put somewhere useful really. (Templates!!)
				It could also be more efficient if necessary

********************************************************************************************/
ADDR FindBytes(ADDR pStart, UINT32 BufferSize, ADDR pBytes, UINT32 BytesSize)
{
	UINT32 BytesRemaining = BufferSize;

	ADDR pCurrentStart = pStart;
	while (BytesRemaining - BytesSize > 0)
	{
		ADDR pFirstFound = (ADDR)memchr(pCurrentStart, pBytes[0], BytesRemaining - BytesSize + 1);
		if (pFirstFound == NULL)
		{	
			return NULL;
		}
		if (memcmp(pFirstFound + 1, pBytes + 1, BytesSize - 1) == 0)
		{
			return pFirstFound;
		}
		pCurrentStart = pFirstFound + 1;
		BytesRemaining = BufferSize - (pCurrentStart - pStart);
	}
	return NULL;
}


/********************************************************************************************

>	virtual INT32 JPEGImportFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, 
										UINT32 HeaderSize, UINT32 FileSize)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Inputs:		Filename - name of the file.
				HeaderStart - Address of the first few bytes of the file.
				HeaderSize - the number of bytes in the header pointed to by FileStart.
				FileSize - the size of the whole file, in bytes.
	Returns:	0 => Not a JPEG file.
				10 => Bound to be a JPEG file.
				1 - 9 => Might be
	Purpose:	Overrides the Filter class member to determine if the given file is reckoned
				to be a JPEG.

********************************************************************************************/
INT32 JPEGImportFilter::HowCompatible(PathName& Filename, ADDR pHeaderStart, UINT32 HeaderSize, 
							 UINT32 FileSize)
{
PORTNOTE("byteorder", "TODO: Check byte ordering")
	static BYTE		JPEGSignature[] = {0xFF,0xD8};
	static BYTE		APP0Signature[] = {0xFF,0xE0};
	static BYTE		APP1Signature[] = {0xFF,0xE1};
	static BYTE		JFIFSignature[] = "JFIF";
	static BYTE		EXIFSignature[] = "Exif";
	// If we're forced to import the file when we don't think there's the remotest
	// chance of a match, start at the beginning of the file in ReadFromFile
	m_uStartOffset = 0;	

	// Check that we've got enough data to do our check
	if (HeaderSize < 2)
	{
		// Not enough data - ignore this file.
		return 0;
	}

	INT32 Compatability = 0;
	BOOL bFoundSOI	= FALSE;
	BOOL bFoundJFIF	= FALSE;
	BOOL bFoundEXIF	= FALSE;

	// Check the header for the JPEG SOI signature, 0xFFD8 (unless it's 0xD8FF).
	ADDR pJPEGHeader = FindBytes(pHeaderStart, HeaderSize, JPEGSignature, 2);
	if (pJPEGHeader != NULL)
	{
		// Add in a value of 0 to 5 depending on the distance from the
		// start of the header
		m_uStartOffset = pJPEGHeader - pHeaderStart;
		bFoundSOI = TRUE;
	}

	if (bFoundSOI)
	{
		OFFSET HeaderBytesRemaining = HeaderSize - (pJPEGHeader - pHeaderStart);

		// Look for an APP0 containing JFIF
		char* pAPP0Start = (char*)FindBytes(pJPEGHeader + 2, HeaderBytesRemaining - 2, APP0Signature, 2);
		if (pAPP0Start)
		{
			if (strncmp(pAPP0Start + 4, (char *)JFIFSignature, sizeof(JFIFSignature) / sizeof(TCHAR)) == 0)
			{
				bFoundJFIF = TRUE;
			}
		}
		
		// Look for an APP1 containing Exif
		char* pAPP1Start = (char*)FindBytes(pJPEGHeader + 2, HeaderBytesRemaining - 2, APP1Signature, 2);
		if (pAPP1Start)
		{
			if (strncmp(pAPP1Start + 4, (char *)EXIFSignature, sizeof(EXIFSignature) / sizeof(TCHAR)) == 0)
			{
				bFoundEXIF = TRUE;
			}
		}

		// Andy Hills, 02-11-00
		// Previously, non-JPEG files were scoring as highly as 9, on the
		// basis that they had JPEG headers ... only, not at the start of the
		// file, but several hundred bytes in!!!
		// Now, we will give a score of 10 to a legit JPEG, or a score of 2-5
		// depending upon the nearness of the JPEG header to the start of
		// the file. This means that files with JPEG previews will score a few
		// points, e.g. PSD & PSP files.

		switch (m_uStartOffset)
		{
		case 0:
			// the header was at the very start of the file -->
			// it really is a JPEG!!!
			// we will give it a score of up to 10
			Compatability = 6;
			break;

		default:
			// the header was some way into the file -->
			// it probably isn't a JPEG, but has a JPEG preview
			// we will give it a max score of 5
			
			// Nearness evaluates to between 0 and 3
			INT32 Nearness = (((HeaderSize - m_uStartOffset) * 4) / HeaderSize);
			// Compatibility evaluates to between -2 and 1
			Compatability = Nearness - 2;
			break;
		}
	}

	if (bFoundJFIF || bFoundEXIF)
	{
		// Closer still
		Compatability += 4;
	}

	// Return the found value to the caller.
	return max(Compatability,0);
}


/********************************************************************************************

 >	virtual BOOL JPEGImportFilter::IsFormatLossy() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/08/96

	Returns:	TRUE
	Purpose:	The JPEGImportFilter imports a lossy format & therefore always returns TRUE
	See Also:	BaseBitmapFilter::IsFormatLossy()

********************************************************************************************/
BOOL JPEGImportFilter::IsFormatLossy() const
{
	return TRUE;
}


/********************************************************************************************

 >	virtual OFFSET BaseBitmapFilter::GetDataStartOffset() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/08/96

	Returns:	The offset from the HeaderStart parameter given in HowCompatible(), 
				from which data essential to the correct functionality of this filter 
				starts.

	Purpose:	Provides information to the BaseBitmapFilter, so that it knows where
				to start any required BitmapSource. It will only be useful to derived 
				classes that deal with a lossy file format.

	See Also:	BaseBitmapFilter::GetDataStartOffset()

********************************************************************************************/
OFFSET JPEGImportFilter::GetDataStartOffset() const
{
	return m_uStartOffset;
}


/********************************************************************************************

>	virtual BitmapSource* BaseBitmapFilter::CreateBitmapSource() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/08/96
	Returns:	A pointer to a new BitmapSource
	Purpose:	Provides a BitmapSource for lossy file formats.
				The JPEGImportFilter returns a JPEGBitmapSource
	SeeAlso:	BaseBitmapFIlter::CreateBitmapSource
	Scope: 		protected

********************************************************************************************/
BitmapSource* JPEGImportFilter::CreateBitmapSource(OFFSET Size) const
{
	BitmapSource* pSource = new JPEGBitmapSource(Size);
	return pSource;
}


/********************************************************************************************

>	BOOL JPEGImportFilter::ReadFromFile(OILBitmap* pOilBitmap, BaseCamelotFilter* pFilter,
										CCLexFile* pFile, BOOL IsCompressed)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Neville Humphrys in PNGFilter)
	Created:	14/08/96
	Inputs:		pFilter:	the BaseCamelotFilter which provides functions like
							progress update
				pFile		the CCFile class to use to read the data from
				IsCompressed :Flag to say the bitmap is compressed or not.  
	Outputs:	pOilBitmap: Will have filled in BMInfo	pointer to the bitmap header to fill
							in. BMBytes	pointer to the bitmap data to fill in
	Purpose:	Entry point for Native File Format stuff
				Actually does the process of reading a bitmap from a file.
				Inherited classes override this to read in different file formats.
				It is used by the web/native filters to pull out a bitmap definition from
				inside a bitmap definition record.
	Returns:	TRUE if worked, FALSE if failed.

	Notes:		m_uStartOffset, the offset in the stream at which the JPEG header starts,
				is set to zero through this entry point. This assumes, therefore, that the
				JPEG contains no Adobe style garbage in front of it.
				Also assumes:
				* pFile has already been opened up for reading
				* pFilter has been set up for reading the data e.g. progress bar 

********************************************************************************************/

BOOL JPEGImportFilter::ReadFromFile(OILBitmap* pOilBitmap, BaseCamelotFilter* pFilter,
									CCLexFile* pFile, BOOL IsCompressed)
{
	ERROR2IF(pOilBitmap == NULL,FALSE,"pOilBitmap NULL");
	ERROR2IF(pFilter == NULL,FALSE,"pFilter NULL");
	ERROR2IF(pFile == NULL,FALSE,"pFile NULL");

//	CCFile* pFile	= (CCFile*)pFile->GetIOFile();
	
	m_pFilterForProgress	= (Filter*)pFilter;
	m_pFileForProgress		= NULL;
	m_uImportSize			= pFilter->GetCurrentRecordSize();

	// The encapsulated JPEG should always have its header starting at offset
	// zero inside its .xar Record
	m_uStartOffset = 0;

	// Try to import bitmap from JPEG file
	if (!DoFilter(pFile, pOilBitmap))
	{
		return FALSE;
	}

	// Make sure this bitmap is treated as lossy
	pOilBitmap->SetAsLossy();

	// Clear any eof signal as CCFile considers this an error
	if (m_pFile->eof())
	{
		m_pFile->SetGoodState();
	}
	return TRUE;
}


/********************************************************************************************

>	BOOL JPEGImportFilter::ReadFromFile(OILBitmap* pOilBitmap)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		pOilBitmap : pointer to the OILBitmap to be filled in
	Outputs:	pOilBitmap : If successful will contain the bitmap data from reading the
				JPEG file (GetImportFile()).
	Returns:	TRUE if successful
				FALSE if not
	Purpose:	Actually does the process of reading a bitmap from a file.
				Inherited classes override this to read in different file formats.
				JPEG reads a JPEG file.
	Errors:		ERROR2s for invalid parameters

	Notes:		m_uStartOffset must have been set prior to calling this (use HowCompatible()
				or set it zero).
				

********************************************************************************************/
BOOL JPEGImportFilter::ReadFromFile(OILBitmap* pOilBitmap)
{
	ERROR2IF(pOilBitmap == NULL, FALSE, "pOilBitmap is NULL");

	m_pFileForProgress		= GetImportFile();		// get BaseBitmapFilter member
	m_pFilterForProgress	= NULL;

	CCFile* pFile = GetImportFile();

	return DoFilter(pFile, pOilBitmap);
}



/********************************************************************************************

>	BOOL JPEGImportFilter::DoFilter(CCFile* pInputFile, OILBitmap* pOilBitmap)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		pInputFile : a pointer to an open CCFile containing the JPEG data to be
				imported
				pOilBitmap : pointer to the OILBitmap to be filled in
	Outputs:	pOilBitmap : If successful will contain the bitmap data from reading the
				JPEG file (GetImportFile()).
	Returns:	TRUE if successful
				FALSE if not
	Purpose:	Provides the main processing point for the JPEG import.
				Called by the external interface

	Notes:		Here's how the IJG docs say we do it:
				1) Allocate and initialize a JPEG decompression object
				2) Specify the source of the compressed data (eg, a file)
				3) Call jpeg_read_header() to obtain image info
				4) Set parameters for decompression
				5) jpeg_start_decompress(...);
				6) while (scan lines remain to be read)
					jpeg_read_scanlines(...);
				7) jpeg_finish_decompress(...);
				8) Release the JPEG decompression object

	See Also:	ReadFromFile(...)
				

********************************************************************************************/
BOOL JPEGImportFilter::DoFilter(CCFile* pInputFile, OILBitmap* pOilBitmap)
{
	ERROR2IF(pOilBitmap == NULL || pInputFile == NULL, FALSE, "NULL Args");
	ERROR3IF(!pInputFile->IS_KIND_OF(CCFile), "pInputFile isn't");

	m_pFile = pInputFile;

	CWxBitmap* pWBitmap = (CWxBitmap*)pOilBitmap;
	ERROR2IF(!pWBitmap->IS_KIND_OF(CWxBitmap), FALSE, "Only CWxBitmap supported");

	m_ppInfo	= &(pWBitmap->BMInfo);
	m_ppBytes	= &(pWBitmap->BMBytes);
	
	// Try to import the bitmap from a JPEG file.
	if (!PrepareFilterForOperation())
		return FALSE;

	try
	{
		ReadHeader();

		PrepareForImage(m_pImportOptions);

		ReadImage();

		OnImageCompletion();
	}
	catch(...)
	{
		// catch our form of a file exception
		// Tidy up
		StringID errorString = m_pErrorHandler->GetStringIDForError();
		
		Error::SetError(errorString);

		if (*m_ppInfo != NULL && *m_ppBytes != NULL)
		{
			FreeDIB( *m_ppInfo, *m_ppBytes );							// free any alloced memory
		}
		*m_ppInfo = NULL;										// and NULL the pointers
		*m_ppBytes = NULL;

		/*
		The IJG library says:

		You can abort a decompression cycle by calling jpeg_destroy_decompress() or
		jpeg_destroy() if you don't need the JPEG object any more, or
		jpeg_abort_decompress() or jpeg_abort() if you want to reuse the object.
		*/

		OnFilterCompletion();		

		return FALSE;
	}

	OnFilterCompletion();

	return TRUE;
}



/********************************************************************************************

>	virtual BOOL JPEGFilter()::PrepareFilterForOperation();

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Purpose:	Default constructor for a JPEGFilter object.

1. Allocate and initialize a JPEG decompression object.

This is just like initialization for compression, as discussed above,
except that the object is a "struct jpeg_decompress_struct" and you
call jpeg_create_decompress().  Error handling is exactly the same.

A JPEG compression object is a "struct jpeg_compress_struct".  (It also has
a bunch of subsidiary structures which are allocated via malloc(), but the
application doesn't control those directly.)  This struct can be just a local
variable in the calling routine, if a single routine is going to execute the
whole JPEG compression sequence.  Otherwise it can be static or allocated
from malloc().

You will also need a structure representing a JPEG error handler.  The part
of this that the library cares about is a "struct jpeg_error_mgr".  If you
are providing your own error handler, you'll typically want to embed the
jpeg_error_mgr struct in a larger structure; this is discussed later under
"Error handling".  For now we'll assume you are just using the default error
handler.  The default error handler will print JPEG error/warning messages
on stderr, and it will call exit() if a fatal error occurs.

You must initialize the error handler structure, store a pointer to it into
the JPEG object's "err" field, and then call jpeg_create_compress() to
initialize the rest of the JPEG object.

Typical code:

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	...
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

jpeg_create_compress allocates a small amount of memory, so it could fail
if you are out of memory.  In that case it will exit via the error handler;
that's why the error handler must be initialized first.

(Both here and in the IJG code, we usually use variable name "cinfo" for
both compression and decompression objects.)

2. Specify the source of the compressed data (eg, a file).

As previously mentioned, the JPEG library reads compressed data from a "data
source" module.  The library includes one data source module which knows how
to read from a stdio stream.  You can use your own source module if you want
to do something else, as discussed later.

If you use the standard source module, you must open the source stdio stream
beforehand.  Typical code for this step looks like:

	FILE * infile;
	...
	if ((infile = fopen(filename, "rb")) == NULL) {
	    fprintf(stderr, "can't open %s\n", filename);
	    exit(1);
	}
	jpeg_stdio_src(&cinfo, infile);

where the last line invokes the standard source module.

WARNING: it is critical that the binary compressed data be read unchanged.
On non-Unix systems the stdio library may perform newline translation or
otherwise corrupt binary data.  To suppress this behavior, you may need to use
a "b" option to fopen (as shown above), or use setmode() or another routine to
put the stdio stream in binary mode.  See cjpeg.c and djpeg.c for code that
has been found to work on many systems.

You may not change the data source between calling jpeg_read_header() and
jpeg_finish_decompress().  If you wish to read a series of JPEG images from
a single source file, you should repeat the jpeg_read_header() to
jpeg_finish_decompress() sequence without reinitializing either the JPEG
object or the data source module; this prevents buffered input data from
being discarded.

********************************************************************************************/
BOOL JPEGImportFilter::PrepareFilterForOperation()
{
	// The error handling in the following should probably be replaced by exceptions at
	// some time, when somebody can decide on a standard.

	// Set up a JPEGErrorManager
	if (!InitErrorHandler())
	{
		return FALSE;
	}
	
	// Initialize the main JPG library structure
	try
	{
		using namespace libJPEG;
		jpeg_create_decompress( &m_cinfo );
	}
	catch(...)
	{
		return FALSE;
	}

	// Setup a JPEGDataSource
	if (!InitFileHandler())
	{
		return FALSE;
	}

	// Set up a progress bar
	if (!InitProgressMonitor())
	{
		return FALSE;
	}

	// Don't let the CCFile report errors or throw exceptions
	m_bOldThrowExceptions	= m_pFile->SetThrowExceptions(FALSE);
	m_bOldReportErrors		= m_pFile->SetReportErrors(FALSE);

	return TRUE;
}




/********************************************************************************************

>	void JPEGImportFilter::ReadHeader()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Returns:	TRUE if header was correctly read & the data set
				FALSE otherwise
	Purpose:	This will read the source datastream header markers, up to the beginning
				of the compressed data proper.  On return, the image dimensions and other
				info have been stored in the JPEG object.  The application may wish to
				consult this information before selecting decompression parameters.
	Errors:		Throws exceptions
	Notes:		PrepareFilterForOperation must have been called prior to this call.
				
				From the IJG docs:

				More complex code is necessary if
				  * A suspending data source is used --- in that case jpeg_read_header()
					may return before it has read all the header data.
				  * Abbreviated JPEG files are to be processed --- see the section on
					abbreviated datastreams.  Standard applications that deal only in
					interchange JPEG files need not be concerned with this case either.

				It is permissible to stop at this point if you just wanted to find out the
				image dimensions and other header info for a JPEG file.  In that case,
				call jpeg_destroy() when you are done with the JPEG object, or call
				jpeg_abort() to return it to an idle state before selecting a new data
				source and reading another header.

********************************************************************************************/
void JPEGImportFilter::ReadHeader()
{
	jpeg_read_header(&m_cinfo, TRUE);
}


/********************************************************************************************

>	virtual BitmapImportOptions* JPEGImportFilter::GetImportOptions()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Returns:	A pointer to some new JPEGImportOptions if successful
				NULL otherwise
	Purpose:	Overrides the BaseBitmapFilter::GetImportOptions to This will read the source datastream header markers, up to the beginning

	Notes:		From the IJG docs:


jpeg_read_header() sets appropriate default decompression parameters based on
the properties of the image (in particular, its colorspace).  However, you
may well want to alter these defaults before beginning the decompression.
For example, the default is to produce full color output from a color file.
If you want colormapped output you must ask for it.  Other options allow the
returned image to be scaled and allow various speed/quality tradeoffs to be
selected.  "Decompression parameter selection", below, gives details.

If the defaults are appropriate, nothing need be done at this step.

Note that all default values are set by each call to jpeg_read_header().
If you reuse a decompression object, you cannot expect your parameter
settings to be preserved across cycles, as you can for compression.
You must set desired parameter values each time.

	See Also:	BaseBitmapFilter::GetImportOptions()

********************************************************************************************/
BitmapImportOptions* JPEGImportFilter::GetImportOptions()
{
	JPEGImportOptions* pOptions = NULL;

	// Should ReadHeader here...

	pOptions = new JPEGImportOptions;

/*
	Might want to set one or two of these at some point

	J_COLOR_SPACE out_color_space; // colorspace for output

	UINT32 scale_num, scale_denom; // fraction by which to scale image

	double output_gamma;		// image gamma wanted in output

	boolean buffered_image;	// TRUE=multiple output passes
	boolean raw_data_out;		// TRUE=downsampled data wanted

	J_DCT_METHOD dct_method;	// IDCT algorithm selector
	boolean do_fancy_upsampling;	// TRUE=apply fancy upsampling
	boolean do_block_smoothing;	// TRUE=apply interblock smoothing

	boolean quantize_colors;	// TRUE=colormapped output wanted

	// the following are ignored if not quantize_colors:
	J_DITHER_MODE dither_mode;	//type of color dithering to use
	boolean two_pass_quantize;	// TRUE=use two-pass color quantization
	INT32 desired_number_of_colors;	// max # colors to use in created colormap
	
	// these are significant only in buffered-image mode:
	boolean enable_1pass_quant;	// enable future use of 1-pass quantizer
	boolean enable_external_quant;// enable future use of external colormap
	boolean enable_2pass_quant;	 enable future use of 2-pass quantizer
*/

	return (BitmapImportOptions*)pOptions;
}


/********************************************************************************************

>	virtual void JPEGImportFilter::PrepareForImage(BitmapImportOptions* pOptions)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Inputs:		pOptions : a pointer to some JPEGImportOptions (currently NULL)
	Purpose:	Prepares a place for the actual scanlines using the given options

	Notes:		From the IJG docs:

				Once the parameter values are satisfactory, call jpeg_start_decompress() to
				begin decompression.  This will initialize internal state, allocate working
				memory, and prepare for returning data.
				If you have requested a multi-pass operating mode, such as 2-pass color
				quantization, jpeg_start_decompress() will do everything needed before data
				output can begin.  In this case jpeg_start_decompress() may take quite a while
				to complete.  With a single-scan (non progressive) JPEG file and default
				decompression parameters, this will not happen; jpeg_start_decompress() will
				return quickly.

				After this call, the final output image dimensions, including any requested
				scaling, are available in the JPEG object; so is the selected colormap, if
				colormapped output has been requested.  Useful fields include

					output_width		image width and height, as scaled
					output_height
					out_color_components	# of color components in out_color_space
					output_components	# of color components returned per pixel
					colormap		the selected colormap, if any
					actual_number_of_colors		number of entries in colormap
				
				output_components is 1 (a colormap index) when quantizing colors; otherwise it
				equals out_color_components.  It is the number of JSAMPLE values that will be
				emitted per pixel in the output arrays.
	
				Typically you will need to allocate data buffers to hold the incoming image.
				You will need output_width * output_components JSAMPLEs per scanline in your
				output buffer, and a total of output_height scanlines will be returned.

********************************************************************************************/
void JPEGImportFilter::PrepareForImage(BitmapImportOptions* pOptions)
{
	//ERROR2IF(pOptions == NULL, FALSE, "pOptions is NULL");

	// Set up conversion parameters (should be in GetImportOptions really)
 
	//  "The return value need be inspected only if a suspending data source is used"

	/*
		One day...
	if (m_cinfo.jpeg_color_space == JCS_YCCK)
	{
		// Convert YCCK to RGB
		m_cinfo.out_color_space = JCS_RGB;
	}
	*/
	(void)jpeg_start_decompress(&m_cinfo);	

	// Store the image width & height
	m_uWidth = m_cinfo.output_width;
	m_uHeight = m_cinfo.output_height;

	// Compute colormap size and total file size
	switch (m_cinfo.out_color_space)
	{
		case libJPEG::JCS_RGB:
		{
			// Unquantized, full color RGB
			m_uBitsPerPixel = 24;
			break;
		}

		case libJPEG::JCS_GRAYSCALE:
		{
			// Grayscale output
	    	m_uBitsPerPixel = 8;
			break;
		}

		default:
		{
			m_pErrorHandler->ThrowError(_R(IDS_JPEG_ERROR_UNSUPPORTED));
		}
	}

	// So we know
	*m_ppInfo = NULL;
	*m_ppBytes = NULL;

	// we know what sort of bitmap we are - let's allocate a new LPBITMAPINFO and some bytes
	*m_ppInfo = AllocDIB( m_uWidth, m_uHeight, m_uBitsPerPixel, m_ppBytes, NULL );
	if (*m_ppInfo == NULL || *m_ppBytes == NULL)
	{
		m_pErrorHandler->ThrowError( _R(IDS_OUT_OF_MEMORY) );
	}

	if (!ReadPalette())
	{
		m_pErrorHandler->ThrowError( _R(IDS_PNG_ERR_READ_PALETTE) ); // Should be bad palette error
	}

}



/********************************************************************************************

>	BOOL JPEGImportFilter::SetBitmapResolution()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Returns:	TRUE if resolution set OK
				FALSE otherwise
	Purpose:	Support function that sets the X & Y pixel densities for the bitmap imported
				from the JPEG file.

********************************************************************************************/
BOOL JPEGImportFilter::SetBitmapResolution()
{
	// Set the horizontal & vertical pixel densities
	LPBITMAPINFOHEADER pHeader = &(*m_ppInfo)->bmiHeader;

	switch (m_cinfo.density_unit)
	{
		case 1:		// dots/inch
			pHeader->biXPelsPerMeter = (INT32)(m_cinfo.X_density * (100 / 2.54));
			pHeader->biYPelsPerMeter = (INT32)(m_cinfo.Y_density * (100 / 2.54));
			break;

		case 2:		// dots/cm
			pHeader->biXPelsPerMeter = m_cinfo.X_density * 100;
			pHeader->biYPelsPerMeter = m_cinfo.Y_density * 100;
			break;

		default:	// unknown
			// Note that the pixel aspect ratio is defined by X_density/Y_density 
			// even when density_unit=0.
			// So leave it as is: 96dpi
			break;
	}
	return TRUE;
}


/********************************************************************************************

>	BOOL JPEGImportFilter::ReadPalette()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Returns:	TRUE if a palette was read correctly & the data set
				FALSE otherwise
	Purpose:	Reads a palette for the image
				In this case it just generates a greyscale palette for JCS_GRAYSCALE JPEGs.
				All other colour models are 24-bit

********************************************************************************************/
BOOL JPEGImportFilter::ReadPalette()
{
	if (m_cinfo.out_color_space == libJPEG::JCS_GRAYSCALE)
	{
		// We'll have to generate a palette for it
		LPRGBQUAD pPalette = (*m_ppInfo)->bmiColors;
		if (pPalette == NULL)
		{
			return FALSE;
		}

		if (!DIBUtil::GenGreyscalePalette(pPalette, 256))
		{
			return FALSE;
		}
	}
	return TRUE;
}


/********************************************************************************************

>	virtual void JPEGImportFilter::ReadImage()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Purpose:	Reads the actual scanlines of the image into this's DIB data members
	Errors:		Throws exceptions

	Notes:		Once PrepareForImage() has been called this function can read in the data

********************************************************************************************/
void JPEGImportFilter::ReadImage()
{
//#ifndef WIN32
//	Error::SetError( _R(IDE_BADFORMAT) );
//	return FALSE;
//#else

	// Sanity checks on the file that we have been asked to load.
	if ((m_uBitsPerPixel != 24 && m_uBitsPerPixel != 8)
		|| (m_uWidth == 0 || m_uHeight == 0)
		|| m_cinfo.output_components > 3)
	{
		m_pErrorHandler->ThrowError( _R(IDE_FORMATNOTSUPPORTED) );
	}

	// Work out the word/byte rounded line width rather than the pixel width
	INT32 WidthOfLine = DIBUtil::ScanlineSize( m_uWidth, m_uBitsPerPixel );

	// Of course being DIBs we need to read the data in upside down! i.e. bottom to top
	// So start from the bottom
	LPBYTE pCurrentScanLine = *m_ppBytes + 
								((m_uHeight - 1 - m_cinfo.output_scanline)  * WidthOfLine);

	while (m_cinfo.output_scanline < m_cinfo.output_height)
	{
		// jpeg_read_scanlines expects an array of pointers to scanlines.
		// Here the array is only one element long, but you could ask for
		// more than one scanline at a time if that's more convenient.
	
		// Read in from bottom upwards
		(void) jpeg_read_scanlines(&m_cinfo, &pCurrentScanLine, 1);
		//
		// Swap red and blue channels.
		//
#if !defined(__WXMSW__) && defined(BIG_ENDIAN)
		if ( m_uBitsPerPixel==24 )
		{
			BYTE* pLine = pCurrentScanLine;
			for ( UINT32 i=0 ; i<m_uWidth ; i++ )
			{
				BYTE t   = pLine[0];
				pLine[0] = pLine[2];
				pLine[2] = t;
				pLine += 3;
			}
		}
#endif

		// If JobState is False then the user has probably pressed escape and we should
		// immediately stop what we are doing. 
		if (m_pProgressMonitor && m_pProgressMonitor->UserAborted())
		{
			m_pErrorHandler->ThrowError(_R(IDW_CANCELLEDBMPIMPORT));	// Expects error set on cancel
		}
		// Work backwards through the DIB
		pCurrentScanLine -= WidthOfLine;
	}
//#endif
}


/********************************************************************************************

>	void JPEGImportFilter::OnImageCompletion()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Purpose:	Once an image has been read, this member should be called to complete
				any processing associated with that image.
	Errors:		Throws exceptions

	Notes:		Taken from IJG docs:

				After all the image data has been read, call jpeg_finish_decompress() to
				complete the decompression cycle.  This causes working memory associated
				with the JPEG object to be released.

				It is an error to call jpeg_finish_decompress() before reading the correct
				total number of scanlines.  If you wish to abort compression, call
				jpeg_abort() as discussed below.

				After completing a decompression cycle, you may dispose of the JPEG object
				as discussed next, or you may use it to decompress another image.  In that
				case return to step 2 or 3 as appropriate.  If you do not change the source
				manager, the next image will be read from the same source.

********************************************************************************************/
void JPEGImportFilter::OnImageCompletion()
{
	jpeg_finish_decompress(&m_cinfo);

/*
	Might be able to do this on GreyScale

	UINT32 Bpp = pWBitmap->GetBPP();
	if (TransColour != -1 && Bpp <= 8)
		pOilBitmap->SetTransparencyIndex(TransColour);
*/

	if (!SetBitmapResolution())
	{
		m_pErrorHandler->ThrowError( _R(IDS_JPEG_ERROR_UNSUPPORTED) );
	}

	SetLastBitmap();		// can only import one bitmap at the moment
}



/********************************************************************************************

>	virtual BOOL JPEGImportFilter::OnFilterCompletion()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Purpose:	Default constructor for a JPEGFilter object.

	Notes:		Taken from IJG docs:

				When you are done with a JPEG decompression object, destroy it by calling
				jpeg_destroy_decompress() or jpeg_destroy().  

********************************************************************************************/
BOOL JPEGImportFilter::OnFilterCompletion()
{
	if (m_pErrorHandler != NULL)
	{
		delete m_pErrorHandler;
		m_pErrorHandler = NULL;
		m_cinfo.err = NULL;
	}

	if (m_pSourceHandler != NULL)
	{
		delete m_pSourceHandler;
		m_pSourceHandler = NULL;
		m_cinfo.src = NULL;
	}

	if (m_pProgressMonitor != NULL)
	{
		delete m_pProgressMonitor;
		m_pProgressMonitor = NULL;
		m_cinfo.progress = NULL;
	}

	jpeg_destroy_decompress(&m_cinfo);

	m_pFile->SetThrowExceptions(m_bOldThrowExceptions);
	m_pFile->SetReportErrors(m_bOldReportErrors);

	// Start at the beginning of the file next time
	m_uStartOffset = 0;

	return TRUE;
}



/********************************************************************************************

>	BOOL JPEGImportFilter::InitErrorHandler()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Purpose:	Creates & initializes an error handler for this filter

********************************************************************************************/
BOOL JPEGImportFilter::InitErrorHandler()
{
	m_pErrorHandler = new JPEGErrorManager;
	if (m_pErrorHandler == NULL)
	{
		return FALSE;
	}

	m_cinfo.err = m_pErrorHandler->GetErrorMgrStruct();

	return TRUE;
}



/********************************************************************************************

>	BOOL JPEGImportFilter::InitFileHandler()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Purpose:	Creates & initializes a source data provider for this filter.

********************************************************************************************/
BOOL JPEGImportFilter::InitFileHandler()
{

	if (m_cinfo.src == NULL)
	{
		if (m_pSourceHandler == NULL)
		{
			m_pSourceHandler = new JPEGDataSource(m_pFile);
			if (m_pSourceHandler == NULL)
				return FALSE;
		}
		m_cinfo.src = m_pSourceHandler;
		if (!m_pSourceHandler->InitBuffer(&m_cinfo))
			return FALSE;
	}
	return TRUE;
}
		


/********************************************************************************************

>	BOOL JPEGImportFilter::InitProgressMonitor()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/08/96
	Purpose:	Creates & initializes the Progress bar for the filtering operation

********************************************************************************************/
BOOL JPEGImportFilter::InitProgressMonitor()
{
	PORTNOTETRACE("filters","JPEGImportFilter::InitProgressMonitor - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if (m_pFileForProgress == NULL)
	{
		m_pProgressMonitor = new JPEGProgressByFilter(&m_cinfo, m_pFilterForProgress, 
														m_uImportSize);
	}
	else
	{
		// We have to provide the progress bar
		String_64 string = GetImportProgressString(m_pFileForProgress, GetImportMsgID());
		m_pProgressMonitor = new JPEGProgressBySelf(&m_cinfo, string);
	}

	// Couldn't create one!!!
	if (m_pProgressMonitor == NULL)
	{
		return FALSE;
	}

	// Provide this ProgressMonitor to the IJG library
    m_cinfo.progress = m_pProgressMonitor;
#endif
	return TRUE;
}


