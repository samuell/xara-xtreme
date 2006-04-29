// $Id$
// Implementation of the controlling class of the v2 file format
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

#include "camtypes.h"

#include "cxfile.h"
#include "cxfrec.h"
#include "cxfrech.h"
#include "cxfdefs.h"
#include "cxfmap.h"

#include "ccfile.h"
//#include "progress.h"
//#include "filtrres.h"

#include "ccpanose.h"

#if !defined(EXCLUDE_FROM_XARLIB)
#include "bmpsrc.h"		// BitmapSource
#include "ccbuffil.h"
#endif

#include "cxftags.h"	// The tag definitions
#include "cxfdefs.h"	// The constants
#include "zutil.h"		// ZLIB_VERSIONNO

#if !defined(EXCLUDE_FROM_XARLIB)
#include "camfiltr.h"	// BaseCamelotFilter
#else
#include "rechcomp.h"
#endif

#include "hardwaremanager.h"
using namespace oilHardwareManager;


//-----------------------------------------------

CC_IMPLEMENT_DYNAMIC(CXaraFile,CCObject);
CC_IMPLEMENT_DYNAMIC(StandardDefaultRecordHandler,CamelotRecordHandler);
CC_IMPLEMENT_DYNAMIC(GeneralRecordHandler,CamelotRecordHandler);

#if !defined(EXCLUDE_FROM_XARLIB)
CC_IMPLEMENT_DYNAMIC(NULLXaraFile,CXaraFile);
#endif

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	CXaraFile::CXaraFile()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/96
	Purpose:	Default constructor
	SeeAlso:	-

********************************************************************************************/

CXaraFile::CXaraFile()
{
	pCCFile			= NULL;
	pRecord			= NULL;
	WriteToRecord	= FALSE;

	RecordNumber    = 0;

	NumBytesWritten		= 0;
	NumBytesRead		= 0;
	TotalNumBytesToRead	= 0;

	// the record handler list
	pRecordHandlerList = NULL;

	// The standard set of record handlers
	pDefaultRecordHandler				= NULL;
	pStandardDefaultRecordHandler		= NULL;
	pStripSubTreeRecordHandler			= NULL;
	pStandardStripSubTreeRecordHandler	= NULL;

	StartOfStreamedRecord = 0;
	WritingStreamedRecord = FALSE;
	CompOffDueToStreamedRecord = FALSE;

	pFilter = NULL;
	pMap	= NULL;

#if defined(EXCLUDE_FROM_XARLIB)
	pAtomicTagList = NULL;
	pEssentialTagList = NULL;
#endif
}


/********************************************************************************************

>	BOOL CXaraFile::OpenToWrite(CCLexFile* pThisCCFile)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/96
	Inputs:		pThisCCFile = ptr to the file to write to
	Returns:	TRUE if the the system is ready to write to the file
				FALSE otherwise
	Purpose:	Opens the CXaraFile object ready for writing.
				It uses the given CCFile object as the file to write to.

				NOTE:  It is assumed that the supplied CCFile has already been opened for writing.

	Errors:		-
	SeeAlso:	Close()

********************************************************************************************/

BOOL CXaraFile::OpenToWrite(CCLexFile* pThisCCFile)
{
	// Check entry params
	ERROR2IF(pThisCCFile == NULL,FALSE,"pThisCCFile is NULL");

	//  pCCFile should be NULL at this point
	ERROR3IF(pCCFile != NULL,"pThisCCFile is NULL");

	// Set our ptr the the CCFile to the one provided
	pCCFile = pThisCCFile;

	// Reset the write vars
	RecordNumber	= 0;
	NumBytesWritten = 0;

	BOOL ok = TRUE;

	// The first 8 bytes should be our unique ID sequence
	if (ok) ok = Write((UINT32)CXF_IDWORD1);
	if (ok) ok = Write((UINT32)CXF_IDWORD2);

	return ok;
}

/********************************************************************************************

>	BOOL CXaraFile::SetUpHandlers(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Inputs:		pFilter = ptr to associated filter
	Returns:	TRUE if the the system is ready to read from the file
				FALSE otherwise
	Purpose:	Sets up some record handlers that are needed for reading the file successfully

				Includes set up of the default record handler & the sub tree stripping handler
	Errors:		-
	SeeAlso:	Close()

********************************************************************************************/
#if !defined(EXCLUDE_FROM_XARLIB)
BOOL CXaraFile::SetUpHandlers(BaseCamelotFilter* pFilter)
{
	SetUpHandlerMap();

	BOOL ok = TRUE;

	// Set up the default handler, but only if there hasn't been one set up yet
	if (pDefaultRecordHandler == NULL)
	{
		if (pStandardDefaultRecordHandler == NULL)
			pStandardDefaultRecordHandler = new StandardDefaultRecordHandler;

		if (pStandardDefaultRecordHandler == NULL)
		{
			ERROR3("Unable to create StandardDefaultRecordHandler");
			return FALSE;
		}

		SetDefaultRecordHandler(pStandardDefaultRecordHandler);
	}

	// Init the default handler, and inform it we're about to start importing
	if (pDefaultRecordHandler != NULL)
	{
		if (ok) ok = pDefaultRecordHandler->Init(pFilter);
		if (ok) ok = pDefaultRecordHandler->BeginImport();
	}

	// Set up the sub tree stripper handler, but only if one has not been set yet
	if (pStripSubTreeRecordHandler == NULL)
	{
		if (pStandardStripSubTreeRecordHandler == NULL)
			pStandardStripSubTreeRecordHandler = new StripSubTreeRecordHandler;

		if (pStandardStripSubTreeRecordHandler == NULL)
		{
			ERROR3("Unable to create standard sub tree stripper");
			return FALSE;
		}

		SetStripSubTreeRecordHandler(pStandardStripSubTreeRecordHandler);
	}

	// Init the sub tree stripper handler, and inform it we're about to start importing
	if (pStripSubTreeRecordHandler != NULL)
	{
		if (ok) ok = pStripSubTreeRecordHandler->Init(pFilter);
		if (ok) ok = pStripSubTreeRecordHandler->BeginImport();
	}

	return ok;
}
#else

#define NEW_RECORD_HANDLER(HandlerClassName)				\
{															\
	CamelotRecordHandler* pCamelotRecordHandler;			\
	pCamelotRecordHandler = new HandlerClassName;			\
	if (pCamelotRecordHandler != NULL)						\
		pRecordHandlerList->AddTail(pCamelotRecordHandler);	\
	else													\
		return FALSE;										\
}															\

BOOL CXaraFile::SetUpHandlers(void)
{
	BOOL ok = TRUE;
	
	pRecordHandlerList = new List;
	if (!pRecordHandlerList)
		return(FALSE);
	NEW_RECORD_HANDLER(GeneralRecordHandler);		// The general record handler
	NEW_RECORD_HANDLER(CompressionRecordHandler);	// The compression record handler

	// Initialise all the handlers
	CamelotRecordHandler* pCamelotRecordHandler = (CamelotRecordHandler*)pRecordHandlerList->GetHead();
	while (ok && pCamelotRecordHandler != NULL)
	{
				ok = pCamelotRecordHandler->Init(this);
		if (ok) ok = pCamelotRecordHandler->BeginImport();

		pCamelotRecordHandler = (CamelotRecordHandler*)pRecordHandlerList->GetNext(pCamelotRecordHandler);
	}

	if (ok)
	{
		SetUpHandlerMap();

		// Set up the default handler, but only if there hasn't been one set up yet
		if (pDefaultRecordHandler == NULL)
		{
			if (pStandardDefaultRecordHandler == NULL)
				pStandardDefaultRecordHandler = new StandardDefaultRecordHandler;

			if (pStandardDefaultRecordHandler == NULL)
			{
				ERROR3("Unable to create StandardDefaultRecordHandler");
				return FALSE;
			}

			SetDefaultRecordHandler(pStandardDefaultRecordHandler);
		}

		// Init the default handler, and inform it we're about to start importing
		if (pDefaultRecordHandler != NULL)
		{
			if (ok) ok = pDefaultRecordHandler->Init(this);
			if (ok) ok = pDefaultRecordHandler->BeginImport();
		}

		// Set up the sub tree stripper handler, but only if one has not been set yet
		if (pStripSubTreeRecordHandler == NULL)
		{
			if (pStandardStripSubTreeRecordHandler == NULL)
				pStandardStripSubTreeRecordHandler = new StripSubTreeRecordHandler;

			if (pStandardStripSubTreeRecordHandler == NULL)
			{
				ERROR3("Unable to create standard sub tree stripper");
				return FALSE;
			}

			SetStripSubTreeRecordHandler(pStandardStripSubTreeRecordHandler);
		}

		// Init the sub tree stripper handler, and inform it we're about to start importing
		if (pStripSubTreeRecordHandler != NULL)
		{
			if (ok) ok = pStripSubTreeRecordHandler->Init(this);
			if (ok) ok = pStripSubTreeRecordHandler->BeginImport();
		}
	}

	return ok;
}

void CXaraFile::SetExternalRecordHandler(void* pMagic, RecordHandler* pfnRecordHandler)
{
	if (pStandardDefaultRecordHandler)
		((StandardDefaultRecordHandler*)pStandardDefaultRecordHandler)->SetExternalHandler(pMagic, pfnRecordHandler);
}

#endif


