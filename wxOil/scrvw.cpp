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

// The ScreenView class - subclass of CamView used for screen display.

/*
*/

#include "camtypes.h"


//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "scrvw.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "camelot.h"
#include "camdoc.h"
//#include "ccmdikid.h"
#include "vstate.h"
#include "rendwnd.h"
//#include "scroller.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ops.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "oiltool.h"
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "justin.h"
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "ccdc.h"
#include "csrstack.h"
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "camframe.h"
//#include "oilkeys.h"
//#include "monotime.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "pen.h"
//#include "bars.h"
#include "impexpop.h"	// Import/Export ops
//#include "resource.h"
#include "statline.h"
#include "prntview.h"
//#include "prncamvw.h"
#include "page.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
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
#include "localenv.h"
//#include "tool.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "mario.h"	//For _R(IDE_NOMORE_MEMORY) - Matt 26/10/2000

//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "zoomops.h"

//#include "redraw.h"
//#include "custmsg.h"
#include "brushmsg.h" // for ScreenChanged message
//#include <winuser.h>

//#include <mmsystem.h> // Needed by MFC 3
//#include <limits.h>

#if (_OLE_VER >= 0x200)
  // Extra OLE 2.0 stuff in MFC 2.5
  #include "cntritem.h"
  #include "ole2id.h"
#endif

DECLARE_SOURCE("$Revision$");

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
CC_IMPLEMENT_DYNCREATE(PageDropInfo, CCObject)
CC_IMPLEMENT_DYNAMIC(ViewDragTarget, WinoilDragTarget)
#endif

/////////////////////////////////////////////////////////////////////////////
// ScreenCanvas the message map.

BEGIN_EVENT_TABLE( ScreenView::ScreenCanvas, CCamView::CCamCanvas )
	EVT_SIZE(			ScreenView::ScreenCanvas::OnSize )
	EVT_LEFT_DOWN(		ScreenView::ScreenCanvas::OnLButtonDown )
    EVT_LEFT_DCLICK(	ScreenView::ScreenCanvas::OnLButtonDblClk )
    EVT_LEFT_UP(		ScreenView::ScreenCanvas::OnLButtonUp )
    EVT_MIDDLE_DOWN(	ScreenView::ScreenCanvas::OnMButtonDown )
    EVT_MIDDLE_DCLICK(	ScreenView::ScreenCanvas::OnMButtonDblClk )
    EVT_MIDDLE_UP(		ScreenView::ScreenCanvas::OnMButtonUp )
    EVT_RIGHT_DOWN(		ScreenView::ScreenCanvas::OnRButtonDown )
    EVT_RIGHT_DCLICK(	ScreenView::ScreenCanvas::OnRButtonDblClk )
    EVT_RIGHT_UP(		ScreenView::ScreenCanvas::OnRButtonUp )
    EVT_MOTION(			ScreenView::ScreenCanvas::OnMouseMove )
	EVT_MOUSEWHEEL(		ScreenView::ScreenCanvas::OnMouseWheel )
END_EVENT_TABLE()

ReflectIntoView( ScreenView::ScreenCanvas, ScreenView, OnSize,			wxSizeEvent )
ReflectIntoView( ScreenView::ScreenCanvas, ScreenView, OnLButtonDown,	wxMouseEvent )
ReflectIntoView( ScreenView::ScreenCanvas, ScreenView, OnLButtonDblClk,	wxMouseEvent )
ReflectIntoView( ScreenView::ScreenCanvas, ScreenView, OnLButtonUp,		wxMouseEvent )
ReflectIntoView( ScreenView::ScreenCanvas, ScreenView, OnMButtonDown,	wxMouseEvent )
ReflectIntoView( ScreenView::ScreenCanvas, ScreenView, OnMButtonDblClk,	wxMouseEvent )
ReflectIntoView( ScreenView::ScreenCanvas, ScreenView, OnMButtonUp,		wxMouseEvent )
ReflectIntoView( ScreenView::ScreenCanvas, ScreenView, OnRButtonDown,	wxMouseEvent )
ReflectIntoView( ScreenView::ScreenCanvas, ScreenView, OnRButtonDblClk,	wxMouseEvent )
ReflectIntoView( ScreenView::ScreenCanvas, ScreenView, OnRButtonUp,		wxMouseEvent )
ReflectIntoView( ScreenView::ScreenCanvas, ScreenView, OnMouseMove,		wxMouseEvent )
ReflectIntoView( ScreenView::ScreenCanvas, ScreenView, OnMouseWheel,	wxMouseEvent )

/////////////////////////////////////////////////////////////////////////////
// ScreenView serialization & the message map.

IMPLEMENT_DYNAMIC_CLASS( ScreenView, CCamView )

BEGIN_EVENT_TABLE( ScreenView, CCamView )
END_EVENT_TABLE()

PORTNOTE("other","Removed MessageMap - hopefully very temporarily")
#ifndef EXCLUDE_FROM_XARALX
BEGIN_MESSAGE_MAP(ScreenView, CCamView)
	//{{AFX_MSG_MAP(ScreenView)
	ON_WM_DROPFILES()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_CANCELMODE()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_MESSAGE(WM_RENDERVIEW, OnRenderView)
	ON_MESSAGE(WM_SCROLLEREVENT, OnScrollerEvent)
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()

#ifdef WIN32
	ON_MESSAGE(MM_JOY1MOVE, OnJoystickMove)
#endif

	ON_COMMAND(_R(ID_OLE_INSERT_NEW), OnInsertObject)
#if (_OLE_VER >= 0x200)
	ON_WM_SETFOCUS()
	ON_COMMAND(_R(ID_CANCEL_EDIT), OnCancelEdit)
#endif

#ifdef _DEBUG
	ON_MESSAGE(WM_DESCRIBEVIEW, OnDescribeView)
#endif
	//}}AFX_MSG_MAP

//	WEBSTER-ranbirr-27/03/97
#ifndef WEBSTER
	// Standard print commands:-
#ifndef STANDALONE
	ON_COMMAND(_R(ID_FILE_PRINT), OnFilePrint)
#endif
#endif //webster

	ON_COMMAND(_R(ID_FILE_PRINT_PREVIEW), OnFilePrintPreview)

END_MESSAGE_MAP()
#endif

//	WEBSTER-ranbirr-27/03/97
#ifndef WEBSTER

#ifdef STANDALONE
	void ScreenView::OnFilePrint() {}
#endif
#endif //webster

///////////////////////////////////////////////////////////////////////////////////////////
// ScreenView construction, attachment, and destruction.

INT32 ScreenView::PopCount = 0;
ViewState* ScreenView::pReadyMadeViewState = NULL;
//static INT32 TimerCount = 0;


/*********************************************************************************************
>	ScreenView::ScreenView()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for a ScreenView window.  Sets the pointers to the
				child windows to null, so that an incomplete construction of the view
				that is later destroyed won't try to delete invalid pointers.  The
				Windows side of construction is done later, in the OnCreate() message
				handler.

				Updates the ScreenView population counter. Called from somewhere deep
				inside MFC.				
	Errors:		-
	Scope:	    Protected.
	SeeAlso:    ScreenView::OnCreate(); ScreenView::~ScreenView()

**********************************************************************************************/ 

ScreenView::ScreenView()
{
//	TRACEUSER( "Richard", _T("ScreenView::ScreenView()\n"));

	// No child windows or connections yet.
//	RenderWindow = NULL;
	HScrollBar = NULL;
	VScrollBar = NULL;

PORTNOTE("other","Removed CScrollerCorner usage")
#ifndef EXCLUDE_FROM_XARALX
	Corner = NULL;
#endif

// WEBSTER - markn 15/1/97
// No rulers in Webster
#ifndef WEBSTER
	HRuler = NULL;
	VRuler = NULL;
	OGadget= NULL;
#endif // WEBSTER


	// Set/clear these internal flags.
	fSetupDone = FALSE;
	fInitialUpdate = false;

	// Not dragging or scrolling either.
	ScrollerStyle = TRUE;
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

	// Triple and quad click support
	LastDoubleClickPos = wxPoint( 0, 0 );
	ThisSingleIsTriple = FALSE;
	ThisDoubleIsQuad = FALSE;

	// Set this flag until we have been through OnActivateView once, after which we can
	// include this view in the eat-activating-click system.
	fJustCreated = TRUE;
}



/*********************************************************************************************
>	virtual ScreenView::~ScreenView()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    -
	Outputs:	-
	Returns:	-
	Purpose:	Destroys a ScreenView window.  Deletes child windows first.
				Updates the ScreenView population counter.
	Errors:	    -
	Scope:		Public
	SeeAlso:	ScreenView::ScreenView()

**********************************************************************************************/ 

ScreenView::~ScreenView()
{
	TRACEUSER( "Richard", _T("ScreenView::~ScreenView()\n"));
	// Deregister for WM_DROPFILES messages
	// VC2 Beta 1 asserts with a zero handle here so we check
PORTNOTE("other","Removed DragAcceptFiles usage")
#ifndef EXCLUDE_FROM_XARALX
	if (m_hWnd) DragAcceptFiles(TRUE);
#endif

//	delete RenderWindow;	GetCanvas()?
PORTNOTE("other","Removed CWinScroller usage")
#ifndef EXCLUDE_FROM_XARALX
	delete HScrollBar;
	delete VScrollBar;
#endif
PORTNOTE("other","Removed CScrollerCorner usage")
#ifndef EXCLUDE_FROM_XARALX
	delete Corner;
#endif

// WEBSTER - markn 15/1/97
// No rulers in Webster
#ifndef WEBSTER
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	delete HRuler;
	delete VRuler;
	delete OGadget;
#endif
#endif // WEBSTER

	PopCount--;			// one less ScreenView in the world, sigh
}


/*********************************************************************************************
>	virtual BOOL ScreenView::Create(LPCTSTR, LPCTSTR lpszWindowName, DWORD dwStyle,
					  	  		  const RECT& rect, wxWindow* parent, UINT32 id,
					  	  		  CCreateContext* ctxt = 0)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		A long pointer to a constant character string (can be NULL, not used); a
				long pointer to the view window's title; the Windows instance-style flags;
				a rectangle describing the extent of the view; a pointer to the view's
				parent window; a numeric identifier; possibly a pointer to a CCreateContext.
	Outputs:    -
	Returns:    TRUE if the view is successfully created (attached to a Windows object).
	Purpose:    Calls CCamView::Create() to attach a Windows object to the previously
				constructed C++ object.  The view has a standard arrow cursor and
				a custom icon (_R(IDR_CAMTYPE)).  The view is responsible for painting
				its background itself.  The view is initially hidden, until its size and
				position have been properly set.
	Errors:	    -
	Scope:		Public
	SeeAlso:	ScreenView::ScreenView(); ScreenView::OnCreate()

**********************************************************************************************/ 

/*BOOL ScreenView::Create(LPCTSTR, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect,
					  wxWindow *parent, UINT32 id, CCreateContext* ctxt)
{
	TRACEUSER( "Richard", _T("ScreenView::Create\n"));

 	return CCamView::Create(AfxRegisterWndClass(CS_DBLCLKS,
						   					 0,
											 0,
											 AfxGetApp()->LoadIcon(_R(IDR_CAMTYPE))),
						 lpszWindowName,
						 dwStyle | WS_CLIPCHILDREN,		// NB.  *NOT* WS_VISIBLE!!!
						 rect,
						 parent,
						 id,
						 ctxt);
} */

/********************************************************************************************
>	static void ScreenView::SetReadyMadeViewState(ViewState* pvs)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/95
	Inputs:		pvs				pointer to a ViewState object
	Purpose:	Call this to set up a view-state object that ScreenView::OnInitialUpdate
				will use in preference to the one the ScreenView has got already.  This
				is a nasty little "back-door" to changing the creation state of a
				ScreenView, used by the code that restores docs and views when the app
				starts.  The reason we have to use code like this is that the Camelot doc/view
				architecture is NOT MFC.
	SeeAlso:	ScreenView::OnInitialUpdate; LoadAppWindowState
********************************************************************************************/

void ScreenView::SetReadyMadeViewState(ViewState* pvs)
{
	pReadyMadeViewState = pvs;
}

/********************************************************************************************
>	static BOOL ScreenView::ReadViewPrefs()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/11/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the .INI file settings were loaded (or created) successfully.
	Purpose:	Tries to load preferences for ScreenView, creating them with default values
				if they don't exist.
	Errors:		-
	SeeAlso:	Application::DeclareSection; Application::DeclarePref
********************************************************************************************/

