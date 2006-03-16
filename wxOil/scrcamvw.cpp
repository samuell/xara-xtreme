// $Id: scrcamvw.cpp 662 2006-03-14 21:31:49Z alex $
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

// The ScreenCamView class - subclass of ScreenView used for screen display.

/*
*/

#include "camtypes.h"


#include "docview.h"
#include "scrcamvw.h"
#include "scrvw.h"
#include "app.h"
#include "camelot.h"
#include "camdoc.h"
//#include "ccmdikid.h"
#include "vstate.h"
#include "rendwnd.h"
//#include "scroller.h"
#include "docview.h"
#include "ops.h"
//#include "oiltool.h"
#include "errors.h"
//#include "justin.h"
#include "ensure.h"
#include "ccdc.h"
#include "csrstack.h"
#include "document.h"
//#include "mainfrm.h"
//#include "oilkeys.h"
#include "monotime.h"
#include "pen.h"
//#include "bars.h"
#include "impexpop.h"	// Import/Export ops
//#include "resource.h"
#include "statline.h"
#include "prntview.h"
//#include "prncamvw.h"
#include "page.h"
#include "spread.h"
//#include "dragmgr.h"
//#include "dragcol.h"
//#include "viewrc.h"
//#include "printdlg.h"
//#include "prdlgctl.h"
#include "princomp.h"
#include "printctl.h"
#include "psrndrgn.h"
//#include "markn.h"
//#include "oilruler.h"
//#include "rulers.h"
#include "keypress.h"
//#include "localenv.h"
#include "tool.h"

#include "cutop.h"
#include "insertnd.h"

#include "clipint.h"
//#include "clipext.h"
//#include "srvritem.h"
//#include "oleprefs.h"
#include "cutop.h"
#include "selector.h"
//#include "justin3.h"

#include "osrndrgn.h"

#if (_OLE_VER >= 0x200)
  // Extra OLE 2.0 stuff in MFC 2.5
  #include "cntritem.h"
  #include "ole2id.h"
#endif

DECLARE_SOURCE("$Revision: 662 $");

/////////////////////////////////////////////////////////////////////////////
// ScreenCamCanvas the message map.

BEGIN_EVENT_TABLE( ScreenCamView::ScreenCamCanvas, ScreenView::ScreenCanvas )
	EVT_SIZE(			ScreenCamView::ScreenCamCanvas::OnSize )
END_EVENT_TABLE()

ReflectIntoView( ScreenCamView::ScreenCamCanvas, ScreenCamView, OnSize,		wxSizeEvent )

/////////////////////////////////////////////////////////////////////////////
// ScreenCamView serialization & the message map.

IMPLEMENT_DYNAMIC_CLASS( ScreenCamView, ScreenView )

BEGIN_EVENT_TABLE( ScreenCamView, ScreenView )
END_EVENT_TABLE()

/*********************************************************************************************
>	afx_msg INT32 ScreenCamView::OnCreate(LPCREATESTRUCT lpCreateStruct)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    A long pointer to a Windows CREATESTRUCT object.
	Outputs:	Calling the base class handler may change the CREATESTRUCT pointed to . . . ?
	Returns:    0 for success, -1 for failure.
	Purpose:    Called upon receipt of a WM_CREATE message for the view window.  Constructs
				MFC/C++ surrogates on the heap, then tries to link up with Windows by
				calling the base class function OnCreate() and the Create function for
				each of the child windows of ScreenView, any or all of which can fail.
				Calculates the size of a pixel and sets the appropriate fields
				in the ViewState.  If this is a new view upon a new document
				the rest of the fields of the ViewState are filled by the
				default characteristics of the ScreenView.  If the document is
				not new then the ScreenView assumes that the kernel has already
				set up the ViewState, and changes its size, position etc to
				fit.				
	Errors:	    _R(IDE_CREATEVIEWFAILED) if either the C++ or Windows side of construction
				fails.  Note that if the function does fail then MFC is responsible for
				tidying up afterwards.
	Scope:		Protected
	SeeAlso:	ScreenView::ScreenView(); ScreenView::Create() CView::OnCreate()

**********************************************************************************************/ 