/********************************************************************************************

>	void CXaraFile::ResetHandlers()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Inputs:		-
	Returns:	-
	Purpose:	Resets the handlers set up via SetUpHandlers()

				All it does is delete any objects it had to create at set up time.
	Errors:		-
	SeeAlso:	Close()

********************************************************************************************/

void CXaraFile::ResetHandlers()
{
	// Delete the StandardDefaultRecordHandler
	if (pStandardDefaultRecordHandler != NULL)
	{
		delete pStandardDefaultRecordHandler;
		pStandardDefaultRecordHandler = NULL;
	}

	// Delete the StandardStripSubTreeRecordHandler 
	if (pStandardStripSubTreeRecordHandler != NULL)
	{
		delete pStandardStripSubTreeRecordHandler;
		pStandardStripSubTreeRecordHandler = NULL;
	}

	// delete the tag-to-handler map
	if (pMap != NULL)
	{
		delete pMap;
		pMap = NULL;
	}
}


/********************************************************************************************

>	void CXaraFile::SetUpHandlerMap()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/8/96
	Inputs:		-
	Returns:	-
	Purpose:	Sets up the tag to handler map so that LookUpHandler() can function.
	Errors:		-
	SeeAlso:	LookUpHandler()

********************************************************************************************/

void CXaraFile::SetUpHandlerMap()
{
	if (pMap != NULL)
		delete pMap;

	pMap = new CXaraFileMapTagToHandler;

	if (pMap != NULL && pRecordHandlerList != NULL)
	{
		CXaraFileRecordHandler* pHandler = (CXaraFileRecordHandler*) pRecordHandlerList->GetHead();
		while (pHandler != NULL)
		{
			UINT32* pTagList = pHandler->GetTagList();

			if (pTagList != NULL)
			{
				while (*pTagList != CXFRH_TAG_LIST_END)
				{
					pMap->Add(*pTagList,pHandler);
					pTagList++;
				}
			}

			pHandler = (CXaraFileRecordHandler*) pRecordHandlerList->GetNext(pHandler);
		}
	}
}

/********************************************************************************************

>	BOOL CXaraFile::OpenToRead(CCLexFile* pThisCCFile)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Inputs:		pThisCCFile = ptr to the file to read from
	Returns:	TRUE if the the system is ready to read from the file
				FALSE otherwise
	Purpose:	Opens the CXaraFile object ready for reading.
				It uses the given CCFile object as the file to read from.

				NOTE:	It is assumed that the supplied CCFile has already been opened for reading
						from the start of the file.	This function will return FALSE if the initial
						8 bytes do not contain the unique file header sequence.

				This also function sets the default record handler to the standard one, unless you
				have previously set the default handler with a call to SetDefaultRecordHandler().
				You can also set a default handler with SetDefaultRecordHandler() after this call.
				It's up to you, before or after, which
	Errors:		-
	SeeAlso:	Close()

********************************************************************************************/

BOOL CXaraFile::OpenToRead(CCLexFile* pThisCCFile)
{
	// Check entry params
	ERROR2IF(pThisCCFile == NULL,FALSE,"pThisCCFile is NULL");

	//  pCCFile should be NULL at this point
	ERROR3IF(pCCFile != NULL,"pThisCCFile is NULL");

	// Set our ptr the the CCFile to the one provided
	pCCFile = pThisCCFile;

	// Reset the read vars
	RecordNumber		= 0;
	NumBytesRead		= 0;
	TotalNumBytesToRead	= 0;

	BOOL ok = TRUE;
	UINT32 n;

	// The first 8 bytes should be our unique ID sequence
	if (ok) ok = Read(&n);
	if (ok) ok = (n == CXF_IDWORD1);
	if (ok) ok = Read(&n);
	if (ok) ok = (n == CXF_IDWORD2);

	return ok;
}

/********************************************************************************************

>	BOOL CXaraFile::Close()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/96
	Inputs:		-
	Returns:	TRUE if the the system closed down correctly
				FALSE otherwise
	Purpose:	This closes the CXaraFile object.
				You should call this func at the very end of reading from or writing to the file

				NOTE: The supplied pCCFile passed to OpenToWrite() and OpenToRead() is NOT
				closed.  The calling code will have to handle the low level closing of the file.

	Errors:		-
	SeeAlso:	OpenToWrite(), OpenToRead()

********************************************************************************************/

BOOL CXaraFile::Close()
{
	// Sanity Check 
	ERROR3IF(pCCFile == NULL,"pCCFile is NULL");

	// Reset all handlers that we set up via SetUpHandlers()
	ResetHandlers();

	return TRUE;
}

//--------------------
void CXaraFile::SetDefaultRecordHandler(CXaraFileRecordHandler* pHandler)
{
	if (pHandler != NULL)
		pDefaultRecordHandler = pHandler;
	else
		pDefaultRecordHandler = pStandardDefaultRecordHandler;

	ERROR3IF(pDefaultRecordHandler == NULL,"NULL handler ptr");
}

//--------------------
CXaraFileRecordHandler* CXaraFile::GetDefaultRecordHandler()
{
	return pDefaultRecordHandler;
}

//--------------------
BOOL StandardDefaultRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"NULL record ptr");

	return UnrecognisedTag(pCXaraFileRecord->GetTag());
}

//-------------------------------------------------------------------------------------------
void CXaraFile::SetStripSubTreeRecordHandler(StripSubTreeRecordHandler* pHandler)
{
	if (pHandler != NULL)
		pStripSubTreeRecordHandler = pHandler;
	else
		pStripSubTreeRecordHandler = pStandardStripSubTreeRecordHandler;

	ERROR3IF(pStripSubTreeRecordHandler == NULL,"NULL handler ptr");
}

//--------------------
StripSubTreeRecordHandler* CXaraFile::GetStripSubTreeRecordHandler()
{
	return pStripSubTreeRecordHandler;
}

//--------------------
void CXaraFile::StripNextSubTree()
{
	if (pStripSubTreeRecordHandler != NULL)
		pStripSubTreeRecordHandler->StripSubTreeOn();
}

/********************************************************************************************

>	BOOL CXaraFile::SetCompression(BOOL NewState)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/96
	Inputs:		The new compression state required, True for on, False for off
	Returns:	TRUE if ok
				FALSE otherwise
	Purpose:	Function to turn Compression on or off on the underlying CCFile.
				This is used during reading to change the state of the file compression.
	Errors:		-
	SeeAlso:	CompressionRecordHandler::HandleRecord(); BaseCamelotFilter::SetCompression();

********************************************************************************************/

BOOL CXaraFile::SetCompression(BOOL NewState)
{
	ERROR2IF(pCCFile == NULL,FALSE,"CXaraFile::SetCompression pCCFile is NULL");

	// This is used when reading to change the file compression state
	// It is called by the compression record handler when it is given the compression on/off
	// tags to handle.
	// The CCFile ZLib combination will read in the values
	BOOL ok = TRUE;
	if (NewState)
	{
		// If turning on then read the version number and check it
		UINT32 Version = 0L;
		Read(&Version);
		
		UINT32 MajorVersion = Version / 100;
		UINT32 MinorVersion = Version - MajorVersion * 100;
TRACEUSER( "Neville", _T("CXaraFile::SetCompression compression version %d.%d\n"),MajorVersion,MinorVersion);
		
		// Now tell CCFile to turn compression on
		ok = pCCFile->SetCompression(TRUE);
	}
	else
	{
TRACEUSER( "Neville", _T("CXaraFile::SetCompression off\n"));
		// Now tell CCFile to turn compression off
		// The Zlib/CCFile combination will automatically read in the CRC and uncompressed size
		// and check these.
		ok = pCCFile->SetCompression(FALSE);

		pCCFile->SetGoodState();
		FilePos Pos = pCCFile->tellIn();
TRACEUSER( "Neville", _T("CXaraFile::SetCompression pos = %d\n"),Pos);
	}

	return ok;
}

/********************************************************************************************

>	virtual BOOL CXaraFile::StartCompression()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/96
	Inputs:		-
	Returns:	TRUE if ok
				FALSE otherwise
	Purpose:	Function to turn Compression on when we are writing to the file. It asks the
				underlying CCFile and CXaraFile to start the compression process up.
				Will only send the record out if the preference option is on.
	Errors:		-
	SeeAlso:	StopCompression(); BaseCamelotFilter::StartCompression();

********************************************************************************************/

BOOL CXaraFile::StartCompression()
{
	ERROR2IF(pCCFile == NULL,FALSE,"CXaraFile::StartCompression pCCFile is NULL");
	
	// First check whether we are meant to be doing this or not
	// by looking at the filter preference
#if !defined(EXCLUDE_FROM_XARLIB)
	if (!BaseCamelotFilter::GetNativeCompression())
		return TRUE;
#endif

	if (pCCFile->IsCompressionSet())
	{
		// Just continue as if nothing has happened.
		//ERROR3("Trying to compress an already compressed stream!");
		return TRUE;
	}

	// Send out a start compression record
	CXaraFileRecord Rec(TAG_STARTCOMPRESSION, TAG_STARTCOMPRESSION_SIZE);

	BOOL ok = Rec.Init();

	// Save the version and type of compression all in one word or UINT32.
	// Top byte is the type of compression which is zero by default = zlib compression
	UINT32 Version = ZLIB_MAJOR_VERSIONNO * 100 + ZLIB_MINOR_VERSIONNO;
	Rec.WriteUINT32(Version);
	
	if (ok) ok = (Write(&Rec) != 0);
	
	// Now ask the CCFile to turn compression on as everything after this needs to be
	// compressed.
	// First, see if we have enough memory to start up the compression system
	if (ok) ok = pCCFile->InitCompression();
	
	// If this fails then don't try anything
	if (ok) ok = pCCFile->SetCompression(TRUE);
	
	return ok;
}

