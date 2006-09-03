///////////////////////////////////////////////////////////////////////////////
// Name:        gridcombo.cpp
// Purpose:     A grid grid combo
// Author:      Mikhail Tatarnikov
// Modified by:
// Created:     2006-08-16
// RCS-ID:      $Id: $
// Copyright:   (c) 2006 Xara Ltd
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "gridcombo.h"
#include <wx/utils.h>


#define HIGHLITED_TIMERID		777
#define HIGHLITED_TIMERINTERVAL	250



BEGIN_EVENT_TABLE(wxGridComboPopup, wxScrolledWindow)
	EVT_TIMER(HIGHLITED_TIMERID, wxGridComboPopup::OnTimer)
    EVT_SIZE(wxGridComboPopup::OnSize)
    EVT_MOTION(wxGridComboPopup::OnMouseMove)
    EVT_KEY_DOWN(wxGridComboPopup::OnKey)
    EVT_LEFT_UP(wxGridComboPopup::OnLeftClick)
END_EVENT_TABLE()



wxGridComboPopup::wxGridComboPopup(wxGridCombo* pCombo)
	: m_tmrHighlited(this, HIGHLITED_TIMERID)
{
	m_pCombo	  = pCombo;
	m_iHighlighed = -1;

}


/******************************************************************************
Function  : wxGridComboPopup::GetColumns
Author    : Mikhail Tatarnikov
Purpose   : A helper function to obtain the number of columns specified by the user
Returns   : int - the number of columns to display in the popup.
Exceptions: 
Parameters: None
Notes     : All the information is stored in one place - the combobox control itself.
******************************************************************************/
int wxGridComboPopup::GetColumns() const
{
	return m_pCombo->GetColumns();
}

/******************************************************************************
Function  : intwxGridComboPopup::GetRows
Author    : Mikhail Tatarnikov
Purpose   : A helper function to obtain the number of rows that should be
			displayd. Not all rows will be visible at once.
Returns   : int - the number of all rows (visible and invisible).
Exceptions: 
Parameters: None
Notes     : All the required to calculate the rows coun is stored in one
			place - the combobox control itself.
******************************************************************************/
int	wxGridComboPopup::GetRows() const
{
	return m_pCombo->GetRows();
}

/******************************************************************************
Function  : intwxGridComboPopup::GetItemsNum
Author    : Mikhail Tatarnikov
Purpose   : A helper function to obtain the number of items in the combobx.
Returns   : int - the number of items to display.
Exceptions: 
Parameters: None
Notes     : All the information is stored in one place - the combobox control itself.
******************************************************************************/
int	wxGridComboPopup::GetItemsNum() const
{
	return m_pCombo->GetItemsNum();
}

/******************************************************************************
Function  : wxGridComboPopup::GetItemSize
Author    : Mikhail Tatarnikov
Purpose   : A helper function to obtain the size of items - e.g. the popup cell
			size.
Returns   : wxSize - the items size.
Exceptions: 
Parameters: None
Notes     : All the information is stored in one place - the combobox control itself.
			All items have the same size.
******************************************************************************/
wxSize wxGridComboPopup::GetItemSize() const
{
	return m_pCombo->GetItemSize();
}

/******************************************************************************
Function  : wxGridComboPopup::GetSelected
Author    : Mikhail Tatarnikov
Purpose   : Get the currentrly selected element.
Returns   : int - the index of currently selected element, -1 if none.
Exceptions: 
Parameters: None
Notes     : 
******************************************************************************/
int wxGridComboPopup::GetSelected() const
{
	return m_pCombo->GetSelected();
}


/******************************************************************************
Function  : wxGridComboPopup::SetSelected
Author    : Mikhail Tatarnikov
Purpose   : Selects an element
Returns   : void
Exceptions: 
Parameters: [in] int iItem - an element index to select.
Notes     : 
******************************************************************************/
void wxGridComboPopup::SetSelected(int iItem)
{
	m_pCombo->SetSelected(iItem);
}


