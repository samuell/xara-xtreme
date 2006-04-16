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
// The pen tools edit code. Contains all operations provided for the 
// pen tool
// Created by Mike on 25/9/94

#include "camtypes.h"
//#include "resource.h"
#include "penedit.h"
//#include "mike.h"
#include "osrndrgn.h"
#include "docview.h"
#include "nodepath.h"
#include "document.h"
#include "ops.h"
#include "selop.h"
#include "progress.h"
#include "attrmgr.h"
#include "pathedit.h"
#include "app.h"
#include "stockcol.h"
#include "spread.h"

CC_IMPLEMENT_DYNCREATE( OpPenHandles, Operation )
CC_IMPLEMENT_DYNCREATE( OpPenDragBlobs, OpPenHandles )
CC_IMPLEMENT_DYNCREATE( OpPenCreateInternal, OpPenDragBlobs )
CC_IMPLEMENT_DYNCREATE( OpPenEditInternal, OpPenDragBlobs )
CC_IMPLEMENT_DYNCREATE( OpPenEditPath, OpPenHandles )
CC_IMPLEMENT_DYNCREATE( OpPenCreatePath, OpPenEditPath )
CC_IMPLEMENT_DYNCREATE( OpPenAddElement, OpPenEditPath )
CC_IMPLEMENT_DYNCREATE( OpPenClosePath, OpPenEditPath )

CC_IMPLEMENT_DYNCREATE( OpAddPath, SelOperation )
CC_IMPLEMENT_DYNCREATE( OpAddNewPath, OpAddPath )
CC_IMPLEMENT_DYNCREATE( OpAddPathToPath, OpAddPath )
CC_IMPLEMENT_DYNCREATE( OpClosePathWithPath, OpAddPath )


#define new CAM_DEBUG_NEW

/*
*/


/********************************************************************************************

>  HandleFlags::HandleFlags(BOOL rTrackEnd = TRUE,
							BOOL rGhostEnd = TRUE,
							BOOL TrackPtMoves = TRUE,
							BOOL TrackPtSpins = FALSE,
							BOOL GhostPtMoves = TRUE
							BOOL GhostPtSpins = FALSE);

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/9/94
	Purpose:	constructor for handle flags class
	SeeAlso:	-

********************************************************************************************/

HandleFlags::HandleFlags(	BOOL rTrackEnd,
							BOOL rGhostEnd,
							BOOL TrackPtMoves,
							BOOL GhostPtMoves,
							BOOL TrackPtSpins,
							BOOL GhostPtSpins)
{
	// Set up some flags for rendering handles correctly
	RenderTrackEnd = rTrackEnd;
	RenderGhostEnd = rGhostEnd;
	TrackPointMoves = TrackPtMoves;
	GhostPointMoves = GhostPtMoves;
	TrackPointSpins = TrackPtSpins;
	GhostPointSpins = GhostPtSpins;
}


/********************************************************************************************

>  WobbleFlags::WobbleFlags(BOOL pSister  = FALSE,
							BOOL pBrother = TRUE,
							BOOL nBrother = FALSE,
							BOOL nSister  = FALSE)
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/9/94
	Purpose:	constructor for wobble flags class
	SeeAlso:	-

********************************************************************************************/

WobbleFlags::WobbleFlags(	BOOL pSister,
							BOOL pBrother,
							BOOL nBrother, 
							BOOL nSister)
{
	// Set up some flags for wobbling a curves control points correctly
	PrevSister	= pSister;
	PrevBrother = pBrother;
	NextBrother = nBrother;
	NextSister  = nSister;
};

/********************************************************************************************

	ControlPts::ControlPts() / ~ControlPts()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/9/94
	Purpose:	Control points class constructor and destructor
	SeeAlso:	-

********************************************************************************************/

ControlPts::ControlPts()
{
	pHndSpread = NULL;
}

ControlPts::~ControlPts()
{
}


/********************************************************************************************

>	OpPenHandles::OpPenHandles()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/9/94
	Purpose:	OpPenEorHandles constructor
	SeeAlso:	-

********************************************************************************************/

OpPenHandles::OpPenHandles()
{
	// default constructor (do nothing)
}


/********************************************************************************************

>	DocCoord OpPenHandles::GetTrackHandle() const;

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/9/94
	Inputs:		-
	Outputs:	The current track handle coordinate
	Purpose:	Return the private track handle coordinate for reference by none
				inclass code.

********************************************************************************************/

DocCoord OpPenHandles::GetTrackHandle() const
{
	return TrackEnd;
}

DocCoord OpPenHandles::GetGhostHandle()	const
{
	return GhostEnd;
}

DocCoord OpPenHandles::GetMidHandle() const
{
	return MidPoint;
}


/********************************************************************************************

>	DocCoord OpPenHandles::CalcGhostEnd(DocCoord start, DocCoord end) const;

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/9/94
	Inputs:		start	= point 1
				end		= point 2
	Returns:	ghost doc coordinate
	Purpose:	Calculate a ghost end coordinate given two input coordinates.
				ghost = point1 - (point2 - point1) 

********************************************************************************************/

DocCoord OpPenHandles::CalcGhostEnd(DocCoord start, DocCoord end) const
{
	DocCoord ghost;
	ghost.x = start.x - (end.x - start.x);
	ghost.y = start.y - (end.y - start.y);

	return ghost;
}



/********************************************************************************************

>	DocRect OpPenHandles::GetHandlesRect()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Returns:	DocRect, 
	Purpose:	Calculate the bounding rect of the drag blobs.

********************************************************************************************/

DocRect OpPenHandles::GetHandlesRect()
{
	// just set it to be an empty rectangle
	DocRect BoundRect (0,0,0,0);
	DocView* pDocView = DocView::GetSelected();
	ERROR2IF(pDocView == NULL, BoundRect, 
			 "No selected DocView in OpPenHandles::GetHandlesRect()");

	// Find the Rect around the drag blobs if there is any, otherwise return that bbox
	// of the start pos.

	OSRenderRegion::GetBlobRect(pDocView->GetViewScale(), TrackEnd, BT_SELECTEDLARGEST, &BoundRect);
	DocRect BlobRect;
	OSRenderRegion::GetBlobRect(pDocView->GetViewScale(), GhostEnd, BT_SELECTEDLARGEST, &BlobRect);
	BoundRect = BoundRect.Union(BlobRect);
	return BoundRect;
}



/********************************************************************************************

>	void OpPenHandles::RecalcHandles(DocCoord DragPos, BOOL constrain)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Inputs:		DragPos		= New position to take as the drag position
				constrain	= TRUE the constrain the handle positions
	Purpose:	Recalc handles, given a new drag position which is generally following the
				mouse pointer, will work out the relative positions of the drag handles.
				The drag handles can be made to follow the drag position absolutely, or
				reletively dependent on their start positions.

********************************************************************************************/

void OpPenHandles::RecalcHandles(DocCoord DragPos, BOOL constrain)
{

	// Recalculate the position of the handle which is following the
	// mouse.

 	DocCoord Constrain = DragPos;
	if (constrain)
		DocView::ConstrainToAngle(MidPoint, &Constrain);

	if (Hflags.TrackPointMoves)
			TrackEnd=Constrain;
	else if (Hflags.TrackPointSpins)
	{
		double Glen = TrackEnd.Distance(MidPoint);
		double Dlen = Constrain.Distance(MidPoint);
		DocCoord direction;
		direction.x = Constrain.x - MidPoint.x;
		direction.y = Constrain.y - MidPoint.y;
		if (Dlen!=0)
		{
			double ndx = direction.x / Dlen;
			double ndy = direction.y / Dlen;
			TrackEnd.x = MidPoint.x + (INT32)(Glen*ndx);
			TrackEnd.y = MidPoint.y + (INT32)(Glen*ndy);
		}
	}

	// Recalc the position of the handle opposite, the track handle accross
	// the midpoint.

	if (Hflags.GhostPointMoves)
	{
	 	GhostEnd.x = MidPoint.x - (Constrain.x - MidPoint.x);
		GhostEnd.y = MidPoint.y - (Constrain.y - MidPoint.y);
	}
	else if (Hflags.GhostPointSpins)
	{
		// if we're spinning, the ghost point will remain a set distance
		// from the midpoint, but just spin around coincident with
		// the drag pos.
		double Glen = GhostEnd.Distance(MidPoint);
		double Dlen = Constrain.Distance(MidPoint);
		DocCoord direction;
		direction.x = Constrain.x - MidPoint.x;
		direction.y = Constrain.y - MidPoint.y;
		if (Dlen!=0)
		{
			double ndx = direction.x / Dlen;
			double ndy = direction.y / Dlen;
			GhostEnd.x = MidPoint.x - (INT32)(Glen*ndx);
			GhostEnd.y = MidPoint.y - (INT32)(Glen*ndy);
		}
	}

}



/********************************************************************************************

>	void OpPenHandles::RenderHandles()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Purpose:	Render some drag control handles as the mouse moves around. These will
				spin around an anchor point.

********************************************************************************************/

void OpPenHandles::RenderHandles()
{
	DocRect Rect = GetHandlesRect();
	RenderRegion* pRegion = DocView::RenderOnTop( &Rect, StartSpread, ClippedEOR );
	while ( pRegion )
	{
		UpdateHandles(pRegion);
		pRegion = DocView::GetNextOnTop(NULL);
	}	
}


/********************************************************************************************

>	void OpPenHandles::UpdateHandles( RenderRegion* pRegion )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Inputs:		pRegion	=	Pointer to a region to draw into
	Purpose:	Render some drag control handles as the mouse moves around. These will
				spin around an anchor point. Their appearence and positioning is set
				by the flags variable hflags.
	SeeAlso:	Hflags


********************************************************************************************/

