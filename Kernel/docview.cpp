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
                                    
/********************************************************************************************
 DocView.CPP - DocView class definition file.

	This is the Camelot document viewer object.

********************************************************************************************/

#include "camtypes.h"
#include "docview.h"
#include "camview.h"
#include "vstate.h"
#include "wrkrect.h"
#include "tool.h"
#include "toollist.h"
#include "monotime.h"
#include "rndrgn.h"
#include "osrndrgn.h"
#include "grndrgn.h"
#include "paper.h"
#include "oilcoord.h"
#include "doccoord.h"
#include "winrect.h"
#include "oilrect.h"
#include "errors.h"
//#include "mario.h"
//#include "will.h"
#include "camelot.h" 		// For DisableSys flag
#include "ccdc.h"
#include "csrstack.h"
#include "app.h"
#include "document.h"
#include "nodedoc.h"
#include "chapter.h"
#include "spread.h"
#include "page.h"
#include "ink.h"
#include "blobs.h"
#include "fillattr.h"
#include "snap.h"
//#include "progress.h"
#include "lineattr.h"
#include "attrmgr.h"
#include "nodepath.h"
#include "nodeshap.h"
#include "nodershp.h"
//#include "dragcol.h"
//#include "mainfrm.h"
#include "zoomops.h"
#include "viewmenu.h"
#include "menuops.h"
#include "clikmods.h"
#include "nodetext.h"
#include "nodetxts.h"
//#include "rulers.h"
#include "dlgmgr.h"
//#include "bubbleid.h"
#include "filters.h"
#include "keypress.h"
#include "nodebev.h"

// for shadow node hit-testing 
#include "nodecont.h"
#include "nodeshad.h"
#include "nbevcont.h"

#include "pushtool.h"	// For OpPush
#include "bars.h"

#ifdef PHOTOSHOPPLUGINS
#include "plugmngr.h"	// CheckHaveDetailsOnPlugIns
#endif //PHOTOSHOPPLUGINS

//#include "resource.h"	// Matt 12/01/2001 - For _R(IDC_PUSHTOOLCURSOR)

#include "colplate.h"	// For on-screen separated rendering

#include "rendwnd.h"	// Shouldn't need this

DECLARE_SOURCE("$Revision$");


// Used by SpreadRedraws class.
#define MAX_REDRAW_REGIONS (20)

/********************************************************************************************

>	class SpreadRedraws : public ListItem

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/94
	Purpose:	Encapsulates a set of regions on a spread, which must be accumulated in order
				for them to be redrawn at some point, possibly all in one go.  This class
				chooses the most efficient invalidation method, according to the number
				and nature of regions added.
	SeeAlso:	PendingRedraws

********************************************************************************************/

class SpreadRedraws : public ListItem
{
	CC_DECLARE_MEMDUMP(SpreadRedraws);

public:
	SpreadRedraws(Spread*);

	Spread *GetSpread() { return pInvalidSpread; }
	void AddInvalidRegion(DocRect, Node* pInvalidNode);
	void FlushRedraw(DocView*);
	void ClearBackmostChangedNode(Node* pNode);

private:
	Spread *pInvalidSpread;
	UINT32 RectCount;
	BOOL TooManyRegions;
	DocRect InvalidRegions[MAX_REDRAW_REGIONS];
	Node* m_pBackmostChangedNode;
};



CC_IMPLEMENT_MEMDUMP(PendingRedraws, List)
CC_IMPLEMENT_MEMDUMP(SpreadRedraws, ListItem)
CC_IMPLEMENT_DYNAMIC(DocView, View)
CC_IMPLEMENT_DYNAMIC(DocViewMsg, Msg)
CC_IMPLEMENT_DYNCREATE(OpToggleFore, Operation)
CC_IMPLEMENT_DYNCREATE(OpToggleScroll, Operation)
CC_IMPLEMENT_DYNCREATE(OpToggleSolidDrag, Operation)


#define new CAM_DEBUG_NEW



//-----------------------------------------------------------------------------------------//
// Swap macro
// this is too beautiful to work!  (This is too obscure to be worth it!)

#define Swap(a, b)		((a) ^= (b), (b) ^= (a), (a) ^= (b))



// *******************************************************
// MIN and MAX Macros - These should not be here, I'm sure

#define MIN(a, b)		((a) < (b) ? (a) : (b))
#define MAX(a, b)		((a) < (b) ? (b) : (a))

// *******************************************************



//-----------------------------------------------------------------------------------------//
// Initialise class static variables...
//
DocView*		DocView::Selected = NULL;

INT32			DocView::OnTopCount	 = 0;
OnTopCode		DocView::OnTopReason = Normal;
Spread*			DocView::OnTopSpread = NULL;
RenderRegion* 	DocView::OnTopRenderRegion = NULL;


/********************************************************************************************

	Preference:	ConstrainAngle
	Section:	Constraints
	Range:		>0 ---> <2PI
	Purpose:	This is the default angle to which angles are constrained to (eg when
				rotating with constrain on, it will only rotate in multiples of this
				constrain angle.  It must be greater than zero degrees and less than
				360 degrees (note that it is recorded in radians).

********************************************************************************************/

double DocView::DefaultConstrainAngle = PI / 4.0;


/********************************************************************************************

	Preference:	BackgroundRendering
	Section:	Rendering
	Range:		0 or 1
	Purpose:	Defines whether or not to use background rendering on new views.
				0 => No background rendering
				1 => Use background rendering

********************************************************************************************/

BOOL DocView::BackgroundRendering = TRUE;


//-----------------------------------------------------------------------------------------//



/********************************************************************************************

	Preference:	SolidDragging
	Section:	Dragging
	Range:		0 or 1
	Purpose:	Defines whether or not solid dragging is enabled.
				0 => Solid dragging disabled
				1 => Solid dragging enabled

********************************************************************************************/

BOOL DocView::SolidDragging = TRUE;


//-----------------------------------------------------------------------------------------//



/********************************************************************************************

	Preference:	IdleDragDelay
	Section:	Dragging
	Range:		0 to 10000
	Purpose:	Defines delay time in milliseconds between detecting idle state and showing
				extra dragging info.

********************************************************************************************/

UINT32 DocView::IdleDragDelay = 200;


//-----------------------------------------------------------------------------------------//



/********************************************************************************************

	Preference:	IdleDragDelay2
	Section:	Dragging
	Range:		0 to 10000
	Purpose:	Defines delay time in milliseconds between detecting idle state and showing
				extra dragging info.

********************************************************************************************/

UINT32 DocView::IdleDragDelay2 = 1000;


//-----------------------------------------------------------------------------------------//



/********************************************************************************************

	Preference:	SolidDragTimeLimit
	Section:	Dragging
	Range:		0 to 10000
	Purpose:	Defines time limit in milliseconds for solid drag redraw. When redraw takes
				longer than this value, dragging falls back to outline mode

********************************************************************************************/

UINT32 DocView::SolidDragTimeLimit = 333;


//-----------------------------------------------------------------------------------------//



/********************************************************************************************

	Preference:	OriginOutlineShowNonOverlap
	Section:	Dragging
	Range:		0 or 1
	Purpose:	Original outlines will be shown as soon as dragged items don't overlap
				originals.

********************************************************************************************/

BOOL DocView::OriginOutlineShowNonOverlap = FALSE;


//-----------------------------------------------------------------------------------------//



/********************************************************************************************

	Preference:	OriginOutlineShowAlways
	Section:	Dragging
	Range:		0 or 1
	Purpose:	Original outlines will be shown as soon as dragged items don't overlap
				originals.

********************************************************************************************/

BOOL DocView::OriginOutlineShowAlways = FALSE;


//-----------------------------------------------------------------------------------------//



/********************************************************************************************

	Preference:	OutlineShowBounds
	Section:	Dragging
	Range:		0 or 1
	Purpose:	Dragged outlines will show a bounding rectangle.

********************************************************************************************/

BOOL DocView::OutlineShowBounds = TRUE;


//-----------------------------------------------------------------------------------------//



/********************************************************************************************

>	SpreadRedraws::SpreadRedraws(Spread *pSpread)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/94
	Inputs:		pSpread - the spread on which invalid regions will occur.
	Purpose:	Construct a spread redraw regions object for a given spread.  The invalid
				region is initially NULL.
	SeeAlso:	SpreadRedraws; PendingRedraws

********************************************************************************************/

SpreadRedraws::SpreadRedraws(Spread *pSpread)
{
	pInvalidSpread = pSpread;
	RectCount = 0;
	TooManyRegions = FALSE;
	m_pBackmostChangedNode = NULL;
}

/********************************************************************************************

>	void SpreadRedraws::AddInvalidRegion(DocRect Rect, Node* pInvalidNode)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/94
	Inputs:		Rect - the region to add (in document co-ordinates)
	Purpose:	Add another invalid region to the collection of regions for a spread.
	SeeAlso:	SpreadRedraws; PendingRedraws

********************************************************************************************/

void SpreadRedraws::AddInvalidRegion(DocRect Rect, Node* pInvalidNode)
{
	if (pInvalidNode!=NULL)
		if (m_pBackmostChangedNode==NULL || pInvalidNode->IsUnder(m_pBackmostChangedNode)) m_pBackmostChangedNode = pInvalidNode;

	if (TooManyRegions)
	{
		// We have exceeded the maximum number of inidividual regions, so we just
		// union them all together.
		InvalidRegions[0] = InvalidRegions[0].Union(Rect);
	}
	else
	{
		// Have we just exceeded the maximum number of regions?
		if (RectCount == MAX_REDRAW_REGIONS)
		{
			// Yes - gather all the existing regions together into one.
			for (INT32 i = 1; i < MAX_REDRAW_REGIONS; i++)
			{
				InvalidRegions[0] = InvalidRegions[0].Union(InvalidRegions[i]);
			}

			// Now merge the new region in
			InvalidRegions[0] = InvalidRegions[0].Union(Rect);

			// Indicate that we have too many regions
			RectCount = 1;
			TooManyRegions = TRUE;
		}
		else
		{
			// Otherwise, just add region to the array
			InvalidRegions[RectCount] = Rect;
			RectCount++;
		}
	}
}

/********************************************************************************************

>	void SpreadRedraws::ClearBackMostChangedNode(Node* pNode)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/94
	Inputs:		Rect - the region to add (in document co-ordinates)
	Purpose:	Add another invalid region to the collection of regions for a spread.
	SeeAlso:	SpreadRedraws; PendingRedraws

********************************************************************************************/

void SpreadRedraws::ClearBackmostChangedNode(Node* pNode)
{
	if (pNode && (m_pBackmostChangedNode==pNode || pNode->IsNodeInSubtree(m_pBackmostChangedNode)))
		m_pBackmostChangedNode = pNode->FindParentSpread();
}

/********************************************************************************************

>	void SpreadRedraws::FlushRedraw(DocView *pDocView)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/94
	Inputs:		pDocView - the DocView to invalidate
	Purpose:	Force the specified DocView to immediately update all the regions 
				encapsulated by this object.
	SeeAlso:	SpreadRedraws; PendingRedraws

********************************************************************************************/

void SpreadRedraws::FlushRedraw(DocView *pDocView)
{
	// Cause an immediate update of the DocView
	for (UINT32 i = 0; i < RectCount; i++)
	{
		// For the moment, assume that pending redraws must have been due to editing...
		pDocView->ForceRedraw(pInvalidSpread, InvalidRegions[i], FALSE, m_pBackmostChangedNode);
	}
}


/********************************************************************************************

>	PendingRedraws::PendingRedraws()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/94
	Purpose:	Initialise a PendingRedraws object.
	SeeAlso:	SpreadRedraws; PendingRedraws

********************************************************************************************/

PendingRedraws::PendingRedraws()
{
}


/********************************************************************************************

>	PendingRedraws::~PendingRedraws()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/94
	Purpose:	Destroys a PendingRedraws object.
	SeeAlso:	SpreadRedraws; PendingRedraws

********************************************************************************************/

PendingRedraws::~PendingRedraws()
{
	DeleteAll();
}


/********************************************************************************************

>	BOOL PendingRedraws::AddInvalidRegion(Spread *pSpread, DocRect Rect, Node* pInvalidNode)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/94
	Inputs:		pSpread - the spread on which the invalid region is located.
				Rect - the invalid region (in document co-ordinates)
	Returns:	TRUE if added ok;
				FALSE if not 
	Purpose:	Add another invalid region to the specified Spread's collection.
	Errors:		Out of memory => ERROR1
	SeeAlso:	SpreadRedraws; PendingRedraws

********************************************************************************************/

BOOL PendingRedraws::AddInvalidRegion(Spread *pSpread, DocRect Rect, Node* pInvalidNode)
{
	// Look for this spread in our list
	SpreadRedraws *pItem = (SpreadRedraws *) GetHead();

	while ((pItem != NULL) && (pItem->GetSpread() != pSpread))
	{
		// Nope - try next item
		pItem = (SpreadRedraws *) GetNext(pItem);
	}

	if (pItem == NULL)
	{
		// No entry for the spread - we'd better make one
		pItem = new SpreadRedraws(pSpread);

		// Did it work?
		if (pItem == NULL)
			return FALSE;

		// Yes - add to the list
		AddTail(pItem);
	}

	// Got an an entry for this spread - add the region
	pItem->AddInvalidRegion(Rect, pInvalidNode);

	// All ok
	return TRUE;
}

/********************************************************************************************

>	void PendingRedraws::FlushRedraw(DocView *pDocView)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/94
	Inputs:		pDocView - the DocView to invalidate.
	Purpose:	Cause all pending update regions on all spreads for this DocView to be 
				updated immediately.
	SeeAlso:	SpreadRedraws; PendingRedraws

********************************************************************************************/

void PendingRedraws::FlushRedraw(DocView *pDocView)
{
	// Invalidate the regions on each spread...
	SpreadRedraws *pItem = (SpreadRedraws *) GetHead();

	while (pItem != NULL)
	{
		// Invalid the regions on this spread, and delete this item
		pItem->FlushRedraw(pDocView);
		delete RemoveItem(pItem);

		// Now try the next item...
		pItem = (SpreadRedraws *) GetHead();
	}
}


/********************************************************************************************

>	void PendingRedraws::HanldeNodeDeletion(Node* pNode)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/94
	Inputs:		pDocView - the DocView to invalidate.
	Purpose:	Cause all pending update regions on all spreads for this DocView to be 
				updated immediately.
	SeeAlso:	SpreadRedraws; PendingRedraws

********************************************************************************************/

void PendingRedraws::HandleNodeDeletion(Node* pNode)
{
	// Invalidate the regions on each spread...
	SpreadRedraws *pItem = (SpreadRedraws *) GetHead();

	while (pItem != NULL)
	{
		pItem->ClearBackmostChangedNode(pNode);

		// Now try the next item...
		pItem = (SpreadRedraws *) this->GetNext(pItem);
	}
}


/********************************************************************************************

>	static BOOL DocView::DeclarePreferences()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/10/94
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Declares any preferences that the DocView class needs to decalre

********************************************************************************************/

BOOL DocView::DeclarePreferences()
{
	// NB. Constrain angle must be between 1 and 359 degrees.
	return GetApplication()->DeclareSection(TEXT("Constraints"), 1)
		&& GetApplication()->DeclarePref(TEXT("Constraints"), TEXT("Constrain Angle"),
								  		 &DefaultConstrainAngle,
								  		 (2 * PI) / 360, ((2 * PI) * 359) / 360)

		&& GetApplication()->DeclareSection(TEXT("Rendering"), 2)
		&& GetApplication()->DeclarePref(TEXT("Rendering"), TEXT("BackgroundRendering"),
								  		 &BackgroundRendering, FALSE, TRUE)

		&& GetApplication()->DeclareSection(TEXT("AutoScrolling"), 1)
		&& GetApplication()->DeclarePref(TEXT("AutoScrolling"), TEXT("ScrollToShowMargin"),
										 &ScrollToShowMargin)

		&& GetApplication()->DeclareSection(TEXT("Dragging"), 6)
		&& GetApplication()->DeclarePref(TEXT("Dragging"), TEXT("SolidDragging"),
								  		 &SolidDragging, FALSE, TRUE)
		&& GetApplication()->DeclarePref(TEXT("Dragging"), TEXT("IdleDragDelay"),
								  		 (UINT32*)&IdleDragDelay, 0, 10000)
		&& GetApplication()->DeclarePref(TEXT("Dragging"), TEXT("IdleDragDelay2"),
								  		 (UINT32*)&IdleDragDelay2, 0, 10000)
		&& GetApplication()->DeclarePref(TEXT("Dragging"), TEXT("SolidDragTimeLimit"),
								  		 (UINT32*)&SolidDragTimeLimit, 0, 10000)
		&& GetApplication()->DeclarePref(TEXT("Dragging"), TEXT("OriginOutlineShowAlways"),
								  		 &OriginOutlineShowAlways, FALSE, TRUE)
		&& GetApplication()->DeclarePref(TEXT("Dragging"), TEXT("OriginOutlineShowNonOverlap"),
								  		 &OriginOutlineShowNonOverlap, FALSE, TRUE)
		&& GetApplication()->DeclarePref(TEXT("Dragging"), TEXT("OutlineShowBounds"),
								  		 &OutlineShowBounds, FALSE, TRUE);
}

/********************************************************************************************

>	DocView::DocView() 

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/93
	Inputs:		Pointer to Document to which it belongs
	Outputs:	None
	Returns:	None
	Purpose:	DocView class constructor
				A DocView object is usually created by the Document and is linked into a list 
				of DocViews owned by the Document. So the DocView constructor takes and stores 
				a pointer to the Document object so that it can get to it quickly.
	Errors:		None

	
********************************************************************************************/
/*
	Technical notes:
	In debugging mode, constructor will assert that the Owner document pointer is not null.

********************************************************************************************/

DocView::DocView(Document* pOwnerDoc) :
	m_PushCursor( (Tool_v1*)NULL, _R(IDC_PUSHTOOLCURSOR) )
{
	// Set the flag that indicates if the view is dying or not. Make sure it is not dying
	IsViewDying = FALSE;

	// Implementation...
	ERROR3IF( pOwnerDoc == NULL, "DocView has no Owner Document !!" );

	pDoc = pOwnerDoc;							// Maintain link to Document which owns us
	pViewWindow = NULL;							// Clear pointer to window
	pVState = NULL;
	PushToolCursorID = 0;	// Matt 12/01/2001 - Ensure it is set to a sensible value at creation
	pCurrentDragOp = NULL;						// we aren't dragging just yet
	Scale = 1.0;								// Viewing scale = 1.00

	ViewFlags.BackgroundRender 	= BackgroundRendering;		// Use preference

	ViewFlags.GridShow 			= FALSE;		// Grid not shown
	ViewFlags.GridSnap 			= FALSE;		// Grid not snapping
	ViewFlags.ObjectsSnap 		= FALSE;		// All objects not snapping
	ViewFlags.MagObjectsSnap 	= FALSE;		// Magnetic objects not snapping
	ViewFlags.PrintBorderShow 	= FALSE;		// Print borders not shown
	ViewFlags.LogicalView 		= TRUE;			// Chapters are shown one above the other
	ViewFlags.WorkClickCached 	= FALSE;		// Haven't got a click cached
	ViewFlags.Dragging 			= FALSE;		// We're not dragging anything
	ViewFlags.GuidesSnap		= TRUE;			// Not snapping to guides
	ViewFlags.GuidesShow		= TRUE;			// Show guides by default

		// Get the default quality level
	RenderQuality = Quality::DefaultQuality;

	// Initialise the snap object to NULL.  This is set up in the Snap functions
	pCSnap = NULL;

	// init pointer to rulers
	pRulerPair = NULL;

	// make sure we don't prevent ourselves from rendering
	m_bPreventRenderView = FALSE;

	// Setup solid dragging flags
	m_bSolidDragSupported = FALSE;
	m_bSolidDrag = FALSE;

	// Make ourselves current.
	SetCurrent();

	// Broadcast a message to all that there's a new view on the block . . .
	BROADCAST_TO_ALL(DocViewMsg(this, DocViewMsg::BORN));

}



/********************************************************************************************
>	virtual DocView::~DocView() 

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/93
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	DocView class destructor
	Errors:		None
********************************************************************************************/

DocView::~DocView()
{
	// delete the rulers (must be done first to prevent them receiving doc view dying messages)
	if(pRulerPair)
	{
		PORTNOTETRACE("other","DocView::~DocView - pRulerPair not deleted");
#ifndef EXCLUDE_FROM_XARALX
		delete pRulerPair;
#endif
	}
	pRulerPair=NULL;

	// Set the flag that indicates if the view is dying.
	IsViewDying = TRUE;

	// Make sure that Current and Selected do not refer to the dying DocView
	if (this == Current)	SetNoCurrent();
	if (this == Selected)	Document::SetNoSelectedViewAndSpread();

	Camelot.DeleteRenderRegions(this);

	// Inform parent doc that we are about to die
	pDoc->OnDocViewDying(this);

	// Bye bye to the ViewState object (DON'T deallocate - done by CCamView).
	pVState = NULL;

	// If we have an attached CSnap object, junk it
	if (pCSnap != NULL) delete pCSnap;

	// Broadcast to everyone that this view is a dead duck.
	BROADCAST_TO_ALL(DocViewMsg(this, DocViewMsg::KILLED));
}


/********************************************************************************************
>	BOOL DocView::Init()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/95
	Returns:	FALSE if fails
	Purpose:	Init a doc view
********************************************************************************************/

BOOL DocView::Init()
{
	pRulerPair = NULL;

PORTNOTE("other","Removed RulerPair usage")
#ifndef EXCLUDE_FROM_XARALX
	pRulerPair = new RulerPair();
	ERROR2IF(pRulerPair==NULL,FALSE,"DocView::Init() - failed to create RulerPair");
#endif
	// update indicator in StatusLine
	DialogBarOp::SetSystemStateChanged();

	return TRUE;
}

