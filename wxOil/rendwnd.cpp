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
//
// The CRendWnd class implementation.
//


#include "camtypes.h"
#include "rendwnd.h"
#include "camelot.h"							// for IsWin32s()
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "palman.h"
#include "ccdc.h"
//#include "prncamvw.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "camview.h"
#include "camframe.h"
#include "keypress.h"

DECLARE_SOURCE("$Revision$");

#if 0
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
#endif

CC_IMPLEMENT_DYNCREATE(OpToggleDoubleBuffer, Operation)

IMPLEMENT_DYNAMIC_CLASS( CRenderWnd, wxWindow )

BEGIN_EVENT_TABLE( CRenderWnd, wxWindow )
	EVT_PAINT(				CRenderWnd::OnPaint )			
	EVT_ERASE_BACKGROUND(	CRenderWnd::OnErase )
	EVT_LEFT_DOWN(			CRenderWnd::OnLButtonDown )
	EVT_LEFT_DCLICK(		CRenderWnd::OnLButtonDblClk )
	EVT_LEFT_UP(			CRenderWnd::OnLButtonUp )
	EVT_MIDDLE_DOWN(		CRenderWnd::OnMButtonDown )
	EVT_MIDDLE_DCLICK(		CRenderWnd::OnMButtonDblClk )
	EVT_MIDDLE_UP(			CRenderWnd::OnMButtonUp )
	EVT_RIGHT_DOWN(			CRenderWnd::OnRButtonDown )
	EVT_RIGHT_DCLICK(		CRenderWnd::OnRButtonDblClk )
	EVT_RIGHT_UP(			CRenderWnd::OnRButtonUp )
	EVT_MOTION(				CRenderWnd::OnMouseMove )
	EVT_MOUSEWHEEL(			CRenderWnd::OnMouseWheel )
	EVT_SIZE(				CRenderWnd::OnSize)
	EVT_SET_CURSOR(			CRenderWnd::OnSetCursor )
	EVT_KEY_DOWN(			CRenderWnd::OnKey)
	EVT_KEY_UP(				CRenderWnd::OnKey)
	EVT_CHAR(				CRenderWnd::OnChar)
	
#if defined(__WXGTK__)
	EVT_ENTER_WINDOW(		CRenderWnd::OnEnter )
	EVT_LEAVE_WINDOW(		CRenderWnd::OnLeave )
#endif
END_EVENT_TABLE()

BOOL CRenderWnd::m_DoubleBuffer = FALSE;

CRenderWnd::CRenderWnd(CCamView* pView) :
	m_pView(pView), m_pCCClientDC(NULL)
{
	// Nothing else to do for now...
}

CRenderWnd::~CRenderWnd()
{
	TRACEUSER("Gerry", _T("Deleting CRenderWnd at 0x%08x\n"), this);
	if (m_pCCClientDC)
	{
		delete(m_pCCClientDC);
		m_pCCClientDC = NULL;
	}
}

/*********************************************************************************************
>	virtual wxClientDC * CRenderWnd::GetClientDC()

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	12/06/2006
	Inputs:		None
	Outputs:	None
	Returns:	Pointer to the CCClientDC
	Purpose:	Returns a pointer to the appropriate client DC, allocating it if necessary
	Errors:		-
	Scope:	    Public
	SeeAlso:    CCamView::OnCreate()

**********************************************************************************************/ 

wxClientDC * CRenderWnd::GetClientDC()
{
	if (!m_pCCClientDC)
		m_pCCClientDC = new CCClientDC(this); // OK if it fails
	return (wxClientDC*)(m_pCCClientDC?m_pCCClientDC->GetDC():NULL);
}

/*********************************************************************************************
>	virtual wxClientDC * CRenderWnd::GetClientDC()

>	void CCamView::DoneWithDC()

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	12/06/2006
	Purpose:	Hints that we've done with our DC
	SeeAlso:	View; PaperRenderRegion.

Note this is merely a hint. This routine is not guaranteed to eb called

**********************************************************************************************/ 

void CRenderWnd::DoneWithDC()
{
	if (m_pCCClientDC)
	{
		delete m_pCCClientDC;
		m_pCCClientDC=NULL;
	}
}

