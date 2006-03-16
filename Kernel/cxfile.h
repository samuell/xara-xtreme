// $Id: cxfile.h 662 2006-03-14 21:31:49Z alex $
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

#ifndef INC_CXFILE
#define INC_CXFILE

#include "doccoord.h"
#include "list.h"
#include "ccfile.h"

class CCLexFile;
class Progress;
class StringBase;
class BaseCamelotFilter;
class CCPanose;
class BitmapSource;

class CXaraFileRecord;
class CXaraFileRecordHandler;
class CamelotRecordHandler;
class StripSubTreeRecordHandler;
class CXaraFileMapTagToHandler;

/********************************************************************************************

>	class CXaraFile : public CCObject

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/96
	Base Class:	-
	Purpose:	Encapsulates the v2 file format functionality.
				It provides a standard way for reading and writing v2 format files.

				It uses a general file handling class (in this case CCFile) to do all the reading
				and writing to the file.  This class provides a record-based interface,
				which also allows this class (and asscociated class) to provide extra error handling
				functionality.

	SeeAlso:	CCFile

********************************************************************************************/


class CXaraFile : public CCObject
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(CXaraFile);

public:
	CXaraFile();
	virtual ~CXaraFile() { /* empty */ }

	virtual BOOL OpenToWrite(CCLexFile* pCCFile);
	virtual BOOL OpenToRead(CCLexFile* pCCFile);
	virtual BOOL Close();

	// Function to turn Compression on or off on the underlying CCFile
	// Reading
	BOOL SetCompression(BOOL NewState);

	// Writing
	virtual BOOL StartCompression();
	virtual BOOL StopCompression();
	virtual BOOL IsCompressionOn();

	// This allows people access to the CCFile which is being used
	// Note: only people who have a genuine reason need to access this - e.g. bitmap savers
	virtual CCLexFile* GetCCFile(void) const;

	// This allows the record size to be got at when reading
	UINT32 GetCurrentRecordSize();

	virtual INT32 StartRecord(UINT32 Tag,INT32 Size);
	virtual BOOL EndRecord(void);
	virtual INT32 StartStreamedRecord(UINT32 Tag,INT32 Size);
	virtual BOOL EndStreamedRecord(UINT32 *RecordSize);

	// These are used during writing???
	UINT32 GetRecordTag();
	INT32  GetRecordNum();

	// Write functions
	virtual BOOL Write(BYTE b);

	BOOL Write(UINT32 n);
	BOOL Write(INT32 n);
	BOOL Write(const DocCoord& Coord);
	BOOL Write(BYTE* pBuf,UINT32 Size);
	BOOL Write(FLOAT f);
	BOOL Write(double d);

	BOOL WriteWCHAR(WCHAR w);

	BOOL WriteCCPanose(const CCPanose& Panose);

	BOOL WriteBitmapSource(const BitmapSource& Source, UINT32 Height, BaseCamelotFilter* pThisFilter);

	BOOL Write(TCHAR* pStr);				// Calls WriteUnicode() (Use WriteASCII() for ASCII writing)
	BOOL WriteASCII(TCHAR* pStr);
	BOOL WriteUnicode(TCHAR* pStr);

	// Writes a complete record out to file
	// DO NOT use this for records which will be referenced
	virtual UINT32 Write(CXaraFileRecord* pRecord);

	// Writes a complete record out to file
	// Use this for records which will be referenced
	virtual UINT32 WriteDefinitionRecord(CXaraFileRecord* pRecord);

	// Read functions
	BOOL Read(UINT32* pUINT32);
	BOOL Read(BYTE* pBuf,UINT32 Size);
	BOOL Read(BYTE* pBYTE);
	BOOL Read(FLOAT* pf);
	BOOL Read(double* pd);

	BOOL ReadWCHAR(WCHAR *pw);

	BOOL ReadCCPanose(CCPanose *pPanose);

	BOOL Read(TCHAR* pStr,UINT32 MaxChars);
	BOOL ReadUnicode(TCHAR* pStr,UINT32 MaxChars);

	BOOL ReadNextRecord();
	BOOL ReadNextRecordHeader();

	virtual UINT32	GetNumBytesWritten()			{ return NumBytesWritten; }
	virtual UINT32	GetNumBytesRead()				{ return NumBytesRead; }
	virtual UINT32	GetTotalNumBytesToRead()		{ return TotalNumBytesToRead; }
	virtual void	SetTotalNumBytesToRead(UINT32 n)	{ TotalNumBytesToRead = n; }

	virtual FilePos GetFilePos();

	virtual BOOL SetUpHandlers(BaseCamelotFilter* pThisFilter);

	virtual void SetDefaultRecordHandler(CXaraFileRecordHandler* pHandler);
	virtual CXaraFileRecordHandler* GetDefaultRecordHandler();

	virtual void SetStripSubTreeRecordHandler(StripSubTreeRecordHandler* pHandler);
	virtual StripSubTreeRecordHandler* GetStripSubTreeRecordHandler();

	// This function will ensure that the next sub tree is thrown away.
	void StripNextSubTree();

	// Error function
	void GotError(UINT32 errorID);
	void GotError( UINT32 errorID , const TCHAR* errorString);

	// Record Handler registration
	BOOL RegisterRecordHandlers(List* pListOfHandlers);

	// Camelot-specific stuff
	void				SetFilter(BaseCamelotFilter* pThisFilter)	{ pFilter = pThisFilter; }
	BaseCamelotFilter*	GetFilter()									{ return pFilter; }
	UINT32				GetLastReadTag()							{ return ReadTag; }