/********************************************************************************************

>	DocView::ScrollToShowMargin

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/6/1996
	Purpose:	Margins to use (in millipoints) when autoscrolling with a margin

	See Also:	ScrollToShowWithMargin

********************************************************************************************/

UINT32 DocView::ScrollToShowMargin = 18000;	// defaults to 1/4 inch

/********************************************************************************************
>	BOOL DocView::SetScrollOffsets(WorkCoord NewTopLeft, BOOL RedrawNeeded = TRUE)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/7/93
	Inputs:		New scroll offsets
				Flag to control whether redraw is needed or not
	Purpose:	To scroll this view to a new position over the document. The coordinate
				supplied is the coordinate of the top left corner of the viewport onto the
				document. The RedrawNeeded flag is TRUE when any invalid areas created should
				be redrawn immediately and FALSE if they should be ignored.
********************************************************************************************/

BOOL DocView::SetScrollOffsets(WorkCoord NewTopLeft, BOOL RedrawNeeded)
{
	if (!View::SetScrollOffsets(NewTopLeft, RedrawNeeded))
		// Something went wrong - report failure to caller.
		return FALSE;

	WorkCoord RoundedTopLeft = GetScrollOffsets();

	// Tell Oil they've changed
	pViewWindow->SetScrollOffset(RoundedTopLeft, RedrawNeeded);

	return TRUE;
}



/********************************************************************************************

>	void DocView::ScrollToShow(DocCoord* pCoord)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/3/1996
	Inputs:		pCoord		- coord to show in spread coords
	Outputs:	-
	Returns:	-
	Purpose:	Scrolls the currently selected document view to bring this coord into view.
				If this coord is already visible then we do nothing.

				If an ordinate of pCoord is zero then it is ignored and the view willnot
				be scrolled in that direction.

********************************************************************************************/

void DocView::ScrollToShow(DocCoord* pCoord)
{
	ERROR3IF(pCoord == NULL,"pCoord is NULL");
	if (pCoord == NULL) return;

	// call ScrollToShowAux, which does the actual work
	ScrollToShowAux(pCoord, DocCoord(0,0));
}



/********************************************************************************************

>	void DocView::ScrollToShow(DocRect *RectToShow)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/96
	Inputs:		RectToShow - Rectangle to show, in spread coords

	Purpose:	Scrolls the currently selected view to ensure that as much as possible of
				RectToShow is visible. If the entire rect cannot be seen, then it will scroll
				the minimum distance in order to make a part of it visible.

				i.e. Unlike the other ScrollToShow functions, this actually scrolls to show
				the given rectangle, as opposed to single points.

********************************************************************************************/

void DocView::ScrollToShow(DocRect *RectToShow)
{
	ERROR3IF(RectToShow == NULL, "Illegal NULL params");
	
	Spread* pSpread = GetFirstSelectedSpread();

	if (pSpread == NULL)
		return;

	// The TRUE argument for GetPasteBoardRect means that the result gets
	// pixelised. Otherwise the clipping code below doesn't work properly
	DocRect ViewRect = (GetDocViewRect(pSpread)).ToSpread(pSpread, this);

	// If the rect is entirely visible, or as much as possible of it is visible, then don't scroll!
	if (ViewRect.ContainsRect(*RectToShow) || RectToShow->ContainsRect(ViewRect))
		return;

	// Find the distance of the rectangle from each edge of the view rect, and work out
	// the distance we should scroll the view by
	DocCoord Offset(0,0);

	// If we need to scroll in x axis, work out by how much
	// "Need to scroll" means: The target rect lies outside the view rect on *only* one side
	if (
		(RectToShow->lo.x < ViewRect.lo.x || RectToShow->hi.x > ViewRect.hi.x) &&	// Outside
		!(RectToShow->lo.x < ViewRect.lo.x && RectToShow->hi.x > ViewRect.hi.x)		// On one side only
		)
	{
		// Find the distance needed to scroll the left edge of target to the left edge of the view
		// and the right edge to the right edge.
		INT32 Diff1 = RectToShow->lo.x - ViewRect.lo.x;
		INT32 Diff2 = RectToShow->hi.x - ViewRect.hi.x;

		// We need to scroll by the smaller of these 2 distances
		INT32 AbsDiff1 = ABS(Diff1);
		INT32 AbsDiff2 = ABS(Diff2);

		Offset.x = (AbsDiff1 < AbsDiff2) ? Diff1 : Diff2;
	}

	// Repeat the above for the Y axis
	if (
		(RectToShow->lo.y < ViewRect.lo.y || RectToShow->hi.y > ViewRect.hi.y) &&	// Outside
		!(RectToShow->lo.y < ViewRect.lo.y && RectToShow->hi.y > ViewRect.hi.y)		// On one side only
		)
	{
		// Find the distance to scroll to the left and right edges
		INT32 Diff1 = RectToShow->lo.y - ViewRect.lo.y;
		INT32 Diff2 = RectToShow->hi.y - ViewRect.hi.y;

		// We need to scroll by the smaller of these 2 distances
		INT32 AbsDiff1 = ABS(Diff1);
		INT32 AbsDiff2 = ABS(Diff2);

		Offset.y = (AbsDiff1 < AbsDiff2) ? Diff1 : Diff2;
	}

	// Ensure we don't scroll off the pasteboard area. This should really be done by
	// SetScrollOffsets, but it appears that function is complete pants, and I've already
	// had to change over 60 files because of poorly designed fundamental code, so
	// I really don't have time to fix yet more problems.
	DocRect PasteRect = (pSpread->GetPasteboardRect(TRUE)).ToSpread(pSpread, this);
	if (Offset.x < 0)
	{
		if (ViewRect.lo.x + Offset.x < PasteRect.lo.x)
			Offset.x = ViewRect.lo.x - PasteRect.lo.x;
	}
	else
	{
		if (ViewRect.hi.x + Offset.x > PasteRect.hi.x)
			Offset.x = PasteRect.hi.x - ViewRect.hi.x;
	}

	if (Offset.y < 0)
	{
		if (ViewRect.lo.y + Offset.y < PasteRect.lo.y)
			Offset.y = ViewRect.lo.y - PasteRect.lo.y;
	}
	else
	{
		if (ViewRect.hi.y + Offset.y > PasteRect.hi.y)
			Offset.y = PasteRect.hi.y - ViewRect.hi.y;
	}

	// determine if we need to scroll or not,
	if (Offset != DocCoord(0,0))
	{
		WorkCoord ScrollOffset;
		WorkCoord WorkOffset;

		WorkOffset.x = (XLONG) (MakeXLong(Offset.x) * GetViewScale());
		WorkOffset.y = (XLONG) (MakeXLong(Offset.y) * GetViewScale());

		ScrollOffset = GetScrollOffsets();
		ScrollOffset.x = ScrollOffset.x + WorkOffset.x;
		ScrollOffset.y = ScrollOffset.y + WorkOffset.y;

		if (ScrollOffset.x < (XLONG) 0) ScrollOffset.x = (XLONG) 0;
		if (ScrollOffset.y > (XLONG) 0) ScrollOffset.y = (XLONG) 0;

		SetScrollOffsets(ScrollOffset, TRUE);
	}
}



/********************************************************************************************

>	DocView::ScrollToShow(DocRect* pBoundingBox, DocCoord Direction)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/3/1996
	Inputs:		pBoundingBox		- DocRect to show in spread coords
				Direction			- Direction of movement
	Outputs:	-
	Returns:	-
	Purpose:	Scrolls the currently selected view to ensure that edge(s) of pBoundingBox
				are visible. The Direction argument tells us which edges. If the movement
				was towards the top of the screen then we need to make the top edge of the
				DocRect visible.

				If an ordinate of Direction is zero then we ignore it and no scrolling is done
				in that direction.

********************************************************************************************/

void DocView::ScrollToShow(DocRect* pBoundingBox, DocCoord Direction)
{
	ERROR3IF(pBoundingBox == NULL,"pBoundingBox is NULL");
	if (pBoundingBox == NULL) return;

	DocCoord Point;

	if (Direction.x > 0)
		Point.x = pBoundingBox->hi.x;
	else if (Direction.x < 0)
		Point.x = pBoundingBox->lo.x;
	else
		Point.x = 0;

	if (Direction.y > 0)
		Point.y = pBoundingBox->hi.y;
	else if (Direction.y < 0)
		Point.y = pBoundingBox->lo.y;
	else
		Point.y = 0;

	// call ScrollToShowAux which does the actual work
	ScrollToShowAux(&Point, DocCoord(0,0));
}



/********************************************************************************************

>	void DocView::ScrollToShowWithMargin(DocCoord* pCoord)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/3/1996
	Inputs:		pCoord - coord to show in spread coords
	Outputs:	-
	Returns:	-
	Purpose:	Scrolls the currently selected view to show the coord; if the coord in already
				in view then we do nothing.

				If an ordinate of pCoord is zero then we ignore it and do not scroll in that
				direction.

********************************************************************************************/

void DocView::ScrollToShowWithMargin(DocCoord* pCoord)
{
	ERROR3IF(pCoord == NULL,"pCoord is NULL");
	if (pCoord == NULL) return;

	DocCoord Margin;

	double Scale = GetViewScale().MakeDouble();

	Margin.x = (INT32) (ScrollToShowMargin/Scale);
	Margin.y = (INT32) (ScrollToShowMargin/Scale);

	// call ScrollToShowAux, which does all the actual work
	ScrollToShowAux(pCoord, Margin);
}



/********************************************************************************************

>	void DocView::ScrollToShowWithMargin(DocRect *RectToShow)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/96
	Inputs:		RectToShow - Rectangle to show, in spread coords

	Purpose:	Scrolls the currently selected view to ensure that as much as possible of
				RectToShow is visible. If the entire rect cannot be seen, then it will scroll
				the minimum distance in order to make a part of it visible.

				i.e. Unlike the other ScrollToShow functions, this actually scrolls to show
				the given rectangle, as opposed to single points.

********************************************************************************************/

void DocView::ScrollToShowWithMargin(DocRect *RectToShow)
{
	ERROR3IF(RectToShow == NULL, "Illegal NULL params");

	DocRect Temp(*RectToShow);

	double Scale = GetViewScale().MakeDouble();

	INT32 Margin = (INT32) (ScrollToShowMargin / Scale);

	Temp.Inflate(Margin, Margin);

	ScrollToShow(&Temp);
}



/********************************************************************************************

>	void DocView::ScrollToShowWithMargin(DocRect* pBoundingBox, DocCoord Direction)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/3/1996
	Inputs:		pBoundingBox	- DocRect to show, in spread coords
				Direction		- direction of movement
	Outputs:	-
	Returns:	-
	Purpose:	Scrolls the current view to show parts of pBoundingBox. The edge(s) of
				pBoundingBox are determined by Direction, if we have moved objects towards
				the top of the screen then we need to ensure that the top edge of the DocRect
				is visible.

				If an ordinate of Direction is zero then we ignore it and do not scroll in
				that direction.

********************************************************************************************/

void DocView::ScrollToShowWithMargin(DocRect* pBoundingBox, DocCoord Direction)
{
	ERROR3IF(pBoundingBox == NULL,"pBoundingBox is NULL");
	if (pBoundingBox == NULL) return;

	DocCoord Point;
	DocCoord Margin;

	if (Direction.x > 0)
		Point.x = pBoundingBox->hi.x;
	else if (Direction.x < 0)
		Point.x = pBoundingBox->lo.x;
	else
		Point.x = 0;

	if (Direction.y > 0)
		Point.y = pBoundingBox->hi.y;
	else if (Direction.y < 0)
		Point.y = pBoundingBox->lo.y;
	else
		Point.y = 0;

	double Scale = GetViewScale().MakeDouble();
	Margin.x = (INT32) (ScrollToShowMargin/Scale);
	Margin.y = (INT32) (ScrollToShowMargin/Scale);

	// call ScrollToShowAux, which does all the actual work
	ScrollToShowAux(&Point, Margin);
}

/********************************************************************************************

>	void DocView::ScrollToShowAux(DocCoord *pCoord, DocCoord Margin)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/3/1996
	Inputs:		pCoord		- coord to show in spread coords
				Margin		- margin to give between point and edge of view
	Outputs:	-
	Returns:	-
	Purpose:	Scrolls the view to ensure that pCoord is in the view, with a margin of Margin
				between it and the edges of the view.

				If an ordinate of pCoord is zero then we ignore it and do not scroll in that
				direction.

********************************************************************************************/

void DocView::ScrollToShowAux(DocCoord *pCoord, DocCoord Margin)
{
	ERROR3IF(pCoord == NULL,"pCoord is NULL");
	if (pCoord == NULL) return;

	ERROR3IF(Margin.x < 0,"Negative X margin");
	if (Margin.x < 0) return;

	ERROR3IF(Margin.y < 0,"Negative Y margin");
	if (Margin.y < 0) return;
	
	Spread* pSpread = GetFirstSelectedSpread();

	if (pSpread == NULL)
	{
		// do nothing
	}
	else
	{
		DocRect ViewRect;
		DocRect PasteRect;
		DocCoord Offset;

		// The TRUE argument for GetPasteBoardRect means that the result gets
		// pixelised. Otherwise the clipping code below doesn't work properly
		ViewRect = (GetDocViewRect(pSpread)).ToSpread(pSpread, this);
		PasteRect = (pSpread->GetPasteboardRect(TRUE)).ToSpread(pSpread, this);

		Offset = DocCoord(0,0);

		// suppose the view rectangle is smaller than two margins width, either in height or
		// width, we'll need to alter the margin to half the height or width as appropriate
		{
			INT32 Width = ViewRect.hi.x - ViewRect.lo.x;
			INT32 Height = ViewRect.hi.y - ViewRect.lo.y;

			if (Width < 2*Margin.x)
				Margin.x = Width/2;

			if (Height < 2*Margin.y)
				Margin.y = Height/2;
		}
				
		// only scroll in the X direction if pCoord->x is not zero
		if (pCoord->x != 0)
		{
			if (pCoord->x >= PasteRect.hi.x-Margin.x) pCoord->x = PasteRect.hi.x-Margin.x;
			if (pCoord->x <= PasteRect.lo.x+Margin.x) pCoord->x = PasteRect.lo.x+Margin.x;
			
			if (pCoord->x < (ViewRect.lo.x+Margin.x))
			{
				// need to scroll to the left
				Offset.x = pCoord->x - (ViewRect.lo.x + Margin.x);
			}
			
			if (pCoord->x > (ViewRect.hi.x-Margin.x))
			{
				// need to scroll to the right
				Offset.x = pCoord->x - (ViewRect.hi.x - Margin.x);
			}
		}
		else
		{
			Offset.x = 0;
		}

		// only scroll in the Y direction if pCoord->y is not zero
		if (pCoord->y != 0)
		{
			if (pCoord->y >= PasteRect.hi.y-Margin.y) pCoord->y = PasteRect.hi.y-Margin.y;
			if (pCoord->y <= PasteRect.lo.y+Margin.y) pCoord->y = PasteRect.lo.y+Margin.y;
				
			if (pCoord->y < (ViewRect.lo.y+Margin.y))
			{
				// need to scroll down
				Offset.y = pCoord->y - (ViewRect.lo.y+Margin.y);
			}
			
			if (pCoord->y > (ViewRect.hi.y-Margin.y))
			{
				// need to scroll up
				Offset.y = pCoord->y - (ViewRect.hi.y-Margin.y);
			}
		}
		else
		{
			Offset.y = 0;
		}
		
		// determine if we need to scroll or not,
		if (Offset != DocCoord(0,0))
		{
			WorkCoord ScrollOffset;
			WorkCoord WorkOffset;

			WorkOffset.x = (XLONG) (MakeXLong(Offset.x) * GetViewScale());
			WorkOffset.y = (XLONG) (MakeXLong(Offset.y) * GetViewScale());

			ScrollOffset = GetScrollOffsets();
			ScrollOffset.x = ScrollOffset.x + WorkOffset.x;
			ScrollOffset.y = ScrollOffset.y + WorkOffset.y;

			if (ScrollOffset.x < (XLONG) 0) ScrollOffset.x = (XLONG) 0;
			if (ScrollOffset.y > (XLONG) 0) ScrollOffset.y = (XLONG) 0;

			SetScrollOffsets(ScrollOffset, TRUE);
		}
		// else do nothing
	}
}

/********************************************************************************************

>	BOOL DocView::ViewStateChanged()

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/93
	Purpose:	Inform the DocView that the ViewState has been changed in some way.
				When this function detects a new ViewState object (IsNewView = TRUE) it sets
				the scroll offsets to the top left hand corner of the first page in the
				document.

********************************************************************************************/
/*
Technical notes:
	This function is called soon after construction of every DocView. At this time the CCamView 
	has written lots of useful stuff into the ViewState (like the size of pixels) and so we
	can now calculate the extents of the document and give them back to CCamView.

********************************************************************************************/

BOOL DocView::ViewStateChanged()
{
	// Local variables...
	DocCoord	ExtentLo;						// coord of bottom-left of logical extent
	DocCoord	ExtentHi;						// coord of top-right of logical extent

	DialogBarOp::SetSystemStateChanged();

	// Implementation...
	ERROR2IF(this==NULL,FALSE,"DocView member func called on NULL pointer");
	ERROR2IF(pDoc==NULL,FALSE,"ViewStateChanged: There MUST be an	owner doc for this view!");
	
	// Read DocCoords extent of document & set extent in platform-indy ViewState struct.
	pDoc->GetExtents(&ExtentLo, &ExtentHi, &PhysExtent, this);
	SetExtent(ExtentLo, ExtentHi);				

	// Set extent in platform-indy ViewState struct
	ViewFlags.WorkClickCached = FALSE;			
	return TRUE;
}


/*********************************************************************************************

> 	BOOL DocView::SetViewScale(FIXED16 NewScale)

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	31/5/93
	Inputs:		New view scale factor
    Purpose:    Set the viewing scale factor for this view.
				(Also, sets up the scaled pixel size in DocCoord if this View is current.
                   			                                     
*********************************************************************************************/

BOOL DocView::SetViewScale(FIXED16 NewScale)
{
	if (!View::SetViewScale(NewScale))
		// Error
		return FALSE;

	// Tell everyone that a DocView scale has changed.
	BROADCAST_TO_ALL(DocViewMsg(this,DocViewMsg::SCALECHANGED));

	return TRUE;
}



/********************************************************************************************
>	void DocView::OnNewView()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called by CCamView as part of its "initial update" function provided that
				this view onto a document has not been previously saved and reloaded.
				Sets the defaults, eg. scroll offset, for newly created views.
	Errors:		-
	SeeAlso:	DocView::OnLoadedView; CCamView::OnInitialUpdate; struct ViewState
********************************************************************************************/

void DocView::OnNewView()
{
	// Only set the scroll offsets if they have not been set yet.
	// eg. When a document is loaded it is possible that it has already set the Scroll Offsets
	// to something sensible.
	WorkCoord CurrentOffset = GetScrollOffsets();
	if ((CurrentOffset.x==0) && (CurrentOffset.y==0))
	{
		// Find the first page in the document and set the scroll offsets so that the top
		// left corner of the page is in the top left corner of the view.
		Node* pSearchNode = Node::DocFindFirstDepthFirst(pDoc);
		while (pSearchNode != NULL && !pSearchNode->IsSpread())
			pSearchNode = pSearchNode->DocFindNextDepthFirst();

		ERROR3IF(pSearchNode == NULL, "ViewStateChanged: Can't find first spread.");

		if (pSearchNode != NULL)
		{
			Spread *pSpread = (Spread *)pSearchNode;

			// OK, found the spread. Go look for the first page
			Page *pPage = pSpread->FindFirstPageInSpread();
			ERROR3IF(pPage == NULL, "ViewStateChanged: Can't find first page in spread.");

			if (pPage != NULL)
			{
				// OK, found the first page.
				// Read its position in the spread, translate into Doc Coord space
				DocRect PageRect = pPage->GetPageRect();

				DocCoord TopLeft(PageRect.lo.x, PageRect.hi.y);
				pSpread->SpreadCoordToDocCoord(&TopLeft);

				// Then translate to work coords
				WorkCoord TopLeftWrk = TopLeft.ToWork(pSpread, this);
			
				// Shift the corner of the page 1/8th of an inch into the window.
				TopLeftWrk.x -= (72000/8);
				TopLeftWrk.y += (72000/8);

				// Only set the new scroll offsets if valid. Otherwise we will get an Error2.
				// We may try to set illegal values now that we allow zero sized pasterboards.
				if (TopLeftWrk.x >= (XLONG)0 && TopLeftWrk.y <= (XLONG)0)
				{
					// And scroll to place that offset at the top left of the view window
					SetScrollOffsets(TopLeftWrk, FALSE);
				}
			}
		}
	}

	DialogBarOp::SetSystemStateChanged();

	// Change the View State so that it is no longer marked as new
PORTNOTE("other","Removed ViewState usage")
#ifndef EXCLUDE_FROM_XARALX
	if (pVState!=NULL)
		pVState->IsNewView = FALSE;
#endif
}



/********************************************************************************************
>	void DocView::OnLoadedView()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called by CCamView as part of its "initial update" function provided that
				this view onto a document has been previously saved and reloaded.
				Currently does nothing, but one day - who knows.
	Errors:		-
	SeeAlso:	DocView::OnNewView; CCamView::OnInitialUpdate; struct ViewState
********************************************************************************************/

void DocView::OnLoadedView()
{
	// empty for now
}