/*********************************************************************************************
>	virtual BOOL CRenderWnd::Create(const wxRect& rect,
									wxWindow* parent, UINT32 id)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Windows instance-style flags; a
				rectangle describing the position of the scroller; a pointer to its
				parent window; a child window numeric identifier
	Outputs:	-
	Returns:	TRUE if the window is successfully created.
	Purpose:	Registers a new window class with the operating system, which accepts
				double clicks, is byte-aligned in video RAM and is responsible for
				drawing is own backgrounds.
	Errors:		-
	Scope:	    Public
	SeeAlso:    CCamView::OnCreate()

**********************************************************************************************/ 
BOOL CRenderWnd::Create(const wxRect& rect,
						wxWindow *pParent, UINT32 id)
{
	BOOL ok=wxWindow::Create(pParent, id, rect.GetTopLeft(), rect.GetSize(), wxNO_FULL_REPAINT_ON_RESIZE);
#if defined(__WXGTK__)
	::SetDoubleBuffer(this, m_DoubleBuffer);
#endif
	return ok;
}



/////////////////////////////////////////////////////////////////////////////
// CRenderWnd message handlers.

/*********************************************************************************************
>	afx_msg void CRenderWnd::OnPaint()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a CPaintDC and sends a private WM_RENDERVIEW message, together with
				a pointer to the CPaintDC in the LPARAM, to the parent CCamView. Uses a
				CCPaintDC so we can get to the rectangle list subsequently.
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CCamView::OnRenderView()

**********************************************************************************************/ 
void CRenderWnd::OnPaint( wxPaintEvent &evnt )
{
#ifndef EXCLUDE_FROM_XARALX
	if ((!CCamApp::IsDisabled()) && (!PrintMonitor::IsPrintingNow()))
#else
	// print monitor not present on standalone versions
	if (!CCamApp::IsDisabled())
#endif
	{
		CCPaintDC dc(this);
//		wxPalette* OldPal = PaletteManager::StartPaintPalette(&dc);

		if (m_pView)
			m_pView->OnDraw(dc.GetDC());

//		PaletteManager::StopPaintPalette(&dc, OldPal);
	}
	else
	{
#ifndef EXCLUDE_FROM_XARALX
		// if we were printing, we have to draw something - white will do
		if (PrintMonitor::IsPrintingNow())
		{
/*				// Draw a white rectangle here
			CPaintDC MyPaint(this);

			// Lets have a white brush...
			CBrush WhiteBrush;
			WhiteBrush.CreateStockObject(WHITE_BRUSH);
			CBrush* pOldBrush = MyPaint.SelectObject(&WhiteBrush);

			// and no pen please
			CPen NoPen;
			NoPen.CreateStockObject(NULL_PEN);
			CPen* pOldPen = MyPaint.SelectObject(&NoPen);
		
			// Draw the rect.
			CRect RedrawRect(&MyPaint.m_ps.rcPaint);
			RedrawRect.InflateRect(2,2);
			MyPaint.Rectangle(RedrawRect);

			// Put the old brush and pen back
			MyPaint.SelectObject(pOldPen);
			MyPaint.SelectObject(pOldBrush);
			
			// ask for a full redraw at the end
			PrintMonitor::WantFullRedraw(TRUE); */
		}
		else
#endif
		{
			wxPaintDC dc( this );								// Clear paint condition 
		}
	}
}



/*********************************************************************************************
>	afx_msg void CRenderWnd::OnLButtonDown(UINT32, CPoint)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Not used.
	Outputs:	-
	Returns:	-
	Purpose:	Passes the mouse message on to the parent CCamView via a call to
				CRenderWnd::PassMsgToParent().
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CRenderWnd::PassMsgToParent(); CCamView::OnLButtonDown()

**********************************************************************************************/ 
void CRenderWnd::OnLButtonDown( wxMouseEvent &evnt )
{
	if (m_pView)
		m_pView->OnLButtonDown(evnt);
}



/*********************************************************************************************
>	afx_msg void CRenderWnd::OnLButtonDblClk(UINT32, CPoint)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Not used.
	Outputs:	-
	Returns:	-
	Purpose:	Passes the mouse message on to the parent CCamView via a call to
				CRenderWnd::PassMsgToParent().
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CRenderWnd::PassMsgToParent(); CCamView::OnLButtonDblClk()

**********************************************************************************************/ 
void CRenderWnd::OnLButtonDblClk( wxMouseEvent &evnt )
{
	if (m_pView)
		m_pView->OnLButtonDblClk(evnt);
}



