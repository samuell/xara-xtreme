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


// CustomList.cpp : implementation file
//
// Contains the classes that make up the custom list control. These are :
//
// CCustomList					-	represents a custom list control to the 
//									outside world. A calling class interacts with
//									this class, not the helpers. (Hence perhaps
//									the helper classes should be private nested?)
// CCustomListScrollableArea	-	helper class containing the whole scrollable
//									area, some of which may not be visible (ie.
//									the verticle area is less than the area of
//									the containing CCustomList
// CCustomListRowWnd			-	helper class representing a list row
//
// Author:						MarcP
// Date:						18/11/04

#include "camtypes.h" 
#include "customlist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//								CCustomList                                //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

const INT32		CCustomList::MAXCOLUMNS = 8;
const INT32		CCustomList::MAXROWS = 100;
const INT32		CCustomList::FONTHEIGHT = 14;
const INT32		CCustomList::ROWHEIGHT = 17 ;
const INT32		CCustomList::COLOUR_PATCH_WIDTH  =	12 ;
const INT32		CCustomList::COLOUR_PATCH_HEIGHT = 12 ;
const CString	CCustomList::WNDCLASSNAME = "cc_CustomList";

BEGIN_MESSAGE_MAP(CCustomList, CWnd)
	//{{AFX_MSG_MAP(CCustomList)
	ON_WM_CREATE()
	ON_WM_VSCROLL()
	ON_WM_SETFOCUS()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CCustomList constructor
CCustomList::CCustomList() :
	m_VScrollBar(NULL),
	m_ScrollableArea(NULL)
{
	// create column positioning data
	m_ColumnOffsetsArray = new INT32[MAXCOLUMNS];
	m_ColumnOffsetsArray[0] = 10; // indent for first offset
	for( INT32 i=1; i < MAXCOLUMNS ; i++)
		m_ColumnOffsetsArray[i] = -1;
}

// Intial window procedure for the custom control, specified in the WNDCLASS structure - see RegisterWindowClass.
// Once registered Dialogs creating a cc_CustomList window will start through here. It is only called once per 
// instance so once we hahave created the object we can then hand over to MFC to do manage the rest.
extern "C" LRESULT CALLBACK EXPORT CCustomList::CustomWindowProc(HWND hWnd, UINT32 nMsg, WPARAM wParam, LPARAM lParam)
{
	// Create CCustomList object and attach it to window
	CCustomList* pWnd = new CCustomList;
	pWnd->Attach(hWnd);
	// Switch over to MFC WndProc
	::SetWindowLong(hWnd, GWL_WNDPROC, (DWORD)AfxWndProc);
	// and call it
	return ::CallWindowProc(AfxWndProc, hWnd, nMsg, wParam, lParam);
}

// register the the window class for this custom control. this must be executed before
// invoking any dialogs that use the control, because it specifies the WndProc that 
// they must use to create it
BOOL CCustomList::RegisterWindowClass()
{
    HINSTANCE hInst = AfxGetInstanceHandle();
	WNDCLASS wc;
	if (!(::GetClassInfo(hInst, WNDCLASSNAME, &wc)))
	{
		// Fill in the values for the class
		wc.style         = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS ;
		wc.lpfnWndProc   = (WNDPROC) CCustomList::CustomWindowProc;
		wc.cbClsExtra       = wc.cbWndExtra = 0;
		wc.hInstance     = hInst;
		wc.hIcon         = NULL;
		wc.hCursor       = LoadCursor (NULL, _R(IDC_ARROW));
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = (LPSTR) NULL;
		wc.lpszClassName = (LPCTSTR) WNDCLASSNAME;

		// try and register the class
		if (!RegisterClass (&wc))
		{
			ASSERT(FALSE);
			return FALSE;
		}
	}
    return TRUE;
}

CCustomList::~CCustomList()
{
	// delete column offsets array
	delete m_ColumnOffsetsArray;
	// delete CScrollbar
	delete m_VScrollBar ;
}

// Called by the default OnNcDestroy member function after the window has been destroyed.
void CCustomList::PostNcDestroy() 
{
	delete this;
	CWnd::PostNcDestroy();
}


// Static function to retrieve a pointer to a custom list gadget knowing its resource ID and the parent's handle
CCustomList* CCustomList::GetGadget(CWindowID hDlg, CGadgetID nIDDlgItem)
{
	HWND listwnd = ::GetDlgItem(hDlg, nIDDlgItem);
	CCustomList* pListGadget = (CCustomList*)CWnd::FromHandlePermanent(listwnd);
	return pListGadget ;
}


// Adds an item to the list (pItemImage points to an optional bitmap associated with the item and displayed to its right
BOOL CCustomList::AddItem(StringBase& itemString, KernelBitmap* pItemImage)
{
	CCustomListRowWnd* pNewRow = GetScrollableArea()->AddRow();
	pNewRow ->AddCheck(0);
	pNewRow ->AddText(1,(TCHAR *)itemString);
	return TRUE;
}

