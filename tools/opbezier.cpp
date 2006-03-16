// $Id: opbezier.cpp 662 2006-03-14 21:31:49Z alex $
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
// Implementation of the Line tool operations

/*
*/

#include "camtypes.h"
#include "opbezier.h"	

#include "app.h"
#include "beztool.h"
#include "blobs.h"
#include "docview.h"
//#include "jim.h"
#include "nodepath.h"
#include "progress.h"
//#include "rik.h"
#include "bubbleid.h"

DECLARE_SOURCE( "$Revision: 662 $" );

CC_IMPLEMENT_DYNCREATE(OpSelectPathPoints, UndoableOperation)
CC_IMPLEMENT_DYNCREATE(OpInsertFloater, UndoableOperation)
CC_IMPLEMENT_DYNCREATE(OpRemoveFloater, UndoableOperation)
CC_IMPLEMENT_DYNCREATE(RemoveFloaterAction, Action)
CC_IMPLEMENT_DYNCREATE(InsertFloaterAction, Action)
CC_IMPLEMENT_MEMDUMP(InsertFloaterParam, OpParam)


// Must come after the last CC_IMPLEMENT.. macro
#define new CAM_DEBUG_NEW     


///////////////////////////////////////////////////////////////////////////////////////////
//  The OpSelectPathPoints Operation

BOOL OpSelectPathPoints::DragUnderway = FALSE;

/********************************************************************************************

>	OpSelectPathPoints::OpSelectPathPoints()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/6/94
	Purpose:	Constructor. This simply sets a few of the operation flags.

********************************************************************************************/

OpSelectPathPoints::OpSelectPathPoints()
{
}

/********************************************************************************************

>	BOOL OpSelectPathPoints::Init()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/94
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpSelectPathPoints::Init()
{
	return (RegisterOpDescriptor(	0, 
									_R(IDS_NODEPATH_EDIT),
									CC_RUNTIME_CLASS(OpSelectPathPoints), 
									OPTOKEN_NODEPATH,
									OpSelectPathPoints::GetState,
									0,	/* help ID */
									_R(IDBBL_NODEPATHOP),
									0	/* bitmap ID */));	   
}



/********************************************************************************************

>	static BOOL OpSelectPathPoints::DragInProgress()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/94
	Inputs:		-
	Returns:	TRUE if a drag is underway, otherwise FALSE.
	Purpose:	To find out if there is a select points operation in progress.

********************************************************************************************/

BOOL OpSelectPathPoints::DragInProgress()
{
	return DragUnderway;
}



/********************************************************************************************

>	void OpSelectPathPoints::DoDrag( DocCoord Anchor, Spread *pSpread, BOOL isAdjust, 
									 BezierTool* tptr,
									 ClickModifiers ClickMods)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>	- Modified by Peter
	Created:	29/6/94
	Inputs:		Anchor 	- The position of the mouse at the start of the Drag
				pSpread - The spread that the drag started on
				isAdjust - lets the operation remember if the drag is adjust or not
				tptr points at the calling tool, so that we can call it back if this
				drag happened to really insert a floating endpoint
				ClickMods are the modifiers so that we know if adjust was down
	Purpose:	Starts dragging a rectangle, and when the drag finishes, any points inside
				the rectangle will be selected. If isAdjust, points inside the rectangle will
				be toggled.

				If the drag finishes where it started, we assume that actually the user wanted
				to insert a floating endpoint at the click position, so we have to call back to
				the bezier tool to tell it where to put the endpoint.

				On the drag start all control point blobs are removed, also if the drag is
				a select drag (as opposed to an adjust drag) the selected endpoints blobs
				are replaced with unselected blobs.

********************************************************************************************/

