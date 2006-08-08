// $Id$
// Implementation of the CXaraTemplateFile class
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

#include "cxftfile.h"
//#include "cxfrec.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "cxfrech.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "cxfdefs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cxfmap.h"

//#include "ccfile.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "progress.h"
//#include "filtrres.h"

#include "ccpanose.h"

#include "bmpsrc.h"		// BitmapSource
#include "ccbuffil.h"

#include "cxftags.h"	// The tag definitions
//#include "cxfdefs.h"	// The constants - in camtypes.h [AUTOMATICALLY REMOVED]
#include "zutil.h"		// ZLIB_VERSIONNO

//#include "camfiltr.h"	// BaseCamelotFilter - in camtypes.h [AUTOMATICALLY REMOVED]

#include "fttyplis.h"

#include "fileutil.h"

#include "ccbhfile.h"	// include the translating file proxy

//#include "fthelper.h"

//-----------------------------------------------

CC_IMPLEMENT_DYNAMIC(CXaraTemplateFile, CXaraFile);

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	CXaraTemplateFile::CXaraTemplateFile()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/97
	Purpose:	Default constructor
	SeeAlso:	-

********************************************************************************************/

CXaraTemplateFile::CXaraTemplateFile()
{
	m_pTempFile = NULL;
	m_pBinHexFile = NULL;
	m_pOrigCCFile = NULL;
}



/********************************************************************************************

>	BOOL CXaraTemplateFile::OpenToWrite(CCLexFile* pThisCCFile)

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

BOOL CXaraTemplateFile::OpenToWrite(CCLexFile* pThisCCFile)
{
	// Check entry params
	ERROR2IF(pThisCCFile == NULL,FALSE,"pThisCCFile is NULL");

	//  pCCFile should be NULL at this point
	ERROR3IF(pCCFile != NULL,"pThisCCFile is NULL");

	// Set our ptr the the CCFile to the one provided
	pCCFile = pThisCCFile;

	// Try to create a CCBinHexFile for later

	m_pBinHexFile = new CCBinHexFile(pCCFile, FALSE, FALSE);

	if (m_pBinHexFile == NULL)
		return(FALSE);

	// Reset the write vars
	RecordNumber	= 0;
	NumBytesWritten = 0;

	BOOL ok = TRUE;

	// The first 8 bytes should be our unique ID sequence
	if (ok) ok = pCCFile->write("FLARETEXT\r\n", 11).good();

	return ok;
}



/********************************************************************************************

>	BOOL CXaraTemplateFile::OpenToRead(CCLexFile* pThisCCFile)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/07/97
	Inputs:		pThisCCFile = ptr to the file to read from
	Returns:	TRUE if the the system is ready to read from the file
				FALSE otherwise
	Purpose:	Opens the CXaraFile object ready for reading.
				It actually uses the given CCFile object as the file to read from.

				NOTE:	It is assumed that the supplied CCFile has already been opened for reading
						from the start of the file.	This function will return FALSE if the initial
						10 bytes do not contain the file header sequence.
	Errors:		-
	SeeAlso:	Close()

********************************************************************************************/

