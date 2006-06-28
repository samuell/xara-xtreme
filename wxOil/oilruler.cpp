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
//#include "mainfrm.h"
//#include "camafx.h"
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "mario.h"
//#include "winrect.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "camview.h"
#include "rulers.h"
//#include "fonts.h"
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

//#ifdef _DEBUG
//	#undef THIS_FILE
//	static char BASED_CODE THIS_FILE[] = __FILE__;
//#endif

#define MAJOR_GRAT_LEN 6
#define MIN_PIXEL_GRATICULE_SPACING_X 5
#define MIN_PIXEL_GRATICULE_SPACING_Y 5

// statics ...
String_256*			OILRuler::FontName         = NULL; /*String_256(_R(IDS_OILRULER_SMALL_FONTS)); "Small Fonts";*/ // - read from .ini
INT32        		OILRuler::FontSize         = 7;				// read from .ini
INT32       		OILRuler::RenderWidth      = 0;		// this is set up on app start up
UINT32       		OILRuler::RulerWidth       = 0;		// actually offset to next bar (based on RenderWidth)
INT32       		OILRuler::CharHeight       = 0;
INT32       		OILRuler::CharWidth        = 0;
DocView*   			OILRuler::pPaintDocView    = NULL;
wxDC*		  		OILRuler::pPaintDC         = NULL;
wxSize      		OILRuler::RulerToDocOffset = wxSize(0,0);
OpGuidelineParam	OILRuler::NewGuidelineParam;


/////////////////////////////////////////////////////////////////////////////////////////////
//									 OILRuler Base Class
/////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(OILRuler, wxWindow)

BEGIN_EVENT_TABLE( OILRuler, wxWindow )
	EVT_LEFT_DOWN(		OILRuler::OnLButtonDown)
	EVT_LEFT_DCLICK(	OILRuler::OnLButtonDblClk)
	EVT_RIGHT_UP(		OILRuler::OnRButtonUp)
	EVT_MOTION(			OILRuler::OnMouseMove)
	EVT_PAINT(			OILRuler::OnPaint)
END_EVENT_TABLE()


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
	if (FontName == NULL)
		FontName = new String_256(_R(IDS_OILRULER_SMALL_FONTS));

	BOOL ok=GetApplication()->DeclareSection(_T("Rulers"), 2);
	if (ok && FontName != NULL) ok=GetApplication()->DeclarePref(_T("Rulers"), _T("FontName"), FontName);
	if (ok) ok=GetApplication()->DeclarePref(_T("Rulers"), _T("FontSize"), &FontSize, 2,72);
	if (!ok)
		return FALSE;
	ERROR2IF(FontName == NULL,FALSE,"OILRuler::SetRenderWidth() - couldn't create a string_256 - eek !");
	if (FontName->Length()==0)
		FontName->Load(_R(IDS_OILRULER_SMALL_FONTS));
	ERROR2IF(FontName->Length()==0,FALSE,"OILRuler::SetRenderWidth() - invalid FontName in .ini file");

	WinRect WinTextSize;
	ok=GetTextSize(&WinTextSize, _T("8"));
	if (!ok)
		return FALSE;

	CharHeight  = WinTextSize.GetHeight();
	CharWidth   = WinTextSize.GetWidth();
	RenderWidth = CharWidth*3 + 4;

	return TRUE;
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
>	static wxFont OILRuler::GetRulerFont()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/Jun/2006
	Purpose:	Get a stock font for use in rulers
********************************************************************************************/

wxFont OILRuler::GetRulerFont()
{
PORTNOTE("ruler", "Temporarilly replace font factory with direct wxFont access")
#if !defined(EXCLUDE_FROM_XARALX)
		wxFont font = FontFactory::GetFont(STOCKFONT_RULERS);
#else
	wxFont FixedFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	FixedFont.SetPointSize(OILRuler::FontSize);		// 6 is too small, 7 is too big, really...

	return FixedFont;
#endif
}




/********************************************************************************************
>	void OILRuler::PatB(wxDC* pDC, INT32 x, INT32 y, INT32 dx, INT32 dy, wxColour rgb)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/94
	Inputs:		hDC - destination DC
				x,y,dx,dy - rectangle to fill
				rgb - colour to fill with
	Purpose:	Paints a rectangle in the given (dithered) colour
********************************************************************************************/

void OILRuler::PatB(wxDC* pDC, INT32 x, INT32 y, INT32 dx, INT32 dy, wxColour rgb)
{
	wxRect rect;
	rect.x	= x;
	rect.y	= y;
	rect.width	= dx;
	rect.height	= dy;

	wxBrush brush(rgb);
	pDC->SetBrush(brush);
	pDC->DrawRectangle(rect);
}




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
>	BOOL OILRuler::Create(CCamView* pOwnerView, INT32 id)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		pParentWindow - 
	Returns:	FALSE if fails
	Purpose:	Creates the ruler window.
********************************************************************************************/
                    
BOOL OILRuler::Create(CCamView* pOwnerView, INT32 id)
{ 
	ERROR2IF(pOwnerView==NULL, FALSE, "DockingBar must have a Parent");
	
	m_pOwnerView = pOwnerView;

	if (!wxWindow::Create(pOwnerView->GetParentFrame(), id, wxDefaultPosition, wxSize(RulerWidth, RulerWidth), wxNO_BORDER))
		return(FALSE);

	PostCreate();

	return TRUE;
}


/********************************************************************************************
>	virtual BOOL OILRuler::StartDrag(UINT32 nFlags, wxPoint point)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Returns:	FALSE if fails
	Purpose:	base StartDrag function  - don't call
********************************************************************************************/
                    