void OpPenHandles::UpdateHandles( RenderRegion* pRegion )
{
	pRegion->SetLineColour(COLOUR_BEZIERBLOB);
	pRegion->SetFillColour(COLOUR_TRANS);
	pRegion->DrawBlob(MidPoint, BT_SELECTED);

	if (TrackEnd != MidPoint)
	{
		pRegion->SetLineColour(COLOUR_BEZIERLINE);
		pRegion->SetFillColour(COLOUR_TRANS);

		if (Hflags.RenderTrackEnd && Hflags.RenderGhostEnd)
			pRegion->DrawLine(TrackEnd, GhostEnd);
		else
		{
			if (Hflags.RenderTrackEnd)
				pRegion->DrawLine(TrackEnd,MidPoint);
			if (Hflags.RenderGhostEnd)
				pRegion->DrawLine(GhostEnd,MidPoint);
 		}

		pRegion -> SetLineColour(COLOUR_TRANS);
		pRegion -> SetFillColour(COLOUR_UNSELECTEDBLOB);

		if (Hflags.RenderTrackEnd)
			pRegion -> DrawBlob(TrackEnd, BT_UNSELECTED);
		if (Hflags.RenderGhostEnd)
			pRegion -> DrawBlob(GhostEnd, BT_UNSELECTED);

	}
}


/********************************************************************************************

>	void OpPenHandles::SetHandleEnds(DocCoord ClickPos, DocCoord DragPos)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Purpose:	

********************************************************************************************/

void OpPenHandles::SetHandleEnds(DocCoord ClickPos, DocCoord DragPos, DocCoord GhostPos)
{
	MidPoint = ClickPos;
	TrackEnd = DragPos;
	GhostEnd = GhostPos;
}



/********************************************************************************************

>	OpPenHandles::SetDragHandles(	HandleFlags HandFlags,
									DocCoord Click,
									DocCoord Drag,
									DocCoord Ghost,
									Spread* pSpread)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/9/94
	Purpose:	Set up the initial positions of the drag handles. Note, the handles
				contain their own coordinates of where they think they are. These are
				separate from the drag mouse position. This allows the handles to represent
				a relation to the drag point and not an absolute position.
	SeeAlso:	-

********************************************************************************************/

void OpPenHandles::SetDragHandles(	HandleFlags HandFlags,
									DocCoord Click,
									DocCoord Drag,
									DocCoord Ghost,
									Spread* pSpread)
{
	StartMousePos = Click;
	CurrentMousePos = Click;
	StartSpread = pSpread;
	Hflags = HandFlags;
	SetHandleEnds(Click, Drag, Ghost);
}


/********************************************************************************************

>	void OpPenHandles::ChangeTrackHandle(DocCoord newpos, BOOL constrain)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Purpose:	

********************************************************************************************/

void OpPenHandles::ChangeTrackHandle(DocCoord newpos, BOOL constrain)
{
	CurrentMousePos = newpos;
	RecalcHandles(newpos, constrain);
}



/********************************************************************************************

>	void OpPenHandles::HasMouseMoved(DocCoord& OriginalPoint, DocCoord& PointerPos, double threshold=2)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Inputs		OriginalPoint = usually the last mouse position recorded
				PointerPos	  =	usually the current mouse position to check
				threshold	  = a value to multiply the scaled pixel size for the view by.
	Purpose:	Checks to see whether two coordinates are the same. The current test also
				checks for movement of less than two pixels in the scaled view. If the mouse
				has moved less than threshold*ScaledPixelSize then the points are considered
				equivalent.

********************************************************************************************/

BOOL OpPenHandles::HasMouseMoved(DocCoord& OriginalPoint, DocCoord& PointerPos, double threshold)
{
	if (OriginalPoint==PointerPos)
		return FALSE;

	INT32 dist = (INT32)(OriginalPoint.Distance(PointerPos)+0.5);
	
	FIXED16 ScaledPixelWidth,
			ScaledPixelHeight;
	GetWorkingView()->GetScaledPixelSize(&ScaledPixelWidth, &ScaledPixelHeight);

	INT32 PixWidth 	= ScaledPixelWidth.MakeLong();
	INT32 PixHeight	= ScaledPixelHeight.MakeLong();

	PixWidth	= (INT32)(PixWidth*threshold+0.5);
	PixHeight	= (INT32)(PixHeight*threshold+0.5);

	INT32 range;
	(PixWidth<PixHeight) ? (range=PixHeight) : (range=PixWidth);

	return (dist>range);
}	



/********************************************************************************************

>	void OpPenHandles::RenderTestRect( DocRect Rect, Spread* pSpread, StockColour colour )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Purpose:	Render a black rectangle. Useful for checking render regions and clipping

********************************************************************************************/

void OpPenHandles::RenderTestRect( DocRect Rect, Spread* pSpread, StockColour colour )
{
	if (Error::IsUserName("Mike"))
	{
		TRACE( _T("RectLox=%d\n"),Rect.lo.x);
		TRACE( _T("RectLoy=%d\n"),Rect.lo.y);
		TRACE( _T("RectHix=%d\n"),Rect.hi.x);
		TRACE( _T("RectHiy=%d\n\n"),Rect.hi.y);
	}

	RenderRegion* pRegion = DocView::RenderOnTop( &Rect, pSpread, ClippedEOR );
	while ( pRegion )
	{
		pRegion -> SetFillColour(colour);
		pRegion -> DrawRect(&Rect);
		pRegion = DocView::GetNextOnTop(NULL);
	}
}

/********************************************************************************************

>	OpPenDragBlobs::OpPenDragBolbs()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/9/94
	Purpose:	OpPenDragBlobs constructor
	SeeAlso:	-

********************************************************************************************/

OpPenDragBlobs::OpPenDragBlobs()
{
}


/********************************************************************************************

>	BOOL OpPenDragBlobs::DoPenDragBlobs()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Inputs:		tptr	=	pointer to calling tool.
	Outputs:	-
	Returns:	TRUE if able to start the drag, FALSE if not
	Purpose:	
	Errors:		failandexecute will be called if the operation fails in some way, most
				likely when no memory is available. 

********************************************************************************************/
	
BOOL OpPenDragBlobs::DoPenDragBlobs()
{   
	// And tell the Dragging system that we need drags to happen
	DocRect HandlesRect = GetHandlesRect();
	return StartDrag(DRAGTYPE_AUTOSCROLL, &HandlesRect, &CurrentMousePos);
}



/********************************************************************************************

>	void OpPenDragBlobs::DragPointerMove(	DocCoord PointerPos,
											ClickModifiers ClickMods,
											Spread* pSpread,
											BOOL bSolidDrag)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods - Which key modifiers are being pressed
	Purpose:	This is called every time the mouse moves, during a drag.
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpPenDragBlobs::DragPointerMove(DocCoord PointerPos, 
									 ClickModifiers ClickMods,
									 Spread* pSpread,
									 BOOL bSolidDrag)
{
	// If drag has moved onto a different spread, convert the coord to be relative to the
	// original spread.
	if (pSpread != StartSpread)
		PointerPos = MakeRelativeToSpread(StartSpread, pSpread, PointerPos);

	// Make sure we get the grid snapping involved
	DocCoord SnapPos = PointerPos;
	DocView::SnapCurrent(pSpread,&SnapPos);

	// Rub out the old EORed version of the handles, and stick some new ones on.
	RenderHandles();
	ChangeTrackHandle(SnapPos, ClickMods.Constrain);
	RenderHandles();
}



/********************************************************************************************

>	void OpPenDragBlobs::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods,
											Spread* pSpread, BOOL Success, BOOL bSolidDrag)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickMods - the key modifiers being pressed
				Success - TRUE if the drag was terminated properly, FALSE if it
				was ended with the escape key being pressed
	Purpose:	This is called when a drag operation finishes.
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpPenDragBlobs::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
										Spread* pSpread, BOOL Success, BOOL bSolidDrag)
{
	// terminate the drag and operation
	EndDrag();		
}



/********************************************************************************************

>	virtual void OpPenDragBlobs::RenderDragBlobs( DocRect Rect, Spread* pSpread, BOOL bSolidDrag )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Purpose:	This function is called to render our drag blobs during scrolling.

********************************************************************************************/

void OpPenDragBlobs::RenderDragBlobs( DocRect Rect, Spread* pSpread, BOOL bSolidDrag )
{
	if (StartSpread!=pSpread) return;
	RenderRegion* pRegion = DocView::RenderOnTop( &Rect, pSpread, ClippedEOR );
	while ( pRegion )
	{
		UpdateHandles(pRegion);
		pRegion = DocView::GetNextOnTop(NULL);
	}
}




/********************************************************************************************

>	OpPenCreateInternal::OpPenCreateInternal()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/9/94
	Purpose:	OpPenCreateInternal constructor
	SeeAlso:	-

********************************************************************************************/

OpPenCreateInternal::OpPenCreateInternal()
{
}



/********************************************************************************************

>	BOOL OpPenCreateInternal::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/9/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpDeletePoints initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpPenCreateInternal::Init()
{
	return (RegisterOpDescriptor(0,										// tool ID
 								_R(IDS_PENCREATEINTERNALOP),				// string resource ID
								CC_RUNTIME_CLASS(OpPenCreateInternal),	// runtime class for Op
 								OPTOKEN_PENCREATEINTERNAL,				// Ptr to token string
 								OpPenCreateInternal::GetState,			// GetState function
 								0,										// help ID = 0
 								_R(IDBBL_PENCREATEINTERNALOP),				// bubble help ID = 0
 								0										// resource ID = 0
 								)); 

}               


/********************************************************************************************

>	OpState	OpPenCreateInternal::GetState(String_256*, OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpPenCreateInternal
	Purpose:	For finding the OpPenCreateInternal's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpPenCreateInternal::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
	return OpSt;   
}







/********************************************************************************************

>	void OpPenCreateInternal::DoPenCreateInternal(	DocCoord Anchor, 
													Spread *pSpread, 
													ControlPts* pHandles)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Inputs:		Anchor	=	position of first click
				pSpread	=	pointer to spread where first click occured
				pHandles=	pointer to handles block
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		failandexecute will be called if the operation fails in some way, most
				likely when no memory is available. 

********************************************************************************************/
	
void OpPenCreateInternal::DoPenCreateInternal(DocCoord Anchor, Spread* pSpread, ControlPts* pHandles)
{   
	// initialise the drag handles into the default mode.

	pUserHandles = pHandles;
	HandleFlags Flags;

	// Make sure we get the grid snapping involved
	DocCoord SnapPos = Anchor;
	DocView::SnapCurrent(pSpread,&SnapPos);

	SetDragHandles(Flags, SnapPos, SnapPos, SnapPos, pSpread);

	// call the base classes drag init function
	if (!DoPenDragBlobs())
	{
		FailAndExecute();
		End();
		return;
	}

	// Render the current drag blobs where necessary
	RenderHandles();
}




