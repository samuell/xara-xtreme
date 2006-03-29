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
#include "camtypes.h"
#include "swfbitmp.h"

#define new CAM_DEBUG_NEW

/********************************************************************************************

>	FlashBitmapRecord::FlashBitmapRecord ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		-
	Returns:	-
	Purpose:	Creates a FlashBitmapRecord list node, and initialises all member variables.

********************************************************************************************/

FlashBitmapRecord::FlashBitmapRecord ( void )
{
	// Pointers should always be initialised to NULL when created.
	mpNext			= NULL;		// No subsequent nodes.
	mpLast			= NULL;		// No previous nodes.
	mpBitmap		= NULL;		// No bitmap attached.

	mIsContone		= FALSE;
	mBitmapID		= 0;

	mTransparency	= 255;		// The bitmap is (initially) transparent.
}

/********************************************************************************************

>	FlashBitmapRecord::~FlashBitmapRecord ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		-
	Returns:	-
	Purpose:	Destroys this instance of FlashBitmapRecord.

********************************************************************************************/

FlashBitmapRecord::~FlashBitmapRecord ( void )
{
	// Clean up any stray values.
	mpNext		= NULL;
	mpLast		= NULL;
	mpBitmap	= NULL;
}

/********************************************************************************************

>	FlashBitmapRecord* FlashBitmapRecord::AddElement ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		-
	Returns:	pTail - The pointer to the new node.
	Purpose:	Adds an element to the tail of the list.

********************************************************************************************/

FlashBitmapRecord* FlashBitmapRecord::AddElement ( void )
{
	FlashBitmapRecord *pTail = new FlashBitmapRecord;
	
	// Set the appropriate pointers.
	pTail->SetLast ( this );		// Ensure that a reference exists to this object...
	pTail->SetNext ( mpNext );		// Avoids any problems if mpLast isn't NULL.
	mpNext = pTail;					// ... and a reference exists to the new one.

	return pTail;
}

/********************************************************************************************

>	void FlashBitmapRecord::DeleteLastElement ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		-
	Returns:	-
	Purpose:	Deletes the previous item in the list.

********************************************************************************************/

void FlashBitmapRecord::DeleteLastElement ( void )
{
	FlashBitmapRecord *pToDelete = mpLast;
	
	// Reset mpLast to be mpLast->GetLast (), so that the list isn't broken.
	if ( mpLast != NULL )
		mpLast = mpLast->GetLast ();

	delete pToDelete;
}

/********************************************************************************************

>	void FlashBitmapRecord::DeleteNextElement ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		-
	Returns:	-
	Purpose:	Deletes the next item in the list.

********************************************************************************************/

void FlashBitmapRecord::DeleteNextElement ( void )
{
	FlashBitmapRecord *pToDelete = mpNext;
	
	// Reset mpNext to be mpNext->GetNext (), so that the list isn't broken.
	if ( mpNext != NULL )
		mpNext = mpNext->GetNext ();

	delete pToDelete;
}

/********************************************************************************************

>	OILBitmap* FlashBitmapRecord::GetBitmap ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		-
	Returns:	mpBitmap - A pointer to a kernel bitmap.
	Purpose:	Gets the value of mpBitmap.

********************************************************************************************/

OILBitmap* FlashBitmapRecord::GetBitmap ( void )
{
	return mpBitmap;
}

/********************************************************************************************

>	WORD FlashBitmapRecord::GetBitmapID ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		-
	Returns:	mBitmapID - The internal ID number used by the Flash file.
	Purpose:	Gets the value of mBitmapID.

********************************************************************************************/

WORD FlashBitmapRecord::GetBitmapID ( void )
{
	return mBitmapID;
}

/********************************************************************************************

>	DocColour FlashBitmapRecord::GetContoneStart ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/99
	Inputs:		-
	Returns:	mStartColour - The start colour for a contone operation.
	Purpose:	Gets the value of mStartColour.

********************************************************************************************/

DocColour FlashBitmapRecord::GetContoneStart ( void )
{
	return mStartColour;
}

/********************************************************************************************

>	DocColour FlashBitmapRecord::GetContoneEnd ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/99
	Inputs:		-
	Returns:	mEndColour - The end colour for a contone operation.
	Purpose:	Gets the value of mEndColour.

********************************************************************************************/

DocColour FlashBitmapRecord::GetContoneEnd ( void )
{
	return mEndColour;
}

