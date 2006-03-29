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
#include "paths.h"
#include "swffont.h"

CC_IMPLEMENT_DYNAMIC( FlashFontRecord, CCObject )

#define new CAM_DEBUG_NEW

/********************************************************************************************

>	FlashFontRecord::FlashFontRecord ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/99
	Inputs:		-
	Returns:	-
	Purpose:	Creates a FlashFontRecord list node, and initialises any values.

********************************************************************************************/

FlashFontRecord::FlashFontRecord ( void )
{
	// Initialise the member variables, so as to avoid any unpleasentness later.
	mFontID		= 0;		// Set as 0.
	mTypeface	= 0;		// Default to 0.
	mIsBold		= FALSE;	// Initially the font isn't bold.
	mIsItalic	= FALSE;	// Initially the font isn't italic.

	// Pointers should always be initialised to NULL when created.
	mpLast		= NULL;		// No previous nodes.
	mpNext		= NULL;		// No subsequent nodes.

	// Ensure that the arrays are full of 0s.
	for ( UINT32 i = 0; i < FLASH_TEXT_ARRAY_SIZE; i++ )
	{
		mGlyphs [i] = 0;
		mPaths [i] = NULL;
	}
}

/********************************************************************************************

>	FlashFontRecord::~FlashFontRecord ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/99
	Inputs:		-
	Returns:	-
	Purpose:	Destroys this instance of FlashFontRecord.

********************************************************************************************/

FlashFontRecord::~FlashFontRecord ( void )
{
	// Clean up any stray values.
	for ( UINT32 i = 0; i < FLASH_TEXT_ARRAY_SIZE; i++ )
	{
		Path* pPath = mPaths[i];

		if(pPath)
			delete pPath;
	}
}

/********************************************************************************************

>	FlashFontRecord* FlashFontRecord::AddElement ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/99
	Inputs:		-
	Returns:	pTail - The pointer to the new node.
	Purpose:	Adds an element to the tail of the list.

********************************************************************************************/

FlashFontRecord* FlashFontRecord::AddElement ( void )
{
	FlashFontRecord *pTail = new FlashFontRecord;
	
	// Set the appropriate pointers.
	pTail->SetLast ( this );		// Ensure that a reference exists to this object...
	pTail->SetNext ( mpNext );		// Avoids any problems if mpLast isn't NULL.
	mpNext = pTail;					// ... and a reference exists to the new one.

	return pTail;
}

/********************************************************************************************

>	void FlashFontRecord::DeleteLastElement ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/99
	Inputs:		-
	Returns:	-
	Purpose:	Deletes the previous item in the list.

********************************************************************************************/

void FlashFontRecord::DeleteLastElement ( void )
{
	FlashFontRecord *pToDelete = mpLast;
	
	// Reset mpLast to be mpLast->GetLast (), so that the list isn't broken.
	mpLast = mpLast->GetLast ();

	delete pToDelete;
}

/********************************************************************************************

>	void FlashFontRecord::DeleteNextElement ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/99
	Inputs:		-
	Returns:	-
	Purpose:	Deletes the next item in the list.

********************************************************************************************/

void FlashFontRecord::DeleteNextElement ( void )
{
	FlashFontRecord *pToDelete = mpNext;
	
	// Reset mpNext to be mpNext->GetNext (), so that the list isn't broken.
	mpNext = mpNext->GetNext ();

	delete pToDelete;
}

/********************************************************************************************

>	WORD FlashFontRecord::GetTypeface ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/99
	Inputs:		-
	Returns:	mTypeface - The typeface ID number.
	Purpose:	Gets the value of mFontID.

********************************************************************************************/

WORD FlashFontRecord::GetTypeface ( void )
{
	return mTypeface;
}

/********************************************************************************************

>	WORD FlashFontRecord::GetFontID ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/99
	Inputs:		-
	Returns:	mFontID - The font ID number.
	Purpose:	Gets the value of mFontID.

********************************************************************************************/

WORD FlashFontRecord::GetFontID ( void )
{
	return mFontID;
}


/********************************************************************************************

>	BOOL FlashFontRecord::GetIsBold ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/99
	Inputs:		-
	Returns:	IsBold - mIs this font style bold?
	Purpose:	Gets the value of mIsBold.

********************************************************************************************/

BOOL FlashFontRecord::GetIsBold ( void )
{
	return mIsBold;
}

/********************************************************************************************

>	BOOL FlashFontRecord::GetIsItalic ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/99
	Inputs:		-
	Returns:	IsItalic - mIs this font style Italic?
	Purpose:	Gets the value of mIsItalic.

********************************************************************************************/

BOOL FlashFontRecord::GetIsItalic ( void )
{
	return mIsItalic;
}

/********************************************************************************************

>	WCHAR* FlashFontRecord::GetGlyphs ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/99
	Inputs:		-
	Returns:	*mGlyphs - A pointer to the array of characters used.
	Purpose:	Gets a pointer to mGlyphs.

********************************************************************************************/

WCHAR* FlashFontRecord::GetGlyphs ( void )
{
	return mGlyphs;
}