void OpSelectPathPoints::DoDrag(DocCoord Anchor, 
								Spread *pSpread, 
								BOOL isAdjust, 
								BezierTool* tptr,
								ClickModifiers ClickMods )
{
	// Make a mental note of the start point
	StartPoint		= Anchor;
	StartSpread		= pSpread;
	PreviousPoint	= Anchor;
	AdjustDrag		= isAdjust;
	BezToolPtr		= tptr;
	Mods 			= ClickMods;

	// Go through all the selected NodePaths and remove all the 
	// control point blobs
	SelRange*	Selection = GetApplication()->FindSelection();
	Node*		pNode = Selection->FindFirst();
	NodePath*   pSelected = NULL;

	while (pNode != NULL)
	{
 		if (pNode->IsNodePath())
 			pSelected = (NodePath*)pNode;
		else
			pSelected =	(NodePath*) (pNode->HasEditableChild(CC_RUNTIME_CLASS(NodePath), NULL));

		if (pSelected)
		{
			Path* ThisPath = &(pSelected->InkPath);
			Spread* pSpread = pNode->FindParentSpread();

			// Dereference the pointers
			DocCoord* Coords = ThisPath->GetCoordArray();
			PathFlags* Flags = ThisPath->GetFlagArray();
			PathVerb* Verbs = ThisPath->GetVerbArray();
			INT32 UsedSlots = ThisPath->GetNumCoords();

			DocCoord StartCoord = Coords[0];

			// Go into a RenderOnTop loop
			RenderRegion* pRender = DocView::RenderOnTop(NULL, pSpread, ClippedEOR);
			while (pRender!=NULL)
			{
				// loop through the path and count the selected blobs
				INT32 endcount = 0;		// Count the number of selected endpoints
				INT32 EndPtIndex = 0;	// Index of only selected endpoint
				for (INT32 i=0; i<UsedSlots; i++)
				{
					if (Flags[i].IsEndPoint && Flags[i].IsSelected)
					{
						if (!(Verbs[i] & PT_CLOSEFIGURE))
						{
							EndPtIndex = i;
							endcount++;
							if (!AdjustDrag)
							{
								// If in select mode then remove the selection blob
								ThisPath->DrawBlob(pRender, Coords[i], TRUE);
								ThisPath->DrawBlob(pRender, Coords[i], FALSE);
							}
						}
					}
				}
				if (endcount == 1)
				{
					// Remove the control points and lines
					ThisPath->RenderPathControlBlobs(pSpread,EndPtIndex);
				}
				pRender = DocView::GetNextOnTop(NULL);
			}	// while the render region
		}	// if its a node path
		pNode = Selection->FindNext(pNode);
	}	// while there are selected nodes
	
	// And tell the Dragging system that we need drags to happen
	StartDrag( DRAGTYPE_AUTOSCROLL );

	DragUnderway = TRUE;
}




