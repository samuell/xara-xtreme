// $Id: coldrop.cpp 1052 2006-05-15 14:19:14Z alex $
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

// coldrop.cpp - Colour drop-down list support

/*
*/


//-----------------------------------------------------------------------------------------
// Include files

#include "camtypes.h"
#include "camelot.h"
#include "errors.h"
#include "palman.h"
#include "cartprov.h"

#include "bitmapdropdown.h"




CC_IMPLEMENT_DYNCREATE(CBitmapDropDown, ListItem);



//-----------------------------------------------------------------------------------------
// This must be defined AFTER all CC_IMPLEMENT_DYNCREATE calls
#define new CAM_DEBUG_NEW


CBDDItemInfo::CBDDItemInfo()
{
	m_uiBitmapResID = (UINT32)-1;
}


CBDDItemInfo::CBDDItemInfo(UINT32 uiBitmapResID, String_256 strText)
{
	m_uiBitmapResID = uiBitmapResID;
	m_bHasText		= strText.IsEmpty();
	
	if (m_bHasText)
		m_strText = strText;
	else
	{
		// Workaround the DropDown feature - it distinguish the items by
		// their names (so we can't select items with empty text.
		static UINT32 s_iDummy = 0;
		m_strText._MakeMsg(_T("%d"), ++s_iDummy);
	}
}

const CBDDItemInfo& CBDDItemInfo::operator=(const CBDDItemInfo& rOther)
{
	m_uiBitmapResID = rOther.m_uiBitmapResID;
	m_bHasText		= rOther.m_bHasText;
	m_strText		= rOther.m_strText;

	return *this;
}

UINT32 CBDDItemInfo::GetBitmapID()
{
	return m_uiBitmapResID;
}

String_256 CBDDItemInfo::GetText()
{
	return m_strText;
}
	
BOOL CBDDItemInfo::HasText()
{
	return m_bHasText;
}











/******************************************************************************
Function  : wxCamBitmapDropdownPopup::PaintComboControl
Author    : Mikhail Tatarnikov
Purpose   : Paints the currently selected item into the control itself (not to the dropdown list)
Returns   : void
Exceptions: 
Parameters: [in] wxDC&		   dc	- context to draw to;
            [in] const wxRect& rect - the item size.
Notes     : The default behaivour is to leave the control blank if no item is selected.
******************************************************************************/
void wxCamBitmapDropdownPopup::PaintComboControl(wxDC& dc, const wxRect& rect)
{
    if (!(m_combo->GetWindowStyle() & wxODCB_STD_CONTROL_PAINT))
    {
//		m_combo->DrawFocusBackground(dc,rect,0);
        
        // Draw the control regardless whether it has an item selected or not.
		OnDrawItem(dc, rect, m_value, wxODCB_PAINTING_CONTROL);
		
		return;
    }

    wxComboPopup::PaintComboControl(dc,rect);
}

/******************************************************************************
Function  : wxCamBitmapDropdownPopup::GetAdjustedSize
Author    : Mikhail Tatarnikov
Purpose   : Calculates the dropdown list size
Returns   : wxSize - the popup list size.
Exceptions: 
Parameters: [in] INT32 minWidth   - minimum width; in the current implemetation it's the 
								  size of combobox control itself. In this function we
								  ignore it;
            [in] INT32 prefHeight - the preffered height;
            [in] INT32 maxHeight  - maximum allowed height.
Notes     : 
******************************************************************************/
wxSize wxCamBitmapDropdownPopup::GetAdjustedSize(INT32 minWidth, INT32 prefHeight, INT32 maxHeight)
{
	// We should ignore the min width, since it's usually the size of the combobox
	// (we can have a dropdown with width less than the control).
	
    INT32  height			= 250;
    BOOL bNeedScrollbar = FALSE;

    if ( m_strings.GetCount() )
    {
        if ( prefHeight > 0 )
            height = prefHeight;

        if ( height > maxHeight )
            height = maxHeight;

        INT32 totalHeight = GetTotalHeight(); // + 3;
        if ( height >= totalHeight )
        {
            height = totalHeight;
        }
        else
        {
            // Adjust height to a multiple of the height of the first item
            // NB: Calculations that take variable height into account
            //     are unnecessary.
            INT32 fih = GetLineHeight(0);
            INT32 shown = height/fih;
            height = shown * fih;
            
            bNeedScrollbar = TRUE;
        }
    }
    else
        height = 50;

    // Take scrollbar into account in width calculations
    INT32 iWidth = GetWidestItemWidth();
	if (bNeedScrollbar)
    	iWidth += wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
//	INT32 widestWidth = m_widestWidth + wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
    
    return wxSize(iWidth, height+2);
}