BOOL OILRuler::StartDrag(UINT32 nFlags, wxPoint point)
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
	Show(show);
	
	BROADCAST_TO_ALL(ScreenChangeMsg());

	if (show)
	{
		// we need to take into account camelots new floating toolbar ....

PORTNOTE("ruler", "Removed code to move floating toolbars")
#if !defined(EXCLUDE_FROM_XARALX)
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
#endif
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
	Refresh();
	Update();
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
>	void OILRuler::OnPaint( wxPaintEvent &evnt )

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Redraws the colour bar
	Errors:		-

**********************************************************************************************/ 
void OILRuler::OnPaint( wxPaintEvent &evnt )
{
	// Always create this so that the area is validated
	wxPaintDC dc(this);

	if (!CCamApp::IsDisabled())
	{
		DoPaint(&dc);
	}
}



/*********************************************************************************************
>	void OILRuler::DoPaint(wxDC* pDC)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Purpose:	Paints the client area of the Ruler which isn't covered by child windows,
				in response to a WM_PAINT message.
**********************************************************************************************/

void OILRuler::DoPaint(wxDC* pDC)
{
	// get and validate a few pointers before we start
	DocView* pDocView = m_pOwnerView->GetDocViewPtr();
	if (pDocView==NULL)
	{
		ERROR2RAW("OILRuler::OnPaint() - pDocView==NULL");
		return;
	}
	wxWindow* pRenderWnd=pDocView->GetRenderWindow();
	if (pRenderWnd==NULL)
	{
		ERROR2RAW("OILRuler::OnPaint() - pRenderWnd==NULL");
		return;
	}

	// get the update region in docview OilCoords
	WinRect RulerUpdateRect;
	pDC->GetClippingBox(&RulerUpdateRect.x, &RulerUpdateRect.y, &RulerUpdateRect.width, &RulerUpdateRect.height);
	if (RulerUpdateRect.IsEmpty())
		RulerUpdateRect = GetClientRect();
	OilRect UpdateOilRect = ClientToOil(pDocView,RulerUpdateRect);

	// cache the offsets from ruler to doc window relative coords
	// note only x valid when painting horizontal ruler and only y valid for painting vertical ruler
	WinRect UpdateRect = ClientToOtherClient(pRenderWnd,RulerUpdateRect);
	ERROR3IF(UpdateRect.GetWidth()!=RulerUpdateRect.GetWidth() || UpdateRect.GetHeight()!=RulerUpdateRect.GetHeight(),"OILRuler::OnPaint() - ruler and doc window have incompatible coordinate spaces");
	RulerToDocOffset = wxSize(UpdateRect.x-RulerUpdateRect.x, UpdateRect.GetBottomEx()-RulerUpdateRect.GetBottomEx());


	// read the necessary colours
	wxColour BackCol  = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	wxColour TopLeft  = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT);
	wxColour BotRight = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW);
	wxColour TextCol  = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT);

  	// get info about client rect
	wxRect ClientRect = GetClientRect();
	INT32 left   = ClientRect.x;
	INT32 top    = ClientRect.y;
	INT32 width  = ClientRect.GetWidth();
	INT32 height = ClientRect.GetHeight();
//	INT32 right  = left + width;
//	INT32 bottom = top  + height;
//	if (IsHorizontal() && height!=RenderWidth || !IsHorizontal() && width!=RenderWidth)
//		ERROR3_PF(("OILRuler::OnPaint() - ruler client size (%d,%d) and RenderWidth (%d) incompatible!",width,height,RenderWidth));

	// render the background
	wxPen penNoOutline(BackCol, 0, wxTRANSPARENT);
	pDC->SetPen(penNoOutline);
	PatB(pDC, left,top, width,height, BackCol);
	
	// paint borders to be over written by graticules
/*	if(IsHorizontal())
	{
		PatB(pDC, left+1,top+1,  width-2,1,  TopLeft);	// horizontal ruler top    hi-light
		PatB(pDC, left+1,bottom, width-1,-1, BotRight);	// horizontal ruler bottom lo-light
	}
	else
	{
		PatB(pDC, left+1,top+1,  1,height-2, TopLeft);	// vertical ruler left  hi-light
		PatB(pDC,  right,top+1, -1,height-1, BotRight);	// vertical ruler right lo-light
	}
*/
	pDC->SetPen(wxPen(*wxBLACK, 1, wxSOLID));		// Restore previous Pen

	// get the kernel to render the ruler
	if (pKernelRuler)
	{
		// store pointers to CDC and DocView required for redraw
		pPaintDC      = pDC;
		pPaintDocView = pDocView;

		// set up DC objects required to draw the ruler
		pDC->SetTextForeground(TextCol);
		pDC->SetBackgroundMode(wxTRANSPARENT);
		pDC->SetFont(GetRulerFont());

		// draw the ruler
		pKernelRuler->Redraw(&UpdateOilRect);
	}

	// paint the bits of the border that need to cover the ruler rendering
/*	if(IsHorizontal())
	{
		PatB(pDC,   left,top,    1,height,   BackCol);	// horizontal ruler light grey left edge
		PatB(pDC, left+1,top+1,  1,height-2, TopLeft);	// horizontal ruler left  hi-light
		PatB(pDC,  right,top+1, -1,height-1, BotRight);	// horizontal ruler right lo-light
	}
	else
	{
		PatB(pDC,   left,top,    width-1,1,  BackCol);	// vertical ruler top light grey top edge
		PatB(pDC, left+1,top+1,  width-2,1,  TopLeft);	// vertical ruler top    hi-light
		PatB(pDC, left+1,bottom, width-1,-1, BotRight);	// vertical ruler bottom lo-light
	}
*/
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
	pPaintDC->DrawRectangle(GratWinRect);

	return TRUE;
}


/********************************************************************************************
>	afx_msg void OILRuler::OnLButtonDown(wxMouseEvent& event);

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		nFlags - 
				point  -
	Purpose:	process LButtonDoown event 
********************************************************************************************/
                    