/********************************************************************************************

>	void OpSelectPathPoints::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
								  Spread *pSpread, BOOL bSolidDrag)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/6/94
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods - Which key modifiers are being pressed
				pSpread - The spread that the mouse is moving over
	Purpose:	Handles the event of the mouse moving during a drag
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpSelectPathPoints::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
										  Spread *pSpread, BOOL bSolidDrag)
{
	// If drag has moved onto a different spread, convert the coord to be relative to the
	// original spread.
	if (pSpread != StartSpread)
		PointerPos = MakeRelativeToSpread(StartSpread, pSpread, PointerPos);

	// Remove old rubber rectangle
	DocRect OldOutline(StartPoint, StartPoint);
	OldOutline.IncludePoint(PreviousPoint);
	RenderRubberRect(&OldOutline);

	// Change the variable that remembers the other corner of the rectangle
	PreviousPoint = PointerPos;

	// And render the rubber rect in its new position
	DocRect NewOutline(StartPoint, StartPoint);
	NewOutline.IncludePoint(PreviousPoint);
	RenderRubberRect(&NewOutline);

	// Now we need to render selection status of newly entered/exited endpoints
	// Go through all the endpoints on selected NodePaths.  
	SelRange*	Selection = GetApplication()->FindSelection();
	Node*		pNode = Selection->FindFirst();
	NodePath*	pSelected = NULL;

	while (pNode != NULL)
	{
 		if (pNode->IsNodePath())
 			pSelected = (NodePath*)pNode;
		else
			pSelected =	(NodePath*) (pNode->HasEditableChild(CC_RUNTIME_CLASS(NodePath), NULL));

		if (pSelected)
		{
			Path* ThisPath = &(pSelected->InkPath);
			Spread* pSpread = pNode->FindParentSpread();

			// Dereference the pointers
			DocCoord*	Coords = ThisPath->GetCoordArray();
			PathFlags*	Flags = ThisPath->GetFlagArray();
			PathVerb*	Verbs = ThisPath->GetVerbArray();
			INT32 UsedSlots = ThisPath->GetNumCoords();

			// Go into a RenderOnTop loop
			RenderRegion* pRender = DocView::RenderOnTop(NULL, pSpread, ClippedEOR);
			while (pRender!=NULL)
			{
				// loop through the path, processing every endpoint
				for (INT32 i=0; i<UsedSlots; i++)
				{
					if (Flags[i].IsEndPoint && !(Verbs[i] & PT_CLOSEFIGURE) )
					{
						BOOL First = TRUE;		// Default selection status/es/i/whatever
						BOOL Second = FALSE;	// This means we can skip doing thigs
						BOOL Render = FALSE;
						if (!(AdjustDrag && Flags[i].IsSelected))
						{
							if (NewOutline.ContainsCoord(Coords[i]) && 
														!OldOutline.ContainsCoord(Coords[i]) )
							{
								// Point has just entered the selection rect
								First = FALSE;
								Second = TRUE;
								Render = TRUE;
							}
							if (!NewOutline.ContainsCoord(Coords[i]) 
											&& OldOutline.ContainsCoord(Coords[i]) )
							{
								// Point has just left the selection rect
								Render = TRUE;
							}
						}
						if (Render)
						{
							ThisPath->DrawBlob(pRender, Coords[i], First);
							ThisPath->DrawBlob(pRender, Coords[i], Second);
						}
					}
				}
				pRender = DocView::GetNextOnTop(NULL);
			}	// while the render region
		}	// if its a node path
		pNode = Selection->FindNext(pNode);
	}	// while there are selected nodes
}



/********************************************************************************************

>	void OpSelectPathPoints::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods,
							   Spread *pSpread, BOOL Success, BOOL bSolidDrag)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/6/94
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickMods - the key modifiers being pressed
				pSpread - The spread that the drag finished on
				Success - TRUE if the drag was terminated properly, FALSE if it
				was ended with the escape key being pressed
	Purpose:	Handles the drag finishing by rubbing out an EOR on the screen and
				selecting any points which fall within the rectangle dragged out
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpSelectPathPoints::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods,
							   Spread *pSpread, BOOL Success, BOOL bSolidDrag)
{

	DocRect RubberRect(StartPoint, StartPoint);
	RubberRect.IncludePoint(PreviousPoint);
	RenderRubberRect(&RubberRect);

	DragUnderway = FALSE;

	BeginSlowJob();
	// End the Drag
	if ( !EndDrag() )
	{
		FailAndExecute(); 
		End(); 
	}

	// select any points within the rectangle if the drag completed properly
	// If the rectangle is tiny, we're not selecting points, we're inserting a new
	// path at the click position

	if ( Success )
	{
		DocRect temp;
		GetApplication()->GetBlobManager()->GetBlobRect(StartPoint,&temp);
		if (!temp.ContainsCoord(PreviousPoint))
		{
			// Must be selecting points
			// Get all selected points, and remove any selected EORing, replacing it with
			// normal EORing

			SelRange* Selected = GetApplication()->FindSelection();
			Node* pNode = Selected->FindFirst();
			NodePath* pSelected = NULL;

			while (pNode)
			{
		 		if (pNode->IsNodePath())
 					pSelected = (NodePath*)pNode;
				else
					pSelected =	(NodePath*) (pNode->HasEditableChild(CC_RUNTIME_CLASS(NodePath), NULL));

				if ((pSelected) && (pSelected->FindParentSpread() == StartSpread))
				{
					// Go through the points in this path. 
					// Select any unselected points that fall in the rectangle. 
					// If Adjust, deselected any selected points that fall in the rectangle.
					// If not Adjust, deselect any selected points that fall outside.

					DocCoord testcoord;
					Path*	ThisPath = &(pSelected->InkPath);
					DocCoord*	Coords = ThisPath->GetCoordArray();
					PathFlags*	Flags = ThisPath->GetFlagArray();
					PathVerb*	Verbs = ThisPath->GetVerbArray();
					INT32	UsedSlots = ThisPath->GetNumCoords();
					INT32	SelectedEndPoints = 0;
					INT32	EndPtIndex = 0;

					for (INT32 i=0; i<UsedSlots; i++)
					{
						if (Flags[i].IsEndPoint)
						{
							// Selected points change depending on Adjust and whether they are
							// within the rectangle.
							if (Flags[i].IsSelected)
							{
								if (!AdjustDrag && !RubberRect.ContainsCoord(Coords[i]))
								{
									Flags[i].IsSelected = FALSE;
								}
								else
								{
									if (!(Verbs[i] & PT_CLOSEFIGURE))
									{
										SelectedEndPoints++;
										EndPtIndex = i;
									}
								}
							}
							else
							{
								// unselected points inside rect are selected
								if (RubberRect.ContainsCoord(Coords[i]))
								{
									Flags[i].IsSelected = TRUE;
									if (!(Verbs[i] & PT_CLOSEFIGURE))
									{
										SelectedEndPoints++;
										EndPtIndex = i;
									}
								}
							}
						}
					}
					ThisPath->EnsureSelection(TRUE);
					// Now render on the control points if there was just one selected endpoint
					if (SelectedEndPoints == 1)
					{
						ThisPath->RenderPathControlBlobs(pSpread,EndPtIndex);
					}
				}
				pNode = Selected->FindNext(pNode);
			} // while

		}
		else
		{
			// Must have been a single click. If Adjust was down, clear all selected points
			// if not, still clear all selected points, but tell the bezier tool that
			// the user wants to start a new path at this point
			// Get all selected points, and remove any selected EORing, replacing it with
			// normal EORing
			RemoveDragBlobs(FALSE, TRUE);

			// Now we've cleared the selection, check Adjust. If adjust is not set
			// we have to put in a floating endpoint
			if (!Mods.Adjust)
			{
				DocView::SnapCurrent(pSpread, &StartPoint);
				BezToolPtr->SetMoveTo(StartPoint, pSpread, GetWorkingDoc());

				if (!RemoveFloaterAction::DoRemove(this, &UndoActions, BezToolPtr))
				{
					FailAndExecute();
					End();
					return;
				}
			}
		}
	}
	else
	{
		// Escape was pressed, we just have to restore the blob state to the previous
		RemoveDragBlobs(TRUE, FALSE);
	}
	GetApplication()->UpdateSelection();
	End();
}



/********************************************************************************************

>	virtual void OpSelectPathPoints::RenderDragBlobs( DocRect Rect, Spread* pSpread, BOOL bSolidDrag )

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/6/94
	Inputs:		Rect - The Rectangle that needs to be re-rendered
				pSpread - The spread on which the rendering is to take place
	Purpose:	EORs the rectangle onto the screen
	SeeAlso:	OpBezier::RenderLineSegment()

********************************************************************************************/

