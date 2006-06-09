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

/*
*/

#include "camtypes.h"

// WEBSTER - markn 15/1/97
// No rulers in Webster
#ifndef WEBSTER

//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "oilruler.h"
#include "mainfrm.h"
#include "camafx.h"
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "mario.h"
//#include "winrect.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "scrcamvw.h"
#include "rulers.h"
#include "fonts.h"
#include "guides.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "camelot.h"
#include "statline.h"
//#include "ed.h"
#include "ctrlhelp.h"
//#include "ops.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "richard2.h"	// For naughty inline strings...
#include "basebar.h"
#include "brushmsg.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define MAJOR_GRAT_LEN 6
#define MIN_PIXEL_GRATICULE_SPACING_X 5
#define MIN_PIXEL_GRATICULE_SPACING_Y 5

// statics ...
String_256 			*OILRuler::FontName        = NULL; /*String_256(_R(IDS_OILRULER_SMALL_FONTS)); "Small Fonts";*/ // - read from .ini
INT32        			OILRuler::FontSize         = 7;				// read from .ini
INT32       			OILRuler::RenderWidth      = 0;		// this is set up on app start up 
UINT32       			OILRuler::RulerWidth       = 0;		// actually offset to next bar (based on RenderWidth)
INT32       			OILRuler::CharHeight       = 0;
INT32       			OILRuler::CharWidth        = 0;
DocView*   			OILRuler::pPaintDocView    = NULL;
CPaintDC*  			OILRuler::pPaintDC         = NULL;
CSize      			OILRuler::RulerToDocOffset = WinCoord(0,0);
OpGuidelineParam	OILRuler::NewGuidelineParam;


/********************************************************************************************
>	void PatB(CDC* hDC, INT32 x, INT32 y, INT32 dx, INT32 dy, COLORREF rgb)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (Some MFC dude, actually)
	Created:	14/3/94
	Inputs:		hDC - destination DC
				x,y,dx,dy - rectangle to fill
				rgb - colour to fill with
	Purpose:	Paints a rectangle in the given (dithered) colour
				It looks pretty hideous, but this is how the MFC buttonbar does it...
				The conclusions that this leads to are left as an exercise for the reader.
				(OK, so they're not. It suggest that either MFC sux, or plotting text is
				easier/faster than creating a brush, in which case Windoze sux)
********************************************************************************************/

