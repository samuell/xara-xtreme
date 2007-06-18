// $Id: dropdown.h 1386 2006-06-28 17:49:55Z alex $
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

// GridDropDown.h - The grid Drop-down list base class

#ifndef WXOIL_GRIDDROPDOWN_H
#define WXOIL_GRIDDROPDOWN_H

//#include "dlgtypes.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "gridcombo.h"

// class DialogManager;
class CGridDropDown;



/*************************************************************************
Class         : wxCamGridPopup
Base Class    : public wxGridComboPopup
Author        : Mikhail Tatarnikov
Description   : Customization of the wxGridComboPopup.
Pure Virtual  : None
Known Issues  : None
Usage Notes   : None
Override Notes: None
**************************************************************************/
class wxCamGridPopup : public wxGridComboPopup
{
// friend class CGridDropDown;
public:
	wxCamGridPopup(CGridDropDown* pDropDown, wxGridCombo* pCombo);
	virtual ~wxCamGridPopup();

	virtual void DrawItem(wxDC& dc, const wxRect& rect, int /* TYPENOTE: Correct */ iItem, INT32 iFlags);

protected:
	CGridDropDown* m_pDropDown;
};




/*************************************************************************
Class         : CGridDropDown
Base Class    : public ListItem
Author        : Mikhail Tatarnikov
Description   : Control class for wxGridCombo control.
Pure Virtual  : None
Known Issues  : None
Usage Notes   : None
Override Notes: None
**************************************************************************/
class CGridDropDown : public ListItem
{
	friend class wxCamGridPopup;

public:			// Creation/destruction
	CGridDropDown();
	virtual ~CGridDropDown();	

	virtual BOOL Init(CWindowID ParentWindow, CGadgetID ParentControl);

	static void KillDropDownsByWindow(CWindowID Window);

	// Sets/gets the index of the selected item
	void  SetSelected(INT32 iSelectedIndex);
	INT32 GetSelected();

	// Set the columns number.
	void SetColumns(INT32 iColumns);

	// Sets the size of items (images).
	void   SetItemSize(wxSize szItem);
	wxSize GetItemSize();
	
	// Determines how many items are in the list
	INT32 GetItemsNum(void);

	// Removes all entries from the combobox.
	void Clear(void);

	// Deletes an item
	void DeleteItem(INT32 index);


protected:
	static List CurrentDropDowns;					// A list of all currently active dropdowns

	virtual void KillList() {} // allow reclaiming of memory on death

// --- Winoily section

protected:		// Methods that must be overridden by derived classes

	virtual void DrawItem(wxDC& dc, const wxRect& rect, INT32 iItem, INT32 iFlags);

	virtual wxRect PreDrawItem(wxDC& dc, const wxRect& rect, INT32 iItem, INT32 iFlags);
	virtual void   DrawItemCore(wxDC& dc, const wxRect& rect, INT32 iItem, INT32 iFlags) = 0;
	virtual void   PostDrawItem(wxDC& dc, const wxRect& rect, INT32 iItem, INT32 iFlags);

	void SetListRedraw(BOOL Enable);	// Enables/disables redraw of the list (use while adding items)

protected:		// Useful methods to call from derived classes

	// Adds an item to the end of the list
	void AddItem(CGridComboUserData* pItemData);

	// Retrieves the data associated with a given item
	CGridComboUserData* GetItemData(INT32 iItem);

	// Get a pointer to the combo box
	wxGridCombo* GetBox();

	// Helper functions.
	void DrawShadedRect(wxDC& dc, const wxRect& rect, const wxPen& pen1, const wxPen& pen2);


protected:
	BOOL			m_bInitialised;						// TRUE once we have been initialised

	CWindowID		m_widParentDlg;					// CWindowID of the parent window
	CGadgetID		m_gidParentGadget;					// Gadget ID of the combo box
	wxCamGridPopup* m_pPopup;				// Pointer to the popup control
};


#endif // WXOIL_GRIDDROPDOWN_H

