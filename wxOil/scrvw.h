// $Id: scrvw.h 662 2006-03-14 21:31:49Z alex $
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

#ifndef INC_SCRVW
#define INC_SCRVW

#include "camview.h"
//#include "draginfo.h"
//#include "dragtrgt.h"
#include "ink.h"
#include "clikmods.h"
#include "rendwnd.h"
#include "wrkrect.h"
//#include "oilruler.h"

#ifdef _DEBUG
	// When CCamViews get this message they describe themselves in the debug
	// output.
	#define WM_DESCRIBEVIEW		(WM_USER + 101)
#endif

enum ObjectDragTarget
{
	NO_TARGET,
	FILL_TARGET,
	LINE_TARGET,
	STARTCOL_TARGET,
	ENDCOL_TARGET,
	ENDCOL2_TARGET,
	ENDCOL3_TARGET,
	MANY_TARGET
};

enum MouseOverRulerHit
{
	OVER_NO_RULERS,
	OVER_HORIZONTAL,
	OVER_VERTICAL,
	OVER_ORIGIN_GADGET
};

//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER
class CCPrintInfo;
#endif //webster
class PrintControl;
class OILHorizontalRuler;
class OILVerticalRuler;
class OriginGadget;
class CWinScroller;
class CObject;

/********************************************************************************************

>	class ScreenView : public CCamView

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/95
	Purpose:	A subclass of CCamView which is the class actually used for screen
				display.
	SeeAlso:	CCamView; PrintCamView

********************************************************************************************/

class ScreenView : public CCamView
{
////////////////////////////////////////////////////////////////////////////////
// A contained canvas to emulate MFC style views
protected:
	class ScreenCanvas :
		public CCamView::CCamCanvas
	{
	public:
		ScreenCanvas( wxView *v, wxMDIChildFrame *frame, const wxPoint& pos, const wxSize& size, 
			/* TYPENOTE: Correct */ long style ) : CCamCanvas( v, frame, pos, size, style )
		{
		}
		
	////////////////////////////////////////////////////////////////////////////////////////////////
	// Message response functions.
	protected:
		void 	OnSize( wxSizeEvent &event );
		void 	OnLButtonDown( wxMouseEvent &event );
		void 	OnLButtonDblClk( wxMouseEvent &event );
		void 	OnLButtonUp( wxMouseEvent &event );
		void 	OnMButtonDown( wxMouseEvent &event );
		void 	OnMButtonDblClk( wxMouseEvent &event );
		void 	OnMButtonUp( wxMouseEvent &event );
		void 	OnRButtonDown( wxMouseEvent &event );
		void 	OnRButtonDblClk( wxMouseEvent &event );
		void 	OnRButtonUp( wxMouseEvent &event );
		void 	OnMouseMove( wxMouseEvent &event );
		void	OnMouseWheel( wxMouseEvent &event );

	private:
		DECLARE_EVENT_TABLE()
	};

