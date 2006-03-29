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

// dragmgr.cpp - Implementation of global dragging system manager (DragManagerOp)

/*
*/


#include "camtypes.h"

#include "app.h"
#include "camelot.h"
#include "dialogop.h"
#include "dragmgr.h"
#include "keypress.h"
#include "oilcoord.h"
#include "camframe.h"
#include "cursor.h"
//#include "resource.h"
#include "gbrush.h"

#include "oilbitmap.h"

CC_IMPLEMENT_DYNCREATE(DragManagerOp, Operation)
IMPLEMENT_DYNAMIC_CLASS(CaptureWnd, wxFrame)

#define new CAM_DEBUG_NEW

// Define this to make drag captures use the methods in CCamFrame rather than creating a CaptureWnd
// This is currently required to make this stuff work on wxGTK.
#define DRAG_MAINFRAME

//------------------------------------------------------------------------------------------
//		CaptureWnd
//------------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------
// CaptureWnd  Message map - maintained by Class Wizard NOT !!!.

BEGIN_EVENT_TABLE( CaptureWnd, wxFrame )
	EVT_LEFT_UP(			CaptureWnd::OnLButtonUp )
	EVT_RIGHT_UP(			CaptureWnd::OnRButtonUp )
	EVT_MOTION(				CaptureWnd::OnMouseMove )
	EVT_WINDOW_CREATE(		CaptureWnd::OnWindowCreate)
END_EVENT_TABLE()


/********************************************************************************************

>	CaptureWnd::CaptureWnd()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95

	Purpose:	CaptureWnd constructor 

********************************************************************************************/

CaptureWnd::CaptureWnd()
{
	TRACEUSER("Gerry", _T("CaptureWnd::CaptureWnd"));
	pDisplayDC = NULL;
	BackBitmap = NULL;
	MaskBitmap = NULL;
}

/********************************************************************************************

>	CaptureWnd::~CaptureWnd()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95

	Purpose:	CaptureWnd destructor 

********************************************************************************************/

CaptureWnd::~CaptureWnd()
{
	TRACEUSER("Gerry", _T("CaptureWnd::~CaptureWnd"));
// 	// Need to clean up all the pointers here...
}



/********************************************************************************************

>	BOOL CaptureWnd::Create()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95

	Purpose:	Create a hidden window to capture all mouse input during drags  

********************************************************************************************/

BOOL CaptureWnd::Create()
{
	TRACEUSER("Gerry", _T("CaptureWnd::Create"));
	BOOL WindowCreated = FALSE;

	WindowCreated = wxFrame::Create(NULL, wxID_ANY, wxString(_T("")), wxPoint(0, 0), wxSize(20, 20), 0);
	ERROR2IF(!WindowCreated, FALSE, _T("Couldn't create a capture window for the drag manager"));

	// Defer this to OnCreate so it might work
//	CaptureMouse();

	Show();

	return TRUE;
}

//-------------------------------------------------------------------------------------------
// Capture Window Message Handlers
//-------------------------------------------------------------------------------------------

/*********************************************************************************************
>	void CaptureWnd::OnWindowCreate(wxWindowCreateEvent &event)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/03/06
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Redraws the colour bar
	Errors:		-

**********************************************************************************************/ 

void CaptureWnd::OnWindowCreate(wxWindowCreateEvent &event)
{
	TRACEUSER("Gerry", _T("CaptureWnd::OnWindowCreate"));
	CaptureMouse();
	event.Skip();
}


/********************************************************************************************

>	void CaptureWnd::OnLButtonUp(wxMouseEvent& event)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/01/95
	Inputs:		MFC
	Purpose:	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void CaptureWnd::OnLButtonUp(wxMouseEvent& event)
{
	TRACEUSER("Gerry", _T("CaptureWnd::OnLButtonUp"));
	DragManagerOp::EndDrag(1);// 1 == left click for now
	event.Skip();
}

/********************************************************************************************

>	void CaptureWnd::SetUpSolidDrag(CPoint StartPos)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/01/95
	Inputs:		-
	Purpose:	Sets up Device Contexts / Bitmaps etc.. for Solid Drag
	Errors:		-
	SeeAlso:	-

// ********************************************************************************************/

BOOL CaptureWnd::SetUpSolidDrag(wxPoint StartPos)
{
	TRACEUSER("Gerry", _T("SetUpSolidDrag(%d, %d)"), StartPos.x, StartPos.y);

	ERROR2IF(DragManagerOp::CurrentManager == NULL ||
			 DragManagerOp::CurrentManager->CurrentDragInfo == NULL,
			 FALSE,
			 _T("CaptureWnd::SetUpSolidDrag - The current drag manager is invalid"));

	if (!DragManagerOp::CurrentManager->CurrentDragInfo->DoesSolidDrag)
		return TRUE;

	if (DragManagerOp::CurrentManager->RedrawInProgress || pDisplayDC != NULL)
		return FALSE;

	// get a couple of draggy bits
	wxPoint SolidDragOffset = DragManagerOp::CurrentManager->CurrentDragInfo->SolidDragOffset;
	wxSize DSize = DragManagerOp::CurrentManager->CurrentDragInfo->SolidDragSize;

	pDisplayDC = new wxScreenDC();
	wxMemoryDC BackGroundDC;
	BackBitmap = new wxBitmap(DSize.x, DSize.y);
	
	if (pDisplayDC==NULL || BackBitmap == NULL)
	{
	 	return FALSE;
	}

	// Offset the drag from the pointer
	StartPos += SolidDragOffset;

	// select bitmap into the dc
	BackGroundDC.SelectObject(*BackBitmap);
	
#ifndef DRAG_MAINFRAME
	StartPos = ScreenToClient(StartPos);
#endif
	// init drag rect
	DragRect = wxRect(StartPos.x, StartPos.y, DSize.x, DSize.y);

	// blit the screen into the bitmap
	BackGroundDC.Blit(0,0,DSize.x,DSize.y,pDisplayDC,DragRect.x,DragRect.y);

	BackGroundDC.SelectObject(wxNullBitmap);

	INT32 DragTransparency = 
		DragManagerOp::CurrentManager->CurrentDragInfo->GetDragTransparency();

	KernelBitmap* DragMask = 
		DragManagerOp::CurrentManager->CurrentDragInfo->GetSolidDragMask();
	
	if (DragTransparency > 0 || DragMask != NULL)
 	{
		// If we're doing a transparency drag, then we'll need
		// a monochome mask bitmap.
		// This will be a simple grey level, hatch pattern,
		// which will be used to 'knock' pixels out of the
		// drag bitmap.

		wxMemoryDC MaskDC;
		MaskBitmap = new wxBitmap(DSize.x, DSize.y, 1);

		if (MaskBitmap == NULL)
		{
		 	return FALSE;
		}

		MaskDC.SelectObject(*MaskBitmap);

		// get a grey level hatched brush

		// DragTransparency is between 0 and 100,
		// and we need a grey level between 0 and 255.
		INT32 GreyLevel = (DragTransparency * 255) / 100;

		// create the Brush and	Pen
		wxBrush DragBrush(wxColour(GreyLevel, GreyLevel, GreyLevel), wxSOLID);
		wxPen MyPen(wxColour(0, 0, 0), 1, wxTRANSPARENT);
	
		// the rectangle to draw into
		wxRect DrawRect(0, 0, DSize.x, DSize.y);

		// select brushes and pens ..
		MaskDC.SetBrush(DragBrush);
		MaskDC.SetPen(MyPen);

	 	MaskDC.DrawRectangle(DrawRect);

PORTNOTE("other","Removed bitmap masking in drags")
#ifndef EXCLUDE_FROM_XARALX
		// Now combine the DragMask with the transparency mask.
		if (DragMask)
		{
			// This needs to create a wxBitmap from DragMask
			// and OR it into the MaskDC

			WinBitmap* pMaskBmp = (WinBitmap*)DragMask->ActualBitmap;
			RGBQUAD* Palette = (RGBQUAD *) (pMaskBmp->BMInfo->bmiColors);

			// set the first colours to black and white
			Palette[0].rgbRed = Palette[0].rgbBlue = Palette[0].rgbGreen = 0;
			Palette[1].rgbRed = Palette[1].rgbBlue = Palette[1].rgbGreen = 255;
	
			// set the reserved bytes to zero
			Palette[0].rgbReserved = Palette[1].rgbReserved = 0;

			wxMemoryDC MemDC;
			MemDC.CreateCompatibleDC(pMaskDC);

			// Make a Windows DDB from the Kernel Mask Bitmap
			HBITMAP hBmp = CreateDIBitmap(MemDC.m_hDC,
			 								(BITMAPINFOHEADER*)pMaskBmp->BMInfo,
											CBM_INIT,
											pMaskBmp->BMBytes,
											pMaskBmp->BMInfo,
											DIB_RGB_COLORS
			 								);

			// Get a CBitmap version to select into the MemoryDC
			CBitmap* WinMaskBmp = CBitmap::FromHandle(hBmp);
			CBitmap* OldBmp = MemDC.SelectObject(WinMaskBmp);

			// Now OR the Mask Bitmap into the MaskDC, so that
			// it 'masks' the transparency mask.
			MaskDC.Blit(DrawRect.x, DrawRect.y, DrawRect.width, DrawRect.height, &MemDC, 0, 0, wxOR);

			MemDC.SelectObject(OldBmp);
			WinMaskBmp->DeleteObject();
		}
#endif
	    // clean up the dc
	    MaskDC.SetBrush(wxNullBrush);
		MaskDC.SetPen(wxNullPen);
		MaskDC.SelectObject(wxNullBitmap);
	}
	else
		MaskBitmap = NULL;

	StartPos += wxPoint(-SolidDragOffset.x, -SolidDragOffset.y);
	DrawSolidDrag(StartPos);

	return TRUE;
}



