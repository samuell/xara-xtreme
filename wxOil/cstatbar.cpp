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
// CCStatusBar


#include "camtypes.h"
#include <afxpriv.h>
#include "errors.h"
#include "fonts.h"
#include "csrstack.h"
#include "cstatbar.h"
#include "ctrlhelp.h"
#include "statline.h"
#include "app.h"
#include <afxcmn.h>

#define IS_CHICAGO ((LOBYTE(LOWORD(GetVersion()))) > 3) 

DECLARE_SOURCE("$Revision$");

IMPLEMENT_DYNAMIC(CCStatusBar, CStatusBar)

#define new CAM_DEBUG_NEW

BEGIN_MESSAGE_MAP(CCStatusBar, CStatusBar)
	//{{AFX_MSG_MAP(CCStatusBar)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE_VOID(WM_INITIALUPDATE, OnInitialUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// #define STATUSBAR_POPOUT(X) TRUE
// #define STATUSBAR_POPOUT(X) (X & SBPS_POPOUT)
// SBPS_POPOUT = SBT_POPOUT = 0x0200

/******************************************************************************
>	CCStatusBar::CCStatusBar()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/94
	Purpose:	CCStatusBar constructor
******************************************************************************/

CCStatusBar::CCStatusBar() : CStatusBar()
{
	ERROR3IF(this == NULL,"CStatusBar::CStatusBar() - this == NULL");

	OldMousePos = CPoint(0,0);
	NumPanes = 0;
	pPaneData = NULL;
	// >>Webster (Adrian 2/2/97)
	// We change the status bar font to Arial Narrow 9
	// Note that this is a system-wide setting, so we change it "quietly" (i.e. don't broadcast a WM_WININICHANGE)
	// so as not to affect other applications already running
	//NONCLIENTMETRICS ncMetrics;
	//ncMetrics.cbSize = sizeof(NONCLIENTMETRICS);
	//::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncMetrics, 0);
	//m_OldFont.CreateFontIndirect(&ncMetrics.lfStatusFont);
	//CFont newStatusFont;
	//newStatusFont.CreatePointFont(90, _T("Arial Narrow")); 
	//newStatusFont.GetLogFont(&ncMetrics.lfStatusFont);
	//::SystemParametersInfo(SPI_SETNONCLIENTMETRICS, 0, &ncMetrics, 0);
	// <<Webster
}


/******************************************************************************
>	CCStatusBar::~CCStatusBar()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/95
	Purpose:	CCStatusBar destructor
	Errors:		this==NULL
				NumPanes==0
				pPaneData==NULL
	******************************************************************************/
CCStatusBar::~CCStatusBar()
{
	ERROR3IF(     this==NULL,"CStatusBar::~CStatusBar() - this==NULL");
	ERROR3IF( NumPanes==0,   "CStatusBar::~CStatusBar() - NumPanes==0");
	ERROR3IF(pPaneData==NULL,"CStatusBar::~CStatusBar() - pPaneData==NULL");

	delete pPaneData;

}


/******************************************************************************
>	BOOL CCStatusBar::Create(CFrameWnd* pParentWnd,
							DWORD dwStyle=WS_CHILD|WS_VISIBLE|CBRS_BOTTOM,
							UINT32 nID=_R(AFX_IDW_STATUS_BAR));

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/94
	Purpose:	Just a copy of CStatusBar Create() but allow double clicks
				and use our own font
	Inputs:		pParentWnd - pointer to parent window
				dwStyle    - stlye flags
				nID        - window ID
	Returns:	FALSE if fails (see Errors)
	Errors:		this==NULL,
				pParentWnd==NULL,
				CWnd::Create() failed
******************************************************************************/
/*	UIC Function body changed to below - for win '95 Common Control

BOOL CCStatusBar::Create(CFrameWnd* pParentWnd, DWORD dwStyle, UINT32 nID)
{
	ERROR2IF(      this == NULL,FALSE,"CCStatusBar::Create() - this == NULL");
	ERROR2IF(pParentWnd == NULL,FALSE,"CCStatusBar::Create() - pParentWnd == NULL");

	m_dwStyle = (UINT32)dwStyle;	// save the style
	CString WndClassName = AfxRegisterWndClass(CS_DBLCLKS,					// class style bits
											 Cursor::Arrow->Handle(),		// cursor handle
											 (HBRUSH)(COLOR_BTNFACE+1));	// window background
	// create the HWND
	CRect rect;
	rect.SetRectEmpty();
	BOOL CreatedOK = CWnd::Create(WndClassName, NULL, dwStyle, rect, pParentWnd, nID);
	ERROR2IF(!CreatedOK,FALSE,"CCStatusBar::Create() - CWnd::Create() failed");

	// set initial font and calculate bar height
	SendMessage(WM_SETFONT, (WPARAM)FontFactory::GetFont(STOCKFONT_STATUSBAR));
	return TRUE;
}
*/
BOOL CCStatusBar::Create(CFrameWnd* pParentWnd, DWORD dwStyle, UINT32 nID)
{
	BOOL CreatedOK = CStatusBar::Create((CWnd*)pParentWnd,dwStyle,nID);
	ERROR2IF(!CreatedOK,FALSE,"CCStatusBar::Create() - CWnd::Create() failed");

	// set initial font and calculate bar height
	SendMessage(WM_SETFONT, (WPARAM)FontFactory::GetFont(STOCKFONT_STATUSBAR));

	INT32 Bits = GetClassLong(m_hWnd,GCL_STYLE);
	Bits |= CS_DBLCLKS;
	SetClassLong(m_hWnd,GCL_STYLE,Bits); 

	// >>Webster (Adrian 2/2/97)
	// We change the system -wide status bar font back to what it used to be before starting Webster
	//NONCLIENTMETRICS ncMetrics;
	//ncMetrics.cbSize = sizeof(NONCLIENTMETRICS);
	//::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncMetrics, 0);
	//m_OldFont.GetLogFont(&ncMetrics.lfStatusFont);
	//::SystemParametersInfo(SPI_SETNONCLIENTMETRICS, 0, &ncMetrics, 0);
	// <<Webster

	
	return TRUE;
}
/******************************************************************************
>	BOOL CCStatusBar::UpdateStatusLineFont()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/94
	Purpose:	
	Returns:	FALSE if fails
	Errors:		
******************************************************************************/
// UIC function for Win '95 changes