BOOL CXaraTemplateFile::OpenToRead(CCLexFile* pThisCCFile)
{
	PORTNOTETRACE("other","CXaraTemplateFile::OpenToRead - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Check entry params
	ERROR2IF(pThisCCFile == NULL,FALSE,"pThisCCFile is NULL");

	//  pCCFile should be NULL at this point
	ERROR3IF(pCCFile != NULL,"pThisCCFile is NULL");

	// Remember the original file
	m_pOrigCCFile = pThisCCFile;
	
	// Translate the file
	CCLexFile* pNewFile = FlareTemplateHelper::ConvertFile(pThisCCFile);
	
	if (pNewFile == NULL)
	{
		return(FALSE);
	}

	// And call the base class to do the rest
	return(CXaraFile::OpenToRead(pNewFile));
#else
	return false;
#endif
}



/********************************************************************************************

>	BOOL CXaraTemplateFile::Close()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/97
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

BOOL CXaraTemplateFile::Close()
{
	// Reset all handlers that we set up via SetUpHandlers()
	ResetHandlers();

	if (m_pBinHexFile != NULL)
	{
		delete m_pBinHexFile;
		m_pBinHexFile = NULL;
	}

	// We must not delete this but we must delete the CCFile that we created
	if (m_pOrigCCFile != NULL)
	{
		if (pCCFile)
		{
			PathName TempPath = pCCFile->GetPathName();
			delete pCCFile;								// delete the CCFile (closes it)
	PORTNOTETRACE("other","Removed FileUtil::DeleteFile usage");
#ifndef EXCLUDE_FROM_XARALX
			FileUtil::DeleteFile(&TempPath);			// delete the actual file
#endif
		}
		pCCFile = m_pOrigCCFile;
		m_pOrigCCFile = NULL;
	}

	return TRUE;
}

/********************************************************************************************

>	virtual CCLexFile* CXaraTemplateFile::GetCCFile() const

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/97
	Inputs:		-
	Returns:	The CCLexFile that is being used to export the data.
	Purpose:	Function to give public access to the underlying CCLexFile that is being used
				to save out the data.

				This either returns a special proxy CCFile which translates the data
				or a temporary disk file (during streamed records)

********************************************************************************************/

CCLexFile* CXaraTemplateFile::GetCCFile() const
{
//	TRACEUSER( "Gerry", _T("CXaraTemplateFile::GetCCFile()\n"));

	if (m_pOrigCCFile)
		return(pCCFile);

	if (m_pTempFile)
		return(m_pTempFile);
	else
		return(m_pBinHexFile);
}



/********************************************************************************************

>	INT32 CXaraTemplateFile::StartRecord(UINT32 Tag,INT32 Size)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/97
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

INT32 CXaraTemplateFile::StartRecord(UINT32 Tag,INT32 Size)
{
	if (EndRecord())
	{
		// Inc the record number
		RecordNumber++;

		// Create a new record, & initialise it
		pRecord = new CXaraFileRecord(Tag, Size);

		WriteToRecord = (pRecord != NULL && pRecord->Init());

		if (WriteToRecord)
		{
			pRecord->SetRecordNumber(RecordNumber);
			return RecordNumber;
		}
	}

	return 0;
}


INT32 CXaraTemplateFile::StartStreamedRecord(UINT32 Tag,INT32 Size)
{
//	TRACEUSER( "Gerry", _T("StartStreamed  %d, %d\n"), Tag, Size);
	
	ERROR2IF(WritingStreamedRecord,0,"Already writing a streamed record.  Forgot to call EndRecord()?");

	ERROR3IF(m_pTempFile != NULL, "TempFile is non-NULL!!!");

	WritingStreamedRecord = TRUE;
	WriteToRecord = FALSE;

	// Don't do any compression stuff
//	CompOffDueToStreamedRecord = IsCompressionOn();
//	BOOL ok = StopCompression();
	
	// And don't do any of this rampant poking of length values
//	StartOfStreamedRecord = GetFilePos();
	
	// All streamed records have labels stuck on them (for now),
	// have length set to zero and consist of one BINH data item
	char Buf[64];
	INT32 Num = sprintf(Buf, "%%R%d%%{%d,%d,BINH(", (RecordNumber+1), Tag, 0);
	
	// make sure we write directly to the file
	BOOL ok = pCCFile->write(Buf, Num).good();

	if (!ok)
		return(0);

	// Now we create a temporary file which GetCCFile
	// will return until EndStreamedRecord is called
	PORTNOTETRACE("other","Removed FileUtil::GetTemporaryPathName usage");
#ifndef EXCLUDE_FROM_XARALX
	m_TempPath = FileUtil::GetTemporaryPathName();
#endif
	m_pTempFile = new CCDiskFile(m_TempPath, ios::out | ios::binary | ios::trunc);

	if (m_pTempFile == NULL || m_pTempFile->bad())
		return(0);

	RecordNumber++;
	return(RecordNumber);
}


/********************************************************************************************

>	BOOL CXaraTemplateFile::EndRecord()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/97
	Inputs:		-
	Returns:	TRUE if the the system wrote out the record
				FALSE otherwise
	Purpose:	Ends the current record.

				If the record was started by StartRecord(), this call will write out the entire
				buffered record (record header and data section) to the file.

	Errors:		-
	SeeAlso:	StartRecord()

********************************************************************************************/

BOOL CXaraTemplateFile::EndRecord()
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

//		if (ok) ok = Write(pRecord->GetTag());
//		if (ok) ok = Write(pRecord->GetSize());
//		if (ok) ok = Write(pRecord->GetBuffer(),pRecord->GetSize());

		delete pRecord;
		pRecord = NULL;
	}

	return (ok);
}