/********************************************************************************************

>	BOOL CaptureWnd::CleanUpSolidDrag()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/01/95
	Inputs:		-

	Returns:	FALSE if it fails

	Purpose:	Removes any solid-drag stuff we may have on-screen at the moment and cleans up
				the solid drag system. Called at the end of a drag to clean up, and also
				whenever someone is redrawing underneath us, and we thus need to remove the 
				drag stuff while they redraw.
	Errors:		-
	SeeAlso:	CaptureWnd::CleanUpSolidDragInScreenArea

********************************************************************************************/

BOOL CaptureWnd::CleanUpSolidDrag()
{
	TRACEUSER("Gerry", _T("CleanUpSolidDrag"));

	if(!DragManagerOp::CurrentManager->CurrentDragInfo->DoesSolidDrag)
	   return TRUE;

	if (pDisplayDC == NULL)
		return FALSE;

	wxMemoryDC BackGroundDC;
	
	// select bitmap into the dc
	BackGroundDC.SelectObject(*BackBitmap);

	// remove the last drag draw (only if we drew something)
	if (!DragManagerOp::CurrentManager->DragPending)
	{
		pDisplayDC->Blit(DragRect.x,DragRect.y,DragRect.width, DragRect.height,&BackGroundDC,0,0);
	}
  	
  	// clean up  and delete the DC's
	BackGroundDC.SelectObject(wxNullBitmap);
  	
  	if (pDisplayDC)
	{
		delete pDisplayDC;
		pDisplayDC = NULL;
	}

	if (BackBitmap)
	{
		delete BackBitmap;
		BackBitmap = NULL;
	}

	if (MaskBitmap)
	{
		delete MaskBitmap;
		MaskBitmap = NULL;
	}

	DragRect = wxRect(0, 0, 0, 0);

	return TRUE;
}



/********************************************************************************************

>	BOOL CaptureWnd::CleanUpSolidDragInScreenArea(const wxRect& Area)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/95

	Inputs:		Area - The SCREEN coordinates of the area ofd screen which has become
				compromised (usually because it is being redrawn).

	Returns:	TRUE if it called CleanUpSolidDrag, FALSE if it didn't bother

	Purpose:	Calls CleanUpSolidDrag. However, it ONLY calls that method if the
				compromised screen area overlaps the solid drag area - if not, then the
				solid drag stuff can't be overwritten, so there is no need to remove it.
				This can drastically reduce flicker when things are background redrawing
				on other parts of the screen.

	SeeAlso:	CaptureWnd::CleanUpSolidDrag

********************************************************************************************/

BOOL CaptureWnd::CleanUpSolidDragInScreenArea(const wxRect& Area)
{
	TRACEUSER("Gerry", _T("CleanUpSolidDragInScreenArea"));

	// No solid drag, so no need to do anything
	if (!DragManagerOp::CurrentManager->CurrentDragInfo->DoesSolidDrag)
	   return(FALSE);

	// No solid stuff has yet been drawn, so no need to do anything
	if (DragManagerOp::CurrentManager->DragPending || DragRect.IsEmpty())
		return(FALSE);

	wxRect Isect(Area);
	if (!Isect.Intersect(DragRect).IsEmpty())
	{
		CleanUpSolidDrag();
		return(TRUE);
	}

	return(FALSE);
}



