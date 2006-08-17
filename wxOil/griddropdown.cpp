// $Id: dropdown.cpp 1412 2006-07-05 19:53:47Z alex $
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
#include "griddropdown.h"
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "palman.h"


//-----------------------------------------------------------------------------------------
// Implementation of dynamic classes


//-----------------------------------------------------------------------------------------
// This must be defined AFTER all CC_IMPLEMENT_DYNCREATE calls
#define new CAM_DEBUG_NEW


//-----------------------------------------------------------------------------------------
// Statics

List CGridDropDown::CurrentDropDowns;

wxCamGridPopup::wxCamGridPopup(CGridDropDown* pDropDown, wxGridCombo* pCombo) :
	wxGridComboPopup(pCombo)
{
	m_pDropDown = pDropDown;
}

wxCamGridPopup::~wxCamGridPopup()
{
}

/******************************************************************************
Function  : wxCamGridPopup::DrawItem
Author    : Mikhail Tatarnikov
Purpose   : Draws an item.
Returns   : void
Exceptions: 
Parameters: [in] wxDC&	 dc		- the device context to draw to;
            [in] wxRect& rect	- the area of the item;
            [in] int	 iItem	- item index;
            [in] int	 iFlags - additional flags (selected, highlighted, ...)
Notes     : Delegates the call to the control class.
******************************************************************************/
void wxCamGridPopup::DrawItem(wxDC& dc, const wxRect& rect, int iItem, int iFlags)
{
	if (m_pDropDown)
		m_pDropDown->DrawItem(dc, rect, iItem, iFlags);
}







CGridDropDown::CGridDropDown()
{
	m_widParentDlg	  = NULL;
	m_pPopup		  = NULL;
	m_gidParentGadget = 0;
	m_bInitialised	  = FALSE;
}



CGridDropDown::~CGridDropDown()
{
	if (m_bInitialised)
	{
		// Do any deinit here
		Init(NULL, 0);
	}
}

/******************************************************************************
Function  : CGridDropDown::GetBox
Author    : Mikhail Tatarnikov
Purpose   : Helper function to get the control gadget
Returns   : wxGridCombo* - the combo cobtrol the object controls.
Exceptions: 
Parameters: None
Notes     : 
******************************************************************************/
wxGridCombo* CGridDropDown::GetBox()
{
	wxWindow* pGadget = DialogManager::GetGadget(m_widParentDlg, m_gidParentGadget);
	ERROR2IF(!pGadget || !pGadget->IsKindOf(CLASSINFO(wxGridCombo)), NULL, "Bad GridDropdown Gadget");
	return (wxGridCombo*)pGadget;
}


/******************************************************************************
Function  : CGridDropDown::Init
Author    : Mikhail Tatarnikov
Purpose   : Intialization of the control
Returns   : BOOL -
Exceptions: 
Parameters: [in] CWindowID Window - window the control resides;
            [in] CGadgetID Gadget - control ID.
Notes     : 
******************************************************************************/
BOOL CGridDropDown::Init(CWindowID Window, CGadgetID Gadget)
{
	if (Window)
	{
		wxWindow* pGadget = DialogManager::GetGadget(Window, Gadget);
		if (pGadget && pGadget->IsKindOf(CLASSINFO(wxGridCombo)))
		{
			m_widParentDlg = Window;
			m_gidParentGadget = Gadget;
			
			if (!m_bInitialised)			// Only ever add myself to the list once
			{
				m_pPopup = new wxCamGridPopup(this, GetBox());
				ERROR2IF(!m_pPopup, FALSE, "Could not get new list popup");
				((wxGridCombo*)pGadget)->SetPopupControl(m_pPopup);
				CurrentDropDowns.AddHead(this);
			}
	
	
			m_bInitialised = TRUE;
			return(TRUE);
		}
		ERROR3("CGridDropDown::Init failed - illegal Gadget");
		return(FALSE);
	}
	else
	{
		// release all memory
		KillList();
		Clear();
		m_widParentDlg = NULL;
		m_gidParentGadget=0;
		m_bInitialised=FALSE;
		CurrentDropDowns.RemoveItem(this);
		return TRUE;
	}
}

/********************************************************************************************

>	static void CGridDropDown::KillDropDownsByWindow(CWindowID Window)

	Author:		Alex Bligh <alex@alex.org.uk>
	Date:		15/05/2005

	Purpose:	Kills all the dropdowns associated with a particular window
				This is called when that window is dying. We can't delete the dropdowns
				but we can deinit them so they won't cause anyone eny hassle.

********************************************************************************************/
void CGridDropDown::KillDropDownsByWindow(CWindowID Window)
{
	// First kill any associated with this window
	CGridDropDown* Ptr = (CGridDropDown*) CurrentDropDowns.GetHead();
	while (Ptr != NULL)
	{
		CGridDropDown* Next = (CGridDropDown*) CurrentDropDowns.GetNext(Ptr); // as we may remove this item from the list
		if (Ptr->m_bInitialised && (Ptr->m_widParentDlg == Window))
			Ptr->Init(NULL, 0);
		Ptr = Next;
	}

	// Now process children if any
	wxWindowList::Node * pNode = Window->GetChildren().GetFirst();
	while (pNode)
	{
		KillDropDownsByWindow(pNode->GetData());
		pNode = pNode->GetNext();
	}
	return;
}



