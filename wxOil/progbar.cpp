// $Id: progbar.cpp 662 2006-03-14 21:31:49Z alex $
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
// ProgBar.cpp - Implementation of progress bar window class


/*
*/


// NOTE: This class is based upon the MFC CStatusWindow (see afxext.h, barcore.cpp)
// It uses the AFX StatusBar window resource to create itself from, so as to
// be the same size, position, etc. as the status bar.
//
// Note however, that it uses the system font instead of the same font as the
// status bar. This is because (a) system font is bolder, and (b) because I
// couldn't work out how to get 10 point high text, suggesting that either MFC
// or the online documentation has a serious flaw in this area... This could
// possibly mean that the bar comes up a different height from the status bar,
// which will look a bit naff...

#include "camtypes.h"

#include "app.h"
#include "mainfrm.h"
#include "statline.h"
#include "progbar.h"
#include "fonts.h"
#include "camafx.h"
#include "cstatbar.h"


#if _MFC_VER == 0x400
// Strangeness required for VC4...

#define AFX_WNDCONTROLBAR_REG   (0x0002)

#define AfxDeferRegisterClass(fClass) \
	((afxRegisteredClasses & fClass) ? TRUE : AfxEndDeferRegisterClass(fClass))

extern BOOL AFXAPI AfxEndDeferRegisterClass(short fClass);

#endif

// Stuff that is normally private to MFC AFX routines...
// If you get a new version of MFC and the progress bar Goes South, then
// these are a damn good place to start laying blame...

#if _MSC_VER >= 1200
	static char* _afxWndControlBar = "MyAfxControlBar";
#else
	#if WIN32
		#ifndef _AFXDLL
		extern const char NEAR _afxWndControlBar[]; // in mfc\src\wincore.cpp or similar
		#else
		// the variable isn't available in the DLL version of MFC, so we use this naughty method
		#define	_afxWndControlBar	"AfxControlBar"
		#endif
	#else
	extern const char BASED_CODE _afxWndControlBar[];
	#endif
#endif

#define CX_BORDER 1
#define CY_BORDER 1

#include <afxpriv.h>





#ifdef _DEBUG

//#pragma message("")
//#pragma message("  Special rendering debug is ENABLED in this build")
//#pragma message("")

#define CHECKPEN(PTR)	((CPen *)CheckPtr((void *)(PTR), __LINE__))
#define CHECKBRUSH(PTR)	((CBrush *)CheckPtr((void *)(PTR), __LINE__))
#define CHECKFONT(PTR)	((CFont *)CheckPtr((void *)(PTR), __LINE__))

static void *CheckPtr(void *Pointer, INT32 LineNum)
{
	if (Pointer == NULL)
	{
		TRACE( _T("progbar.cpp, line %ld: CPen/CBrush/CFont returned was NULL!\n"), LineNum);
		TRACE( _T(" --> GetLastError claims the problem was: 0x%x\n"), (INT32)GetLastError());
	}
	return(Pointer);
}

#else
#define CHECKPEN(Ptr)	((CPen *) (Ptr))
#define CHECKBRUSH(Ptr)	((CBrush *) (Ptr))
#define CHECKFONT(Ptr)	((CFont *) (Ptr))
#endif






static const MAXBARWIDTH = 200; 		// Bar should never be > 200 pixels wide
										// - looks better, & leaves room for text


IMPLEMENT_DYNAMIC(CProgressBar, CControlBar)



CProgressBar::CProgressBar()
{
	m_hFont = FontFactory::GetFont(STOCKFONT_STATUSBAR);
	CurrentPercent = -1;
}


/********************************************************************************************
>	BOOL CProgressBar::~CProgressBar()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroys the object, as one might expect.
	Notes:		IMPORTANT - After calling the destructor (delete ProgressBar) it is
				VITAL that you then call RecalcLayout() for the main frame window in
				which the Progress Bar is appearing (the one you passed in to PB::Create().
				If you don't do this, the bar won't disappear until the next time the main 
				window is resized or a tool is chosen.

********************************************************************************************/

