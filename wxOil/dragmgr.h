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

// dragmgr.h - Definitions for global dragging system manager

#ifndef INC_DRAGMGR
#define INC_DRAGMGR


#include "draginfo.h"
#include "dragmsg.h"
#include "dragtrgt.h"
#include "list.h"
#include "cursor.h"

/********************************************************************************************

>	class CaptureWnd : public CWnd

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/01/95
	Purpose:	A hidden window to centralise all click detection during a drag
	SeeAlso:	

********************************************************************************************/

class CaptureWnd : public wxFrame
{
	DECLARE_DYNAMIC_CLASS(CaptureWnd)

	friend class DragManagerOp;	
	friend class DragInformation;

 public:
	CaptureWnd(); 
	~CaptureWnd();
	BOOL Create();

	void OnWindowCreate(wxWindowCreateEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnLButtonUp(wxMouseEvent& event);
	void OnRButtonUp(wxMouseEvent& event);

public:
	BOOL SetUpSolidDrag(wxPoint StartPos);
	BOOL CleanUpSolidDrag();

	BOOL CleanUpSolidDragInScreenArea(const wxRect& Area);
		// Called to remove any on-scren solid drag stuff if it overlaps the given area
		// This minimises flicker when unrelated areas of the screen are redrawn while we're dragging

	BOOL DrawSolidDrag(wxPoint point);
	BOOL DrawTransparentDrag(wxPoint point, INT32 Transparency);

protected:
	wxScreenDC* pDisplayDC;

	wxBitmap* BackBitmap;
	wxRect DragRect;	

	// Some things for semi-transparent drags
	wxBitmap* MaskBitmap;

	DECLARE_EVENT_TABLE()
};                                                                                                                                                         



/*********************************************************************************************

>	class DragManagerOp : public Operation

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/1/95
	Purpose:	An instantiation of this class (created by a call to its static StartDrag
				method) is used to describe a current global drag. When the drag completes,
				this object will self destruct. While alive, this object controls the drag,
				passing relevant events on to registered drag targets, etc.
				See the documentation for a fuller description of the global drag system.

	SeeAlso:	DragManagerOp::StartDrag; DragTarget; DragInformation

	Documentation:	Docs\Specs\DragMan.doc

*********************************************************************************************/

class CCAPI DragManagerOp : public Operation
{
friend class DragTarget;
friend class KernelDragTarget;
friend class OilDragTarget;
friend class CaptureWnd;
friend class DragInformation;

	CC_DECLARE_DYNCREATE(DragManagerOp)


protected:		// Protected constructors/Destructor
	DragManagerOp();					// use StartDrag to create a new DragManagerOp
	DragManagerOp(DragInformation *Descriptor);

	~DragManagerOp();					// use AbortDrag to destruct a DragManagerOp


public:			// External drag management methods
	static void StartDrag(DragInformation *Descriptor);
		// To start a drag going

	static void EndDrag(INT32 Flags);
		// To end a drag - called from the capture window on mouse ups

	static void AbortDrag(void);
		// To abort the current drag (if any)

	static DragManagerOp *GetCurrentManager(void);
		// Returns NULL, or a pointer to the current drag manager
	static BOOL GetStatusText(String_256 * StatusText);

	static CaptureWnd * GetDragCaptureWnd();

	static DragInformation * GetCurrentDragInfo();

	static wxPoint GetDragMousePos();

	static BOOL IsDragActive(){return DragActive;}

	static void SetDragActive(BOOL State){DragActive = State;}

	static void RedrawStarting(void);
	static void RedrawStarting(CWindowID TheWindow, CGadgetID TheGadget, DocRect *TheArea = NULL);
	static void RedrawFinished(void);
		// Call to tell the drag manager that a redraw is occurring, during a drag.
		// This allows the drag manager to ensure all drag blobs are removed from screen during the redraw

public:			// Operation class overrides
	virtual BOOL OnIdleEvent(void);
		// Handles idle events

protected:		// Methods for use only by friend classes
	void RegisterTarget(DragTarget *NewTarget, BOOL HighPriority = FALSE);
		// Registers a target with the current drag manager

	void DeregisterTarget(DragTarget *OldTarget);
		// De-registers a registered target. Is quiet if the target is already deregistered

	static void SetCursor();
		// Sets the cursor shape. If nobody tries to set the shape during an event processing
		// pass, then it will be set back to the default. Repeated calls with the same shape
		// have no effect (except to stop the pointer reverting)
	
	static void SetStatusLineText();
		// writes status line text

private:		// Internal methods
	BOOL ProcessEvent(DragEventType Event);
		// Passes a Drag Event to all (or until event claimed) registered DragTargets

	void CleanUpAfterDrag(void);
		// Deletes all of our memory claims
  
private:		// private data
	static DragManagerOp *CurrentManager;	// ptr to the currently active Drag Manager
											 
	static DragInformation *CurrentDragInfo;// Points to descriptor of current drag

	static DragTarget * CurrentDragTarget;  // Points to the current drag target

	static UINT32 StatusLineStringID;			// ID of Current StatusLine Text string

	UINT32 CurrentID;
	Cursor* CurrentCursor;

	List 		Targets;					// List of DragTargets for this current drag

	KeyPress	*CurrentKeypress;			// NULL, or last keypress info

	wxPoint		InitialMousePos;			// Contains first winoily screen mouse position
	wxPoint		CurrentMousePos;			// Contains last winoily screen mouse position	
	wxPoint		LastMousePos;				// Last position the mouse was at

	DragEventType LastEvent;				// Type of the last Drag Event processed	

	static CaptureWnd * TheCaptureWindow;	// The window we use to centralised mouse handling

	static BOOL DragPending;				// Drag pending flag ie. this may be a click !		
	
	static BOOL DragEnded;

	static MonotonicTime DragStartTimer;

	static UINT32 DragMinDist;				// drag start distance

	static UINT32 DragDelay;					// millisecond delay before drag starts

	static wxRect DragStartRect;
	
	static wxRect StillClickRect;			// initial area for drag start - if pointer
											// leaves this area we start a drag.

	static BOOL DragActive;

	static BOOL RedrawInProgress;			// An external redraw is occurring
};


#endif

