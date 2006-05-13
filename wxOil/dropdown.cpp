// $Id: wxOil/dropdown.cpp, 1, 01-Jan-2006, Anonymous $
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

// dropdown.cpp - Drop-down list base class

// >>>> See O:\camelot\docs\howtouse\DropDowns.doc for details of use <<<<

/*
*/


//-----------------------------------------------------------------------------------------
// Include files

#include "camtypes.h"

#include "camelot.h"
#include "dropdown.h"
#include "errors.h"
#include "palman.h"


//-----------------------------------------------------------------------------------------
// Implementation of dynamic classes

CC_IMPLEMENT_DYNCREATE(DropDown, ListItem)


//-----------------------------------------------------------------------------------------
// This must be defined AFTER all CC_IMPLEMENT_DYNCREATE calls
#define new CAM_DEBUG_NEW


//-----------------------------------------------------------------------------------------
// Statics

List DropDown::CurrentDropDowns;

class wxCamVListBoxComboPopup : public wxVListBoxComboPopup
{
friend class DropDown;
public:
	wxCamVListBoxComboPopup(DropDown * pDropDown=NULL) :
		wxVListBoxComboPopup(), m_pDropDown(pDropDown) {} 
	~wxCamVListBoxComboPopup() {}

	virtual void OnDrawItem( wxDC& dc, const wxRect& rect, int /*TYPENOTE: CORRECT*/ item, int /*TYPENOTE: CORRECT*/ flags ) const
	{
		dc.SetFont( m_useFont );
		if (m_pDropDown)
			m_pDropDown->HandleDrawItemInternal(dc, rect, item, flags, TRUE);
	}

	virtual wxCoord OnMeasureItem( size_t item ) const
	{
		wxScreenDC dc;
		dc.SetFont( m_useFont );
		if (m_pDropDown)
			return m_pDropDown->HandleDrawItemInternal(dc, wxRect(-1, -1, -1, -1), item, 0, FALSE).GetHeight();
		else
			return 24;
	}

	virtual wxCoord OnMeasureItemWidth( size_t item ) const
	{
		wxScreenDC dc;
	    dc.SetFont( m_useFont );
		if (m_pDropDown)
			return m_pDropDown->HandleDrawItemInternal(dc, wxRect(-1, -1, -1, -1), item, 0, FALSE).GetWidth();
		else
			return -1; // default - will be measured from text width
	}

protected:
	DropDown * m_pDropDown;
};


/********************************************************************************************

>	DropDown::DropDown()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		29/8/95

	Purpose:	DropDown constructor

	Notes:		Follow this up with a call to Init()

	SeeAlso:	DropDown::Init; ColourDropDown; FontDropDown

********************************************************************************************/

DropDown::DropDown()
{
	ParentDlg = NULL;
	m_pPopup = NULL;
	ParentGadget = 0;
	Initialised = FALSE;
}



/********************************************************************************************

>	virtual DropDown::~DropDown()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		29/8/95

	Purpose:	DropDown destructor

********************************************************************************************/

DropDown::~DropDown()
{
	if (Initialised)
	{
		// Do any deinit here
		Initialised=FALSE;
	}

	if (ParentDlg != NULL)
		CurrentDropDowns.RemoveItem(this);
}

/********************************************************************************************

>	wxOwnerDrawnComboBox * DropDown::GetBox()

	Author:		AlexBligh
	Date:		13/05/2005
	Returns:    A pointer to the combo box
	Purpose:	Return the pointer to the combo box (once class is initialized) or NULL
				on error

********************************************************************************************/

wxOwnerDrawnComboBox * DropDown::GetBox()
{
	wxWindow * pGadget = DialogManager::GetGadget(ParentDlg, ParentGadget);
	ERROR2IF(!pGadget || !pGadget->IsKindOf(CLASSINFO(wxOwnerDrawnComboBox)), NULL, "Bad Dropdown Gadget");
	return (wxOwnerDrawnComboBox *)pGadget;
}

