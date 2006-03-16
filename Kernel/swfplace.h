// $Id: swfplace.h 662 2006-03-14 21:31:49Z alex $
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
#ifndef INC_FLASH_PLACE_OBJECT
#define INC_FLASH_PLACE_OBJECT

#include "ccobject.h"

#define FLASH_FIRST_ID	1						// The ID of the first object to render.

// Enumerated type for the object state.
typedef enum
{
	FLASH_TEXT,
	FLASH_SHAPE,
	FLASH_SPRITE,
	FLASH_BUTTON
} FlashType;

// Enumerated type for the layer state.
typedef enum
{
	NO_BUTTON,
	CLICKED,
	DEFAULT,
	MOUSE,
	SELECTED
} LayerState;

/********************************************************************************************

>	class FlashPlaceObject : public CCObject

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/99
	Purpose:	Contains the information necessary to do a PlaceObject tag.

********************************************************************************************/

class FlashPlaceObject : public CCObject
{
public:
	FlashPlaceObject ();
	~FlashPlaceObject ();

	// Access functions.
	FlashPlaceObject* GetNext ( void );			// Get mpNext.
	void SetNext ( FlashPlaceObject *pNext );	// Set mpNext.
	FlashPlaceObject* AddNext ( void );			// Create a new element at mpNext.

	FlashPlaceObject* GetLast ( void );			// Get mpLast.
	void SetLast ( FlashPlaceObject *pLast );	// Set mpLast.

	void SetType ( FlashType Type );			// Sets mToRender.
	FlashType GetType ( void );					// Gets mToRender.

	void SetPosition ( DocCoord Point );		// Position to place the object at.
	DocCoord GetPosition ( void );				// Get the position the object is at.

	void SetID ( WORD ID );						// Set the ID of the shape to be placed.
	WORD GetID ( void );						// Get the ID of the shape.
	WORD GetRawID ( void );						// Get the raw ID (i.e. mID ).

	void SetDoTransform ( BOOL State );			// Sets mDoTransform.
	BOOL GetDoTransform ( void );				// Gets mDoTransform.

	// Static member functions.
	static void ResetStatics ( void );			// Reset all the counter variables.

	static void IncBitmapCount ( void );		// Increase mBitmapCount.
	static void IncFontCount ( void );			// Increase mFontCount.
	static void IncTextCount ( void );			// Increase mTextCount.
	static void IncShapeCount ( void );			// Increase mShapeCount.
	static void IncSpriteCount ( void );		// Increase mSpriteCount.
	static void DecSpriteCount ( void );		// Decrease mSpriteCount.
	static void IncButtonCount ( void );		// Increase mButtonCount.

	static WORD GetBitmapCount ( void );		// Get mBitmapCount.
	static WORD GetFontCount ( void );			// Get mFontCount.
	static WORD GetTextCount ( void );			// Get mTextCount.
	static WORD GetShapeCount ( void );			// Get mShapeCount.
	static WORD GetSpriteCount ( void );		// Get mSpriteCount.
	static WORD	GetButtonCount ( void );		// Get mButtonCount.

private:
	WORD				mID;					// ID number.
	DocCoord			mPosition;				// Translation position.
	FlashPlaceObject	*mpNext;				// Pointer to next item.
	FlashPlaceObject	*mpLast;				// Pointer to previous item.
	FlashType			mToRender;				// The type of record to render.
	BOOL				mDoTransform;			// Are the co-ordinates OK for Flash?

	// Static member variables.
	static WORD			mBitmapCount;			// The number of bitmaps.
	static WORD			mFontCount;				// The number of font definitions.
	static WORD			mTextCount;				// The number of text strings.
	static WORD			mShapeCount;			// The number of shape records.
	static WORD			mSpriteCount;			// The number of sprite records.
	static WORD			mButtonCount;			// The number of button records.
};

#endif