static void NEAR PASCAL PatB(CDC* cDC, INT32 x, INT32 y, INT32 dx, INT32 dy, COLORREF rgb)
{
	RECT rect;
	rect.left	= x;
	rect.top	= y;
	rect.right	= x + dx;
	rect.bottom	= y + dy;

	cDC->SetBkColor(rgb);
	cDC->ExtTextOut(0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
}
 


/////////////////////////////////////////////////////////////////////////////////////////////
//									 OILRuler Base Class
/////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(OILRuler ,CWnd)

BEGIN_MESSAGE_MAP( OILRuler,CWnd)
	//{{AFX_MSG_MAP( OILRuler )
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP( )


/********************************************************************************************
>	OILRuler::OILRuler()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Purpose:	Creates an OILRuler object.
********************************************************************************************/
                    
OILRuler::OILRuler()
{ 
	InDrag       = FALSE;
	pKernelRuler = NULL;

	// if ruler has a border, the ruler has a one pixel black border,
	// one side of this border overlaps with the adjacent bar etc
	// so the offset to the next bar etc must only account for 1 of the 2 border pixels
	ERROR3IF(RenderWidth==0,"OILRuler::OILRuler() - RenderWidth==0 (not initialised!)");

// BODGE RULER - this should be OILRuler::RenderWidth+1 and OILRuler::RenderWidth !!!
	if(RULER_BORDERS)
		RulerWidth = RenderWidth+2;
	else
		RulerWidth = RenderWidth;
}


/********************************************************************************************
>	BOOL OILRuler::Create(CWnd* pParentWindow)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		pParentWindow - 
	Returns:	FALSE if fails
	Purpose:	Creates the ruler window.
********************************************************************************************/
                    
BOOL OILRuler::Create(CWnd* pParentWindow)
{ 
	BOOL CreatedWindow;
	ENSURE(pParentWnd,"DockingBar must have a Parent");
	
	pParentWnd = (ScreenCamView*) pParentWindow;

	CRect rect;			// Rectangle for Create
	rect.SetRectEmpty();
	DWORD style = WS_CHILD  | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE;
	if(RULER_BORDERS)
		style |=WS_BORDER;
	#ifndef WIN32
		DWORD
	#endif
	
	// create the HWND
	CreatedWindow = CWnd::Create(NULL,
						AfxRegisterWndClass(CS_DBLCLKS,0,0,0),
						style,
						rect,
						pParentWnd,
						0,
						NULL);
	
	PostCreate();

	return CreatedWindow;
}


/********************************************************************************************
>	virtual BOOL OILRuler::StartDrag(UINT32 nFlags,CPoint point)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Returns:	FALSE if fails
	Purpose:	base StartDrag function  - don't call
********************************************************************************************/
                    
BOOL OILRuler::StartDrag(UINT32 nFlags,CPoint point)
{ 
	return TRUE;
}


/*********************************************************************************************
>	void OILRuler::ShowRuler(BOOL show)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Inputs:		show - FALSE to hide the ruler
	Purpose:	Shows or hides this ruler
**********************************************************************************************/ 

void OILRuler::ShowRuler(BOOL show)
{
	ShowWindow(show ? SW_SHOWNA : SW_HIDE);
	
	BROADCAST_TO_ALL(ScreenChangeMsg());

	if (show)
	{
		// we need to take into account camelots new floating toolbar ....

		DockBarType DockBarType;// = DOCKBAR_FLOAT;
		KernelBarPos * pKernelBarPos;

		// find out where the toolbar is docked
		GetMainFrame()->GetBarPosInfo(&(String_32 ("Toolbar")), &DockBarType, &pKernelBarPos);

		if (DockBarType == DOCKBAR_FLOAT)
		{
			RECT MainFrameWindowRect;
			::GetWindowRect (GetSafeHwnd (), &MainFrameWindowRect);

			POINT ToolbarTL;
			ToolbarTL.x = pKernelBarPos->x;
			ToolbarTL.y = pKernelBarPos->y;

			if (PtInRect (&MainFrameWindowRect, ToolbarTL))
			{
				KernelBarPos NewToolBarPos = *pKernelBarPos;
				BaseBar* PtrToToolBar = BaseBar::GetPtrToToolBar ();
				
				// then we need to move the toolbar ....
				if (IsHorizontal ())
				{
					INT32 dy = MainFrameWindowRect.bottom - ToolbarTL.y;
					NewToolBarPos.y += dy;
				}
				else // its the vertical ruler
				{
					INT32 dx = MainFrameWindowRect.right - ToolbarTL.x;
					NewToolBarPos.x += dx;
				}

				if (PtrToToolBar)
				{
					PtrToToolBar->MoveBar ((WPARAM) DOCKBAR_FLOAT, (LPARAM) &NewToolBarPos);
				}
			}
		}
	}
} 


/*********************************************************************************************
>	void OILRuler::UpdateRuler()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Inputs:		-
	Purpose:	Redraw the whole ruler
**********************************************************************************************/ 

void OILRuler::UpdateRuler()
{
	Invalidate();
	UpdateWindow();
} 


/********************************************************************************************
>	virtual OILRuler::~OILRuler()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Purpose:	Destroys an OILRuler object 
********************************************************************************************/

OILRuler::~OILRuler()
{
}
 

/*********************************************************************************************
>	afx_msg void OILRuler::OnPaint()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Purpose:	Paints the client area of the Ruler which isn't covered by child windows,
				in response to a WM_PAINT message.
/**********************************************************************************************/ 

void OILRuler::OnPaint()
{
	// prevent recursive redraws when an error ocurs
	if (CCamApp::DisableSys)
		return;

	// get and validate a few pointers before we start
	DocView* pDocView=pParentWnd->GetDocViewPtr();
	if (pDocView==NULL)
	{
		ERROR2RAW("OILRuler::OnPaint() - pDocView==NULL");
		return;
	}
	CWnd* pRenderWnd=pDocView->GetRenderWindow();
	if (pRenderWnd==NULL)
	{
		ERROR2RAW("OILRuler::OnPaint() - pRenderWnd==NULL");
		return;
	}

	// get the update region in docview OilCoords
	WinRect RulerUpdateRect;
	GetUpdateRect(&RulerUpdateRect);
	OilRect UpdateOilRect = ClientToOil(pDocView,RulerUpdateRect);

	// cache the offsets from ruler to doc window relative coords
	// note only x valid when painting horizontal ruler and only y valid for painting vertical ruler
	WinRect UpdateRect = ClientToOtherClient(pRenderWnd,RulerUpdateRect);
	ERROR3IF(UpdateRect.Width()!=RulerUpdateRect.Width() || UpdateRect.Height()!=RulerUpdateRect.Height(),"OILRuler::OnPaint() - ruler and doc window have incompatible coordinate spaces");
	RulerToDocOffset = CSize(UpdateRect.left-RulerUpdateRect.left, UpdateRect.bottom-RulerUpdateRect.bottom);

	// Create a painting DC
	CPaintDC dc(this);
	dc.SetROP2(R2_COPYPEN);

	// read the necessary colours
	COLORREF BackCol  = ::GetSysColor(COLOR_BTNFACE);
	COLORREF TopLeft  = ::GetSysColor(COLOR_BTNHIGHLIGHT);
	COLORREF BotRight = ::GetSysColor(COLOR_BTNSHADOW);
	COLORREF TextCol  = ::GetSysColor(COLOR_BTNTEXT);

  	// get info about client rect
	CRect ClientRect;
	GetClientRect(&ClientRect);
	INT32 left   = ClientRect.left;
	INT32 top    = ClientRect.top;
	INT32 width  = ClientRect.Width();
	INT32 height = ClientRect.Height();
	INT32 right  = left + width;
	INT32 bottom = top  + height;
	if (IsHorizontal() && height!=RenderWidth || !IsHorizontal() && width!=RenderWidth)
		ERROR3_PF(("OILRuler::OnPaint() - ruler client size (%d,%d) and RenderWidth (%d) incompatible!",width,height,RenderWidth));

	// render the background
	PatB(&dc, left,top, width,height, BackCol);
	
	// paint borders to be over written by graticules
	if(IsHorizontal())
	{
		PatB(&dc, left+1,top+1,  width-2,1,  TopLeft);	// horizontal ruler top    hi-light
		PatB(&dc, left+1,bottom, width-1,-1, BotRight);	// horizontal ruler bottom lo-light
	}
	else
	{
		PatB(&dc, left+1,top+1,  1,height-2, TopLeft);	// vertical ruler left  hi-light
		PatB(&dc,  right,top+1, -1,height-1, BotRight);	// vertical ruler right lo-light
	}
	
	// get the kernel to render the ruler
	if (pKernelRuler)
	{
		// store pointers to CDC and DocView required for redraw
		pPaintDC      = &dc;
		pPaintDocView = pDocView;

		// set up DC objects required to draw the ruler
		CPen PaintPen(PS_SOLID,0,TextCol);
		CPen*    pOldPaintPen    = pPaintDC->SelectObject(&PaintPen);
		INT32      OldBkMode       = pPaintDC->SetBkMode(TRANSPARENT);
		COLORREF OldPaintTextCol = pPaintDC->SetTextColor(TextCol);
		HGDIOBJ  pOldPaintFont   = pPaintDC->SelectObject(FontFactory::GetFont(STOCKFONT_RULERS));
		if (pOldPaintPen==NULL || pOldPaintFont==NULL)
		{
			ERROR2RAW("OILRuler::OnPaint() - pPaintDC->SelectObject() failed");
			return;
		}

		// draw the ruler
		pKernelRuler->Redraw(&UpdateOilRect);

		// restore DC
		HGDIOBJ  pTempFont   = pPaintDC->SelectObject(pOldPaintFont);
		COLORREF TempTextCol = pPaintDC->SetTextColor(OldPaintTextCol);
		INT32      TempBkMode  = pPaintDC->SetBkMode(OldBkMode);
		CPen*    pTempPen    = pPaintDC->SelectObject(pOldPaintPen);
		if (pTempPen==NULL || pTempFont==NULL)
		{
			ERROR2RAW("OILRuler::OnPaint() - pPaintDC->SelectObject() failed");
			return;
		}

		// ensure pointers to DC and DocView are reset
		pPaintDC      = NULL;
		pPaintDocView = NULL;
	}

	// paint the bits of the border that need to cover the ruler rendering
	if(IsHorizontal())
	{
		PatB(&dc,   left,top,    1,height,   BackCol);	// horizontal ruler light grey left edge
		PatB(&dc, left+1,top+1,  1,height-2, TopLeft);	// horizontal ruler left  hi-light
		PatB(&dc,  right,top+1, -1,height-1, BotRight);	// horizontal ruler right lo-light
	}
	else
	{
		PatB(&dc,   left,top,    width-1,1,  BackCol);	// vertical ruler top light grey top edge   
		PatB(&dc, left+1,top+1,  width-2,1,  TopLeft);	// vertical ruler top    hi-light
		PatB(&dc, left+1,bottom, width-1,-1, BotRight);	// vertical ruler bottom lo-light
	}
}


/*********************************************************************************************
>	BOOL OILRuler::DrawMinorGraticule(OilCoord GratOilPos, INT32 ExtraSize=0)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/9/95
	Inputs:		GratOilPos - position of graticule on ruler in OilCoords
							 only x used for horizontal ruler (and y for vrtical)
				ExtraSize  - 0,1,2 extra number of pixels to add onto graticule size
	Returns:	FALSE if fails
	Purpose:	Draw a minor graticule on the horizontal/vertical ruler
	Also see:	DrawMajorGraticule()
**********************************************************************************************/ 

BOOL OILRuler::DrawMinorGraticule(OilCoord GratOilPos, INT32 ExtraSize)
{
	ERROR2IF(pPaintDC     ==NULL,FALSE,"OILRuler::DrawMinorGraticule() - pPaintDC==NULL");
	ERROR2IF(pPaintDocView==NULL,FALSE,"OILRuler::DrawMinorGraticule() - pPaintDocView==NULL");

	// convert to ruler relative win coords
	WinCoord GratWinPos=GratOilPos.ToWin(pPaintDocView);
	GratWinPos -= RulerToDocOffset;

	// determine the size and orientation of the graticule
	INT32 MinorGratLen = RenderWidth/6 + ExtraSize;
	WinRect GratWinRect;
	if (IsHorizontal())
		GratWinRect = WinRect(GratWinPos.x, RenderWidth-MinorGratLen, GratWinPos.x+1, RenderWidth);
	else
		GratWinRect = WinRect(RenderWidth-MinorGratLen, GratWinPos.y-1, RenderWidth, GratWinPos.y);

	// draw the graticule
	BOOL ok=(pPaintDC->Rectangle(&GratWinRect)!=0);
	ERROR2IF(!ok,FALSE,"OILRuler::DrawMinorGraticule() - pPaintDC->Rectangle() failed");

	return TRUE;
}


/********************************************************************************************
>	afx_msg void OILRuler::OnLButtonDown(UINT32 nFlags, CPoint point);

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		nFlags - 
				point  -
	Purpose:	process LButtonDoown event 
********************************************************************************************/
                    
void OILRuler::OnLButtonDown(UINT32 nFlags, CPoint point)
{
	StartDrag(nFlags,point);
}


/********************************************************************************************
>	void OILRuler::OnRButtonUp(UINT32 nFlags, CPoint point);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
	Inputs:		nFlags - 
				point  -
	Purpose:	Handle left button up events - pop-up context sensitive menu
********************************************************************************************/
                    
void OILRuler::OnRButtonUp(UINT32 nFlags, CPoint point)
{
	// get a few pointers - no error checking yet!
	DocView* pDocView = pParentWnd->GetDocViewPtr();
	Spread*  pSpread  = pKernelRuler->GetpRulerPair()->GetpSpread();
	if (pDocView==NULL || pSpread==NULL)
	{
		ERROR3("OILRuler::OnLButtonUp() - pDocView==NULL || pSpread==NULL");
		return;
	}

	// convert to spread coords
	OilCoord MouseOilPos    = ClientToOil(pDocView,point);
	DocCoord MouseSpreadPos = MouseOilPos.ToDoc(pSpread,pDocView).ToSpread(pSpread,pDocView);
	CSnap SnapObject(pDocView,pSpread);
	SnapObject.Snap(&MouseSpreadPos,FALSE);

	// precharge static buffer with all the info to insert a new guide
	// at the click point on the off chance this is the option the user chooses from
	// the menu - as it is not poosible to pass params via the pop-up menu system
	if (IsHorizontal())
		GuidelinePropDlg::SetNewGuidelineParams(GUIDELINE_VERT, MouseSpreadPos.x);
	else
		GuidelinePropDlg::SetNewGuidelineParams(GUIDELINE_HORZ, MouseSpreadPos.y);
//	// insert immediate (witout dialog)
//	if (IsHorizontal())
//		OpNewGuideline::SetNewGuidelineParam(GUIDELINE_VERT, MouseSpreadPos.x);
//	else
//		OpNewGuideline::SetNewGuidelineParam(GUIDELINE_HORZ, MouseSpreadPos.y);

	RulerContextMenu* pRulerMenu = new RulerContextMenu;
	pRulerMenu->Show();
}


/********************************************************************************************
>	void OILRuler::OnLButtonDblClk(UINT32 nFlags, CPoint point);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95
	Inputs:		nFlags - 
				point  - 
	Purpose:	handle left button double clicks
********************************************************************************************/
                    
void OILRuler::OnLButtonDblClk(UINT32 nFlags, CPoint point)
{
	// get a few ponters
	DocView* pDocView = pParentWnd->GetDocViewPtr();
	Spread*  pSpread  = pKernelRuler->GetpRulerPair()->GetpSpread();
	if (pDocView==NULL || pSpread==NULL)
	{
		ERROR3("OILRuler::OnLButtonDblClk() - pDocView==NULL || pSpread==NULL");
		return;
	}

	OilCoord MouseOilPos    = ClientToOil(pDocView,point);
	DocCoord MouseSpreadPos = MouseOilPos.ToDoc(pSpread,pDocView).ToSpread(pSpread,pDocView);
	CSnap SnapObject(pDocView,pSpread);
	SnapObject.Snap(&MouseSpreadPos,FALSE);

	if (IsHorizontal())
		OpNewGuideline::SetNewGuidelineParam(GUIDELINE_VERT, MouseSpreadPos.x);
	else
		OpNewGuideline::SetNewGuidelineParam(GUIDELINE_HORZ, MouseSpreadPos.y);

 	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NEWGUIDELINE2);
	if (pOpDesc!=NULL)
		pOpDesc->Invoke();
	else
		ERROR3("OILRuler::OnLButtonDblClk() - FindOpDescriptor(OPTOKEN_NEWGUIDELINE) failed");
}