BOOL CCStatusBar::UpdateStatusLineFont()
{
	SendMessage(WM_SETFONT, (WPARAM)FontFactory::GetFont(STOCKFONT_STATUSBAR));

	return TRUE;
}

/******************************************************************************
>	BOOL CCStatusBar::SetIndicators(StatusLinePaneData* pSLPD, INT32 Panes)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/94
	Purpose:	Interface to CStatusBar::SetIndicators
				ALSO enables/disables panes according to data passed in
	Inputs:		pSLPD - pointer to array of StatusLinePaneData
				Panes - number of panes
	Returns:	FALSE if fails
	Errors:		this==NULL
				PAneState() fails
******************************************************************************/
BOOL CCStatusBar::SetIndicators(StatusBarPaneData* pSLPD, INT32 Panes)
{
	ERROR2IF(this == NULL,FALSE,"CStatusBar::SetIndicators() - this == NULL");
	ERROR2IF(NumPanes != 0,   FALSE,"CStatusBar::SetIndicators() - NumPanes != 0");
	ERROR2IF(pPaneData != NULL,FALSE,"CStatusBar::SetIndicators() - pPaneData != NULL");

	// copy pane data into class
	NumPanes = Panes;
	pPaneData = new StatusBarPaneData[NumPanes];	
	ERROR2IF(pPaneData==NULL,FALSE,"CCStatusBar::SetIndicators() - pPaneData==NULL");
	for (UINT32 i = 0; i < NumPanes; i++)
		pPaneData[i] = pSLPD[i];

	// call equivalent routine in base class
	UINT32* pPaneIDs = new UINT32[NumPanes];
	ERROR2IF(pPaneIDs == NULL,FALSE,"CCStatusBar::SetIndicators() - pPaneIDs == NULL");
	for (i = 0; i < NumPanes; i++)
		pPaneIDs[i] = pPaneData[i].PaneID;
	CStatusBar::SetIndicators(pPaneIDs,NumPanes);
	delete pPaneIDs;

	// set initial pane states & bitmap pane widths
	BOOL ReturnValue = TRUE;
	for (i = 0; i < NumPanes; i++)
	{
		if (PaneState(pPaneData[i].PaneID, pPaneData[i].InitState) == Fail)
			ReturnValue = FALSE;

		if(pPaneData[i].OwnerDrawn)
		{
		  	UINT32 PaneStyle = GetPaneStyle(i);
			SetPaneStyle(i,PaneStyle|SBPS_OWNERDRAW);
   		}

		if (pPaneData[i].BitmapID)
		{
			CBitmap bitmap;
			BITMAP  BMObject;
			if (bitmap.LoadBitmap(pPaneData[i].BitmapID) == 0)
				ReturnValue = FALSE;

			if (bitmap.GetObject(sizeof(BITMAP), &BMObject) == 0)
				ReturnValue = FALSE;

			// Small bodge here to allow Print Colours pane to contain both bitmap and text
			// Don't force pane width if paneID is that of the Print Colours pane
			if (pPaneData[i].PaneID!=_R(IDS_SL_PRINTMODE))
			{
				if (SetPaneWidth(BMObject.bmWidth-2, pPaneData[i].PaneID) == FALSE) // NB bodge offset '-2' !!!!
					ReturnValue = FALSE;
			}
		}
	}

	return ReturnValue;
}


/******************************************************************************
>	BOOL CCStatusBar::UnSetIndicators()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/01/2004
	Purpose:	Interface to CStatusBar::SetIndicators
				ALSO enables/disables panes according to data passed in
	Inputs:		pSLPD - pointer to array of StatusLinePaneData
				Panes - number of panes
	Returns:	FALSE if fails
	Errors:		this==NULL
******************************************************************************/
BOOL CCStatusBar::UnSetIndicators()
{
	ERROR2IF(this == NULL,FALSE,"CStatusBar::SetIndicators() - this == NULL");
//	ERROR2IF(NumPanes != 0,   FALSE,"CStatusBar::SetIndicators() - NumPanes != 0");
//	ERROR2IF(pPaneData != NULL,FALSE,"CStatusBar::SetIndicators() - pPaneData != NULL");

	if (NumPanes!=0 && pPaneData!=NULL)
	{
		delete[] pPaneData;
	}

	NumPanes=0;
	pPaneData = NULL;

	return TRUE;
}