BOOL ScreenView::ReadViewPrefs()
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// Declare and load the .INI file settings.
	BOOL ok =  	Camelot.DeclareSection(TEXT("Mouse"), 10) &&
		   		Camelot.DeclarePref(TEXT("Mouse"), TEXT("DragLatency"),
		   					   		&ScreenView::DragLatency, 1, 10) &&
		   		Camelot.DeclarePref(TEXT("Mouse"), TEXT("DragDelay"),
		   					   		&DragDelay, 100, 2000)

			 && Camelot.DeclareSection(TEXT("WindowFurniture"), 10) &&
		   	    Camelot.DeclarePref(TEXT("WindowFurniture"), TEXT("PropScrollers"),
		   					   	   &ScreenView::PropScrollersOn, FALSE, TRUE) &&
		   	    Camelot.DeclarePref(TEXT("WindowFurniture"), TEXT("Scrollers Visibilty State"),
		   					   	   &ScreenView::DefaultScrollersState, FALSE, TRUE)&&
		   	    Camelot.DeclarePref(TEXT("WindowFurniture"), TEXT("Rulers Visibilty State"),
		   					   	   &ScreenView::DefaultRulersState, FALSE, TRUE)

			 && Camelot.DeclareSection(TEXT("Windows"), 10) &&
		   	    Camelot.DeclarePref(TEXT("Windows"), TEXT("UnzoomOnNewView"),
		   					   	   &ScreenView::UnzoomOnNewView, FALSE, TRUE) &&
				Camelot.DeclarePref(TEXT("Windows"), TEXT("ZoomSingleView"),
		   					   	   &ScreenView::ZoomSingleView, FALSE, TRUE);

	return ok;
#else
	return TRUE;
#endif
}



/*********************************************************************************************
>	static INT32 ScreenView::HowMany()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    -
	Outputs:    -
	Returns:    The number of ScreenViews existing at the moment.
	Purpose:	None immediately springs to mind, although one day - who knows?
	Errors:		-
	Scope:		Public
	SeeAlso:	ScreenView::ScreenView(); ScreenView::~ScreenView();

**********************************************************************************************/ 

INT32 ScreenView::HowMany()
{
	return PopCount;
}



/*********************************************************************************************
>	BOOL ScreenView::AreRulersVisible()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	ages ago
	Inputs:	    -
	Outputs:    -
	Returns:    Ruler Visible state.
	Purpose:	-
	Errors:		-
	Scope:		Public

**********************************************************************************************/ 

BOOL ScreenView::AreRulersVisible()
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
 	if(Status)
 		return Status->RulersVisible;
#endif
 	return FALSE;		
};
		
/*********************************************************************************************
>	BOOL ScreenView::AreScrollersVisible()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	ages ago
	Inputs:	    -
	Outputs:    -
	Returns:    Scroller Visible state.
	Purpose:	-
	Errors:		-
	Scope:		Public

**********************************************************************************************/ 

BOOL ScreenView::AreScrollersVisible()
{ 
	if(Status)
 		return Status->ScrollersVisible;
 	return FALSE;		
}


 /*********************************************************************************************
>	BOOL ScreenView::IsMouseOverRuler()

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

MouseOverRulerHit ScreenView::IsMouseOverRuler()
{ 
// WEBSTER - markn 15/1/97
// No rulers in Webster
#ifndef WEBSTER
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// check that we have some rulers
	if(!HRuler||!VRuler||!OGadget)
		return OVER_NO_RULERS;
	// get the mouse position in screen coords
	wxPoint ScrMousePos;
	::GetCursorPos(&ScrMousePos);

	// get the HWND under the mouse
	HWND ThisControl = ::WindowFromPoint(ScrMousePos);
	
	// check the HWND against the rulers
	if(ThisControl == HRuler->GetSafeHwnd())
		return OVER_HORIZONTAL;
	else  	if(ThisControl == VRuler->GetSafeHwnd())
		return OVER_VERTICAL;
	else  	if(ThisControl == OGadget->GetSafeHwnd())
		return OVER_ORIGIN_GADGET;
		
	
#endif
#endif // WEBSTER
	return OVER_NO_RULERS;		
 };




/*********************************************************************************************
>	BOOL ScreenView::CreateDragTarget(DragInformation * DragInfo)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/95
	Inputs:	    DragInfo : details about the current drag
	Outputs:    -
	Purpose:	will create drag targets for appropriate drags
	Errors:		-

**********************************************************************************************/ 

BOOL ScreenView::CreateDragTarget(DragInformation * DragInfo)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
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
		NewTarget = new ViewDragTarget(RenderWindow->m_hWnd,NULL,DragInfo);
		return TRUE;
	}
#endif

	return FALSE;
}



/********************************************************************************************
>	static DocView* ScreenView::GetDocViewFromHwnd(HWND hwnd)

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

DocView *ScreenView::GetDocViewFromHwnd( wxWindow *hwnd )
{
	PORTNOTETRACE("other","ScreenView::GetDocViewFromHwnd - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// For all kernel documents in existence . . .
	List* pDocList = &(GetApplication()->Documents);
	for (Document* pKernelDoc = (Document*) pDocList->GetHead();
		 pKernelDoc != NULL;
		 pKernelDoc = (Document*) pDocList->GetNext(pKernelDoc))
	{
	
	 	if(pKernelDoc->IsARalphDoc())
		{
			// a bit unpleasent - only works because Ralph Docs have only one view..
	  		// get the first (and only) docview
			DocView * pDocView = pKernelDoc->GetFirstDocView();
			// check handles
			if(pDocView)
				if( pDocView->GetRenderWindow() == hwnd )
					return pDocView;
		}
		else
		{

			 // Convert from a kernel document to an OIL document.
			CCamDoc* pCamDoc = pKernelDoc->GetOilDoc();

			// For all views onto the OIL document . . .
			POSITION pos = pCamDoc->GetFirstViewPosition();
			while (pos != NULL)
			{
				// Compare the handle of the view's render window to the given handle,
				// returning the associated DocView if they match.
				ScreenView* pCamView = (ScreenView*) pCamDoc->GetNextView(pos);

				// Not interested unless it is a ScreenView (it could be a print preview view)
				if (pCamView->IsKindOf(RUNTIME_CLASS(ScreenView)))
				{
					if (pCamView->RenderWindow->m_hWnd == hwnd)
					{
						return pCamView->pDocView;
					}
				}
			}
		}
	} 
#endif
	// Couldn't find the handle, so return nothing.
	return NULL;
}	


/********************************************************************************************
>	static DocView* ScreenView::GetDocViewFromWindowID(CWindowID WinID)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/95
	Inputs:		Window ID
	Returns:	A pointer to the DocView associated with the window ID, else NULL
	Also See:	GetDocViewFromHwnd()
********************************************************************************************/

DocView* ScreenView::GetDocViewFromWindowID(CWindowID WinID)
{
	return GetDocViewFromHwnd( (wxWindow *)WinID );
}



///////////////////////////////////////////////////////////////////////////////////////////
// Position & Z-order management.

BOOL ScreenView::ZoomSingleView = TRUE;		// if only a single view, maximise it
BOOL ScreenView::UnzoomOnNewView = FALSE;	// a new view unmaximises a maximised view





/*********************************************************************************************
>	void ScreenView::SetViewStateFromThis() const

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
	SeeAlso:	ScreenView::OnInitialUpdate; ScreenView::SetThisFromViewState();
				DocView::ViewStateChanged(); class ViewState

**********************************************************************************************/ 

