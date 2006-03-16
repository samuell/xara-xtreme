// $Id: fontdrop.h 662 2006-03-14 21:31:49Z alex $
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

// fontdrop.h - The Colour Drop-down list support class

#ifndef INC_FONTDROP
#define INC_FONTDROP

#include "dropdown.h"
#include "fontbase.h"

class Document;

/********************************************************************************************

>	class FontDropItem : public CCObject

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/95
	Purpose:	Item class for the FontDrop system

********************************************************************************************/

class FontDropItem : public ListItem
{
	CC_DECLARE_DYNCREATE(FontDropItem)

	String_64 FontName; 		// Textual name of font in question
	FontClass Type;				// What sort of font are we dealing with ?

	FontDropItem();
	FontDropItem(TCHAR *Name, FontClass FontType);

public:
	String_64	GetFontName() { return FontName; }
};


/********************************************************************************************

>	class FontDropEnumFont : public OILEnumFonts

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/95
	Purpose:	Class for enumerating the fonts for the font dropdown system

********************************************************************************************/

class FontDropEnumFont : public OILEnumFonts
{
	CC_DECLARE_DYNCREATE( FontDropEnumFont )
	
	public:
		FontDropEnumFont() { count=0; }
		INT32 GetCount() { return count; };
		virtual BOOL NewFont(FontClass Class, ENUMLOGFONT FAR* lpelf);
	
	private:
		INT32 count;
};


/********************************************************************************************

>	class FontDropDown : public DropDown

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/95
	Purpose:	FontDropDown class itself

********************************************************************************************/

class FontDropDown : public DropDown
{
friend class DialogManager;

CC_DECLARE_DYNCREATE(FontDropDown)

// FontDropDown management
public:

	// Default constructor and destructor
	FontDropDown();
	~FontDropDown();

	// (Re) initialise the font list
	virtual BOOL Init(CWindowID ParentWindow, CGadgetID ParentControl);

	// Kills the list and reclaims any memory...
	void KillList(void);
	
	// Add a single font to the list
	BOOL AddFontToList(TCHAR *FontName, FontClass Type);

	// Fills in the combobox list of available fonts, and selects a useful one
	BOOL FillInFontList();

	// Update the top item above the pull-down
	BOOL SetTopFontName(StringBase* StrValue, FontClass Type, BOOL Deselect = TRUE);

	// Sets the selected item in the list
	BOOL SetSelection(FontDropItem *TheFont);

	// Decodes a selection in the list, returning the font that it references
	FontDropItem *DecodeSelection(INT32 SelectionIndex);


// Overriding methods
protected:

	// Returns TRUE if the item has an icon.
	virtual BOOL HasIcon(DWORD ItemData);

	// Handles redraw of the icon, if any.
	virtual BOOL DrawIcon(DWORD ItemData, HDC hDC, RECT *IconRect, BOOL Disabled);

	// Handles redraw of the text for an item.
	virtual BOOL DrawText(DWORD ItemData, HDC hDC, RECT *TextRect);

	// Handles redrawing the top item, which may be a special case...
	virtual BOOL HandleDrawItemInternal(HWND hDlg, UINT32 wParam, INT32 lParam);


// Member variables
public:
	// this is required because enumerating fonts uses a callback function...
	static DWORD CurrentFontDropDown;
	
	// List of font items
	List Fonts;

	// We need a special entry for the top item...
	FontDropItem TheTopItem;

protected:
	// True if the item above the list (-1) is being redrawn...
	BOOL RedrawingTopItem;

};

/********************************************************************************************

>	class DocumentFontDropDown : public FontDropDown

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/11/95
	Purpose:	DocumentFontDropDown class itself.
				This uses the FontDropDown class to provide a font drop down but overides
				some of the functionality so that it works out the fonts in the current
				document rather than the fonts avalaible in the system.
********************************************************************************************/

class DocumentFontDropDown : public FontDropDown
{

CC_DECLARE_DYNCREATE(DocumentFontDropDown)

// DocumentFontDropDown management
public:

	// Default constructor and destructor
	DocumentFontDropDown();
	~DocumentFontDropDown();

	// Fills in the combobox list of fonts in the specified document, and selects a useful one
	BOOL FillInFontList(Document * WorkDoc);

};


#endif