/******************************************************************************
>	void CCStatusBar::OnLButtonDblClk(UINT32 nFlags, CPoint point)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/94
	Inputs:		nFlags - 
				point  - mouse position
	Purpose:	Handle mouse left button double clicks over the status bar
	Errors:		this==NULL
				GetpStatusLine() fails
******************************************************************************/

void CCStatusBar::OnLButtonDblClk(UINT32 nFlags, CPoint point)
{
	if (this!=NULL)
	{
		StatusLine* pStatusLine=GetApplication()->GetpStatusLine();
		if (pStatusLine)
		{
			INT32 PaneIndex=GetPaneIndexFromPoint(point);
			INT32 PaneID=GetItemID(PaneIndex);
			pStatusLine->PaneDoubleClick(PaneID);
		}
	}
	else
		ERROR3("CCStatusBar::OnLButtonDblClk() - this==NULL");
}

/******************************************************************************
>	BOOL CCStatusBar::UpdateStatusLineFont()PaneText(INT32 PaneID, StringBase* ptext, BOOL ImmediateUpdate=FALSE);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/12/94
	Purpose:	Update specified status bar pane
	Inputs:		PaneID - id of pane
				ptext  - text (or NULL)
				ImmediateUpdate - redraw pane now (rather than queuing)
	Returns:	FALSE if failed (see Errors)
	Errors:		this==NULL,
				PaneID unrecognised
******************************************************************************/

BOOL CCStatusBar::UpdatePaneText(INT32 PaneID, StringBase* ptext, BOOL ImmediateUpdate)
{
	ERROR2IF(this==NULL,FALSE,"CStatusBar::UpdatePaneText() - this==NULL");
   	
	INT32 PaneIndex=CommandToIndex(PaneID);
	ERROR2IF(PaneIndex==-1,FALSE,"CStatusBar::UpdatePaneText() - unknown PaneID");
	
	SetPaneText(PaneIndex, ptext==NULL ? NULL : (TCHAR*)*ptext);
	if (ImmediateUpdate && ptext) UpdateWindow();

	return TRUE;
}

/******************************************************************************
>	void CCStatusBar::OnInitialUpdate()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/94
	Purpose:	Just a copy of CStatusBar version but doesn't disable panes!
******************************************************************************/

void CCStatusBar::OnInitialUpdate()
{
	OnIdleUpdateCmdUI(FALSE, 0L);
}


/******************************************************************************
>	BOOL CCStatusBar::GetPaneRect(CRect* pRect, INT32 PaneID);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Purpose:	get dimensions of status bar pane 
	Inputs:		PaneID - id of pane
	Outputs:	pRect  - pane rect
	Returns:	FALSE if failed (see Errors)
	Errors:		this==NULL,
				PaneID unrecognised
******************************************************************************/
BOOL CCStatusBar::GetPaneRect(CRect* pRect, INT32 PaneID)
{
	ERROR2IF(this==NULL,FALSE,"CStatusBar::UpdatePaneText() - this==NULL");

	INT32 PaneIndex=CommandToIndex(PaneID);
	ERROR2IF(PaneIndex==-1,FALSE,"CStatusBar::UpdatePaneText() - unknown PaneID");

	GetItemRect(PaneIndex,pRect);
	return TRUE;
}

/******************************************************************************
>	INT32 CCStatusBar::GetTextWidth(StringBase* ptext, BOOL fBold = FALSE);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Purpose:	Get the width of the given text as if it were on the status bar
	Inputs:		ptext  - text
				fBold  - if TRUE, assume bold text (by default FALSE)
	Returns:	width of text, or -1 if failed (see Errors)
	Errors:		this==NULL,
******************************************************************************/
INT32 CCStatusBar::GetTextWidth(StringBase* ptext, BOOL fBold)
{
	ERROR2IF(this==NULL,-1,"CStatusBar::GetTextWidth() - this==NULL");
	
	StockFont sf = (fBold ? STOCKFONT_STATUSBARBOLD : STOCKFONT_STATUSBAR);

	CClientDC dcScreen(NULL);
	HGDIOBJ OldObject = dcScreen.SelectObject(FontFactory::GetFont(sf));
	INT32 result = dcScreen.GetTextExtent(*ptext, ptext->Length()).cx;
	dcScreen.SelectObject(OldObject);	

	return result;
}

/******************************************************************************
>	BOOL CCStatusBar::SetPaneWidth(INT32 width, INT32 PaneID);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Purpose:	Set the width of a status bar pane
	Inputs:		PaneID - id of pane
				width  - new pane width
	Returns:	FALSE if failed (see Errors)
	Errors:		this==NULL,
				PaneID unrecognised (or 0 as it is variable width!),
				width<0
******************************************************************************/

BOOL CCStatusBar::SetPaneWidth(INT32 width, INT32 PaneID)
{
	ERROR2IF(this==NULL,FALSE,"CStatusBar::SetPaneWidth() - this==NULL");
	ERROR2IF(width<0 ,FALSE,"CStatusBar::SetPaneWidth() - width<0");

	INT32 PaneIndex=CommandToIndex(PaneID);
	ERROR2IF(PaneIndex == -1,FALSE,"CStatusBar::SetPaneWidth() - unknown PaneID");
	ERROR2IF(PaneIndex == 0 ,FALSE,"CStatusBar::SetPaneWidth() - Pane with ID==0 is variable width!!!");

	UINT32 nStyle;
	UINT32 OldPaneID;
	INT32  OldWidth;
	GetPaneInfo(PaneIndex, OldPaneID, nStyle, OldWidth);
	SetPaneInfo(PaneIndex, OldPaneID, nStyle, width);

	return TRUE;
}