void ScreenView::SetViewStateFromThis() const
{
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
>	void ScreenView::SetThisFromViewState()

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
	SeeAlso:	ScreenView::SetViewStateFromThis(); ScreenView::OnInitialUpdate()

**********************************************************************************************/ 

void ScreenView::SetThisFromViewState()
{
	PORTNOTETRACE("other","ScreenView::SetThisFromViewState - do nothing");
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
	
	// Set the scroll offset.
	OldOffset = Status->GetScrollPos();
	SetScrollOffset(OldOffset);

    // Set the scale factor.
	pDocView->SetViewScale(Status->ViewScale);

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
>	void ScreenView::UpdateViewPosition() const

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

void ScreenView::UpdateViewPosition() const
{
	PORTNOTETRACE("other","ScreenView::UpdateViewPosition - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ENSURE(Status != NULL, "No ViewState object in ScreenView::UpdateViewPosition!");

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



/*********************************************************************************************
>	virtual void ScreenView::OnInitialUpdate()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19 Jan 1994
	Inputs:	    -
	Outputs:	-
	Returns:	-
	Purpose:	Called when the view must first display the document.  Moves this
				view/frame to the position given in the ViewState object.
	Errors:	    -
	Scope:		Protected
	SeeAlso:	ScreenView::SetThisFromViewState; ScreenView::SetViewStateFromThis;
				DocView::ViewStateChanged
**********************************************************************************************/ 

void ScreenView::OnUpdate(wxView* sender, wxObject* hint)
{
	if ( sender==NULL && !fInitialUpdate )
	{
		OnInitialUpdate();
		fInitialUpdate = true;
	}
	CCamView::OnUpdate(sender,hint);
}


//
// OnIntialUpdate is no longer called automatically, so we call it instead
// just once from OnUpdate.
//
void ScreenView::OnInitialUpdate()
{
	// Set up current etc.
	SetCurrentStates();

	// Check if this view is being created by LoadAppWindowState at the start.  If it is
	// then pick up the ViewState object ready-made by LoadAppWindowState and set
	// this view according to that.
	if (pReadyMadeViewState != NULL)
	{
		// Set the flags saved in temp. storage in the ViewState in the corresponding
		// DocView.
		DocView* pdv = GetDocViewPtr();
		ERROR3IF(pdv == NULL, "Null DocView* in ScreenView::OnInitialUpdate");

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
	PORTNOTETRACE("other","ScreenView::OnInitialUpdate - code removed");
#ifndef EXCLUDE_FROM_XARALX
	if(HScrollBar&&VScrollBar)
	{

		// Calculate the scroller page & line sizes, and its range, and make sure the
		// work area extent is updated.
		FIXED16 PixelWidth, PixelHeight;
		pDocView->GetPixelSize(&PixelWidth, &PixelHeight);
		XLONG x1 = CurrentSize.GetWidth() * PixelWidth;
		XLONG x2 = CurrentSize.GetHeight() * PixelHeight;
		HScrollBar->SetPageSize(x1);
		VScrollBar->SetPageSize(x2);
		HScrollBar->SetLineSize(x1 / xlong(10) + xlong(1));	
		VScrollBar->SetLineSize(x2 / xlong(10) + xlong(1));
	}
#endif
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

PORTNOTE("other","ScreenView::OnInitialUpdate - code removed")
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
					ENSURE(pMainWnd, "No MDI frame window in ScreenView::OnShowWindow!");
					CMDIChildWnd* pMaxiWnd = pMainWnd->MDIGetActive(&fIsMaxed);

					// If it is maximised then restore it.
					if (fIsMaxed && pMaxiWnd != NULL)
					{
						wxView* pv = pMaxiWnd->GetActiveView();
						if (pv && pv->GetDocument() &&
							((CCamDoc*) pv->GetDocument())->IsUntouched())
						{
							TRACEUSER( "JustinF", _T("MDI Restore in ScreenView::OnInitialUpdate\n"));
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
		TRACEUSER( "JustinF", _T("\t- no parent frame in ScreenView::OnInitialUpdate\n"));
	}
#endif
#endif
	// Initialise the OLE selection (TODO: remove this when selection model code
	// is written).
	#if (_OLE_VER >= 0x200)		
		m_pSelection = 0;
	#endif

	// Make sure the render window is repainted and inform the kernel that something
	// has happened.
//	InvalidateView();
	pDocView->ViewStateChanged();

	// Allow the base class to do it's OnInitialUpdate stuff last, because it just
	// calls OnUpdate.
//	CCamView::OnInitialUpdate();

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
		ERROR3IF(!pOp, "Can't find the Zoom Operation in ScreenView::OnInitialUpdate");
		pOp->Invoke(0);

		// Restore the old prev. zoom setting.
		pDocView->SetPrevZoomIndex(nPrev);
		pDocView->SetPrevZoomScale(fxPrev);
		pDocView->SetPrevZoomOffset(wcPrev);
	}

#endif 
}



/*********************************************************************************************
>	void ScreenView::OnActivateView(BOOL bActivate, CView* pActiveView, CView* pDeactiveView)

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

void ScreenView::OnActivateView( bool bActivate, wxView* pActiveView, wxView* pDeactiveView )
{
	if ( !pDocView )
	{
		TRACE( _T("ScreenView::OnActivateView - Warning: pDocView uninitialised\n") );
		return;
	}

	if (bActivate) SetCurrentStates();

	Document* KernelDoc = NULL; 
	if (pDocView) KernelDoc = pDocView->GetDoc();

	// MFC does some hidden stuff here . . .
	CCamView::OnActivateView( bActivate, pActiveView, pDeactiveView );

	// Trigger off some messages into the kernel.
	if (bActivate)
	{
		// make sure the global preference reflects the current state
		SetDefaultRulersState(Status->RulersVisible);
		SetDefaultScrollersState(Status->ScrollersVisible);
		
		// Update our records of who is top-most and set the keyboard input focus to
		// the rendering child window.
		GetCanvas()->SetFocus();

		// Inform the kernel that the view's Z-order has changed. This sets the selected
		// View, Document, and spread, to legal values.
		Document::SetSelectedViewAndSpread(KernelDoc, pDocView, NULL);
	}
}



/********************************************************************************************
>	BOOL ScreenView::IsTopmost() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this view is the active view.
	Purpose:	Tests if this ScreenView is the active view, ie. is on top of all other views.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL ScreenView::IsTopmost() const
{
	PORTNOTETRACE("other","ScreenView::IsTopmost - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// always true for ralph
#ifdef RALPH
	   return TRUE;
#else
	   return ( (wxView *)this ) == GetParentFrame()->GetActiveView();
#endif
#else
	return TRUE;
#endif
}



 /********************************************************************************************
>	void ScreenView::ShowScrollers(BOOL ShowOrHide)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Hide or Show Scrollers.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void ScreenView::ShowScrollers(BOOL Show) 
{
	PORTNOTETRACE("other","ScreenView::ShowScrollers - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Set flags and Show/Hide the Scrollers
	if (Show)
	{
		VScrollBar->ShowWindow(SW_SHOW);
		HScrollBar->ShowWindow(SW_SHOW);
	}
	else
	{
		Status->ScrollersVisible = FALSE;
		VScrollBar->ShowWindow(SW_HIDE);
		HScrollBar->ShowWindow(SW_HIDE);
	}

	Status->ScrollersVisible = Show;

	// call OnSize directly to force new layout	
	wxRect				WRect;
	GetClientRect(&WRect);
	SendMessage(WM_SIZE, SIZE_RESTORED, MAKELPARAM(WRect.Width(), WRect.Height())); */
#endif
}



 /********************************************************************************************
>	void ScreenView::ShowRulers(BOOL ShowOrHide)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Hide or Show Rulers.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void ScreenView::ShowRulers(BOOL Show) 
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
>	afx_msg void ScreenView::OnSize(UINT32 nType, INT32 cx, INT32 cy)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    A flag indicating whether the window is maximised, minimised
				etc., the width and height of the ScreenView client area.
	Outputs:    -
	Returns:	-
	Purpose:	Moved down to ScreenCamView
	Errors:	    -
	Scope:		Protected
	SeeAlso:	CScroller::CalcPosFromParentClient(); CScroller::SetPageSize();
				CScroller::SetLineSize(); class CRendWnd; class CScrollerCorner;
				struct ViewState; DocView::ViewStateChanged()

**********************************************************************************************/ 

void ScreenView::OnSize( wxSizeEvent & )
{
}



/*********************************************************************************************
>	afx_msg LRESULT ScreenView::OnFrameMove(WPARAM, LPARAM)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12.4.94
	Inputs:		Not used.
	Outputs:	-
	Returns:	-
	Purpose:	Updates the ViewState object with the view's position, in an OS-
				independent form.  This message is sent by the view's frame window
				whenever it is moved.
	Errors:		-
	Scope:		Protected
	SeeAlso:	UpdateViewPosition; CCamMDIChild::OnMove; 
				struct ViewState; DocView::ViewStateChanged()

**********************************************************************************************/ 

// v1.1 BODGE BODGE BODGE
// JustinF says: Don't process WM_FRAMEMOVE messages to the view as Windows sends a WM_MOVE
// message to a view frame before ScreenView knows its correct size (ie. before
// ScreenView is sent a WM_SIZE message).  This causes the scrollers to be drawn
// momentarily in the wrong place when a view window is being created or activated.
// Fixes bug #1448.

#if 0

LRESULT ScreenView::OnFrameMove(WPARAM, LPARAM)
{
	
	SetCurrentStates();
	// Update the ViewState.
	UpdateViewPosition();
	if (fSetupDone) pDocView->ViewStateChanged();
	return 0;
}

#endif


///////////////////////////////////////////////////////////////////////////////////////////
// Painting.

/*********************************************************************************************
>	void ScreenView::GetClientSize(int * pWidth, int * pHeight) const TYPENOTE: Correct

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	The width and height of the client area which the kernel can render into.
	Returns:	-
	Purpose:	Calculates the size of the ScreenView's RendWnd.
	Errors:		-
	Scope:		Public
	SeeAlso:	class CRendWnd

**********************************************************************************************/ 

void ScreenView::GetClientSize(int * pWidth, int * pHeight) const /* TYPENOTE: Correct */
{
//	OilRect r = CurrentSize.ToOil(pDocView);
//	*pWidth = r.Width();
//	*pHeight = r.Height();
	WinRect wrect(0,0,0,0);
	GetCanvas()->GetClientSize(&wrect.width,&wrect.height);
	OilRect orect = wrect.ToOil(pDocView);
	*pWidth  = orect.Width();
	*pHeight = orect.Height();
}



/*********************************************************************************************
>	void ScreenView::InvalidateView(const OilRect* pRect = NULL, BOOL updatenow = FALSE)

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
	SeeAlso:	class CRendWnd; ScreenView::OnDraw()

**********************************************************************************************/ 

void ScreenView::InvalidateView(const OilRect* pRect, BOOL updatenow)
{
	if (pRect != NULL)
	{
		// Convert passed rectangle to an MFC-derived type and invalidate it.  Add an extra
		// pixel to the right and bottom edges of the rectangle to ensure that the whole
		// rectangle, after scaling and rounding, is invalidated.
		WinRect	r = pRect->ToWin( pDocView );
		r.width++;
		r.height++;		
       	GetCanvas()->Refresh( true, &r );
    }
	else
	{
		// We are completely redrawing the client area, so it is possible that the
		// "cached" previous scroll offsets, contained in OldOffset, will become
		// incorrect.
		GetCanvas()->Refresh( true );
		GetScrollOffset(&OldOffset);
	}
	
	if (updatenow) GetCanvas()->Update();
}




/*********************************************************************************************
>	afx_msg void ScreenView::OnPaint()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles WM_PAINT messages for the ScreenView window.  As the window is
				entirely covered by child windows, which know how to paint themselves,
				this function simply validates its (invisible) client area, removing
				the paint message.  No drawing of any kind is done.
	Errors:	    -
	Scope:	    Protected
	SeeAlso:    class CRendWnd

**********************************************************************************************/ 

PORTNOTE("other","Removed ScreenView::OnPaint usage")
#ifndef EXCLUDE_FROM_XARALX
void ScreenView::OnPaint()
{
	try
	{
		Error::RenderThreadIn();
		wxPaintDC dc(this);			// this validates the client area
		Error::RenderThreadOut();
	}
	catch( CResourceException e )
	{
		ENSURE(FALSE, "Resource exception in ScreenView::OnPaint!\n");
	}
}
#endif

/*********************************************************************************************

>	afx_msg LRESULT ScreenView::OnRenderView(WPARAM, LPARAM lp)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    A pointer to a CPaintDC, cast into an LPARAM.  The WPARAM is not used.
	Outputs:	-
	Returns:	Returns 0, indicating to Windows that the message was successfully
				processed.
	Purpose:	Responds to a WM_PAINT message sent to the RendWnd window object.
				Effectively tricks MFC into painting the RendWnd as if it was a
				CView window, not a child of a CView - calls CView::OnPrepareDC()
				and ScreenView::OnDraw().
	Errors:		-
	Scope:		Protected
	SeeAlso:	class CRendWnd; CRenderWnd::OnPaint(); ScreenView::OnDraw();

**********************************************************************************************/ 

PORTNOTE("other","Removed ScreenView::OnRenderView usage")
#ifndef EXCLUDE_FROM_XARALX
LRESULT ScreenView::OnRenderView(WPARAM, LPARAM lp)
{
	CPaintDC* pDC = (CPaintDC*) lp;
	OnPrepareDC(pDC);
	OnDraw(pDC);
	return 0;
}
#endif


// set this to 1 to show update rectangles using cross-hatching, 0 to not
#define	SHOW_RECT	0

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
>	virtual void ScreenView::OnDraw(CDC* pDC)

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
	SeeAlso:	CView::OnPrepareDC(); ScreenView::OnRenderView(); DocView::OnDraw()

**********************************************************************************************/ 

void ScreenView::OnDraw( wxDC *pDC )
{
//	TRACE( _T("ScreenView::OnDraw\n"));

	if (pDocView==NULL)
		return;
		
	// Set the current DocView, ie. the DocView being worked upon. (and the Doc!)
	pDocView->SetCurrent();
	(pDocView->GetDoc())->SetCurrent();

	// Are we printing?
PORTNOTE("print","Removed Printing from OnDraw")
#ifndef EXCLUDE_FROM_XARALX
	if (pDC->IsPrinting())
	{
		//TRACE( _T("BAD ! ScreenView::OnDraw reckons we're printing\n"));
#ifndef STANDALONE

		// Yes - do the printing!

		// Find out from the display context the rectangle bounding the invalid
		// region.
		WinRect clip;

		if (pDC->GetClipBox(&clip) == NULLREGION) 
			return;

		// Pass the drawing parameters on to the associated PrintView object.
		pPrintView->AttachToDC(pDC);
		pPrintView->OnDraw(pDC, clip.ToOil(pPrintView));
		pPrintView->AttachToDC(NULL);

#endif
	}
	else
#endif
	{
		WinRect clip;

		// wxWidget only provides a unified accessor, so that's
		// what we'll use
		wxRegionIterator upd( m_pCanvas->GetUpdateRegion() ); // get the update rect list
		while( upd )
		{
			clip.x		= upd.GetX();
			clip.y		= upd.GetY();
			clip.width  = upd.GetW();
			clip.height = upd.GetH();

//			DumpRect( pDC, &clip, wxT("OnDraw UpdateRect : ") );

			OilRect oRect = clip.ToOil( pDocView );
//			TRACE( _T("OilRect = (%d, %d) - (%d, %d)\n"), oRect.lo.x, oRect.lo.y, oRect.hi.x, oRect.hi.y);
			
			pDocView->OnDraw( pDC, oRect );

			upd ++ ;
		}

//#pragma message( __LOCMSG__ "ScreenView::OnDraw - Forcing redraw" )
//TRACE( _T("ScreenView::OnDraw - Forcing redraw") );
//		Camelot.GetRegionList()->ImmediateRender(true);

#if FALSE
		// Find out from the display context the rectangle bounding the invalid
		// region.  If we can, try to use lists of rectangles.
		wxRect		   *lpRect;
		UINT32			count = CCDC::GetRectangleList( pDC, &lpRect );

		TRACE( _T("ScreenView::OnDraw called with %d regions\n"), count);

		// If there's more than one rectangle then iterate through them all...
		if (count > 1)
		{
			while (count--)
			{
				clip.x		= lpRect->x;
				clip.x		= lpRect->y;
				clip.width  = lpRect->width;
				clip.height = lpRect->height;

				DumpRect( pDC, &clip, "camrect1" );

				pDocView->OnDraw( pDC, clip.ToOil(pDocView) );
				lpRect++;
			}
		}
		else
		{
			//TRACE( _T("ScreenView::OnDraw doing simple rectangle stuff"));
			
			// . . . else handle the simple <2 rectangle code
			pDC->GetClippingBox( clip );
			if( !clip.IsEmpty() )
			{
				// Pass the drawing parameters on to the associated DocView object.
				DumpRect( pDC, &clip, "camrect2" );
				pDocView->OnDraw( pDC, clip.ToOil(pDocView) );
			}
		}
#endif
	}

//	TRACE( _T("Leaving ScreenView::OnDraw\n"));
}

/*********************************************************************************************
>	BOOL ScreenView::OnPreparePrinting(CPrintInfo* pInfo)

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

PORTNOTE("print","Removed ScreenView::OnPreparePrinting")
#ifndef EXCLUDE_FROM_XARALX
BOOL ScreenView::OnPreparePrinting(CPrintInfo* pInfo)
{
#ifndef STANDALONE

	if (!CCPrintDialog::OnPreparePrinting(pInfo,pDocView->GetDoc()))
		ERROR2(FALSE,"Unable to initialise the customized print dialog");

	// default preparation
//	BOOL Result = CCamView::DoPreparePrinting(pInfo);
//	if (!Result)
//		return FALSE;

	// Check because brainless CView only checks for valid DC in debug builds - handy eh? NOT!
	if (pInfo->m_pPD->m_pd.hDC == NULL)
	{
		ERROR1(FALSE,_R(IDE_NULL_PRINTDC));
		//ERROR2(FALSE, "No valid DC returned by print dialog!");
	}

	// Get ourselves a print view and attach it to this CamView.
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
		ERROR2(FALSE, "Unable to attach PrintView to CamView.");
	}

#endif

	// OK if we got this far.
	return TRUE;
}
#endif

/*********************************************************************************************
>	wxDC* ScreenView::GetRenderDC() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2nd Sept 1993
	Inputs:	    -
	Outputs:	-
	Returns:	A pointer to the CDC object associated with the RenderWindow.
	Purpose:	Allows the kernel to draw on the client area at times other than
				when processing WM_PAINT messages.  Note that the pointer returned
				is temporary and should NOT be stored away for future use - it may well
				become invalid on the next idle event.
	Errors:		Returns NULL if the DC could not be obtained.
	Scope:		Public
	SeeAlso:	ScreenView::OnDraw()

**********************************************************************************************/ 

wxDC *ScreenView::GetRenderDC() const
{
//
// GAT: Warning - I don't think we want to be using this function anymore.
// Create a DC when required.
//
	PORTNOTETRACE("other","ScreenView::GetRenderDC - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	return GetCanvas()->GetDC();
#else
	return NULL;
#endif
}



/*********************************************************************************************
>	wxWindow* ScreenView::GetRenderWindow() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15th October 1993
	Inputs:	    -
	Outputs:	-
	Returns:	A pointer to the RenderWindow child of this ScreenView
	Purpose:	Allows the kernel to access to the render window, useful on the odd
				occasion, although it does distort "encapsulation".
	Errors:		-
	Scope:		Public
	SeeAlso:	ScreenView::GetRenderDC()

**********************************************************************************************/ 

wxWindow *ScreenView::GetRenderWindow() const
{
	return GetCanvas();
}

	


////////////////////////////////////////////////////////////////////////////////	
// Scrolling functions.

// This preference controls whether scrollers appear proportional or standard.
BOOL ScreenView::PropScrollersOn = TRUE;
BOOL ScreenView::DefaultScrollersState = TRUE;
BOOL ScreenView::DefaultRulersState = FALSE;
BOOL ScreenView::UseVisibleDefaults = TRUE; 


/*********************************************************************************************
>	void ScreenView::ScrollTo(const WorkCoord& offset)

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
	SeeAlso:	ScreenView::SetScrollPos()

**********************************************************************************************/ 

void ScreenView::ScrollTo(const WorkCoord& offset)
{
	PORTNOTETRACE("other","ScreenView::ScrollTo - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Calculate the difference between where we are and where we want to be.  Notice
	// the asymmetry in the calculations, to account for the Windows coordinate origin
	// being in the top-left, not the bottom-left.
	FIXED16 PixelWidth, PixelHeight;
	pDocView->GetPixelSize(&PixelWidth, &PixelHeight);
	INT32 dx = ((offset.x - OldOffset.x) / PixelWidth.MakeLong() );
	INT32 dy = ((OldOffset.y - offset.y) / PixelHeight.MakeLong() );
	if (dx == 0 && dy == 0) return;
	
	// ZZZ
	
	
	// Scroll part of, or redraw the whole of, the client area.
	if (Abs(dx) >= CurrentSize.GetRight() || Abs(dy) >= CurrentSize.GetBottom() )
		GetCanvas()->Refresh( true );
	else
		GetCanvas()->ScrollWindow(-dx, -dy);
	
// WEBSTER - markn 15/1/97
// No rulers in Webster
#ifndef WEBSTER
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	if(Status->RulersVisible)
	{
		if(HRuler)
			HRuler->ScrollRuler(-dx);
		if(VRuler)
			VRuler->ScrollRuler(-dy);

	}
#endif
#endif // WEBSTER
	// Remember this scroll offset, which corresponds to what will be drawn on the screen,
	// and force an immediate repaint of the invalid area.
	OldOffset = offset;
	GetCanvas()->UpdateWindow();

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// Make sure that the system will update the state of gadgets after this, some of
	// which may now be "greyed" because what they apply to is no longer visible etc.
	// This is the best place to put this, as all scroll requests, whether they come
	// from the user or are internally generated, go through this function.
	DialogBarOp::SetSystemStateChanged(TRUE);
#endif
#endif
}



/*********************************************************************************************
>	BOOL ScreenView::GetScrollerStyle() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    -
	Outputs:	-
	Returns:	TRUE if this ScreenView is currently sporting proportional scroll bars, FALSE
				if it isn't, and its scroll bars look like typical naff Windows ones.
	Purpose:    Tests whether proportional scroll bars are currently in operation.
	Errors:		-
	Scope:		Public
	SeeAlso:	ScreenView::SetScrollerStyle; CScroller::SetProportional()

**********************************************************************************************/ 

BOOL ScreenView::GetScrollerStyle() const
{
	return ScrollerStyle;
}



/*********************************************************************************************
>	void ScreenView::SetScrollerStyle(BOOL proportional)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    A boolean which if TRUE means that proportional scroll bars should be turned
				on, if FALSE that the scroll bars should look normal.
	Outputs:	-
	Returns:	-
	Purpose:	Allows DocView to change the style - groovy or dull - of ScreenView's scroll
				bars.  Records the setting and calls the appropriate functions of the scroll
				bar gadgets
	Errors:		-
	Scope:		Public
	SeeAlso:	ScreenView::GetScrollerStyle(); CScroller::SetProportional()

**********************************************************************************************/ 

void ScreenView::SetScrollerStyle(BOOL proportional)
{
	if(!HScrollBar||!VScrollBar)
		return;

	ScrollerStyle = proportional;
	PORTNOTETRACE("other","ScreenView::SetScrollerStyle - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	HScrollBar->SetProportional(ScrollerStyle);
	VScrollBar->SetProportional(ScrollerStyle);
#endif
}



/*********************************************************************************************
>	void ScreenView::GetWorkAreaExtent(WorkRect* pArea) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	Changes the pointed to WorkRect to the current WorkAreaExtent.
	Returns:	-
	Purpose:	Provides read-only access to the WorkAreaExtent, as maintained by
				DocView/ScreenView.
	Errors:		-
	Scope:		Public
	SeeAlso:	ScreenView::SetWorkAreaExtent()

**********************************************************************************************/ 
/*
GAT: Never used.

void ScreenView::GetWorkAreaExtent(WorkRect* pArea) const
{
	*pArea = Status->WorkAreaExtent;
}

*/

/*********************************************************************************************
>	void ScreenView::SetWorkAreaExtent(const WorkRect& area, BOOL redraw)

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
	SeeAlso:	ScreenView::GetWorkAreaExtent(); CScroller::SetPageSize();
				CScroller::SetScrollRange(); class CRendWnd

**********************************************************************************************/ 

void ScreenView::SetWorkAreaExtent(const WorkRect& area, BOOL redraw)
{
	WorkArea = area;
	FIXED16 PixelWidth, PixelHeight;
	pDocView->GetPixelSize(&PixelWidth,&PixelHeight);
	// This doesn't give us a redraw flag.
	GetCanvas()->SetVirtualSize(
		INT32((area.hi.x-area.lo.x)/PixelWidth .MakeDouble()),
		INT32((area.hi.y-area.lo.y)/PixelHeight.MakeDouble())
	);
}



/*********************************************************************************************
>	void ScreenView::GetScrollOffset(WorkCoord* pOffset) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	The current scroll offset is written to the passed WorkCoord.
	Returns:	-
	Purpose:	Reads the offset from the horizontal and vertical scroll bars,
				combining then into a WorkCoord.
	Errors:		-
	Scope:		Public
	SeeAlso:	ScreenView::SetScrollOffset(); CScroller::GetScrollPos()

**********************************************************************************************/ 

void ScreenView::GetScrollOffset(WorkCoord* pOffset) const
{	
	FIXED16 PixelWidth,PixelHeight;
	pDocView->GetPixelSize(&PixelWidth,&PixelHeight);

//	TRACE( _T("SV::GetScrollOffset\n"));
//	TRACE( _T("CanvasPos = (%d, %d)\n"), GetCanvas()->GetScrollPos(wxHORIZONTAL), GetCanvas()->GetScrollPos(wxVERTICAL));

	pOffset->x = XLONG(WorkArea.lo.x+GetCanvas()->GetScrollPos(wxHORIZONTAL)*PixelWidth .MakeDouble());
	pOffset->y = XLONG(WorkArea.hi.y-GetCanvas()->GetScrollPos(wxVERTICAL  )*PixelHeight.MakeDouble());

//	TRACE( _T("ScrollOffset = (%d, %d)\n"), (INT32)(pOffset->x), (INT32)(pOffset->y));
}



/*********************************************************************************************
>	void ScreenView::SetScrollOffset(const WorkCoord& pos, BOOL redraw)

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
	SeeAlso:	ScreenView::GetScrollOffset; CScroller::SetScrollPos();
				ScreenView::OnScrollerEvent()

**********************************************************************************************/ 

void ScreenView::SetScrollOffset(const WorkCoord& pos, BOOL redraw)
{
//	TRACE( _T("SV::SetScrollOffset(%d, %d)\n"), (INT32)(pos.x), (INT32)(pos.y));
	FIXED16 PixelWidth,PixelHeight;
	pDocView->GetPixelSize(&PixelWidth,&PixelHeight);

//	INT32 sx = INT32((pos.x-WorkArea.lo.x)/PixelWidth .MakeDouble());
//	INT32 sy = INT32((WorkArea.hi.y-pos.y)/PixelHeight.MakeDouble());
//	TRACE( _T("Scrolling to (%d, %d)\n"), sx, sy);

	GetCanvas()->Scroll(
		INT32((pos.x-WorkArea.lo.x)/PixelWidth .MakeDouble()),
		INT32((WorkArea.hi.y-pos.y)/PixelHeight.MakeDouble())
	);

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
>	afx_msg LRESULT ScreenView::OnScrollerEvent(WPARAM redraw, LPARAM)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    A boolean, cast to a WPARAM, which indicates whether ScreenView should update
				the client area in response to this message.  The LPARAM is not used.
	Outputs:    -
	Returns:	Returns 0, indicating to Windows that the message has been processed
				successfully.
	Purpose:	This message is sent by the scroll bars in response to an earlier call to
				ScreenView::SetScrollOffset().  The scroll offset is read from the scroll bars
				and the ScrollPosition field of the ViewState is updated accordingly.  If
				the WPARAM parameter is TRUE then the client area is repainted via a call
				to ScreenView::ScrollTo().
	Errors:		-
	Scope:		Protected
	SeeAlso:	ScreenView::SetScrollOffset(); ScreenView::ScrollTo(); CScroller::MoveThumb()

**********************************************************************************************/ 

PORTNOTE("other","Removed ScreenView::OnScrollerEvent")
#ifndef EXCLUDE_FROM_XARALX
LRESULT ScreenView::OnScrollerEvent(WPARAM redraw, LPARAM barptr)
{
	SetCurrentStates();
	if (Status && fSetupDone)
	{
		WorkCoord temp;
		GetScrollOffset(&temp);
		Status->SetScrollPos(temp);
		if (redraw) ScrollTo( Status->GetScrollPos() );
	}
	return 0;
}
#endif

/*********************************************************************************************
>	afx_msg LRESULT ScreenView::OnJoystickMove(WPARAM buttons, LPARAM pos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:	    The Joystick position and button states
	Outputs:    -
	Returns:	Returns 0, indicating to Windows that the message has been processed
				successfully.
	Purpose:	This function is called whenever the joystick moves.
				It is used currently to set the Pretend Pen Pressure from the joystick
				position.
				This function only exists under Win32.
	Errors:		-
	Scope:		Protected
	SeeAlso:	ScreenView::OnKeyDown()

**********************************************************************************************/ 

PORTNOTE("other","Removed ScreenView::OnJoystickMove")
#ifndef EXCLUDE_FROM_XARALX
#ifdef WIN32
LRESULT ScreenView::OnJoystickMove(WPARAM buttons, LPARAM pos)
{
	SetCurrentStates();
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
// WEBSTER - markn 25/4/97
// No pen stuff required in Webster
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
	(Camelot.GetPressurePen())->SetPressureFromJoystick(buttons, pos);
#endif // VECTOR_STROKING
#endif

	return 0;
}
#endif
#endif


///////////////////////////////////////////////////////////////////////////////
// Mouse dragging and tool operation functions.

// This user preference controls how many pixels the mouse must move before a
// drag is initiated.
INT32 ScreenView::DragLatency = 3;

// Used to start drags after user has clicked but not moved mouse for n milliseconds.
INT32 ScreenView::DragDelay = 500;
MonotonicTime ScreenView::DragTimer;



/*********************************************************************************************
>	afx_msg BOOL ScreenView::OnSetCursor(wxWindow* pWnd, UINT32 nHitTest, UINT32 nMessage)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14 Oct 93
	Inputs:		"pWnd" points to the window that contains the cursor;
				"nHitTest" contains a code indicating the part of the window the cursor is over;
				"nMessage" contains the mouse-message numeric identifier (WM_LBUTTONDOWN etc).
	Returns:	TRUE, ie. the cursor has been set, and no further processing is necessary.
				FALSE if the cursor has not been set, e.g. this view is not active.
	Purpose:	Calls the kernel DocView object to set the cursor for this ScreenView.
	Scope:	    Protected
	SeeAlso:	DocView::OnSetCursor

**********************************************************************************************/ 

PORTNOTE("other","Removed ScreenView::OnSetCursor")
#ifndef EXCLUDE_FROM_XARALX
BOOL ScreenView::OnSetCursor(wxWindow* pWnd, UINT32 nHitTest, UINT32 nMessage)
{
	// Child windows assume responsibility for setting their own cursor.
	if (pWnd == GetCanvas())
	{	
		// Get the kernel to do the business for the view display ....
		ENSURE(pDocView != 0, "No link to kernel DocView in ScreenView::OnSetCursor!\n");

		SetCurrentStates();
		pDocView->OnSetCursor();
		return TRUE;
	}
	else if (pWnd == Corner)
		return Corner->OnSetCursor(Corner, nHitTest, nMessage);

	return FALSE;
}
#endif

/*********************************************************************************************
>	static void ScreenView::GetClickMods(wxMouseEvent &event, ClickModifiers* p)

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
	SeeAlso:    ScreenView::HandleDragEvent()
**********************************************************************************************/ 

void ScreenView::GetClickMods(wxMouseEvent &event, ClickModifiers* p)
{
	*p = ClickModifiers::GetClickModifiers(event);
}




/********************************************************************************************
>	void ScreenView::HandleDragEvent(UINT32 Button, wxMouseEvent event, ClickType t) const

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
	Errors:		-
	Scope:		Private
	SeeAlso:	ScreenView::GetClickMods(); ScreenView::OnLButtonDown(); ScreenView::OnRButtonDown()
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

void ScreenView::HandleDragEvent(UINT32 Button, wxMouseEvent &event, ClickType t)
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
	GetClickMods(event, &LastClickMods);

	// If it's the first single click, then reset the drag delay timer, and ask for a 
	// Windows timer.
	if (((t == CLICKTYPE_SINGLE) || (t == CLICKTYPE_DOUBLE)) && 
		(FirstClickButton == 0))
	{
		// Remember this event.
		LastClickPoint = event.GetPosition();
		LastClickType = t;
		LastClickButton = Button;
		FirstClickButton = Button;
		DragTimer.Sample();

#if (_OLE_VER >= 0x200)
		// The user's doing something so don't update any container docs
		CCamDoc::ResetContainerUpdateTimer();
#endif

PORTNOTE("other","Removed FixSetTimer usage")
#ifndef EXCLUDE_FROM_XARALX
		// Ask the system for a timer.  Fails (returns FALSE) if there isn't one available.
		// The timer is used to generate DragPointerIdle events, which are not directly
		// supported by Windows.
		UINT32 rate;
		::SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &rate, 0);
		if (!::FixSetTimer(m_hWnd, 1, rate, 0))
		{
			ENSURE(FALSE, "Cannot get a timer in ScreenView::StartDrag()!\n");
		}
#endif
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
		WinCoord		point;
		event.GetPosition( &point.x, &point.y );
		pDocView->OnClick( ( (WinCoord *)&point )->ToOil(pDocView, TRUE), t, LastClickMods);
	}
}                                                             



/********************************************************************************************
>	BOOL ScreenView::HandleMouseWheelEvent(UINT32 nFlags, short zDelta, wxPoint pt)
	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/2000
	Purpose:	Handle MouseWheel for Scroll document and Zoom operations
				Also dispatches a message to the Gallery the mouse is over, if any...
	Inputs:		nFlags - The mouse flags
				zDelta - movement of the mousewheel
				pt - the coordinate of the mouse cursor
********************************************************************************************/ 

BOOL ScreenView::HandleMouseWheelEvent( wxMouseEvent &event )
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

	PORTNOTETRACE("other","ScreenView::HandleMouseWheelEvent - do nothing");
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
		if (nFlags & MK_CONTROL)
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
//						pZoomOp->ZoomAtPoint(WorkMouse, pZoomOp->GetPresetZoomScale(i), FALSE);
						pZoomOp->MouseWheelZoomAtPoint(WorkMouse, pZoomOp->GetPresetZoomScale(i), FALSE);
						break;
					}
				}

//				pZoomOp->ZoomIn(WorkMouse);
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
//						pZoomOp->ZoomAtPoint(WorkMouse, pZoomOp->GetPresetZoomScale(i), FALSE);
						pZoomOp->MouseWheelZoomAtPoint(WorkMouse, pZoomOp->GetPresetZoomScale(i), FALSE);
						break;
					}
				}

//				pZoomOp->ZoomOut(WorkMouse);
			}

			// Tidy away...
			delete pZoomOp;

			return TRUE;
		}
		else
		{
			// The CTRL key was not pressed, so we should simply (is anything simple around here)
			// scroll the viewport up/down. (OR left/right if SHIFT is held down - Matt 16/11/2000)
	//		TRACEUSER( "Matt", _T("MouseWheel = Scroll, in ScreenView\n"));

			if (nFlags & MK_SHIFT)
			{
				// If HScrollBar doesn't exist at this point then something's not quite right...
				if(!HScrollBar)	{	return FALSE;	}

				// Get the current Horizontal Scroll Position, increment it by a zoom-dependent amount
				// and set the Horizontal Scroll Position to be this value, flagging it to redraw...
				XLONG horizpos;
				HScrollBar->GetScrollPos(&horizpos);
				horizpos -= (XLONG)((zDelta/120) * 750 * 15);
				HScrollBar->SetScrollPos(horizpos, true);
			}
			else
			{
				// If VScrollBar doesn't exist at this point then something's not quite right...
				if(!VScrollBar)	{	return FALSE;	}

				// Get the current Vertical Scroll Position, increment it by a zoom-dependent amount
				// and set the Vertical Scroll Position to be this value, flagging it to redraw...
				XLONG vertpos;
				VScrollBar->GetScrollPos(&vertpos);
				vertpos -= (XLONG)((zDelta/120) * 750 * 15);
				VScrollBar->SetScrollPos(vertpos, true);
			}

			// NOTE : If we wanted to be really flash, we could make the scroll distance affected by the
			// user's settings in the Control Panel - ie, default is to scroll 3 lines on 120 zDelta, but
			// can be adjusted so we could look at that value...
		}
		// tell everyone we've changed the screen
		BROADCAST_TO_ALL(ScreenChangeMsg());
	}
#endif
	// If we get to here, then we must have handled the MouseWheel, so return true...
	return TRUE;
}