/********************************************************************************************

>	void OpPenCreateInternal::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods,
											Spread* pSpread, BOOL Success, BOOL bSolidDrag)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickMods - the key modifiers being pressed
				Success - TRUE if the drag was terminated properly, FALSE if it
				was ended with the escape key being pressed
	Purpose:	This is called when a drag operation finishes.
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpPenCreateInternal::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
										Spread* pSpread, BOOL Success, BOOL bSolidDrag)
{
	// Rub out the old EORed version of the handles
	RenderHandles();

	// inform the base class to stop dragging
	OpPenDragBlobs::DragFinished( PointerPos, ClickMods, pSpread, Success, bSolidDrag);

	// inform the pen tool that a drag has come to an end
	if (Success)
	{
		pUserHandles->HndClick = GetMidHandle();
		pUserHandles->HndDrag = GetTrackHandle();
		pUserHandles->pHndSpread = StartSpread;
	}
	else
		FailAndExecute();
	// End the operation
	End();
}




/********************************************************************************************

>	OpPenEditInternal::OpPenEditInternal()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/9/94
	Purpose:	OpPenEditInternal constructor
	SeeAlso:	-

********************************************************************************************/

OpPenEditInternal::OpPenEditInternal()
{
}


/********************************************************************************************

>	BOOL OpPenEditInternal::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/9/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpPenEditInternal initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpPenEditInternal::Init()
{
	return (RegisterOpDescriptor(0,										// tool ID
 								_R(IDS_PENEDITINTERNALOP),					// string resource ID
								CC_RUNTIME_CLASS(OpPenEditInternal),	// runtime class for Op
 								OPTOKEN_PENEDITINTERNAL,				// Ptr to token string
 								OpPenEditInternal::GetState,			// GetState function
 								0,										// help ID = 0
 								_R(IDBBL_PENEDITINTERNALOP),				// bubble help ID = 0
 								0										// resource ID = 0
 								)); 

}               


/********************************************************************************************

>	OpState	OpPenEditInternal::GetState(String_256*, OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpPenEditInternal
	Purpose:	For finding the OpPenEditInternal's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpPenEditInternal::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
	return OpSt;   
}



/********************************************************************************************

>	void OpPenEditInternal::DoPenEditInternal(ControlPts* pHandles)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Inputs:		pHandles	= Pointer to a handles block containing click pos, drag pos
							  and spread ptr
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		failandexecute will be called if the operation fails in some way, most
				likely when no memory is available. 

********************************************************************************************/
	
void OpPenEditInternal::DoPenEditInternal(ControlPts* pHandles)
{   

	// Set the internal state
	pUserHandles = pHandles;

	HandleFlags Flags;
	DocCoord ghst = CalcGhostEnd(pHandles->HndClick, pHandles->HndDrag);
	SetDragHandles(Flags, pHandles->HndClick, pHandles->HndDrag, ghst, pHandles->pHndSpread);

	// call the base classes drag init function
	if (!DoPenDragBlobs())
	{
		FailAndExecute();
		End();
		return;
	}
}


/********************************************************************************************

>	void OpPenEditInternal::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods,
											Spread* pSpread, BOOL Success, BOOL bSolidDrag)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickMods - the key modifiers being pressed
				Success - TRUE if the drag was terminated properly, FALSE if it
				was ended with the escape key being pressed
	Purpose:	This is called when a drag operation finishes.
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpPenEditInternal::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
										Spread* pSpread, BOOL Success, BOOL bSolidDrag)
{
	// Rub out the old EORed version of the handles
	RenderHandles();

	// inform the base class to stop dragging
	OpPenDragBlobs::DragFinished( PointerPos, ClickMods, pSpread, Success, bSolidDrag);

	// inform the pen tool that a drag has come to an end
	if (Success)
	{
		pUserHandles->HndClick  = GetMidHandle();
		pUserHandles->HndDrag   = GetTrackHandle();
		pUserHandles->pHndSpread = StartSpread;
	}
	else
		FailAndExecute();

	// terminate the op
	End();		
}




/********************************************************************************************

>	OpPenEditPath::OpPenEditPath()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Purpose:	OpPenEditPath constructor
	SeeAlso:	-

********************************************************************************************/

OpPenEditPath::OpPenEditPath()
{
}

/********************************************************************************************

>	OpPenEditPath::SetWobbleIndex(WobbleFlags wibble, const INT32 index)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Inputs:		wibble	= wobble flags indicating which control handles will follow those
						  of the drag handles.
				index	= the index into our edit curve of the endpoint handle whose brother
						  and sister handles are being dragged around.
	Outputs:	-
	Purpose:	Set the control handle index of our edit curve which will follow the
				dragging control handles.
	SeeAlso:	-

********************************************************************************************/

void OpPenEditPath::SetWobbleIndex(WobbleFlags wibble, const INT32 index)
{
	Wobble = wibble;
	WobbleIndex = index;
}


		
/********************************************************************************************

>	BOOL OpPenEditPath::DoPenDragPath(Path* pEditPath)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Inputs:		pEditPath = pointer a path to begin editing.
	Returns:	TRUE if able to start the drag, FALSE otherwise
	Returns:	-
	Purpose:	

********************************************************************************************/
	
BOOL OpPenEditPath::DoPenDragPath()
{   
	// Tell the Dragging system that we need drags to happen
	DocRect HandlesRect = GetBoundingRect();
	return StartDrag(DRAGTYPE_AUTOSCROLL, &HandlesRect, &CurrentMousePos);
}



/********************************************************************************************

>	void OpPenEditPath::WobbleCoords

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		
	Purpose:	Moves the control handles around dependent on the classes wobble flags

********************************************************************************************/

void OpPenEditPath::WobbleCoords()
{
	DocCoord* Coords = pEditPath->GetCoordArray();

	if (Wobble.PrevBrother)
		Coords[WobbleIndex-1] = GetGhostHandle();

	if (Wobble.PrevSister)
	{
		Coords[WobbleIndex-2].x = (2*Coords[WobbleIndex-3].x + Coords[WobbleIndex-1].x)/3;
		Coords[WobbleIndex-2].y = (2*Coords[WobbleIndex-3].y + Coords[WobbleIndex-1].y)/3;
	}

	if (Wobble.NextBrother)
		Coords[WobbleIndex+1] = GetTrackHandle();

	if (Wobble.NextSister)
	{
		Coords[WobbleIndex+2].x = (Coords[WobbleIndex+1].x + 2*Coords[WobbleIndex+3].x)/3;
		Coords[WobbleIndex+2].y = (Coords[WobbleIndex+1].y + 2*Coords[WobbleIndex+3].y)/3;
	}
}


/********************************************************************************************

>	void OpPenEditPath::DragPointerMove(DocCoord PointerPos, 
										ClickModifiers ClickMods,
										Spread* pSpread, BOOL bSolidDrag)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods - Which key modifiers are being pressed
	Purpose:	This is called every time the mouse moves, during a drag.
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpPenEditPath::DragPointerMove(DocCoord PointerPos,
									ClickModifiers ClickMods,
									Spread* pSpread, BOOL bSolidDrag)
{
	// If drag has moved onto a different spread, convert the coord to be relative to the
	// original spread.
	if (pSpread != StartSpread)
		PointerPos = MakeRelativeToSpread(StartSpread, pSpread, PointerPos);

	// Make sure we get the grid snapping involved
	DocCoord SnapPos = PointerPos;
	DocView::SnapCurrent(pSpread,&SnapPos);

	if (CurrentMousePos != SnapPos)
	{

		// Rub out the old EORed version of the path and stick a new one on
		DocRect Rect = OpPenEditPath::GetBoundingRect();
		RenderDragBlobs(Rect, StartSpread, bSolidDrag);

		Rect.IncludePoint(SnapPos);

		// Alter the curve drag handles
		ChangeTrackHandle(SnapPos, ClickMods.Constrain);
		
		// make sure we wobble all the coordinates around this control point
		// correctly.
		WobbleCoords();

		// Now render the blobs back on again
		Rect = Rect.Union(OpPenEditPath::GetBoundingRect());
		RenderDragBlobs(Rect, StartSpread, bSolidDrag);
	}

}



/********************************************************************************************

>	DocRect OpPenEditPath::GetBoundingRect()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Inputs:		-
	Purpose:	Find the bounding box of the edit path and the drag handles.

********************************************************************************************/

DocRect OpPenEditPath::GetBoundingRect()
{

	DocRect Rect;	
	DocView* pDocView = DocView::GetSelected();
	ENSURE( pDocView != NULL, "There was no selected docview when getting the bounding box" );

	if ( pDocView != NULL )
	{
		DocRect BlobRect;
		DocCoord Coord;
		Rect = pEditPath->GetBoundingRect();
		Rect = Rect.Union(OpPenHandles::GetHandlesRect());
		// inflate by 1/4 of an inch to avoid eor problems
		Rect.Inflate(72000>>2);
	}

	return Rect;
}


/********************************************************************************************

>	DocRect OpPenEditPath::ConvertToLine(const INT32 index)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Inputs:		index	= index of element to remove from.
	Purpose:	Convert the currently edited curve element into a line

********************************************************************************************/

void OpPenEditPath::ConvertToLine(const INT32 index)
{
	// remove the curve element, then stick a none smooth line
	// segment in there.

	pEditPath->DeleteFromElement(index);
	PathFlags tempflags;
	tempflags.IsSelected = TRUE;
	pEditPath->InsertLineTo(GetMidHandle(), &tempflags);
}



/********************************************************************************************

>	void OpPenEditPath::RemoveRotateEnd(const INT32 index)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Inputs:		index	= index of bezierto element
	Purpose:	Remove any rotate bits from a bezier element, starting at index
	
********************************************************************************************/

void OpPenEditPath::RemoveRotateEnd(const INT32 index)
{
	// Remove all rotate values from the end curve element
	PathFlags* Flags = pEditPath->GetFlagArray();
	Flags[index].IsRotate = FALSE;
	Flags[index+1].IsRotate = FALSE;
	Flags[index+2].IsRotate = FALSE;
}