/********************************************************************************************

>	void DocView::SetExtent(DocCoord lolog, DocCoord hilog)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/93
	Inputs:		Two DocCoords describing the low and high corners of a rectangle in logical
				space describing the full extent of the document. I.E. the bottom-left corner
				of the last chapter and the top-right corner of the first.
	Purpose:	Informs the DocView that the extent of the document has changed in some way and 
				that the view must be altered to take this into account (namely the 
				scrollbars).

********************************************************************************************/

void DocView::SetExtent(DocCoord lolog, DocCoord hilog)
{
	if (ViewFlags.LogicalView)
	{
		// "Pixelise" the extent DocCoords.
		// Effectively, this helps ensure that the spread will be aligned to a whole pixel boundary
		// and allows both GDraw and GDI to consistently plot the same pixels when rendering
		// the same primitive
		lolog.Pixelise(this);
		hilog.Pixelise(this);

		// Logical view means that chapters are shown in a contiguous column
		// Convert the extent given in DocCoord to WorkCoords...
		pVState->WorkAreaExtent.lo = lolog.ToWork(pDoc, this);
		pVState->WorkAreaExtent.hi = hilog.ToWork(pDoc, this);

		// If we know where the OIL view is then tell it that the extents have changed...
		if (pViewWindow)
		{
			pViewWindow->SetWorkAreaExtent(pVState->WorkAreaExtent, FALSE);
		}
	}
	else
	{
		// Physical view means that chapters are shown in their true positions in Document space
		ERROR3("DocView::SetExtent - Can't do physical views yet!");
	}
}






/********************************************************************************************

>	WorkRect DocView::GetViewRect()

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Returns:	A WorkCoords rectangle describing the viewport onto the document which this
				view is displaying. I.E. The top left corner of this rectangle is the same as
				the value returned by GetScrollOffsets().
	Purpose:	To find the rectangle describing the viewport onto the document.

********************************************************************************************/

WorkRect DocView::GetViewRect()
{
	ERROR3IF(pViewWindow == NULL, "DocView::GetViewRect called when we have no ViewWindow");

	// Get the width and height of the view in OS units
	INT32 ClientWidth = 0;
	INT32 ClientHeight = 0;
	pViewWindow->GetClientSize(&ClientWidth, &ClientHeight);

	OilRect OilViewRect;
	OilViewRect.lo.x = 0;
	OilViewRect.lo.y = -ClientHeight;
	OilViewRect.hi.x = ClientWidth;
	OilViewRect.hi.y = 0;

	// Convert the OSRect into Workarea coords
//	WorkCoord pos;
//	pViewWindow->GetScrollOffset(&pos);
//	pVState->SetScrollPos(pos);
//	return OilViewRect.ToWork(pos);
	return OilViewRect.ToWork(pVState->GetScrollPos());
}




/********************************************************************************************

>	DocRect DocView::GetDocViewRect(Spread* pSpread)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/7/93
	Inputs:		pSpread - the spread that we are interested in
	Returns:	A DocCoords rectangle describing the viewport onto the document which this
				view is displaying. I.E. The top left corner of this rectangle is the same as
				the value returned by GetScrollOffsets() except in DocCoords.
	Purpose:	To find the rectangle describing the viewport onto the document.

********************************************************************************************/

DocRect DocView::GetDocViewRect(Spread* pSpread)
{
	ERROR3IF(pViewWindow == NULL, "DocView::GetDocViewRect called when we have no view");

	INT32 ClientWidth = 0;
	INT32 ClientHeight = 0;

	// Get the width and height of the view in OS units
	pViewWindow->GetClientSize(&ClientWidth, &ClientHeight);

	OilRect OilViewRect;
	OilViewRect.lo.x = 0;
	OilViewRect.lo.y = -ClientHeight;
	OilViewRect.hi.x = ClientWidth;
	OilViewRect.hi.y = 0;

	// Convert the OSRect into Doc coords
	return OilViewRect.ToDoc(pSpread, this);
}



/*********************************************************************************************

> 	void DocView::SetForeBackMode(BOOL NewFlag)

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	31/5/93
	Inputs:		NewFlag - New bg render flag.
    Purpose:    Set the background render flag for this view.
    			Also sets the new value to be used as the default preference.
                   			                                     
*********************************************************************************************/

void DocView::SetForeBackMode(BOOL NewFlag)
{
	ViewFlags.BackgroundRender = NewFlag;
	
	// Update the preference to use this
	// Ensure that we either set TRUE or FALSE and not -1 or 0.
	// The preference is limited to 0 and 1 values and so -1 is illegal
	if (NewFlag == FALSE)
		BackgroundRendering = FALSE;
	else
		BackgroundRendering = TRUE;
}



/*********************************************************************************************

> 	BOOL DocView::GetForeBackMode()

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	31/5/93
    Returns:   	TRUE if background rendering is on
    Purpose:    To find out if background rendering is on
                   			                                     
*********************************************************************************************/

BOOL DocView::GetForeBackMode()
{
	// As the flag is a BOOL in a bitfield, it seems to like returning -1 or 0
	// Therefore, ensure that we always return TRUE or FALSE

	if (ViewFlags.BackgroundRender == FALSE)
		return (FALSE);
	else
		return (TRUE);

}




/********************************************************************************************

>	void DocView::RenderView(RenderRegion* pRRegion, Matrix& ViewTransform, Spread *pSpread,
								BOOL bPrintPaper, BOOL fDeleteRegionAfter = TRUE,
								BOOL bForceImmediate = TRUE)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/93
	Inputs:		pRRegion - the region to use for rendering the spread
				ViewTransform - the matrix to use when transforming document coordinates
				into Window coordinates.
				pSpread - the spread we are rendering with this render region.
				bPrintPaper should be TRUE if paper-like items need to be rendered e.g. page
				marks, blobs, etc, or FALSE otherwise. A TRUE value also will enable background
				rendering, so long as Viewflags.BackgroundRender is also set.
				fDeleteRegionAfter  ---  by default TRUE - if so then this function will
				deallocate the RenderRegion passed to it when it has been comletely rendered.
	Purpose:	To render a Document through a given RenderRegion.
				The dominating factor in this routine is that it renders the document with as 
				little time overhead as possible. Obviously, this is mainly down to 
				RenderRegions and the tree scanning routines working fast but we can do a bit 
				to help.
				This routine is called directly by other functions in DocView and externally
				from the Background redraw Idle event processor.

********************************************************************************************/
/*
Technical notes:
				This routine stores and restores its position in the document tree from the 
				RenderRegion. If it ever picks up a NULL it assumes that the RenderRegion has 
				just been created and renders the background to the document, eg. pages, 
				pasteboard, etc...
				When the routine hits the end of the tree it will pass a NULL in for the 
				RenderRegion to store as its state. This can be used a a signal that rendering 
				in this RenderRegion is complete and that the RenderRegion can be destroyed.

				The control flow path through this routine is very carefully laid out. When
				the call to GetRenderState returns NULL, we assume that the RenderRegion is 
				newly created and in that case we render all paper nodes and find the first 
				ink	node before dropping through to the renderer proper.
				When GetRenderState returns a pointer to a node then this RenderRegion has
				already rendered some ink nodes in the tree and so we must carry on where we
				left off. In this case the control path must be VERY CLEAN - it must not
				alter the tree pointer or change any attributes because the RenderRegion has
				records of all of these things set from the previous session.

********************************************************************************************/

void DocView::RenderView(RenderRegion* pRRegion, Matrix& ViewTransform,
						 Spread *pSpread, BOOL bPrintPaper,
						 BOOL fDeleteRegionAfter,
						 BOOL bForceImmediate)
{
	Node* pNode = NULL;
	BOOL CleanUp = TRUE;							// TRUE if cleanup at end, FALSE if dont
	BOOL IsMoreBands = FALSE;
	DocRect ClipRect;

	BOOL HasRendered = FALSE;
	BOOL Continue = TRUE;
	BOOL bPaperRenderedThisTime = FALSE;

	// Record time when rendering started
	StartTime.Sample();

	// Scroll offsets etc may have changed, so transfer matrix.
	pRRegion->SetMatrix(ViewTransform);

	// Prepare to find out whether any bounds in selection are changed during redraw
	m_bBoundsChanged = FALSE;

	// Start the first band
	if (!pRRegion->SetFirstBand())
	{
		goto ExitRenderNoStop;
	}

	do
	{
		// See if we are actually waiting for memory
		if (pRRegion->IsWaitingForRAM)
		{
			// See if we can get the memory yet ?
			pRRegion->GetNextBand();

			if (pRRegion->IsWaitingForRAM)
				break;	// Nope still not enough
		}

		// Tell the RenderRegion that I am going to start rendering.
		if (!pRRegion->StartRender())
		{
			TRACEALL( _T("StartRender failed in RenderView\n") );
																// will be deleted quickly
			goto ExitRenderNoStop;								// skip StopRender call
		}

		// If the state is NULL I take this to mean that the RenderRegion has only just been 
		// created and I will therefore ensure that the window background is rendered by rendering 
		// all the "paper" parts of the document. That is, the pages, chapters, pasteboard, etc...
		if (pRRegion->IsPaperRendered == FALSE)
		{
			if (bPrintPaper)
			{
				// Render all paper nodes from there into the real render region
				pRRegion->SaveContext();
				RenderPaper(pRRegion, pSpread);
				pRRegion->RestoreContext();

				if (pRRegion->NeedsOSPaper)
				{
					// This render region uses the OS to render paper, so if we don't
					// render anything except paper then there is no need for e.g.
					// GRenderRegions to blit anything.  Hence we call this function
					// to clear the changed bbox.
					pRRegion->SetClean(TRUE, FALSE);
				}
			}

#ifdef _DEBUG
// TEMP BODGE! DO NOT RELEASE!
//if (pRRegion->IsKindOf(CC_RUNTIME_CLASS(GRenderRegion)))
//	((GRenderRegion*)pRRegion)->DebugTrace();
#endif 

			// The Paper is now done
			pRRegion->IsPaperRendered = TRUE;
			bPaperRenderedThisTime = TRUE;
		}

		// ------------------------------------------
		// Call the central tree renderer...
		Continue = pRRegion->RenderTree(pSpread,
										ViewFlags.BackgroundRender && !bForceImmediate		// Only timeslice if bgr && ! rendering paper
										);
		// ------------------------------------------

		// Reset state to new posn in tree.
		BOOL bRendered = pRRegion->StopRender();
		HasRendered = HasRendered || bRendered;

		// Dragging blobs/boxes.
		Operation* pDragOp = Operation::GetCurrentDragOp();
		if (pDragOp) pDragOp->RenderDragBlobs(pRRegion->GetClipRect(), pSpread, m_bSolidDrag);

		// Assume we ran out of time
		IsMoreBands = FALSE;

		// If we did not run out of time, prepare the next band
		if (Continue)
		{
			IsMoreBands = pRRegion->GetNextBand();

			if (pRRegion->IsWaitingForRAM)
				break;		// Leave this region for a while
		}
	} while (IsMoreBands);

	// Could jump straight to here if the render region was waiting for some ram to show up

ExitRenderNoStop:
	pNode = pRRegion->GetRenderState();

	// Find out whether all bounds have become cached during this render
	// If so, update the SelRange and tell all tools the SelRange has changed
	// This handles the situation where effects change their bounds during rendering
	if (m_bBoundsChanged)
	{
		// Only broadcast to the rest of the program if we're not trying to be quick
		Camelot.FindSelection()->Update(!Operation::GetQuickRender(NULL));
	}

	// Find out the rectangle of the whole unbanded region
	DocRect BlobClipRect = pRRegion->GetRegionRect();
	BOOL IsRegionFinished = CleanUp && !pRRegion->IsWaitingForRAM && pRRegion->IsLastBand && !pNode;

	// If the region has finished rendering, delete it, otherwise update its context node.
	if (IsRegionFinished)
	{
		if (fDeleteRegionAfter)
		{
			// Unlink the RenderRegion from list
			BOOL InList = Camelot.DeleteRenderRegion(pRRegion);
			ERROR2IF(!InList, (void)0, "DocView::RenderView tried to delete a RenderRegion not in list");
			delete pRRegion;
		}
	}

	// Draw the blobs if we are the "selected" view.
	// And we're not solid dragging...
	if (this == Selected && !(pCurrentDragOp!=NULL && m_bSolidDrag))
	{
		// Tool & selection blobs.
		// HasRendered really means "Has blitted and wiped out previous blobs within the changedrect..."
		// So we should redraw the blobs inside the changed rect (note that the paper chanegd rect
		// needs to be derived differently than the ink changed rect)
		// BUT we don't know here what the changed rect is!
		// SO, all we can do is wait until the render is over then put the blobs back on
//		if (HasRendered || bPaperRenderedThisTime))
		if (IsRegionFinished)
		{
			Tool* pTool = Tool::GetCurrent();
			if (pTool)
			{
				pTool->RenderToolBlobs(pSpread, &BlobClipRect);
			}

			// Render the Currently selected objects selection blobs
			RenderSelectBlobs(BlobClipRect, pSpread);
		}
	}
}




/********************************************************************************************

>	void DocView::ContinueRenderView(RenderRegion* pRRegion, Chapter* pChapter,
									 BOOL fRenderPaper = TRUE,
									 BOOL fDeleteRegionAfter = TRUE,
									 BOOL bForceImmediate = FALSE)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/93
	Inputs:		Pointer to RenderRegion which needs rendering to be performed.
				Pointer to chapter which RenderRegion represents.
				fRenderPaper		---	 if TRUE then render paper, blobs etc.
										 By default TRUE.
				fDeleteRegionAfter  ---  if TRUE then this function will deallocate the
										 RenderRegion passed to it when it has been
										 completely rendered.  By default TRUE.
	Outputs:	None
	Returns:	None
	Scope:		Public
	Purpose:	To re-start rendering a Document through a given RenderRegion. This call is
				different from RenderView because it may need to re-compute the Doc to OS
				matrix.
	Errors:		None

********************************************************************************************/

void DocView::ContinueRenderView(RenderRegion* pRRegion, Spread* pSpread,
								 BOOL fRenderPaper /* = TRUE */,
								 BOOL fDeleteRegionAfter /* = TRUE */,
								 BOOL bForceImmediate /*= FALSE*/)
{
	// Diccon 10/5/2000 my awful flag that prevents rendering of the view from occurring
	// whilst a brush stroke is in progress
	if (m_bPreventRenderView == TRUE)
		return;

	// Remove the drag blobs from this region, if there are any.
	if (fRenderPaper)
	{
		Operation* pDragOp = Operation::GetCurrentDragOp();
		if (pDragOp)
			pDragOp->RenderDragBlobs(pRRegion->GetClipRect(), pSpread, m_bSolidDrag);
	}

	// Render the document in one operation.
	Matrix mxConv = ConstructRenderingMatrix(pSpread);

	if (OnTopCount == 0 && GetColourPlate() != NULL && !GetColourPlate()->IsDisabled())
	{
		ColourPlateType PlateType = GetColourPlate()->GetType();
		if (PlateType != COLOURPLATE_NONE && PlateType != COLOURPLATE_COMPOSITE)
		{
			// We have to do this before the render as the RenderSimpleView call
			// will delete the render region
			DocRect BlobClipRect = pRRegion->GetRegionRect();
			DocRect DragClipRect = pRRegion->GetClipRect();
			
			// Do a printer style simple/complex ScanningRenderRegion type render
			RenderSimpleView(pRRegion, mxConv, pSpread, fRenderPaper);

			// Dragging blobs/boxes.
			if (fRenderPaper)
			{
				Operation* pDragOp = Operation::GetCurrentDragOp();
				if (pDragOp) pDragOp->RenderDragBlobs(DragClipRect, pSpread, m_bSolidDrag);
			}

			// Draw the blobs if we are the "selected" view.
			// And we're not solid dragging...
			if (fRenderPaper && this == Selected && !(pCurrentDragOp!=NULL && m_bSolidDrag))
			{
				Tool* pTool = Tool::GetCurrent();
				if (pTool) pTool->RenderToolBlobs(pSpread, &BlobClipRect);

				// Render the Currently selected objects selection blobs
				RenderSelectBlobs(BlobClipRect, pSpread);
			}
			return;
		}
	}
	
	RenderView(pRRegion, mxConv, pSpread, fRenderPaper, fDeleteRegionAfter, bForceImmediate);
}




/********************************************************************************************

>	void DocView::RenderEntireView(RenderRegion* pRender, Spread* pSpread)

	Author: 	Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/2000
	Inputs:		pRRegion - the region to use for rendering the spread
			
	Purpose:	To render a Document through a given RenderRegion.
				This differs from your regular RenderView service in that it pays no mind as to
				how long things take.  Once you start this function off it will run until it 
				renders the lot.
				The reason for this is that it is designed for use with the brush tool.  Here we
				will need to render the entire view into an offscreen bitmap which is then
				used to with the brush.  It serves as an alternative when we cannot simply blit
				the contents of the screen.
				I don't really recommend using this function in other circumstances as if you
				have a complex document it will probably lock up for a while.
********************************************************************************************/

void DocView::RenderEntireView(RenderRegion* pRRegion, Spread* pSpread)
{
	DocRect ClipRect;

	Matrix ViewTransform = ConstructRenderingMatrix(pSpread);
	// Scroll offsets etc may have changed, so transfer matrix.
	pRRegion->SetMatrix(ViewTransform);

	// Get the region to clip all the rendering to
	ClipRect = pRRegion->GetClipRect();

	// If the state is NULL I take this to mean that the RenderRegion has only just been 
	// created and I will therefore ensure that the window background is rendered by rendering 
	// all the "paper" parts of the document. That is, the pages, chapters, pasteboard, etc...
	if (pRRegion->IsPaperRendered == FALSE)
	{
		// Render all paper nodes from there into the real render region
		pRRegion->SaveContext();
		RenderPaper(pRRegion, pSpread);
		pRRegion->RestoreContext();

		// The Paper is now done
		pRRegion->IsPaperRendered = TRUE;
	}

	// Now render all the ink (non-paper) parts of the document.
	pRRegion->RenderTree(pSpread, FALSE);

	// Reset state to new posn in tree.
	pRRegion->StopRender();
}


/********************************************************************************************

>	void DocView::GetViewParams(FIXED16* pReturnViewScale,
								FIXED16* pReturnPixelScaleX,
								FIXED16* pReturnPixelScaleY,
								WorkCoord* pReturnScrollOffset) 

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/5/93
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	This function returns the parameters which control the transformation of 
				coordinates between Document space and OS space. These parameters can be used 
				to build transformation matrices to perform the transformation in either 
				direction.
	Errors:		None

********************************************************************************************/
/*
void DocView::GetViewParams(FIXED16* pReturnViewScale,
							FIXED16* pReturnPixelScaleX,
							FIXED16* pReturnPixelScaleY,
							WorkCoord* pReturnScrollOffset) 
{
	ENSURE( pViewWindow != NULL, "DocView::GetViewParams() called when there we no ViewWindow");

	*pReturnViewScale = Scale;
	*pReturnPixelScaleX = pVState->PixelXScale;
	*pReturnPixelScaleY = pVState->PixelYScale;
	*pReturnScrollOffset = pVState->GetScrollPos();
}
*/

/********************************************************************************************

>	static DocRect DocView::ChapterClip(DocRect& DocClip, Chapter* pChapter)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/9/93
	Inputs:		The DocRect to Clip, and a Chapter to clip it with.
	Outputs:	-
	Returns:	A Clipped DocRect
	Purpose:	Clips the DocRect so that it does not go outside the Chapter Bounds.
				This is used to try and stop the problems with DocCoords that are not within
				any chapters (ie. in OffPaste area).
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
/*
static DocRect DocView::ChapterClip(DocRect& DocClip, Chapter* pChapter)
{
	DocRect ChapterRect = pChapter->GetPasteboardRect();

	ChapterRect.lox = MAX(DocClip.lox, ChapterRect.lox);
	ChapterRect.loy = MAX(DocClip.loy, ChapterRect.loy);
	ChapterRect.hix = MIN(DocClip.hix, ChapterRect.hix);
	ChapterRect.hiy = MIN(DocClip.hiy, ChapterRect.hiy);
                
	return ChapterRect;
}
*/




/*********************************************************************************************

> 	static DocView *DocView::GetCurrent()

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	24/5/93
    Returns:   	Pointer to the current DocView object.
    Purpose:    Find the current DocView object which SHOULD have been set as one of the very 
    			first actions during event processing.             
                   			                                     
*********************************************************************************************/

DocView *DocView::GetCurrent()
{
	View *pView = View::GetCurrent();

	// Make sure it really is a DocView;
	if ((pView == NULL) || (pView->IS_KIND_OF(DocView)))
		// Yes - return it.
		return (DocView *) pView;

	// Complain!
	//ERROR2(NULL, "DocView::GetCurrent called when current view is not a DocView!");
	TRACEALL( _T("DocView::GetCurrent called when current view is not a DocView!\n") );
	return NULL;
}


/********************************************************************************************

>	void DocView::SetViewPixelSize()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/10/95
	Purpose:	Sets up the normal and scaled pixel sizes according to this view.
	SeeAlso:	OilCoord::SetPixelSize; DocCoord::SetScaledPixelSize

********************************************************************************************/

void DocView::SetViewPixelSize()
{
	// Set the scaled pixel size
	ScaledPixelWidth = PixelWidth / Scale;
	ScaledPixelHeight = PixelHeight / Scale;
}

/*********************************************************************************************
> 	static CNativeWnd* DocView::GetCurrentRenderWindow()

    Author: 	Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>	(modified to use DocView::GetRenderWindow() by Ed 1/9/95)
    Created:	15 Oct 93
    Returns:   	Pointer to the current CNativeWnd window object which is being rendered.
    Purpose:    Useful function allowing fairly direct access to CCamView's render window,
    			which comes in handy on the odd occasion.
	Errors:		Returns NULL if there isn't one.
*********************************************************************************************/