/*********************************************************************************************
>	void ScreenView::HandleButtonUp(UINT32 nFlags, wxPoint point)

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
	SeeAlso:	ScreenView::GetClickMods(); ScreenView::OnLButtonDown(); ScreenView::OnRButtonDown()
				DocView::OnClick()

**********************************************************************************************/ 

void ScreenView::HandleButtonUp(UINT32 Button, wxMouseEvent &event)
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
if (m_pCanvas && m_pCanvas->GetCapture()==m_pCanvas)
			m_pCanvas->ReleaseMouse();
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
PORTNOTE("other","Removed FixKillTimer usage")
#ifndef EXCLUDE_FROM_XARALX
			::FixKillTimer(m_hWnd, 1);
#endif
			FirstClickButton = 0;

			//Then pass the Up-Click message to ScreenView::OnClick
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
			//Pass the Up-Click message to ScreenView::OnClick
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
//		"ScreenView::HandleButtonUp - drag operation in progress but no buttons down");
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
>	BOOL ScreenView::InvokeDragOp(String_256 OpToken,UINT32 Flags,wxPoint point,OpParam* pParam)

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

BOOL ScreenView::InvokeDragOp(String_256* pOpToken,OpParam* pParam,UINT32 Flags,wxPoint point)
{
	PORTNOTETRACE("other","ScreenView::InvokeDragOp - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	DragOpToken  = *pOpToken;
	pDragOpParam = pParam;
	DragOpInvoke = TRUE;

	PostMessage(WM_LBUTTONDOWN,Flags,point.x | (point.y << 16));
#endif
	return TRUE;
}




/*********************************************************************************************
>	BOOL ScreenView::StartDrag(Operation* pOp, DragType type, BOOL KeepAccuracy)

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
	SeeAlso:	ScreenView::EndDrag(); ScreenView::HandleDragEvent();
                          
**********************************************************************************************/ 

BOOL ScreenView::StartDrag(Operation* pOp, DragType type, BOOL KeepAccuracy)
{
	ENSURE(	pCurrentDragOp == 0, "ScreenView::StartDrag: pCurrentDragOp is not zero");
	
	// This has been commented out as some screen drivers can not handle the mouse drifting
	// off the top of the screen.
	// Unbound mouse from the screen and set the current drag operation.  Note that under
	// Win32 raw mouse coordinates change from being 16-bit INTs to 16-bit SHORTs.
	//WinRect unbound(SHRT_MIN / 4, SHRT_MIN / 4, SHRT_MAX / 4, SHRT_MAX / 4);
	//::GetClipCursor(&MouseClipRect);
	//::ClipCursor(&unbound);
	GetCanvas()->CaptureMouse();

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
>	BOOL ScreenView::EndDrag(Operation* pOp)

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
	SeeAlso:	ScreenView::StartDrag(); ScreenView::HandleDragEvent()

**********************************************************************************************/ 

BOOL ScreenView::EndDrag(Operation*)
{
/*
#ifdef	_DEBUG
	// Prevent a very nasty recursive ENSURE!
	if (!CCamApp::IsDisabled())
	{
		ENSURE(pCurrentDragOp != 0, "ScreenView::EndDrag: pCurrentDragOp is zero");
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
if (m_pCanvas && m_pCanvas->GetCapture() == m_pCanvas)
	m_pCanvas->ReleaseMouse();

	// Check status of temporary tool selection - this is because we don't change tool
	// mid-drag! (e.g. user holds down Alt to get selector, starts drag, releases Alt,
	// then ends drag - we want to restore the original tool).
	Tool::EndDrag();

	// Kill the timer started in HandleDragEvent().
PORTNOTE("other","Removed FixKillTimer usage")
#ifndef EXCLUDE_FROM_XARALX
	return ::FixKillTimer(m_hWnd, 1);
#else
	return TRUE;
#endif
}



/********************************************************************************************

>	BOOL ScreenView::ChangeDragType(DragType Type)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/4/95
	Inputs:		Type - the type of drag to change to
	Returns:	TRUE
	Purpose:	Allows someone to change the type of a drag (eg have Auto Scrolling on
				in some situations, but not in others). This function is mostly called from
				the Kernel (DocView) version of the same function
	SeeAlso:	DocView::ChangeDragType

********************************************************************************************/

BOOL ScreenView::ChangeDragType(DragType Type)
{
	// Change the current drag type
	CurrentDragType = Type;
	m_OLELastOutTime.Sample();
	OLERecaptureMouse = FALSE;

	return TRUE;
}



/********************************************************************************************

>	BOOL ScreenView::IsWaitingMouseMove()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Returns:	TRUE if there is a mouse move message waiting to be processed
	Purpose:	There are some things in Camelot that are processed as a result of a mouse
				move message (Magnetic snapping to all the objects in the document for
				example). If these operations take a long time, there is little point
				in carrying on with the calculations if the mouse has already moved away
				from the coord being tested (and hence invalidating the findings of the
				search).
				This function checks to see if there is a mouse move message waiting in
				the queue to be processed, so that the callers can abort any processing they
				were doing if applicable.

********************************************************************************************/

BOOL ScreenView::IsWaitingMouseMove()
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	MSG Msg;
	// if there is a current drag all messages will be routed to the drag capture
	// capture window so there's no point in looking in this queue  
	if(DragManagerOp::GetCurrentManager()==NULL)
	{
		while (PeekMessage(&Msg,m_hWnd, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_NOREMOVE))
		{
			// Check that this message is meant for this CamView OR it's RenderWindow child
			// window because the OS sometimes leaves the RenderWindow handle in the MouseMove
			// message (that window was the first to receive the message)
			if (m_hWnd==Msg.hwnd || Msg.hwnd==GetCanvas()->m_hWnd)
			{
				// There is a mouse move message in the queue.
				// Have to cast to INT8 so we get correct sign-extension.
				wxPoint Point;
				Point.x = (INT8) LOWORD(Msg.lParam);
				Point.y = (INT8) HIWORD(Msg.lParam);

				// See if this mouse move message is different from the last one
				if (Point!=CurrentMousePos)
					return TRUE;

				// Sadly, it was the same as the last one
				// so remove it from the queue as this should not happen
				PeekMessage(&Msg, m_hWnd, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE);
			}
			else
			{
				// Peek message bug. Gave me a message for someone elses window
				// Have to return something, so we will assume that there was not
				// a message in the queue.
				return FALSE;
			}
		}
	} 
	else   // there is a current drag
	{
		if(DragManagerOp::GetDragCaptureWnd()!=NULL){
			while (PeekMessage(&Msg,DragManagerOp::GetDragCaptureWnd()->m_hWnd,WM_MOUSEMOVE, WM_MOUSEMOVE, PM_NOREMOVE))
			{
				// Check that this message is meant for this CamView OR it's RenderWindow child
				// window because the OS sometimes leaves the RenderWindow handle in the MouseMove
				// message (that window was the first to receive the message)
				if (Msg.hwnd==DragManagerOp::GetDragCaptureWnd()->m_hWnd)
				{
					// There is a mouse move message in the queue.
					// Have to cast to INT8 so we get correct sign-extension.
					wxPoint Point;
					Point.x = (INT8) LOWORD(Msg.lParam);
					Point.y = (INT8) HIWORD(Msg.lParam);

					// See if this mouse move message is different from the last one
					if (Point!=CurrentMousePos)
						return TRUE;

					// Sadly, it was the same as the last one
					// so remove it from the queue as this should not happen
					PeekMessage(&Msg, m_hWnd, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE);
				}
				else
				{
					// Peek message bug. Gave me a message for someone elses window
					// Have to return something, so we will assume that there was not
					// a message in the queue.
					return FALSE;
				}
			}
		}
	}
#endif
	// there were no mouse move messages in the queue
	return FALSE;
}



/*********************************************************************************************
>	void ScreenView::HandleDragScrolling(wxPoint point)

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
	SeeAlso:	ScreenView::OnMouseMove; ScreenView::OnTimer

**********************************************************************************************/ 

void ScreenView::HandleDragScrolling(wxPoint point)
{
	// Get the size of the view.  We may have to deflate the rectangle a little here, if the
	// bottom-right edges of the view coincide with the bottom-right edges of the screen,
	// otherwise it is not possible to auto-scroll to the right or down as the mouse never
	// leaves the view window.
	WinRect wrSize = CurrentSize;
	point = GetCanvas()->ClientToScreen( point );
	point = GetFrame()->ScreenToClient( point );
	
// WEBSTER - markn 15/1/97
// No rulers in Webster
#ifndef WEBSTER
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
#endif // WEBSTER

PORTNOTE("other","Removed special handling for Zoomed and FullScreen")
#ifndef EXCLUDE_FROM_XARALX
	CCamFrame* pMainWnd = GetMainFrame();
	if (pMainWnd != NULL && (pMainWnd->InFullScreenMode() || pMainWnd->IsZoomed()) &&
		CurrentDragType != DRAGTYPE_OLESCROLL)
	{
//		TRACEUSER( "JustinF", _T("Deflating bounds for special auto-scroll handling\n"));
		//const INT32 nMargin = (IsWin32c()) ? 4 : 8;
		const INT32 nMargin = (CCamApp::IsNewWindowsUI()) ? 4 : 8;
		wrSize.Inflate(-nMargin, -nMargin);
	}
#endif

	// Initialise Deltas...
	INT32 dx = 0;
	INT32 dy = 0;

	// Depending on the type of the drag...
	switch (CurrentDragType)
	{
	//-------------------------
	case DRAGTYPE_NOSCROLL:
		break;

	//-------------------------
	case DRAGTYPE_AUTOSCROLL:
		{
			// Has the mouse moved outside there bounds?
			if (!wrSize.IsEmpty() && !wrSize.Inside(point))
			{
				// The mouse is outside the view, and we have to scroll the view
				// proportionate to how far outside it is.
				CalcScrollDeltas(point,wrSize,&dx,&dy);
			}
			break;
		}

	//-------------------------
	case DRAGTYPE_OLESCROLL:
		{
PORTNOTE("other","Disable DRAGTYPE_OLESCROLL")
#ifndef EXCLUDE_FROM_XARALX
			WinRect Outer = wrSize;

			// Set auto-scroll region to be one scrollbar width within the current
			// view (as per The Windows Interface Guidelines Chapter 5).
			INT32 ScrollbarSize = VScrollBar->GetScrollerSize();
			wrSize.Inflate(-ScrollbarSize,-ScrollbarSize);

			if (!wrSize.IsEmpty() && !Outer.IsEmpty()
				&& !wrSize.Inside(point)		// Outside allowed scroll rectangle
				)
			{
				if (Outer.Inside(point))		// and still inside visible window
				{
					// If we've been in the OLE scroll region for long enough
					if (m_OLELastOutTime.Elapsed(100))
					{
						// NOTE: According to Windows Interface Guidelines Chapter 5 we should
						// Test the "speed" of the mouse here but because we have already
						// checked that the mouse has been in the region for longer than 100
						// milliseconds it doesn't seem neccessary. (To be within the region
						// for longer than 100 milliseconds is sort of a speed test anyway!)

						// The mouse is in the scroll region, and we have to scroll the view
						// proportionate to how far into the region it is.
						CalcScrollDeltas(point,wrSize,&dx,&dy);
					}
				}
				else
				{
					// We've dragged beyond the OLE scroll area, so we can now convert the drag
					// into an OLE export drag if we so desire.
					HandleOleDragOutOfView(point);
				}
			}
			else
			{
				// We're not in the OLE scroll region so reset the timer
				m_OLELastOutTime.Sample();
			}
#endif
			break;
		}

	//-------------------------
	case DRAGTYPE_DEFERSCROLL:
		{
			// If the mouse is within the window, and we are performing deferred-scrolling
			// then we can change over to auto-scrolling.
			if (!wrSize.IsEmpty() && wrSize.Inside(point))
			{
				CurrentDragType = DRAGTYPE_AUTOSCROLL;
		 		AutoScrollExcludeRulers = TRUE;
			}
			break;
		}

	default:
		ERROR3("Shouldn't ever get here!");
		break;
	}

	//------------------------------------
	// Now do the scroll if necessary...
	// If dx<>0 or dy<>0 and scroll lock is disabled

	// Test if the SCROLL LOCK key has been set.
	if( !wxGetKeyState( CAMKEY(SCROLL) ) && !(dx==0 && dy==0))
	{
		// Set Current states...
		SetCurrentStates();

		// Convert these pixel values into a WorkCoord offset, add to the
		// scroll position and update the scroll bars etc.  Note that the
		// y offset is subtracted as the y-axis is reversed compared to
		// Windows . . .
		WorkCoord offset;
		GetScrollOffset(&offset);
		FIXED16 PixelWidth, PixelHeight;
		pDocView->GetPixelSize(&PixelWidth, &PixelHeight);
		offset.x += dx * PixelWidth;
		offset.y -= dy * PixelHeight;
			
		WorkRect wrScrollRect = GetMaxScrollRect();
		if (offset.x < wrScrollRect.lo.x)	offset.x = wrScrollRect.lo.x;
		if (offset.y < wrScrollRect.lo.y)	offset.y = wrScrollRect.lo.y;
		if (offset.x > wrScrollRect.hi.x)	offset.x = wrScrollRect.hi.x;
		if (offset.y > wrScrollRect.hi.y)	offset.y = wrScrollRect.hi.y;

		// By calling DocView to do the scroll we give it a chance to remove
		// any blobbies it might have put on the screen.  Note that the scrollers
		// will prevent any overscroll.
		pDocView->SetScrollOffsets(offset, TRUE);
	}
}




/*********************************************************************************************
>	void ScreenView::CalcScrollDeltas(wxPoint point, WinRect wrSize, INT32* dx, INT32* dy)

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
	SeeAlso:	ScreenView::HandleDragScrolling

**********************************************************************************************/ 

void ScreenView::CalcScrollDeltas(wxPoint point, WinRect wrSize, INT32* dx, INT32* dy)
{
	if( point.x < wrSize.GetLeft() )
		*dx = point.x - wrSize.GetLeft();
	else
	if( point.x >= wrSize.GetRight() )
		*dx = point.x - wrSize.GetRight();
	else
		*dx = 0;

	if( point.y < wrSize.GetTop() )
		*dy = point.y - wrSize.GetTop();
	else
	if( point.y >= wrSize.GetBottom() )
		*dy = point.y - wrSize.GetBottom();
	else
		*dy = 0;
}




/*********************************************************************************************

>	virtual void ScreenView::HandleOleDragOutOfView(wxPoint point);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/96

	Purpose:	Called when doing a DRAGTYPE_OLESCROLL and the mouse pointer moves outside
				the view window. The base class does nothing, so dragging continues as normal,
				but in the derived ScreenCamView class, this is used to cancel the internal
				XOR drag operation, and start an OLE drag and drop export procedure.

	Notes:		May cancel the current Drag Op

	Scope:		Protected
	SeeAlso:	ScreenCamView::HandleOleDragOutOfView

**********************************************************************************************/ 

void ScreenView::HandleOleDragOutOfView(wxPoint point)
{
	// The base class does nothing - this should keep Ralph happy
}




/*********************************************************************************************
>	afx_msg void ScreenView::OnLButtonDown(UINT32 nFlags, wxPoint point)

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
	SeeAlso:	class CRendWnd; ScreenView::HandleDragEvent()

**********************************************************************************************/ 

void ScreenView::OnLButtonDown( wxMouseEvent &event )
{
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
	if( ClickBounds.Inside( event.GetPosition() ) && ( TimeDelay <= (INT32)ClickGap ) )
		ThisSingleIsTriple = TRUE;
	else
		ThisSingleIsTriple = FALSE;

	// Deal with the click
	HandleDragEvent( MK_LBUTTON, event, CLICKTYPE_SINGLE );

	TimeOfLastClick.Sample();
}

/*********************************************************************************************
>	afx_msg void ScreenView::OnMButtonDown(UINT32 nFlags, wxPoint point)

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
	SeeAlso:	class CRendWnd; ScreenView::HandleDragEvent()

**********************************************************************************************/ 

void ScreenView::OnMButtonDown( wxMouseEvent &event )
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
// WEBSTER - markn 25/4/97
// No pen stuff required in Webster
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
	(Camelot.GetPressurePen())->CheckMouseMessage(WM_MBUTTONDOWN, point);
#endif // VECTOR_STROKING
#endif
	HandleDragEvent(MK_MBUTTON, event, CLICKTYPE_SINGLE);
}

/*********************************************************************************************
>	afx_msg void ScreenView::OnRButtonDown(UINT32 nFlags, wxPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    Flags describing which buttons & keys were down when the message was sent;
				the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Same as for ScreenView::OnLButtonDown(), but for the right mouse button.
	Errors:		-
	Scope:		Protected
	SeeAlso:	class CRendWnd; ScreenView::HandleDragEvent(); ScreenView::OnLButtonDown()

**********************************************************************************************/ 

void ScreenView::OnRButtonDown( wxMouseEvent &event )
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
// WEBSTER - markn 25/4/97
// No pen stuff required in Webster
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
	(Camelot.GetPressurePen())->CheckMouseMessage( WM_RBUTTONDOWN, event.GetPosition() );
#endif // VECTOR_STROKING
#endif
	HandleDragEvent(MK_RBUTTON, event, CLICKTYPE_SINGLE);
}