/********************************************************************************************

>	void OpPenEditPath::InsertBezier(DocCoord pt1, DocCoord pt2, DocCoord pt3)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Inputs:		pt1,pt2,pt3 = coords of control points for bezier
	Purpose:	Add a specific curve at the end of the path
	
********************************************************************************************/

BOOL OpPenEditPath::InsertBezier(DocCoord pt1, DocCoord pt2, DocCoord pt3)
{
	// insert a particular type of curve
	PathFlags tempflags;
	tempflags.IsRotate = TRUE;
	BOOL done = pEditPath->InsertCurveTo(pt1, pt2, pt3, &tempflags);

	if (done)
	{
		INT32 npts = pEditPath->GetNumCoords();
		PathFlags* Flags = pEditPath->GetFlagArray();
		Flags[npts-1].IsSelected = TRUE;
	}
	return done;
}



/********************************************************************************************

>	void OpPenEditPath::InsertCurvedLine(DocCoord pt1, DocCoord pt2, DocCoord pt3)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/94
	Inputs:		pt1,pt2 = coords of the end control points for the bezier
	Purpose:	Add a specific curve at the end of the path. The curve is made of colinear
				points
	
********************************************************************************************/

BOOL OpPenEditPath::InsertCurvedLine(DocCoord pt1, DocCoord pt2)
{

	BOOL ok = pEditPath->InsertMoveTo(pt1, NULL);
	if (ok)
	{
		DocCoord Cpt1;
		Cpt1.x = (2*pt1.x + pt2.x)/3;
		Cpt1.y = (2*pt1.y + pt2.y)/3;

		DocCoord Cpt2;
		Cpt2.x = (pt1.x + 2*pt2.x)/3;
		Cpt2.y = (pt1.y + 2*pt2.y)/3;

		ok = InsertBezier(Cpt1, Cpt2, pt2);

	}
	return ok;
} 


/********************************************************************************************

>	BOOL OpPenEditPath::InsertGhostedBezier(DocCoord pt1, DocCoord pt2, DocCoord pt3)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/94
	Inputs:		pt1,pt2 = coords of the end control points of a previous bezier
				pt3		= coordinate for new end point of inserted bezier
	Purpose:	Add a specific curve at the end of the path.
				The curve control points are made up of	
				{ pt2, Ghost(pt1,pt2), LinIntpl(Ghost,pt3), pt3 }
	
********************************************************************************************/

BOOL OpPenEditPath::InsertGhostedBezier(DocCoord pt1, DocCoord pt2, DocCoord pt3)
{
	BOOL ok = pEditPath->InsertMoveTo(pt2, NULL);
	if (ok)
	{
		DocCoord Cpt1;
		Cpt1.x = pt2.x - (pt1.x - pt2.x);
		Cpt1.y = pt2.y - (pt1.y - pt2.y);

		DocCoord Cpt2;
		Cpt2.x = (Cpt1.x + 2*pt3.x)/3;
		Cpt2.y = (Cpt1.y + 2*pt3.y)/3;

		ok = InsertBezier(Cpt1, Cpt2, pt3);
	}
	return ok;
}



/********************************************************************************************

>	BOOL OpPenEditPath::ConvertPathEnd(Path* pDocPath)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/94
	Inputs:		-
	Purpose:	Common code to convert an edited element to a line, or remove its
				rotation bits. This should happen when the user simply clicks and does
				not perform a drag. The built curve element should be replaced by either
				a none smoothed curve or a line.

********************************************************************************************/

BOOL OpPenEditPath::ConvertPathEnd(Path* pDocPath)
{
	BOOL converted = FALSE;

	// set the path position and read the coordinate data
	INT32 LastEl	= pDocPath->GetNumCoords();
	pDocPath->SetPathPosition(LastEl-1);

	DocCoord  CtrlPtC = pDocPath->GetCoord();
	PathVerb  CtrlPtV = (pDocPath->GetVerb()) & ~PT_CLOSEFIGURE;
	PathFlags CtrlPtF = pDocPath->GetFlags();

	// now create an element dependent on the control point type

	switch (CtrlPtV)
	{
		case PT_BEZIERTO:
			if (CtrlPtF.IsRotate)
				RemoveRotateEnd(1);
			else
				ConvertToLine(1);
				converted=TRUE;
			break;

		case PT_LINETO:
			ConvertToLine(1);
			converted=TRUE;
			break;
	}
	return converted;
}



/********************************************************************************************

>	void OpPenEditPath::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods,
											Spread* pSpread, BOOL Success, BOOL bSolidDrag)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Inputs:		PointerPos	- The position of the mouse at the end of the drag
				ClickMods	- the key modifiers being pressed
				Success		- TRUE if the drag was terminated properly, FALSE if it
				was ended with the escape key being pressed
	Purpose:	This is called when a drag operation finishes.
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpPenEditPath::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
								  Spread* pSpread, BOOL Success, BOOL bSolidDrag)
{
	// terminate the drag and operation
	EndDrag();		
}



/********************************************************************************************

>	void OpPenEditPath::RenderDragBlobs( DocRect Rect, Spread* pSpread, BOOL bSolidDrag )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Purpose:	Render some drag control handles as the mouse moves around. These will
				spin around an anchor point.

********************************************************************************************/

void OpPenEditPath::RenderDragBlobs( DocRect Rect, Spread* pSpread, BOOL bSolidDrag )
{

	RenderRegion* pRegion = DocView::RenderOnTop( &Rect, StartSpread, ClippedEOR );
	while ( pRegion )
	{
		// Get the edit path to render itself
		pRegion -> SetLineColour(COLOUR_XOREDIT);
		pRegion -> SetLineWidth(0);
		pRegion -> DrawPath(pEditPath);

		// Render the drag handles on top
		OpPenHandles::UpdateHandles(pRegion);

		pRegion = DocView::GetNextOnTop(NULL);
	}
}




/********************************************************************************************

>	OpPenCreatePath::OpPenCreatePath()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Purpose:	OpPenCreatePath constructor
	SeeAlso:	-

********************************************************************************************/

OpPenCreatePath::OpPenCreatePath()
{
	// Dummy constructor
}


/********************************************************************************************

>	BOOL OpPenCreatePath::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpPenCreatePath initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpPenCreatePath::Init()
{
	return (RegisterOpDescriptor(0,										// tool ID
 								_R(IDS_PENCREATEPATHOP),					// string resource ID
								CC_RUNTIME_CLASS(OpPenCreatePath),		// runtime class for Op
 								OPTOKEN_PENCREATEPATH,					// Ptr to token string
 								OpPenCreatePath::GetState,				// GetState function
 								0,										// help ID = 0
 								_R(IDBBL_PENCREATEPATHOP),					// bubble help ID = 0
 								0										// resource ID = 0
 								)); 

}               



/********************************************************************************************

>	OpState	OpPenCreatePath::GetState(String_256*, OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpPenCreatePath
	Purpose:	For finding the OpPenCreatePath's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
				
OpState OpPenCreatePath::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;
	// Always enabled at the moment.
	return OpSt;
}


/********************************************************************************************

>	void OpPenCreatePath::DoPenCreatePath(ControlPts* pHandles,
										  DocCoord ClickPoint,
										  Spread *pClickSpread, 
										  Path* pEditPath)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Inputs:		pHandles	=	pointer to a handles block containing drag handle info
				ClickPoint 	=	the last mouse click position
				pClickSpread=	pointer to spread where the click occured
				pEditPath	=	pointer to path to edit
				
	Outputs:	pEditPath	=	Updated to contain a single path element, either
								a curve or line dependent on whether the operation
								performs a drag.
	Returns:	-
	Purpose:	Creates a path element when the user clicks or drags at a particular point.
				The edit path will contain the necessary element when the operation has
				finished. If the operation is not a success, the edit path will contain
				nothing.
	Errors:		failandexecute will be called if the operation fails in some way, most
				likely when no memory is available. 

********************************************************************************************/
	
void OpPenCreatePath::DoPenCreatePath(	ControlPts* pHandles,
										DocCoord ClickPoint,
										Spread *pClickSpread, 
										Path* pPath)
{   
	// ok, we always create a curve element in the path and descover whether
	// the points all lie in a straight line at the end of the drag. If so, we
	// will alter the path element back to a line at that stage.

	BOOL ok;
	
	pEditPath = pPath;
	
	Handle0 = pHandles->HndClick;
	Handle1 = pHandles->HndDrag;

	// make the new click point relative to our handle coordinates.
	if (pHandles->pHndSpread != pClickSpread)
		ClickPoint = MakeRelativeToSpread(pHandles->pHndSpread, pClickSpread, ClickPoint);

	// make sure we get the grid snapping involved
	DocCoord SnapPos = ClickPoint;
	DocView::SnapCurrent(pClickSpread,&SnapPos);

	// create the first element in the path
	ok = (pEditPath->InsertMoveTo(Handle0, NULL));

	if (ok)
	{
		DocCoord Cpt1 = Handle1;
		if (Handle0 == Handle1)
		{
			Cpt1.x = (2*Handle0.x + SnapPos.x)/3;
			Cpt1.y = (2*Handle0.y + SnapPos.y)/3;
		}
		DocCoord Cpt2;
		Cpt2.x = (Handle1.x + 2*SnapPos.x)/3;
		Cpt2.y = (Handle1.y + 2*SnapPos.y)/3;

		ok = InsertBezier(Cpt1, Cpt2, SnapPos);
	}
	
	if (ok)
	{
		// Set info about the click point and spread
		HandleFlags hFlags;
		SetDragHandles(hFlags, SnapPos, SnapPos, SnapPos, pClickSpread);
			
		// Set the edit control to tell the drag code which 
		// point to overwrite 
		WobbleFlags wFlags;
		SetWobbleIndex(wFlags, 3);

 		// Render the first eor version on
  		DocRect Rect = GetBoundingRect();
		RenderDragBlobs(Rect, StartSpread, FALSE);
		
		// Now start the drag op on this path
		ok = OpPenEditPath::DoPenDragPath();
	}

	if (!ok)
	{
		// failed to perform create so lets tidy up the path and exit
		pEditPath->ClearPath();
		FailAndExecute();
		End();
	}
}	




