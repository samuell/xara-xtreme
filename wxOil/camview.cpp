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
#include "camtypes.h"

#include "camview.h"

#include "camelot.h"
#include "camframe.h"
#include "camdoc.h"
#include "vstate.h"
#include "ccdc.h"
#include "csrstack.h"

#include "rendwnd.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "prntview.h"
#include "osrndrgn.h"
#include "scroller.h"
//#include "bars.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "localenv.h"
#include "zoomops.h"
#include "vkextra.h"
#include "brushmsg.h"
#include "statline.h"
#include "draginfo.h"
#include "prdlgctl.h"
#include "prncamvw.h"
#include "opbevel.h"
#include "psrndrgn.h"
#include "prnmks.h"
#include "princomp.h"

/***************************************************************************************************************************/

IMPLEMENT_DYNAMIC_CLASS( CCamView, wxView )
CC_IMPLEMENT_DYNAMIC( ViewDragTarget, OilDragTarget )
CC_IMPLEMENT_DYNCREATE( PageDropInfo, CCObject )

// This user preference controls how many pixels the mouse must move before a
// drag is initiated.
INT32 CCamView::DragLatency = 3;

INT32 CCamView::s_MouseMoveEliminationPeriod = 0;

// Used to start drags after user has clicked but not moved mouse for n milliseconds.
INT32 CCamView::DragDelay = 500;
MonotonicTime CCamView::DragTimer;

INT32 CCamView::PopCount = 0;
ViewState* CCamView::pReadyMadeViewState = NULL;
BOOL CCamView::DefaultScrollersState = TRUE;
BOOL CCamView::DefaultRulersState = FALSE;
BOOL CCamView::UseVisibleDefaults = TRUE;

// Default to a nonsense size.  Will be fixed up in OnSize
INT32 CCamView::ScrollBarSize = 1;

BEGIN_EVENT_TABLE( CCamView, wxView )
	EVT_SIZE(CCamView::OnSize)
	EVT_SCROLL(CCamView::OnScroll)
	EVT_TIMER(DragIdleID, CCamView::OnDragIdle)
END_EVENT_TABLE()

/*********************************************************************************************

>	CCamView::CCamView()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for a CCamView window.  Sets the pointers to the
				child windows to null, so that an incomplete construction of the view
				that is later destroyed won't try to delete invalid pointers.  The
				Windows side of construction is done later, in the OnCreate() message
				handler.

				Updates the CCamView population counter. Called from somewhere deep
				inside MFC.				
	Errors:		-
	Scope:	    Protected.
	SeeAlso:    CCamView::OnCreate(); CCamView::~CCamView()

**********************************************************************************************/ 

CCamView::CCamView()
{
	TRACEUSER("Gerry", _T("Creating CCamView at 0x%08x\n"), this);

	// No child windows or connections yet.
	pDocView = NULL;
	pPrintView = NULL;

	// Create a ViewState object to maintain device-independent information.
	Status = new ViewState;

	RenderWindow = NULL;
	HScrollBar = NULL;
	VScrollBar = NULL;
	Corner = NULL;
	HRuler = NULL;
	VRuler = NULL;
	OGadget= NULL;

	// Set/clear these internal flags.
	fSetupDone = FALSE;
	fInitialUpdate = FALSE;

	// Not dragging or scrolling either.
	pCurrentDragOp = NULL;

	OLERecaptureMouse = FALSE;

	// Set up click event variables
	LastClickType = CLICKTYPE_NONE;
	LastClickButton = 0;
	LastClickPoint.x = 0;
	LastClickPoint.y = 0;
	CurrentMousePos.x = 0;
	CurrentMousePos.y = 0;
	FirstClickButton = 0;
	DragKeepAccuracy = TRUE;
	AutoScrollExcludeRulers = FALSE;
	
	// Bump the population.
	PopCount++;

	// Vars used by InvokeDragOp() mechanism
	DragOpToken = "";
	pDragOpParam = NULL;
	DragOpInvoke = FALSE;

	// Set up the drag idle timer
	m_DragIdleTimer.SetOwner(this, DragIdleID);

	// Triple and quad click support
	LastDoubleClickPos = wxPoint( 0, 0 );
	ThisSingleIsTriple = FALSE;
	ThisDoubleIsQuad = FALSE;

	// Set this flag until we have been through OnActivateView once, after which we can
	// include this view in the eat-activating-click system.
	fJustCreated = TRUE;

	// Mouse move handling
	m_LastMouseState=0;
	m_LastMousePosition=wxDefaultPosition;
	m_LastMouseUsedTimestamp=0;
	m_CouldSkipNextMouse=FALSE;

}

/*********************************************************************************************
>	virtual CCamView::~CCamView()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    -
	Outputs:	-
	Returns:	-
	Purpose:	Destroys a CCamView window.  Deletes child windows first.
				Updates the CCamView population counter.
	Errors:	    -
	Scope:		Public
	SeeAlso:	CCamView::CCamView()

**********************************************************************************************/ 

CCamView::~CCamView()
{
	TRACEUSER("Gerry", _T("Deleting CCamView at 0x%08x\n"), this);

	PopCount--;			// one less ScreenView in the world, sigh

	if(pDocView != NULL)
		delete pDocView;
	
	if(Status != NULL)
		delete Status;

	pDocView = NULL;
	Status = NULL;

	// We shouldn't have a print view here!
	if (pPrintView != NULL)
	{
		ERROR3("CCamView is dying when it still has a PrintView attached!");
		delete pPrintView;
	}
}

/*********************************************************************************************/

// What to do when a view is created. Creates actual
// windows for displaying the view.
bool CCamView::OnCreate( wxDocument *pDoc, /* TYPENOTE: Correct */ long flags )
{
	TRACEUSER("Gerry", _T("CCamView::OnCreate at 0x%08x\n"), this);

	// Construct the (C++) child windows.
PORTNOTE("other","ScreenCamView::OnCreate - Removed scroller / ruler usage")
	RenderWindow = new CRenderWnd(this);
	HScrollBar = new CWinScroller(TRUE);
	VScrollBar = new CWinScroller(FALSE);

#ifndef EXCLUDE_FROM_XARALX
	Corner = new CScrollerCorner;
	HRuler = new OILHorizontalRuler;
	VRuler = new OILVerticalRuler;
	OGadget = new OriginGadget;
#endif // EX_LX
	if (!RenderWindow
		|| !HScrollBar 
		|| !VScrollBar 
#ifndef EXCLUDE_FROM_XARALX
		|| !Corner 
		|| !HRuler ||!VRuler	||!OGadget
#endif // EX_LX
		)
	{
		Error::SetError(_R(IDE_CREATE_VIEW_FAILED), 0);
		InformError();
		return false;
	} 

    m_pFrame = wxGetApp().CreateChildFrame( pDoc, this );
    m_pFrame->SetTitle(_T("DrawingView"));

	INT32 width, height;
	m_pFrame->GetClientSize( &width, &height );

	// Now do the other half of the construction.  The stated sizes and
	// positions of the windows here will be changed very soon.
	const WinRect rcARect(-100,-100,-90,-90);
	if (!RenderWindow->Create(rcARect, m_pFrame, WID_RENDERWINDOW)
		|| !HScrollBar->Create(m_pFrame, WID_HSCROLLBAR, wxPoint(-100, -100))
		|| !VScrollBar->Create(m_pFrame, WID_VSCROLLBAR, wxPoint(-100, -100))
//		|| !Corner->Create("", "", 0, rcARect, this, WID_SIZECORNER)
//		|| !HRuler->Create(this, WID_HRULER)
//		|| !VRuler->Create(this, WID_VRULER)
//		|| !OGadget->Create(this, WID_RULERORIGIN)
		)
	{
		Error::SetError(_R(IDE_CREATE_VIEW_FAILED), 0);
		InformError();
		return false;
	}

	CreateNewDocView();

	// init the kernel rulers and establish pointer links to them
PORTNOTE("other","ScreenCamView::OnCreate - Removed ruler usage")
#ifndef EXCLUDE_FROM_XARALX
	RulerPair* pRulers=pDocView->GetpRulerPair();
	pRulers->Init(pDocView,HRuler,VRuler,OGadget);
	HRuler->LinkToKernel(pRulers->GetpHorizontalRuler());
	VRuler->LinkToKernel(pRulers->GetpVerticalRuler());
#endif
	ENSURE(pDocView != 0, "CCamView::ScreenView can't get a new DocView!");
	pDocView->ConnectToOilView(this);
	
	// find the last view so we can use some of it's settings to create the new
//	DocView * LastView = DocView::GetSelected();

	// Link this and the DocView to the ViewState object.
	pDocView->SetViewState(Status);

////////////////////////////////////////////////////////////////////////////////
	
	wxScreenDC dc;
	wxSize pixsize=OSRenderRegion::GetFixedDCPPI(dc);

	// Tell DocView how big the pixel size is.
	FIXED16 PixelWidth  = FIXED16(72000.0/pixsize.x);
	FIXED16 PixelHeight = FIXED16(72000.0/pixsize.y);
	ERROR3IF(PixelWidth != PixelHeight, "Luke says non-square pixels are not supported");
	pDocView->SetPixelSize(PixelWidth, PixelHeight);

	// Make our DocView the current DocView
	pDocView->SetCurrent();

	if (GetFrame())
		GetFrame()->GetClientSize(&CurrentSize.width,&CurrentSize.height);
	// Now the scrollers have all their information, we can set their appearance.
	// Make sure that they are invisible until the rest of the frame is ready
/*	XLONG x1 = CurrentSize.GetWidth () * PixelWidth;
	XLONG x2 = CurrentSize.GetHeight() * PixelHeight;
	GetFrame()->SetScrollbar(wxHORIZONTAL,0,x1,Status->WorkAreaExtent.hi.x-Status->WorkAreaExtent.lo.x,false);
	GetFrame()->SetScrollbar(  wxVERTICAL,0,x2,Status->WorkAreaExtent.hi.y-Status->WorkAreaExtent.lo.y,false);
*/
 	
 	ShowScrollers(DefaultScrollersState);
	ShowRulers(DefaultRulersState);

///////////////////////////////////////////////////////////////////////////////
	
	// Register for WM_DROPFILES messages
//	DragAcceptFiles(TRUE);

///////////////////////////////////////////////////////////////////////////////

	// now that the ScreenView (and hence DocView) is stable, broadcast a message to let everyone know
	BROADCAST_TO_ALL(DocViewMsg(pDocView,DocViewMsg::NEWANDSTABLE));

#ifdef __X__
    // X seems to require a forced resize
    m_pFrame->SetSize(wxDefaultCoord, wxDefaultCoord, width, height);
#endif
    m_pFrame->Show(true);

	// This has been removed as it causes the problem where loaded documents
	// don't render correctly if a wxYield happens before the document has 
	// finished loading.  It may be possible to rewrite the handling in this 
	// class so that the order of activation and OnInitialUpdate is not important 
	// but efforts so far have been in vain.
	// The view seems to be activated correctly in any case so hopefully it 
	// should not be necessary
//	Activate(true);

	TRACEUSER("Gerry", _T("Leaving CCamView::OnCreate at 0x%08x\n"), this);

    return true;
}

/*********************************************************************************************/

// Clean up windows used for displaying the view.
bool CCamView::OnClose( bool fDeleteWindow /*= TRUE*/ )
{
	TRACEUSER("Gerry", _T("CCamView::OnClose at 0x%08x\n"), this);

	if( !GetDocument()->Close() )
		return false;
	
	SetFrame( (wxFrame *)NULL );
	
	Activate( false );

	if( fDeleteWindow )
	{
		TRACEUSER("Gerry", _T("CCamView::OnClose deleting frame at 0x%08x\n"), m_pFrame);
		m_pFrame->Show( false );
		
		delete m_pFrame;
		m_pFrame = NULL;

		// This will also have deleted the child windows of the frame making our
		// pointers invalid so vape them here
		RenderWindow = NULL;
		HScrollBar = NULL;
		VScrollBar = NULL;

#ifndef EXCLUDE_FROM_XARALX
		Corner = NULL;
		HRuler = NULL;
		VRuler = NULL;
		OGadget = NULL;
#endif // EX_LX

	}

	return true;
}

/********************************************************************************************
>	static BOOL CCamView::ReadViewPrefs()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/11/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the .INI file settings were loaded (or created) successfully.
	Purpose:	Tries to load preferences for CCamView, creating them with default values
				if they don't exist.
	Errors:		-
	SeeAlso:	Application::DeclareSection; Application::DeclarePref
********************************************************************************************/

BOOL CCamView::ReadViewPrefs()
{
	// Declare and load the .INI file settings.
	BOOL ok =  	Camelot.DeclareSection(TEXT("Mouse"), 10) &&
		   		Camelot.DeclarePref(TEXT("Mouse"), TEXT("DragLatency"),
		   					   		&CCamView::DragLatency, 1, 10) &&
		   		Camelot.DeclarePref(TEXT("Mouse"), TEXT("DragDelay"),
		   					   		&CCamView::DragDelay, 100, 2000) &&
		   		Camelot.DeclarePref(TEXT("Mouse"), TEXT("MoveEliminationPeriod"),
		   					   		&CCamView::s_MouseMoveEliminationPeriod, 0, 1000) &&

			 	Camelot.DeclareSection(TEXT("WindowFurniture"), 10) &&
#ifndef EXCLUDE_FROM_XARALX
		   	    Camelot.DeclarePref(TEXT("WindowFurniture"), TEXT("PropScrollers"),
		   					   	   &CCamView::PropScrollersOn, FALSE, TRUE) &&
#endif
		   	    Camelot.DeclarePref(TEXT("WindowFurniture"), TEXT("ScrollersVisibiltyState"),
		   					   	   &CCamView::DefaultScrollersState, FALSE, TRUE)&&
		   	    Camelot.DeclarePref(TEXT("WindowFurniture"), TEXT("RulersVisibiltyState"),
		   					   	   &CCamView::DefaultRulersState, FALSE, TRUE) &&

#ifndef EXCLUDE_FROM_XARALX
			 	Camelot.DeclareSection(TEXT("Windows"), 10) &&
		   	    Camelot.DeclarePref(TEXT("Windows"), TEXT("UnzoomOnNewView"),
		   					   	   &CCamView::UnzoomOnNewView, FALSE, TRUE) &&
				Camelot.DeclarePref(TEXT("Windows"), TEXT("ZoomSingleView"),
		   					   	   &CCamView::ZoomSingleView, FALSE, TRUE) &&
#endif
				TRUE ;

	return ok;
}

///////////////////////////////////////////////////////////////////////////////////////////
// Interface to the kernel's DocView and Document.

/*********************************************************************************************
>	CCamDoc* CCamView::GetDocument() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to the CCamDoc object associated with this CCamView.
	Purpose:	Mainly used by MFC.
	Errors:		ASSERTion failure if CCamView::m_pDocument does not point to
				a CCamDoc.
	Scope:		Public
	SeeAlso:	class CCamDoc; DocView::GetDocument
**********************************************************************************************/ 

CCamDoc* CCamView::GetDocument() const
{
	ENSURE(	m_viewDocument->IsKindOf( CLASSINFO(CCamDoc) ), 
			"CCamView::GetDocument: Document is not a Camelot document");
	return (CCamDoc *)m_viewDocument;
}



/*********************************************************************************************
>	BOOL CCamView::CreateNewDocView()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/6/96
	Inputs:		-
	Outputs:	-
	Returns:	Success/Fail
	Purpose:	Creates a new attached DocView - overridden in RalphView 
				to avoid the no Document problem.
	
**********************************************************************************************/ 

BOOL CCamView::CreateNewDocView()
{
	
	Document* KernelDoc = GetDocument()->GetKernelDoc();
	pDocView = KernelDoc->GetNewDocView();
	return TRUE;

}

/********************************************************************************************
>	ViewState* CCamView::GetViewState() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/94
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to the ViewState shared between this CCamView with a DocView.
	Purpose:	Allows access to the platform-independent description of a view.
	Errors:		-
	SeeAlso:	DocView::SetViewState
********************************************************************************************/

ViewState* CCamView::GetViewState() const
{
	ENSURE(Status != NULL, "Null ViewState* in CCamView::GetViewState");
	return Status;
}



/********************************************************************************************
>	void CCamView::SetViewState(ViewState* pvs)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/9/95
	Inputs:		pvs			the new view-state
	Returns:	pointer to the old view-state
	Purpose:	Sets this CCamView's shared ViewState object.
	SeeAlso:	CCamView::GetViewState
********************************************************************************************/

ViewState* CCamView::SetViewState(ViewState* pvs)
{
	ViewState* pvsOld = Status;
	Status = pvs;
	if (pvsOld->pView != NULL) pvsOld->pView->SetViewState(Status);
	return pvsOld;
}



/********************************************************************************************
>	void CCamView::SetCurrentStates()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/96
	Inputs:		-
	Purpose:	Set the  View and Document current
	SeeAlso:	
********************************************************************************************/