// Adds a "references" item to the list
BOOL CCustomList::AddRefsItem(UINT32 idStatusBitmap, StringBase& strItemName, StringBase& strDetails)
{
	CCustomListRowWnd* pNewRow = GetScrollableArea()->AddRow();
	if (pNewRow)
	{
		if (idStatusBitmap!=0)
		{
			HBITMAP hBitmap = LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(idStatusBitmap));
			pNewRow->AddBitmap(0, hBitmap, NULL, 0xFFFFFF);	// We know background colour is white for these bitmaps!
		}
		pNewRow->AddText(1, (TCHAR*)strItemName);
		pNewRow->AddText(2, (TCHAR*)strDetails);
		return TRUE;
	}

	return FALSE;
}

// Retrieve the text of an item or one of its components
BOOL CCustomList::GetItemString(StringBase& itemString, UINT32 itemIndex, UINT32 columnIndex) const
{
	CString s = GetScrollableArea()->GetRow(itemIndex)->GetText(columnIndex);
	itemString.Empty(); // ensure buffer is empty
	itemString+= s;
	return TRUE;
}

// Retrieves the current state of a switch (ON/OFF)
BOOL CCustomList::GetSwitchState(UINT32 itemIndex, UINT32 switchIndex) const
{
	return GetScrollableArea()->GetRow(itemIndex)->IsChecked(switchIndex);
}

// Returns the number of items contained in the list
INT32 CCustomList::GetItemCount() const
{
	return GetScrollableArea()->m_RowCount;
}

// Returns the index of the item currently selected, -1 if none 
INT32 CCustomList::GetSelectedItemIndex() const
{
	return GetScrollableArea()->m_CurrentSelectedRow;
}

// Sets the state of a switch (determines whether the bitmapOn or bitmapOf is shown)
BOOL CCustomList::SetSwitchState(BOOL state, UINT32 itemIndex, UINT32 switchIndex)
{
	GetScrollableArea()->GetRow(itemIndex)->SetChecked(switchIndex,state);
	return TRUE;
}

// Adds an item to the list, creating the associated image from a bitmap resource ID
BOOL CCustomList::AddItem(StringBase& itemString, UINT32 bitmapEnabledID, UINT32 bitmapDisabledID)
{
	HBITMAP hItemBitmapEnabled = LoadBitmap (AfxGetResourceHandle(),MAKEINTRESOURCE(bitmapEnabledID));
	// the greyed bitmap feature is unimplemented for now
	//HBITMAP hItemBitmapDisabled = LoadBitmap (AfxGetResourceHandle(),MAKEINTRESOURCE(bitmapDisabledID));

	// note: RowWnd objects destroy any bitmaps on deletion 
	CCustomListRowWnd* pNewRow = GetScrollableArea()->AddRow();
	pNewRow ->AddCheck(0);
	pNewRow ->AddBitmap(1,hItemBitmapEnabled,NULL);
	pNewRow ->AddText(2,(TCHAR *)itemString);

	return TRUE;
}

// Sets the given item to be selected (and deselects all other items)
void CCustomList::SetSelectedItemIndex(INT32 NewSel)
{
	GetScrollableArea()->SelectRow(NewSel);
}

// deletes the list by deleting the scrollable area and replacing it with a new one
BOOL CCustomList::DeleteAllItems()
{
	if(m_ScrollableArea)
	{
		m_ScrollableArea->DestroyWindow();
		//delete m_ScrollableArea;
	}
	NewScrollableArea();
	return TRUE;
}

// creates a new scrollable area
void CCustomList::NewScrollableArea()
{
	m_ScrollableArea = new CCustomListScrollableArea(this);
	BOOL b = m_ScrollableArea->Create(_T("STATIC"), NULL, WS_CHILD | WS_VISIBLE,CRect(0,0,0,0),this,0,NULL);
}

// called on creation. creates the v scroller and a new scrollable area
INT32 CCustomList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_VScrollBar = new CScrollBar();
	CRect parentRect;
	CRect rect;
	GetClientRect(&parentRect);
	rect.left = parentRect.right - ::GetSystemMetrics(SM_CXVSCROLL) ;
	rect.top = 0;
	rect.right = parentRect.right ;
	rect.bottom = parentRect.bottom ;
	m_VScrollBar->Create(SBS_VERT, rect, this, NULL);
	m_VScrollBar->ShowWindow(SW_SHOW);

	NewScrollableArea();

	return 0;
}