/*********************************************************************************************
>	afx_msg void CRenderWnd::OnLButtonUp(UINT32, CPoint)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Not used.
	Outputs:	-
	Returns:	-
	Purpose:	Passes the mouse message on to the parent CCamView via a call to
				CRenderWnd::PassMsgToParent().
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CRenderWnd::PassMsgToParent(); CCamView::OnLButtonUp()

**********************************************************************************************/ 
void CRenderWnd::OnLButtonUp( wxMouseEvent &evnt )
{
	if (m_pView)
		m_pView->OnLButtonUp(evnt);
}



/*********************************************************************************************
>	afx_msg void CRenderWnd::OnMouseMove(UINT32, CPoint)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Not used.
	Outputs:	-
	Returns:	-
	Purpose:	Passes the mouse message on to the parent CCamView via a call to
				CRenderWnd::PassMsgToParent().
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CRenderWnd::PassMsgToParent(); CCamView::OnMouseMove()

**********************************************************************************************/ 
void CRenderWnd::OnMouseMove( wxMouseEvent &event )
{
	if (m_pView)
		m_pView->OnMouseMove( event );
	
#if defined(__WXGTK__)
	wxSetCursorEvent	CursorEvent( event.m_x, event.m_y );
	OnSetCursor( CursorEvent );
	if( CursorEvent.HasCursor() )
	{
		// The window cursor seems to be ignored, but is needed for
		// CaptureMouse to work
		wxSetCursor( CursorEvent.GetCursor() );
		SetCursor( CursorEvent.GetCursor() );
	}
#endif
}

/*********************************************************************************************
>	void CRenderWnd::OnKey( wxKeyEvent & event )

	Author:		Alex Bligh (alex@alex.org.uk)
	Created:	2 May 2006
	Inputs:		reference to the event
	Outputs:	-
	Returns:	-
	Purpose:	This DOES NOT ACTUALLY HANDLE EVENTS, it only notes a key has been 
				pressed and stops the mouse motion mangler eating the next mouse move.
	Errors:		-
	Scope:	    Protected
	SeeAlso:    -

**********************************************************************************************/ 

void CRenderWnd::OnKey( wxKeyEvent & event )
{
	if (m_pView)
		m_pView->DontSkipNextMouse();
	
	event.Skip(); // Pass the key event on to someone who really wants it.
}

void CRenderWnd::OnChar( wxKeyEvent & event )
{
	TRACEUSER( "jlh92", _T("CRenderWnd::OnChar \"%c\" \n"), event.GetUnicodeKey() );

	if( !AfxGetApp().HandleKeyPress( event ) )
		event.Skip(); // Pass the key event on to someone who really wants it.
}

/*********************************************************************************************
>	afx_msg void CRenderWnd::OnRButtonDown(UINT32, CPoint)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Not used.
	Outputs:	-
	Returns:	-
	Purpose:	Passes the mouse message on to the parent CCamView via a call to
				CRenderWnd::PassMsgToParent().
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CRenderWnd::PassMsgToParent(); CCamView::OnRButtonDown()

**********************************************************************************************/ 
void CRenderWnd::OnRButtonDown( wxMouseEvent &evnt )
{
	if (m_pView)
		m_pView->OnRButtonDown(evnt);
}



/*********************************************************************************************
>	afx_msg void CRenderWnd::OnRButtonDblClk(UINT32, CPoint)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Not used.
	Outputs:	-
	Returns:	-
	Purpose:	Passes the mouse message on to the parent CCamView via a call to
				CRenderWnd::PassMsgToParent().
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CRenderWnd::PassMsgToParent(); CCamView::OnRButtonDblClk()

**********************************************************************************************/ 
void CRenderWnd::OnRButtonDblClk( wxMouseEvent &evnt )
{
	if (m_pView)
		m_pView->OnRButtonDblClk(evnt);
}