/********************************************************************************************

>	void CaptureWnd::OnMouseMove(wxMouseEvent& event)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/01/95
	Inputs:		MFC
	Purpose:	call solid drag draw code.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CaptureWnd::OnMouseMove(wxMouseEvent& event)
{
//	TRACEUSER("Gerry", _T("CaptureWnd::OnMouseMove"));
	// Abort if the system has been disabled (for an error box)
	if (CCamApp::IsDisabled())
	{
//		DragManagerOp::AbortDrag();		// This could be deeply scary - it mustn't cause a redraw!
		return;
	}

	if (!DragManagerOp::CurrentManager->RedrawInProgress)
	{
		wxPoint point = event.GetPosition();
#ifdef DRAG_MAINFRAME
		point = GetMainFrame()->ClientToScreen(point);
#else
		point = ClientToScreen(point);
#endif
		DrawSolidDrag(point);
	}
}



/********************************************************************************************

>	void CaptureWnd::DrawSolidDrag(wxPoint point)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/01/95
	Inputs:		Screen Coord
	Purpose:	to allow solid flicker-free drag objects to be drawn.
				a helper function in the current drag is called - but this function
				looks after the rest.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CaptureWnd::DrawSolidDrag(wxPoint point)
{
	TRACEUSER("Gerry", _T("DrawSolidDrag(%d, %d)"), point.x, point.y);

	if (HasCapture())
	{
		TRACEUSER("Gerry", _T("Has got capture"));
	}
	else
	{
		TRACEUSER("Gerry", _T("Hasn't got capture"));
	}

	ERROR2IF(DragManagerOp::CurrentManager == NULL ||
			 DragManagerOp::CurrentManager->CurrentDragInfo == NULL,
			 FALSE,
			 "No current drag in CaptureWnd::DrawSolidDrag!");
	
	// not interested ? ...
	if(DragManagerOp::CurrentManager->DragPending)
	   	return TRUE;
	if(!DragManagerOp::CurrentManager->CurrentDragInfo->DoesSolidDrag)
	   	return TRUE;

	INT32 DragTransparency = DragManagerOp::CurrentManager->CurrentDragInfo->
															GetDragTransparency();
	// If the Drag Info says it wants to be transparent,
	// then call the transparent drag routine.
	if (DragTransparency > 0 || MaskBitmap != NULL)
	{
		if (DrawTransparentDrag(point, DragTransparency))
			return TRUE;

		// If the Transparency Drag fails (eg. not enough resources under Win32s !!)
		// then just fall though, and try a normal solid drag .....
	}

	// offset mouse pos by drag offset
	point += DragManagerOp::CurrentManager->CurrentDragInfo->SolidDragOffset;

	// size of solid drag draw bounds
	wxSize DSize = DragManagerOp::CurrentManager->CurrentDragInfo->SolidDragSize;

	// create a few	DCs !!!
	
	// this one will hold the old background
	wxMemoryDC BackDC;
	
	// select bitmap into the dc
	BackDC.SelectObject(*BackBitmap);

	// this one is just temp
	wxMemoryDC ScratchDC;
	wxBitmap ScratchBit(DSize.x, DSize.y);

	// select bitmap into the dc
	ScratchDC.SelectObject(ScratchBit);
	
	// make copy of last rect
	wxRect OldRect(DragRect);
	
	// set new drag draw bounds
	DragRect = wxRect(point.x, point.y, DSize.x, DSize.y);
	
	// Copy screen to new background
	ScratchDC.Blit(0,0,DSize.x,DSize.y,pDisplayDC,DragRect.x,DragRect.y);
	
	wxPoint Change = OldRect.GetPosition() - DragRect.GetPosition();
	
	// Replace part of new bkg with old background
	if (!OldRect.IsEmpty())
		ScratchDC.Blit(Change.x,Change.y,DSize.x,DSize.y,&BackDC,0,0);

	// Copy image to screen
	DragManagerOp::CurrentManager->CurrentDragInfo->
		OnDrawSolidDrag(DragRect.GetPosition(), pDisplayDC);

	// Copy part of image to old background
	DragManagerOp::CurrentManager->CurrentDragInfo->
		OnDrawSolidDrag(wxPoint(-Change.x,-Change.y), &BackDC);
	
	// Copy old background to screen
	if (!OldRect.IsEmpty())
		pDisplayDC->Blit(OldRect.x,OldRect.y,DSize.x,DSize.y,&BackDC,0,0);

	// copy new background into old for next time round
	BackDC.Blit(0,0,DSize.x,DSize.y,&ScratchDC,0,0);

	// clean up and delete scratch dc and bitmap
	ScratchDC.SelectObject(wxNullBitmap);

	BackDC.SelectObject(wxNullBitmap);

	return TRUE;
}


/********************************************************************************************

>	void CaptureWnd::DrawTransparentDrag(wxPoint point)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/03/95
	Inputs:		Screen Coord
	Purpose:	to allow solid flicker-free semi-transparent drag objects to be drawn.
				a helper function in the current drag is called - but this function
				looks after the rest.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CaptureWnd::DrawTransparentDrag(wxPoint point, INT32 Transparency)
{
	TRACEUSER("Gerry", _T("DrawTransparentDrag(%d, %d, %d)"), point.x, point.y, Transparency);

	// offset mouse pos by drag offset
	point += DragManagerOp::CurrentManager->CurrentDragInfo->SolidDragOffset;

	// size of solid drag draw bounds
	wxSize DSize = DragManagerOp::CurrentManager->CurrentDragInfo->SolidDragSize;

  	// create a few	DCs !!!
	// this one will hold the old background
 	wxMemoryDC BackDC;

	// select bitmap into the dc
	BackDC.SelectObject(*BackBitmap);

	// this one is just temp
	wxMemoryDC ScratchDC;
	wxBitmap ScratchBit(DSize.x,DSize.y);

	wxBitmap TempBitmap(DSize.x, DSize.y);
	wxMemoryDC TempDC;

	wxMemoryDC MaskDC;

	// select bitmap into the dc
	ScratchDC.SelectObject(ScratchBit);
	
	// make copy of last rect
	wxRect OldRect = DragRect;
	
	// set new drag draw bounds
	DragRect = wxRect(point.x,point.y,DSize.x,DSize.y);
	
	// Copy screen to new background
	ScratchDC.Blit(0,0,DSize.x,DSize.y,pDisplayDC,DragRect.x,DragRect.y);

	wxPoint Change = OldRect.GetPosition() - DragRect.GetPosition();

	// Replace part of new bkg with old background
	if (!OldRect.IsEmpty())
		ScratchDC.Blit(Change.x,Change.y,DSize.x,DSize.y,&BackDC,0,0);

	TempDC.SelectObject(TempBitmap);

	// Render into the temporary bitmap
	DragManagerOp::CurrentManager->CurrentDragInfo->
		OnDrawSolidDrag(wxPoint(0,0), &TempDC);

	// set the colours for the masking
//	TempDC.SetBkColor(RGB(0,0,0));
//	TempDC.SetTextColor(RGB(255,255,255));

	MaskDC.SelectObject(*MaskBitmap);

	TempDC.Blit(0, 0, DSize.x, DSize.y, &MaskDC, 0, 0, wxAND);

	wxBitmap OffScreenBmp(DSize.x, DSize.y);
	wxMemoryDC OffScreenDC;
	OffScreenDC.SelectObject(OffScreenBmp);

	OffScreenDC.Blit(0, 0, DSize.x, DSize.y, &ScratchDC, 0, 0);

	OffScreenDC.Blit(0, 0, DSize.x, DSize.y, &MaskDC, 0, 0, wxAND);

	OffScreenDC.Blit(0, 0, DSize.x, DSize.y, &TempDC, 0, 0, wxOR);

	// Copy part of image to old background
	BackDC.Blit(-Change.x,-Change.y, DSize.x, DSize.y, &OffScreenDC, 0,0);
	
	// Copy image to screen
	pDisplayDC->Blit(DragRect.x, DragRect.y, DSize.x, DSize.y, &OffScreenDC, 0, 0);

	// Copy old background to screen
	if(!OldRect.IsEmpty())
		pDisplayDC->Blit(OldRect.x, OldRect.y, DSize.x, DSize.y, &BackDC, 0, 0);
	
	// copy new background into old for next time round
	BackDC.Blit(0, 0, DSize.x, DSize.y, &ScratchDC, 0, 0);

	// clean up and delete DCs and bitmaps
	OffScreenDC.SelectObject(wxNullBitmap);
	TempDC.SelectObject(wxNullBitmap);
	MaskDC.SelectObject(wxNullBitmap);
	ScratchDC.SelectObject(wxNullBitmap);
	BackDC.SelectObject(wxNullBitmap);

	return TRUE;
}

/********************************************************************************************

>	void CaptureWnd::OnRButtonUp(wxMouseEvent& event)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/01/95
	Inputs:		MFC
	Purpose:	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void CaptureWnd::OnRButtonUp(wxMouseEvent& event)
{
	TRACEUSER("Gerry", _T("CaptureWnd::OnRButtonUp"));
	DragManagerOp::EndDrag(-1);//	-1 == Right click for now
	event.Skip();
}




//------------------------------------------------------------------------------------------
//				Drag ManagerOp
//------------------------------------------------------------------------------------------

// -----------------   STATICS

DragManagerOp *DragManagerOp::CurrentManager = NULL;
CaptureWnd  *DragManagerOp::TheCaptureWindow = NULL;

// Drag delay and distance. These are not the suggested OLE-2 default values, because
// those values are mindbogglingly dumb (2 pixels, which makes it dead easy to accidentally
// start a drag if the mouse wobbles, and 0.2 secs which means any non-fast click
// will always be a drag).
const UINT32 DEFAULT_DRAGDIST	 = 6;	// 6 pixel "radius" outside which click becomes drag
const UINT32 DEFAULT_DRAGDELAY = 400;	// 400 millisecond (0.4 sec) delay before click=>drag
UINT32 DragManagerOp::DragMinDist	= DEFAULT_DRAGDIST;
UINT32 DragManagerOp::DragDelay	= DEFAULT_DRAGDELAY;

MonotonicTime DragManagerOp::DragStartTimer;
BOOL DragManagerOp::DragPending = FALSE;
BOOL DragManagerOp::DragEnded = FALSE;
BOOL DragManagerOp::DragActive = FALSE;

wxRect DragManagerOp::DragStartRect;
wxRect DragManagerOp::StillClickRect;

BOOL DragManagerOp::RedrawInProgress = FALSE;

UINT32 DragManagerOp::StatusLineStringID = 0;

DragInformation * DragManagerOp::CurrentDragInfo = NULL;	// Descriptor of current drag
DragTarget * DragManagerOp::CurrentDragTarget = NULL;		// Current drag target



/********************************************************************************************

>	DragManagerOp::DragManagerOp()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95

	Purpose:	DragManagerOp constructor - DO NOT CALL THE DEFAULT CONSTRUCTOR!

********************************************************************************************/