bool ScreenCamView::OnCreate( wxDocument* doc, /* TYPENOTE: Correct */ long flags )
{
	// Construct the (C++) child windows.
PORTNOTE("other","ScreenCamView::OnCreate - Removed scroller / ruler usage")
#ifndef EXCLUDE_FROM_XARALX
	RenderWindow = new CRenderWnd;

	HScrollBar = new CWinScroller(TRUE);
	VScrollBar = new CWinScroller(FALSE);
	Corner = new CScrollerCorner;
// WEBSTER - markn 15/1/97
// No rulers in Webster
#ifndef WEBSTER
	HRuler = new OILHorizontalRuler;
	VRuler = new OILVerticalRuler;
	OGadget = new OriginGadget;
#endif //webster	
	if (!RenderWindow || !HScrollBar || !VScrollBar || !Corner 
#ifndef WEBSTER
		||!HRuler ||!VRuler	||!OGadget
#endif //webster
		)
	{
		Error::SetError(_R(IDE_CREATE_VIEW_FAILED), 0);
		InformError();
		return -1;
	} 
#endif
	// Get base class to call CView functions, and get a document for this view.
	if( !CCamView::OnCreate( doc, flags ) )
		// Something went wrong - pass error back.
		return false;

PORTNOTE("other","ScreenCamView::OnCreate - Removed scroller usage")
	// Now get Windows to do its side of the construction.  The stated sizes and
	// positions of the windows here will be changed very soon.
#ifndef EXCLUDE_FROM_XARALX
	const CRect rcARect(-100,-100,-90,-90);
	if (!RenderWindow->Create("", "", 0, rcARect, this, 1) ||
		!HScrollBar->Create(0, rcARect, this, 2) ||
		!VScrollBar->Create(0, rcARect, this, 3) ||
		!Corner->Create("", "", 0, rcARect, this, 4)
// WEBSTER - markn 15/1/97
// No rulers in Webster
#ifndef WEBSTER
		|| !OGadget->Create(this)
		|| !HRuler->Create(this)
		|| !VRuler->Create(this)
#endif //webster
		)
	{
		Error::SetError(_R(IDE_CREATE_VIEW_FAILED), 0);
		InformError();
		return -1;
	}
#endif
	CreateNewDocView();

// WEBSTER - markn 15/1/97
// No rulers in Webster
#ifndef WEBSTER

	// init the kernel rulers and establish pointer links to them
PORTNOTE("other","ScreenCamView::OnCreate - Removed ruler usage")
#ifndef EXCLUDE_FROM_XARALX
	RulerPair* pRulers=pDocView->GetpRulerPair();
	pRulers->Init(pDocView,HRuler,VRuler,OGadget);
	HRuler->LinkToKernel(pRulers->GetpHorizontalRuler());
	VRuler->LinkToKernel(pRulers->GetpVerticalRuler());
#endif
#endif //webster
	ENSURE(pDocView != 0, "ScreenView::ScreenView can't get a new DocView!");
	pDocView->ConnectToOilView(this);
	
	// find the last view so we can use some of it's settings to create the new
//	DocView * LastView = DocView::GetSelected();

	// Link this and the DocView to the ViewState object.
	pDocView->SetViewState(Status);

////////////////////////////////////////////////////////////////////////////////
	
	wxScreenDC dc;
	wxSize pixsize=OSRenderRegion::GetFixedDCPPI(dc);

PORTNOTE("other","ScreenCamView::OnCreate - Removed scroller usage")
#ifndef EXCLUDE_FROM_XARALX
	// Set the logical pixel size accordingly (measured in millipoints).  Take the
	// opportunity to pass the values into the scrollers and the OIL -> Windows coordinate
	// transform system.
	HScrollBar->SetGranularity(72000L / pixwidth);
	VScrollBar->SetGranularity(72000L / pixheight);
#endif

	// Tell DocView how big the pixel size is.
	FIXED16 PixelWidth  = FIXED16(72000.0/pixsize.x);
	FIXED16 PixelHeight = FIXED16(72000.0/pixsize.y);
	ERROR3IF(PixelWidth != PixelHeight, "Luke says non-square pixels are not supported");
	pDocView->SetPixelSize(PixelWidth, PixelHeight);

	// Make our DocView the current DocView
	pDocView->SetCurrent();

	GetCanvas()->SetScrollRate(1,1);

	if (GetFrame())
		GetFrame()->GetClientSize(&CurrentSize.width,&CurrentSize.height);
	// Now the scrollers have all their information, we can set their appearance.
	// Make sure that they are invisible until the rest of the frame is ready
/*	XLONG x1 = CurrentSize.GetWidth () * PixelWidth;
	XLONG x2 = CurrentSize.GetHeight() * PixelHeight;
	GetFrame()->SetScrollbar(wxHORIZONTAL,0,x1,Status->WorkAreaExtent.hi.x-Status->WorkAreaExtent.lo.x,false);
	GetFrame()->SetScrollbar(  wxVERTICAL,0,x2,Status->WorkAreaExtent.hi.y-Status->WorkAreaExtent.lo.y,false);
*/
	SetScrollerStyle(ScrollerStyle = PropScrollersOn);
 	
 	ShowScrollers(DefaultScrollersState);
	ShowRulers(DefaultRulersState);

///////////////////////////////////////////////////////////////////////////////
	
	// Register for WM_DROPFILES messages
//	DragAcceptFiles(TRUE);

// WEBSTER - markn 12/2/97	
#if (_OLE_VER >= 0x200)
	// Register with OLE as a drop target.
	m_DropTarget.Register(this);
#endif

///////////////////////////////////////////////////////////////////////////////

	// now that the ScreenView (and hence DocView) is stable, broadcast a message to let everyone know
	BROADCAST_TO_ALL(DocViewMsg(pDocView,DocViewMsg::NEWANDSTABLE));

// ****************** BODGE **************************
// This code will tell windows to send us messages when
// the joystick gets waggled about.
// We should really release the joystick at some point later,
// but it gets released automatically when this ScreenView is
// destroyed, so it will do for now.

// The messages get passed to 'OnJoystickMove' member of this
// class.
	
#ifdef WIN32

PORTNOTE("other","ScreenCamView::OnCreate - Removed joystick usage")
#ifndef EXCLUDE_FROM_XARALX
	JOYINFO joyinfo;
	UINT32 wNumDevs, wDeviceID;
	BOOL bDev1Attached, bDev2Attached;

	// Are there any Joysticks available ?
    if((wNumDevs = joyGetNumDevs()) == 0)
        return 0;	// Nope.

	// Are there One or Two of them ?
    bDev1Attached = joyGetPos(JOYSTICKID1,&joyinfo) != JOYERR_UNPLUGGED;
    bDev2Attached = wNumDevs == 2 &&
                     joyGetPos(JOYSTICKID2,&joyinfo) != JOYERR_UNPLUGGED;

    if(bDev1Attached || bDev2Attached)		// Decide which joystick to use
        wDeviceID = bDev1Attached ? JOYSTICKID1 : JOYSTICKID2;
    else
        return 0;

	// Grab those Messages !!
	MMRESULT JoyResult = joySetCapture(m_hWnd, wDeviceID, NULL, TRUE);
#endif

#endif

// ****************** BODGE **************************

	// Return success!
	return true;
}