/*********************************************************************************************
>	afx_msg void ScreenView::OnMouseWheel(UINT32 nFlags, short zDelta, wxPoint pt)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/2000
	Purpose:	
	Scope:		Protected

**********************************************************************************************/ 

void ScreenView::OnMouseWheel( wxMouseEvent &event )
{
	HandleMouseWheelEvent( event );
}

/*********************************************************************************************
>	afx_msg void ScreenView::OnMouseMove(UINT32 nFlags, wxPoint point)

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
	SeeAlso:	class CRendWnd; ScreenView::HandleDragEvent(); DocView::DragPointerMove();
				ScreenView::GetClickMods(); ScreenView::HandleDragScrolling();
				DocView::OnMouseMove()
**********************************************************************************************/ 

void ScreenView::OnMouseMove( wxMouseEvent &event )
{
	// Ignore if system is disabled
	if (CCamApp::IsDisabled())
		return;						     	// If the system is disabled, ignore
	
	// Only respond to real mouse move messages
	if (event.GetPosition()==CurrentMousePos)
		return;
	
	// Ignore mouse moves if we're not the foreground window.
	if (pDocView != DocView::GetSelected())
	{
		return;
	}

	UINT32 nFlags = ClickModifiers::SynthesizeMouseEventFlags(event);

//PORTNOTE("other","Remove StatusLine related code")
//#ifndef EXCLUDE_FROM_XARALX
	StatusLine* pStatusLine=NULL;
	// flag that the StatusLine's mouse position field needs updating
#ifndef RALPH

	pStatusLine=GetApplication()->GetpStatusLine();
	if (pStatusLine)
		pStatusLine->FlagMousePosNeedsUpdating();
#endif
//#endif
	SetCurrentStates();

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
		if (GetCanvas()->GetCapture() == GetCanvas())
		{
			WinCoord	point;
			event.GetPosition( &point.x, &point.y );

			// Send an on-going drag event to the kernel.
			pDocView->DragPointerMove(pCurrentDragOp, point.ToOil(pDocView, TRUE), LastClickMods);

			// Check if we need to scroll this view.
			HandleDragScrolling(point);
		}
	}
	else
	{
		// We are not dragging at the moment, but are we about to start a drag?  If the
		// last mouse event was a single click, the same buttons are down, and the mouse
		// has either moved (3 or more), then a drag is beginning.
		WinCoord point;
		event.GetPosition( &point.x, &point.y );

		INT32	dx  = point.x - LastClickPoint.x;
		INT32	dy  = point.y - LastClickPoint.y;
		INT32	dz2 = (dx * dx) + (dy * dy);
		if ((nFlags & FirstClickButton) && (dz2 >= (DragLatency * DragLatency)))
		{
			// Inform the kernel that a drag is starting.
			wxMouseEvent KernelEvent( event );
			KernelEvent.m_x = LastClickPoint.x;
			KernelEvent.m_y = LastClickPoint.y;
			HandleDragEvent(FirstClickButton, KernelEvent, CLICKTYPE_DRAG);
		}
		else
		{
			// We are not dragging, just moving the mouse around, so pass the position 
			// of the mouse to the tool, giving it an opportunity to set the cursor etc.
			pDocView->OnMouseMove( point.ToOil(pDocView, TRUE), LastClickMods);		
		}
	}

	// Remember the last position of the mouse passed on to the kernel.  The position
	// passed on any subsequent DragPointerIdle events must be the same as this!
	CurrentMousePos = event.GetPosition();

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
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


