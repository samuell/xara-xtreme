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
// The Ellipes creation and Editing operations


/*
*/


#include "camtypes.h"
#include "opsmpshp.h"

// Code headers
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "noderect.h"
#include "progress.h"
#include "objchge.h"
//#include "rndrgn.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "trans2d.h" - in camtypes.h [AUTOMATICALLY REMOVED]

// Resource headers
//#include "mario.h"
//#include "resource.h"
//#include "rik.h"
#include "bubbleid.h"

CC_IMPLEMENT_DYNCREATE( OpEditRectangle, SelOperation )


#define new CAM_DEBUG_NEW


/********************************************************************************************

>	OpEditRectangle::OpEditRectangle()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/94
	Purpose:	Dummy Contrustor

********************************************************************************************/

OpEditRectangle::OpEditRectangle()
{
	RectPath = NULL;
}




/********************************************************************************************

>	void OpEditRectangle::DoDrag(DocCoord Anchor, Spread* pSpread, NodeEllipse* pElip, INT32 Corner)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/94
	Inputs:		Anchor - The starting position of the drag
				pSpread - The spread that the drag started over
				pElip - The NodeEllipse that is being edited
				Corner - The corner of the Parallelogram that is fixed
	Purpose:	Starts up a drag for editing a NodeEllipse. It records the start postion and
				start spread etc as well as copying the parallelogram of the NodeEllipse
				to change and creating a new NodeEllipse for the purpose of render EORed
				stuff

********************************************************************************************/

void OpEditRectangle::DoDrag(DocCoord Anchor, Spread* pSpread, NodeRect* pRect, INT32 Corner)
{
	// Various starting positions
	StartSpread = pSpread;
	StartPoint = Anchor;
	LastMousePosition = Anchor;

	// The original shape in the tree
	OldRect = pRect;

	// Make a copy of the bounding parallelogram
	Parallel[0] = pRect->Parallel[0];
	Parallel[1] = pRect->Parallel[1];
	Parallel[2] = pRect->Parallel[2];
	Parallel[3] = pRect->Parallel[3];

	// Which corner of the parallelogram is not moving
	FixedCorner = Corner;

	// We will make a rect for the drag blobs
	UseRect = FALSE;
	RectPath = new NodeRect;
	if (RectPath!=NULL)
	{	
		if (RectPath->SetUpPath(12,12))
		{
			// Mark the Ellipse as being valid
			UseRect = TRUE;

			// Give the ellipse a valid path
			RectPath->CreateShape(pRect->GetBoundingRect());
			RectPath->Parallel[Corner] = Parallel[Corner];

			// Build a rect from the parallel ready for the Eor blobs
			RebuildParallelogram(Anchor);

			// and draw the blobs in
			RenderDragBlobs(DocRect(0,0,0,0), StartSpread, FALSE);

			// And tell the Dragging system that we need drags to happen
			StartDrag( DRAGTYPE_AUTOSCROLL );
		}
	}

	// If we do not have a path, then report the error
	if (UseRect == FALSE)
		InformError(_R(IDS_OUT_OF_MEMORY), _R(IDS_OK));
}
	


/********************************************************************************************

>	void OpEditRectangle::DragPointerMove(DocCoord PointerPos, ClickModifiers ClickMods,
										Spread* pSpread, BOOL bSolidDrag)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/94
	Inputs:		PointerPos - The current position of the mouse
				ClickMods - Which buttons and modifiers are held down
				pSpread - The Spread that the mouse is over now.
	Purpose:	Recalculates the ellipse according to the new position of the corner that
				is being dragged, EORs it to the screen and keeps the Parallelogram up
				to date.

********************************************************************************************/

void OpEditRectangle::DragPointerMove(DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL bSolidDrag)
{
	// Snap the actual mouse position to the grid if needed
	DocView::SnapCurrent(pSpread, &PointerPos);

	// Constrain the mouse motion if needed
	if (ClickMods.Constrain)
		DocView::ConstrainToAngle(StartPoint, &PointerPos);

	// If the mouse is in a different position then do something
	if (PointerPos != LastMousePosition)
	{
		// First Rub out the old Drag blobs
		RenderDragBlobs(DocRect(0,0,0,0), StartSpread, bSolidDrag);

		// Make sure that the coords are relative to the coorect spread
		if (pSpread != StartSpread)
			PointerPos = MakeRelativeToSpread(StartSpread, pSpread, PointerPos);

		// Build an ellipse ready for the Eor blobs
		RebuildParallelogram(PointerPos);

		// Update the last mouse position and re-calc the bounding rect
		LastMousePosition = PointerPos;
		RenderDragBlobs(DocRect(0,0,0,0), StartSpread, bSolidDrag);
	}
}