/********************************************************************************************

>	BOOL CXaraTemplateFile::EndStreamedRecord(UINT32 *RecordSize)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/97
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

BOOL CXaraTemplateFile::EndStreamedRecord(UINT32 *RecordSize)
{
	ERROR3IF(!WritingStreamedRecord,"Trying to end a non-streamed record!");
	if (!WritingStreamedRecord)
		return FALSE;

	BOOL ok = TRUE;
	// We must reset WritingStreamedRecord to FALSE immediately, to prevent stack overflow
	// due to recursion via the StartCompression() call below
	WritingStreamedRecord = FALSE;

	UINT32 SizeOfFile = 0;

	if (m_pTempFile == NULL)
		return(FALSE);

	m_pTempFile->close();
	delete m_pTempFile;
	m_pTempFile = NULL;

	m_pTempFile = new CCDiskFile(CCFILE_DEFAULTSIZE, FALSE, FALSE);
	if (m_pTempFile == NULL)
		return(FALSE);
	if(!m_pTempFile->open(m_TempPath, ios::in | ios::binary))
		return(FALSE);

	BYTE	Buffer[BinHexMaxLineLength * 10];		// Currently 1000
	size_t	left = m_pTempFile->Size();
	UINT32	len = BinHexMaxLineLength * 10;

	while (m_pTempFile->good() && m_pBinHexFile->good() && len > 0)
	{
		if (len > left)
			len = left;
		m_pTempFile->read( Buffer, len );

		if (m_pTempFile->fail())
			TRACEUSER( "Gerry", _T("read from temp file failed!!!\n"));

		m_pBinHexFile->write(Buffer, len);

		left -= len;
	}

	// Close and delete the temp file
	m_pTempFile->close();
	delete m_pTempFile;
	m_pTempFile = NULL;
	PORTNOTETRACE("other","Removed FileUtil::DeleteFile usage");
#ifndef EXCLUDE_FROM_XARALX
	FileUtil::DeleteFile(&m_TempPath);
#endif

	if (m_pBinHexFile->fail())
		return(FALSE);

	// Try to finish the record...
	if (pCCFile->write(")}\r\n", 4).fail())
		return(FALSE);

	// If we are passed a pointer then return the size to the caller
	if (RecordSize)
		*RecordSize = SizeOfFile;

	// Add this number to the number of bytes written
//	TRACEUSER( "Gerry", _T("EndStreamedRecord update size by %d\n"), SizeOfFile);
	IncNumBytesWritten(SizeOfFile);
	
	// Reset the streamed record vars
	StartOfStreamedRecord = 0;
	CompOffDueToStreamedRecord = FALSE;
		
	return ok;
}

/********************************************************************************************

>	virtual BOOL CXaraTemplateFile::FixStreamedRecordHeader(UINT32 *RecordSize)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/97
	Inputs:		-
	Outputs:	RecordSize - the size of the record just written
	Returns:	True if worked ok, False otherwise.
	Purpose:	Fixes up the previously saved record header for a streamed record.
				The RecordSize is the size of the data section of the record and so does not
				include the 8 bytes in the header.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraTemplateFile::FixStreamedRecordHeader(UINT32 *RecordSize)
{
	ERROR2IF(pCCFile == NULL,FALSE,"CXaraFile::FixStreamedRecordHeader NULL pCCFile");
	ERROR2IF(StartOfStreamedRecord == 0,FALSE,"CXaraFile::FixStreamedRecordHeader StartOfStreamedRecord == 0");

	// Dont do nuthin' 'ere 'cos we can't get the current file postion to determine the
	// length of the streamed record
	
	return(TRUE);
}


/********************************************************************************************

>	UINT32 CXaraTemplateFile::WriteRecordHeader(UINT32 Tag, INT32 Size)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/97
	Inputs:		Tag		- the tag for this record
				Size	- the size of the record
	Returns:	The record number of this record in the file.
				If an error occurs, 0 is returned
	Purpose:	Writes out a record header directly to the file.
				It expects the caller to handle other aspects of the record writing.
				This should only be used by people who know what they are doing e.g. compression
				It should not be required for template files but we'll see
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 CXaraTemplateFile::WriteRecordHeader(UINT32 Tag, INT32 Size)
{
//	TRACEUSER( "Gerry", _T("WriteRecordHeader  %d, %d\n"), Tag, Size);
	
	BOOL ok = TRUE;

	char Buf[32];
	INT32 Num = sprintf(Buf, "{%d,%d", Tag, Size);
	
	// make sure we write directly to the file
	ok = pCCFile->write(Buf, Num).good();

	// Inc the record number
	RecordNumber++;

	if (ok)
		return RecordNumber;
	else
		return 0;
}



/********************************************************************************************

>	virtual BOOL CXaraTemplateFile::Write(BYTE b)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/97
	Inputs:		pRecord = ptr to a record to write out to file
	Returns:	The record number of this record in the file.
				If an error occurs, 0 is returned
	Purpose:	Writes out the record to the file.  Uses WriteAsText to do the work
	SeeAlso:	WriteAsText()

********************************************************************************************/