/********************************************************************************************

>	virtual BOOL CXaraFile::IsCompressionOn()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/96
	Inputs:		-
	Returns:	TRUE if compression is on
				FALSE if off
	Purpose:	This returns the compression state of the associated CCFile object.

				It returns the result of CCFile::IsCompressionSet()

	Errors:		-
	SeeAlso:	StartCompression(); BaseCamelotFilter::StopCompression();

********************************************************************************************/

BOOL CXaraFile::IsCompressionOn()
{
	ERROR2IF(pCCFile == NULL,FALSE,"pCCFile is NULL");

	return pCCFile->IsCompressionSet();
}


/********************************************************************************************

>	virtual BOOL CXaraFile::StopCompression()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/96
	Inputs:		-
	Returns:	TRUE if ok
				FALSE otherwise
	Purpose:	Function to turn Compression off when we are writing to the file. It asks the
				underlying CCFile and CXaraFile to stop the compression process.
				Will only send the record out if the preference option is on.
	Errors:		-
	SeeAlso:	StartCompression(); BaseCamelotFilter::StopCompression();

********************************************************************************************/

BOOL CXaraFile::StopCompression()
{
	ERROR2IF(pCCFile == NULL,FALSE,"CXaraFile::StopCompression pCCFile is NULL");

	// First check whether we are meant to be doing this or not
	// by looking at the filter preference
#if !defined(EXCLUDE_FROM_XARLIB)
	if (!BaseCamelotFilter::GetNativeCompression())
		return TRUE;
#endif

	if (!pCCFile->IsCompressionSet())
	{
		// Just continue as if nothing has happened
		//ERROR3("Trying to stop compression on an uncompressed stream!");
		return TRUE;
	}

	// Now send out the end compression record
	UINT32 RecordNumber = WriteRecordHeader(TAG_ENDCOMPRESSION, TAG_ENDCOMPRESSION_SIZE);
	if (RecordNumber < 0)
		return FALSE;

	// If we are turning compression off then just ask the CCFile to do this for us
	// This should write out the CRC and file size to the file, so we cannot use the
	// legal way of writing records, we have to use the special WriteHeader function above.
	BOOL ok = pCCFile->SetCompression(FALSE);

	return ok;
}

/********************************************************************************************

>	virtual CCLexFile* CXaraFile::GetCCFile() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/96
	Inputs:		-
	Returns:	The CCLexFile that is being used to export the data.
	Purpose:	Function to give public access to the underlying CCLexFile that is being used
				to save out the data.
				Note: only people who have a genuine reason need to access this - e.g. bitmap savers
	Errors:		-
	SeeAlso:	BitmapListComponent::SaveBitmapDefinition; BaseCamelotFilter::GetCCFile;

********************************************************************************************/

CCLexFile* CXaraFile::GetCCFile() const
{
	ERROR2IF(pCCFile == NULL,FALSE,"CXaraFile::GetCCFile pCCFile is NULL");
	return pCCFile;
}


/********************************************************************************************

>	UINT32 CXaraFile::GetCurrentRecordSize()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/96
	Inputs:		-
	Returns:	The size of the current record.
	Purpose:	Function to get the current size of the record that has been read in by the 
				CXaraFile and RecordHandlers and are currently processing. This can be used
				by streamed record handlers, such as bitmaps, which need to know the size of
				the recordso that they can update the progress bar size by a proportion of
				this amount on loading. (Not require on saving as the streamed record handlers
				can supply what value they like to the progress system and hence update with
				whatever value they require.
	SeeAlso:	BaseCamelotFilter::GetCurrentRecordSize();

********************************************************************************************/

UINT32 CXaraFile::GetCurrentRecordSize()
{
	return ReadSize;
}


/********************************************************************************************

>	BOOL CXaraFile::Read(UINT32* pUINT32)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Inputs:		pUINT32 = place to stuff a UINT32 in
	Returns:	TRUE if read successfully
				FALSE otherwise
	Purpose:	Reads in the give value type from the file
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFile::Read(UINT32* pUINT32)
{
	BOOL ok = Read((BYTE*)pUINT32,sizeof(UINT32));
	*pUINT32 = LEtoNative(*pUINT32);
	return ok;
}

BOOL CXaraFile::Read(FLOAT* pf)
{
	return (Read((BYTE*)pf,sizeof(FLOAT)));
}

BOOL CXaraFile::Read(double* pd)
{
	return (Read((BYTE*)pd,sizeof(double)));
}

BOOL CXaraFile::ReadWCHAR(WCHAR *pw)
{
	BOOL ok = Read((BYTE*)pw, SIZEOF_XAR_UTF16);	// sizeof(WCHAR));
	*pw = LEtoNative(*pw);
	*pw = UTF16ToNative(*pw);
	return ok;
}

/********************************************************************************************

>	BOOL CXaraFile::Read(BYTE* pBuf,UINT32 Size)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Inputs:		pBuf = ptr to a buffer
				Size = size of the buffer in bytes
	Returns:	TRUE if read successfully
				FALSE otherwise
	Purpose:	Reads in the give value type from the file
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFile::Read(BYTE* pBuf,UINT32 Size)
{
	ERROR3IF(pBuf == NULL,"pBuf is NULL");

	BOOL ok = (pBuf != NULL);

//	for (UINT32 i=0;ok && i< Size;i++)
//		ok = Read(pBuf+i);

	pCCFile->read((void*)pBuf,Size);
	IncNumBytesRead(Size);

	// AMB commented out eof() line - it seems wrong as reading the last bytes in a file
	// will otherwise fail
	ok = (!pCCFile->bad() && !pCCFile->fail() /*&& !pCCFile->eof()*/);

	return ok;
}