void OpSelectPathPoints::RenderDragBlobs( DocRect Rect, Spread* pSpread, BOOL bSolidDrag)
{
	// If being called from DocView::RenderView, then the spread could be wrong - so
	// convert the rectangle if necessary.
	if (pSpread != StartSpread)
	{
		Rect.lo = MakeRelativeToSpread(StartSpread, pSpread, Rect.lo);
		Rect.hi = MakeRelativeToSpread(StartSpread, pSpread, Rect.hi);
	}

	RenderRubberRect(&Rect);
	RenderRegion* pRegion = DocView::RenderOnTop(&Rect, StartSpread, UnclippedEOR);

	while (pRegion)
	{
		if (DragUnderway)
			RenderRubberSelection(pRegion);

		// Get the Next render region
		pRegion = DocView::GetNextOnTop(&Rect);
	}
}



/********************************************************************************************

>	void OpSelectPathPoints::RenderRubberRect(DocRect* Rect)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/94
	Inputs:		Rect - pointer to the bounding box of the render region.  NOT the rubber
				rectangle.
	Returns:	-
	Purpose:	EORs the selection rectangle onto the screen.  The rectangle is rendered
				using the coordinates StartPoint to PreviousPoint.
	SeeAlso:	OpSelectPathPoints::RenderRubberSelection(), OpSelectPathPoints::RenderDragBlobs()

********************************************************************************************/
void OpSelectPathPoints::RenderRubberRect(DocRect* Rect)
{
	RenderRegion* pRegion = DocView::RenderOnTop(Rect, StartSpread, UnclippedEOR);
	while (pRegion)
	{
		// Set the line colour 
		pRegion -> SetLineColour(COLOUR_XORNEW);
		pRegion -> SetFillColour(COLOUR_NONE);

		// And Draw the rect
		DocRect RubberBox(StartPoint, StartPoint);
		RubberBox.IncludePoint(PreviousPoint);
		
		// Draw the rectangle
		pRegion->DrawDragRect(&RubberBox);

		// Get the Next render region
		pRegion = DocView::GetNextOnTop(Rect);
	}
}



/********************************************************************************************

>	void OpSelectPathPoints::RenderRubberSelection(RenderRegion* pRegion)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/94
	Inputs:		pRegion - pointer to the render region to draw into.
	Returns:	-
	Purpose:	EORs the blobs on the selected paths onto the screen.  This is is needed to
				solve quite an ugly situation.  If the path(s) are scrolled then their blobs
				will be redrawn as per their document status.  We need to go through
				all the paths rendering their selection status as we want them to be.
	SeeAlso:	OpSelectPathPoints::RenderRubberRect(), OpSelectPathPoints::RenderDragBlobs()

********************************************************************************************/

