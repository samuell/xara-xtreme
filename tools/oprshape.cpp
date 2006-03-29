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
// Implementation of the Regular Shape tool operations

/*
*/

#include "camtypes.h"
#include "oprshape.h"	

#include "app.h"
#include "attrmgr.h"
#include "blobs.h"
#include "csrstack.h"
#include "cursor.h"
#include "docview.h"
#include "lineattr.h"
#include "noderect.h"
#include "nodershp.h"
//#include "peter.h"
#include "progress.h"							   
//#include "resource.h"
#include "rndrgn.h"
#include "shapeops.h"
#include "tool.h"
#include "trans2d.h"
//#include "viewrc.h"
#include "spread.h"

DECLARE_SOURCE( "$Revision: 662 $" );

CC_IMPLEMENT_DYNCREATE(OpNewRegShape, SelOperation)

#define new CAM_DEBUG_NEW     


/********************************************************************************************

>	OpNewRegShape::OpNewRegShape()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Purpose:	Constructor. This simply sets a few of the operation flags.

********************************************************************************************/

OpNewRegShape::OpNewRegShape()
{
	pCursor = NULL;
	IsShiftDown = FALSE;
}



/********************************************************************************************

>	void OpNewRegShape::DoDrag( Spread* pSpread, DocCoord Anchor, INT32 NumSides, CreateMode DragMode
											BOOL Circular, BOOL Stellated, BOOL Curved)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		pSpread - The spread that the drag was started on
				Anchor - starting position of the drag
				NumSides - the number of sides the new regular shape should have (0 implies an ellipse)
				CreateMode - the way the drag and the new object should interact.  Either RADIUS,
				DIAMETER or BOUNDS
				Circular - TRUE to base the created shape on a circle, FALSE if it is a polygon
				Curved - TRUE if the created shape has rounded corners.
	Purpose:	Starts dragging from the coordinate passed in.
				For RADIUS mode you are dragging the major axes from a fixed centre point
				For DIAMETER mode you are dragging the major axes, the centre point is the 
				midpoint of the line between the start and current points.
				For BOUNDS mode you are dragging a bounding box.  The major and minor axes
				just touch the edge of the box.

********************************************************************************************/
void OpNewRegShape::DoDrag( Spread* pSpread, DocCoord Anchor, INT32 NumSides, CreateMode DragMode, 
												BOOL Circular, BOOL Stellated, BOOL Curved)
{
	DocView::SnapCurrent(pSpread, &Anchor);

	// We had better take a note of the starting point of the drag.
	StartSpread = pSpread;
	StartPoint = Anchor;
	LastPoint = Anchor;
	CreationMode = DragMode;
	
	// Create the shape that is to be dragged around.
	NewShape = new (NodeRegularShape);

	if ((NewShape == NULL) || !NewShape->SetUpShape() )
	{
		InformError(_R(IDS_OUT_OF_MEMORY), _R(IDS_OK));
		return;
	}

	// And make sure it is filled in
	NewShape->SetNumSides(NumSides);

	const DocCoord Origin(0,0);
	NewShape->SetCentrePoint(Origin);
	NewShape->SetMajorAxes(Origin);
	NewShape->SetMinorAxes(Origin);

	NewShape->SetCircular(Circular);
	NewShape->SetStellated(Stellated);
	NewShape->SetPrimaryCurvature(Curved);
	NewShape->SetStellationCurvature(Curved);

	Matrix InitialMat(StartPoint.x, StartPoint.y);
	NewShape->SetTransformMatrix(&InitialMat);

	// Eor the rectangle on for the first time
	DocRect EditPathBBox = NewShape->GetBoundingRect();
	RenderDragBlobs(EditPathBBox, StartSpread, FALSE);

	// And tell the Dragging system that we need drags to happen
	StartDrag( DRAGTYPE_AUTOSCROLL, NULL, &Anchor, FALSE );
}