CBitmapDropDown::CBitmapDropDown()
{
}



CBitmapDropDown::~CBitmapDropDown()
{
}




/******************************************************************************
Function  : CBitmapDropDown::AddItem
Author    : Mikhail Tatarnikov
Purpose   : Adds a new item (icon + text)
Returns   : void
Exceptions: 
Parameters: [in] UINT32		uiBitmapResID - bitmap resource for the item;
            [in] String_256 strText		  - item label.
Notes     : 
******************************************************************************/
void CBitmapDropDown::AddItem(UINT32 uiBitmapResID, String_256 strText)
{
	m_vecItems.push_back(CBDDItemInfo(uiBitmapResID, strText));

	// Get the point to the last stored 
	CBDDItemInfo* poItem = &(m_vecItems[m_vecItems.size() - 1]);
	DropDown::AddItem((void*)poItem);

	SetSelectedIndex(0);
}

/******************************************************************************
Function  : CBitmapDropDown::AddDivider
Author    : Mikhail Tatarnikov
Purpose   : Add a divider
Returns   : void
Exceptions: 
Parameters: None
Notes     : 
******************************************************************************/
void CBitmapDropDown::AddDivider()
{
	DropDown::AddItem(NULL);
}

/******************************************************************************
Function  : CBitmapDropDown::DeleteItem
Author    : Mikhail Tatarnikov
Purpose   : Removes an item from dropdown
Returns   : void
Exceptions: 
Parameters: [in] INT32 iIndex - index of an item to be removed.
Notes     : 
******************************************************************************/
void CBitmapDropDown::DeleteItem(INT32 iIndex)
{
	ASSERT(FALSE);	// Not implemented yet.
}


/******************************************************************************
Function  : CBitmapDropDown::GetItemData
Author    : Mikhail Tatarnikov
Purpose   : Obtains the data for an item.
Returns   : void* - the pointer to the data.
Exceptions: 
Parameters: [in] INT32 iItemIndex - item index to get data for.
Notes     : Since the information is stored in the internal array (not in the combobox
			itself), we need to override the default behaivour.
******************************************************************************/
void* CBitmapDropDown::GetItemData(INT32 iItemIndex)
{
	// Check if we are requested information about "unselected" item.
	if (iItemIndex < 0)
		return (void*)(&m_oUnselectedItemInfo);

	// The normal items are stored in the items collection.
	CBDDItemInfo* poItem = &(m_vecItems[iItemIndex]);

	return (void*)poItem;
}


/******************************************************************************
Function  : CBitmapDropDown::HasIcon
Author    : Mikhail Tatarnikov
Purpose   : Determines if the item (represented by its data) has an icon
Returns   : BOOL - TRUE if icon was supplied for the item, FALSE otherwise.
Exceptions: 
Parameters: [in] void* pvItemData - the item data.
Notes     : 
******************************************************************************/
BOOL CBitmapDropDown::HasIcon(void* pvItemData)
{
	CBDDItemInfo* pItemInfo = reinterpret_cast<CBDDItemInfo*>(pvItemData);
	if (pItemInfo->GetBitmapID() != (UINT32)-1)
		return TRUE;

	return FALSE;
}

/******************************************************************************
Function  : CBitmapDropDown::HasText
Author    : Mikhail Tatarnikov
Purpose   : Determines whether an item has text label
Returns   : BOOL - TRUE if text was supplied for the item, FALSE otherwise.
Exceptions: 
Parameters: [in] void* pvItemData - the item data.
Notes     : 
******************************************************************************/
BOOL CBitmapDropDown::HasText(void* pvItemData)
{
	CBDDItemInfo* pItemInfo = reinterpret_cast<CBDDItemInfo*>(pvItemData);
	return pItemInfo->HasText();
}


/******************************************************************************
Function  : CBitmapDropDown::GetText
Author    : Mikhail Tatarnikov
Purpose   : Obtains label for the item.
Returns   : wxString -
Exceptions: 
Parameters: [in] void* pvItemData - the item data;
            [in] INT32 iItem	  - the item index.
Notes     : 
******************************************************************************/
wxString CBitmapDropDown::GetText(void* pvItemData, INT32 iItem)
{
	if (!pvItemData)
		return DropDown::GetText(pvItemData, iItem);

	CBDDItemInfo* pItemInfo = reinterpret_cast<CBDDItemInfo*>(pvItemData);
	
	return pItemInfo->GetText();
}