/********************************************************************************************

>	void OpPenCreatePath::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods,
										Spread* pSpread, BOOL Success, BOOL bSolidDrag)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Inputs:		PointerPos	- The position of the mouse at the end of the drag
				ClickMods	- the key modifiers being pressed
				Success		- TRUE if the drag was terminated properly, FALSE if it
				was ended with the escape key being pressed
	Purpose:	This is called when a drag operation finishes.
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpPenCreatePath::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
									Spread* pSpread, BOOL Success, BOOL bSolidDrag)
{
	// inform the base class to stop dragging
	OpPenEditPath::DragFinished( PointerPos, ClickMods, pSpread, Success, bSolidDrag);

	// Rub out the old EORed version of the path
	DocRect Rect = GetBoundingRect();
	RenderDragBlobs( Rect, StartSpread, bSolidDrag);

 	if (Success)
	{
		if (!HasMouseMoved(StartMousePos,CurrentMousePos))
		{
			// nothing moved so check to see if we can convert to a line
			if (Handle0 == Handle1)
				ConvertToLine(1);			// delete the curve element and create a line!
			else
				RemoveRotateEnd(1);			// make sure the end has no smooth bits set
		}
	}
	else
		FailAndExecute();

	End();
}





/********************************************************************************************

>	OpPenAddElement::OpPenAddElement()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Purpose:	OpPenAddElement constructor.
	SeeAlso:	-

********************************************************************************************/

OpPenAddElement::OpPenAddElement()
{
}



/********************************************************************************************

>	BOOL OpPenAddElement::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpPenAddElement initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpPenAddElement::Init()
{
	return (RegisterOpDescriptor(0,										// tool ID
 								_R(IDS_PENADDELEMENTOP),					// string resource ID
								CC_RUNTIME_CLASS(OpPenAddElement),		// runtime class for Op
 								OPTOKEN_PENADDELEMENT,					// Ptr to token string
 								OpPenAddElement::GetState,				// GetState function
 								0,										// help ID = 0
 								_R(IDBBL_PENADDELEMENTOP),					// bubble help ID = 0
 								0										// resource ID = 0
 								)); 

}               




/********************************************************************************************

>	OpState	OpPenAddElement::GetState(String_256*, OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpPenAddElement
	Purpose:	For finding the OpPenAddElement's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
				
OpState OpPenAddElement::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;
	// Always enabled at the moment.
	return OpSt;
}



/********************************************************************************************

>	void OpPenAddElement::DoPenAddElement(	NodePath* pNode,
											INT32 Attach,
											DocCoord ClickPoint,
									 		Spread *pClickSpread, 
									 		Path* pPath)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94

	Inputs:		pNode		=	pointer to path in document to be edited
				Attach		=	index of coordinate to attach new element to
				ClickPoint 	=	position of click over document.
				pClickSpread=	pointer to spread where first click occured
				pEditPath	=	pointer to path to edit
				
	Outputs:	-
	Returns:	-
	Purpose:	This function starts of the dragging of a new path element which will be
				added to the path held within pNode. The add position is taken as the
				paths CurrentPosition variable.

	Errors:		failandexecute will be called if the operation fails in some way, most
				likely when no memory is available. 

********************************************************************************************/
	
void OpPenAddElement::DoPenAddElement(	NodePath* pNode,
										INT32 Attach,
										DocCoord ClickPoint,
									 	Spread *pClickSpread, 
									 	Path* pPath)
{   
	// right we need to build a new path element, whose start coordinate and control is
	// determined by the end element of the passed docpath.
	// here are the rules for element addition
	// (1)	A curve is always added, and possibly converted to a line at the end
	//		of the drag
	// (2)	The curve elements smoothness is controled by the previous path elements
	//		smoothness. ie if we are adding to a smooth end point then the element
	//		we create will form a smooth join.
	// (3)	If the previous element is a line, we will create either a none smooth
	//		curve or another line, dependent on the drag.

	BOOL ok = FALSE;
	pEditPath = pPath;
	pEditNode = pNode;
    EditIndex = Attach;

	// Make sure the click point is in the correct coordinate system
	Spread* pPathSpread = pNode->FindParentSpread();
	if (pPathSpread != pClickSpread)
		ClickPoint = MakeRelativeToSpread(pPathSpread, pClickSpread, ClickPoint);

	// Get some interresting details about the path...
	Path* pDocPath = &(pNode->InkPath);
	INT32 ncoords = pDocPath->GetNumCoords();

	pDocPath->SetPathPosition(EditIndex);

	// Ensure things are in range and sensible
	if (ncoords<=0)
	{
		FailAndExecute();
		End();
		return;
	}

	// Read the attach point control details
	DocCoord  AttachC = pDocPath->GetCoord();
	PathVerb  AttachV = pDocPath->GetVerb();
	PathFlags AttachF = pDocPath->GetFlags();

	WobbleFlags wFlags;

	// Make sure we get the grid snapping involved
	DocCoord SnapPos = ClickPoint;
	DocView::SnapCurrent(pClickSpread,&SnapPos);

	// now create an element dependent on the control point type

	switch (AttachV)
	{
		case PT_MOVETO:
		// Nasty option to control adding paths to the begining of the path
		{
			pDocPath->SetPathPosition(EditIndex+1);
			DocCoord NextC = pDocPath->GetCoord();
			PathFlags NextF = pDocPath->GetFlags();
			
			if (NextF.IsRotate)
				ok = InsertGhostedBezier(NextC, AttachC, SnapPos);
			else
			{
				wFlags.PrevSister = TRUE;
				ok = InsertCurvedLine(AttachC, SnapPos);
			}
		} 
		break;


		case PT_BEZIERTO:
		{
			pDocPath->SetPathPosition(EditIndex-1);
			DocCoord PrevC = pDocPath->GetCoord();

			if (AttachF.IsRotate)
				ok = InsertGhostedBezier(PrevC, AttachC, SnapPos);
			else
			{
				wFlags.PrevSister = TRUE;
				ok = InsertCurvedLine(AttachC, SnapPos);
			}
		}
		break;


		case PT_LINETO:
		{
			wFlags.PrevSister = TRUE;
			ok = InsertCurvedLine(AttachC, SnapPos);
		}
		break;
	}

	if (ok)
	{
		// if we've created the element, set the wobble flags
		SetWobbleIndex(wFlags,3);

		// Save info about the click point and spread
		HandleFlags hFlags;
		SetDragHandles(hFlags, SnapPos, SnapPos, SnapPos, pClickSpread);

		// Render the first eor version on
		DocRect Rect = GetBoundingRect();
		RenderDragBlobs(Rect, StartSpread, FALSE);

		// now attempt to start the drag		
		ok = OpPenEditPath::DoPenDragPath();
	}

	if (!ok)
	{
		// failed to perform create so lets tidy up the path and exit
		pEditPath->ClearPath();
		FailAndExecute();
		End();
	}
}	




/********************************************************************************************

>	void OpPenAddElement::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods,
										Spread* pSpread, BOOL Success, BOOL bSolidDrag)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickMods - the key modifiers being pressed
				Success - TRUE if the drag was terminated properly, FALSE if it
				was ended with the escape key being pressed
	Purpose:	This is called when a drag operation finishes.
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpPenAddElement::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
									Spread* pSpread, BOOL Success, BOOL bSolidDrag)
{
	// inform the base class to stop dragging
	OpPenEditPath::DragFinished( PointerPos, ClickMods, pSpread, Success, bSolidDrag);

	// Rub out the old EORed version of the path
	DocRect Rect = GetBoundingRect();
	RenderDragBlobs( Rect, StartSpread, bSolidDrag);

 	if (Success)
	{
		// if the mouse hasn't moved make the curve into a path
		if (!HasMouseMoved(StartMousePos,CurrentMousePos))
			ConvertPathEnd(&(pEditNode->InkPath));

		// if we're adding to the start of a subpath, reverse our edited path
		PathVerb* verbs = pEditNode->InkPath.GetVerbArray();
		if (verbs[EditIndex] == PT_MOVETO)
			pEditPath->Reverse();
	}
	else
		FailAndExecute();

	// Once done remove the edit path
	End();
}



/********************************************************************************************

>	OpPenClosePath::OpPenClosePath()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Purpose:	OpPenClosePath constructor.
	SeeAlso:	-

********************************************************************************************/

OpPenClosePath::OpPenClosePath()
{
	NumElements = 0;
	FirstDrag = TRUE;
}



/********************************************************************************************

>	BOOL OpPenClosePath::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpPenClosePath initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpPenClosePath::Init()
{
	return (RegisterOpDescriptor(0,										// tool ID
 								_R(IDS_PENCLOSEPATHOP),						// string resource ID
								CC_RUNTIME_CLASS(OpPenClosePath),		// runtime class for Op
 								OPTOKEN_PENCLOSEPATH,					// Ptr to token string
 								OpPenClosePath::GetState,				// GetState function
 								0,										// help ID = 0
 								_R(IDBBL_PENCLOSEPATHOP),					// bubble help ID = 0
 								0										// resource ID = 0
 								)); 

}               




/********************************************************************************************

>	OpState	OpPenClosePath::GetState(String_256*, OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpPenClosePath
	Purpose:	For finding the OpPenClosePath's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
				
OpState OpPenClosePath::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;
	// Always enabled at the moment.
	return OpSt;
}



/********************************************************************************************

>	void OpPenClosePath::DoPenClosePath(NodePath* pNode,
										INT32 ClickIndex,
										Spread *pSpread,
										Path* pPath)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Inputs:		pNode		=	pointer to nodepath in document to be edited
				ClickIndex	=	index of element where click occured.
				pSpread		=	pointer to spread where first click occured
				pPath		=	pointer to path to edit
				
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		failandexecute will be called if the operation fails in some way, most
				likely when no memory is available. 

********************************************************************************************/
	
