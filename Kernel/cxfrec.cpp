// $Id$
// Implementation of the record class of the v2 file format
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

#include "cxfrec.h"
#include "cxfile.h"
#include "cxfdefs.h"
#include "camfiltr.h"

//#include "ccpanose.h"
#include "fixmem.h"
#include "paths.h"
#include "unicdman.h"

#include "fttyplis.h"

#include "hardwaremanager.h"
using namespace oilHardwareManager;

// This define has different meaning under Linux
#if !defined(__WXMSW__)
#undef UNICODE
#endif

CC_IMPLEMENT_DYNAMIC(CXaraFileRecord,CCObject);
CC_IMPLEMENT_DYNAMIC(CamelotFileRecord,CXaraFileRecord);

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


#define RELPATHINTERLEAVE

//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------


/********************************************************************************************

>	CXaraFileRecord::CXaraFileRecord(UINT32 Tag,INT32 Size)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		Tag	- Tag value for the record
				Size- The exact size of the record in bytes, or CXF_UNKNOWN_SIZE
	Purpose:	Creates a record object.
				These are used by CXaraFile for reading and writing records.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CXaraFileRecord::CXaraFileRecord(UINT32 Tag,INT32 Size)
{
	SetUpVars(Tag,Size);
}

/********************************************************************************************

>	void CXaraFileRecord::SetUpVars(UINT32 ThisTag,INT32 Size)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		ThisTag	- Tag value for the record
				Size	- The exact size of the record in bytes, or CXF_UNKNOWN_SIZE
	Returns:	-
	Purpose:	Sets up the member vars for this record

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CXaraFileRecord::SetUpVars(UINT32 ThisTag,INT32 Size)
{
	Tag			= ThisTag;
	KnownSize	= Size;
	SizeIsKnown	= (Size != CXF_UNKNOWN_SIZE);
	RecordNumber= 0;

	pBuffer		= NULL;
	BufferSize	= 0;
	CurrentPos	= 0;
	Overflow	= FALSE;
	m_pTypeList = NULL;
	Initialised = FALSE;
}

/********************************************************************************************

>	CXaraFileRecord::~CXaraFileRecord()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Purpose:	Default destructor

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CXaraFileRecord::~CXaraFileRecord()
{
	FreeBuffer();
}

/********************************************************************************************

>	BOOL CXaraFileRecord::Init(BOOL ZeroMemBlock = FALSE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		ZeroMemBlock = If TRUE, any memory allocated for the record is filled with zeros
	Returns:	TRUE if initialised OK
				FALSE otherwise
	Purpose:	You must call this function before use use any of the Write functions.
				It initialises the record ready for writing data to (i.e. allocates a buffer)				

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFileRecord::Init(BOOL ZeroMemBlock)
{
	if (!Initialised)
	{
		ERROR2IF(SizeIsKnown && KnownSize < 0,FALSE,"A -ve size has been given");

		ERROR3IF(pBuffer != NULL,"Initialising the record, yet it already has a buffer");
		
		if (SizeIsKnown)
			Initialised = AllocBuffer(KnownSize);
		else
			Initialised = AllocBuffer(CXF_RECORD_CHUNK);
	}

	if (Initialised && ZeroMemBlock)
	{
		BYTE* pBuf = GetBuffer();
		if (pBuf != NULL)
		{
			for (UINT32 i=0;i<BufferSize;i++)
				pBuf[i] = 0;
		}
	}

	if (Initialised)
	{
		m_pTypeList = new FTTypeList;
		if (m_pTypeList == NULL)
			TRACEUSER( "Gerry", _T("Failed to create FTTypeList\n"));
	}
	
	return Initialised;
}


/********************************************************************************************

>	void CXaraFileRecord::DumpTypes(void)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/07/97
	Purpose:	Dumps the type list to TRACE

********************************************************************************************/

void CXaraFileRecord::DumpTypes(void)
{
	TRACEUSER( "Gerry", _T("Tag:% 5d  Size:% 6d  Data:"), Tag, GetSize());

	if (m_pTypeList == NULL ||
		m_pTypeList->GetCount() == 0)
	{
		TRACEUSER( "Gerry", _T(" None\n"));
		return;
	}

	BYTE* pPtr = m_pTypeList->GetBuffer();
	DWORD Count = m_pTypeList->GetCount();
	DWORD Index;
	for (Index = 0; Index < Count; Index++)
	{
		PTSTR			pType = NULL;
		DWORD len = 0;
		switch (pPtr[Index])
		{
		case FTT_BYTE:		pType = _T(" Byte");		break;
		case FTT_UINT32:		pType = _T(" ULong");		break;
		case FTT_INT32:		pType = _T(" Long");		break;
		case FTT_UINT16:	pType = _T(" UInt16");		break;
		case FTT_INT16:		pType = _T(" Int16");		break;
		case FTT_FLOAT:		pType = _T(" Float");		break;
		case FTT_DOUBLE:	pType = _T(" Double");		break;
		case FTT_WCHAR:		pType = _T(" WChar");		break;
		case FTT_ASCII:		pType = _T(" Ascii");		break;
		case FTT_UNICODE:	pType = _T(" Unicode");		break;
		case FTT_COORD:		pType = _T(" Coord");		break;
		case FTT_INTCOORD:	pType = _T(" IntCoord");	break;
		case FTT_BINHEX:
			{
				pType = _T(" Buffer");
				len = *((DWORD*)(pPtr+Index+1));	// Get the length (BYTE pointer arithmetic)
				Index += 4;							// and skip it
				break;
			}
		default:			pType = _T(" <BAD TYPE>");	break;
		}

		if (pType)
			TRACEUSER("Gerry", pType);
		if (len != 0)
			TRACEUSER( "Gerry", _T("(%d)"), len);
	}
	TRACEUSER( "Gerry", _T("\n"));
}


