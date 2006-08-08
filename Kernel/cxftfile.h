// $Id$
// Header for the CXaraTemplateFile class
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

#ifndef INC_CXFTFILE
#define INC_CXFTFILE

#include "cxfile.h"

/********************************************************************************************

>	class CXaraTemplateFile : public CXaraFile

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/97
	Purpose:	CXaraFile derived class for handling Flare template files
				This translates the records written by the standard native format export
				into the text based format.

				It achieves this by overriding certain functions to maintain a "type list"
				of the contents of the record so it can translate it when the record is 
				written to the file.

				Streamed records also need to be translated and this is achieved by returning
				a proxy CCFile derived class (CCBinHexFile) which translates all data written
				from binary to the BinHex format (a stream of hexadecimal digits where
				each pair of digits represents a byte e.g.

				void Translate(const BYTE* pBuf, char* pStr);
				{
					for (i = 0; i < BUFSIZE; i++)
					{
						sprintf(pStr, "%02x", pBuf[i]);
						pStr += 2;
					}
				}

				(Not that it'll actually use that code, rampant as it is... 8-)

	SeeAlso:	CCBinHexFile

********************************************************************************************/


class CXaraTemplateFile : public CXaraFile
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(CXaraTemplateFile);

public:
	CXaraTemplateFile();
	virtual ~CXaraTemplateFile() { /* empty */ }

	virtual BOOL OpenToWrite(CCLexFile* pCCFile);
	virtual BOOL OpenToRead(CCLexFile* pThisCCFile);
	virtual BOOL Close(void);

	// This is overriden to return our special CCBinHexFile
	virtual CCLexFile* GetCCFile(void) const;

	virtual INT32 StartRecord(UINT32 Tag, INT32 Size);
	virtual BOOL EndRecord(void);
	virtual INT32 StartStreamedRecord(UINT32 Tag, INT32 Size);
	virtual BOOL EndStreamedRecord(UINT32 *RecordSize);
	virtual UINT32 Write(CXaraFileRecord* pRecord);	// Writes a complete record out to file
	virtual UINT32 WriteDefinitionRecord(CXaraFileRecord* pRecord);	// Writes a complete record out to file
	virtual BOOL Write(BYTE b);

	virtual BOOL StartCompression() { return(TRUE); };
	virtual BOOL StopCompression() { return(TRUE); };
	virtual BOOL IsCompressionOn() { return(FALSE); };


protected:
	// Should only be used by people who know what they are doing e.g. compression
	// Hence why it is protected
	UINT32 WriteRecordHeader(UINT32 Tag, INT32 Size);
	virtual BOOL FixStreamedRecordHeader(UINT32 *RecordSize);

protected:	
	// Functions for translating binary into text
	BOOL WriteAsText(CXaraFileRecord* pRecord);
	BOOL WriteBinHex(BYTE* pBuf, UINT32 BufSize);
	BOOL WriteLabel(void);
	BOOL IsSimpleUnicode(UINT16* pStr);
	BOOL WriteSimpleUnicode(UINT16* pStr);
	BOOL WriteSimpleASCII(char* pStr);
	UINT32 WriteMultipleWCHARs(BYTE* pTypes, DWORD Count, UINT16* pChars);
	BOOL WriteSingleWCHAR(WCHAR ch);

	inline BOOL IsPrint(WCHAR wc)
	{
		return(((wc) > 31) && ((wc) < 127));
	};

protected:
	PathName m_TempPath;
	CCDiskFile* m_pTempFile;
	CCLexFile* m_pBinHexFile;
	CCLexFile* m_pOrigCCFile;
};

#endif	// INC_CXFTFILE