void OILRuler::OnLButtonDown(wxMouseEvent& event)
{
	PORTNOTE("ruler", "TODO: Set nFlags");
	UINT32 nFlags = 0;
	StartDrag(nFlags, event.GetPosition());
}


/********************************************************************************************
>	void OILRuler::OnRButtonUp(wxMouseEvent& event);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
	Inputs:		nFlags - 
				point  -
	Purpose:	Handle left button up events - pop-up context sensitive menu
********************************************************************************************/
                    
void OILRuler::OnRButtonUp(wxMouseEvent& event)
{
	PORTNOTE("ruler", "TODO: Set nFlags");
//	UINT32 nFlags = 0;
	wxPoint point = event.GetPosition();

	// get a few pointers - no error checking yet!
	DocView* pDocView = m_pOwnerView->GetDocViewPtr();
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
>	void OILRuler::OnLButtonDblClk(wxMouseEvent& event);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95
	Inputs:		nFlags - 
				point  - 
	Purpose:	handle left button double clicks
********************************************************************************************/
                    
void OILRuler::OnLButtonDblClk(wxMouseEvent& event)
{
	PORTNOTE("ruler", "TODO: Set nFlags");
//	UINT32 nFlags = 0;
	wxPoint point = event.GetPosition();

	// get a few ponters
	DocView* pDocView = m_pOwnerView->GetDocViewPtr();
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
>	void OILRuler::OnMouseMove(wxMouseEvent& event);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/95
	Inputs:		nFlags - 
				point  - 
	Purpose:	handle mouse moves over the ruler
********************************************************************************************/
                    
void OILRuler::OnMouseMove(wxMouseEvent& event)
{
	PORTNOTE("ruler", "TODO: Set nFlags");
//	UINT32 nFlags = 0;
	wxPoint MousePos = event.GetPosition();

	static wxPoint OldMousePos = wxPoint(0,0);
	if (MousePos==OldMousePos)
		return;

	StatusLine* pStatusLine=GetApplication()->GetpStatusLine();
	if (pStatusLine)
	{
		String_256 Text("");
		GetStatusLineText(&Text, WinCoord(MousePos.x, MousePos.y), this);
		pStatusLine->UpdateText(&Text, STATUSLINE_SELDESC_STATBAR);
	}

	OldMousePos = MousePos;
}


/******************************************************************************
>	BOOL OILRuler::GetStatusLineText(String_256* pText, WinCoord MousePos, CWindowID hWnd)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/95
	Inputs:		MousePos - position in window (not used)
				hWnd     - handle of window
	Outputs:	pText    -
	Returns:	TRUE if ptext hold valid text, else FALSE if not over a pane
	Purpose:	if over a ruler return status line help
******************************************************************************/

BOOL OILRuler::GetStatusLineText(String_256* pText, WinCoord MousePos, CWindowID hWnd)
{
	ERROR2IF(pText==NULL,FALSE,"OILRuler::GetStatusLineText() - pText==NULL");

	if (this!=hWnd)
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
>	static BOOL OILRuler::GetTextSize(OilRect* pTextSize, LPCTSTR str, DocView* pDocView)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/9/95
	Inputs:		pText    -
				pDocView - 
	Outputs:	pTextSize - x,y size of text in OilCoords
	Returns:	FALSE if fails
********************************************************************************************/
                    
BOOL OILRuler::GetTextSize(OilRect* pTextSize, LPCTSTR str, DocView* pDocView)
{
	ERROR2IF(pTextSize==NULL,FALSE,"OILRuler::GetTextSize() - pTextSize==NULL");
	ERROR2IF( pDocView==NULL,FALSE,"OILRuler::GetTextSize() - pDocView==NULL");

	WinRect WinTextSize;
	BOOL ok=GetTextSize(&WinTextSize, str);
	if (!ok)
		return FALSE;

	*pTextSize = WinTextSize.ToOil(pDocView);
	return TRUE;
}


/********************************************************************************************
>	static BOOL OILRuler::GetTextSize(WinRect* pTextSize, LPCTSTR str)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/9/95
	Inputs:		pText     -
	Outputs:	pTextSize - x,y size of text in WinCoords
	Returns:	FALSE if fails
********************************************************************************************/
                    
BOOL OILRuler::GetTextSize(WinRect* pTextSize, LPCTSTR str)
{
	ERROR2IF(pTextSize==NULL,FALSE,"OILRuler::GetTextSize() - pTextSize==NULL");
	ERROR2IF(      str==NULL,FALSE,"OILRuler::GetTextSize() - pText==NULL");

	try
	{
		// create a screen compatible DC to find the size of the text
		wxScreenDC dc;

		wxFont font = GetRulerFont();
		dc.SetFont(font);

		wxSize TextSize(0,0);
		wxString Text(str);
		dc.GetTextExtent(Text, &TextSize.x, &TextSize.y);
		pTextSize->x   = 0;
		pTextSize->y    = 0;
		pTextSize->SetRightEx(TextSize.x);
		pTextSize->SetBottomEx(TextSize.y);

	}
	catch (...)
	{
		ERROR2(FALSE, "OILRuler::GetTextSize() - failed to create dc");
	}

	return TRUE;
}


/********************************************************************************************
>	WinRect OILRuler::ClientToOtherClient(wxWindow* pOtherCWnd, WinRect WR)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95
	Inputs:		pOtherCWnd - window to which to make rect relative to
				WR         - win rect to convert
	Returns:	WinRect    - converted win rect
	Purpose:	Convert rect relative to this window to rect relative to another window
	Note:		Nice if this was in a parent CCWnd class
********************************************************************************************/

WinRect OILRuler::ClientToOtherClient(wxWindow* pOtherCWnd, WinRect WR)
{
	wxPoint tl = WR.GetTopLeft();
	wxPoint br = WR.GetBottomRightEx();

	ClientToScreen(tl);
	ClientToScreen(br);
	pOtherCWnd->ScreenToClient(tl);
	pOtherCWnd->ScreenToClient(br);

	WR.SetTopLeft(tl);
	WR.SetBottomRightEx(br);

	return WR;
}


/********************************************************************************************
>	wxPoint OILRuler::ClientToOtherClient(wxWindow* pOtherCWnd, wxPoint point)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95
	Inputs:		pOtherCWnd - window to which to make rect relative to
				point      - win coord to convert
	Returns:	WinCoord   - converted win coord
	Purpose:	Convert coord relative to this window to coord relative to another window
	Note:		Nice if this was in a parent CCWnd class
********************************************************************************************/

wxPoint OILRuler::ClientToOtherClient(wxWindow* pOtherCWnd, wxPoint point)
{
	ClientToScreen(point);
	pOtherCWnd->ScreenToClient(point);
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
	wxWindow* pRenderWnd = pDocView->GetRenderWindow();
	ERROR2IF(pRenderWnd==NULL,OilRect(OilCoord(0,0),OilCoord(0,0)),"OILRuler::ClientToOil() - pRenderWnd==NULL");
	return ClientToOtherClient(pRenderWnd,WR).ToOil(pDocView);
}


/********************************************************************************************
>	OilCoord OILRuler::ClientToOil(DocView* pDocView, wxPoint point)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95
	Inputs:		pDocView    - associated doc view
				point       - win coord to convert
	Returns:	OilCoord    - converted win coord
	Purpose:	Convert a (client) coord relative to this window to a document Oil coord
	Note:		Nice if this was in CWnd class but this is MFC, could do with a CCWnd class
********************************************************************************************/

OilCoord OILRuler::ClientToOil(DocView* pDocView, wxPoint point)
{
	wxWindow* pRenderWnd = pDocView->GetRenderWindow();
	ERROR2IF(pRenderWnd==NULL,OilCoord(0,0),"OILRuler::ClientToOil() - pRenderWnd==NULL");
	wxPoint DocPoint = ClientToOtherClient(pRenderWnd,point);
	return WinCoord(DocPoint.x,DocPoint.y).ToOil(pDocView);
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
	wxDC* pDC = pPaintDC;
//	BOOL LocalDC = (pDC==NULL);
//	if (LocalDC)
//		pDC=GetDC();

	BOOL ok = DrawMouseFollower(OilPos, pDocView, RenderType, pDC);

//	if (LocalDC)
//		ReleaseDC(pDC);

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
								 MouseFollowerRenderType RenderType, wxDC* pDC)
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
	wxPen pen(*wxGREEN, 1, wxSOLID);
	pDC->SetPen(pen);
	pDC->DrawLine(FollowerWinRect.GetLeft(), FollowerWinRect.GetTop(), FollowerWinRect.GetRightEx(), FollowerWinRect.GetBottomEx());

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// 				OILHorizontalRuler
/////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(OILHorizontalRuler, wxWindow)

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
	if (pLegend)
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
	if (pLegend == NULL)
		return FALSE;

	pLegend->Create(this, -1);

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
	WinRect ClipRect;
	ClipRect = GetClientRect();
	ClipRect.x+=2;
	ClipRect.SetRightEx(ClipRect.GetRightEx()-1);

	ScrollWindow(amount, 0, &ClipRect);
	Update();
}


/********************************************************************************************
>	virtual BOOL OILHorizontalRuler::StartDrag(UINT32 nFlags, wxPoint point)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Returns:	FALSE if fails
	Purpose:	StartDrag function .
********************************************************************************************/
                    
BOOL OILHorizontalRuler::StartDrag(UINT32 nFlags, wxPoint point)
{ 
	NewGuidelineParam.Method 	= GUIDELINEOPMETHOD_NEW_DRAG;
	NewGuidelineParam.Type 		= GUIDELINE_HORZ;

	String_256 OpToken(OPTOKEN_GUIDELINE);

	if (m_pOwnerView != NULL)
		m_pOwnerView->InvokeDragOp(&OpToken ,&NewGuidelineParam, nFlags, point);

	return TRUE;
}


/*********************************************************************************************
>	virtual void OILHorizontalRuler::CalcPosFromParentClient(wxRect* lpRect)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		A long pointer to a rectangle which describes the dimensions of the client
				area of the window containing the scroller.
	Outputs:	Changes lpRect to the position a horizontal ruler would occupy
				normally, ie. along the top of the owning window.
	Purpose:	Called by CCamView when its size changes, so it knows where to reposition
				the CHorzScroller.
**********************************************************************************************/ 

void OILHorizontalRuler::CalcPosFromParentClient(WinRect* lpRect)
{
	WinRect parentRect;
	parentRect = m_pOwnerView->GetParentFrame()->GetClientRect();
	if (RULER_BORDERS)
	{
	  parentRect.y --;
	  parentRect.x -=2;
	}
	
	lpRect->x   = parentRect.x + RulerWidth;
	lpRect->y    = parentRect.y;
	lpRect->SetRightEx(parentRect.GetRightEx()+1);
	lpRect->SetBottomEx(parentRect.y  + RulerWidth);
	
}


/*********************************************************************************************
>	BOOL OILHorizontalRuler::DrawMajorGraticule(OilCoord GratOilPos, LPCTSTR str)

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

BOOL OILHorizontalRuler::DrawMajorGraticule(OilCoord GratOilPos, LPCTSTR str)
{
	ERROR2IF(str		  ==NULL,FALSE,"OILHorizontalRuler::DrawMajorGraticule() - pGrtaLabel==NULL");
	ERROR2IF(pPaintDC     ==NULL,FALSE,"OILHorizontalRuler::DrawMajorGraticule() - pPaintDC==NULL");
	ERROR2IF(pPaintDocView==NULL,FALSE,"OILHorizontalRuler::DrawMajorGraticule() - pPaintDocView==NULL");

	// convert to ruler relative win coords
	WinCoord GratWinPos = GratOilPos.ToWin(pPaintDocView);
	GratWinPos -= RulerToDocOffset;

	// draw the major graticule in 2 halves with a gap for the text between
	INT32 GratLen = MAJOR_GRAT_LEN;
	WinRect GratWinRect(GratWinPos.x, RenderWidth-MAJOR_GRAT_LEN, GratWinPos.x+1, RenderWidth);
	pPaintDC->DrawRectangle(GratWinRect);

	// draw the graticule label (centred about grat pos)
	wxSize TextSize(0,0);
	wxString Text(str);
	pPaintDC->GetTextExtent(Text, &TextSize.x, &TextSize.y);

	INT32   Textx    = GratWinPos.x-TextSize.x/2;				// centre text about grat (+1 for gap to right - left inclusive)
	INT32   Texty    = RenderWidth-(GratLen+TextSize.y);
	pPaintDC->DrawText(Text, Textx, Texty);						// actual digits drawn 1 pixel beyond, and 2 pixels below this point

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


/*********************************************************************************************
>	void OILHorizontalRuler::ShowRuler(BOOL show)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Inputs:		show - FALSE to hide the ruler
	Purpose:	Shows or hides this ruler
**********************************************************************************************/ 

void OILHorizontalRuler::ShowRuler(BOOL show)
{
	OILRuler::ShowRuler(show);
	if (pLegend)
		pLegend->ShowLabel(show);
} 


/////////////////////////////////////////////////////////////////////////////////////////////
// 				OILVerticalRuler
/////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(OILVerticalRuler, wxWindow)

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
	WinRect ClipRect;
	ClipRect = GetClientRect();
	ClipRect.y+=2;
	ClipRect.SetBottomEx(ClipRect.GetBottomEx()-1);

	ScrollWindow(0, amount, &ClipRect);
	Update();
}


/********************************************************************************************
>	virtual BOOL OILVerticalRuler::StartDrag(UINT32 nFlags, wxPoint point)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Returns:	FALSE if fails
	Purpose:	Sends a start vertical guideline drag message.
********************************************************************************************/
                    
BOOL OILVerticalRuler::StartDrag(UINT32 nFlags, wxPoint point)
{ 
	NewGuidelineParam.Method 	= GUIDELINEOPMETHOD_NEW_DRAG;
	NewGuidelineParam.Type 		= GUIDELINE_VERT;

	String_256 OpToken(OPTOKEN_GUIDELINE);

	if (m_pOwnerView != NULL)
		m_pOwnerView->InvokeDragOp(&OpToken, &NewGuidelineParam, nFlags, point);

	return TRUE;
}


/*********************************************************************************************
>	virtual void OILVerticalRuler::CalcPosFromParentClient(wxRect* lpRect)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		A long pointer to a rectangle which describes the dimensions of the client
				area of the window containing the scroller.
	Outputs:	Changes lpRect to the position a vretical rulerr would occupy
				normally, ie. along the left side of the owning window.
	Purpose:	Called by CCamView when its size changes, so it knows where to reposition
				the CVertical ruler.
**********************************************************************************************/ 

void OILVerticalRuler::CalcPosFromParentClient(WinRect* lpRect)
{
	WinRect parentRect;
	parentRect = m_pOwnerView->GetParentFrame()->GetClientRect();
	if (RULER_BORDERS)
	{
	  parentRect.y -= 2;
	  parentRect.y--;
	}
	lpRect->x   = parentRect.x;
	lpRect->y    = parentRect.y  + RulerWidth;
	lpRect->SetRightEx(parentRect.x + RulerWidth);
	lpRect->SetBottomEx(parentRect.GetBottomEx()+1);
}


/*********************************************************************************************
>	BOOL OILVerticalRuler::DrawMajorGraticule(OilCoord GratOilPos, LPCTSTR str)

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

BOOL OILVerticalRuler::DrawMajorGraticule(OilCoord GratOilPos, LPCTSTR str)
{
	ERROR2IF(str		  ==NULL,FALSE,"OILVerticalRuler::DrawMajorGraticule() - pGrtaLabel==NULL");
	ERROR2IF(pPaintDC     ==NULL,FALSE,"OILVerticalRuler::DrawMajorGraticule() - pPaintDC==NULL");
	ERROR2IF(pPaintDocView==NULL,FALSE,"OILVerticalRuler::DrawMajorGraticule() - pPaintDocView==NULL");

	// convert to ruler relative win coords
	WinCoord GratWinPos = GratOilPos.ToWin(pPaintDocView);
	GratWinPos -= RulerToDocOffset;

	wxString Text(str);

	// get width of gap for text
	INT32 TextLen  = Text.Length();
	INT32 GapWidth = CharWidth*max(2,TextLen)+1;		// +1 for gap to right (left already included)

	// draw the major graticule in 2 halves with a gap for the text between
	INT32 GratLen = min(MAJOR_GRAT_LEN,RenderWidth-(GapWidth+1));
	WinRect GratWinRect(RenderWidth-GratLen, GratWinPos.y-1, RenderWidth, GratWinPos.y);
	pPaintDC->DrawRectangle(GratWinRect);

	// draw the graticule label (centred about grat pos)
	wxSize TextSize(0,0);
	pPaintDC->GetTextExtent(Text, &TextSize.x, &TextSize.y);
	INT32	TextWidth = TextSize.x+1;
	INT32   Offset    = (GapWidth-TextWidth)/2;					// offset to centre text in text gap (if proportional font)
	INT32   Textx     = RenderWidth-GratLen-(TextWidth+Offset);
	INT32   Texty     = GratWinPos.y-TextSize.y/2-1;
	pPaintDC->DrawText(Text, Textx, Texty);						// actual digits drawn 1 pixel beyond, and 2 pixels below this point

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//									 OriginGadget Class
/////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(OriginGadget, wxWindow)

BEGIN_EVENT_TABLE( OriginGadget, wxWindow )
	EVT_LEFT_DOWN(		OriginGadget::OnLButtonDown)
	EVT_LEFT_DCLICK(	OriginGadget::OnLButtonDblClk)
	EVT_RIGHT_UP(		OriginGadget::OnRButtonUp)
	EVT_MOTION(			OriginGadget::OnMouseMove)
	EVT_PAINT(			OriginGadget::OnPaint)
END_EVENT_TABLE()

/********************************************************************************************
>	OriginGadget::OriginGadget()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Purpose:	Creates an OILDockingBar object.
********************************************************************************************/
                    
OriginGadget::OriginGadget()
{ 
	pGlyph = NULL;
PORTNOTE("ruler", "Removed use of CBitmap")
#if !defined(EXCLUDE_FROM_XARALX)
	pGlyph = new CBitmap;
	if(pGlyph)
	{
		if(! pGlyph->LoadBitmap(_R(IDB_ORIGINBMP)))
		{	
			delete pGlyph;
			pGlyph = NULL;
		}
	}
#endif
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
	Show(show);
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



BOOL OriginGadget::PlotMaskedBitmap(wxDC* destDC, wxBitmap* srcBitmap, INT32 SrcOffset,
								INT32 xPlotOffset,INT32 yPlotOffset, INT32 width, INT32 height)
{
	
PORTNOTE("ruler", "Removed use of Windows stuff")
#if !defined(EXCLUDE_FROM_XARALX)
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
#endif

	return TRUE;
}


/*********************************************************************************************
>	void OriginGadget::OnPaint( wxPaintEvent &evnt )

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Redraws the colour bar
	Errors:		-

**********************************************************************************************/ 
void OriginGadget::OnPaint( wxPaintEvent &evnt )
{
	// Always create this so that the area is validated
	wxPaintDC dc(this);

	if (!CCamApp::IsDisabled())
	{
		DoPaint(&dc);
	}
}



/********************************************************************************************
>	void OriginGadget::DoPaint(wxDC* pDC)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Purpose:	Paints the client area of the Origin Gadget
				Mask plots the origin glyph
********************************************************************************************/

void OriginGadget::DoPaint(wxDC* pDC)
{
PORTNOTE("ruler", "Removed use of Windows stuff")
#if !defined(EXCLUDE_FROM_XARALX)
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
#endif
}


/*********************************************************************************************
>	virtual void OriginGadget::CalcPosFromParentClient(WinRect* lpRect)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Inputs:		A long pointer to a rectangle which describes the dimensions of the client
				area of the window containing the gadget.
	Outputs:	Changes lpRect to the position a horizontal scroll bar would occupy
				normally, ie. along the bottom of the owning window.
	Purpose:	Called by CCamView when its size changes, so it knows where to reposition
				the OriginGadget.
**********************************************************************************************/ 

void OriginGadget::CalcPosFromParentClient(WinRect* lpRect)
{
	wxRect parentRect;
	parentRect = m_pOwnerView->GetParentFrame()->GetClientRect();
	if (RULER_BORDERS)
	{
	  parentRect.y --;
	  parentRect.x --;
	}
	lpRect->x   = parentRect.x ;
	lpRect->y    = parentRect.y;
	lpRect->SetRightEx(parentRect.x + OILRuler::GetWidth());
	lpRect->SetBottomEx(parentRect.y  + OILRuler::GetWidth());

	// we need to take into account camelots new floating toolbar ....

PORTNOTE("ruler", "Removed use of Kernel toolbars")
#if !defined(EXCLUDE_FROM_XARALX)
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
#endif
	
}


/********************************************************************************************
>	BOOL OriginGadget::Create(CCamView* pOwnerView, INT32 id)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		pParentWindow - 
	Returns:	FALSE if fails
	Purpose:	Creates an OriginGadget object.
********************************************************************************************/

BOOL OriginGadget::Create(CCamView* pOwnerView, INT32 id)
{ 
	ENSURE(pOwnerView,"DockingBar must have a Parent");

	m_pOwnerView = pOwnerView;

	if (!wxWindow::Create(pOwnerView->GetParentFrame(), id, wxDefaultPosition, wxSize(OILRuler::GetWidth(), OILRuler::GetWidth()), wxNO_BORDER))
		return(FALSE);

	return TRUE;
}


/********************************************************************************************
>	void OriginGadget::OnLButtonDown(wxMouseEvent& event);

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		nFlags - 
				point  - 
	Purpose:	handle left button downs
********************************************************************************************/
                    
void OriginGadget::OnLButtonDown(wxMouseEvent& event)
{
	PORTNOTE("ruler", "TODO: Set nFlags");
	UINT32 nFlags = 0;
	wxPoint point = event.GetPosition();

	String_256 OpToken(OPTOKEN_SPREADORIGIN);

	if (m_pOwnerView != NULL)
		m_pOwnerView->InvokeDragOp(&OpToken, NULL, nFlags, point);
}


/********************************************************************************************
>	void OriginGadget::OnRButtonUp(wxMouseEvent& event);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
	Inputs:		nFlags - 
				point  -
	Purpose:	Handle left button up events - pop-up context sensitive menu
********************************************************************************************/
                    
void OriginGadget::OnRButtonUp(wxMouseEvent& event)
{
	PORTNOTE("ruler", "TODO: Set nFlags");
//	UINT32 nFlags = 0;
	wxPoint point = event.GetPosition();

	OriginContextMenu* pOriginMenu = new OriginContextMenu;
	pOriginMenu->Show();
}


/********************************************************************************************
>	void OriginGadget::OnLButtonDblClk(wxMouseEvent& event);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95
	Inputs:		nFlags - 
				point  - 
	Purpose:	handle left button double clicks
********************************************************************************************/
                    
void OriginGadget::OnLButtonDblClk(wxMouseEvent& event)
{
	PORTNOTE("ruler", "TODO: Set nFlags");
//	UINT32 nFlags = 0;
	wxPoint point = event.GetPosition();

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
>	void OriginGadget::OnMouseMove(wxMouseEvent& event);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/95
	Inputs:		nFlags - 
				point  - 
	Purpose:	handle mouse moves over Origin
********************************************************************************************/
                    
void OriginGadget::OnMouseMove(wxMouseEvent& event)
{
	PORTNOTE("ruler", "TODO: Set nFlags");
//	UINT32 nFlags = 0;
	wxPoint MousePos = event.GetPosition();

	static wxPoint OldMousePos = wxPoint(0,0);
	if (MousePos==OldMousePos)
		return;

PORTNOTE("rulers", "Bubble help removed from OriginGadget")
#if !defined(EXCLUDE_FROM_XARALX)
	ControlHelper::DoBubbleHelpOn(GetSafeHwnd(), 0, BubbleHelpCallBack,this);
#endif

	StatusLine* pStatusLine=GetApplication()->GetpStatusLine();
	if (pStatusLine)
	{
		String_256 Text("");
		GetStatusLineText(&Text, WinCoord(MousePos.x, MousePos.y), this);
		pStatusLine->UpdateText(&Text, STATUSLINE_SELDESC_STATBAR);
	}

	OldMousePos = MousePos;
}


/******************************************************************************
>	static TCHAR* OriginGadget::BubbleHelpCallBack(CWindowID hWnd, UINT32 PaneIndex, void* UserData)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/95
	Purpose:	Bubble help call back handler
				(called some time after call to DoBubbleHelpOn() if mouse ha not moved)
	Inputs:		hWnd     - not used
				PaneID   - not used
				UserData - not used
	Returns:	pointer to text, or NULL if fails
******************************************************************************/

PORTNOTE("rulers", "Bubble help removed from OriginGadget")
#if !defined(EXCLUDE_FROM_XARALX)
TCHAR* OriginGadget::BubbleHelpCallBack(CWindowID, UINT32 PaneIndex, void*)
{
	static String_64 BubbleHelpText("");

	BubbleHelpText.Load(_R(IDS_ORIGIN_BH));

	return (TCHAR*)BubbleHelpText;
}
#endif


/******************************************************************************
>	BOOL OriginGadget::GetStatusLineText(String_256* pText, WinCoord MousePos, CWindowID hWnd)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/95
	Inputs:		MousePos - position in window (not used)
				hWnd     - handle of window
	Outputs:	pText    -
	Returns:	TRUE if ptext hold valid text, else FALSE if not over a pane
	Purpose:	if over origin gadget return status line help
******************************************************************************/

BOOL OriginGadget::GetStatusLineText(String_256* pText, WinCoord MousePos, CWindowID hWnd)
{
	ERROR2IF(pText==NULL,FALSE,"OILRuler::GetStatusLineText() - pText==NULL");

	if (this!=hWnd)
		return FALSE;

	return (pText->Load(_R(IDS_ORIGIN_SH))!=0);
}


/////////////////////////////////////////////////////////////////////////////////////////////
//		LegendLabel Class
/////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(LegendLabel, wxWindow)

BEGIN_EVENT_TABLE( LegendLabel, wxWindow )
	EVT_LEFT_DOWN(		LegendLabel::OnLButtonDown)
	EVT_LEFT_DCLICK(	LegendLabel::OnLButtonDblClk)
	EVT_RIGHT_UP(		LegendLabel::OnRButtonUp)
	EVT_MOTION(			LegendLabel::OnMouseMove)
	EVT_PAINT(			LegendLabel::OnPaint)
END_EVENT_TABLE()

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
	Show(show);
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


/*********************************************************************************************
>	void LegendLabel::OnPaint( wxPaintEvent &evnt )

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Redraws the colour bar
	Errors:		-

**********************************************************************************************/ 
void LegendLabel::OnPaint( wxPaintEvent &evnt )
{
	// Always create this so that the area is validated
	wxPaintDC dc(this);

	if (!CCamApp::IsDisabled())
	{
		DoPaint(&dc);
	}
}



/********************************************************************************************
>	afx_msg void LegendLabel::DoPaint(wxDC* pDC)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/95
	Purpose:	Paints the client area of the Legend Label
********************************************************************************************/

void LegendLabel::DoPaint(wxDC* pDC)
{
	// read the necessary colours
	wxColour BackCol  = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	wxColour TopLeft  = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT);
	wxColour BotRight = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW);
	wxColour TextCol  = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT);

  	// get info about client rect
	wxRect ClientRect = GetClientRect();
	INT32 left   = ClientRect.x;
	INT32 top    = ClientRect.y;
	INT32 width  = ClientRect.GetWidth();
	INT32 height = ClientRect.GetHeight();
	INT32 right  = left + width;
	INT32 bottom = top  + height;

	// render the background
	wxPen penNoOutline(BackCol, 0, wxTRANSPARENT);
	pDC->SetPen(penNoOutline);
	OILRuler::PatB(pDC, left,top, width,height, BackCol);
	
	// set up dc to paint transparent black text in correct font
	pDC->SetTextForeground(TextCol);
	pDC->SetBackgroundMode(wxTRANSPARENT);
	pDC->SetFont(OILRuler::GetRulerFont());

	// now draw the legend text formated into the client area
	wxString Text((LPCTSTR)LegendText);
	pDC->DrawText(Text, ClientRect.x, ClientRect.y);

	// paint hi/lo light
	OILRuler::PatB(pDC,  left,top,    width-1,1,        TopLeft);		// top
	OILRuler::PatB(pDC,  left,top,          1,height-1, TopLeft);		// left
	OILRuler::PatB(pDC,  left,bottom,   width,-1,       BotRight);	// bottom
	OILRuler::PatB(pDC, right,top,         -1,height,   BotRight);	// right
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
		Refresh();
		Update();
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
	BOOL ok=OILRuler::GetTextSize(&WinTextSize, (LPCTSTR)LegendText);
	if (!ok)
		return FALSE;

	LegendHeight = WinTextSize.GetHeight() +2;		// add pixel border top and bottom
	LegendWidth  = WinTextSize.GetWidth()  +2+7;	// add border + 3 pixel gap each end + pixel to right to balance 1 to left

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
	WinRect ParentRect;
	ParentRect = GetParent()->GetClientRect();
	
	// determine position of window
	WinRect LegendRect;
	LegendRect.y 	  = (ParentRect.GetHeight()-LegendHeight)/2;	// centre vertically
	LegendRect.SetBottomEx(ParentRect.GetBottomEx()-LegendRect.y);		// with same gap at bottom
	LegendRect.SetRightEx(ParentRect.GetRightEx()-(LegendRect.y+6));	// and this gap plus some from right end of ruler
	LegendRect.x   = LegendRect.GetRightEx()-LegendWidth;

	// actually move the window
	SetSize(LegendRect);

	return TRUE;
}