CProgressBar::~CProgressBar()
{
	if (JobDescription != NULL)
		delete JobDescription;

//	ParentWindow->RecalcLayout();				// Cause ourself to be hidden
		// The above does not work, as it appears to be called before the window
		// has actually been deleted, and therefore has no effect. Thus, the 
		// responsibility for doing this is passed on to the entity calling 'delete'
}


/********************************************************************************************
>	BOOL CProgressBar::Create(CFrameWnd *ParentWnd, String_64 *JobDescription = NULL,
								DWORD dwstyle = WS_CHILD | WS_VISIBLE | CBRS_BOTTOM,
								UINT32 nID = _R(AFX_IDW_STATUS_BAR));

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/94
	Inputs:		ParentWnd - The parent frame window (in Camelot, there can be only one)
				JobDescription - A BRIEF string describing the job currently being undertaken.
						This will be displayed on the progress bar if possible
				dwstyle, nID - you shouldn't have to mess with these (cf CStatusBar)
	Outputs:	-
	Returns:	TRUE if the initialisation of the progress bar was successful.
	Purpose:	Creates a window and associates it with this CProgressBar object. The window
				appears immediately over the status bar.
	Notes:		This currently assumes that it'll only ever be opened at the bottom of the
				Main Frame window.
	SeeAlso:	CProgressBar::SetPercent; CProgressBar::GetPercent
********************************************************************************************/

BOOL CProgressBar::Create(CFrameWnd* pParentWnd, String_64 *JobDescrip,
							DWORD dwStyle, UINT32 nID)
{
	ASSERT_VALID(pParentWnd);   // must have a parent

	CurrentPercent = -1;
	if (JobDescrip == NULL)
		JobDescription = NULL;
	else
		JobDescription = new String_64(*JobDescrip);
#if WIN32
	// this element of CControlBar does not exist in MFC 2.5 16-bit
	m_dwStyle = dwStyle;
#endif

#if _MFC_VER == 0x400
	// MFC 4 defers registering of window classes, so make sure the window class
	// is registered before we use it.
	// (code taken from mfc/src/bardock.cpp, in MFC 4)
	if (!AfxDeferRegisterClass(AFX_WNDCONTROLBAR_REG))
		return FALSE;
#endif


	// Work out the position of the bar.
	CRect rect;
	rect.SetRectEmpty();

	// create the window
	if (!CWnd::Create(_afxWndControlBar, NULL, dwStyle, rect, pParentWnd, nID))
		return FALSE;
		// NOTE: Parent must resize itself for control bar to be resized

	// set initial font and calculate bar height
	OnSetFont((WPARAM)m_hFont, 0);  			// initialize font height etc


	pParentWnd->RecalcLayout();					// Cause ourself to be shown
	SetPercent(0, TRUE);		// Force redraw of window _including background_

	return TRUE;
}



void CProgressBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
// I'm not entirely sure how much of this is necessary, but it doesn't seem to
// do any harm, and things didn't work properly until I put it in...
{
	CCmdUI state;
	state.m_pOther = this;
	state.m_nIndexMax = (UINT32)m_nCount;

	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
	  state.m_nIndex++)
	{
		state.m_nID = 0;
		state.DoUpdate(pTarget, bDisableIfNoHndler);
	}

	// update the dialog controls added to the status bar (of which there are none)
	UpdateDialogControls(pTarget, bDisableIfNoHndler);
}



/********************************************************************************************
>	INT32 CProgressBar::GetPercent(void);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/94
	Inputs:		-
	Outputs:	-
	Returns:	The current percentage setting displayed in the progressbar window.
	Purpose:	Reads the currently displayed percentage of the progress bar.
	SeeAlso:	CProgressBar::SetPercent; CProgressBar::Create
********************************************************************************************/

INT32 CProgressBar::GetPercent(void) const
{
	return(CurrentPercent);
}


