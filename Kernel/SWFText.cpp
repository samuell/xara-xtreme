// $Id: SWFText.cpp 662 2006-03-14 21:31:49Z alex $
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
#include "nodetext.h"
#include "swfexpdc.h"
#include "swftext.h"

#define new CAM_DEBUG_NEW

/********************************************************************************************

>	FlashTextRecord::FlashTextRecord ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/99
	Inputs:		-
	Returns:	-
	Purpose:	Creates a FlashTextRecord list node, and initialises any values.

********************************************************************************************/

FlashTextRecord::FlashTextRecord ( void )
{
	// Initialise the member variables, so as to avoid any unpleasentness later.
	mTextID		= 0;
	mCurrent	= 0;
	mScaleX		= FLASH_FIXED_ONE;
	mScaleY		= FLASH_FIXED_ONE;
	mSkewY		= 0;
	mSkewX		= 0;
	mAscent		= 0;
	mpLast		= NULL;				// Pointer to the previous node.
	mpNext		= NULL;				// Pointer to the next node.
}

/********************************************************************************************

>	FlashTextRecord::~FlashTextRecord ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/99
	Inputs:		-
	Returns:	-
	Purpose:	Destroys this instance of FlashTextRecord.

********************************************************************************************/

FlashTextRecord::~FlashTextRecord ( void )
{
	// Clean up any stray values.
}

/********************************************************************************************

>	FlashTextRecord* FlashTextRecord::AddElement ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/99
	Inputs:		-
	Returns:	pTail - The pointer to the new node.
	Purpose:	Adds an element to the tail of the list.

********************************************************************************************/

FlashTextRecord* FlashTextRecord::AddElement ( void )
{
	FlashTextRecord *pTail = new FlashTextRecord;
	
	// Set the appropriate pointers.
	pTail->SetLast ( this );		// Ensure that a reference exists to this object...
	pTail->SetNext ( mpNext );		// Avoids any problems if mpLast isn't NULL.
	mpNext = pTail;					// ... and a reference exists to the new one.

	return pTail;
}

/********************************************************************************************

>	void FlashTextRecord::DeleteLastElement ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/99
	Inputs:		-
	Returns:	-
	Purpose:	Deletes the previous item in the list.

********************************************************************************************/

void FlashTextRecord::DeleteLastElement ( void )
{
	FlashTextRecord *pToDelete = mpLast;
	
	// Reset mpLast to be mpLast->GetLast (), so that the list isn't broken.
	mpLast = mpLast->GetLast ();

	delete pToDelete;
}

/********************************************************************************************

>	void FlashTextRecord::DeleteNextElement ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/99
	Inputs:		-
	Returns:	-
	Purpose:	Deletes the next item in the list.

********************************************************************************************/

void FlashTextRecord::DeleteNextElement ( void )
{
	FlashTextRecord *pToDelete = mpNext;
	
	// Reset mpNext to be mpNext->GetNext (), so that the list isn't broken.
	mpNext = mpNext->GetNext ();

	delete pToDelete;
}


/********************************************************************************************

>	TCHAR* FlashTextRecord::GetString ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/99
	Inputs:		-
	Returns:	TCHAR* - A pointer to the contents of mText.
	Purpose:	Gets a pointer to mText.

********************************************************************************************/

INT32* FlashTextRecord::GetString ( void )
{
	return mText;
}

/********************************************************************************************

>	BOOL FlashTextRecord::IsFull ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/99
	Inputs:		-
	Returns:	TRUE if the string is full, otherwise FALSE.
	Purpose:	Used to determine whether the string is full.

********************************************************************************************/

BOOL FlashTextRecord::IsFull ( void )
{
	// Compare the string's length with the maximum length of the string. Doing it this way
	// allows the string length to be changed internally without having to alter the class's
	// interface.
	if ( mCurrent >= FTR_STRING_SIZE )
		return TRUE;
	else
		return FALSE;
}

/********************************************************************************************

>	DocCoord FlashTextRecord::GetPosition ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/99
	Inputs:		-
	Returns:	mPosition - The position of this string in the Flash image.
	Purpose:	Gets the value of mPosition.

********************************************************************************************/

DocCoord FlashTextRecord::GetPosition ( void )
{
	return mPosition;
}

/********************************************************************************************

>	DocRect FlashTextRecord::GetBounds ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/99
	Inputs:		-
	Returns:	mTextBounds - The bounding box of the text.
	Purpose:	Gets the value of mTextBounds.

********************************************************************************************/

DocRect FlashTextRecord::GetBounds ( void )
{
	return mTextBounds;
}