/******************************************************************************
Function  : wxGridComboPopup::Init
Author    : Mikhail Tatarnikov
Purpose   : Performs any initialization required.
Returns   : void
Exceptions: 
Parameters: None
Notes     : 
******************************************************************************/
void wxGridComboPopup::Init()
{
}


/******************************************************************************
Function  : wxGridComboPopup::Create
Author    : Mikhail Tatarnikov
Purpose   : Creates the popup window
Returns   : bool - true if successfull, false otherwise.
Exceptions: 
Parameters: [in] wxWindow* pwndParent - the parent window (combobox control).
Notes     : 
******************************************************************************/
bool wxGridComboPopup::Create(wxWindow* pwndParent)
{
	if (!wxScrolledWindow::Create(pwndParent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxBORDER_SIMPLE | wxLB_INT_HEIGHT | wxWANTS_CHARS | wxVSCROLL | wxHSCROLL))
		return false;

	SetAutoLayout(true);

	return true;
}

/******************************************************************************
Function  : wxGridComboPopup::ChangeSelection
Author    : Mikhail Tatarnikov
Purpose   : Notification that the selection has been changed.
Returns   : void
Exceptions: 
Parameters: [in] int iOldSelected - the prevoiusly selected item;
            [in] int iNewSelected - the newly selected item.
Notes     : Control has to repqint both items.
******************************************************************************/
void wxGridComboPopup::ChangeSelection(int iOldSelected, int iNewSelected)
{
	InvalidateItem(iOldSelected);
	InvalidateItem(iNewSelected);
}

/******************************************************************************
Function  : wxGridComboPopup::UpdateScrollers
Author    : Mikhail Tatarnikov
Purpose   : Update the scroll bars with the current information
			(items number, columns, sizes, etc).
Returns   : void
Exceptions: 
Parameters: None
Notes     : 
******************************************************************************/
void wxGridComboPopup::UpdateScrollers()
{
	wxSize szItem = GetItemSize();

	wxPoint ptGrid(0, 0);
	int iSelected = GetSelected();
	if (iSelected != -1)
		ptGrid = ItemToGrid(iSelected);

	SetScrollbars(szItem.x, szItem.y, GetColumns(), GetRows(),
		ptGrid.x, ptGrid.y);
}


/******************************************************************************
Function  : wxGridComboPopup::ItemToGrid
Author    : Mikhail Tatarnikov
Purpose   : Calculates position of an item based on its index
Returns   : wxPoint - the item position (row/column).
Exceptions: 
Parameters: [in] int iItem - item index to calculate positio for.
Notes     : 
******************************************************************************/
wxPoint wxGridComboPopup::ItemToGrid(int iItem)
{
	wxPoint ptGrid;
	int iCols = GetColumns();
	ptGrid.x = iItem % iCols;
	ptGrid.y = iItem / iCols;

	return ptGrid;
}

/******************************************************************************
Function  : wxGridComboPopup::ItemFromPoint
Author    : Mikhail Tatarnikov
Purpose   : Finds item based on point (in client virtual coordinates)
Returns   : int - the item index which the point belong.
Exceptions: 
Parameters: [in] wxPoint ptClient - the point to find item from.
Notes     : 
******************************************************************************/
int wxGridComboPopup::ItemFromPoint(wxPoint ptClient)
{
	// TODO: Optimise - calculate instead of brute force iterate.

	int iItemsNum = GetItemsNum();
	for (int i = 0; i < iItemsNum; ++i)
	{
		// Calculate the area of the current item.
		wxRect rcItem = GetItemRect(i);

		// Check if the item should be updated.
		if (rcItem.Inside(ptClient))
			return i;
	}

	return -1;
}