CNativeWnd *DocView::GetCurrentRenderWindow()
{
	DocView* pCurrent=GetCurrent();
	if (pCurrent==NULL)
		return NULL;

	return pCurrent->GetRenderWindow();
}


/*********************************************************************************************
> 	CNativeWnd* DocView::GetRenderWindow()

    Author: 	Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
    Created:	1/9/95
    Returns:   	Pointer to CNativeWnd (else NULL if none)
    Purpose:	Get a pointer to a CNativeWnd window object associated with a DocView
*********************************************************************************************/

CNativeWnd* DocView::GetRenderWindow()
{
	if (pViewWindow==NULL)
		return NULL;

	return pViewWindow->GetRenderWindow();
}


/*********************************************************************************************

> 	static DocView *DocView::GetSelected()

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	24/5/93
    Returns:   	Pointer to the selected DocView object.
    Purpose:    Find the selected DocView object which SHOULD have been set as one of the very 
    			first actions during event processing.
                   			                                     
*********************************************************************************************/

DocView *DocView::GetSelected()
{
	return Selected;
}



	
/*********************************************************************************************

> 	BOOL DocView::SetCurrent()

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	24/5/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if function set current correctly
				FALSE otherwise (then an Error has been set)
    Purpose:    Make this object be the 'current' DocView.

*********************************************************************************************/

BOOL DocView::SetCurrent()
{
	return View::SetCurrent();
}													  



/*********************************************************************************************

> 	static void DocView::SetNoCurrent()

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	24/5/93
	Inputs:		-
	Outputs:	-
	Returns:	-
    Purpose:    Set the current DocView pointer to be NULL, i.e., there is no current
    			DocView object.
                   			                                     
*********************************************************************************************/

void DocView::SetNoCurrent()
{
	View::SetNoCurrent();
}



/*********************************************************************************************

>	static void DocView::SetSelectedInternal(DocView *NewSelected)

    Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:	14/2/95

	Inputs:		NewSelected - NULL, or the new selected DocView

    Purpose:    This is an INTERNAL function, which MUST NOT BE CALLED by anything
				other than Document::SetSelectedViewAndSpread.

				It sets the new selected DocView, but DOES NOT broadcast a message.
				The broadcast follows later, via SendSelectedMessage.

				It can be used to set No Selected DocView (NewSelected = NULL)

				It also sets this DocView as the Current DocView.

	SeeAlso:	Document::SetSelectedViewAndSpread; DocView::SendSelectedMessage

*********************************************************************************************/

void DocView::SetSelectedInternal(DocView *NewSelected)
{
	Selected = NewSelected;
	
	if (NewSelected == NULL)
		DocView::SetNoCurrent();
	else
		NewSelected->SetCurrent();
}



/*********************************************************************************************

>	static void DocView::SendSelectedMessage(DocView *OldSelected, DocView *NewSelected);

    Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:	14/2/95

	Inputs:		OldSelected - Points to the previously selected DocView (may be NULL)
				NewSelected - Points to the new selected DocView (may be NULL)

    Purpose:    This is an INTERNAL function, which MUST NOT BE CALLED by anything
				other than Document::SetSelectedViewAndSpread.

				It is used to broadcast a DocViewMsg (SELCHANGED) to inform of a selection
				change. It follows a SetSelectedInternal call (a bit later, so that the
				message is only broadcast once the document, view, and spread states have
				all been updated correctly)

	SeeAlso:	Document::SetSelectedViewAndSpread; DocView::SetSelectedInternal

*********************************************************************************************/

void DocView::SendSelectedMessage(DocView *OldSelected, DocView *NewSelected)
{
	BROADCAST_TO_ALL(DocViewMsg(OldSelected, NewSelected, DocViewMsg::SELCHANGED));
}



/********************************************************************************************
>	INT32 DocView::GetZoomTableIndex() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/94
	Inputs:		-
	Outputs:	-
	Returns:	Returns the value of the zoom table index.
	Purpose:	
	Errors:		-
	SeeAlso:	DocView::SetZoomTableIndex
********************************************************************************************/

INT32 DocView::GetZoomTableIndex() const
{
	return ZoomTableIndex;
}



/********************************************************************************************
>	void DocView::SetZoomTableIndex(INT32 nIndex)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/94
	Inputs:		The new index value.
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		-
	SeeAlso:	DocView::GetZoomTableIndex
********************************************************************************************/

void DocView::SetZoomTableIndex(INT32 nIndex)
{
	ZoomTableIndex = nIndex;
}



/********************************************************************************************
>	INT32 DocView::GetPrevZoomIndex() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Returns the index into the zoom tool's tabel of a previous zoom on this
				DocView.
	Errors:		-
	SeeAlso:	DocView::SetPrevZoomIndex
********************************************************************************************/

INT32 DocView::GetPrevZoomIndex() const
{
	return PrevIndex;
}



/********************************************************************************************
>	void DocView::SetPrevZoomIndex(INT32 nIndex)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/94
	Inputs:		The previous zoom index.
	Outputs:	-
	Returns:	-
	Purpose:	Sets the zoom table index of a previous zoom onto this DocView.
	Errors:		-
	SeeAlso:	DocView::GetPrevZoomIndex
********************************************************************************************/

void DocView::SetPrevZoomIndex(INT32 nIndex)
{
	PrevIndex = nIndex;
}





/********************************************************************************************
>	FIXED16 DocView::GetPrevZoomScale() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/94
	Inputs:		-
	Outputs:	-
	Returns:	The scale factor of this view as set by a previous zoom.
	Purpose:	
	Errors:		-
	SeeAlso:	DocView::SetPrevZoomScale
********************************************************************************************/

FIXED16 DocView::GetPrevZoomScale() const
{
	return PrevScale;
}



/********************************************************************************************
>	void DocView::SetPrevZoomScale(FIXED16 fxScale)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/94
	Inputs:		The previous zoom scale.
	Outputs:	-
	Returns:	-
	Purpose:	Sets the scale factor this DocView will "remember" as a previous zoom.
	Errors:		-
	SeeAlso:	DocView::GetPrevZoomScale
********************************************************************************************/

void DocView::SetPrevZoomScale(FIXED16 fxScale)
{
	PrevScale = fxScale;
}



/********************************************************************************************
>	WorkCoord DocView::GetPrevZoomOffset() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/94
	Inputs:		-
	Outputs:	-
	Returns:	The scroll offset of this DocView during a previous zoom.
	Purpose:	
	Errors:		-
	SeeAlso:	DocView::SetPrevZoomOffset
********************************************************************************************/

WorkCoord DocView::GetPrevZoomOffset() const
{
	return PrevOffset;
}



/********************************************************************************************
>	void DocView::SetPrevZoomOffset(const WorkCoord& wcOffset)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/94
	Inputs:		The previous zoom's scroll offset.
	Outputs:	-
	Returns:	-
	Purpose:	Sets the scroll offset this DocView will "remember" as a previous zoom.
	Errors:		-
	SeeAlso:	DocView::GetPrevZoomOffset
********************************************************************************************/

void DocView::SetPrevZoomOffset(const WorkCoord& wcOffset)
{
	PrevOffset = wcOffset;
}




/********************************************************************************************

> 	void DocView::ForceRedraw(BOOL ForcePaper = FALSE)

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	1/7/93
	Inputs:		ForcePaper: TRUE => do a normal OS-style invalidation
						   FALSE => do a 'clever' internal invalidation which does not 
									cause paper to be drawn straight away, so we get a
									smoother feel to the update.
    Purpose:    Force the entire view to be redrawn.
				NB. If you are redrawing because you have changed the zoom or scroll 
					offsets, then you must pass in TRUE for ForcePaper!!!
					(Because (a) Otherwise our scroll offsets are invalid and the next
								 time we scroll it gets mucked up, and
						     (b) Scrolling or zooming should give instant feedback to
								 the user.)
                   			                                     
********************************************************************************************/

void DocView::ForceRedraw(BOOL ForcePaper)
{
	// Make sure we use this DocView
	SetCurrent();

	// Get rid of any render regions associated with this DocView
	Camelot.DeleteRenderRegions(this);

	// Invalidate the whole view window
	if (ForcePaper)
	{
		// We want an instant update with paper drawn now, so use the OS to do the
		// invalidation.
		pViewWindow->InvalidateView();
		return;
	}

	// find type of rendering device
	CCamView *pCamView = GetConnectionToOilView();
	ERROR2IF(pCamView == NULL, (void)0, "DocView::ForceRedraw: DocView has no CamView");
	CNativeDC* pDevContext = pCamView->GetRenderDC();
	const RenderType rType = CCDC::GetType(pDevContext, TRUE);

	// Get DocView rectangle in 64 bit WorkCoods
	WorkRect ClipRect = GetViewRect();

	ERROR3IF(ClipRect.lo.x > ClipRect.hi.x, "DocView::ForceRedraw: clipping rect is invalid");
	ERROR3IF(ClipRect.lo.y > ClipRect.hi.y, "DocView::ForceRedraw: clipping rect is invalid");

	// Find all the spreads in the document that intersect the clipping rect, and create
	// a render region for each of them.

	// Get the first chapter in the document
	for (Chapter* pChapter = Node::FindFirstChapter(pDoc);
		 pChapter != 0;
		 pChapter = pChapter->FindNextChapter())
	{
		// Convert chapter bounding box to logical coords
		DocRect PhysChapterRect = pChapter->GetPasteboardRect(TRUE, this);
		WorkRect LogChapterRect;
		LogChapterRect.lo = PhysChapterRect.lo.ToWork(pDoc, this);
		LogChapterRect.hi = PhysChapterRect.hi.ToWork(pDoc, this);

		// Check to see if this chapter intersects the clipping rectangle.
		// If the chapter is the last one in the document, then the chapter's pasteboard
		// does not include the area of the bottom of the last spread, so we only check
		// the chapter's top boundary.

		BOOL fIsLastChapter = (pChapter->FindNextChapter() == 0);

		if (ClipRect.lo.y <= LogChapterRect.hi.y &&
		    (fIsLastChapter || ClipRect.hi.y >= LogChapterRect.lo.y))
		{
			for (Spread* pSpread = pChapter->FindFirstSpread();
				 pSpread != 0;
				 pSpread = pSpread->FindNextSpread())
			{
				// Convert spread bounding box to logical coords
				DocRect PhysSpreadRect = pSpread->GetPasteboardRect(TRUE);	// Pixelised
				WorkRect LogSpreadRect;
				LogSpreadRect.lo = PhysSpreadRect.lo.ToWork(pSpread, this);
				LogSpreadRect.hi = PhysSpreadRect.hi.ToWork(pSpread, this);

				BOOL fIsLastSpread = (pSpread->FindNextSpread() == 0);

				// Check if spread intersects the clipping rect
				if (ClipRect.lo.y <= LogSpreadRect.hi.y &&
				    ((fIsLastChapter && fIsLastSpread) || ClipRect.hi.y >= LogSpreadRect.lo.y))
				{
					// Make render region for intersection between spread and cliprect.
					DocRect SpreadClipRect = pSpread->GetWidePasteboard(this);

					// Convert clip rectangle to document coords
					DocRect DocClipRect = GetDocViewRect(pSpread);

					// Clip to spread rectangle
					SpreadClipRect = SpreadClipRect.Intersection(DocClipRect);

					// Make sure that the clip region is valid after the intersection
					if (SpreadClipRect.IsValid() && !SpreadClipRect.IsEmpty())
					{
						// Convert it to Spread coords and create a render region
						pSpread->DocCoordToSpreadCoord(&SpreadClipRect);
						MakeNewRenderRegion(pSpread, SpreadClipRect, pDevContext, rType, FALSE);
					}
				}
			}
		}
	}
}


/********************************************************************************************

> 	void DocView::ForceRedraw(const DocRect& ForceRect)

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	1/7/93
	Inputs:		ForceRect - the rectangle to be invalidated
    Purpose:    Force a specific rectangle of the view to be redrawn
                   			                                     
********************************************************************************************/

void DocView::ForceRedraw(WorkRect& ForceRect)
{
	PORTNOTETRACE("other","DocView::ForceRedraw - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR3("Should not be called!");

	OilCoord OilRectLo = (ForceRect.lo).ToOil(GetScrollOffsets());
	OilCoord OilRectHi = (ForceRect.hi).ToOil(GetScrollOffsets());

	OilRect RedrawOilRect(OilRectLo, OilRectHi);

	pViewWindow->InvalidateView(&RedrawOilRect);
#endif
}



/********************************************************************************************

>	void DocView::ForceRedraw(Spread *pSpread, DocRect& SpreadRect, BOOL Accumulate = FALSE, Node* pInvalidNode = NULL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com> & Rik
	Created:	14/12/93
	Inputs:		pSpread - the spread which contains the invalid region.
				SpreadRect - the invalid region (in spread coordinates, i.e. (0,0) based)
				Accumulate - TRUE => invalidate *now*
							FALSE => don't invalidate right now, but accumulate this
									 into a list of rectangles for this view, and update
									 intelligently when DocView::FlushRedraw() is called.
				ForceRecache - TRUE => Make sure that any overlapping cached bitmaps that capture bits of
										objects around/under/over them are thrown away during this redraw
							  FALSE => Don't need to worry about cached objects because nothing in the tree
										has changed

	Purpose:	Force redraw of an area of a spread.
	SeeAlso:	DocView::FlushRedraw

********************************************************************************************/

void DocView::ForceRedraw(Spread *pSpread, DocRect SpreadRect, BOOL Accumulate, Node* pInvalidNode)
{
	ERROR3IF(pSpread == NULL, "ForceRedraw has been passed a NULL spread pointer");

	// Make sure we use this DocView
	SetCurrent();

	// This rectangle has come directly from the program and might not lie on pixel
	// boundaries. So pixelise it before passing it directly on to the rendering system.
	SpreadRect.lo.Pixelise(this);
	SpreadRect.hi.Pixelise(this);

	// Should we do it now?
// IMMEDIATE REDRAW
//	if (FALSE)
	if (Accumulate)
	{
		// No - add it to our list and wait until the Operation finishes...as long as it
		// works.
		if (PendingRedrawList.AddInvalidRegion(pSpread, SpreadRect, pInvalidNode))
		{
			// Worked - exit now and wait for FlushRedraw() to be called.
			return;
		}

		// Otherwise fall through to normal processing (which will probably fail too!)
	}

	// Is this a bodge I see before me???
	// Not really. This modification expands the invalid region to take anti-aliasing
	// pixels into account. It could be made conditional on anti-aliasing being turned on
	// but this is simpler and the overhead when anti-aliasing is off is not great.
	SpreadRect.Inflate(ScaledPixelWidth.MakeLong(), ScaledPixelHeight.MakeLong());

	// Translate spread coords to document coords
	pSpread->SpreadCoordToDocCoord(&SpreadRect);

	// Clip this region to the spread
	DocRect WidePasteRect = pSpread->GetWidePasteboard(this);
	SpreadRect = SpreadRect.Intersection(WidePasteRect);

	// need to check for valid intersections here as Intersecting with
	// an invalid rectangle again will give a valid rectangle would you
	// believe!	(Mike 20/12/95)
	if ( SpreadRect.IsEmpty() || (!SpreadRect.IsValid()) )
	{
		return;
	}

 	// Convert clip rectangle to document coords
 	DocRect DocClipRect = GetDocViewRect(pSpread);

 	// Clip to spread rectangle
 	SpreadRect = SpreadRect.Intersection(DocClipRect);
                                                        
	// Check to see if the result is an empty rectangle
	if ((SpreadRect.IsValid()) && (!SpreadRect.IsEmpty()))
	{
		// Find type of rendering device
		CCamView *pCamView = GetConnectionToOilView();
		ERROR2IF(pCamView == NULL, (void)0, "DocView has no CamView!");

// These next two are commented out because 
// a) rType is never used, and
// b) pDevContext idn't needed in MakeNewRenderRegion if PaintPaper is FALSE (which is is)

//		CDC *pDevContext = pCamView->GetRenderDC();
//		const RenderType rType = CCDC::GetType(pDevContext, TRUE);

		// Translate document coords back to spread coords
		pSpread->DocCoordToSpreadCoord(&SpreadRect);

		// Use this to make a render region.
//		MakeNewRenderRegion(pSpread, SpreadRect, pDevContext, RENDERTYPE_SCREEN, FALSE);	// See changed line below
		
		// Notice that because the PaintPaper parameter (5th one) is FALSE, we can pass
		// NULL as a pointer to a device context, and thus we don't have to call
		// GetRenderDC above at all, and hopefully, this will speed up translation
		// no end.
		MakeNewRenderRegion(pSpread, SpreadRect, NULL, RENDERTYPE_SCREEN, FALSE, pInvalidNode);
	}
}


/********************************************************************************************

>	void DocView::FlushRedraw()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/94
	Purpose:	Intelligently invalidate all the regions gathered so far for this DocView.
				This performs smart invalidation depending on the number and nature of the 
				regions.
	SeeAlso:	DocView::ForceRedraw; PendingRedraws; SpreadRedraws

********************************************************************************************/

void DocView::FlushRedraw()
{
	// Ask the update list to do its stuff
	PendingRedrawList.FlushRedraw(this);
}

/********************************************************************************************

>	void DocView::HandleNodeDeletion(Node* pNode)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/94
	Purpose:	Intelligently invalidate all the regions gathered so far for this DocView.
				This performs smart invalidation depending on the number and nature of the 
				regions.
	SeeAlso:	DocView::ForceRedraw; PendingRedraws; SpreadRedraws

********************************************************************************************/

void DocView::HandleNodeDeletion(Node* pNode)
{
	// Ask the update list to do its stuff
	PendingRedrawList.HandleNodeDeletion(pNode);
}

/********************************************************************************************

> 	ObjectDragTarget DocView::IsPointerOverNode(NodeRenderableInk** ThisNode, INT32 Range,
												BOOL Interrupt = TRUE, BOOL bIsColourDrag=FALSE)

    Author: 	Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> (and Will)
    Created:	23/1/95
	Outputs:	ThisNode - ptr to node over if any NULL otherwise
	Inputs:		Range -	Margin to use in line hittest
				Interrupt  - TRUE if the Hit Test should be interruptable or not.

				// DMc inserted 6/5/99
				For colour drags, you don't want to addess the shadow
				or bevel controller node, but what's inside
				
	Returns:	an ObjectDragTarget - FILL/LINE/STARTFILL/ENDFILL		
    Purpose:    to determine whether pointer is over 1 - an objects outline
                   			                         2 - any visible fill blobs
                   			                         3 - a simple (non grouped) object            
********************************************************************************************/

ObjectDragTarget DocView::IsPointerOverNode(NodeRenderableInk** pThisNode, INT32 Range,
											BOOL Interrupt, BOOL bIsColourDrag)
{
 	Spread *pSpread=NULL;
	DocCoord pMousePos;
		
	// get the current spread and mouse pos
	if(!GetCurrentMousePos(&pSpread, &pMousePos))
	   	return NO_TARGET;
	
	// mouse pos to be used at end of drag
	DragPos = pMousePos;

	// ouch !
	if(!pSpread)
		return NO_TARGET;

	// See if we are close to the TextStory caret - We can't hittest on it as it is only a blob
	if (TextStory::GetFocusStory() != NULL)
	{
		CaretNode* pCaret = TextStory::GetFocusStory()->GetCaret();
		Matrix matrix;

		if ((pCaret != NULL) && pCaret->IsSelected() && (pCaret->GetStoryAndCharMatrix(&matrix)!=FALSE))
		{
			// get the bounds of the caret path for the blob bounds
			Path* pCaretPath = pCaret->CreatePath(&matrix);
			if (pCaretPath != NULL)
			{
				INT32 CloseSlot = 0;
				double mu = 0.0;
				if (pCaretPath->IsPointCloseTo(pMousePos, Range*Range, &CloseSlot, &mu))
				{
					*pThisNode = pCaret;
					delete pCaretPath;
					return FILL_TARGET;
				}
				delete pCaretPath;
			}
		}
	}

	// Allow the hit-test to be interrupted if the mouse moves!
	Node* pInterruptedNode = NULL;
	NodeRenderableInk * pCompoundNode = NULL;
	NodeRenderableInk* pSimpleNode = NodeRenderableInk::FindSimpleAtPoint(pSpread,
				  pMousePos,
				  NULL,
				  Interrupt ? &pInterruptedNode : NULL);

	if (pInterruptedNode!=NULL)
		// hit-test was interrupted...
		pSimpleNode=NULL;

	// Now we have the 'Simple' node that the pointer was over.

	// If we can we should use any Selected nodes, so lets see if
	// this node or any of it's parents are selected.
	// We will stop if we get to the layer.

	if (pSimpleNode != NULL)	// Did we hit anything ?
	{
		Node* pSelNode = NULL;

		if ((ClickModifiers::GetClickModifiers().Constrain) || pSimpleNode->IsSelected())
		{
			// The node we hit was selected, so we will just use that
			pSelNode = pSimpleNode;
		}
		else
		{
			// Hunt up the tree, looking for a selected parent
			Node* pParent = pSimpleNode->FindParent();
			while (pParent != NULL && !pParent->IsLayer())
			{
				if (pParent->IsSelected())
				{
					// Found a selected parent, so we'll use that

					// don't do it for shadow controller nodes, or bevel nodes
					if ((!pSimpleNode->IsKindOf(CC_RUNTIME_CLASS(NodeShadow)) &&
						!pSimpleNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevel))) ||
						!bIsColourDrag)
					{
						pSelNode = pParent;
					}
					else
					{
						pSelNode = pSimpleNode;
					}

  					
					break;
				}

				pParent = pParent->FindParent();
			}
		}

		if (pSelNode != NULL)
		{
			// We will use the selected node in preference to anything else
			pSimpleNode = (NodeRenderableInk*)pSelNode;
		}
		else
		{
			// We didn't find any selected nodes, so we will try and apply
			// to a Parent group of the hit node.

			// This function will return the same node, if it has no parent group

			// DMc 6/5/99
			// change to hit testing for drags - don't do a compound node
			// for bevel controller or shadow controller nodes
			pCompoundNode = NodeRenderableInk::FindCompoundFromSimple(pSimpleNode);
			pSimpleNode = pCompoundNode;

			if ((!pSimpleNode->IsKindOf(CC_RUNTIME_CLASS(NodeShadow)) &&
						!pSimpleNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevel))) ||
						!bIsColourDrag)
			{
				pSimpleNode = pCompoundNode;
			}
	
		}
	}
 	
	// we can pass this back to the drag target

	*pThisNode = pSimpleNode;
 	
	
	// get the object bounds for this object 
	DocRect pSimpleBounds;
	if(pSimpleNode)
		pSimpleBounds = pSimpleNode->GetBoundingRect();
	else
		pSimpleBounds.MakeEmpty();
	
	AttrColourDrop *Attrib = new AttrColourDrop(pMousePos,pSimpleBounds,DocColour(COLOUR_TRANS));

  	// this will sort out whether we are over any fill blobs 
  	// pThisNode will then point to the relevant Node 
  	// OverFill is the FillControl type we are currently over
  	FillControl OverFill = AttrFillGeometry::DoColourDropTest(Attrib,pThisNode,NULL);
  	delete Attrib;

	ObjectDragTarget TargetHit;
	
  	// return the appropriate colour drag target 
  	switch(OverFill)
  	{
  	   	case FILLCONTROL_STARTPOINT:
			TargetHit = STARTCOL_TARGET;
			break;
		
		case FILLCONTROL_ENDPOINT:
		case FILLCONTROL_SECONDARYPOINT:
			TargetHit = ENDCOL_TARGET;
			break;
		
		case FILLCONTROL_ENDPOINT2:
			TargetHit = ENDCOL2_TARGET;
			break;
		
		case FILLCONTROL_ENDPOINT3:
			TargetHit = ENDCOL3_TARGET;
			break;
		
		case FILLCONTROL_MANY:		
			TargetHit = MANY_TARGET;
			break;

		case FILLCONTROL_NULL:
			TargetHit = FILL_TARGET;
			break;

		default:
			TargetHit = FILL_TARGET;
			break;
  	}	
	
		// BODGE ??? This should all change when we can do hit testing on lines.
	if(pSimpleNode)
	{
		// we're over an object - we'll see how close to its path we are
		Path* pPath = NULL; 
	
		if (IS_A(pSimpleNode, NodePath))
		{
			// get the node as a NodePath
			NodePath* pNPath = (NodePath*) pSimpleNode;
			pPath= &(pNPath->InkPath);
		}
		else if (pSimpleNode->IsKindOf(CC_RUNTIME_CLASS(NodeSimpleShape)))
		{
			// get the node as a NodeSimpleShape
			NodeSimpleShape * pNShape = (NodeSimpleShape*) pSimpleNode;
			pPath= &(pNShape->InkPath);
		}
		else if (pSimpleNode->IsKindOf(CC_RUNTIME_CLASS(NodeRegularShape)))
		{
			NodeRegularShape * pNShape = (NodeRegularShape*) pSimpleNode;
			pNShape->BuildShapePath(&pPath);
		}

		// we'll use the snap range to determine whether we are close to the path or not
		if(pPath)
		{
			INT32 CloseSlot = 0;
			double mu = 0.0;
			if (pPath->IsPointCloseTo(pMousePos,Range*Range, &CloseSlot, &mu))
			{
				return LINE_TARGET; 		// yes we are
			}
		}
	}

	return TargetHit;
}

