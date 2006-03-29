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
#include "ccobject.h"

#ifndef INC_FLASH_FONT_RECORD
#define INC_FLASH_FONT_RECORD

#define FLASH_TEXT_ARRAY_SIZE 256

/********************************************************************************************
>	class FlashFontRecord : public CObject

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/99
	Purpose:	Maintains a record of all text styles used, and the characters they contain.
				This is then used when exporting path records to create the DefineFont
				entries within a Flash format file when exporting the image. Elements are
				stored in a double-linked list.

********************************************************************************************/

class FlashFontRecord : public CCObject
{
public:
	CC_DECLARE_DYNAMIC( FlashFontRecord )

	// Constructor and destructor.
	FlashFontRecord ( void );								// Constructor.
	~FlashFontRecord ( void );								// Destructor.

	// Add and remove elements to a linked list.
	FlashFontRecord*	AddElement ( void );				// Adds an element to the list.
	void				DeleteLastElement ( void );			// Deletes the element mpLast.
	void				DeleteNextElement ( void );			// Deletes the element mpNext.

	// Data access functions.
	WORD				GetTypeface ( void );				// Gets the value of mTypeface.
	WORD				GetFontID ( void );					// Gets the value of mFontID.
	BOOL				GetIsBold ( void );					// Gets the value of mIsBold.
	BOOL				GetIsItalic ( void );				// Gets the value of mIsItalic.
	WCHAR*				GetGlyphs ( void );					// Gets a pointer to mGlyphs.
	Path**				GetPaths ( void );					// Gets a pointer to mGlyphs.
	FlashFontRecord*	GetLast ( void );					// Gets mpLast.
	FlashFontRecord*	GetNext ( void );					// Gets mpNext.

	// Set value functions.
	void				SetTypeface ( WORD Typeface );		// Sets the value of mTypeface.
	void				SetFontID ( WORD FontID );			// Sets the value of mFontID.
	void				SetIsBold ( BOOL IsBold );			// Sets the value of mIsBold.
	void				SetIsItalic ( BOOL IsItalic );		// Sets the value of mIsItalic.
	BOOL				AddGlyph ( WCHAR Glyph,				// Adds a glyph to mGlyphs.
								   INT32 &Code );
	void				AddPath ( Path* ToAdd, INT32 Index );	// Adds a glyph path to the array.
	void				SetLast ( FlashFontRecord *pLast );	// Sets the value of mpLast.
	void				SetNext ( FlashFontRecord *pNext );	// Sets the value of mpNext.

private:
	// Member variables.
	WORD				mTypeface;							// A Camelot font name handle.
	WORD				mFontID;							// The font's ID number.
	BOOL				mIsBold;							// Is the font bold?
	BOOL				mIsItalic;							// Is the font italic?
	WCHAR				mGlyphs [FLASH_TEXT_ARRAY_SIZE];	// A list of the glyphs used.
	Path				*mPaths [FLASH_TEXT_ARRAY_SIZE];	// An array of the paths.
	FlashFontRecord		*mpLast;							// Pointer to the previous node.
	FlashFontRecord		*mpNext;							// Pointer to the next node.
};

#endif