/******************************************************************************
Function  : wxGridComboPopup::GetItemRect
Author    : Mikhail Tatarnikov
Purpose   : Calculates the item area (in client virtual coordinates)
Returns   : wxRect - the area occupied by the item.
Exceptions: 
Parameters: [in] int iItem - item to calculate the area for.
Notes     : 
******************************************************************************/
wxRect wxGridComboPopup::GetItemRect(int iItem)
{
	wxPoint ptGrid = ItemToGrid(iItem);
	wxSize  szItem = GetItemSize();

	ptGrid.x *= szItem.x;
	ptGrid.y *= szItem.y;

	wxRect rcItem(ptGrid, szItem);

	return rcItem;
}


/******************************************************************************
Function  : wxGridComboPopup::ClientToScrolledVirtual
Author    : Mikhail Tatarnikov
Purpose   : Transfers a point from client to virtual (scrolled) coordinates
Returns   : void
Exceptions: 
Parameters: [in/out] int* pX - the absciss value to convert;
            [in/out] int* pY - the ordinate value to convert.
Notes     : 
******************************************************************************/
void wxGridComboPopup::ClientToScrolledVirtual(int* pX, int* pY)
{
	wxSize szItem	 = GetItemSize();
	int iXOrigin = 0;
	int iYOrigin = 0;
	GetViewStart(&iXOrigin, &iYOrigin);
	
	if (pX)
		*pX += iXOrigin * szItem.x;
		
	if (pY)
		*pY += iYOrigin * szItem.y;

}


/******************************************************************************
Function  : wxGridComboPopup::OnShow
Author    : Mikhail Tatarnikov
Purpose   : Notification that the popup is about to be shown
Returns   : void
Exceptions: 
Parameters: None
Notes     : 
******************************************************************************/
void wxGridComboPopup::OnShow()
{
}

/******************************************************************************
Function  : wxGridComboPopup::OnSize
Author    : Mikhail Tatarnikov
Purpose   : Resize notification handler.
Returns   : void
Exceptions: 
Parameters: [in] wxSizeEvent& event - size event information.
Notes     : We need to update our scroll bars.
******************************************************************************/
void wxGridComboPopup::OnSize(wxSizeEvent& event)
{
	wxScrolledWindow::OnSize(event);
	
	UpdateScrollers();
}


/******************************************************************************
Function  : wxGridComboPopup::OnMouseMove
Author    : Mikhail Tatarnikov
Purpose   : mouse movement notification handler
Returns   : void
Exceptions: 
Parameters: [in] wxMouseEvent& event - mouse event information.
Notes     : 
******************************************************************************/
void wxGridComboPopup::OnMouseMove(wxMouseEvent& event)
{
	// Convert the poin to the logical coordinates.
	wxPoint ptMouse(event.GetX(), event.GetY());
	ClientToScrolledVirtual(&ptMouse.x, &ptMouse.y);	

	// Find out what item the mouse hover over.
	int iItem = ItemFromPoint(ptMouse);

	// Check if mouse is hovered withing the same item.
	if (iItem == m_iHighlighed)
		return;

	// If an item is hightlighted we need to start "dishighlight" timer
	// since this message can be the last one before leaving the control.
	if (iItem != -1)
		m_tmrHighlited.Start(HIGHLITED_TIMERINTERVAL);

	// Repaint the old and new items.
	InvalidateItem(m_iHighlighed);
	InvalidateItem(iItem);

	m_iHighlighed = iItem;
}


/******************************************************************************
Function  : wxGridComboPopup::OnTimer
Author    : Mikhail Tatarnikov
Purpose   : Timer notification handler
Returns   : void
Exceptions: 
Parameters: [in] wxTimerEvent& event - time event information.
Notes     : We need it to trace situations when the mouse highlight an item and
			then leave the control leaving the item highlighted.
******************************************************************************/
void wxGridComboPopup::OnTimer(wxTimerEvent& event)
{
	if (m_iHighlighed == -1)
	{
		m_tmrHighlited.Stop();
		return;
	}

	wxMouseState oMouseState = wxGetMouseState();
	wxPoint ptMouse(oMouseState.GetX(), oMouseState.GetY());

	ScreenToClient(&(ptMouse.x), &(ptMouse.y));

	int iItem = ItemFromPoint(ptMouse);

	if (iItem == -1)
	{
		m_tmrHighlited.Stop();
		
		int iCurHighlighted = m_iHighlighed;
		m_iHighlighed		 = -1;
		InvalidateItem(iCurHighlighted);
	}

}