/*********************************************************************************************
>	afx_msg void ScreenCamView::OnSize(UINT32 nType, INT32 cx, INT32 cy)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    A flag indicating whether the window is maximised, minimised
				etc., the width and height of the ScreenView client area.
	Outputs:    -
	Returns:	-
	Purpose:	Resizes and repositions all child windows to fit the new size
				of the ScreenView.  Updates the ViewState object with the
				size and position of the view, in an OS-independent manner.
				Resets the scroller's page and line size, and forces an
				immediate redraw of all affected windows.
	Errors:	    -
	Scope:		Protected
	SeeAlso:	CScroller::CalcPosFromParentClient(); CScroller::SetPageSize();
				CScroller::SetLineSize(); class CRendWnd; class CScrollerCorner;
				struct ViewState; DocView::ViewStateChanged()


**********************************************************************************************/ 

void ScreenCamView::OnSize( wxSizeEvent &event )
{
	// This is called early, so if pDocView is null do nothing
	if( NULL == pDocView )
		return;

	SetCurrentStates();

	Document* pCurDoc = Document::GetCurrent();
	DocView* pCurView = DocView::GetCurrent();
	
	// these lines are here to stop very strange things happening on exit under Win32s
	// when this fn gets called when it really shouldn't. I would like to really know
	// just what on earth os going on under Win32s but it iss something strange in message
	// handling as far as I can tell.

	wxSize	size( event.GetSize() );

	// Check for irrelevant or potty messages.
	if (size.x <= 0 || size.y <= 0)
	{
//		TRACEUSER( "JustinF", _T("Strange size msg in ScreenView::OnSize(0x%X, %d, %d)\n"), 
//					nType, cx, cy);
		return;
	}

	// Handle OLE 2.0 in-place activation stuff.
#if (_OLE_VER >= 0x200)
	if(GetDocument())
	{
		COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
		if (pActiveItem) pActiveItem->SetItemRects();
	}	
#endif

	if (Status->ScrollersVisible)
	{
PORTNOTETRACE( "other", "ScreenCamView::OnSize - Removed scroller usage" );
#if !defined(EXCLUDE_FROM_XARALX)
		// Resize and reposition the proportional scrollers.
		wxRect hrect, vrect;
		HScrollBar->CalcPosFromParentClient(&hrect);

		UINT32 RulerWidth = OILRuler::GetWidth();
		//if rulers are switched on the scroll bars are made
		// smaller to accomodate them
		if (Status->RulersVisible)
			hrect.left += RulerWidth;
		if (RULER_BORDERS)
			hrect.left-=2;

		HScrollBar->MoveWindow(&hrect, TRUE);
		VScrollBar->CalcPosFromParentClient(&vrect);

		if (Status->RulersVisible)
			vrect.top += RulerWidth;
		if (RULER_BORDERS)
			vrect.top-=2;

		VScrollBar->MoveWindow(&vrect, TRUE);

 		// Reposition the corner window at the bottom-right.
		Corner->MoveWindow(vrect.left, hrect.top, vrect.Width(), hrect.Height());
	 
		// Resize/reposition the rendering window.
		CurrentSize.left = CurrentSize.top = 0;
		CurrentSize.right = cx - vrect.Width() + 1;
		CurrentSize.bottom = cy - hrect.Height() + 1;
#endif
	}
	else
	{	
		CurrentSize.x = CurrentSize.y = 0;
		CurrentSize.width  = size.x;
		CurrentSize.height = size.y;
	}

	if (Status->RulersVisible)
	{
PORTNOTETRACE( "other", "ScreenCamView::OnSize - Removed scroller / ruler usage" );
#if !defined(EXCLUDE_FROM_XARALX)
	 	wxRect hRect, vRect, oRect;

	 	HRuler->CalcPosFromParentClient(&hRect);
		HRuler->MoveWindow(&hRect, TRUE);
		HRuler->PositionLegend();
		
		CurrentSize.top = 0 + hRect.Height() ;
			
	 	VRuler->CalcPosFromParentClient(&vRect);
		VRuler->MoveWindow(&vRect, TRUE);
		CurrentSize.left = 0 + vRect.Width(); 

	 	OGadget->CalcPosFromParentClient(&oRect);
		OGadget->MoveWindow(&oRect, TRUE);
		if (RULER_BORDERS)
		{
			CurrentSize.top --;
			CurrentSize.left--; 
		}
#endif
	}

PORTNOTE( "other", "ScreenCamView::OnSize - Removed RenderWindow usage -not sure if needed" )
#ifndef EXCLUDE_FROM_XARALX
	RenderWindow->MoveWindow(&CurrentSize, TRUE);
#endif
	// Update the rest of the window placement information.
	UpdateViewPosition();

	// Calculate the work area, page & line sizes etc etc.
	FIXED16 PixelWidth, PixelHeight;
	pDocView->GetPixelSize(&PixelWidth, &PixelHeight);

PORTNOTE( "other", "ScreenCamView::OnSize - Removed scroller usage" )
#if !defined(EXCLUDE_FROM_XARALX)
	XLONG x1 = CurrentSize.GetWidth() * PixelWidth;
	XLONG x2 = CurrentSize.GetHeight() * PixelHeight;
	HScrollBar->SetPageSize(x1);
	VScrollBar->SetPageSize(x2);
	HScrollBar->SetLineSize(x1 / xlong(10) + xlong(1));	
	VScrollBar->SetLineSize(x2 / xlong(10) + xlong(1));
#endif
	SetWorkAreaExtent(Status->WorkAreaExtent, FALSE);

	// Make sure the scroll offsets are valid - if we resize the bottom of the window
	// when at the extreme bottom of the view, then the scroll offsets should be
	// changed - we use the scrollers' own integrity checks to do this automatically.
	// Don't do this until the setup flag is TRUE, so we don't overwrite scroll offsets
	// that have been reloaded.
	if (fSetupDone)
	{
		WorkCoord CurrScrollPos;
		GetScrollOffset(&CurrScrollPos);
		SetScrollOffset(CurrScrollPos, TRUE);
	}	

	// Inform the associated DocView object that something has happened.
	pDocView->ViewStateChanged();
	pCurDoc->SetCurrent();
	pCurView->SetCurrent();
}