/********************************************************************************************

>	virtual BOOL DropDown::Init(CWindowID Window, CGadgetID Gadget)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		29/8/95

	Inputs:		Window - The Window in which your dropdown list gadget resides
				Gadget - The GadgetID of the deropdown list gadget

	Returns:	TRUE if it succeeded in connecting itself to your gadget
				FALSE if it failed

	Purpose:	DropDown initialiser

********************************************************************************************/

BOOL DropDown::Init(CWindowID Window, CGadgetID Gadget)
{
	wxWindow * pGadget = DialogManager::GetGadget(Window, Gadget);
	if (pGadget && pGadget->IsKindOf(CLASSINFO(wxOwnerDrawnComboBox)))
	{
		if (!Initialised)			// Only ever add myself to the list once
		{
			m_pPopup = new wxCamVListBoxComboPopup(this);
			ERROR2IF(!m_pPopup, FALSE, "Could not get new list popup");
			((wxOwnerDrawnComboBox *)pGadget)->SetPopupControl(m_pPopup);
			CurrentDropDowns.AddHead(this);
		}

		ParentDlg = Window;
		ParentGadget = Gadget;

		Initialised = TRUE;
		return(TRUE);
	}

	ERROR3("DropDown::Init failed - illegal Gadget");
	return(FALSE);
}



/********************************************************************************************

>	void DropDown::ClearList(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		13/9/95

	Purpose:	Clears all items from the list

********************************************************************************************/

void DropDown::ClearList(void)
{
	wxOwnerDrawnComboBox * pGadget = GetBox();
	if (!pGadget)
		return;
	pGadget->Clear();
}



/********************************************************************************************

>	void DropDown::SetListRedraw(BOOL Enable)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		13/9/95

	Inputs:		Enable - TRUE to enable, FALSE to disable redraw

	Purpose:	Enables/Disables redraw of the combobox. Put around a sequence of calls
				to AddItem to stop flickery redraws as you add items.

********************************************************************************************/

void DropDown::SetListRedraw(BOOL Enable)
{
	wxOwnerDrawnComboBox * pGadget = GetBox();
	if (!pGadget)
		return;
	if (Enable)
		pGadget->Thaw();
	else
		pGadget->Freeze();
}



/********************************************************************************************

>	void DropDown::AddItem(void * ItemData)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		13/9/95

	Inputs:		ItemData - A data value to associate with this item. This must be unique
				so you can use this ItemData to determine how to redraw the item.
				The value NULL is special: It puts a non-selectable divider line into
				the list.

	Purpose:	Adds another item or divider to the end of the current list

********************************************************************************************/

void DropDown::AddItem(void * ItemData)
{
	wxOwnerDrawnComboBox * pGadget = GetBox();
	if (!pGadget)
		return;
	m_pPopup->SetItemClientData(pGadget->Append(wxEmptyString), ItemData, wxClientData_Void);
}



/********************************************************************************************

>	void DropDown::SetSelectedIndex(INT32 SelectedIndex)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		13/9/95

	Inputs:		SelectedIndex - the index (0 based) of the item in the drop list

	Purpose:	Sets the index of the selected item in the list.
				Take care never to set a divider as the selected item (it'll work, 
				but it's a silly item to have selected!)

********************************************************************************************/

void DropDown::SetSelectedIndex(INT32 SelectedIndex)
{
	wxOwnerDrawnComboBox * pGadget = GetBox();
	if (!pGadget)
		return;
	pGadget->SetSelection(SelectedIndex);		// And set the appropriate selected item
}


/********************************************************************************************

>	void * DropDown::GetItemData(INT32 ItemIndex)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		13/9/95

	Inputs:		ItemIndex - the index (0 based) of the item in the drop list (e.g. as
				given to you by GetSelectedValueIndex() on DIM_SELECTION_CHANGED)

	Returns:	NULL, or the data word held in the indexed item

	Purpose:	Retrieves the data for a given item in the list

********************************************************************************************/

void * DropDown::GetItemData(INT32 ItemIndex)
{
	wxOwnerDrawnComboBox * pGadget = GetBox();
	if (!pGadget)
		return NULL;
	return m_pPopup->GetItemClientData(ItemIndex);
}