/********************************************************************************************
>	void OILRuler::OnMouseMove(UINT32 nFlags, CPoint MousePos);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/95
	Inputs:		nFlags - 
				point  - 
	Purpose:	handle mouse moves over the ruler
********************************************************************************************/
                    
void OILRuler::OnMouseMove(UINT32 nFlags, CPoint MousePos)
{
	static CPoint OldMousePos = CPoint(0,0);
	if (MousePos==OldMousePos)
		return;

	StatusLine* pStatusLine=GetApplication()->GetpStatusLine();
	if (pStatusLine)
	{
		String_256 Text("");
		GetStatusLineText(&Text,MousePos,GetSafeHwnd());
		pStatusLine->UpdateText(&Text,STATUSLINE_SELDESC_STATBAR);
	}

	OldMousePos = MousePos;
}


/******************************************************************************
>	BOOL OILRuler::GetStatusLineText(String_256* pText, CPoint MousePos, HWND hWnd)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/95
	Inputs:		MousePos - position in window (not used)
				hWnd     - handle of window
	Outputs:	pText    -
	Returns:	TRUE if ptext hold valid text, else FALSE if not over a pane
	Purpose:	if over a ruler return status line help
******************************************************************************/

BOOL OILRuler::GetStatusLineText(String_256* pText, CPoint MousePos, HWND hWnd)
{
	ERROR2IF(pText==NULL,FALSE,"OILRuler::GetStatusLineText() - pText==NULL");

	if (GetSafeHwnd()!=hWnd)
		return FALSE;

	UINT32 StatusHelpID = IsHorizontal() ? _R(IDS_HRULER_SH) : _R(IDS_VRULER_SH);
	return (pText->Load(StatusHelpID)!=0);
}


/********************************************************************************************
>	BOOL OILRuler::GetMinGraticuleSpacing(OilRect* pSpacing, DocView* pDocView)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/9/95
	Inputs:		pDocView - 
	Outputs:	pSpacing - 
	Returns:	minimum graticule spacing in OilCords
********************************************************************************************/
                    
BOOL OILRuler::GetMinGraticuleSpacing(OilRect* pSpacing, DocView* pDocView)
{
	ERROR2IF(pSpacing==NULL,FALSE,"OILRuler::GetMinGraticuleSpacing() - pSpacing==NULL");
	ERROR2IF(pDocView==NULL,FALSE,"OILRuler::GetMinGraticuleSpacing() - pDocView==NULL");

	WinCoord WinSpacing(MIN_PIXEL_GRATICULE_SPACING_X,-MIN_PIXEL_GRATICULE_SPACING_Y);
	pSpacing->lo=OilCoord(0,0);
	pSpacing->hi=WinSpacing.ToOil(pDocView);
	return TRUE;
}


/********************************************************************************************
>	static BOOL OILRuler::GetTextSize(OilRect* pTextSize, StringBase* pText, DocView* pDocView)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/9/95
	Inputs:		pText    -
				pDocView - 
	Outputs:	pTextSize - x,y size of text in OilCoords
	Returns:	FALSE if fails
********************************************************************************************/
                    
BOOL OILRuler::GetTextSize(OilRect* pTextSize, StringBase* pText, DocView* pDocView)
{
	ERROR2IF(pTextSize==NULL,FALSE,"OILRuler::GetTextSize() - pTextSize==NULL");
	ERROR2IF( pDocView==NULL,FALSE,"OILRuler::GetTextSize() - pDocView==NULL");

	WinRect WinTextSize;
	BOOL ok=GetTextSize(&WinTextSize,pText);
	if (!ok)
		return FALSE;

	*pTextSize = WinTextSize.ToOil(pDocView);
	return TRUE;
}


/********************************************************************************************
>	static BOOL OILRuler::GetTextSize(WinRect* pTextSize, StringBase* pText)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/9/95
	Inputs:		pText     -
	Outputs:	pTextSize - x,y size of text in WinCoords
	Returns:	FALSE if fails
********************************************************************************************/
                    
BOOL OILRuler::GetTextSize(WinRect* pTextSize, StringBase* pText)
{
	ERROR2IF(pTextSize==NULL,FALSE,"OILRuler::GetTextSize() - pTextSize==NULL");
	ERROR2IF(    pText==NULL,FALSE,"OILRuler::GetTextSize() - pText==NULL");

	TRY
	{
		// create a screen compatible DC to find the size of the text
		CDC dc;		// may throw CResourceException exception
		if (dc.CreateCompatibleDC(NULL)==FALSE)
			ERROR2(FALSE,"OILRuler::GetTextSize() - CreateCompatibleDC() failed");

		HGDIOBJ pOldFont = dc.SelectObject(FontFactory::GetFont(STOCKFONT_RULERS));
		ERROR2IF(pOldFont==NULL,FALSE,"OILRuler::GetTextSize() - failed to select font");

		TCHAR* pchars   = (TCHAR*)*pText;
		INT32   TextLen  = pText->Length();
		CSize  TextSize = dc.GetTextExtent(pchars,TextLen);
		pTextSize->left   = 0;
		pTextSize->top    = 0;
		pTextSize->right  = TextSize.cx;
		pTextSize->bottom = TextSize.cy;

		HGDIOBJ pFont = dc.SelectObject(pOldFont);
		ERROR2IF(pFont==NULL,FALSE,"OILRuler::GetTextSize() - failed to reselct old font");
	}
	CATCH (CResourceException, e)
	{
		ERROR2(FALSE,"OILRuler::GetTextSize() - failed to create dc");
	}
	END_CATCH

	return TRUE;
}


/********************************************************************************************
>	WinRect OILRuler::ClientToOtherClient(CWnd* pOtherCWnd, WinRect WR)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95
	Inputs:		pOtherCWnd - window to which to make rect relative to
				WR         - win rect to convert
	Returns:	WinRect    - converted win rect
	Purpose:	Convert rect relative to this window to rect relative to another window
	Note:		Nice if this was in a parent CCWnd class
********************************************************************************************/

WinRect OILRuler::ClientToOtherClient(CWnd* pOtherCWnd, WinRect WR)
{
	ClientToScreen(&WR);
	pOtherCWnd->ScreenToClient(&WR);
	return WR;
}


/********************************************************************************************
>	CPoint OILRuler::ClientToOtherClient(CWnd* pOtherCWnd, CPoint point)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95
	Inputs:		pOtherCWnd - window to which to make rect relative to
				point      - win coord to convert
	Returns:	WinCoord   - converted win coord
	Purpose:	Convert coord relative to this window to coord relative to another window
	Note:		Nice if this was in a parent CCWnd class
********************************************************************************************/

CPoint OILRuler::ClientToOtherClient(CWnd* pOtherCWnd, CPoint point)
{
	ClientToScreen(&point);
	pOtherCWnd->ScreenToClient(&point);
	return point;
}


/********************************************************************************************
>	OilRect OILRuler::ClientToOil(DocView* pDocView, WinRect WR)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95
	Inputs:		pDocView    - associated doc view
				WR          - win rect to convert
	Returns:	OilRect     - converted win rect
	Purpose:	Convert a (client) rect relative to this window to a document Oil rect
	Note:		Nice if this was in CWnd class but this is MFC, could do with a CCWnd class
********************************************************************************************/

OilRect OILRuler::ClientToOil(DocView* pDocView, WinRect WR)
{
	CWnd* pRenderWnd = pDocView->GetRenderWindow();
	ERROR2IF(pRenderWnd==NULL,OilRect(OilCoord(0,0),OilCoord(0,0)),"OILRuler::ClientToOil() - pRenderWnd==NULL");
	return ClientToOtherClient(pRenderWnd,WR).ToOil(pDocView);
}