// handle a scroll message
void CCustomList::OnVScroll(UINT32 nSBCode, UINT32 nPos, CScrollBar* pScrollBar) 
{
	m_ScrollableArea->HandleScrollMessage(nSBCode, nPos) ;
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

// helper function - creates a bitmap of the specified width and height
HBITMAP CreateScreenCompatibleBitmap( INT32 width, INT32 height)
{
	HDC hProbeDC = ::CreateCompatibleDC(NULL); // device context used to probe the current screen mode
	ERROR3IF(hProbeDC == NULL, "Couldn't create probe DC");
	const BITMAP bitmapData = 
	{ 
		0,
		width,
		height,
		width * GetDeviceCaps(hProbeDC, BITSPIXEL) * GetDeviceCaps(hProbeDC, PLANES) / 8 ,
		GetDeviceCaps(hProbeDC, PLANES),
		GetDeviceCaps(hProbeDC, BITSPIXEL),
		0L
	};
	DeleteDC(hProbeDC);
	return CreateBitmapIndirect(&bitmapData);
}

// Used if the the listitem is a actually a colour - it displays a square colour patch next to the color name
BOOL CCustomList::AddColourListItem(StringBase& colourName, INT32 red, INT32 green, INT32 blue, BOOL IsSpotColour)
{
	//First create and insert the colour patch associated with the item
	//Create a memory DC and 2 bitmaps (enbled/disabled) compatible with the screen 
	HDC hDC = ::CreateCompatibleDC(NULL);
	ERROR3IF(hDC == NULL, "Couldn't create rendering DC");

	// do not create grey bitmaps for now (to add back in extend array to 2 and comment
	// below back in)
	HBITMAP bitmaps[1];
	bitmaps[0] = CreateScreenCompatibleBitmap(COLOUR_PATCH_WIDTH,	COLOUR_PATCH_HEIGHT);
	//bitmaps[1] = CreateScreenCompatibleBitmap(COLOUR_PATCH_WIDTH,	COLOUR_PATCH_HEIGHT);
	//ERROR2IF(!(bitmaps[0] && bitmaps[1]), FALSE, "GDI Error");

	// And draw the colour patch into the bitmap
	for (INT32 i = 0; i <= 1; i++)
	{
		if (hDC != NULL)
		{
			HBITMAP OldBitmap = (HBITMAP) ::SelectObject(hDC, bitmaps[i]);

			COLORREF Colour = RGB(red, green, blue);
			if (i != 0)
			{
				// Convert the colour to a greyscale
				BYTE Grey = BYTE((red * 0.305) + (green * 0.586) + (blue * 0.109));
				Colour = RGB(Grey, Grey, Grey);
			}

			HBRUSH Brush = ::CreateSolidBrush(Colour);

			ERROR3IF(Brush == NULL, "Couldn't create brush");

			HPEN BlackPen = (HPEN) ::GetStockObject(BLACK_PEN);

			HBRUSH WhiteBrush	= (HBRUSH) ::GetStockObject(WHITE_BRUSH);
			HPEN NullPen		= (HPEN) ::GetStockObject(NULL_PEN);
			HPEN OldPen			= (HPEN) ::SelectObject(hDC, NullPen);
			HBRUSH OldBrush		= (HBRUSH) ::SelectObject(hDC, WhiteBrush);
			if (IsSpotColour)
			{
				// Spot colours are drawn as circles - but first, fill the bitmap with white,
				// so that the un-covered corners of the square are a sensible colour.

				::Rectangle(hDC, 0, 0, COLOUR_PATCH_WIDTH+1, COLOUR_PATCH_HEIGHT+1);
				::SelectObject(hDC, BlackPen);
				::SelectObject(hDC, Brush);
				::Ellipse(hDC, 0, 0, COLOUR_PATCH_WIDTH, COLOUR_PATCH_HEIGHT);

			}
			else
			{
				// Process colours are shown as rectangles

				::Rectangle(hDC, 0, 0, COLOUR_PATCH_WIDTH+1, COLOUR_PATCH_HEIGHT+1);
				::SelectObject(hDC, BlackPen);
				::SelectObject(hDC, Brush);
				::Rectangle(hDC, 0, 0, COLOUR_PATCH_WIDTH-1, COLOUR_PATCH_HEIGHT-1);
			
			}

			::SelectObject(hDC, OldPen);
			::SelectObject(hDC, OldBrush);
			::SelectObject(hDC, OldBitmap);

			::DeleteObject(Brush);
		}
	}

	DeleteDC(hDC);
//	this->AddItem(colourName,bitmaps[0],bitmaps[1]);
	CCustomListRowWnd* pNewRow = GetScrollableArea()->AddRow();
	pNewRow ->AddCheck(0);
	pNewRow ->AddCheck(1);
	pNewRow ->AddBitmap(2,bitmaps[0],NULL);
	pNewRow ->AddText(3,(TCHAR*)colourName);

	return TRUE;
}

// Sets the text for an item or one of its components
BOOL CCustomList::SetItemString(StringBase& itemString, UINT32 itemIndex, UINT32 columnIndex)
{
	GetScrollableArea()->GetRow(itemIndex)->SetText(columnIndex,(TCHAR*)  itemString);
    return TRUE;
}

// helper function to loop through and enable / disable all child windows
void EnableDescendants(HWND hWnd, BOOL enable)
{
	// walk through HWNDs to avoid creating temporary CWnd objects
	// unless we need to call this function recursively
	for (HWND hWndChild = ::GetTopWindow(hWnd); hWndChild != NULL;
		hWndChild = ::GetNextWindow(hWndChild, GW_HWNDNEXT))
	{
		CWnd* pWnd = CWnd::FromHandlePermanent(hWndChild);
		if (pWnd != NULL)
		{
			pWnd->EnableWindow(enable);
			pWnd->RedrawWindow();
		}
		if (::GetTopWindow(hWndChild) != NULL)
		{
			// send to child windows after parent
			EnableDescendants(hWndChild,enable);
		}
	}
}

// We are using our own enabler function as the Windows one looks awful (called by the DialogManager)
// DO NOT call DialogManager::EnableGadget on a control before you have created it!
BOOL CCustomList::SetEnabled(BOOL enabled)
{
	GetScrollableArea()->SelectRow(-1); // deselect any selected row 
	EnableWindow(enabled);
	EnableDescendants(m_hWnd,enabled);
	return TRUE;
}

// sets the offset of the numbered column by adding the specified width to preceding offsets
void	CCustomList::SetColumnWidth(INT32 colnum,INT32 colwidth)
{
	ASSERT(colnum + 1 < MAXCOLUMNS);

	// set at colnum+1
	///
	ERROR3IF(m_ColumnOffsetsArray[colnum]==-1,"SetColumnWidth - illegal column offset. ensure preceding column widths are set");
	m_ColumnOffsetsArray[colnum+1] = m_ColumnOffsetsArray[colnum] + colwidth;
}

// helper function to change the background colour of the bitmap to the one used by dialog backgrounds
BOOL SetBitmapBkgToSystem(HBITMAP hBitmap)
{
	BITMAP bitmap;
	HDC hBitmapDC = CreateCompatibleDC(NULL);
	if (!GetObject(hBitmap, sizeof(bitmap), &bitmap) || !hBitmapDC)
	{
		ERROR2RAW("Non-fatal GDI error");
		return(FALSE);
	}
	SelectObject(hBitmapDC, hBitmap);
	// We make the assumption that the pixel in the lower right corner has the background colour 
	DWORD currentBkColour = (DWORD) GetPixel(hBitmapDC, bitmap.bmWidth - 1, bitmap.bmHeight -1); 
	DWORD sysBkColour = GetSysColor(COLOR_3DFACE);
	for (INT32 i = 0; i < bitmap.bmWidth; i++)
	{
		for (INT32 j = 0; j < bitmap.bmHeight; j++)
		{
			if ((DWORD) GetPixel(hBitmapDC, i, j) == currentBkColour)
				SetPixelV(hBitmapDC, i, j, (COLORREF) sysBkColour);
		}
	}
	DeleteDC(hBitmapDC);
	return TRUE;
}

// Function to create a custom header from a bitmap resource
BOOL CCustomList::CreateCustomHeader(UINT32 bitmapID)
{
	//The listview control has a built-in header, but we will create our own as we want it to display bitmaps
	//First get control's coordinates, so that we know where to place the header

	CRect listviewRect;
	GetWindowRect(&listviewRect);
	POINT listviewOrigin = { listviewRect.left, listviewRect.top };
	::ScreenToClient((HWND) GetOwner()->m_hWnd, &listviewOrigin);

	if(!m_hHeaderBitmap.LoadBitmap(MAKEINTRESOURCE(bitmapID)))
		ERROR2RAW("Failed to load header bitmap");

	//Get the height of the bitmap so we can figure out the height of the header
	BITMAP bitmap ;
	if (!m_hHeaderBitmap.GetBitmap(&bitmap))
	{
		ERROR2RAW("Failed to get header bitmap data");
		return(FALSE);
	}
	//Change the background colour of the bitmap to the one used by dialog backgrounds, in case the colour scheme used is not the default 
	SetBitmapBkgToSystem(m_hHeaderBitmap);
	
	CRect srect;
	srect.left = listviewOrigin.x + m_ColumnOffsetsArray[0] + 1;
	srect.right = listviewRect.right  ;
	srect.top	= listviewOrigin.y - bitmap.bmHeight;
	srect.bottom = listviewOrigin.y;

	m_hHeader.Create(NULL, WS_VISIBLE | SS_BITMAP, srect, this->GetOwner());
	m_hHeader.SetBitmap(m_hHeaderBitmap);
	return TRUE;
}


// called when the custom control receives the focus. ensure we are sensibly
// initialised before actually doing anything
void CCustomList::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	
	if(GetScrollableArea()->GetSafeHwnd()!=NULL)
	{
		if(GetScrollableArea()->m_RowCount > 0 && GetScrollableArea()->m_ListRowsArray)
		{
			INT32 Sel = GetScrollableArea()->m_CurrentSelectedRow;
			if(Sel==-1 || GetScrollableArea()->m_CurrentSelectedRow > GetScrollableArea()->m_RowCount)
				Sel=0;
			GetScrollableArea()->SelectRow(Sel);
		}
	}
	
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//					CCustomListScrollableArea                              //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCustomListScrollableArea, CWnd)
	//{{AFX_MSG_MAP(CCustomListScrollableArea)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CCustomListScrollableArea constructor
CCustomListScrollableArea::CCustomListScrollableArea(CCustomList* parent) :
	m_CurrentSelectedRow(-1),
	m_RowCount(0),
	m_Parent(parent),
	m_ListRowsArray(NULL),
	m_ScrollPos(0)
{
	// create font for text items
	LOGFONT lf;                        // Used to create the CFont.
	memset(&lf, 0, sizeof(LOGFONT));   // Clear out structure.
	lf.lfHeight = CCustomList::FONTHEIGHT;
	strcpy(lf.lfFaceName, "Microsoft Sans Serif");    //    with face name "Arial".
	m_Font.CreateFontIndirect(&lf);    // Create the font.

	// create the row objects
	m_ListRowsArray = new CCustomListRowWnd*[CCustomList::MAXROWS];
	for(INT32 i=0; i<CCustomList::MAXROWS; i++)
	{
		m_ListRowsArray[i] = NULL;
	}

	//	set up window class for the rows
	WNDCLASS NewWindowClass;
	memset(&NewWindowClass, 0, sizeof(WNDCLASS));
	NewWindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS ;
	NewWindowClass.lpfnWndProc = ::DefWindowProc;
	NewWindowClass.hInstance = AfxGetInstanceHandle();
	NewWindowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); ;//(HBRUSH) ((COLOR_BTNFACE));
	NewWindowClass.lpszMenuName = NULL;
	NewWindowClass.lpszClassName = _T("RowClass");
	AfxRegisterClass(&NewWindowClass);
}