/********************************************************************************************

>	void OpEditRectangle::DragFinished(DocCoord PointerPos, ClickModifiers ClickMods,
									Spread* pSpread, BOOL Success, BOOL bSolidDrag)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/94
	Inputs:		PointerPos - The current position of the mouse
				ClickMods - Which buttons and modifiers are held down
				pSpread - The Spread that the mouse is over now.
				Success - TRUE if the drag was completed sucessfully.
	Purpose:	If the drag was a success then a copy of the original node in the tree is
				created and updated ellipse built. The original NodeEllipse is hidden and
				the new one if inserted into the tree. If any of these things fail then
				the operation will fail.

********************************************************************************************/

void OpEditRectangle::DragFinished(DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL Success, BOOL bSolidDrag)
{
	// First Rub out the old Drag blobs and end the drag
	RenderDragBlobs(DocRect(0,0,0,0), StartSpread, bSolidDrag);
	EndDrag();

	// Flag to say if everything has worked
	BOOL IsOk = FALSE;

	// if the drag was a sucess then start to build undo etc
	if (Success)
	{
		// Will the ellipse allow the op?
		ObjChangeFlags cFlags;
		cFlags.ReplaceNode = TRUE;
		ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,OldRect,this);
		IsOk = OldRect->AllowOp(&ObjChange);	

		// build all the undo and complete the edit
		if (IsOk) IsOk = CompleteOperation();

		if (IsOk)
		{
			// Update all the effected parents
			ObjChange.Define(OBJCHANGE_FINISHED,cFlags,OldRect,this);
			IsOk = UpdateChangedNodes(&ObjChange);
		}
	}

	// If something went wrong, then fail
	if (IsOk==FALSE)
		FailAndExecute();

	// If we have an rect to work with then get rid of it
	if (UseRect)
		delete RectPath;

	// always call end
	End();
}




/********************************************************************************************

>	BOOL OpEditRectangle::CompleteOperation()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/94
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Builds all the undo and generally edits the rectangle.

********************************************************************************************/

BOOL OpEditRectangle::CompleteOperation()
{
	// Start a slow job
	BeginSlowJob();

	// Get a node to copy the original into
	Node* pNode;
	if (!OldRect->NodeCopy(&pNode))
		return FALSE;

	// Get our new node as the correct type
	ENSURE(pNode->IsKindOf(CC_RUNTIME_CLASS(NodeRect)), "Node should have been a NodeRect");
	NodeRect* pRect = (NodeRect*) pNode;

	// Create an rect to fill the bounding rect
	pRect->Parallel[0] = RectPath->Parallel[0];
	pRect->Parallel[1] = RectPath->Parallel[1];
	pRect->Parallel[2] = RectPath->Parallel[2];
	pRect->Parallel[3] = RectPath->Parallel[3];

	pRect->UpdateShape();
	pRect->InkPath.IsFilled = TRUE;

	// Start building the undo
	if (!DoStartSelOp(FALSE))
		return FALSE;

	// Add the new rect to the tree
	if (!DoInsertNewNode(pRect, OldRect, NEXT, TRUE))
		return FALSE;

	// Invalidate the region of the old rect
	if (!DoInvalidateNodeRegion(OldRect, TRUE))
		return FALSE;

	// and remove the old Rect from the tree
	if (!DoHideNode(OldRect, TRUE))
		return FALSE;

	// Transform the Node Children according to bounding box changes
	if (!TransformChildAttrs(pRect))
		return FALSE;

	// everything has worked, so return TRUE
	return TRUE;
}



/********************************************************************************************

>	BOOL OpEditRectangle::TransformChildAttrs(NodeRect* pRect)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/94
	Purpose:	This performs a bodge that Charles asked to be added that scales the attributes
				when the rectangle is edited.

********************************************************************************************/

BOOL OpEditRectangle::TransformChildAttrs(NodeRect* pRect)
{
	DocRect OldBounds = OldRect->GetBoundingRect(TRUE);
	DocRect NewBounds = pRect->GetBoundingRect(TRUE);

	// Get the Width and Height of the two rectangles
	INT32 OldWidth  = OldBounds.Width();
	INT32 OldHeight = OldBounds.Height();
	INT32 NewWidth  = NewBounds.Width();
	INT32 NewHeight = NewBounds.Height();

	// Find the Centre of each rectangle
	OldBounds.Translate(OldWidth/2, OldHeight/2);
	NewBounds.Translate(NewWidth/2, NewHeight/2);
	DocCoord OldCentre = OldBounds.lo;
	DocCoord NewCentre = NewBounds.lo;

	// Calculate the difference in size between the two rectangles
	FIXED16 xscale = FIXED16(double(NewWidth)/double(OldWidth));
	FIXED16 yscale = FIXED16(double(NewHeight)/double(OldHeight));

	// And now make a transform for the attribute
	// First move the old attribute position to the origin
	Matrix AttrTrans = Matrix(-OldCentre.x, -OldCentre.y);

	// Now scale it
	AttrTrans *= Matrix(xscale, yscale);

	// And finally move it to the new position
	AttrTrans *= Matrix(NewCentre.x, NewCentre.y);

	// Now scan for all Attribute Children and Transform them
	Node* pNode = pRect->FindFirstChild();
	while (pNode != NULL)
	{
		if (pNode->IsKindOf(CC_RUNTIME_CLASS(AttrFillGeometry)))
		{
			// get the transform to use
			Trans2DMatrix* Trans = new Trans2DMatrix(AttrTrans);

			// get the node and transform it
			//NodeAttribute* NodeToTransform = (NodeAttribute*)pNode;
//			RangeControl rc = {TRUE, TRUE, FALSE};
			Range temp(pNode, pNode, RangeControl(TRUE,TRUE,FALSE));
			if (!DoTransformNodes(temp, Trans))
			{
				delete Trans;
				return FALSE;
			}
		}

		// go get the next one
		pNode = pNode->FindNext();
	}

	return TRUE;
}