DragManagerOp::DragManagerOp()
{
	ERROR3("DragManagerOp::DragManagerOp - Illegal (default) constructor called!\n");

	ERROR3IF(CurrentManager != NULL, "Attempt to start a drag while already dragging!");
	CurrentManager = this;

	CurrentID = 0;
	CurrentCursor = NULL;
}



/********************************************************************************************

>	DragManagerOp::DragManagerOp(DragInformation *Descriptor)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95

	Inputs:		Descriptor - Describes the current drag. May not be NULL.

	Purpose:	DragManagerOp constructor

********************************************************************************************/

DragManagerOp::DragManagerOp(DragInformation *Descriptor)
{
	ERROR3IF(Descriptor == NULL, "DragManagerOp must be given a valid DragInformation ptr");
	ERROR3IF(CurrentManager != NULL, "Attempt to start a drag while already dragging!");

	AbortDrag();
	CurrentManager	= this;
	CurrentDragInfo	= Descriptor;

	CurrentKeypress	= NULL;
	
	DragEnded = FALSE;

	CurrentID = 0;
	CurrentCursor = NULL;

	// get system drag start values. We override the default values suggested by the
	// OLE2 docs, because their values are stupid. (2 pixels? Bleedin' heck! I wish MY
	// mouse was that steady!). However, we red the values from Win.INI like good boys, so
	// the user can override them if they so desire.
//	DragMinDist = GetProfileInt("windows", "DragMinDist", DEFAULT_DRAGDIST);
//	DragDelay = GetProfileInt("windows", "DragDelay", DEFAULT_DRAGDELAY);

	DragMinDist = DEFAULT_DRAGDIST;
	DragDelay = DEFAULT_DRAGDELAY;

	DragStartRect = wxRect();
}



/********************************************************************************************

>	DragManagerOp::~DragManagerOp()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95

	Purpose:	DragManagerOp destructor

********************************************************************************************/

DragManagerOp::~DragManagerOp()
{
	// Ensure all memory allocations are deleted
	CleanUpAfterDrag();

	// We are no longer the current drag manager
	CurrentManager = NULL;

	DragActive = FALSE;

	StatusLineStringID = 0; 

	if (CurrentCursor)
		delete CurrentCursor;
}



/********************************************************************************************

>	static CPoint DragManagerOp::GetDragMousePos()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95

	Purpose:	return a CPoint for the Last Mouse Position

********************************************************************************************/

wxPoint DragManagerOp::GetDragMousePos()
{
	wxPoint MPos(0,0);
	
	if( CurrentManager != NULL )
		MPos = CurrentManager->CurrentMousePos;
	return MPos;
}


/********************************************************************************************

>	static CaptureWnd * DragManagerOp::GetDragCaptureWnd()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95

	Purpose:	return a CWnd * for the Capture Window

********************************************************************************************/

CaptureWnd *DragManagerOp::GetDragCaptureWnd()
{
	if( CurrentManager == NULL )
		return NULL;
	
	return TheCaptureWindow;
}

/********************************************************************************************

>	static CaptureWnd * DragManagerOp::GetCurrentDragInfo()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95

	Purpose:	return Current drag information

********************************************************************************************/

DragInformation * DragManagerOp::GetCurrentDragInfo()
{
	if( CurrentManager == NULL )
		return NULL;
	
	return CurrentDragInfo;

}



/********************************************************************************************

>	static void DragManagerOp::StartDrag(DragInformation *Descriptor)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95

	Inputs:		Descriptor - A DragInformation object describing the current drag.
				NOTE WELL that this is given to the DragManager, who is then responsible
				for deleting it when the drag completes. DO NOT DELETE IT!

	Purpose:	Starts a global-drag off
				This will create a new DragManagerOp object, set up the drag, and
				broadcast a DragMsg DRAGSTARTED, as a result of which, DragTargets
				should be created by interested parties (these will register with the
				Drag Manager, and then be called to handle events during the drag)

********************************************************************************************/