/********************************************************************************************

>	BOOL CXaraFile::Read(BYTE* pBYTE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Inputs:		pUINT32 = place to stuff a UINT32 in
	Returns:	TRUE if read successfully
				FALSE otherwise
	Purpose:	Reads in the give value type from the file
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFile::Read(BYTE* pBYTE)
{
	ERROR3IF(pBYTE == NULL,"pBYTE is NULL");
	ERROR2IF(pCCFile == NULL,FALSE,"Can't write a BYTE with no pCCFile");

	if (pBYTE == NULL)
		return FALSE;

	pCCFile->read((void*)pBYTE);
	IncNumBytesRead(1);

	return (!pCCFile->bad() && !pCCFile->fail() && !pCCFile->eof());
}

/********************************************************************************************

>	BOOL CXaraFile::Read(TCHAR* pStr,UINT32 MaxChars)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/6/96
	Inputs:		MaxChars = maximum number of characters that this string can take
	Outputs:	pStr = String to put the data into
	Returns:	TRUE if read successfully
				FALSE otherwise
	Purpose:	Reads in the given string from the file
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFile::Read(TCHAR* pStr,UINT32 MaxChars)
{
	return ReadUnicode(pStr,MaxChars);
}

/********************************************************************************************

>	BOOL CXaraFile::Read(TCHAR* pStr,UINT32 MaxChars)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/6/96
	Inputs:		MaxChars = maximum number of characters that this string can take
	Outputs:	pStr = String to put the data into
	Returns:	TRUE if read successfully
				FALSE otherwise
	Purpose:	Reads in the given string from the file
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFile::ReadUnicode(TCHAR* pStr,UINT32 MaxChars)
{
	ERROR3IF(pStr == NULL,"pStr is NULL");
	ERROR2IF(pCCFile == NULL,FALSE,"Can't read a Unicode String with no pCCFile");

	if (pStr == NULL)
		return FALSE;

	BOOL ok = TRUE;

#ifdef UNICODE
	if (MaxChars==0)
		return FALSE;

	WCHAR* pBuf = pStr;
	WCHAR* pBufEnd = pBuf+MaxChars;				// Pointer arithmetic!
	WCHAR c = 0;

	do
	{
		ok = ReadWCHAR(&c);						// Read two bytes into the WCHAR buffer
		if (!ok) c = 0;							// If the read failed then write a terminator
		*pBuf++ = c;							// Add the character to the string
	}
	while (c!=0 && pBuf<pBufEnd);// Until end of string or out of room in buffer

#else
	BYTE* pBuf = (BYTE*)pStr;
	BYTE* pBufEnd = pBuf+MaxChars;
	BYTE b1 = 1;
	BYTE b2 = 1;

	while (ok && !(b1 == 0 && b2 == 0))
	{
		ok = Read(&b1) && Read(&b2);

		if (ok) ok = (pBuf < pBufEnd);
		if (ok) *pBuf++ = b1;
	}
#endif

	return ok;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/********************************************************************************************

>	BOOL CXaraFile::Write(UINT32 n)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/96
	Inputs:		n = UINT32 to write to
	Returns:	TRUE if written successfully
				FALSE otherwise
	Purpose:	Writes out the given value to the file
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFile::Write(UINT32 n)
{
	UINT32 t = NativetoLE(n);
	return (Write((BYTE*)&t,sizeof(UINT32)));
}

BOOL CXaraFile::Write(INT32 n)
{
	INT32 t = NativetoLE(n);
	return (Write((BYTE*)&t,sizeof(INT32)));
}

BOOL CXaraFile::Write(FLOAT f)
{
	return (Write((BYTE*)&f,sizeof(FLOAT)));
}

BOOL CXaraFile::Write(double d)
{
	return (Write((BYTE*)&d,sizeof(double)));
}

BOOL CXaraFile::WriteWCHAR(WCHAR w)
{
	WCHAR t = NativeToUTF16(w);
	t = NativetoLE(t);
	return (Write((BYTE*)&t, SIZEOF_XAR_UTF16));		//sizeof(WCHAR)));
}

BOOL CXaraFile::WriteCCPanose(const CCPanose & MyCCPanose)
{
	BOOL ok = TRUE;

	if (ok) ok = Write(MyCCPanose.GetFamilyType());
	if (ok) ok = Write(MyCCPanose.GetSerifStyle());
	if (ok) ok = Write(MyCCPanose.GetWeight());
	if (ok) ok = Write(MyCCPanose.GetProportion());
	if (ok) ok = Write(MyCCPanose.GetContrast());
	if (ok) ok = Write(MyCCPanose.GetStrokeVariation());
	if (ok) ok = Write(MyCCPanose.GetArmStyle());
	if (ok) ok = Write(MyCCPanose.GetLetterform());
	if (ok) ok = Write(MyCCPanose.GetMidline());
	if (ok) ok = Write(MyCCPanose.GetXHeight());

	return ok;
}

/********************************************************************************************

>	BOOL CXaraFile::Write(TCHAR* pStr)
	BOOL CXaraFile::WriteUnicode(TCHAR* pStr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/96
	Inputs:		pStr = ptr to zero-terminated string
	Returns:	TRUE if written successfully
				FALSE otherwise
	Purpose:	Writes out the string as a Unicode string (UTF16)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFile::Write(TCHAR* pStr)
{
	return WriteUnicode(pStr);
}

BOOL CXaraFile::WriteUnicode(TCHAR* pStr)
{
	ERROR3IF(pStr == NULL,"NULL pStr");
	if (pStr == NULL)
		return FALSE;

#ifdef UNICODE
	// pStr points to a Unicode string, so just write it out
	//
	// We must cope with byte-order differences between native storage and XAR file storage:
	// Native may be big-endian or little-endian, XAR is always little-endian
	// Native WCHAR may be 16 or 32 bits, XAR is always 16 bits
	// These differences are handled in WriteWCHAR
	BOOL ok = TRUE;
	WCHAR c = 0;
	INT32 i = 0;

	do
	{
		c = pStr[i++];
		ok = WriteWCHAR(c);						// Read two bytes into the WCHAR buffer
		if (!ok) c = 0;							// If the read failed then write a terminator
	}
	while (c!=0);// Until end of string or no longer OK to write

	return ok;									// If we terminated due to Read failure tell the caller

#else
	// pStr points to an ASCII string, and we want it written as a Unicode string
	// Write out each char, followed by a 0 byte

	size_t len = camStrlen(pStr);

	BOOL ok= TRUE;
	for (unsigned i=0;ok && i<=len;i++)
		ok = Write(BYTE(pStr[i])) && Write(BYTE(0));

	return ok;
#endif
}

/********************************************************************************************

>	BOOL CXaraFile::WriteASCII(TCHAR* pStr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/96
	Inputs:		pStr = ptr to zero-terminated string
	Returns:	TRUE if written successfully
				FALSE otherwise
	Purpose:	Writes out the string as an ASCII string
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFile::WriteASCII(TCHAR* pStr)
{
	ERROR3IF(pStr == NULL,"NULL pStr");
	if (pStr == NULL)
		return FALSE;

	UINT32 len = camStrlen(pStr);

#ifdef UNICODE
	// Writing a Unicode string at pStr as an ASCII string
	// Just write out the first byte of each unicode char.

	BOOL ok= TRUE;
	for (UINT32 i=0;ok && i<=len;i++)
		ok = Write(BYTE(pStr[i*2]));

	return ok;
#else
	// pStr points at an ASCII string, so write it out
	return (Write((BYTE*)pStr,len+1));
#endif
}


/********************************************************************************************

>	BOOL CXaraFile::Write(const DocCoord& Coord)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/96
	Inputs:		Coord = a coord
	Returns:	TRUE if written successfully
				FALSE otherwise
	Purpose:	Writes out the coord
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFile::Write(const DocCoord& Coord)
{
	return (Write(Coord.x) && Write(Coord.y));
}

#if !defined(EXCLUDE_FROM_XARLIB)
/********************************************************************************************

>	BOOL CXaraFile::WriteBitmapSource(const BitmapSource& Source, UINT32 Height, 
										BaseCamelotFilter* pThisFilter)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/08/96
	Inputs:		Source : the BitmapSource to write out
				Height : the height of the bitmap in this source
				pThisFilter : the filter whose IncProgressBarCount we will use for the
				progress indicator
	Returns:	TRUE if written successfully
				FALSE otherwise
	Purpose:	Writes out the given BitmapSource to this file.

	Errors:		ERROR2's if Height is zero or pThisFilter NULL

********************************************************************************************/

BOOL CXaraFile::WriteBitmapSource(const BitmapSource& Source, UINT32 Height, 
									BaseCamelotFilter* pThisFilter)
{
	ERROR2IF(pThisFilter == NULL, FALSE, "pThisFilter == NULL");
	ERROR2IF(Height == 0, FALSE, "Height == 0");

	CCBufferFile* pBufferFile = new CCBufferFile(GetCCFile());
	// Now create a file that can fill it
	if (pBufferFile == NULL || !pBufferFile->IsInited())
	{
		return FALSE;
	}
	// and provide the filter with something to work with
	Source.AttachToBufferFile(pBufferFile);

	// Work out how much to increment the progress bar by on each loop
	OFFSET Size = Source.GetSize();
	UINT32 Increment = (Height * CCBufferFile::DEFAULT_BUFSIZ) / Size;
	if (Increment > Height)
	{
		Increment = Height;
	}
	else if (Increment < 1)
	{
		Increment = 1;
	}
	UINT32 UpdateEvery = Size / (Height * CCBufferFile::DEFAULT_BUFSIZ);

	// Write out the BitmapSource DEFAULT_BUFSIZ bytes at a time
	UINT32 WritesSinceUpdate = 0;
	while (!pBufferFile->IsAllWritten())
	{
		pBufferFile->write(NULL, CCBufferFile::DEFAULT_BUFSIZ);
		if (pBufferFile->bad() || pBufferFile->fail())
		{
			delete pBufferFile;
			return FALSE;
		}

		// We've made some progress so provide some user feedback
		++WritesSinceUpdate;
		if (WritesSinceUpdate >= UpdateEvery)
		{
			pThisFilter->IncProgressBarCount(Increment);
			WritesSinceUpdate = 0;
		}
	};

	// Destroy our file handle
	delete pBufferFile;

	return TRUE;
}
#endif

/********************************************************************************************

>	BOOL CXaraFile::Write(BYTE* pBuf,UINT32 Size)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/96
	Inputs:		pBuf = ptr to a BYTE buffer
				Size = Number of bytes in pBuf
	Returns:	TRUE if written successfully
				FALSE otherwise
	Purpose:	Writes out the given buffer of bytes to the file.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFile::Write( BYTE* pBuf, UINT32 Size )
{
	ERROR3IF(pBuf == NULL && Size > 0,"NULL pBuf param yet Size > 0");

	BOOL ok = !(pBuf == NULL && Size > 0);

	for (UINT32 i=0;i<Size && ok;i++)
		ok = Write(pBuf[i]);

	return ok;
}

/********************************************************************************************

>	virtual BOOL CXaraFile::Write(BYTE b)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/96
	Inputs:		b = BYTE to write to
	Returns:	TRUE if written successfully
				FALSE otherwise
	Purpose:	Writes out the given value to the file
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFile::Write(BYTE b)
{
	ERROR2IF(pCCFile == NULL,FALSE,"Can't write a BYTE with no pCCFile");

	BOOL ok = TRUE;

	if (WriteToRecord && pRecord != NULL)
		ok = pRecord->WriteBYTE(b);
	else
	{
		pCCFile->write((void const*)&b);
		IncNumBytesWritten(1);
	}

	return ok;
}

/********************************************************************************************

>	INT32 CXaraFile::StartRecord(UINT32 Tag,INT32 Size)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		Tag = the tag for the record
				Size = the size of the data section of the record in bytes, or CXF_UNKNOWN_SIZE
	Returns:	The record number of the record being written to the file.
				If the function fails, 0 will be returned
	Purpose:	Prepares a record ready for writing to the file.

				This function prepares a record that is buffered in memory before being
				written out to file.  
				
				After this call all Write() functions write to the data section of the record.
				The complete record (inc. the tag & size fields) is written out when EndRecord() is called.

				If you know the exact size in bytes of the record you are writing, supply this
				value via the Size parameter.  This allows the write functions to provide additional
				error checking for you (e.g. if you try and write extra bytes)
				
				If you are not sure of the exact number of bytes that will be written to the data
				section, set the Size param to CXF_UNKNOWN_SIZE.  EndRecord() will ensure that the
				written record will have its size field set to the number of bytes that were written.

	Errors:		-
	SeeAlso:	EndRecord()

********************************************************************************************/

INT32 CXaraFile::StartRecord(UINT32 Tag,INT32 Size)
{
	if (EndRecord())
	{
		// Inc the record number
		RecordNumber++;

		// Create a new record, & initialise it
		pRecord = new CXaraFileRecord(Tag,Size);

		WriteToRecord = (pRecord != NULL && pRecord->Init());

		if (WriteToRecord)
		{
			pRecord->SetRecordNumber(RecordNumber);
			return RecordNumber;
		}
	}

	return 0;
}

