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
//  Description file for DocView class
//
//  Created by Phil, 17/05/93
//
/*
 */ 


#ifndef INC_DOCVIEW
#define INC_DOCVIEW

#include "view.h"
#include "ccmaths.h"
#include "rect.h"
#include "doccoord.h"
#include "binds.h"
#include "clikmods.h"
#include "ops.h"
#include "monotime.h"
#include "oilcoord.h"
#include "quality.h"
#include "docvmsg.h"
#include "draginfo.h"
#include "dragtrgt.h"
#include "cursor.h"

#define OPTOKEN_TOGGLESOLIDDRAG		TEXT("ToggleSolidDrag")

class ImportPosition;

// Declare classes so that circular references are resolved and I don't need to include
// their header files...
class CCamView;
class ViewState;
class CursorStack;
class Document;
class Node;
class Chapter;
class RenderRegion;
class WorkRect;
class OilRect;
class CSnap;
class RulerPair;
class AttrUser;
class NodeRenderableInk;

// Enum for the RenderOnTop function. Use one of these as a reason code to pass to it
enum OnTopCode
{
	Normal = 0,				// do normal rendering
	UnclippedEOR = 1,		// Do EOR rendering
	UnclippedXOR = 1,		// Do EOR rendering
	ClippedEOR = 2,			// Do EOR rendering clipped to the spread
	ClippedXOR = 2,			// Do EOR rendering clipped to the spread
	CLICKBITMAP,			// render into a colour bitmap for click detection
	COLOURBITMAP			// render into a colour bitmap
};


/********************************************************************************************

>	class PendingRedraws : public List

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/94
	Purpose:	Encapsulates a set of regions in a DocView, which must be accumulated in 
				order for them to be redrawn at some point, possibly all in one go.  This 
				class chooses the most efficient invalidation method, according to the number
				and nature of regions added.
	SeeAlso:	SpreadRedraws

********************************************************************************************/

class PendingRedraws : public List
{
	CC_DECLARE_MEMDUMP(PendingRedraws);

public:
	PendingRedraws();
	~PendingRedraws();

	BOOL AddInvalidRegion(Spread*, DocRect, Node* pInvalidNode);
	void FlushRedraw(DocView*);
	void HandleNodeDeletion(Node* pNode);
};



/********************************************************************************************

>	class DocView : public View

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/1993
	Base Class:	ListItem
	Friends:	CCamView
	Purpose:

		The DocView class is used to hold non-oil data about views onto a document. When a 
		CCamView object exists there is always a DocView object on the other side of the OIL-
		interface and they are linked. However, the DocView object is not destroyed when the 
		CCamView object is destroyed so that it records the state of views while they are 
		closed.

		Currently it contains:
			The rectangle of physical document space being shown by this view.
			The user viewing scale factor (as opposed to the PhysToWindow factor).
			The user WYSIWYG value for this view.

		When fully implemented it will also hold:
			Information about the viewing of separations in this view.
			Information about the state of "snap-to-grid" in this view.
			Information about the viewing of print-borders and tile boundaries.

		The kernel calls functions in this class to change the view, and DocView in turn, calls 
		CCamView.
		There are also functions which the OIL layer calls to inform the kernel when the OS 
		forcibly changes the view.

		If you can't find the information that you want from a DocView object then the next 
		place to try is it's associated CCamView (if there is one) and after that the Document 
		object itself. All these objects are linked to each other.

	SeeAlso:	View

********************************************************************************************/

class CCAPI DocView : public View
{
friend class Document;		// Give Document access to SetSelectedInternal()

/////////////////////////////////////////////////////////////////////////////////////////////////
//	Creation & destruction.

private:
	CC_DECLARE_DYNAMIC(DocView);

public:
	DocView(Document*);							// DocView constructor
	virtual ~DocView();							// DocView destructor
	BOOL Init();
	static BOOL DeclarePreferences();


/////////////////////////////////////////////////////////////////////////////////////////////////
//	Interface to the OIL layer CCamView and the kernel Document.

public:
	void OnNewView();							// called by CCamView for new views!
	void OnLoadedView();						// and the same for (re)loaded views!

/////////////////////////////////////////////////////////////////////////////////////////////////
//	"Current" and "Selected" DocViews

private:
	static DocView* Selected;					// Global which records Selected View
	BOOL			IsViewDying;				// This is set to TRUE when the view starts to die

public:
	static DocView* GetCurrent();				// Returns which DocView is current
	BOOL SetCurrent();							// Make this View current
	static void	SetNoCurrent();					// Make NO View current
	virtual void SetViewPixelSize();			// Set pixel size according to this view.