void DragManagerOp::StartDrag(DragInformation *Descriptor)
{
	if (Descriptor == NULL)
	{
		// Handle a NULL pointer by just not starting up the drag
		TRACE( _T("NULL Descriptor given to DragManagerOp::StartDrag()\n"));
		return;
	}

	ERROR3IF(Descriptor == NULL, "DragManagerOp must be given a valid DragInformation ptr");

	DragManagerOp* pNewManager = new DragManagerOp(Descriptor);

	if (Descriptor->IsAdjustDrag)		// Adjust drags just immediately become clicks
	{
		DragPending = TRUE;				// Make sure it turns into a click properly (see EndDrag)
		if (pNewManager != NULL)
		{
			pNewManager->CurrentMousePos = wxGetMousePosition();
			pNewManager->InitialMousePos = pNewManager->LastMousePos = pNewManager->CurrentMousePos;

		   	EndDrag(-1);
		}
		return;
	}

	TheCaptureWindow = new CaptureWnd();
	
	if (pNewManager != NULL && TheCaptureWindow != NULL)
	{
		DragPending = TRUE;	  
		
		// Remember where the mouse is at the start of the drag
		pNewManager->CurrentMousePos = wxGetMousePosition();
		pNewManager->InitialMousePos = pNewManager->LastMousePos = pNewManager->CurrentMousePos;

		// Request that all interested parties attach DragTargets now
		BROADCAST_TO_ALL(DragMessage(DragMessage::DRAGSTARTED, pNewManager, Descriptor));

		// views don't receive messages so we'll have to do this the hard way..
		// App->Document->View... 
		GetApplication()->CreateDragTargets(Descriptor);
		
		// Send an Initialise event to all registered targets
#ifdef _DEBUG
		if (pNewManager->Targets.IsEmpty())
			TRACE( _T("DragManagerOp::StartDrag - No drag targets specified for this drag!"));
#endif
		pNewManager->ProcessEvent(DRAGEVENT_INITIALISE);		
		 
		// Create a hidden window to capture mouse events
#ifdef DRAG_MAINFRAME
		GetMainFrame()->StartDragManagerDrag(TheCaptureWindow);
#else
		TheCaptureWindow->Create();
#endif

		// Register for idle events
		GetApplication()->RegisterIdleProcessor(IDLEPRIORITY_HIGH, pNewManager);

		// get drag start time
		DragStartTimer.Sample();

		// we will use this rect to test whether we have started a drag
		StillClickRect = wxRect( pNewManager->InitialMousePos.x - DragMinDist,
								pNewManager->InitialMousePos.y - DragMinDist,
								DragMinDist * 2,
								DragMinDist * 2);
		SetDragActive(TRUE);
	}
	GetMainFrame()->SetFocus();
}



/********************************************************************************************

>	static void DragManagerOp::EndDrag(INT32 Flags)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95
	Purpose:
	Input  :  Flags : mouse button state( 1=leftclick/-1= rightclick )	

********************************************************************************************/

void DragManagerOp::EndDrag(INT32 Flags)
{
	if(!CurrentManager)
		return;

	// clean up
	if (TheCaptureWindow != NULL)
		TheCaptureWindow->CleanUpSolidDrag();

	if(DragPending)		 // not a drag - user intended a click
	{
		// call the click handler in the drag
		wxPoint StartMouse;
		StartMouse.x = CurrentManager->InitialMousePos.x;
		StartMouse.y = CurrentManager->InitialMousePos.y;
		CurrentManager->CurrentDragInfo->OnClick(Flags,StartMouse);   	
	}
	else 				// this is a genuine end of drag
	{
		// Update the mouse position, so we give the event to the correct target
		CurrentManager->LastMousePos = CurrentManager->CurrentMousePos;
		CurrentManager->CurrentMousePos = wxGetMousePosition();

		// Process the drag completion event
	 	DragEventType Event = DRAGEVENT_COMPLETED;
	 	CurrentManager->ProcessEvent(Event);

		// make sure we don't get any idles after the drag has finished
		DragEnded = TRUE;
	}

	// Let all drag targets know that the drag is well and truly dead
	if (CurrentManager)
		CurrentManager->ProcessEvent(DRAGEVENT_DEINITIALISE);

   	SetDragActive(FALSE);

	// end this op
	if (CurrentManager)
	   	CurrentManager->End();
}


/********************************************************************************************

>	static void DragManagerOp::AbortDrag(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95

	Purpose:	To forcibly abort the current drag. If there is no current drag, then
				nothing happens (so it can be called just to ensure that no drag is
				left current on exit for example)

********************************************************************************************/

void DragManagerOp::AbortDrag(void)
{
	if (CurrentManager != NULL)
	{
		CurrentManager->LastMousePos = CurrentManager->CurrentMousePos;
		CurrentManager->CurrentMousePos = wxGetMousePosition();

		if (TheCaptureWindow != NULL)
			TheCaptureWindow->CleanUpSolidDrag();

		CurrentManager->ProcessEvent(DRAGEVENT_ABORT);

		// Let all drag targets know that the drag is well and truly dead
		CurrentManager->ProcessEvent(DRAGEVENT_DEINITIALISE);

		CurrentManager->End();
	}

	SetDragActive(FALSE);	
}



/********************************************************************************************

>	static DragManagerOp *DragManagerOp::GetCurrentManager(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95

	Returns:	NULL (if no current drag), or a pointer to the current DragManagerOp

	Purpose:	To find the current drag manager

********************************************************************************************/

DragManagerOp *DragManagerOp::GetCurrentManager(void)
{
	return(CurrentManager);
}



/********************************************************************************************

>	virtual BOOL DragManagerOp::ProcessEvent(DragEventType Event)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95

	Inputs:		Event - Indicates what type of Drag Event is occurring

	Returns:	TRUE if a DragTarget claimed the event

	Purpose:	To process global drag events 
				This method calls each DragTarget event processor in turn (until one
				claims the event), with the event. The 3 'Current' member variables are
				passed in to the event processor, so ensure they are set up appropriately
				before calling this method.

	Notes:		DragTargets can safely de-register themselves during processing of the
				events (i.e. the list traversal code won't get upset)

				When calling a kernel target which includes both a dialogue ID and a gadget,
				the coordinates passed into the event handler are millipoint offsets from
				the bottom left corner of the gadget, just as with normal kernel-rendered
				gadgets. Note that the event will not be passed to the gadget unless the
				mouse pointer is over the gadget window. If no gadget is specified, then
				the mouse coordinates will always be (0,0) at present.

				The INITIALISE and DEINITIALISE events are broadcast to all active targets,
				while all other events are broadcast only to general (no target window)
				targets, and the target (if any) over which the mouse pointer lies.

	SeeAlso:	keyword DragEventType
	SeeAlso:	DragTarget::ProcessEvent

********************************************************************************************/