// CCustomListScrollableArea destructor
CCustomListScrollableArea::~CCustomListScrollableArea()
{
	delete m_ListRowsArray;
	m_ListRowsArray = NULL;
}

// Called after the window has been destroyed.
void CCustomListScrollableArea::PostNcDestroy() 
{
	delete this;
	CWnd::PostNcDestroy();
}


// respond to a scrollbar event by scrolling this window up or down
void CCustomListScrollableArea::HandleScrollMessage(UINT32 nSBCode, UINT32 nPos )
{
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_TRACKPOS|SIF_PAGE|SIF_RANGE;
	m_Parent->GetVScrollBar()->GetScrollInfo(&si);

	INT32 NewPos = m_ScrollPos ;
	switch(nSBCode)
	{
	case SB_PAGEDOWN:
		NewPos = m_ScrollPos + si.nPage;
		break ;
	case SB_PAGEUP:
		NewPos = m_ScrollPos - si.nPage;
		break ;
	case SB_LINEDOWN:
		NewPos = m_ScrollPos + CCustomList::ROWHEIGHT;
		break ;
	case SB_LINEUP:
		NewPos = m_ScrollPos - CCustomList::ROWHEIGHT;
		break ;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		NewPos = nPos;
		break;
	}
	INT32 LastPos = si.nMax-si.nPage + 1 ; 

	if( NewPos < 0 )
		NewPos = 0;
	if( NewPos > LastPos )
		NewPos = LastPos ;

	INT32 offset = m_ScrollPos - NewPos;
	this->ScrollWindow(0,offset);
	m_ScrollPos = NewPos;
	m_Parent->GetVScrollBar()->SetScrollPos(m_ScrollPos , true);
}