#if (_OLE_VER >= 0x200)

/*********************************************************************************************
>	BOOL ScreenCamView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/96
	Inputs:	    pDataObject		Pointer to ColeDataObject
				dropEffect		Copy, Move or Link
				point			The drop position
	Outputs:    -
	Returns:	-
	Purpose:	Respond to OLE drop on a ScreenCamView
	Errors:	    -
	Scope:		Public
**********************************************************************************************/ 

BOOL ScreenCamView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	if (!m_DropFormatOK || pDataObject == NULL)
		return FALSE;

//	Document* pSelDoc = Document::GetSelected();

	Document* pClipDoc = InternalClipboard::Instance();
	if (pClipDoc != NULL)
	{
		Document::SetSelectedViewAndSpread(pClipDoc);
		pClipDoc->ResetInsertionPosition();
		pClipDoc->SetCurrent();

		// If there is *still* a format on the external clip board we like, then it's ok to drop
		CLIPFORMAT ClipFormat = ExternalClipboard::GetBestPasteFormat(pDataObject);
		ExternalClipboard* pExtClipBoard = ExternalClipboard::GetExternalClipboard();

		if (ClipFormat != 0 && pExtClipBoard !=NULL && pExtClipBoard->PrepareForOlePaste(pDataObject))
		{
			Document::SetSelectedViewAndSpread(GetDocument()->GetKernelDoc(),GetDocViewPtr());
			DocView *pDocView = DocView::GetCurrent();

			if (pDocView != NULL)
			{
				// Make the drop point a WinCoord so we can turn it into an oil coord
				WinCoord DropPoint(point.x,point.y);

				// Turn it into Oil coordinates...
				OilCoord OilPos = DropPoint.ToOil(pDocView);

				// Find the spread that contains the coord
				Spread* pSpread = pDocView->FindEnclosingSpread(OilPos);
				if (pSpread != NULL)
				{
					// First of all convert the OilCoord into device coords
					DocCoord Centre = OilPos.ToDoc(pSpread, pDocView);
				
					// Translate the coord to spread coords
					pSpread->DocCoordToSpreadCoord(&Centre);

					// 'Centre' now contains the centre point for the paste, in spread coords

					TRY
					{
						// Wrap all this in a TRY/CATCH block so that if OLE throws a wobbly, we can
						// ensure that the clipboard remains intacted on exit.

						// Copy the selection to the drag'n'drop clipboard.
						OpDescriptor* pOp = OpDescriptor::FindOpDescriptor(OPTOKEN_PASTE);
						ERROR3IF(!pOp, "No paste operation in ScreenCamView::OnDrop");

						if (pOp != NULL)
						{
							Document::SetSelectedViewAndSpread(pClipDoc);
							pClipDoc->SetCurrent();
							pClipDoc->ResetInsertionPosition();
							InsertionNode* pInsertNode = pClipDoc->GetInsertionPosition();
							if (pInsertNode != NULL)
							{
								pInsertNode->UnlinkNodeFromTree();
								delete pInsertNode;
								pInsertNode = NULL;
							}

							Document::SetSelectedViewAndSpread(GetDocument()->GetKernelDoc(),GetDocViewPtr());
							
							// create a param object that contains the centre point in the spread at which
							// we would like the objects to be pasted
							OpParamPasteAtPosition Param(pSpread,Centre);

							// Call the paste op via its DoWithParams() funtion
							pOp->Invoke(&Param);
						}
					}
					CATCH_ALL(e)
					{
						if (m_ClipBoardSwapped = FALSE)
						{
//							TRACEUSER( "Markn", _T("Swapping back to clipboard doc (Catch in ScreenCamView::OnDrop())\n"));
							InternalClipboard::Swap();
							ExternalClipboard::UpdateSystemClipboard(TRUE);
						}
						THROW_LAST();
					}
					END_CATCH_ALL
				}
			}
		}
	}

	// Tidy up any data which was stored when the mouse entered the view...
	OnDragLeave();

	Document::SetSelectedViewAndSpread(GetDocument()->GetKernelDoc(),GetDocViewPtr());
	((CMDIChildWnd*)GetParentFrame())->MDIActivate();