	static DocView* GetSelected();				// Returns which DocView is selected

// These 2 functions are now handled by the centralised Document::SetSelectedViewAndSpread
// method, which interacts with this class via SetSelectedInternal, below
//	void SetSelected();							// Make this View selected
//	static void	SetNoSelected();				// Make NO View selected
	
	static  CNativeWnd *GetCurrentRenderWindow();	// Returns which render window is current
	virtual CNativeWnd *GetRenderWindow();			// Returns render window associated with docview

protected:		// Only available to friend class Document
	static void SetSelectedInternal(DocView *NewSelected);
	static void SendSelectedMessage(DocView *OldSelected, DocView *NewSelected);


/////////////////////////////////////////////////////////////////////////////////////////////////
//	Cursor management

private:
	INT32 PushToolCursorID;	// Matt 12/01/2001 - Current cursor ID
	Cursor m_PushCursor;

/////////////////////////////////////////////////////////////////////////////////////////////////
//	Dragging

private:
	DocCoord  	StartDragPos;					// Coord of the start of the drag box
	DocCoord  	OldDragPos;						// Coord of the last position of the drag box
	WorkCoord 	LastWorkClick;					// WorkCoord of most recent click
	Operation* 	pCurrentDragOp;					// not NULL if a drag is running
	DocCoord	DragPos;						// Coord of mouse on last drag event
	BOOL		m_bSolidDragSupported;			// Flag indicating whether current drag op can do solid drags
//	BOOL		m_bSolidDrag;					// Flag indicating that current drag is solid

public:
	BOOL StartDrag( Operation*,
					DragType,
					DocRect* pMoveBBox,
					DocCoord* StartPos,
					BOOL KeepAccuracy,
					BOOL bSolidDragSupported = FALSE);
	BOOL EndDrag(Operation*);
	BOOL ChangeDragType(DragType Type);

	void DragPointerMove(Operation*, OilCoord, ClickModifiers);
	void DragPointerIdle(Operation*, OilCoord, ClickModifiers);
	void DragFinished(Operation*, OilCoord, ClickModifiers, BOOL);

	BOOL IsSingleClickReallyTriple() const;
	BOOL IsSingleClickReallyQuad() const;

	void OnMouseMove(OilCoord, ClickModifiers);
	void OnClick(OilCoord, ClickType, ClickModifiers);
	void RenderSelectBlobs(DocRect Rect, Spread *pSpread);

	BOOL IsMouseMoved();

	WorkCoord GetClickWorkCoord();				// The WorkCoord pos of the last click

	// This returns TRUE if the mouse is within the selected DocView, in which case
	// the mouse position in spread/doc coords in stored in the output parameters.
	static BOOL GetCurrentMousePos(Spread** ppSpread, DocCoord* pdcMousePos);

	ObjectDragTarget IsPointerOverNode(NodeRenderableInk** ThisNode, INT32 Range, BOOL Interrupt = TRUE,
		BOOL bColourDrag = FALSE);

	BOOL DM_DragFinished(DragInformation* DragInfo, ViewDragTarget* pDragTarget);

	void SetSolidDragState(BOOL bNewState) {m_bSolidDrag = bNewState;}

	BOOL GetCurrentSolidDragAbility() {return m_bSolidDragSupported;}
	BOOL GetCurrentSolidDragState() {return m_bSolidDrag;}
	void ClearClickState();


/////////////////////////////////////////////////////////////////////////////////////////////////
//	Keyboard handling.

public:
	virtual BOOL HandleKeyPress(KeyPress* pKeyPress);

	// DMc
	// resets the rendering's start time
	// effectively, when called after a Node::Render function disables background
	// rendering for the time being
	void ResetRenderingStartTime() { StartTime.Sample(); }

/////////////////////////////////////////////////////////////////////////////////////////////////
//	Rendering

private:
	struct CCAPI 								// View flags
	{
		BOOL 	BackgroundRender : 1;			// TRUE if background rendering
		BOOL 	GridShow : 1;					// TRUE if snapping grid is shown
		BOOL 	GridSnap : 1;					// TRUE if snapping grid is active
		BOOL	ObjectsSnap : 1;				// TRUE if snapping to all objects is active
		BOOL	MagObjectsSnap : 1;				// TRUE if snapping to magnetic objects is active
		BOOL 	PrintBorderShow : 1;			// TRUE if print borders are shown
		BOOL	LogicalView : 1;				// TRUE if chapters should be shown vert.
		BOOL	WorkClickCached : 1;			// TRUE if LastWorkClick is valid
		BOOL	Dragging : 1;					// TRUE if Dragging something
		BOOL	GuidesSnap : 1;					// TRUE if snapping to objects in guide layers
		BOOL	GuidesShow : 1;					// TRUE if showing objects in guide layers
	} ViewFlags;

