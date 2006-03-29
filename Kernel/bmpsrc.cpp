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
#include "bmpsrc.h"
#include "ccbuffil.h"


// Place any IMPLEMENT type statements here
CC_IMPLEMENT_MEMDUMP(BitmapSource, CC_CLASS_MEMDUMP)


// Declare smart memory handling in Debug builds
// We want better memory tracking
#define new CAM_DEBUG_NEW


// Functions follow

/********************************************************************************************

>	BitmapSource::BitmapSource(OFFSET Size)
 					
 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
 	Created:	8/8/96
 
 	Inputs:		Size :	The size of the bitmap source (usually the file size). Must be
						greater than one.
 
 	Purpose:	Constructor providing BitmapSource objects of the given size.
				We construct objects of the required size, and associate bitmaps with
				them via SetOriginalSource
 
 	SeeAlso:	KernelBitmap::SetOriginalSource
 
********************************************************************************************/
BitmapSource::BitmapSource(OFFSET Size)
{
	m_pBuffer			= NULL;
	m_nSize				= 0;
	m_ReadWritePosition	= 0;	

	if (Size > 0)
	{
		m_pBuffer	= new BYTE[Size];
		m_nSize		= Size;
	}
}


/********************************************************************************************

>	BitmapSource::~BitmapSource()
 					
 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
 	Created:	18/08/96
 
 	Purpose:	Destructor for BitmapSource objects
 
********************************************************************************************/
BitmapSource::~BitmapSource()
{
	if (m_pBuffer != NULL)
	{
		delete m_pBuffer;
		m_pBuffer = NULL;
	}		
	m_nSize = 0;
}


/********************************************************************************************

>	BOOL BitmapSource::IsOK() const
 					
 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
 	Created:	8/8/96

	Returns:	TRUE if constructor succeeded
				FALSE otherwise
 
 	Purpose:	Since the constructor can fail, this member function determines whether or
				not it was wholly successful.
				Users should always call this function after constructing BitmapSource
				objects
 
 	SeeAlso:	BitmapSource::BitmapSource(...)
 
********************************************************************************************/
BOOL BitmapSource::IsOK() const
{
	return (m_pBuffer != NULL);
}


/********************************************************************************************
 
>	virtual BOOL BitmapSource::GetByte(BYTE& byte)
 					
 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/08/96
 
 	Outputs:	byte :	Returns the byte from the current position in the BitmapSource
 
 	Purpose:	Allows this BitmapSource to be copied elsewhere (to disk) a byte at a time.
	Notes:		SeekPos() should be called to reset the current position
	Errors:		ERROR2 if trying to read beyond the end of this BitmapSource

 
********************************************************************************************/
BOOL BitmapSource::GetByte(BYTE& byte)
{
	ERROR2IF(m_ReadWritePosition > GetSize(), FALSE, "m_ReadWritePosition > GetSize()");

	byte = m_pBuffer[m_ReadWritePosition];
	++m_ReadWritePosition;

	return TRUE;
}


/********************************************************************************************

>	virtual BOOL BitmapSource::PutByte(const BYTE byte)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/08/96

	Inputs:		byte:	a BYTE to place at the current position in this BitmapSource

	Returns:	TRUE if put succeeded
				FALSE otherwise
	Errors:		ERROR2 if trying to put after end of BitmapSource
	Purpose:	Adds a byte to this BitmapSorce object.

********************************************************************************************/
BOOL BitmapSource::PutByte(const BYTE byte)
{
	ERROR2IF(m_ReadWritePosition > GetSize(), FALSE, "m_ReadWritePosition > GetSize()");

	m_pBuffer[m_ReadWritePosition] = byte;
	++m_ReadWritePosition;

	return TRUE;
}


/********************************************************************************************

>	virtual FilePos BitmapSource::SeekPos(FilePos nPos)


	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/08/96

	Inputs:		nPos:	The position to be set

	Returns:	TRUE : if successful
				FALSE:	otherwise

	Purpose:	Sets the position in the BitmapSource at which the next GetByte/PutByte will
				occur. Usually this function will just be used to start from the beginning
				of the BitmapSource, by passing zero for nPos.

	Errors:		ERROR2 if trying to seek after the end of this BitmapSource

********************************************************************************************/
BOOL BitmapSource::SeekPos(OFFSET nPos)
{
	ERROR2IF(nPos > GetSize(), FALSE, "nPos > GetSize()");

	m_ReadWritePosition = nPos;
	return TRUE;
}


/********************************************************************************************

>	BOOL BitmapSource::AttachToBufferFile(CCBufferFile* pFile) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/08/96

	Returns:	A pointer to a CCBufferFile object which will provide a reference to this
				BitmapSource.
				NULL is returned if an error occurred

	Purpose:	The entry points to filters need a CCFile object. This function
				provides a CCFile object which uses this BitmapSource when calling the
				CCFile member functions.
	Notes:		The returned CCFile should be destroyed using delete when no longer
				needed

********************************************************************************************/
BOOL BitmapSource::AttachToBufferFile(CCBufferFile* pFile) const
{
	ERROR3IF(!pFile->IS_KIND_OF(CCBufferFile), "pFile is not");

	pFile->init(m_pBuffer, m_nSize);

	return TRUE;
}