/********************************************************************************************

>	BOOL FlashBitmapRecord::GetIsContone ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/99
	Inputs:		-
	Returns:	mIsContone - Is the bitmap contoned.
	Purpose:	Gets the value of mIsContone.

********************************************************************************************/

BOOL FlashBitmapRecord::GetIsContone ( void )
{
	return mIsContone;
}

/********************************************************************************************

>	UINT32 FlashBitmapRecord::GetTransparency ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/00
	Inputs:		-
	Returns:	mTransparency - The transparency channel of the bitmap.
	Purpose:	Gets the value of mTransparency.

********************************************************************************************/

UINT32 FlashBitmapRecord::GetTransparency ( void )
{
	return mTransparency;
}

/********************************************************************************************

>	FlashBitmapRecord* FlashBitmapRecord::GetLast ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		-
	Returns:	*mpLast - A pointer to the previous node in the linked list.
	Purpose:	Gets mpLast.

********************************************************************************************/

FlashBitmapRecord* FlashBitmapRecord::GetLast ( void )
{
	return mpLast;
}

/********************************************************************************************

>	FlashBitmapRecord* FlashBitmapRecord::GetNext ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		-
	Returns:	*mpNext - A pointer to the next node in the linked list.
	Purpose:	Gets mpNext.

********************************************************************************************/

FlashBitmapRecord* FlashBitmapRecord::GetNext ( void )
{
	return mpNext;
}

/********************************************************************************************

>	void FlashBitmapRecord::SetBitmap ( OILBitmap *pBitmap )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		pBitmap - A pointer to a kernel bitmap.
	Returns:	-
	Purpose:	Sets the value of mpBitmap.

********************************************************************************************/

void FlashBitmapRecord::SetBitmap ( OILBitmap *pBitmap )
{
	mpBitmap = pBitmap;
}

/********************************************************************************************

>	void FlashBitmapRecord::SetContoneColours ( const DocColour &Start,
												const DocColour &End )
	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/99
	Inputs:		Start	- The start colour for a contone fill.
				End		- The end colour for a contone fill.
	Returns:	-
	Purpose:	Sets the values of mStartColour and mEndColour.

********************************************************************************************/

void FlashBitmapRecord::SetContoneColours ( const DocColour &Start,
											const DocColour &End )
{
	// Set the values for the start and end colours.
	mStartColour	= Start;
	mEndColour		= End;
}

/********************************************************************************************

>	void FlashBitmapRecord::SetIsContone ( BOOL IsContone )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/99
	Inputs:		IsContone - Is the bitmap to be rendered as a contone bitmap.
	Returns:	-
	Purpose:	Sets the value of mIsContone.

********************************************************************************************/

void FlashBitmapRecord::SetIsContone ( BOOL IsContone )
{
	mIsContone = IsContone;
}

/********************************************************************************************

>	void FlashBitmapRecord::SetTransparency ( UINT32 Transparency )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/00
	Inputs:		Transparency - The transparency of the bitmap.
	Returns:	-
	Purpose:	Sets the value of mTransparency.

********************************************************************************************/

void FlashBitmapRecord::SetTransparency ( UINT32 Transparency )
{
	mTransparency = Transparency;
}

/********************************************************************************************

>	void FlashBitmapRecord::SetBitmapID ( WORD BitmapID )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		BitmapID - The ID value for this bitmap.
	Returns:	-
	Purpose:	Sets the value of mBitmapID.

********************************************************************************************/

void FlashBitmapRecord::SetBitmapID ( WORD BitmapID )
{
	mBitmapID = BitmapID;
}

/********************************************************************************************

>	void FlashBitmapRecord::SetLast ( FlashBitmapRecord *pLast )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		pLast - A pointer to the previous FlashBitmapRecord in the linked list.
	Returns:	-
	Purpose:	Sets the value of mpLast.

********************************************************************************************/

void FlashBitmapRecord::SetLast ( FlashBitmapRecord *pLast )
{
	mpLast = pLast;
}

/********************************************************************************************

>	void FlashBitmapRecord::SetNext ( FlashBitmapRecord *pNext )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		pNext - A pointer to the next FlashBitmapRecord in the linked list.
	Returns:	-
	Purpose:	Sets the value of mpNext.

********************************************************************************************/

void FlashBitmapRecord::SetNext ( FlashBitmapRecord *pNext )
{
	mpNext = pNext;
}