void CCamView::SetCurrentStates()
{
	if(pDocView)
	{
		pDocView->SetCurrent();
		Document* KernelDoc = pDocView->GetDoc();
		if(KernelDoc)
			KernelDoc->SetCurrent();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
// Painting.

/********************************************************************************************

>	CNativeDC* CCamView::GetRenderDC()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/09/95
	Returns:	pointer to a CNativeDC for rendering into this view
	Purpose:	Access function for this CCamView's device context.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CNativeDC* CCamView::GetRenderDC() const
{
	return RenderWindow?RenderWindow->GetClientDC():NULL;
}

/********************************************************************************************

>	void CCamView::DoneWithDC()

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	12/06/2006
	Purpose:	Hints that we've done with our DC
	SeeAlso:	View; PaperRenderRegion.

Note this is merely a hint. This routine is not guaranteed to eb called

********************************************************************************************/

void CCamView::DoneWithDC() const
{
	if (RenderWindow)
		RenderWindow->DoneWithDC();
}

/********************************************************************************************

>	void CCamView::AllocateDC()

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	12/06/2006
	Purpose:	Hints that we've done with our DC
	SeeAlso:	View; PaperRenderRegion.

Note this is merely a hint. This routine is not guaranteed to eb called

********************************************************************************************/

void CCamView::AllocateDC() const
{
	if (RenderWindow)
		RenderWindow->AllocateDC();
}


/*********************************************************************************************
>	void CCamView::GetClientSize(int * pWidth, int * pHeight) const    TYPENOTE: Correct

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	The width and height of the client area which the kernel can render into.
	Returns:	-
	Purpose:	Calculates the size of the CCamView's RendWnd.
	Errors:		-
	Scope:		Public
	SeeAlso:	class CRendWnd

**********************************************************************************************/ 

void CCamView::GetClientSize(int * pWidth, int * pHeight) const /* TYPENOTE: Correct */
{
	if (GetRenderWindow())
	{
		WinRect wrect(0,0,0,0);
		GetRenderWindow()->GetClientSize(&wrect.width, &wrect.height);
		if (wrect.width<0) wrect.width = 0;
		if (wrect.height<0) wrect.height = 0;
		OilRect orect = wrect.ToOil(pDocView);
		*pWidth  = orect.Width();
		*pHeight = orect.Height();
	}
	else
	{
		*pWidth = 0;
		*pHeight = 0;
	}
}


// set this to 1 to show update rectangles using cross-hatching, 0 to not
#define	SHOW_RECT 0

#if	SHOW_RECT
void DumpRect( wxDC *pDC, WinRect *lpRect, TCHAR *type )
{
	static INT32 BrushNum = wxFIRST_HATCH;
	wxBrush Brush;

	Brush.SetStyle(BrushNum);
	Brush.SetColour(0, 0, 0);

	if (++BrushNum > wxLAST_HATCH)
		BrushNum = wxFIRST_HATCH;

	pDC->SetBrush(Brush);
	pDC->DrawRectangle(*lpRect);

	if (type)
	{
		TRACE( _T("%s (%ld, %ld) [%ld, %ld]"), type, lpRect->GetLeft(), lpRect->GetTop(), 
			lpRect->GetWidth(), lpRect->GetHeight() );
	}
}

#else
#define	DumpRect(pdc, rect, name)
#endif

/*********************************************************************************************

>	virtual void CCamView::OnDraw(CDC* pDC)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    A pointer to the CDC object to render into.
	Outputs:	-
	Returns:	-
	Purpose:	Makes sure that the kernel knows which view to render into.  Retrieves the
				clipping rectangle from the CDC.  Passes the CDC and the rectangle to
				DocView::OnDraw(), which does the actual rendering.
	Errors:	    -
	Scope:		Public
	SeeAlso:	CView::OnPrepareDC(); CCamView::OnRenderView(); DocView::OnDraw()

**********************************************************************************************/ 

void CCamView::OnDraw( wxDC* pDC )
{
//	TRACEUSER("Gerry",  _T("CCamView::OnDraw\n"));

	if (pDocView==NULL)
		return;
		
	// Set the current DocView, ie. the DocView being worked upon. (and the Doc!)
	pDocView->SetCurrent();
	(pDocView->GetDoc())->SetCurrent();

	// Are we printing?
	if (CCDC::ConvertFromNativeDC(pDC)->IsPrinting())
	{
		//TRACE( _T("BAD ! CCamView::OnDraw reckons we're printing\n"));
#ifndef STANDALONE

		// Yes - do the printing!

		// Find out from the display context the rectangle bounding the invalid
		// region.
		WinRect clip;
		pDC->GetClippingBox(clip);

		if (clip.IsEmpty())
			return;

		// Pass the drawing parameters on to the associated PrintView object.
		pPrintView->AttachToDC(pDC);
		pPrintView->OnDraw(pDC, clip.ToOil(pPrintView));
		pPrintView->AttachToDC(NULL);

#endif
	}
	else
	{
		WinRect clip;

		// wxWidget only provides a unified accessor, so that's
		// what we'll use
		wxRegionIterator upd( GetRenderWindow()->GetUpdateRegion() ); // get the update rect list
		while( upd )
		{
			clip.x		= upd.GetX();
			clip.y		= upd.GetY();
			clip.width  = upd.GetW();
			clip.height = upd.GetH();

//			DumpRect( pDC, &clip, wxT("OnDraw UpdateRect : ") );

			OilRect oRect = clip.ToOil( pDocView );
//			TRACEUSER("Gerry", _T("OilRect = (%d, %d) - (%d, %d)\n"), oRect.lo.x, oRect.lo.y, oRect.hi.x, oRect.hi.y);
			
			pDocView->OnDraw( pDC, oRect );

			upd ++ ;
		}
	}

//	TRACEUSER("Gerry", _T("Leaving CCamView::OnDraw\n"));
}

/*********************************************************************************************

>	afx_msg LRESULT CCamView::OnRenderView(WPARAM, LPARAM lp)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    A pointer to a CPaintDC, cast into an LPARAM.  The WPARAM is not used.
	Outputs:	-
	Returns:	Returns 0, indicating to Windows that the message was successfully
				processed.
	Purpose:	Responds to a WM_PAINT message sent to the RendWnd window object.
				Effectively tricks MFC into painting the RendWnd as if it was a
				CView window, not a child of a CView - calls CView::OnPrepareDC()
				and ScreenCamView::OnDraw().
	Errors:		-
	Scope:		Protected
	SeeAlso:	class CRendWnd; CRenderWnd::OnPaint(); ScreenCamView::OnDraw();

**********************************************************************************************/ 

LRESULT CCamView::OnRenderView( WPARAM, LPARAM lp )
{
	PORTNOTETRACE("other","CCamView::OnRenderView - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	wxPaintDC		  *pDC = (wxPaintDC *)lp;
	OnPrepareDC(pDC);
	OnDraw(pDC);
#endif
	return 0;
}

#define USERNAME "Mike"

//static RECT DriverBand;
//static RECT TestBand;

/********************************************************************************************

>	void CCamView::OnFilePrint()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/09/95
	Purpose:	The main printing control function.  This is a modified version of the MFC
				one because MFC does not allow the concept of having a mapping between
				pages and physical pieces of paper other than 1 to 1.  i.e. MFC can't handle
				two-up printing, fit lots, pamphlet printing etc.
	Errors:		-
	SeeAlso:	CView::OnFilePrint(); CCamView::OnFilePrintPreview()

********************************************************************************************/
//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER
void CCamView::OnFilePrint()
{
#ifndef _DEBUG
	::wxMessageBox(_T("Printing currently disabled in retail builds"));
	return;
#endif

	SetCurrentStates();

	// Set Current states...
	Document* KernelDoc = GetDocument()->GetKernelDoc();
	ENSURE(KernelDoc != NULL, "Oh no!!");
	KernelDoc->SetCurrent();
	pDocView->SetCurrent();

	TRACEUSER( USERNAME, _T("OnFilePrint()\n"));

	// get default print info
	CCPrintInfo *pPrintInfo;
	pPrintInfo = new CCPrintInfo(KernelDoc,this);
	if (!pPrintInfo)
	{
		// Out of memory - report error and exit.
		Error::SetError(_R(IDS_OUT_OF_MEMORY));
		InformError();
		return;
	}


	// Make sure the user can't get 2 print dialog boxes up
	PrintMonitor::SetPrintingActive(TRUE);

	// Now bring up the print dialog
	if (!pPrintInfo->OnPreparePrinting())
	{
		TRACEUSER( USERNAME, _T("OnPreparePrinting() returned FALSE\n"));
		delete pPrintInfo;
		PrintMonitor::SetPrintingActive(FALSE);
		return;
	}

	List CompoundNodeList;
	NodeCompound * pCompound = NULL;
	NodeListItem * pItem = NULL;
	// Get ourselves a print view and attach it to this CCamView.
	// NB. Must be after the user has clicked OK on print dialog, as we set up
	//     anti-aliasing etc. in PrintView constructor.
	// Delete any old print view
	if (pPrintView != NULL)
	{
		delete pPrintView;
		pPrintView = NULL;
		delete pPrintInfo;
		PrintMonitor::SetPrintingActive(FALSE);
		ERROR2((void)0, "OnPreparePrinting(): we already have a PrintView!");
	}
	// make a new one
	pPrintView = new PrintView(pDocView->GetDoc());
	if (pPrintView == NULL)
	{
		delete pPrintInfo;
		PrintMonitor::SetPrintingActive(FALSE);
		ERROR2((void)0, "Not enough memory to create PrintView object");
	}
	// and connect it to the OilView
	if (!pPrintView->ConnectToOilView(this))
	{
		delete pPrintView;
		pPrintView = NULL;
		delete pPrintInfo;
		PrintMonitor::SetPrintingActive(FALSE);
		ERROR2((void)0, "Unable to attach PrintView to CCamView.");
	}

	double dpi = 96.0;

	PrintControl* pPrCtrl = NULL;
	PrintComponent* pComp = (PrintComponent*)KernelDoc->GetDocComponent(CC_RUNTIME_CLASS(PrintComponent));
	if (pComp != NULL)
		pPrCtrl = pComp->GetPrintControl();

	if (pPrCtrl != NULL)
		pPrCtrl->SetUp(KernelDoc->GetSelectedSpread());

	wxString strTemp;

	TRACEUSER( "ChrisS", _T("------------------------------------------------------------------------\n"));
	TRACEUSER( "ChrisS", _T("------------------------------------------------------------------------\n"));
	TRACEUSER( "ChrisS", _T("Entering print loop\n"));
	TRACEUSER( "ChrisS", _T("------------------------------------------------------------------------\n"));
	TRACEUSER( "ChrisS", _T("------------------------------------------------------------------------\n"));

//	ERROR3IF(pPrintInfo->m_pPD->m_pd.hDC == NULL, "NULL DC when trying to print.");
	// must be set (did you remember to call DoPreparePrinting?)

	// Let's see if we can start printing...
	if ((!pPrintInfo->GetDC()) || 
		!PrintMonitor::StartPrintJob(pPrintInfo->GetDC()))
	{
		TRACEUSER( USERNAME, _T("StartPrintJob() screwed up\n"));

		// No - something went wrong.
		if (!pPrintInfo->GetDC())
		{
			// No DC - set a generic "we can't print" error.
			Error::SetError(_R(IDE_PRINT_ERROR_SYSTEM));
		}

		// Tell user and exit.
		InformError();
		delete pPrintInfo;
		PrintMonitor::SetPrintingActive(FALSE);
		return;
	}

	// OnBeginPrinting(pPrintInfo->GetDC(), pPrintInfo);

	// Get access to our print control object
	PrintControl *pPrintControl = pPrintView->GetPrintControl();

	Spread * pSpread = Document::GetSelectedSpread();

	// disable main window while printing & init printing status dialog
//	AfxGetMainWnd()->EnableWindow(FALSE);

PORTNOTE("printing", "Disable pDocInfo stuff");
#ifndef EXCLUDE_FROM_XARALX
	// set up document info and start the document printing process
	DOCINFO* pDocInfo = NULL;

	// start document printing process
	if  ((pPrintInfo->SetAbortProc(&dcPrint) < 0) ||
		 !pPrintInfo->SetUpDocInfo(&pDocInfo) 	  ||		// SetUpDocInfo() allocs a DOCINFO for pDocInfo
		 pDocInfo == NULL 						  ||
#else
	if (
#endif
		 (!pPrintInfo->GetDC()->StartDoc(wxString((TCHAR *)(Document::GetSelected()->GetTitle()))))
		)
	{
		TRACEUSER( USERNAME, _T("Unable to start document\n"));

		// enable main window before proceeding
//		AfxGetMainWnd()->EnableWindow(TRUE);

		// cleanup and show error message
//		OnEndPrinting(&dcPrint, pPrintInfo);

		BOOL PrintToFileUserAborted = pPrintInfo->GetPrintControl ()->GetPrintToFile ();

		delete pPrintInfo;
		pPrintInfo = NULL;
		PrintMonitor::EndPrintJob();

		// Lose our PrintView
		delete pPrintView;
		pPrintView = NULL;

PORTNOTE("printing", "Disabled pDocInfo")
#ifndef EXCLUDE_FROM_XARALX
		// Delete a the docinfo object
		if (pDocInfo != NULL)
		{
			delete pDocInfo;
			pDocInfo = NULL;
		}
#endif

//		dcPrint.Detach();   // will be cleaned up by CPrintInfo destructor
		if (!PrintToFileUserAborted)
		{
			InformError(_R(AFX_IDP_FAILED_TO_START_PRINT));
		}
		PrintMonitor::SetPrintingActive(FALSE);
		return;
	}

	BOOL StartedPrinting = FALSE;

	// CGS:  any blended compound nodes MUST be tagged as having been generated for printing
	NodeGroup::SetIsForPrinting (TRUE);

	// begin page printing loop
	BOOL bError = TRUE;

	// Set up the print layout system.
	// This allows GetNextPaper() and ReprintPaper() functions
	// to be called.
	if (!pPrintInfo->StartPrinting())
	{
		TRACEUSER( USERNAME, _T("StartPrinting() failed\n"));
		// Error in starting printing.
		Error::SetError(_R(IDE_PRINT_ERROR_SYSTEM));
		goto ExitPrint;
	}

	// Must remember to tell print info object when we are done.
	StartedPrinting = TRUE;

	// Work out if we should do our own PostScript
	BOOL DoPostScript;
	if ((pPrintControl->GetPrintMethod() == PRINTMETHOD_NORMAL) &&
		(pPrintInfo->GetCCDC()->GetRenderType() == RENDERTYPE_PRINTER_PS))
	{
		// Not printing as a bitmap and it is a PostScript printer so mark it as such.
		DoPostScript = TRUE;
	}
	else
	{
		// Either it's not a PostScript printer or we are printing as a bitmap,
		// so don't do weird PostScript stuff
		DoPostScript = FALSE;
	}

	// If we are printing as bitmap, then set the "Printing blah.art" message in the dialog.
	// (This is done by RenderSimpleView() when we are printing normally).
	if ((pPrintControl->GetPrintMethod() != PRINTMETHOD_NORMAL) && (pPrintInfo != NULL))
		// We going to print the document now
		pPrintInfo->SetPrinting();

PORTNOTE("printing", "Disable banding call");
#ifndef EXCLUDE_FROM_XARALX
	// Ask the printer if it supports banding. We don't actually need the result of this call,
	// but asking the printer about banding has the side effect that the printer driver then
	// knows that we support banding! If we don't do this, then some printing (e.g. to an HP DeskJet
	// on Win3.1) simply chucks out blank pages, because the driver thinks we can't band, and there
	// isn't enough memory/disc space (or something) to do the page in one go.
	//
	// BLOCK
	{
		BOOL BandInfoSupported = FALSE;
		WORD wEscape = BANDINFO;
		if (!DoPostScript && 
			PrintMonitor::PrintWithDriverBands &&
			dcPrint.Escape(QUERYESCSUPPORT, sizeof(WORD), (LPCSTR) &wEscape, NULL) > 0)
		{
			// The BANDINFO Escape is supported  -  but we don't actually care!
			BandInfoSupported = TRUE;
		}
	}
#endif

	// DMc 22-7-99
	// MRH 11/9/00 - Major rewrite of the below david code. Fixed major logic problem and
	// cut out the regeneratenodes function as this is not required!
	BevelTools::GetAllNodesUnderNode(pSpread, &CompoundNodeList, CC_RUNTIME_CLASS(NodeCompound));
	
	dpi = OSRenderRegion::GetFixedDCPPI(*pPrintInfo->GetDC()).GetWidth();

	if (pPrintControl)
		dpi = (double)pPrintControl->GetDotsPerInch();
	
	pItem = (NodeListItem *)CompoundNodeList.GetHead();
	
	while (pItem)
	{
		pCompound = (NodeCompound *)pItem->pNode;
		
		if (pCompound)
		{
			if (pCompound->RegenerateForPrinting())
			{
				pCompound->SetDPI(dpi);
				pCompound->SetPrinting(TRUE);
			}
		}
		
		pItem = (NodeListItem *)CompoundNodeList.GetNext(pItem);
	}
	
	TRACEUSER( "DavidM", _T("Beginning print dpi %d\n"), dpi);

	// Loop around for each physical page ("paper") we have to print. Note that now we may print out
	// multiple physical pieces of paper for each "paper" (page) as we may do C, M, Y, K plates etc.

	while (pPrintInfo->MorePaper())
	{
		// Start off the print marks system bits for this paper
		// We lock progress window updates during this call so that the progress window will
		// ignore all attempts to update the display.
		pPrintInfo->LockProgressUpdate(TRUE);
			EnumeratePagePatches(pPrintInfo);
		pPrintInfo->LockProgressUpdate(FALSE);

		// Initialise the plate printing system for any plates of this page
		// It also sets up GetNextPlate() and ReprintPlate functions
		UINT32 plateerr;
		if (!pPrintInfo->StartPlatePrinting(pPrintView, &plateerr))
		{
			TRACEUSER( USERNAME, _T("StartPlatePrinting() failed\n"));
			// Error in startplateprinting! the error id may have been returned
			// We ignore the id if zero, ie StartPlatePrinting reported it.
			if (plateerr>0)
			{
				String_64 reason;
				String_256 ErrorMsg;
				BOOL ok = ( reason.Load(plateerr) );
				ok = ok && ( ErrorMsg.MakeMsg( _R(IDE_PRINT_ERROR), (LPCTSTR) reason) > 0);
				if (ok)
					Error::SetError(_R(IDE_PRINT_ERROR), (TCHAR *) ErrorMsg, 0);
				else
					Error::SetError(_R(IDE_PRINT_USERABORT));
			}
			goto ExitPrint;
		}
		
		// Loop around for each logical plate we have to print - this generates a piece of physical
		// paper out of the printer on each pass.
		while (pPrintInfo->MorePlates())
		{
			// Get the first "paper" (plate or page) to print
			pPrintInfo->GetNextPaper();

			// and set up the plate printing system ready to print the next plate on the next pass
			if (!pPrintInfo->SetNextPlate(pPrintView))
			{
				TRACEUSER( USERNAME, _T("SetNextPlate() failed!\n"));
				// Error in starting printing.
				Error::SetError(_R(IDE_PRINT_ERROR_SYSTEM));
				pPrintInfo->EndPlatePrinting(pPrintView);
				goto ExitPrint;
			}

			// Now, prepare this paper (plate/page) for printing
			TRACEUSER( USERNAME, _T("Starting physical page\n"));			
//			OnPrepareDC(pPrintInfo->GetDC(), pPrintInfo);

			// check for end of print
			if (!pPrintInfo->m_bContinuePrinting || pPrintInfo->Abort())
			{
				TRACEUSER( USERNAME, _T("Print abort 1\n"));
				pPrintInfo->EndPlatePrinting(pPrintView);
				goto ExitPrint;
			}

			// attempt to start the current paper
			pPrintInfo->GetDC()->StartPage();
			if (0) // error handling non-existant
			{
				TRACEUSER( USERNAME, _T("Unable to StartPage()\n"));
				Error::SetError(_R(IDE_PRINT_ERROR_SYSTEM));
				pPrintInfo->EndPlatePrinting(pPrintView);
				goto ExitPrint;
			}				  

			// Is it a PostScript printer?
			if (DoPostScript)
			{
				// Yes, so before we start the page, we send our PostScript procset to the device.
				if (!PrintPSRenderRegion::InitPSDevice(pPrintInfo->GetDC(), pPrintView))
				{
					TRACEUSER( USERNAME, _T("Unable to Init PS device\n"));
					Error::SetError(_R(IDE_PRINT_ERROR_SYSTEM));
					pPrintInfo->EndPlatePrinting(pPrintView);
					goto ExitPrint;
				}
			}

PORTNOTE("printing", "Disable banding call");
#ifndef EXCLUDE_FROM_XARALX
			// If banding, find the first band to print
			// Only use driver banding if the preference says so, and never band
			// to PS devices (yes, the Win95 PS driver supports banding, except
			// not really and it just screws up).
			if (PrintMonitor::PrintWithDriverBands && !DoPostScript)
			{
				// Do the driver banding
				dcPrint.Escape(NEXTBAND, 0, NULL, (LPSTR) &DriverBand);

				TRACEUSER( USERNAME, _T("INITIALBAND: (%d,%d), (%d, %d)\n"),
						 DriverBand.left, DriverBand.top, DriverBand.right, DriverBand.bottom);
			}
#endif

			TRACEUSER( USERNAME, _T("Starting PrintPaper() loop...\n"));

			// Now render the paper into each band.
			BOOL MoreBands = TRUE;
			while (MoreBands)
			{
				TRACEUSER( USERNAME, _T("Calling PrintPaper()...\n"));
				// Print this piece of paper (to the current band)
				if (!PrintPaper(pPrintInfo->GetDC(), pPrintInfo, pPrintControl))
				{
					// Something went wrong - quit and abort print job.
					TRACEUSER( USERNAME, _T("PrintPaper() failed\n"));
					Error::SetError(_R(IDE_PRINT_ERROR_SYSTEM));
					pPrintInfo->EndPlatePrinting(pPrintView);
					goto ExitPrint;
				}

				// Only use driver banding if the preference says so, and never band to PS devices.
				MoreBands = FALSE;		// Assume no more bands until told otherwise
PORTNOTE("printing", "Disable banding call");
#ifndef EXCLUDE_FROM_XARALX
				if (PrintMonitor::PrintWithDriverBands && !DoPostScript)
				{
					// We've printed this band - see if printer driver has any bands left,
					// NB we kludge this as some drivers keep returning the same band...fabby eh?
					dcPrint.Escape(NEXTBAND, 0, NULL, (LPSTR) &DriverBand);
					TRACEUSER( USERNAME, _T("Driver returned valid band: (%d,%d), (%d, %d)\n"),
							  DriverBand.left, DriverBand.top, DriverBand.right, DriverBand.bottom);

					// If it does, then we need to loop around to print this physical piece of paper
					// into the next band...
					if (!IsRectEmpty(&DriverBand))
					{
						TRACEUSER( USERNAME, _T("Forcing a reprint of paper [for band: (%d,%d), (%d, %d)]\n"),
								  DriverBand.left, DriverBand.top, DriverBand.right, DriverBand.bottom);
						MoreBands = TRUE;
					}
					else
					{
						TRACEUSER( USERNAME, _T("End of bands for this page\n"));
					}
				}
#endif

				// If we're going to print more bands, then set up the "paper" for the next band
				if (MoreBands)
				{
					// Tell the print control that we want the same "paper" again, rather than moving
					// on to the next one.
					pPrintControl->ReprintPaper();

					// And reset all the PrintInfo data for this "paper" ready for the next band
					pPrintInfo->GetNextPaper();
				}
			}

			TRACEUSER( USERNAME, _T("Ending physical page\n"));
			
			// End of page clean up.
			// Note that we don't check for an error from EndPage() because due to
			// a bug (in Windows/GDI) it returns -1 when using banding.
			// The MS workaround is to not check the return value!
			pPrintInfo->GetDC()->EndPage();

			if (!pPrintInfo->m_bContinuePrinting || pPrintInfo->Abort())
			{
				TRACEUSER( USERNAME, _T("Print abort 2\n"));
				pPrintInfo->EndPlatePrinting(pPrintView);
				goto ExitPrint;
			}
		
			// Any more plates to print?, if so we need to print the plate as a complete new page.
			// We set ReprintPaper so that the next pass tries to print the same "paper" (document page)
			// again, but we've moved it on to the next plate (in SetNextPlate, above), so we'll actually
			// be rendering a different plate the next time around.
			if (pPrintInfo->MorePlates())
				pPrintControl->ReprintPaper();
		}	

		// End the plate printing function
		pPrintInfo->EndPlatePrinting(pPrintView);
	}

	// Set the flag to indicate that we're exiting happily
	bError = FALSE;


ExitPrint:
	// cleanup document printing process
	PrintMonitor::EndPrintJob();

	if (!bError)
		pPrintInfo->GetDC()->EndDoc();
	else
	{
		TRACEUSER( USERNAME, _T("ExitPrint: an error occured()\n"));
		// An error occured - abort the job.
//		pPrintInfo->GetDC()->AbortDoc();

		// If the user did not cancel the job, report what went wrong.
		if (!pPrintInfo->Abort())
			InformError();
	}

//	AfxGetMainWnd()->EnableWindow();    // enable main window

//	OnEndPrinting(pPrintInfo->GetDC(), pPrintInfo);    // clean up after printing

	// Clean up kernel printing code
	if (StartedPrinting)
		pPrintInfo->EndPrinting();

PORTNOTE("printing", "Remove DOCINFO stuff");
#ifndef EXCLUDE_FROM_XARALX
	// Delete a the docinfo object
	if (pDocInfo != NULL)
	{
		delete pDocInfo;
		pDocInfo = NULL;
	}
#endif

	PrintMonitor::SetPrintingActive(FALSE);

	// Lose our PrintView (if it exists) - not needed anymore.
	if (pPrintView)
	{
		delete pPrintView;
		pPrintView = NULL;
	}


	if (pPrintInfo)
	{
		delete pPrintInfo;
		pPrintInfo = NULL;
	}

	// If we need to redraw everything then do it.
	if (PrintMonitor::FullRedrawNeeded)
	{
		// iterate through all the documents and get them to redraw themselves
		Document* pDoc = (Document*) Camelot.Documents.GetHead();

		while (pDoc!=NULL)
		{
			// Get it to redraw itself
			pDoc->ForceRedraw();

			// get the next document in the list
			pDoc = (Document*) Camelot.Documents.GetNext(pDoc);
		}
	}

	// mark the PrintMonitor as not needing a redraw
	PrintMonitor::WantFullRedraw(FALSE);

	// DMc restore the compound nodes dpi's
	pItem = (NodeListItem *)CompoundNodeList.GetHead();
	
	while (pItem)
	{
		pCompound = (NodeCompound *)pItem->pNode;
		
		if (pCompound)
		{
			if (pCompound->RegenerateForPrinting())
			{
				pCompound->SetDPI(96.0);
				pCompound->SetPrinting(FALSE);
			
				pCompound->RegenerateNode(NULL, FALSE);
			}
		}
		
		pItem = (NodeListItem *)CompoundNodeList.GetNext(pItem);
	}	

	CompoundNodeList.DeleteAll();

	NodeGroup::KillAllBlendBecomeAConsLists (FALSE, TRUE);
	NodeGroup::SetIsForPrinting (FALSE);

	TRACEUSER( "ChrisS", _T("------------------------------------------------------------------------\n"));
	TRACEUSER( "ChrisS", _T("------------------------------------------------------------------------\n"));
	TRACEUSER( "ChrisS", _T("Exiting print loop\n"));
	TRACEUSER( "ChrisS", _T("------------------------------------------------------------------------\n"));
	TRACEUSER( "ChrisS", _T("------------------------------------------------------------------------\n"));
}

#endif //webster



// Size of bands to use when printing as bitmap.
#define BAND_HEIGHT (128)

/********************************************************************************************

>	BOOL ScreenCamView::PrintPaper(CDC *pPrintDC, 
								   CCPrintInfo *pPrintInfo,
								   PrintControl *pPrintControl)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/11/95
	Inputs:		pPrintDC - the (printer) device context to print to.
				pPrintInfo - the object to use for layout control.
	Returns:	TRUE if the paper was printed ok;
				FALSE if not.
	Purpose:	Prints all the ink onto a physical piece of paper.  This includes coping
				with patches (multiple copies on one piece of paper, and banding bitmap
				printing to improve performance.
	Errors:		pPrintDC has a bad DC handle => ERROR3
				Problem with setting up clipping region => ERROR2
	SeeAlso:	ScreenCamView::OnFilePrint; ScreenCamView::GetPrintClipRect

********************************************************************************************/
//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER
BOOL CCamView::PrintPaper(CNativeDC *pPrintDC, 
							CCPrintInfo *pPrintInfo, 
							PrintControl *pPrintControl)
{
//	TRACEUSER( "Tim", _T("PrintPaper() started\n"));

//	TRACEUSER( "Tim", _T("PrintPaper() printing band=(%d,%d),(%d,%d)\n"),
//			  DriverBand.left, DriverBand.top, DriverBand.right, DriverBand.bottom);


	// Page successfully started, so now render the page by doing each 'patch'
	// in turn.  Note that we store the patch information in the print view,
	// which the view will use when setting up to render
	// (see PrintView::ConstructRenderingMatrix())
	while (pPrintInfo->GetNextPatch(&pPrintView->PatchInfo))
	{
		// Tell the view what scale this is.
		pPrintView->SetViewScale(pPrintView->PatchInfo.Scale / FIXED16(100));

		// Set up drawing rect to entire page (in logical coordinates)
		wxRect r(wxPoint(0, 0), pPrintDC->GetSize());
		wxCoord x1=pPrintDC->DeviceToLogicalX(r.GetLeft());
		wxCoord x2=pPrintDC->DeviceToLogicalX(r.GetRight()+1);
		wxCoord y1=pPrintDC->DeviceToLogicalY(r.GetTop());
		wxCoord y2=pPrintDC->DeviceToLogicalY(r.GetBottom()+1);
		wxCoord lowx=wxMin(x1,x2);
		wxCoord highx=wxMax(x1,x2);
		wxCoord lowy=wxMin(y1,y2);
		wxCoord highy=wxMax(y1,y2);
		pPrintInfo->m_rectDraw=wxRect(wxPoint(lowx,lowy),wxSize(highx-lowx,highy-lowy));

		// Let's band this to make performance acceptable.

		// Bodge banding - many printer drivers don't do banding so performance
		// is a dog (i.e. not enough memory to print on my 32Mb machine!)
		const INT32 BandHeight = BAND_HEIGHT;
		INT32 BottomOfPage = pPrintInfo->m_rectDraw.GetBottom()+1; //+1 is wxRect stupidity
//		const INT32 PageHeight = pPrintInfo->m_rectDraw.GetBottom() - pPrintInfo->m_rectDraw.GetTop();

		// Find print control object for this document, to see if we are
		// printing via bitmap, and so if we need to band the output.
		PrintMethodType PrintType;
		PrintType = pPrintControl->GetPrintMethod();

		// Work out whether or not to use banding for printing.
		// (i.e. are we printing with a bitmap?)
		if ((PrintType == PRINTMETHOD_AABITMAP) ||
			(PrintType == PRINTMETHOD_BITMAP))
		{
			// Banding is required - set up first band.
			pPrintInfo->SetSliderSubRangeMax(pPrintInfo->m_rectDraw.GetHeight() / BAND_HEIGHT);
			pPrintInfo->m_rectDraw.height = BandHeight;
		}

		INT32 CurrentBand = 0;

		TRACEUSER( "Tim", _T("Rectangle: (%d,%d), (%d,%d) BottomOfPage=%d\n"),
				 pPrintInfo->m_rectDraw.GetLeft(), pPrintInfo->m_rectDraw.GetTop(),
				 pPrintInfo->m_rectDraw.GetRight()+1,pPrintInfo->m_rectDraw.GetBottom()+1,
				 BottomOfPage);

		while (pPrintInfo->m_rectDraw.GetBottom()+1 <= BottomOfPage)
	    {
			// Update slider
			if ((PrintType == PRINTMETHOD_AABITMAP) ||
				(PrintType == PRINTMETHOD_BITMAP))
			{
				// Doing our own banding, so update progress slider.
				pPrintInfo->SetSliderSubRangePos(CurrentBand);
			}

			//
			// Strange stuff to work out what area to print in this band.
			//
			WinRect OILClip = GetPrintClipRect(pPrintDC, pPrintInfo);

			// Check for no intersection.
			if (!OILClip.IsEmpty())
			{
				// Put this clip region into print info structure.
				pPrintInfo->m_rectDraw = OILClip;

				wxRect oldclip(wxPoint(0,0), wxSize(0,0));
				pPrintDC->GetClippingBox(oldclip);
				pPrintDC->SetClippingRegion(pPrintInfo->m_rectDraw);

				// Print the band.
				pPrintView->SetCurrent();
				OnPrint(pPrintDC, pPrintInfo);

				pPrintDC->SetClippingRegion(oldclip);

				if (!pPrintInfo->m_bContinuePrinting || pPrintInfo->Abort())
					// Printing has finished.
					return FALSE;
			}

			// Is this the last band?
			if (pPrintInfo->m_rectDraw.GetBottom()+1 < BottomOfPage)
			{
				// No - get the next band.
				pPrintInfo->m_rectDraw.y = pPrintInfo->m_rectDraw.GetBottom()+1;
				pPrintInfo->m_rectDraw.height = BandHeight;

				// Limit to bottom of page if necessary.
				if (pPrintInfo->m_rectDraw.GetBottom()+1 > BottomOfPage)
					pPrintInfo->m_rectDraw.height -= pPrintInfo->m_rectDraw.GetBottom()+1 - BottomOfPage;
			}
			else
			{
				// No bands left - so we're done here.
				TRACEUSER( "Tim", _T("No bespoke banding\n"));
				break;
			}

			CurrentBand++;
		}
	}

	TRACEUSER( "Tim", _T("PrintPaper() finished\n"));

	// All worked ok
	return TRUE;
}
#endif //webster


/********************************************************************************************

>	WinRect CCamView::GetPrintClipRect(CDC *pPrintDC, CCPrintInfo *pPrintInfo)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/11/95
	Inputs:		pPrintDC - the device context we are printing to - we need this because
						   otherwise the PrintView object can't compute the rendering
						   matrix, which we need in order to find out the position of the
						   patch on the DC.
				pPrintInfo - the print info object, which specifies the window onto the
							 page/DC that we can print into.
	Returns:	The intersection of the area of the current patch (as held in 
				ScreenCamView::pPrintView->PatchInfo) and the printable area of the DC,
				in WinCoords.
	Purpose:	Works out which area of the paper to print on, given the current patch
				(as held in the PrintView object), and the printing area specified by
				the CCPrintInfo object.
	SeeAlso:	ScreenCamView::PrintPaper; PrintView::ConstructRenderingMatrix

********************************************************************************************/
//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER
WinRect CCamView::GetPrintClipRect(CNativeDC *pPrintDC, CCPrintInfo *pPrintInfo)
{
	// We need to attach the PrintView while we are doing this so it knows how big
	// the pixels are and so on.
	pPrintView->AttachToDC(pPrintDC);

	// Get the rendering matrix for this patch
	Spread *pSpread = Document::GetSelectedSpread(); // Should be PatchInfo.pSpread
	Matrix TheMatrix = pPrintView->ConstructRenderingMatrix(pSpread);

	// Use matrix to transform clip rect of patch to Oil coords.
	DocRect ClipRect = pPrintView->PatchInfo.GetClipRect(FALSE,FALSE);

	// turn this clip rect into an OilRect
	OilRect PatchClipRect(OilCoord(ClipRect.lo.x, ClipRect.lo.y),
					      OilCoord(ClipRect.hi.x, ClipRect.hi.y));

	TheMatrix.transform(&PatchClipRect.lo);
	TheMatrix.transform(&PatchClipRect.hi);

	// Rendering transform may involve a rotation so correct if necesary.
	pPrintView->CorrectRotatedRectangle((Rect *) &PatchClipRect);

	// We need to inflate the clipping rectangle as this is used to determin the
	// bitmap size when rendering to AABITMAP output (and BITMAP).
	// Ok, There is a problem here.... We need to set the clip rectangle to cover
	// the crop area too. Otherwise, when rendering text as text, GDI will bin the
	// crop mark text we render. We do this after the transform to avoid the transform
	// scaling this unscalable value.
	pPrintView->PatchInfo.InflateRectBy(&PatchClipRect, TRUE, TRUE);

	// Convert the OIL cliprect to logical GDI units
	WinRect WinClipRect;
	WinClipRect = PatchClipRect.ToWin(pPrintView);

	// Find intersection with clip rect.
	WinRect OILClip=WinClipRect;
	OILClip.Intersect(pPrintInfo->m_rectDraw);

	// Finished messing with the PrintView for the mo so we can detach the DC.
	// It is eventually re-attached (in ScreenCamView::OnDraw()) when we call OnPrint().
	pPrintView->AttachToDC(NULL);

	return OILClip;
}
#endif //webster

/********************************************************************************************

>	void CCamView::EnumeratePagePatches(CCPrintInfo *pPrintInfo)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/09/96
	Inputs:		pPrintInfo - the print info object, which specifies the window onto the
							 page/DC that we can print into.
	Returns:	
	Purpose:	In order to locate crop marks / registration marks etc correctly, we need
				to work out the bounds of all patches printed on a single sheet of paper.
				This routine does just that.
				
********************************************************************************************/
//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER
void CCamView::EnumeratePagePatches(CCPrintInfo *pPrintInfo)
{
#ifndef EXCLUDE_FROM_RALPH
#ifndef STANDALONE

	DocRect Bounds;
	DocRect cliprect;
	INT32 bleed;
	BOOL emuldwn;

	// get a pointer to the print marks manager
	PrintMarksMan *pMarksMan = GetApplication()->GetMarksManager();
	if (pMarksMan==NULL)
		return;

	// Destroy what we may already have from a previous piece of paper
	pMarksMan->ResetPageRects();

	PrintPatchInfo* pPatchInfo = &pPrintView->PatchInfo;
	if (pPrintInfo->GetNextPaper())
	{
		// scan through each patch on the first paper region
		while (pPrintInfo->GetNextPatch(pPatchInfo))
		{
			// Get raw patch clip rectangle
			cliprect = pPatchInfo->GetClipRect(FALSE,FALSE);
			bleed = pPatchInfo->GetBleed();
			emuldwn = pPatchInfo->EmulsionDown;
			// dont inflate by the bleed, this is passed in and used by the print marks manager
			//cliprect.Inflate(pPatchInfo->Bleed);
			// Build a Spread=>OS conversion matrix for this patch
			Matrix TheMatrix = pPrintView->ConstructRenderingMatrix(pPatchInfo->pSpread);
			// transform the patch
			TheMatrix.transform(&cliprect.lo);
			TheMatrix.transform(&cliprect.hi);
			// Rendering matrix may involve a rotation when printing so correct the rectangle
			// for this if necessary.
			pPrintView->CorrectRotatedRectangle((Rect *)&cliprect);
			// tell the marks manager about these bounds.
			pMarksMan->AddPageRect(cliprect);
		}
	}
	
	// Get the marks manager to update its imagesetting rect
	pMarksMan->SetImagesettingRect();
	pMarksMan->SetBleed(bleed);
	pMarksMan->SetEmulsionDown(emuldwn);
	
	// reprint this piece of paper so we dont upset the print system
	pPrintInfo->ReprintPaper();

#endif
#endif
}

#endif //webster

PORTNOTE("printing", "Disabled lots of printing code")
#ifndef EXCLUDE_FROM_XARALX

/*********************************************************************************************
>	BOOL CCamView::OnPreparePrinting(CPrintInfo* pInfo)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	by the wonderful AppWizard
	Inputs:	    -
	Outputs:	Fills the passed CPrintInfo with some stuff.
	Returns:	TRUE if successful preparation for printing.
	Purpose:	Current just returns that value of CView::DoPreparePrinting()
	Errors:		-
	Scope:		Public
	SeeAlso:	CView::DoPreparePrinting()

**********************************************************************************************/ 

BOOL CCamView::OnPreparePrinting(CPrintInfo* pInfo)
{
#ifndef STANDALONE

//	if (!CCPrintDialog::OnPreparePrinting(pInfo,pDocView->GetDoc()))
//		ERROR2(FALSE,"Unable to initialise the customized print dialog");

	// default preparation
	BOOL Result = CCamView::DoPreparePrinting(pInfo);
	if (!Result)
		return FALSE;

	// Check because brainless CView only checks for valid DC in debug builds - handy eh? NOT!
	if (pInfo->m_pPD->m_pd.hDC == NULL)
	{
		ERROR1(FALSE,_R(IDE_NULL_PRINTDC));
		//ERROR2(FALSE, "No valid DC returned by print dialog!");
	}

	// Get ourselves a print view and attach it to this CCamView.
	// NB. Must be after the user has clicked OK on print dialog, as we set up
	//     anti-aliasing etc. in PrintView constructor.
	if (pPrintView != NULL)
	{
		delete pPrintView;
		pPrintView = NULL;
		ERROR2(FALSE, "OnPreparePrinting(): we already have a PrintView!");
	}

	pPrintView = new PrintView(pDocView->GetDoc());
	if (pPrintView == NULL)
		ERROR2(FALSE, "Not enough memory to create PrintView object");

	if (!pPrintView->ConnectToOilView(this))
	{
		delete pPrintView;
		pPrintView = NULL;
		ERROR2(FALSE, "Unable to attach PrintView to CCamView.");
	}

#endif

	// OK if we got this far.
	return TRUE;
}

#endif //EXCLUDE_FROM_XARALX

/////////////////////////////////////////////////////////////////////////////
// Menu functions



/*********************************************************************************************
>	void CCamView::OnFilePrintPreview()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Simply makes CView::OnFilePrintPreview() have public scope.
				Used by the menu system.
	Errors:		-
	Scope:		Public
	SeeAlso:	CView::OnFilePrintPreview(); CCamView::OnFilePrint()

**********************************************************************************************/ 

void CCamView::OnFilePrintPreview()
{
	SetCurrentStates();

//	CView::OnFilePrintPreview();
}


/*********************************************************************************************
>	void CCamView::InvalidateView(const OilRect* pRect = NULL, BOOL updatenow = FALSE)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		A pointer to the client area rectangle, an OilRect, which is to be
				invalidated (marked as needing to be redrawn).  This parameter may be
				omitted.  If the first parameter is present, an optional second parameter
				may be specified, which if TRUE tells Windows to immediately send a
				WM_PAINT message to the view, which will force an instant redraw of any
				invalid areas (normally Windows waits until all other messages have been
				processed before sending the WM_PAINT message).
	Outputs:	-
	Returns:	-
	Purpose:	Invalidates the appropriate section of the RendWnd client area - if the
				passed parameter is NULL then the whole client area is invalidated.  The
				rectangle will be redrawn at some time in the (near) future.
	Errors:	    -
	Scope:		Public
	SeeAlso:	class CRendWnd; CCamView::OnDraw()

**********************************************************************************************/ 

void CCamView::InvalidateView(const OilRect* pRect, BOOL updatenow)
{
	if (pRect != NULL)
	{
		// Convert passed rectangle to an MFC-derived type and invalidate it.  Add an extra
		// pixel to the right and bottom edges of the rectangle to ensure that the whole
		// rectangle, after scaling and rounding, is invalidated.
		WinRect	r = pRect->ToWin( pDocView );
//		r.width++;
//		r.height++;		
       	GetRenderWindow()->Refresh( true, &r );
    }
	else
	{
		// We are completely redrawing the client area, so it is possible that the
		// "cached" previous scroll offsets, contained in OldOffset, will become
		// incorrect.
		GetRenderWindow()->Refresh( true );
		GetScrollOffset(&OldOffset);
	}
	
	if (updatenow) GetRenderWindow()->Update();
}


/*********************************************************************************************
>	BOOL CCamView::StartDrag(Operation* pOp, DragType type, BOOL KeepAccuracy)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		A pointer to the current (drag) Operation; the type of the drag.
	Outputs:	-
	Returns:	Returns TRUE if successful.  Returns FALSE if a timer couldn't be allocated.
	Purpose:	Begins a drag operation, from the point of view of the OIL layer.  Unbounds
				the mouse cursor, so it can go off-screen (display driver permitting).
				Captures all mouse input, to guarantee that the drag being started will
				terminate properly at some time in the future.  Sets some internal variables
				indicating that ScreenView is in "drag mode".  Finally, asks the system for a
				timer which is used to generate "idle" mouse events.
	Errors:	    ASSERTion failure if the ScreenView is already dragging.
	Scope:		Public
	SeeAlso:	CCamView::EndDrag(); CCamView::HandleDragEvent();
                          
**********************************************************************************************/ 

BOOL CCamView::StartDrag(Operation* pOp, DragType type, BOOL KeepAccuracy)
{
	ENSURE(	pCurrentDragOp == 0, "CCamView::StartDrag: pCurrentDragOp is not zero");
	
	// This has been commented out as some screen drivers can not handle the mouse drifting
	// off the top of the screen.
	// Unbound mouse from the screen and set the current drag operation.  Note that under
	// Win32 raw mouse coordinates change from being 16-bit INTs to 16-bit SHORTs.
	//WinRect unbound(SHRT_MIN / 4, SHRT_MIN / 4, SHRT_MAX / 4, SHRT_MAX / 4);
	//::GetClipCursor(&MouseClipRect);
	//::ClipCursor(&unbound);
	GetRenderWindow()->CaptureMouse();

	// Remember what kind of drag we are performing.
	pCurrentDragOp = pOp;
	CurrentDragType = type;
	OLERecaptureMouse = FALSE;
	DragKeepAccuracy = KeepAccuracy;
	AutoScrollExcludeRulers = FALSE;
	m_OLELastOutTime.Sample();
	return TRUE;
}



/*********************************************************************************************
>	BOOL CCamView::EndDrag(Operation* pOp)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		A pointer to the current drag operation (currently not used).
	Outputs:	-
	Returns:    TRUE if the drag is successfully terminated.
	Purpose:	Finishes a drag operation in the OIL layer.  Releases mouse capture and
				restores the previous mouse cursor clipping rectangle (generally the screen).
				Sets internal variables to indicate that ScreenView is not in "drag mode".
				Kills the timer set in StartDrag().
	Errors:		ASSERTion failure if ScreenView was not already in "drag mode".	
	Scope:		Public
	SeeAlso:	CCamView::StartDrag(); CCamView::HandleDragEvent()

**********************************************************************************************/ 

BOOL CCamView::EndDrag(Operation*)
{
/*
#ifdef	_DEBUG
	// Prevent a very nasty recursive ENSURE!
	if (!CCamApp::IsDisabled())
	{
		ENSURE(pCurrentDragOp != 0, "CCamView::EndDrag: pCurrentDragOp is zero");
	}
#endif	// _DEBUG
*/
	// Restore the previous mouse clipping rectangle.  Windows will move the
	// mouse, if necessary, so that the mouse is within the new clipping
	// rectangle (or so the documentation says!)
	//::ClipCursor(&MouseClipRect);
	pCurrentDragOp = 0;
	FirstClickButton = 0;
	DragKeepAccuracy = TRUE;
	if (GetRenderWindow()->GetCapture() == RenderWindow)
		GetRenderWindow()->ReleaseMouse();

	// Check status of temporary tool selection - this is because we don't change tool
	// mid-drag! (e.g. user holds down Alt to get selector, starts drag, releases Alt,
	// then ends drag - we want to restore the original tool).
	Tool::EndDrag();

	// Kill the timer started in HandleDragEvent().
	m_DragIdleTimer.Stop();
	
	return TRUE;
}

/********************************************************************************************
>	BOOL CCamView::GetCurrentMousePos(OilCoord* pMousePos) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		-
	Outputs:	pMousePos		the mouse position within this view, in OIL coordinates
	Returns:	TRUE if the mouse is within this view, and hence that *pMousePos holds
				its position; FALSE if the mouse is outside the view (in which case
				*pMousePos is unchanged).
	Purpose:	Finds out the current mouse position within this view window.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL CCamView::GetCurrentMousePos(OilCoord* pMousePos) const
{
	// Get the Windows screen coordinates of the mouse cursor.
	// Convert from screen to window coordinates.
	WinCoord pt;
	*(wxPoint *)&pt = GetRenderWindow()->ScreenToClient( ::wxGetMousePosition() );

	// If the mouse is outside the view then we can do nothing more.
	if (!CurrentSize.Inside(pt)) return FALSE;

	// Convert to OIL coordinates.
	if (pMousePos!=NULL)
		*pMousePos = pt.ToOil(pDocView, TRUE);

	// We stored something useful.
	return TRUE;
}


/*********************************************************************************************
>	void CCamView::SetWorkAreaExtent(const WorkRect& area, BOOL redraw)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    A WorkRect describing the new WorkAreaExtent, a boolean which if TRUE
				means immediately repaint the view window to reflect the change.
	Outputs:	-
	Returns:	-
	Purpose:	Calculates the size of the RendWnd, setting the "page size" of the
				scroll bars accordingly.  Sets the scroll range of the scroll bars
				to the size of the WorkAreaExtent (note that the Y range is negated
				and inverted - the mother of all bodges).
	Errors:		-
	Scope:		Public
	SeeAlso:	CCamView::GetWorkAreaExtent(); CScroller::SetPageSize();
				CScroller::SetScrollRange(); class CRendWnd

**********************************************************************************************/ 

void CCamView::SetWorkAreaExtent(const WorkRect& area, BOOL redraw)
{
	WorkArea = area;
	FIXED16 PixelWidth, PixelHeight;
	pDocView->GetPixelSize(&PixelWidth,&PixelHeight);
	INT32 HScrRange = (INT32)(WorkArea.Width()/PixelWidth.MakeDouble());
	INT32 VScrRange = (INT32)(WorkArea.Height()/PixelHeight.MakeDouble());
	if (HScrollBar)
		HScrollBar->SetScrollbar(HScrollBar->GetThumbPosition(), CurrentSize.GetWidth(), HScrRange, CurrentSize.GetWidth(), true);
	if (VScrollBar)
		VScrollBar->SetScrollbar(VScrollBar->GetThumbPosition(), CurrentSize.GetHeight(), VScrRange, CurrentSize.GetHeight(), true);

	// Should really do something about the redraw param here
	// E.g. call the function that causes an immediate repaint of invalid areas
	// However, until I see some obvious sign that this is ever necessary I'll 
	// leave it commented out
//	if (redraw)
//	{
//		if (RenderWindow)
//			RenderWindow->Update();
//	}
}

/*********************************************************************************************
>	void CCamView::GetScrollOffset(WorkCoord* pOffset) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	The current scroll offset is written to the passed WorkCoord.
	Returns:	-
	Purpose:	Reads the offset from the horizontal and vertical scroll bars,
				combining then into a WorkCoord.
	Errors:		-
	Scope:		Public
	SeeAlso:	CCamView::SetScrollOffset(); CScroller::GetScrollPos()

**********************************************************************************************/ 

void CCamView::GetScrollOffset(WorkCoord* pOffset) const
{	
//	TRACEUSER("Gerry", _T("CV::GetScrollOffset\n"));

	FIXED16 PixelWidth,PixelHeight;
	pDocView->GetPixelSize(&PixelWidth,&PixelHeight);

	INT32 HPos = HScrollBar->GetThumbPosition();
	INT32 VPos = VScrollBar->GetThumbPosition();

//	TRACEUSER("Gerry", _T("CanvasPos = (%d, %d)\n"), HPos, VPos);

	pOffset->x = XLONG(WorkArea.lo.x + HPos*PixelWidth.MakeDouble());
	pOffset->y = XLONG(WorkArea.hi.y - VPos*PixelHeight.MakeDouble());

//	TRACEUSER("Gerry", _T("ScrollOffset = (%d, %d)\n"), (INT32)(pOffset->x), (INT32)(pOffset->y));
}



/*********************************************************************************************
>	void CCamView::SetScrollOffset(const WorkCoord& pos, BOOL redraw)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    The desired scroll offset, in WorkCoords, and a boolean indicating if
				TRUE that the view window should be repainted immediately to reflect
				the change in scroll position.
	Outputs:	-
	Returns:	-
	Purpose:	Changes the scroll offset in the horizontal and vertical scroll bar.  Note
				that it DOES NOT immediately scroll the client area, which is only done
				when the scroll bars send back a WM_SCROLLEREVENT message to the ScreenView.
	Errors:	    -
	Scope:		Public
	SeeAlso:	CCamView::GetScrollOffset; CCamView::OnScrollerEvent()

**********************************************************************************************/ 

void CCamView::SetScrollOffset(const WorkCoord& pos, BOOL redraw)
{
//	TRACEUSER("Gerry", _T("CV::SetScrollOffset(%d, %d)\n"), (INT32)(pos.x), (INT32)(pos.y));
	FIXED16 PixelWidth,PixelHeight;
	pDocView->GetPixelSize(&PixelWidth,&PixelHeight);

	// Validate suggested ScrollOffset...
	WorkCoord tpos = pos;
	WorkRect wrScrollRect = GetMaxScrollRect();
	if (tpos.x < wrScrollRect.lo.x)	tpos.x = wrScrollRect.lo.x;
	if (tpos.y < wrScrollRect.lo.y)	tpos.y = wrScrollRect.lo.y;
	if (tpos.x > wrScrollRect.hi.x)	tpos.x = wrScrollRect.hi.x;
	if (tpos.y > wrScrollRect.hi.y)	tpos.y = wrScrollRect.hi.y;

	INT32 sx = INT32((tpos.x-WorkArea.lo.x)/PixelWidth .MakeDouble());
	INT32 sy = INT32((WorkArea.hi.y-tpos.y)/PixelHeight.MakeDouble());
//	TRACEUSER("Gerry", _T("Scrolling to (%d, %d)\n"), sx, sy);

	HScrollBar->SetThumbPosition(sx);
	VScrollBar->SetThumbPosition(sy);

	SetCurrentStates();
	if (Status && fSetupDone)
	{
		WorkCoord temp;
		GetScrollOffset(&temp);
		Status->SetScrollPos(temp);
		if (redraw)
			ScrollTo(temp);
	}

	// Neville 22/8/97
	// Somebody is setting the scroll position so make sure that this is set
	// Other calls to OnScrollerEvent can be filtered out completely so that a document
	// can load in completely but not had one single call to ScrollTo code allowed. Everything
	// looks ok but if you scroll slightly then becuase the OldOffset is wrong, usually 0,0,
	// a large redraw problem can occur OR the whole drawing will be redrawn needlessly.
	// This is called by the document loading code and so when it says it wants this scroll position
	// it is generally late enough in the loading process that the 'Fixes bug #1448.' comment
	// should have been sorted.
	fSetupDone = TRUE;
}


/*********************************************************************************************
>	virtual void CCamView::OnInitialUpdate()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19 Jan 1994
	Inputs:	    -
	Outputs:	-
	Returns:	-
	Purpose:	Called when the view must first display the document.  Moves this
				view/frame to the position given in the ViewState object.
	Errors:	    -
	Scope:		Protected
	SeeAlso:	CCamView::SetThisFromViewState; CCamView::SetViewStateFromThis;
				DocView::ViewStateChanged
**********************************************************************************************/ 

void CCamView::OnUpdate(wxView* sender, wxObject* hint)
{
	TRACEUSER("Gerry", _T("CCamView::OnUpdate\n"));
	if ( sender==NULL && !fInitialUpdate )
	{
		OnInitialUpdate();
		fInitialUpdate = TRUE;
	}
	wxView::OnUpdate(sender, hint);
}


//
// OnIntialUpdate is no longer called automatically, so we call it instead
// just once from OnUpdate.
//
void CCamView::OnInitialUpdate()
{
	TRACEUSER("Gerry", _T("CCamView::OnInitialUpdate\n"));
	// Set up current etc.
	SetCurrentStates();

	// Check if this view is being created by LoadAppWindowState at the start.  If it is
	// then pick up the ViewState object ready-made by LoadAppWindowState and set
	// this view according to that.
	if (pReadyMadeViewState != NULL)
	{
		TRACEUSER("Gerry", _T("Got pReadyMadeViewState\n"));

		// Set the flags saved in temp. storage in the ViewState in the corresponding
		// DocView.
		DocView* pdv = GetDocViewPtr();
		ERROR3IF(pdv == NULL, "Null DocView* in CCamView::OnInitialUpdate");

		// Deallocate the old ViewState and set the new.
		delete SetViewState(pReadyMadeViewState);

		// Set these flags as well in the DocView.
		pdv->ViewFlags.GridShow 	   = pReadyMadeViewState->_GridShow;
		pdv->ViewFlags.GridSnap 	   = pReadyMadeViewState->_GridSnap;
		pdv->ViewFlags.ObjectsSnap 	   = pReadyMadeViewState->_ObjectsSnap;
		pdv->ViewFlags.MagObjectsSnap  = pReadyMadeViewState->_MagObjectsSnap;
		pdv->ViewFlags.PrintBorderShow = pReadyMadeViewState->_PrintBorderShow;
		pdv->ViewFlags.LogicalView 	   = pReadyMadeViewState->_LogicalView;
		pdv->ViewFlags.GuidesSnap 	   = pReadyMadeViewState->_GuidesSnap;
		pdv->ViewFlags.GuidesShow 	   = pReadyMadeViewState->_GuidesShow;
		
		// Make sure we set this view from the view-state, not the other way around.
		pReadyMadeViewState = NULL;
		Status->IsNewView = FALSE;
	}

	// Force the DocView to set the work area extent.
	pDocView->ViewStateChanged();

	if (HScrollBar)
	{
		// Calculate the scroller page & line sizes, and its range, and make sure the
		// work area extent is updated.
		INT32 Width = CurrentSize.GetWidth();
		HScrollBar->SetScrollbar(HScrollBar->GetThumbPosition(), Width, HScrollBar->GetRange(), Width);
	}

	if (VScrollBar)
	{
		INT32 Height = CurrentSize.GetHeight();
		VScrollBar->SetScrollbar(VScrollBar->GetThumbPosition(), Height, VScrollBar->GetRange(), Height);
	}

	// Either make this view conform to the ViewState, or update the ViewState with
	// information about this view, depending on whether the view is new or (re)loaded.
	if (Status->IsNewView)
	{
		SetViewStateFromThis();
		pDocView->OnNewView();			// give the DocView a shot
	}
	else
	{
		SetThisFromViewState();
		pDocView->OnLoadedView();		// let kernel have its say
	}

PORTNOTE("other","CCamView::OnInitialUpdate - code removed")
#ifndef EXCLUDE_FROM_XARALX
	// RALPH
	if (GetParentFrame())
	{
	#if (_OLE_VER >= 0x200)
		// In OLE builds only do this for non-embedded docs, as OnShowDocument will handle it
		// for embedded docs.
		if (!GetDocument()->IsServer())
	#endif
		{
			// Apply preferences.  To begin, maximise this view if it is the only one around.
			BOOL fAppIsIcon = GetMainFrame()->IsIconic();
			if (HowMany() == 1)
			{
				// This flag is TRUE during workspace restoration of more than one doc.  We don't
				// want to maximise any views if its TRUE.
				extern BOOL fDontMaximiseViews;
				if (ZoomSingleView && !fAppIsIcon && !fDontMaximiseViews)
					((CMDIChildWnd*) GetParentFrame())->MDIMaximize();
			}
			else
			{
				// Otherwise, if a view is already maximised, un-maximise it so that this
				// view is visible.
				if (UnzoomOnNewView || fAppIsIcon)
				{
					// Find out the active view, which must be the one maximised, if any are.
					BOOL fIsMaxed;
					CMDIFrameWnd* pMainWnd = ((CMDIChildWnd*) GetParentFrame())->GetMDIFrame();
					ENSURE(pMainWnd, "No MDI frame window in CCamView::OnShowWindow!");
					CMDIChildWnd* pMaxiWnd = pMainWnd->MDIGetActive(&fIsMaxed);

					// If it is maximised then restore it.
					if (fIsMaxed && pMaxiWnd != NULL)
					{
						wxView* pv = pMaxiWnd->GetActiveView();
						if (pv && pv->GetDocument() &&
							((CCamDoc*) pv->GetDocument())->IsUntouched())
						{
							TRACEUSER( "JustinF", _T("MDI Restore in CCamView::OnInitialUpdate\n"));
							pMaxiWnd->MDIRestore();
						}
					}
				}
			}
		}
	}
#ifdef _DEBUG
	else
	{
		TRACEUSER( "JustinF", _T("\t- no parent frame in CCamView::OnInitialUpdate\n"));
	}
#endif
#endif

	// Make sure the render window gets repainted and inform the kernel that something
	// has happened.
	GetRenderWindow()->Refresh(false);
	pDocView->ViewStateChanged();

	// Indicate that we have set the position, size, focus etc so OnActivateView is safe
	// to update the HasFocus fields of ViewStates.
	fSetupDone = TRUE;

#if (_OLE_VER >= 0x200)

	// Check if the document wants to zoom to the page.  This is the best place to
	// do this as everything should now be stable.
	LPCTSTR lpcszZoomOp = GetDocument()->GetInitialZoom();
	if (lpcszZoomOp)
	{
		// Remember the old prev. zoom setting.
		INT32 nPrev;
		FIXED16 fxPrev;
		WorkCoord wcPrev;
		nPrev  = pDocView->GetPrevZoomIndex();
		fxPrev = pDocView->GetPrevZoomScale();
		wcPrev = pDocView->GetPrevZoomOffset();

		// There is, so zoom it to show the new page bounds.  Set-up the kernel.
		DocView* pOldCurrent = DocView::GetCurrent();
		pDocView->SetCurrent();

		// Perform the Zoom-to-Page Operation (it's non-undo-able).
		OpDescriptor* pOp = OpDescriptor::FindOpDescriptor((LPTSTR) lpcszZoomOp);
		ERROR3IF(!pOp, "Can't find the Zoom Operation in CCamView::OnInitialUpdate");
		pOp->Invoke(0);

		// Restore the old prev. zoom setting.
		pDocView->SetPrevZoomIndex(nPrev);
		pDocView->SetPrevZoomScale(fxPrev);
		pDocView->SetPrevZoomOffset(wcPrev);
	}

#endif 
}



/*********************************************************************************************
>	void CCamView::OnActivateView(BOOL bActivate, wxView* pActiveView, wxView* pDeactiveView)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		A boolean which if TRUE means that this view is being activated, if FALSE
				that this view is being deactivated; two pointers to the CView objects which
				are being activated/deactivated. 
	Outputs:	Possibly changes the pointed-to views.
	Returns:	-
	Purpose:	Called by MFC when a view gains or loses the input focus, either due to the
				the user clicking about, or another application popping up a window etc etc.
				I would just like to point out that this MFC function is the most brain-damaged
				Microsoft have come up so far.  It's documentation bears no resemblance to
				the way the function is called/used - for example, as far as I can see, the
				two CView* parameters are NEVER different!
	Errors:		-
	Scope:		Protected
	SeeAlso:	Pretty unique, this one, so I can't think of anything to put here.

**********************************************************************************************/ 

void CCamView::OnActivateView( bool bActivate, wxView* pActiveView, wxView* pDeactiveView )
{
	TRACEUSER("Gerry", _T("CCamView::OnActivateView(%s) - 0x%08x\n"), bActivate ? _T("true") : _T("false"), this);

	if ( !pDocView )
	{
		TRACE( _T("CCamView::OnActivateView - Warning: pDocView is NULL\n") );
		return;
	}

	if (bActivate) SetCurrentStates();

	Document* KernelDoc = NULL; 
	if (pDocView) KernelDoc = pDocView->GetDoc();

	// MFC does some hidden stuff here . . .
	wxView::OnActivateView( bActivate, pActiveView, pDeactiveView );

	// Trigger off some messages into the kernel.
	if (bActivate)
	{
		// make sure the global preference reflects the current state
		SetDefaultRulersState(Status->RulersVisible);
		SetDefaultScrollersState(Status->ScrollersVisible);
		
		// Update our records of who is top-most and set the keyboard input focus to
		// the rendering child window.
		GetRenderWindow()->SetFocus();

		// Inform the kernel that the view's Z-order has changed. This sets the selected
		// View, Document, and spread, to legal values.
		Document::SetSelectedViewAndSpread(KernelDoc, pDocView, NULL);
	}
	else
	{
		TRACEUSER("Gerry", _T("Deactivating the view\n"));

		// Lets just try setting no selected for the time being but only if this view is the selected one
		if (DocView::GetSelected() == pDocView)
		{
			PORTNOTE("other", "CCamView::OnActivateView(false) now setting no selected doc and view")
			Document::SetNoSelectedViewAndSpread();
		}
		else
		{
			TRACEUSER("Gerry", _T("Not the selected view\n"));
		}
	}
}


/*********************************************************************************************
>	void CCamView::SetViewStateFromThis() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called by a virgin ScreenView, ie. a view onto a new, unedited
				document.  The function reads the attributes of the view window
				from the operating system, converts them into a device-
				independent form, and stores them in the ViewState.
	Errors:	    -
	Scope:		Private
	SeeAlso:	CCamView::OnInitialUpdate; CCamView::SetThisFromViewState();
				DocView::ViewStateChanged(); class ViewState

**********************************************************************************************/ 

void CCamView::SetViewStateFromThis() const
{
	TRACEUSER("Gerry", _T("SetViewStateFromThis\n"));

	// Set the current docview.
	pDocView->SetCurrent();

	// Set most of the ViewState according to the current state (the default) of this
	// ScreenView.
	UpdateViewPosition();

	// Make sure the scale factor is accurate.
	FIXED16 scale = pDocView->GetViewScale();
	if (scale != Status->ViewScale) Status->ViewScale = scale;
	
	// Record the current scroller position.
	WorkCoord temp;
	GetScrollOffset(&temp);
	Status->SetScrollPos(temp);

	if( NULL != GetParentFrame() )
	{
		// Record the top-most attribute of the frame window.
PORTNOTE("other","Removed WM_TOPMOSTENQUIRY usage - is this really needed?")
#ifndef EXCLUDE_FROM_XARALX
		Status->AlwaysOnTop = m_pFrame->SendMessage( WM_TOPMOSTENQUIRY );
#endif
	}
}




/*********************************************************************************************
>	void CCamView::SetThisFromViewState()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    -
	Outputs:    -
	Returns:    -
	Purpose:	Move and resize the ScreenView according to the (device-independent)
				settings of a ViewState object.  This typically happens when a new
				view is opened on a saved document.
	Errors:		-
	Scope:	    Private
	SeeAlso:	CCamView::SetViewStateFromThis(); CCamView::OnInitialUpdate()

**********************************************************************************************/ 

void CCamView::SetThisFromViewState()
{
	TRACEUSER("Gerry", _T("SetThisFromViewState\n"));

	PORTNOTE("other","CCamView::SetThisFromViewState - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Get a pointer to this view's frame window.
	wxMDIChildFrame	   *pframewnd = (wxMDIChildFrame *)GetParentFrame();
	//ENSURE(pframewnd != NULL, "Found a ScreenView without a frame window?!?");

	// Fill in a Windows placement structure with some converted positions from the
	// ViewState object, and set the placement of the parent frame window accordingly.
	WINDOWPLACEMENT wp;
	wp.length = sizeof(wp);
	wp.flags = WPF_SETMINPOSITION;
	wp.ptMaxPosition.x = wp.ptMaxPosition.y = 0;

	// Work out how to display the window, either as an icon, maximised, or just normal.
	// Note that it is possible for a maximised window to be iconised without being
	// restore first.
	if (Status->IsMinimised)
	{
		wp.showCmd = SW_SHOWMINIMIZED;				// display as an icon
		if (Status->IsMaximised)
		{
			wp.flags |= WPF_RESTORETOMAXIMIZED;		// go from icon to maximised
		}
	}
	else if (Status->IsMaximised)
	{
		wp.showCmd = SW_SHOWMAXIMIZED;				// display maximised
	}
	else
	{
		wp.showCmd = SW_SHOWNORMAL;					// display normally
	}

	Coord2POINT(Status->IconPoint, &wp.ptMinPosition);
	Rect2RECT(Status->ViewRect, &wp.rcNormalPosition);

	//RALPH
	if(pframewnd)
		pframewnd->SetWindowPlacement(&wp);
#endif

	// Needs to set the window position and size

	// Set the scroll offset.
	OldOffset = Status->GetScrollPos();
	SetScrollOffset(OldOffset);

    // Set the scale factor.
	pDocView->SetViewScale(Status->ViewScale);

	PORTNOTE("other","CCamView::SetThisFromViewState - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// If the view was "always on top" then fake a user selection of the appropriate
	// menu option.
	// RALPH
	if (Status->AlwaysOnTop && GetParentFrame())
	{
		GetParentFrame()->PostMessage(WM_SYSCOMMAND, _R(IDM_ALWAYSONTOP));
	}
#endif
}


/********************************************************************************************
>	void CCamView::UpdateViewPosition() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Updates the ViewState object shared between this ScreenView and its
				associated DocView with the new size of the view etc.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void CCamView::UpdateViewPosition() const
{
	TRACEUSER("Gerry", _T("CCamView::UpdateViewPosition\n"));

	ENSURE(Status != NULL, "No ViewState object in CCamView::UpdateViewPosition!");

	wxRect rc;
	wxFrame* pFrame = GetParentFrame();
	if(pFrame)
	{
		// Get the rectangle of the parent's MDICLIENT window.  All child window sizes and
		// positions are calculated as relative to this.  This is more accurate than just
		// taking the size of the main frame window as a whole, including menus, title bar etc.
		GetMainFrame()->GetClientSize( &rc.width, &rc.height );
		wxRect2Rect(rc, &Status->ParentRect);
	}

	if(pFrame)
		rc = pFrame->GetRect();

	// Fill in the ViewState with the position of the normal and iconised
	// window.  The maximised position of the window does not need to be
	// stored as it always fills the whole MDI client area of its parent.
	wxRect2Rect(rc, &Status->ViewRect);

	Status->IsMinimised = Status->IsMaximised = FALSE;

	PORTNOTE("other","CCamView::UpdateViewPosition - removed GetWindowPlacement");
#ifndef EXCLUDE_FROM_XARALX
	ENSURE(Status != NULL, "No ViewState object in CCamView::UpdateViewPosition!");

	// Get the size of the screen.  We don't use this for anything, but I'm afraid to
	// take it out in case something breaks!
	// LH 31/08/2005 - Removed, if something breaks I'll revisit!
	wxRect				rc;
//	GetDesktopWindow()->GetWindowRect( &rc );
//	RECT2Rect(rc, &Status->ScreenRect);

	wxFrame			   *pframewnd = GetParentFrame();

	if(pframewnd)
	{
		// Get the rectangle of the parent's MDICLIENT window.  All child window sizes and
		// positions are calculated as relative to this.  This is more accurate than just
		// taking the size of the main frame window as a whole, including menus, title bar etc.
		GetMainFrame()->GetClientSize( &rc.width, &rc.height );
		RECT2Rect(rc, &Status->ParentRect);
	}

	// Get the placement information for this view (MDI child) window.  Note
	// that because it is a child window, the position is taken relative to the
	// client area of its parent.
	WINDOWPLACEMENT wp;
	wp.length = sizeof(wp);
	
	
	//ENSURE(pframewnd != NULL, "Found a ScreenView without a frame window?!?");
	if(pframewnd)
		pframewnd->GetWindowPlacement(&wp);

	// Fill in the ViewState with the position of the normal and iconised
	// window.  The maximised position of the window does not need to be
	// stored as it always fills the whole MDI client area of its parent.
	RECT2Rect(wp.rcNormalPosition, &Status->ViewRect);
	POINT2Coord(wp.ptMinPosition, &Status->IconPoint);

	// Update the mini/maxi flags.  Note that it is possible for IsMinised and IsMaximised
	// to both be TRUE, if a maximised window has been iconised (but NOT if an iconised
	// window has been maximised!).
	if (wp.showCmd == SW_SHOWMINIMIZED)
	{
		Status->IsMinimised = TRUE;
	}
	else if (wp.showCmd == SW_SHOWMAXIMIZED)
	{
		Status->IsMinimised = FALSE;
		Status->IsMaximised = TRUE;
	}
	else // must be SW_SHOWNORMAL, ie. "restored"
	{
		Status->IsMinimised = Status->IsMaximised = FALSE;
	}
#endif
}


/********************************************************************************************
>	void CCamView::ShowScrollers(BOOL ShowOrHide)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Hide or Show Scrollers.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void CCamView::ShowScrollers(BOOL Show)
{
	// Set flags and Show/Hide the Scrollers
	bool bShow = Show ? true : false;
	VScrollBar->Show(bShow);
	HScrollBar->Show(bShow);
	Status->ScrollersVisible = Show;

	// call OnSize directly to force new layout	
	wxSize Size;
	m_pFrame->GetSize(&Size.x, &Size.y);
	TRACEUSER("Gerry", _T("Force an OnSize here (%d, %d)\n"), Size.x, Size.y);

	wxSizeEvent evSize(Size, 0);
	OnSize(evSize);
}



/********************************************************************************************
>	void CCamView::ShowRulers(BOOL ShowOrHide)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Hide or Show Rulers.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void CCamView::ShowRulers(BOOL Show) 
{
// WEBSTER - markn 15/1/97
// No rulers in Webster
#ifndef WEBSTER
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	if(!VRuler||!HRuler)
		return;

	// Set flags and Show/Hide the Scrollers
	if(VRuler)
		VRuler->ShowRuler(Show);
	if(HRuler)
		HRuler->ShowRuler(Show);

	Status->RulersVisible = Show;

	// call OnSize directly to force new layout	
	CRect WRect;
	GetClientRect(&WRect);
	SendMessage(WM_SIZE, SIZE_RESTORED, MAKELPARAM(WRect.Width(), WRect.Height()));
#endif
#endif // WEBSTER
}



/*********************************************************************************************
>	BOOL CCamView::AreRulersVisible()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	ages ago
	Inputs:	    -
	Outputs:    -
	Returns:    Ruler Visible state.
	Purpose:	-
	Errors:		-
	Scope:		Public

**********************************************************************************************/ 

BOOL CCamView::AreRulersVisible()
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
 	if(Status)
 		return Status->RulersVisible;
#endif
 	return FALSE;		
};
		
/*********************************************************************************************
>	BOOL CCamView::AreScrollersVisible()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	ages ago
	Inputs:	    -
	Outputs:    -
	Returns:    Scroller Visible state.
	Purpose:	-
	Errors:		-
	Scope:		Public

**********************************************************************************************/ 

BOOL CCamView::AreScrollersVisible()
{ 
	if(Status)
 		return Status->ScrollersVisible;
 	return FALSE;		
}


/*********************************************************************************************
>	BOOL CCamView::IsMouseOverRuler()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	1/10/95
	Inputs:	    -
	Outputs:    -
	Returns:    enum MouseOverRulerHit can be 	OVER_NO_RULERS,
												OVER_HORIZONTAL,
												OVER_VERTICAL,
												OVER_ORIGIN_GADGET.
	Purpose:	-
	Errors:		-
	Scope:		Public

**********************************************************************************************/ 

MouseOverRulerHit CCamView::IsMouseOverRuler()
{ 
#if !defined(EXCLUDE_FROM_RALPH)
	// check that we have some rulers
	if (!HRuler||!VRuler||!OGadget)
		return OVER_NO_RULERS;

	// Find the window the pointer is over
	wxWindow* pWindow = ::wxChildWindowFromPoint(::wxGetMousePosition(), false, -1);

	// check the HWND against the rulers
	if (pWindow == (wxWindow*)HRuler)
		return OVER_HORIZONTAL;
	else if (pWindow == (wxWindow*)VRuler)
		return OVER_VERTICAL;
	else if (pWindow == (wxWindow*)OGadget)
		return OVER_ORIGIN_GADGET;
#endif

	return OVER_NO_RULERS;		
 };




/*********************************************************************************************
>	afx_msg void CCamView::OnLButtonDown(UINT32 nFlags, wxPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Flags describing which buttons & keys were down when the message was sent;
				the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Called by MFC upon receipt of a left-button-down mouse message.  Finds out
				which other buttons are down (eg. SHIFT) and passes the message on to the kernel.
				Note that the mouse	message will have originally come from the RendWnd object, a
				child window of ScreenView.
	Errors:	    -
	Scope:	    Protected
	SeeAlso:	class CRendWnd; CCamView::HandleDragEvent()

**********************************************************************************************/ 

void CCamView::OnLButtonDown( wxMouseEvent &event )
{
	DontSkipNextMouse();
	// If the event is not from the RenderWindow then skip it
//TRACEUSER("Phil", _T("OnLButtonDown\n"));
	INT32 nID = event.GetId();
	if (nID != WID_RENDERWINDOW)
	{
//		TRACEUSER("Gerry", _T("Ignoring mouse event in window %d\n"), nID);
		event.Skip();
		return;
	}

	wxPoint point = event.GetPosition();
	UINT32 nFlags = ClickModifiers::SynthesizeMouseEventFlags(event);

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
// WEBSTER - markn 25/4/97
// No pen stuff required in Webster
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
	// Setup pressure if pen active
	(Camelot.GetPressurePen())->CheckMouseMessage(WM_LBUTTONDOWN, point);
#endif // VECTOR_STROKING
#endif

	// Support triple clicks
	wxRect ClickBounds(	LastDoubleClickPos.x - LocalEnvironment::GetXMouseDoubleClickMove(),
						LastDoubleClickPos.y - LocalEnvironment::GetYMouseDoubleClickMove(),
						LocalEnvironment::GetXMouseDoubleClickMove() * 2,
						LocalEnvironment::GetYMouseDoubleClickMove() * 2 );
	
	MonotonicTime ThisClickTime( event.GetTimestamp() );
	MonotonicTime ClickGap(LocalEnvironment::GetMouseDoubleClickDelay());

	// Is click within allowed movement rectangle and permitted time delay
	INT32 TimeDelay = (ThisClickTime - TimeOfLastClick);
	if( ClickBounds.Inside( point ) && ( TimeDelay <= (INT32)ClickGap ) )
		ThisSingleIsTriple = TRUE;
	else
		ThisSingleIsTriple = FALSE;

	// Deal with the click
	HandleDragEvent( MK_LBUTTON, nFlags, point, CLICKTYPE_SINGLE );

	TimeOfLastClick.Sample();
}

/*********************************************************************************************
>	afx_msg void CCamView::OnMButtonDown(UINT32 nFlags, wxPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Flags describing which buttons & keys were down when the message was sent;
				the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Called by MFC upon receipt of a middle-button-down mouse message.  Finds out
				which other buttons are down (eg. SHIFT) and passes the message on to the kernel.
				Note that the mouse	message will have originally come from the RendWnd object, a
				child window of ScreenView.
	Errors:	    -
	Scope:	    Protected
	SeeAlso:	class CRendWnd; CCamView::HandleDragEvent()

**********************************************************************************************/ 

void CCamView::OnMButtonDown( wxMouseEvent &event )
{
	DontSkipNextMouse();
	// If the event is not from the RenderWindow then skip it
	INT32 nID = event.GetId();
	if (nID != WID_RENDERWINDOW)
	{
//		TRACEUSER("Gerry", _T("Ignoring mouse event in window %d\n"), nID);
		event.Skip();
		return;
	}

	wxPoint point = event.GetPosition();
	UINT32 nFlags = ClickModifiers::SynthesizeMouseEventFlags(event);

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
// WEBSTER - markn 25/4/97
// No pen stuff required in Webster
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
	(Camelot.GetPressurePen())->CheckMouseMessage(WM_MBUTTONDOWN, point);
#endif // VECTOR_STROKING
#endif
	HandleDragEvent(MK_MBUTTON, nFlags, point, CLICKTYPE_SINGLE);
}

/*********************************************************************************************
>	afx_msg void CCamView::OnRButtonDown(UINT32 nFlags, wxPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    Flags describing which buttons & keys were down when the message was sent;
				the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Same as for CCamView::OnLButtonDown(), but for the right mouse button.
	Errors:		-
	Scope:		Protected
	SeeAlso:	class CRendWnd; CCamView::HandleDragEvent(); CCamView::OnLButtonDown()

**********************************************************************************************/ 

void CCamView::OnRButtonDown( wxMouseEvent &event )
{
	DontSkipNextMouse();
	// If the event is not from the RenderWindow then skip it
	INT32 nID = event.GetId();
	if (nID != WID_RENDERWINDOW)
	{
//		TRACEUSER("Gerry", _T("Ignoring mouse event in window %d\n"), nID);
		event.Skip();
		return;
	}

	wxPoint point = event.GetPosition();
	UINT32 nFlags = ClickModifiers::SynthesizeMouseEventFlags(event);

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
// WEBSTER - markn 25/4/97
// No pen stuff required in Webster
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
	(Camelot.GetPressurePen())->CheckMouseMessage( WM_RBUTTONDOWN, event.GetPosition() );
#endif // VECTOR_STROKING
#endif
	HandleDragEvent(MK_RBUTTON, nFlags, point, CLICKTYPE_SINGLE);
}


/*********************************************************************************************
>	afx_msg void CCamView::OnLButtonUp(UINT32 nFlags, wxPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    Flags describing which buttons & keys were down when the message was sent;
				the coordinates of the mouse cursor
	Outputs:	-
	Returns:	-
	Purpose:	Passes the event on to HandleButtonUp()
	Errors:		-
	Scope:		Protected
	SeeAlso:    class CRendWnd; CCamView::OnLButtonDown(); CCamView::GetClickMods();
				DocView::DragFinished(); CCamView::HandleButtonUp()

**********************************************************************************************/ 

void CCamView::OnLButtonUp(wxMouseEvent &event)
{
	DontSkipNextMouse();
	// If the event is not from the RenderWindow then skip it
//TRACEUSER("Phil", _T("OnLButtonUp\n"));
	INT32 nID = event.GetId();
	if (nID != WID_RENDERWINDOW)
	{
//		TRACEUSER("Gerry", _T("Ignoring mouse event in window %d\n"), nID);
		event.Skip();
		return;
	}

	HandleButtonUp(MK_LBUTTON, event);
}




/*********************************************************************************************
>	afx_msg void CCamView::OnMButtonUp(UINT32 nFlags, wxPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    Flags describing which buttons & keys were down when the message was sent;
				the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Passes the event on to HandleButtonUp().
	Errors:		-
	Scope:		Protected
	SeeAlso:    class CRendWnd; CCamView::OnMButtonDown(); CCamView::GetClickMods();
				DocView::DragFinished(); CCamView::HandleButtonUp()

**********************************************************************************************/ 

void CCamView::OnMButtonUp(wxMouseEvent &event)
{
	DontSkipNextMouse();
	// If the event is not from the RenderWindow then skip it
	INT32 nID = event.GetId();
	if (nID != WID_RENDERWINDOW)
	{
//		TRACEUSER("Gerry", _T("Ignoring mouse event in window %d\n"), nID);
		event.Skip();
		return;
	}

	HandleButtonUp(MK_MBUTTON, event);
}




/*********************************************************************************************
>	afx_msg void CCamView::OnRButtonUp(UINT32 nFlags, wxPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Flags describing which buttons & keys were down when the message was sent;
				the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Same as CCamView::OnLButtonUp(), but for the right mouse button.
	Errors:		-
	Scope:		Protected
	SeeAlso:	class CRendWnd; CCamView::OnRButtonDown(); CCamView::GetClickMods();
				DocView::DragFinished(); CCamView::HandleButtonUp()

**********************************************************************************************/ 

void CCamView::OnRButtonUp( wxMouseEvent &event )
{	
	DontSkipNextMouse();
	// If the event is not from the RenderWindow then skip it
	INT32 nID = event.GetId();
	if (nID != WID_RENDERWINDOW)
	{
//		TRACEUSER("Gerry", _T("Ignoring mouse event in window %d\n"), nID);
		event.Skip();
		return;
	}

	HandleButtonUp(MK_RBUTTON, event);
} 




/*********************************************************************************************
>	afx_msg void CCamView::OnLButtonDblClk(UINT32 nFlags, wxPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    Flags describing which buttons & keys were down when the message was sent;
				the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Passes a left-button double-click to CCamView::HandleDragEvent().
	Errors:		-
	Scope:		Protected
	SeeAlso:	class CRendWnd; CCamView::HandleDragEvent()

**********************************************************************************************/ 

void CCamView::OnLButtonDblClk(wxMouseEvent &event)
{
	DontSkipNextMouse();
	// If the event is not from the RenderWindow then skip it
	INT32 nID = event.GetId();
	if (nID != WID_RENDERWINDOW)
	{
//		TRACEUSER("Gerry", _T("Ignoring mouse event in window %d\n"), nID);
		event.Skip();
		return;
	}

	wxPoint point = event.GetPosition();
	UINT32 nFlags = ClickModifiers::SynthesizeMouseEventFlags(event);

	// Support quad clicks
	wxRect ClickBounds(	LastDoubleClickPos.x-LocalEnvironment::GetXMouseDoubleClickMove(),
						LastDoubleClickPos.y-LocalEnvironment::GetYMouseDoubleClickMove(),
						LastDoubleClickPos.x+LocalEnvironment::GetXMouseDoubleClickMove(),
						LastDoubleClickPos.y+LocalEnvironment::GetYMouseDoubleClickMove());
	MonotonicTime ThisClickTime( event.GetTimestamp() );
	MonotonicTime ClickGap(LocalEnvironment::GetMouseDoubleClickDelay());
	// Is click within allowed movement rectangle and permitted time delay
	INT32 TimeDelay = (ThisClickTime - TimeOfLastClick);
	if (ClickBounds.Inside( point ) && (TimeDelay <= (INT32)ClickGap))
		ThisDoubleIsQuad = TRUE;
	else
		ThisDoubleIsQuad = FALSE;

	HandleDragEvent(MK_LBUTTON, nFlags, point, CLICKTYPE_DOUBLE);

	TimeOfLastClick.Sample();
	LastDoubleClickPos = point;
}




/*********************************************************************************************
>	afx_msg void CCamView::OnMButtonDblClk(UINT32 nFlags, wxPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    Flags describing which buttons & keys were down when the message was sent;
				the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Passes a middle-button double-click to CCamView::HandleDragEvent().
	Errors:		-
	Scope:		Protected
	SeeAlso:	class CRendWnd; CCamView::HandleDragEvent()

**********************************************************************************************/ 

void CCamView::OnMButtonDblClk(wxMouseEvent &event)
{
	DontSkipNextMouse();
	// If the event is not from the RenderWindow then skip it
	INT32 nID = event.GetId();
	if (nID != WID_RENDERWINDOW)
	{
//		TRACEUSER("Gerry", _T("Ignoring mouse event in window %d\n"), nID);
		event.Skip();
		return;
	}

	wxPoint point = event.GetPosition();
	UINT32 nFlags = ClickModifiers::SynthesizeMouseEventFlags(event);

	HandleDragEvent(MK_MBUTTON, nFlags, point, CLICKTYPE_DOUBLE);
}




/*********************************************************************************************
>	afx_msg void CCamView::OnRButtonDblClk(UINT32 nFlags, wxPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Flags describing which buttons & keys were down when the message was sent;
				the coordinates of the mouse cursor
	Outputs:	-
	Returns:	-
	Purpose:	Same as CCamView::OnLButtonDblClk(), bit for the right button.
	Errors:		-
	Scope:		Protected
	SeeAlso:	class CRendWnd; CCamView::OnLButtonDblClk()

**********************************************************************************************/ 

void CCamView::OnRButtonDblClk( wxMouseEvent &event )
{
	DontSkipNextMouse();
	// If the event is not from the RenderWindow then skip it
	INT32 nID = event.GetId();
	if (nID != WID_RENDERWINDOW)
	{
//		TRACEUSER("Gerry", _T("Ignoring mouse event in window %d\n"), nID);
		event.Skip();
		return;
	}

	wxPoint point = event.GetPosition();
	UINT32 nFlags = ClickModifiers::SynthesizeMouseEventFlags(event);

	HandleDragEvent(MK_RBUTTON, nFlags, point, CLICKTYPE_DOUBLE);
}


/*********************************************************************************************
>	afx_msg void CCamView::OnMouseWheel(UINT32 nFlags, short zDelta, wxPoint pt)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/2000
	Purpose:	
	Scope:		Protected

**********************************************************************************************/ 

void CCamView::OnMouseWheel( wxMouseEvent &event )
{
	DontSkipNextMouse();
	HandleMouseWheelEvent( event );
}

/*********************************************************************************************
>	afx_msg void CCamView::OnMouseMove(UINT32 nFlags, wxPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    Flags describing which buttons & keys were down when the message was sent;
				the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Packages up OIL-layer mouse-move messages before passing them on to
				DocView::DragPointerMove().  If ScreenView is not in "drag mode" the event is
				passed to our DocView, which will forward it to the current tool.  Otherwise,
				checks if a CLICKTYPE_DRAG message has been sent to the kernel, sending one if
				it hasn't (this smooths the difference between Windows and RISC OS, the former
				system providing little support for drag operations).
	Errors:		-
	Scope:		Protected
	SeeAlso:	class CRendWnd; CCamView::HandleDragEvent(); DocView::DragPointerMove();
				CCamView::GetClickMods(); CCamView::HandleDragScrolling();
				DocView::OnMouseMove()
**********************************************************************************************/ 

void CCamView::OnMouseMove( wxMouseEvent &event )
{
	// Ignore if system is disabled
	if (CCamApp::IsDisabled())
		return;						     	// If the system is disabled, ignore

//TRACEUSER("Phil", _T("OnMouseMove %d %d %x\n"), event.LeftIsDown(), event.ShiftDown(), pCurrentDragOp);
	wxPoint point = event.GetPosition();
	
	// Only respond to real mouse move messages
	if (point == CurrentMousePos)
		return;

	UINT32 nFlags = ClickModifiers::SynthesizeMouseEventFlags(event);
	
	// Ignore mouse moves if we're not the foreground window.
	if (pDocView != DocView::GetSelected())
	{
		return;
	}

	StatusLine* pStatusLine=NULL;
	// flag that the StatusLine's mouse position field needs updating
#ifndef RALPH

	pStatusLine=GetApplication()->GetpStatusLine();
	if (pStatusLine)
		pStatusLine->FlagMousePosNeedsUpdating();
#endif

	SetCurrentStates();

	wxMouseState WXCMS = ::wxGetMouseState();
	wxPoint CurrentMousePosition(WXCMS.GetX(), WXCMS.GetY());
	// Make a number representing the current button and control key state - we don't use this for anything
	// other than seeing whether it has changed
	INT32 CurrentMouseState =
		(WXCMS.LeftDown()	?1:0) |
		(WXCMS.MiddleDown()	?2:0) |
		(WXCMS.RightDown()	?4:0) |
		(WXCMS.ControlDown()?8:0) |
		(WXCMS.ShiftDown()	?16:0) |
		(WXCMS.AltDown()	?32:0) |
		(WXCMS.MetaDown()	?64:0);
#if 0
	TRACEUSER("amb", _T("CCamView:OnMouseMove XY (%d,%d) was (%d,%d) State %2x was %2x"),
		CurrentMousePosition.x,
		CurrentMousePosition.y,
		m_LastMousePosition.x,
		m_LastMousePosition.y,
		CurrentMouseState,
		m_LastMouseState);
#endif
	INT32 CurrentTimestamp = event.GetTimestamp();

	BOOL SkipThis = FALSE;
	// wxWidgets compliant version of the PeekMessage code
	if (!DragKeepAccuracy && s_MouseMoveEliminationPeriod)
	{
		// What the windows version did was look ahead in the message queue for further
		// mouse moves. We don't have the luxury of that, so we do something slightly
		// different, which is that we look at where the mouse pointer has gone.
		// If it's moved, we know another mouse move event will be generated. So there
		// is the possibility of ignoring this one. We do this if the mouse BUTTON
		// state has not changed, the new mouse position is over the current window,
		// and some external event hasn't happened (normally a keypress)

		// If this isn't a motion event or the button state has changed, don't skip this
		// event, or the next pure move
		if ((event.GetEventType() != wxEVT_MOTION) || (CurrentMouseState != m_LastMouseState))
		{
			// Don't skip the next mouse event and don't skip this one
			m_CouldSkipNextMouse = FALSE;
		}
		else
		{
			m_CouldSkipNextMouse = TRUE; // next mouse move is a candidate for skipping

			// we know the buttons are in the same position as before (and the modifiers)
			// and this is solely a motion event - a candidate to be skipped
			if (m_CouldSkipNextMouse && (CurrentMousePosition != m_LastMousePosition) /*&& (CurrentMousePosition != event.GetPosition())*/)
			{
				// OK, we know the mouse is moving. Insert a point every 20ms regardless
				if ((CurrentTimestamp - m_LastMouseUsedTimestamp) < s_MouseMoveEliminationPeriod)
				{
					SkipThis = TRUE;
				}
			}
		}
	}
	else
	{
		m_CouldSkipNextMouse = FALSE;
	}

	m_LastMouseState = CurrentMouseState;
	m_LastMousePosition = CurrentMousePosition;

	if (SkipThis)
	{
		return;
	}

	m_LastMouseUsedTimestamp = CurrentTimestamp;

PORTNOTE("other","Remove PeekMessage related code")
#ifndef EXCLUDE_FROM_XARALX
	// Only discard mouse move messages if the initiator of the drag has requested this.
	if (!DragKeepAccuracy)
	{
		// Due to problems with auto-repeat on modifier keys, we can lag a long way behind
		// with mouse messages, so we discard all except the most recent mouse move messages.
		// However we stop discarding mouse move messages if we come across any other mouse
		// messages such as button up/down events.
		MSG Msg;
		BOOL FoundMsg = FALSE;

		for(;;)
		{
			// We have to test that the window handles match because of a bug in PeekMessage (we 
			// get messages which aren't for the window we specified, strangely).

			if (::PeekMessage(&Msg, m_hWnd, WM_MOUSEFIRST, WM_MOUSELAST, PM_NOREMOVE) &&
				(m_hWnd == Msg.hwnd) && 
				(Msg.message == WM_MOUSEMOVE))
			{
				// Found a mouse move message on the queue...
				// Remember it then discard it.
				nFlags = (UINT32) Msg.wParam;

				// Have to cast to INT8 so we get correct sign-extension.
				point.x = (INT8) LOWORD(Msg.lParam);
				point.y = (INT8) HIWORD(Msg.lParam);

				// Discard the message
				PeekMessage(&Msg, m_hWnd, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE);
				FoundMsg = TRUE;
			}
			else
			{
				// Either: there are no messages, or it's not for our window, or it's a mouse
				// event which is not a mouse *move* event - whatever it is, stop looking
				// for mouse events.
				break;
			}
		}

		// If we found any other mouse messages, use the data from the most recent one and
		// ignore the others.
		if (FoundMsg)
		{
			ENSURE(m_hWnd == Msg.hwnd, "Bad message in OnMouseMove");
			nFlags = (UINT32) Msg.wParam;
			point.x = (INT8) LOWORD(Msg.lParam);
			point.y = (INT8) HIWORD(Msg.lParam);

		
		}
	}
	
	// Slow mouse reponse due to key auto-repeat msgs?
	//
	// We could be experiencing delayed mouse move messages due to the message
	// queue getting flooded with key auto-repeat msgs.
	//
	// To overcome this, we remove all mouse move messages, keeping only the most 
	// recent msg's coords & flags.
	// 
	// Stripping the message cue in this way means we get less mouse moves to drag handlers, hence less accurate
	// mouse move data.  But, as we only do this if we get an auto-repeated key stroke, accuracy should not be
	// a problem, even for the freehand tool.  Freehand drags have two key modifiers (Alt for straught line and
	// Shift for rubbing out) that only require the latest mouse move event

	MSG Msg;
	// Has there been a key msg during the mouse move?
	if (::PeekMessage(&Msg, m_hWnd, WM_KEYFIRST, WM_KEYLAST, PM_NOREMOVE))
	{
		// Have a look at the key msg (use KeyPressSysMsg to turn the msg into a system-independant format)
		KeyPressSysMsg KeySysMsg(&Msg);

		// Is the key msg an auto-repeated key press (i.e. previously down)?
		if (KeySysMsg.IsValid() && KeySysMsg.PrevDown)
		{
			// Auto repeat key msg found, so get the latest mouse move msg
			// Note: We don't remove the msg from the queue, it may need to be processed elsewhere (e.g. button-ups)
			while (::PeekMessage(&Msg, m_hWnd, WM_MOUSEFIRST, WM_MOUSELAST, PM_NOREMOVE))
			{
				// Is the mouse msg a mouse move msgs?
				if (Msg.message == WM_MOUSEMOVE)
				{
					// Found a mouse move message on the queue so remember it 
					nFlags = (UINT32) Msg.wParam;

					// Have to cast to INT8 so we get correct sign-extension.
					point.x = (INT8) LOWORD(Msg.lParam);
					point.y = (INT8) HIWORD(Msg.lParam);

					// Remove the mouse move msg, and get the next mouse msg in the queue
					::PeekMessage(&Msg, m_hWnd, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE);
				}
				else
				{
					// We have got a mouse msg that's not a mouse move msg, so break out of 'while' loop
					// This could be a button-up msg, in which case we should stop looking for
					// mouse moves as they may be irrelevent to the current drag
					break;	
				}
			}
		}
	}
#endif

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
// WEBSTER - markn 25/4/97
// No pen stuff required in Webster
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING

	// Now check the Mouse Message to see if it has any pressure information
	(Camelot.GetPressurePen())->CheckMouseMessage(WM_MOUSEMOVE, point);
#endif // VECTOR_STROKING
#endif

	// Set the DocView (and Document)
//	pDocView->SetCurrent();


	// Find out which buttons/keys are down.
	GetClickMods(event, &LastClickMods);

	// Are we dragging at the moment?
	if (pCurrentDragOp)
	{
		// Yes, but just check that the drag isn't "paused" while we do an OLE drag outside the window
		if (GetRenderWindow()->GetCapture() == GetRenderWindow())
		{
			// Check if we need to scroll this view.
			HandleDragScrolling(point);

			WinCoord wpoint(point.x, point.y);
			OilCoord CurDragOilPoint = wpoint.ToOil(pDocView, TRUE);
//			TRACEUSER("Gerry", _T("DragMove (%d, %d)\n"), (INT32)CurDragOilPoint.x, (INT32)CurDragOilPoint.y);
			// Send an on-going drag event to the kernel.
			pDocView->DragPointerMove(pCurrentDragOp, CurDragOilPoint, LastClickMods);
		}
	}
	else
	{
		// We are not dragging at the moment, but are we about to start a drag?  If the
		// last mouse event was a single click, the same buttons are down, and the mouse
		// has either moved (3 or more), then a drag is beginning.
		INT32	dx  = point.x - LastClickPoint.x;
		INT32	dy  = point.y - LastClickPoint.y;
		INT32	dz2 = (dx * dx) + (dy * dy);
		if ((nFlags & FirstClickButton) && (dz2 >= (DragLatency * DragLatency)))
		{
			// Inform the kernel that a drag is starting.
			HandleDragEvent(FirstClickButton, nFlags, LastClickPoint, CLICKTYPE_DRAG);
		}
		else
		{
			WinCoord wpoint(point.x, point.y);
			// We are not dragging, just moving the mouse around, so pass the position 
			// of the mouse to the tool, giving it an opportunity to set the cursor etc.
			pDocView->OnMouseMove( wpoint.ToOil(pDocView, TRUE), LastClickMods);		
		}
	}

	// Remember the last position of the mouse passed on to the kernel.  The position
	// passed on any subsequent DragPointerIdle events must be the same as this!
	CurrentMousePos = point;

#if !defined(EXCLUDE_FROM_RALPH)
	// if status line has not already been updated by someone calling CSnap::Snap(), call
	// CSnap::Snap(), snapping to grid only, to update status line (and ruler mouse followers)
	if (pStatusLine && pStatusLine->MousePosNeedsUpdating())
	{
		OilCoord OilPos=((WinCoord*)&point)->ToOil(pDocView, TRUE);
		DocCoord DocPos(0,0);
		Spread*  pSpread=pDocView->OilToSpreadCoord(OilPos,&DocPos);
		if (pSpread)
			pDocView->Snap(pSpread,&DocPos,TRUE,TRUE);
	}
#endif
}

/*********************************************************************************************
>	void CCamView::OnSetCursor(wxSetCursorEvent& event)

	Author:		Luke_Hart (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/02/06
	Inputs:	    The event.
	Outputs:	-
	Returns:	-
	Purpose:	Eat the cursor update requests to stop anyone else playing with them, this is 
				needed on MSW or the cursor reverts to the normal arrow.
	Errors:		-
	Scope:		Protected
	SeeAlso:	

**********************************************************************************************/ 

void CCamView::OnSetCursor( wxSetCursorEvent& event )
{
//	TRACEUSER("Gerry", _T("CCamView::OnSetCursor"));
#if !defined(EXCLUDE_FROM_XARALX)
	// Child windows assume responsibility for setting their own cursor.
	if( event.GetEventObject() == RenderWindow )
	{
#endif
		// Ignore if system is disabled
		if (CCamApp::IsDisabled())
			return;							// If the system is disabled, ignore

		SetCurrentStates();

		Cursor* pCursor = NULL;
		
		if (IsTopmost() ||
			CursorStack::GIsActive(Cursor::Busy) ||
			CursorStack::GIsActive(Cursor::Progress))
		{
			// The view window is active, or the machine is busy on a slow task, so we must show
			// the cursor on the top of the cursor stack.
			pCursor = CursorStack::GetActive();
//			TRACEUSER("Gerry", _T("Setting CursorStack::GetActive (0x%08x)"), pCursor);
		}
		else
		{
			// This view is overlapped and the machine isn't busy, so show a normal arrow cursor.
//			TRACEUSER("Gerry", _T("Setting Cursor::Arrow"));
			pCursor = Cursor::Arrow;
		}
	
		if( NULL != pCursor )
		{
//			TRACEUSER("Gerry", _T("Setting event cursor to 0x%08x"), pCursor->Handle());
			event.SetCursor( *(pCursor->Handle()) );
		}

#if !defined(EXCLUDE_FROM_XARALX)
	}
	else
	if( event.GetEventObject() == Corner )
		Corner->OnSetCursor( event );
#endif
}


/*********************************************************************************************
>	void CCamView::OnScroll(wxSizeEvent &event)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    The wxScrollEvent object
	Outputs:    -
	Returns:	-
	Purpose:	Handles all the scroll events.
	Errors:	    -
	SeeAlso:	


**********************************************************************************************/ 

void CCamView::OnScroll(wxScrollEvent &event)
{
//	TRACEUSER("Gerry", _T("CCamView::OnScroll  Orient = %d  Position = %d\n"), event.GetOrientation(), event.GetPosition());

	wxEventType Type = event.GetEventType();

/*	if (Type == wxEVT_SCROLL_CHANGED)
	{
		TRACEUSER("Gerry", _T("wxEVT_SCROLL_CHANGED\n"));
		if (event.GetOrientation() == wxHORIZONTAL)
		{
			TRACEUSER("Gerry", _T("Horizontal = %d\n"), event.GetPosition());
		}
		else
		{
			TRACEUSER("Gerry", _T("Vertical = %d\n"), event.GetPosition());
		}
	}*/

	if (Type == wxEVT_SCROLL_LINEUP)
	{
//		TRACEUSER("Gerry", _T("wxEVT_SCROLL_LINEUP\n"));
		CWinScroller* pBar = NULL;
		if (event.GetOrientation() == wxHORIZONTAL)
			pBar = HScrollBar;
		else
			pBar = VScrollBar;
		if (pBar)
			pBar->SetThumbPosition(pBar->GetThumbPosition() - 20);
	}
	else if (Type == wxEVT_SCROLL_LINEDOWN)
	{
//		TRACEUSER("Gerry", _T("wxEVT_SCROLL_LINEDOWN\n"));
		CWinScroller* pBar = NULL;
		if (event.GetOrientation() == wxHORIZONTAL)
			pBar = HScrollBar;
		else
			pBar = VScrollBar;
		if (pBar)
			pBar->SetThumbPosition(pBar->GetThumbPosition() + 20);
	}

//	TRACEUSER("Gerry", _T("Pos = (%d, %d)\n"), HPos, VPos);
	SetCurrentStates();
	if (Status && fSetupDone)
	{
		WorkCoord temp;
		GetScrollOffset(&temp);
		Status->SetScrollPos(temp);
		ScrollTo(temp);
	}
}


/*********************************************************************************************
>	afx_msg void CCamView::OnSize(wxSizeEvent &event)

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

void CCamView::OnSize( wxSizeEvent &event )
{
	TRACEUSER("Gerry", _T("CCamView::OnSize(%d, %d)\n"), event.m_size.x, event.m_size.y);

	// This is called early, so if pDocView is null do nothing
	if( NULL == pDocView )
		return;

	SetCurrentStates();

	Document* pCurDoc = Document::GetCurrent();
	DocView* pCurView = DocView::GetCurrent();

	wxSize NewSize( event.GetSize() );
//	TRACEUSER("Gerry", _T("CCamView::OnSize Size = (%d, %d)\n"), NewSize.x, NewSize.y);

	wxSize ClientSize;
	m_pFrame->GetClientSize(&ClientSize.x, &ClientSize.y);
	wxRect RenderRect(ClientSize);

	// Check for irrelevant or potty messages.
	if (NewSize.x <= 0 || NewSize.y <= 0)
	{
		TRACE(_T("Bad size in CCamView::OnSize(%d, %d)\n"), NewSize.x, NewSize.y);
		return;
	}

	if (Status->RulersVisible)
	{
PORTNOTETRACE( "other", "CCamView::OnSize - Removed ruler usage" );
#if !defined(EXCLUDE_FROM_XARALX)
		// Get size of rulers and shove the left and top of the RenderRect in
		// This is all old code and needs fixing
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

	if (Status->ScrollersVisible)
	{
		// Resize and reposition the proportional scrollers.

		wxSize HScrSize;
		HScrollBar->GetBestSize(&HScrSize.x, &HScrSize.y);
//		TRACEUSER("Gerry", _T("HScroll BestSize = (%d, %d)\n"), HScrSize.x, HScrSize.y);
		wxSize VScrSize;
		VScrollBar->GetBestSize(&VScrSize.x, &VScrSize.y);
//		TRACEUSER("Gerry", _T("VScroll BestSize = (%d, %d)\n"), VScrSize.x, VScrSize.y);

		wxRect HScrRect(RenderRect.x, RenderRect.height - HScrSize.y, RenderRect.width - VScrSize.x, HScrSize.y);
//		TRACEUSER("Gerry", _T("HScrRect = (%d, %d) [%d, %d]\n"), HScrRect.x, HScrRect.y, HScrRect.width, HScrRect.height);
		HScrollBar->SetSize(HScrRect);

		wxRect VScrRect(RenderRect.width - VScrSize.x, RenderRect.y, VScrSize.x, RenderRect.height - HScrSize.y);
//		TRACEUSER("Gerry", _T("VScrRect = (%d, %d) [%d, %d]\n"), VScrRect.x, VScrRect.y, VScrRect.width, VScrRect.height);
		VScrollBar->SetSize(VScrRect);

 		// Reposition the corner window at the bottom-right.
		wxRect CornerRect(VScrRect.x, HScrRect.y, VScrRect.width, HScrRect.height);
//		Corner->SetSize(CornerRect);

		// Adjust the RenderRect to account for the scroll bars
		RenderRect.width -= VScrSize.x;
		RenderRect.height -= HScrSize.y;
		ScrollBarSize = max(VScrSize.x, HScrSize.y);
	}

	wxRect OldRect = RenderWindow->GetRect();
//	TRACEUSER("Gerry", _T("OldRect = (%d, %d) [%d, %d]\n"), OldRect.x, OldRect.y, OldRect.width, OldRect.height);

	// Resize/reposition the rendering window.
	CurrentSize = RenderRect;
//	TRACEUSER("Gerry", _T("RenderRect = (%d, %d) [%d, %d]\n"), RenderRect.x, RenderRect.y, RenderRect.width, RenderRect.height);
	RenderWindow->SetSize(RenderRect);

	// Update the rest of the window placement information.
	UpdateViewPosition();

	// Calculate the work area, page & line sizes etc etc.
//	FIXED16 PixelWidth, PixelHeight;
//	pDocView->GetPixelSize(&PixelWidth, &PixelHeight);

//	XLONG x1 = CurrentSize.GetWidth() * PixelWidth;
//	XLONG x2 = CurrentSize.GetHeight() * PixelHeight;
//	HScrollBar->SetPageSize(x1);
//	VScrollBar->SetPageSize(x2);
//	HScrollBar->SetLineSize(x1 / xlong(10) + xlong(1));	
//	VScrollBar->SetLineSize(x2 / xlong(10) + xlong(1));

	SetWorkAreaExtent(Status->WorkAreaExtent, FALSE);

	// Make sure the scroll offsets are valid - if we resize the bottom of the window
	// when at the extreme bottom of the view, then the scroll offsets should be
	// changed - we use the scrollers' own integrity checks to do this automatically.
	// Don't do this until the setup flag is TRUE, so we don't overwrite scroll offsets
	// that have been reloaded.
	if (fSetupDone)
	{
		TRACEUSER("Gerry", _T("fSetupDone so setting scroll offsets\n"));
		WorkCoord CurrScrollPos;
		GetScrollOffset(&CurrScrollPos);
		SetScrollOffset(CurrScrollPos, TRUE);
	}

	// Inform the associated DocView object that something has happened.
	pDocView->ViewStateChanged();
	pCurDoc->SetCurrent();
	pCurView->SetCurrent();
}


/*********************************************************************************************
>	void CCamView::OnDragIdle(wxTimerEvent& event)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    The elapsed timer's numeric identifier (not used).
	Outputs:	-
	Returns:	-
	Purpose:	Sends DragPointerIdle "events" to the kernel, when dragging.  As Windows
				doesn't support such "idle" messages, the OIL drag system sets up a timer
				when a drag starts, and sends idle messages every time the timer elapses,
				until the drag is finished.
	Errors:		-
	Scope:		Protected
	SeeAlso:	CCamView::HandleDragEvent(); DocView::DragPointerMove();
				CCamView::GetClickMods();

**********************************************************************************************/ 

void CCamView::OnDragIdle(wxTimerEvent& event)
{
	DontSkipNextMouse();
	// Ignore if system is disabled
	if (CCamApp::IsDisabled())
		return;						     	// If the system is disabled, ignore

	// This window should never get a timer event that does not occur between calls to
	// StartDrag() and EndDrag().
//	ENSURE(pCurrentDragOp != 0, "CCamView::StartDrag: pCurrentDragOp is null");

	// Find out which keys/buttons are being held down at the moment.  The mouse buttons
	// are more difficult, as they may have been swapped.
	UINT32 nFlags = ClickModifiers::SynthesizeMouseEventFlags();

	SetCurrentStates();

	if (pCurrentDragOp == NULL)
	{
		// We are not dragging at the moment, so check for a time-out condition to 
		// start a drag:
		// If a mouse button is down, and the timer has expired, then start a drag.
		if (LastClickType!=CLICKTYPE_NONE
			&& (nFlags & (MK_LBUTTON|MK_MBUTTON|MK_RBUTTON))
			&& DragTimer.Elapsed(DragDelay)
			&& GetCurrentMousePos(NULL)
			)
		{
			// Inform the kernel that a drag is starting.
			HandleDragEvent(FirstClickButton, nFlags, LastClickPoint, CLICKTYPE_DRAG);
		}
	}
	else
	{
		// If we're doing an OLE drag and drop operation, then our DragOperation is still
		// active, but we have lost the Mouse Capture, so we should not update anything.
		// If the user drags back into our view window, the OLE drag will be shut down
		// and we will be given back our mouse capture so we can continue as if nothing
		// had happened. This just stops us scrolling while the OLE system is in charge!
		if (GetRenderWindow()->GetCapture() == GetRenderWindow())
		{
			WorkCoord OldScrollOffset;
			GetScrollOffset(&OldScrollOffset);

			// Check if we need to scroll the screen . . .
			HandleDragScrolling(CurrentMousePos);

			// We need to know if we have scrolled
			WorkCoord NewScrollOffset;
			GetScrollOffset(&NewScrollOffset);
			BOOL bScrolled = (NewScrollOffset.x != OldScrollOffset.x) || (NewScrollOffset.y != OldScrollOffset.y);

			// The above call may have aborted the current DragOp, so we must check if it is NULL
			if (pCurrentDragOp != NULL)
			{
				// Now "fake" a mouse event for the kernel . . .
				LastClickMods = ClickModifiers::GetClickModifiers(nFlags);
				OilCoord CurDragOilPoint = ((WinCoord*)&CurrentMousePos)->ToOil(pDocView, TRUE);
				if (bScrolled)
				{
//					TRACEUSER("Gerry", _T("DragMove (%d, %d)\n"), (INT32)CurDragOilPoint.x, (INT32)CurDragOilPoint.y);
					pDocView->DragPointerMove(pCurrentDragOp, CurDragOilPoint, LastClickMods);
				}
				else
				{
//					TRACEUSER("Gerry", _T("DragIdle (%d, %d)\n"), (INT32)CurDragOilPoint.x, (INT32)CurDragOilPoint.y);
					pDocView->DragPointerIdle(pCurrentDragOp, CurDragOilPoint, LastClickMods);
				}
			}
		}
		else
		{
			// We don't own the capture - but if the user was OLE dragging and has dragged back into our
			// view window, then the OLE drag will have been cancelled, and we need to recapture the
			// mouse so that we can continue processing the drag as if nothing happened. Our OLE DropSource
			// will set our OLERecaptureMouse flag if it cancels itself and wishes us to continue...
			if (OLERecaptureMouse)
			{
				GetRenderWindow()->CaptureMouse();
				OLERecaptureMouse = FALSE;		// Only capture it back once!
			}
		}
	}
}

/*********************************************************************************************
>	void CCamView::ScrollTo(const WorkCoord& offset)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		The position, in WorkCoords, that the top-left corner of the view is to
				move to.
	Outputs:    -
	Returns:	-
	Purpose:	Scroll the client area of the window to the specified position.  The offset
				must be converted to OS coordinates before being passed to Windows.  If the
				new position of the view does not overlap the old position then the client
				area is completely redrawn.  Otherwise, the Windows USER module is called
				to scroll the window.
	Errors:		-
	Scope:	    Private
	SeeAlso:	CCamView::SetScrollPos()

**********************************************************************************************/ 

void CCamView::ScrollTo(const WorkCoord& offset)
{
//	TRACEUSER("Gerry", _T("CCamView::ScrollTo(%d, %d)\n"), (INT32)offset.x, (INT32)offset.y);
	// Calculate the difference between where we are and where we want to be.  Notice
	// the asymmetry in the calculations, to account for the Windows coordinate origin
	// being in the top-left, not the bottom-left.
	FIXED16 PixelWidth, PixelHeight;
	pDocView->GetPixelSize(&PixelWidth, &PixelHeight);
	INT32 dx = ((offset.x - OldOffset.x) / PixelWidth.MakeLong() );
	INT32 dy = ((OldOffset.y - offset.y) / PixelHeight.MakeLong() );
	if (dx == 0 && dy == 0)
	{
//		TRACEUSER("Gerry", _T("No change\n"));
		return;
	}
	
	// Scroll part of, or redraw the whole of, the client area.
	if (Abs(dx) >= CurrentSize.GetRight() || Abs(dy) >= CurrentSize.GetBottom() )
	{
//		TRACEUSER("Gerry", _T("Full refresh\n"));
		GetRenderWindow()->Refresh(true);
	}
	else
	{
//		TRACEUSER("Gerry", _T("ScrollWindow(%d, %d)\n"), -dx, -dy);
		GetRenderWindow()->ScrollWindow(-dx, -dy);
	}
	
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	if (Status->RulersVisible)
	{
		if(HRuler)
			HRuler->ScrollRuler(-dx);
		if(VRuler)
			VRuler->ScrollRuler(-dy);

	}
#endif

	// Remember this scroll offset, which corresponds to what will be drawn on the screen,
	// and force an immediate repaint of the invalid area.
	OldOffset = offset;
	GetRenderWindow()->Update();
//	TRACEUSER("Gerry", _T("Done update\n"));

#if !defined(EXCLUDE_FROM_RALPH)	// && !defined(EXCLUDE_FROM_XARALX)
	// Make sure that the system will update the state of gadgets after this, some of
	// which may now be "greyed" because what they apply to is no longer visible etc.
	// This is the best place to put this, as all scroll requests, whether they come
	// from the user or are internally generated, go through this function.
	DialogBarOp::SetSystemStateChanged(TRUE);
#endif
}



/********************************************************************************************
>	static void CCamView::wxPoint2Coord(const wxPoint& point, Coord* pcoord)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Converts a wxPoint to a kernel Coord.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void CCamView::wxPoint2Coord(const wxPoint& point, Coord* pcoord)
{
	pcoord->x = point.x;
	pcoord->y = point.y;
}



/********************************************************************************************
>	static void CCamView::Coord2wxPoint(const Coord& coord, wxPoint* ppoint)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Converts a kernel Coord to a wxPoint.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void CCamView::Coord2wxPoint(const Coord& coord, wxPoint* ppoint)
{
	ppoint->x = (INT32) coord.x;
	ppoint->y = (INT32) coord.y;
}



/********************************************************************************************
>	static void CCamView::wxRect2Rect(const wxRect& rect, Rect* pkrect)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Converts a wxRect into a kernel Rect.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void CCamView::wxRect2Rect(const wxRect& rect, Rect* pkrect)
{
	pkrect->lo.x = rect.GetLeft();
	pkrect->lo.y = rect.GetTop();
	pkrect->hi.x = rect.GetLeft() + rect.GetWidth();
	pkrect->hi.y = rect.GetTop() + rect.GetHeight();
}


/********************************************************************************************
>	static void CCamView::Rect2wxRect(const Rect& krect, wxRect* prect)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Converts a kernel Rect into a wxRect.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void CCamView::Rect2wxRect(const Rect& krect, wxRect* prect)
{
	prect->x = (INT32) krect.lo.x;
	prect->y = (INT32) krect.lo.y;
	prect->width = (INT32) krect.Width();
	prect->height = (INT32) krect.Height();
}



/********************************************************************************************
>	void CCamView::HandleDragEvent(UINT32 Button, UINT32 nFlags, wxPoint point, ClickType t) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Button - the actual button being pressed for a click/double click event.
				nFlags - The mouse button flags
				point - the coordinate of the mouse cursor
				t -  the kind of mouse event (button up/down, move, drag start/finish etc.)
	Outputs:	-
	Returns:    -
	Purpose:    Gathers together Windows information about a mouse event and passes it on to
				the kernel, by calling DocView::OnClick()
				HandleDragEvent needs to be called from OnTimer code so rather than 
				synthesizing a wxMouseEvent object this function has been left taking 
				non-wx params
	Errors:		-
	Scope:		Private
	SeeAlso:	CCamView::GetClickMods(); CCamView::OnLButtonDown(); CCamView::OnRButtonDown()
				DocView::OnClick()

********************************************************************************************/ 

/********************************************************************************************

Technical notes (by Tim):

The Button is parameter is necessary, because sometimes when we get the *first* button
down event, nFlags has more than one mouse button bit set.  This only happens when you
rampantly click buttons very quickly but nevertheless we *do* need to know which button
was pressed, otherwise we never release the drag event and we end up with button-up-drag
city.

I assume this is due to some latency in Windows, e.g., it gets a button down event, and
while preparing it goes and reads the mouse button state and meanwhile another button has
been pressed so we get a button down event with nFlags indicating more than one button
is down.

********************************************************************************************/ 

void CCamView::HandleDragEvent(UINT32 Button, UINT32 nFlags, wxPoint point, ClickType t)
{
	if (DocView::GetSelected() != pDocView)
	{
//		TRACEUSER( "JustinF", _T("Ignored drag-event cos not in the selected view\n"));
		// If selected doc is null, let's select it anyway.
		if (DocView::GetSelected() == NULL)
		{
			// We're in a weird state that should never happen but does - a document
			// has the focus, but isn't selected
			Document* pKDoc =NULL;
			if(pDocView)
				pKDoc = pDocView->GetDoc();

			if (pKDoc)
			{
				Document::SetSelectedViewAndSpread(pKDoc, pDocView, NULL);
			}
			
		}
		return;	
	}

	SetCurrentStates();

	// Find out which buttons etc are down.
	LastClickMods = ClickModifiers::GetClickModifiers(nFlags);

	// If it's the first single click, then reset the drag delay timer, and ask for a 
	// Windows timer.
	if (((t == CLICKTYPE_SINGLE) || (t == CLICKTYPE_DOUBLE)) && 
		(FirstClickButton == 0))
	{
		// Remember this event.
		LastClickPoint = point;
		LastClickType = t;
		LastClickButton = Button;
		FirstClickButton = Button;
		DragTimer.Sample();

#if (_OLE_VER >= 0x200)
		// The user's doing something so don't update any container docs
		CCamDoc::ResetContainerUpdateTimer();
#endif

		// Ask the system for a timer.
		// The timer is used to generate DragPointerIdle events, which are not directly
		// supported by Windows.
		UINT32 rate = 100;
PORTNOTE("other","Removed reading of keyboard autorepeat rate")
#ifndef EXCLUDE_FROM_XARALX
		::SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &rate, 0);
#endif

		m_DragIdleTimer.Start(rate);
	}

	if (DragOpInvoke)
	{
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(DragOpToken);

		ERROR3IF_PF(pOpDesc == NULL,("FindOpDescriptor(%s) failed",(TCHAR*)DragOpToken));

		if (pOpDesc != NULL)
			pOpDesc->Invoke(pDragOpParam);

		// Reset the vars ready for the next call to InvokeDragOp()
		DragOpInvoke = FALSE;
		pDragOpParam = NULL;
	}
	else
	{
		// Convert the click position to OIL coordinates before passing to the kernel.
		//	pDocView->SetCurrent();
		WinCoord wpoint(point.x, point.y);
		pDocView->OnClick( ( (WinCoord *)&point )->ToOil(pDocView, TRUE), t, LastClickMods);
	}
}                                                             



/********************************************************************************************
>	BOOL CCamView::HandleMouseWheelEvent(UINT32 nFlags, short zDelta, wxPoint pt)
	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/2000
	Purpose:	Handle MouseWheel for Scroll document and Zoom operations
				Also dispatches a message to the Gallery the mouse is over, if any...
	Inputs:		nFlags - The mouse flags
				zDelta - movement of the mousewheel
				pt - the coordinate of the mouse cursor
********************************************************************************************/ 

BOOL CCamView::HandleMouseWheelEvent( wxMouseEvent &event )
{
	// Reasons Why This Is Done As It Is...
	// ************************************
	//
	// The WM_MOUSEWHEEL message is only ever sent to the window with focus. Galleries
	// never have the focus, even though they show they have (their windows are highlighted)
	// as this is done using rendering onto a blank window (as is their grey-ing out)
	// As a result of this, a Gallery is never sent a WM_MOUSEWHEEL message.
	// We therefore need to trap the message (here is as good a place as any) and check if the
	// user was actually over a gallery...
	// Finding out if they are over a Gallery SHOULD have been quite easy (eg WindowFromPoint())
	// EXCEPT that we don't create the Galleries in any sensible manner, thus they aren't
	// children of the current window - they are children of the Program Manager - sensible...
	// So, when we ask for the appropriate HWND it returns a nonsense value... Anyhow, the
	// following code gets around this and gives us the real HWND of the Gallery underneath the
	// mouse, OR returns NULL if no bar was found.
	// In the case where we get the Gallery HWND, we dispatch a message to scroll up/down according
	// to mousewheel delta. In the case where we have no HWND we check if the mouse lies within the
	// currently selected view and if it does, then scroll appropriately.
	// If anyone can see an easier way, then let me know!!!
	// Matt 1/11/00

	INT32 zDelta = event.GetWheelRotation();
	INT32 zStep = event.GetWheelDelta();

	PORTNOTETRACE("other","CCamView::HandleMouseWheelEvent - removed gallery bits");
#ifndef EXCLUDE_FROM_XARALX
	// We need to know if the mouse pointer is over a Gallery, and if it is what the
	// HWND of that Gallery is, so that I can send it a scroll message...
	HWND OverBar = GetMainFrame()->IsScreenPointInBar(pt);

	if (OverBar)
	{
		if (zDelta < 0)
		{
			BROADCAST_TO_CLASS(DialogMsg(OverBar, DIM_MOUSEWHEEL_DOWN, 256), DialogOp);
		}
		else
		{
			BROADCAST_TO_CLASS(DialogMsg(OverBar, DIM_MOUSEWHEEL_UP, 256), DialogOp);
		}
	}
	else
#endif
	{
		//Preliminary Checks...
		if (DocView::GetSelected() != pDocView)
		{
			// If selected doc is null, let's select it anyway.
			if (DocView::GetSelected() == NULL)
			{
				// We're in a weird state that should never happen but does - a document
				// has the focus, but isn't selected
				Document* pKDoc =NULL;
				if(pDocView)
				{
					pKDoc = pDocView->GetDoc();
				}

				if (pKDoc)
				{
					Document::SetSelectedViewAndSpread(pKDoc, pDocView, NULL);
				}
			
			}
			return FALSE;	
		}

		SetCurrentStates();

		// We now need to work out if the Mouse is currently over part of the document - if it isn't
		// then we should ignore the message and let something else handle it...
		Spread* pSpread = Document::GetSelectedSpread();
		if (pSpread == NULL)	{	return FALSE;	}

		DocCoord MousePos;
		if(!DocView::GetCurrentMousePos(&pSpread, &MousePos))
		{
			return FALSE;
		}


#if (_OLE_VER >= 0x200)
		// The user's doing something so don't update any container docs
		CCamDoc::ResetContainerUpdateTimer();
#endif

		//If the user is holding down CTRL whilst scrolling the wheel, then we should do the standard zoom-in/out
		if (event.ControlDown())
		{
//			TRACEUSER( "Matt", _T("MouseWheel + CTRL = Zoom, in ScreenView\n"));

			// Try to create an instance of the zoom operation.
			OpZoom* pZoomOp = new OpZoom;
			if (pZoomOp == NULL)
			{
				// We ran out of memory, so sorry.
				InformError(_R(IDE_NOMORE_MEMORY));
				return FALSE;
			}

			DocCoord DocMouse = MousePos.ToDoc(pSpread, pDocView);
			WorkCoord WorkMouse;
			WorkMouse = DocMouse.ToWork(pSpread, pDocView);

			// Get the current view's scaling factor as a rounded precentage.
			INT32 nOldScalePercent = ((pDocView->GetViewScale() * 100) + FIXED16_DBL(0.5)).MakeInt();

			// Zoom in or out around the mouse position...
			if (zDelta > 0)
			{
				// Search backwards until we find a higher zoom, or the end of the table
				for (INT32 i = cZoomTableSize - 1; i >= 0; i--)
				{
					// Found a higher zoom?  If so, set it, remember it's position and return.
					if (nOldScalePercent < pZoomOp->GetPresetZoomPercent(i))
					{
						// Do the zoom.  We will (optionally) end the operation.
						pDocView->SetZoomTableIndex(i);
						pZoomOp->MouseWheelZoomAtPoint(WorkMouse, pZoomOp->GetPresetZoomScale(i), FALSE);
						break;
					}
				}
			}
			else
			{
				// Search forwards through the zoom table until we find a lower zoom factor
				for (INT32 i = 0; i < cZoomTableSize; i++)
				{
					// Found a lower zoom?  If so, set it, remember it's position in the zoom table
					if (nOldScalePercent > pZoomOp->GetPresetZoomPercent(i))
					{
						// Do the zoom.  We will (optionally) end the operation.
						pDocView->SetZoomTableIndex(i);
						pZoomOp->MouseWheelZoomAtPoint(WorkMouse, pZoomOp->GetPresetZoomScale(i), FALSE);
						break;
					}
				}
			}

			// Tidy away...
			delete pZoomOp;

			return TRUE;
		}
		else
		{
			// The CTRL key was not pressed, so we should simply (is anything simple around here)
			// scroll the viewport up/down. (OR left/right if SHIFT is held down - Matt 16/11/2000)

			if (event.ShiftDown())
			{
				// Get the current Horizontal Scroll Position, increment it by a zoom-dependent amount
				// and set the Horizontal Scroll Position to be this value, flagging it to redraw...
				ScrollBy(-(zDelta/zStep) * 20, 0);
			}
			else
			{
				// Get the current Vertical Scroll Position, increment it by a zoom-dependent amount
				// and set the Vertical Scroll Position to be this value, flagging it to redraw...
				ScrollBy(0, -(zDelta/zStep) * 20);
			}

			// NOTE : If we wanted to be really flash, we could make the scroll distance affected by the
			// user's settings in the Control Panel - ie, default is to scroll 3 lines on 120 zDelta, but
			// can be adjusted so we could look at that value...
		}
		// tell everyone we've changed the screen
		BROADCAST_TO_ALL(ScreenChangeMsg());
	}

	// If we get to here, then we must have handled the MouseWheel, so return true...
	return TRUE;
}



/*********************************************************************************************
>	void CCamView::HandleButtonUp(UINT32 nFlags, wxPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9th Sept 1993
	Inputs:		Button - which mouse button has been released.
				nFlags - The mouse button flags.
				point - the coordinate of the mouse cursor.
	Outputs:	-
	Returns:    -
	Purpose:    Finishes the current drag, if there is one, by elucidating the state of
				the buttons etc and calling DragDFinished() in the kernel (DocView).  Called
				whenever a mouse button is released.
	Errors:		-
	Scope:		Private
	SeeAlso:	CCamView::GetClickMods(); CCamView::OnLButtonDown(); CCamView::OnRButtonDown()
				DocView::OnClick()

**********************************************************************************************/ 

void CCamView::HandleButtonUp(UINT32 Button, wxMouseEvent &event)
{
	//Graham 18/6/96: Changed the logical conditions of this function
	//to support clicks with one mouse button while another button is
	//dragging
	
	if (DocView::GetSelected() != pDocView)
	{
		ENSURE(pCurrentDragOp == NULL, "Drag operation in a view that isn't 'selected' !!");
//		TRACEUSER( "JustinF", _T("Ignored button-up cos not in the selected view\n"));
		return;
	}

	// Set Current states...
	SetCurrentStates();

	//Graham: Here's the change - in the logical conditions only. The actual
	//functions called are the same.

	WinCoord			point;
	event.GetPosition( &point.x, &point.y );

	//Was the button that went up the first one that went down?
	if (Button==FirstClickButton)
	{
		//Yes it was.
		//Is there a drag currently running?
		if (pCurrentDragOp != NULL)
		{
			// Yes there is.
			// Release the capture and terminate the drag.  Note that we try to release
			// the mouse capture as soon as possible after a drag has finished, in case the
			// DragFinished method below brings up a dialog box or something.
			if (GetRenderWindow() && GetRenderWindow()->GetCapture()==GetRenderWindow())
				GetRenderWindow()->ReleaseMouse();
			pDocView->DragFinished(pCurrentDragOp, ((WinCoord*) &point)->ToOil(pDocView, TRUE), 
							   LastClickMods, TRUE);


			// Now cater for the special case that the user used the PushCentreTool Click Modifier
			// and they didn't move the mouse - this means they either wanted to Centre the
			// view about the cursor, ZoomIn() or ZoomOut() depending on the modifiers....
			if ((LastClickPoint == CurrentMousePos) && LastClickMods.PushCentreTool)
			{
				// Did we want to ZoomIn()?
				if( event.ControlDown() )
				{
					OpZoom* pZoomOp = new OpZoom;
					ERROR3IF(!pZoomOp,"Couldn't create pZoomOp");

					// Find the mid-point of the view, in work coordinates...
					WorkRect wrView = pDocView->GetViewRect();
					WorkCoord wcMidView((wrView.lo.x + wrView.hi.x) / 2, (wrView.lo.y + wrView.hi.y) / 2);

					pZoomOp->ZoomIn(wcMidView);
				}
				// Did we want to ZoomOut()?
				else if( event.ShiftDown() )
				{
					OpZoom* pZoomOp = new OpZoom;
					ERROR3IF(!pZoomOp,"Couldn't create pZoomOp");

					// Find the mid-point of the view, in work coordinates...
					WorkRect wrView = pDocView->GetViewRect();
					WorkCoord wcMidView((wrView.lo.x + wrView.hi.x) / 2, (wrView.lo.y + wrView.hi.y) / 2);

					pZoomOp->ZoomOut(wcMidView);
				}
				else
				{
					// Then we must have wanted to centre the screen about the cursor...

					DocCoord MousePos;
					Spread *pSpread = Document::GetSelectedSpread();
					if(!DocView::GetCurrentMousePos(&pSpread, &MousePos))
					{
						return;
					}


					WorkCoord testwork(MousePos.ToWork(pSpread, pDocView));


					// Find out where the middle of the current view is...
					DocRect ViewRect = (pDocView->GetDocViewRect(pSpread)).ToSpread(pSpread, pDocView);
					DocCoord ViewMidPoint(0,0);
					ViewMidPoint.x = INT32(ViewRect.lo.x + (ViewRect.hi.x - ViewRect.lo.x)/2.0);
					ViewMidPoint.y = INT32(ViewRect.lo.y - (ViewRect.lo.y - ViewRect.hi.y)/2.0);

					// Work out what the offset between the centre of the screen and the MousePos is...
					DocCoord Offset = MousePos - ViewMidPoint;

					WorkCoord ScrollOffset;
					WorkCoord WorkOffset;

					WorkOffset.x = (XLONG) (MakeXLong(Offset.x) * pDocView->GetViewScale());
					WorkOffset.y = (XLONG) (MakeXLong(Offset.y) * pDocView->GetViewScale());

					ScrollOffset = pDocView->GetScrollOffsets();
					ScrollOffset.x = ScrollOffset.x + WorkOffset.x;
					ScrollOffset.y = ScrollOffset.y + WorkOffset.y;

					if (ScrollOffset.x < (XLONG) 0) ScrollOffset.x = (XLONG) 0;
					if (ScrollOffset.y > (XLONG) 0) ScrollOffset.y = (XLONG) 0;

					// Scroll the screen to centre about the cursor!
					pDocView->SetScrollOffsets(ScrollOffset, TRUE);
				}
			}
		}
		else
		{
			// No there isn't.
			// Kill the timer started in HandleDragEvent(), 
			// as we don't want to wait for a drag anymore.
			m_DragIdleTimer.Stop();

			FirstClickButton = 0;

			//Then pass the Up-Click message to CCamView::OnClick
			pDocView->OnClick(((WinCoord*) &point)->ToOil(pDocView, TRUE), CLICKTYPE_UP, LastClickMods);
		}
	}
	else
	{
		//No, the button that went up was not the first one to go down.
		//In that case, we expect the first button that went down is still down and
		//hence there is a drag running.
		//Is there a drag running?
		if (pCurrentDragOp != NULL)
		{
			//Yes, as expected.
			//Pass the Up-Click message to CCamView::OnClick
			pDocView->OnClick(((WinCoord*) &point)->ToOil(pDocView, TRUE), CLICKTYPE_UP, LastClickMods);
		}
		//If not, something odd has happened - perhaps the user has clicked fast
		//with the mouse. This need not worry us but we should not do anything
		//with the up-click message.
		//The only problem that could occur is if we are in a button-up-drag. This
		//is handled below.
		
	}

	//Ensure there is not a button-up drag in progress
	//That is, there is a drag in progress AND no buttons down
//	ERROR3IF((Operation::GetCurrentDragOp()==NULL)&&(nFlags & ~(MK_SHIFT | MK_CONTROL) == 0),
//		"CCamView::HandleButtonUp - drag operation in progress but no buttons down");
}

// The following is Justin's original logical conditions, commented out, in case
// anyone needs them or if the above doesn't work
/*	// Are we in a real mess, Mr User?
	if (Button != FirstClickButton)
	{
//		TRACEUSER( "JustinF", _T("Ignored button-up cos different button went down\n"));
		return;
	}

	

	// Are we currently running a drag operation.
	if (pCurrentDragOp != NULL)
	{
		// Yes, so release the capture and terminate the drag.  Note that we try to release
		// the mouse capture as soon as possible after a drag has finished, in case the
		// DragFinished method below brings up a dialog box or something.
		::ReleaseCapture();
		pDocView->DragFinished(pCurrentDragOp, ((WinCoord*) &point)->ToOil(pDocView, TRUE), 
							   LastClickMods, TRUE);
	}
	else
	{
		// No.  Is this the button that was pressed first?  If so, kill the timer started in
		// HandleDragEvent(), as we don't want to wait for a drag anymore.
		if (Button == FirstClickButton)
		{
			::FixKillTimer(m_hWnd, 1);
			FirstClickButton = 0;
		}
		else
		{
			// NB. the test above should prevent this ENSURE going off . . .
			ENSURE((nFlags & ~(MK_SHIFT | MK_CONTROL)) == 0,
				   "No buttons down - Welcome to Button-Up-Drag City!");
		}

		// Send a button-up event if we weren't running a drag.
		pDocView->OnClick(((WinCoord*) &point)->ToOil(pDocView, TRUE), CLICKTYPE_UP, LastClickMods);

	}*/



/*********************************************************************************************
>	BOOL CCamView::InvokeDragOp(String_256 OpToken,UINT32 Flags,wxPoint point,OpParam* pParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/95
	Inputs:		OpToken = Op token of op to invoke
				pParam	= ptr to a param block that's passed to the op
						  NOTE! Object at *pParam must have a long life time.  I.e. do NOT
						        call this func with ptr to a local OpParam var
				Flags   = flags that specify which modifiers were down during the click
				point   = screen coord of the start of the drag
	Outputs:	-
	Returns:	Returns TRUE if successful.  Returns FALSE if not
	Purpose:	This func provides a mechanism that allows a drag op to be invoked as if it has been started
				as a result of clicking in the view.  This allows drag ops to be invoked that aren't
				associated to a given tool.

				This allows a drag op to be started directly, instead of being a result of a tool handling
				the click.  This allows you to write drag ops in the usual way.

				If pParam is NULL, the op's Do() function will be called.  If pParam != NULL, the op's
				DoWithParam(pParam) func will be called.  Either way, Operation::StartDrag() should be
				called as a result.

				It was introduced to allow the rulers to create guidelines undoably, no matter what
				tool you are in.  See winoil\oilruler.cpp & kernel\guides.cpp & OpGuideLine

	Errors:	    -
	Scope:		-
	SeeAlso:	-
                          
**********************************************************************************************/ 
/* 	Implementation notes:

	It does this:
		Posts itself a WM_LBUTTONDOWN message with the flags and point provided

	and in HandleDragEvent()
		Creates an instance of the op specified by OpToken
		Calls the DragStart() virt func, that should call Operation::StartDrag() at some point
*/

BOOL CCamView::InvokeDragOp(String_256* pOpToken,OpParam* pParam,UINT32 Flags,wxPoint point)
{
	PORTNOTETRACE("other","CCamView::InvokeDragOp - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	DragOpToken  = *pOpToken;
	pDragOpParam = pParam;
	DragOpInvoke = TRUE;

	PostMessage(WM_LBUTTONDOWN,Flags,point.x | (point.y << 16));
#endif
	return TRUE;
}








/********************************************************************************************

>	BOOL CCamView::ChangeDragType(DragType Type)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/4/95
	Inputs:		Type - the type of drag to change to
	Returns:	TRUE
	Purpose:	Allows someone to change the type of a drag (eg have Auto Scrolling on
				in some situations, but not in others). This function is mostly called from
				the Kernel (DocView) version of the same function
	SeeAlso:	DocView::ChangeDragType

********************************************************************************************/

BOOL CCamView::ChangeDragType(DragType Type)
{
	// Change the current drag type
	CurrentDragType = Type;
	m_OLELastOutTime.Sample();
	OLERecaptureMouse = FALSE;

	return TRUE;
}


/*********************************************************************************************
>	void CCamView::HandleDragScrolling(wxPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20th Sept 1993
	Inputs:		The current position of the mouse cursor.
	Outputs:	-
	Returns:    -
	Purpose:	Checks if the view window needs to be scrolled because the cursor has been
				dragged outside the window.  The window will be scrolled if the current
				mode is AUTO_SCROLL (and the Scroll Lock key isn't latched!!).  The scrolling
				itself is done by calling the routine in DocView, not ScreenView, so that the
				kernel has fair warning to remove any blobbies before the blit is done.
				
				Called when ScreenView receives mouse-move messages and timer ("mouse-idle")
				messages.  Also handles DEFERRED_SCROLLING.
	Errors:		-
	Scope:		Private
	SeeAlso:	CCamView::OnMouseMove; CCamView::OnTimer

**********************************************************************************************/ 

void CCamView::HandleDragScrolling(wxPoint point)
{
	// First we check for the type that does nothing and get out quick
	if (CurrentDragType == DRAGTYPE_NOSCROLL)
		return;

	// Get the size of the view.  We may have to deflate the rectangle a little here, if the
	// bottom-right edges of the view coincide with the bottom-right edges of the screen,
	// otherwise it is not possible to auto-scroll to the right or down as the mouse never
	// leaves the view window.

	WinRect wrSize = CurrentSize;
//	TRACEUSER("Gerry", _T("wrSize = (%d, %d) [%d, %d]\n"), wrSize.x, wrSize.y, wrSize.width, wrSize.height);
//	TRACEUSER("Gerry", _T("Point = (%d, %d)\n"), point.x, point.y);
	point = GetRenderWindow()->ClientToScreen( point );
	point = GetFrame()->ScreenToClient( point );

//	TRACEUSER("Gerry", _T("AdjPoint = (%d, %d)\n"), point.x, point.y);
	
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// This is used to allow guidelines to be deleted by dropping on to the rulers - normally
	// we would start to auto scroll at this point which is a little off putting..
	if (AutoScrollExcludeRulers && CurrentDragType != DRAGTYPE_OLESCROLL)
	{
		UINT32 RulerWidth = OILRuler::GetWidth();
		wrSize.left -= RulerWidth;
		wrSize.top -= RulerWidth;	
	}
#endif

	// Now check for deferred scrolling
	if (CurrentDragType == DRAGTYPE_DEFERSCROLL)
	{
		// If the mouse is within the window, and we are performing deferred-scrolling
		// then we can change over to auto-scrolling.
		if (!wrSize.IsEmpty() && wrSize.Inside(point))
		{
			CurrentDragType = DRAGTYPE_AUTOSCROLL;
		 	AutoScrollExcludeRulers = TRUE;
		}
		return;
	}

	// Initialise Deltas...
	INT32 dx = 0;
	INT32 dy = 0;

	WinRect Outer = wrSize;

	// Set auto-scroll region to be one scrollbar width within the current view
	// (as per The Windows Interface Guidelines Chapter 5).
	wrSize.Inflate(-ScrollBarSize,-ScrollBarSize);

	// Has the mouse moved outside there bounds?
	if (!wrSize.IsEmpty() && !wrSize.Inside(point))
	{
		BOOL bCalcDeltas = TRUE;
#if !defined(EXCLUDE_FROM_XARALX)
		if (CurrentDragType == DRAGTYPE_OLESCROLL)
		{
			if (Outer.Inside(point))		// and still inside visible window
			{
				// If we've not been in the OLE scroll region for long enough
				if (!m_OLELastOutTime.Elapsed(100))
				{
					bCalcDeltas = FALSE;
				}
			}
			else
			{
				// We've dragged beyond the OLE scroll area, so we can now convert the drag
				// into an OLE export drag if we so desire.
				HandleOleDragOutOfView(point);
				bCalcDeltas = FALSE;
			}
		}
#endif
		if (bCalcDeltas)
		{
			// The mouse is outside the view, and we have to scroll the view
			// proportionate to how far outside it is.
			CalcScrollDeltas(point,wrSize,&dx,&dy);
			TRACEUSER("Gerry", _T("ScrollDeltas = (%d, %d)\n"), dx, dy);
		}
	}
#if !defined(EXCLUDE_FROM_XARALX)
	else if (CurrentDragType == DRAGTYPE_OLESCROLL)
	{
		// We're not in the OLE scroll region so reset the timer
		m_OLELastOutTime.Sample();
	}
#endif

	//------------------------------------
	// Now do the scroll if necessary...
	// If dx<>0 or dy<>0 and scroll lock is disabled

	// Test if the SCROLL LOCK key has been set.
	if( !wxGetKeyState( CAMKEY(SCROLL) ) && !(dx==0 && dy==0))
	{
		// Set Current states...
		SetCurrentStates();

		// Do the scroll
		ScrollBy(dx, dy);
	}
}


/*********************************************************************************************
>	static void CCamView::GetClickMods(wxMouseEvent &event, ClickModifiers* p)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		The mouse event passed by wx to a mouse message-handler function.
	Outputs:	Changes the ClickModifier to reflect the button flags.
	Returns:    -
	Purpose:	Helps convert an OIL-layer mouse event into a form acceptable to the kernel.
				Sets the fields of the ClickModifier according to which button is down/up,
				whether the SHIFT, CONTROL, or ALT key is held down etc.  Note that as the
				fields of a ClickModifier are only one bit wide, we must be carefule when
				assigning the (integer) results of logical tests to them.
	Errors:	    -
	Scope:	    Public
	SeeAlso:    CCamView::HandleDragEvent()
**********************************************************************************************/ 

void CCamView::GetClickMods(wxMouseEvent &event, ClickModifiers* p)
{
	*p = ClickModifiers::GetClickModifiers(event);
}


/*********************************************************************************************
>	void CCamView::CalcScrollDeltas(wxPoint point, WinRect wrSize, INT32* dx, INT32* dy)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com> (from Justin)
	Created:	22/9/96
	Inputs:		point	The current position of the mouse cursor.
				wrSize	The rectangle controlling scrolling.
	Outputs:	dx		The amount to scroll in the X axis.
				dy		The amount to scroll in the Y axis.
	Returns:    -
	Purpose:	Calculates the scroll deltas due to the current mouse position being
				outside the allowed rectangle.
				Helper function.
	Errors:		-
	Scope:		Protected
	SeeAlso:	CCamView::HandleDragScrolling

**********************************************************************************************/ 

void CCamView::CalcScrollDeltas(wxPoint point, WinRect wrSize, INT32* dx, INT32* dy)
{
	if( point.x < wrSize.GetLeft() )
		*dx = max(point.x - wrSize.GetLeft(), -ScrollBarSize);
	else if( point.x >= wrSize.GetRight() )
		*dx = min(point.x - wrSize.GetRight(), ScrollBarSize);
	else
		*dx = 0;

	if( point.y < wrSize.GetTop() )
		*dy = max(point.y - wrSize.GetTop(), -ScrollBarSize);
	else if( point.y >= wrSize.GetBottom() )
		*dy = min(point.y - wrSize.GetBottom(), ScrollBarSize);
	else
		*dy = 0;
}


/*********************************************************************************************
>	void CCamView::ScrollBy(INT32 dx, INT32 dy)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com> (from Justin)
	Created:	22/9/96
	Inputs:		dx		The amount to scroll in the X axis.
				dy		The amount to scroll in the Y axis.
	Returns:    -
	Purpose:	Scrolls the view by asking the DocView to do it so it can remove 
				and replace blobs.
	Errors:		-
	Scope:		Protected

**********************************************************************************************/ 

void CCamView::ScrollBy(INT32 dx, INT32 dy)
{
	// If we're not scrolling then don't bother
	if (dx == 0 && dy == 0)
		return;

	// Convert these pixel values into a WorkCoord offset, add to the
	// scroll position and update the scroll bars etc.  Note that the
	// y offset is subtracted as the y-axis is reversed compared to
	// Windows . . .
	WorkCoord offset;
	GetScrollOffset(&offset);
//	TRACEUSER("Gerry", _T("OldOffset = (%d, %d)\n"), (INT32)offset.x, (INT32)offset.y);

	FIXED16 PixelWidth, PixelHeight;
	pDocView->GetPixelSize(&PixelWidth, &PixelHeight);
	offset.x += dx * PixelWidth;
	offset.y -= dy * PixelHeight;

	WorkRect wrScrollRect = GetMaxScrollRect();
	if (offset.x < wrScrollRect.lo.x)	offset.x = wrScrollRect.lo.x;
	if (offset.y < wrScrollRect.lo.y)	offset.y = wrScrollRect.lo.y;
	if (offset.x > wrScrollRect.hi.x)	offset.x = wrScrollRect.hi.x;
	if (offset.y > wrScrollRect.hi.y)	offset.y = wrScrollRect.hi.y;
//	TRACEUSER("Gerry", _T("NewOffset = (%d, %d)\n"), (INT32)offset.x, (INT32)offset.y);

	// By calling DocView to do the scroll we give it a chance to remove
	// any blobbies it might have put on the screen.  Note that the scrollers
	// will prevent any overscroll.
	pDocView->SetScrollOffsets(offset, TRUE);
}


/********************************************************************************************
>	WorkRect CCamView::GetMaxScrollRect() const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/May/2006
	Inputs:		-
	Outputs:	-
	Returns:	WorkRect describing legal area for scroll offsets
	Purpose:	Find the legal area in which scroll offsets can exist
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

WorkRect CCamView::GetMaxScrollRect() const
{
	WorkRect wrScrollRect = Status->WorkAreaExtent;

	FIXED16 PixelWidth, PixelHeight;
	pDocView->GetPixelSize(&PixelWidth, &PixelHeight);

	WorkCoord WindowSize;
	WindowSize.x = CurrentSize.GetWidth() * PixelWidth;
	WindowSize.y = CurrentSize.GetHeight() * PixelHeight;

	if (WindowSize.x > wrScrollRect.hi.x)		// If window wider than document
		wrScrollRect.hi.x = wrScrollRect.lo.x;	// no horz scrolling is possible
	else
		wrScrollRect.hi.x -= WindowSize.x;		// Restrict scrollable area to ensure view never sees outside workarea

	if (WindowSize.y < wrScrollRect.lo.y)		// If window wider than document
		wrScrollRect.lo.y = wrScrollRect.hi.y;	// no vert scrolling is possible
	else
		wrScrollRect.lo.y += WindowSize.y;		// Restrict scrollable area to ensure view never sees outside workarea

	return wrScrollRect;
}


/********************************************************************************************
>	BOOL CCamView::IsTopmost() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this view is the active view.
	Purpose:	Tests if this ScreenView is the active view, ie. is on top of all other views.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL CCamView::IsTopmost() const
{
	return( wxGetApp().CCamApp::GetDocumentManager()->GetCurrentView() == this );
}


/********************************************************************************************
>	static DocView* CCamView::GetDocViewFromWindow(wxWindow* pWindow)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/94
	Inputs:		The handle of the render window to find.
	Outputs:	-
	Returns:	A pointer to the DocView associated with a render window, or NULL if
				there isn't one.
	Purpose:	Given a window handle of a ScreenView render window, this function will
				return a pointer to the kernel DocView object that renders into that
				window, or NULL if there isn't one.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

DocView *CCamView::GetDocViewFromWindow(wxWindow *pWindow)
{
	// For all kernel documents in existence . . .
	List* pDocList = &(GetApplication()->Documents);
	for (Document* pKernelDoc = (Document*) pDocList->GetHead();
		 pKernelDoc != NULL;
		 pKernelDoc = (Document*) pDocList->GetNext(pKernelDoc))
	{
	 	if (pKernelDoc->IsARalphDoc())
		{
			// a bit unpleasent - only works because Ralph Docs have only one view..
	  		// get the first (and only) docview
			DocView * pDocView = pKernelDoc->GetFirstDocView();
			// check handles
			if (pDocView)
				if( pDocView->GetRenderWindow() == pWindow )
					return pDocView;
		}
		else
		{
			 // Convert from a kernel document to an OIL document.
			CCamDoc* pCamDoc = pKernelDoc->GetOilDoc();

			wxNode *node = pCamDoc->GetViews().GetFirst();
			while (node)
			{
				// Compare the handle of the view's render window to the given handle,
				// returning the associated DocView if they match.
				CCamView* pCamView = (CCamView*) node->GetData();

				// Not interested unless it is a ScreenView (it could be a print preview view)
//				if (pCamView->IsKindOf(RUNTIME_CLASS(CCamView)))
//				{
					if (pCamView->GetRenderWindow() == pWindow)
					{
						return pCamView->pDocView;
					}
//				}
				node = node->GetNext();
			}
		}
	} 

	// Couldn't find the handle, so return nothing.
	return NULL;
}	


/*********************************************************************************************
>	BOOL CCamView::CreateDragTarget(DragInformation * DragInfo)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/95
	Inputs:	    DragInfo : details about the current drag
	Outputs:    -
	Purpose:	will create drag targets for appropriate drags
	Errors:		-

**********************************************************************************************/ 

BOOL CCamView::CreateDragTarget(DragInformation * DragInfo)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// TEMPORARY - only create drag targets for the selected (frontmost/input-focus)
	// DocView. This stops us allowing colour drags to go into the wrong document
	// if the user ctrl-tabs to a different window during the drag.
	// Ideally, drags will be allowed, but the colour will be copied to the destination
	// document before being applied, but currently we don't have time to implement all that
	if (pDocView != DocView::GetSelected())
		return FALSE;

	if (DragInfo->CanDropOnPage())
	{
		// If the drag can be dropped onto an object or the page, then we will accept it
		ViewDragTarget * NewTarget;
		NewTarget = new ViewDragTarget(RenderWindow,NULL,DragInfo);
		return TRUE;
	}
#endif
	return FALSE;
}


/********************************************************************************************
>	static DocView* CCamView::GetDocViewFromHwnd(CWindowID WindowID)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/94
	Inputs:		The handle of the render window to find.
	Outputs:	-
	Returns:	A pointer to the DocView associated with a render window, or NULL if
				there isn't one.
	Purpose:	Given a window handle of a ScreenView render window, this function will
				return a pointer to the kernel DocView object that renders into that
				window, or NULL if there isn't one.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

DocView *CCamView::GetDocViewFromWindowID( CWindowID WindowID )
{
	// For all kernel documents in existence . . .
	List* pDocList = &(GetApplication()->Documents);
	for (Document* pKernelDoc = (Document*) pDocList->GetHead();
		 pKernelDoc != NULL;
		 pKernelDoc = (Document*) pDocList->GetNext(pKernelDoc))
	{

		DocView * pDocView = pKernelDoc->GetFirstDocView();
		
		while (pDocView)
		{
			if ( pDocView->GetRenderWindow() == WindowID )
				return pDocView;
			pDocView = pKernelDoc->GetNextDocView(pDocView);
		}
	} 

	// Couldn't find the handle, so return nothing.
	return NULL;
}	


/********************************************************************************************

>	void ViewDragTarget::ViewDragTarget() 
	 
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95		  
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
ViewDragTarget::ViewDragTarget(wxWindow* TheWindow, wxRect *ClientArea, DragInformation* pDragInfo)
		: OilDragTarget(TheWindow,ClientArea)
{
	pThisDoc =  CCamView::GetDocViewFromWindow(TheWindow);
	OverOutline = FALSE;
	OverNode = NULL;

	pCurrentDragInfo = pDragInfo;

	DocView::GetCurrentMousePos(&pSpread, &DropPos);
	
	// Find out the Magnetic Line Radius preference from its owner
	LineRad = NodeRenderableBounded::MagneticLineRadius;
	
	// Scale it according to the ScaleFactor.
	if (pThisDoc!=NULL)
	{
		// Get the Scale factor and modify the magnetic radius
		double Scale = pThisDoc->GetViewScale().MakeDouble();
		LineRad = (INT32) ((double)LineRad / Scale);
	}

}



/********************************************************************************************

>	void ViewDragTarget::GetCursorID()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/95
	Purpose:	Base Method to set cursor over this target


********************************************************************************************/

UINT32 ViewDragTarget::GetCursorID()
{
	ERROR2IF(pCurrentDragInfo==NULL,FALSE,"No DragInfo available when getting Cursor");

	return pCurrentDragInfo->GetCursorID(this);
}



/********************************************************************************************

>	virtual BOOL ViewDragTarget::GetStatusLineText(String_256 * TheText)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/95
	Returns:	Whether String is valid
	Purpose:	provide status line text for this target
   
********************************************************************************************/


BOOL ViewDragTarget::GetStatusLineText(String_256 * TheText)
{
	ERROR2IF(TheText==NULL,FALSE,"NULL string in GetStatusLineText()");
	ERROR2IF(pCurrentDragInfo==NULL,FALSE,"No DragInfo available when getting Status Text");

	return pCurrentDragInfo->GetStatusLineText(TheText, this);
}

/********************************************************************************************

>	BOOL ViewDragTarget::GetDropInfo(PageDropInfo* pDropInfo)
 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95		  
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Get info about the current drag state of this target
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ViewDragTarget::GetDropInfo(PageDropInfo* pDropInfo)
{
	ERROR2IF(pDropInfo==NULL,FALSE,"NULL DropInfo pass to GetDropInfo()");

	pDropInfo->pDocView = pThisDoc;
	pDropInfo->pDoc		= pThisDoc->GetDoc();
	pDropInfo->pSpread  = pSpread;
	pDropInfo->DropPos  = DropPos;

	pDropInfo->TargetHit  	= OverTarget;
	pDropInfo->pObjectHit  	= OverNode;

	return TRUE;
}

/********************************************************************************************

>	void ViewDragTarget::ProcessEvent(DragEventType Event,
						DragInformation *pDragInfo,
						wxPoint *pMousePos, KeyPress* pKeyPress) 
	 
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95		  
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Event Handler for View Drag target 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ViewDragTarget::ProcessEvent(DragEventType Event,
						DragInformation *pDragInfo,
						wxPoint *pMousePos, KeyPress* pKeyPress)
{
	switch (Event)
   	{
		case DRAGEVENT_INITIALISE:
			break;
		case DRAGEVENT_DEINITIALISE:
			break;
		case DRAGEVENT_ABORT:
			break;
		case DRAGEVENT_KEYPRESS:
			break;
		case DRAGEVENT_COMPLETED:
			{
				// Do another HitTest check
				OverNode = NULL;
				DocView::GetCurrentMousePos(&pSpread, &DropPos);
				OverTarget = pThisDoc->IsPointerOverNode(&OverNode,LineRad,FALSE,TRUE);
	
				if (!OverNode) OverTarget = NO_TARGET;
	
				// call doc dragfinished to do whatever is needed
				return pThisDoc->DM_DragFinished(pDragInfo,this);
				break;
			}
		case DRAGEVENT_MOUSESTOPPED:
		case DRAGEVENT_MOUSEMOVED:
		case DRAGEVENT_MOUSEIDLE:
			{
				OverNode = NULL;
				DocView::GetCurrentMousePos(&pSpread, &DropPos);
				OverTarget = pThisDoc->IsPointerOverNode(&OverNode,LineRad, TRUE, TRUE);
	
				if (!OverNode) OverTarget = NO_TARGET;
	
				return TRUE;
				break;
			}
		default:
			break;
	}
	return FALSE;
}