// return a pointer to the specified row object
CCustomListRowWnd* CCustomListScrollableArea::GetRow(INT32 row)
{
	ASSERT(row < m_RowCount);
	return m_ListRowsArray[row] ;
}


// select the specified row, scrolling if necessary so that 
// the selection is in view. a value of -1 deselects any 
// selection

void CCustomListScrollableArea::SelectRow(INT32 RowNum)
{
	if( m_CurrentSelectedRow > -1 )
	{
		m_ListRowsArray[m_CurrentSelectedRow]->m_Selected = false ;
		m_ListRowsArray[m_CurrentSelectedRow]->RedrawWindow();
	}
	m_CurrentSelectedRow = RowNum;

	if( m_CurrentSelectedRow > -1 && this->IsWindowEnabled())
	{
		// scroll if necessary so that the selection is in view
		CRect parentRect;
		m_Parent->GetClientRect(&parentRect);
		INT32 Page = parentRect.Height();
		if((m_CurrentSelectedRow) * CCustomList::ROWHEIGHT < m_ScrollPos)
		{
			HandleScrollMessage(SB_THUMBPOSITION,m_CurrentSelectedRow * CCustomList::ROWHEIGHT);
		}
		else if((m_CurrentSelectedRow+1) * CCustomList::ROWHEIGHT > (m_ScrollPos + Page))
		{
			HandleScrollMessage(SB_THUMBPOSITION,(m_CurrentSelectedRow+1) * CCustomList::ROWHEIGHT - Page);
		}

		ASSERT(m_ListRowsArray!=NULL);
		m_ListRowsArray[m_CurrentSelectedRow]->m_Selected = true ;
		m_ListRowsArray[m_CurrentSelectedRow]->RedrawWindow();
		m_ListRowsArray[m_CurrentSelectedRow]->SetFocus();
	}
}

