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

// dropdown.h - The Drop-down list base class

#ifndef INC_DROPDOWN
#define INC_DROPDOWN

//#include "dlgtypes.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]


class DialogManager;
class wxCamVListBoxComboPopup;

class DropDown : public ListItem
{
friend class DialogManager;
friend class wxCamVListBoxComboPopup;

CC_DECLARE_DYNCREATE(DropDown)

public:			// Creation/destruction
	DropDown();
	virtual ~DropDown();	

	virtual BOOL Init(CWindowID ParentWindow, CGadgetID ParentControl);

	static void KillDropDownsByWindow(CWindowID Window);

protected:
	static List CurrentDropDowns;					// A list of all currently active dropdowns

	virtual void KillList() {} // allow reclaiming of memory on death

// --- Winoily section

protected:		// Methods that must be overridden by derived classes
	virtual BOOL HasIcon(void * ItemData);
				// Returns TRUE if the item has an icon. Default is no icon

	virtual BOOL DrawIcon(void * ItemData, wxDC& dc, wxRect& IconRect, BOOL Disabled, INT32 flags);
				// Handles redraw of the icon, if any. Default is nothing gets drawn

	virtual wxSize DrawText(void * ItemData, wxDC& dc, wxRect& TextRect, INT32 item, INT32 flags, BOOL Draw);
				// Handles redraw of the text for an item. Default draws the text
				// returned by GetText. You this probably do not need to override this

	virtual wxString GetText(void * ItemData, INT32 Item) {return wxEmptyString;}
				// By default returns a dummy string


public:			// Useful methods
	void ClearList(void);
				// Clears all items in the list


protected:		// Useful methods to call from derived classes
	void SetListRedraw(BOOL Enable);
				// Enables/disables redraw of the list (use while adding items)

	void AddItem(void * ItemData);
				// Adds an item to the end of the list

	void DeleteItem(INT32 index);
				// Deletes an item

	void SetSelectedIndex(INT32 SelectedIndex);
				// Sets the index of the selected item

	void * GetItemData(INT32 ItemIndex);
				// Retrieves the data associated with a given item

	INT32 GetNumberOfItems(void);
				// Determines how many items are in the list

	wxOwnerDrawnComboBox * GetBox();
				// Get a pointer to the combo box

// --- Base class stuff you shouldn't have to mess with

protected:
	virtual wxSize HandleDrawItemInternal(wxDC& dc, const wxRect& Rect, INT32 item, INT32 flags, BOOL Draw);
				// Draws the item HandleDrawItem

protected:
	BOOL Initialised;								// TRUE once we have been initialised

	CWindowID ParentDlg;							// CWindowID of the parent window
	CGadgetID ParentGadget;							// Gadget ID of the combo box
	wxCamVListBoxComboPopup * m_pPopup;				// Pointer to the popup control
};


#endif