/********************************************************************************************

>	BOOL CXaraFileRecord::Reinit(UINT32 ThisTag,INT32 ThisSize)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/96
	Inputs:		ThisTag = tag for the record
				ThisSize= The exact size of the record in bytes, or CXF_UNKNOWN_SIZE
	Returns:	TRUE if initialised OK
				FALSE otherwise
	Purpose:	Allows you to reinit the record with a different record tag & size

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFileRecord::Reinit(UINT32 ThisTag,INT32 ThisSize)
{
	FreeBuffer();
	SetUpVars(ThisTag,ThisSize);
	return Init();
}

/********************************************************************************************

>	CXaraFileRecord* CXaraFileRecord::GetCopy()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96
	Inputs:		-
	Returns:	ptr to a CXaraFileRecord, which is an exact copy
	Purpose:	Copy function
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CXaraFileRecord* CXaraFileRecord::GetCopy()
{
	BOOL ok = TRUE;
	UINT32 Size = GetSize();

	CXaraFileRecord* pRecord = new CXaraFileRecord(GetTag(),Size);

	if (pRecord != NULL && Size > 0)
	{
		ok = pRecord->Init();

		if (ok)
		{
			BYTE* pSrcBuf  = GetBuffer();
			BYTE* pDestBuf = pRecord->GetBuffer();

			ok = (pSrcBuf != NULL) && (pDestBuf != NULL);
			if (ok) memcpy(pDestBuf,pSrcBuf,Size);
		}
	}

	if (pRecord != NULL && RecordNumber > 0)
		pRecord->SetRecordNumber(RecordNumber);

	if (!ok && pRecord != NULL)
	{
		delete pRecord;
		pRecord = NULL;
	}
	
	return pRecord;
}

/********************************************************************************************

>	UINT32 CXaraFileRecord::GetSize()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/96
	Inputs:		-
	Returns:	The size of the record
	Purpose:	Returns the size of the record.

				If the size of the record is known exactly, it returns the size of the buffer
				that's been allocated for the record.

				If it is not known (i.e. when creating a rec of unknown length for writing purposes)
				the current write position in the buffer is returned.  This is the dangerous case, because
				it is possible that the size returned is not the correct final size.

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 CXaraFileRecord::GetSize()
{
	if (SizeIsKnown)
		return BufferSize;
	else
	{
		//TRACEUSER( "Markn", _T("*+*+* Getting the size of a record, yet don't know it's exact size\n"));
		return CurrentPos;
	}
}

/********************************************************************************************

>	void CXaraFileRecord::FreeBuffer()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		-
	Returns:	-
	Purpose:	Deallocates the associated buffer, if it has one.
				Can be safely called if the record no longer has a buffer.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CXaraFileRecord::FreeBuffer()
{
	if (pBuffer != NULL)
	{
		CCFree(pBuffer);
		pBuffer = NULL;
		BufferSize = 0;
	}

	if (m_pTypeList != NULL)
	{
		delete m_pTypeList;
		m_pTypeList = NULL;
	}

	Initialised = FALSE;
}

/********************************************************************************************

>	BOOL CXaraFileRecord::AllocBuffer(INT32 Size)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		Size = Size of buffer in bytes
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Allocates a buffer of the given size.

				If Size == 0, no buffer is allocated, but TRUE is still returned.

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFileRecord::AllocBuffer(INT32 Size)
{
	ERROR3IF(Size < 0,"Buffer size is < 0, it's a bit difficult to allocate that many bytes");

	FreeBuffer();

	if (Size == 0)
		return TRUE;

	if (Size > 0)
		pBuffer = CCMalloc(Size);

	if (pBuffer != NULL)
	{
		BufferSize = Size;
		return TRUE;
	}
	else
		return FALSE;
}

/********************************************************************************************

>	BOOL CXaraFileRecord::ExtendBuffer(INT32 Size)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		Size = number of *extra* bytes to add to the buffer
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Extends the buffer by the given amount of bytes
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFileRecord::ExtendBuffer(INT32 Size)
{
	ERROR3IF(pBuffer == NULL,"No buffer to extend");
	if (pBuffer == NULL)
		return FALSE;

	pBuffer = CCRealloc(pBuffer,BufferSize+Size);

	if (pBuffer != NULL)
	{
		BufferSize += Size;
		return TRUE;
	}
	else
		return FALSE;
}

/********************************************************************************************

>	INT32 CXaraFileRecord::GetRecordNumber()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		-
	Returns:	The record number of this record
	Purpose:	Returns the record's number.
				This value is set by a call to SetRecordNumber()

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

INT32 CXaraFileRecord::GetRecordNumber()
{
	ERROR3IF(RecordNumber == 0,"Record number has not been set, or set incorrectly");

	return RecordNumber;
}

/********************************************************************************************

>	void CXaraFileRecord::SetRecordNumber(INT32 n)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		n = The record number of this record (must be > 0)
	Returns:	-
	Purpose:	Sets the record's number.
				This value is read by a call to GetRecordNumber()

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CXaraFileRecord::SetRecordNumber(UINT32 n)
{
	ERROR3IF(n < 1,"Record number is invalid.  Must be >= 1");

	RecordNumber = n;
}

/********************************************************************************************

>	BOOL CXaraFileRecord::Write(UINT32 n)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		n = a UINT32 value
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Writes the UINT32 to the record's data section

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFileRecord::WriteUINT32(UINT32 n)
{
	ENTERWRITEFUNCTION(FTT_UINT32);
	n = NativetoLE(n);
	BOOL ok = WriteBuffer((BYTE*)&n,sizeof(UINT32));
	LEAVEWRITEFUNCTION;
	return (ok);
}	

BOOL CXaraFileRecord::WriteINT32(INT32 n)
{
	ENTERWRITEFUNCTION(FTT_INT32);
	n = NativetoLE(n);
	BOOL ok = WriteBuffer((BYTE*)&n,sizeof(INT32));
	LEAVEWRITEFUNCTION;
	return (ok);
}	

BOOL CXaraFileRecord::WriteReference(INT32 n)
{
	ENTERWRITEFUNCTION(FTT_REFERENCE);
	n = NativetoLE(n);
	BOOL ok = WriteBuffer((BYTE*)&n,sizeof(INT32));
	LEAVEWRITEFUNCTION;
	return (ok);
}	

BOOL CXaraFileRecord::WriteFLOAT(FLOAT f)
{
	ENTERWRITEFUNCTION(FTT_FLOAT);
	BOOL ok = WriteBuffer((BYTE*)&f,sizeof(FLOAT));
	LEAVEWRITEFUNCTION;
	return (ok);
}	

BOOL CXaraFileRecord::WriteUINT16(UINT16 f)
{
	ENTERWRITEFUNCTION(FTT_UINT16);
	f = NativetoLE(f);
	BOOL ok = WriteBuffer((BYTE*)&f,sizeof(UINT16));
	LEAVEWRITEFUNCTION;
	return (ok);
}	

BOOL CXaraFileRecord::WriteINT16(INT16 f)
{
	ENTERWRITEFUNCTION(FTT_INT16);
	f = NativetoLE(f);
	BOOL ok = WriteBuffer((BYTE*)&f,sizeof(INT16));
	LEAVEWRITEFUNCTION;
	return (ok);
}	

BOOL CXaraFileRecord::WriteFIXED16(FIXED16 f)
{
	ENTERWRITEFUNCTION(FTT_INT32);
	f = NativetoLE(f);
	BOOL ok = WriteBuffer((BYTE*)&f,sizeof(FIXED16));
	LEAVEWRITEFUNCTION;
	return (ok);
}

BOOL CXaraFileRecord::WriteANGLE(ANGLE a)
{
	ENTERWRITEFUNCTION(FTT_INT32);
	a = NativetoLE(a);
	BOOL ok = WriteBuffer((BYTE*)&a,sizeof(ANGLE));
	LEAVEWRITEFUNCTION;
	return (ok);
}

BOOL CXaraFileRecord::WriteDOUBLE(double d)
{
	ENTERWRITEFUNCTION(FTT_DOUBLE);
	BOOL ok = WriteBuffer((BYTE*)&d,sizeof(double));
	LEAVEWRITEFUNCTION;
	return (ok);
}	

BOOL CXaraFileRecord::WriteWCHAR(WCHAR w)
{
	ENTERWRITEFUNCTION(FTT_WCHAR);
	w = NativeToUTF16(w);
	w = NativetoLE(w);
	BOOL ok = WriteBuffer((BYTE*)&w, SIZEOF_XAR_UTF16);	//sizeof(WCHAR));	2-byte UNICODE stored in Xar files
	LEAVEWRITEFUNCTION;
	return (ok);
}

/********************************************************************************************

>	BOOL CXaraFileRecord::WriteCoord(const DocCoord& Coord)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/96
	Inputs:		Coord = a coord
	Returns:	TRUE if written successfully
				FALSE otherwise
	Purpose:	Writes out the coord
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFileRecord::WriteCoord(const DocCoord& Coord)
{
	ENTERWRITEFUNCTION(FTT_COORD);
	BOOL ok = WriteINT32(Coord.x) && WriteINT32(Coord.y);
	LEAVEWRITEFUNCTION;
	return (ok);
}

BOOL CXaraFileRecord::WriteCoordTrans(const DocCoord& Coord,INT32 dx,INT32 dy)
{
	ENTERWRITEFUNCTION(FTT_COORD);
	BOOL ok = WriteINT32(Coord.x + dx) && WriteINT32(Coord.y + dy);
	LEAVEWRITEFUNCTION;
	return (ok);
}

/********************************************************************************************

>	virtual	BOOL CXaraFileRecord::WriteCoordInterleaved(const DocCoord& Coord)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/8/96
	Inputs:		Coord = coord to write to the record
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Writes out the coord to the record, in an interleaved format.

				The Coord is translated to the origin set up in the filter
				that was used to construct this object

				This version interleaves the bytes of the X & Y components, to try and
				maximise the run of common bytes, in order to improve the zlib compression.

				It writes out the top byte of x, then top byte of y, then next byte of x,
				then next byte of y, and so on.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFileRecord::WriteCoordInterleaved(const DocCoord& Coord)
{
	ENTERWRITEFUNCTION(FTT_INTCOORD);
	BOOL ok = WriteCoordTransInterleaved(Coord,0,0);
	LEAVEWRITEFUNCTION;
	return (ok);
}

BOOL CXaraFileRecord::WriteCoordTransInterleaved(const DocCoord& Coord,INT32 dx,INT32 dy)
{
	ENTERWRITEFUNCTION(FTT_INTCOORD);
	INT32 x = Coord.x + dx;
	INT32 y = Coord.y + dy;

	BOOL ok = TRUE;

	if (ok) ok = WriteBYTE(BYTE((x >> 24) & 0xff));
	if (ok) ok = WriteBYTE(BYTE((y >> 24) & 0xff));

	if (ok) ok = WriteBYTE(BYTE((x >> 16) & 0xff));
	if (ok) ok = WriteBYTE(BYTE((y >> 16) & 0xff));

	if (ok) ok = WriteBYTE(BYTE((x >>  8) & 0xff));
	if (ok) ok = WriteBYTE(BYTE((y >>  8) & 0xff));

	if (ok) ok = WriteBYTE(BYTE((x >>  0) & 0xff));
	if (ok) ok = WriteBYTE(BYTE((y >>  0) & 0xff));

	LEAVEWRITEFUNCTION;
	return ok;
}

/********************************************************************************************

>	BOOL CXaraFileRecord::WriteBuffer(BYTE* pBuf,UINT32 BufSize)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		pBuf	= ptr to a BYTE buffer
				BufSize	= the number of bytes in the buffer
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Writes out a buffer of BYTEs to the record's data section.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFileRecord::WriteBuffer(BYTE* pBuf,size_t BufSize)
{
	ERROR3IF(pBuf == NULL,"NULL pBuf param");

	BOOL ok = (pBuf != NULL);
	ENTERWRITEFUNCTION(FTT_BINHEX);
	STORELENGTH( DWORD(BufSize) );

	for (UINT32 i=0;i<BufSize && ok;i++)
		ok = WriteBYTE(pBuf[i]);

	LEAVEWRITEFUNCTION;
	return ok;
}

/********************************************************************************************

>	BOOL CXaraFileRecord::WriteCCPanose(const CCPanose &MyCCPanose)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		Panose	- CCPanose structure to write out
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Writes a CCPanose structure to th file

********************************************************************************************/