BOOL DragManagerOp::ProcessEvent(DragEventType Event)
{
	BOOL BroadcastToAll = FALSE;		// Determine if the event goes to everybody or
									// only to target under the pointer
	if (Event == DRAGEVENT_INITIALISE || Event == DRAGEVENT_DEINITIALISE)
		BroadcastToAll = TRUE;

	if(CurrentManager == NULL || (DragEnded && Event != DRAGEVENT_DEINITIALISE))
		return FALSE;

	LastEvent = Event;			// Remember the type of the last event we processed
	
	DragTarget *Ptr  = (DragTarget *) Targets.GetHead();
	DragTarget *Next;

	OilCoord KernelMousePos(0,0);
	wxPoint   WinoilMousePos(CurrentMousePos);
	CurrentDragTarget = NULL;
	BOOL OverTarget = FALSE;

	while (Ptr != NULL)
	{
		// Copy the mouse position, as each iteration of the loop corrupts it
		WinoilMousePos = CurrentMousePos;

		// Allow things like targets de-registering during processing
		Next = (DragTarget *) Targets.GetNext(Ptr);
		BOOL GoAhead = TRUE;
		
		if (Ptr->IsAKernelObject())
		{
			KernelMousePos = OilCoord(0,0);

			DialogOp  *pDialogOp;
			CGadgetID GadgetID;
			Ptr->GetTargetAreaInfo(&pDialogOp, &GadgetID);

			if (pDialogOp != NULL)
			{
				wxWindow* TargetWindow = (wxWindow*)pDialogOp->WindowID;	// use whole window if no gadget specified

				if (GadgetID != 0)
					TargetWindow = TargetWindow->FindWindow((INT32)GadgetID);

				wxRect TargetRect = TargetWindow->GetRect();

				if (BroadcastToAll || Ptr->WantsAllEvents() ||
					TargetRect.Inside(WinoilMousePos))
				{
					// Determine if the pointer is over the target window, or any of its children
					wxWindow* WindowUnderPoint = wxFindWindowAtPoint(WinoilMousePos);
					BOOL AreOverTargetWnd = (WindowUnderPoint == TargetWindow);

// wx doesn't have ChildWindowFromPoint
#ifndef EXCLUDE_FROM_XARALX
					if (!AreOverTargetWnd)
					{
						// We're not immediately over the background of the window, but may be over
						// a child-window of our window! 

						// Get the mouse position in client coords
						wxPoint ClientPoint;
						ClientPoint.x = WinoilMousePos.x;
						ClientPoint.y = WinoilMousePos.y;
						ClientPoint = TargetWindow->ScreenToClient(ClientPoint);

						wxWindow* ChildWindowUnderPoint = TargetWindow->ChildWindowFromPoint(ClientPoint);
						AreOverTargetWnd = (ChildWindowUnderPoint != NULL &&
												ChildWindowUnderPoint == WindowUnderPoint);
					}
#endif	// FALSE
					if (BroadcastToAll || Ptr->WantsAllEvents() || AreOverTargetWnd)
					{
						INT32 DPI = 96;
#ifndef EXCLUDE_FROM_XARALX
						// Get the screen DPI
						HDC ScreenDC = CreateCompatibleDC(NULL);
						if (ScreenDC == NULL)
						{
							ERROR2(FALSE, "DragManagerOp::ProcessEvent: Unable to create screen DC");
						}

						DPI = GetDeviceCaps(ScreenDC, LOGPIXELSY);
						DeleteDC(ScreenDC);
#endif
						KernelMousePos.x = ((WinoilMousePos.x - TargetRect.GetLeft()) * 72000) / DPI;
// CHECKRECT: This may need to be the exclusive bottom coord
						KernelMousePos.y = ((TargetRect.GetBottom() - WinoilMousePos.y) * 72000) / DPI;
					}
					else
						GoAhead	= FALSE;
				}
				else
					GoAhead = FALSE;
			}

			if (GoAhead &&
				Ptr->ProcessEvent(Event, CurrentDragInfo, &KernelMousePos, CurrentKeypress))
			{
			 	CurrentDragTarget = Ptr;
				OverTarget = TRUE;	// This Target claimed the event, so return TRUE

				if (!BroadcastToAll)
					break;
			}
		}
		else
		{
			wxWindow* TargetWindow;
			wxRect TargetRect;
			Ptr->GetTargetAreaInfo(&TargetWindow, &TargetRect);

			wxPoint ClientPoint;					// This will be screen coords, or will end up as 
			ClientPoint.x = WinoilMousePos.x;	// client coords if we have a window... 
			ClientPoint.y = WinoilMousePos.y;

			if (TargetWindow != NULL)
			{
				// Get the mouse position in client coords
				ClientPoint = TargetWindow->ScreenToClient(ClientPoint);

				// Don't bother giving the event to targets which don't contain the pointer
				if (BroadcastToAll || Ptr->WantsAllEvents() ||
					TargetRect.Inside(ClientPoint))
				{
					// Don't give the event to oil targets unless the pointer is over the
					// window (ie dont pass on events to overlapped windows) unless we want
					// to broadcast to all, or this target really wants to know!
					
					wxWindow* WindowUnderPoint = wxFindWindowAtPoint(WinoilMousePos);
					BOOL AreOverTargetWnd = (WindowUnderPoint == TargetWindow);

// wx doesn't have ChildWindowFromPoint
#ifndef EXCLUDE_FROM_XARALX
					if (!AreOverTargetWnd)
					{
						// We're not immediately over the background of the window, but may be over
						// a child-window of our window! 
						HWND ChildWindowUnderPoint = ChildWindowFromPoint(TargetWindow, ClientPoint);
						AreOverTargetWnd = (ChildWindowUnderPoint != NULL &&
												ChildWindowUnderPoint == WindowUnderPoint);
					}
#endif
					if (!BroadcastToAll && !Ptr->WantsAllEvents() && !AreOverTargetWnd)
					{
						GoAhead	= FALSE;
					}
				}
				else
					GoAhead = FALSE;
			}
			
			// we are in a target area, or this is a broadcast-to-all, so send the event
			if (GoAhead)
			{
				wxPoint PointInWindow(ClientPoint);	// "Cast" to wxPoint

				if (Ptr->ProcessEvent(Event, CurrentDragInfo, &PointInWindow, CurrentKeypress))
				{
					CurrentDragTarget = Ptr;
					OverTarget = TRUE;	// This Target claimed the event, so return TRUE

					if (!BroadcastToAll)
						break;
				}
			}
		}

		Ptr = Next;
	}

	if (!DragPending && Event != DRAGEVENT_COMPLETED)
	{ 
		SetStatusLineText();
		SetCursor();
	}	

	return OverTarget;	// Return status to indicate wheteher or not the event was claimed
}

		 

/********************************************************************************************

>	virtual BOOL DragManagerOp::OnIdleEvent(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> & Chris
	Created:	9/1/95

	Returns:	TRUE if it does not want low-priority idle handlers to be called this time
				FALSE if it's OK to call low-priority idles (i.e. if I'm not busy this turn)

	Purpose:	To process idle events. If the mouse hasn't moved, then we'll allow low
				priority handlers a look-in. This means that (e.g.) when dragging background-
				redrawing galleries, we get instant response while dragging, and the
				background redraw will only occur while the mouse is held still.

********************************************************************************************/