/******************************************************************************
Function  : CGridDropDown::SetListRedraw
Author    : Mikhail Tatarnikov
Purpose   : Pause/resume control drawing.
Returns   : void
Exceptions: 
Parameters: [in] BOOL Enable - TRUE for enabling redraw, FALSE otherwise.
Notes     : 
******************************************************************************/
void CGridDropDown::SetListRedraw(BOOL Enable)
{
	wxGridCombo* pGadget = GetBox();
	if (!pGadget)
		return;
	if (Enable)
		pGadget->Thaw();
	else
		pGadget->Freeze();
}



/******************************************************************************
Function  : CGridDropDown::AddItem
Author    : Mikhail Tatarnikov
Purpose   : Adds a new item
Returns   : void
Exceptions: 
Parameters: [in] CGridComboUserData* pItemData - new data to associate with the item.
Notes     : 
******************************************************************************/
void CGridDropDown::AddItem(CGridComboUserData* pItemData)
{
	wxGridCombo* pGadget = GetBox();
	pGadget->AddItem(pItemData);
}

/******************************************************************************
Function  : CGridDropDown::Clear
Author    : Mikhail Tatarnikov
Purpose   : Remove all items from the control
Returns   : void
Exceptions: 
Parameters:
Notes     : 
******************************************************************************/
void CGridDropDown::Clear()
{
	wxGridCombo* pGadget = GetBox();
	if (!pGadget)
		return;
	pGadget->Clear();
}


/******************************************************************************
Function  : CGridDropDown::DeleteItem
Author    : Mikhail Tatarnikov
Purpose   : Deletes a specific item.
Returns   : void
Exceptions: 
Parameters: [in] INT32 index - item to delete.
Notes     : 
******************************************************************************/
void CGridDropDown::DeleteItem(INT32 index)
{
}


/******************************************************************************
Function  : CGridDropDown::SetSelected
Author    : Mikhail Tatarnikov
Purpose   : Select an item.
Returns   : void
Exceptions: 
Parameters: [in] INT32 iSelectedIndex - an item to select.
Notes     : 
******************************************************************************/
void CGridDropDown::SetSelected(INT32 iSelectedIndex)
{
	wxGridCombo* pGadget = GetBox();
	pGadget->SetSelected(iSelectedIndex);
}

/******************************************************************************
Function  : CGridDropDown::GetSelected
Author    : Mikhail Tatarnikov
Purpose   : Gets the selected item.
Returns   : INT32 - the item selected, -1 if none.
Exceptions: 
Parameters: None
Notes     : 
******************************************************************************/
INT32 CGridDropDown::GetSelected()
{
	wxGridCombo* pGadget = GetBox();
	return pGadget->GetSelected();
}


/******************************************************************************
Function  : CGridDropDown::GetItemData
Author    : Mikhail Tatarnikov
Purpose   : Obtains the user data associated with the item.
Returns   : CGridComboUserData* - the user data for the item.
Exceptions: 
Parameters: [in] INT32 iItem - item to obtain the user data for.
Notes     : 
******************************************************************************/
CGridComboUserData* CGridDropDown::GetItemData(INT32 iItem)
{
	wxGridCombo* pGadget = GetBox();
	return pGadget->GetUserData(iItem);
}


/******************************************************************************
Function  : CGridDropDown::GetItemsNum
Author    : Mikhail Tatarnikov
Purpose   : Gets the number of items
Returns   : INT32 - the number of items in the control.
Exceptions: 
Parameters: None
Notes     : 
******************************************************************************/
INT32 CGridDropDown::GetItemsNum()
{
	wxGridCombo* pGadget = GetBox();
	return pGadget->GetItemsNum();
}


/******************************************************************************
Function  : CGridDropDown::SetColumns
Author    : Mikhail Tatarnikov
Purpose   : Sets the number of column to display in the popup
Returns   : void
Exceptions: 
Parameters: [in] INT32 iColumns - column number to display.
Notes     : 
******************************************************************************/
void CGridDropDown::SetColumns(INT32 iColumns)
{
	wxGridCombo* pGadget = GetBox();
	pGadget->SetColumns(iColumns);
}


/******************************************************************************
Function  : CGridDropDown::DrawShadedRect
Author    : Mikhail Tatarnikov
Purpose   : A helper function to draw a shaded box
Returns   : void
Exceptions: 
Parameters: [in] wxDC&		   dc	- device context to draw to;
            [in] const wxRect& rect - box to draw;
            [in] const wxPen&  pen1 - pen to draw top-left sides;
            [in] const wxPen&  pen2 - pen to draw bottom-right sides;
Notes     : 
******************************************************************************/
void CGridDropDown::DrawShadedRect(wxDC& dc, const wxRect& rect, const wxPen& pen1, const wxPen& pen2)
{
    // draw the rectangle
    dc.SetPen(pen1);
    dc.DrawLine(rect.GetLeft(), rect.GetTop(),
                rect.GetLeft(), rect.GetBottom());
    dc.DrawLine(rect.GetLeft() + 1, rect.GetTop(),
                rect.GetRight(), rect.GetTop());
    dc.SetPen(pen2);
    dc.DrawLine(rect.GetRight(), rect.GetTop(),
                rect.GetRight(), rect.GetBottom());
    dc.DrawLine(rect.GetLeft(), rect.GetBottom(),
                rect.GetRight() + 1, rect.GetBottom());

}