//	GetParentFrame()->GetParent()->PostMessage(WM_MDIACTIVATE, (WPARAM) GetParentFrame()->GetSafeHwnd());

	if (CCamSrvrItem::GetDragStartView() != NULL)
	{
		GetRenderWindow()->InvalidateRect(NULL);
		CCamSrvrItem::GetDragStartView()->GetRenderWindow()->InvalidateRect(NULL);
	}

	return TRUE;
}




/*********************************************************************************************
>	DROPEFFECT ScreenCamView::OnDragOver( COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/96
	Inputs:	    pDataObject		Pointer to ColeDataObject
				dwKeyState		Shift, Ctrl, Alt state
				point			The proposed drop position
	Outputs:    -
	Returns:	DROPEFFECT		Drop possibility
	Purpose:	Respond to OLE drop on a ScreenCamView
	Errors:	    -
	Scope:		Public
**********************************************************************************************/ 

DROPEFFECT ScreenCamView::OnDragOver( COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{
	DROPEFFECT DropEffect = DROPEFFECT_NONE;

	if (m_DropFormatOK)
	{
		DWORD KeyModifierMask = (MK_CONTROL | MK_SHIFT | MK_ALT);

		// WEBSTER - markn 14/2/97
		// Changed the unmodified drag to be DROPEFFECT_COPY & the modified one to DROPEFFECT_MOVE
		// It used to be the other way round

		if ((dwKeyState & KeyModifierMask) == 0)				// No modifier keys? if so move.
			DropEffect = DROPEFFECT_COPY;
		else if ((dwKeyState & KeyModifierMask) == MK_CONTROL)	// Ctrl only down? if so copy
			DropEffect = DROPEFFECT_MOVE;

		// If we are moving data between two views of the same doc, then change to a copy
		// because move does not work correctly
		if (DropEffect == DROPEFFECT_MOVE)
		{
			if (CCamSrvrItem::GetDragStartView() != NULL)
			{
				CCamDoc* pThisViewDoc = GetDocument();
				CCamDoc* pOtherViewDoc = CCamSrvrItem::GetDragStartView()->GetDocument();

				if (pThisViewDoc == pOtherViewDoc)
					DropEffect = DROPEFFECT_COPY;
			}
		}
	}

	return DropEffect;
}




/*********************************************************************************************
>	DROPEFFECT ScreenCamView::OnDragEnter( COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/96
	Inputs:	    pDataObject		Pointer to ColeDataObject
				dwKeyState		Shift, Ctrl, Alt state
				point			The proposed drop position
	Outputs:    -
	Returns:	DROPEFFECT		Drop possibility
	Purpose:	Mouse is entering our view so cache any useufl data which would speed up
				future calls to OnDragOver.
	Errors:	    -
	Scope:		Public
**********************************************************************************************/ 

DROPEFFECT ScreenCamView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	if (pDataObject == NULL)
		return DROPEFFECT_NONE;

	// Set up the kernel with this item's document as 'current'.
	GetDocument()->GetKernelDoc()->SetCurrent();

	// Disconnect from the Windows clipboard and swap in the drag'n'drop clipboard.
	if (InternalClipboard::GetCurrentID() == InternalClipboard::CLIPBOARD)
	{
//		TRACEUSER( "Markn", _T("Swapping to DragNDrop clipboard doc (ScreenCamView::OnDragEnter())\n"));
		ExternalClipboard::UpdateSystemClipboard(FALSE);
		InternalClipboard::Swap();
		InternalClipboard::Clear();
		m_ClipBoardSwapped = TRUE;
	}
	else
		m_ClipBoardSwapped = FALSE;

	// If there is a format on the external clipboard we like, then it's ok to drop.
	UINT32 nFormat = ExternalClipboard::GetBestPasteFormat(pDataObject);
	m_DropFormatOK = (nFormat != 0);
//	m_DropFormatOK = TRUE;
	
	// Update the status-line prefix with what we would drop.
	if (m_DropFormatOK)
	{
		// Ask the clipboard what the name of the given format is.
		String_64 strName;
		ExternalClipboard::GetExternalFormatName(nFormat, &strName);

		// Set the formatted text in the status line.
		String_64 strLine;
		strLine.MakeMsg(_R(IDS_DROP_FORMAT_TEXT), (LPCTSTR) strName);
		StatusLine::SetPrefix(strLine);
	}

	// Call OnDragOver to ensure consistent response
	return OnDragOver(pDataObject, dwKeyState, point);
}