void OpSelectPathPoints::RenderRubberSelection(RenderRegion* pRegion)
{
	ERROR3IF (pRegion == NULL, "Pointer to RenderRegion was NULL");
	ERROR3IF (!DragUnderway, "RenderRubberSelection called when no drag was underway");
	
	SelRange*	Selection = GetApplication()->FindSelection();
	Node*		pNode = Selection->FindFirst();

	DocRect RubberBox(StartPoint, StartPoint);
	RubberBox.IncludePoint(PreviousPoint);
	NodePath* pSelected = NULL;

	while (pNode)
	{
 		if (pNode->IsNodePath())
			pSelected = (NodePath*)pNode;
		else
			pSelected =	(NodePath*) (pNode->HasEditableChild(CC_RUNTIME_CLASS(NodePath), NULL));

		if ((pSelected) && (pSelected->FindParentSpread() == StartSpread))
		{
			Path*	ThisPath = &(pSelected->InkPath);
			INT32	UsedSlots = ThisPath->GetNumCoords();
			DocCoord*	Coords = ThisPath->GetCoordArray();
			PathFlags*	Flags = ThisPath->GetFlagArray();
			PathVerb*	Verbs = ThisPath->GetVerbArray();
			INT32 EndPtIndex =0;
			INT32 endcount = 0;

			for (INT32 i=0; i<UsedSlots; i++)
			{
				if (Flags[i].IsEndPoint && !(Verbs[i] & PT_CLOSEFIGURE))
				{
					// Now we need to render on the selection blob status
					BOOL First = TRUE;		// By default deselect a selected blob
					BOOL Second = FALSE;	
					BOOL Render = FALSE;

					if (Flags[i].IsSelected)
					{
						EndPtIndex = i;
						endcount++;
						Render = TRUE;
					}
					
					if (AdjustDrag)
						Render = FALSE; 

					if (RubberBox.ContainsCoord(Coords[i]) && !Flags[i].IsSelected)
					{
						// Render an unselected blob as selected
						Render = TRUE;
						First = FALSE;
						Second = TRUE;
					}

					if (Render)
					{
						ThisPath->DrawBlob(pRegion, Coords[i], First);
						ThisPath->DrawBlob(pRegion, Coords[i], Second);
					}
				}
			}
			if (endcount == 1)
			{
				// Remove the control points and lines
				ThisPath->RenderPathControlBlobs(StartSpread,EndPtIndex);
			}
		}
		pNode = Selection->FindNext(pNode);
	}
}



/********************************************************************************************

>	BOOL OpSelectPathPoints::Declare()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpSelectPathPoints::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_SELECTPATHPOINTS),
								CC_RUNTIME_CLASS(OpSelectPathPoints), 
								OPTOKEN_BEZTOOL,
								OpSelectPathPoints::GetState,
								0,	/* help ID */
								_R(IDBBL_FREEHANDTOOLOP),
								0	/* bitmap ID */));
}



/********************************************************************************************

>	void OpSelectPathPoints::RemoveDragBlobs(BOOL RenderControlBlobs, BOOL Deselect)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/94
	Inputs:		RenderContolBlobs - TRUE if the control blobs should be rendered
				Deselect          - TRUE if all endpoints should be deslected
	Returns:	-
	Purpose:	Implement a common bit of code shared by two bits at the end of DragFinished.
				Both want to go through all selected paths although they do different things
				with them.
	SeeAlso:	OpSelectPathPoints::DragFinished()

********************************************************************************************/

void OpSelectPathPoints::RemoveDragBlobs(BOOL RenderControlBlobs, BOOL Deselect)
{
	DocRect RubberRect(StartPoint, StartPoint);
	RubberRect.IncludePoint(PreviousPoint);

	SelRange* Selected = GetApplication()->FindSelection();
	Node* pNode = Selected->FindFirst();
	NodePath* pSelected = NULL;

	while (pNode)
	{
 		if (pNode->IsNodePath())
			pSelected = (NodePath*)pNode;
		else
			pSelected =	(NodePath*) (pNode->HasEditableChild(CC_RUNTIME_CLASS(NodePath), NULL));

		if ((pSelected) && (pSelected->FindParentSpread() == StartSpread))
		{
			Path*	ThisPath = &(pSelected->InkPath);
			INT32	UsedSlots = ThisPath->GetNumCoords();
			DocCoord*	Coords = ThisPath->GetCoordArray();
			PathFlags*	Flags = ThisPath->GetFlagArray();
			PathVerb*	Verbs = ThisPath->GetVerbArray();

			RenderRegion* pRender = DocView::RenderOnTop(NULL, StartSpread, ClippedEOR);
			while (pRender!=NULL)
			{
				INT32 SelectedEndPoints = 0;
				INT32 EndPtIndex = 0;
				DocCoord EndPtCoord;
				for (INT32 i=0; i<UsedSlots; i++)
				{
					if (Flags[i].IsEndPoint)
					{
						if ( ((AdjustDrag && Flags[i].IsSelected) ||
							 	RubberRect.ContainsCoord(Coords[i]))
							  && !(Verbs[i] & PT_CLOSEFIGURE) )
						{
							// Remove the selection blobs
							ThisPath->DrawBlob(pRender, Coords[i], TRUE);
							ThisPath->DrawBlob(pRender, Coords[i], FALSE);
						}
						if (Flags[i].IsSelected)
						{
							SelectedEndPoints ++;
							EndPtIndex = i;
							EndPtCoord = Coords[i];
						}
					}
					if (Deselect)
					{
						Flags[i].IsSelected = FALSE;
					}
				}
				if ((SelectedEndPoints == 1) && (RenderControlBlobs))
				{
					ThisPath->RenderPathControlBlobs(StartSpread,EndPtIndex);
					if (!(AdjustDrag && RubberRect.ContainsCoord(EndPtCoord)))
					{
						ThisPath->DrawBlob(pRender, EndPtCoord, FALSE);
						ThisPath->DrawBlob(pRender, EndPtCoord, TRUE);
					}
				}
				pRender = DocView::GetNextOnTop(NULL);
			}
		}
		pNode = Selected->FindNext(pNode);
	}
}