void OpPenClosePath::DoPenClosePath(NodePath* pNode,
									INT32 ClickIndex,
									Spread *pSpread,
									Path* pPath)
{   

	BOOL ok = FALSE;
	pEditPath = pPath;
	pEditNode = pNode;
	EditIndex = ClickIndex;

	Path* pDocPath = &(pNode->InkPath);

	// find the extreems of this sub path. 
	INT32 lasti = ClickIndex;
	INT32 firsti = ClickIndex;
	pDocPath->FindEndElOfSubPath(&lasti);
	pDocPath->FindStartOfSubPath(&firsti);

	// ok the click could have occured over the begining of a subpath
	// or over the end of a subpath. The verb at ClickIndex should tell
	// us this.

	if ((lasti<=0) || (lasti<=firsti))
	{
		FailAndExecute();
		End();
		return;
	}

	OnMoveto = (ClickIndex == firsti);

	// Get the arrays for the destination path
	PathFlags* Flags  = pDocPath->GetFlagArray();
	PathVerb*  Verbs  = pDocPath->GetVerbArray();
	DocCoord*  Coords = pDocPath->GetCoordArray();

	DocCoord* EditCoords = pEditPath->GetCoordArray();

	// Read the final control point details
	PathVerb  LastV  = Verbs[lasti] & ~PT_CLOSEFIGURE;
	PathFlags LastF  = Flags[lasti];

	PathVerb  SecondV = Verbs[firsti+1] & ~PT_CLOSEFIGURE;
	PathFlags SecondF = Flags[firsti+1];

	// make sure the first index is sensible, ie offsets to a moveto
	if (Verbs[firsti] != PT_MOVETO)
	{
		FailAndExecute();
		End();
		return;
	}

	// dont render the track end until the pointer moves
	FirstHFlags.RenderTrackEnd = FALSE;

	// now create an element dependent on the control point type

	if (OnMoveto)
	{
		switch (LastV)
		{
			case PT_BEZIERTO:
				if (LastF.IsRotate)
					ok = InsertGhostedBezier(Coords[lasti-1], Coords[lasti], Coords[firsti]);
				else
				{
					FirstWFlags.PrevSister = TRUE;
					ok = InsertCurvedLine(Coords[lasti], Coords[firsti]);
				}
				NumElements++;
				break;
		
			case PT_LINETO:
				FirstWFlags.PrevSister = TRUE;
				ok = InsertCurvedLine(Coords[lasti], Coords[firsti]);
				NumElements++;
				break;

			default:
				FailAndExecute();
				End();
				return;
				break;
		}
		if (ok)
			SetDragHandles(FirstHFlags, Coords[firsti], Coords[firsti], EditCoords[2], pSpread);
	}
	else
	{
		switch (SecondV)
		{
			case PT_BEZIERTO:
				if (SecondF.IsRotate)
					ok = InsertGhostedBezier(Coords[firsti+1], Coords[firsti], Coords[lasti]);
				else
				{
					FirstWFlags.PrevSister = TRUE;
					ok = InsertCurvedLine(Coords[firsti], Coords[lasti]);
				}
				NumElements++;
				break;
		
			case PT_LINETO:
				FirstWFlags.PrevSister = TRUE;
				ok = InsertCurvedLine(Coords[firsti], Coords[lasti]);
				NumElements++;
				break;

			default:
				FailAndExecute();
				End();
				return;
				break;
		}
		if (ok)
			SetDragHandles(FirstHFlags, Coords[lasti], Coords[lasti], EditCoords[2], pSpread);
	}

	
	if (ok)
	{
		SetWobbleIndex(FirstWFlags,3);
 		// Render the first eor version on
  		DocRect Rect = GetBoundingRect();
		RenderDragBlobs(Rect, StartSpread, FALSE);
		// Now start the drag op on this path
		ok = OpPenEditPath::DoPenDragPath();
	}

	if (!ok) 
	{
		// failed to perform create so lets tidy up the path and exit
		pEditPath->ClearPath();
		FailAndExecute();
		End();
	}
}	



/********************************************************************************************

>	BOOL OpPenClosePath::AddFirstElement()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:
	Purpose:	This routine adds another element to the eor path. When closing a path
				the user can either click or drag on the start point of a path. Now, we
				dont really want to render what will become a smoothed start curve along
				with the close element, if just a click has occured. So we wait for the
				first DragPointerMove call and use this function to create the second
				element. This basically looks much neater on screen.
	
********************************************************************************************/

BOOL OpPenClosePath::AddFirstElement(Spread* pSpread)
{

	// Calculate the first control point on the forward bezier.
	// This is calculated from the last element inserted into the
	// edit curve, ie the last control points ghost point.

	Path* pDocPath = &(pEditNode->InkPath);

	// Get the arrays for this path
	PathFlags* Flags  = pDocPath->GetFlagArray();
	PathVerb*  Verbs  = pDocPath->GetVerbArray();
	DocCoord*  Coords = pDocPath->GetCoordArray();

	DocCoord* EditCoords = pEditPath->GetCoordArray();

	INT32 Associate = EditIndex;
	if (OnMoveto)
		Associate++;

	PathVerb AssociateV = Verbs[Associate] & ~PT_CLOSEFIGURE;

	DocCoord Control;
	Control.x = EditCoords[3].x - (EditCoords[2].x - EditCoords[3].x);
	Control.y = EditCoords[3].y - (EditCoords[2].y - EditCoords[3].y);

	BOOL ok = TRUE;
			
	switch (AssociateV)
	{
		case PT_BEZIERTO:
		{
			FirstHFlags.RenderTrackEnd = TRUE;
			FirstHFlags.TrackPointMoves = FALSE;
			FirstHFlags.TrackPointSpins = TRUE;
			FirstWFlags.NextBrother = TRUE;

			PathFlags tempflags = Flags[Associate];
			if (OnMoveto)
				ok = pEditPath->InsertCurveTo(Control, Coords[Associate+1], Coords[Associate+2], &tempflags);
			else
				ok = pEditPath->InsertCurveTo(Control, Coords[Associate-2], Coords[Associate-3], &tempflags);

			NumElements++;
		}
		break;

		case PT_LINETO:
		{
			FirstHFlags.RenderTrackEnd = FALSE;
			FirstHFlags.TrackPointMoves = FALSE;
			FirstHFlags.TrackPointSpins = FALSE;
		}
		break;
	}

	// if we've created the element, set the wobble flags
	if (ok) 
	{
		SetWobbleIndex(FirstWFlags,3);
		if (OnMoveto)
			SetDragHandles(FirstHFlags, Coords[EditIndex], Coords[EditIndex+1], EditCoords[2], pSpread);
		else
			SetDragHandles(FirstHFlags, Coords[EditIndex], Coords[EditIndex-1], EditCoords[2], pSpread);
	}

	return ok;
}




/********************************************************************************************

>	void OpPenClosePath::DragPointerMove(DocCoord PointerPos, 
										ClickModifiers ClickMods,
										Spread* pSpread, BOOL bSolidDrag)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods - Which key modifiers are being pressed
	Purpose:	This is called every time the mouse moves, during a drag.
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpPenClosePath::DragPointerMove(DocCoord PointerPos, 
									ClickModifiers ClickMods,
									Spread* pSpread, BOOL bSolidDrag)
{

	// If drag has moved onto a different spread, convert the coord to be relative to the
	// original spread.
	if (pSpread != StartSpread)
		PointerPos = MakeRelativeToSpread(StartSpread, pSpread, PointerPos);

	// Make sure we get the grid snapping involved
	DocCoord SnapPos = PointerPos;
	DocView::SnapCurrent(pSpread,&SnapPos);

	if (FirstDrag)
	{

		// Rub out the old EORed version of the path and stick a new one on
		DocRect Rect = GetBoundingRect();
		RenderDragBlobs(Rect, StartSpread, bSolidDrag);

		if (!AddFirstElement(pSpread))
		{
			pEditPath->ClearPath();
			EndDrag();
			FailAndExecute();
			End();
			return;
		}

		Rect.IncludePoint(SnapPos);

		// Alter the curve drag handles
		ChangeTrackHandle(SnapPos, ClickMods.Constrain);
		
		// make sure we wobble all the coordinates around this control point
		// correctly.
		WobbleCoords();

		// Now render the blobs back on again
		Rect = Rect.Union(GetBoundingRect());
		RenderDragBlobs(Rect, StartSpread, bSolidDrag);

	}
	else
	{
		OpPenEditPath::DragPointerMove(SnapPos, ClickMods, pSpread, bSolidDrag);
	} 

	FirstDrag = FALSE;
}




/********************************************************************************************

>	void OpPenClosePath::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods,
										Spread* pSpread, BOOL Success, BOOL bSolidDrag)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickMods - the key modifiers being pressed
				Success - TRUE if the drag was terminated properly, FALSE if it
				was ended with the escape key being pressed
	Purpose:	This is called when a drag operation finishes.
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpPenClosePath::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
									Spread* pSpread, BOOL Success, BOOL bSolidDrag)
{
	// inform the base class to stop dragging
	OpPenEditPath::DragFinished( PointerPos, ClickMods, pSpread, Success, bSolidDrag);

	// Rub out the old EORed version of the path
	DocRect Rect = GetBoundingRect();
	RenderDragBlobs( Rect, StartSpread, bSolidDrag);

 	if (Success)
	{
		if (!HasMouseMoved(StartMousePos,CurrentMousePos))
		{
			// if the mouse hasn't moved then we haven't edited the
			// first element in the path being closed, so bin our replacement
			// element.
			if (NumElements == 2)
			{
				INT32 last = 0;
				pEditPath->FindEndOfSubPath(&last);
				pEditPath->DeleteFromElement(last);				
				NumElements--;
			}
			ConvertPathEnd(&(pEditNode->InkPath));
		}
	}
	else
		FailAndExecute();

	End();
}



/********************************************************************************************

>	OpAddPath::OpAddPath()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Purpose:	OpAddPath constructor.
	SeeAlso:	-

********************************************************************************************/

OpAddPath::OpAddPath()
{
	// dummy constructor for the moment.
}



/********************************************************************************************

>	void OpAddPath::AugmentPathWithPath(Path* SourcePath, NodePath* DestinNode, INT32 after) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		SourcePath
				DestinNode
				pNode		= Pointer to nodepath in document to add element to
				pElement	= Pointer to path element to add
				after 		= offset of verb (line, move, curve) after which to add element 

	Outputs:	-
	Purpose:	This operation is called to update an existing path. If successful it will
				create a new node, copy the contents of pElement into it and apply the
				current attributes to it.

********************************************************************************************/