// the size has changed so adjust the scrollbar to reflect the new scrollable area size
void CCustomListScrollableArea::OnSize(UINT32 nType, INT32 cx, INT32 cy) 
{
	ASSERT( GetHeight() == cy );
	CWnd::OnSize(nType, cx, cy);
	
	CRect parentRect;
	m_Parent->GetClientRect(parentRect);

	// scrollbar window may not actually be visible
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE|SIF_RANGE;
	si.nPage = (INT32)parentRect.Height();
	si.nMin = 0;     
	si.nMax = GetHeight(); 
	m_Parent->m_VScrollBar->SetScrollInfo(&si, true);
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//							CCustomListRowWnd                              //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCustomListRowWnd, CWnd)
	//{{AFX_MSG_MAP(CCustomListRowWnd)
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CCustomListRowWnd::CCustomListRowWnd(INT32 RowNo, CCustomListScrollableArea* parent) :
	m_BackBrush(RGB(255,255,255)) ,
	m_BackBrushSel(::GetSysColor (COLOR_HIGHLIGHT)),
	m_Selected(false),
	m_RowNum(RowNo),
	m_Parent(parent)
{

	// column object storage
	m_ColumnObjects = new CObject* [CCustomList::MAXCOLUMNS];
	for( INT32 i=0; i < CCustomList::MAXCOLUMNS; i++)
		m_ColumnObjects[i] = NULL;
}

CCustomListRowWnd::~CCustomListRowWnd()
{
	// clear down column object storage
	for( INT32 i=0; i < CCustomList::MAXCOLUMNS; i++)
	{
		if ( m_ColumnObjects[i] !=NULL )
		{
			delete m_ColumnObjects[i];
			m_ColumnObjects[i] = NULL;
		}
	}
	delete m_ColumnObjects ;
}

//Called after the window has been destroyed.
void CCustomListRowWnd::PostNcDestroy() 
{
	delete this;
	CWnd::PostNcDestroy();
}

// Ensure that any bitmap handles are deleted. We do it here because we need
// the controls that store them to still be "alive"
void CCustomListRowWnd::OnDestroy() 
{
	CWnd::OnDestroy();
	for( INT32 i=0; i < CCustomList::MAXCOLUMNS; i++)
	{
		if ( m_ColumnObjects[i] !=NULL )
		{
			if( m_ColumnObjects[i]->IsKindOf(RUNTIME_CLASS(CStatic)) )
			{
				if( ((CStatic*)m_ColumnObjects[i])->GetSafeHwnd() )
				{
					HBITMAP hBitmap = ((CStatic*)m_ColumnObjects[i])->GetBitmap();
					if(hBitmap)
					{
						::DeleteObject(hBitmap);
					}
				}
			}
		}
	}
}

// add a check box at the specified column
void CCustomListRowWnd::AddCheck(INT32 col)
{
	CButton* pBut = new CButton();
	m_ColumnObjects[col] = pBut ;

	CRect cr;
	GetClientRect(&cr);
	ASSERT(cr.Height() == CCustomList::ROWHEIGHT);

	CRect rect;
	rect.left	=	m_Parent->m_Parent->m_ColumnOffsetsArray[col] ;
	rect.right	=	rect.left + GetSystemMetrics(SM_CXMENUCHECK);
	rect.top	= (CCustomList::ROWHEIGHT - GetSystemMetrics(SM_CYMENUCHECK))/2;
	rect.bottom = rect.top + GetSystemMetrics(SM_CYMENUCHECK)  ;
	pBut->Create(NULL, WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX, rect, this, 4);
}

// set the checkbox at the specified  column
void CCustomListRowWnd::SetChecked(INT32 col, BOOL checked)
{
	ASSERT(m_ColumnObjects[col] != NULL );
	ASSERT(m_ColumnObjects[col]->IsKindOf(RUNTIME_CLASS( CButton)));

	CButton* pBut = (CButton*)m_ColumnObjects[col];
	pBut->SetCheck(checked ? BST_CHECKED : BST_UNCHECKED );
	pBut->UpdateWindow();
}

// test the switch value at the specified  column
bool CCustomListRowWnd::IsChecked(INT32 col) const
{
	CButton* pBut = (CButton*)m_ColumnObjects[col];
	return pBut->GetCheck() == BST_CHECKED;
}


// we have been clicked - but let the parent class manage it
void CCustomListRowWnd::OnLButtonUp(UINT32 nFlags, CPoint point) 
{
	CWnd::OnLButtonUp(nFlags, point);
	m_Parent->SelectRow(m_RowNum);
	CWnd::OnLButtonUp(nFlags, point);
}

// called when object background needs erasing (for example, when resized). 
BOOL CCustomListRowWnd::OnEraseBkgnd(CDC* pDC) 
{
	CBrush* pOldBrush = pDC->SelectObject(m_Selected ? &m_BackBrushSel : &m_BackBrush);

	CRect rect;
	pDC->GetClipBox(&rect);     // Erase the area needed

	pDC->PatBlt(rect.left,rect.top,rect.Width(),rect.Height(),PATCOPY);
	pDC->SelectObject(pOldBrush);

	return true;
}

// add text to the specified column of this row
void CCustomListRowWnd::AddText(INT32 col, CString text)
{
	ASSERT(m_ColumnObjects[col] == NULL);

	CStatic* pStat = new CStatic();
	m_ColumnObjects[col] = pStat ;
	
	CRect cr;
	GetClientRect(&cr);

	CRect srect;
	srect.left= m_Parent->m_Parent->m_ColumnOffsetsArray[col] ;
	if( col < CCustomList::MAXCOLUMNS && m_Parent->m_Parent->m_ColumnOffsetsArray[col + 1] > 0)
		srect.right= m_Parent->m_Parent->m_ColumnOffsetsArray[col + 1] ;
	else
		srect.right= cr.right;
	srect.top	= (CCustomList::ROWHEIGHT - CCustomList::FONTHEIGHT)/2;
	srect.bottom = srect.top + CCustomList::FONTHEIGHT;
	pStat->Create(text, WS_CHILD|WS_VISIBLE|WS_EX_LEFT, srect, this);
	pStat->SetFont(&m_Parent->m_Font);
}

// set the text at the specified column of this row
void CCustomListRowWnd::SetText(INT32 col, CString text)
{
	CStatic* pStat = (CStatic*)m_ColumnObjects[col];
	if( pStat == NULL )
		AddText(col,text);
	else
	{
		pStat->SetWindowText(text);
		pStat->UpdateWindow();
	}
}

// add bitmap at the specified  column
void CCustomListRowWnd::AddBitmap(INT32 col, HBITMAP hBitmap,HBITMAP, DWORD dwBackColour)
{
	CStatic* pStat = new CStatic();
	ASSERT(m_ColumnObjects[col] == NULL);
	m_ColumnObjects[col] = pStat ;

	CRect cr;
	GetClientRect(&cr);

	BITMAP bitmap;
	CBitmap::FromHandle(hBitmap)->GetBitmap(&bitmap);
	CRect srect;
	srect.left = m_Parent->m_Parent->m_ColumnOffsetsArray[col] ;
	srect.right = srect.left + bitmap.bmWidth;
	srect.top	= cr.Height()/2 - bitmap.bmHeight/2;
	srect.bottom = cr.Height()/2 + bitmap.bmHeight/2;

	////
	HDC hBitmapDC = CreateCompatibleDC(NULL);
	if (!hBitmapDC)
	{
		ERROR2RAW("Non-fatal GDI error");
	}
	SelectObject(hBitmapDC, hBitmap);
	// Iff we haven't been told what the background colour is...
	// We make the assumption that the pixel in the lower right corner has the background colour
	if (dwBackColour == 0xFFFFFFFF)
		dwBackColour = (DWORD) GetPixel(hBitmapDC, bitmap.bmWidth - 1, bitmap.bmHeight -1); 
	DWORD sysBkColour = GetSysColor(COLOR_3DFACE);
	for (INT32 i = 0; i < bitmap.bmWidth; i++)
	{
		for (INT32 j = 0; j < bitmap.bmHeight; j++)
		{
			if ((DWORD) GetPixel(hBitmapDC, i, j) == dwBackColour)
				SetPixelV(hBitmapDC, i, j, (COLORREF) sysBkColour);
		}
	}
	DeleteDC(hBitmapDC);

	////
	pStat->Create(NULL, WS_VISIBLE | SS_BITMAP, srect, this);
	pStat->SetBitmap(hBitmap);
}

// invoked when child control is about to be drawn. prepare the pDC for drawing the 
// control using the correct system colors.
HBRUSH CCustomListRowWnd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT32 nCtlColor) 
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
	COLORREF text = m_Selected ? GetSysColor(COLOR_HIGHLIGHTTEXT) : GetSysColor(COLOR_WINDOWTEXT);
	pDC->SetTextColor(text);
	pDC->SetBkMode(TRANSPARENT);

	return GetBackgroundBrush();
}