/********************************************************************************************

>	OpState OpSelectPathPoints::GetState(String_256* Description, OpDescriptor*)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/6/94
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the push tool
	Returns:	The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpSelectPathPoints::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}



////////////////////////////////////////////////////////////////////////////////////////////
//                                   OpInsertFloater                                      //
////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************************************

>	OpInsertFloater::OpInsertFloater()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/94
	Purpose:	Constructor. Currently does nothing.

********************************************************************************************/

OpInsertFloater::OpInsertFloater()
{
}



/********************************************************************************************

>	OpState OpInsertFloater::GetState(String_256* Description, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/94
	Outputs:	-
	Returns:	-
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpInsertFloater::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}



/********************************************************************************************

>	BOOL OpInsertFloater::Declare()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/94
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpInsertFloater::Declare()
{
	return (RegisterOpDescriptor(	0, 
									_R(IDS_NODEPATH_EDIT),
									CC_RUNTIME_CLASS(OpInsertFloater), 
									OPTOKEN_INSERTFLOATER,
									OpInsertFloater::GetState));
}



/********************************************************************************************

>	void OpInsertFloater::DoWithParam(OpDescriptor* pOpDesc, OpParam* pOpParam)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/94
	Inputs:		pOpDesc - pointer to the OpDescriptor for the operation
				pOpParam - pointer to the operations parameter object
	Returns:	-
	Purpose:	Performs the insertion of the floating endpoint into the Line tool, creating
				undo actions.

********************************************************************************************/

void OpInsertFloater::DoWithParam(OpDescriptor* pOpDesc, OpParam* pOpParam)
{
	InsertFloaterParam*	Params = (InsertFloaterParam*)pOpParam;

	if (Params->pOpWorkingDocument != NULL)
	{
		pOurDoc = Params->pOpWorkingDocument;
		pOurView = NULL;
	}

	if (!InsertFloaterAction::DoInsert(this, &UndoActions, Params->pTool,
												&Params->InsertPos, Params->InsertSpread))
	{
		FailAndExecute();
	}

	End();
}



/********************************************************************************************

>	void InsertFloaterParam::InsertFloaterParam(DocCoord* pInsertPos,
												Spread* pInsertSpread,
												BezierTool* pLineTool,
												Document* pWorkDoc)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/94
	Inputs:		pInsertPos - pointer to a coordinate to insert the floating endpoint.
				pInsertSpread - pointer to a spread to insert the floating endpoint onto.
				pLineTool - pointer to the Line Tool (which contains the floating endpoint)
				pWorkDoc - points to the document to invoke this operation into
	Outputs:	-
	Returns:	-
	Purpose:	Constructs the InsertFloaterParam object ready for passing to OpInsertFloater

********************************************************************************************/
InsertFloaterParam::InsertFloaterParam(DocCoord* pInsertPos, Spread* pInsertSpread,
												BezierTool* pLineTool, Document* pWorkDoc)
{
	ERROR3IF(pInsertPos == NULL, "pInsertPos was NULL");
	ERROR3IF(pInsertSpread == NULL, "pInsertSpread was NULL");
	ERROR3IF(pLineTool == NULL, "pTool was NULL");

	InsertPos = *pInsertPos;
	InsertSpread = pInsertSpread;
	pTool = pLineTool;
	pOpWorkingDocument = pWorkDoc;
}



////////////////////////////////////////////////////////////////////////////////////////////
//                                   OpRemoveFloater                                      //
////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************************************

>	OpRemoveFloater::OpRemoveFloater()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/94
	Purpose:	Constructor. Currently does nothing.

********************************************************************************************/

OpRemoveFloater::OpRemoveFloater()
{
}