/******************************************************************************
>	FlagState CCStatusBar::PaneState(INT32 PaneID, FlagState NewState=NoChange);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Purpose:	Read/write pane state
	Inputs:		PaneID - id of pane
				state  - Disable/Enable/Toggle/NoChange
	Returns:	previous state Disable/Enable/Fail (see Errors)
	Errors:		this==NULL,
				PaneID unrecognised
				NewState invalid
******************************************************************************/

FlagState CCStatusBar::PaneState(INT32 PaneID, FlagState NewState)
{
	ERROR2IF(this == NULL,Fail,"CStatusBar::PaneState() - this == NULL");

	INT32 PaneIndex = CommandToIndex(PaneID);
	ERROR2IF(PaneIndex == -1, Fail, "CStatusBar::PaneState() - unknown PaneID");

	UINT32 OldStyle;
	UINT32 NewStyle;
	UINT32 OldPaneID;
	INT32  width;
	GetPaneInfo(PaneIndex, OldPaneID, OldStyle, width);
	switch (NewState)
	{
		case Disable:  NewStyle = OldStyle | SBPS_DISABLED; break;
		case Enable:   NewStyle = OldStyle &~ SBPS_DISABLED; break;
		case Toggle:   NewStyle = OldStyle ^ SBPS_DISABLED; break;
		case NoChange: NewStyle = OldStyle; break;
		default: ERROR3("CCStatusBar::PaneState() - passed invalid NewState");
	}
	if (NewStyle != OldStyle)
		SetPaneInfo(PaneIndex, OldPaneID, NewStyle, width);

	return ((OldStyle & SBPS_DISABLED) ? Disable : Enable);
}


/******************************************************************************
>	BOOL CCStatusBar::PaneExists(INT32 PaneID);

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/01/2004
	Purpose:	Test wheter pane exists without ERRORing
	Inputs:		PaneID - id of pane
	Returns:	TRUE if specified pane exists
	Errors:		this==NULL,
******************************************************************************/

BOOL CCStatusBar::PaneExists(INT32 PaneID)
{
	ERROR2IF(this == NULL,Fail,"CStatusBar::PaneState() - this == NULL");

	INT32 PaneIndex = CommandToIndex(PaneID);
	return (PaneIndex != -1);
}


/******************************************************************************
>	void CCStatusBar::OnMouseMove(UINT32 nFlags, CPoint MousePos )

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/95
	Purpose:	handle mouse moves over the status bar
	Inputs:		nFlags   - 
				MousePos -
	Errors:		this==NULL,
******************************************************************************/

void CCStatusBar::OnMouseMove(UINT32 nFlags, CPoint MousePos)
{
	ERROR3IF(this==NULL,"CCStatBar::OnMouseMove() - this==NULL");

	if (MousePos==OldMousePos)
		return;

	INT32 PaneIndex=GetPaneIndexFromPoint(MousePos);
	if (PaneIndex==-1)
		ControlHelper::BubbleHelpDisable();
	else
		ControlHelper::DoBubbleHelpOn(GetSafeHwnd(),PaneIndex,BubbleHelpCallBack,this);

	StatusLine* pStatusLine=GetApplication()->GetpStatusLine();
	if (pStatusLine)
	{
		String_256 Text("");
		GetStatusLineText(&Text,MousePos,GetSafeHwnd());
		if (!pStatusLine->UpdateText(&Text,STATUSLINE_SELDESC_STATBAR))
			ERROR3("CCStatusBar::OnMouseMove() - pStatusLine->UpdateText() failed");
	}

	OldMousePos=MousePos;
}


/******************************************************************************
>	BOOL CCStatusBar::GetStatusLineText(String_256* pText, CPoint MousePos, HWND hWnd)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/95
	Purpose:	if over StatusLine, gets help for a given point
	Inputs:		MousePos - position in window
				hWnd     - handle of window
	Outputs:	pText    -
	Returns:	TRUE if ptext hold valid text, else FALSE if not over a pane
	Errors:		this==NULL,
				StringBase::Load() fails,
******************************************************************************/

BOOL CCStatusBar::GetStatusLineText(String_256* pText, CPoint MousePos, HWND hWnd)
{
	ERROR2IF(this==NULL,FALSE,"CCStatusBar::GetStatusLineText() - this==NULL");

	if (GetSafeHwnd()!=hWnd)
		return FALSE;

	INT32 PaneIndex=GetPaneIndexFromPoint(MousePos);
	if (PaneIndex==-1)
		return FALSE;

	return (pText->Load(pPaneData[PaneIndex].StatusHelpID)!=0);
}


/******************************************************************************
>	INT32 CCStatusBar::GetPaneIndexFromPoint(CPoint point)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/95
	Purpose:	find the Index of a status bar pane given a point
	Inputs:		point - 
	Returns:	Index of pane containing point, or -1 if none
	Errors:		this==NULL,
******************************************************************************/