	// These functions want no-nonsense access to the above flags, so there.
//	friend void MakeKeyFromView(HKEY, CCamView*);
	friend class CCamView;

	MonotonicTime StartTime;					// Time at which rendering started

	// the normal rendering loop
 	RenderRegion* NewRenderRegion(DocRect, Matrix&, CNativeDC*, Spread*, RenderType);
	
	void RenderView(RenderRegion* pRegion, Matrix& mxTxform, Spread* pSpread,
					BOOL fRenderPaper, BOOL fDeleteRegionAfter = TRUE,
					BOOL bForceImmediate = FALSE);

	CNativeDC* GetRenderDC();

public:
	BOOL GetShowPrintBorders()				{ return (ViewFlags.PrintBorderShow != 0); }
	void SetShowPrintBorders(BOOL State)	{ ViewFlags.PrintBorderShow = State; }

public:
	Spread* FindEnclosingSpread(OilCoord);
	Spread* OilToSpreadCoord(OilCoord, DocCoord*);

public:
	virtual BOOL RenderPageMarks(RenderRegion*, Matrix&, DocRect&, Spread*);

public:
	void ContinueRenderView(RenderRegion* pRegion, Spread* pSpread,
							BOOL fRenderPaper = TRUE,
							BOOL fDeleteRegionAfter = TRUE,
							BOOL bForceImmediate = FALSE);

	void RenderEntireView(RenderRegion* pRender, Spread* pSpread);

	virtual BOOL GetForeBackMode();
	virtual void SetForeBackMode(BOOL);

	// Force the rendering of whole view
	void ForceRedraw(BOOL ForcePaper = FALSE);	
	
	// Force the rendering of rect in WorkCoords
	void ForceRedraw(WorkRect&);

	// Force the rendering of rect in DocCoords
	void ForceRedraw(Spread*, DocRect, BOOL Accumulate = FALSE, Node* pInvalidNode = NULL);
	void FlushRedraw();

	void HandleNodeDeletion(Node* pNode);

	static BOOL BackgroundRendering;			// Preference for bg/fg rendering.
	static BOOL SolidDragging;					// Preference for solid dragging.
	static UINT32 IdleDragDelay;					// Preference for time before showing extra info when dragging
	static UINT32 IdleDragDelay2;				// Preference for time before showing extra info when dragging
	static BOOL OriginOutlineShowNonOverlap;
	static BOOL OriginOutlineShowAlways;
	static UINT32 SolidDragTimeLimit;
	static BOOL OutlineShowBounds;

private:
	PendingRedraws PendingRedrawList;

/////////////////////////////////////////////////////////////////////////////////////////////////
//	Exclusive-OR Rendering

private:
	static	OnTopCode 		OnTopReason; 			// Reason code passed to RenderOnTop
	static  Spread*			OnTopSpread; 			// Spread being rendered in RenderOnTop
	static  RenderRegion* 	OnTopRenderRegion;		// The render region used in RenderOnTop
	static	INT32			OnTopCount;	 			// Global count of the nesting level in
													// the RenderOnTop function
public:
	static RenderRegion* RenderOnTop(DocRect* Rect, Spread *pSpread, OnTopCode Reason);
	static RenderRegion* GetNextOnTop(DocRect*);

	RenderRegion* GetFirstRenderRegion(DocRect&, Spread*, OnTopCode Reason);
	RenderRegion* GetNextRenderRegion(DocRect&, Spread*, OnTopCode Reason);


/////////////////////////////////////////////////////////////////////////////////////////////////
//	Scrolling, scaling and extents

private:
	static UINT32 ScrollToShowMargin;
	void ScrollToShowAux(DocCoord* pCoord, DocCoord Margin);
public:
	void ShowViewScrollers(BOOL fIsVisible);
	void ShowViewRulers(BOOL fIsVisible);
	BOOL AreRulersVisible();
   	BOOL AreScrollersVisible();