/********************************************************************************************

> 	BOOL DocView::DM_DragFinished(DragInformation* DragInfo, ViewDragTarget* pDragTarget)

    Author: 	Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> (and Will)
    Created:	23/1/95
	Inputs:		DragInformation - about this drag..
				pDragTarget - The drag target created for this DocView
    Purpose:    act at the end of a general drag according to the drag type.
                   			                                     
********************************************************************************************/

BOOL DocView::DM_DragFinished(DragInformation* DragInfo, ViewDragTarget* pDragTarget)
{
	// The DragInfo must know what to do, or else we wouldn't 
	// have accepted the drag
	BOOL DropOK = DragInfo->OnPageDrop(pDragTarget);

	return DropOK;
}

/********************************************************************************************

> 	void DocView::DragPointerMove(Operation* pDragOp, OilCoord PointerPos, ClickModifiers Mods)

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	2/7/93
	Inputs:		pDragOp - Pointer to the current drag operation
				PointerPos - Coordinate of pointer in OS space
				Mods - Click modifiers structure
    Purpose:    Tell the current dragging operation that the pointer has moved. This function
   				is ony called when a drag has been started in CCamView by calling its
   				StartDrag function and should not be called after EndDrag has been called.
                   			                                     
********************************************************************************************/

void DocView::DragPointerMove(Operation* pDragOp, OilCoord PointerPos, ClickModifiers Mods)
{
//	BOOL bDragSolidly = FALSE;

	// Ignore if system is disabled
	if (CCamApp::IsDisabled())
		return;						     	// If the system is disabled, ignore

	// Find the spread that contains DocPos
	Spread *pSpread = FindEnclosingSpread(PointerPos);

// Note GetSelectedSpread is quicker in situations where you know you only have one spread
//		but it prevents drags on multiple spread views working properly...
//	Spread *pSpread = Document::GetSelectedSpread();

	if (pSpread == NULL)
	{
		ERROR3("Could not find spread in which the click happened");
		return; // Exit reasonably nicely
	}

	// Convert the OSRect into document coords
	DocCoord DocPos = PointerPos.ToDoc(pSpread, this);

	// Convert the coord to spread coords
	pSpread->DocCoordToSpreadCoord(&DocPos);

	// Store away the WorkCoord version of this click in the DocView for later extraction by
	// GetClickWorkCoord();
	LastWorkClick = PointerPos.ToWork( pVState->GetScrollPos() );
	ViewFlags.WorkClickCached = TRUE;			// Cached WorkCoord is valid.

	// Now call the current tool with the DocCoord click point
	ERROR3IF( pDragOp==NULL, "There was not operation in DragPointerMove()" );
	if (pDragOp)
		pDragOp->DragPointerMove(DocPos, Mods, pSpread, m_bSolidDrag);

	ViewFlags.WorkClickCached = FALSE; 			// Cached WorkCoord is no longer valid!
}




/********************************************************************************************

> 	void DocView::DragPointerIdle(Operation* pDragOp, OilCoord PointerPos, ClickModifiers Mods)

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	2/7/93
	Inputs:		pDragOp - Pointer to the current drag operation
				PointerPos - Coordinate of pointer in OS space
				Mods - Click modifiers structure
    Purpose:    Tell the current dragging operation that the pointer has NOT moved and that
    			there is idle CPU time to be used. This function is only called when a drag has
    			been started in CCamView by calling its StartDrag function and should not be
    			called after EndDrag has been called.
                   			                                     
********************************************************************************************/

void DocView::DragPointerIdle(Operation* pDragOp, OilCoord PointerPos, ClickModifiers Mods)
{
	ERROR3IF(pDragOp == NULL, "Illegal NULL parameter!");

	// Ignore if system is disabled
	if (CCamApp::IsDisabled())
		return;						     	// If the system is disabled, ignore

	// Find the spread that contains DocPos
	Spread *pSpread = FindEnclosingSpread(PointerPos);

// Note GetSelectedSpread is quicker in situations where you know you only have one spread
//		but it prevents drags on multiple spread views working properly...
//	Spread *pSpread = Document::GetSelectedSpread();

	if (pSpread == NULL)
	{
		ERROR3("Could not find spread in which the click happened");
		return; // Exit reasonably nicely
	}

	// First of all convert the OSRect into document coords
	DocCoord DocPos = PointerPos.ToDoc(pSpread, this);

	// Convert the coord to spread coords
	pSpread->DocCoordToSpreadCoord(&DocPos);

	// Store away the WorkCoord version of this click in the DocView for later extraction by
	// GetClickWorkCoord();
	LastWorkClick = PointerPos.ToWork( pVState->GetScrollPos() );
	ViewFlags.WorkClickCached = TRUE;			// Cached WorkCoord is valid.

	// Now call the current tool with the DocCoord click point
	if (pDragOp != NULL)
		pDragOp->DragPointerIdle(DocPos, Mods, pSpread, m_bSolidDrag);

	// Cached WorkCoord is no longer valid!
	ViewFlags.WorkClickCached = FALSE;
}




/********************************************************************************************

> 	void DocView::DragFinished(Operation* pDragOp, OilCoord PointerPos, ClickModifiers Mods,
								BOOL Success)

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	2/7/93
	Inputs:		pDragOp - Pointer to the current drag operation
				PointerPos - Coordinate of pointer in OS space
				Mods - Click modifiers structure
				Success - Flag saying whether drag ended OK or was aborted for some reason
    Purpose:    Tell the current dragging operation that the drag has ended either because
    			the pointer buttons have all been released or the drag has been aborted in
    			some way (Escape was pressed?).
    			This function is ony called when a drag has been started in CCamView by
    			calling its StartDrag function and should not be called after EndDrag has been
    			called.
                   			                                     
********************************************************************************************/

void DocView::DragFinished(Operation* pDragOp, OilCoord PointerPos, ClickModifiers Mods,
							BOOL Success)
{
	// Ignore if system is disabled
	if (CCamApp::IsDisabled())
		return;						     	// If the system is disabled, ignore
	// SHOULD END THE DRAG

	// Find the spread that contains DocPos
	Spread *pSpread = FindEnclosingSpread(PointerPos);

	if (pSpread == NULL)
	{
		ERROR3("Could not find spread in which the click happened");
		return; // Exit reasonably nicely
	}

	// First of all convert the OSRect into document coords
	DocCoord DocPos = PointerPos.ToDoc( pSpread, this );

	// Convert the coord to spread coords
	pSpread->DocCoordToSpreadCoord(&DocPos);

	// Store away the WorkCoord version of this click in the DocView for later extraction by
	// GetClickWorkCoord();
	LastWorkClick = PointerPos.ToWork( pVState->GetScrollPos() );

	// Cached WorkCoord is valid.
	ViewFlags.WorkClickCached = TRUE;

	// Broadcast a "starting operation" message to begin with (normally broadcast by
	// OpDescriptor::Invoke, but thanks to the brain-damaged Camelot architecture, operations
	// which drag do not invoke, and this is the best place to catch them.  Go figure.)
	BROADCAST_TO_ALL(OpMsg(pDragOp, OpMsg::BEGIN));

	// Now call the current tool with the DocCoord click point
	pDragOp->DragFinished(DocPos, Mods, pSpread, Success, m_bSolidDrag);

	// Cached WorkCoord is no longer valid!
	ViewFlags.WorkClickCached = FALSE;
	m_bSolidDrag = FALSE;
}




/********************************************************************************************

> 	void DocView::OnClick(OilCoord PointerPos, ClickType Click, ClickModifiers Mods)

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	2/7/93
	Inputs:		PointerPos - Position of click from OS
				Click - Type of click (single, double or drag)
				Mods - Other inputs which modify the meaning of the click
    Purpose:    Convert click coordinates into DocCoords and pass them on to the current
			    tool.
                   			                                     
********************************************************************************************/
/*	Technical notes:
				This routine also converts the click position into WorkCoords and keeps it
				for those routines which need it. They can get it by calling
				GetClickWorkCoord so long as the ViewState hasn't changed in the meantime.

********************************************************************************************/

void DocView::OnClick(OilCoord PointerPos, ClickType Click, ClickModifiers Mods)
{
	// Ignore if system is disabled
	if (CCamApp::IsDisabled())
		return;						     	// If the system is disabled, ignore

	// grab the focus if it's stuck in  a control somewhere
	DialogManager::DefaultKeyboardFocus();	
	
	// Find the spread in which the click happened
	Spread *pSpread = FindEnclosingSpread(PointerPos);

	if (pSpread == NULL)
	{
		ERROR3("Could not find spread in which the click happened");
		return; // Exit reasonably nicely
	}

	// When the user clicks on a spread which is not the selected spread then this spread becomes
	// selected and the selection is cleared.
	//Document::SetSelectedSpread(pSpread);
	Document::SetSelectedViewAndSpread(pDoc, this, pSpread);

	//GetApplication()->FindSelection()->SetSelectedSpread(pSpread);

	// First of all convert the OilRect into device coords
	DocCoord DocPos = PointerPos.ToDoc( pSpread, this );
	
	// Convert the coord to spread coords
	pSpread->DocCoordToSpreadCoord(&DocPos);


	// Store away the WorkCoord version of this click in the DocView for later extraction by
	// GetClickWorkCoord();
	LastWorkClick = PointerPos.ToWork( pVState->GetScrollPos() );
	ViewFlags.WorkClickCached = TRUE;			// Cached WorkCoord is valid.

	// Check the click mods to see if they specify an operation which we can do directly here
	if (Mods.IsHandledByTool())
	{
		// Now call the current tool with the DocCoord click point
		// Graham 18/6/96: When one button clicks while another one is dragging
		// we call the current drag operation, not the tool
		if (Mods.ClickWhileDrag)
		{
			//First make sure there is a current drag operation
			ERROR3IF(Operation::GetCurrentDragOp()==NULL, "DocView::OnClick - no current drag operation");

			//Now call the current drag operation.
			//If the current drag operation is not a transform operation, this
			//will call the empty virtual function Operator::OnClickWhileDragging.
			Operation::GetCurrentDragOp()->OnClickWhileDragging(PointerPos, Click, Mods, m_bSolidDrag);
		}
		else if (Mods.PushCentreTool)
		{
			// Matt 16/11/2000 - Use the push tool/auto-screen-centrey-tool when required
			OpPush* pOpPush = new OpPush;
			ERROR3IF(!pOpPush,"Out of memory in DocView() -> Cannot create PushOp");

			// Matt 12/01/2001
			// Change the cursor to show the hand icon
			PushToolCursorID = CursorStack::GPush( &m_PushCursor, TRUE );

			// Start the drag operation and pass in the Anchor Point to the push operation
			pOpPush -> DoDrag( DocPos );
		}
		else
		{
			Tool* pCurrentTool = Tool::GetCurrent();

			if(pCurrentTool)
			{
				pCurrentTool->OnClick(DocPos, Click, Mods, pSpread);
			}
		else
			ERROR3("No current tool in DocView::OnClick()!");
		}

		// ---------------------------------------------------------------------------
		// Do special handling for menu clicks
		// Allow the Selector to do it's normal stuff for "Click" events and "Up" events
		// So that menu clicks perform selection as required by Windows UI guidelines
		// On the up stroke show the context menu
		if (Mods.Menu)
		{
#ifndef STANDALONE
			Tool* pSelector = Tool::FindTool(TOOLID_SELECTOR);
			ERROR3IF(pSelector==NULL, "ARGH! No Selector when DocView got a menu click!");
			Tool* pCurrentTool = Tool::GetCurrent();
			if (pSelector!=pCurrentTool &&
				(Click == CLICKTYPE_SINGLE || Click == CLICKTYPE_UP) &&	// Don't allow menu drags while selector not current
				!Mods.ClickWhileDrag &&
				Operation::GetCurrentDragOp()==NULL)
			{
				pSelector->OnClick(DocPos, Click, Mods, pSpread);
			}

			// On the up stroke
			if (Click==CLICKTYPE_UP && !Mods.ClickWhileDrag)
			{
#ifdef PHOTOSHOPPLUGINS
				// As we are just about to invoke a context menu, check that the plug-in manager has its list
				// of available plug-ins otherwise, go and get it now as we are just about to need it.
				// Must do it now before even the hint of a context menu is started as the context
				// menus stack the automatic menu id and then restore it after they are destroyed.
				// This is bad if our main plug in menu ids are in this range!
				// We have to do it generally as at this point we don't know whether we are over a node
				// such as NodeBitmap which requires it.
				PlugInManager* pManager = GetApplication()->GetPlugInManager();
				if (pManager)
					pManager->CheckHaveDetailsOnPlugIns();
#endif // PHOTOSHOPPLUGINS
				
				// Pop up the context sensitive view menu.
				ViewContextMenu* pViewPopup = new ViewContextMenu;
				pViewPopup->ShowOverView(pSpread, DocPos, Mods);
			}
#endif
		}
		// ---------------------------------------------------------------------------
	}
	else
	{
		if (Click==CLICKTYPE_UP)
		{
			if (Mods.EditObject)
			{
				// Find the node under the pointer and ask it what its favourite editing
				// tool is.
			}

			else if (Mods.FullScreen)
			{
				// Use the full screen toggle.
				OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor( OPTOKEN_VIEWFULLSCREEN );
				if (pOpDesc) pOpDesc->Invoke();
			}

			else if (Mods.ZoomIn)
			{
				// Try to create an instance of the zoom operation.
				OpZoom* pZoomOp = new OpZoom;
				if (pZoomOp == NULL)
				{
					// We ran out of memory, so sorry.
					InformError(_R(IDE_NOMORE_MEMORY));
					return;
				}			

				// Zoom in or out around the mouse position.
				if (Mods.Adjust)
				{
					pZoomOp->ZoomOut(LastWorkClick);
				}
				else
				{
					pZoomOp->ZoomIn(LastWorkClick);
				}
			}
			else if (Mods.PushCentreTool)
			{
				// Then the user wants us to centre the view about their mouse position
			}

		}
	}

	ViewFlags.WorkClickCached = FALSE;			// Cached WorkCoord is no longer valid!
}



/********************************************************************************************
> 	void DocView::OnMouseMove(OilCoord Pos, ClickModifiers mods)

    Author: 	Justin_Flude (Xara Group Ltd) <camelotdev@xara.com> (Updated by Tim & Rik)
    Created:	14 Oct 93
	Inputs:		Pos - The position of the mouse, in OIL layer coordinates.
				mods - which buttons/keys are down.
    Purpose:    Converts the mouse position into document coordinates, then calls the
    			OnMouseMove() function of the currently selected tool with this
    			information.

********************************************************************************************/

void DocView::OnMouseMove(OilCoord Pos, ClickModifiers mods)
{
	// Ignore if system is disabled
	if (CCamApp::IsDisabled())
		return;						     	// If the system is disabled, ignore

	Spread *pSpread = FindEnclosingSpread(Pos);
	if (pSpread != NULL)
	{
		// Convert from OIL coordinates to document coordinates
		DocCoord DocPos = Pos.ToDoc(pSpread, this );

		// Convert the coord to spread coords
		pSpread->DocCoordToSpreadCoord(&DocPos);

	    // Call the currently selected tool.
		Tool* pTool = Tool::GetCurrent();

		//Is there a currently selected tool?
		if (pTool != NULL)
		{
			// And pass the mouse move event to the tool.
			pTool->OnMouseMove(DocPos, pSpread, mods);
		}
		else
			//No, there's no currently selected tool. We're in Camelot so that's worrying.
			//Show an error.
			ERROR3("No current tool in DocView::OnMouseMove()!");
	}
}



/********************************************************************************************
>	virtual BOOL DocView::HandleKeyPress(KeyPress* pKeyPress)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Inputs:		A pointer to the keypress event.
	Outputs:	-
	Returns:	TRUE if the event was handled by this function, or a function it called.
	Purpose:	Passes keystroke events to the current drag op, if dragging, or the
				current tool, if not.
	Errors:		-
	SeeAlso:	App::OnKeyPress; Tool::OnKeyPress; Operation::DragKeyPress
********************************************************************************************/

BOOL DocView::HandleKeyPress(KeyPress* pKeyPress)
{
	// Ignore if system is disabled
	if (CCamApp::IsDisabled())
		return FALSE;					     	// If the system is disabled, ignore

	// If we are dragging then pass the event on to the current drag operation.
	if (pCurrentDragOp != NULL)
	{
		// Handle the TAB key out here on behalf of ALL drag ops
		if (m_bSolidDragSupported && pKeyPress->IsPress() && !pKeyPress->IsRepeat())	// I.e. is this a non-auto-repeated key-down event?
		{
			switch (pKeyPress->GetVirtKey())
			{
				// TAB key means "change solid dragging mode"
				case CAMKEY(TAB):
				{
					DocView::SolidDragging = !DocView::SolidDragging;
					m_bSolidDrag = (m_bSolidDragSupported && DocView::SolidDragging);
					pCurrentDragOp->DragModeChanged(m_bSolidDrag);

					// update indicator in StatusLine
					DialogBarOp::SetSystemStateChanged();
					break;
				}
			}
		}

		return pCurrentDragOp->DragKeyPress(pKeyPress, m_bSolidDrag);
	}
	
	// Otherwise give the current tool, if any, the chance to deal with it.
	Tool* pTool = Tool::GetCurrent();
	return (pTool != NULL) ? pTool->OnKeyPress(pKeyPress) : FALSE;
}



/********************************************************************************************

> 	WorkCoord DocView::GetClickWorkCoord()

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	7/7/93
    Returns:   	The position of the last click in work coords
    Purpose:    To let people know where the last click was in Work Coordinates
                   			                                     
********************************************************************************************/

WorkCoord DocView::GetClickWorkCoord()
{
	ERROR3IF(!ViewFlags.WorkClickCached,
				"DocView::GetClickWorkCoord called when WorkClickCached is FALSE");
	return LastWorkClick;
}