INT32 CCStatusBar::GetPaneIndexFromPoint(CPoint point)
{
	ERROR2IF(this==NULL,-1,"CCStatBar::GetPaneIndexFromPoint() - this==NULL");

	for (UINT32 i=0; i<NumPanes; i++)
	{
		RECT PaneRect;
		GetItemRect(i,&PaneRect);
		if (CRect(PaneRect).PtInRect(point))
			return i;
	}
	return -1;
}


/*********************************************************************************************
	static TCHAR* CCStatusBar::BubbleHelpCallBack(HWND hWnd, UINT32 PaneIndex, void* UserData)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/95
	Purpose:	Bubble help call back handler
				(called some time after call to DoBubbleHelpOn() if mouse ha not moved)
	Inputs:		hWnd     - not used
				PaneID   - ID of pane bubble help requesteed for
				UserData - not used
	Returns:	pointer to text, or NULL if fails (see Errors)
	Errors:		PaneIndex out of range
				StringBase::Load() fails
**********************************************************************************************/
TCHAR* CCStatusBar::BubbleHelpCallBack(HWND, UINT32 PaneIndex, void*)
{
	static String_64 BubbleHelpText("");

	CCStatusBar* pCCStatusBar=GetApplication()->GetpCCStatusBar();
	ERROR3IF(pCCStatusBar==NULL,"CCStatusBar::BubbleHelpCallBack() - pCCStatusBar==NULL");
	if (pCCStatusBar && PaneIndex>=0 && PaneIndex<pCCStatusBar->NumPanes)
		BubbleHelpText.Load(pCCStatusBar->pPaneData[PaneIndex].BubbleHelpID);
	else
		ERROR3("CCStatusBar::BubbleHelpCallBack() - PaneIndex out of range");

	return (TCHAR*)BubbleHelpText;
}


//////////////////////////////////////////////////////////////////////////////
// the following are copied from MFC in order to get greater control over StatusBar redraw
/*  UIC Commented-out struct AFX_STATUSPANE
struct AFX_STATUSPANE
{
	UINT32    nID;        // IDC of indicator: 0 => normal text area
	UINT32    nStyle;     // style flags (SBPS_*)
	INT32     cxText;     // width of string area in pixels
						//   on both sides there is a 1 pixel gap and
						//    a one pixel border, making a pane 4 pixels wider
	LPCTSTR  lpszText;  // text in the pane
};
*/

struct AUX_DATA
{
	// system metrics
	INT32 cxVScroll, cyHScroll;
	INT32 cxIcon, cyIcon;

	INT32 cxBorder2, cyBorder2;

	// device metrics for screen
	INT32 cxPixelsPerInch, cyPixelsPerInch;
	INT32 cySysFont;

	// solid brushes with convenient gray colors and system colors
	HBRUSH hbrLtGray, hbrDkGray;
	HBRUSH hbrBtnHilite, hbrBtnFace, hbrBtnShadow;
	HBRUSH hbrWindowFrame;
	HPEN hpenBtnHilite, hpenBtnShadow, hpenBtnText;

	// color values of system colors used for CToolBar
	COLORREF clrBtnFace, clrBtnShadow, clrBtnHilite;
	COLORREF clrBtnText, clrWindowFrame;

	// standard cursors
	HCURSOR hcurWait;
	HCURSOR hcurArrow;
	HCURSOR hcurHelp;       // cursor used in Shift+F1 help

	// special GDI objects allocated on demand
	HFONT   hStatusFont;
	HFONT   hToolTipsFont;
	HBITMAP hbmMenuDot;

	// other system information
	UINT32    nWinVer;        // Major.Minor version numbers
	BOOL    bWin32s;        // TRUE if Win32s (or Chicago)
	BOOL    bWin4;          // TRUE if Windows 4.0
	BOOL    bNotWin4;       // TRUE if not Windows 4.0
	BOOL    bSmCaption;     // TRUE if WS_EX_SMCAPTION is supported
	BOOL    bWin31;         // TRUE if actually Win32s on Windows 3.1

	// special Windows API entry points
	INT32 (WINAPI* pfnSetScrollInfo)(HWND, INT32, LPCSCROLLINFO, BOOL);
	BOOL (WINAPI* pfnGetScrollInfo)(HWND, INT32, LPSCROLLINFO);

// Implementation
	AUX_DATA();
	~AUX_DATA();
	void UpdateSysColors();
};

extern AFX_DATA AUX_DATA afxData;

#define CX_BORDER   1
#define CY_BORDER   1

/******************************************************************************
>	void CCStatusBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/96
	Purpose:	The status line is incorrectly sized under Windows 95
				
******************************************************************************/
// UIC 

CSize CCStatusBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	CSize NewSize(0,0);
	NewSize = CStatusBar::CalcFixedLayout(0,bHorz);
// Removed, makes the status line too wide.
/*	if(IS_CHICAGO)
	{
		NewSize.cy +=4;
	}
*/
	return NewSize;
 }

/******************************************************************************
>	void CCStatusBar::DoPaint(CDC* pdc)

	Author:		-
	Created:	-
	Purpose:	Complete copy of MFC function so that it can call the CCStatusBar
				version of DrawStatusText (which is not virtual in CStatusBar!!!!)
******************************************************************************/
// UIC Replaced (DoPaint) with DrawItem(LPITEMSTRUCT lpDrawItemStruct) 