BOOL CXaraTemplateFile::Write(BYTE b)
{
	ERROR2IF(GetCCFile() == NULL,FALSE,"Can't write a BYTE with no pCCFile");

	BOOL ok = TRUE;

	// If we are writing to a record then let it handle the translation
	if (WriteToRecord && pRecord != NULL)
		ok = pRecord->WriteBYTE(b);
	else
	{
		// For streamed records we need to use our proxy CCFile
		GetCCFile()->write((void const*)&b);
		IncNumBytesWritten(1);
	}

	return ok;
}



/********************************************************************************************

>	UINT32 CXaraTemplateFile::Write(CXaraFileRecord* pRecord)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/97
	Inputs:		pRecord = ptr to a record to write out to file
	Returns:	The record number of this record in the file.
				If an error occurs, 0 is returned
	Purpose:	Writes out the record to the file.  Uses WriteAsText to do the work
	SeeAlso:	WriteAsText()

********************************************************************************************/

UINT32 CXaraTemplateFile::Write(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL,FALSE,"pRecord param is NULL");

	BOOL ok = EndRecord();

	ERROR3IF(!pRecord->IsDataSectionFull(),"Data section of the record has not been filled");

	if (ok) ok = WriteAsText(pRecord);
	
	// Inc the record number
	RecordNumber++;

	if (ok)
		return RecordNumber;
	else
		return 0;
}


/********************************************************************************************

>	UINT32 CXaraTemplateFile::WriteDefinitionRecord(CXaraFileRecord* pRecord)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/07/97
	Inputs:		pRecord = ptr to a record to write out to file
	Returns:	The record number of this record in the file.
				If an error occurs, 0 is returned
	Purpose:	Writes out the record to the file
				This version writes a label to the file

********************************************************************************************/

UINT32 CXaraTemplateFile::WriteDefinitionRecord(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL,FALSE,"pRecord param is NULL");

	BOOL ok = EndRecord();

	ERROR3IF(!pRecord->IsDataSectionFull(),"Data section of the record has not been filled");

	if (ok) ok = WriteLabel();
	if (ok) ok = WriteAsText(pRecord);
	
	// Inc the record number
	RecordNumber++;

	if (ok)
		return RecordNumber;
	else
		return 0;
}


