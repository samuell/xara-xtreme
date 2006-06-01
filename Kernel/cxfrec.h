// $Id$
// Header for the controlling class of the v2 file format
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

#ifndef INC_CXFREC
#define INC_CXFREC

#include "doccoord.h"
#include "cxfdefs.h"

class DocCoord;
class Matrix;
class Path;
class BaseCamelotFilter;
class CCPanose;
class CCLexFile;

class FTTypeList;

//---------------------------------------------------------

#define CXF_RECORD_CHUNK 256

/********************************************************************************************

>	class CXaraFileRecord : public CCObject

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/96
	Purpose:	This is the class that encapsulates a record in the new v2 format.

********************************************************************************************/

class CXaraFileRecord : public CCObject
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(CXaraFileRecord);

public:
	CXaraFileRecord(UINT32 Tag,INT32 Size = -1);
	virtual ~CXaraFileRecord();

	void DumpTypes(void);
	void WriteAsText(CCLexFile* pFile);

	virtual	CXaraFileRecord* GetCopy();		// Returns a ptr to an exact copy

	virtual	BOOL Init(BOOL ZeroMemBlock = FALSE);
	virtual	BOOL Reinit(UINT32 ThisTag,INT32 Size);

	virtual	BOOL WriteBYTE(BYTE b);
	virtual	BOOL WriteUINT32(UINT32 n);
	virtual	BOOL WriteINT32(INT32 n);
	virtual BOOL WriteReference(INT32 n);
	virtual	BOOL WriteUINT16(UINT16 n);
	virtual	BOOL WriteINT16(INT16 n);
	virtual	BOOL WriteBuffer(BYTE* pBuf,size_t BufSize);
	virtual	BOOL WriteFLOAT(FLOAT f);
	virtual	BOOL WriteDOUBLE(double d);
	virtual	BOOL WriteFIXED16(FIXED16 n);
	virtual BOOL WriteANGLE(ANGLE a);
	virtual BOOL WriteWCHAR(WCHAR w);
	virtual BOOL WriteWCHARs(const WCHAR* pw, UINT32 Count);

	virtual	BOOL WriteCoord(const DocCoord& Coord);
	virtual	BOOL WriteCoordTrans(const DocCoord& Coord,INT32 dx,INT32 dy);
	virtual	BOOL WriteCoordInterleaved(const DocCoord& Coord);
	virtual	BOOL WriteCoordTransInterleaved(const DocCoord& Coord,INT32 dx,INT32 dy);
	virtual	BOOL WritePath(Path* pPath);
	virtual	BOOL WritePathRelative(Path* pPath);
	virtual	BOOL WritePathTrans(Path* pPath,INT32 dx,INT32 dy);
	virtual	BOOL WritePathRelativeTrans(Path* pPath,INT32 dx,INT32 dy);
	virtual	BOOL WriteXOrd(INT32 XOrdinate);
	virtual	BOOL WriteYOrd(INT32 YOrdinate);
	virtual BOOL WriteCCPanose(const CCPanose &MyCCPanose);

	virtual	BOOL WriteASCII(const TCHAR* pStr);
	virtual	BOOL WriteUnicode(const TCHAR* pStr);
PORTNOTE("other","WriteBSTR removed - Windows concept")
#ifndef EXCLUDE_FROM_XARALX
	virtual	BOOL WriteBSTR(_bstr_t bstr);