INT32 CXaraFile::StartStreamedRecord(UINT32 Tag,INT32 Size)
{
	ERROR2IF(WritingStreamedRecord,0,"Already writing a streamed record.  Forgot to call EndRecord()?");

	WritingStreamedRecord = TRUE;
	WriteToRecord = FALSE;

	// Make sure compression off before starting the streamed record.
	// If it was turned on, than make sure we know that the writing of the streamed record 
	// caused it to be turned off.
	CompOffDueToStreamedRecord = IsCompressionOn();
	BOOL ok = StopCompression();
	
	// Must do this after turning compression off as otherwise CCFile will complain
	// as FilePos in the compressed stream is an unknown entity
	StartOfStreamedRecord = GetFilePos();
	
	if (ok) ok = Write(Tag);
	if (ok) ok = Write(Size);

	if (ok)
	{
		RecordNumber++;
		return RecordNumber;
	}

	return 0;
}


/********************************************************************************************

>	BOOL CXaraFile::EndRecord()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		-
	Returns:	TRUE if the the system wrote out the record
				FALSE otherwise
	Purpose:	Ends the current record.

				If the record was started by StartRecord(), this call will write out the entire
				buffered record (record header and data section) to the file.

	Errors:		-
	SeeAlso:	StartRecord()

********************************************************************************************/

BOOL CXaraFile::EndRecord()
{
	ERROR3IF(WriteToRecord && pRecord == NULL,"Writing to a NULL record!");

	BOOL ok = TRUE;

	if (WritingStreamedRecord)
	{
		// Not interested in the result, but we will pass through a variable anyway
		// Complete the process of writing out the streamed record
		UINT32 RecordSize = 0L;
		return EndStreamedRecord(&RecordSize);
	}

	if (WriteToRecord && pRecord != NULL)
	{
		WriteToRecord = FALSE;

		ERROR3IF(!pRecord->IsDataSectionFull(),"Data section of the record has not been filled");

		if (ok) ok = Write(pRecord->GetTag());
		if (ok) ok = Write(pRecord->GetSize());
		if (ok) ok = Write(pRecord->GetBuffer(),pRecord->GetSize());

		delete pRecord;
		pRecord = NULL;
	}

	return (ok);
}

/********************************************************************************************

>	BOOL CXaraFile::EndStreamedRecord(UINT32 *RecordSize)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/8/96
	Inputs:		-
	Outputs:	RecordSize - the size of the record just written
	Returns:	TRUE if the the system wrote out the record
				FALSE otherwise
	Purpose:	Ends the current streamed record.

				If the record was started by StartStreamedRecord(), this call will do all
				that is necessary to clean up the streamed record.
				This mainly involves:-
					Fixing the size in the record header to be correct.
					Starting up compression, if required

	Errors:		-
	SeeAlso:	StartRecord()

********************************************************************************************/

BOOL CXaraFile::EndStreamedRecord(UINT32 *RecordSize)
{
	ERROR3IF(!WritingStreamedRecord,"Trying to end a non-streamed record!");
	if (!WritingStreamedRecord)
		return FALSE;

	BOOL ok = TRUE;
	// We must reset WritingStreamedRecord to FALSE immediately, to prevent stack overflow
	// due to recursion via the StartCompression() call below
	WritingStreamedRecord = FALSE;

	// Use our vitrual function to fix the header up with the correct info
	// Null filter overides this to do nothing
	UINT32 SizeOfRecord = 0L;
	ok = FixStreamedRecordHeader(&SizeOfRecord);
	// If we are passed a pointer then return the size to the caller
	if (RecordSize)
		*RecordSize = SizeOfRecord;
	// Add this number to the number of bytes written
	TRACEUSER( "Neville", _T("EndStreamedRecord update size by %d\n"),SizeOfRecord);
	IncNumBytesWritten(SizeOfRecord);
	
	// If compression was turned off due to the streamed record, make sure we turn it back on again
	if (ok && CompOffDueToStreamedRecord)
		ok = StartCompression();

	// Reset the streamed record vars
	StartOfStreamedRecord = 0;
	CompOffDueToStreamedRecord = FALSE;
		
	return ok;
}

/********************************************************************************************

>	virtual BOOL CXaraFile::FixStreamedRecordHeader(UINT32 *RecordSize)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/6/96
	Inputs:		-
	Outputs:	RecordSize - the size of the record just written
	Returns:	True if worked ok, False otherwise.
	Purpose:	Fixes up the previously saved record header for a streamed record.
				The RecordSize is the size of the data section of the record and so does not
				include the 8 bytes in the header.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFile::FixStreamedRecordHeader(UINT32 *RecordSize)
{
	ERROR2IF(pCCFile == NULL,FALSE,"CXaraFile::FixStreamedRecordHeader NULL pCCFile");
	ERROR2IF(StartOfStreamedRecord == 0,FALSE,"CXaraFile::FixStreamedRecordHeader StartOfStreamedRecord == 0");

	// Note the current position in the file
	FilePos Pos = GetFilePos();
	// Work out the record size from it and the previosuly stored start of record position
	UINT32 Size = Pos-StartOfStreamedRecord-8;
	UINT32 tSize = NativetoLE(Size);

	// Seek back to the size field in the record header
	pCCFile->seek(StartOfStreamedRecord+4);

	// Fix the size to be correct
	if (pCCFile->write(&tSize, 4).fail())
		return FALSE;

	// Now get back to where we were in the file
	pCCFile->seek(Pos);

	// If we are passed a pointer then return the size to the caller
	if (RecordSize)
		*RecordSize = Size;

	return TRUE;
}


/********************************************************************************************

>	UINT32 CXaraFile::Write(CXaraFileRecord* pRecord)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/96
	Inputs:		pRecord = ptr to a record to write out to file
	Returns:	The record number of this record in the file.
				If an error occurs, 0 is returned
	Purpose:	Writes out the record to the file
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 CXaraFile::Write(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL,FALSE,"pRecord param is NULL");

	BOOL ok = EndRecord();

	ERROR3IF(!pRecord->IsDataSectionFull(),"Data section of the record has not been filled");

	if (ok) ok = Write(pRecord->GetTag());
	if (ok) ok = Write(pRecord->GetSize());
	if (ok) ok = Write(pRecord->GetBuffer(),pRecord->GetSize());

	// Inc the record number
	RecordNumber++;

	if (ok)
		return RecordNumber;
	else
		return 0;
}

/********************************************************************************************

>	UINT32 CXaraFile::WriteDefinitionRecord(CXaraFileRecord* pRecord)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/07/97
	Inputs:		pRecord = ptr to a record to write out to file
	Returns:	The record number of this record in the file.
				If an error occurs, 0 is returned
	Purpose:	Writes out the record to the file
				Override this if your filter needs to keep track of definition records

********************************************************************************************/

UINT32 CXaraFile::WriteDefinitionRecord(CXaraFileRecord* pRecord)
{
	// This base class just calls the standard Write function
	return(Write(pRecord));
}



/********************************************************************************************

>	UINT32 CXaraFile::WriteRecordHeader(UINT32 Tag, INT32 Size)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/96
	Inputs:		Tag		- the tag for this record
				Size	- the size of the record
	Returns:	The record number of this record in the file.
				If an error occurs, 0 is returned
	Purpose:	Writes out a record header directly to the file.
				It expects the caller to handle other aspects of the record writing.
				This should only be used by people who know what they are doing e.g. compression
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 CXaraFile::WriteRecordHeader(UINT32 Tag, INT32 Size)
{
	BOOL ok = TRUE;

	// Make sure we write directly to file
	WriteToRecord = FALSE;

	ok = Write(Tag);
	if (ok) ok = Write(Size);

	// Inc the record number
	RecordNumber++;

	if (ok)
		return RecordNumber;
	else
		return 0;
}

/********************************************************************************************

>	FilePos CXaraFile::GetFilePos()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/6/96
	Inputs:		-
	Returns:	The current file pos
	Purpose:	Gets the current file pos of the associated CCFile object

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

FilePos CXaraFile::GetFilePos()
{
	ERROR2IF(pCCFile == NULL,0,"No pCCFile ptr");
	return pCCFile->tell();
}

/********************************************************************************************

>	UINT32 CXaraFile::GetRecordTag()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		-
	Returns:	The tag value of the current record, or 0xffffffff if it fails
	Purpose:	Gets the tag value.

				You can only call this after StartRecord() and before EndRecord()

	Errors:		-
	SeeAlso:	StartRecord(), EndRecord()

********************************************************************************************/

UINT32 CXaraFile::GetRecordTag()
{
	ERROR3IF(!WriteToRecord,"Can't get tag - no current record");
	ERROR3IF(WriteToRecord && pRecord == NULL,"current record is NULL");

	if (!WriteToRecord || (pRecord == NULL))
		return UINT32(-1);

	return pRecord->GetTag();
}

/********************************************************************************************

>	INT32 CXaraFile::GetRecordNum()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		-
	Returns:	The record number for the current record, or 0 if no current record
	Purpose:	Gets the current record's record number

				You can only call this after StartRecord() and before EndRecord()

				The record number defines the position of the record in the file.
				Record number 1 means the first record in the file, 2 the second, etc.

	Errors:		-
	SeeAlso:	StartRecord(), EndRecord()

********************************************************************************************/