/********************************************************************************************

>	INT32* FlashTextRecord::GetAdvances ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/99
	Inputs:		-
	Returns:	DWORD* - A pointer to mAdvances [0].
	Purpose:	Gets a pointer to mAdvances

********************************************************************************************/

INT32* FlashTextRecord::GetAdvances ( void )
{
	return mAdvances;
}

/********************************************************************************************

>	WORD* FlashTextRecord::GetSizes ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/99
	Inputs:		-
	Returns:	WORD* - A pointer to mCharacterSize.
	Purpose:	Gets a pointer to mCharacterSize [].


********************************************************************************************/

WORD* FlashTextRecord::GetSizes ( void )
{
	return mCharacterSize;
}

/********************************************************************************************

>	FlashColour* FlashTextRecord::GetColours ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/99
	Inputs:		-
	Returns:	DocColour* - A pointer to mCharacterColour.
	Purpose:	Gets a pointer to mCharacterColour.

********************************************************************************************/

FlashColour* FlashTextRecord::GetColours ( void )
{
	return mCharacterColour;
}

/********************************************************************************************

>	WORD* FlashTextRecord::GetFontStyles ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/99
	Inputs:		-
	Returns:	WORD* - A pointer to mCharacterStyle.
	Purpose:	Gets a pointer to mCharacterStyle.

********************************************************************************************/

WORD* FlashTextRecord::GetFontStyles ( void )
{
	return mCharacterStyle;
}

/********************************************************************************************

>	INT8* FlashTextRecord::GetOffsets ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/99
	Inputs:		-
	Returns:	INT8* - A pointer to mOffsetss.
	Purpose:	Gets a pointer to mOffsetss.

********************************************************************************************/

INT8* FlashTextRecord::GetOffsets ( void )
{
	return mOffsets;
}

/********************************************************************************************

>	FIXED16 FlashTextRecord::GetAspect ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/99
	Inputs:		-
	Returns:	FIXED16 - The character's aspect ratio.
	Purpose:	Gets the value of mAspectRatio

********************************************************************************************/

FIXED16 FlashTextRecord::GetAspect ( void )
{
	return mAspectRatio;
}

/********************************************************************************************

>	INT32 FlashTextRecord::GetAscent ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/99
	Inputs:		-
	Returns:	INT32 - The character's ascent.
	Purpose:	Gets the value of mAscent.

********************************************************************************************/

INT32 FlashTextRecord::GetAscent ( void )
{
	return mAscent;
}

/********************************************************************************************

>	INT32 FlashTextRecord::GetCount ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/99
	Inputs:		-
	Returns:	INT32 - The length of the text string.
	Purpose:	Gets the length of the text string stored within the FlashTextRecord.

********************************************************************************************/

INT32 FlashTextRecord::GetCount ( void )
{
	return mCurrent;
}

/********************************************************************************************

>	WORD FlashTextRecord::GetTextID ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/99
	Inputs:		-
	Returns:	WORD - The value of mTextID.
	Purpose:	Gets the ID for this text record.

********************************************************************************************/

WORD FlashTextRecord::GetTextID ( void )
{
	return mTextID;
}

/********************************************************************************************

>	FlashTextRecord* FlashTextRecord::GetLast ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/99
	Inputs:		-
	Returns:	*mpLast - A pointer to the previous node in the linked list.
	Purpose:	Gets mpLast.

********************************************************************************************/

FlashTextRecord* FlashTextRecord::GetLast ( void )
{
	return mpLast;
}

/********************************************************************************************

>	FlashTextRecord* FlashTextRecord::GetNext ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/99
	Inputs:		-
	Returns:	*mpNext - A pointer to the next node in the linked list.
	Purpose:	Gets mpNext.

********************************************************************************************/

FlashTextRecord* FlashTextRecord::GetNext ( void )
{
	return mpNext;
}

/********************************************************************************************

>	INT32 FlashTextRecord::AddChar ( TextChar *pTheLetter,
								   INT32 FlashCode )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/99
	Inputs:		TheLetter - The character to add to mText.
	Returns:	The position of this character within the array, or -1 if there's been a
				problem.
	Purpose:	Adds FlashCode to the end of mText, and updates the bounding rectangle, and
				calculates the Advance for this letter from the original letter record within
				the Camelot tree. The FlashCode value is returned by
				FlashFontRecord::AddGlyph (), and is needed to access the correct character
				record by a Flash viewer.

********************************************************************************************/