/********************************************************************************************

>	void OpNewRegShape::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
								   Spread* pSpread, BOOL bSolidDrag)
	
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods - Which key modifiers are being pressed
				pSpread - The spread that the mouse pointer is over
	Purpose:	Takes the pointer position of the corner of the shape being dragged
				and redisplays the outline
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpNewRegShape::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
								   Spread* pSpread, BOOL bSolidDrag)
{
	// Rub out the old shape
	DocRect EditPathBBox = NewShape->GetBoundingRect();
	RenderDragBlobs(DocRect(0,0,0,0), StartSpread, bSolidDrag);

	// make sure the rect does not wrap around the edge of the spread
	if (pSpread != StartSpread)
		PointerPos = MakeRelativeToSpread(StartSpread, pSpread, PointerPos);

	// If the SHIFT key is down then drop into RADIUS mode
	CreateMode TempCreationMode = CreationMode;
	if (ClickMods.Adjust && CreationMode == DIAMETER)
		TempCreationMode = RADIUS;
	if (ClickMods.Adjust && ClickMods.Constrain && CreationMode == BOUNDS)
		TempCreationMode = RADIUS;

	// Now see if we should constrain the point
	if (ClickMods.Constrain)
	{
		// If we are in bounds creation mode then we want to restrict to 45, 135 225, and 315 degrees
		// This stops thin shapes (ie zero wdith or height)
		if (TempCreationMode == BOUNDS)
		{
			double length = StartPoint.Distance(PointerPos);
		
			if (length != 0.0)
			{
				DocCoord Offset = PointerPos - StartPoint; 
				double rotangle = atan2((double)Offset.y, (double)Offset.x);

				if ((rotangle >= 0) && (rotangle <= PI/2))
					rotangle = PI/4;
				if (rotangle > PI/2)
					rotangle = 3*(PI/4);
				if ((rotangle < 0) && (rotangle >= -PI/2))
					rotangle = -PI/4;
				if (rotangle < -PI/2)
					rotangle = -3*(PI/4);

				PointerPos.x = StartPoint.x + (INT32)length; 
				PointerPos.y = StartPoint.y; 

				Trans2DMatrix Trans(StartPoint, rotangle*(180/PI));
				Trans.Transform(&PointerPos, 1);
			}
		}
		else
		{
			DocCoord Centre = NewShape->GetCentrePoint();
			if (TempCreationMode == DIAMETER)
				Centre = StartPoint;
			DocView::ConstrainToAngle(Centre, &PointerPos);
		}
	}

	// snap to the grid too
	DocView::SnapCurrent(pSpread, &PointerPos);

	// Change our shape to fit the current mouse position
	switch (TempCreationMode)
	{
		case RADIUS:
		{	// The major axes point is the current mouse pos rotated around the	centre point by  
			// 180 degrees.  The minor point is the same point rotated by a further 90 degrees
			DocCoord OldCentre = NewShape->GetCentrePoint();
			DocCoord NewMajor = PointerPos - OldCentre;
			DocCoord NewMinor = PointerPos - OldCentre;
			Trans2DMatrix Trans1(DocCoord(0,0), -180);
			Trans1.Transform(&NewMajor, 1);
			Trans2DMatrix Trans2(DocCoord(0,0), -270);
			Trans2.Transform(&NewMinor, 1);
			NewShape->SetMajorAxes(NewMajor);
			NewShape->SetMinorAxes(NewMinor);
			NewShape->SetCentrePoint(DocCoord(0,0));
			Matrix InitialMat(OldCentre.x, OldCentre.y);
			NewShape->SetTransformMatrix(&InitialMat);
			break;
		}
		case DIAMETER:
		{	// The centre point is the midpoint of the line from the start point to the current point.
			// The major axes is positioned so that a vertex is on the starting position.
			// The minor axes is the major axes rotated by -90 degrees.
			DocCoord NewCentre;
			NewCentre.x = (StartPoint.x + PointerPos.x) / 2;
			NewCentre.y = (StartPoint.y + PointerPos.y) / 2;
			NewShape->SetCentrePoint(DocCoord(0,0));
			DocCoord NewMajor = PointerPos - NewCentre;
			Trans2DMatrix Trans1(DocCoord(0,0), 180-(180.0/NewShape->GetNumSides()));
			Trans1.Transform(&NewMajor, 1);
			NewShape->SetMajorAxes(NewMajor);
			DocCoord NewMinor = NewMajor;
			Trans2DMatrix Trans2(DocCoord(0,0), -90);
			Trans2.Transform(&NewMinor, 1);
			NewShape->SetMinorAxes(NewMinor);
			Matrix InitialMat(NewCentre.x, NewCentre.y);
			NewShape->SetTransformMatrix(&InitialMat);
			break;
		}
		case BOUNDS:
		{	
			DocCoord NewCentre;
			DocCoord NewMajor;
			DocCoord NewMinor;
			// If SHIFT is down then we need to reposition the shape in a similar way to the old
			// rectangle tool
			if (ClickMods.Adjust)
			{
				if (!IsShiftDown)
				{
					IsShiftDown = TRUE;
					RecentrePoint.x = (StartPoint.x + PointerPos.x) / 2;
					RecentrePoint.y = (StartPoint.y + PointerPos.y) / 2;
				}
				DocRect	BoundsRect(RecentrePoint, RecentrePoint);
				BoundsRect.IncludePoint(PointerPos);
				BoundsRect.IncludePoint(RecentrePoint + (RecentrePoint - PointerPos));
				InflateShape(&BoundsRect, &NewCentre, &NewMajor, &NewMinor);
			}
			else
			{
				if (IsShiftDown)
				{
					IsShiftDown = FALSE;
					StartPoint = RecentrePoint + (RecentrePoint - PointerPos);
				}

				DocRect	BoundsRect(StartPoint, StartPoint);
				BoundsRect.IncludePoint(PointerPos);
				InflateShape(&BoundsRect, &NewCentre, &NewMajor, &NewMinor);
			}

			NewShape->SetCentrePoint(DocCoord(0,0));    
			NewShape->SetMajorAxes(NewMajor - NewCentre);    
			NewShape->SetMinorAxes(NewMinor - NewCentre);    
			Matrix InitialMat(NewCentre.x, NewCentre.y);
			NewShape->SetTransformMatrix(&InitialMat);
			break;
		}
		default:
			ERROR3("Unknown creation type");
	}

	// Put the Eor back on again for the new shape
	EditPathBBox = NewShape->GetBoundingRect();
	RenderDragBlobs(EditPathBBox, StartSpread, bSolidDrag);

	BROADCAST_TO_ALL(ShapeEditedMsg(NewShape, pSpread));

	LastPoint = PointerPos;
}



/********************************************************************************************

>	void OpNewRegShape::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
								Spread* pSpread, BOOL Success, BOOL bSolidDrag)

    
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickMods - the key modifiers being pressed
				pSpread - The spread that the drag finished on
				Success - TRUE if the drag was terminated properly, FALSE if it
				was ended with the escape key being pressed
	Purpose:	Ends the drag and creates a rectangle using the dimensions and position
				of the outline rectangle produced by the dragging system. It also ends the
				operation.
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpNewRegShape::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
								Spread* pSpread, BOOL Success, BOOL bSolidDrag)
{
	// Start a slow job
	BeginSlowJob();

	// End the Drag                             
	EndDrag();

	// First Rub out the old box
	DocRect EditPathBBox = NewShape->GetBoundingRect();
	RenderDragBlobs(EditPathBBox, StartSpread, bSolidDrag);
		
	// If we should carry on, then do it
	BOOL Worked = FALSE;
	if ( Success && (StartPoint != PointerPos) ) 
		Worked = CompleteOperation();

	// if it did not work or the user pressed ESCAPE then fail
	if (!Worked)
	{
		// It did not work, so we had better recover
		NewShape->CascadeDelete();
		delete NewShape;

		FailAndExecute();
	}

	End();
}




/********************************************************************************************

>	BOOL OpNewRegShape::CompleteOperation()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Returns:	TRUE if the operation worked, FALSE if not
	Purpose:	Does all the work in building the undo etc. It builds a shape and inserts it
				into the tree.

********************************************************************************************/