/********************************************************************************************

>	void OpEditRectangle::RebuildParallelogram(DocCoord PointerPos)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/94
	Inputs:		PointerPos - The coords of the Parallelograms corner that is being dragged
	Purpose:	Recalculates the parallelogram based on the corner being dragged and the
				fixed corner. The new parallelogram is copied into the Ellipse we are using
				for EORing and the Ellipse rebuilt to fit the parallelogram

********************************************************************************************/

void OpEditRectangle::RebuildParallelogram(DocCoord PointerPos)
{
	// Lets number the coordinates. 0 is the fixed one and go clockwise from there
	INT32 pos0, pos1, pos2, pos3;
	pos0 = FixedCorner;

	if (pos0>1)
		pos2 = pos0-2;
	else
		pos2 = pos0+2;

	if (pos0==3)
		pos1 = 0;
	else
		pos1 = pos0+1;

	if (pos0==0)
		pos3 = 3;
	else
		pos3 = pos0-1;

	// Calculate the offsets from the old point and the fixed point
	double dx0 = PointerPos.x - Parallel[pos2].x;
	double dy0 = PointerPos.y - Parallel[pos2].y;

	double dx3 = Parallel[pos1].x - Parallel[pos0].x;
	double dy3 = Parallel[pos1].y - Parallel[pos0].y;

	double dx4 = Parallel[pos3].x - Parallel[pos0].x;
	double dy4 = Parallel[pos3].y - Parallel[pos0].y;
	
	// Calculate the bits that we need to know
	double dx1 = dx3 * ((dy0*dx4-dy4*dx0) / (dx4*dy3-dy4*dx3));
	double dy1 = dy3 * ((dy0*dx4-dy4*dx0) / (dx4*dy3-dy4*dx3));
	double dx2 = dx4 * ((dx3*dy0-dy3*dx0) / (dy4*dx3-dx4*dy3));
	double dy2 = dy4 * ((dx3*dy0-dy3*dx0) / (dy4*dx3-dx4*dy3));

	// Change the parallelogram
	RectPath->Parallel[pos1].x = Parallel[pos1].x + (INT32)dx1;
	RectPath->Parallel[pos1].y = Parallel[pos1].y + (INT32)dy1;

	RectPath->Parallel[pos3].x = Parallel[pos3].x + (INT32)dx2;
	RectPath->Parallel[pos3].y = Parallel[pos3].y + (INT32)dy2;
	
	RectPath->Parallel[pos2] = PointerPos;

	// Build an ellipse in this parallelogram
	RectPath->UpdateShape();
}



/********************************************************************************************

>	void OpEditRectangle::RenderDragBlobs(DocRect Rect, Spread* pSpread, BOOL bSolidDrag)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/94
	Inputs:		Rect - the rect that needs redrawing
				pSpread  - the spread that is being rendered
	Purpose:	Renders the Ellipse as it will look if the drag were to end. If we failed
				to create the Ellipse earlier, it will draw a bounding rect

********************************************************************************************/

void OpEditRectangle::RenderDragBlobs(DocRect Rect, Spread* pSpread, BOOL bSolidDrag)
{
	// If being called from DocView::RenderView, then the spread could be wrong
	if (pSpread != StartSpread)
		return;

	// Decide what to area to redraw
	DocRect* pRect = NULL;
	if (!Rect.IsEmpty())
		pRect = &Rect;

	// Start an eor render session
	RenderRegion* pRegion = DocView::RenderOnTop(pRect, pSpread, ClippedEOR);
	while (pRegion)
	{
		// Set the line colour 
		pRegion->SetLineColour(COLOUR_XORNEW);

		// Draw the Rectangle
		RectPath->RenderEorDrag(pRegion);

		// Get the Next render region
		pRegion = DocView::GetNextOnTop(pRect);
	}
}




/********************************************************************************************

>	BOOL OpEditRectangle::Declare()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/93
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpEditRectangle::Init()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_RECTANGLE_TOOL),
								CC_RUNTIME_CLASS(OpEditRectangle), 
								OPTOKEN_EDITRECT,
								OpEditRectangle::GetState,
								0,	/* help ID */
								_R(IDBBL_EDITRECTOP),
								0	/* bitmap ID */));
}


/********************************************************************************************

>	OpState OpEditRectangle::GetState(String_256* Description, OpDescriptor*)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/93
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the push tool
	Returns:	The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpEditRectangle::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}