/********************************************************************************************
>	OilCoord OILRuler::ClientToOil(DocView* pDocView, CPoint point)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95
	Inputs:		pDocView    - associated doc view
				point       - win coord to convert
	Returns:	OilCoord    - converted win coord
	Purpose:	Convert a (client) coord relative to this window to a document Oil coord
	Note:		Nice if this was in CWnd class but this is MFC, could do with a CCWnd class
********************************************************************************************/

OilCoord OILRuler::ClientToOil(DocView* pDocView, CPoint point)
{
	CWnd* pRenderWnd = pDocView->GetRenderWindow();
	ERROR2IF(pRenderWnd==NULL,OilCoord(0,0),"OILRuler::ClientToOil() - pRenderWnd==NULL");
	CPoint DocPoint = ClientToOtherClient(pRenderWnd,point);
	return WinCoord(DocPoint.x,DocPoint.y).ToOil(pDocView);
}


/********************************************************************************************
>	static void OILRuler::Deinit()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/95
	Purpose:	return memory 'new'd by the Init function
********************************************************************************************/

void OILRuler::Deinit()
{
	if(FontName != NULL)
	{
		delete FontName;
		FontName = NULL;
	}
}

/********************************************************************************************
>	static BOOL OILRuler::Init()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/9/95
	Returns:	FALSE if fails
	Purpose:	read the font name and size for the ruler from the .ini file
				and use this to determine the size of the rulers
********************************************************************************************/

BOOL OILRuler::Init()
{
	if(FontName == NULL)
		FontName = new String_256(_R(IDS_OILRULER_SMALL_FONTS));

	BOOL ok=GetApplication()->DeclareSection("Rulers",2);
	if (ok && FontName != NULL) ok=GetApplication()->DeclarePref("Rulers", "FontName", FontName);
	if (ok) ok=GetApplication()->DeclarePref("Rulers", "FontSize", &FontSize, 2,72);
	if (!ok)
		return FALSE;
	ERROR2IF(FontName == NULL,FALSE,"OILRuler::SetRenderWidth() - couldn't create a string_256 - eek !");
	ERROR2IF(FontName->Length()==0,FALSE,"OILRuler::SetRenderWidth() - invalid FontName in .ini file");

	WinRect WinTextSize;
	ok=GetTextSize(&WinTextSize,&String_8(_R(IDS_OILRULER_DEFAULT_TEXT_SIZE)/*"8"*/));
	if (!ok)
		return FALSE;

	CharHeight  = WinTextSize.Height();
	CharWidth   = WinTextSize.Width();
	RenderWidth = CharWidth*3 + 4;

	return TRUE;
}


/*********************************************************************************************
>	BOOL OILRuler::PaintMouseFollower(OilCoord OilPos, DocView* pDocView,
									  MouseFollowerRenderType RenderType)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/95
	Inputs:		OilPos     - position of mouse for which to draw x/y follower
				pDocView   - 
				RenderType - RenderOn/RenderOff (not yet used - just inverts previous state)
	Returns:	FALSE if fails
	Purpose:	interface to DrawMouseFollower()
**********************************************************************************************/

BOOL OILRuler::PaintMouseFollower(OilCoord OilPos, DocView* pDocView,
								  MouseFollowerRenderType RenderType)
{
	// get hold of a dc (if in middle of OnPaint(), pPaintDC should be valid)
	CDC* pDC=pPaintDC;
	BOOL LocalDC = (pDC==NULL);
	if (LocalDC)
		pDC=GetDC();

	BOOL ok = DrawMouseFollower(OilPos,pDocView,RenderType,pDC);

	if (LocalDC)
		ReleaseDC(pDC);

	return ok;
}

/*********************************************************************************************
>	BOOL OILRuler::DrawMouseFollower(OilCoord OilPos, DocView* pDocView,
									 MouseFollowerRenderType RenderType, CDC* pDC)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/95
	Inputs:		OilPos     - position of mouse for which to draw x/y follower
				pDocView   - 
				RenderType - RenderOn/RenderOff (not yet used - just inverts previous state)
				pDC        - 
	Returns:	FALSE if fails
	Purpose:	Draw an horizontal or vetical mouse follower
**********************************************************************************************/