/********************************************************************************************

>	BOOL CXaraTemplateFile::WriteLabel(void)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/07/97
	Inputs:		-
	Returns:	FALSE if fails
	Purpose:	Writes a label to the file of the form %R<RecordNumber>%

********************************************************************************************/

BOOL CXaraTemplateFile::WriteLabel(void)
{
	// TODOG: Well, not yet it doesn't...

	char Buffer[32];

	UINT32 Num = sprintf(Buffer, "%%R%d%%", (RecordNumber+1));

	return(pCCFile->write(Buffer, Num).good());
}


/********************************************************************************************

>	BOOL CXaraTemplateFile::WriteAsText(CXaraFileRecord* pRecord)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/07/97
	Inputs:		pRec - pointer to record to write
	Purpose:	Writes the record in text format

	Notes:		This function is not particularly elegant (mainly due to time restrictions)

********************************************************************************************/

BOOL CXaraTemplateFile::WriteAsText(CXaraFileRecord* pRecord)
{
	ERROR3IF(pRecord == NULL, "NULL record in CXaraTemplateFile::WriteAsText");

	if (pRecord == NULL)
		return(FALSE);

	char Buffer[256];				// Allocate a buffer on the stack for now
	Buffer[0] = 0;					// and make it empty
	INT32 Num;

	FTTypeList* pTypeList = pRecord->GetTypeList();

	// Write the "record header"
	Num = sprintf(Buffer, "{%d,%d", pRecord->GetTag(), pRecord->GetSize());
	BOOL ok = pCCFile->write(Buffer, Num).good();

	if (ok &&
		pTypeList != NULL &&
		pTypeList->GetCount() > 0)
	{
		BYTE* pBinPtr = pRecord->GetBuffer();
		BYTE* pPtr = pTypeList->GetBuffer();
		DWORD Count = pTypeList->GetCount();
		DWORD Index;
		for (Index = 0; ok && (Index < Count); Index++)
		{
//			char* pType = NULL;
			Num = 0;				// Nothing to write

			// Now we fill the buffer and set Num to the number of bytes to write
			// or just write it ourselves and set Num to zero
			
			switch (pPtr[Index])
			{
				case FTT_BYTE:
					{
						BYTE ByteVal = *pBinPtr;
						TRACEUSER( "Gerry", _T("Byte : %d\n"), ByteVal);
						Num = sprintf(Buffer, ",B(%u)", ByteVal);
						pBinPtr+=1;
						break;
					}

				case FTT_UINT32:
					{
						UINT32 ULongVal = *((UINT32*)pBinPtr);
						TRACEUSER( "Gerry", _T("ULong : %u\n"), ULongVal);
						Num = sprintf(Buffer, ",U(%u)", ULongVal);
						pBinPtr+=4;
						break;
					}
				
				case FTT_INT32:
					{
						INT32 LongVal = *((INT32*)pBinPtr);
						TRACEUSER( "Gerry", _T("Long : %d\n"), LongVal);
						Num = sprintf(Buffer, ",L(%d)", LongVal);
						pBinPtr+=4;
						break;
					}
				
				case FTT_UINT16:
					{
						WORD WordVal = *((WORD*)pBinPtr);
						TRACEUSER( "Gerry", _T("UINT16 : %hu\n"), WordVal);
						Num = sprintf(Buffer, ",W(%hu)", WordVal);
						pBinPtr+=2;
						break;
					}
				
				case FTT_INT16:
					{
						INT16 ShortVal = *((INT16*)pBinPtr);
						TRACEUSER( "Gerry", _T("INT16 : %hd\n"), ShortVal);
						Num = sprintf(Buffer, ",I(%hd)", ShortVal);
						pBinPtr+=2;
						break;
					}

				case FTT_FLOAT:
					{
						double DoubleVal = *((FLOAT*)pBinPtr);
						TRACEUSER( "Gerry", _T("Float : %g\n"), DoubleVal);
						Num = sprintf(Buffer, ",F(%g)", DoubleVal);
						pBinPtr+=4;
						break;
					}
				
				case FTT_DOUBLE:
					{
						double DoubleVal = *((double*)pBinPtr);
						TRACEUSER( "Gerry", _T("Double : %g\n"), DoubleVal);
						Num = sprintf(Buffer, ",D(%g)", DoubleVal);
						pBinPtr+=8;
						break;
					}
				
				case FTT_REFERENCE:
					{
						INT32 LongVal = *((INT32*)pBinPtr);
						TRACEUSER( "Gerry", _T("Long : %d\n"), LongVal);
						if (LongVal <= 0)
						{
							// then write it as an INT32
							Num = sprintf(Buffer, ",L(%d)", LongVal);
						}
						else
						{
							// Otherwise write a label reference
							Num = sprintf(Buffer, ",R(R%d)", LongVal);
						}
						pBinPtr+=4;
						break;
					}
				
				case FTT_WCHAR:
					{
						TRACEUSER( "Gerry", _T("WCHAR : %d items\n"), Count - Index);
						UINT32 NumChars = WriteMultipleWCHARs(&(pPtr[Index]), Count - Index, (UINT16*)pBinPtr);
						if (NumChars == 0)
							ok = FALSE;
						pBinPtr += (NumChars * sizeof(WCHAR));	// Skip the pointerand 
						Index += NumChars;						// index on by NumChars
						break;
					}
				
				case FTT_COORD:
					{
						INT32* pLong = (INT32*)pBinPtr;
//						TRACEUSER( "Gerry", _T("Coord : %d, %d\n"), pLong[0], pLong[1]);
						Num = sprintf(Buffer, ",C(%d,%d)", pLong[0], pLong[1]);
						pBinPtr += 8;
						break;
					}
				
				case FTT_INTCOORD:
					{
						INT32 x = (pBinPtr[0] << 24);
						INT32 y = (pBinPtr[1] << 24);
						x += (pBinPtr[2] << 16);
						y += (pBinPtr[3] << 16);
						x += (pBinPtr[4] <<  8);
						y += (pBinPtr[5] <<  8);
						x += (pBinPtr[6] <<  0);
						y += (pBinPtr[7] <<  0);
						TRACEUSER( "Gerry", _T("IntCoord : %d, %d\n"), x, y);
						Num = sprintf(Buffer, ",K(%d,%d)", x, y);
						pBinPtr += 8;
						break;
					}
				
				case FTT_ASCII:
					{
						// Use ,ASC("String")
						size_t len = strlen((char*)pBinPtr) + 1;
						TRACEUSER("Gerry", _T("ASCII %d"), len);
						ok = WriteSimpleASCII((char*)pBinPtr);
						pBinPtr += len;
						break;
					}

				case FTT_UNICODE:
					{
						UINT32 len = 1;
						UINT16* pCh = (UINT16*)pBinPtr;
						while (*pCh++ != 0)
							len++;
						TRACEUSER("Gerry", _T("Unicode %d"), len);
						if (IsSimpleUnicode((UINT16*)pBinPtr))
						{
							ok = WriteSimpleUnicode((UINT16*)pBinPtr);
						}
						else
						{
							ok = WriteBinHex(pBinPtr, len);
						}
						pBinPtr += len;
						break;
					}

				case FTT_BINHEX:
					{
						UINT32 len = *((DWORD*)(pPtr+Index+1));	// Get the length (BYTE pointer arithmetic)
						TRACEUSER("Gerry", _T("BinHex %d"), len);
						Index += 4;
						ok = WriteBinHex(pBinPtr, len);
						pBinPtr += len;
						break;
					}
				default:
					{
						ok = FALSE;
						break;
					}
			}

			if (ok && Num > 0)
			{
//				TRACEUSER( "Gerry", _T("Writing buffer  %d\n"), Num);
				if (ok) ok = pCCFile->write(Buffer, Num).good();
			}
		}
	}
	
	if (ok) ok = pCCFile->write("}\r\n", 3).good();

	return(ok);
}