/******************************************************************************
Function  : wxGridComboPopup::OnKey
Author    : Mikhail Tatarnikov
Purpose   : Keyboard notification handler.
Returns   : void
Exceptions: 
Parameters: [in] wxKeyEvent& event - keyboard event information.
Notes     : 
******************************************************************************/
void wxGridComboPopup::OnKey(wxKeyEvent& event)
{
}


/******************************************************************************
Function  : wxGridComboPopup::OnLeftClick
Author    : Mikhail Tatarnikov
Purpose   : Left mouse button click handler
Returns   : void
Exceptions: 
Parameters: [in] wxMouseEvent& event - mouse event information.
Notes     : 
******************************************************************************/
void wxGridComboPopup::OnLeftClick(wxMouseEvent& event)
{
	// Convert the poin to the logical coordinates.
	wxPoint ptMouse(event.GetX(), event.GetY());
	ClientToScrolledVirtual(&ptMouse.x, &ptMouse.y);	
	
	// Select a new item, if any was clicked.
	int iItem = ItemFromPoint(ptMouse);
	if (iItem != -1)
		SetSelected(iItem);

	DismissWithEvent();
}

/******************************************************************************
Function  : wxGridComboPopup::DismissWithEvent
Author    : Mikhail Tatarnikov
Purpose   : Helper  function to setup a new selection, close the popup and send
			a notification message.
Returns   : void
Exceptions: 
Parameters: None
Notes     : 
******************************************************************************/
void wxGridComboPopup::DismissWithEvent()
{
    Dismiss();

	int iSelected = GetSelected();
    SendComboBoxEvent(iSelected);
}

/******************************************************************************
Function  : wxGridComboPopup::SendComboBoxEvent
Author    : Mikhail Tatarnikov
Purpose   : Sends a combobox selection event
Returns   : void
Exceptions: 
Parameters: [in] int iSelection - a new selection to notify clients about.
Notes     : 
******************************************************************************/
void wxGridComboPopup::SendComboBoxEvent(int iSelection)
{
	wxCommandEvent evt(wxEVT_COMMAND_COMBOBOX_SELECTED, m_combo->GetId());

	evt.SetEventObject(m_pCombo);

	evt.SetInt(iSelection);

	// Set client data, if any
	if (iSelection >= 0 && GetItemsNum() > iSelection)
	{
		CGridComboUserData* pClientData = m_pCombo->GetUserData(iSelection);
		evt.SetClientData(pClientData);
	}

	m_combo->GetEventHandler()->AddPendingEvent(evt);
}


/******************************************************************************
Function  : wxGridComboPopup::GetAdjustedSize
Author    : Mikhail Tatarnikov
Purpose   : Calculates preferable size.
Returns   : wxSize - the final size requested.
Exceptions: 
Parameters: [in] int iMinWidth	 - minimum width for the control. In some circumstances
								   can be ignored;
            [in] int iPrefHeight - preferable height;
            [in] int iMaxHeight	 - maximum of height allowed.
Notes     : 
******************************************************************************/
wxSize wxGridComboPopup::GetAdjustedSize(int iMinWidth, int iPrefHeight, int iMaxHeight)
{
	wxSize szAdjusted(0, 0);
	wxSize szItem	= GetItemSize();
	int	   iColumns = GetColumns();
	int	   iRows	= GetRows();
	
	szAdjusted.x = szItem.x * iColumns;

	int iRowsFit = iPrefHeight / szItem.y + 1;

	// Check if we fit the maximum height.
	int iHeight = iRowsFit * szItem.y;
	if (iHeight > iMaxHeight)
		iHeight = --iRowsFit * szItem.y;

	// We must show at least one row.
	iRowsFit = std::max(iRowsFit, 1);
	
	// No point in showing more rows than we have.
	iRowsFit = std::min(iRowsFit, iRows);

	szAdjusted.y = iRowsFit * szItem.y;

	// Check if a slider is required.
	if (iRowsFit < iRows)
		szAdjusted.x += wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);

	// Take into account the border.
	szAdjusted.x += 2;
	szAdjusted.y += 2;
	
	
	return szAdjusted;
}