void CCStatusBar::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	INT32 itemID = lpDrawItemStruct->itemID;
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	
	ASSERT_VALID(pDC);
	ASSERT_VALID(this);

	CRect rect;
	GetClientRect(&rect);
	CStatusBarCtrl* pCStatusBarCtrl = &GetStatusBarCtrl();
	pCStatusBarCtrl->GetRect(itemID, &rect);
	CBrush TheBrush(GetSysColor(COLOR_BTNFACE)) ;
	pDC->FillRect(&rect,&TheBrush);

//	ASSERT_VALID(pDC);
//	ASSERT_VALID(this);

		// slight bodge here
		if(itemID==0)
		{
			CString Text = GetPaneText(itemID);
			DrawStatusText(pDC, lpDrawItemStruct->rcItem,(LPCTSTR)Text, GetPaneStyle(itemID));
		}
		else if ((pPaneData[itemID].PaneID)== _R(IDS_SL_MOUSEPOS))
		{
			CString Text = GetPaneText(itemID);
			DrawStatusText(pDC, lpDrawItemStruct->rcItem, (LPCTSTR)Text, GetPaneStyle(itemID));
		}
		else if ((pPaneData[itemID].PaneID)== _R(IDS_SL_PRINTMODE))
		{
			RECT rcPane = lpDrawItemStruct->rcItem;

			if (pPaneData[itemID].BitmapID)
			{
				// Paint bitmap to left end of pane
				rcPane.right = rcPane.left + 16;
				PaintBitmap(pDC, rcPane, pPaneData[itemID].BitmapID, GetPaneStyle(itemID));

				// Reset rectangle so text doesn't overlap bitmap
				rcPane = lpDrawItemStruct->rcItem;
				rcPane.left = rcPane.left + 16;
			}
			CString Text = GetPaneText(itemID);
			DrawStatusText(pDC, rcPane, (LPCTSTR)Text, GetPaneStyle(itemID));
		}
		else if (pPaneData[itemID].BitmapID)
		{
			if (GetPaneStyle(itemID) & SBPS_DISABLED)
			{ 
				PaintBitmap(pDC, lpDrawItemStruct->rcItem, pPaneData[itemID].OffBitmapID, GetPaneStyle(itemID));
			}
			else
			{  
				PaintBitmap(pDC, lpDrawItemStruct->rcItem, pPaneData[itemID].BitmapID, GetPaneStyle(itemID));
			}
		    
		}	
}
  
/*
 void CCStatusBar::DoPaint(CDC* pDC)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	CControlBar::DoPaint(pDC);      // draw border

	CRect rect;
	GetClientRect(rect);
	CalcInsideRect(rect, TRUE);

	ASSERT(m_hFont != NULL);        // must have a font!
	HGDIOBJ hOldFont = pDC->SelectObject(m_hFont);

	// protect space for size box
	INT32 cxSizeBox = m_bHideSizeBox ? 0 : m_cxSizeBox;
	INT32 xMax = (rect.right -= cxSizeBox);
	if (cxSizeBox == 0)
		xMax += m_cxRightBorder + 1;

	// walk through to calculate extra space
	INT32 cxExtra = rect.Width() + m_cxDefaultGap;
	AFX_STATUSPANE* pSBP = (AFX_STATUSPANE*)m_pData;
	for (INT32 i = 0; i < m_nCount; i++, pSBP++)
		cxExtra -= (pSBP->cxText + CX_BORDER * 4 + m_cxDefaultGap);
	// if cxExtra <= 0 then we will not stretch but just clip

	for (i = 0, pSBP = (AFX_STATUSPANE*)m_pData; i < m_nCount; i++, pSBP++)
	{
		ASSERT(pSBP->cxText >= 0);
		INT32 cxText = pSBP->cxText;
		if ((pSBP->nStyle & SBPS_STRETCH) && cxExtra > 0)
		{
			cxText += cxExtra;
			cxExtra = 0;
		}
		rect.right = rect.left + cxText + CX_BORDER * 4;
		rect.right = min(rect.right, xMax);
//		if (!afxData.bWin32s || pDC->RectVisible(&rect))
		if (pDC->RectVisible(&rect))
		{
//NEW CODE
			if (pPaneData[i].BitmapID)
			{
				if (pSBP->nStyle & SBPS_DISABLED)
					PaintBitmap(pDC, rect, pPaneData[i].OffBitmapID, pSBP->nStyle);
				else
					PaintBitmap(pDC, rect, pPaneData[i].BitmapID, pSBP->nStyle);
			}
			else
// END OF NEW CODE
				DrawStatusText(pDC, rect, pSBP->lpszText, pSBP->nStyle);
		}
		rect.left = rect.right + m_cxDefaultGap;
		if (rect.left >= xMax)
			break;
	}
	pDC->SelectObject(hOldFont);

	// draw the size box in the bottom right corner
	if (cxSizeBox != 0)
	{
		INT32 cxMax = min(cxSizeBox, rect.Height()+m_cyTopBorder);
		rect.left = xMax + (cxSizeBox - cxMax) + CX_BORDER;
		rect.bottom -= CX_BORDER;
		HPEN hPenOld = (HPEN)pDC->SelectObject(afxData.hpenBtnHilite);
		for (INT32 i = 0; i < cxMax; i += 4)
		{
			pDC->MoveTo(rect.left+i, rect.bottom);
			pDC->LineTo(rect.left+cxMax, rect.bottom-cxMax+i);
		}
		pDC->SelectObject(afxData.hpenBtnShadow);
		for (i = 1; i < cxMax; i += 4)
		{
			pDC->MoveTo(rect.left+i, rect.bottom);
			pDC->LineTo(rect.left+cxMax, rect.bottom-cxMax+i);
		}
		for (i = 2; i < cxMax; i += 4)
		{
			pDC->MoveTo(rect.left+i, rect.bottom);
			pDC->LineTo(rect.left+cxMax, rect.bottom-cxMax+i);
		}
		pDC->SelectObject(hPenOld);
	}
} 

*/