/*********************************************************************************************
>	afx_msg void CRenderWnd::OnRButtonUp(UINT32, CPoint)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Not used.
	Outputs:	-
	Returns:	-
	Purpose:	Passes the mouse message on to the parent CCamView via a call to
				CRenderWnd::PassMsgToParent().
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CRenderWnd::PassMsgToParent(); CCamView::OnRButtonUp()

**********************************************************************************************/ 
void CRenderWnd::OnRButtonUp( wxMouseEvent &evnt )
{
	if (m_pView)
		m_pView->OnRButtonUp(evnt);
}


/*********************************************************************************************
>	afx_msg void CRenderWnd::OnMButtonDown(UINT32, CPoint)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Not used.
	Outputs:	-
	Returns:	-
	Purpose:	Passes the mouse message on to the parent CCamView via a call to
				CRenderWnd::PassMsgToParent().
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CRenderWnd::PassMsgToParent(); CCamView::OnLButtonDown()

**********************************************************************************************/ 
void CRenderWnd::OnMButtonDown( wxMouseEvent &evnt )
{
	if (m_pView)
		m_pView->OnMButtonDown(evnt);
}



/*********************************************************************************************
>	afx_msg void CRenderWnd::OnMButtonDblClk(UINT32, CPoint)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Not used.
	Outputs:	-
	Returns:	-
	Purpose:	Passes the mouse message on to the parent CCamView via a call to
				CRenderWnd::PassMsgToParent().
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CRenderWnd::PassMsgToParent(); CCamView::OnLButtonDblClk()

**********************************************************************************************/ 
void CRenderWnd::OnMButtonDblClk( wxMouseEvent &evnt )
{
	if (m_pView)
		m_pView->OnMButtonDblClk(evnt);
}



/*********************************************************************************************
>	afx_msg void CRenderWnd::OnMButtonUp(UINT32, CPoint)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Not used.
	Outputs:	-
	Returns:	-
	Purpose:	Passes the mouse message on to the parent CCamView via a call to
				CRenderWnd::PassMsgToParent().
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CRenderWnd::PassMsgToParent(); CCamView::OnLButtonUp()

**********************************************************************************************/ 
void CRenderWnd::OnMButtonUp( wxMouseEvent &evnt )
{
	if (m_pView)
		m_pView->OnMButtonUp(evnt);
}


/*********************************************************************************************
>	void CRenderWnd::OnMouseWheel(wxMouseEvent &event)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/2006
	Inputs:		Not used.
	Outputs:	-
	Returns:	-
	Purpose:	Passes the mouse message on to the parent CCamView
	Errors:		-
	Scope:	    Protected
	SeeAlso:    -

**********************************************************************************************/ 

void CRenderWnd::OnMouseWheel( wxMouseEvent &evnt )
{
	if (m_pView)
		m_pView->OnMouseWheel(evnt);
}


/*********************************************************************************************
>	void CRenderWnd::OnSize( wxSizeEvent &evnt )

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/2006
	Inputs:		Not used.
	Outputs:	-
	Returns:	-
	Purpose:	Does nothing, skipping the event
	Errors:		-
	Scope:	    Protected
	SeeAlso:    -

**********************************************************************************************/ 

void CRenderWnd::OnSize( wxSizeEvent &evnt )
{
	TRACEUSER("Gerry", _T("CRenderWnd::OnSize(%d, %d)\n"), evnt.m_size.x, evnt.m_size.y);
	evnt.Skip();
}