INT32 CXaraFile::GetRecordNum()
{
	ERROR3IF(!WriteToRecord,"Can't get record num - no current record");

	if (!WriteToRecord)
		return 0;

	return RecordNumber; 
}

/********************************************************************************************

>	virtual void CXaraFile::GotError(UINT32 errorID)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/96
	Inputs:		errorID = ID of error resource string
	Returns:	-
	Purpose:	Sets an error using the given resource string
				All it does is call the mirror function in the attached CCFile.
	SeeAlso:	BaseCamelotFilter::PrepareToExport; BaseCamelotFilter::DoExport
	Scope: 		Protected

********************************************************************************************/

void CXaraFile::GotError(UINT32 errorID)
{
	if (pCCFile != NULL)
		pCCFile->GotError(errorID);
}

/********************************************************************************************

>	virtual void CXaraFile::GotError( UINT32 errorID , const TCHAR* errorString)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Inputs:		errorID = ID of error resource string
				errorString = ptr to the error string.
	Returns:	-
	Purpose:	Sets an error using the given resource string
				All it does is call the mirror function in the attached CCFile.
	SeeAlso:	BaseCamelotFilter::PrepareToExport; BaseCamelotFilter::DoExport
	Scope: 		Protected

********************************************************************************************/

void CXaraFile::GotError(UINT32 errorID, const TCHAR* errorString)
{
	if (pCCFile != NULL)
		pCCFile->GotError(errorID,errorString);
}


/********************************************************************************************

>	BOOL CXaraFile::ReadNextRecordHeader()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Inputs:		-
	Returns:	-
	Purpose:	Reads in the header (i.e. tag & size fields) of the next record in the file
	SeeAlso:	ReadNextRecord()
	Scope: 		-

********************************************************************************************/

BOOL CXaraFile::ReadNextRecordHeader()
{
	BOOL ok = TRUE;

	if (ok) ok = Read(&ReadTag);
	if (ok) ok = Read(&ReadSize);
	if (ok) RecordNumber++;

	return ok;
}

/********************************************************************************************

>	BOOL CXaraFile::RegisterRecordHandler(CXaraFileRecordHandler* pCXaraFileRecordHandler)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Inputs:		pCXaraFileRecordHandler = ptr 
	Returns:	-
	Purpose:	Finds the record handler that's been registered to handle a record with the given tag value
	SeeAlso:	ReadNextRecord()
	Scope: 		-

********************************************************************************************/

BOOL CXaraFile::RegisterRecordHandlers(List* pListOfHandlers)
{
	ERROR2IF(pListOfHandlers == NULL, FALSE,"pListOfHandlers is NULL");
	pRecordHandlerList = pListOfHandlers;
	return TRUE;
}



/********************************************************************************************

>	CXaraFileRecordHandler* CXaraFile::LookUpHandler(UINT32 Tag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/8/96
	Inputs:		Tag = the tag value of a record
	Returns:	ptr to the record handler (or NULL if none could be found)
	Purpose:	This looks up the tag in the tag-to-handler map

				If it's not in the map (or there isn't a map) NULL is returned.

	SeeAlso:	FindHandler()
	Scope: 		-

********************************************************************************************/

CXaraFileRecordHandler* CXaraFile::LookUpHandler(UINT32 Tag)
{
	CXaraFileRecordHandler* pHandler = NULL;

	if (pMap != NULL)
		pHandler = pMap->LookUp(Tag);

	return pHandler;
}

/********************************************************************************************

>	CXaraFileRecordHandler* CXaraFile::SearchForHandler(UINT32 Tag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Inputs:		Tag = the tag value of a record
	Returns:	ptr to the record handler (or NULL if none could be found)
	Purpose:	Searches through the list of registered record handlers for a handler
				for the given tag value

				This will scan the tag list of every registered handler, until the right handler is found.

				If one can't be found, a ptr to the default record handler is returned (if there is one).

	SeeAlso:	FindHandler()
	Scope: 		-

********************************************************************************************/

CXaraFileRecordHandler* CXaraFile::SearchForHandler(UINT32 Tag)
{
	if (pRecordHandlerList != NULL)
	{
		CXaraFileRecordHandler* pHandler = (CXaraFileRecordHandler*) pRecordHandlerList->GetHead();
		while (pHandler != NULL)
		{
			if (pHandler->IsTagInList(Tag))
				return pHandler;

			pHandler = (CXaraFileRecordHandler*) pRecordHandlerList->GetNext(pHandler);
		}
	}

	return NULL;
}

/********************************************************************************************

>	CXaraFileRecordHandler* CXaraFile::FindHandler(UINT32 Tag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Inputs:		Tag = the tag value of a record
	Returns:	ptr to the record handler (or NULL if none could be found)
	Purpose:	Finds the record handler that's been registered to handle a record with the given tag value
	SeeAlso:	ReadNextRecord()
	Scope: 		-

********************************************************************************************/

CXaraFileRecordHandler* CXaraFile::FindHandler(UINT32 Tag)
{
	ERROR3IF(pRecordHandlerList == NULL && GetDefaultRecordHandler()==NULL,"pRecordHandlerList is NULL & there's no default handler");

	CXaraFileRecordHandler* pHandler = NULL;
	
	// Does the sub tree stripper want to process the tag?
	pHandler = GetStripSubTreeRecordHandler();
	if (pHandler != NULL)
	{
		if (pHandler->IsTagInList(Tag))
			return pHandler;
	}

	// See if handler is in the map (i.e. do the quick method first)
	pHandler = LookUpHandler(Tag);
	if (pHandler != NULL)
		return pHandler;

	// Search the handler list for a suitable handler
	pHandler = SearchForHandler(Tag);
	if (pHandler != NULL)
		return pHandler;

	// If all else fails, fall through to the default record handler.
	return GetDefaultRecordHandler();
}

/********************************************************************************************

>	BOOL CXaraFile::ReadNextRecord()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Inputs:		-
	Returns:	-
	Purpose:	Reads in the next record from the file
	SeeAlso:	BaseCamelotFilter::ReadFile
	Scope: 		-

********************************************************************************************/

BOOL CXaraFile::ReadNextRecord()
{
	ERROR2IF(pCCFile == NULL,FALSE,"NULL pCCFile ptr");

	BOOL ok = FALSE;

	if (ReadNextRecordHeader())
	{
		CXaraFileRecordHandler* pCXaraFileRecordHandler = FindHandler(ReadTag);
		if (pCXaraFileRecordHandler != NULL)
		{
			// Only if it is not a streamed record do we want to reda in the data into a buffer
			if (pCXaraFileRecordHandler->IsStreamed(ReadTag))
			{
				// Its a streamed record so just ask the handler to read it.
				ok = pCXaraFileRecordHandler->HandleStreamedRecord(this,ReadTag,ReadSize,RecordNumber);
			}
			else
			{
				// It is not a streamed record so read it in and ask the handler to
				// deal with it
				CXaraFileRecord* pCXaraFileRecord = new CamelotFileRecord(pFilter,ReadTag,ReadSize);

				if (pCXaraFileRecord != NULL)
				{
					ok = pCXaraFileRecord->Init();
					pCXaraFileRecord->SetRecordNumber(RecordNumber);

					// Read data section, if there is one
					if (ok && ReadSize > 0)
					{
						BYTE* pBuf = pCXaraFileRecord->GetBuffer();
						ok = (pBuf != NULL);
						if (ok) ok = Read(pBuf,ReadSize);
					}

					if (ok) ok = pCXaraFileRecordHandler->HandleRecord(pCXaraFileRecord);
#if !defined(EXCLUDE_FROM_XARLIB)
					if (ok)		 pCXaraFileRecordHandler->IncProgressBarCount(pCXaraFileRecord->GetSize()+8);
#endif
					delete pCXaraFileRecord;
				}
			}

#if !defined(EXCLUDE_FROM_XARLIB)
			if (ok && pFilter != NULL) ok = pFilter->SetLastRecordHandler(pCXaraFileRecordHandler,ReadTag);
#endif
		}
		else
		{
			// No handler for this tag
			//TRACEUSER( "Markn", _T("Unhandled tag value : %d\n"),ReadTag);

			// Read the rest of this record, and send the data to oblivion
			ok = TRUE;
			BYTE b;
			for (UINT32 i=0;ok && i<ReadSize;i++)
				ok = Read(&b);
		}
	}

//TRACE(_T("ReadNextTag %d %d\n"), ReadTag, ok);


	return ok;
}