/******************************************************************************
Function  : wxGridComboPopup::InvalidateItem
Author    : Mikhail Tatarnikov
Purpose   : Requests repaint of an item.
Returns   : void
Exceptions: 
Parameters: [in] int iItem - an item to repaint.
Notes     : 
******************************************************************************/
void wxGridComboPopup::InvalidateItem(int iItem)
{
	if (iItem == -1)
		return;

	int iX = 0;
	int iY = 0;
	
	wxRect rcItem = GetItemRect(iItem);
	
	ClientToScrolledVirtual(&iX, &iY);
	rcItem.x -= iX;
	rcItem.y -= iY;
	
	Refresh(false, &rcItem);
}


/******************************************************************************
Function  : wxGridComboPopup::OnDraw
Author    : Mikhail Tatarnikov
Purpose   : The default implementation of drawing.
Returns   : void
Exceptions: 
Parameters: [in] wxDC& dc - device context of the control to paint to.
Notes     : Breaks the control to separate items and sends a request for painting
			each item via DrawItem pure virtual function.
******************************************************************************/
void wxGridComboPopup::OnDraw(wxDC& dc)
{
	wxRect rcInvalid = GetUpdateClientRect();
	ClientToScrolledVirtual(&(rcInvalid.x), &(rcInvalid.y));

	// Iterate over all items, simply figing out whether we
	// need to update it or not.
	// TODO: optimize - iterate over visible items only.
	int iItemsNum = GetColumns() * GetRows();
	for (int i = 0; i < iItemsNum; ++i)
	{
		// Calculate the area of the current item.
		wxRect rcItem = GetItemRect(i);

		// Check if the item should be updated.
		if (!rcItem.Intersects(rcInvalid))
			continue;

		// Check if the item is selected or highlighted.
		int iFlags = 0;
		if (i == GetSelected())
			iFlags |= keSelected;

		if (i == m_iHighlighed)
			iFlags |= keHighlight;

		DrawItem(dc, rcItem, i, iFlags);

	}
}

/******************************************************************************
Function  : wxGridComboPopup::PaintComboControl
Author    : Mikhail Tatarnikov
Purpose   : Paints an item in the combo control itself.
Returns   : void
Exceptions: 
Parameters: [in] wxDC&		   dc	- dc to paint to;
            [in] const wxRect& rect - area for painting.
Notes     : Uses DrawItem pure virtual function.
******************************************************************************/
void wxGridComboPopup::PaintComboControl(wxDC& dc, const wxRect& rect)
{
	int iSelected = GetSelected();
	if (iSelected == -1)
	{
	    wxComboPopup::PaintComboControl(dc, rect);
		return;
	}

	int iFlags = keComboControl;
	if (!m_pCombo->IsEnabled())
		iFlags |= keDisabled;

	DrawItem(dc, rect, iSelected, iFlags);
}


