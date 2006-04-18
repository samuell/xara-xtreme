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

#include "ccbhfile.h"	// include the translating file proxy
//#include "tim.h"		// For error IDs

//-----------------------------------------------

CC_IMPLEMENT_DYNAMIC(CCBinHexFile, CCLexFile);

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


/********************************************************************************************

>	virtual CCFile& CCBinHexFile::write(const void *buf, UINT32 length = 1)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/07/97
	Inputs:		buf - pointer to buffer to write
				length - number of bytes to write
	Purpose:	Writes the buffer to the file

********************************************************************************************/

CCFile& CCBinHexFile::write(const void *buf, UINT32 length)
{
	// Number of bytes we can translate in one go (half the buffer)
	UINT32 Blocklen = BinHexMaxLineLength / 2;
	UINT32 Index;
	char* pPtr;
	BYTE* pSrc = (BYTE*)buf;
	BOOL bCRLF = (length > Blocklen);

	while (length > 0)
	{
		if (Blocklen > length)
			Blocklen = length;

		if (bCRLF)
		{
			if (m_pFile->write("\r\n", 2).fail())
			{
				GotError(_R(IDE_FILE_WRITE_ERROR));
				break;
			}
		}

		pPtr = (char*)m_Buffer;

		for (Index = 0; Index < Blocklen; Index++)
		{
			ByteToHex(*pSrc, pPtr);
			pSrc += 1;
			pPtr += 2;
		}

		if (m_pFile->write(m_Buffer, Blocklen * 2).fail())
		{
			GotError(_R(IDE_FILE_WRITE_ERROR));
			break;
		}

		length -= Blocklen;
	}

	return(*this);
}


/********************************************************************************************

>	virtual CCFile& CCBinHexFile::write(const StringBase& buf, UINT32 length = 0)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/07/97
	Inputs:		buf - string reference to write
				length - number of characters to write
	Purpose:	Writes the buffer to the file

********************************************************************************************/

CCFile& CCBinHexFile::write(const StringBase& buf, UINT32 length)
{
	if (length == 0)
		length = buf.Length();


#if 0 != wxUSE_UNICODE
	size_t				cchTCSrc = camWcstombs( NULL, (const TCHAR *)buf, 0 ) + 1;
	PSTR				pTCSrc = PSTR( alloca( cchTCSrc ) );
	camWcstombs( pTCSrc, (const TCHAR *)buf, cchTCSrc );
#else
	const char* pTCSrc = buf;
#endif

	const char* pSrc = (const char*) pTCSrc;
	
	// Number of chars we can translate in one go (max half of buffer size)
	UINT32 Blocklen = BinHexMaxLineLength / 2;
	UINT32 Index;
	char* pPtr;
	BOOL bCRLF = (length > Blocklen);

	while (length > 0)
	{
		if (Blocklen > length)
			Blocklen = length;

		if (bCRLF)
		{
			if (m_pFile->write("\r\n", 2).fail())
			{
				GotError(_R(IDE_FILE_WRITE_ERROR));
				break;
			}
		}

		pPtr = (char*)m_Buffer;

		for (Index = 0; Index < Blocklen; Index++)
		{
			ByteToHex(*pSrc, pPtr);
			pSrc += 1;
			pPtr += 2;
		}

		if (m_pFile->write(m_Buffer, Blocklen * 2).fail())
		{
			GotError(_R(IDE_FILE_WRITE_ERROR));
			break;
		}

		length -= Blocklen;
	}

	return(*this);
}


/********************************************************************************************

>	void CCBinHexFile::ByteToHex(BYTE b, char* pPtr)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/07/97
	Inputs:		b - byte to convert
	Outputs:	pPtr - pointer to at least two characters
	Purpose:	Translates the byte to hex (always writes two chars)

********************************************************************************************/

void CCBinHexFile::ByteToHex(BYTE b, char* pPtr)
{
	char nibble = (b >> 4);
	if (nibble > 9)
		nibble += 55;		// ('A' - 10)
	else
		nibble += '0';
	pPtr[0] = nibble;
	nibble = b & 0xF;
	if (nibble > 9)
		nibble += 55;		// ('A' - 10)
	else
		nibble += '0';
	pPtr[1] = nibble;
}




/********************************************************************************************

>	virtual CCFile& CCBinHexFile::write(char& buf)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/07/97
	Inputs:		buf - char reference???
	Purpose:	Writes the char to the file

********************************************************************************************/


CCFile& CCBinHexFile::write(char& buf)
{
	char Trans[2];

	ByteToHex((BYTE)buf, Trans);
	
	return(m_pFile->write(Trans, 2));
}