/********************************************************************************************
>	BOOL LegendLabel::Create(wxWindow* pParentWindow, INT32 id)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/94
	Inputs:		pParentWindow - 
	Returns:	FALSE if fails
	Purpose:	Creates an LegendLabel object.
********************************************************************************************/
                    
BOOL LegendLabel::Create(wxWindow* pParentWindow, INT32 id)
{ 
	ENSURE(pParentWindow,"DockingBar must have a Parent");

	if (!wxWindow::Create(pParentWindow, id, wxDefaultPosition, wxSize(10, 20), wxNO_BORDER))
		return(FALSE);

	return TRUE;
}


/********************************************************************************************
>	void LegendLabel::OnLButtonDown(wxMouseEvent& event);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95
	Inputs:		nFlags - 
				point  - 
	Purpose:	handle left button down events - pass on to parent ruler
********************************************************************************************/
                    
void LegendLabel::OnLButtonDown(wxMouseEvent& event)
{
	((OILHorizontalRuler*)GetParent())->OnLButtonDown(event);
}


/********************************************************************************************
>	void LegendLabel::OnRButtonUp(wxMouseEvent& event);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95
	Inputs:		nFlags - 
				point  - 
	Purpose:	handle left button up events -  pass on to parent ruler
********************************************************************************************/
                    