/********************************************************************************************
>	BOOL ScreenView::GetCurrentMousePos(OilCoord* pMousePos) const

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

BOOL ScreenView::GetCurrentMousePos(OilCoord* pMousePos) const
{
	// Get the Windows screen coordinates of the mouse cursor.
	// Convert from screen to window coordinates.
	WinCoord pt;
	*(wxPoint *)&pt = GetCanvas()->ScreenToClient( ::wxGetMousePosition() );

	// If the mouse is outside the view then we can do nothing more.
	if (!CurrentSize.Inside(pt)) return FALSE;

	// Convert to OIL coordinates.
	if (pMousePos!=NULL)
		*pMousePos = pt.ToOil(pDocView, TRUE);

	// We stored something useful.
	return TRUE;
}



/*********************************************************************************************
>	afx_msg void ScreenView::OnKeyDown(UINT32 nChar, UINT32 nRepCnt, UINT32 nFlags)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:	    Vitual key code of the key that was pressed.
	Outputs:	-
	Returns:	-
	Purpose:	Called when a key is pressed, so we can do something with it.
				It is currently only used to adjust the Pretend Pen Pressure.
				THIS CODE HAS BEEN COMMENTED OUT UNTIL THE NEW KEYBOARD SYSTEM IS FULLY
				OPERATIONAL!!
	Errors:		-
	Scope:		Protected
	SeeAlso:	ScreenView::OnJoystickMove
**********************************************************************************************/ 
/*
void ScreenView::OnKeyDown(UINT32 nChar, UINT32 nRepCnt, UINT32 nFlags)
{
	switch (nChar)
	{
		case CamKey_IncPressure:								// Defined in 'pen.h'
			Camelot.GetPressurePen()->IncreasePenPressure();
			break;

		case CamKey_DecPressure:
			Camelot.GetPressurePen()->DecreasePenPressure();	
			break;
	};
}
*/