	BOOL SetViewScale(FIXED16 NewScale);
	BOOL SetScrollOffsets(WorkCoord NewTopLeft, BOOL RedrawNeeded);

	// Scroll view so that the DocCoord, DocRect, or corner of a DocRect is in view
	void ScrollToShow(DocCoord* pCoord);
	void ScrollToShow(DocRect *RectToShow);
	void ScrollToShow(DocRect* pBoundingBox, DocCoord Direction);
	void ScrollToShowWithMargin(DocCoord* pCoord);
	void ScrollToShowWithMargin(DocRect *RectToShow);
	void ScrollToShowWithMargin(DocRect* pBoundingBox, DocCoord Direction);

	BOOL ViewStateChanged();					// informs us that ViewState has changed

	DocRect GetDocViewRect(Spread*);
	void SetExtent(DocCoord, DocCoord);			// Reset the extent of the document
	WorkRect GetViewRect();						// The physical rect of the view

//	void GetViewParams(FIXED16*, FIXED16*, FIXED16*, WorkCoord*);												// (as reflected in the scroll bars)
//	static DocRect ChapterClip(DocRect& DocClip, Chapter* pChapter);


/////////////////////////////////////////////////////////////////////////////////////////////////
//	Zoom tool per-view data and functions.

private:
	INT32			ZoomTableIndex;					// working storage used by zoom tool
	INT32			PrevIndex;						// "previous zoom" table index
	FIXED16 	PrevScale;						// "previous zoom" scale
	WorkCoord 	PrevOffset;						// "previous zoom" scroll offset

public:	
	INT32 GetZoomTableIndex() const;				// tracks scale as entry in zoom table
	void SetZoomTableIndex(INT32 nIndex);			// related, sets the index

	INT32 GetPrevZoomIndex() const;
	void SetPrevZoomIndex(INT32 nIndex);

	FIXED16 GetPrevZoomScale() const;			// same for the scale factor of a
	void SetPrevZoomScale(FIXED16 fxScale);		// previous zoom

	WorkCoord GetPrevZoomOffset() const;		// ditto for scroll offsets
	void SetPrevZoomOffset(const WorkCoord& wcOffset);


/////////////////////////////////////////////////////////////////////////////////////////////////
//	Grids and snapping

public:
	// General snap functions of DocView.
	BOOL Snap(Spread* pSpread, DocCoord* pDocCoord,
								BOOL TryMagSnap = TRUE,
								BOOL TryGridSnap = TRUE);
	BOOL Snap(Spread* pSpread, DocRect* pDocRect, 
							const DocCoord& PrevCoord, const DocCoord& CurCoord);

	// Easy ways of snapping to the current and selected DocViews.
	static BOOL SnapSelected(Spread* pSpread, DocCoord* pDocCoord,
								BOOL TryMagSnap = TRUE,
								BOOL TryGridSnap = TRUE);
	static BOOL SnapSelected(Spread* pSpread, DocRect* pDocRect,
							const DocCoord& PrevCoord, const DocCoord& CurCoord);

	static BOOL SnapCurrent(Spread* pSpread, DocCoord* pDocCoord,
								BOOL TryMagSnap = TRUE,
								BOOL TryGridSnap = TRUE);
	static BOOL SnapCurrent(Spread* pSpread, DocRect* pDocRect,
							const DocCoord& PrevCoord, const DocCoord& CurCoord);

	static BOOL SnapToMagneticNode(Spread* pSpread, Node* pNode, DocCoord* pDocCoord);
	static BOOL ForceSnapToGrid(Spread* pSpread, DocCoord* pDocCoord);

	// Snap state functions.
	BOOL GetSnapToGridState();
	void SetSnapToGridState(BOOL SnapToGrid);

	BOOL GetShowGridState();
	void SetShowGridState(BOOL ShowGrid);

	BOOL GetSnapToObjectsState();
	void SetSnapToObjectsState(BOOL SnapToObjects);

	BOOL GetSnapToMagObjectsState();
	void SetSnapToMagObjectsState(BOOL SnapToMagObjects);

	BOOL GetSnapToGuidesState()				{ return (ViewFlags.GuidesSnap != 0); }
	void SetSnapToGuidesState(BOOL State)	{ ViewFlags.GuidesSnap = State; }