#endif
	virtual BOOL WriteUTF16STR(const StringVar& pvstr);

	virtual	BOOL WriteMatrix(const Matrix & m);
	virtual	BOOL WriteMatrixTrans(const Matrix & m,INT32 dx,INT32 dy);

	virtual	UINT32 GetTag()				{ return Tag; }
	virtual	UINT32 GetSize();
	virtual	BYTE* GetBuffer()			{ return (BYTE*)pBuffer; }

	virtual FTTypeList* GetTypeList(void)	{ return(m_pTypeList); };

	virtual	INT32  GetRecordNumber();
	virtual	void  SetRecordNumber(UINT32 n);

	virtual	BOOL IsDataSectionFull()	{ return (!(SizeIsKnown && CurrentPos < KnownSize)); }

	virtual	void ResetReadPos();	// Resets read pos to beginning of the record

	virtual	BOOL ReadBYTE(BYTE* pb);
	virtual	BOOL ReadBYTEtoBOOL(BOOL* pb);			// This function reads byte into BOOL
	virtual	BOOL ReadBYTEnoError(BYTE* pb);
	virtual	BOOL ReadUINT32(UINT32* pn);
	virtual	BOOL ReadINT32(INT32* pn);
	virtual	BOOL ReadINT32noError(INT32* pn);
	virtual	BOOL ReadUINT16(UINT16* pn);
	virtual	BOOL ReadINT16(INT16* pn);
	virtual	BOOL ReadBuffer(BYTE* pBuf,UINT32 BufSize);
	virtual	BOOL ReadBuffernoError(BYTE* pBuf,UINT32 BufSize);
	virtual	BOOL ReadFLOAT(FLOAT* pf);
	virtual	BOOL ReadDOUBLE(double* pd);
	virtual	BOOL ReadDOUBLEnoError(double* pd);
	virtual	BOOL ReadFIXED16(FIXED16 *pn);
	virtual BOOL ReadANGLE(ANGLE *pa);
	virtual BOOL ReadWCHAR(WCHAR *pw);

	virtual	BOOL ReadCoord(DocCoord* pCoord);
	virtual	BOOL ReadCoordTrans(DocCoord* pCoord,INT32 dx,INT32 dy);
	virtual	BOOL ReadCoordInterleaved(DocCoord* pCoord);
	virtual	BOOL ReadCoordTransInterleaved(DocCoord* pCoord,INT32 dx,INT32 dy);
	virtual	BOOL ReadPath(Path* pPath);
	virtual	BOOL ReadPathRelative(Path* pPath);
	virtual	BOOL ReadPathTrans(Path* pPath,INT32 dx,INT32 dy);
	virtual	BOOL ReadPathRelativeTrans(Path* pPath,INT32 dx,INT32 dy);
	virtual	BOOL ReadXOrd(INT32* pXOrdinate);
	virtual	BOOL ReadYOrd(INT32* pYOrdinate);
	virtual BOOL ReadCCPanose(CCPanose *pPanose);

	virtual	BOOL ReadUnicode(StringBase* pStr);
	virtual	BOOL ReadUnicode(TCHAR* pStr,UINT32 MaxChars);
	virtual	BOOL ReadASCII(TCHAR* pStr,UINT32 MaxChars);
#if defined(_UNICODE)
	virtual	BOOL ReadASCII( char *pStr, UINT32 MaxChars );
#endif
PORTNOTE("other","WriteBSTR removed - Windows concept")
#ifndef EXCLUDE_FROM_XARALX
	virtual	BOOL ReadBSTR(_bstr_t* bstr, UINT32 MaxChars);
#endif
	virtual BOOL ReadUTF16STR(StringVar* pvstr, UINT32 MaxChars);

	virtual	BOOL ReadMatrix(Matrix *pMatrix);
	virtual	BOOL ReadMatrixTrans(Matrix *pMatrix,INT32 dx,INT32 dy);

private:
	void SetUpVars(UINT32 ThisTag,INT32 Size);

	void FreeBuffer();
	BOOL AllocBuffer(INT32 Size);
	BOOL ExtendBuffer(INT32 Size);

	BOOL	Initialised;

	UINT32	Tag;
	UINT32	BufferSize;
	UINT32	KnownSize;
	BOOL	SizeIsKnown;
	INT32	RecordNumber;

	void*	pBuffer;
	UINT32	CurrentPos;
	BOOL	Overflow;

	FTTypeList* m_pTypeList;
};


/********************************************************************************************

>	class CamelotFileRecord : public CXaraFileRecord

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/96
	Purpose:	This derived class does all the things that are Camelot-specific.

********************************************************************************************/

class CamelotFileRecord : public CXaraFileRecord
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(CamelotFileRecord);

public:
	CamelotFileRecord(BaseCamelotFilter* pFilter,UINT32 Tag,INT32 Size = -1);
	~CamelotFileRecord();

#if !defined(EXCLUDE_FROM_XARLIB)
	// Translates the coords and the paths automatically on writing
	virtual	BOOL WriteCoord(const DocCoord& Coord);
	virtual	BOOL WriteCoordInterleaved(const DocCoord& Coord);
	virtual	BOOL WritePath(Path* pPath);
	virtual	BOOL WritePathRelative(Path* pPath);
	virtual	BOOL WriteMatrix(const Matrix & m);
	virtual	BOOL WriteXOrd(INT32 XOrdinate);
	virtual	BOOL WriteYOrd(INT32 YOrdinate);

	// Translates the coords and the paths automatically on reading
	virtual	BOOL ReadCoord(DocCoord* pCoord);
	virtual	BOOL ReadCoordInterleaved(DocCoord* pCoord);
	virtual	BOOL ReadPath(Path* pPath);
	virtual	BOOL ReadPathRelative(Path* pPath);
	virtual	BOOL ReadMatrix(Matrix *pMatrix);
	virtual	BOOL ReadXOrd(INT32* pXOrdinate);
	virtual	BOOL ReadYOrd(INT32* pYOrdinate);

private:
	DocCoord CoordOrigin;
#endif	// EXCLUDE_FROM_XARLIB
};

#endif	// INC_CXFREC