/*********************************************************************************************
>	afx_msg void ScreenView::OnTimer(UINT32 nIDEvent)

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
	SeeAlso:	ScreenView::HandleDragEvent(); DocView::DragPointerMove();
				ScreenView::GetClickMods();

**********************************************************************************************/ 

PORTNOTE("other","Removed ScreenView::OnTimer usage")
#ifndef EXCLUDE_FROM_XARALX
void ScreenView::OnTimer(UINT32)
{

	// Ignore if system is disabled
	if (CCamApp::IsDisabled())
		return;						     	// If the system is disabled, ignore

	// This window should never get a timer event that does not occur between calls to
	// StartDrag() and EndDrag().
//	ENSURE(pCurrentDragOp != 0, "ScreenView::StartDrag: pCurrentDragOp is null");

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
		if (GetCanvas()->GetCapture() == GetCanvas())
		{
			// Check if we need to scroll the screen . . .
			HandleDragScrolling(CurrentMousePos);

			// The above call may have aborted the current DragOp, so we must check if it is NULL
			if (pCurrentDragOp != NULL)
			{
				// Now "fake" a mouse event for the kernel . . .
				GetClickMods(nFlags, &LastClickMods);
				pDocView->DragPointerIdle(pCurrentDragOp, 
										  ((WinCoord*)&CurrentMousePos)->ToOil(pDocView, TRUE),
										  LastClickMods);
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
				GetCanvas()->SetCapture();
				OLERecaptureMouse = FALSE;		// Only capture it back once!
			}
		}
	}
}
#endif

/*********************************************************************************************

>	void ScreenView::RecaptureMouseOnNextUpdate(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?/10/96

	Purpose:	Called by the OLE drag & drop system (CCamDropSource::QueryContinueDrag,
				winoil\dataobj.cpp) when dragging a selection - if the mouse pointer goes
				outside the view, the selection drag is "paused", and an OLE drag is started.
				If the pointer comes back into our view, we cancel the OLE drag and resume
				the selector drag - the only thing we need to do to resume is to reclaim
				the mouse capture (but we must do it on the next idle to ensure that
				we get & keep the capture, so we flag it here and fix it up in OnTimer, above)

				The other thing we must now do is to XOR the dragged objects once to restore
				them to screen (as we now hide the XOR when you drag out of the window)

**********************************************************************************************/ 

void ScreenView::RecaptureMouseOnNextUpdate(void)
{
	// Flag the fact that we must recapture the mouse
	OLERecaptureMouse = TRUE;

	Spread *pSpread = pDocView->FindEnclosingSpread(((WinCoord*)&CurrentMousePos)->ToOil(pDocView, TRUE));

	if (pSpread != NULL)
	{
		// Get the DocView's viewport, in Spread Coords, because we have to supply a DocRect to
		// this stupid routine which won't let us pass a NULL pointer in to just redraw
		// it all, depsite the fact that all the functions it calls will allow it. Grrr.
		// (And using the Spread's bounding rect doesn't work because its bounding rect is in
		// chapter coords, not spread coords. Tread carefully...
		DocRect ClipRect = pDocView->GetDocViewRect(pSpread);
		pSpread->DocCoordToSpreadCoord(&ClipRect);

		if (pCurrentDragOp != NULL)
			pCurrentDragOp->RenderDragBlobs(ClipRect, pSpread, pDocView->GetCurrentSolidDragState());
	}
}



/*********************************************************************************************
>	afx_msg void ScreenView::OnCancelMode()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22nd Sept 1993
	Inputs:	    -
	Outputs:	-
	Returns:	-
	Purpose:	Called by the system whenever this window loses the focus to a dialogue box
				etc.  Checks if the window is in drag mode, if it is makes sure that the
				drag is tidily cancelled, eg. the mouse is uncaptured and its clipping area
				restored.  Calls DocView::DragFinished() (WITH JUNK PARAMETERS FOR THE MOUSE
				POSITION/BUTTONS) to indicate that the drag has been cancelled.
	Errors:		-
	Scope:		Protected
	SeeAlso:    DocView::DragFinished(); ScreenView::EndDrag()

**********************************************************************************************/ 

PORTNOTE("other","Removed ScreenView::OnCancelMode usage")
#ifndef EXCLUDE_FROM_XARALX
void ScreenView::OnCancelMode()
{
	SetCurrentStates();
	// If the system is disabled all messages should be ignored 
	if (!pCurrentDragOp) return;

	// Make sure the capture is released.
	::ReleaseCapture();

	if (!CCamApp::IsDisabled())
		pDocView->DragFinished(pCurrentDragOp, OilCoord(0, 0), LastClickMods, FALSE);

//	EndDrag(pCurrentDragOp);
}
#endif


/*********************************************************************************************
>	afx_msg void ScreenView::OnLButtonUp(UINT32 nFlags, wxPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    Flags describing which buttons & keys were down when the message was sent;
				the coordinates of the mouse cursor
	Outputs:	-
	Returns:	-
	Purpose:	Passes the event on to HandleButtonUp()
	Errors:		-
	Scope:		Protected
	SeeAlso:    class CRendWnd; ScreenView::OnLButtonDown(); ScreenView::GetClickMods();
				DocView::DragFinished(); ScreenView::HandleButtonUp()

**********************************************************************************************/ 

void ScreenView::OnLButtonUp(wxMouseEvent &event)
{
	HandleButtonUp(MK_LBUTTON, event);
}




/*********************************************************************************************
>	afx_msg void ScreenView::OnMButtonUp(UINT32 nFlags, wxPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    Flags describing which buttons & keys were down when the message was sent;
				the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Passes the event on to HandleButtonUp().
	Errors:		-
	Scope:		Protected
	SeeAlso:    class CRendWnd; ScreenView::OnMButtonDown(); ScreenView::GetClickMods();
				DocView::DragFinished(); ScreenView::HandleButtonUp()

**********************************************************************************************/ 

void ScreenView::OnMButtonUp(wxMouseEvent &event)
{
	HandleButtonUp(MK_MBUTTON, event);
}




/*********************************************************************************************
>	afx_msg void ScreenView::OnRButtonUp(UINT32 nFlags, wxPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Flags describing which buttons & keys were down when the message was sent;
				the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Same as ScreenView::OnLButtonUp(), but for the right mouse button.
	Errors:		-
	Scope:		Protected
	SeeAlso:	class CRendWnd; ScreenView::OnRButtonDown(); ScreenView::GetClickMods();
				DocView::DragFinished(); ScreenView::HandleButtonUp()

**********************************************************************************************/ 

void ScreenView::OnRButtonUp( wxMouseEvent &event )
{	
	HandleButtonUp(MK_RBUTTON, event);
} 




/*********************************************************************************************
>	afx_msg void ScreenView::OnLButtonDblClk(UINT32 nFlags, wxPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    Flags describing which buttons & keys were down when the message was sent;
				the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Passes a left-button double-click to ScreenView::HandleDragEvent().
	Errors:		-
	Scope:		Protected
	SeeAlso:	class CRendWnd; ScreenView::HandleDragEvent()

**********************************************************************************************/ 

void ScreenView::OnLButtonDblClk(wxMouseEvent &event)
{
	// Support quad clicks
	wxRect ClickBounds(	LastDoubleClickPos.x-LocalEnvironment::GetXMouseDoubleClickMove(),
						LastDoubleClickPos.y-LocalEnvironment::GetYMouseDoubleClickMove(),
						LastDoubleClickPos.x+LocalEnvironment::GetXMouseDoubleClickMove(),
						LastDoubleClickPos.y+LocalEnvironment::GetYMouseDoubleClickMove());
	MonotonicTime ThisClickTime( event.GetTimestamp() );
	MonotonicTime ClickGap(LocalEnvironment::GetMouseDoubleClickDelay());
	// Is click within allowed movement rectangle and permitted time delay
	INT32 TimeDelay = (ThisClickTime - TimeOfLastClick);
	if (ClickBounds.Inside( event.GetPosition() ) && (TimeDelay <= (INT32)ClickGap))
		ThisDoubleIsQuad = TRUE;
	else
		ThisDoubleIsQuad = FALSE;

	HandleDragEvent(MK_LBUTTON, event, CLICKTYPE_DOUBLE);

	TimeOfLastClick.Sample();
	LastDoubleClickPos = event.GetPosition();
}




/*********************************************************************************************
>	afx_msg void ScreenView::OnMButtonDblClk(UINT32 nFlags, wxPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    Flags describing which buttons & keys were down when the message was sent;
				the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Passes a middle-button double-click to ScreenView::HandleDragEvent().
	Errors:		-
	Scope:		Protected
	SeeAlso:	class CRendWnd; ScreenView::HandleDragEvent()

**********************************************************************************************/ 

void ScreenView::OnMButtonDblClk(wxMouseEvent &event)
{
	HandleDragEvent(MK_MBUTTON, event, CLICKTYPE_DOUBLE);
}




/*********************************************************************************************
>	afx_msg void ScreenView::OnRButtonDblClk(UINT32 nFlags, wxPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Flags describing which buttons & keys were down when the message was sent;
				the coordinates of the mouse cursor
	Outputs:	-
	Returns:	-
	Purpose:	Same as ScreenView::OnLButtonDblClk(), bit for the right button.
	Errors:		-
	Scope:		Protected
	SeeAlso:	class CRendWnd; ScreenView::OnLButtonDblClk()

**********************************************************************************************/ 

void ScreenView::OnRButtonDblClk( wxMouseEvent &event )
{
	HandleDragEvent(MK_RBUTTON, event, CLICKTYPE_DOUBLE);
}



/********************************************************************************************
>	void ScreenView::OnDropFiles(HDROP hDropInfo)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/94
	Inputs:		hDropInfo - handle to information about drag event.
	Purpose:	Respond to files being dropped onto the view window.  It sets up the
				current document and docview, and passes control on to the OpDroppedFile
				operation.
	SeeAlso:	OpDroppedFile
********************************************************************************************/