BOOL OpNewRegShape::CompleteOperation()
{
	// We will only create the shape if it is bigger than a blob rect at the current scale
	DocRect temp;
	GetApplication()->GetBlobManager()->GetBlobRect(StartPoint, &temp);
	if (temp.ContainsCoord(LastPoint))
	{
		return FALSE;
	}

	// Find the document for this operation.
	Document* pDoc = GetWorkingDoc();
	if (pDoc==NULL)
	{
		TRACEALL( _T("OpNewRegShape::CompleteOperation has no working document.")); 
		return FALSE;
	}

	// Apply a few attributes
	if (!(pDoc->GetAttributeMgr().ApplyCurrentAttribsToNode((NodeRenderableInk*)NewShape)))
		return FALSE;

	// Delete any applied join attribute
	NodeAttribute* pAppliedJoin = NewShape->GetChildAttrOfType(CC_RUNTIME_CLASS(AttrJoinType));
	if (pAppliedJoin != NULL) 
	{
		pAppliedJoin->CascadeDelete();
		delete pAppliedJoin;
	}

	// Apparently all shapes should have mitre joins.
	AttrJoinType* pMitreJoin = new AttrJoinType(NewShape, LASTCHILD);
	if (pMitreJoin == NULL)
		return FALSE;
	pMitreJoin->Value.JoinType = MitreJoin;

	// Try and start the the operation
	if (!DoStartSelOp(FALSE))
		return FALSE;

	// Ok, add it in and invalidate the region
	if (!DoInsertNewNode(NewShape, StartSpread, TRUE))
		return FALSE;

	// Finally, make sure we haven't added the new object partially off the spread
	StartSpread->ExpandPasteboardToInclude(NewShape->GetBoundingRect());

	// all worked
	return TRUE;
}