/********************************************************************************************

> 	BOOL DocView::StartDrag(Operation* pDragOp, 
							DragType type, 
							DocRect* pMoveBBox,
							DocCoord* StartPos,
							BOOL KeepAccuracy,
							BOOL bSolidDragSupported)

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	2/7/93
	Inputs:		pDragOp - Pointer to the operation
				type - the type of drag to do
				pMoveBBox - 
				StartPos - 
				KeepAccuracy - TRUE if drag is biased towards accurate following of all mouse
									movements;
							   FALSE if drag is biased towards keeping up to date with user.
				bSolidDragSupported
							- FALSE (default) Do EORed outline dragging
							  TRUE	Update the tree during dragging
    Returns:   	TRUE if all went well
    Purpose:    Tells the world that a drag is starting
                   			                                     
********************************************************************************************/

BOOL DocView::StartDrag(Operation* pDragOp, 
						DragType type, 
						DocRect* pMoveBBox, 
						DocCoord* StartPos,
						BOOL KeepAccuracy,
						BOOL bSolidDragSupported)
{
	// Ignore if system is disabled
	if (CCamApp::IsDisabled())
		return TRUE;					     	// If the system is disabled, ignore

	if (pMoveBBox != NULL)
	{
		StartDragPos = *StartPos;
		if (pMoveBBox->ContainsCoord(StartDragPos))
		{
		}
	}

	// Remember the drag operation (this also serves as a flag to indicate whether we
	// are dragging or not).
	ERROR3IF(pCurrentDragOp != NULL, "Nested drag in DocView::StartDrag");
	ERROR3IF(pDragOp == NULL, "Null DragOp* in DocView::StartDrag?");
	pCurrentDragOp = pDragOp;
	m_bSolidDragSupported = bSolidDragSupported;
	m_bSolidDrag = (m_bSolidDragSupported && DocView::SolidDragging);

	ERROR3IF(pViewWindow==NULL, "ARGH! DocView is not connected to a window in StartDrag!");
	return pViewWindow->StartDrag(pDragOp, type, KeepAccuracy);
}




/********************************************************************************************

> 	BOOL DocView::EndDrag(Operation* pDragOp)

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	2/7/93
	Inputs:		pDragOp - The current operation
    Returns:   	TRUE if it workded
    Purpose:    Tell the oil layer that the drag has finished
                   			                                     
********************************************************************************************/

BOOL DocView::EndDrag(Operation* pDragOp)
{
	ERROR3IF(pViewWindow == NULL, "ARGH! DocView is not connected to a window in EndDrag!");
//	ERROR3IF(pCurrentDragOp == NULL, "Not dragging in DocView::EndDrag?");
//	ERROR3IF(pDragOp != pCurrentDragOp, "DragOp pointer inconsistent in DocView::EndDrag");
	pCurrentDragOp = NULL;

	// Matt 12/01/2001 - Restore the cursor if we've changed it earlier to the push tool cursor...
	if (PushToolCursorID && (PushToolCursorID != 0))
	{
			CursorStack::GPop(PushToolCursorID);
			PushToolCursorID = 0;
	}

	return pViewWindow->EndDrag(pDragOp);
}



/********************************************************************************************

>	BOOL DocView::ChangeDragType(DragType Type)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/4/95
	Inputs:		Type - The type of drag to change to
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Change the type of drag during the middle of a drag. This can only be called
				if there is a drag already happening (ie between calls to StartDrag and
				EndDrag).
	Errors:		ERROR3 if there is no render window or there is no drag happening
	SeeAlso:	ScreenCamView::ChangeDragType

********************************************************************************************/

BOOL DocView::ChangeDragType(DragType Type)
{
	TRACE( _T("Warning - DocView::ChangeDragType called\n") );
	// A bit of error checking
	ERROR3IF(pViewWindow == NULL, "ARGH! DocView is not connected to a window in ChangeDragType!");
	ERROR3IF(pCurrentDragOp == NULL, "Not dragging in ChangeDragType");

	// Ask the Oil side of things to change the drag
	BOOL Result = pViewWindow->ChangeDragType(Type);
	return Result;
}



/********************************************************************************************

>	void DocView::RenderSelectBlobs( DocRect Rect )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/93
	Inputs:		Rect - The Clipping Rectangle of the bit to draw
	Purpose:	Renders the Selection blobs of all the currently selected objects.
	SeeAlso:	DocView::OnDraw

********************************************************************************************/

void DocView::RenderSelectBlobs( DocRect Rect, Spread *pSpread )
{
//	TRACEUSER("Gerry", _T("RenderSelectBlobs (%d, %d) - (%d, %d)\n"), Rect.lo.x, Rect.lo.y, Rect.hi.x, Rect.hi.y);

	// Only render blobs if there is a selection, and this spread owns it.
	SelRange* Selected = GetApplication()->FindSelection();

	// Karim 29/06/2000
	//	PromoteToParent should never be set TRUE on the selection range, outside
	//	of code in which its modifications are required.
	//	I have included a TRACE statement here, as a 'quiet' note to programmers,
	//	should this occur.
	RangeControl rc = Selected->GetRangeControlFlags();
	if (rc.PromoteToParent)
	{
		TRACE( _T("DocView::RenderSelectBlobs; PromoteToParent is TRUE! Read inline comment for details.\n") );
		rc.PromoteToParent = FALSE;
		Selected->Range::SetRangeControl(rc);
	}

//PostProcessorStack* pStack = Selected->GetPostProcessorStack();
//ListRange* pLevel = pStack->GetNewLevelRange(FALSE);

	Node* pNode = Selected->FindFirst();
//Node* pNode = pLevel->FindFirst();
	if ((pNode == NULL) || ( pNode->FindParentSpread() != pSpread))
		// This spread does not own selection
		return;

	// Go find the blob manager
	BlobManager* BlobMgr = GetApplication()->GetBlobManager();
	ERROR3IF( BlobMgr == NULL, "BlobManager unexpectedly turned out NULL!");

	// Go and render the blobs
	RenderRegion* pRender = DocView::RenderOnTop( &Rect, pSpread, ClippedEOR );
	while (pRender)
	{
		pNode = Selected->FindFirst();
//pNode = pLevel->FindFirst();
		while (pNode != NULL)
		{
			ERROR3IF(!pNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk)),
					"Selected node is not a NodeRenderableInk in DocView::RenderSelectBlobs()");

			// Make sure that the nodes bounding box overlaps the redraw box
			//if ( Rect.IsIntersectedWith(((NodeRenderableInk*)pNode)->GetBlobBoundingRect()))
				// tell the node to draw in its selection blobs
			//	((NodeRenderableInk*)pNode)->RenderSelectBlobs();

			// Render those blobs			
			BlobMgr->RenderMyBlobs(&Rect, pSpread, (NodeRenderable*)pNode);

			// Find the next selected node to render
			pNode = Selected->FindNext(pNode);
//pNode = pLevel->FindNext(pNode);
		}

		// Get the next render region
		pRender = DocView::GetNextOnTop(&Rect);
	}
//delete pLevel;

	// Bodge to stop fill meshes EOR each other out.
	AttrFillGeometry::LastRenderedMesh = NULL;
}



/********************************************************************************************

>	BOOL DocView::IsMouseMoved()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Returns:	TRUE if the mouse has moved, FALSE if not
	Purpose:	This function finds out if the mouse has moved yet. This is only really
				useful if you are doing some processing as a result of a mouse move
				event. If, in the middle of your processing, you discover that the mouse
				has moved again, you may be able to stop processing immediatly and wait
				for the next mouse event to arrive in the not too distant future.
	SeeAlso:	CCamView::IsWaitingMouseMove

********************************************************************************************/

