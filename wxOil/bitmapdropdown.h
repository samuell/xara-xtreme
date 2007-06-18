// $Id: coldrop.h 1052 2006-05-15 14:19:14Z alex $
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

// coldrop.h - The Colour Drop-down list support class

#ifndef BITMAPDROPDOWN_H
#define BITMAPDROPDOWN_H

#include "dropdown.h"
//#include "gbrush.h"
//#include "pump.h"



/*************************************************************************
Class         : wxCamBitmapDropdownPopup 
Base Class    : public wxCamVListBoxComboPopu
Author        : Mikhail Tatarnikov
Description   : Represents popup list for the bitmap control.
Pure Virtual  : No
Known Issues  : None
Usage Notes   : Required to specify size of the list - if the bitmap is smaller
				then the control width, the dropdown should be of the bitmap size.
				Also the "unselected" item should be painted separately.
Override Notes: None
**************************************************************************/
class wxCamBitmapDropdownPopup : public wxCamVListBoxComboPopup
{
public:
	wxCamBitmapDropdownPopup(DropDown * pDropDown=NULL) :
		wxCamVListBoxComboPopup(pDropDown)
	{
	} 
	
	// Redraws the combo control itself.
    virtual void PaintComboControl(wxDC& dc, const wxRect& rect);


	// Calculates the dropdown size. We need to override it to allow the dropdown list to have smaller
	// width that the cobobox itself (in  case we have images only).
    virtual wxSize GetAdjustedSize(int /* TYPENOTE: Correct*/ minWidth, int /* TYPENOTE: Correct*/ prefHeight, INT32 maxHeight);
};



/*************************************************************************
Class         : CBDDItemInfo 
Base Class    : none
Author        : Mikhail Tatarnikov
Description   : Item information for Bitmap Dropdown control.
Pure Virtual  : No
Known Issues  : None
Usage Notes   : We need two separate fields for text, since the base DropDown class
				identifies items by their text. So, if all items have no string, they are
				considered the same, and as result some operations (like SetSelection)
				work incorrectly.
Override Notes: None
**************************************************************************/
class CBDDItemInfo
{
public:
	CBDDItemInfo();
	CBDDItemInfo(UINT32 uiBitmapResID, String_256 strText);

	// Accessors
	UINT32	   GetBitmapID();
	BOOL	   HasText();
	String_256 GetText();

	const CBDDItemInfo& operator=(const CBDDItemInfo& rOther);

protected:
	UINT32	   m_uiBitmapResID;
	BOOL	   m_bHasText;
	String_256 m_strText;
};



/*************************************************************************
Class         : CBitmapDropDown 
Base Class    : public DropDow
Author        : Mikhail Tatarnikov
Description   : Represents a combobox with bitmaps and text
Pure Virtual  : No
Known Issues  : None
Usage Notes   : None
Override Notes: The item data is stored in a vector and owned by this class.
**************************************************************************/
class CBitmapDropDown : public DropDown
{
friend class DialogManager;

CC_DECLARE_DYNCREATE(CBitmapDropDown)

// --- Kernelly section

public:			// CBitmapDropDown  management
	CBitmapDropDown();
	virtual ~CBitmapDropDown();

	virtual BOOL Init(CWindowID Window, CGadgetID Gadget);
	
	// Adds a new item (icon + text).
	void AddItem(UINT32 uiBitmapResID, String_256 strText);
	void AddDivider();

	// Select an item.
	void SetSelectedIndex(INT32 iSelectedIndex){DropDown::SetSelectedIndex(iSelectedIndex);}

	// Specify the look of "unselected" item - a bitmap/text that will be drawn when no item is selected.
	void SetUnselectedIntem(UINT32 uiBitmapID, String_256 strText);

	// Deletes an existing item.
	void DeleteItem(INT32 index);

protected:
	// Overrides item data access function.
	void* GetItemData(INT32 iItemIndex);

	// Returns TRUE if the item has an icon. Default is no icon
	virtual BOOL HasIcon(void* pvItemData);
	
	// Returns TRUE if the item has a text label.
	virtual BOOL HasText(void* pvItemData);
	
	virtual wxString GetText(void* pvItemData, INT32 iItem);
	
	// Draw the item.
	virtual wxSize HandleDrawItemInternal(wxDC& dc, const wxRect& Rect, INT32 item, INT32 flags, BOOL Draw);
	
	// Draw the associated icon.
	virtual wxSize DrawIcon(wxDC& dc, const wxRect& Rect, INT32 item, INT32 flags, BOOL Draw);

protected:
	// Items information. No need to delete item data while removing an item.
	std::vector<CBDDItemInfo> m_vecItems;

	// Information about the unselected item.
	CBDDItemInfo m_oUnselectedItemInfo;

// --- Winoily section


};





#endif	// BITMAPDROPDOWN_H