/******************************************************************************
Function  : CBitmapDropDown::Init
Author    : Mikhail Tatarnikov
Purpose   : Initialize the control
Returns   : BOOL - TRUE if success, FALSE otherwise.
Exceptions: 
Parameters: [in] CWindowID Window - the parent window, NULL to deinitialize;
            [in] CGadgetID Gadget - the control ID.
Notes     : 
******************************************************************************/
BOOL CBitmapDropDown::Init(CWindowID Window, CGadgetID Gadget)
{
	if (Window)
	{
		// Obtain the gadget and check if it has an appropriate class (we can
		// be attached to owner-draw combobox obly).
		wxWindow * pGadget = DialogManager::GetGadget(Window, Gadget);
		if (pGadget && pGadget->IsKindOf(CLASSINFO(wxOwnerDrawnComboBox)))
		{
			if (!Initialised)			// Only ever add myself to the list once
			{
				m_pPopup = new wxCamBitmapDropdownPopup(this);
				ERROR2IF(!m_pPopup, FALSE, "Could not get new list popup");
				((wxOwnerDrawnComboBox *)pGadget)->SetPopupControl(m_pPopup);
				CurrentDropDowns.AddHead(this);
			}
	
			ParentDlg	 = Window;
			ParentGadget = Gadget;
	
			Initialised = TRUE;
			return(TRUE);
		}
		ERROR3("CBitmapDropDown::Init failed - illegal Gadget");
		return(FALSE);
	}
	else
	{
		// release all memory
		KillList();
		ClearList();
		ParentDlg=NULL;
		ParentGadget=0;
		Initialised=FALSE;
		CurrentDropDowns.RemoveItem(this);
		return TRUE;
	}
}


/******************************************************************************
Function  : CBitmapDropDown::HandleDrawItemInternal
Author    : Mikhail Tatarnikov
Purpose   : Handles items drawing
Returns   : wxSize - the size of the item.
Exceptions: 
Parameters: [in] wxDC& dc			- device context to draw to;
            [in] const wxRect& Rect - the clipping area for the item;
            [in] INT32 item			- item index;
            [in] INT32 flags		- flags (see odcombo.h, the only specified flag at the
									  moment of writing is wxCP_PAINTING_CONTROL);
            [in] BOOL  Draw			- TRUE if we should draw, FALSE in case of size measurement.
Notes     : 
******************************************************************************/
wxSize CBitmapDropDown::HandleDrawItemInternal(wxDC& dc, const wxRect& Rect, INT32 item, INT32 flags, BOOL Draw)
{
	const INT32 ciBorderSize = 2;
	const INT32 ciInterval	 = 6;


	if (CCamApp::IsDisabled())			// Inside an error handler
		return (wxDefaultSize);

//	wxOwnerDrawnComboBox* pGadget = GetBox();

//	if ((INT32)pInfo->itemID == -1 || (INT32)pInfo->itemData == -1)	// Draw item -1: just exit
//		return(FALSE);

	void* pvItemData = GetItemData(item);
	
	// Determine if it is a divider item
	if (!pvItemData)
	{
		// It's a divider, so draw it specially - it is a simple black line across the center of the rectangle
		if (Draw)
		{
			wxCoord midpoint = Rect.GetTop()+Rect.GetHeight()/2;
			wxPen OldPen=dc.GetPen();
			dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT)));
			dc.DrawLine(Rect.GetLeft(), midpoint, Rect.GetRight()+1, midpoint);
			dc.SetPen(OldPen);
		}
		return(wxSize(-1,5));
	}