/********************************************************************************************

>	Path** FlashFontRecord::GetPaths ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/99
	Inputs:		-
	Returns:	*mpPaths - A pointer to the array of paths of the characters used.
	Purpose:	Gets a pointer to mpPaths.

********************************************************************************************/

Path** FlashFontRecord::GetPaths ( void )
{
	return mPaths;
}

/********************************************************************************************

>	FlashFontRecord* FlashFontRecord::GetLast ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/99
	Inputs:		-
	Returns:	*mpLast - A pointer to the previous node in the linked list.
	Purpose:	Gets mpLast.

********************************************************************************************/

FlashFontRecord* FlashFontRecord::GetLast ( void )
{
	return mpLast;
}

/********************************************************************************************

>	FlashFontRecord* FlashFontRecord::GetNext ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/99
	Inputs:		-
	Returns:	*mpNext - A pointer to the next node in the linked list.
	Purpose:	Gets mpNext.

********************************************************************************************/

FlashFontRecord* FlashFontRecord::GetNext ( void )
{
	return mpNext;
}

/********************************************************************************************

>	void FlashFontRecord::SetTypeface ( WORD Typeface )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/99
	Inputs:		Typeface - The ID number of a font in the Camelot image.
	Returns:	-
	Purpose:	Sets the value of mTypeface. This is a handle to a font name that is stored
				within the Camelot font manager.

********************************************************************************************/

void FlashFontRecord::SetTypeface ( WORD Typeface )
{
	mTypeface = Typeface;
}

/********************************************************************************************

>	void FlashFontRecord::SetFontID ( WORD FontID )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/99
	Inputs:		FontID - The ID number of a font within a Flash file.
	Returns:	-
	Purpose:	Sets the value of mFontID, which is the number used to relate a DefineFont
				entry to a DefineText entry.

********************************************************************************************/

void FlashFontRecord::SetFontID ( WORD FontID )
{
	mFontID = FontID;
}

/********************************************************************************************

>	void FlashFontRecord::SetIsBold ( BOOL IsBold )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/99
	Inputs:		IsBold - Is the current font in a bold typeface?
	Returns:	-
	Purpose:	Sets the value of mIsBold.

********************************************************************************************/

void FlashFontRecord::SetIsBold ( BOOL IsBold )
{
	mIsBold = IsBold;
}

/********************************************************************************************

>	void FlashFontRecord::SetIsItalic ( BOOL IsItalic )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/99
	Inputs:		IsItalic - Is the current font in an Italic typeface?
	Returns:	-
	Purpose:	Sets the value of mIsItalic.

********************************************************************************************/

void FlashFontRecord::SetIsItalic ( BOOL IsItalic )
{
	mIsItalic = IsItalic;
}

/********************************************************************************************

>	BOOL FlashFontRecord::AddGlyph ( WCHAR Glyph,
									 INT32 &Code )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/99
	Inputs:		Glyph - A single unicode character.
				&Code - A reference which contains the position of the glyph in the array.
	Returns:	TRUE if a character is added, otherwise false.
	Purpose:	Adds a glyph to mGlyphs.

********************************************************************************************/

BOOL FlashFontRecord::AddGlyph ( WCHAR Glyph,
								 INT32 &Code )
{
	// Set up the return values.
	BOOL bResult = TRUE;
	Code = 0;

	// Compare the value of the new character to the entries in the array.
	while ( mGlyphs [Code] != 0 )
	{
		if ( mGlyphs [Code] == Glyph )
		{
			// The character is already present within the listing, so quit out.
			bResult = FALSE;
			break;
		}

		// Increment the value of Code so that it points to the next element in the array.
		Code++;
	}

	mGlyphs[Code] = Glyph;			// Write the new value into the array.

	return bResult;
}

/********************************************************************************************

>	void FlashFontRecord::AddPath ( Path* ToAdd, INT32 Index )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/99
	Inputs:		ToAdd - A pointer to a path.
				Index - The matrix element to which you want to add it to.
	Returns:	-
	Purpose:	Adds a path reference to mpPaths.

********************************************************************************************/

void FlashFontRecord::AddPath ( Path* ToAdd, INT32 Index )
{
	mPaths [ Index ] = ToAdd;
}

/********************************************************************************************

>	void FlashFontRecord::SetLast ( FlashFontRecord *pLast )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/99
	Inputs:		pLast - A pointer to the previous FlashFontRecord in the linked list.
	Returns:	-
	Purpose:	Sets the value of mpLast.

********************************************************************************************/

void FlashFontRecord::SetLast ( FlashFontRecord *pLast )
{
	mpLast = pLast;
}

/********************************************************************************************

>	void FlashFontRecord::SetNext ( FlashFontRecord *pNext )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/99
	Inputs:		pNext - A pointer to the next FlashFontRecord in the linked list.
	Returns:	-
	Purpose:	Sets the value of mpNext.

********************************************************************************************/

void FlashFontRecord::SetNext ( FlashFontRecord *pNext )
{
	mpNext = pNext;
}