/********************************************************************************************
>	BOOL CProgressBar::SetPercent(INT32 NewPercent,
									BOOL ClearBackground = FALSE, String_64 *JobDescrip = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/94
	Inputs:		NewPercent - the new percentage value to be displayed by the progress bar

				ClearBackground - Clears the entire bar background and redraws everything, rather
				than doing a (far more efficient) update of the bar/percentage alone. Use the default
				of FALSE unless absolutely necessary!

				JobDescrip - NULL, or a pointer to a new job description - pass NULL if this hasn't
				changed, as it makes a copy of the new string every time it is changed.	If this is
				non-NULL, then the ClearBackground flag will be forced to TRUE to draw the new text.

	Outputs:	-
	Returns:	TRUE if it successfully makes the change.
	Purpose:	Sets the currently displayed percentage of the progress bar. The bar
				will be immediately redrawn to reflect the new value. Values outside the
				range 0..99 (inclusive) are clipped to 0 or 99. No redraw will be done if
				NewPercent equals the currently displayed value.
	SeeAlso:	CProgressBar::GetPercent; CProgressBar::Create
********************************************************************************************/

BOOL CProgressBar::SetPercent(INT32 NewPercent, BOOL ClearBackground /* =FALSE */,
								String_64 *JobDescrip /* =NULL */)
// The extra argument ClearBackground is not mentioned in the help, because it
// is used internally - When the window is first created, the background must be cleared,
// but on normal updates this causes horrible flicker, so is to be avoided.
{
	if (NewPercent < 0) NewPercent = 0;
	if (NewPercent > 99) NewPercent = 99;

	// If there's no change, don't bother updating
	if (JobDescrip == NULL && NewPercent == CurrentPercent)
		return(TRUE);

	CurrentPercent = NewPercent;

	// If there is a new Job Description, change to use it
	if (JobDescrip != NULL)
	{
		delete JobDescription;
		JobDescription = new String_64(*JobDescrip);

		ClearBackground = TRUE;		// Ensure that the new text is drawn
	}


#if FALSE
/*
	CDC *DC = GetDC();

	if (ClearBackground)
	{
		CRect	Rect;
		CBrush	FillBrush(GetSysColor(COLOR_BTNFACE));

		GetClientRect(&Rect);					// Get window position/area...
		DC->FillRect(Rect, &FillBrush);			// [...& fill backgnd with grey]
	}

	DoPaint(DC);								// Force immediate redraw
	ReleaseDC(DC);
*/
#else
	Invalidate(ClearBackground);				// Invalidate the window
	UpdateWindow();								// And redraw it immediately
#endif

	return(TRUE);
}


static void Paint3dPlinth(CDC* pDC, CRect *rect, BOOL Indent = TRUE)
// Paints a 3-D plinth rectangle. If Indent is TRUE, it is indented, else it
// is raised. Other 3-D effects are not supported
// NOTE that the rectangle is drawn around the OUTSIDE of the given rectangle
{
// **** If 3d look is turned off, then we need to select black for BOTH of these

	CPen Black(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));
	CPen White(PS_SOLID, 0, GetSysColor(COLOR_BTNHIGHLIGHT));

	CPen *OldPen = CHECKPEN(pDC->SelectObject((Indent) ? &White : &Black));

	pDC->MoveTo(rect->left, rect->bottom-1);
	pDC->LineTo(rect->left, rect->top);
	pDC->LineTo(rect->right-1, rect->top);

	CHECKPEN(pDC->SelectObject((Indent) ? &Black : &White));
	pDC->MoveTo(rect->right-1, rect->top+1);	// Ensure corner pixel is correct
	pDC->LineTo(rect->right-1, rect->bottom-1);
	pDC->LineTo(rect->left, rect->bottom-1);

	CHECKPEN(pDC->SelectObject(OldPen));
}