#if defined(EXCLUDE_FROM_XARLIB)
/********************************************************************************************

>	virtual void CXaraFile::AddAtomicTag(AtomicTagListItem* pItem)

	Author:		Markn
	Created:	16/8/96
	Inputs:		pItem = ptr to an atomic list item
	Returns:	-
	Purpose:	Adds the item to the list of atomic tags compiled during import

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

void CXaraFile::AddAtomicTag(AtomicTagListItem* pItem)
{
	if (pAtomicTagList == NULL)
		pAtomicTagList = new AtomicTagList;

	if (pAtomicTagList != NULL)
		pAtomicTagList->AddTail(pItem);
}

/********************************************************************************************

>	virtual void CXaraFile::AddEssentialTag(EssentialTagListItem* pItem)

	Author:		Markn
	Created:	16/8/96
	Inputs:		pItem = ptr to an Essential list item
	Returns:	-
	Purpose:	Adds the item to the list of Essential tags compiled during import

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

void CXaraFile::AddEssentialTag(EssentialTagListItem* pItem)
{
	if (pEssentialTagList == NULL)
		pEssentialTagList = new EssentialTagList;

	if (pEssentialTagList != NULL)
		pEssentialTagList->AddTail(pItem);
}

/********************************************************************************************

>	BOOL CXaraFile::WriteRemainingAtomicTagDefinitions ()

	Author:		Chris Snook
	Created:	14/9/2000
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if changed value, FALSE otherwise
	Purpose:	All compound nodes (e.g.  bevels, contours, shadows, clipview) are now defined
				as being atomic.  This is so that they can be backwards compatible with CX2.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFile::WriteRemainingAtomicTagDefinitions ()
{
	BOOL ok = TRUE;
	
	CXaraFileRecord atomicRec(TAG_ATOMICTAGS, TAG_ATOMICTAGS_SIZE);
	if (ok) ok = atomicRec.Init();
	if (ok) ok = atomicRec.WriteUINT32(TAG_BEVEL);						// NodeBevelController
	if (ok) ok = atomicRec.WriteUINT32(TAG_BEVELINK);					// NodeBevel
	if (ok) ok = atomicRec.WriteUINT32(TAG_CONTOURCONTROLLER);			// NodeContourController
	if (ok) ok = atomicRec.WriteUINT32(TAG_CONTOUR);						// NodeContour
	if (ok) ok = atomicRec.WriteUINT32(TAG_SHADOWCONTROLLER);			// NodeShadowController
	if (ok) ok = atomicRec.WriteUINT32(TAG_SHADOW);						// NodeShadow
	if (ok) ok = atomicRec.WriteUINT32(TAG_CLIPVIEWCONTROLLER);			// NodeClipViewController
	if (ok) ok = atomicRec.WriteUINT32(TAG_CLIPVIEW);					// NodeClipView
	if (ok) ok = atomicRec.WriteUINT32(TAG_CURRENTATTRIBUTES);			// Current Attributes container/component
	if (ok)	ok = Write(&atomicRec);

	return (ok);
}

/********************************************************************************************

>	virtual BOOL CXaraFile::IsTagInAtomicList(UINT32 Tag)

	Author:		Markn
	Created:	16/8/96
	Inputs:		Tag = tag value to look for
	Returns:	TRUE if found, FALSE otherwsie
	Purpose:	Searches the atomic tag list to see of the given tag is in the list.

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

BOOL CXaraFile::IsTagInAtomicList(UINT32 Tag)
{
	if (pAtomicTagList != NULL)
	{
		AtomicTagListItem* pItem = pAtomicTagList->GetHead();
		while (pItem != NULL)
		{
			if (pItem->GetTag() == Tag)
				return TRUE;

			pItem = pAtomicTagList->GetNext(pItem);
		}
	}

	return FALSE;
}

/********************************************************************************************

>	virtual BOOL CXaraFile::IsTagInEssentialList(UINT32 Tag)

	Author:		Markn
	Created:	16/8/96
	Inputs:		Tag = tag value to look for
	Returns:	TRUE if found, FALSE otherwsie
	Purpose:	Searches the Essential tag list to see of the given tag is in the list.

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

BOOL CXaraFile::IsTagInEssentialList(UINT32 Tag)
{
	if (pEssentialTagList != NULL)
	{
		EssentialTagListItem* pItem = pEssentialTagList->GetHead();
		while (pItem != NULL)
		{
			if (pItem->GetTag() == Tag)
				return TRUE;

			pItem = pEssentialTagList->GetNext(pItem);
		}
	}

	return FALSE;
}
#endif


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#if !defined(EXCLUDE_FROM_XARLIB)
/********************************************************************************************

>	virtual BOOL NULLXaraFile::Write(BYTE b)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/6/96
	Inputs:		b = BYTE to write to
	Returns:	TRUE if written successfully
				FALSE otherwise
	Purpose:	This version doesn't actually write anything out.
				It just counts those bytes
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL NULLXaraFile::Write(BYTE b)
{
	BOOL ok = TRUE;

	if (WriteToRecord && pRecord != NULL)
		ok = pRecord->WriteBYTE(b);
	else
		IncNumBytesWritten(1);

	return ok;
}

/********************************************************************************************

>	virtual BOOL NULLXaraFile::OpenToWrite(CCLexFile* pThisCCFile)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/6/96
	Inputs:		params not used
	Returns:	TRUE 
	Purpose:	Writes out the 8 byte header
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL NULLXaraFile::OpenToWrite(CCLexFile* pThisCCFile)
{
	BOOL ok = TRUE;

	// The first 8 bytes should be our unique ID sequence
	if (ok) ok = CXaraFile::Write((UINT32)CXF_IDWORD1);
	if (ok) ok = CXaraFile::Write((UINT32)CXF_IDWORD2);

	return ok;
}

/********************************************************************************************

>	virtual BOOL NULLXaraFile::Close()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/6/96
	Inputs:		-
	Returns:	TRUE 
	Purpose:	Doesn't do anything
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL NULLXaraFile::Close()
{
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL NULLXaraFile::GetFilePos()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/6/96
	Inputs:		-
	Returns:	0
	Purpose:	Always returns 0
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

FilePos NULLXaraFile::GetFilePos()
{
	return 0;
}

/********************************************************************************************

>	virtual BOOL NULLXaraFile::IsCompressionOn()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/96
	Inputs:		-
	Returns:	Always returns FALSE
	Purpose:	Function to see if compression is on.
				As this is the null filter used to count objects, it always returns FALSE
	Errors:		-
	SeeAlso:	StopCompression(); BaseCamelotFilter::StartCompression();

********************************************************************************************/

BOOL NULLXaraFile::IsCompressionOn()
{
	return FALSE;
}

/********************************************************************************************

>	virtual BOOL NULLXaraFile::StartCompression()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/6/96
	Inputs:		-
	Returns:	TRUE if ok
				FALSE otherwise
	Purpose:	Function to turn Compression on when we are writing to the file.
				As this is the null filter used to count objects, do nothing.
	Errors:		-
	SeeAlso:	StopCompression(); BaseCamelotFilter::StartCompression();

********************************************************************************************/