	// Creator shim, that creates a canvas suitable for this view
	virtual CCamCanvas* CreateCanvas( wxView *v, wxMDIChildFrame *frame, const wxPoint& pos, 
		const wxSize& size, /* TYPENOTE: Correct */ long style )
	{
		return new ScreenCanvas( v, frame, pos, size, style );
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////
// Creation and destruction.
	
protected:
	static INT32 PopCount;					// how many ScreenViews around
	static ViewState* pReadyMadeViewState;	// points to the ViewState to use on next create
		
public:
	ScreenView();    				// default constructor
	virtual ~ScreenView();

	void SetViewStateFromThis() const;  // set Status according to this view
	void SetThisFromViewState();		// set this view according to Status

	static BOOL ReadViewPrefs();
	static INT32 HowMany();

	static DocView* GetDocViewFromHwnd( wxWindow *hwnd );
	static DocView* GetDocViewFromWindowID(CWindowID WinID);

	virtual BOOL IsScreenView() { return TRUE; }

	MouseOverRulerHit IsMouseOverRuler();

	// Call this to provide a preset ViewState object that will be used for the next
	// view that is created.
	static void SetReadyMadeViewState(ViewState* pvs);

////////////////////////////////////////////////////////////////////////////////////////////////
// Position and Z-order management.

protected:
	static BOOL UnzoomOnNewView;		// preferences for creating new views
	static BOOL ZoomSingleView;

	BOOL fSetupDone;					// if TRUE then all inits done
	bool fInitialUpdate;				// Set after calling OnInitialUpdate
	BOOL fJustCreated;					// this view has never been activated before, it's new

	void UpdateViewPosition() const;	// called on moves & resizes

protected:
	virtual void OnInitialUpdate();		// called once between creation & painting
    virtual void OnUpdate(wxView* sender, wxObject* hint=(wxObject*)NULL);
	virtual void OnActivateView(bool bActivate, wxView* pActiveView,
								wxView* pDeactiveView);

public:
	BOOL IsTopmost() const;				// TRUE if this view is active
	BOOL ShouldEatClick() const;		// TRUE if this view has just been activated
	void ClearEatFlag();			// clears the above flag.
	

////////////////////////////////////////////////////////////////////////////////////////////////
// Painting.

//protected:
//	CRenderWnd* RenderWindow;

public:
	wxDC* GetRenderDC() const;
	wxWindow* GetRenderWindow() const;
	
	virtual void GetClientSize(int * width, int * height) const; /* TYPENOTE: Correct*/
	void InvalidateView(const OilRect* pRect = 0, BOOL updatenow = FALSE);
		
	virtual void OnDraw( wxDC *pDc );
PORTNOTE("other","Removed OnPreparePrinting - not supported yet")
#ifndef EXCLUDE_FROM_XARALX
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
#endif
				
////////////////////////////////////////////////////////////////////////////////////////////////
// Scrolling data and functions.

protected:
	CWinScroller* HScrollBar;				// proportional scroll bars
	CWinScroller* VScrollBar;

PORTNOTE("other","Removed CScrollerCorner usage")
#ifndef EXCLUDE_FROM_XARALX
		CScrollerCorner* Corner;                // the "size box"
#endif

//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER
	OILHorizontalRuler * HRuler;
	OILVerticalRuler * VRuler;
	OriginGadget * OGadget;
#endif //wesbter
	
	WinRect CurrentSize;					// the size of this view
	WorkRect WorkArea;
	WorkCoord OldOffset;					// previous scroller position

	BOOL ScrollerStyle;                     // proportional or not
	static BOOL PropScrollersOn;			// the preference for the above
	
	static BOOL DefaultScrollersState;
	static BOOL DefaultRulersState;
	static BOOL UseVisibleDefaults; 

	void ScrollTo(const WorkCoord& offset);

public:

	static void SetUseVisibleDefaults(BOOL state){ UseVisibleDefaults = state;};
	BOOL GetScrollerStyle() const;
	void SetScrollerStyle(BOOL proportional);

//	void GetWorkAreaExtent(WorkRect* pArea) const;
	void SetWorkAreaExtent(const WorkRect& area, BOOL redraw = TRUE);
	void GetScrollOffset(WorkCoord* pOffset) const;
	void SetScrollOffset(const WorkCoord& pos, BOOL redraw = TRUE);
	void ShowScrollers(BOOL Show); 
	void ShowRulers(BOOL Show);
	
	BOOL AreRulersVisible();
	BOOL AreScrollersVisible();

	static BOOL GetUseVisibleDefaults()				 { return UseVisibleDefaults;};
	static BOOL GetDefaultRulersState()				 { return DefaultRulersState;};
	static BOOL GetDefaultScrollersState()			 { return DefaultScrollersState;};
	static void SetDefaultRulersState(BOOL State)	 { DefaultRulersState = State;};
	static void SetDefaultScrollersState(BOOL State) { DefaultScrollersState = State;};

////////////////////////////////////////////////////////////////////////////////////////////////
// Drag'n'drop data and functions.

protected:
	static INT32 DragLatency;				// mouse must move >= this for a drag, except:
	static INT32 DragDelay;				// delay before we start a drag anyway, even if
										// mouse has not moved.
	static MonotonicTime DragTimer;		// Used to time the drag delay above.

	BOOL DragKeepAccuracy;				// Used to control drag responsiveness

	BOOL AutoScrollExcludeRulers;		// Flag used to extend the autoscroll region to 
										// include rulers - allows guidelines to be deleted

	ClickType 	   LastClickType;		// click type of last START_DRAG
	UINT32 		   LastClickButton;		// buttons down on last START_DRAG
	wxPoint 		   LastClickPoint;      // mouse position of last START_DRAG
	wxPoint 		   CurrentMousePos;		// the mouse was here last time we looked
	ClickModifiers LastClickMods;		// ClickMods on last mouse event (NOT
										// modified by button-up events!)

	UINT32 FirstClickButton;				// Which button started the drag (if dragging).

	Operation* pCurrentDragOp;			// current mouse-drag operation
	DragType CurrentDragType;			// what to do when dragging off-screen
	WinRect MouseClipRect;				// cursor was clipped against this

	BOOL OLERecaptureMouse;				// Used when OLE drags return to the view window - we resume our
										// drag, and we have to recapture the mouse to recover properly.

	void HandleDragEvent(UINT32 Button, wxMouseEvent &event, ClickType t);
	BOOL HandleMouseWheelEvent( wxMouseEvent &event );
	void HandleDragScrolling(wxPoint point);
	void CalcScrollDeltas(wxPoint point, WinRect wrSize, INT32* dx, INT32* dy);
	void HandleButtonUp(UINT32 Button, wxMouseEvent &event);
	virtual void HandleOleDragOutOfView(wxPoint point);	// Used to allow derived classes to provide OLE drag and drop

	MonotonicTime m_OLELastOutTime;		// Used to time the delay before "OLE scrolling"

public:
	void RecaptureMouseOnNextUpdate(void);

	static void GetClickMods( wxMouseEvent &event, ClickModifiers *p );
	BOOL StartDrag(Operation* pOp, DragType type, BOOL KeepAccuracy);
	BOOL EndDrag(Operation* pOp);
	BOOL ChangeDragType(DragType Type);
	void ClearClickState();

	// Allows a drag op to be invoked from the ScreenView object, as if it had been
	// invoked as a result of clicking into the document.
	// Useful esp. for drag ops that are not connected to a particular tool and/or started
	// from clicks outside the view, e.g. Creating guidelines by clicking on a ruler window
	BOOL InvokeDragOp(String_256* pOpToken,OpParam* pParam,UINT32 Flags,wxPoint point);

	// Function to test if there is another mouse move message in the queue
	BOOL IsWaitingMouseMove();

	// This sets the output parameter to the current mouse position within this
	// ScreenView.  If the cursor isn't within the view it returns FALSE.
	BOOL GetCurrentMousePos(OilCoord* pMousePos) const;


	BOOL CreateDragTarget(DragInformation * DragInfo);

protected:
	// Vars used by InvokeDragOp()
	String_256 	DragOpToken;
	OpParam* 	pDragOpParam;
	BOOL 		DragOpInvoke;

////////////////////////////////////////////////////////////////////////////////////////////////
// OLE container support

#if (_OLE_VER >= 0x200)
	// m_pSelection holds the selection to the current CCamCntrItem.
	// For many applications, such a member variable isn't adequate to
	// represent a selection, such as a multiple selection or a selection
	// of objects that are not CCamCntrItem objects.  This selection
	// mechanism is provided just to help you get started.

	// TODO: replace this selection mechanism with one appropriate to your app.

public:
	CCamCntrItem* m_pSelection;
#endif

public:
	virtual BOOL IsSelected(const CObject* pDocItem) const;



////////////////////////////////////////////////////////////////////////////////////////////////
// Menu functions, put in at Mario's behest.

public:
//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER
PORTNOTE("other","Removed OnFilePrint - printing support")
#ifndef EXCLUDE_FROM_XARALX
		void OnFilePrint();
#endif
#endif //webster
	void OnFilePrintPreview();
				
	DocView* GetDocViewPtr() const;					// for Will

//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER

protected:
	// Helper functions for printing.
	BOOL PrintPaper(wxDC *pPrintDC, CCPrintInfo *pPrintInfo, PrintControl *pPrintControl);
	WinRect GetPrintClipRect(wxDC *pPrintDC, CCPrintInfo *pPrintInfo);
	void EnumeratePagePatches(CCPrintInfo *pPrintInfo);

#endif //webster
////////////////////////////////////////////////////////////////////////////////////////////////
// Triple and quad click support

public:
	BOOL IsSingleClickReallyTriple() const	{return ThisSingleIsTriple;}
	BOOL IsSingleClickReallyQuad() const	{return ThisDoubleIsQuad;}

protected:
	MonotonicTime TimeOfLastClick;
	wxPoint LastDoubleClickPos;
	BOOL ThisSingleIsTriple;
	BOOL ThisDoubleIsQuad;

    DECLARE_DYNAMIC_CLASS( ScreenView )
    DECLARE_EVENT_TABLE()

	void 	OnSize( wxSizeEvent &event );
	void 	OnLButtonDown( wxMouseEvent &event );
	void 	OnLButtonDblClk( wxMouseEvent &event );
	void 	OnLButtonUp( wxMouseEvent &event );
	void 	OnMButtonDown( wxMouseEvent &event );
	void 	OnMButtonDblClk( wxMouseEvent &event );
	void 	OnMButtonUp( wxMouseEvent &event );
	void 	OnRButtonDown( wxMouseEvent &event );
	void 	OnRButtonDblClk( wxMouseEvent &event );
	void 	OnRButtonUp( wxMouseEvent &event );
	void 	OnMouseMove( wxMouseEvent &event );
	void	OnMouseWheel( wxMouseEvent &event );

PORTNOTE("other","Removed MessageMap - hopefully very temporarily")
#ifndef EXCLUDE_FROM_XARALX
	//{{AFX_MSG(ScreenView)
		afx_msg void 	OnDropFiles(HDROP hDropInfo);
		afx_msg INT32		OnMouseActivate(wxWindow* pDesktopWnd, UINT32 nHitTest, UINT32 nMessage);
		afx_msg void 	OnTimer(UINT32 nIDEvent);
		afx_msg void 	OnCancelMode();
		afx_msg void    OnInsertObject();
		afx_msg void    OnPaint();
		afx_msg BOOL    OnSetCursor(wxWindow* pWnd, UINT32 nHitTest, UINT32 nMessage);
		afx_msg LRESULT OnScrollerEvent(WPARAM wp, LPARAM lp);
#ifdef WIN32
		afx_msg LRESULT OnJoystickMove(WPARAM wp, LPARAM lp);
#endif
		afx_msg LRESULT OnRenderView(WPARAM wp, LPARAM lp);
		afx_msg LRESULT OnFrameMove(WPARAM x, LPARAM y);
#if (_OLE_VER >= 0x200)
		afx_msg void 	OnSetFocus(wxWindow* pOldWnd);
		afx_msg void 	OnCancelEdit();
#endif
#ifdef _DEBUG
		afx_msg LRESULT OnDescribeView(WPARAM, LPARAM);
#endif
	afx_msg void OnVScroll( UINT32 nSBCode, UINT32 nPos, CScrollBar* pScrollBar ) ;
	afx_msg void OnHScroll( UINT32 nSBCode, UINT32 nPos, CScrollBar* pScrollBar ) ;
}}AFX_MSG

		DECLARE_MESSAGE_MAP()