/********************************************************************************************

>	INT32 DropDown::GetNumberOfItems(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		13/9/95

	Returns:	A count of how many items are in the list.
				Indexes in the list will go from 0 to GetNumberOfItems()-1

	Purpose:	Determines the number of items in the list

********************************************************************************************/

INT32 DropDown::GetNumberOfItems(void)
{
	wxOwnerDrawnComboBox * pGadget = GetBox();
	if (!pGadget)
		return 0;
	return((INT32)pGadget->GetCount());
}



/********************************************************************************************

>	virtual BOOL DropDown::HandleDrawItemInternal(CWindowID hDlg, UINT32 wParam, INT32 lParam)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		29/8/95

	Inputs:		hDlg - The CWindowID of the control which needs redrawing
				wParam, lParam - As for the WM_DRAWITEM message

	Returns:	TRUE if it handled (claimed) the message
				FALSE if it did not handle the message

	Purpose:	Handles redraw of items in a DropDown control

	Notes:		Called by HandleDrawItem when this object has been identified as the owner
				of the control to be redrawn.

				This method should only be overridden in extreme circumstances

				If the item data for this item is NULL, then a divider line will be drawn.
				Otherwise, your derived DrawIcon and DrawText methods will be called.

	Scope:		private

	SeeAlso:	DropDown::HasIcon; DropDown::DrawIcon; DropDown::DrawText

********************************************************************************************/

wxSize DropDown::HandleDrawItemInternal(wxDC& dc, const wxRect& Rect, INT32 item, INT32 flags, BOOL Draw)
{
	const INT32 border = 2;

	if (CCamApp::IsDisabled())			// Inside an error handler
		return(wxDefaultSize);

	wxOwnerDrawnComboBox * pGadget = GetBox();

//	if ((INT32)pInfo->itemID == -1 || (INT32)pInfo->itemData == -1)	// Draw item -1: just exit
//		return(FALSE);

	void * ItemData = GetItemData(item);
		// Determine if it is a divider item
	if (!ItemData)
	{
		// It's a divider, so draw it specially - it is a simple black line across the center of the rectangle
		wxCoord midpoint = Rect.GetBottom()+Rect.GetHeight()/2;
		if (Draw)
			dc.DrawLine(Rect.GetLeft(), midpoint, Rect.GetRight(), midpoint);
		return(wxSize(-1,5));
	}

	// If we aren't drawing, we should get the size of the text and return that appopriately modified
	if (!Draw)
	{
		// Call the derived class
		wxRect def(-1,-1,-1,-1);
		wxSize TextSize = DrawText(ItemData, dc, def, item, FALSE); // Rect is unused here as Draw is FALSE
		TextSize.x+=2*border;
		TextSize.y+=2*border; // This gives us the bounding rect as we leave some space around it
		if (HasIcon(ItemData))
		{
			// There is an icon. It's width is equal to the text height less 2 (deflated in both
			// directions. There is also a 6 pixel space
			TextSize.x += (TextSize.y-2)+6;
		}
		return TextSize;
	}
	
	wxRect rect=Rect;
	rect.Deflate(border);

	// Calculate where the colour splodge (if any) will go (also used to shift text to the right later)
	wxRect IconRect=rect;
	IconRect.Deflate(1);
	IconRect.SetWidth(IconRect.GetHeight());

	wxRect TextRect=rect;

	wxPalette * OldPal = NULL;

	// If it's a special item with a colour splodge, or a normal colour item, draw the colour splodge
	if (HasIcon(ItemData))
	{
		if (PaletteManager::UsePalette())
			OldPal = PaletteManager::StartPaintPalette(&dc);

		// Call the derived class to draw the icon
		if (Draw)
			DrawIcon(ItemData, dc, IconRect, !pGadget->IsEnabled());

		// Shift the text to the right of the icon
		INT32 shift=IconRect.GetWidth()+6;
		TextRect.Offset(shift,0);
		INT32 NewWidth=TextRect.GetWidth()-shift;
		TextRect.SetWidth(NewWidth<1?1:NewWidth);
	}
	
	if (TextRect.GetWidth()>1)		// if there's room to draw any text, draw it
	{
		// Call derived class to draw the text
		if (Draw)
			DrawText(ItemData, dc, TextRect, item, TRUE);
	}

	// Restore the DC's previous palette if we selected our one in
	if (OldPal)
		PaletteManager::StopPaintPalette(&dc, OldPal);

	return(wxDefaultSize);
}