/********************************************************************************************

>	BOOL CXaraTemplateFile::WriteBinHex(BYTE* pBuf, UINT32 BufSize)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/07/97
	Inputs:		pBuf - pointer to buffer to write
				BufSize - number of bytes to write
	Purpose:	Writes the record in text format

	Notes:		This function is not particularly elegant (mainly due to time restrictions)

********************************************************************************************/

BOOL CXaraTemplateFile::WriteBinHex(BYTE* pBuf, UINT32 BufSize)
{
//	TRACEUSER( "Gerry", _T("Writing BinHex  %d\n"), BufSize);
	
	// Write the type specifier
	BOOL ok = pCCFile->write(",BINH(", 6).good();
	// And get our special CCFile to translate it
	if (ok) ok = m_pBinHexFile->write(pBuf, BufSize).good();
	if (ok) ok = pCCFile->write(")", 1).good();

	return(ok);
}



/********************************************************************************************

>	BOOL CXaraTemplateFile::IsSimpleUnicode(UINT16* pStr)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/07/97
	Inputs:		pStr - pointer to UNICODE string
	Purpose:	Determines if UNICODE is safe as ASCII

********************************************************************************************/

BOOL CXaraTemplateFile::IsSimpleUnicode(UINT16* pStr)
{
	if (pStr == NULL)
		return(FALSE);
	
	while (*pStr != 0)
	{
		if (((*pStr) > 126) ||
			((*pStr) < 32))
		{
			return(FALSE);
		}
		pStr++;
	}
	
	return(TRUE);
}