ExecuteType OpAddPath::AugmentPathWithPath(Path* pSourcePath, NodePath* pDestinNode, INT32 after)
{
#ifndef STANDALONE

	// Get a pointer to the path object (makes reading easier)
	Path* pDestPath = &(pDestinNode->InkPath);

	PathFlags* Flags = pDestPath->GetFlagArray();
	PathVerb* Verbs = pDestPath->GetVerbArray();
	DocCoord* Coords = pDestPath->GetCoordArray();

	PathVerb AfterVerb = Verbs[after] & ~PT_CLOSEFIGURE;

	// Set the insert position (v important for path->makespace func)
	pDestPath->SetPathPosition(after+1);

	// calc how many coords in the insert path
	INT32 NumInSource = pSourcePath->GetNumCoords();

	// If we're undoing, create an action for this insert
	Action* UnAction;				// pointer to action that might be created
	ActionCode Act;					// Action code that might be used

	// Remove selection from around this handle
	Act = DeselectHandle(pDestinNode, after);
	if (Act == AC_FAIL)
		return ExeInclusive;

	// if inserting after a moveto, we change the moveto.
	if (AfterVerb == PT_MOVETO)
	{
		// alter the coordinate of the move to
		Act = ModifyElementAction::Init(this, 
										&UndoActions,
										Verbs[after],
										Flags[after],
										Coords[after],
										after,
										pDestinNode,
										(Action**)&UnAction);

		if (Act == AC_FAIL)
			return ExeInclusive;

		DocCoord* SCoords = pSourcePath->GetCoordArray();
		Coords[after] =	SCoords[0];
		Flags[after].IsSelected = TRUE;

	}

	// record a record for the undo. we're inserting elements so we'll want to delete them during undo		
	Act = RemovePathElementAction::Init(this, &UndoActions, NumInSource-1, after+1, (Action**)(&UnAction));
	if (Act == AC_FAIL)
		return ExeInclusive;

	// record the path pointer where the remove will take place. (Why isn't this part of the above?)
	if (Act == AC_OK)
		((RemovePathElementAction*)UnAction)->RecordPath(pDestinNode);

	// copy the necessary data into the destination path. return fail if unable to do so
	if (!pDestPath->MergeSectionFrom(after+1, *pSourcePath, 1, NumInSource-1))
		return ExeExclusive;

#endif
	return ExeNone;
																	 
}



/********************************************************************************************

>	void OpAddPath::DeselectPoint(NodePath* pDestNode, INT32 after) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		pDestinNode = pointer to a node whose path contains a point we will deselect
				position	= offset of verb (line, move, curve) which will be deselected 
	Outputs:	-
	Purpose:

********************************************************************************************/

ActionCode OpAddPath::DeselectPoint(NodePath* pDestNode, INT32 position)
{
#ifndef STANDALONE

	Action* UnAction;				// pointer to action that might be created
	ActionCode Act;					// Action code that might be used

	PathFlags* Flags = pDestNode->InkPath.GetFlagArray();

	Act = ModifyFlagsAction::Init(this, &UndoActions, Flags[position], position, pDestNode, (Action**)(&UnAction));
	if (Act != AC_FAIL)
		Flags[position].IsSelected = FALSE;

	return Act;

#else
	return AC_OK;
#endif
}



/********************************************************************************************

>	void OpAddPath::DeselectHandle(NodePath* pDestNode, INT32 after) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		pDestinNode = pointer to a node whose path contains a point we will deselect
				after 		= offset of verb (line, move, curve) which will be deselected  
	Outputs:	-
	Purpose:	

********************************************************************************************/

ActionCode OpAddPath::DeselectHandle(NodePath* pDestinNode, INT32 after)
{
#ifndef STANDALONE

	ActionCode Act;					// Action code that might be used

	// Remove the selected end bit
	Act = DeselectPoint(pDestinNode, after);
	if (Act == AC_FAIL)
		return Act;

	INT32 lasti = pDestinNode->InkPath.GetNumCoords() -1;

	// Now check and remove any other selection bits around the handle
	PathVerb* Verbs = pDestinNode->InkPath.GetVerbArray();
	PathVerb AfterVerb = Verbs[after] & ~PT_CLOSEFIGURE;

	switch (AfterVerb)
	{
		case PT_MOVETO:
			if (after < lasti)
			{
				PathVerb NextVerb = Verbs[after+1] & ~PT_CLOSEFIGURE;
				if (NextVerb == PT_BEZIERTO)
					Act = DeselectPoint(pDestinNode,after+1);
			}
		break;

		case PT_BEZIERTO:
			if (after>0)
				Act = DeselectPoint(pDestinNode,after-1);
			break;
	}

	return Act;

#else
	return AC_OK;
#endif
}





/********************************************************************************************

>	OpAddNewPath::OpAddNewPath()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Purpose:	OpAddNewPath constructor
	SeeAlso:	-

********************************************************************************************/

OpAddNewPath::OpAddNewPath()
{
}



/********************************************************************************************

>	BOOL OpAddNewPath::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpAddNewPath initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpAddNewPath::Init()
{
	return (RegisterOpDescriptor(0,										// tool ID
 								_R(IDS_ADDNEWPATHOP),						// string resource ID
								CC_RUNTIME_CLASS(OpAddNewPath),			// runtime class for Op
 								OPTOKEN_ADDNEWPATH,						// Ptr to token string
 								OpAddNewPath::GetState,					// GetState function
 								0,										// help ID = 0
 								_R(IDBBL_ADDNEWPATHOP),						// bubble help ID = 0
 								0										// resource ID = 0
 								)); 

}               


/********************************************************************************************

>	OpState	OpAddNewPath::GetState(String_256*, OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpAddNewPath
	Purpose:	For finding the OpAddNewPath's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpAddNewPath::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
	return OpSt;   
}


/********************************************************************************************

>	void OpAddNewPath::GetOpName(String_256* OpName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description
				appropriate to the type of attribute that the operation applies.
					
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpAddNewPath::GetOpName(String_256* OpName)
{
	*OpName = String_256(_R(IDS_UNDO_ADDNEWPATHOP));
}





/********************************************************************************************

>	void OpAddNewPath::DoAddNewPath( Path* pAddPath, Spread* pSpread )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Inputs:		pAddPath	= Pointer to a path to add to the document
				pSpread		= Pointer to the spread to add the path to.
	Outputs:	-
	Purpose:	This operation is called to add a path to the tree. If successfull it will
				create a new node, copy the contents of pAddPath into it and apply the
				current attributes to it.

********************************************************************************************/

void OpAddNewPath::DoAddNewPath(Path* pAddPath, Spread* pSpread)
{
	BeginSlowJob();

	if (!DoStartSelOp(FALSE,TRUE))
	{
		FailAndExecute(); End(); return;
	}
	
	// We had better copy the path back over the original and re-calc the bounding box
	DocView* pDocView = DocView::GetSelected();
	ERROR2IF( pDocView == NULL, (void)0, "There was no selected DocView when editing a path" );

	// Create a path to hold the data
	NodePath* NewPath = new NodePath;
	if (!NewPath)
	{
		FailAndExecute(); End(); return;
	}

	if (!NewPath->SetUpPath(24, 12))
	{
		InformError( _R(IDS_OUT_OF_MEMORY), _R(IDS_OK) );
		delete NewPath;
		FailAndExecute(); End(); return;
	}
	
	// Copy the data from the edit path to the new path
	if (!NewPath->InkPath.CopyPathDataFrom(pAddPath))
	{
		NewPath->CascadeDelete();
		delete NewPath;
		FailAndExecute(); End(); return;
	}
	
	// Apply attributes to the new node
	Document* pDoc = GetWorkingDoc();
	if (pDoc!=NULL)
	{
		// Apply the current attributes to the path
		if (!(pDoc->GetAttributeMgr().ApplyCurrentAttribsToNode((NodeRenderableInk*)NewPath)))
		{
			NewPath->CascadeDelete();
			delete NewPath;
			FailAndExecute(); End(); return;
		}
	}
	else
	{
		NewPath->CascadeDelete();
		delete NewPath;
		FailAndExecute(); End(); return;
	}

	if (!DoInsertNewNode(NewPath, pSpread, TRUE))
	{
		NewPath->CascadeDelete();
		delete NewPath;
		FailAndExecute(); End(); return;
	}

	// Reselect the last endpoint in the new path
	NewPath->InkPath.GetFlagArray()[NewPath->InkPath.GetNumCoords()-1].IsSelected = TRUE;

	// terminate the op
	End();
		
}



/********************************************************************************************

>	OpAddPathToPath::OpAddPathToPath()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	OpAddPathToPath constructor
	SeeAlso:	-

********************************************************************************************/

OpAddPathToPath::OpAddPathToPath()
{
}



/********************************************************************************************

>	BOOL OpAddPathToPath::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpAddPathToPath initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpAddPathToPath::Init()
{
	return (RegisterOpDescriptor(0,										// tool ID
 								_R(IDS_ADDPATHTOPATHOP),					// string resource ID
								CC_RUNTIME_CLASS(OpAddPathToPath),		// runtime class for Op
 								OPTOKEN_ADDPATHTOPATH,					// Ptr to token string
 								OpAddPathToPath::GetState,				// GetState function
 								0,										// help ID = 0
 								_R(IDBBL_ADDPATHTOPATHOP),					// bubble help ID = 0
 								0										// resource ID = 0
 								)); 

}               


/********************************************************************************************

>	OpState	OpAddPathToPath::GetState(String_256*, OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpAddPathToPath
	Purpose:	For finding the OpAddPathToPath's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpAddPathToPath::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
	return OpSt;   
}


/********************************************************************************************

>	void OpAddPathToPath::GetOpName(String_256* OpName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description
				appropriate to the type of attribute that the operation applies.
					
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpAddPathToPath::GetOpName(String_256* OpName)
{
	*OpName = String_256(_R(IDS_UNDO_ADDPATHTOPATHOP));
}





/********************************************************************************************

>	void OpAddPathToPath::DoAddPathToPath( NodePath* pNode, Path* pElement, INT32 index )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		pNode		= Pointer to nodepath in document to add element to
				pElement	= Pointer to path element to add
				index		= index within path at which to add the new elements

	Outputs:	-
	Purpose:	This operation is called to update an existing path. If successful it will
				create a new node, copy the contents of pElement into it and apply the
				current attributes to it.

********************************************************************************************/