/********************************************************************************************

>	void OpNewRegShape::InflateShape(DocRect *Bounds, DocCoord* NewCentre, DocCoord* NewMajor, DocCoord* NewMinor)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/95
	Inputs:		Bounds - The bounding box of the initial shape
				NewCenter - the centre point of the shape
				NewMajor - the proposed new major point
				NewMinor - the proposed new major point
	Outputs:	NewMajor and NewMinor may have been repositioned
	Returns:	-
	Purpose:	NewMajor and NewMinor are initially positioned half way along the top and
				halfway down the righthand side of the Bounds rect respectivly,  Their
				positions are then increased so that the shape path passes through their
				previous positions.

********************************************************************************************/
void OpNewRegShape::InflateShape(DocRect *Bounds, DocCoord* NewCentre, DocCoord* NewMajor, DocCoord* NewMinor)
{
	// Set their initial positions
	NewCentre->x = (Bounds->lo.x + Bounds->hi.x) / 2;
	NewCentre->y = (Bounds->lo.y + Bounds->hi.y) / 2;
	NewMajor->x = (Bounds->lo.x + Bounds->hi.x) / 2;
	NewMajor->y = Bounds->hi.y;
	NewMinor->x = Bounds->hi.x;
	NewMinor->y = (Bounds->lo.y + Bounds->hi.y) / 2;
	
	// Move them if required
	if (!NewShape->IsCircular())
	{
		DocCoord MajorRender = *NewMajor;
		DocCoord MinorRender = *NewMinor;
		double radius = NewCentre->Distance(*NewMajor);
		double SideA = cos(PI/NewShape->GetNumSides())*radius;
		if (SideA != 0.0)
		{
			DocCoord Update = DocCoord::PositionPointFromRatio(*NewCentre, *NewMajor, radius/SideA);
			NewMajor->y = Update.y;
		}

		if ( ((NewShape->GetNumSides() % 2) == 0) && (((NewShape->GetNumSides()/2) % 2) == 0) )
		{
			radius = NewCentre->Distance(*NewMinor);
			SideA = cos(PI/NewShape->GetNumSides())*radius;
			if (SideA != 0.0)
			{
				DocCoord Update = DocCoord::PositionPointFromRatio(*NewCentre, *NewMinor, radius/SideA);
				NewMinor->x = Update.x;
			}
		}
	}
}



/********************************************************************************************

>	virtual void OpNewRegShape::RenderDragBlobs( DocRect Rect, Spread* pSpread, BOOL bSolidDrag )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		Rect - The region that needs the blobs to be drawn
				pSpread - The spread that the drawing will happen on
	Purpose:	Draws an EORed version of the current shape

********************************************************************************************/

void OpNewRegShape::RenderDragBlobs(DocRect Rect, Spread* pSpread, BOOL bSolidDrag)
{
	// If being called from DocView::RenderView, then the spread could be wrong - so
	// convert the rectangle if necessary.
	if (pSpread != StartSpread)
	{
		Rect.lo = MakeRelativeToSpread(StartSpread, pSpread, Rect.lo);
		Rect.hi = MakeRelativeToSpread(StartSpread, pSpread, Rect.hi);
	}

	// start a rendering loop
	RenderRegion* pRegion = DocView::RenderOnTop(NULL, pSpread, ClippedEOR);
	while (pRegion)
	{
		// Set the line colour 
		pRegion -> SetFillColour(COLOUR_NONE);
		pRegion -> SetLineColour(COLOUR_XORNEW);

		// Draw the outline
		NewShape->RenderEorDrag(pRegion);

		// Get the Next render region
		pRegion = DocView::GetNextOnTop(NULL);
	}
}



/********************************************************************************************

>	BOOL OpNewRegShape::Declare()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94																		
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/
BOOL OpNewRegShape::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_NEWREGULARSHAPEOP),
								CC_RUNTIME_CLASS(OpNewRegShape), 
								OPTOKEN_NEWREGSHAPE,
								OpNewRegShape::GetState));

}



/********************************************************************************************

>	OpState OpNewRegShape::GetState(String_256* Description, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Outputs:	-
	Returns:	Ungreyed, Unticked
	Purpose:	Find out the state of the new regular shape at the specific time

********************************************************************************************/
OpState OpNewRegShape::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}