/********************************************************************************************

>	OpState OpRemoveFloater::GetState(String_256* Description, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/94
	Outputs:	-
	Returns:	-
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpRemoveFloater::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}



/********************************************************************************************

>	BOOL OpRemoveFloater::Declare()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/94
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpRemoveFloater::Declare()
{
	return (RegisterOpDescriptor(	0, 
									_R(IDS_NODEPATH_EDIT),
									CC_RUNTIME_CLASS(OpRemoveFloater), 
									OPTOKEN_REMOVEFLOATER,
									OpRemoveFloater::GetState));
}



/********************************************************************************************

>	void OpRemoveFloater::DoWithParam(OpDescriptor* pOpDesc, OpParam* pOpParam)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/94
	Inputs:		pOpDesc - pointer to the OpDescriptor for the operation
				pOpParam - pointer to the operations parameter object
	Returns:	-
	Purpose:	Performs the removal of the floating endpoint from the Line tool, creating
				undo actions.

********************************************************************************************/

void OpRemoveFloater::DoWithParam(OpDescriptor* pOpDesc, OpParam* pOpParam)
{
	InsertFloaterParam*	Params = (InsertFloaterParam*)pOpParam;

	if (Params->pOpWorkingDocument != NULL)
	{
		pOurDoc = Params->pOpWorkingDocument;
		pOurView = NULL;
	}

	// Create an undo action for this operation
	DocCoord	OldPos;
	Spread*		OldpSpread;
	Document*	OldpDoc;
	
	if (Params->pTool->GetMoveTo(&OldpSpread, &OldPos, &OldpDoc))
	{
		if (!InsertFloaterAction::DoInsert(this, &UndoActions, Params->pTool, &OldPos, OldpSpread))
		{
			FailAndExecute();
		}
	}

	// and now perform the operation
	Params->pTool->ClearMoveTo();

	End();
}



///////////////////////////////////////////////////////////////////////////////////////////////
// The InsertFloaterAction class													    	 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	InsertFloaterAction::InsertFloaterAction()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action to insert a floating endpoint.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
InsertFloaterAction::InsertFloaterAction()
{
	pLineTool = NULL;
}


/********************************************************************************************

>	ActionCode InsertFloaterAction::Init(	Operation* pOp,
											ActionList* pActionList,
											BezierTool* pTool,
											Action** NewAction)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/12/94
	Inputs:		pOp is the pointer to the operation to which this action belongs
				pTool is a pointer to the Line tool so the action can access the SetLineTo
				and ClearLineTo functions
				pActionList is the action list to which this action should be added
	Outputs:	NewAction is a pointer to a pointer to an action, allowing the function to
				return a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no
				room in the undo buffer (which is determined by the base class Init function
				called within) the function will either return AC_NO_RECORD which means the
				operation can continue, but no undo information needs to be stored, or AC_OK
				which means the operation should continue AND record undo information. If
				the function returns AC_FAIL, there was not enough memory to record the undo
				information, and the user has decided not to continue with the operation.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/
ActionCode InsertFloaterAction::Init(	Operation* pOp,
										ActionList* pActionList,
										Action** NewAction)
{
	UINT32 ActSize = sizeof(InsertFloaterAction);

	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(InsertFloaterAction), NewAction);
	return Ac;
}



/********************************************************************************************

>	ActionCode InsertFloaterAction::DoInsert(Operation* pOp,
											ActionList* pActionList,
											BezierTool* pTool)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/12/94
	Inputs:		pOp is the currently running operation
				pActionList is a pointer ot the action list to which the action should be appended
				pTool is a pointer to the Line tool.
	Outputs:	-
	Returns:	Action code which indicates success or failure to create the action
	Purpose:	This static function makes it a little easier to use this action. It creates an instance
				of this action and appends it to the action list.
	Errors:		-
	SeeAlso:	RemoveFloaterAction::DoRemove

********************************************************************************************/

ActionCode InsertFloaterAction::DoInsert(Operation* pOp,
										ActionList* pActionList,
										BezierTool* pTool,
										DocCoord*	pPoint,
										Spread*		pSpread)
{
	InsertFloaterAction* RecAction = NULL;
	ActionCode Act = InsertFloaterAction::Init(pOp, pActionList, (Action**)&RecAction);
	if ( (Act == AC_OK) && (RecAction != NULL) )
	{
		RecAction->pLineTool = pTool;
		RecAction->pSpread = pSpread;
		RecAction->Point = *pPoint;
	}
	return Act;
}