/******************************************************************************
Function  : wxGridComboPopup::DrawItem
Author    : Mikhail Tatarnikov
Purpose   : Draws item based on row and column
Returns   : void
Exceptions: 
Parameters: [in] wxDC&	 dc		   - device context to paint to;
            [in] wxRect& rcRect	   - the item position;
            [in] int	 iRow	   - element's row;
            [in] int	 iCol	   - element's column;
            [in] bool	 bSelected - indicate whether the item is selected or not.
Notes     : Calculates the item index and dispatch the call to descendants via
			DrawItem pure virtual function.
******************************************************************************/
void wxGridComboPopup::DrawItem(wxDC& dc, const wxRect& rcRect, int iRow, int iCol, bool bSelected)
{
	int iItem = iRow * GetColumns() + iCol;

	int iFlags = 0;
	if (bSelected)
		iFlags |= keSelected;

	DrawItem(dc, rcRect, iItem, iFlags);
}


/******************************************************************************
Function  : wxGridComboPopup::UpdateColumnsNum
Author    : Mikhail Tatarnikov
Purpose   : Updates the coumn number.
Returns   : void
Exceptions: 
Parameters: None
Notes     : 
******************************************************************************/
void wxGridComboPopup::UpdateColumnsNum()
{
	UpdateScrollers();
}










IMPLEMENT_DYNAMIC_CLASS(wxGridCombo, wxComboCtrl)

/******************************************************************************
Function  : wxGridCombo::Init
Author    : Mikhail Tatarnikov
Purpose   : Initialization routine
Returns   : void
Exceptions: 
Parameters: None
Notes     : 
******************************************************************************/
void wxGridCombo::Init()
{
	m_iColumns	= 1;	// A standard list box with one column by default.
	m_iSelected = -1;
}

wxGridCombo::wxGridCombo(wxWindow* pwndParent,
						 wxWindowID wiID,
						 const wxPoint& crptPos,
						 const wxSize& crszSize,
						 long lStyle,
						 const wxString& crstrName)
{
    Init();
    Create(pwndParent, wiID, crptPos, crszSize, lStyle, crstrName);
}

/******************************************************************************
Function  : wxGridCombo::Create
Author    : Mikhail Tatarnikov
Purpose   : Creates the control
Returns   : bool - see wxWindow::Create
Exceptions: 
Parameters: see wxWindow::Create.
Notes     : 
******************************************************************************/
bool wxGridCombo::Create(wxWindow* pwndParent,
						   wxWindowID wiID,
						   const wxPoint& crptPos,
						   const wxSize& crszSize,
						   long lStyle,
						   const wxString& crstrName)
{
	if (!wxComboCtrl::Create(pwndParent, wiID, wxEmptyString, crptPos, crszSize, lStyle, wxDefaultValidator, crstrName))
		return false;

	return true;
}

/******************************************************************************
Function  : wxGridCombo::~wxGridCombo
Author    : Mikhail Tatarnikov
Purpose   : Destroys the object
Returns   : void
Exceptions: 
Parameters: None
Notes     :
******************************************************************************/
wxGridCombo::~wxGridCombo()
{
	DeleteUserData();
}

/******************************************************************************
Function  : wxGridCombo::DeleteUserData
Author    : Mikhail Tatarnikov
Purpose   : Deletes all user data for itmes.
Returns   : void
Exceptions: 
Parameters: None
Notes     : 
******************************************************************************/
void wxGridCombo::DeleteUserData()
{
	std::vector<CGridComboUserData*>::const_iterator citCur = m_vecUserData.begin();
	for (; citCur != m_vecUserData.end(); ++citCur)
		delete *citCur;

	m_vecUserData.clear();
}

/******************************************************************************
Function  : wxGridCombo::AddItem
Author    : Mikhail Tatarnikov
Purpose   : Adds a new item (appended to the end)
Returns   : void
Exceptions: 
Parameters: [in] CGridComboUserData* pUserData - a new item to add.
Notes     : 
******************************************************************************/
void wxGridCombo::AddItem(CGridComboUserData* pUserData)
{
	m_vecUserData.push_back(pUserData);
	
	wxGridComboPopup* pPopup = (wxGridComboPopup*)GetPopupControl();
	if (pPopup)
		pPopup->UpdateScrollers();
}