/*********************************************************************************************
>	void ScreenCamView::OnDragLeave()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/96
	Purpose:	The mouse is leaving our View so tidy up anything that was setup in
				OnDragEnter if we need to.
	Scope:		Public
**********************************************************************************************/ 

void ScreenCamView::OnDragLeave()
{
	// Swap out the drag'n'drop clipboard and reconnect to the Windows clipboard.
	if (m_ClipBoardSwapped)
	{
//		TRACEUSER( "Markn", _T("Swapping back to clipboard doc (ScreenCamView::OnDragLeave())\n"));
		InternalClipboard::Swap();
		ExternalClipboard::UpdateSystemClipboard(TRUE);
	}

	// Remove the status-line prefix.
	StatusLine::SetDefaultPrefix();
}




/*********************************************************************************************
>	virtual void ScreenCamView::HandleOleDragOutOfView(CPoint point);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>/Phil/Justin
	Created:	26/9/96
	Inputs:		point - The current mouse pointer position
	Purpose:	Called when doing a DRAGTYPE_OLESCROLL and the mouse pointer moves outside
				the view window. The base class does nothing, so dragging continues as normal,
				but in the derived ScreenCamView class, this is used to cancel the internal
				XOR drag operation, and start an OLE drag and drop export procedure.
	Notes:		May cancel the current Drag Op
	Scope:		Protected
	SeeAlso:	ScreenView::HandleOleDragOutOfView
**********************************************************************************************/ 