BOOL NULLXaraFile::StartCompression()
{
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL NULLXaraFile::StopCompression()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/6/96
	Inputs:		-
	Returns:	TRUE if ok
				FALSE otherwise
	Purpose:	Function to turn Compression off when we are writing to the file.
				As this is the null filter used to count objects, do nothing.
	Errors:		-
	SeeAlso:	StartCompression(); BaseCamelotFilter::StopCompression();

********************************************************************************************/

BOOL NULLXaraFile::StopCompression()
{
	return TRUE;
}

/********************************************************************************************

>	virtual CCLexFile * NULLXaraFile::GetCCFile() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/96
	Inputs:		-
	Returns:	The CCLexFile that is being used to export the data.
	Purpose:	Function to give public access to the underlying CCLexFile that is being used
				to save out the data.
				Note: only people who have a genuine reason need to access this - e.g. bitmap savers
				As this is the null filter used to count objects, do nothing.
	Errors:		-
	SeeAlso:	BitmapListComponent::SaveBitmapDefinition; BaseCamelotFilter::GetCCFile;

********************************************************************************************/

CCLexFile* NULLXaraFile::GetCCFile() const
{
	return NULL;
}

/********************************************************************************************

>	virtual BOOL NULLXaraFile::FixStreamedRecordHeader(UINT32 *RecordSize)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/6/96
	Inputs:		-
	Outputs:	RecordSize - the size of the record just written
	Returns:	True if worked ok, False otherwise.
	Purpose:	Fixes up the previously saved record header for a streamed record.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL NULLXaraFile::FixStreamedRecordHeader(UINT32 *RecordSize)
{
	return TRUE;
}
#endif	// EXCLUDE_FROM_XARLIB

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

UINT32* GeneralRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {	TAG_ENDOFFILE,
								TAG_FILEHEADER,
#if !defined(EXCLUDE_FROM_XARLIB)
								TAG_UP,
								TAG_DOWN,
								TAG_TAGDESCRIPTION,
								TAG_DOCUMENTNUDGE,
#endif
								TAG_ATOMICTAGS,
								TAG_ESSENTIALTAGS,
								CXFRH_TAG_LIST_END};
	return (UINT32*)&TagList;
}

BOOL GeneralRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");

	BOOL ok = TRUE;

	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_FILEHEADER :
			ok = HandleFileHeader(pCXaraFileRecord);
			break;

		case TAG_ENDOFFILE:
#if !defined(EXCLUDE_FROM_XARLIB)
			EndOfFile();
#else
			m_pCXFile->SetEndOfFile();
#endif
			break;

#if !defined(EXCLUDE_FROM_XARLIB)
		case TAG_UP:
			ok = DecInsertLevel();
			break;

		case TAG_DOWN:
			ok = IncInsertLevel();
			break;

		case TAG_TAGDESCRIPTION:
			ok = HandleTagDescriptionRecord(pCXaraFileRecord);
			break;

		case TAG_DOCUMENTNUDGE:
			ok = HandleTagNudgeSizeRecord(pCXaraFileRecord);
			break;
#endif
		case TAG_ATOMICTAGS:
			ok = HandleAtomicTagsRecord(pCXaraFileRecord);
			break;

		case TAG_ESSENTIALTAGS:
			ok = HandleEssentialTagsRecord(pCXaraFileRecord);
			break;

		default:
			ok = FALSE;
			ERROR3_PF(("I don't handle records with the tag (%d)\n",pCXaraFileRecord->GetTag()));
			break;
	}

	return ok;
}

/********************************************************************************************

>	BOOL GeneralRecordHandler::HandleFileHeader(CXaraFileRecord* pCXaraFileRecord)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/96
	Inputs:		pCXaraFileRecord = ptr to a record
	Returns:	-
	Purpose:	Handles the file header record
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL GeneralRecordHandler::HandleFileHeader(CXaraFileRecord* pCXaraFileRecord)
{
	BOOL ok = TRUE;
	BYTE Buffer[100];
	BYTE FileType[4];
	UINT32 n;
	UINT32 PreCompFlags;
	UINT32 FileSize;

	if (ok) ok = pCXaraFileRecord->ReadBuffer(FileType,3);			// File type (ensuring only 3 chars are read)
	if (ok) ok = pCXaraFileRecord->ReadUINT32(&FileSize);			// File size
	if (ok) ok = pCXaraFileRecord->ReadUINT32(&n);					// Native/Web link ID
	if (ok) ok = pCXaraFileRecord->ReadUINT32(&PreCompFlags);		// Precompression flags
	if (ok) ok = pCXaraFileRecord->ReadASCII((TCHAR*)Buffer,100);	// Producer
	if (ok) ok = pCXaraFileRecord->ReadASCII((TCHAR*)Buffer,100);	// Producer version
	if (ok) ok = pCXaraFileRecord->ReadASCII((TCHAR*)Buffer,100);	// Producer build

#if !defined(EXCLUDE_FROM_XARLIB)
	SetTotalProgressBarCount(FileSize);

	SetImportFileType((char*)FileType);

	ok = SetPreCompression(PreCompFlags);
#endif
	return ok;
}


/********************************************************************************************

>	BOOL GeneralRecordHandler::HandleAtomicTagsRecord(CXaraFileRecord* pCXaraFileRecord)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Inputs:		pCXaraFileRecord = ptr to a record
	Returns:	-
	Purpose:	Handles the atomic tags record
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL GeneralRecordHandler::HandleAtomicTagsRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_ATOMICTAGS,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	UINT32 Size = pCXaraFileRecord->GetSize();
	UINT32 Tag;

	// Create an atomic tag item for each atomic tag in the record

	for (UINT32 i = 0;ok && i < Size; i+=sizeof(UINT32))
	{
		AtomicTagListItem* pItem = NULL;

		ok = pCXaraFileRecord->ReadUINT32(&Tag);
		if (ok) pItem = new AtomicTagListItem(Tag);
		if (ok) ok = (pItem != NULL);
		if (ok) AddAtomicTag(pItem);
	}

	return ok;
}

/********************************************************************************************

>	BOOL GeneralRecordHandler::HandleEssentialTagsRecord(CXaraFileRecord* pCXaraFileRecord)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Inputs:		pCXaraFileRecord = ptr to a record
	Returns:	-
	Purpose:	Handles the essential tags record
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL GeneralRecordHandler::HandleEssentialTagsRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_ESSENTIALTAGS,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	UINT32 Size = pCXaraFileRecord->GetSize();
	UINT32 Tag;

	// Create an essential tag item for each essential tag in the record

	for (UINT32 i = 0;ok && i < Size; i+=sizeof(UINT32))
	{
		EssentialTagListItem* pItem = NULL;

		ok = pCXaraFileRecord->ReadUINT32(&Tag);
		if (ok) pItem = new EssentialTagListItem(Tag);
		if (ok) ok = (pItem != NULL);
		if (ok) AddEssentialTag(pItem);
	}

	return ok;
}


#if !defined(EXCLUDE_FROM_XARLIB)
/********************************************************************************************

>	BOOL GeneralRecordHandler::HandleTagDescriptionRecord(CXaraFileRecord* pCXaraFileRecord)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Inputs:		pCXaraFileRecord = ptr to a record
	Returns:	-
	Purpose:	Handles the tag description record
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL GeneralRecordHandler::HandleTagDescriptionRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_TAGDESCRIPTION,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	UINT32 NumTags;
	UINT32 Tag;
	String_256* pDesc = NULL;

	ok = pCXaraFileRecord->ReadUINT32(&NumTags);

	// Create a tag description item for each tag description in the record

	for (UINT32 i = 0;ok && i < NumTags; i++)
	{
		TagDescriptionListItem* pItem = NULL;

		pDesc = new String_256;
		ok = (pDesc != NULL);

		if (ok) ok = pCXaraFileRecord->ReadUINT32(&Tag);
		if (ok) ok = pCXaraFileRecord->ReadUnicode(pDesc);//*pDesc,pDesc->MaxLength());
		if (ok) pItem = new TagDescriptionListItem(Tag,pDesc);
		if (ok) ok = (pItem != NULL);
		if (ok) ok = AddTagDescription(pItem);

		if (!ok && pItem != NULL)
		{
			delete pItem;
			pItem = NULL;
		}
	}

	return ok;
}



/********************************************************************************************

>	BOOL GeneralRecordHandler::HandleTagNudgeSizeRecord(CXaraFileRecord* pCXaraFileRecord)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/2000
	Inputs:		pCXaraFileRecord = ptr to a record
	Returns:	-
	Purpose:	Handles the nudge size description record
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL GeneralRecordHandler::HandleTagNudgeSizeRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_DOCUMENTNUDGE,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	INT32 val = (INT32) 2835;		//DEFAULT_NUDGE_SIZE;	I'm not moving this just so it will be consistent
	
	ok = pCXaraFileRecord->ReadINT32 (&val);

	ok = SetDocumentNudgeSize ((UINT32) val);

	return ok;
}
#endif

/********************************************************************************************

>	virtual void GeneralRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/96
	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update
	Returns:	-
	Purpose:	The general record handler text dumper
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#if XAR_TREE_DIALOG
void GeneralRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	// Call base class first
	CamelotRecordHandler::GetRecordDescriptionText(pRecord,pStr);

	switch (pRecord->GetTag())
	{
		case TAG_FILEHEADER :
		{
			TCHAR FileType[4];
			FileType[3] = 0;

			char s[256];
			TCHAR Producer[100];
			TCHAR ProducerVersion[100];
			TCHAR ProducerBuild[100];
			UINT32 NativeWebLink;
			UINT32 PrecompressionFlags;
			UINT32 FileSize;

			pRecord->ReadBuffer((BYTE*)FileType,3);		// File type (ensuring only 3 chars are read)
			pRecord->ReadUINT32(&FileSize);				// File size
			pRecord->ReadUINT32(&NativeWebLink);			// Native/Web link ID
			pRecord->ReadUINT32(&PrecompressionFlags);	// Precompression flags
			pRecord->ReadASCII(Producer,100);			// Producer
			pRecord->ReadASCII(ProducerVersion,100);	// Producer version
			pRecord->ReadASCII(ProducerBuild,100);		// Producer build

			(*pStr) += "File Type\t\t\t: ";
			(*pStr) += FileType;
			(*pStr) += "\r\n";

			(*pStr) += "Uncompressed File Size\t: ";
			_stprintf(s,"%d",FileSize);
			(*pStr) += s;
			(*pStr) += "\r\n";

			(*pStr) += "Native/Web Link ID\t: ";
			_stprintf(s,"%d",NativeWebLink);
			(*pStr) += s;
			(*pStr) += "\r\n";

			(*pStr) += "Precompression Flags\t: ";
			_stprintf(s,"0x%x",PrecompressionFlags);
			(*pStr) += s;
			(*pStr) += "\r\n";

			(*pStr) += "Producer\t\t\t: ";
			(*pStr) += Producer;
			(*pStr) += "\r\n";

			(*pStr) += "Producer Version\t\t: ";
			(*pStr) += ProducerVersion;
			(*pStr) += "\r\n";

			(*pStr) += "Producer Build\t\t: ";
			(*pStr) += ProducerBuild;
			(*pStr) += "\r\n";
		}
		break;

		case TAG_ATOMICTAGS:
		{
			UINT32 Size = pRecord->GetSize();
			UINT32 Tag;
			String_256 TagText;

			(*pStr) += "Atomic tag list:\r\n";

			for (UINT32 i = 0;i < Size; i+=sizeof(UINT32))
			{
				pRecord->ReadUINT32(&Tag);
				GetTagText(Tag,TagText);
				(*pStr) += TagText;
				(*pStr) += "\r\n";
			}
		}
		break;

		case TAG_ESSENTIALTAGS:
		{
			UINT32 Size = pRecord->GetSize();
			UINT32 Tag;
			String_256 TagText;

			(*pStr) += "Essential tag list:\r\n";

			for (UINT32 i = 0;i < Size; i+=sizeof(UINT32))
			{
				pRecord->ReadUINT32(&Tag);
				GetTagText(Tag,TagText);
				(*pStr) += TagText;
				(*pStr) += "\r\n";
			}
		}
		break;

		case TAG_TAGDESCRIPTION:
		{
			UINT32 NumTags;
			UINT32 Tag;
			String_256 Desc,TagText;

			(*pStr) += "Tag description list:\r\n";

			pRecord->ReadUINT32(&NumTags);

			for (UINT32 i = 0;i < NumTags; i++)
			{
				pRecord->ReadUINT32(&Tag);
				pRecord->ReadUnicode(&Desc);//Desc,Desc.MaxLength());
				GetTagText(Tag,TagText);
				(*pStr) += TagText;
				(*pStr) += " (";
				(*pStr) += Desc;
				(*pStr) += ")\r\n";
			}
		}
		break;
	}
}
#endif