void OpAddPathToPath::DoAddPathToPath(NodePath* pNode, Path* pElement, INT32 index)
{
#ifndef STANDALONE

	BeginSlowJob();

	DoStartSelOp(TRUE,TRUE);
	
	DocView* pDocView = DocView::GetSelected();
	ERROR2IF( pDocView == NULL, (void)0, "There was no selected docview when augmenting a path" );

	// Save the bounds of the path for undo/redo
	if (RecalcBoundsAction::DoRecalc(this, &UndoActions, pNode) == AC_FAIL)
	{
		FailAndExecute();
		End();
		return;
	}

	// Go and copy the edited path to the end of the original

	ExecuteType Exe	= AugmentPathWithPath(pElement, pNode, index);
	if (Exe != ExeNone)
	{
		InformError( _R(IDS_OUT_OF_MEMORY), _R(IDS_OK) );
		if (Exe == ExeInclusive)
			FailAndExecute();
		if (Exe == ExeExclusive)
			FailAndExecuteAllButLast();
		End();
		return;
	}	

	// Recalculate the path's bounding box
	pNode->InvalidateBoundingRect();
	
	// tell the world that something in the selection has changed 
	// so that selection bounds are updated
	GetApplication()->FindSelection()->Update(TRUE);

	// record new bounds action undo/redo
	if (RecordBoundsAction::DoRecord(this, &UndoActions, pNode) == AC_FAIL)
	{
		FailAndExecute();
		End();
		return;
	}

#endif
 	End();
}






/********************************************************************************************

>	OpClosePathWithPath::OpClosePathWithPath()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	OpClosePathWithPath constructor
	SeeAlso:	-

********************************************************************************************/

OpClosePathWithPath::OpClosePathWithPath()
{
}



/********************************************************************************************

>	BOOL OpClosePathWithPath::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpClosePathWithPath initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpClosePathWithPath::Init()
{
	return (RegisterOpDescriptor(0,										// tool ID
 								_R(IDS_CLOSEPATHWITHPATHOP),				// string resource ID
								CC_RUNTIME_CLASS(OpClosePathWithPath),	// runtime class for Op
 								OPTOKEN_CLOSEPATHWITHPATH,				// Ptr to token string
 								OpClosePathWithPath::GetState,			// GetState function
 								0,										// help ID = 0
 								_R(IDBBL_CLOSEPATHWITHPATHOP),				// bubble help ID = 0
 								0										// resource ID = 0
 								)); 

}               


/********************************************************************************************

>	OpState	OpClosePathWithPath::GetState(String_256*, OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpClosePathWithPath
	Purpose:	For finding the OpClosePathWithPath's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpClosePathWithPath::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
	return OpSt;   
}


/********************************************************************************************

>	void OpClosePathWithPath::GetOpName(String_256* OpName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description
				appropriate to the type of attribute that the operation applies.
					
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpClosePathWithPath::GetOpName(String_256* OpName)
{
	*OpName = String_256(_R(IDS_UNDO_CLOSEPATHWITHPATHOP));
}





/********************************************************************************************

>	void OpClosePathWithPath::DoClosePathWithPath( NodePath* pNode, Path* pEditPath, INT32 index )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		pNode		= Pointer to nodepath in document to add element to
				pEditPath	= Pointer to path element to add
				index		= offset of subpath end in destin path. The end can either be
							  the moveto of the subpath or the index of the final coordinate
							  in the subpath.
				
	Outputs:	-
	Purpose:	This op takes as parameters a destination path being affected, and a source 
				path which may be the result of a drag edit operation. 
				The idea is that the op will take the components of the source path and add
				them in a deterministic way	to the destination path, resulting in a closed
				destination path.
				The source path is assumed to have either 1 or 2 elements in it. The first
				element will be added to the end of the destination path and act as a
				closing piece. The second element of the source path will affect the
				first element in the destination path. If both elements match in type, ie
				are two curves, the none end control handles of the source will be copied 
				to the destination.
				This op as can be seen is rather specific, and is used by the pen tool to
				close an open path.	Imagine clicking on the first control point of a path
				and draging out control handles. The handles will affect the previous and
				next elements of the path. The previous being a close element and the next
				being the first element of the path. Hence the need for this strange op.
	
********************************************************************************************/

void OpClosePathWithPath::DoClosePathWithPath(NodePath* pDestinNode, Path* pEditPath, INT32 index)
{
#ifndef STANDALONE

	BeginSlowJob();

	DoStartSelOp(TRUE,TRUE);
	
	DocView* pDocView = DocView::GetSelected();
	ERROR2IF( pDocView == NULL, (void)0, "There was no selected doc view when closing a path" );

	// Save the bounds of the path for undo/redo
	if (RecalcBoundsAction::DoRecalc(this, &UndoActions, pDestinNode) == AC_FAIL)
	{
		FailAndExecute();
		End();
		return;
	}

	// go and count the number of elements given to us in the edit path
	INT32 ind = 0;
	INT32 i;
	for (i=0; pEditPath->FindNext(&ind); i++);
	ENSURE(i<3, "There are two many elements in the edit path when closing a path" );

	if (i<1 || i>2)
	{
		FailAndExecute();
		End();
		return;
	}

	// get a pointer to the destination path
	Path* pDestinPath = &(pDestinNode->InkPath);

	// Get the arrays for these paths
	PathFlags* SFlags  = pEditPath->GetFlagArray();
	PathVerb*  SVerbs  = pEditPath->GetVerbArray();
	PathVerb*  DVerbs  = pDestinPath->GetVerbArray();

	BOOL OnMove = (DVerbs[index] == PT_MOVETO);

	INT32 SEnd = 0;
	pEditPath->FindEndOfSubPath(&SEnd);
	
	if (i==2)
	{
		// if we have two elements in the path, go and affect the first element in
		// the destin path with the data held in the second element of the edit path
		// Er, this could be the other way around so watch out!

		INT32 First = index;
		INT32 Second;

		if (OnMove)
		{
			First++;
			Second = First+1;
		}
		else
		{
			First--;
			Second = First-1;
		}

		// If the elements match in type, affect the destination
		if ((SVerbs[SEnd] == DVerbs[First]) && (DVerbs[First] == PT_BEZIERTO))
		{
			
			DocCoord*  SCoords = pEditPath->GetCoordArray();
			DocCoord*  DCoords = pDestinPath->GetCoordArray();
			PathFlags* DFlags  = pDestinPath->GetFlagArray();

			ModifyElementAction* UnAction;
			ActionCode Act;

			// Alter the moveto's flags
			Act = ModifyElementAction::Init(this, 
											&UndoActions,
											DVerbs[index],
											DFlags[index],
											DCoords[index],
											index,
											pDestinNode,
											(Action**)&UnAction);
			if (Act == AC_FAIL)
			{	
				FailAndExecute();
				End();
				return;
			}
			DFlags[index].IsRotate = TRUE;

			// Move the first control point
			Act = ModifyElementAction::Init(this, 
											&UndoActions,
											DVerbs[First],
											DFlags[First],
											DCoords[First],
											First,
											pDestinNode,
											(Action**)&UnAction);
			if (Act == AC_FAIL)
			{	
				FailAndExecute();
				End();
				return;
			}
			DCoords[First] = SCoords[SEnd];
			DFlags[First].IsRotate = TRUE;

			// Move the second control point
			Act = ModifyElementAction::Init(this, 
											&UndoActions,
											DVerbs[Second],
											DFlags[Second],
											DCoords[Second],
											Second,
											pDestinNode,
											(Action**)&UnAction);
			if (Act == AC_FAIL)
			{	
				FailAndExecute();
				End();
				return;
			}
			DCoords[Second] = SCoords[SEnd+1];
			DFlags[Second].IsRotate = TRUE;
		}

		// now bin the last element of the path
		pEditPath->DeleteFromElement(SEnd);
	}

	INT32 DStart = index;
	INT32 DEnd = index;
	pDestinPath->FindStartOfSubPath(&DStart);
	pDestinPath->FindEndElOfSubPath(&DEnd);

	if (!OnMove)
	{
		// if OnMove is false ie someone is closing a curve from the
		// selected moveto backwards, then we need to deselect the moveto
		// element and reverse the path.

		ActionCode Act = DeselectHandle(pDestinNode, DStart);
		if (Act == AC_FAIL)
		{	
			FailAndExecute();
			End();
			return;
		}
		pEditPath->Reverse();
	}

	// set a close figure on the element we're about to add and bin any selection
	SEnd = (pEditPath->GetNumCoords())-1;
	SVerbs[SEnd] = SVerbs[SEnd] | PT_CLOSEFIGURE;
	SFlags[SEnd].IsSelected = FALSE;
	
	// now set the filled bit on the path to make sure it really does
	// get filled once closed.
	ModifyFilledAction* pAction;

	if (ModifyFilledAction::Init(this, &UndoActions, TRUE, FALSE, pDestinNode, (Action**)(&pAction))== AC_FAIL)
	{	
		FailAndExecute();
		End();
		return;
	}

	pDestinNode->InkPath.IsFilled = TRUE;

	// Go and copy the edited path to the end of the original
	ExecuteType Exe	= AugmentPathWithPath(pEditPath, pDestinNode, DEnd);
	if (Exe != ExeNone)
	{
		InformError( _R(IDS_OUT_OF_MEMORY), _R(IDS_OK) );
		if (Exe == ExeInclusive)
			FailAndExecute();
		if (Exe == ExeExclusive)
			FailAndExecuteAllButLast();
		End();
		return;
	}	

	// Recalculate the path's bounding box
	pDestinNode->InvalidateBoundingRect();
	
	// tell the world that something in the selection has changed 
	// so that selection bounds are updated
	GetApplication()->FindSelection()->Update(TRUE);

	// record new bounds action undo/redo
	if (RecordBoundsAction::DoRecord(this, &UndoActions, pDestinNode) == AC_FAIL)
	{
		FailAndExecute();
		End();
		return;
	}

#endif

 	End();
}