void CProgressBar::DoPaint(CDC* pDC)
// Paints the bar, assuming that the background has already been cleared to grey.
// The bar has a percentage display on the left, followed by a progress bar
// which is sized to fit the available space up to a maximum of MAXBARWIDTH pixels
// If there is enough room, the current Job Description will also be displayed, to
// the right of the bar.
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	CControlBar::DoPaint(pDC);      // draw border

	CRect rect;
	GetClientRect(rect);
	CalcInsideRect(rect);

	ASSERT(m_hFont != NULL);
	CFont *OurFont = CHECKFONT(CFont::FromHandle(m_hFont));		// Get a CFont* for this HFONT
	CFont *OldFont = CHECKFONT(pDC->SelectObject(OurFont));

	rect.InflateRect(-CX_BORDER, -CY_BORDER);

	CString PercentText("00%");							// Generate %age string
	if (CurrentPercent > 0 && CurrentPercent < 100)
	{
		if (CurrentPercent > 9)
			PercentText.SetAt(0, (CurrentPercent / 10) + '0');
		PercentText.SetAt(1, (CurrentPercent % 10) + '0');
	}

	pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));		// Draw %age string
	pDC->SetBkColor(GetSysColor(COLOR_BTNFACE));
	pDC->SetBkMode(OPAQUE);
	SetTextAlign(pDC->m_hDC, TA_LEFT | TA_BOTTOM);
	pDC->TextOut(rect.left, rect.bottom, PercentText);

	CSize TextSize = pDC->GetTextExtent(PercentText, 3);

	INT32 BarStart = rect.left + TextSize.cx + 4;			// Calc bar position, size
	INT32 WindowEnd = rect.right - 4;
	INT32 BarWidth = WindowEnd - BarStart;
	
	if (BarWidth < 10) return;		// Not enough room to do a bar!

	if (BarWidth > MAXBARWIDTH) BarWidth = MAXBARWIDTH;

	if (JobDescription != NULL && WindowEnd - BarStart > MAXBARWIDTH + 64)
	{	// If enough room for some text, add the job description
		CString OutText = TEXT(*JobDescription);

		rect.left = BarStart + BarWidth + 4;
		rect.right = WindowEnd;
		pDC->TextOut(rect.left, rect.bottom, OutText);
	}

	CHECKFONT(pDC->SelectObject(OldFont));	// Restore previous font 

	rect.left = BarStart;				// Finally, calc and paint the bar,,,
	rect.right = BarStart+BarWidth;
	rect.bottom -= m_FontDescent - 2;	// Line up bar bottom with text baseline
	Paint3dPlinth(pDC, &rect, FALSE);
	
	BarWidth -= 4;						// Bar is 2 pixels inside plinth rectangle
	rect.left = BarStart + 2;
	rect.top += 2;
	rect.bottom -= 2;
	rect.right = rect.left + (INT32) (((WORD)BarWidth * (WORD)CurrentPercent) / 100);

	CBrush BlueBrush(RGB(0,0,255));
	pDC->FillRect(rect, &BlueBrush);
}

 

BEGIN_MESSAGE_MAP(CProgressBar, CControlBar)
	//{{AFX_MSG_MAP(CProgressBar)
	// control messages
	ON_WM_SIZE()
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_MESSAGE(WM_GETFONT, OnGetFont)
	ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CProgressBar::OnSize(UINT32, INT32, INT32)
{
	// force repaint on resize
	Invalidate();
}