BOOL DragManagerOp::OnIdleEvent(void)
{
	LastMousePos = CurrentMousePos;
	CurrentMousePos = wxGetMousePosition();

	if(DragEnded)
		return(FALSE);	// Done nothing, so let low-priority handlers have a go

	BOOL JustStartedDrag = FALSE;

	// We have received a start drag message but we are not sure whether this is the real 
	// thing yet.
	if (DragPending)
	{
	 	// We start a drag if DragDelay milliseconds have elapsed
		// or the pointer has left DragStartRect
	 	if (!DragStartTimer.Elapsed(DragDelay) &&
	 		StillClickRect.Inside(CurrentMousePos))
		{
			// The drag is still pending
			return(FALSE);	// Done nothing, so let low-priority handlers have a go
		}

	 	DragPending = FALSE;
		JustStartedDrag = TRUE;	// Flag that we have "just turned on" the drag

		TheCaptureWindow->SetUpSolidDrag(CurrentMousePos);
	}
	


	// Determine what event type to send around...
	DragEventType Event = DRAGEVENT_MOUSEIDLE;
	if (LastMousePos == CurrentMousePos)
	{
		if (LastEvent == DRAGEVENT_MOUSESTOPPED || LastEvent == DRAGEVENT_MOUSEIDLE)
		{
			// The mouse has not moved for a while, so send another idle
			Event = DRAGEVENT_MOUSEIDLE;
		}
		else
		{
			// The mouse has only just stopped moving, so send a mouse-stopped
			Event = DRAGEVENT_MOUSESTOPPED;
		}
	}
	else
	{
		// The mouse has moved since we last checked, so send a mouse-moved
		Event = DRAGEVENT_MOUSEMOVED;
	}

	// If the mouse moved, then we will not allow low-priority idle processors to have a go
	// this time around, for maximum mouse-move interactiveness.
	BOOL ClaimTheIdle = (Event == DRAGEVENT_MOUSEMOVED);

// **** !!!! ToDo: This could be considered bodgy and nasty, mostly because it is.

// ToDo: Somehow detect if the drag has ended? A bodge I know, but we wanna compile
// this and see if it works!

if (KeyPress::IsEscapePressed())	// If escape (or equivalent) pressed, abort the drag
{
	AbortDrag();
	return(FALSE);	// Let low-priority handlers have a go
}

// **** 

	ProcessEvent(Event);	// Pass the event around all registered DragTargets

#if FALSE
/* On the first update, the mouse may not have moved, so we need to pop up the
   solid drag stuff. Unfortunately, this doesn't work quite right and I don't
   have time to deal with it right now.

	if (!DragEnded && JustStartedDrag)
	{
		// The mouse may not have moved, but we have just 'turned on' the drag - we'd
		// better ensure that the solid drag stuff is drawn onto screen for the fist time

		if (TheCaptureWindow != NULL)
		{
			CPoint PointInCaptureWnd(CurrentMousePos);

			// Convert the current mouse coordinate into capture window client coords,
			// and call it to update the solid drag
			TheCaptureWindow->ScreenToClient(&PointInCaptureWnd);
			TheCaptureWindow->DrawSolidDrag(PointInCaptureWnd);
		}
	}
*/
#endif

	return(ClaimTheIdle);	// Return, allowing or not-allowing low-priority handlers to have a go
							// depending on whether the mouse moved or not.
}



/********************************************************************************************

>	void DragManagerOp::RegisterTarget(DragTarget *NewTarget, BOOL HighPriority = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95

	Inputs:		NewTarget - points at the DragTarget object to add. NOTE that from this
				time on, the manager owns the target, and it will delete it when the drag
				completes. However, the caller may delete the target at any time, in which
				case it will automatically deregister itself before it dies.

				HighPriority - TRUE to place the target on the front of the target list,
				FALSE to place it on the end. (Targets are called in list order to handle
				events, so one on the front of the list has higher priority...)

	Purpose:	To register a Drag Target with the current Drag Manager
				This method is automatically called by DragTargets when they are
				constructed, and is only available to frined classes.

	Notes:		A drag target object can be registered several times, in which case it will
				be placed on the list several times. Not that this should happen...

	Scope:		protected (for friend class DragTarget only)

********************************************************************************************/

void DragManagerOp::RegisterTarget(DragTarget *NewTarget, BOOL HighPriority)
{
	ERROR3IF(NewTarget == NULL,
				"DragManagerOp::RegisterTarget - NULL Target parameter is illegal");

	if (HighPriority)
		Targets.AddHead(NewTarget);
	else
		Targets.AddTail(NewTarget);
}



/********************************************************************************************

>	void DragManagerOp::DeregisterTarget(DragTarget *OldTarget)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95

	Inputs:		OldTarget - points at the DragTarget object to remove

	Purpose:	To register a Drag Target with the current Drag Manager
				This method is automatically called by DragTargets when they are
				constructed, and is only available to frined classes.

	Notes:		Copes with a drag target being registered several times.

	Scope:		protected (for friend class DragTarget only)

********************************************************************************************/

void DragManagerOp::DeregisterTarget(DragTarget *OldTarget)
{
	DragTarget *Ptr = (DragTarget *) Targets.GetHead();
	DragTarget *Next;

	while (Ptr != NULL)
	{
		Next = (DragTarget *) Targets.GetNext(Ptr);

		if (Ptr == OldTarget)
		{
			Targets.RemoveItem(Ptr);
			delete Ptr;
		}

		Ptr = Next;
	}
}



/********************************************************************************************

>	void DragManagerOp::CleanUpAfterDrag(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95

	Purpose:	Generic clean-up method. Deletes all registered DragTargets, and any
				other information (DragInformation, last keypress info, etc) which is
				still lying about in the DragManagerOp.
				Also deregisters our idle event handlers etc.
				Used by destructor, and also by all forms of Drag completion to free
				our memory claims, etc.

	Scope:		private

********************************************************************************************/

void DragManagerOp::CleanUpAfterDrag(void)
{
	GetApplication()->RemoveIdleProcessor(IDLEPRIORITY_HIGH, this);

	if (TheCaptureWindow != NULL)
	{
#ifdef DRAG_MAINFRAME
		GetMainFrame()->EndDragManagerDrag(TheCaptureWindow);
		delete TheCaptureWindow;
#else
		if (TheCaptureWindow->HasCapture())
			TheCaptureWindow->ReleaseMouse();
		TheCaptureWindow->Destroy();
#endif
		TheCaptureWindow = NULL;
	}

	if (CurrentDragInfo != NULL)
	{
		delete CurrentDragInfo;
		CurrentDragInfo = NULL;
	}

	if (CurrentKeypress != NULL)
	{
		delete CurrentKeypress;
		CurrentKeypress = NULL;
	}

	// Delete all entries from the list. We do it this way to ensure that we don't try
	// to destruct the same target multiple times if it registered multiple times
	while (!Targets.IsEmpty())
		DeregisterTarget((DragTarget *) Targets.GetHead());
}



/********************************************************************************************

>	void DragManagerOp::SetCursor()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/95
	Returns:	-
	Purpose:	Set the cursor !
   
********************************************************************************************/