/*********************************************************************************************
>	void CRenderWnd::OnErase( wxEraseEvent &evnt )

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/2006
	Inputs:		Not used.
	Outputs:	-
	Returns:	-
	Purpose:	Does nothing
	Errors:		-
	Scope:	    Protected
	SeeAlso:    -

**********************************************************************************************/ 
void CRenderWnd::OnErase( wxEraseEvent &evnt )
{
	// Do nothing
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

void CRenderWnd::OnSetCursor( wxSetCursorEvent& event )
{
	if( NULL != m_pView )
		m_pView->OnSetCursor( event );
}

#if defined(__WXGTK__)
void CRenderWnd::OnEnter( wxMouseEvent &event )
{
	TRACEUSER( "Luke", _T("OnEnter %p\n"), event.GetEventObject() );
	
//	if( event.GetEventObject() != m_pFrame )
//		return;
	
	wxPoint				pt( event.GetPosition() );
	wxSetCursorEvent	CursorEvent( pt.x, pt.y );
	m_pView->OnSetCursor( CursorEvent );
	if( CursorEvent.HasCursor() )
	{
		wxSetCursor( CursorEvent.GetCursor() );
	}
}

void CRenderWnd::OnLeave( wxMouseEvent &event )
{
	TRACEUSER( "Luke", _T("OnLeave %p\n"), event.GetEventObject() );

//	if( event.GetEventObject() != m_pFrame )
//		return;
	
	::wxSetCursor( *wxSTANDARD_CURSOR );
}
#endif

void CRenderWnd::SetDoubleBuffer(BOOL DoubleBuffer)
{
	m_DoubleBuffer = DoubleBuffer;
	// Now go through and set or clear double buffering on each window
	wxWindow * pWindow = CCamFrame::GetMainFrame();
	if (pWindow) ReflectDoubleBufferingInChildren(pWindow);
}

void CRenderWnd::ReflectDoubleBufferingInChildren(wxWindow * pWindow)
{
#if defined(__WXGTK__)
	// Process this one
	if (pWindow->IsKindOf(CLASSINFO(CRenderWnd)))
		::SetDoubleBuffer(pWindow, m_DoubleBuffer);
#endif

	// Now process children if any
	wxWindowList::Node * pNode = pWindow->GetChildren().GetFirst();
	while (pNode)
	{
		ReflectDoubleBufferingInChildren(pNode->GetData());
		pNode = pNode->GetNext();
	}
	return;
}

/********************************************************************************************

>	OpToggleDoubleBuffer::OpToggleDoubleBuffer() : Operation()

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	14 Mar 2006
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructs an OpToggleDoubleBuffer object.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpToggleDoubleBuffer::OpToggleDoubleBuffer() : Operation()
{

}



/********************************************************************************************

>	OpToggleDoubleBuffer::~OpToggleDoubleBuffer()

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	14 Mar 2006
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destructs an OpToggleDoubleBuffer object.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpToggleDoubleBuffer::~OpToggleDoubleBuffer()
{
	// Empty
}



/********************************************************************************************

>	void OpToggleDoubleBuffer::Do(OpDescriptor*)

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	14 Mar 2006
	Inputs:		Pointer to Operation Descriptor
	Outputs:	-
	Returns:	-
	Purpose:	Actually "DO" a ToggleFore operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpToggleDoubleBuffer::Do(OpDescriptor*)
{
	CRenderWnd::SetDoubleBuffer(!CRenderWnd::GetDoubleBuffer());
	End();
}



/********************************************************************************************

>	OpState OpToggleDoubleBuffer::GetState(String_256* UIDescription, OpDescriptor*)

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	14 Mar 2006
	Inputs:		Pointer to Operation Descriptor
				Text Description
	Outputs:	-
	Returns:	-
	Purpose:	Find the state of the OpToggleDoubleBuffer operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState OpToggleDoubleBuffer::GetState(String_256* UIDescription, OpDescriptor*)
{
	// Default to !ticked and greyed
	OpState blobby(FALSE, TRUE);
	blobby.Ticked = CRenderWnd::GetDoubleBuffer();
	blobby.Greyed = FALSE;
	return(blobby);
}



/********************************************************************************************

>	BOOL OpToggleDoubleBuffer::Init()

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	14 Mar 2006
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Create an OpDescriptor for the OpToggleDoubleBuffer operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpToggleDoubleBuffer::Init()	
{
	return Operation::RegisterOpDescriptor( 
											0, 
											_R(IDS_TOGGLE_DOUBLEBUFFER),
											CC_RUNTIME_CLASS(OpToggleDoubleBuffer), 
											OPTOKEN_TOGGLEDOUBLEBUFFER,
											OpToggleDoubleBuffer::GetState,
											0,						// help ID
											0,// _R(IDBBL_FOREBACKGRNDOP),
											0,						// bitmap ID
											0,						// control ID
											SYSTEMBAR_ILLEGAL,		// group bar ID
											TRUE,
											FALSE,
											TRUE,
											NULL,
											0,
											0,
											TRUE
										   );
}