// The row has been double-clicked. Notify the control calling window
// (usually a dialog) 
void CCustomListRowWnd::OnLButtonDblClk(UINT32 nFlags, CPoint point) 
{
	m_Parent->SelectRow(m_RowNum); // make sure we are selected
	CWnd* caller = m_Parent->m_Parent->GetOwner();
	INT32 ControlID = m_Parent->m_Parent->GetDlgCtrlID();
	if(caller && ControlID )
	{
		NMHDR nm;
		nm.hwndFrom = m_Parent->m_Parent->m_hWnd;
		nm.idFrom = ControlID;
		nm.code = NM_DBLCLK;

		::SendMessage(caller->m_hWnd ,WM_NOTIFY,ControlID , (LPARAM)&nm	);
	}
}

// return the text at the specified column
CString CCustomListRowWnd::GetText(INT32 col) const
{
	CStatic* pStat = (CStatic*)m_ColumnObjects[col];
	CString ret ;
	pStat->GetWindowText(ret);
	return ret;
}

// if up or down has been clicked, pass it on to the scrolling code
void CCustomListRowWnd::OnKeyDown(UINT32 nChar, UINT32 nRepCnt, UINT32 nFlags) 
{
	if(nChar == CAMKEY(DOWN) && m_RowNum + 1 < m_Parent->m_RowCount)
		m_Parent->SelectRow(m_RowNum+1);
	else if(nChar == CAMKEY(UP) && m_RowNum - 1 >= 0)
		m_Parent->SelectRow(m_RowNum-1);

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

//	Make sure we receive UP/DOWN arrow key message (otherwise dialogs swallow them)
UINT32 CCustomListRowWnd::OnGetDlgCode() 
{
    UINT32 uRet = CWnd::OnGetDlgCode();
    uRet |= DLGC_WANTARROWS;
    return uRet;
}

// The framework calls this member function when a child control sends a notification message, 
// or when an accelerator keystroke is translated. 
BOOL CCustomListRowWnd::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	UINT32 nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	INT32 nCode = HIWORD(wParam);
	///
	m_Parent->SelectRow(m_RowNum); // make sure we are selected
	CWnd* caller = m_Parent->m_Parent->GetOwner();
	INT32 ControlID = m_Parent->m_Parent->GetDlgCtrlID();
	if(caller && ControlID )
	{
		NMHDR nm;
		nm.hwndFrom = m_hWnd;
		nm.idFrom = ControlID;
		nm.code = NULL;

		::SendMessage(caller->m_hWnd ,WM_NOTIFY, ControlID , (LPARAM)&nm);
	}
	return CWnd::OnCommand(wParam, lParam);
}