/********************************************************************************************

>	BOOL CXaraTemplateFile::WriteSimpleUnicode(WCHAR* pStr)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/07/97
	Inputs:		pBuf - pointer to buffer to write
				BufSize - number of bytes to write
	Purpose:	Writes the record in text format

	Notes:		This function is not particularly elegant (mainly due to time restrictions)

********************************************************************************************/

BOOL CXaraTemplateFile::WriteSimpleUnicode(UINT16* pStr)
{
	if (pStr == NULL)
		return(FALSE);
	
	// Write the type specifier
	BOOL ok = pCCFile->write(",UNC(\"", 6).good();

	while (ok && *pStr != 0)
	{
		// Write a single byte
		ok = pCCFile->write(pStr).good();

		// If it's a quote then double it up
		if (ok && *pStr == '"')
			ok = pCCFile->write(pStr).good();
		
		// Advance to next UINT16
		pStr++;
	}

	if (ok) ok = pCCFile->write("\")", 2).good();

	return(ok);
}



/********************************************************************************************

>	BOOL CXaraTemplateFile::WriteSimpleASCII(char* pStr)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/07/97
	Inputs:		pBuf - pointer to buffer to write
				BufSize - number of bytes to write
	Purpose:	Writes the record in text format

	Notes:		This function is not particularly elegant (mainly due to time restrictions)

********************************************************************************************/

BOOL CXaraTemplateFile::WriteSimpleASCII(char* pStr)
{
	if (pStr == NULL)
		return(FALSE);
	
	// Write the type specifier
	BOOL ok = pCCFile->write(",ASC(\"", 6).good();

	while (ok && *pStr != 0)
	{
		// Write a single byte
		ok = pCCFile->write(pStr).good();

		// If it's a quote then double it up
		if (ok && *pStr == '"')
			ok = pCCFile->write(pStr).good();
		
		// Advance to next char
		pStr++;
	}

	if (ok) ok = pCCFile->write("\")", 2).good();

	return(ok);
}



/********************************************************************************************

>	UINT32 CXaraTemplateFile::WriteMultipleWCHARs(BYTE* pTypes, DWORD Count, UINT16* pChars)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/07/97
	Inputs:		pTypes - pointer to array of types
				Count - Size of type array
				pChars - pointer to actual WCHARs
	Purpose:	Writes out one or more WCHARs using fancy concatenation to make the
				output more readable

********************************************************************************************/