BOOL CXaraFileRecord::WriteCCPanose(const CCPanose &MyCCPanose)
{
	BOOL ok = TRUE;

PORTNOTE("other","Removed CCPanose usage - write NULL")
#ifndef EXCLUDE_FROM_XARALX
	if (ok) ok = WriteBYTE(MyCCPanose.GetFamilyType());
	if (ok) ok = WriteBYTE(MyCCPanose.GetSerifStyle());
	if (ok) ok = WriteBYTE(MyCCPanose.GetWeight());
	if (ok) ok = WriteBYTE(MyCCPanose.GetProportion());
	if (ok) ok = WriteBYTE(MyCCPanose.GetContrast());
	if (ok) ok = WriteBYTE(MyCCPanose.GetStrokeVariation());
	if (ok) ok = WriteBYTE(MyCCPanose.GetArmStyle());
	if (ok) ok = WriteBYTE(MyCCPanose.GetLetterform());
	if (ok) ok = WriteBYTE(MyCCPanose.GetMidline());
	if (ok) ok = WriteBYTE(MyCCPanose.GetXHeight());
#else
	if (ok) ok = WriteBYTE( 0 );
	if (ok) ok = WriteBYTE( 0 );
	if (ok) ok = WriteBYTE( 0 );
	if (ok) ok = WriteBYTE( 0 );
	if (ok) ok = WriteBYTE( 0 );
	if (ok) ok = WriteBYTE( 0 );
	if (ok) ok = WriteBYTE( 0 );
	if (ok) ok = WriteBYTE( 0 );
	if (ok) ok = WriteBYTE( 0 );
	if (ok) ok = WriteBYTE( 0 );
#endif
	return ok;
}

/********************************************************************************************

>	BOOL CXaraFileRecord::WriteUnicode(TCHAR* pStr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/96
	Inputs:		pStr = ptr to zero-terminated string
	Returns:	TRUE if written successfully
				FALSE otherwise
	Purpose:	Writes out the string as a Unicode string
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFileRecord::WriteUnicode(TCHAR* pStr)
{
	ERROR3IF(pStr == NULL,"NULL pStr");
	if (pStr == NULL)
		return FALSE;

	ENTERWRITEFUNCTION(FTT_UNICODE);
	BOOL ok = TRUE;
	
#ifdef _UNICODE

	// We must cope with byte-order differences between native storage and XAR file storage:
	// Native may be big-endian or little-endian, XAR is always little-endian
	// Native WCHAR may be 16 or 32 bits, XAR is always 16 bits
	// These differences are handled in WriteWCHAR
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
	// pStr points to an ASCII or DBCS string, and we want it written as a Unicode string
	// Write out each char separately, converting it to the correct Unicode value
	// using TextManager::MultiByteToUnicode()

	// skip along the string, saving each character in turn
	TCHAR* pCurrentChar = pStr;

	while (*pCurrentChar!=0 && ok)
	{
		// Convert the char to Unicode
		UINT32 mc = 0;
		if (UnicodeManager::IsDBCSOS() && UnicodeManager::IsDBCSLeadByte(*pCurrentChar))
		{
			mc = UnicodeManager::ComposeMultiBytes(*pCurrentChar, *(pCurrentChar+1));
			pCurrentChar += 2;
		}
		else
		{
			mc = *pCurrentChar;
			pCurrentChar += 1;
		}

		WCHAR wc = UnicodeManager::MultiByteToUnicode(mc);

		ok = WriteBuffer( (BYTE*)&wc, sizeof(WCHAR) );
	}

	// Now write a zero terminator
	if (ok)
	{
		WCHAR wc = 0;
		ok = WriteBuffer( (BYTE*)&wc, sizeof(WCHAR) );
	}

#endif	// _UNICODE

	LEAVEWRITEFUNCTION;
	return ok;
}

/********************************************************************************************

>	BOOL CXaraFileRecord::WriteASCII(TCHAR* pStr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/96
	Inputs:		pStr = ptr to zero-terminated string
	Returns:	TRUE if written successfully
				FALSE otherwise
	Purpose:	Writes out the string as an ASCII string
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFileRecord::WriteASCII(TCHAR* pStr)
{
	ERROR3IF(pStr == NULL,"NULL pStr");
	if (pStr == NULL)
		return FALSE;

	ENTERWRITEFUNCTION(FTT_ASCII);
	size_t len = camStrlen(pStr);
	BOOL ok= TRUE;

#ifdef UNICODE
	// Writing a Unicode string at pStr as an ASCII string
	// Just write out the first byte of each unicode char.

	for (INT32 i=0;ok && i<=len;i++)
		ok = WriteBYTE(pStr[i*2]);

	LEAVEWRITEFUNCTION;
	return ok;
#else
	// pStr points at an ASCII string, so write it out
	ok = WriteBuffer((BYTE*)pStr,len+1);
	LEAVEWRITEFUNCTION;
	return (ok);
#endif
}

/********************************************************************************************

>	BOOL CXaraFileRecord::WriteBSTR(_bstr_t bstr)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/01/2004
	Inputs:		bstr - ptr to zero-terminated WCHAR string
	Returns:	TRUE if written successfully
				FALSE otherwise
	Purpose:	Writes out the string as a Unicode string
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

/*BOOL CXaraFileRecord::WriteBSTR(_bstr_t bstr)
{
	ERROR3IF((wchar_t*)bstr == NULL, "NULL bstr");
	if ((wchar_t*)bstr == NULL)
		return FALSE;

	ENTERWRITEFUNCTION(FTT_UNICODE);

	BOOL ok = WriteBuffer((unsigned char*)((wchar_t*)bstr), bstr.length()*sizeof(WCHAR));
	ok = ok && WriteWCHAR(0);					// Zero terminate to be consistent with FTT_UNICODE

	LEAVEWRITEFUNCTION;

	return (ok);
} */