//	CBDDItemInfo* pItemInfo = reinterpret_cast<CBDDItemInfo*>(pvItemData);

	if (!Draw)
	{
		// The item height is the icon height, if any. We don't shrink the icon.
		// If there's no icon, the height is the text height.
		wxRect rcDummy(-1, -1, -1, -1);

		// Obtain the icon size.
		wxSize szIcon(0, 0);
		if (HasIcon(pvItemData))
			szIcon = DrawIcon(dc, rcDummy, item, flags, FALSE);

		// Obtain the text size.
		wxSize szText(0, 0);
		if (HasText(pvItemData))
			szText = DrawText(pvItemData, dc, rcDummy, item, flags, FALSE);

		wxSize szItem;
		szItem.y = max(szText.y, szIcon.y);
		szItem.x = szIcon.x + szText.x;

		// If both text and icon exist, there should be an interval between them
		if (HasIcon(pvItemData) && HasText(pvItemData))
			szItem.x += ciInterval;

		// Add a border.
		szItem.x += 2*ciBorderSize;
		szItem.y += 2*ciBorderSize;

		return szItem;
	}



	wxRect rcTemp = Rect;
	rcTemp.Deflate(ciBorderSize);

	wxPalette* popalOld = NULL;

	// Draw the icon, if any.
	if (HasIcon(pvItemData))
	{
		if (PaletteManager::UsePalette())
			popalOld = PaletteManager::StartPaintPalette(&dc);

		wxSize szIcon = DrawIcon(dc, rcTemp, item, flags, TRUE);

		// Calculate the text size.
		INT32 iShift = szIcon.x + ciInterval;
		rcTemp.Offset(iShift, 0);
		
		INT32 iNewWidth = rcTemp.GetWidth() - ciInterval;
		rcTemp.SetWidth(iNewWidth < 1 ? 1 : iNewWidth);
	}


	
	if (HasText(pvItemData))
		DrawText(pvItemData, dc, rcTemp, item, flags, TRUE);

	// Restore the DC's previous palette if we selected our one in
	if (popalOld)
		PaletteManager::StopPaintPalette(&dc, popalOld);

	return(wxDefaultSize);
}


/******************************************************************************
Function  : CBitmapDropDown::DrawIcon
Author    : Mikhail Tatarnikov
Purpose   : Draws item icon
Returns   : wxSize - the icon size
Exceptions: 
Parameters: [in] wxDC& dc			- device context to draw to;
            [in] const wxRect& Rect - the clipping area for the item;
            [in] INT32 item			- item index;
            [in] INT32 flags		- flags (see odcombo.h, the only specified flag at the
									  moment of writing is wxCP_PAINTING_CONTROL);
            [in] BOOL  Draw			- TRUE if we should draw, FALSE in case of size measurement.
Notes     : 
******************************************************************************/
wxSize CBitmapDropDown::DrawIcon(wxDC& dc, const wxRect& rcIcon, INT32 item, INT32 flags, BOOL Draw)
{
//	wxBitmap * pBitmap = CamArtProvider::Get()->FindBitmap(BitmapID, (CamArtFlags)(CAF_DEFAUL T| (Disabled?CAF_GREYED:0)));
	wxSize szBitmap(0, 0);
	
	void* pvItemData = GetItemData(item);
	CBDDItemInfo* pItemInfo = reinterpret_cast<CBDDItemInfo*>(pvItemData);

	wxBitmap* pBitmap = CamArtProvider::Get()->FindBitmap(pItemInfo->GetBitmapID(), CAF_DEFAULT);
	if (!pBitmap)
		return szBitmap;

	szBitmap.x = pBitmap->GetWidth();
	szBitmap.y = pBitmap->GetHeight();

	if (Draw)
	{
		dc.DrawBitmap(*pBitmap, rcIcon.GetLeft(), rcIcon.GetTop(), TRUE);
/*		
		// Calculate the destination bitmap size (the bitmap can be bigger than the draw rect, so
		// we need to shrink it.
		wxSize szDestination;
		szDestination.x = min(szBitmap.x, rcIcon.GetWidth());
		szDestination.y = min(szBitmap.y, rcIcon.GetHeight());
		
		wxMemoryDC dcMem;
		dcMem.SelectObject(*pBitmap);

		dc.Blit(rcIcon.GetLeft(), rcIcon.GetTop(), szDestination.x, szDestination.y, &dcMem, 0, 0);
*/
	}
	return szBitmap;
}


/******************************************************************************
Function  : CBitmapDropDown::SetUnselectedIntem
Author    : Mikhail Tatarnikov
Purpose   : Sets the information for "unselected item"
Returns   : void
Exceptions: 
Parameters: [in] UINT32		uiBitmapID - the "unselected" item bitmap;
            [in] String_256 strText	   - label for the item.
Notes     : These bitmap and label will be drawn when no item is selected
			(by default nothing is drawn)
******************************************************************************/
void CBitmapDropDown::SetUnselectedIntem(UINT32 uiBitmapID, String_256 strText)
{
	m_oUnselectedItemInfo = CBDDItemInfo(uiBitmapID, strText);
}