INT32 FlashTextRecord::AddChar ( TextChar *pTheLetter,
							   INT32 FlashCode )
{
	INT32 Index = -1;		// Traditional UNIX error value.
	
	if ( pTheLetter != NULL )
	{
		// Get the current character, and store it within the array.
		mText [mCurrent] = FlashCode;
		Index = mCurrent;
		mCurrent ++;

		// Set the advance value. (i.e. how far the character is from the start of the string.
		SetAdvance ( pTheLetter->GetCharAdvance (), Index );
	}

	return Index;
}

/********************************************************************************************

>	void FlashTextRecord::SetPosition ( DocCoord Point )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/99
	Inputs:		Point - The position of the start of the TextLine.
	Returns:	-
	Purpose:	Sets the value of mPosition.

********************************************************************************************/

void FlashTextRecord::SetPosition ( DocCoord Point )
{
	mPosition = Point;
}

/********************************************************************************************

>	void FlashTextRecord::SetBounds ( DocRect Bounds )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/99
	Inputs:		Bounds - The bounding box of the text string.
	Returns:	-
	Purpose:	Sets the value of mTextBounds.

********************************************************************************************/

void FlashTextRecord::SetBounds ( DocRect Bounds )
{
	mTextBounds = Bounds;
}

/********************************************************************************************

>	void FlashTextRecord::SetAdvance ( INT32 Advance,
									  INT32 i )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/99
	Inputs:		Advance - The advance for this character.
				i - The position at which the value is to be added.
	Returns:	-
	Purpose:	Sets the value of mAdvances at position i. ( mAdvances [i] )

********************************************************************************************/

void FlashTextRecord::SetAdvance ( INT32 Advance,
								  INT32 i )
{
	// Convert from millipoints to twips.
	Advance /= FLASH_SCALE;

	// Stores the advance for the current font.
	mAdvances [i] = Advance;
}

/********************************************************************************************

>	void FlashTextRecord::SetSize ( MILLIPOINT Size,
									INT32 i )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/99
	Inputs:		Size - The height of the current font style.
				i - The position at which the value is to be added.
	Returns:	-
	Purpose:	Sets the value of mCharacterSize at position i. ( mCharacterSize [i] )

********************************************************************************************/

void FlashTextRecord::SetSize ( MILLIPOINT Size,
								INT32 i )
{
	Size /= FLASH_SCALE;
	mCharacterSize [i] = ( WORD ) Size;
}

/********************************************************************************************

>	void FlashTextRecord::SetColour ( DocColour *Colour,
									  BYTE Alpha,
									  INT32 i )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/99
	Inputs:		Colour - A standard Camelot colour.
				Alpha - The transparency value.
				i - The index value.
	Returns:	-
	Purpose:	Sets the value of ColourValue[i] to have the RGB colour from the DocColour,
				and the Alpha value from Alpha.

********************************************************************************************/

void FlashTextRecord::SetColour ( DocColour *Colour,
								  UINT32 *Alpha,
								  INT32 i )
{
	INT32 lRed, lGreen, lBlue;

	// If no colour pointer has been set, return.
	if ( Colour == NULL )
		return;

	// Extract the colour value from DocColour.
	Colour->GetRGBValue ( &lRed, &lGreen, &lBlue );

	// And cast it into the record as an RGB triplet of BYTEs.
	mCharacterColour [i].Red	= ( BYTE ) lRed;
	mCharacterColour [i].Green	= ( BYTE ) lGreen;
	mCharacterColour [i].Blue	= ( BYTE ) lBlue;
	mCharacterColour [i].Alpha	= 255 - ( BYTE ) ( *Alpha );
}

/********************************************************************************************

>	void FlashTextRecord::SetStyle ( WORD Style,
									 INT32 i )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/99
	Inputs:		Style - A handle to a current font sytle.
				i - The position at which the value is to be added.
	Returns:	-
	Purpose:	Sets the value of mCharacterStyle [i].

********************************************************************************************/

void FlashTextRecord::SetStyle ( WORD Style,
								 INT32 i )
{
	mCharacterStyle [i] = Style;
}

/********************************************************************************************

>	void FlashTextRecord::SetOffset ( INT32 Offset,
										INT32 i )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/99
	Inputs:		Offset - The value of the current font's offset.
				i - The position at which the value is to be added.
	Returns:	-
	Purpose:	Sets the value of mOffsets [i].

********************************************************************************************/

void FlashTextRecord::SetOffset ( INT32 Offset,
								  INT32 i )
{
	Offset /= FLASH_SCALE;
	mOffsets [i] = ( INT8 ) Offset;
}