	BOOL GetShowGuidesState()				{ return (ViewFlags.GuidesShow != 0); }
	void SetShowGuidesState(BOOL State)		{ ViewFlags.GuidesShow = State; }

private:
	CSnap* pCSnap;		// Ptr to the CSnap object that will perform all snapping in a given DocView and spread

/////////////////////////////////////////////////////////////////////////////////////////////////
//	Constraint

public:
	static void ConstrainToAngle(DocCoord&, DocCoord*, double Constrain=0.0);
	static void ConstrainToAngle(DocCoord&, double, DocCoord*);
	static double DefaultConstrainAngle;

	static void ConstrainToAspect(DocCoord& FixedPoint, FIXED16& AspectLock, DocCoord* CurCoord);

	

/////////////////////////////////////////////////////////////////////////////////////////////////
//	Spreads

public:

	Spread* GetVisibleSpread();			// returns pointer to largest spread within view
	Spread* GetFirstSelectedSpread();	// returns first spread with a selected object

/////////////////////////////////////////////////////////////////////////////////////////////////
// Rulers

public:
	RulerPair* GetpRulerPair() { return pRulerPair; }

protected:
	RulerPair* pRulerPair;

/////////////////////////////////////////////////////////
// Graham 15/7/97: Insertion positions

public:
	DocCoord GetCentreCoord();
	ImportPosition GetCentreImportPosition();

	DocCoord GetTopLeftCoord();
	ImportPosition GetTopLeftImportPosition();

/////////////////////////////////////////////////////////////////////////////////////////////
// Karim 17/02/2000: Get the current zoom factor.

public:
	double GetZoomFactor();

private:

///////////////////////////////////////////////////////////////////////////////////////////////
// Diccon 10/5/2000 The devious flag that is set during brush strokes and prevents background rendering
// from occurring until the stroke ends.  And yes I do know that this isn't very nice
public:
	void SetPreventRenderView(BOOL Value);
	BOOL GetPreventRenderView();

private:
	BOOL m_bPreventRenderView;


/////////////////////////////////////////////////////////////////////////////////////////////
public:
	void NotifyBoundsChanged(BOOL bNewValue = TRUE) {m_bBoundsChanged = (m_bBoundsChanged || bNewValue);}

private:
	BOOL m_bBoundsChanged;
};







#define OPTOKEN_TOGGLEFORE		TEXT("ToggleForeBack")
#define OPTOKEN_TOGGLESCROLL 	TEXT("ToggleScrollBars")



/********************************************************************************************

>	class OpToggleFore : public Operation

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	This class represents the ToggleFore operation.
				Creating an instance of this class and calling its "Do" function will toggle
				the state of the flag controlling whether any RenderRegions created by the
				current DocView will be submitted to the background rendering system or not.

********************************************************************************************/

class OpToggleFore: public Operation
{
	CC_DECLARE_DYNCREATE( OpToggleFore )
	
public:
	OpToggleFore();											// Constructor
	~OpToggleFore();										// Destructor

	static BOOL		Init();									// Register an OpDescriptor
	void			Do(OpDescriptor*);						// "Do" function
	static OpState	GetState(String_256*, OpDescriptor*);	// Read the state of an operation
};




/********************************************************************************************

>	class OpToggleScroll : public Operation

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	This class represents the ToggleScroll operation.
				Creating an instance of this class and calling its "Do" function will change
				the state of the BOOL which determines whether standard Window scroll bars or
				RISCOS-like proportional scroll bars are used.

********************************************************************************************/

class OpToggleScroll: public Operation
{
	CC_DECLARE_DYNCREATE( OpToggleScroll )

public:
	OpToggleScroll();										// Constructor
	~OpToggleScroll();										// Destructor

	static BOOL		Init();									// Register an OpDescriptor
	void			Do(OpDescriptor*);						// "Do" function
	static OpState	GetState(String_256*, OpDescriptor*);	// Read the state of an operation
};


/********************************************************************************************

>	class OpToggleSolidDrag : public Operation

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	14 Mar 2006
	Purpose:	This class represents the OpToggleSolidDrag operation.
				Creating an instance of this class and calling its "Do" function will toggle
				the state of the flag controlling whether double buffering is used for
				rendering on GTK

********************************************************************************************/

class OpToggleSolidDrag: public Operation
{
	CC_DECLARE_DYNCREATE( OpToggleSolidDrag )
	
public:
	OpToggleSolidDrag();											// Constructor
	~OpToggleSolidDrag();										// Destructor

	static BOOL		Init();									// Register an OpDescriptor
	void			Do(OpDescriptor*);						// "Do" function
	static OpState	GetState(String_256*, OpDescriptor*);	// Read the state of an operation
};



#endif		// INC_DOCVIEW