#endif
};

/*********************************************************************************************

>	class PageDropInfo : public CCObject

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Purpose:	Gives info about where an drag has, or is going to be, dropped on a page.

	SeeAlso:	DragInformation

*********************************************************************************************/

class PageDropInfo : public CCObject
{
	CC_DECLARE_DYNCREATE(PageDropInfo)

public:
	Document* 	pDoc;
	DocView* 	pDocView;
	Spread* 	pSpread;
	DocCoord 	DropPos;

	ObjectDragTarget 	TargetHit; 
	NodeRenderableInk* 	pObjectHit;
};

/*********************************************************************************************
>	class 	ViewDragTarget : public WinoilDragTarget

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>.
	Created:	2/1/95

	Purpose:	An instantiation of this class is created by each entity which wishes to 
				provide a 'destination' to which the mouse can go to complete a drag.
				WinoilDragTargets are used as a base class from which to derive drag targets
				for WINOIL entities (an optional HWND)

	Notes:		Drag targets are destructed automatically when a drag finishes by the drag
				manager with which they were registered.

				To remove a drag target at any time, destruct it - it automatically deregisters
				and cleans up.
				
	SeeAlso:	DragManagerOp::StartDrag; DragInformation; DragTarget

	Documentation:	Docs\Specs\DragMan.doc
*********************************************************************************************/

PORTNOTE("other","Removed ViewDragTarget - WinoilDragTarget usage")
#ifndef EXCLUDE_FROM_XARALX
class ViewDragTarget : public WinoilDragTarget
{
friend class DragManagerOp;
friend class ColourDragInformation;
	
CC_DECLARE_DYNAMIC(ViewDragTarget)


public:	// Public interface
	
	ViewDragTarget( wxWindow *TheWindow, CRect *ClientArea = NULL, DragInformation* pDragInfo = NULL);
	
	virtual UINT32 GetCursorID();
	virtual BOOL GetStatusLineText(String_256 * TheText);

	virtual BOOL GetDropInfo(PageDropInfo*);

protected:
	
	// Process a drag-related event
	BOOL ProcessEvent(DragEventType Event,
						DragInformation *pDragInfo,
						wxPoint *pMousePos, KeyPress* pKeyPress);

	DragInformation* pCurrentDragInfo;

	BOOL OverOutline;
	INT32 LineRad;

	DocView* 	pThisDoc;
	Spread* 	pSpread;
	DocCoord 	DropPos;

	ObjectDragTarget 	OverTarget;
	NodeRenderableInk* 	OverNode;
};
#endif

#endif  // INC_SCRCAMVW
