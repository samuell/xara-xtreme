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


#include "camtypes.h"
#include "bmpsrc.h"		// for OFFSET - put it in camtypes!
#include "ccbuffil.h"


// Place any IMPLEMENT type statements here
CC_IMPLEMENT_DYNAMIC(CCBufferFile, CCStreamFile)


// Declare smart memory handling in Debug builds
// We want better memory tracking
#define new CAM_DEBUG_NEW


// Functions follow

/********************************************************************************************

>	CCBufferFile::CCBufferFile(CCLexFile* pFile, BOOL bReportErrors, BOOL bThrowExceptions)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/96
	Inputs:		pFile:				A pointer to the stream to use
				bReportErrors:		See CCStreamFile for details
				bThrowExceptions:	See CCStreamFile for details
	
	Purpose:	Provides objects representing in-memory files.

	Notes:		This class is only partially implemented to provide support for BitmapSource

********************************************************************************************/
CCBufferFile::CCBufferFile(CCLexFile* pFile, BOOL bReportErrors, BOOL bThrowExceptions)
	: CCStreamFile(pFile->GetIOFile(), 0, bReportErrors, bThrowExceptions)
{
	m_pTrueFile				= pFile;

	m_pBuffer				= NULL;

	m_pBufferFillPosition	= NULL;
	m_BytesToFillBuffer		= 0;

	m_pBufferWritePosition	= NULL;
	m_BytesToWrite			= 0;
}



/********************************************************************************************

>	virtual CCBufferFile::~CCBufferFile()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/96
	Purpose:	Destructor - stops the base class destructor from deleting the stream we
				borrowed

********************************************************************************************/
CCBufferFile::~CCBufferFile()
{
	IOFile = NULL;
}

/********************************************************************************************

>	virtual void CCBufferFile::init(BYTE* pBuffer, FilePos size)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/96
	Purpose:	Initializes the buffer associated with this CCBufferFile
				Having constructed a CCBufferFile object a user can initialize the buffer
				associated with it. Useful for BitmapSources

********************************************************************************************/
void CCBufferFile::init(BYTE* pBuffer, FilePos size)
{
	m_pBuffer				= pBuffer;

	m_pBufferFillPosition	= pBuffer;
	m_BytesToFillBuffer		= size;

	m_pBufferWritePosition	= pBuffer;
	m_BytesToWrite			= size;
}


/********************************************************************************************

>	virtual BOOL CCBufferFile::setMode(INT32 fileMode)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/96
	Returns:	TRUE
	Purpose:	Provides dummy implementation for pure function

********************************************************************************************/
BOOL CCBufferFile::setMode(INT32 fileMode)
{
	return TRUE;
}


/********************************************************************************************

>	virtual CCFile& CCBufferFile::read(void* pBuffer, UINT32 length)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/96
	Returns:	A reference to this CCFile
	Purpose:	Attempts to read the given number of bytes into the given buffer while at
				the same time builds up the whole stream in the buffer given in the
				constructor or via init()
	Errors:		ERROR3 if internal buffer not initialized

********************************************************************************************/
CCFile& CCBufferFile::read(void* pBuffer, UINT32 length)
{
	// Read in as many bytes as the internal buffer can take up to a maximum of the
	// number given in length
	INT32 BytesToRead = m_BytesToFillBuffer > length ? length : m_BytesToFillBuffer;

	ERROR3IF(m_pBufferFillPosition == NULL, "m_pBufferFillPosition == NULL");

	m_pTrueFile->read((BYTE*)m_pBufferFillPosition, BytesToRead);

	// Find the number of bytes actually read
	INT32 BytesRead = IOFile->gcount();

	// Copy the read data to the given buffer if any
	if (pBuffer != NULL)
	{
		memcpy(pBuffer, m_pBufferFillPosition, BytesRead);
	}

	// Update the internal buffer details
	m_pBufferFillPosition += BytesRead;
	m_BytesToFillBuffer -= BytesRead;

	return *this;
}



/********************************************************************************************

>	virtual BOOL CCBufferFile::bad() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/96
	Returns:	TRUE if the CCBufferFile is bad
				FALSE otherwise
	Purpose:	Mimics the functionality of the stream class bad() function

********************************************************************************************/
/*BOOL CCBufferFile::bad() const
{
	return IOFile->bad();
}
*/

/********************************************************************************************

>	virtual BOOL CCBufferFile::fail() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/96
	Returns:	TRUE if the CCBufferFile has failed
				FALSE otherwise
	Purpose:	Mimics the functionality of the stream class fail() function

********************************************************************************************/
/*BOOL CCBufferFile::fail() const
{
	return IOFile->fail();
}
*/

/********************************************************************************************

>	CCFile& CCBufferFile::write(const void* pBuffer, UINT32 length = 1)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/96
	Inputs:		pBuffer : Buffer to write from. If NULL internal buffer will be used
				length : the number of bytes to write
	Returns:	A reference to this CCFile
	Purpose:	Attempts to write out the given number of bytes to the CCFile given in the
				constructor.
				Supplying a pBuffer will cause the data in the given buffer to be copied to
				the internal buffer before it is written out. If the write to file is
				unsuccessful therefore, the internal buffer can be considered corrupt (much
				as the file would be).
	Errors:		ERROR3 if internal buffer not initialized

********************************************************************************************/
CCFile& CCBufferFile::write(const void* pBuffer, UINT32 length)
{
	// Write as many bytes as the internal buffer will allow up to a maximum of the
	// number given in length
	INT32 BytesToWrite = m_BytesToWrite > length ? length : m_BytesToWrite;

	ERROR3IF(m_pBufferWritePosition == NULL, "m_pBufferWritePosition == NULL");

	if (pBuffer != NULL)
	{
		// Overwrite the data in the internal buffer with that in the given buffer
		memcpy(m_pBufferWritePosition, pBuffer, BytesToWrite);
	}
	// Write out whatever's in that position
	m_pTrueFile->write((BYTE*)m_pBufferWritePosition, BytesToWrite);
		

	// Update the internal buffer details
	m_pBufferWritePosition += BytesToWrite;
	m_BytesToWrite -= BytesToWrite;

	return *this;
}


/********************************************************************************************

>	BOOL CCBufferFile::IsAllWritten() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/96
	Returns:	TRUE : if all bytes in the buffer have been written
				FALSE : otherwise
	Purpose:	Determines whether write() has been called sufficiently often to have
				written out the whole buffer.

********************************************************************************************/
BOOL CCBufferFile::IsAllWritten() const
{
	return (m_BytesToWrite == 0);
}