void LegendLabel::OnRButtonUp(wxMouseEvent& event)
{
	((OILHorizontalRuler*)GetParent())->OnRButtonUp(event);
}


/********************************************************************************************
>	void LegendLabel::OnLButtonDblClk(wxMouseEvent& event);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95
	Inputs:		nFlags - 
				point  - 
	Purpose:	handle left button double click events - pass on to parent ruler
********************************************************************************************/
                    
void LegendLabel::OnLButtonDblClk(wxMouseEvent& event)
{
	((OILHorizontalRuler*)GetParent())->OnLButtonDblClk(event);
}


/********************************************************************************************
>	void LegendLabel::OnMouseMove(wxMouseEvent& event);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/95
	Inputs:		nFlags - 
				point  - 
	Purpose:	handle mousemoves over ruler legend label
********************************************************************************************/
                    
void LegendLabel::OnMouseMove(wxMouseEvent& event)
{
	PORTNOTE("ruler", "TODO: Set nFlags");
//	UINT32 nFlags = 0;
	wxPoint MousePos = event.GetPosition();

	static wxPoint OldMousePos = wxPoint(0,0);
	if (MousePos==OldMousePos)
		return;

	StatusLine* pStatusLine=GetApplication()->GetpStatusLine();
	if (pStatusLine)
	{
		String_256 Text("");
		GetStatusLineText(&Text, WinCoord(MousePos.x, MousePos.y), this);
		pStatusLine->UpdateText(&Text,STATUSLINE_SELDESC_STATBAR);
	}

	OldMousePos = MousePos;
}