LRESULT CProgressBar::OnSizeParent(WPARAM, LPARAM lParam)
// This overrides the normal OnSizeParent method (CControlBar)
// Normal control bars can never overlap - each is opened using the parent's
// client rectangle, and then their area is subtracted from the available client
// area, so that they never overlap. 
// However, the progress bar wants to sit over the top of the status bar if
// possible, so before allowing the normal action to proceed...
// If the status window's client rectangle butts onto the layout rect passed in,
// then we extend the layout rectangle to include the status bar, and hence we
// will be placed over the top of the SB.
// NOTE - currently only does this if attached to the BOTTOM of the window.
// If this fails (sttaus bar not open, or at top of window, etc), the progress
// bar will appear normally (above/below all other control bars)
{
	AFX_SIZEPARENTPARAMS FAR* lpLayout = (AFX_SIZEPARENTPARAMS FAR*)lParam;
	CRect StatusRect;

	((CMainFrame *) AfxGetApp()->m_pMainWnd)->GetStatusBarWindowRect(&StatusRect);

	if (!StatusRect.IsRectEmpty())					// Cover status bar (if open)
	{
		// JustinF says: this is a quick bodge so that the progress-bar respects any
		// overriding prefix displayed in the status line by StatusLine::SetPrefix.
		StatusRect.left = StatusLine::GetPrefixWidth();

		// We just move our window so it is covering the status bar, using the
		// HWDP supplied in the AFX_SIZEPARENTPARAMS structure.
		lpLayout->hDWP = ::DeferWindowPos(lpLayout->hDWP, m_hWnd, HWND_TOP,
									 	  StatusRect.left, StatusRect.top,  
										  StatusRect.right - StatusRect.left,
										  StatusRect.bottom - StatusRect.top,
										  SWP_NOACTIVATE);
	}
	else
	{

	#if _MFC_VER < 0x300
		// No status bar - just plonk the progress bar where we can...
		return (CControlBar::OnSizeParent((WPARAM)0, lParam));
	#else
		// resize and reposition this control bar based on styles
		DWORD dwStyle = (m_dwStyle & (CBRS_ALIGN_ANY|CBRS_BORDER_ANY)) |
			(GetStyle() & WS_VISIBLE);

		if ((dwStyle & WS_VISIBLE) && (dwStyle & CBRS_ALIGN_ANY) != 0)
		{
			// align the control bar
			CRect rect;

			if (StatusRect.IsRectEmpty())
			{
				// Status bar is not open - just fit in where we can.
				// (This code copied from barcore.cpp in MFC 3)
				rect.CopyRect(&lpLayout->rect);

				CSize sizeAvail = rect.Size();	// maximum size available

				// get maximum requested size
				CSize size = CalcFixedLayout(lpLayout->bStretch, 
					(dwStyle & CBRS_ORIENT_HORZ) ? TRUE : FALSE);

				size.cx = min(size.cx, sizeAvail.cx);
				size.cy = BarHeight;

				lpLayout->sizeTotal.cy += size.cy;
				lpLayout->sizeTotal.cx = max(lpLayout->sizeTotal.cx, size.cx);
				rect.top = rect.bottom - size.cy;
				lpLayout->rect.bottom -= size.cy;

				rect.right = rect.left + size.cx;
				rect.bottom = rect.top + size.cy;

				// only resize the window if doing layout and not just rect query
				if (lpLayout->hDWP != NULL)
				{
					AfxRepositionWindow(lpLayout, m_hWnd, &rect);
				}
			}
		}
	#endif
	}

	return 0;

}



LRESULT CProgressBar::OnSetFont(WPARAM wParam, LPARAM)
// Copied verbatim from barcore.cpp
{
	m_hFont = (HFONT)wParam;

	ASSERT(m_hFont != NULL);

	// recalculate based on font height + borders
	TEXTMETRIC tm;
	// get text metrics of font
	{
		CClientDC dcScreen(NULL);

		CFont *OurFont = CHECKFONT(CFont::FromHandle(m_hFont));
		CFont *OldFont = CHECKFONT(dcScreen.SelectObject(OurFont));

		VERIFY(dcScreen.GetTextMetrics(&tm));
		CHECKFONT(dcScreen.SelectObject(OldFont));
	}
	CRect rectSize;
	rectSize.SetRectEmpty();
	CalcInsideRect(rectSize);       // will be negative size

#if _MFC_VER < 0x300
	m_sizeFixedLayout.cy = (tm.tmHeight - tm.tmInternalLeading) + 
			CY_BORDER*3 /* 1 extra on top, 2 on bottom */ - rectSize.Height();
	ASSERT(m_sizeFixedLayout.cx == 32767);  // max size
#else
	BarHeight = (tm.tmHeight - tm.tmInternalLeading) + 
			CY_BORDER*3 /* 1 extra on top, 2 on bottom */ - rectSize.Height();
#endif

	m_FontDescent = tm.tmDescent;	// Cache descent size

	return 0L;      // does not re-draw or invalidate - resize parent instead
}


LRESULT CProgressBar::OnGetFont(WPARAM, LPARAM)
{
	return (LRESULT)(UINT32)m_hFont;
}