/********************************************************************************************

>	void FlashTextRecord::SetAspect ( FIXED16 AspectRatio )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/99
	Inputs:		FIXED16 - The AspectRatio of the line of text.
	Returns:	-
	Purpose:	Sets the value of mAspectRatio.

********************************************************************************************/

void FlashTextRecord::SetAspect ( FIXED16 AspectRatio )
{
	mAspectRatio = AspectRatio;
}

/********************************************************************************************

>	void FlashTextRecord::SetAscent ( INT32 Ascent )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/99
	Inputs:		INT32 - The ascent of the first character in the line of text.
	Returns:	-
	Purpose:	Sets the value of mAscent.

********************************************************************************************/

void FlashTextRecord::SetAscent ( INT32 Ascent )
{
	Ascent /= FLASH_SCALE;

	if ( Ascent > mAscent )
		mAscent = Ascent;
}

/********************************************************************************************

>	void FlashTextRecord::SetTextID ( WORD ID )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/99
	Inputs:		WORD - The ID value to be set.
	Returns:	-
	Purpose:	Sets the value of mTextID.

********************************************************************************************/

void FlashTextRecord::SetTextID ( WORD ID )
{
	mTextID = ID;
}

/********************************************************************************************

>	void FlashTextRecord::SetLast ( FlashTextRecord *pLast )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/99
	Inputs:		pLast - A pointer to the previous FlashTextRecord in the linked list.
	Returns:	-
	Purpose:	Sets the value of mpLast.

********************************************************************************************/

void FlashTextRecord::SetLast ( FlashTextRecord *pLast )
{
	mpLast = pLast;
}

/********************************************************************************************

>	void FlashTextRecord::SetNext ( FlashTextRecord *pNext )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/99
	Inputs:		pNext - A pointer to the next FlashTextRecord in the linked list.
	Returns:	-
	Purpose:	Sets the value of mpNext.

********************************************************************************************/

void FlashTextRecord::SetNext ( FlashTextRecord *pNext )
{
	mpNext = pNext;
}

/********************************************************************************************

>	void FlashTextRecord::SetSkewY ( INT32 Skew )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/99
	Inputs:		Skew - The value for mSkewY.
	Returns:	-
	Purpose:	Sets mSkewY.

********************************************************************************************/

void FlashTextRecord::SetSkewY ( INT32 Skew )
{
	mSkewY = Skew;
}

/********************************************************************************************

>	INT32 FlashTextRecord::GetSkewY ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/99
	Inputs:		-
	Returns:	mSkewY
	Purpose:	Gets mSkewY.

********************************************************************************************/

INT32 FlashTextRecord::GetSkewY ( void )
{
	return mSkewY;
}

/********************************************************************************************

>	void FlashTextRecord::SetSkewX ( INT32 Skew )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/99
	Inputs:		Skew - The value for mSkewX.
	Returns:	-
	Purpose:	Sets mSkewX.

********************************************************************************************/

void FlashTextRecord::SetSkewX ( INT32 Skew )
{
	mSkewX = Skew;
}

/********************************************************************************************

>	INT32 FlashTextRecord::GetSkewX ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/99
	Inputs:		-
	Returns:	mSkewX
	Purpose:	Gets mSkewX.

********************************************************************************************/

INT32 FlashTextRecord::GetSkewX ( void )
{
	return mSkewX;
}

/********************************************************************************************

>	void FlashTextRecord::SetScaleX ( INT32 Scale )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/99
	Inputs:		Scale - The value for mScaleX.
	Returns:	-
	Purpose:	Sets mScaleX.

********************************************************************************************/

void FlashTextRecord::SetScaleX ( INT32 Scale )
{
	mScaleX = Scale;
}

/********************************************************************************************

>	INT32 FlashTextRecord::GetScaleX ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/99
	Inputs:		-
	Returns:	mScaleX
	Purpose:	Gets mScaleX.

********************************************************************************************/

INT32 FlashTextRecord::GetScaleX ( void )
{
	return mScaleX;
}

/********************************************************************************************

>	void FlashTextRecord::SetScaleY ( INT32 Scale )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/99
	Inputs:		Scale - The value for mScaleY.
	Returns:	-
	Purpose:	Sets mScaleY.

********************************************************************************************/

void FlashTextRecord::SetScaleY ( INT32 Scale )
{
	mScaleY = Scale;
}

/********************************************************************************************

>	INT32 FlashTextRecord::GetScaleY ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/99
	Inputs:		-
	Returns:	mScaleY
	Purpose:	Gets mScaleY.

********************************************************************************************/

INT32 FlashTextRecord::GetScaleY ( void )
{
	return mScaleY;
}