/*************************************************************************************************
>	void CCStatusBar::DrawStatusText(CDC* pDC, const CRect& rect, LPCTSTR lpszText, UINT32 nStyle)
	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	-
	Purpose:	Copy of MFC function to allow everything upto the first ':' to be emboldened
*************************************************************************************************/

void CCStatusBar::DrawStatusText(CDC* pDC, const CRect& rect, LPCTSTR lpszText, UINT32 nStyle)
{
	ASSERT_VALID(pDC);
/*	UIC 
	if (!(nStyle & SBPS_NOBORDERS))
	{
		// Colours are in parameter order:
		// clrTopLeft   Specifies the color of the top and left sides of the three-dimensional rectangle.
		// clrBottomRight   Specifies the color of the bottom and right sides of the three-dimensional rectangle.
		if (nStyle & SBPS_POPOUT)
			pDC->Draw3dRect(rect, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW)); //afxData.clrBtnHilite, afxData.clrBtnShadow);
		else
			pDC->Draw3dRect(rect, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHIGHLIGHT)); //afxData.clrBtnShadow, afxData.clrBtnHilite);
	}
*/ 
	// just support left justified text
	if (lpszText != NULL) // && !(nStyle & SBPS_DISABLED))
	{
		CRect rectText(rect);
		if (!(nStyle & SBPS_NOBORDERS)) // only adjust if there are borders
			rectText.InflateRect(-2*CX_BORDER, -CY_BORDER);
		else
			rectText.OffsetRect(0, -CY_BORDER); // baselines line up
	/*	UIC
		// chicago bodge - shift the text up by one pixel
		if(IS_CHICAGO)
			rectText.OffsetRect(0, -1); 
	*/	

		// background is already grey   
		INT32 nOldMode = pDC->SetBkMode(TRANSPARENT);  
		COLORREF crBkColor = GetSysColor(COLOR_BTNFACE);  
		pDC->SetBkColor(crBkColor);  
		//COLORREF crBkColor   = pDC->SetBkColor(afxData.clrBtnFace);  
		COLORREF crTextColor = GetSysColor(COLOR_BTNTEXT);
		pDC->SetTextColor(crTextColor); //afxData.clrBtnText); 

	if (nStyle & SBPS_DISABLED)
			pDC->SetTextColor(crTextColor);	//afxData.clrBtnShadow);

		// align on bottom (since descent is more important than ascent)
		pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);

// NEW CODE - output all text upto the first colon (if any) in bold
		String_256 text(lpszText);
		INT32 BoldLen=text.Sub(String_8("::"))+1;	// include one colon
		if (BoldLen>0)
		{
			// split into 2 strings
			String_256 BoldText("");
			text.Left(&BoldText,BoldLen);
			text.Right(&text,text.Length()-BoldLen-1); // strip one colon

			// select bold font and find width of bold text (limited to text rect width)
		//	HGDIOBJ hOldFont=pDC->SelectObject(FontFactory::GetFont(STOCKFONT_STATUSBARBOLD));
			HGDIOBJ hOldBoldFont=pDC->SelectObject(FontFactory::GetFont(STOCKFONT_STATUSBARBOLD));  //UIC 

			INT32 BoldTextWidth=pDC->GetTextExtent(BoldText,BoldText.Length()).cx;
			INT32 TextRectWidth=rectText.Width();
			if (BoldTextWidth>TextRectWidth)
				BoldTextWidth=TextRectWidth;

			// output the bold text, modify text rect, restore font
			pDC->ExtTextOut(rectText.left, rectText.bottom,
				ETO_CLIPPED, &rectText, (TCHAR*)BoldText, BoldLen, NULL);
			rectText.left+=BoldTextWidth;
		//	pDC->SelectObject(hOldFont);
			pDC->SelectObject(hOldBoldFont); // UIC
		}
// END OF NEW CODE
		HGDIOBJ hOldFont=pDC->SelectObject(FontFactory::GetFont(STOCKFONT_STATUSBAR)); // UIC	
		// output remaining text
		pDC->ExtTextOut(rectText.left, rectText.bottom,
			ETO_CLIPPED, &rectText, (TCHAR*)text, text.Length(), NULL);
		pDC->SelectObject(hOldFont);	// UIC 
	}
}

////////////////////////// End of bits copied from MFC //////////////////////////////


/******************************************************************************

>	BOOL CCStatusBar::PaintBitmap(CDC* pDC, const CRect& rect, UINT32 BitmapID, UINT32 nStyle)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/95
	Purpose:	Paint a bitmap into a StatusLine pane
				Accounting for Styles: BORDERS/POPOUT/DISABLED
	Inputs:		pDC      - 
				rect     - 
				BitmapID - 
				nStyle   -
	Returns:	FALSE if failed
	Errors:		this == NULL
	SeeAlso:	CColourBar::PaintEditOrNewButton;

******************************************************************************/