void ScreenCamView::HandleOleDragOutOfView(CPoint point)
{
	// Is it OK to start an OLE drag and drop now? (This depends on a preference, and may
	// depend on whether a special modifier key is depressed).
	if (!OLEPrefs::OKToActivateDrag()) return;

	// Check for junk.
	TRACEUSER( "JustinF", _T("Starting up an OLE drag ...\n"));
	ERROR3IF(InternalClipboard::GetCurrentID() != InternalClipboard::CLIPBOARD,
				"Current clipboard is wrong in ScreenCamView::HandleOleDragOutOfView");
	ERROR3IF(!Tool::GetCurrent() || Tool::GetCurrent()->GetID() != TOOLID_SELECTOR,
				"Selector tool isn't current in ScreenCamView::HandleOleDragOutOfView");

	// Suspend any kernel drag operation (but *don't* kill it, we may restart it later).
	if (pCurrentDragOp)
	{
		// Release the mouse so OLE can process the mouse's messages.
		::ReleaseCapture();

		// Make the kernel drag operation remove all of its XOR blobs. This isn't really necessary
		// for most drags, but is vital if we drag into another view of the same document, as
		// otherwise the XOR gets well screwed up. Of course if nobody remembers to draw the XOR
		// back in before resuming this drag, the XOR will be screwed beyond belief anyway.
		Spread* pSpread = pDocView->FindEnclosingSpread(((WinCoord*) &point)->ToOil(pDocView, TRUE));
		if (pSpread)
		{
			// Get the DocView's viewport, in Spread Coords, because we have to supply a DocRect to
			// this stupid routine which won't let us pass a NULL pointer in to just redraw
			// it all, depsite the fact that all the functions it calls will allow it. Grrr.
			// (And using the Spread's bounding rect doesn't work because its bounding rect is in
			// chapter coords, not spread coords. Tread carefully...
			DocRect ClipRect = pDocView->GetDocViewRect(pSpread);
			pSpread->DocCoordToSpreadCoord(&ClipRect);
			pCurrentDragOp->RenderDragBlobs(ClipRect, pSpread, pDocView->GetCurrentSolidDragState());
		}
	}

	// Find the bounds of the dragged object, in Document coordinates / millipoints.
	ERROR3IF(Tool::GetCurrent()->GetID() != TOOLID_SELECTOR, 
				"ScreenCamView::HandleOleDragOutOfView: not the selector tool");
	DocRect drBounds = ((SelectorTool*) Tool::GetCurrent())->GetSelectionBounds();
	if (drBounds.IsEmpty() || !drBounds.IsValid())
	{
		TRACEUSER( "JustinF", _T("Invalid/empty bounds in ScreenCamView::HandleOleDragOutOfView\n"));
		return;
	}

	// Convert it's dimensions to pixels.
	CSize szItem(CCamSrvrItem::MPtoPX(drBounds.Width()),
				 CCamSrvrItem::MPtoPX(drBounds.Height()));

	// Work out the bounds of the object, in pixels, and the offset from the mouse to the
	// top-left corner of the object, in pixels.
	CRect rItem(CPoint(point.x - szItem.cx / 2, point.y - szItem.cy / 2), szItem);
	CPoint ptOffset(point - CPoint(rItem.left, rItem.top));

	// Ask the drag'n'drop clipboard to start an OLE drag operation.
	InternalClipboard::Other()->GetOilDoc()->GetEmbeddedItem()->DoDragDrop(&rItem, ptOffset);
}

#endif