/******************************************************************************
Function  : wxGridCombo::Clear
Author    : Mikhail Tatarnikov
Purpose   : Removes all elements from the combobox
Returns   : void
Exceptions: 
Parameters: None
Notes     : 
******************************************************************************/
void wxGridCombo::Clear()
{
	DeleteUserData();
	
	wxGridComboPopup* pPopup = (wxGridComboPopup*)GetPopupControl();

	if (pPopup)
		pPopup->UpdateScrollers();
}


/******************************************************************************
Function  : wxGridCombo::GetUserData
Author    : Mikhail Tatarnikov
Purpose   : Retrieves user data associated with an item.
Returns   : CGridComboUserData* - the user data for the item, if any.
Exceptions: 
Parameters: [in] int iIndex - index of the item to retrieve associated user data for.
Notes     : 
******************************************************************************/
CGridComboUserData* wxGridCombo::GetUserData(int iIndex)
{
	if (iIndex < GetItemsNum())
		return m_vecUserData[iIndex];

	return NULL;
}

/******************************************************************************
Function  : wxGridCombo::GetRows
Author    : Mikhail Tatarnikov
Purpose   : Calculates the number of rows.
Returns   : int - the number of rows.
Exceptions: 
Parameters: None
Notes     : 
******************************************************************************/
int wxGridCombo::GetRows()
{
	// Calculate the full rows based on the item number and the columns requested.
	// Then add a partial row if required.
	int iRows = GetItemsNum() / GetColumns();
	if (GetItemsNum() % GetColumns() != 0)
		iRows++;

	return iRows;
}


/******************************************************************************
Function  : wxGridCombo::SetItemSize
Author    : Mikhail Tatarnikov
Purpose   : Sets the item size
Returns   : void
Exceptions: 
Parameters: [in] wxSize szItem - a new size for items.
Notes     : All items have the same size.
******************************************************************************/
void wxGridCombo::SetItemSize(wxSize szItem)
{
	m_szItem = szItem;

	wxGridComboPopup* pPopup = (wxGridComboPopup*)GetPopupControl();

	if (!pPopup)
		return;

	pPopup->UpdateScrollers();
}


/******************************************************************************
Function  : wxGridCombo::SetColumns
Author    : Mikhail Tatarnikov
Purpose   : Sets the number of columns
Returns   : void
Exceptions: 
Parameters: [in] int iCols - new column number.
Notes     : 
******************************************************************************/
void wxGridCombo::SetColumns(int iCols)
{
	m_iColumns = iCols;
	
	wxGridComboPopup* pPopup = (wxGridComboPopup*)GetPopupControl();

	if (!pPopup)
		return;

	pPopup->UpdateColumnsNum();
}


/******************************************************************************
Function  : wxGridCombo::SetSelected
Author    : Mikhail Tatarnikov
Purpose   : Selects an item.
Returns   : void
Exceptions: 
Parameters: [in] int iItem - an item to select, -1 for unselecting.
Notes     : 
******************************************************************************/
void wxGridCombo::SetSelected(int iItem)
{
	// Check if we actually changed anything.
	if (GetSelected() == iItem)
		return;

	wxGridComboPopup* pPopup = (wxGridComboPopup*)GetPopupControl();

	if (!pPopup)
		return;

	pPopup->ChangeSelection(GetSelected(), iItem);

	m_iSelected = iItem;

	// Invalidate the combo control.
	Refresh(false);
}

/******************************************************************************
Function  : wxGridCombo::GetSelected
Author    : Mikhail Tatarnikov
Purpose   : Gets the selected item
Returns   : int - the index of the selected item, -1 otherwise.
Exceptions: 
Parameters: None
Notes     : 
******************************************************************************/
int wxGridCombo::GetSelected() const
{
	return m_iSelected;
}