// adds a row window object to the scrollable area. Note the resultant
// height change triggers a WM_SIZE message (handled by OnSize()).
// Returns a pointer to the row window
CCustomListRowWnd* CCustomListScrollableArea::AddRow()
{
	CCustomListRowWnd* pRow = new CCustomListRowWnd(m_RowCount,this);
	ASSERT(pRow);
	m_ListRowsArray[m_RowCount] = pRow;
	m_RowCount++;

	CRect parentRect;
	m_Parent->GetClientRect(&parentRect);

	CRect rect ;
	rect.top = (m_RowCount-1) * CCustomList::ROWHEIGHT ;
	rect.left = 0 ;
	rect.bottom = 0 + (m_RowCount) * CCustomList::ROWHEIGHT ;
	rect.right = parentRect.right - 1;

	pRow->Create("RowClass","",WS_CHILD,rect,this,0);
	pRow->ShowWindow(SW_SHOW);

	// recalculate height and scrollbar position
	BOOL needScroll = GetHeight() > parentRect.Height();
	INT32 width = parentRect.right;
	if(needScroll)
		width=width - ::GetSystemMetrics(SM_CXVSCROLL); //+2
	m_Parent->m_VScrollBar->ShowWindow(needScroll ? SW_SHOW : SW_HIDE);
	SetWindowPos(NULL,0,0,width,GetHeight(),0);

	return pRow;
}



// respond to mousewheel events by paging or scrolling accordingly
BOOL CCustomList::OnMouseWheel(UINT32 nFlags, short zDelta, CPoint pt) 
{
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_TRACKPOS|SIF_PAGE|SIF_RANGE;
	GetVScrollBar()->GetScrollInfo(&si);

	INT32 nStep = GetKeyState(CAMKEY(CONTROL))<0 ? si.nPage : ROWHEIGHT ;

	INT32 newPos = GetVScrollBar()->GetScrollPos() - nStep*zDelta/WHEEL_DELTA;

	// force to zero if the first line would appear partially off screen
	if(newPos < ROWHEIGHT)
		newPos = 0;
	// keep the upper limit within bounds
	if(newPos > si.nMax)
		newPos = si.nMax;

	if (m_ScrollableArea)
	{
		m_ScrollableArea->HandleScrollMessage(SB_THUMBPOSITION, newPos);
	}
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}
	