BOOL OILRuler::DrawMouseFollower(OilCoord OilPos, DocView* pDocView,
								 MouseFollowerRenderType RenderType, CDC* pDC)
{
	ERROR2IF(pDocView==NULL,FALSE,"OILRuler::DrawMouseFollower() - pDocView==NULL");
	ERROR2IF(     pDC==NULL,FALSE,"OILRuler::DrawMouseFollower() - pDC==NULL");

	// convert follower position to a rectangle to render
	WinCoord WinPos = OilPos.ToWin(pDocView);
	WinRect FollowerWinRect;
	if(IsHorizontal())
		FollowerWinRect = WinRect(WinPos.x,0,WinPos.x,RenderWidth);
	else
		FollowerWinRect = WinRect(0,WinPos.y-1,RenderWidth,WinPos.y-1);

	// set up dc
	COLORREF FollowerColor = RGB(0,255,0);
	CPen     pen(PS_SOLID,0,FollowerColor);
	CPen*    pOldPen   = pDC->SelectObject(&pen);
	INT32      OldROP2   = pDC->SetROP2(R2_NOT);
	ERROR2IF(pOldPen==NULL,FALSE,"OILRuler::DrawMouseFollower() - pDC->SelectObject() failed");

	// render the follower
	pDC->MoveTo(FollowerWinRect.left, FollowerWinRect.top);
	pDC->LineTo(FollowerWinRect.right,FollowerWinRect.bottom);

	// restore the dc
	INT32     TempROP2   = pDC->SetROP2(OldROP2);
	CPen*   pTempPen   = pDC->SelectObject(pOldPen);
	ERROR2IF(pTempPen==NULL,FALSE,"OILRuler::DrawMouseFollower() - pDC->SelectObject() failed");

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// 				OILHorizontalRuler
/////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(OILHorizontalRuler ,OILRuler)

BEGIN_MESSAGE_MAP( OILHorizontalRuler, OILRuler )
	//{{AFX_MSG_MAP( OILHorizontalRuler )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP( )


/********************************************************************************************
>	OILHorizontalRuler::OILHorizontalRuler()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Purpose:	Creates an OILHorizontalRuler object.
********************************************************************************************/
                    
OILHorizontalRuler::OILHorizontalRuler()
{
	pLegend = NULL; 
}


/********************************************************************************************
>	OILHorizontalRuler::~OILHorizontalRuler()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Purpose:	destructor for OILHorizontalRuler object - delete the legend label;.
********************************************************************************************/
                    
OILHorizontalRuler::~OILHorizontalRuler()
{
	if(pLegend)
	{
		delete pLegend;
		pLegend=NULL;
	}	 
}


/********************************************************************************************
>	virtual BOOL OILHorizontalRuler::PostCreate()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Returns:	FALSE if fails
	Purpose:	Called from the base class  Create() to create the legend label.
********************************************************************************************/
                    
BOOL OILHorizontalRuler::PostCreate()
{ 
	pLegend = new LegendLabel;
	if(!pLegend)
		return FALSE;

	pLegend->Create(this);

	return TRUE;
}


/********************************************************************************************
>	virtual BOOL OILHorizontalRuler::ScrollRuler()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Purpose:	Scroll the ruler
********************************************************************************************/
                    
void OILHorizontalRuler::ScrollRuler(INT32 amount)
{ 
		
	CRect ClipRect;
	GetClientRect(&ClipRect);
	ClipRect.left+=2;
	ClipRect.right--;

	ScrollWindow(amount,0,&ClipRect,&ClipRect);
	UpdateWindow();
		
}


/********************************************************************************************
>	virtual BOOL OILHorizontalRuler::StartDrag(UINT32 nFlags,CPoint point)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Returns:	FALSE if fails
	Purpose:	StartDrag function .
********************************************************************************************/
                    
BOOL OILHorizontalRuler::StartDrag(UINT32 nFlags,CPoint point)
{ 
	NewGuidelineParam.Method 	= GUIDELINEOPMETHOD_NEW_DRAG;
	NewGuidelineParam.Type 		= GUIDELINE_HORZ;

	String_256 OpToken(OPTOKEN_GUIDELINE);

	if (pParentWnd != NULL)
		pParentWnd->InvokeDragOp(&OpToken,&NewGuidelineParam,nFlags,point);

	return TRUE;
}


/*********************************************************************************************
>	virtual void OILHorizontalRuler::CalcPosFromParentClient(LPRECT lpRect)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		A long pointer to a rectangle which describes the dimensions of the client
				area of the window containing the scroller.
	Outputs:	Changes lpRect to the position a horizontal ruler would occupy
				normally, ie. along the top of the owning window.
	Purpose:	Called by CCamView when its size changes, so it knows where to reposition
				the CHorzScroller.
**********************************************************************************************/ 

void OILHorizontalRuler::CalcPosFromParentClient(LPRECT lpRect)
{
	CRect parentRect;
	GetParent()->GetClientRect(&parentRect);
	if(RULER_BORDERS)
	{
	  parentRect.top --;
	  parentRect.left -=2;
	}
	
	lpRect->left   = parentRect.left + RulerWidth;
	lpRect->top    = parentRect.top;
	lpRect->right  = parentRect.right+1;
	lpRect->bottom = parentRect.top  + RulerWidth;
	
}


/*********************************************************************************************
>	BOOL OILHorizontalRuler::DrawMajorGraticule(OilCoord GratOilPos, StringBase* pGratLabel)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/9/95
	Inputs:		GratOilPos - position of graticule on ruler in OilCoords (y ignored)
				pGratLabel - label for the graticule
	Returns:	FALSE if fails
	Purpose:	Draw a major graticule on the horizontal ruler
	Note:		Must be called from within the scope of the OILRuler::OnPaint()
				as it is this function which is responsible for creating a DC
				and selecting the correct objects into it
**********************************************************************************************/ 

BOOL OILHorizontalRuler::DrawMajorGraticule(OilCoord GratOilPos, StringBase* pGratLabel)
{
	ERROR2IF(pGratLabel   ==NULL,FALSE,"OILHorizontalRuler::DrawMajorGraticule() - pGrtaLabel==NULL");
	ERROR2IF(pPaintDC     ==NULL,FALSE,"OILHorizontalRuler::DrawMajorGraticule() - pPaintDC==NULL");
	ERROR2IF(pPaintDocView==NULL,FALSE,"OILHorizontalRuler::DrawMajorGraticule() - pPaintDocView==NULL");

	// convert to ruler relative win coords
	WinCoord GratWinPos = GratOilPos.ToWin(pPaintDocView);
	GratWinPos -= RulerToDocOffset;

	// draw the major graticule in 2 halves with a gap for the text between
	INT32 GratLen = MAJOR_GRAT_LEN;
	WinRect GratWinRect(GratWinPos.x, RenderWidth-MAJOR_GRAT_LEN, GratWinPos.x+1, RenderWidth);
	BOOL ok=(pPaintDC->Rectangle(&GratWinRect)!=0);
	ERROR2IF(!ok,FALSE,"OILHorizontalRuler::DrawMajorGraticule() - pPaintDC->Rectangle() failed");

	// draw the graticule label (centred about grat pos)
	TCHAR* pText    = (TCHAR*)*pGratLabel;
	INT32   TextLen  = pGratLabel->Length();
	CSize  TextSize = pPaintDC->GetTextExtent(pText,TextLen);	// real text width not necessarily == CharWidth*LextLen with proportional fonts
	INT32   Textx    = GratWinPos.x-TextSize.cx/2;				// centre text about grat (+1 for gap to right - left inclusive)
	INT32   Texty    = RenderWidth-(GratLen+TextSize.cy);
	ok=(pPaintDC->TextOut(Textx,Texty,pText,TextLen)!=0);		// actual digits drawn 1 pixel beyond, and 2 pixels below this point
	ERROR2IF(!ok,FALSE,"OILHorizontalRuler::DrawMajorGraticule() - pPaintDC->TextOut() failed");

	return TRUE;
}


/*********************************************************************************************
>	BOOL OILHorizontalRuler::PositionLegend()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/9/95
	Returns:	FALSE if fails
	Purpose:	Ask the Legend Label to position itself
**********************************************************************************************/ 

BOOL OILHorizontalRuler::PositionLegend()
{
//	ERROR2IF(pLegend==NULL,FALSE,"OILHorizontalRuler::PositionLegend() - pLegend==NULL");
	if (pLegend==NULL)
	{
		TRACE( _T("This should never be NULL but is regularly on Win95 implying the creation order is wrong\n"));
		return FALSE;
	}

	return pLegend->PositionLegend();
}


/*********************************************************************************************
>	BOOL OILHorizontalRuler::SetLegendText(StringBase* pText)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/9/95
	Returns:	FALSE if fails
	Purpose:	Ask the Legend Label to position itself
**********************************************************************************************/ 

BOOL OILHorizontalRuler::SetLegendText(StringBase* pText)
{
//	ERROR2IF(pLegend==NULL,FALSE,"OILHorizontalRuler::SetLegendText() - pLegend==NULL");
	if (pLegend==NULL)
	{
		TRACE( _T("This should never be NULL but is regularly on Win95 implying the creation order is wrong\n"));
		return FALSE;
	}

	return pLegend->SetLegendText(pText);
}


/////////////////////////////////////////////////////////////////////////////////////////////
// 				OILVerticalRuler
/////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(OILVerticalRuler ,OILRuler)

BEGIN_MESSAGE_MAP( OILVerticalRuler, OILRuler )
	//{{AFX_MSG_MAP( OILVerticalRuler )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP( )


/********************************************************************************************
>	OILVerticalRuler::OILVerticalRuler()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Purpose:	Creates an OILVerticalRuler object.
********************************************************************************************/
                    
OILVerticalRuler::OILVerticalRuler()
{ 
}


/********************************************************************************************
>	virtual BOOL OILVerticalRuler::PostCreate()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Returns:	FALSE if fails
	Purpose:	Called from the base class  Create()- does nothing at the moment.
********************************************************************************************/
                    
BOOL OILVerticalRuler::PostCreate()
{ 
	return TRUE;
}


/********************************************************************************************
>	virtual BOOL OILVerticalRuler::ScrollRuler()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Returns:	-
	Purpose:	Scroll the ruler
********************************************************************************************/
                    
void OILVerticalRuler::ScrollRuler(INT32 amount)
{ 
	CRect ClipRect;
	GetClientRect(&ClipRect);
	ClipRect.top +=2;
	ClipRect.bottom --;

	ScrollWindow(0,amount,&ClipRect,&ClipRect);
	UpdateWindow();
}


/********************************************************************************************
>	virtual BOOL OILVerticalRuler::StartDrag(UINT32 nFlags,CPoint point)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Returns:	FALSE if fails
	Purpose:	Sends a start vertical guideline drag message.
********************************************************************************************/
                    
BOOL OILVerticalRuler::StartDrag(UINT32 nFlags,CPoint point)
{ 
	NewGuidelineParam.Method 	= GUIDELINEOPMETHOD_NEW_DRAG;
	NewGuidelineParam.Type 		= GUIDELINE_VERT;

	String_256 OpToken(OPTOKEN_GUIDELINE);

	if (pParentWnd != NULL)
		pParentWnd->InvokeDragOp(&OpToken,&NewGuidelineParam,nFlags,point);

	return TRUE;
}


/*********************************************************************************************
>	virtual void OILVerticalRuler::CalcPosFromParentClient(LPRECT lpRect)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		A long pointer to a rectangle which describes the dimensions of the client
				area of the window containing the scroller.
	Outputs:	Changes lpRect to the position a vretical rulerr would occupy
				normally, ie. along the left side of the owning window.
	Purpose:	Called by CCamView when its size changes, so it knows where to reposition
				the CVertical ruler.
**********************************************************************************************/ 

void OILVerticalRuler::CalcPosFromParentClient(LPRECT lpRect)
{
	CRect parentRect;
	GetParent()->GetClientRect(&parentRect);
	if(RULER_BORDERS)
	{
	  parentRect.top -=2;
	  parentRect.left --;
	}
	lpRect->left   = parentRect.left;
	lpRect->top    = parentRect.top  + RulerWidth;
	lpRect->right  = parentRect.left + RulerWidth;
	lpRect->bottom = parentRect.bottom+1;
}


/*********************************************************************************************
>	BOOL OILVerticalRuler::DrawMajorGraticule(OilCoord GratOilPos, StringBase* pGratLabel)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/9/95
	Inputs:		GratOilPos - position of graticule on ruler in OilCoords (y ignored)
				pGratLabel - label for the graticule
	Returns:	FALSE if fails
	Purpose:	Draw a major graticule on the horizontal ruler
	Note:		Must be called from within the scope of the OILRuler::OnPaint()
				as it is this function which is responsible for creating a DC
				and selecting the correct objects into it
**********************************************************************************************/ 

BOOL OILVerticalRuler::DrawMajorGraticule(OilCoord GratOilPos, StringBase* pGratLabel)
{
	ERROR2IF(pGratLabel   ==NULL,FALSE,"OILVerticalRuler::DrawMajorGraticule() - pGrtaLabel==NULL");
	ERROR2IF(pPaintDC     ==NULL,FALSE,"OILVerticalRuler::DrawMajorGraticule() - pPaintDC==NULL");
	ERROR2IF(pPaintDocView==NULL,FALSE,"OILVerticalRuler::DrawMajorGraticule() - pPaintDocView==NULL");

	// convert to ruler relative win coords
	WinCoord GratWinPos = GratOilPos.ToWin(pPaintDocView);
	GratWinPos -= RulerToDocOffset;

	// get width of gap for text
	INT32 TextLen  = pGratLabel->Length();
	INT32 GapWidth = CharWidth*max(2,TextLen)+1;		// +1 for gap to right (left already included)

	// draw the major graticule in 2 halves with a gap for the text between
	INT32 GratLen = min(MAJOR_GRAT_LEN,RenderWidth-(GapWidth+1));
	WinRect GratWinRect(RenderWidth-GratLen, GratWinPos.y-1, RenderWidth, GratWinPos.y);
	BOOL ok=(pPaintDC->Rectangle(&GratWinRect)!=0);
	ERROR2IF(!ok,FALSE,"OILVerticalRuler::DrawMajorGraticule() - pPaintDC->Rectangle() failed");

	// draw the graticule label (centred about grat pos)
	TCHAR* pText     = (TCHAR*)*pGratLabel;
	CSize  TextSize  = pPaintDC->GetTextExtent(pText,TextLen);	// real text width not necessarily == CharWidth*LextLen with proportional fonts
	INT32   TextWidth = TextSize.cx+1;							// +1 for gap to right (left already included)
	INT32   Offset    = (GapWidth-TextWidth)/2;					// offset to centre text in text gap (if proportional font)
	INT32   Textx     = RenderWidth-GratLen-(TextWidth+Offset);
	INT32   Texty     = GratWinPos.y-TextSize.cy/2-1;
	ok=(pPaintDC->TextOut(Textx,Texty,pText,TextLen)!=0);		// actual digits drawn 1 pixel beyond, and 2 pixels below this point
	ERROR2IF(!ok,FALSE,"OILVerticalRuler::DrawMajorGraticule() - pPaintDC->TextOut() failed");

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//									 OriginGadget Class
/////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(OriginGadget ,CWnd)

BEGIN_MESSAGE_MAP(OriginGadget,CWnd)
	//{{AFX_MSG_MAP( OriginGadget )
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP( )


/********************************************************************************************
>	OriginGadget::OriginGadget()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Purpose:	Creates an OILDockingBar object.
********************************************************************************************/
                    
OriginGadget::OriginGadget()
{ 
	pGlyph = NULL;
	pGlyph = new CBitmap;
	if(pGlyph)
	{
		if(! pGlyph->LoadBitmap(_R(IDB_ORIGINBMP)))
		{	
			delete pGlyph;
			pGlyph = NULL;
		}
	}
}


/*********************************************************************************************
>	void OriginGadget::ShowGadget(BOOL show)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Inputs:		show - FALSE to hide gadet
	Purpose:	Shows or hides this gadget
**********************************************************************************************/ 

void OriginGadget::ShowGadget(BOOL show)
{
	ShowWindow(show ? SW_SHOWNA : SW_HIDE);
}

 
/********************************************************************************************
>	virtual OriginGadget::~OriginGadget()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Purpose:	Destroys an OriginGadget object 
********************************************************************************************/

OriginGadget::~OriginGadget()
{
	// kill the bitmap
	if (pGlyph)
		delete pGlyph;
}


/********************************************************************************************
>	void PlotMaskedBitmap(CDC* destDC, CBitmap* srcBitmap, INT32 x, INT32 y, INT32 width, INT32 height)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> and Chris
	Created:	14/3/94
	Inputs:		destDC    - DC into which the BMP will be plotted
				srcBitmap - NULL, or the source bitmap
				x, y, width, height - rectangle to plot into
	Purpose:	Plots the given bitmap (srcDC) into the given rectangle of the destination
				(destDC).
				if srcDC == NULL, then just fills the button with grey, else...
********************************************************************************************/

// Raster Ops, used to...

// Plot colour in the glyph bitmaps transparent (grey) regions
#define ROP_DSPDxax  0x00E20746L

// Plot colour in the glyph bitmaps non-transparent (coloured) regions
#define ROP_PSDPxax  0x00B8074AL



BOOL OriginGadget::PlotMaskedBitmap(CDC* destDC, CBitmap* srcBitmap, INT32 SrcOffset,
								INT32 xPlotOffset,INT32 yPlotOffset, INT32 width, INT32 height)
{
	
	ERROR2IF(srcBitmap == NULL,FALSE,"NULL Bitmap in PlotMaskedBitmap()");
	ERROR2IF(destDC == NULL,FALSE,"NULL DC in PlotMaskedBitmap()");

	// always fill the entire background with BTNFACE to make sure we don't
	// miss any pixels 

	PatB(destDC, 0, 0, width, height, GetSysColor(COLOR_BTNFACE));
	
	// create a screen dc
	CDC scrDC ;
	scrDC.CreateCompatibleDC(destDC);
	
	// and select the bitmap into it
	CBitmap* OldSrcBitmap = (CBitmap *) scrDC.SelectObject(srcBitmap);

	// create a mask for the button glyph... First a DC
	CDC hDCMono ;
	hDCMono.CreateCompatibleDC(NULL);

	// create a mono bitmap for the mask
	CBitmap hbmMono ; 
	hbmMono.CreateBitmap(width, height, 1, 1, NULL);
	
	CBitmap * oldmonobitmap = (CBitmap *)hDCMono.SelectObject(&hbmMono);

	// initalize the bitmapto white (all 1's)
	hDCMono.PatBlt(0, 0, width, height, WHITENESS);

	// plot all non-magenta  pixels in the glyph as black in our mask
	scrDC.SetBkColor( RGB(255,0,255));
	hDCMono.BitBlt(xPlotOffset, yPlotOffset, width, height, &scrDC,
			SrcOffset, 0, SRCCOPY);

	destDC->SetTextColor(0L);                  // 0's in mono -> 0 (for ROP)
	destDC->SetBkColor((COLORREF)0x00FFFFFFL); // 1's in mono -> 1

	// plot the glyph 
	destDC->BitBlt( xPlotOffset, yPlotOffset, width, height,
					&scrDC, SrcOffset, 0, SRCCOPY);
		
	// fill with the normal BTNFACE colour
	CBrush HighlightBrush ; 
	HighlightBrush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

	CBrush* hbrOld = (CBrush *) destDC->SelectObject(&HighlightBrush);
	if (hbrOld != NULL)
	{
		// draw highlight color where we have 0's in the mask
		destDC->BitBlt(0,0, width, height,  &hDCMono, 0, 0, ROP_DSPDxax);
		destDC->SelectObject(hbrOld);
	}
	
	// clean up DC's
	hDCMono.SelectObject(oldmonobitmap);
	scrDC.SelectObject(OldSrcBitmap);
	return TRUE;
}


/********************************************************************************************
>	afx_msg void OriginGadget::OnPaint()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Purpose:	Paints the client area of the Origin Gadget
				Mask plots the origin glyph
********************************************************************************************/

void OriginGadget::OnPaint()
{
	// prevent recursive redraws when an error ocurs
	if (CCamApp::DisableSys)
		return;

	// Create a painting DC.
	CPaintDC dc(this);
	dc.SetROP2(R2_COPYPEN);
	
	// read the necessary colours
	COLORREF BackCol  = ::GetSysColor(COLOR_BTNFACE);
	COLORREF TopLeft  = ::GetSysColor(COLOR_BTNHIGHLIGHT);
	COLORREF BotRight = ::GetSysColor(COLOR_BTNSHADOW);
  
  	// get info about client rect
	CRect ClientRect;
	GetClientRect(&ClientRect);
	INT32 left   = ClientRect.left;
	INT32 top    = ClientRect.top;
	INT32 width  = ClientRect.Width();
	INT32 height = ClientRect.Height();
	INT32 right  = left + width;
	INT32 bottom = top  + height;

	// plot the little orign bitmap in the centre of this window
	INT32 BitmapSize = 15;	
	INT32 offset     = (width-BitmapSize)/2;
	PlotMaskedBitmap(&dc,pGlyph,0,offset,offset,width,height);

	// paint the hi/lo lights
//	PatB(&dc,  left,top,    width-1,1,        TopLeft);		// top
//	PatB(&dc,  left,top,          1,height-1, TopLeft);		// left

	PatB(&dc,  left+1,top+1,    width-2,1,        TopLeft);		// top
	PatB(&dc,  left+1,top+1,          1,height-2, TopLeft);		// left
	PatB(&dc,  left+1,bottom,   width-1,-1,       BotRight);	// bottom
	PatB(&dc, right,top+1,         -1,height-1,   BotRight);	// right
}


/*********************************************************************************************
>	virtual void OriginGadget::CalcPosFromParentClient(LPRECT lpRect)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Inputs:		A long pointer to a rectangle which describes the dimensions of the client
				area of the window containing the gadget.
	Outputs:	Changes lpRect to the position a horizontal scroll bar would occupy
				normally, ie. along the bottom of the owning window.
	Purpose:	Called by CCamView when its size changes, so it knows where to reposition
				the OriginGadget.
**********************************************************************************************/ 

void OriginGadget::CalcPosFromParentClient(LPRECT lpRect)
{
	CRect parentRect;
	GetParent()->GetClientRect(&parentRect);
	if(RULER_BORDERS)
	{
	  parentRect.top --;
	  parentRect.left --;
	}
	lpRect->left   = parentRect.left ;
	lpRect->top    = parentRect.top;
	lpRect->right  = parentRect.left + OILRuler::GetWidth();
	lpRect->bottom = parentRect.top  + OILRuler::GetWidth();

	// we need to take into account camelots new floating toolbar ....

	DockBarType DockBarType;// = DOCKBAR_FLOAT;
	KernelBarPos * pKernelBarPos;

	// find out where the toolbar is docked
	GetMainFrame()->GetBarPosInfo(&(String_32 ("Toolbar")), &DockBarType, &pKernelBarPos);

	if (DockBarType == DOCKBAR_FLOAT)
	{
		RECT MainFrameWindowRect = *lpRect;
		//::GetWindowRect (GetSafeHwnd (), &MainFrameWindowRect);
		ClientToScreen (&MainFrameWindowRect);

		POINT ToolbarTL;
		ToolbarTL.x = pKernelBarPos->x;
		ToolbarTL.y = pKernelBarPos->y;

		if (PtInRect (&MainFrameWindowRect, ToolbarTL))
		{
			KernelBarPos NewToolBarPos = *pKernelBarPos;
			BaseBar* PtrToToolBar = BaseBar::GetPtrToToolBar ();
			
			INT32 dy = MainFrameWindowRect.bottom - ToolbarTL.y;
			INT32 dx = MainFrameWindowRect.right - ToolbarTL.x;

			NewToolBarPos.y += dy;
			NewToolBarPos.x += dx;

			if (PtrToToolBar)
			{
				PtrToToolBar->MoveBar ((WPARAM) DOCKBAR_FLOAT, (LPARAM) &NewToolBarPos);
			}
		}
	}
	
}


/********************************************************************************************
>	BOOL OriginGadget::Create(CWnd* pParentWindow)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		pParentWindow - 
	Returns:	FALSE if fails
	Purpose:	Creates an OriginGadget object.
********************************************************************************************/

BOOL OriginGadget::Create(CWnd* pParentWindow)
{ 
	BOOL CreatedWindow;
	ENSURE(pParentWindow,"DockingBar must have a Parent");

	pParentWnd = (ScreenCamView*) pParentWindow;

	CRect rect;			// Rectangle for Create
	rect.SetRectEmpty();
	DWORD style = WS_CHILD  | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE;
	if(RULER_BORDERS)
		style |=WS_BORDER;
	#ifndef WIN32
		DWORD
	#endif
	
	// create the HWND
	CreatedWindow = CWnd::Create(NULL,
						AfxRegisterWndClass(CS_DBLCLKS,0,0,0),
						style,
						rect,
						pParentWindow,
						0,
						NULL);


	return CreatedWindow;
}


/********************************************************************************************
>	void OriginGadget::OnLButtonDown(UINT32 nFlags, CPoint point);

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		nFlags - 
				point  - 
	Purpose:	handle left button downs
********************************************************************************************/
                    
void OriginGadget::OnLButtonDown(UINT32 nFlags, CPoint point)
{
	String_256 OpToken(OPTOKEN_SPREADORIGIN);

	if (pParentWnd != NULL)
		pParentWnd->InvokeDragOp(&OpToken,NULL,nFlags,point);
}


/********************************************************************************************
>	void OriginGadget::OnRButtonUp(UINT32 nFlags, CPoint point);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
	Inputs:		nFlags - 
				point  -
	Purpose:	Handle left button up events - pop-up context sensitive menu
********************************************************************************************/
                    
void OriginGadget::OnRButtonUp(UINT32 nFlags, CPoint point)
{
	OriginContextMenu* pOriginMenu = new OriginContextMenu;
	pOriginMenu->Show();
}


/********************************************************************************************
>	void OriginGadget::OnLButtonDblClk(UINT32 nFlags, CPoint point);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95
	Inputs:		nFlags - 
				point  - 
	Purpose:	handle left button double clicks
********************************************************************************************/
                    
void OriginGadget::OnLButtonDblClk(UINT32 nFlags, CPoint point)
{
	if (OpResetSpreadOrigin::GetState(NULL,NULL).Greyed==FALSE)
	{
	 	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_RESETSPREADORIGIN);
		if (pOpDesc!=NULL)
			pOpDesc->Invoke();
		else
			ERROR3("OriginGadget::OnLButtonDblClk() - FindOpDescriptor(OPTOKEN_RESETSPREADORIGIN) failed");
	}
}


/********************************************************************************************
>	void OriginGadget::OnMouseMove(UINT32 nFlags, CPoint MousePos);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/95
	Inputs:		nFlags - 
				point  - 
	Purpose:	handle mouse moves over Origin
********************************************************************************************/
                    
void OriginGadget::OnMouseMove(UINT32 nFlags, CPoint MousePos)
{
	static CPoint OldMousePos = CPoint(0,0);
	if (MousePos==OldMousePos)
		return;

	ControlHelper::DoBubbleHelpOn(GetSafeHwnd(),0,BubbleHelpCallBack,this);

	StatusLine* pStatusLine=GetApplication()->GetpStatusLine();
	if (pStatusLine)
	{
		String_256 Text("");
		GetStatusLineText(&Text,MousePos,GetSafeHwnd());
		pStatusLine->UpdateText(&Text,STATUSLINE_SELDESC_STATBAR);
	}

	OldMousePos = MousePos;
}


/******************************************************************************
>	static TCHAR* OriginGadget::BubbleHelpCallBack(HWND hWnd, UINT32 PaneIndex, void* UserData)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/95
	Purpose:	Bubble help call back handler
				(called some time after call to DoBubbleHelpOn() if mouse ha not moved)
	Inputs:		hWnd     - not used
				PaneID   - not used
				UserData - not used
	Returns:	pointer to text, or NULL if fails
******************************************************************************/

TCHAR* OriginGadget::BubbleHelpCallBack(HWND, UINT32 PaneIndex, void*)
{
	static String_64 BubbleHelpText("");

	BubbleHelpText.Load(_R(IDS_ORIGIN_BH));

	return (TCHAR*)BubbleHelpText;
}


/******************************************************************************
>	BOOL OriginGadget::GetStatusLineText(String_256* pText, CPoint MousePos, HWND hWnd)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/95
	Inputs:		MousePos - position in window (not used)
				hWnd     - handle of window
	Outputs:	pText    -
	Returns:	TRUE if ptext hold valid text, else FALSE if not over a pane
	Purpose:	if over origin gadget return status line help
******************************************************************************/

BOOL OriginGadget::GetStatusLineText(String_256* pText, CPoint MousePos, HWND hWnd)
{
	ERROR2IF(pText==NULL,FALSE,"OILRuler::GetStatusLineText() - pText==NULL");

	if (GetSafeHwnd()!=hWnd)
		return FALSE;

	return (pText->Load(_R(IDS_ORIGIN_SH))!=0);
}


/////////////////////////////////////////////////////////////////////////////////////////////
//		LegendLabel Class
/////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(LegendLabel ,CWnd)

BEGIN_MESSAGE_MAP(LegendLabel,CWnd)
	//{{AFX_MSG_MAP( LegendLabel )
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP( )


/********************************************************************************************
>	LegendLabel::LegendLabel()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Purpose:	Creates an OILDockingBar object.
********************************************************************************************/
                    
LegendLabel::LegendLabel()
{
	LegendText = _R(IDS_OILRULER_INCHES); // "Inches";
 	LegendWidth = LegendHeight = 0;
}


/*********************************************************************************************
>	void LegendLabel::ShowLabel(BOOL show)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Inputs:		show - FALSE to hide label
	Purpose:	Shows or hides this label
**********************************************************************************************/ 

void LegendLabel::ShowLabel(BOOL show)
{
	ShowWindow(show ? SW_SHOWNA : SW_HIDE);
}

 
/********************************************************************************************
>	virtual LegendLabel::~LegendLabel()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Purpose:	Destroys an LegendLabel object 
********************************************************************************************/

LegendLabel::~LegendLabel()
{
}


/********************************************************************************************
>	afx_msg void LegendLabel::OnPaint()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Purpose:	Paints the client area of the Legend Label
********************************************************************************************/

void LegendLabel::OnPaint()
{
	// prevent recursive redraws when an error ocurs
	if (CCamApp::DisableSys)
		return;

	// Create a painting DC.
	CPaintDC dc(this);
	dc.SetROP2(R2_COPYPEN);
	
	// read the necessary colours
	COLORREF BackCol  = ::GetSysColor(COLOR_BTNFACE);
	COLORREF BotRight = ::GetSysColor(COLOR_BTNHIGHLIGHT);
	COLORREF TopLeft  = ::GetSysColor(COLOR_BTNSHADOW);
	COLORREF TextCol  = ::GetSysColor(COLOR_BTNTEXT);

  	// get info about client rect
	CRect ClientRect;
	GetClientRect(&ClientRect);
	INT32 left   = ClientRect.left;
	INT32 top    = ClientRect.top;
	INT32 width  = ClientRect.Width();
	INT32 height = ClientRect.Height();
	INT32 right  = left + width;
	INT32 bottom = top  + height;

	// paint the background
	PatB(&dc, left,top, width,height, BackCol);

	// set up dc to paint transparent black text in correct font
	INT32      OldBkMode  = dc.SetBkMode(TRANSPARENT);
	COLORREF OldTextCol = dc.SetTextColor(TextCol);
	HGDIOBJ  pOldFont   = dc.SelectObject(FontFactory::GetFont(STOCKFONT_RULERS));
	if (pOldFont==NULL)
	{
		ERROR2RAW("LegendLabel::OnPaint() - dc.SelectObject() failed");
		return;
	}

	// now draw the legend text formated into the client area
	dc.DrawText((TCHAR*)LegendText, -1, &ClientRect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);

	// restore the dc
	HGDIOBJ  pTempFont   = dc.SelectObject(pOldFont);
	COLORREF TempTextCol = dc.SetTextColor(OldTextCol);
	INT32      TempBkMode  = dc.SetBkMode(OldBkMode);
	if (pTempFont==NULL)
	{
		ERROR2RAW("LegendLabel::OnPaint() - dc.SelectObject() failed");
		return;
	}

	// paint hi/lo light
	PatB(&dc,  left,top,    width-1,1,        TopLeft);		// top
	PatB(&dc,  left,top,          1,height-1, TopLeft);		// left
	PatB(&dc,  left,bottom,   width,-1,       BotRight);	// bottom
	PatB(&dc, right,top,         -1,height,   BotRight);	// right
}


/*********************************************************************************************
>	virtual void LegendLabel::SetLegendText(StringBase* pText)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Returns:	FALSE if fails
	Purpose:	Called to set legend text - resize and reposition Legend.
**********************************************************************************************/ 

BOOL LegendLabel::SetLegendText(StringBase* pText)
{
	ERROR2IF(pText==NULL,FALSE,"SetLegendText called with a NULL string");
	LegendText = *pText;
	
	BOOL ok=SetLegendSize();
	if (ok)
	{
		ok=PositionLegend();
		Invalidate();
		UpdateWindow();
	}

	return ok;
}


/*********************************************************************************************
>	virtual BOOL LegendLabel::SetLegendSize()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Returns:	FALSE if fails
	Purpose:	Sets the width of the window to fit the legend text.
	Note:		The actual Legend window may be 1 pixel larger to ensure that 
				it is vertically centred correctly in ruler (ie gap above == gap below)
**********************************************************************************************/ 

BOOL LegendLabel::SetLegendSize()
{
	WinRect WinTextSize;
	BOOL ok=OILRuler::GetTextSize(&WinTextSize,&LegendText);
	if (!ok)
		return FALSE;

	LegendHeight = WinTextSize.Height() +2;		// add pixel border top and bottom
	LegendWidth  = WinTextSize.Width()  +2+7;	// add border + 3 pixel gap each end + pixel to right to balance 1 to left

	return TRUE;
}


/*********************************************************************************************
>	virtual BOOL LegendLabel::PositionLegend()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Returns:	FALSE if fails
	Purpose:	Called by Horizontal ruler to position the LegendLabel.
**********************************************************************************************/ 

BOOL LegendLabel::PositionLegend()
{
	// get the client area of the horizontal ruler
	CRect ParentRect;
	GetParent()->GetClientRect(&ParentRect);
	
	// determine position of window
	CRect LegendRect;
	LegendRect.top 	  = (ParentRect.Height() -  LegendHeight)/2;	// centre vertically
	LegendRect.bottom = ParentRect.bottom    -  LegendRect.top;		// with same gap at bottom
	LegendRect.right  = ParentRect.right     - (LegendRect.top+6);	// and this gap plus some from right end of ruler
	LegendRect.left   = LegendRect.right     -  LegendWidth;

	// actually move the window
	MoveWindow(&LegendRect, TRUE);

	return TRUE;
}


/********************************************************************************************
>	BOOL LegendLabel::Create(CWnd* pParentWindow)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		pParentWindow - 
	Returns:	FALSE if fails
	Purpose:	Creates an LegendLabel object.
********************************************************************************************/
                    
BOOL LegendLabel::Create(CWnd* pParentWindow)
{ 
	BOOL CreatedWindow;
	ENSURE(pParentWindow,"DockingBar must have a Parent");


	CRect rect;			// Rectangle for Create
	rect.SetRectEmpty();
	DWORD style = WS_CHILD  | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE;

	// what is this supposed to do?!
	#ifndef WIN32
		DWORD
	#endif
	
	// create the HWND
	CreatedWindow = CWnd::Create(NULL,
						AfxRegisterWndClass(CS_DBLCLKS,0,0,0),
						style,
						rect,
						pParentWindow,
						0,
						NULL);

	// size the window to fit the legend text
	if(CreatedWindow)
		SetLegendSize();

	return CreatedWindow;
}


/********************************************************************************************
>	void LegendLabel::OnLButtonDown(UINT32 nFlags, CPoint point);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95
	Inputs:		nFlags - 
				point  - 
	Purpose:	handle left button down events - pass on to parent ruler
********************************************************************************************/
                    
void LegendLabel::OnLButtonDown(UINT32 nFlags, CPoint point)
{
	((OILHorizontalRuler*)GetParent())->OnLButtonDown(nFlags, ClientToParentClient(point));
}


/********************************************************************************************
>	void LegendLabel::OnRButtonUp(UINT32 nFlags, CPoint point);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95
	Inputs:		nFlags - 
				point  - 
	Purpose:	handle left button up events -  pass on to parent ruler
********************************************************************************************/
                    
void LegendLabel::OnRButtonUp(UINT32 nFlags, CPoint point)
{
	((OILHorizontalRuler*)GetParent())->OnRButtonUp(nFlags, ClientToParentClient(point));
}


/********************************************************************************************
>	void LegendLabel::OnLButtonDblClk(UINT32 nFlags, CPoint point);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95
	Inputs:		nFlags - 
				point  - 
	Purpose:	handle left button double click events - pass on to parent ruler
********************************************************************************************/
                    
void LegendLabel::OnLButtonDblClk(UINT32 nFlags, CPoint point)
{
	((OILHorizontalRuler*)GetParent())->OnLButtonDblClk(nFlags, ClientToParentClient(point));
}


/********************************************************************************************
>	void LegendLabel::OnMouseMove(UINT32 nFlags, CPoint MousePos);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/95
	Inputs:		nFlags - 
				point  - 
	Purpose:	handle mousemoves over ruler legend label
********************************************************************************************/
                    
void LegendLabel::OnMouseMove(UINT32 nFlags, CPoint MousePos)
{
	static CPoint OldMousePos = CPoint(0,0);
	if (MousePos==OldMousePos)
		return;

	StatusLine* pStatusLine=GetApplication()->GetpStatusLine();
	if (pStatusLine)
	{
		String_256 Text("");
		GetStatusLineText(&Text,MousePos,GetSafeHwnd());
		pStatusLine->UpdateText(&Text,STATUSLINE_SELDESC_STATBAR);
	}

	OldMousePos = MousePos;
}


/******************************************************************************
>	BOOL LegendLabel::GetStatusLineText(String_256* pText, CPoint MousePos, HWND hWnd)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/95
	Inputs:		MousePos - position in window (not used)
				hWnd     - handle of window
	Outputs:	pText    -
	Returns:	TRUE if ptext hold valid text, else FALSE if not over a pane
	Purpose:	if over a ruler legend return status line help
******************************************************************************/

BOOL LegendLabel::GetStatusLineText(String_256* pText, CPoint MousePos, HWND hWnd)
{
	ERROR2IF(pText==NULL,FALSE,"LegendLabel::GetStatusLineText() - pText==NULL");

	if (GetSafeHwnd()!=hWnd)
		return FALSE;

	return (pText->Load(_R(IDS_RULER_LEGEND_SH))!=0);
}


/********************************************************************************************
>	CPoint LegendLabel::ClientToParentClient(CPoint point)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95
	Inputs:		point - 
	Returns:	the point relative to the parent window
	Purpose:	convert coord relative to this window to a coord relative to the parent window
	Note:		Nice if this was in a parent CCWnd class
********************************************************************************************/
                    
CPoint LegendLabel::ClientToParentClient(CPoint point)
{
	ClientToScreen(&point);
	GetParent()->ScreenToClient(&point);
	return point;
}

#endif	// WEBSTER