BOOL CCStatusBar::PaintBitmap(CDC* pDC, const CRect& rect, UINT32 BitmapID, UINT32 nStyle)
{
	CRect BMRect(rect);
	if (!(nStyle & SBPS_NOBORDERS))
		BMRect.SetRect(rect.left+CX_BORDER, rect.top+CY_BORDER, rect.right-CX_BORDER, rect.bottom-CY_BORDER);

	if (BitmapID)
	{
		CDC srcDC;
		srcDC.CreateCompatibleDC(NULL);

		CBitmap bitmap;
		bitmap.LoadBitmap(BitmapID);

		CBitmap* pOldBitmap = srcDC.SelectObject(&bitmap);

		pDC->BitBlt(BMRect.left, BMRect.top, BMRect.Width(), BMRect.Height(), &srcDC, 0,0, SRCCOPY);

		// Now plot the glyph bitmap. It is a masked plot - all the light grey pixels
		// are plotted using the system COLOR_BTNFACE colour.
		CDC MonoDC;
		MonoDC.CreateCompatibleDC(NULL);

		// ...Now create a mono bitmap for the mask
		CBitmap MonoBitmap;
		MonoBitmap.CreateBitmap(BMRect.Width(), BMRect.Height(), 1, 1, NULL);
		CBitmap *OldMonoBitmap = MonoDC.SelectObject(&MonoBitmap);

		// Plot all light grey pixels in the glyph as black in our mask, while other pixels are white
		// The transparent bits on the bitmap button should be coloured purple to be see through 
		MonoDC.PatBlt(0, 0, BMRect.Width(), BMRect.Height(), WHITENESS);
		srcDC.SetBkColor(RGB(255,0,255));		// purple masking colour
		MonoDC.BitBlt(0, 0, BMRect.Width(), BMRect.Height(), &srcDC, 0, 0, SRCCOPY);

		pDC->SetTextColor(0L);                  // 0's in mono -> 0 (for ROP)
		pDC->SetBkColor((COLORREF)0x00FFFFFFL); // 1's in mono -> 1

		// And then blit the mask shape over the top, using the correct button face colour
		// (The areas not in the mask will not be plotted, so we are rendering the mask area only)
		CBrush HighlightBrush;
		HighlightBrush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

		CBrush *OldBrush = pDC->SelectObject(&HighlightBrush);
		// draw highlight color where we have 0's in the mask, using a special RasterOp code (DSPDxax)
		pDC->BitBlt(BMRect.left, BMRect.top, BMRect.Width(), BMRect.Height(), &MonoDC, 0, 0, 0x00E20746L);

		// Put back the old brush
		pDC->SelectObject(OldBrush);

		MonoDC.SelectObject(OldMonoBitmap);

		// Put back the old bitmap
		srcDC.SelectObject(pOldBitmap);
	}
/*	UIC 

	if (!(nStyle & SBPS_NOBORDERS))
	{
		// Colours are in parameter order:
		// clrTopLeft   Specifies the color of the top and left sides of the three-dimensional rectangle.
		// clrBottomRight   Specifies the color of the bottom and right sides of the three-dimensional rectangle.

		if (nStyle & SBPS_POPOUT)
			pDC->Draw3dRect(rect, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW)); //afxData.clrBtnHilite, afxData.clrBtnShadow);
		else
			pDC->Draw3dRect(rect, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHIGHLIGHT)); //afxData.clrBtnShadow, afxData.clrBtnHilite);
	}
*/
	return TRUE;
}


/***********************************************************************************************
>	BOOL CCStatusBar::UpdatePaneBitmap(INT32 PaneID, UINT32 BitmapID, BOOL ImmediateUpdate=FALSE);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/95
	Purpose:	Update specified status bar pane
	Inputs:		PaneID   - id of pane
				BitmapID - text (or NULL)
				ImmediateUpdate - redraw pane now (rather than queuing)
	Returns:	FALSE if failed (see Errors)
	Errors:		this==NULL,
				PaneID unrecognised
************************************************************************************************/

BOOL CCStatusBar::UpdatePaneBitmap(INT32 PaneID, UINT32 BitmapID, BOOL ImmediateUpdate)
{
	ERROR2IF(this == NULL,FALSE,"CStatusBar::UpdatePaneBitmap() - this == NULL");

	INT32 PaneIndex = CommandToIndex(PaneID);
	ERROR2IF(PaneIndex == -1,FALSE,"CStatusBar::UpdatePaneBitmap() - unknown PaneID");

	/*if (pPaneData[PaneIndex].BitmapID != BitmapID)
	{
*/
		if (BitmapID!=0)
			pPaneData[PaneIndex].BitmapID = BitmapID;

		CRect rect;
		GetItemRect(PaneIndex,&rect);

		UINT32 nStyle;
		INT32  nWidth;
		UINT32 nID;
		GetPaneInfo(PaneIndex,nID,nStyle,nWidth);
		if (!(nStyle & SBPS_NOBORDERS))
			rect.SetRect(rect.left+CX_BORDER, rect.top+CY_BORDER, rect.right-CX_BORDER, rect.bottom-CY_BORDER);

		InvalidateRect(rect);
		if (ImmediateUpdate)
			UpdateWindow();
//	}

	return TRUE;
}