void DragManagerOp::SetCursor()
{
	UINT32 CursorID;

	// are we over a target ??
	if(CurrentDragTarget== NULL)
	  CursorID = CurrentDragInfo->GetCursorID();  	// nope
	else
	  CursorID = CurrentDragTarget->GetCursorID();	// yep
	
	// the target does not want to change the cursor
	if(CursorID == 0)
		return;
	
	if(CurrentManager->CurrentID != CursorID) 						// nothing to change
	{
		if(CurrentManager->CurrentCursor)							// delete the old
		{
			delete CurrentManager->CurrentCursor;
			CurrentManager->CurrentCursor = NULL;
		}
		if(CurrentManager->CurrentCursor==NULL)
		{
			CurrentManager->CurrentCursor = new Cursor(CursorID);	// create the new
			if (CurrentManager->CurrentCursor)
				CurrentManager->CurrentCursor->SetActive();		   	// make the active cursor
		}
		
		CurrentManager->CurrentID = CursorID;
	}
}


/********************************************************************************************

>	void DragManagerOp::SetStatusLineText()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/95
	Returns:	-
	Purpose:	Write Status line help
   
********************************************************************************************/

void DragManagerOp::SetStatusLineText()
{
	String_256 StatusText;
	if(CurrentDragTarget==NULL)	//we are not over a target
	{	// get the drag status text
		if(CurrentDragInfo)
			if(CurrentDragInfo->GetStatusLineText(&StatusText))
				GetApplication()->UpdateStatusBarText(&StatusText,FALSE); 
	}
	else
	{	// get the target status text
		if(CurrentDragTarget->GetStatusLineText(&StatusText))
			GetApplication()->UpdateStatusBarText(&StatusText,FALSE); 
	}
			

}


/********************************************************************************************

>	virtual BOOL DragManagerOp::GetStatusText(String_256 * StatusText)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the buffer is valid FALSE otherwise
	Purpose:	put status text in a buffer
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL DragManagerOp::GetStatusText(String_256 * StatusText)
{
	ERROR2IF(StatusText==NULL,FALSE,"DragManagerOp::GetStatusLineText() - StatusText==NULL!");

	String_256 Text;
	if(CurrentDragTarget)
	{
		if(CurrentDragInfo->GetStatusLineText(&Text))
			* StatusText = Text;
		else
			return FALSE;
	}
	else
	{
	 	if(CurrentDragTarget->GetStatusLineText(&Text))
		   * StatusText = Text;
		else
			return FALSE;
	}
		
	return TRUE; 

}



/********************************************************************************************

>	void DragManagerOp::RedrawStarting()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/95
	Returns:	-
	Purpose:	Tell the drag manager that an external redraw has started.
				This allows it to remove its solid drag stuff so that the screen display is
				not screwed up. You should also call RedrawFinished when you finish redrawing

	Notes:		If you are redrawing in a particular window & gadget, see the other variant
				of this method, which will reduce solid-drag flicker.

	SeeAlso:	DragManagerOp::RedrawFinished()

********************************************************************************************/

void DragManagerOp::RedrawStarting()
{
	if (RedrawInProgress)
		return;

	RedrawInProgress = TRUE;

	if (DragManagerOp::CurrentManager &&
		DragManagerOp::CurrentManager->CurrentDragInfo &&
		DragManagerOp::CurrentManager->CurrentDragInfo->DoesSolidDrag)
	{
		DragManagerOp::CurrentManager->TheCaptureWindow->CleanUpSolidDrag();
	}
}



/********************************************************************************************

>	static void DragManagerOp::RedrawStarting(CWindowID TheWindow, CGadgetID TheGadget,
												DocRect *TheArea = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/95

	Inputs:		TheWindow - The CWindowID of your DialogOp
				TheGadget - The Gadget ID of the gadget  being redrawn
				TheArea - A kernel-rendered dialogue rectangle (MILLIPOINTS) which is being
				redrawn. If NULL, the entire gadget is assumed. NOTE that this is clipped
				within the gadget bounds, so may be outside the 'visible area' of the gadget


	Returns:	-
	Purpose:	Tell the drag manager that an external redraw has started, within a specific
				window and gadget.

				This allows it to remove its solid drag stuff so that the screen display is
				not screwed up. You should also call RedrawFinished when you finish redrawing

				This call specifies a particular gadget that is redrawing - if this doesn't
				overlap the solid drag area, then nothing will be done - this will reduce
				flicker if you're background redrawing while a drag is going on.

	SeeAlso:	DragManagerOp::RedrawFinished()

********************************************************************************************/

void DragManagerOp::RedrawStarting(CWindowID TheWindow, CGadgetID TheGadget, DocRect *TheArea)
{
 	if (RedrawInProgress)
		return;

	if (DragManagerOp::CurrentManager &&
		DragManagerOp::CurrentManager->CurrentDragInfo &&
		DragManagerOp::CurrentManager->CurrentDragInfo->DoesSolidDrag)
	{
	    wxWindow* pWindow = (wxWindow*)TheWindow;
		wxWindow* pGadget = pWindow->FindWindow((INT32)TheGadget);
		if (pGadget != NULL)
		{
			wxRect AreaClientRect;
			wxRect ClientRect;
			if (TheArea != NULL)
			{
				ReDrawInfoType DlgInfo;
				DialogManager::GetKernelRenderedGadgetInfo(TheWindow, TheGadget, &DlgInfo);

				INT32 PixelSize = 72000 / DlgInfo.Dpi;		// Size of a pixel in MILLIPOINTS
				AreaClientRect.x		= TheArea->lo.x / PixelSize;
				AreaClientRect.y		= TheArea->hi.x / PixelSize;
				AreaClientRect.width	= TheArea->Width() / PixelSize;
				AreaClientRect.height	= TheArea->Height() / PixelSize;
			}
			else
				AreaClientRect = pGadget->GetClientRect();

			ClientRect = pGadget->GetClientRect();

			wxRect ScreenRect(ClientRect);

			// Get the intersection of the rects (clip the redrawing area within the window)
			if (!ScreenRect.Intersect(AreaClientRect).IsEmpty())
			{
				// Convert the client coordinates to screen coords, and tell the drag capture window
				// to get it's solid drag stuff out of the way
				pGadget->ClientToScreen(&ScreenRect.x, &ScreenRect.y);

				RedrawInProgress = DragManagerOp::CurrentManager->TheCaptureWindow->
													CleanUpSolidDragInScreenArea(ScreenRect);
			}
			else
				RedrawInProgress = FALSE;
		}
		else
		{
			// We failed to get the window area, so we'll have to clean up just in case
			DragManagerOp::CurrentManager->TheCaptureWindow->CleanUpSolidDrag();
			RedrawInProgress = TRUE;
		}
	}	
}


/********************************************************************************************

>	void DragManagerOp::RedrawFinished()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/95
	Returns:	-
	Purpose:	Tell the drag manager that an external redraw has finished.
   
********************************************************************************************/

void DragManagerOp::RedrawFinished()
{
	if (!RedrawInProgress)
		return;

	RedrawInProgress = FALSE;

	if (DragManagerOp::CurrentManager &&
		DragManagerOp::CurrentManager->CurrentDragInfo &&
		DragManagerOp::CurrentManager->CurrentDragInfo->DoesSolidDrag)
	{
		wxPoint LastMousePos = DragManagerOp::CurrentManager->GetDragMousePos();
		DragManagerOp::CurrentManager->TheCaptureWindow->SetUpSolidDrag(LastMousePos);
	}
}