UINT32 CXaraTemplateFile::WriteMultipleWCHARs(BYTE* pTypes, DWORD Count, UINT16* pChars)
{
	TRACEUSER( "Gerry", _T("WriteMultipleWCHARs %d\n"), Count);

	ERROR3IF(pTypes == NULL, "NULL type array");
	ERROR3IF(pChars == NULL, "NULL chars array");
	ERROR3IF(pTypes[0] != FTT_WCHAR, "First type isn't a WCHAR");
	ERROR3IF(Count == 0, "No characters");

	if (Count == 0)
		return(0);

	WCHAR ch = pChars[0];
	if (Count == 1 || pTypes[1] != FTT_WCHAR)
		return(WriteSingleWCHAR(ch) ? 1 : 0);

	// Flag for type of current string
	BOOL bAscii = IsPrint(ch);
	BOOL ok = TRUE;

	// Indices where things change
	UINT32 ThisItem = 0;
	UINT32 NextItem = 1;
	UINT32 bContinue = TRUE;
	UINT32 NumWritten = 0;

	// While we should continue
	while (bContinue)
	{
		// While we have a next item, 
		while (ok &&
				(NextItem < Count) &&
				(pTypes[NextItem] == FTT_WCHAR) &&
				(IsPrint(pChars[NextItem]) == bAscii))
		{
			NextItem++;
		}

		// Now the sense has changed or we are at the end...
		// So write out the data item
		if ((NextItem - ThisItem) == 1)
		{
			ok = WriteSingleWCHAR(pChars[ThisItem]);
		}
		else
		{
			if (bAscii)
			{
				// Write the type specifier
				ok = pCCFile->write(",WCS(\"", 6).good();
				UINT32 i;
				for (i = ThisItem; ok && (i < NextItem); i++)
				{
					// Write a single byte
					ok = pCCFile->write(&(pChars[i])).good();

					// If it's a quote then double it up
					if (ok && pChars[i] == '"')
						ok = pCCFile->write(&(pChars[i])).good();
				}

				if (ok) ok = pCCFile->write("\")", 2).good();
				if (ok) NumWritten += NextItem - ThisItem;
			}
			else
			{
				ok = WriteBinHex((BYTE*)&(pChars[ThisItem]), (NextItem - ThisItem) * sizeof(WCHAR));
				if (ok) NumWritten += (NextItem - ThisItem);
			}

		}	// if ((NextItem - ThisItem) == 1)


		if (ok &&
			(NextItem < Count) &&
			(pTypes[NextItem] == FTT_WCHAR))
		{

			// Point at next item
			ThisItem = NextItem;
			// And then increment NextItem
			NextItem++;
			// Set correct sense for the next item
			bAscii = IsPrint(pChars[ThisItem]);
		}
		else
			bContinue = FALSE;		// Otherwise flag to stop
	}

	if (!ok)
		NumWritten = 0;

	return(NumWritten);
}


/********************************************************************************************

>	BOOL CXaraTemplateFile::WriteSingleWCHAR(WCHAR ch)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/07/97
	Inputs:		ch - WCHAR to write
	Purpose:	Writes a single WCHAR as a complete data item
				It uses WC() when the character is printable ascii otherwise it uses W()
				output more readable

********************************************************************************************/

BOOL CXaraTemplateFile::WriteSingleWCHAR(WCHAR ch)
{
	BOOL ok = TRUE;
	char Buffer[16];
	UINT32 Num = 0;
	
	if (ch == '"')
	{
		// Special case the doubled up quotes
		memcpy(Buffer, ",WC(\"\"\"\")", 9);
	}
	else if (IsPrint(ch))
	{
		Num = sprintf(Buffer, ",WC(\"%c\")", (char)(ch & 0xFF));
	}
	else
	{
		Num = sprintf(Buffer, ",W(%hu)", (INT16)(ch & 0xFFFF));
	}

	ok = pCCFile->write(Buffer, Num).good();

	return(ok);
}