/********************************************************************************************

>	BOOL CXaraFileRecord::WriteUTF16STR(const StringVar& pvstr)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/Mar/2006
	Inputs:		pvstr - pointer to StringVar
	Outputs:	-
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Writes a variable length string as a stream of UTF16 unicode characters
	Errors:		-
	SeeAlso:	StartRecord(), EndRecord()

********************************************************************************************/

BOOL CXaraFileRecord::WriteUTF16STR(const StringVar& pvstr)
{
#ifdef UNICODE
	BOOL ok = TRUE;
	WCHAR c = 0;
	INT32 i = 0;

	do
	{
		c = pvstr.CharAt(i++);
		ok = WriteWCHAR(c);						// Read two bytes into the WCHAR buffer
		if (!ok) c = 0;							// If the read failed then write a terminator
	}
	while (c!=0);// Until end of string or no longer OK to write

	return ok;									// If we terminated due to Read failure tell the caller
#else
	ERROR3("StringVar only holds UNICODE data in UNICODE builds");
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL CXaraFileRecord::WriteBYTE(BYTE b)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		b = a BYTE
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Writes a BYTE to the record's data section

	Errors:		-
	SeeAlso:	StartRecord(), EndRecord()

********************************************************************************************/

BOOL CXaraFileRecord::WriteBYTE(BYTE b)
{
	ERROR3IF(!Initialised,"Writing to an uninitialised record");

	if (Overflow)
		return FALSE;

	ENTERWRITEFUNCTION(FTT_BYTE);
	BOOL ok = TRUE;

	if (CurrentPos >= BufferSize)
	{
		if (SizeIsKnown)
		{
			ERROR3("Record Write Overflow");
			Overflow = TRUE;
			LEAVEWRITEFUNCTION;
			return FALSE;
		}
		else
		{
			while (ok && CurrentPos >= BufferSize)
				ok = ExtendBuffer(CXF_RECORD_CHUNK);
		}
	}

	if (ok)
	{
		BYTE* pBuf = (BYTE*)pBuffer;

		if (CurrentPos < BufferSize)
			pBuf[CurrentPos++] = b;
	}

	LEAVEWRITEFUNCTION;
	return ok;
}

/********************************************************************************************

>	void CXaraFileRecord::ResetReadPos()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		-
	Returns:	-
	Purpose:	Resets the read position, so that the next Read() func call starts from the beginning
				of the data section.
	Errors:		-
	SeeAlso:	StartRecord(), EndRecord()

********************************************************************************************/

void CXaraFileRecord::ResetReadPos()
{
	CurrentPos = 0;
}

/********************************************************************************************

>	BOOL CXaraFileRecord::ReadBYTE(BYTE* pBYTE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		pBYTE = ptr to place to stick data
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Reads a BYTE from the record's data section

	Errors:		-
	SeeAlso:	StartRecord(), EndRecord()

********************************************************************************************/

BOOL CXaraFileRecord::ReadBYTE(BYTE* pBYTE)
{
	ERROR3IF(pBYTE == NULL,"pBYTE is NULL");
	if (pBYTE == NULL)
		return FALSE;

	ERROR3IF(CurrentPos >= BufferSize,"No more data in record");
	if (CurrentPos >= BufferSize)
		return FALSE;

	BYTE* pBuf = (BYTE*)pBuffer;
	*pBYTE = pBuf[CurrentPos++];

	return TRUE;
}


/********************************************************************************************

>	BOOL CXaraFileRecord::ReadBYTEtoBOOL(BOOL* pbool)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/Feb/2006
	Inputs:		pbool = ptr to place to stick data
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Reads a BYTE from the record's data section

	Errors:		-
	SeeAlso:	StartRecord(), EndRecord()

********************************************************************************************/

BOOL CXaraFileRecord::ReadBYTEtoBOOL(BOOL* pbool)
{
	ERROR3IF(pbool == NULL,"pbool is NULL");
	if (pbool == NULL)
		return FALSE;

	ERROR3IF(CurrentPos >= BufferSize,"No more data in record");
	if (CurrentPos >= BufferSize)
		return FALSE;

	BYTE* pBuf = (BYTE*)pBuffer;
	*pbool = (BOOL)pBuf[CurrentPos++];

	return TRUE;
}


/********************************************************************************************

>	BOOL CXaraFileRecord::ReadBYTEnoError(BOOL* pBYTE)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/02/2001
	Returns:	TRUE if successfully read a BYTE,
				FALSE if we didn't read a BYTE for any reason.
	Purpose:	Dangerous, no-error version of CXaraFileRecord::ReadBYTE().

				!!! DO NOT USE UNLESS YOU ***KNOW EXACTLY*** WHAT YOU ARE DOING !!!

	See Also:	ReadBuffernoError() .

********************************************************************************************/
BOOL CXaraFileRecord::ReadBYTEnoError(BYTE* pBYTE)
{
	ERROR3IF(pBYTE == NULL, "pBYTE is NULL");
	if (pBYTE == NULL)
		return FALSE;

	// if we run out of buffer, fail quietly (no error!).
	if (CurrentPos >= BufferSize)
		return FALSE;

	BYTE* pBuf = (BYTE*)pBuffer;
	*pBYTE = pBuf[CurrentPos++];

	return TRUE;
}


BOOL CXaraFileRecord::ReadBuffer(BYTE* pBuf,UINT32 BufSize)
{
	ERROR3IF(pBuf == NULL,"pBuf is NULL");
	if (pBuf == NULL)
		return FALSE;

	BOOL ok = TRUE;

	for (UINT32 i=0;ok && i < BufSize;i++)
		ok = ReadBYTE(pBuf+i);

	return ok;
}

/*	The following function allows us to still open up old CX2/CX3 files that do NOT contain
	profiling data.  This was necessary since the standard ReadBuffer will error on such a case.
	Obviously it is only CORRECT to call this function when we are trying to retain backward
	compatibility within CX3.
*/

BOOL CXaraFileRecord::ReadBuffernoError(BYTE* pBuf,UINT32 BufSize)
{
	ERROR3IF(pBuf == NULL,"pBuf is NULL");
	if (pBuf == NULL)
		return FALSE;

	BOOL ok = TRUE;

	if (CurrentPos >= BufferSize)
	{
		pBuf = NULL;
		return ok;
	}
	else
	{
		for (UINT32 i=0;ok && i < BufSize;i++)
		{
			ok = ReadBYTE(pBuf+i);
		}
	}

	return ok;
}

BOOL CXaraFileRecord::ReadUINT32(UINT32* pUINT32)
{
//	return ReadBuffer((BYTE*)pUINT32,sizeof(UINT32));
	BOOL ok = ReadBuffer((BYTE*)pUINT32,sizeof(UINT32));
	*pUINT32 = LEtoNative(*pUINT32);
	return ok;
}


BOOL CXaraFileRecord::ReadINT32(INT32* pINT32)
{
//	return ReadBuffer((BYTE*)pINT32,sizeof(INT32));
	BOOL ok = ReadBuffer((BYTE*)pINT32,sizeof(INT32));
	*pINT32 = LEtoNative(*pINT32);
	return ok;
}

BOOL CXaraFileRecord::ReadINT32noError(INT32* pINT32)
{
//	return ReadBuffernoError((BYTE*)pINT32,sizeof(INT32));
	BOOL ok = ReadBuffernoError((BYTE*)pINT32,sizeof(INT32));
	*pINT32 = LEtoNative(*pINT32);
	return ok;
}

BOOL CXaraFileRecord::ReadFIXED16(FIXED16* pn)
{
	BOOL ok = ReadBuffer((BYTE*)pn,sizeof(FIXED16));
	*pn = LEtoNative(*pn);
	return ok;
}

BOOL CXaraFileRecord::ReadANGLE(ANGLE *pa)
{
	BOOL ok = ReadBuffer((BYTE*)pa,sizeof(ANGLE));
	*pa = LEtoNative(*pa);
	return ok;
}

BOOL CXaraFileRecord::ReadUINT16(UINT16* pn)
{
	BOOL ok = ReadBuffer((BYTE*)pn,sizeof(UINT16));
	*pn = LEtoNative(*pn);
	return ok;
}

BOOL CXaraFileRecord::ReadINT16(INT16* pn)
{
	BOOL ok = ReadBuffer((BYTE*)pn,sizeof(INT16));
	*pn = LEtoNative(*pn);
	return ok;
}

BOOL CXaraFileRecord::ReadFLOAT(FLOAT* pf)
{
	return ReadBuffer((BYTE*)pf,sizeof(FLOAT));
}

BOOL CXaraFileRecord::ReadDOUBLE(double* pd)
{
	return ReadBuffer((BYTE*)pd,sizeof(double));
}

BOOL CXaraFileRecord::ReadDOUBLEnoError(double* pd)
{
	return ReadBuffernoError((BYTE*)pd,sizeof(double));
}

BOOL CXaraFileRecord::ReadWCHAR(WCHAR *pw)
{
	*pw = 0;
	BOOL ok = ReadBuffer((BYTE*)pw, SIZEOF_XAR_UTF16);	//sizeof(WCHAR)); 2-byte UNICODE stored in Xar files
	*pw = LEtoNative(*pw);
	*pw = UTF16ToNative(*pw);
	return ok;
}

BOOL CXaraFileRecord::ReadCoord(DocCoord* pCoord)
{
	if (pCoord != NULL)
		return ReadINT32(&(pCoord->x)) && ReadINT32(&(pCoord->y));
	else
		return FALSE;
}

BOOL CXaraFileRecord::ReadCoordTrans(DocCoord* pCoord,INT32 dx,INT32 dy)
{
	if (pCoord != NULL)
	{
		if (ReadINT32(&(pCoord->x)) && ReadINT32(&(pCoord->y)))
		{
			pCoord->x += dx;
			pCoord->y += dy;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CXaraFileRecord::ReadCoordInterleaved(DocCoord* pCoord)
{
	return ReadCoordTransInterleaved(pCoord,0,0);
}

BOOL CXaraFileRecord::ReadCoordTransInterleaved(DocCoord* pCoord,INT32 dx,INT32 dy)
{
	BOOL ok = TRUE;

	if (pCoord != NULL)
	{
		BYTE b;
		INT32 x = 0;
		INT32 y = 0;

		if (ok) { ok = ReadBYTE(&b); x += (b << 24); }
		if (ok) { ok = ReadBYTE(&b); y += (b << 24); }

		if (ok) { ok = ReadBYTE(&b); x += (b << 16); }
		if (ok) { ok = ReadBYTE(&b); y += (b << 16); }

		if (ok) { ok = ReadBYTE(&b); x += (b <<  8); }
		if (ok) { ok = ReadBYTE(&b); y += (b <<  8); }

		if (ok) { ok = ReadBYTE(&b); x += (b <<  0); }
		if (ok) { ok = ReadBYTE(&b); y += (b <<  0); }

		pCoord->x = x + dx;
		pCoord->y = y + dy;
	}

	return ok;
}

BOOL CXaraFileRecord::ReadCCPanose(CCPanose *pMyCCPanose)
{
	BOOL ok = TRUE;

	BYTE value;

PORTNOTE("other","Removed CCPanose usage - read to NULL")
#ifndef EXCLUDE_FROM_XARALX
	if (ok) ok = pMyCCPanose->SetFamilyType(value);
	if (ok) ok = pMyCCPanose->SetSerifStyle(value);
	if (ok) ok = pMyCCPanose->SetWeight(value);
	if (ok) ok = pMyCCPanose->SetProportion(value);
	if (ok) ok = pMyCCPanose->SetContrast(value);
	if (ok) ok = pMyCCPanose->SetStrokeVariation(value);
	if (ok) ok = pMyCCPanose->SetArmStyle(value);
	if (ok) ok = pMyCCPanose->SetLetterform(value);
	if (ok) ok = pMyCCPanose->SetMidline(value);
	if (ok) ok = pMyCCPanose->SetXHeight(value);
#else
	if (ok) ok = ReadBYTE(&value); // pMyCCPanose->SetFamilyType(value);
	if (ok) ok = ReadBYTE(&value); // pMyCCPanose->SetSerifStyle(value);
	if (ok) ok = ReadBYTE(&value); // pMyCCPanose->SetWeight(value);
	if (ok) ok = ReadBYTE(&value); // pMyCCPanose->SetProportion(value);
	if (ok) ok = ReadBYTE(&value); // pMyCCPanose->SetContrast(value);
	if (ok) ok = ReadBYTE(&value); // pMyCCPanose->SetStrokeVariation(value);
	if (ok) ok = ReadBYTE(&value); // pMyCCPanose->SetArmStyle(value);
	if (ok) ok = ReadBYTE(&value); // pMyCCPanose->SetLetterform(value);
	if (ok) ok = ReadBYTE(&value); // pMyCCPanose->SetMidline(value);
	if (ok) ok = ReadBYTE(&value); // pMyCCPanose->SetXHeight(value);
#endif
	return ok;
}

BOOL CXaraFileRecord::ReadUnicode(StringBase* pStr)
{
	// ASUMPTION!!!!
	// This function assumes the pStr->MaxLength() will return the max buffer size -1,
	// i.e. a value is returned that excludes room for a zero-terminated character.
	//
	// As the func that does all the hard work takes a TCHAR*, we need to pass in the total buffer
	// size, otherwise it will fail for strings that exactly fit into the StringBase object.
	//
	// This is why MaxLength()+1 is passed in.

	if (pStr != NULL)
		return ReadUnicode((TCHAR*)(*pStr),pStr->MaxLength()+1);

	return FALSE;
}

BOOL CXaraFileRecord::ReadUnicode(TCHAR* pStr,UINT32 MaxChars)
{
	BOOL ok = TRUE;
	BYTE* pBuf = (BYTE*)pStr;

#ifdef _UNICODE	
	BYTE b1 = 1;
	BYTE b2 = 1;
	BYTE* pBufEnd = pBuf + ( MaxChars * sizeof(wchar_t) );
	
	while (ok && !(b1 == 0 && b2 == 0))
	{
		ok = ReadBYTE( &b1 ) && ReadBYTE( &b2 );

		if (ok) ok = (pBuf < pBufEnd);
		if (ok) *pBuf++ = b1;
		if (ok) ok = (pBuf < pBufEnd);
		if (ok) *pBuf++ = b2;

		// Yick, Linux has 4 byte unicdoe chars
#if defined(__WXGTK__)
		*pBuf++ = '\0';
		*pBuf++ = '\0';
#endif		
		
	}
#else
	BYTE* pBufEnd = pBuf+MaxChars;
	WCHAR wc = 1;
	BYTE* pwcBuf = (BYTE*)&wc;

	while (ok && wc != 0)
	{
		// Read in the two bytes that make up the Unicode value, and stuff them into 'wc'
		ok = ReadBYTE(pwcBuf) && ReadBYTE(pwcBuf+1);

		// Check for buffer overflow
		if (ok) ok = (pBuf < pBufEnd);

		if (ok)
		{
			// Convert to an ASCII or DBCS
			if (UnicodeManager::IsDBCSOS())
			{
				UINT32 AsciiCode = UnicodeManager::UnicodeToMultiByte(wc);
				BYTE LeadByte = 0;
				BYTE TrailByte = 0;
				UnicodeManager::DecomposeMultiBytes(AsciiCode, &LeadByte, &TrailByte);

				if (LeadByte == 0)
					*pBuf++ = TrailByte;
				else
				{
					if (UnicodeManager::IsDBCSLeadByte(LeadByte))
					{
						ok = (pBuf < pBufEnd-1);
						if (ok)
						{
							*pBuf++ = LeadByte;
							*pBuf++ = TrailByte;
						}
					}
					else
						*pBuf++ = TrailByte;
				}
			}
			else
			{
				UINT32 AsciiCode = UnicodeManager::UnicodeToMultiByte(wc);
				*pBuf++ = BYTE(AsciiCode & 0xff);
			}
		}
	}

#endif // _UNICODE

	return ok;
}


BOOL CXaraFileRecord::ReadASCII(TCHAR* pStr,UINT32 MaxChars)
{
	BOOL ok = TRUE;
	BYTE b=1;
	BYTE* pBuf = (BYTE*)pStr;

#ifdef _UNICODE	
	BYTE* pBufEnd = pBuf+(MaxChars*2);

	while (ok && b != 0)
	{
		ok = ReadBYTE(&b);

		if (ok) ok = (pBuf < pBufEnd);
		if (ok) *pBuf++ = b;
		if (ok) ok = (pBuf < pBufEnd);
		if (ok) *pBuf++ = 0;
	}
#else
	BYTE* pBufEnd = pBuf+MaxChars;

	while (ok && b != 0)
	{
		ok = ReadBYTE(&b);

		if (ok) ok = (pBuf < pBufEnd);
		if (ok) *pBuf++ = b;
	}
#endif

	return ok;
}

#ifdef _UNICODE	
BOOL CXaraFileRecord::ReadASCII( char *pStr, UINT32 MaxChars )
{
	BOOL ok = TRUE;
	BYTE b=1;
	BYTE* pBuf = (BYTE*)pStr;

	BYTE* pBufEnd = pBuf+MaxChars;

	while (ok && b != 0)
	{
		ok = ReadBYTE(&b);

		if (ok) ok = (pBuf < pBufEnd);
		if (ok) *pBuf++ = b;
	}

	return ok;
}
#endif

/********************************************************************************************

>	BOOL CXaraFileRecord::ReadBSTR(_bstr_t* pbstr, UINT32 MaxChars)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/01/2004
	Inputs:		bstr - pointer to WCHAR buffer
				MaxChars - size of buffer in WCHARS
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Reads a BSTR from the record's data section (i.e. a run of WCHARs)

	Errors:		-
	SeeAlso:	StartRecord(), EndRecord()

********************************************************************************************/

/*BOOL CXaraFileRecord::ReadBSTR(_bstr_t* pbstr, UINT32 MaxChars)
{
	BOOL ok = TRUE;
	WCHAR c[2] = {0, 0};
	*pbstr = "";

	do
	{
		ok = ReadWCHAR(c);						// Read two bytes into the WCHAR buffer
		if (!ok) c[0]=0;						// If the read failed then write a terminator
		*pbstr += c;							// Add the character to the BSTR
	}
	while (c[0]!=0 && pbstr->length()<MaxChars);// Until end of string or out of room in buffer

	return ok;									// If we terminated due to Read failure tell the caller
} */




/********************************************************************************************

>	BOOL CXaraFileRecord::ReadUTF16STR(StringVar* pvstr, UINT32 MaxChars)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/Jan/2006
	Inputs:		vstr - pointer to StringVar
				MaxChars - size of buffer in WCHARS
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Reads a BSTR from the record's data section (i.e. a run of WCHARs)

	Errors:		-
	SeeAlso:	StartRecord(), EndRecord()

********************************************************************************************/

BOOL CXaraFileRecord::ReadUTF16STR(StringVar* pvstr, UINT32 MaxChars)
{
#ifdef UNICODE
	BOOL ok = TRUE;
	pvstr->Empty();
	WCHAR c = 0;

	do
	{
		ok = ReadWCHAR(&c);						// Read two bytes into the WCHAR buffer
		if (!ok) c = 0;							// If the read failed then write a terminator
		*pvstr += c;							// Add the character to the VSTR
	}
	while (c!=0 && ((UINT32)(pvstr->Length()))<MaxChars);// Until end of string or out of room in buffer

//TRACEUSER("Phil", _T("ReadUTF16: %s\n"), (TCHAR*)*pvstr);

	return ok;									// If we terminated due to Read failure tell the caller
#else
	ERROR3("StringVar only holds UNICODE data in UNICODE builds");
	return FALSE;
#endif
}




BOOL CXaraFileRecord::WritePath(Path* pPath)
{
	return WritePathTrans(pPath,0,0);
}

BOOL CXaraFileRecord::WritePathTrans(Path* pPath,INT32 dx,INT32 dy)
{
	if (pPath == NULL)
		return FALSE;

	INT32 NumCoords = pPath->GetNumCoords();

	// Write out num coords
	BOOL ok = WriteINT32(NumCoords);

	// Write out path verbs
	if (ok)
	{
		PathVerb* pVerb = pPath->GetVerbArray();
		if (pVerb != NULL)
		{
			for (INT32 i=0;ok && i < NumCoords;i++)
				ok = WriteBYTE(pVerb[i]);
		}
		else
			ok = FALSE;
	}

	// Write out coord array
	if (ok)
	{
		DocCoord* pCoord = pPath->GetCoordArray();
		if (pCoord != NULL)
		{
			for (INT32 i=0;ok && i < NumCoords;i++)
				ok = WriteCoordTrans(pCoord[i],dx,dy);
		}
		else
			ok = FALSE;
	}

	return ok;
}

BOOL CXaraFileRecord::ReadPath(Path* pPath)
{
	return ReadPathTrans(pPath,0,0);
}


BOOL CXaraFileRecord::ReadPathTrans(Path* pPath,INT32 dx,INT32 dy)
{
	if (pPath == NULL)
		return FALSE;

	INT32 NumCoords;
	if (!ReadINT32(&NumCoords))
		return FALSE;

	if (NumCoords < 0)
		return TRUE;

	BOOL ok = pPath->ClearPath(FALSE) && pPath->MakeSpaceAtEnd(NumCoords);

	if (ok)
	{
		INT32 i;

		PathVerb* pVerb = pPath->GetVerbArray();

		ok = (pVerb != NULL);
		for (i=0;ok && i<NumCoords;i++)
			ok = ReadBYTE(pVerb+i);

		DocCoord* pCoord = pPath->GetCoordArray();

		if (ok) ok = (pCoord != NULL);
		for (i=0;ok && i<NumCoords;i++)
			ok = ReadCoordTrans(pCoord+i,dx,dy);

		if (ok) ok = pPath->ExternalArraysAdded(NumCoords);
	}

	return ok;
}

BOOL CXaraFileRecord::WritePathRelative(Path* pPath)
{
	return WritePathRelativeTrans(pPath,0,0);
}

BOOL CXaraFileRecord::WritePathRelativeTrans(Path* pPath,INT32 dx,INT32 dy)
{
	if (pPath == NULL)
		return FALSE;

	BOOL ok = TRUE;

	INT32 NumCoords = pPath->GetNumCoords();

#ifndef RELPATHINTERLEAVE

	// Write out path verbs
	if (ok)
	{
		PathVerb* pVerb = pPath->GetVerbArray();
		if (pVerb != NULL)
		{
			for (INT32 i=0;ok && i < NumCoords;i++)
				ok = WriteBYTE(pVerb[i]);
		}
		else
			ok = FALSE;
	}

	// Write out coord array
	if (ok)
	{
		DocCoord* pCoord = pPath->GetCoordArray();
		if (pCoord != NULL)
		{
			ok = WriteCoordTrans(pCoord[0],dx,dy);

			for (INT32 i=1;ok && i < NumCoords;i++)
			{
				INT32 RelX = pCoord[i-1].x - pCoord[i].x;
				INT32 RelY = pCoord[i-1].y - pCoord[i].y;

						ok = WriteINT32(RelX);
				if (ok) ok = WriteINT32(RelY);
			}
		}
		else
			ok = FALSE;
	}
#else
	if (ok)
	{
		PathVerb* pVerb = pPath->GetVerbArray();
		DocCoord* pCoord = pPath->GetCoordArray();
		if (pVerb != NULL && pCoord != NULL && NumCoords > 0)
		{
			DocCoord TempCoord;

			ok = WriteBYTE(pVerb[0]);
			if (ok) ok = WriteCoordTransInterleaved(pCoord[0],dx,dy);

			for (INT32 i=1;ok && i < NumCoords;i++)
			{
				ok = WriteBYTE(pVerb[i]);

				TempCoord.x = pCoord[i-1].x - pCoord[i].x;
				TempCoord.y = pCoord[i-1].y - pCoord[i].y;

				if (ok) ok = WriteCoordTransInterleaved(TempCoord,0,0);
			}
		}
		else
			ok = FALSE;
	}
#endif

	return ok;
}

BOOL CXaraFileRecord::ReadPathRelative(Path* pPath)
{
	return ReadPathRelativeTrans(pPath,0,0);
}


BOOL CXaraFileRecord::ReadPathRelativeTrans(Path* pPath,INT32 dx,INT32 dy)
{
	if (pPath == NULL)
		return FALSE;

	INT32 NumCoords = INT32(GetSize())/(sizeof(BYTE)+sizeof(INT32)+sizeof(INT32));

	if (NumCoords < 0)
		return TRUE;

	BOOL ok = pPath->ClearPath(FALSE) && pPath->MakeSpaceAtEnd(NumCoords);

#ifndef RELPATHINTERLEAVE

	if (ok)
	{
		INT32 i;

		PathVerb* pVerb = pPath->GetVerbArray();

		ok = (pVerb != NULL);
		for (i=0;ok && i<NumCoords;i++)
			ok = ReadBYTE(pVerb+i);

		DocCoord* pCoord = pPath->GetCoordArray();

		if (ok) ok = (pCoord != NULL);
		if (ok) ok = ReadCoordTrans(pCoord,dx,dy);

		if (ok)
		{
			INT32 RelX,RelY;

			for (i=1;ok && i<NumCoords;i++)
			{
						ok = ReadINT32(&RelX);
				if (ok) ok = ReadINT32(&RelY);

				pCoord[i].x = pCoord[i-1].x - RelX;
				pCoord[i].y = pCoord[i-1].y - RelY;
			}
		}
	}
#else
	if (ok)
	{
		PathVerb* pVerb = pPath->GetVerbArray();
		DocCoord* pCoord = pPath->GetCoordArray();
		ok = (pVerb != NULL) && (pCoord != NULL);

		if (ok) ok = ReadBYTE(pVerb);
		if (ok) ok = ReadCoordTransInterleaved(pCoord,dx,dy);

		if (ok)
		{
			DocCoord TempCoord;

			for (INT32 i=1;ok && i<NumCoords;i++)
			{
				ok = ReadBYTE(pVerb+i);
				if (ok) ok = ReadCoordTransInterleaved(&TempCoord,0,0);

				pCoord[i].x = pCoord[i-1].x - TempCoord.x;
				pCoord[i].y = pCoord[i-1].y - TempCoord.y;
			}
		}
	}
#endif

	if (ok) ok = pPath->ExternalArraysAdded(NumCoords);

	return ok;
}

BOOL CXaraFileRecord::WriteMatrix(const Matrix & m)
{
	return WriteMatrixTrans(m,0,0);
}

BOOL CXaraFileRecord::WriteMatrixTrans(const Matrix & m,INT32 dx,INT32 dy)
{
	BOOL ok = TRUE;
	
	FIXED16 abcd[4];		// matrix components
	INT32 ef[2];				// translation

	m.GetComponents(&abcd[0], &ef[0]);

	if (ok) ok = WriteFIXED16(abcd[0]);
	if (ok) ok = WriteFIXED16(abcd[1]);
	if (ok) ok = WriteFIXED16(abcd[2]);
	if (ok) ok = WriteFIXED16(abcd[3]);

	if (ok) ok = WriteINT32(ef[0]+dx);
	if (ok) ok = WriteINT32(ef[1]+dy);

	return ok;
}

BOOL CXaraFileRecord::ReadMatrix(Matrix *pMatrix)
{
	return ReadMatrixTrans(pMatrix,0,0);
}

BOOL CXaraFileRecord::ReadMatrixTrans(Matrix *pMatrix,INT32 dx,INT32 dy)
{
	BOOL ok = TRUE;

	FIXED16 a;
	FIXED16 b;
	FIXED16 c;
	FIXED16 d;
	INT32 e;
	INT32 f;

	if (ok) ok = ReadFIXED16(&a);
	if (ok) ok = ReadFIXED16(&b);
	if (ok) ok = ReadFIXED16(&c);
	if (ok) ok = ReadFIXED16(&d);
	if (ok) ok = ReadINT32(&e);
	if (ok) ok = ReadINT32(&f);

	(*pMatrix) = Matrix(a,b,c,d,e+dx,f+dy);

	return ok;
}

/********************************************************************************************

>	virtual	BOOL CXaraFileRecord::WriteXOrd(INT32 XOrdinate)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/96
	Inputs:		XOrdinate = x ordinate to write to the record
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Writes out the x ordinate to the record.

				Base class performs the same function as WriteINT32()
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFileRecord::WriteXOrd(INT32 XOrdinate)
{
	return WriteINT32(XOrdinate);
}

BOOL CXaraFileRecord::WriteYOrd(INT32 YOrdinate)
{
	return WriteINT32(YOrdinate);
}

/********************************************************************************************

>	virtual	BOOL CXaraFileRecord::ReadXOrd(INT32* pXOrdinate)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/96
	Inputs:		pXOrdinate = ptr place to stick the x ordinate read in
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Reads the x ordinate from the record.

				Base class performs the same function as ReadINT32()
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXaraFileRecord::ReadXOrd(INT32* pXOrdinate)
{
	return ReadINT32(pXOrdinate);
}

BOOL CXaraFileRecord::ReadYOrd(INT32* pYOrdinate)
{
	return ReadINT32(pYOrdinate);
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

/********************************************************************************************

>	CamelotFileRecord::CamelotFileRecord(BaseCamelotFilter* pFilter,UINT32 Tag,INT32 Size)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/96
	Inputs:		pFilter - ptr to the filter doing the importing/exporting
				Tag		- Tag value for the record
				Size	- The exact size of the record in bytes, or CXF_UNKNOWN_SIZE
	Purpose:	Creates a record object.
				These are used by CXaraFile for reading and writing records.

				This derived version sets up the coord origin using the supplied BaseCamelotFilter.
				This origin is used when writing & reading coords.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CamelotFileRecord::CamelotFileRecord(BaseCamelotFilter* pFilter,UINT32 Tag,INT32 Size) : CXaraFileRecord(Tag,Size)
{
	CoordOrigin = DocCoord(0,0);

	if (pFilter != NULL)
		CoordOrigin = pFilter->GetCoordOrigin();
}

/********************************************************************************************

>	CamelotFileRecord::~CamelotFileRecord()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/96
	Purpose:	Default destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CamelotFileRecord::~CamelotFileRecord()
{
}

/********************************************************************************************

>	virtual	BOOL CamelotFileRecord::WritePath(Path* pPath)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/96
	Inputs:		pPath = ptr to a path
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Writes out the path to the record.

				All Coords in the path are translated to the origin set up in the filter
				that was used to construct this object
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CamelotFileRecord::WritePath(Path* pPath)
{
	return WritePathTrans(pPath,-CoordOrigin.x,-CoordOrigin.y);
}

/********************************************************************************************

>	virtual	BOOL CamelotFileRecord::WritePathRelative(Path* pPath)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/96
	Inputs:		pPath = ptr to a path
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Writes out the relative path to the record.

				All Coords in the path are translated to the origin set up in the filter
				that was used to construct this object
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CamelotFileRecord::WritePathRelative(Path* pPath)
{
	return WritePathRelativeTrans(pPath,-CoordOrigin.x,-CoordOrigin.y);
}

/********************************************************************************************

>	virtual	BOOL CamelotFileRecord::WriteCoord(const DocCoord& Coord)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/96
	Inputs:		Coord = coord to write to the record
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Writes out the coord to the record.

				The Coord is translated to the origin set up in the filter
				that was used to construct this object
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CamelotFileRecord::WriteCoord(const DocCoord& Coord)
{
	return WriteCoordTrans(Coord,-CoordOrigin.x,-CoordOrigin.y);
}

BOOL CamelotFileRecord::WriteCoordInterleaved(const DocCoord& Coord)
{
	return WriteCoordTransInterleaved(Coord,-CoordOrigin.x,-CoordOrigin.y);
}

BOOL CamelotFileRecord::WriteMatrix(const Matrix & m)
{
	return WriteMatrixTrans(m,-CoordOrigin.x,-CoordOrigin.y);
}

/********************************************************************************************

>	virtual	BOOL CamelotFileRecord::WriteXOrd(INT32 XOrdinate)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/96
	Inputs:		XOrdinate = x ordinate to write to the record
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Writes out the x ordinate to the record.

				The ordinate is translated to the origin set up in the filter
				that was used to construct this object
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CamelotFileRecord::WriteXOrd(INT32 XOrdinate)
{
	return WriteINT32(XOrdinate-CoordOrigin.x);
}

BOOL CamelotFileRecord::WriteYOrd(INT32 YOrdinate)
{
	return WriteINT32(YOrdinate-CoordOrigin.y);
}

/********************************************************************************************

>	virtual	BOOL CamelotFileRecord::ReadPath(Path* pPath)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/96
	Inputs:		pPath = ptr to a path
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Reads a path from the record.

				All Coords in the path are translated to the origin set up in the filter
				that was used to construct this object
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CamelotFileRecord::ReadPath(Path* pPath)
{
	return ReadPathTrans(pPath,CoordOrigin.x,CoordOrigin.y);
}

/********************************************************************************************

>	virtual	BOOL CamelotFileRecord::ReadPathRelative(Path* pPath)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/96
	Inputs:		pPath = ptr to a path
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Reads a relative path from the record.

				All Coords in the path are translated to the origin set up in the filter
				that was used to construct this object
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CamelotFileRecord::ReadPathRelative(Path* pPath)
{
	return ReadPathRelativeTrans(pPath,CoordOrigin.x,CoordOrigin.y);
}

/********************************************************************************************

>	virtual	BOOL CamelotFileRecord::ReadCoord(DocCoord* pCoord)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/96
	Inputs:		pCoord = ptr coord to read into
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Read in the coord from the record.

				The Coord is translated to the origin set up in the filter
				that was used to construct this object
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CamelotFileRecord::ReadCoord(DocCoord* pCoord)
{
	return ReadCoordTrans(pCoord,CoordOrigin.x,CoordOrigin.y);
}

BOOL CamelotFileRecord::ReadCoordInterleaved(DocCoord* pCoord)
{
	return ReadCoordTransInterleaved(pCoord,CoordOrigin.x,CoordOrigin.y);
}

BOOL CamelotFileRecord::ReadMatrix(Matrix* pm)
{
	return ReadMatrixTrans(pm,CoordOrigin.x,CoordOrigin.y);
}

/********************************************************************************************

>	virtual	BOOL CamelotFileRecord::ReadXOrd(INT32* pXOrdinate)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/96
	Inputs:		pXOrdinate = ptr place to stick the x ordinate read in
	Returns:	TRUE if OK, FALSE otherwise
	Purpose:	Reads the x ordinate from the record.

				The ordinate is translated to the origin set up in the filter
				that was used to construct this object
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CamelotFileRecord::ReadXOrd(INT32* pXOrdinate)
{
	if (ReadINT32(pXOrdinate))
	{
		*pXOrdinate += CoordOrigin.x;
		return TRUE;
	}

	return FALSE;
}

BOOL CamelotFileRecord::ReadYOrd(INT32* pYOrdinate)
{
	if (ReadINT32(pYOrdinate))
	{
		*pYOrdinate += CoordOrigin.y;
		return TRUE;
	}

	return FALSE;
}