/********************************************************************************************

>	virtual BOOL DropDown::HasIcon(void * ItemData)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		13/9/95

	Inputs:		ItemData - Your item data

	Returns:	TRUE if this item needs an icon to the left of it, FALSE if not

	Purpose:	Determine if an item needs an icon next to it

	Notes:		Called by HandleDrawItemInternal when this object has been identified as the 
				owner of the control to be redrawn.

				This method MUST be overridden by derived classes to provide redraw of their
				dropdown list items. The base class returns FALSE

				If you return TRUE, you must also provide the DrawIcon method

	SeeAlso:	DropDown::DrawIcon; DropDown::DrawText

********************************************************************************************/

BOOL DropDown::HasIcon(void * ItemData)
{
	return(FALSE);
}



/********************************************************************************************

>	virtual BOOL DropDown::DrawIcon(void * ItemData, wxDC& dc, wxRect& IconRect, BOOL Disabled)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		13/9/95

	Inputs:		ItemData - Your item data
				dc - The DC to render into
				IconRect - points at a rectangle (square in fact) to be drawn within
				Disabled - TRUE if the item is disabled so should be drawn greyed

	Returns:	TRUE if this item needs an icon to the left of it, FALSE if not

	Purpose:	Draws the icon for an item

	Notes:		Called by HandleDrawItemInternal when this object has been identified as the 
				owner of the control to be redrawn - ONLY called if HasIcon returned TRUE

				This method MUST be overridden by derived classes to provide redraw of their
				dropdown list items. The base class does nothing.

				On entry, the DC is ready for you to draw into, including having the camelot
				palette selected in etc.

	SeeAlso:	DropDown::HasIcon; DropDown::DrawText; ColourDropDown::DrawIcon

********************************************************************************************/

BOOL DropDown::DrawIcon(void * ItemData, wxDC& dc, wxRect& IconRect, BOOL Disabled)
{
	return(TRUE);
}



/********************************************************************************************

>	virtual wxSize DropDown::DrawText(void * ItemData, wxDC& dc, wxRect& TextRect, INT32 Item, BOOL Draw)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		13/9/95

	Inputs:		ItemData - Your item data
				wxDC& - the DC to draw into
				TextRect - points at a rectangle in which the text should be rendered

	Returns:	The size of the text

	Purpose:	Draws the text for an item

	Notes:		Called by HandleDrawItemInternal when this object has been identified as the 
				owner of the control to be redrawn, if HasIcon returned TRUE

				This method MUST be overridden by derived classes to provide redraw of their
				dropdown list items. The base class draws nothing.

				Note that on entry, the text FG/BG colours have been set up appropriately
				for the state of the item (shaded, selected, etc)
				Basically, all you have to do is find the text and do a DrawText call.

	SeeAlso:	DropDown::DrawIcon; DropDown::DrawIcon; ColourDropDown::DrawText

********************************************************************************************/

wxSize DropDown::DrawText(void * ItemData, wxDC& dc, wxRect& TextRect, INT32 item, BOOL Draw)
{
	if ( m_pPopup->GetSelection() == (INT32) item )
		dc.SetTextForeground( wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT) );
	else
		dc.SetTextForeground( wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT) );

	wxString Text = GetText(ItemData);
	wxCoord w, h;
	dc.GetTextExtent(Text, &w, &h);
	wxSize size(w,dc.GetCharHeight());
	
	if (Draw)
		dc.DrawText( Text, TextRect.x, TextRect.y );

	return size;
}