/********************************************************************************************

>	ActionCode InsertFloaterAction::Execute()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/12/94
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, either AC_OK, AC_NORECORD or AC_FAIL
	Purpose:	Inserts the floating endpoint into the Line Tool, creating an undo action to
				remove the endpoint.
	Errors:		-
	SeeAlso:	RemoveFloaterAction::Execute

********************************************************************************************/
ActionCode InsertFloaterAction::Execute()
{
	ERROR3IF(pLineTool == NULL, "Pointer to line tool was NULL");

	RemoveFloaterAction* ReAction = NULL;
	ActionCode Act = AC_FAIL;
	
	// Create a redo action for this action, which is a RemoveFloaterAction
  	if (pLineTool != NULL)
	{
		Act = RemoveFloaterAction::Init(pOperation, 
										pOppositeActLst, 
										pLineTool,
										(Action**)(&ReAction));
		if (Act == AC_FAIL)
			return AC_FAIL;

		// Now do the actual action
		pLineTool->SetMoveTo(Point, pSpread, GetWorkingDoc());
	}

	return Act;
}

InsertFloaterAction::~InsertFloaterAction()
{
}



///////////////////////////////////////////////////////////////////////////////////////////////
// The RemoveFloaterAction class																 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	RemoveFloaterAction::RemoveFloaterAction()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action to remove a floating endpoint.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
RemoveFloaterAction::RemoveFloaterAction()
{
	pLineTool = NULL;
}


/********************************************************************************************

>	ActionCode RemoveFloaterAction::Init(	Operation* pOp,
											ActionList* pActionList,
											BezierTool* pTool,
											Action** NewAction)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/12/94
	Inputs:		pOp is the pointer to the operation to which this action belongs
				pTool is a pointer to the Line tool so the action can access the SetLineTo
				and ClearLineTo functions
				pActionList is the action list to which this action should be added
	Outputs:	NewAction is a pointer to a pointer to an action, allowing the function to
				return a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no
				room in the undo buffer (which is determined by the base class Init function
				called within) the function will either return AC_NO_RECORD which means the
				operation can continue, but no undo information needs to be stored, or AC_OK
				which means the operation should continue AND record undo information. If
				the function returns AC_FAIL, there was not enough memory to record the undo
				information, and the user has decided not to continue with the operation.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/
ActionCode RemoveFloaterAction::Init(	Operation* pOp,
										ActionList* pActionList,
										BezierTool* pTool,
										Action** NewAction)
{
	UINT32 ActSize = sizeof(RemoveFloaterAction);

	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(RemoveFloaterAction), NewAction);
	if ((Ac == AC_OK) && (*NewAction != NULL))
	{
		((RemoveFloaterAction*)*NewAction)->pLineTool = pTool;
	}
	return Ac;
}



/********************************************************************************************

>	ActionCode RemoveFloaterAction::DoRemove(Operation* pOp,
											ActionList* pActionList,
											BezierTool* pTool)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/12/94
	Inputs:		pOp is the currently running operation
				pActionList is a pointer ot the action list to which the action should be appended
				pTool is a pointer to the Line tool.
	Outputs:	-
	Returns:	Action code which indicates success or failure to create the action
	Purpose:	This static function makes it a little easier to use this action. It creates an instance
				of this action and appends it to the action list.
	Errors:		-
	SeeAlso:	InsertFloaterAction::DoInsert

********************************************************************************************/

ActionCode RemoveFloaterAction::DoRemove(Operation* pOp,
										ActionList* pActionList,
										BezierTool* pTool)
{
	RemoveFloaterAction* RecAction;
	ActionCode Act = RemoveFloaterAction::Init(pOp, pActionList, pTool, (Action**)&RecAction);
	return Act;
}



/********************************************************************************************

>	ActionCode RemoveFloaterAction::Execute()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/12/94
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, either AC_OK, AC_NORECORD or AC_FAIL
	Purpose:	Removes the floating endpoint from the Line Tool, creating an undo action to
				insert the endpoint.
	Errors:		-
	SeeAlso:	InsertFloaterAction::Execute

********************************************************************************************/
ActionCode RemoveFloaterAction::Execute()
{
	ERROR3IF(pLineTool == NULL, "Pointer to line tool was NULL");

	InsertFloaterAction* ReAction = NULL;
	ActionCode Act = AC_FAIL;
	
	// Create a redo action for this action, which is an InsertFloaterAction
  	if (pLineTool != NULL)
	{
		DocCoord	OldPos;
		Spread*		OldpSpread;
		Document*	OldpDoc;

		if (pLineTool->GetMoveTo(&OldpSpread, &OldPos, &OldpDoc))
		{
			Act = InsertFloaterAction::Init(pOperation, pOppositeActLst, (Action**)(&ReAction));
			if (ReAction != NULL)
			{
				ReAction->pLineTool = pLineTool;
				ReAction->pSpread = OldpSpread;
				ReAction->Point = OldPos;
			}

			if (Act == AC_FAIL)
				return AC_FAIL;
		}

		// Now do the actual action
		pLineTool->ClearMoveTo();
	}

	return Act;
}

RemoveFloaterAction::~RemoveFloaterAction()
{
}