/******************************************************************************
Function  : CGridDropDown::SetItemSize
Author    : Mikhail Tatarnikov
Purpose   : Sets the size of items
Returns   : void
Exceptions: 
Parameters: [in] wxSize szItem - items size.
Notes     : 
******************************************************************************/
void CGridDropDown::SetItemSize(wxSize szItem)
{
	wxGridCombo* pGadget = GetBox();
	pGadget->SetItemSize(szItem);
}


/******************************************************************************
Function  : CGridDropDown::GetItemSize
Author    : Mikhail Tatarnikov
Purpose   : Obtains items size.
Returns   : wxSize - the items size.
Exceptions: 
Parameters: None
Notes     : 
******************************************************************************/
wxSize CGridDropDown::GetItemSize()
{
	wxGridCombo* pGadget = GetBox();
	return pGadget->GetItemSize();
}


/******************************************************************************
Function  : CGridDropDown::DrawItem
Author    : Mikhail Tatarnikov
Purpose   : Draws an item
Returns   : void
Exceptions: 
Parameters: [in] wxDC&	 dc		- the device context to draw to;
            [in] wxRect& rect	- the area of the item;
            [in] int	 iItem	- item index;
            [in] int	 iFlags - additional flags (selected, highlighted, ...)
Notes     : 
******************************************************************************/
void CGridDropDown::DrawItem(wxDC& dc, const wxRect& rect, int iItem, INT32 iFlags)
{
	// Erase the background first.
	static wxPen   penBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	static wxBrush brBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

	dc.SetPen(penBackground);
	dc.SetBrush(brBackground);

	dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

	dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);

	// Check if we have a valid item and should to continue
	// drawing.
	if (iItem >= GetItemsNum())
		return;

	wxRect rcCore = PreDrawItem(dc, rect, iItem, iFlags);

	DrawItemCore(dc, rcCore, iItem, iFlags);

	// Post-draw the item.
	PostDrawItem(dc, rect, iItem, iFlags);
}


/******************************************************************************
Function  : CGridDropDown::PreDrawItem
Author    : Mikhail Tatarnikov
Purpose   : Predraws an item (e.g. erases background)
Returns   : wxRect - the area where the item should be drawn.
Exceptions: 
Parameters: [in] wxDC&	 dc		- the device context to draw to;
            [in] wxRect& rect	- the area of the item;
            [in] int	 iItem	- item index;
            [in] int	 iFlags - additional flags (selected, highlighted, ...)
Notes     : The return rect specifies where the item should be drawn. It can be smaller
			if we draw a frame and want the item itself to be drawn inside this frame.
******************************************************************************/
wxRect CGridDropDown::PreDrawItem(wxDC& dc, const wxRect& rect, int iItem, INT32 iFlags)
{
	// Shouldn't do anything if the combobox itself is drawn.
	if (iFlags & wxGridComboPopup::keComboControl)
		return rect;
		
	wxRect rcRect = rect;

	static wxPen penDarkGrey(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW));
	static wxPen penHighlight(wxSystemSettings::GetColour(wxSYS_COLOUR_3DHIGHLIGHT));

	// Depending on whether the 
	if (iFlags & wxGridComboPopup::keHighlight)
		DrawShadedRect(dc, rect, penDarkGrey, penHighlight);
	else
		DrawShadedRect(dc, rect, penHighlight, penDarkGrey);


	// Adjust the drawing rectangle - we want the other painting to be
	// done inside our 3d rectangle.
	rcRect.Deflate(1);
	
	return rcRect;
}

/******************************************************************************
Function  : CGridDropDown::PostDrawItem
Author    : Mikhail Tatarnikov
Purpose   : Post-draws item
Returns   : void
Exceptions: 
Parameters: [in] wxDC&	 dc		- the device context to draw to;
            [in] wxRect& rect	- the area of the item;
            [in] int	 iItem	- item index;
            [in] int	 iFlags - additional flags (selected, highlighted, ...)
Notes     : Here we highlight the selected item.
******************************************************************************/
void   CGridDropDown::PostDrawItem(wxDC& dc, const wxRect& rect, int iItem, INT32 iFlags)
{
	// Shouldn't do anything if the combobox itself is drawn - no reason
	// to draw selection in the combobox itslef.
	if (iFlags & wxGridComboPopup::keComboControl)
		return;
	
	// Check whether we have to highligt the selected item or not.
	if ((iFlags & wxGridComboPopup::keSelected) == 0)
		return;

	dc.SetPen(*wxRED_PEN);
	dc.SetBrush(*wxTRANSPARENT_BRUSH);

	dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

	dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}