/******************************************************************************
>	BOOL LegendLabel::GetStatusLineText(String_256* pText, WinCoord MousePos, CWindowID hWnd)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/95
	Inputs:		MousePos - position in window (not used)
				hWnd     - handle of window
	Outputs:	pText    -
	Returns:	TRUE if ptext hold valid text, else FALSE if not over a pane
	Purpose:	if over a ruler legend return status line help
******************************************************************************/

BOOL LegendLabel::GetStatusLineText(String_256* pText, WinCoord MousePos, CWindowID hWnd)
{
	ERROR2IF(pText==NULL,FALSE,"LegendLabel::GetStatusLineText() - pText==NULL");

	if (this!=hWnd)
		return FALSE;

	return (pText->Load(_R(IDS_RULER_LEGEND_SH))!=0);
}


/********************************************************************************************
>	wxPoint LegendLabel::ClientToParentClient(wxPoint point)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/95
	Inputs:		point - 
	Returns:	the point relative to the parent window
	Purpose:	convert coord relative to this window to a coord relative to the parent window
	Note:		Nice if this was in a parent CCWnd class
********************************************************************************************/
                    
wxPoint LegendLabel::ClientToParentClient(wxPoint point)
{
	ClientToScreen(point);
	GetParent()->ScreenToClient(point);
	return point;
}

#endif	// WEBSTER