protected:

	virtual CXaraFileRecordHandler* FindHandler(UINT32 Tag);
	virtual CXaraFileRecordHandler* SearchForHandler(UINT32 Tag);
	virtual CXaraFileRecordHandler* LookUpHandler(UINT32 Tag);

	virtual void SetUpHandlerMap();
	virtual void ResetHandlers();

protected:
	// Should only be used by people who know what they are doing e.g. compression
	// Hence why it is protected
	UINT32 WriteRecordHeader(UINT32 Tag, INT32 Size);

	virtual BOOL FixStreamedRecordHeader(UINT32 *RecordSize);

	virtual void IncNumBytesWritten(UINT32 n){ NumBytesWritten += n; }
	virtual void IncNumBytesRead(UINT32 n)	{ NumBytesRead	  += n; }

protected:
	CXaraFileRecord*	pRecord;
	BOOL				WriteToRecord;

// This rampant private changed by Gerry on 14/7/97
//private:
protected:
	CCLexFile*			pCCFile;
	INT32				RecordNumber;
	UINT32				NumBytesWritten;
	UINT32				NumBytesRead;
	UINT32				TotalNumBytesToRead;
	UINT32				ProgressID;

	// Header data for the record currently being read in
	UINT32				ReadTag;
	UINT32				ReadSize;

	// Record Handler list
	List* pRecordHandlerList;

	// The default record handler
	CXaraFileRecordHandler* pDefaultRecordHandler;
	CamelotRecordHandler* pStandardDefaultRecordHandler;

	// Handler used to strip sub trees when an unrecognised atomic record is read
	StripSubTreeRecordHandler* pStripSubTreeRecordHandler;
	StripSubTreeRecordHandler* pStandardStripSubTreeRecordHandler;

	// Streamed Record vars
	FilePos	StartOfStreamedRecord;
	BOOL	WritingStreamedRecord;
	BOOL	CompOffDueToStreamedRecord;

	// Camelot-specific stuff
	BaseCamelotFilter*			pFilter;
	CXaraFileMapTagToHandler*	pMap;
};

/********************************************************************************************

>	class NULLXaraFile : public CXaraFile

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/6/96
	Base Class:	CXaraFile
	Purpose:	Dummy CXaraFile that swallows up all bytes written to it

	SeeAlso:	CXaraFile

********************************************************************************************/

class NULLXaraFile : public CXaraFile
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(NULLXaraFile);

public:
	NULLXaraFile() { /* empty */ }

	virtual BOOL OpenToWrite(CCLexFile* pThisCCFile);
	virtual BOOL Close();
	virtual BOOL Write(BYTE b);

	virtual FilePos GetFilePos();

	virtual BOOL IsCompressionOn();
	virtual BOOL StartCompression();
	virtual BOOL StopCompression();

	// This allows people access to the CCFile which is being used
	// Note: only people who have a genuine reason need to access this - e.g. bitmap savers
	virtual CCLexFile* GetCCFile() const;

protected:
	virtual BOOL FixStreamedRecordHeader(UINT32 *RecordSize);
};

#endif	// INC_CXFILE