PORTNOTE("other","Removed ScreenView::OnDropFiles usage")
#ifndef EXCLUDE_FROM_XARALX
void ScreenView::OnDropFiles(HDROP hDropInfo)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
#ifdef STANDALONE
	// Force Viewer to 'Load' rather than Import
	GetMainFrame()->OnDropFiles(hDropInfo);
#else
	// Set the correct docivew (and hence document)
	pDocView->SetCurrent();
	
	// Invoke the dropped files operation
	OpDescriptor *pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_DROPPEDFILE);
	pOpDesc->Invoke((OpParam *) hDropInfo);

	// Clean up
	DragFinish(hDropInfo);
#endif
#endif
}
#endif



/*********************************************************************************************

>	void ScreenView::ClearClickState()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/02/2004

	Purpose:	Clear the current click state so that any logic in progress is stopped before
				it does anything nasty.

**********************************************************************************************/ 

void ScreenView::ClearClickState()
{
	LastClickType = CLICKTYPE_NONE;
	if( GetParentFrame()->HasCapture() )
	{
		GetParentFrame()->ReleaseMouse();
	}
}


/////////////////////////////////////////////////////////////////////////////
// Menu functions


/*********************************************************************************************
>	void ScreenView::OnFilePrintPreview()

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

void ScreenView::OnFilePrintPreview()
{
	SetCurrentStates();

	CCamView::OnFilePrintPreview();
}




/*********************************************************************************************
>	DocView* ScreenView::GetDocViewPtr() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to the DocView object associated with this CCamView.
	Purpose:	Used by Will's rendering system - somehow.
	Errors:		-
	Scope:		Public
	SeeAlso:	Will's stuff.

**********************************************************************************************/ 

DocView* ScreenView::GetDocViewPtr() const
{
	return pDocView;
}





/////////////////////////////////////////////////////////////////////////////
// OLE container/server support and commands

/*********************************************************************************************
>	virtual BOOL ScreenView::IsSelected(const CObject* pDocItem) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		A constant pointer to a CObject which is a candidate for some
				OLE action.
	Outputs:	-
	Returns:	TRUE if the parameter is selected in this view.
	Purpose:    "Tests for a selected OLE client item".
	Errors:		-
	Scope:		Public
	SeeAlso:	ScreenView::OnInsertObject()

**********************************************************************************************/ 

BOOL ScreenView::IsSelected(const CObject* pDocItem) const
{
	// TODO: implement this function that tests for a selected OLE client item

#if (_OLE_VER >= 0x200)
	// This implementation is adequate if your selection only consists of CCamCntrItems.
	// To handle different selection mechanisms, this implementation should be changed.
	return pDocItem == m_pSelection;
#else
	return FALSE;
#endif
}



/*********************************************************************************************
>	afx_msg void ScreenView::OnInsertObject()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:	    -
	Outputs:	-
	Returns:	-
	Purpose:	"Creates an embedded OLE object with a type-name retrieved
				from the user" - currently unimplemented.
	Errors:		-
	Scope:		Protected
	SeeAlso:	ScreenView::IsSelected()

**********************************************************************************************/ 

PORTNOTE("other","Removed ScreenView::OnInsertObject usage")
#ifndef EXCLUDE_FROM_XARALX
void ScreenView::OnInsertObject()
{

	SetCurrentStates();


#if (_OLE_VER >= 0x200)

	// OLE 2.0+ default implementation.
	// Invoke the standard Insert Object dialog box to obtain information for a new
	// CCamCntrItem object.
	COleInsertDialog dlg;
	if (dlg.DoModal() != IDOK) return;

	BeginBusyCursor();

	CCamCntrItem* pItem = NULL;
	TRY
	{
		// Create new item connected to this document.
		CCamDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pItem = new CCamCntrItem(pDoc);
		ASSERT_VALID(pItem);

		// Initialize the item from the dialog data.
		if (!dlg.CreateItem(pItem))
			AfxThrowMemoryException();  // any exception will do
		ASSERT_VALID(pItem);

		// If item created from class list (not from file) then launch
		//  the server to edit the item.
		if (dlg.GetSelectionType() == COleInsertDialog::createNewItem)
			pItem->DoVerb(OLEIVERB_SHOW, this);

		ASSERT_VALID(pItem);

		// As an arbitrary user interface design, this sets the selection
		//  to the last item inserted.

		// TODO: reimplement selection as appropriate for your application

		m_pSelection = pItem;   // set selection to last inserted item
		pDoc->UpdateAllViews(NULL);
	}

	CATCH(CException, e)
	{
		if (pItem != NULL)
		{
			ASSERT_VALID(pItem);
			pItem->Delete();
		}
		AfxMessageBox(_R(IDP_FAILED_TO_CREATE));
	}
	END_CATCH

	EndBusyCursor();

#else

	// OLE 1.0 default implementation.
	wxString			strTypeName;

#if (_MFC_VER < 0x300)
	if (!AfxOleInsertDialog(strTypeName))
		return;		// no OLE class selected
#endif

	TRACE( _T("Trying to Insert OLE item with type '%s'\n"), (const char*)strTypeName);

	// TODO: create an embedded OLE object with that class name
	char szT[300];
	wsprintf(szT, "Listen pal - don't do that again!  This "
		"particular function is not completely implemented.\n"
		"You must add code to create a COleClientItem with a "
		"type name of '%s'.  See ScreenView::OnInsertObject in\n"
		"CAMVW.CPP for more info.", (LPCSTR)strTypeName);
	AfxMessageBox(szT);

#endif
}
#endif


/*********************************************************************************************
>	afx_msg void ScreenView::OnCancelEdit()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com> (AppWizard 2.5)
	Created:	19 Jan 94
	Inputs:	    -
	Outputs:	-
	Returns:	-
	Purpose:	"The following command handler provides the standard keyboard user interface
				to cancel an in-place (OLE 2.0) editing session."
	Errors:		-
	Scope:		Protected
	SeeAlso:	ScreenView::IsSelected()

**********************************************************************************************/

#if (_OLE_VER >= 0x200)

PORTNOTE("other","Removed ScreenView::OnCancelEdit usage")
#ifndef EXCLUDE_FROM_XARALX
void ScreenView::OnCancelEdit()
{
	SetCurrentStates();
	if (GetDocument())
	{
		// Close any in-place active item on this view.
		COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
		if (pActiveItem != NULL)
		{
			pActiveItem->Close();
		}

		ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
	}
}
#endif
#endif




/*********************************************************************************************
>	afx_msg void ScreenView::OnSetFocus(wxWindow* pOldWnd)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com> (AppWizard 2.5)
	Created:	19 Jan 94
	Inputs:	    -
	Outputs:	-
	Returns:	-
	Purpose:	"Special handling of OnSetFocus is required for a container when an object
				is being edited in-place (OLE 2.0)."
	Errors:		-
	Scope:		Protected
	SeeAlso:	ScreenView::IsSelected()

**********************************************************************************************/

#if (_OLE_VER >= 0x200)

void ScreenView::OnSetFocus(wxWindow* pOldWnd)
{
	SetCurrentStates();
	if (GetDocument())
	{
		COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
		if (pActiveItem && (pActiveItem->GetItemState() == COleClientItem::activeUIState))
		{
			// need to set focus to this item if it is in the same view
			wxWindow* pWnd = pActiveItem->GetInPlaceWindow();
			if (pWnd)
			{
				pWnd->SetFocus();   // don't call the base class
				return;
			}
		}
		
		CCamView::OnSetFocus(pOldWnd);
	}
}

#endif


/********************************************************************************************
>	afx_msg LRESULT ScreenView::OnDescribeView(WPARAM, LPARAM);

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/94
	Inputs:		(not used)
	Outputs:	-
	Returns:	0 (not used)
	Purpose:	Dumps some vital statistics about this ScreenView in response to the
				privately-defined WM_DESCRIBEVIEW message, sent by double-clicking on
				a view's bottom-right corner gadget.  Only included in debug builds.
	Errors:		-
	SeeAlso:	CScrollerCorner::OnRBtnDblClick; CCamMDIChild::OnNcRBtnDblClk
********************************************************************************************/

#ifdef _DEBUG

PORTNOTE("other","Removed ScreenView::OnDescribeView usage")
#ifndef EXCLUDE_FROM_XARALX
LRESULT ScreenView::OnDescribeView(WPARAM, LPARAM)
{
	SetCurrentStates();

	// Report the connexions to other objects.
	TRACE( _T("\nScreenView at address 0x%lX (1 of %d)\n"), (UINT32) this, HowMany());
	TRACE( _T("\tAttached to CCamDoc     at 0x%lX\n"), (UINT32) GetDocument());
	TRACE( _T("\t            ViewState   at 0x%lX\n"), (UINT32) Status);
	TRACE( _T("\t            DocView     at 0x%lX\n"), (UINT32) pDocView);
	TRACE( _T("\t            Document    at 0x%lX\n"), (UINT32) pDocView->GetDoc());
	TRACE( _T("\t			FrameWnd    at 0x%lX\n"), (UINT32) GetParentFrame());
	TRACE( _T("\t			RenderWnd   at 0x%lX\n\n"), (UINT32) RenderWindow);

	// Report window handles, to help when using Spy.
	TRACE( _T("\tHandles:    ScreenView	   0x%lX\n"), (UINT32) m_hWnd);
	TRACE( _T("\t            CCamMDIChild   0x%lX\n"), (UINT32) (GetParentFrame()->m_hWnd));
	TRACE( _T("\t            CRenderWnd	   0x%lX\n"), (UINT32) RenderWindow->m_hWnd);

	// Report the "placement" of the frame window, in pixels.
	WINDOWPLACEMENT wp;
	wp.length = sizeof(wp);
    GetParentFrame()->GetWindowPlacement(&wp);

	const TCHAR* pcsz;
	switch (wp.showCmd)
	{
		case SW_SHOWNORMAL:
			pcsz = TEXT("normal");
			break;

		case SW_SHOWMAXIMIZED:
			pcsz = TEXT("maximised");
			break;

		case SW_SHOWMINIMIZED:
			pcsz = TEXT("minimised");
            break;

		default:
			pcsz = TEXT("some weird");
			break;
	}

	TRACE( _T("\n\tFrame is in %s state.\n"), pcsz);
	TRACE( _T("\tIcon      at (%d, %d)\n"), wp.ptMinPosition.x, wp.ptMinPosition.y);
	TRACE( _T("\tNormally  at (%d, %d) --> (%d, %d)\n"),
		  wp.rcNormalPosition.left, wp.rcNormalPosition.top,
		  wp.rcNormalPosition.right, wp.rcNormalPosition.bottom);
	TRACE( _T("\tMaximised at (%d, %d)\n\n"), wp.ptMaxPosition.x, wp.ptMaxPosition.y);
	if(GetDocument())
	{
		// Report the document flags.
		CCamDoc* pDoc = (CCamDoc*) GetDocument();
		TRACE( _T("\nDocument flags:-\n"));
		TRACE( _T("\tAuto-deletable:  is %s\n", (pDoc->IsUntouched() ? "TRUE" : "FALSE")));
		TRACE( _T("\tModified:        is %s\n", (pDoc->IsModified() ? "TRUE" : "FALSE")));
		TRACE( _T("\tRead Only:       is %s\n", (pDoc->IsReadOnly() ? "TRUE" : "FALSE")));
		TRACE( _T("\tCopy:            is %s\n\n", (pDoc->IsACopy() ? "TRUE" : "FALSE")));
	}
	// Now dump out the ViewState, if there is one.
	if (Status != NULL)
	{
		Status->Dump();
	}
	else
	{
		TRACE( _T("There is no ViewState attached to this ScreenView!\n"));
	}

	return 0;
}
#endif
#endif	// _DEBUG


#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)

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
ViewDragTarget::ViewDragTarget(HWND TheWindow, CRect *ClientArea, DragInformation* pDragInfo)
		:WinoilDragTarget(TheWindow,ClientArea )
{
	pThisDoc =  ScreenView::GetDocViewFromHwnd(TheWindow);
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
	}
	return FALSE;
}

/********************************************************************************************

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/95		  
	Purpose:	The framework calls this member function when the user clicks a window's 
				horizontal scroll bar.

********************************************************************************************/

void ScreenView::OnHScroll( UINT32 nSBCode, UINT32 nPos, CScrollBar*)
{
	if(HScrollBar)	
	{	
		HScrollBar->HandleScrollMessage(nSBCode, nPos);
	}
}

/********************************************************************************************

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/95		  
	Purpose:	The framework calls this member function when the user clicks a window's 
				vertical scroll bar.

********************************************************************************************/

void ScreenView::OnVScroll( UINT32 nSBCode, UINT32 nPos, CScrollBar*)
{
	if(VScrollBar)
	{
		VScrollBar->HandleScrollMessage(nSBCode, nPos);
	}
}


#endif // EXCLUDE_FROM_RALPH, EXCLUDE_FROM_XARALX