BOOL DocView::IsMouseMoved()
{
	PORTNOTETRACE("other","DocView::IsMouseMoved - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// If there is a Cam View about, ask it to see if there are more ouse messages
	// kicking about
	if (pViewWindow!=NULL)
		return pViewWindow->IsWaitingMouseMove();
#endif
	// there was no CamView, so no mouse messages
	return FALSE;
}



/********************************************************************************************
>	static BOOL DocView::GetCurrentMousePos(Spread** ppSpread, DocCoord* pdcMousePos)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		-
	Outputs:	ppSpread		pointer to a Spread pointer that will hold the spread
								containing the mouse
				pdcMousePos		pointer to a DocCoord that will hold the coordinates of
								the mouse
	Returns:	TRUE if the mouse is within the "selected" DocView, FALSE otherwise.
	Purpose:	Reports the position of the mouse if it is within the "selected" DocView.
				It position is "returned" in Spread coordinates.
	Errors:		-
	SeeAlso:	CCamView::GetCurrentMousePos
********************************************************************************************/

BOOL DocView::GetCurrentMousePos(Spread** ppSpread, DocCoord* pdcMousePos)
{
	// Valid output arguments must be provided.
	ERROR3IF(ppSpread == NULL, "Null Spread** in DocView::GetCurrentMousePos");
	ERROR3IF(pdcMousePos == NULL, "Null DocCoord* in DocView::GetCurrentMousePos");

	// Check if there is a "selected" DocView.
	if (Selected == NULL) return FALSE;

	// Ask the CCamView attached to the selected DocView to report the mouse position.
	OilCoord pt;
	if (!Selected->pViewWindow->GetCurrentMousePos(&pt)) return FALSE;

	// Convert the OIL coordinate to spread / doccoords.
	Spread* pSpread = Selected->FindEnclosingSpread(pt);
	if (pSpread == NULL)
	{
		ERROR3("Couldn't find a spread containing mouse position in DocView::GetCurrentMousePos");
		return FALSE;
	}

	// Convert the OSRect into document coords
	DocCoord dc = pt.ToDoc(pSpread, Selected );

	// Convert the coord to spread coords
	pSpread->DocCoordToSpreadCoord(&dc);

	// Return result and success code to caller.
	*ppSpread = pSpread;
	*pdcMousePos = dc;
	return TRUE;
}




/********************************************************************************************

>	RenderRegion* DocView::GetFirstRenderRegion(const DocRect& ClipRect, Spread* pSpread,
												OnTopCode Reason )

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/93
	Inputs:		ClipRect - The Rectangle to clip to
				pSpread - The Spread that the render region is for
				Reason - the kind of rendering needed
	Returns:	A Ptr to a RenderRegion to render into.
	Purpose:	RenderRegion's cannt span Spreads, because different Matrices are needed
				for the Coordinate transforms.  This Function (Along with GetNext...) is
				used to Split a DocRect area into a number of RenderRegions depending on
				the number of spreads the DocRect spans.
				First call this function to return the first RenderRegion (there will always
				be at least one for a valid DocRect), and then call GetNextRenderRegion until
				it returns NULL (which may be immediately).  Repeat the Rendering process for
				each RenderRegion returned.
				You do NOT need to call Init/Exit Render as they will be set up for 
				Immediate rendering.
	SeeAlso:	DocView::GetNextRenderRegion()

********************************************************************************************/

RenderRegion* DocView::GetFirstRenderRegion(DocRect& ClipRect, Spread *pSpread, OnTopCode Reason)
{
	// Construct the transformation matrix for the spread.
	Matrix RenderMatrix = ConstructRenderingMatrix(pSpread);

	// Make render region for intersection between spread and cliprect.
	// (Cliprect is still d)
	DocRect SpreadClipRect = pSpread->GetWidePasteboard(this);

	// Convert the coord to spread coords
	pSpread->DocCoordToSpreadCoord(&SpreadClipRect);

	// Clip to spread rectangle, if required
	if (Reason == ClippedEOR)
		SpreadClipRect = SpreadClipRect.Intersection(ClipRect);
	else
		SpreadClipRect = ClipRect;

	// Make sure that the clip region is valid after the intersection
	if ( (SpreadClipRect.IsValid()) && (!SpreadClipRect.IsEmpty()) )
	{
		RenderType rType = RENDERTYPE_NONE; // Initialise it! Mark H

		switch (Reason)
		{
			case CLICKBITMAP:	rType = RENDERTYPE_HITDETECT; break;
			case COLOURBITMAP:	rType = RENDERTYPE_COLOURBITMAP; break;
			case Normal:		rType = RENDERTYPE_SCREEN;
			default:			rType = RENDERTYPE_SCREENXOR; break;
		}

		CNativeDC* pDC = pViewWindow->GetRenderDC();
		RenderRegion *NewRegion =	View::NewRenderRegion(SpreadClipRect, RenderMatrix, 
									pDC, pSpread, rType);

		if (NewRegion == NULL)
			TRACEALL( _T("Not enough memory to create render region\n") );
		else
			NewRegion->ImmediateRender();

		return NewRegion;
	}

	// No render region could be created as the spread clip rect was empty or invalid
	return NULL;
}



/********************************************************************************************

>	RenderRegion* DocView::GetNextRenderRegion(const DocRect& ClipRect, Spread* pSpread,
											   OnTopCode Reason)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/93
	Inputs:		ClipRect - The Rectangle to clip to
				pSpread - The Spread that the render region is for
				Reason - the kind of rendering needed
	Returns:	A Ptr to a RenderRegion to render into.
	Purpose:	Used to split a DocRect to be redrawn into one or more RenderRegions.
				See DocView::GetFirstRenderRegion(DocRect& ClipRect) for more details.
	SeeAlso:	DocView::GetFirstRenderRegion()

********************************************************************************************/

RenderRegion* DocView::GetNextRenderRegion(DocRect& ClipRect, Spread *pSpread, OnTopCode Reason)
{
	// This needs to be expanded!
	// It's ok for Windows, but maybe not for other platforms...
	return NULL;
}



/********************************************************************************************

>	RenderRegion* DocView::RenderOnTop( DocRect* Rect, OnTopCode Reason )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/93
	Inputs:		Rect - The DocRect of the area that needs to be redrawn
				Reason - EOR or Normal or CLICKBITMAP
	Returns:	A pointer to a render region, or NULL if a valid RRegion could not
				be generated (or there were none to make in the first place
	Purpose:	Marks the start of an 'On Top' rendering loop. On Top simply means that
				the rendering will be done directly over the top of anything else that
				happens to be there, without causeing a redraw from the bottom of the tree.
				This functions main use is drawing on the EOR blobs that show selection, are
				part of a tool or are there to show a drag is in progress. This function could
				also be used to draw objects that we know will be placed on top of all other
				objects (ie a new object, being drawn for the first time).
				You should always use a loop like the one below :-
				MonoOn
					RenderRegion* pRegion = DocView::RenderOnTop( MyRect, EOR );
					while ( pRegion )
					{
						// Do your EOR rendering in here
						pRegion = DocView::GetNextOnTop( MyRect );
					}
				MonoOff
				This bit of code will loop through all the invalid regions that the OS needs
				redrawing as well as all the rectangles that Camelot splits a region into
				itself.
				IMPORTANT : Because this is a loop, you should not change the state of any
				data within it, because it will then be different the second time the code
				in the loop is executed.
	SeeAlso:	DocView::GetNextOnTop

********************************************************************************************/

RenderRegion* DocView::RenderOnTop( DocRect* Rect, Spread *pSpread, OnTopCode Reason )
{
	RenderRegion* pRender = NULL; //RenderRegion::GetCurrent();

	if ( OnTopCount == 0 )
	{
		// Find out about the current view
		DocView* pCurrentView = GetSelected();

		// If there is no current view, then return NULL
		if (pCurrentView==NULL)
			return NULL;

		// We should only hand out render regions if the view is not in the process of dying
		if (pCurrentView->IsViewDying)
			return NULL;

		// if the spread is NULL then this is bad
		if (pSpread==NULL)
			return NULL;

		// Find the first rectangle or region that needs redrawing and make
		// a render region for it.
		DocRect ClipRect;
		if (Rect == NULL)
		{
			// Get the DocView's viewport, in Spread Coords
			ClipRect = pCurrentView->GetDocViewRect(pSpread);
			pSpread->DocCoordToSpreadCoord(&ClipRect);
		}
		else
			ClipRect = *Rect;

		// Make sure that the clip rect is valid
		if (!ClipRect.IsValid())
		{
			// The Clip rect was not a valid rectangle
			ERROR3("RenderOnTop was asked to render to an invalid rect.");
			return NULL;
		}
				
		// Go and get a render region based on all this info
		pRender = pCurrentView->GetFirstRenderRegion(ClipRect, pSpread, Reason);
			
		// stop now if we did not get one
		if ( !pRender )
			return NULL;

		// Set the static Reason code so that all future calls to RenderOnTop and
		// GetNextOnTop will do the same thing
		OnTopReason = Reason;
		OnTopSpread = pSpread;
		OnTopRenderRegion = pRender;
	
		// Turn on EORing for the RenderRegion if needed
		if (( OnTopReason == UnclippedEOR ) || (OnTopReason == ClippedEOR))
			pRender -> SetDrawingMode( DM_EORPEN );
			
		// Increment the Static Counter so that we know how many times this
		// function has been called
		OnTopCount++;
		
		// return the render region
		return pRender;
	}
	else
	{
		// Get the current RenderOnTop RenderRegion
		pRender = OnTopRenderRegion;
		
		// If there is no current render region, then we had better stop now
		if (!pRender)
			return NULL;

		// Increment the Static Counter so that we know how many times this
		// function has been nested through
		OnTopCount++;
		
		// if we want to be EORing, then make sure that EORing has been switched on
		if (( OnTopReason == UnclippedEOR ) || (OnTopReason == ClippedEOR))
			pRender -> SetDrawingMode( DM_EORPEN );
		
		// return the current render region
		return pRender;
	}
}





/********************************************************************************************

>	RenderRegion* DocView::GetNextOnTop(DocRect* Rect)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/93
	Inputs:		Rect - A pointer to the DocRect that the Rendering is to be clipped to
	Returns:	A RenderRegion if there is another valid one to be rendered into, or
				NULL if there is not
	Purpose:	This function continues the RenderOnTop loop, returning the next
				RenderRegion to be considered. If we are not in the outer most
				rendering loop then this function will return NULL, as it will be
				up to the top level to re-call everyone with the next render region.
				It should always be used as part of a rendering loop as described in
				DocView::RenderOnTop.
	SeeAlso:	DocView::RenderOnTop

********************************************************************************************/

RenderRegion* DocView::GetNextOnTop( DocRect* Rect )
{
	ERROR3IF( OnTopCount <= 0, "GetNextOnTop was called before RenderOnTop was called\n" );

	RenderRegion* pRender = OnTopRenderRegion;
	
	if (OnTopCount == 1)
	{
		// Delete the old render region that is no longer needed
		if (pRender!=NULL)
		{
			pRender->StopRender();

			if (( OnTopReason == UnclippedEOR ) || (OnTopReason == ClippedEOR))
			{
				// Detach the Static OnTopRegion
				View::DeInitOnTopRegion();
			}
			else
			{
			 	delete pRender;
			}

			pRender = NULL;
			OnTopRenderRegion = NULL;
		}
		
		// Find the current view
		DocView* pView = GetSelected();
		if (pView==NULL)
			return NULL;

		// Get the next rectangle or region and make a RenderRegion
		DocRect ClipRect;
		if (Rect == NULL)
		{
			// Get the DocView's viewport and convert it from document coords to
			// spread coords
			ClipRect = pView->GetDocViewRect(OnTopSpread);
			OnTopSpread->DocCoordToSpreadCoord(&ClipRect);
		}
		else
			ClipRect = *Rect;

		// Go and build a new render region
		pRender = pView->GetNextRenderRegion(ClipRect, OnTopSpread, OnTopReason);
		OnTopRenderRegion = pRender;
			
		// See if we got one
		if (!pRender)
		{
			// There were no more render regions, so decrement the call count and 
			// return NULL
			OnTopCount--;
			return NULL;
		}

		// Turn on EORing for the RenderRegion if needed
		if ((OnTopReason == UnclippedEOR) || (OnTopReason == ClippedEOR))
			pRender->SetDrawingMode(DM_EORPEN);
				
		// return the render region
		return pRender;
	}
	else
	{
		// It is not up to this level to try and find more RenderRegions
		// so decrement the counter
		OnTopCount--;

		// No more RenderRegions here, so return NULL
		return NULL;
	}

	return NULL;
}




/********************************************************************************************

> 	Spread* DocView::OilToSpreadCoord(OilCoord oilpos, DocCoord* pdocpos)

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	22/11/94
	Inputs:		oilpos	- OilCoord to find spread and DocCoord for.
	Outputs:	pdocpos	- Pointer to DocCoord to receive DocCoord.
	Returns:	Spread*	- Pointer to spread in which pdocpos is valid.
    Purpose:    Combines the functions of FindEnclosingSpread and OilCoord::ToDoc.
				Finds which spread an OilCoord is in and then computes the DocCoord of
				that position within the spread.
                       			                                     
********************************************************************************************/

Spread* DocView::OilToSpreadCoord(OilCoord oilpos, DocCoord* pdocpos)
{
	// Preconditions
	ERROR2IF(this==NULL,NULL,"DocView::OilToDoc called on NULL pointer");

	// Clear the destination DocCoord in case it all goes horribly wrong...
	*pdocpos = DocCoord(0,0);

	// Find the spread that contains DocPos, if none then return NULL immediately...
	Spread *pSpread = FindEnclosingSpread(oilpos);
	if (pSpread != NULL)
	{
		// Convert the OilCoord into document coords...
		*pdocpos = oilpos.ToDoc(pSpread, this);

		// and then into Spread coords...
		pSpread->DocCoordToSpreadCoord(pdocpos);
	}

	// Return pointer to enclosing spread to caller.
	return pSpread;
}



/********************************************************************************************

>	Spread *DocView::FindEnclosingSpread(OilCoord Pos)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/93
	Inputs:		Pos - the coord to look for.
	Returns:	The spread that contains the coordinate, or NULL if such a spread does not
				exist.
	Purpose:	Given a chapter and an OilCoord, finds the spread that contains the
				coordinate.

********************************************************************************************/

Spread *DocView::FindEnclosingSpread(OilCoord Pos)
{
	// Find first spread in chapter
	Chapter* pChapter = Node::FindFirstChapter(pDoc);
	if (pChapter==NULL)
	{
		ERROR3( "There was no chapter in this document");
		return NULL;
	}

	Spread* pSpread = (Spread*)pChapter->FindFirstChild();
	ERROR3IF((pSpread == NULL) || !pSpread->IsSpread(), "Chapter has no spreads");

	// Optimisation for simple documents:
	// If only one chapter containing only one spread then that must be the
	// enclosing spread!
	if (pSpread && (pSpread->FindNext()==NULL || !pSpread->FindNext()->IsSpread()) && pChapter->FindNext()==NULL)
		return pSpread;

	// If the cursor was above the top of the document, then return the first spread
	// if there is one
	if (Pos.y >= 0)
		return pSpread;

	// Find this position in 64-bit work coords
	WorkCoord WorkPos = Pos.ToWork( pVState->GetScrollPos() );

	do
	{
		pSpread = (Spread *) pChapter->FindFirstChild();
		ERROR3IF((pSpread == NULL) || !pSpread->IsSpread(), 
				"Chapter has no spreads");

		do
		{
			// Convert spread bounding box to logical coords
			DocRect PhysSpreadRect = pSpread->GetWidePasteboard(this);
			WorkRect LogSpreadRect;
			LogSpreadRect.lo = PhysSpreadRect.lo.ToWork(pSpread, this);
			LogSpreadRect.hi = PhysSpreadRect.hi.ToWork(pSpread, this);

			// Check if spread intersects the clipping rect
			if ((WorkPos.y <= LogSpreadRect.hi.y) &&
			    (WorkPos.y >= LogSpreadRect.lo.y))
			{
				// Found the correct spread so stop looking.
				return pSpread;
			}

			// try next spread
			pSpread = (Spread *) pSpread->FindNext();
			ERROR3IF((pSpread != NULL) && !(pSpread->IsSpread()),
					"Spread is not a Spread!");

		} while (pSpread != NULL);

		// Was not in that chapter, so try the next one
		pChapter = (Chapter*) pChapter->FindNext();
		ERROR3IF( (pChapter!=NULL) && !(pChapter->IsKindOf(CC_RUNTIME_CLASS(Chapter))),
				 "Chapter is not a Chapter!" );

	} while (pChapter != NULL);


	ERROR3IF( pSpread == NULL, "Could not find spread in which the click happened");

	return pSpread;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/********************************************************************************************

>	BOOL DocView::Snap(Spread* pSpread,DocCoord* pDocCoord,
								BOOL TryMagSnap = TRUE,
								BOOL TryGridSnap = TRUE)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/94 (modified 20/9/94)
	Inputs:		pSpread		= a ptr to a spread node
				pDocCoord 	= a coord relative to the given spread
	Returns:	TRUE	- The coord has been snapped to something
				FALSE	- The coord is untouched by man and beast
	Purpose:	Allows you to easily snap a coord to something relevant in the given spread
				(e.g. to a grid, magnetic object, etc).
	SeeAlso:	All Snap member functions of all classes derived from NodeRenderableBounded.

********************************************************************************************/

BOOL DocView::Snap(Spread* pSpread,DocCoord* pDocCoord,
								BOOL TryMagSnap,
								BOOL TryGridSnap)
{
	BOOL Snapped = FALSE;

	if (pCSnap == NULL)
		pCSnap = new CSnap(this,pSpread);

	if (pCSnap != NULL)
	{
		pCSnap->SetSpread(pSpread);
		Snapped = pCSnap->Snap(pDocCoord,TryMagSnap,TryGridSnap);
	}

	return (Snapped);
}
 
/********************************************************************************************

>	BOOL DocView::Snap(Spread* pSpread,DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/94 (modified 20/9/94)
	Inputs:		pSpread		= a ptr to a spread node
				pDocRect 	= a rectangle relative to the given spread
				PrevCoord	= Used to determine how rect is snapped 
				CurCoord	= As above
	Returns:	TRUE	- The rect has been snapped to something
				FALSE	- The rect is untouched by man and beast
	Purpose:	Allows you to easily snap a rectangle to something relevant in the given spread
				(e.g. to a grid, magnetic object, etc).
	SeeAlso:	All Snap member functions of all classes derived from NodeRenderableBounded.

********************************************************************************************/

BOOL DocView::Snap(Spread* pSpread,DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
{
	BOOL Snapped = FALSE;

	if (pCSnap == NULL)
		pCSnap = new CSnap(this,pSpread);

	if (pCSnap != NULL)
	{
		pCSnap->SetSpread(pSpread);
		Snapped = pCSnap->Snap(pDocRect,PrevCoord,CurCoord);
	}

	return (Snapped);
}

/********************************************************************************************

>	BOOL DocView::SnapSelected(Spread* pSpread,DocCoord* pDocCoord,
								BOOL TryMagSnap = TRUE,
								BOOL TryGridSnap = TRUE)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/94
	Inputs:		pSpread		= a ptr to a spread node
				pDocCoord 	= a coord relative to the given spread
	Returns:	TRUE	- The coord has been snapped to something
				FALSE	- The coord is untouched by man and beast
	Purpose:	Calls the Snap(pSpread,pDocCoord) function of the selected DocView.
	Errors:		Will error in debug builds if there is no selected DocView
	SeeAlso:	All Snap member functions of all classes derived from NodeRenderableBounded.

********************************************************************************************/

BOOL DocView::SnapSelected(Spread* pSpread,DocCoord* pDocCoord,
								BOOL TryMagSnap,
								BOOL TryGridSnap)
{
	ERROR3IF(Selected == NULL,"DocView::SnapSelected called when no selected DocView");

	if (Selected != NULL)
		return (Selected->Snap(pSpread,pDocCoord,TryMagSnap,TryGridSnap));
	else
		return (FALSE);
}

/********************************************************************************************

>	static BOOL DocView::SnapToMagneticNode(Spread* pSpread, Node* pNode, DocCoord* pDocCoord)
 
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/94
	Inputs:		pSpread		Pointer to spread on which node exists
				pNode		Pointer to node to test magnetically
				pDocCoord 	A coordinate to test magnetically against the node
	Returns:	TRUE	- The coord has been snapped to something
				FALSE	- The coord is untouched by man and beast
	Purpose:	Calls magnetic snapping functions taking into acount the control flags
				held in this DocView.
	Errors:		Will error in debug builds if there is no selected DocView
	SeeAlso:	All Snap member functions of all classes derived from NodeRenderableBounded.

********************************************************************************************/

BOOL DocView::SnapToMagneticNode(Spread* pSpread, Node* pNode, DocCoord* pDocCoord)
{
	ERROR3IF(Selected == NULL,"DocView::SnapSelected called when no selected DocView");

	if (Selected != NULL)
	{
		BOOL Snapped = FALSE;

		if (Selected->pCSnap == NULL)
			Selected->pCSnap = new CSnap(Selected,pSpread);

		if (Selected->pCSnap != NULL)
		{
			Selected->pCSnap->SetSpread(pSpread);
			Snapped = Selected->pCSnap->TryToSnapToObject(pNode, pDocCoord);
		}

		return (Snapped);
	}
	else
		return (FALSE);
}

/********************************************************************************************

>	static BOOL DocView::ForceSnapToGrid(Spread* pSpread, DocCoord* pDocCoord)
 
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/94
	Inputs:		pSpread		Pointer to spread on which node exists
				pDocCoord 	A coordinate to test magnetically against the node
	Returns:	TRUE	- The coord has been snapped to something
				FALSE	- The coord is untouched by man and beast
	Purpose:	Calls grid snapper ignoring enabling flags.
	Errors:		Will error in debug builds if there is no selected DocView
	SeeAlso:	All Snap member functions of all classes derived from NodeRenderableBounded.

********************************************************************************************/

BOOL DocView::ForceSnapToGrid(Spread* pSpread, DocCoord* pDocCoord)
{
	ERROR3IF(Selected == NULL,"DocView::SnapSelected called when no selected DocView");

	if (Selected != NULL)
	{
		BOOL Snapped = FALSE;

		if (Selected->pCSnap == NULL)
			Selected->pCSnap = new CSnap(Selected,pSpread);

		if (Selected->pCSnap != NULL)
		{
			Selected->pCSnap->SetSpread(pSpread);
			Snapped = Selected->pCSnap->SnapToGrids(pDocCoord);
		}

		return (Snapped);
	}
	else
		return (FALSE);
}

/********************************************************************************************

>	BOOL DocView::SnapSelected(Spread* pSpread,DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/94
	Inputs:		pSpread		= a ptr to a spread node
				pDocRect 	= a rectangle relative to the given spread
				PrevCoord	= Used to determine how rect is snapped 
				CurCoord	= As above
	Returns:	TRUE	- The rect has been snapped to something
				FALSE	- The rect is untouched by man and beast
	Purpose:	Calls the Snap(pSpread,pDocRect,PrevCoord,CurCoord) function of the selected DocView.
	Errors:		Will error in debug builds if there is no selected DocView
	SeeAlso:	All Snap member functions of all classes derived from NodeRenderableBounded.

********************************************************************************************/

BOOL DocView::SnapSelected(Spread* pSpread,DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
{
	ERROR3IF(Selected == NULL,"DocView::SnapSelected called when no selected DocView");

	if (Selected != NULL)
		return (Selected->Snap(pSpread,pDocRect,PrevCoord,CurCoord));
	else
		return (FALSE);
}

/********************************************************************************************

>	BOOL DocView::SnapCurrent(Spread* pSpread,DocCoord* pDocCoord,
								BOOL TryMagSnap = TRUE,
								BOOL TryGridSnap = TRUE)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/94
	Inputs:		pSpread		= a ptr to a spread node
				pDocCoord 	= a coord relative to the given spread
	Returns:	TRUE	- The coord has been snapped to something
				FALSE	- The coord is untouched by man and beast
	Purpose:	Calls the Snap(pSpread,pDocCoord) function of the Current DocView.
	Errors:		Will error in debug builds if there is no Current DocView
	SeeAlso:	All Snap member functions of all classes derived from NodeRenderableBounded.

********************************************************************************************/

BOOL DocView::SnapCurrent(Spread* pSpread,DocCoord* pDocCoord,
								BOOL TryMagSnap,
								BOOL TryGridSnap)
{
	ERROR3IF(Current == NULL,"DocView::SnapCurrent called when no Current DocView");

	DocView *pCurrent = GetCurrent();

	if (pCurrent != NULL)
		return (pCurrent->Snap(pSpread,pDocCoord,TryMagSnap,TryGridSnap));
	else
		return (FALSE);
}

/********************************************************************************************

>	BOOL DocView::SnapCurrent(Spread* pSpread,DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/94
	Inputs:		pSpread		= a ptr to a spread node
				pDocRect 	= a rectangle relative to the given spread
				PrevCoord	= Used to determine how rect is snapped 
				CurCoord	= As above
	Returns:	TRUE	- The rect has been snapped to something
				FALSE	- The rect is untouched by man and beast
	Purpose:	Calls the Snap(pSpread,pDocRect,PrevCoord,CurCoord) function of the Current DocView.
	Errors:		Will error in debug builds if there is no Current DocView
	SeeAlso:	All Snap member functions of all classes derived from NodeRenderableBounded.

********************************************************************************************/

BOOL DocView::SnapCurrent(Spread* pSpread,DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
{
	ERROR3IF(Current == NULL,"DocView::SnapCurrent called when no Current DocView");

	DocView *pCurrent = GetCurrent();

	if (pCurrent != NULL)
		return (pCurrent->Snap(pSpread,pDocRect,PrevCoord,CurCoord));
	else
		return (FALSE);
}

//----------------------------------------------------------------------------

/********************************************************************************************

>	void DocView::SetSnapToGridState(BOOL SnapToGrid)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/94
	Inputs:		SnapToGrid	- A bool used to set the snap status of grids in the view
	Returns:	-
	Purpose:	Allow you to change the "snap to grid" state of a DocView.
	SeeAlso:

********************************************************************************************/

void DocView::SetSnapToGridState(BOOL SnapToGrid)
{
	ViewFlags.GridSnap = SnapToGrid;
}

/********************************************************************************************

>	BOOL DocView::GetSnapToGridState()
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/94
	Inputs:		-
	Returns:	TRUE	- snap to grids in the doc view
				FALSE	- don't snap to grids in the doc view
	Purpose:	To find the "Snap to grid" state of a DocView
	SeeAlso:

********************************************************************************************/

BOOL DocView::GetSnapToGridState()
{
	return (ViewFlags.GridSnap);
}

/********************************************************************************************

>	void DocView::SetSnapToObjectsState(BOOL SnapToObjects)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Inputs:		SnapToObjects - A bool used to set the snap status of all objects in the view
	Returns:	-
	Purpose:	Allow you to change the "snap to all objects" state of a DocView.
	SeeAlso:

********************************************************************************************/

void DocView::SetSnapToObjectsState(BOOL SnapToObjects)
{
	ViewFlags.ObjectsSnap = SnapToObjects;
}

/********************************************************************************************

>	BOOL DocView::GetSnapToObjectsState()
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Inputs:		-
	Returns:	TRUE	- snap to objects in the doc view
				FALSE	- don't snap to objects in the doc view
	Purpose:	To find the "Snap to all objects" state of a DocView
	SeeAlso:

********************************************************************************************/

BOOL DocView::GetSnapToObjectsState()
{
	return (ViewFlags.ObjectsSnap);
}

/********************************************************************************************

>	void DocView::SetSnapToMagObjectsState(BOOL SnapToMagObjects)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Inputs:		SnapToMagObjects - A bool used to set the snap status of magnetic objects in the view
	Returns:	-
	Purpose:	Allow you to change the "snap to magnetic objects" state of a DocView.
	SeeAlso:

********************************************************************************************/

void DocView::SetSnapToMagObjectsState(BOOL SnapToMagObjects)
{
	ViewFlags.MagObjectsSnap = SnapToMagObjects;
}

/********************************************************************************************

>	BOOL DocView::GetSnapToMagObjectsState()
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Inputs:		-
	Returns:	TRUE	- snap to magnetic objects in the doc view
				FALSE	- don't snap to magnetic objects in the doc view
	Purpose:	To find the "Snap to magnetic objects" state of a DocView
	SeeAlso:

********************************************************************************************/

BOOL DocView::GetSnapToMagObjectsState()
{
	return (ViewFlags.MagObjectsSnap);
}

/********************************************************************************************

>	void DocView::SetShowGridState(BOOL ShowGrid)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/94
	Inputs:		ShowGrid	- A bool used to set the visible status of grids in the view
	Returns:	-
	Purpose:	Allow you to change the "show grid" state of a DocView.
	SeeAlso:

********************************************************************************************/

void DocView::SetShowGridState(BOOL ShowGrid)
{
	ViewFlags.GridShow = ShowGrid;
}

/********************************************************************************************

>	BOOL DocView::GetShowGridState()
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/94
	Inputs:		-
	Returns:	TRUE	- grids are visible in the doc view
				FALSE	- grids are not visible in the doc view
	Purpose:	To find the "Show grid" state of a DocView
	SeeAlso:

********************************************************************************************/

BOOL DocView::GetShowGridState()
{
	return (ViewFlags.GridShow);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------




/********************************************************************************************

>	static void DocView::ConstrainToAngle(DocCoord& Centre, DocCoord* CurCoord,
										  double Constrain = 0.0)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		Centre - The point that all the constrain rays come from
				Constrain - The constrain angle in Radians. You should not pass in this
				param except in very special circumstances. When this param is left out
				the constrain angle from the users preferences is used, which should be
				the way it is done in almost all circumstances.
	Outputs:	CurCoord - contains the mouse coord on entry and contains the constrained
				mouse coord afterwards
	Purpose:	Constrains the mouse movement to rays coming out from a central point. This
				is ideal for rotation etc. You should use this version of the function as
				the other verison of the function is being phased out right now.

********************************************************************************************/

void DocView::ConstrainToAngle(DocCoord& Centre, DocCoord* CurCoord, double Constrain)
{
	if (Constrain==0.0)
	{
		// Use the angle found in the preferences
		Constrain = DefaultConstrainAngle;
	}

	// Find out which of the 2 axis are the most significant
	double dx = CurCoord->x - Centre.x;
	double dy = CurCoord->y - Centre.y;

	// make sure that the cursor is not over the origin
	if ((dx==0) && (dy==0))
		return;

	// Find the square of the length of the ray
	double RayLength = sqrt((dy*dy) + (dx*dx));

	// Find the angle that this coord is around a circle
	double Angle = atan2(dy, dx);

	// constrain the angle to the nearest band
	double SnapAngle = (Angle + (Constrain/2)) / Constrain;
	SnapAngle = floor(SnapAngle);
	SnapAngle *= Constrain;

	// recalculate the new coordinate for the cursor
	dx = (INT32)(RayLength * cos(SnapAngle));
	dy = (INT32)(RayLength * sin(SnapAngle));

	// Calculate the new position for the cursor
	CurCoord->x = (INT32)(Centre.x + dx);
	CurCoord->y = (INT32)(Centre.y + dy);
}


/********************************************************************************************

>	void DocView::ConstrainToAngle(DocCoord& Centre, double Constrain, DocCoord* CurCoord)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		Centre - The point that all the constrain rays come from
				Constrain - The constrain angle in Radians
	Outputs:	CurCoord - contains the mouse coord on entry and contains the constrained
				mouse coord afterwards
	Purpose:	Constrains the mouse movement to rays coming out from a central point. This
				is ideal for rotation etc.
				DO NOT USE THIS VERSION OF THE FUNCTION. There is a newer version that
				makes use of the constrain angle in the preferences and should be used
				instead of this version.

********************************************************************************************/

void DocView::ConstrainToAngle(DocCoord& Centre, double Constrain, DocCoord* CurCoord)
{
	// Just call the new version of the function
	ConstrainToAngle(Centre, CurCoord, Constrain);
}


/********************************************************************************************

>	void DocView::ConstrainToAspect(DocCoord& FixedCoord, FIXED16& AspectLock, DocCoord* CurCoord)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		FixedCoord - a fixed reference point, usually the opposite corner of the
				bounding rectangle to be constrained
				AspectLock - The aspect ratio to maintain (1 is square)
	Outputs:	CurCoord - This coordinate will be modified so that it forms a rectangle of
				the correct aspect ratio from the fixed corner.
	Purpose:	Forces the supplied coordinate to be constrained to positions that force
				the aspect ratio from the fixed corner to be the one supplied. It should be
				called by the tools to 'snap' the coordinates from a drag to positions
				where the shape is not deformed.
				NOT FINISHED
	SeeAlso:	DocView::ConstrainToAngle()

********************************************************************************************/

void DocView::ConstrainToAspect(DocCoord& FixedCoord, FIXED16& AspectLock, DocCoord* CurCoord)
{
	// Get something to avoid all the rampant *s in my code
	DocCoord& DiffCoord = *CurCoord;

	// Find out which of the 2 axis are the most significant
	INT32 dx = DiffCoord.x - FixedCoord.x;
	INT32 dy = DiffCoord.y - FixedCoord.y;
	double dAspect = AspectLock.MakeDouble();

	if (abs(dx)>abs(dy))
	{
		// The x coord is the most significant, so change the y coord to match
		// First work out the magnitude of the change and correct it for sign
		INT32 NewOffset = (INT32)(abs(dx) / dAspect);
        if (dy<0)
        	NewOffset = -NewOffset;

		// add in the difference
		DiffCoord.y = FixedCoord.y + NewOffset;
	}
	else
	{
		// The y coord is the most significant, so chnage the x coord to match
		// First work out the magnitude of the change and correct it for sign
		INT32 NewOffset = (INT32)(abs(dy) * dAspect);
        if (dx<0)
        	NewOffset = -NewOffset;

		// add in the difference
		DiffCoord.x = FixedCoord.x + NewOffset;
	}
}




/********************************************************************************************

>	Spread* DocView::GetVisibleSpread()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Returns:	A pointer to a Spread, or NULL.
	Purpose:	Compares the bounding rectangles of Spreads in this view's document with
				the section of the document currently visible in the view, looking for
				the largest spread object that is within the view.  If none are visible
				then the function returns NULL.

				NB. hacked to simply return the first spread for the v1 release.
	Errors:		-
	SeeAlso:	DocView::GetFirstSelectedSpread

d********************************************************************************************/

Spread* DocView::GetVisibleSpread()
{
	// Initially we have no visible spread, which has zero area (of course).
	Spread* pCandidateSpread = NULL;
//	XLONG xlCandidateArea = 0;

	// Begin by finding the extent of the viewport.
	WorkRect wrViewPort = GetViewRect();

	// Check that we have a document.
	if (pDoc == NULL) 
		return NULL;

	// Find the first node in the document's tree.
	Node* pNode = pDoc->GetFirstNode();
	if (pNode == NULL) 
		return NULL;

	// Find its first sibling, which should be a NodeDocument.
	pNode = pNode->FindNext();
	if (pNode == NULL) 
		return NULL;

	ERROR3IF(!pNode->IsKindOf(CC_RUNTIME_CLASS(NodeDocument)),
				"Can't find a NodeDocument in DocView::GetVisibleSpread");

	// Find the first child node of the NodeDocument.
	pNode = pNode->FindFirstChild();
	if (pNode == NULL) return NULL;

	// Check if it's a chapter, skip it if it isn't.
	if (pNode->GetRuntimeClass() != CC_RUNTIME_CLASS(Chapter))
	{ 
		pNode = pNode->FindNext(CC_RUNTIME_CLASS(Chapter));
		if (pNode == NULL) return NULL;
	}
	
	// We've found a chapter.
	Chapter* pChapter = (Chapter*) pNode;
//	do {
		// Find the first child of the chapter.
		pNode = pChapter->FindFirstChild();
		if (pNode == NULL) return NULL;

		// If it isn't a spread then skip it.
		if (pNode->GetRuntimeClass() != CC_RUNTIME_CLASS(Spread))
		{
			pNode = pNode->FindNext(CC_RUNTIME_CLASS(Spread));
 			if (pNode == NULL) return NULL;
		} 
		
		// Now check all sibling spreads.
		Spread* pSpread = (Spread*) pNode;
/*		do {
			// At last, we found a spread!  Get its bounding rectangle in
			// document coordinates.
			DocRect drBound = pSpread->GetPageBounds();

			// Convert to work coordinates.
			WorkRect wrBound(drBound.lo.ToWork(pSpread, this),
							 drBound.hi.ToWork(pSpread, this));

			// If it intersects with the current viewport then we check if it is
			// the largest considered so far.
			WorkRect wrIntersect = wrBound.Intersection(wrViewPort);
			if (wrIntersect.IsValid() && !wrIntersect.IsEmpty())
			{
				XLONG xlArea = wrIntersect.Width() * wrIntersect.Height();
				if (xlArea > xlCandidateArea)
				{
*/					// It is the largest so far, so remember it.
					pCandidateSpread = pSpread;
/*					xlCandidateArea = xlArea;
				}
			}

			// Find the next sibling spread, if any.
			pSpread = (Spread*) pSpread->FindNext(CC_RUNTIME_CLASS(Spread));
		}
		while (pSpread != NULL);

		// Find the next sibling chapter, if any.
		pChapter = (Chapter*) pChapter->FindNext(CC_RUNTIME_CLASS(Chapter));
	} while (pChapter != NULL);
*/
	// If we get here we couldn't find anything!
	return pCandidateSpread;
}



/********************************************************************************************

>	Spread* DocView::GetFirstSelectedSpread()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to a Spread node in this view's document, or NULL.
	Purpose:	Searches for a selected object in the document, and returns a pointer to
				the spread containing said object.  If there are no selected objects then
				it returns NULL.
	Errors:		-
	SeeAlso:	DocView::GetVisibleSpread

********************************************************************************************/

Spread* DocView::GetFirstSelectedSpread()
{
	// Obtain the current selection, if any.
	//GetApplication()->UpdateSelection();
	SelRange* pSel = GetApplication()->FindSelection();
	if (pSel == NULL) return NULL;
	
	// Find the first selected object.
	Node* pFirstSelectedNode = pSel->FindFirst();

	// If there isn't one, then there is no spread with sel objects.
	// Otherwise get the selected object's parent spread.
	return (pFirstSelectedNode != NULL)
		 ? (Spread*) pFirstSelectedNode->FindParent(CC_RUNTIME_CLASS(Spread))
		 : NULL;
}



/***********************************************************************************************

>	BOOL DocView::IsSingleClickReallyTriple() const

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the last click event was a triple click.  FALSE if not, or the attached
				oil view is not a screen view.
	Purpose:	For determining wether the current single click event is really a triple click
				It only makes sense to call this when you are processing a click event!
	SeeAlso:	DocView::IsSingleClickReallyQuad()

*********************************************************************************************/

BOOL DocView::IsSingleClickReallyTriple() const
{
	// Salary check
	if (pViewWindow == NULL)
	{
		ERROR3("No attached oil view");
		return FALSE;
	}

	return pViewWindow->IsSingleClickReallyTriple();
}



/***********************************************************************************************

>	BOOL DocView::IsSingleClickReallyQuad() const

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the last click event was a quad click.  FALSE if not, or the attached
				oil view is not a screen view.
	Purpose:	For determining wether the current single click event is really a quad click
				It only makes sense to call this when you are processing a click event!
	SeeAlso:	DocView::IsSingleClickReallyTriple()

*********************************************************************************************/

BOOL DocView::IsSingleClickReallyQuad() const
{
	// Salary check
	if (pViewWindow == NULL)
	{
		ERROR3("No attached oil view");
		return FALSE;
	}

	return pViewWindow->IsSingleClickReallyQuad();
}




/***********************************************************************************************

>	void DocView::ShowViewScrollers(BOOL fIsVisible)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Pass on Show Hide Scrollers Message to CamView
	SeeAlso:	-

*********************************************************************************************/

void DocView::ShowViewScrollers(BOOL fIsVisible)
{
	PORTNOTETRACE("other","DocView::ShowViewScrollers - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	pViewWindow->ShowScrollers(fIsVisible);
#endif
}

/***********************************************************************************************

>	void DocView::ShowViewRulers(BOOL fIsVisible)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Pass on Show Hide Rulers Message to CamView
	SeeAlso:	-

*********************************************************************************************/

void DocView::ShowViewRulers(BOOL fIsVisible)
{
	PORTNOTETRACE("other","DocView::ShowViewRulers - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	pViewWindow->ShowRulers(fIsVisible);
#endif
}


/***********************************************************************************************

>	BOOL DocView::AreRulersVisible()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/94
	Inputs:		-
	Outputs:	-
	Returns:	Are Rulers Visible on this view
	Purpose:	-
	SeeAlso:	-

*********************************************************************************************/

BOOL DocView::AreRulersVisible()
{
	PORTNOTETRACE("other","DocView::AreRulersVisible - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	return pViewWindow->AreRulersVisible( );
#else
	return FALSE;
#endif
}


/***********************************************************************************************

>	BOOL DocView::AreScrollersVisible()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/94
	Inputs:		-
	Outputs:	-
	Returns:	Are Rulers Visible on this view
	Purpose:	-
	SeeAlso:	-

*********************************************************************************************/

BOOL DocView::AreScrollersVisible()
{
	PORTNOTE("other","DocView::AreScrollersVisible - do nothing")
#ifndef EXCLUDE_FROM_XARALX
	return pViewWindow->AreScrollersVisible( );
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	CDC *DocView::GetRenderDC()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/09/95
	Purpose:	Access function for the device context of this kernel view object.
	SeeAlso:	PrintView; View

********************************************************************************************/

CNativeDC *DocView::GetRenderDC()
{
	// Ask our CCamView to give us its DC and return that.
	return pViewWindow->GetRenderDC();
}


/********************************************************************************************

>	virtual void DocView::RenderPageMarks(Spread *pSpread, 
										  DocRect ClipRect,
										  CDC *pDevContext,
										  Matrix& RenderMatrix
										  RenderType rType)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/08/96
	Inputs:		pSpread		- pointer to the spread concerned
				ClipRect	- Rectangle to create region from
				pDevContect - pointer to the device context for this region (may be NULL
								if PaintPaper == FALSE)
				RenderMatrix- the render matrix to use.
				rType		- the type of render to give the region

	Outputs:	-
	Returns:	-
	Purpose:	Performs the rendering of page marks, ie crop marks, registration marks etc
				to the screen and printer. This base class version does nothing. All
				mark rendering technology is held in DocView and PrintView.
	SeeAlso:	View, PrintView

********************************************************************************************/

/*
void DocView::RenderPageMarks(Spread *pSpread, DocRect ClipRect, CDC *pDevContext, Matrix& RenderMatrix, RenderType rType)
{
#ifndef STANDALONE
	// No paper rendering region - get a new render region
	OSRenderRegion*	pPageMarksRegion = (OSRenderRegion *) NewRenderRegion(ClipRect, RenderMatrix, pDevContext, pSpread, rType);
	if (!pPageMarksRegion)
		return;

	// We save the context here, so that PaperRenderRegion::DetachDevice() can
	// throw away all the attributes.  We only need to do it here because
	// PaperRenderRegion::AttachDevice() (see 2 inches below) calls SaveContext()
	// automagically.
	pPageMarksRegion->SaveContext();

	// Set up the rendering system.
	if (!pPageMarksRegion->StartRender())
	{
		TRACEALL( _T("StartRender failed during View::RenderPageMarks\n"));
		pPageMarksRegion->RestoreContext();
		delete pPageMarksRegion;
		return;
	}

	// Render the page mark objects using the permanent PageMarks render region.
	pPageMarksRegion->SaveContext();
	RenderPageMarks(pPageMarksRegion, pSpread);
	pPageMarksRegion->RestoreContext();
	pPageMarksRegion->StopRender();

	delete pPageMarksRegion;
#endif
}
*/

/********************************************************************************************

>	virtual void DocView::RenderPageMarks(RenderRegion *pRRegion, 
										  Matrix &ViewTrans,
										  ClipRect &ClipRect
										  Spread *pSpread)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/08/96
	Inputs:		pSpread		- pointer to the spread concerned
				pRRegion	- pointer to the render region to use.
	Outputs:	-
	Returns:	-
	Purpose:	Performs the rendering of page marks, ie crop marks, registration marks etc
				to the screen and printer. This base class version does nothing. All
				mark rendering technology is held in DocView and PrintView.
	SeeAlso:	View, PrintView

********************************************************************************************/

BOOL DocView::RenderPageMarks(RenderRegion *pCurrRegion, Matrix &ViewTrans, DocRect &ClipRect, Spread *pSpread)
{
	return TRUE;	
}

/********************************************************************************************

>	DocCoord DocView::GetCentreCoord() const

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/97
	Inputs:		-
	Outputs:	-
	Returns:	The DocCoord for the centre of the view
	Purpose:	Gets a DocCoord for the centre of the view
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DocCoord DocView::GetCentreCoord()	
{
	//First get the document associated with this view
	Document* pDocument=GetDoc();

	//And find the first spread in that document
PORTNOTE("spread", "Multi-spread warning!")
	Spread* pSpread=pDocument->FindFirstSpread();

	//Check we got both of those
	ERROR2IF(pDocument==NULL || pSpread==NULL, DocCoord(0,0), "DocView::GetCentreImportPosition - no document or spread");

	//So get the view rectangle
	DocRect rectView = GetDocViewRect(pSpread);
	
	//And find the middle of that rectangle
   	DocCoord dcCentre;
	dcCentre.x = rectView.lo.x / 2 + rectView.hi.x / 2;
	dcCentre.y = rectView.lo.y / 2 + rectView.hi.y / 2;
				
	//Now make that into a spread coordinate
	DocRect rectSpread = pSpread->GetPasteboardRect();

	DocCoord dcToReturn;
	
	dcToReturn.x = dcCentre.x - rectSpread.lo.x; 
	dcToReturn.y = dcCentre.y - rectSpread.lo.y;
																						
	//And return that DocCoord
	return dcToReturn;
}

/********************************************************************************************

>	ImportPosition DocView::GetCentreImportPosition() const

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/97
	Inputs:		-
	Outputs:	-
	Returns:	The import position for the centre of the view
	Purpose:	Gets an import position for the centre of the view
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ImportPosition DocView::GetCentreImportPosition()
{
	//First get the document associated with this view
	Document* pDocument=GetDoc();

	//And find the first spread in that document
PORTNOTE("spread", "Multi-spread warning!")
	Spread* pSpread=pDocument->FindFirstSpread();

	//Now get the DocCoord in the centre of the view
	DocCoord dcToReturn=GetCentreCoord();

	//Put all those values into an ImportPosition structure
	ImportPosition posToReturn;

	posToReturn.pSpread=pSpread;
	posToReturn.Position=dcToReturn;

	//And return that ImportPosition
	return posToReturn;

}

/********************************************************************************************

>	DocCoord DocView::GetTopLeftCoord()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/97
	Inputs:		-
	Outputs:	-
	Returns:	The DocCoord for the top left of the view
	Purpose:	Gets a DocCoord for the top left of the view
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DocCoord DocView::GetTopLeftCoord()	
{
	//First get the document associated with this view
	Document* pDocument=GetDoc();

	//And find the first spread in that document
PORTNOTE("spread", "Multi-spread warning!")
	Spread* pSpread=pDocument->FindFirstSpread();

	//Check we got both of those
	ERROR2IF(pDocument==NULL || pSpread==NULL, DocCoord(0,0), "DocView::GetCentreImportPosition - no document or spread");

	//So get the view rectangle
	DocRect rectView = GetDocViewRect(pSpread);
	
	//And find the top left of that rectangle
   	DocCoord dcTopLeft(rectView.lo.x, rectView.hi.y);
				
	//Now make that into a spread coordinate
	DocRect rectSpread = pSpread->GetPasteboardRect();

	DocCoord dcToReturn;
	
	dcToReturn.x = dcTopLeft.x - rectSpread.lo.x; 
	dcToReturn.y = dcTopLeft.y - rectSpread.lo.y;
																						
	//And return that DocCoord
	return dcToReturn;
}

/********************************************************************************************

>	ImportPosition DocView::GetTopLeftImportPosition() const

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/97
	Inputs:		-
	Outputs:	-
	Returns:	The import position for the top left of the view
	Purpose:	Gets an import position for the top left of the view
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ImportPosition DocView::GetTopLeftImportPosition()
{
	//First get the document associated with this view
	Document* pDocument=GetDoc();

	//And find the first spread in that document
PORTNOTE("spread", "Multi-spread warning!")
	Spread* pSpread=pDocument->FindFirstSpread();

	//Now get the DocCoord in the top left of the view
	DocCoord dcToReturn=GetTopLeftCoord();

	//Put all those values into an ImportPosition structure
	ImportPosition posToReturn;

	posToReturn.pSpread=pSpread;
	posToReturn.Position=dcToReturn;

	//And return that ImportPosition
	return posToReturn;

}



/********************************************************************************************

>	double DocView::GetZoomFactor() const

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17 February 2000
	Returns:	The current zoom factor for this document view.
	Purpose:	Obtain the current zoom factor for this document view.
	Errors:		
	See also:	GetScaledPixelWidth(), GetPixelWidth().

********************************************************************************************/
double DocView::GetZoomFactor()
{
	return GetScaledPixelWidth().MakeDouble() / GetPixelWidth().MakeDouble();
}


/********************************************************************************************

>	void DocView::SetPreventRenderView(BOOL Value)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/2000
	Inputs:		Value to set
	Returns:	-
	Purpose:	To set our member variable that will prevent rendering of the view. As you might
				imagine this is something to be careful about
	Errors:		
	See also:	

********************************************************************************************/

void DocView::SetPreventRenderView(BOOL Value)
{
	m_bPreventRenderView = Value;
}


/********************************************************************************************

>	BOOL DocView::GetPreventRenderView()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/2000
	Returns:	The state of our flag which prevents the view from rendering
	Purpose:	As above
	Errors:		
	See also:	

********************************************************************************************/

BOOL DocView::GetPreventRenderView()
{
	return m_bPreventRenderView;
}


/********************************************************************************************

>	void DocView::ClearClickState()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/02/2004
	Returns:	-
	Purpose:	Allows click handlers to reset the click logic if they are doing things
				that would break it.
	Errors:		
	See also:	

********************************************************************************************/

void DocView::ClearClickState()
{
	PORTNOTETRACE("other","DocView::ClearClickState - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if (pViewWindow)
	{
		pViewWindow->ClearClickState();
	}
#endif
}


/********************************************************************************************
						TOGGLE FORE/BACKGROUND RENDERING OPERATION
*/
/********************************************************************************************

>	OpToggleFore::OpToggleFore() : Operation()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructs an OpToggleFore object.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpToggleFore::OpToggleFore() : Operation()
{

}



/********************************************************************************************

>	OpToggleFore::~OpToggleFore()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destructs an OpToggleFore object.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpToggleFore::~OpToggleFore()
{
	// Empty
}



/********************************************************************************************

>	void OpToggleFore::Do(OpDescriptor*)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		Pointer to Operation Descriptor
	Outputs:	-
	Returns:	-
	Purpose:	Actually "DO" a ToggleFore operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpToggleFore::Do(OpDescriptor*)
{
	if (DocView::GetSelected() != NULL)
		DocView::GetSelected()->SetForeBackMode(!DocView::GetSelected()->GetForeBackMode());
	End();
}



/********************************************************************************************

>	OpState OpToggleFore::GetState(String_256* UIDescription, OpDescriptor*)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		Pointer to Operation Descriptor
				Text Description
	Outputs:	-
	Returns:	-
	Purpose:	Find the state of the OpToggleFore operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState OpToggleFore::GetState(String_256* UIDescription, OpDescriptor*)
{
	// Default to !ticked and greyed
	OpState blobby(FALSE, TRUE);
	if (DocView::GetSelected() != NULL)
	{
		// If we have a selected view then get the state and ungrey
		blobby.Ticked = DocView::GetSelected()->GetForeBackMode();
		blobby.Greyed = FALSE;
	}
	return(blobby);
}



/********************************************************************************************

>	BOOL OpToggleFore::Init()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Create an OpDescriptor for the ToggleFore operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpToggleFore::Init()	
{
	return Operation::RegisterOpDescriptor( 
											0, 
											_R(IDS_TOGGLE_FORE_BACK),
											CC_RUNTIME_CLASS(OpToggleFore), 
											OPTOKEN_TOGGLEFORE,
											OpToggleFore::GetState,
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


/********************************************************************************************
						TOGGLE PROPORTIONAL SCROLL BARS OPERATION
*/
/********************************************************************************************

>	OpToggleScroll::OpToggleScroll() : Operation()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructs an OpToggleScroll object.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpToggleScroll::OpToggleScroll()
  : Operation()
{
	// Empty
}




/********************************************************************************************

>	OpToggleScroll::~OpToggleScroll()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destructs an OpToggleScroll object.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpToggleScroll::~OpToggleScroll()
{
}




/********************************************************************************************

>	void OpToggleScroll::Do(OpDescriptor*)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		Pointer to an operation descriptor
	Outputs:	-
	Returns:	-
	Purpose:	Actually "DO" a ToggleScroll operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpToggleScroll::Do(OpDescriptor*)
{
	PORTNOTE("other","OpToggleScroll::Do - do nothing")
#ifndef EXCLUDE_FROM_XARALX
	CCamView *pView = DocView::GetSelected()->GetConnectionToOilView();
	BOOL Prop = pView->GetScrollerStyle();
	pView->SetScrollerStyle(!Prop);
	End();
#endif
}



/********************************************************************************************

>	OpState OpToggleScroll::GetState(String_256* UIDescription, OpDescriptor*)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		A Pointer to an operation description
				Text Description
	Outputs:	-
	Returns:	-
	Purpose:	Find the state of the OpToggleScroll operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState OpToggleScroll::GetState(String_256* UIDescription, OpDescriptor*)
{
	PORTNOTETRACE("other","OpToggleScroll::Do - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	CCamView *pView = DocView::GetSelected()->GetConnectionToOilView();
	OpState blobby;
	blobby.Ticked = pView->GetScrollerStyle();
	blobby.Greyed = FALSE;
	return blobby;
#else
	return OpState();
#endif
}




/********************************************************************************************

>	BOOL OpToggleScroll::Init()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Create an OpDescriptor for the ToggleScroll operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpToggleScroll::Init()
{
	PORTNOTETRACE("other","OpToggleScroll::Init - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	return Operation::RegisterOpDescriptor(0, 
										   _R(IDS_TOGGLE_SCROLLBARS),
										   CC_RUNTIME_CLASS(OpToggleScroll), 
										   OPTOKEN_TOGGLESCROLL,
										   OpToggleScroll::GetState,
											0,							// help ID
										   _R(IDBBL_PROPSCROLLERSOP),	// bubble ID 
											0,							// bitmap ID
											0,							// control ID
											SYSTEMBAR_ILLEGAL,			// group bar ID
											TRUE,
											FALSE,
											TRUE,
											NULL,
											0,
											0,
											TRUE
										   );
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	OpToggleSolidDrag::OpToggleSolidDrag() : Operation()

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	14 Mar 2006
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructs an OpToggleSolidDrag object.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpToggleSolidDrag::OpToggleSolidDrag() : Operation()
{

}



/********************************************************************************************

>	OpToggleSolidDrag::~OpToggleSolidDrag()

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	14 Mar 2006
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destructs an OpToggleSolidDrag object.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpToggleSolidDrag::~OpToggleSolidDrag()
{
	// Empty
}



/********************************************************************************************

>	void OpToggleSolidDrag::Do(OpDescriptor*)

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	14 Mar 2006
	Inputs:		Pointer to Operation Descriptor
	Outputs:	-
	Returns:	-
	Purpose:	Actually "DO" a ToggleFore operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpToggleSolidDrag::Do(OpDescriptor*)
{
	DocView::SolidDragging=!DocView::SolidDragging;
	End();
}



/********************************************************************************************

>	OpState OpToggleSolidDrag::GetState(String_256* UIDescription, OpDescriptor*)

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	14 Mar 2006
	Inputs:		Pointer to Operation Descriptor
				Text Description
	Outputs:	-
	Returns:	-
	Purpose:	Find the state of the OpToggleSolidDrag operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState OpToggleSolidDrag::GetState(String_256* UIDescription, OpDescriptor*)
{
	// Default to !ticked and greyed
	OpState blobby(FALSE, TRUE);
	blobby.Ticked = DocView::SolidDragging;
	blobby.Greyed = FALSE;
	return(blobby);
}


/********************************************************************************************

>	BOOL OpToggleSolidDrag::Init()

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	14 Mar 2006
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Create an OpDescriptor for the OpToggleSolidDrag operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpToggleSolidDrag::Init()	
{
	return Operation::RegisterOpDescriptor( 
											0, 
											_R(IDS_TOGGLE_SOLIDDRAG),
											CC_RUNTIME_CLASS(OpToggleSolidDrag), 
											OPTOKEN_TOGGLESOLIDDRAG,
											OpToggleSolidDrag::GetState,
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
