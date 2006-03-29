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
// Contains general purpose operations to be used in path editing

/*
*/

#include "camtypes.h"
#include "pathedit.h"

// Code headers
#include "app.h"
#include "attrmgr.h"
#include "blobs.h"
#include "csrstack.h"
#include "cursor.h"
#include "document.h"
#include "docview.h"
#include "fixmem.h"				// for CCMalloc and CCFree
#include "group.h"
#include "lineattr.h"			// for getting the line width of a NodePath
#include "nodepath.h"
#include "ndtxtpth.h"
#include "nodeattr.h"
#include "objchge.h"
#include "pathops.h"
#include "progress.h"
#include "tool.h"
#include "tranform.h"
#include "attrmap.h"
#include "spread.h"
//#include "bubbleid.h"
#include "ndbldpth.h"
#include "nodeblnd.h"
//#include "blndtool.h"
#include "nodebldr.h"
#include "opdrbrsh.h"
// Resource headers
#include "helpids.h"
//#include "jim.h"
//#include "peter.h"
//#include "resource.h"    
//#include "barsdlgs.h"
//#include "rik.h"
//#include "simon.h"
//#include "viewrc.h"
//#include "phil.h"
#include "becomea.h"
#include "brshattr.h"
#include "opbevel.h"
#include "nodemold.h"
#include "ndmldpth.h"
#include "samplist.h"
#include "brshattr.h"
#include "ophist.h"

#define PATRACE if(IsUserName("Peter")) TRACE

// Useful macro, given a pointer to a CCObject (or an object derived
// from it) and a string format, it will output the class name of the object. 
// Example usage PACLASSNAME(CurrentNode,"Node class = %s\n");
// N.B. No checking is node or parameters.  Note semi-colon on end.
#define PACLASSNAME(a,b) CCRuntimeClass* Blob; Blob = a->GetRuntimeClass(); PATRACE(b,Blob->m_lpszClassName)

// Operations
CC_IMPLEMENT_DYNCREATE( OpJoinShapes, SelOperation)
CC_IMPLEMENT_DYNCREATE( OpBreakShapes, SelOperation) 
CC_IMPLEMENT_DYNCREATE( OpDeletePoints, SelOperation) 		// Deleting of path endpoints
CC_IMPLEMENT_DYNCREATE( OpNodePathEditBlob, SelOperation)	// Dragging of a path endpoint
CC_IMPLEMENT_DYNCREATE( OpToggleSmooth, SelOperation)		// Toggling smooth/cuspness of endpoints
CC_IMPLEMENT_DYNCREATE( OpNodePathEditControlBlob, OpNodePathEditBlob)	// Dragging of a Bezier control point
CC_IMPLEMENT_DYNCREATE( OpReshapeOrAddPoint, OpNodePathEditBlob)		// Reshaping a path OR adding an endpoint
CC_IMPLEMENT_DYNCREATE( OpNodePathAddEndpoint, OpNodePathEditBlob)		// Adding an endpoint to the end of the path
CC_IMPLEMENT_DYNCREATE( OpNewPath, OpNodePathEditBlob)
CC_IMPLEMENT_DYNCREATE( OpCloseNodePaths, OpNodePathAddEndpoint)		// Closing all selected open paths

// Actions
CC_IMPLEMENT_DYNCREATE( ModifyPathAction, Action)
CC_IMPLEMENT_DYNCREATE( RemovePathElementAction, Action)
CC_IMPLEMENT_DYNCREATE( InsertPathElementAction, Action)
CC_IMPLEMENT_DYNCREATE( ModifyElementAction, Action)
CC_IMPLEMENT_DYNCREATE( ModifyFlagsAction, Action)
CC_IMPLEMENT_DYNCREATE( RecalcBoundsAction, Action)
CC_IMPLEMENT_DYNCREATE( RecordBoundsAction, Action)
CC_IMPLEMENT_DYNCREATE( ModifyFilledAction, Action)
CC_IMPLEMENT_DYNCREATE( SavePathArraysAction, Action)
//CC_IMPLEMENT_DYNCREATE( StorePathSubSelStateAction, Action)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	OpNodePathEditBlob::OpNodePathEditBlob()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/93
	Purpose:	Constructor. This simply sets a few of the operation flags.

********************************************************************************************/

OpNodePathEditBlob::OpNodePathEditBlob()
{
	EndSnapped = FALSE;	
	SnapToAnother = FALSE;	
	pMoveCursor = NULL;
	SnapToPath = NULL;
	DragStarted = FALSE;
	SnapToLineOrCurve = FALSE;
	DragPoint = -1;
	UpdatePoint = -1;
	ConstrainPoint = DocCoord(-1,-1);
	ConstrainPrevPoint = DocCoord(-1,-1);
	ConstrainNextPoint = DocCoord(-1,-1);
	MultiplePaths = FALSE;
}

/********************************************************************************************

>	void OpNodePathEditBlob::DoStartDragEdit(NodePath* OrigPath, DocCoord Anchor, Spread *pSpread)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> - latterly Peter
	Created:	5/7/93
	Inputs:		OrigPath - Pointer to the path we are about to edit
				Anchor - The position of the mouse at the start of the Drag
				pSpread - The spread that the path is on
	Purpose:	This is called to start a drag operation on an endpoint on a path

********************************************************************************************/

void OpNodePathEditBlob::DoStartDragEdit(NodePath* OrigPath, DocCoord Anchor, Spread *pSpread)
{
	BOOL Success = TRUE;
	
	// We had better take a note of the starting point of the drag
	LastMousePos = Anchor;
	StartMousePos = Anchor;
	StartSpread  = pSpread;

	SelRange* theSelection = GetApplication ()->FindSelection ();

	BOOL selectionContainsMoulds = FALSE;

	if (theSelection)
	{
		// we need to do some special processing to handle moulds ....

		Node* pCurrentNode = (Node*) theSelection->FindFirst ();

		while (pCurrentNode)
		{
			if (IS_A (pCurrentNode, NodeMould))
			{
				selectionContainsMoulds = TRUE;
				pCurrentNode = NULL;
			}
			else
			{
				pCurrentNode = (Node*) theSelection->FindNext (pCurrentNode);
			}
		}
	}

	if (!selectionContainsMoulds)
	{
		BevelTools::BuildListOfSelectedNodes(&OriginalPaths, CC_RUNTIME_CLASS(NodePath));
	}
	else
	{
		// now go and get those moulds baby !
		BevelTools::BuildListOfSelectedNodes(&OriginalPaths, CC_RUNTIME_CLASS(NodeMouldPath));

		// now, we also have to rescan the selection (again!) BUT this time without
		// the moulds

		Node* pCurrentNode = (Node*) theSelection->FindFirst ();

		while (pCurrentNode)
		{
			if (IS_A (pCurrentNode, NodeMould))
			{
				pCurrentNode = (Node*) theSelection->FindNext (pCurrentNode);
			}
			else
			{
				if (IS_A (pCurrentNode, NodePath))
				{
					NodeListItem* pInsert = new NodeListItem ();
					
					if (pInsert)	// and insert into the paths list ....
					{
						pInsert->pNode = pCurrentNode;
						OriginalPaths.AddHead (pInsert);
					}
				}
				
				pCurrentNode = (Node*) theSelection->FindNext (pCurrentNode);
			}
		}
	}

	if (OriginalPaths.GetCount () == 1)
	//if (TRUE)
	{
		MultiplePaths = FALSE;
		
		OriginalPath = OrigPath;

		// Now calculate DragPoint and UpdatePoint
		if (Success)
		{
			PathFlags* Flags = OriginalPath->InkPath.GetFlagArray();
			PathVerb* Verbs = OriginalPath->InkPath.GetVerbArray();
			INT32 NumCoords = OriginalPath->InkPath.GetNumCoords();

			for (INT32 i=0;i<NumCoords;i++)
			{
				if (Flags[i].IsEndPoint && Flags[i].IsSelected 
								&& !(OriginalPath->InkPath.IsSubPathClosed(i) && (Verbs[i] == PT_MOVETO)) )
				{
					// If you are dragging a closepoint then you are actually dragging two points
					// but we need to update the line tool as the user thinks they are dragging one.
					// If we are on the opening moveto then just skip the tests.
					if (DragPoint != -1)
					{
						UpdatePoint = -1;
						DragPoint = -1;
						break;
					}
					else
					{
						UpdatePoint = i;
						DragPoint = i;
					}
				}
			}
			// On exit from that loop, DragPoint = -1 if there are multiple selected endpoints,
			// otherwise DragPoint is the index to the selected endpoint.  UpdatePoint is the
			// index of the point displaied in the Line tool
		}

		// Set the constrain point
		if (DragPoint != -1)
		{
			ConstrainPoint = OriginalPath->InkPath.GetCoordArray()[DragPoint];

			// Get the previous endpoint
			INT32 OtherEndpoint = DragPoint;
			if (OriginalPath->InkPath.FindPrevEndPoint(&OtherEndpoint))
				ConstrainPrevPoint = OriginalPath->InkPath.GetCoordArray()[OtherEndpoint];
			else
				ConstrainPrevPoint = ConstrainPoint;

			// Get the next endpoint
			OtherEndpoint = DragPoint;
			if (OriginalPath->InkPath.FindNextEndPoint(&OtherEndpoint))
				ConstrainNextPoint = OriginalPath->InkPath.GetCoordArray()[OtherEndpoint];
			else
				ConstrainNextPoint = ConstrainPoint;
		}
		else
		{
			ConstrainPoint = Anchor;
			ConstrainPrevPoint = Anchor;
			ConstrainNextPoint = Anchor;
		}

		// We also need to make a version of the path that we can change
		Success = BuildEditPath();

		// Create and send a change message about this path edit
		// This one is handled by moulds in their OnChildChange() function
		if (Success)
			Success = (EditObjChange.ObjChangeStarting(OrigPath,this,&EditPath,StartSpread,TRUE) == CC_OK);

		// Create and display the cursors for this operation
		if (Success)
			Success = CreateCursors();
		if (Success)
			ChangeCursor(pCrossHairCursor);
		
	//	// Render the bits of the path that are different
		DocRect EditPathBBox = EditPath.GetBoundingRect();
	//	if (Success)
	//		RenderPathEditBlobs(EditPathBBox, pSpread);

		// Tell the Dragging system that we need drags to happen
		if (Success)
			Success = StartDrag(DRAGTYPE_AUTOSCROLL, &EditPathBBox, &LastMousePos);

		if (!Success)
		{
			InformError();
			FailAndExecute();
			End();
		}
	}
	else
	{
		// lets try and keep things the same for the blobs parent as in the one selection case
		// BUT lets also try and do our extra stuff ....  I expect time MUCK UPS to occur !!!!

		MultiplePaths = TRUE;
		
		OriginalPath = OrigPath;

		// we need to make OrigPath the first one in our linked list ....

		NodeListItem* pCurrentOrig = (NodeListItem*) OriginalPaths.GetHead ();

		while (pCurrentOrig)
		{
			NodePath* pOrigPath = (NodePath*) (pCurrentOrig->pNode);

			if (pOrigPath == OrigPath)
			{
				NodeListItem* newHead = (NodeListItem*) OriginalPaths.RemoveItem (pCurrentOrig);
				
				OriginalPaths.AddHead (newHead);

				pCurrentOrig = NULL;
			}
			else
			{
				pCurrentOrig = (NodeListItem*) OriginalPaths.GetNext (pCurrentOrig);
			}
		}

		// Now calculate DragPoint and UpdatePoint
		if (Success)
		{
			NodeListItem* pCurrentOrig = (NodeListItem*) OriginalPaths.GetHead ();

			// CGS:  while loop taken out since we can only be 'drag sensitive' to the point
			// that is actually being dragged

			//while (pCurrentOrig)
			{
				NodePath* pOrigPath = (NodePath*) (pCurrentOrig->pNode);
				
				PathFlags* Flags = pOrigPath->InkPath.GetFlagArray();
				PathVerb* Verbs = pOrigPath->InkPath.GetVerbArray();
				INT32 NumCoords = pOrigPath->InkPath.GetNumCoords();

				for (INT32 i=0;i<NumCoords;i++)
				{
					if (Flags[i].IsEndPoint && Flags[i].IsSelected 
									&& !(pOrigPath->InkPath.IsSubPathClosed(i) && (Verbs[i] == PT_MOVETO)) )
					{
						// If you are dragging a closepoint then you are actually dragging two points
						// but we need to update the line tool as the user thinks they are dragging one.
						// If we are on the opening moveto then just skip the tests.
						if (DragPoint != -1)
						{
							UpdatePoint = -1;
							DragPoint = -1;
							break;
						}
						else
						{
							UpdatePoint = i;
							DragPoint = i;
						}
					}
				}

			//	pCurrentOrig = (NodeListItem*) OriginalPaths.GetNext (pCurrentOrig);
			}
			// On exit from that loop, DragPoint = -1 if there are multiple selected endpoints,
			// otherwise DragPoint is the index to the selected endpoint.  UpdatePoint is the
			// index of the point displaied in the Line tool
		}

		// Set the constrain point - do this only for the path that was clicked on
		// Any guesses as to why?

		// Answer:  cause its the one that is being dragged!
		if (DragPoint != -1)
		{
			NodeListItem* pCurrentOrig = (NodeListItem*) OriginalPaths.GetHead ();
			NodePath* pOrigPath = (NodePath*) (pCurrentOrig->pNode);
			
			ConstrainPoint = pOrigPath->InkPath.GetCoordArray()[DragPoint];

			// Get the previous endpoint
			INT32 OtherEndpoint = DragPoint;
			if (pOrigPath->InkPath.FindPrevEndPoint(&OtherEndpoint))
				ConstrainPrevPoint = pOrigPath->InkPath.GetCoordArray()[OtherEndpoint];
			else
				ConstrainPrevPoint = ConstrainPoint;

			// Get the next endpoint
			OtherEndpoint = DragPoint;
			if (pOrigPath->InkPath.FindNextEndPoint(&OtherEndpoint))
				ConstrainNextPoint = pOrigPath->InkPath.GetCoordArray()[OtherEndpoint];
			else
				ConstrainNextPoint = ConstrainPoint;
		}
		else
		{
			ConstrainPoint = Anchor;
			ConstrainPrevPoint = Anchor;
			ConstrainNextPoint = Anchor;
		}

		// We also need to make versions of the paths that we can change
		Success = BuildEditPaths();

		// Create and send a change message about this path edit
		// This one is handled by moulds in their OnChildChange() function
		if (Success)
		{
			NodeListItem* pCurrentOrig = (NodeListItem*) OriginalPaths.GetHead ();
			NodeListItem* pCurrentEdit = (NodeListItem*) EditPaths.GetHead ();

			while ((pCurrentOrig) && (pCurrentEdit))
			{
				NodePath* pOrigPath = (NodePath*) (pCurrentOrig->pNode);
				Path* pEditPath = (Path*) (pCurrentEdit->pNode);
				
				//////////////////////		obchange stuff ....
				
				ObjChangePathEdit* NewEditObjChange = new ObjChangePathEdit;
				
				Success = (NewEditObjChange->ObjChangeStarting(pOrigPath,this,pEditPath,StartSpread,TRUE) == CC_OK);

				NodeListItem* pInsert = new NodeListItem;
				pInsert->pNode = (Node*) NewEditObjChange;
		
				ObjChanges.AddTail (pInsert);

				//////////////////////		drag started stuff ....

				BOOL* NewDragStartVal = new BOOL;
				*NewDragStartVal = FALSE;
				
				NodeListItem* pInsert2 = new NodeListItem;
				pInsert2->pNode = (Node*) NewDragStartVal;

				PathsDragStarted.AddTail (pInsert2);

				//////////////////////		yea baby

				if (Success)
				{
					pCurrentOrig = (NodeListItem*) OriginalPaths.GetNext (pCurrentOrig);
					pCurrentEdit = (NodeListItem*) EditPaths.GetNext (pCurrentEdit);
				}
				else
				{
					pCurrentOrig = NULL;
					pCurrentEdit = NULL;
				}

				//////////////////////		now delete stuff ....
				
				//delete (NewEditObjChange);
		//		delete (NewDragStartVal);
			}
		}

		// Create and display the cursors for this operation (only once - obviously)
		if (Success)
			Success = CreateCursors();
		if (Success)
			ChangeCursor(pCrossHairCursor);
		
	//	// Render the bits of the path that are different
		DocRect EditPathBBox;// = EditPath.GetBoundingRect();

		SelRange* Selected = GetApplication()->FindSelection();

		EditPathBBox = Selected->GetBlobBoundingRect ();

	//	if (Success)
	//		RenderPathEditBlobs(EditPathBBox, pSpread);

		// Tell the Dragging system that we need drags to happen
		if (Success)
			Success = StartDrag(DRAGTYPE_AUTOSCROLL, &EditPathBBox, &LastMousePos);

		if (!Success)
		{
			InformError();
			FailAndExecute();
			End();
		}
	}
}



/********************************************************************************************

>	void OpNodePathEditBlob::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL bSolidDrag)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods - Which key modifiers are being pressed
	Purpose:	This is called every time the mouse moves, during a drag.
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpNodePathEditBlob::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread *pSpread, BOOL bSolidDrag)
{
	if (!MultiplePaths)
	{
		// If drag has moved onto a different spread, convert the coord to be relative to the
		// original spread.
		if (pSpread != StartSpread)
			PointerPos = MakeRelativeToSpread(StartSpread, pSpread, PointerPos);

		Path TempPath;

		// inform the parent a change is happening
		if (EditObjChange.ChangeMask.EorBlobs)
		{
			ChangeCode Chge = EditObjChange.RenderCurrentBlobs(OriginalPath,this,&EditPath,StartSpread,TRUE);
			if (Chge!=CC_OK)
				return;
			// Create a local copy of the edit path
			if (!TempPath.Initialise(EditPath.GetNumCoords(), 12))
				return;
			TempPath.CopyPathDataFrom(&EditPath);
		}

		// Rub out the old EORed version of the path
		if (DragStarted)
			RenderPathEditBlobs( EditPath.GetBoundingRect(), StartSpread );
		else
			DragStarted = TRUE;

		// Here is where we do all the snapping to grids and endpoints
		// and constrain. First we check for snapping to line ends, then
		// we check constrain, then we check grid snapping

		EndSnapped = FALSE;			// TRUE if this point snaps to the other end
		SnapToAnother = FALSE;		// TRUE if this point snaps to another line end
		SnapToLineOrCurve = FALSE;	// No snapped other line

		// We might be snapping to an endpoint if the point we're dragging around is either
		// a moveto, or is the last point in the subpath. We will also only snap if there is
		// only one point selected on the path

		DocRect PointerBlobRect;
		GetApplication()->GetBlobManager()->GetBlobRect(PointerPos,&PointerBlobRect);

		DocCoord* Coords = EditPath.GetCoordArray();
//		PathFlags* Flags = EditPath.GetFlagArray();
		PathVerb* Verbs = EditPath.GetVerbArray();
		INT32 NumCoords = EditPath.GetNumCoords();

		INT32 i;
		if (DragPoint != -1)
		{
			// Look to see if the selected point is a real endpoint
			if (Verbs[DragPoint] == PT_MOVETO)
			{
				// This is the start element of the path
				i = DragPoint;
				if (!EditPath.IsSubPathClosed(i))
				{
					INT32 NextEndpoint = i;
					if (EditPath.FindNextEndPoint(&NextEndpoint))
					{
						EditPath.FindEndElOfSubPath(&i);		// i indexes to the end element

						// If this sub-path is one element long then closing it is not allowed
						if ((i != NextEndpoint) && PointerBlobRect.ContainsCoord(Coords[i]))
						{
							PointerPos.x = Coords[i].x + LastMousePos.x - Coords[DragPoint].x;
							PointerPos.y = Coords[i].y + LastMousePos.y - Coords[DragPoint].y;
							EndSnapped = TRUE;
						}
					}
				}
			}
			else if (DragPoint+1 == NumCoords || Verbs[DragPoint+1] == PT_MOVETO)
			{
				// This is the last element in the sub-path
				i = DragPoint;
				if (!EditPath.IsSubPathClosed(i))
				{
					INT32 PrevEndpoint = i;
					if (EditPath.FindPrevEndPoint(&PrevEndpoint))
					{
						EditPath.FindStartOfSubPath(&i);			// i is the index of the start of the sub-path
			
						// If this sub-path is one element long then closing it is not allowed
						if ((i != PrevEndpoint) && PointerBlobRect.ContainsCoord(Coords[i]))
						{
							PointerPos.x = Coords[i].x + LastMousePos.x - Coords[DragPoint].x;
							PointerPos.y = Coords[i].y + LastMousePos.y - Coords[DragPoint].y;
							EndSnapped = TRUE;
						}
					}
				}
			}
		}
		
		// That detects snapping to the opposite endpoint, but what about snapping to another
		// endpoint altogether? Time to check for that eventuality
		if (!EndSnapped && DragPoint != -1 && (Verbs[DragPoint] == PT_MOVETO || DragPoint+1 == NumCoords || Verbs[DragPoint+1] == PT_MOVETO) )
		{
			// Get a snapshot of the selection
			SelRange* Selected = GetApplication()->FindSelection();
			Node* pNode = Selected->FindFirst();
			INT32 SubIndex,SubEnd;
			while (pNode)
			{
				if ((pNode->FindParentSpread() == StartSpread) && 
					(pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodePath)) /*&&
					(pNode != OriginalPath)*/ )
				{
					Path* ThisPath = &(((NodePath*)pNode)->InkPath);
					INT32 ThisNum = ThisPath->GetNumCoords();
					SubIndex = 0;
					SubEnd = 0;
					while (SubIndex<ThisNum)		
					{
						DocCoord tempStart,tempEnd;
						ThisPath->SetPathPosition(SubIndex);	// Set the path's internal pos index
						if (ThisPath->GetSubPathEnds(&tempStart, &tempEnd))
						{
							SubEnd = ThisPath->GetPathPosition();
							// Now SubIndex is the index of the first, and SubEnd is the index
							// of the last element (N.B. might be a curveto, in which case add 2)
							if (ThisPath->GetVerb() == PT_BEZIERTO)
								SubEnd+=2;

							BOOL ClosedPath = (ThisPath->GetVerbArray()[SubEnd] & PT_CLOSEFIGURE) ;

							// Now compare start and end with pointer position, but not if this
							// subpath is the currently dragging subpath
							if ( !ClosedPath &&
								 ( (pNode != OriginalPath) ||
								   ((pNode == OriginalPath) && (DragPoint != SubIndex) && (DragPoint != SubEnd)) ) )
							{
								if (PointerBlobRect.ContainsCoord(tempStart))
								{
									break;
								}
								if (PointerBlobRect.ContainsCoord(tempEnd))
								{
									SubIndex = SubEnd;
									break;
								}
							}
						}
						else
						{
							SubEnd = ThisPath->GetPathPosition();
						}
						SubIndex = SubEnd+1;		// Points at start of next subpath
													// or top of whole path
					}
					// if SubIndex < ThisNum we must have found a matching coord
					// so that's what we'll snap to
					if (SubIndex < ThisNum)
						break;				// Exit the outer while condition
				}
				pNode = Selected->FindNext(pNode);
			}
			// At this point, if pNode == NULL we haven't snapped to anything
			// otherwise pNode is the path we've snapped to, and SubIndex is the index
			// of the point we've snapped to
			if (pNode)
			{
				// if the snap to path is the path being edited check that it has the same number of points!
				if (((NodePath*)pNode == OriginalPath) && (OriginalPath->InkPath.GetNumCoords() != EditPath.GetNumCoords()))
				{
//					PATRACE( _T("Attempted to join an edited path!\n"));
					// The problem is that if the OriginalPath has been edited by adding an endpoint then
					// SnapToIndex will be wrong as it counted along the original path, not the edited one....
				}
				else
				{
					SnapToAnother = TRUE;
					SnapToPath = (NodePath*)pNode;
					SnapToIndex = SubIndex;
					SnapToPath->InkPath.SetPathPosition(SnapToIndex);
					PointerPos.x = SnapToPath->InkPath.GetCoord().x + LastMousePos.x - Coords[DragPoint].x;
					PointerPos.y = SnapToPath->InkPath.GetCoord().y + LastMousePos.y - Coords[DragPoint].y;
				}
			}
		}

		// We don't allow closed paths consiting of two straight segments
		if (EndSnapped && NumCoords < 4)
			EndSnapped = FALSE;
			
		if ((EndSnapped || SnapToAnother) && (pMoveCursor != NULL))
			ChangeCursor(pCloseCursor);
		else
			ChangeCursor(pCrossHairCursor);
		
		// Now constrain the mouse point
		ERROR3IF(ConstrainPoint == DocCoord(-1,-1),"ConstrainPoint wasn't set");
		ERROR3IF(ConstrainPrevPoint == DocCoord(-1,-1),"ConstrainPrevPoint wasn't set");
		ERROR3IF(ConstrainNextPoint == DocCoord(-1,-1),"ConstrainNextPoint wasn't set");
		if (!EndSnapped && ClickMods.Constrain)
		{
			if (ClickMods.Adjust)
			{
				if (ClickMods.Alternative1 || ClickMods.Alternative2)
					DocView::ConstrainToAngle(ConstrainNextPoint, &PointerPos);
				else
					DocView::ConstrainToAngle(ConstrainPrevPoint, &PointerPos);
			}
			else
				DocView::ConstrainToAngle(ConstrainPoint, &PointerPos);
		}

		// Only snap to grid if we didn't snap to the end of a path
		if (!EndSnapped)
			DocView::SnapCurrent(StartSpread,&PointerPos);

		// This is the bit where we go off and re-calculate the paths position,
		// based on how much the mouse has moved
		DocCoord Offset;
		Offset.x = PointerPos.x - LastMousePos.x;
		Offset.y = PointerPos.y - LastMousePos.y;

		RecalculatePath(Offset, EndSnapped, DragPoint);

		if (EditObjChange.ChangeMask.EorBlobs)
		{
			ChangeCode Chge = EditObjChange.RenderChangedBlobs(OriginalPath,this,&EditPath,StartSpread,TRUE);
			if (Chge!=CC_OK)
			{
				// replace the old edit path
				EditPath.CopyPathDataFrom(&TempPath);
				RenderPathEditBlobs( EditPath.GetBoundingRect(), StartSpread );
				return;
			}
		}

		// Update the Last Mouse Position
		LastMousePos = PointerPos;

		// Draw in the new version of the path
		RenderPathEditBlobs( EditPath.GetBoundingRect(), StartSpread );

		// If there is just one selected point being dragged then we should show
		// its new position in the line tool infobar.
		if (UpdatePoint != -1)
			BROADCAST_TO_ALL(PathEditedMsg(&EditPath, pSpread, UpdatePoint));

		SetStatusLineHelp();
	}
	else
	{	
		NodeListItem* pCurrentOrig = (NodeListItem*) OriginalPaths.GetHead ();
		NodeListItem* pCurrentEdit = (NodeListItem*) EditPaths.GetHead ();
		NodeListItem* pCurrentChange = (NodeListItem*) ObjChanges.GetHead ();
		NodeListItem* pCurrentDragS = (NodeListItem*) PathsDragStarted.GetHead ();

		DocCoord Offset(0,0);

		while ((pCurrentOrig) && (pCurrentEdit) && (pCurrentChange) && (pCurrentDragS))
		{
			NodePath* pOrigPath = (NodePath*) (pCurrentOrig->pNode);
			Path* pEditPath = (Path*) (pCurrentEdit->pNode);
			ObjChangePathEdit* pObjChange = (ObjChangePathEdit*) (pCurrentChange->pNode);
			BOOL* pDragStarted = (BOOL*) (pCurrentDragS->pNode);

/////////////////////////////////////////////////////////////////////////////////////////////////

			// If drag has moved onto a different spread, convert the coord to be relative to the
			// original spread.
			if (pSpread != StartSpread)
				PointerPos = MakeRelativeToSpread(StartSpread, pSpread, PointerPos);

			Path TempPath;

			// inform the parent a change is happening
			if (pObjChange->ChangeMask.EorBlobs)
			{
				ChangeCode Chge = pObjChange->RenderCurrentBlobs(pOrigPath,this,pEditPath,StartSpread,TRUE);
				if (Chge!=CC_OK)
					return;
				// Create a local copy of the edit path
				if (!TempPath.Initialise(pEditPath->GetNumCoords(), 12))
					return;
				TempPath.CopyPathDataFrom(pEditPath);
			}

			// Rub out the old EORed version of the path
			if (*pDragStarted == TRUE)
			{
				RenderPathEditBlobs(pEditPath, /*pEditPath->GetBoundingRect(),*/ StartSpread );
			}
			else
			{
				*pDragStarted = TRUE;
//				pCurrentDragS->pNode = (Node*) pDragStarted;
			}

			DocRect PointerBlobRect;
			GetApplication()->GetBlobManager()->GetBlobRect(PointerPos,&PointerBlobRect);

			DocCoord* Coords = pEditPath->GetCoordArray();
//			PathFlags* Flags = pEditPath->GetFlagArray();
			PathVerb* Verbs = pEditPath->GetVerbArray();
			INT32 NumCoords = pEditPath->GetNumCoords();

			if (pCurrentOrig == (NodeListItem*) OriginalPaths.GetHead ())
			{
				// CGS:  all of the operations to do with snapping to grid, snapping to opposite
				// end point and snapping to another lines endpoint are relevant only for
				// that control point that the drag was initiated from ....

				// Here is where we do all the snapping to grids and endpoints
				// and constrain. First we check for snapping to line ends, then
				// we check constrain, then we check grid snapping

				EndSnapped = FALSE;			// TRUE if this point snaps to the other end
				SnapToAnother = FALSE;		// TRUE if this point snaps to another line end
				SnapToLineOrCurve = FALSE;	// No snapped other line

				// We might be snapping to an endpoint if the point we're dragging around is either
				// a moveto, or is the last point in the subpath. We will also only snap if there is
				// only one point selected on the path

				INT32 i;
				if (DragPoint != -1)
				{
					// Look to see if the selected point is a real endpoint
					if (Verbs[DragPoint] == PT_MOVETO)
					{
						// This is the start element of the path
						i = DragPoint;
						if (!pEditPath->IsSubPathClosed(i))
						{
							INT32 NextEndpoint = i;
							if (pEditPath->FindNextEndPoint(&NextEndpoint))
							{
								pEditPath->FindEndElOfSubPath(&i);		// i indexes to the end element

								// If this sub-path is one element long then closing it is not allowed
								if ((i != NextEndpoint) && PointerBlobRect.ContainsCoord(Coords[i]))
								{
									PointerPos.x = Coords[i].x + LastMousePos.x - Coords[DragPoint].x;
									PointerPos.y = Coords[i].y + LastMousePos.y - Coords[DragPoint].y;
									EndSnapped = TRUE;
								}
							}
						}
					}
					else if (DragPoint+1 == NumCoords || Verbs[DragPoint+1] == PT_MOVETO)
					{
						// This is the last element in the sub-path
						i = DragPoint;
						if (!pEditPath->IsSubPathClosed(i))
						{
							INT32 PrevEndpoint = i;
							if (pEditPath->FindPrevEndPoint(&PrevEndpoint))
							{
								pEditPath->FindStartOfSubPath(&i);			// i is the index of the start of the sub-path
					
								// If this sub-path is one element long then closing it is not allowed
								if ((i != PrevEndpoint) && PointerBlobRect.ContainsCoord(Coords[i]))
								{
									PointerPos.x = Coords[i].x + LastMousePos.x - Coords[DragPoint].x;
									PointerPos.y = Coords[i].y + LastMousePos.y - Coords[DragPoint].y;
									EndSnapped = TRUE;
								}
							}
						}
					}
				}
			
				// That detects snapping to the opposite endpoint, but what about snapping to another
				// endpoint altogether? Time to check for that eventuality
				if (!EndSnapped && DragPoint != -1 && (Verbs[DragPoint] == PT_MOVETO || DragPoint+1 == NumCoords || Verbs[DragPoint+1] == PT_MOVETO) )
				{
					// Get a snapshot of the selection
					SelRange* Selected = GetApplication()->FindSelection();

					// do something extra in here ....

					Node* pNode = Selected->FindFirst();
					INT32 SubIndex,SubEnd;
					while (pNode)
					{
	/////////////////////////////////////////////////////////////////////////////////////////////////					
						if ((pNode->FindParentSpread() == StartSpread) && 
							(pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodePath)) /*&&
					(pNode != OriginalPath)*/ )
						{
	/////////////////////////////////////////////////////////////////////////////////////////////////
							Path* ThisPath = &(((NodePath*)pNode)->InkPath);
							INT32 ThisNum = ThisPath->GetNumCoords();
							SubIndex = 0;
							SubEnd = 0;
							while (SubIndex<ThisNum)		
							{
								DocCoord tempStart,tempEnd;
								ThisPath->SetPathPosition(SubIndex);	// Set the path's internal pos index
								if (ThisPath->GetSubPathEnds(&tempStart, &tempEnd))
								{
									SubEnd = ThisPath->GetPathPosition();
									// Now SubIndex is the index of the first, and SubEnd is the index
									// of the last element (N.B. might be a curveto, in which case add 2)
									if (ThisPath->GetVerb() == PT_BEZIERTO)
										SubEnd+=2;

									BOOL ClosedPath = (ThisPath->GetVerbArray()[SubEnd] & PT_CLOSEFIGURE) ;

									// Now compare start and end with pointer position, but not if this
									// subpath is the currently dragging subpath
									if ( !ClosedPath &&
										 ( (pNode != OriginalPath) ||
										   ((pNode == OriginalPath) && (DragPoint != SubIndex) && (DragPoint != SubEnd)) ) )
									{
										if (PointerBlobRect.ContainsCoord(tempStart))
										{
											break;
										}
										if (PointerBlobRect.ContainsCoord(tempEnd))
										{
											SubIndex = SubEnd;
											break;
										}
									}
								}
								else
								{
									SubEnd = ThisPath->GetPathPosition();
								}
								SubIndex = SubEnd+1;		// Points at start of next subpath
															// or top of whole path
							}
							// if SubIndex < ThisNum we must have found a matching coord
							// so that's what we'll snap to
							if (SubIndex < ThisNum)
								break;				// Exit the outer while condition
						}
						pNode = Selected->FindNext(pNode);
					}
					// At this point, if pNode == NULL we haven't snapped to anything
					// otherwise pNode is the path we've snapped to, and SubIndex is the index
					// of the point we've snapped to
					if (pNode)
					{
						// if the snap to path is the path being edited check that it has the same number of points!
						if (((NodePath*)pNode == pOrigPath) && (pOrigPath->InkPath.GetNumCoords() != pEditPath->GetNumCoords()))
						{
//							PATRACE( _T("Attempted to join an edited path!\n"));
							// The problem is that if the OriginalPath has been edited by adding an endpoint then
							// SnapToIndex will be wrong as it counted along the original path, not the edited one....
						}
						else
						{
							SnapToAnother = TRUE;
							SnapToPath = (NodePath*)pNode;
							SnapToIndex = SubIndex;
							SnapToPath->InkPath.SetPathPosition(SnapToIndex);
							PointerPos.x = SnapToPath->InkPath.GetCoord().x + LastMousePos.x - Coords[DragPoint].x;
							PointerPos.y = SnapToPath->InkPath.GetCoord().y + LastMousePos.y - Coords[DragPoint].y;
						}
					}
				}

	////////////////////////////////////////////////////////////////////////////////////////////////

				// only do the following for the node that the drag was started from ....

				//if (pCurrentOrig == (NodeListItem*) OriginalPaths.GetHead ())
				//{
					// We don't allow closed paths consiting of two straight segments
					if (EndSnapped && NumCoords < 4)
						EndSnapped = FALSE;
						
					if ((EndSnapped || SnapToAnother) && (pMoveCursor != NULL))
						ChangeCursor(pCloseCursor);
					else
						ChangeCursor(pCrossHairCursor);
				//}
				
				// Now constrain the mouse point
				ERROR3IF(ConstrainPoint == DocCoord(-1,-1),"ConstrainPoint wasn't set");
				ERROR3IF(ConstrainPrevPoint == DocCoord(-1,-1),"ConstrainPrevPoint wasn't set");
				ERROR3IF(ConstrainNextPoint == DocCoord(-1,-1),"ConstrainNextPoint wasn't set");
				if (!EndSnapped && ClickMods.Constrain)
				{
					if (ClickMods.Adjust)
					{
						if (ClickMods.Alternative1 || ClickMods.Alternative2)
							DocView::ConstrainToAngle(ConstrainNextPoint, &PointerPos);
						else
							DocView::ConstrainToAngle(ConstrainPrevPoint, &PointerPos);
					}
					else
						DocView::ConstrainToAngle(ConstrainPoint, &PointerPos);
				}

				// Only snap to grid if we didn't snap to the end of a path
				if (!EndSnapped)
					DocView::SnapCurrent(StartSpread,&PointerPos);
			}

			// This is the bit where we go off and re-calculate the paths position,
			// based on how much the mouse has moved

			if (pCurrentOrig == (NodeListItem*) OriginalPaths.GetHead ())
			{
				Offset.x = PointerPos.x - LastMousePos.x;
				Offset.y = PointerPos.y - LastMousePos.y;
			}

			// need to consider the validness of EndSnapped and DragPoint as well ....
			// CSG says:  we MUST pass EndSnapped and DragPoint as passed in below,
			// since these two values are reserved for the path that the drag was iniated
			// on (i.e.  the head of our linked list).  If we don't do this, then we get
			// loads of nasty asserts and errors going off.
			RecalculatePaths (pEditPath, Offset, FALSE /*EndSnapped*/, -1 /*DragPoint*/);

			if (pObjChange->ChangeMask.EorBlobs)
			{
				ChangeCode Chge = pObjChange->RenderChangedBlobs(pOrigPath,this,pEditPath,StartSpread,TRUE);
				if (Chge!=CC_OK)
				{
					// replace the old edit path
					pEditPath->CopyPathDataFrom(&TempPath);
					RenderPathEditBlobs( pEditPath/*->GetBoundingRect()*/, StartSpread );
					return;
				}
			}

			// Update the Last Mouse Position
			LastMousePos = PointerPos;

			// Draw in the new version of the path
			RenderPathEditBlobs( pEditPath/*->GetBoundingRect()*/, StartSpread );

			// If there is just one selected point being dragged then we should show
			// its new position in the line tool infobar.
//			if (UpdatePoint != -1)
//				BROADCAST_TO_ALL(PathEditedMsg(pEditPath, pSpread, UpdatePoint));

/////////////////////////////////////////////////////////////////////////////////////////////////

			pCurrentOrig = (NodeListItem*) OriginalPaths.GetNext (pCurrentOrig);
			pCurrentEdit = (NodeListItem*) EditPaths.GetNext (pCurrentEdit);
			pCurrentChange = (NodeListItem*) ObjChanges.GetNext (pCurrentChange);
			pCurrentDragS = (NodeListItem*) PathsDragStarted.GetNext (pCurrentDragS);
		}
		
		SetStatusLineHelp();
	}
}



/********************************************************************************************

>	void OpNodePathEditBlob::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL Success, BOOL bSolidDrag)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickMods - the key modifiers being pressed
				Success - TRUE if the drag was terminated properly, FALSE if it
				was ended with the escape key being pressed
	Purpose:	This is called when a drag operation finishes.
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpNodePathEditBlob::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
									   Spread* pSpread, BOOL Success, BOOL bSolidDrag)
{
	if (!MultiplePaths)
	{
		if (DragStarted)
			RenderDraggingBlobs( EditPath.GetBoundingRect(), StartSpread );
		DestroyCursors();
		EndDrag();
		BeginSlowJob();
		ChangeCode Chge;									   

		BOOL NotFailed = Success;
		
		// DY, if we are editing a brushed nodepath then we need a couple of extra actions
		AttrBrushType* pAttrBrush = GetAppliedBrush();

		if (NotFailed)
		{
			if (!DragStarted)
			{
				INT32 i;
				if (OriginalPath->InkPath.FindNearestPoint(	PointerPos, 
														POINTFLAG_ENDPOINTS | 
														POINTFLAG_CONTROLPOINTS | 
														POINTFLAG_ENDSFIRST,
														&i)
				)
				{
					// Deselect all but the clicked endpoint
					OriginalPath->InkPath.RenderPathSelectedControlBlobs(StartSpread);
					OriginalPath->InkPath.ClearSubSelection();
					PathFlags* pFlags = OriginalPath->InkPath.GetFlagArray();
					pFlags[i].IsSelected = TRUE;
					OriginalPath->InkPath.EnsureSelection(TRUE);
					OriginalPath->InkPath.RenderPathSelectedControlBlobs(StartSpread);

					// Send a sel-changed message so tools update (hello jason  8-)
					GetApplication()->FindSelection()->Update();

					// Kill the op so we dont appear in the undo history
					FailAndExecute();
					End();
					return;
				}
			}
			else
			{
				NotFailed = DoStartSelOp(TRUE,TRUE);

				// Create and send a change message about this path edit
				if (NotFailed)
				{
					ObjChangeFlags cFlags;
					cFlags.TransformNode = TRUE;
					ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,OriginalPath,this);
					NotFailed = OriginalPath->AllowOp(&ObjChange, TRUE);
				}

	//			// Store the paths sub-selection state
	//			if (NotFailed)
	//				NotFailed = (StorePathSubSelStateAction::DoRecord(this, &UndoActions, &OriginalPath->InkPath) != AC_FAIL);

				// Check we are dealing with a node path. If not, we shouldn't optimise the redraw.
				// Really we should ask the node whether it can cope with optimised redraw. This means
				// whether its correct simply to redraw the changed rectangular section around the moved
				// coordinate. For envelope and perspective moulds whose shapes are govened by derived
				// path objects its not correct to redraw optimally as this can result in sections of the
				// object being left undrawn when the whole surface / contents have changed.
				BOOL OptimiseRedraw = CopyNeedToRenderFlags() && !(EndSnapped || SnapToAnother);
				if (!IS_A(OriginalPath,NodePath))
					OptimiseRedraw = FALSE;
				
				// if we are editing a brush we don't want to optimise either, as it redraws incorrectly
				if (pAttrBrush != NULL)
					OptimiseRedraw = FALSE;
				// We had better copy the path back over the original and re-calc the bounding box
				// Force a re-draw of the place where the path used to be
				if (NotFailed)
					NotFailed = RecalcBoundsAction::DoRecalc(this, &UndoActions, OriginalPath, OptimiseRedraw) != AC_FAIL;


				//if we are editing a brushed nodepath we need to have this here for the undo to work
				if (pAttrBrush != NULL && pAttrBrush->IsTimeStamping() && NotFailed)
				{
					UpdateBrushAction* pAction;
					NotFailed = (UpdateBrushAction::Init(this, &UndoActions,  OriginalPath, &pAction) != AC_FAIL);
				}
				// If the EndSnapped flag is set, we have to set the PT_CLOSEFIGURE flag in the last
				// element of the copied path
				if (NotFailed && EndSnapped)
					SnapEndsTogether();
		
				// Go and copy the edited path back over the original path
				if (NotFailed)
					NotFailed = CopyEditedPathBack();

				// If the ends snapped, set the filled bit on the path
				if (NotFailed)
					NotFailed = FillPathIfEndsSnapped();
				if (NotFailed)
					NotFailed = JoinWithOtherPath();

				if (NotFailed)
				{
					// Recalculate the path's bounding box
					OriginalPath->InvalidateBoundingRect();
					SelRange *Sel = GetApplication()->FindSelection();
					Sel->UpdateBounds();

					// Expand the pasteboard as necessary to include the new object bounds
					DocRect NewBounds = Sel->GetBoundingRect(TRUE);
					ERROR3IF(pSpread == NULL, "Unexpectedly NULL spread pointer");
					pSpread->ExpandPasteboardToInclude(NewBounds);

					// Force a redraw of the place where the path is now.
					NotFailed = RecordBoundsAction::DoRecord(this, &UndoActions, OriginalPath, OptimiseRedraw) != AC_FAIL;
				}
			}
		}

		// DY 9/99 Check to see if we are editing a blend on a curve, if so we may wish to 
		// change the number of steps in the blend (making use of the path distance).
		NodeGroup* pParent = GetGroupParentOfCurve();
		
		if (pParent != NULL)
		{
			if (pParent->IS_KIND_OF(NodeBlend))
			{
				if (NotFailed)
					InsertChangeBlendStepsAction((NodeBlend*)pParent);               
			}
		
		}

	

		// Inform all the parents of this node that it has been changed.
		if (NotFailed)
		{
			ObjChangeFlags ChgeFlags;
			EditObjChange.Define(OBJCHANGE_FINISHED,ChgeFlags,OriginalPath,this,&EditPath,StartSpread);
			NotFailed = UpdateChangedNodes(&EditObjChange);
		}
		else
		{
			if (EditObjChange.ChangeMask.Finished)
				Chge=EditObjChange.ObjChangeFailed(OriginalPath,this,&EditPath,StartSpread,TRUE);
		}

		
		if (!NotFailed)	
			FailAndExecute();

		OriginalPaths.DeleteAll ();

		GetApplication()->UpdateSelection();
		End();
	}
	else
	{
		NodeListItem* pCurrentOrig = (NodeListItem*) OriginalPaths.GetHead ();
		NodeListItem* pCurrentEdit = (NodeListItem*) EditPaths.GetHead ();
		NodeListItem* pCurrentChange = (NodeListItem*) ObjChanges.GetHead ();
		NodeListItem* pCurrentDragS = (NodeListItem*) PathsDragStarted.GetHead ();

		DocCoord Offset;

		DestroyCursors();
		EndDrag();
		BeginSlowJob();
		ChangeCode Chge;		   

		BOOL NotFailed = Success;
		
		// DY, if we are editing a brushed nodepath then we need a couple of extra actions
		AttrBrushType* pAttrBrush = GetAppliedBrush();

		if (NotFailed)
			NotFailed = DoStartSelOp(TRUE,TRUE);

		while ((pCurrentOrig) && (pCurrentEdit) && (pCurrentChange) && (pCurrentDragS))
		{
			NodePath* pOrigPath = (NodePath*) (pCurrentOrig->pNode);
			Path* pEditPath = (Path*) (pCurrentEdit->pNode);
//			ObjChangePathEdit* pObjChange = (ObjChangePathEdit*) (pCurrentChange->pNode);
			BOOL* pDragStarted = (BOOL*) (pCurrentDragS->pNode);

			if (*pDragStarted == TRUE)
			{
				RenderDraggingBlobs( pEditPath/*EditPath.GetBoundingRect()*/, StartSpread );
			}

			if (NotFailed)
			{
				if (*pDragStarted == FALSE)
				{
					INT32 i;
					if (pOrigPath->InkPath.FindNearestPoint(PointerPos, 
															POINTFLAG_ENDPOINTS | 
															POINTFLAG_CONTROLPOINTS | 
															POINTFLAG_ENDSFIRST,
															&i)
					)
					{
						// Deselect all but the clicked endpoint
						pOrigPath->InkPath.RenderPathSelectedControlBlobs(StartSpread);
						pOrigPath->InkPath.ClearSubSelection();
						PathFlags* pFlags = pOrigPath->InkPath.GetFlagArray();
						pFlags[i].IsSelected = TRUE;
						pOrigPath->InkPath.EnsureSelection(TRUE);
						pOrigPath->InkPath.RenderPathSelectedControlBlobs(StartSpread);

						// Send a sel-changed message so tools update (hello jason  8-)
						GetApplication()->FindSelection()->Update();

						// Kill the op so we dont appear in the undo history
						FailAndExecute();
						End();
						return;
					}
				}
				else
				{
					//NotFailed = DoStartSelOp(TRUE,TRUE);

					// Create and send a change message about this path edit
					if (NotFailed)
					{
						ObjChangeFlags cFlags;
						cFlags.TransformNode = TRUE;
						ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,pOrigPath,this);
						NotFailed = pOrigPath->AllowOp(&ObjChange, TRUE);
					}

		//			// Store the paths sub-selection state
		//			if (NotFailed)
		//				NotFailed = (StorePathSubSelStateAction::DoRecord(this, &UndoActions, &OriginalPath->InkPath) != AC_FAIL);

					// Check we are dealing with a node path. If not, we shouldn't optimise the redraw.
					// Really we should ask the node whether it can cope with optimised redraw. This means
					// whether its correct simply to redraw the changed rectangular section around the moved
					// coordinate. For envelope and perspective moulds whose shapes are govened by derived
					// path objects its not correct to redraw optimally as this can result in sections of the
					// object being left undrawn when the whole surface / contents have changed.
					BOOL OptimiseRedraw = CopyNeedToRenderFlags(pOrigPath, pEditPath) && !(EndSnapped || SnapToAnother);
					if (!IS_A(pOrigPath,NodePath))
						OptimiseRedraw = FALSE;
					
					// if we are editing a brush we don't want to optimise either, as it redraws incorrectly
					if (pAttrBrush != NULL)
						OptimiseRedraw = FALSE;

					// We had better copy the path back over the original and re-calc the bounding box
					// Force a re-draw of the place where the path used to be
					if (NotFailed)
						NotFailed = RecalcBoundsAction::DoRecalc(this, &UndoActions, pOrigPath, OptimiseRedraw) != AC_FAIL;


					//if we are editing a brushed nodepath we need to have this here for the undo to work
					if (pAttrBrush != NULL && pAttrBrush->IsTimeStamping() && NotFailed)
					{
						UpdateBrushAction* pAction;
						NotFailed = (UpdateBrushAction::Init(this, &UndoActions,  pOrigPath, &pAction) != AC_FAIL);
					}
					
					if (pCurrentOrig == (NodeListItem*) OriginalPaths.GetHead ())
					{
						// If the EndSnapped flag is set, we have to set the PT_CLOSEFIGURE flag in the last
						// element of the copied path
						if (NotFailed && EndSnapped)
						{	
							SnapEndsTogether (pEditPath);
						}
					}
				
					// Go and copy the edited path back over the original path
					if (NotFailed)
						NotFailed = CopyEditedPathBack(pOrigPath, pEditPath);

					if (pCurrentOrig == (NodeListItem*) OriginalPaths.GetHead ())
					{
						// If the ends snapped, set the filled bit on the path
						if (NotFailed)
							NotFailed = FillPathIfEndsSnapped(pOrigPath);
					}

					// CGS:  BUT, we need to delay the snapping of a control point to another
					// paths until here.  These is because the path that we are snapping to
					// could have also of been dragged; and it is NOT guaranteed to have been
					// updated until now ....

					BOOL useHeadPath = FALSE;
					NodePath*	pHeadPath = NULL;
					NodePath**	hHeadPath = NULL;

					if (pOrigPath == SnapToPath)
					{
						if (NotFailed)
						{
							NodeListItem* pHeadOrig = (NodeListItem*) OriginalPaths.GetHead ();
							/*NodePath**/ pHeadPath = (NodePath*) (pHeadOrig->pNode);
							hHeadPath = &pHeadPath;

							NotFailed = JoinWithOtherPath(hHeadPath);
							
							if (NotFailed)
							{
								useHeadPath = TRUE;
							}
						}
					}

					if (NotFailed)
					{
						// Recalculate the path's bounding box
						pOrigPath->InvalidateBoundingRect();
						SelRange *Sel = GetApplication()->FindSelection();
						Sel->UpdateBounds();

						// Expand the pasteboard as necessary to include the new object bounds
						DocRect NewBounds = Sel->GetBoundingRect();
						ERROR3IF(pSpread == NULL, "Unexpectedly NULL spread pointer");
						pSpread->ExpandPasteboardToInclude(NewBounds);

						if (!useHeadPath)
						{
							// Force a redraw of the place where the path is now.
							NotFailed = RecordBoundsAction::DoRecord(this, &UndoActions, pOrigPath, OptimiseRedraw) != AC_FAIL;
						}
						else
						{
							//NodeListItem* pHeadOrig = (NodeListItem*) OriginalPaths.GetHead ();
							//NodePath* pHeadPath = (NodePath*) (pHeadOrig->pNode);
							
							// Force a redraw of the place where the path is now.
							NotFailed = RecordBoundsAction::DoRecord(this, &UndoActions, (*hHeadPath), OptimiseRedraw) != AC_FAIL;
						}
					}
				}
			}

			// DY 9/99 Check to see if we are editing a blend on a curve, if so we may wish to 
			// change the number of steps in the blend (making use of the path distance).
			NodeGroup* pParent = GetGroupParentOfCurve(pOrigPath);
			
			if (pParent != NULL)
			{
				if (pParent->IS_KIND_OF(NodeBlend))
				{
					if (NotFailed)
						InsertChangeBlendStepsAction((NodeBlend*)pParent);               
				}
			}

			// Inform all the parents of this node that it has been changed.
			if (NotFailed)
			{
				ObjChangeFlags ChgeFlags;
				EditObjChange.Define(OBJCHANGE_FINISHED,ChgeFlags,pOrigPath,this,pEditPath,StartSpread);
				NotFailed = UpdateChangedNodes(&EditObjChange);
			}
			else
			{
				if (EditObjChange.ChangeMask.Finished)
					Chge=EditObjChange.ObjChangeFailed(pOrigPath,this,pEditPath,StartSpread,TRUE);
			}

			pCurrentOrig = (NodeListItem*) OriginalPaths.GetNext (pCurrentOrig);
			pCurrentEdit = (NodeListItem*) EditPaths.GetNext (pCurrentEdit);
			pCurrentChange = (NodeListItem*) ObjChanges.GetNext (pCurrentChange);
			pCurrentDragS = (NodeListItem*) PathsDragStarted.GetNext (pCurrentDragS);
		}
		
		if (!NotFailed)	
			FailAndExecute();

		GetApplication()->UpdateSelection();

		// now we need to handle destruction ourselves ....

//		pCurrentOrig = (NodeListItem*) OriginalPaths.GetHead ();
		pCurrentEdit = (NodeListItem*) EditPaths.GetHead ();
		pCurrentChange = (NodeListItem*) ObjChanges.GetHead ();
		pCurrentDragS = (NodeListItem*) PathsDragStarted.GetHead ();

		while (/*(pCurrentOrig) &&*/ (pCurrentEdit) && (pCurrentChange) && (pCurrentDragS))
		{
//			NodePath* pOrigPath = (NodePath*) (pCurrentOrig->pNode);
			Path* pEditPath = (Path*) (pCurrentEdit->pNode);
			ObjChangePathEdit* pObjChange = (ObjChangePathEdit*) (pCurrentChange->pNode);
			BOOL* pDragStarted = (BOOL*) (pCurrentDragS->pNode);

	//		delete (pCurrentOrig->pNode);
			delete (pEditPath);
			delete (pObjChange);
			delete (pDragStarted);
			
//			pCurrentOrig = (NodeListItem*) OriginalPaths.GetNext (pCurrentOrig);
			pCurrentEdit = (NodeListItem*) EditPaths.GetNext (pCurrentEdit);
			pCurrentChange = (NodeListItem*) ObjChanges.GetNext (pCurrentChange);
			pCurrentDragS = (NodeListItem*) PathsDragStarted.GetNext (pCurrentDragS);
		}

		OriginalPaths.DeleteAll ();
		EditPaths.DeleteAll ();
		ObjChanges.DeleteAll ();
		PathsDragStarted.DeleteAll ();

		End();
	}
}


/********************************************************************************************

>	void OpNodePathEditBlob::InsertChangeBlendStepsAction(NodeBlend* pNodeBlend)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/99
	Inputs:		the node blend to change
	Outputs:	-
	Returns:	-
	Purpose:	To adjust the number of steps in a blend on a path as a result of the path
				being edited (which is why its here rather than in the blend code).
				First the number of steps is calculated from the new path distance and if
				different a new action is inserted
	Errors:		Error2 if pNodeBlend is null
	SeeAlso:	called from OpNodePathEditBlob::DragFinished or 
				OpNodePathAddEndpoint::CompleteThisPath

********************************************************************************************/
BOOL OpNodePathEditBlob::InsertChangeBlendStepsAction(NodeBlend* pNodeBlend)
{
	ERROR2IF(pNodeBlend==NULL, FALSE, "NodeBlend pointer is NULL");
	// do we wish to keep the distance between steps constant?  If so then determine
	// how many steps we need with the new length and make an action.
	if (pNodeBlend->GetEditState() == EDIT_DISTANCE)
	{
		UINT32 NewNumSteps = 0;
		UINT32 OldNumSteps = pNodeBlend->GetNumBlendSteps();
		double StepDistance = pNodeBlend->GetDistanceEntered();
		double NewPathLength = EditPath.GetPathLength();
		NewNumSteps = (UINT32)(NewPathLength/StepDistance);
		BOOL ok = TRUE;
		
	
		if (OldNumSteps != NewNumSteps)
		{
			ChangeBlendStepsAction* pAction;
			if (ok)	ok = (InvalidateBoundsAction::Init(this,&UndoActions,pNodeBlend,TRUE) != AC_FAIL);
			if (ok) ok = ChangeBlendStepsAction::Init(this,&UndoActions,pNodeBlend,OldNumSteps,StepDistance,&pAction) != AC_FAIL;
			if (ok) ok = DoInvalidateNodeRegion(pNodeBlend,TRUE,FALSE);
			if (ok)	ok = (InvalidateBoundsAction::Init(this,&UndoActions,pNodeBlend,TRUE) != AC_FAIL);
			pNodeBlend->SetNumBlendSteps(NewNumSteps);
		}

		// added 9/9/99 now we are shifting the last object along the curve to ensure precise
		// step distances.  To do this we must set the proportion of the curve used in the 
		// NodeBlender objects.
		if (ok)
		{
			double PathDistanceUsed = NewNumSteps * StepDistance;
			double PathProportion = PathDistanceUsed / NewPathLength;
//			double PathDistanceUnused = NewPathLength - PathDistanceUsed;

			if (PathProportion != 1.0)
			{
				ChangeBlenderOpParam BlenderParam;
				BlenderParam.m_ChangeType = CHANGEBLENDER_PATHEND;
							
				NodeBlender* pNodeBlender = pNodeBlend->FindFirstBlender();
				INT32 NumBlenders = pNodeBlend->GetNumBlenders();
						
				while (pNodeBlender != NULL)
				{
					NumBlenders--;
					if (NumBlenders ==0)
					{									
						BlenderParam.m_NewPathEnd = PathProportion;
						ok = ChangeBlenderAction::Init(this, &UndoActions, pNodeBlender, BlenderParam);
						if (ok)
						{
							DocCoord NewPoint;
							double ExtraParam = 0.0;  //passed to the function but not used afterwards
							ok = pNodeBlender->GetPointOnNodeBlendPath(1.0,&NewPoint,&ExtraParam);
							if (ok)
							{
								NodeRenderableInk* pEnd = pNodeBlender->GetNodeEnd();
								NodeBlend* pNodeBlend = pNodeBlender->GetNodeBlend();
								ok = ((pEnd != NULL) && (pNodeBlend != NULL));
					
								if (ok) 
									ok = pNodeBlend->TransformNodeToPoint(pEnd,&NewPoint,this,ExtraParam);
							}
						}
					}	
					pNodeBlender = pNodeBlend->FindNextBlender(pNodeBlender);
				}
			} // end if (pathproportion..
		} // end if(ok)
	}
	return TRUE;
}

/********************************************************************************************

>	void OpNodePathEditBlob::SnapEndsTogether()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets the closefigure flag in the last element in the subpath. Also turns
				off the rotate bit in the points we're snapping together if their
				smoothing bits are also turned off. This prevents us creating a cusp that
				has rotate flags set.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpNodePathEditBlob::SnapEndsTogether()
{
	// If ends are snapping, there can only be one selected point on the path...
	// Get a pointer to the path's verbs
	PathVerb* Verbs = EditPath.GetVerbArray();
	PathFlags* Flags = EditPath.GetFlagArray();
	INT32 NumCoords = EditPath.GetNumCoords();

	// Find the selected point
	INT32				pos;
	for( pos = 0; pos < NumCoords && !( Flags[pos].IsEndPoint && Flags[pos].IsSelected); pos++ );

	// Now pos is the index of the selected point

	// This next may seem like a lot of faffing around, but we don't know whether the 
	// selected point is the start or the end of the path, so this code doesn't care
	// (thus it's not very optimal, but it shouldn't matter).
	
	// Move to the start of the edit path
	INT32				startpos = pos;
	EditPath.FindStartOfSubPath(&startpos);
	// find the end element (might be a curve or a line)
	pos=startpos;
	EditPath.FindEndOfSubPath(&pos);

	// if the last element is a bezier, pos points at the first control point
	// so we have to add 2 to move to the endpoint of the curve
	if (Verbs[pos] == PT_BEZIERTO)
		pos+=2;
	
	// Now simply set the PT_CLOSEFIGURE flag in this verb
	Verbs[pos] |= PT_CLOSEFIGURE;
	
	// make sure the first and last points are selected, along with adjacent ctrl points
	Flags[pos].IsSelected = TRUE;
	// if it's a bezier, select previous control point and check state of smooth/rotate bits
	if ((Verbs[pos] & ~PT_CLOSEFIGURE) == PT_BEZIERTO)
	{
		Flags[pos-1].IsSelected = TRUE;
		
		// if the smooth flag is not set, clear the rotate flag as well, otherwise
		// We'll have something that looks like a cusp, but as soon as you drag a control
		// point, the opposite point will rotate around.
		if (!Flags[pos-1].IsSmooth)
			Flags[pos-1].IsRotate = FALSE;

		// Do the same for the endpoint...
		if (!Flags[pos].IsSmooth)
			Flags[pos].IsRotate = FALSE;
	}
	pos = startpos;
	Flags[pos].IsSelected = TRUE;
	if (Verbs[pos+1] == PT_BEZIERTO)
	{
		Flags[pos+1].IsSelected = TRUE;

		// As above, clear the rotate bit if the smooth bit is clear
		if (!Flags[pos+1].IsSmooth)
			Flags[pos+1].IsRotate = FALSE;
		// Do the same for the endpoint...
		if (!Flags[pos].IsSmooth)
			Flags[pos].IsRotate = FALSE;
	}
}

/********************************************************************************************

>	void OpNodePathEditBlob::SnapEndsTogether(Path* pEditPath)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> (based upon Jim)
	Created:	17/5/00
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets the closefigure flag in the last element in the subpath. Also turns
				off the rotate bit in the points we're snapping together if their
				smoothing bits are also turned off. This prevents us creating a cusp that
				has rotate flags set.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpNodePathEditBlob::SnapEndsTogether(Path* pEditPath)
{
	// If ends are snapping, there can only be one selected point on the path...
	// Get a pointer to the path's verbs
	PathVerb* Verbs = pEditPath->GetVerbArray();
	PathFlags* Flags = pEditPath->GetFlagArray();
	INT32 NumCoords = pEditPath->GetNumCoords();

	// Find the selected point
	INT32				pos;
	for ( pos = 0;pos<NumCoords && !(Flags[pos].IsEndPoint && Flags[pos].IsSelected);pos++);

	// Now pos is the index of the selected point

	// This next may seem like a lot of faffing around, but we don't know whether the 
	// selected point is the start or the end of the path, so this code doesn't care
	// (thus it's not very optimal, but it shouldn't matter).
	
	// Move to the start of the edit path
	INT32 startpos = pos;
	pEditPath->FindStartOfSubPath(&startpos);
	// find the end element (might be a curve or a line)
	pos=startpos;
	pEditPath->FindEndOfSubPath(&pos);

	// if the last element is a bezier, pos points at the first control point
	// so we have to add 2 to move to the endpoint of the curve
	if (Verbs[pos] == PT_BEZIERTO)
		pos+=2;
	
	// Now simply set the PT_CLOSEFIGURE flag in this verb
	Verbs[pos] |= PT_CLOSEFIGURE;
	
	// make sure the first and last points are selected, along with adjacent ctrl points
	Flags[pos].IsSelected = TRUE;
	// if it's a bezier, select previous control point and check state of smooth/rotate bits
	if ((Verbs[pos] & ~PT_CLOSEFIGURE) == PT_BEZIERTO)
	{
		Flags[pos-1].IsSelected = TRUE;
		
		// if the smooth flag is not set, clear the rotate flag as well, otherwise
		// We'll have something that looks like a cusp, but as soon as you drag a control
		// point, the opposite point will rotate around.
		if (!Flags[pos-1].IsSmooth)
			Flags[pos-1].IsRotate = FALSE;

		// Do the same for the endpoint...
		if (!Flags[pos].IsSmooth)
			Flags[pos].IsRotate = FALSE;
	}
	pos = startpos;
	Flags[pos].IsSelected = TRUE;
	if (Verbs[pos+1] == PT_BEZIERTO)
	{
		Flags[pos+1].IsSelected = TRUE;

		// As above, clear the rotate bit if the smooth bit is clear
		if (!Flags[pos+1].IsSmooth)
			Flags[pos+1].IsRotate = FALSE;
		// Do the same for the endpoint...
		if (!Flags[pos].IsSmooth)
			Flags[pos].IsRotate = FALSE;
	}
}

/********************************************************************************************

>	BOOL OpNodePathEditBlob::JoinWithOtherPath()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/94
	Returns:	FALSE if it failed (lack of memory maybe) TRUE otherwise
	Purpose:	Looks at the member variables dealing with snapping to another path and joins
				the paths together. It will always join the other path to the original path, 
				keeping the original path's attributes intact.
				NOTE: This routine will alter the OriginalPath member variable if it joins paths, 
				because it has to make a copy of the path for undo purposes. 
	Errors:		If it runs out of memory then it will return FALSE

********************************************************************************************/

BOOL OpNodePathEditBlob::JoinWithOtherPath()
{
	// If SnapToAnother is false there's no snapping to be done, so just return successfully
	if (!SnapToAnother)
		return TRUE;

	// If we're joining with another path, the rules say we can only have one selected point.
	PathFlags* Flags = OriginalPath->InkPath.GetFlagArray();
	INT32 NumCoords = OriginalPath->InkPath.GetNumCoords();

	NodePath* OldSnapPath = SnapToPath;
	
	INT32 MainIndex;
	for (MainIndex=0; MainIndex<NumCoords; MainIndex++)
	{
		if (Flags[MainIndex].IsEndPoint && Flags[MainIndex].IsSelected)
			break;
	}

	// on exit, MainIndex is the selected endpoint, or = NumCoords

	ENSURE(MainIndex<NumCoords,"There isn't a selected endpoint in the original path (JoinWithAnotherPath)");
	
	// We're joining to another path, so the easiest way to handle undo is to make a deep copy
	// of the original path, then join that with the othe path, put it back into the tree
	// and delete the old one. Simple.
	Node* pnp;
	OriginalPath->NodeCopy(&pnp);	// Make a deep copy of the original path, with attributes
	NodePath* MainPath = (NodePath*)pnp;

	if (!MainPath)
		return FALSE;

	// Now that we have a copy of this path, let's join it with the other path

	if (SnapToAnother)
	{
		// If we're snapping to a subpath in the same path, change SnapToPath because we're
		// no longer joining to OriginalPath.
		if (SnapToPath == OriginalPath)
			SnapToPath = MainPath;
		
		// If we're snapping to another, the first thing I have to do is change the control
		// point of the point in the other path that we're snapping to, because it might
		// have been smoothed.

		if (SnapToLineOrCurve)		// TRUE if the path has been smoothed
		{
			PathVerb* OtherVerbs = SnapToPath->InkPath.GetVerbArray();
			PathFlags* OtherFlags = SnapToPath->InkPath.GetFlagArray();
			DocCoord* OtherCoords = SnapToPath->InkPath.GetCoordArray();

			if (OtherVerbs[SnapToIndex] == PT_MOVETO)
			{
				// Don't need to undo if SnapToPath == MainPath
				if (SnapToPath == MainPath)
					OtherCoords[SnapToIndex+1] = SnapToCoords[1];
				else
					DoAlterPathElement(	SnapToPath, 
										SnapToIndex+1, 
										SnapToCoords[1], 
										OtherFlags[SnapToIndex+1],
										OtherVerbs[SnapToIndex+1]);
			}
			else
			{
				if (SnapToPath == MainPath)
					OtherCoords[SnapToIndex-1] = SnapToCoords[1];
				else
					DoAlterPathElement(	SnapToPath, 
										SnapToIndex-1, 
										SnapToCoords[1], 
										OtherFlags[SnapToIndex-1],
										OtherVerbs[SnapToIndex-1]);
			}
		}

		if (!MainPath->InkPath.JoinToAnother(&(SnapToPath->InkPath), MainIndex, SnapToIndex))
		{
			MainPath->CascadeDelete();
			delete MainPath;
			return FALSE;
		}
	}

	// We've joined successfully, so let's insert this new node into the tree next to the original
	if (!DoInsertNewNode(MainPath, OriginalPath, NEXT, TRUE, FALSE))
	{
		MainPath->CascadeDelete();
		delete MainPath;
		return FALSE;
	}

	// Hide the original path
	if (!DoHideNode(OriginalPath, TRUE))
		return FALSE;

	// Hide the node we joined to
	if (OldSnapPath != OriginalPath)
	{
		if (!DoHideNode(OldSnapPath, TRUE))
			return FALSE;
	}

	// Change the OriginalPath member variable so that subsequent operations work properly
	OriginalPath = MainPath;

	return TRUE;
}



/********************************************************************************************

>	BOOL OpNodePathEditBlob::JoinWithOtherPath(NodePath** pOrigPath)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> (based upon Jim)
	Created:	17/5/00
	Returns:	FALSE if it failed (lack of memory maybe) TRUE otherwise
	Purpose:	Looks at the member variables dealing with snapping to another path and joins
				the paths together. It will always join the other path to the original path, 
				keeping the original path's attributes intact.
				NOTE: This routine will alter the OriginalPath member variable if it joins paths, 
				because it has to make a copy of the path for undo purposes. 
	Errors:		If it runs out of memory then it will return FALSE

********************************************************************************************/

BOOL OpNodePathEditBlob::JoinWithOtherPath(NodePath** pOrigPath)
{
	// If SnapToAnother is false there's no snapping to be done, so just return successfully
	if (!SnapToAnother)
		return TRUE;

	// If we're joining with another path, the rules say we can only have one selected point.
	PathFlags* Flags = (*pOrigPath)->InkPath.GetFlagArray();
	INT32 NumCoords = (*pOrigPath)->InkPath.GetNumCoords();

	NodePath* OldSnapPath = SnapToPath;
	
	INT32 MainIndex;
	for (MainIndex=0; MainIndex<NumCoords; MainIndex++)
	{
		if (Flags[MainIndex].IsEndPoint && Flags[MainIndex].IsSelected)
			break;
	}

	// on exit, MainIndex is the selected endpoint, or = NumCoords

	ENSURE(MainIndex<NumCoords,"There isn't a selected endpoint in the original path (JoinWithAnotherPath)");
	
	// We're joining to another path, so the easiest way to handle undo is to make a deep copy
	// of the original path, then join that with the othe path, put it back into the tree
	// and delete the old one. Simple.
	Node* pnp;
	(*pOrigPath)->NodeCopy(&pnp);	// Make a deep copy of the original path, with attributes
	NodePath* MainPath = (NodePath*)pnp;

	if (!MainPath)
		return FALSE;

	// Now that we have a copy of this path, let's join it with the other path

	if (SnapToAnother)
	{
		// If we're snapping to a subpath in the same path, change SnapToPath because we're
		// no longer joining to OriginalPath.
		if (SnapToPath == (*pOrigPath))
			SnapToPath = MainPath;
		
		// If we're snapping to another, the first thing I have to do is change the control
		// point of the point in the other path that we're snapping to, because it might
		// have been smoothed.

		if (SnapToLineOrCurve)		// TRUE if the path has been smoothed
		{
			PathVerb* OtherVerbs = SnapToPath->InkPath.GetVerbArray();
			PathFlags* OtherFlags = SnapToPath->InkPath.GetFlagArray();
			DocCoord* OtherCoords = SnapToPath->InkPath.GetCoordArray();

			if (OtherVerbs[SnapToIndex] == PT_MOVETO)
			{
				// Don't need to undo if SnapToPath == MainPath
				if (SnapToPath == MainPath)
					OtherCoords[SnapToIndex+1] = SnapToCoords[1];
				else
					DoAlterPathElement(	SnapToPath, 
										SnapToIndex+1, 
										SnapToCoords[1], 
										OtherFlags[SnapToIndex+1],
										OtherVerbs[SnapToIndex+1]);
			}
			else
			{
				if (SnapToPath == MainPath)
					OtherCoords[SnapToIndex-1] = SnapToCoords[1];
				else
					DoAlterPathElement(	SnapToPath, 
										SnapToIndex-1, 
										SnapToCoords[1], 
										OtherFlags[SnapToIndex-1],
										OtherVerbs[SnapToIndex-1]);
			}
		}

		if (!MainPath->InkPath.JoinToAnother(&(SnapToPath->InkPath), MainIndex, SnapToIndex))
		{
			MainPath->CascadeDelete();
			delete MainPath;
			return FALSE;
		}
	}

	// We've joined successfully, so let's insert this new node into the tree next to the original
	if (!DoInsertNewNode(MainPath, (*pOrigPath), NEXT, TRUE, FALSE))
	{
		MainPath->CascadeDelete();
		delete MainPath;
		return FALSE;
	}

	// Hide the original path
	if (!DoHideNode((*pOrigPath), TRUE))
		return FALSE;

	// Hide the node we joined to
	if (OldSnapPath != (*pOrigPath))
	{
		if (!DoHideNode(OldSnapPath, TRUE))
			return FALSE;
	}

	// Change the pOrigPath variable so that subsequent operations work properly
	(*pOrigPath) = MainPath;

	return TRUE;
}



/********************************************************************************************
>	BOOL OpNodePathEditBlob::BuildEditPaths ()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> (based upon Rik/Peter)
	Created:	17/02/94
	Returns:	TRUE if it managed to build the paths, FALSE if it failed
	Purpose:	Builds a copy of each path in the selection that we can edit, without
				destroying the original data.  Also sets the NeedToRender flags for EOR display.
	Errors:		If it runs out of memory then it will return FALSE
********************************************************************************************/

BOOL OpNodePathEditBlob::BuildEditPaths ()
{
	NodeListItem* pCurrent = (NodeListItem*) OriginalPaths.GetHead ();

	while (pCurrent)
	{
		NodePath* pCurrentPath = (NodePath*) (pCurrent->pNode);
		Path* NewEditPath = new Path ();

		UINT32 NumCoords = pCurrentPath->InkPath.GetNumCoords();

		if (!NewEditPath->Initialise(NumCoords, 24))
			return FALSE;
		if (!NewEditPath->CopyPathDataFrom(&(pCurrentPath->InkPath)))
			return FALSE;

		// Go though all the coords, with scary amounts of looking back and forwards
		UINT32 LastEndPoint = 0;				// The EndPoint before this one
		BOOL SetNextEndPoint = FALSE;		// TRUE if we want to set the next EndPoint to render
		BOOL SetNextNextEndPoint = FALSE;	// TRUE if we want the one after the next one to render
		PathFlags* Flags = NewEditPath->GetFlagArray();
		
		for (UINT32 i=0; i<NumCoords; i++)
		{
			// Make all the flags FALSE by default
			Flags[i].NeedToRender = FALSE;

			if (Flags[i].IsEndPoint)
			{
				// if the endpoint 2 elements back was selected and the last element was smooth
				// then we need to mark this point for rendering
				if (SetNextNextEndPoint)
				{
					Flags[i].NeedToRender = TRUE;
					SetNextNextEndPoint = FALSE;
				}

				// We have found an Endpoint, do we want to mark this one as renderable
				if (SetNextEndPoint)
				{
					// As the last element was selected, this element needs to render
					Flags[i].NeedToRender = TRUE;
					SetNextEndPoint = FALSE;

					// If the smooth flag is set then the next item needs to render as well
					if (Flags[i].IsRotate || Flags[i].IsSmooth)
						SetNextNextEndPoint = TRUE;
				}

				// If its selected, then its renderable
				if (Flags[i].IsSelected)
				{
					Flags[i].NeedToRender = TRUE;
					if (Flags[LastEndPoint].IsRotate || Flags[LastEndPoint].IsSmooth)
						Flags[LastEndPoint].NeedToRender = TRUE;

					// Set the flag for the next endpoint
					SetNextEndPoint = TRUE;
				}

				LastEndPoint = i;
			}
		}

		NodeListItem* pInsert = new NodeListItem;
		pInsert->pNode = (Node*) NewEditPath;
		
		EditPaths.AddTail (pInsert);//NewEditPath);
		
		pCurrent = (NodeListItem*) OriginalPaths.GetNext (pCurrent);

		//delete (pInsert);

//		delete (NewEditPath);				// cleanup that is necessary
	}
	
	return (TRUE);
}


/********************************************************************************************
>	BOOL OpNodePathEditBlob::BuildEditPath()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> - later attacked by Peter
	Created:	17/02/94
	Returns:	TRUE if it managed to build the path, FALSE if it failed
	Purpose:	Builds a copy of the path that we can edit, without destroying the original
				data.  Also sets the NeedToRender flags for EOR display.
	Errors:		If it runs out of memory then it will return FALSE
********************************************************************************************/
BOOL OpNodePathEditBlob::BuildEditPath()
{
	// Make a copy of the original path
	UINT32 NumCoords = OriginalPath->InkPath.GetNumCoords();
	if (!EditPath.Initialise(NumCoords, 24))
		return FALSE;
	if (!EditPath.CopyPathDataFrom(&(OriginalPath->InkPath)))
		return FALSE;

	// Go though all the coords, with scary amounts of looking back and forwards
	UINT32 LastEndPoint = 0;				// The EndPoint before this one
	BOOL SetNextEndPoint = FALSE;		// TRUE if we want to set the next EndPoint to render
	BOOL SetNextNextEndPoint = FALSE;	// TRUE if we want the one after the next one to render
	PathFlags* Flags = EditPath.GetFlagArray();
	for (UINT32 i=0; i<NumCoords; i++)
	{
		// Make all the flags FALSE by default
		Flags[i].NeedToRender = FALSE;

		if (Flags[i].IsEndPoint)
		{
			// if the endpoint 2 elements back was selected and the last element was smooth
			// then we need to mark this point for rendering
			if (SetNextNextEndPoint)
			{
				Flags[i].NeedToRender = TRUE;
				SetNextNextEndPoint = FALSE;
			}

			// We have found an Endpoint, do we want to mark this one as renderable
			if (SetNextEndPoint)
			{
				// As the last element was selected, this element needs to render
				Flags[i].NeedToRender = TRUE;
				SetNextEndPoint = FALSE;

				// If the smooth flag is set then the next item needs to render as well
				if (Flags[i].IsRotate || Flags[i].IsSmooth)
					SetNextNextEndPoint = TRUE;
			}

			// If its selected, then its renderable
			if (Flags[i].IsSelected)
			{
				Flags[i].NeedToRender = TRUE;
				if (Flags[LastEndPoint].IsRotate || Flags[LastEndPoint].IsSmooth)
					Flags[LastEndPoint].NeedToRender = TRUE;

				// Set the flag for the next endpoint
				SetNextEndPoint = TRUE;
			}

			LastEndPoint = i;
		}
	}

	return TRUE;
}



/********************************************************************************************

>	BOOL OpNodePathEditBlob::CopyEditedPathBack()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/02/94
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Copies the contents of the edited path back into the original path

********************************************************************************************/

BOOL OpNodePathEditBlob::CopyEditedPathBack()
{
	// Now to do some undo information. To do this, I have to look at each element in the path
	// and each element in the copy, and see which ones differ. I then have to work out how many
	// elements that was, and create a path to contain those elements, along with an array of indices 
	// telling me where those elements came from
	INT32 NumElements = OriginalPath->InkPath.GetNumCoords();
	PathVerb* SourceVerbs = OriginalPath->InkPath.GetVerbArray();
	DocCoord* SourceCoords = OriginalPath->InkPath.GetCoordArray();
	PathFlags* SourceFlags = OriginalPath->InkPath.GetFlagArray();
	PathVerb* DestVerbs = EditPath.GetVerbArray();
	DocCoord* DestCoords = EditPath.GetCoordArray();
	PathFlags* DestFlags = EditPath.GetFlagArray();

	// DY now we want to keep track of the indexes of the changed section
	INT32 ChangedElements = 0;
	INT32 FirstChanged = -1;
	INT32 LastChanged  = -1;

	INT32 i;
	for (i=0;i<NumElements;i++)
	{
		if (SourceVerbs[i] != DestVerbs[i] || SourceCoords[i] != DestCoords[i] || 
			SourceFlags[i] != DestFlags[i] )
		{
			ChangedElements++;

		//	if (SourceCoords[i] != DestCoords[i])
			{
				// if we have not yet set the first changed index then set it
				if (FirstChanged == -1)
					FirstChanged = i;

				// always set the last changed index
				LastChanged = i;
			}
		}

	}
	

	// ChangedElements is the number of elements in this path that will change.
	// We have to create three arrays to contain the changed elements, plus one array
	// to tell me where the elements should go (the indices)

	// I also have to create an action object to contain these arrays. I have to create the action
	// object first because that does all the work of deciding if there's enough memory in the 
	// undo buffer to store the action, and prompting the user accordingly of there isn't

	if (ChangedElements > 0)
	{
		// do the brush editing here
		// the following block deals with editing path blobs.  For some reason I could not get a
		// sensible value for the distance of the edited path from the changed indexes, however
		// it works quite nicely by finding the contraining points.
		if (DragPoint != -1)
		{
			// this section only applies for editing blobs (as reshaping has no drag point)
			FirstChanged = DragPoint;
			LastChanged = DragPoint;
			OriginalPath->InkPath.FindPrevEndPoint(&FirstChanged);
			OriginalPath->InkPath.FindNextEndPoint(&LastChanged);
		}
		
		AttrBrushType* pAttrBrush = GetAppliedBrush();
		if (pAttrBrush != NULL)
			EditBrushAttribute(FirstChanged, LastChanged, pAttrBrush);

		ModifyPathAction* ModAction;
	
		ActionCode Act;
		Act = ModifyPathAction::Init(this, &UndoActions, ChangedElements, (Action**)(&ModAction));
		if (Act == AC_FAIL)
		{
			FailAndExecute();
			End();
			return FALSE;
		}

		PathVerb* ChangedVerbs=NULL;
		DocCoord* ChangedCoords=NULL;
		PathFlags* ChangedFlags=NULL;
		INT32* ChangedIndices=NULL;

		// If the function returned AC_NO_RECORD we shouldn't record any undo information in the action
		// NOTE - during unwind all actions return AC_OK with a NULL ModAction pointer!!
		if ((Act!=AC_NORECORD) && (ModAction!=NULL))
		{
			// This next bit is a bit hellish. Any one of these four allocations can fail, in which case 
			// we have to tidy up afterwards. Cue a lot of nested ifs and elses.

			ALLOC_WITH_FAIL(ChangedVerbs,(PathVerb*) CCMalloc(ChangedElements * sizeof(PathVerb)),this);
			if (ChangedVerbs)
			{
				ALLOC_WITH_FAIL(ChangedCoords,(DocCoord*) CCMalloc(ChangedElements * sizeof(DocCoord)),this);
				if (ChangedCoords)
				{
					ALLOC_WITH_FAIL(ChangedFlags,(PathFlags*) CCMalloc(ChangedElements * sizeof(PathFlags)),this);
					if (ChangedFlags)
					{
						ALLOC_WITH_FAIL(ChangedIndices,(INT32*) CCMalloc(ChangedElements * sizeof(INT32)),this);
						if (!ChangedIndices)
						{
							CCFree( ChangedFlags );
							CCFree( ChangedCoords );
							CCFree( ChangedVerbs);
							FailAndExecute();
							End();
							return FALSE;
						}
					}
					else
					{
						CCFree( ChangedCoords );
						CCFree( ChangedVerbs );
						FailAndExecute();
						End();
						return FALSE;

					}
				}
				else
				{
					CCFree( ChangedVerbs);
					FailAndExecute();
					End();
					return FALSE;

				}
			}

			// Now to put the undo data into the undo action
			INT32 index = 0;
			for (i=0;i<NumElements;i++)
			{
				if (SourceVerbs[i] != DestVerbs[i] || 
					SourceCoords[i] != DestCoords[i] || 
					SourceFlags[i] != DestFlags[i]
					)
				{
					ChangedVerbs[index] = SourceVerbs[i];
					ChangedFlags[index] = SourceFlags[i];
					ChangedCoords[index] = SourceCoords[i];
					ChangedIndices[index] = i;
					index++;
				}
			}

			// Now we've allocated the arrays, let's tell the action about 'em
			ModAction->StoreArrays(ChangedVerbs, ChangedFlags, ChangedCoords, ChangedIndices, OriginalPath);
		}
	}
		
	if (!OriginalPath->InkPath.CopyPathDataFrom(&EditPath))
		return FALSE;
	else
		return TRUE;
}



/********************************************************************************************

>	BOOL OpNodePathEditBlob::CopyEditedPathBack(NodePath* pOrigPath, Path* pEditPath)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> (based upon Will/Peter)
	Created:	20/4/2000
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Copies the contents of the edited path back into the original path

********************************************************************************************/

BOOL OpNodePathEditBlob::CopyEditedPathBack(NodePath* pOrigPath, Path* pEditPath)
{
	// Now to do some undo information. To do this, I have to look at each element in the path
	// and each element in the copy, and see which ones differ. I then have to work out how many
	// elements that was, and create a path to contain those elements, along with an array of indices 
	// telling me where those elements came from
	INT32 NumElements = pOrigPath->InkPath.GetNumCoords();
	PathVerb* SourceVerbs = pOrigPath->InkPath.GetVerbArray();
	DocCoord* SourceCoords = pOrigPath->InkPath.GetCoordArray();
	PathFlags* SourceFlags = pOrigPath->InkPath.GetFlagArray();
	PathVerb* DestVerbs = pEditPath->GetVerbArray();
	DocCoord* DestCoords = pEditPath->GetCoordArray();
	PathFlags* DestFlags = pEditPath->GetFlagArray();

	INT32 ChangedElements = 0;
	INT32 i;	

	for (i=0;i<NumElements;i++)
	{
		if (SourceVerbs[i] != DestVerbs[i] || 
			SourceCoords[i] != DestCoords[i] || 
			SourceFlags[i] != DestFlags[i]
			)
			ChangedElements++;
	}

	// ChangedElements is the number of elements in this path that will change.
	// We have to create three arrays to contain the changed elements, plus one array
	// to tell me where the elements should go (the indices)

	// I also have to create an action object to contain these arrays. I have to create the action
	// object first because that does all the work of deciding if there's enough memory in the 
	// undo buffer to store the action, and prompting the user accordingly of there isn't

	if (ChangedElements > 0)
	{
		ModifyPathAction* ModAction;
	
		ActionCode Act;
		Act = ModifyPathAction::Init(this, &UndoActions, ChangedElements, (Action**)(&ModAction));
		if (Act == AC_FAIL)
		{
			FailAndExecute();
			End();
			return FALSE;
		}

		PathVerb* ChangedVerbs=NULL;
		DocCoord* ChangedCoords=NULL;
		PathFlags* ChangedFlags=NULL;
		INT32* ChangedIndices=NULL;

		// If the function returned AC_NO_RECORD we shouldn't record any undo information in the action
		// NOTE - during unwind all actions return AC_OK with a NULL ModAction pointer!!
		if ((Act!=AC_NORECORD) && (ModAction!=NULL))
		{
			// This next bit is a bit hellish. Any one of these four allocations can fail, in which case 
			// we have to tidy up afterwards. Cue a lot of nested ifs and elses.

			ALLOC_WITH_FAIL(ChangedVerbs,(PathVerb*) CCMalloc(ChangedElements * sizeof(PathVerb)),this);
			if (ChangedVerbs)
			{
				ALLOC_WITH_FAIL(ChangedCoords,(DocCoord*) CCMalloc(ChangedElements * sizeof(DocCoord)),this);
				if (ChangedCoords)
				{
					ALLOC_WITH_FAIL(ChangedFlags,(PathFlags*) CCMalloc(ChangedElements * sizeof(PathFlags)),this);
					if (ChangedFlags)
					{
						ALLOC_WITH_FAIL(ChangedIndices,(INT32*) CCMalloc(ChangedElements * sizeof(INT32)),this);
						if (!ChangedIndices)
						{
							CCFree( ChangedFlags );
							CCFree( ChangedCoords );
							CCFree( ChangedVerbs);
							FailAndExecute();
							End();
							return FALSE;
						}
					}
					else
					{
						CCFree( ChangedCoords );
						CCFree( ChangedVerbs );
						FailAndExecute();
						End();
						return FALSE;

					}
				}
				else
				{
					CCFree( ChangedVerbs);
					FailAndExecute();
					End();
					return FALSE;

				}
			}

			// Now to put the undo data into the undo action
			INT32 index = 0;
			for (i=0;i<NumElements;i++)
			{
				if (SourceVerbs[i] != DestVerbs[i] || 
					SourceCoords[i] != DestCoords[i] || 
					SourceFlags[i] != DestFlags[i]
					)
				{
					ChangedVerbs[index] = SourceVerbs[i];
					ChangedFlags[index] = SourceFlags[i];
					ChangedCoords[index] = SourceCoords[i];
					ChangedIndices[index] = i;
					index++;
				}
			}

			// Now we've allocated the arrays, let's tell the action about 'em
			ModAction->StoreArrays(ChangedVerbs, ChangedFlags, ChangedCoords, ChangedIndices, pOrigPath);
		}
	}
		
	if (!pOrigPath->InkPath.CopyPathDataFrom(pEditPath))
		return FALSE;
	else
		return TRUE;
}



/********************************************************************************************

>	void OpNodePathEditBlob::CopyNeedToRenderFlags()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/03/95
	Inputs:		-
	Outputs:	(see below)
	Returns:	TRUE if the flags have been copied; FALSE if the paths were different lengths
	Purpose:	This function is used to copy the NeedToRender flags from EditPath to
				OriginalPath.  This are then used in optimised region invalidation.

********************************************************************************************/
BOOL OpNodePathEditBlob::CopyNeedToRenderFlags()
{
	const INT32 OrigLength = OriginalPath->InkPath.GetNumCoords();
	const INT32 EditLength = EditPath.GetNumCoords();

	if (EditLength != OrigLength)
		return FALSE;

	PathFlags* EditFlags = EditPath.GetFlagArray();
	PathFlags* OrigFlags = OriginalPath->InkPath.GetFlagArray();
		
	for (INT32 loop = 0; loop < EditLength; loop ++)
	{
		OrigFlags[loop].NeedToRender = EditFlags[loop].NeedToRender;
	}

	return TRUE;
}



/********************************************************************************************

>	BOOL OpNodePathEditBlob::CopyNeedToRenderFlags(NodePath* pOrigPath, Path* pEditPath)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> (based upon Peter)
	Created:	20/04/95
	Inputs:		-
	Outputs:	(see below)
	Returns:	TRUE if the flags have been copied; FALSE if the paths were different lengths
	Purpose:	This function is used to copy the NeedToRender flags from EditPath to
				OriginalPath.  This are then used in optimised region invalidation.

********************************************************************************************/

BOOL OpNodePathEditBlob::CopyNeedToRenderFlags(NodePath* pOrigPath, Path* pEditPath)
{
	const INT32 OrigLength = pOrigPath->InkPath.GetNumCoords();
	const INT32 EditLength = pEditPath->GetNumCoords();

	if (EditLength != OrigLength)
		return FALSE;

	PathFlags* EditFlags = pEditPath->GetFlagArray();
	PathFlags* OrigFlags = pOrigPath->InkPath.GetFlagArray();
		
	for (INT32 loop = 0; loop < EditLength; loop ++)
	{
		OrigFlags[loop].NeedToRender = EditFlags[loop].NeedToRender;
	}

	return TRUE;
}



/********************************************************************************************

>	void OpNodePathEditBlob::RecalculatePath( 	DocCoord Offset, 
												BOOL SnapEnds = FALSE,
												INT32 SnapIndex = 0 )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/93
	Inputs:		Offset - The Amount to move the selected blobs by before re-calcing everything
				SnapEnds is TRUE if the start and endpoint should snap (affects smoothing)
				SnapIndex is the index of the point that's being snapped, so we can only
				snap one subpath out of many
	Scope:		Private
	Purpose:	This goes through the path, moves all the selected coords by the offset and
				then performs some magic to smooth the rest of the path round the changes if
				it needs it.

********************************************************************************************/

void OpNodePathEditBlob::RecalculatePath( DocCoord Offset, BOOL SnapEnds , INT32 SnapIndex)
{
	// Get the array that I need
	PathFlags* Flags  = EditPath.GetFlagArray();
	DocCoord*  Coords = EditPath.GetCoordArray();

	INT32 NumCoords = EditPath.GetNumCoords();

	// Go though all the points
	for (INT32 i=0; i<NumCoords; i++)
	{
		// and translate them by the drag offset if they are selected
		if (Flags[i].IsSelected)
			Coords[i].translate( Offset.x, Offset.y );
	}

	// And now smooth out the new path
	EditPath.SmoothCurve(TRUE, SnapEnds, SnapIndex);

	// Now to snap (possibly) to another path, if the SnapToAnother flag is set
	// This not only entails changing the part of the line we're dragging, but it might
	// also mean we have to set the SnapToCoords array to contain a snapped bezier segment  
	if (SnapToAnother)
	{
		// Get the array pointers
		PathVerb* Verbs = EditPath.GetVerbArray();
		Flags = EditPath.GetFlagArray();
		Coords = EditPath.GetCoordArray();
		PathVerb* OtherVerbs = SnapToPath->InkPath.GetVerbArray();
		PathFlags* OtherFlags = SnapToPath->InkPath.GetFlagArray();
		DocCoord* OtherCoords = SnapToPath->InkPath.GetCoordArray();

		// Because we're snapping, we must be on either the start or the end of a subpath
		// And the CloseFigure flag will never be set.
		// There are sixteen possibilities - for each line, we could be looking at
		// a curve or a line, at the start or end of each subpath. This logic enumerates
		// all of the possibilities. It may look tortuous, but it gets them all.
		if (Verbs[SnapIndex] == PT_MOVETO)
		{
			// We're dragging the start of a subpath. See if it's a curve
			if (Verbs[SnapIndex+1] == PT_BEZIERTO)
			{
				// We are dragging a curve at the start of the path. See what we've snapped to
				if (OtherVerbs[SnapToIndex] == PT_MOVETO)
				{
					// Snapping a curve at start to the start of the other one
					if (OtherVerbs[SnapToIndex+1] == PT_BEZIERTO)
					{
						// Snapping curve at beginning with curve at beginning
						// Let's smooth this bezier according to the snapping
						if (Flags[SnapIndex+1].IsSmooth)
							Coords[SnapIndex+1] = EditPath.SmoothControlPoint(	SnapIndex+1,
																				FALSE,	// start end not snapped
																				TRUE,	// Joined to another
																				TRUE,	// Other is a curve
																				OtherCoords[SnapToIndex+3]
																				);
						// Note that the other control point is not affected by the snap
						// Since the other segment is a curve, we have to set up the SnapToCoords array
						// so that the render routine works properly
						// Notice that the assignment in the if below is required
						if ((SnapToLineOrCurve = OtherFlags[SnapToIndex+1].IsSmooth))
						{
							SnapToCoords[0] = OtherCoords[SnapToIndex];
							SnapToCoords[1] = SnapToPath->InkPath.SmoothControlPoint(	SnapToIndex+1,
																						FALSE,
																						TRUE,
																						TRUE,
																						Coords[SnapIndex+3]
																						);
							SnapToCoords[2] = OtherCoords[SnapToIndex+2];
							SnapToCoords[3] = OtherCoords[SnapToIndex+3];
						}
					}
					else
					{
						// Snapping curve at beginning with line at beginning
						if (Flags[SnapIndex+1].IsSmooth)
							Coords[SnapIndex+1] = EditPath.SmoothControlPoint(	SnapIndex+1,
																				FALSE,	// start end not snapped
																				TRUE,	// Joined to another
																				FALSE,	// Other is a line
																				OtherCoords[SnapToIndex+1]
																				);
						// Since we're snapping to a line, we just need to set SnapToLineOrCurve to FALSE
						SnapToLineOrCurve = FALSE;

					}
				}
				else
				{
					// Snapping curve at beginning with the end of the other one
					if (OtherVerbs[SnapToIndex] == PT_BEZIERTO)
					{
						// Snapping curve at beginning with curve at end of other one
						// Smooth the editpath bezier if the flag says we can
						if (Flags[SnapIndex+1].IsSmooth)
							Coords[SnapIndex+1] = EditPath.SmoothControlPoint(	SnapIndex+1,
																				FALSE,
																				TRUE,
																				TRUE,
																				OtherCoords[SnapToIndex-3]
																				);
						// Now set up SnapToCoords array
						if ((SnapToLineOrCurve = OtherFlags[SnapToIndex-1].IsSmooth))	// yes the other path is a curve
						{
							SnapToCoords[0] = OtherCoords[SnapToIndex];
							SnapToCoords[1] = SnapToPath->InkPath.SmoothControlPoint(	SnapToIndex-1,
																						FALSE,
																						TRUE,
																						TRUE,
																						Coords[SnapIndex+3]
																						);
							SnapToCoords[2] = OtherCoords[SnapToIndex-2];
							SnapToCoords[3] = OtherCoords[SnapToIndex-3];
						}
					}
					else
					{
						// snapping curve at beginning with line at end of other one
						if (Flags[SnapIndex+1].IsSmooth)
							Coords[SnapIndex+1] = EditPath.SmoothControlPoint(	SnapIndex+1,
																				FALSE,
																				TRUE,
																				FALSE,
																				OtherCoords[SnapToIndex-1]
																				);
						// Other is a line, so the SnapToCoords array isn't used
						SnapToLineOrCurve = FALSE;
					}
				}

			}
			else
			{
				// Dragging a line at the beginning. What are we snapping to
				if (OtherVerbs[SnapToIndex] == PT_MOVETO)
				{
					// Snapping a line at start to the start of the other one
					if (OtherVerbs[SnapToIndex+1] == PT_BEZIERTO)
					{
						// Snapping line at beginning with curve at beginning
						// There's nothing to smooth in the edit path, but we have to
						// set up the SnapToCoords array
						if ((SnapToLineOrCurve = OtherFlags[SnapToIndex+1].IsSmooth))
						{
							SnapToCoords[0] = OtherCoords[SnapToIndex];
							SnapToCoords[1] = SnapToPath->InkPath.SmoothControlPoint(	SnapToIndex+1,
																						FALSE,
																						TRUE,
																						FALSE,
																						Coords[SnapIndex+1]
																						);
							SnapToCoords[2] = OtherCoords[SnapToIndex+2];
							SnapToCoords[3] = OtherCoords[SnapToIndex+3];
						}
					}
					{
						// Snapping line at beginning with line at beginning
						// Nothing whatever to smooth, just clear the flag
						SnapToLineOrCurve = FALSE;
					}
				}
				else
				{
					// Snapping line at beginning with the end of the other one
					if (OtherVerbs[SnapToIndex] == PT_BEZIERTO)
					{
						// Snapping line at beginning with curve at end of other one
						// Nothing to smooth in editpath, let's set up SnapToCoords array
						if ((SnapToLineOrCurve = OtherFlags[SnapToIndex-1].IsSmooth))
						{
							SnapToCoords[0] = OtherCoords[SnapToIndex];
							SnapToCoords[1] = SnapToPath->InkPath.SmoothControlPoint(	SnapToIndex-1,
																						FALSE,
																						TRUE,
																						FALSE,
																						Coords[SnapIndex+1]
																						);
							SnapToCoords[2] = OtherCoords[SnapToIndex-2];
							SnapToCoords[3] = OtherCoords[SnapToIndex-3];
						}
					}
					else
					{
						// snapping line at beginning with line at end of other one
						// Just clear the flag
						SnapToLineOrCurve = FALSE;
					}
				}

			}
		}
		else
		{
			// We're snapping the end of the path, let's see what
			if (Verbs[SnapIndex] == PT_BEZIERTO)
			{
				// Snapping a curve at the end to... what?
				if (OtherVerbs[SnapToIndex] == PT_MOVETO)
				{
					// Snapping a curve at the end to the start of the other one
					if (OtherVerbs[SnapToIndex+1] == PT_BEZIERTO)
					{
						// Snapping curve at the end with curve at beginning
						if (Flags[SnapIndex-1].IsSmooth)
							Coords[SnapIndex-1] = EditPath.SmoothControlPoint(	SnapIndex-1,
																				FALSE,
																				TRUE,
																				TRUE,
																				OtherCoords[SnapToIndex+3]
																				);
						// Set up the SnapToCoords array
						if ((SnapToLineOrCurve = OtherFlags[SnapToIndex+1].IsSmooth))
						{
							SnapToCoords[0] = OtherCoords[SnapToIndex];
							SnapToCoords[1] = SnapToPath->InkPath.SmoothControlPoint(	SnapToIndex+1,
																					 	FALSE,
																						TRUE,
																						TRUE,
																						Coords[SnapIndex-3]
																						);
							SnapToCoords[2] = OtherCoords[SnapToIndex+2];
							SnapToCoords[3] = OtherCoords[SnapToIndex+3];
						}
					}
					else
					{
						// Snapping curve at the end with line at beginning
						if (Flags[SnapIndex-1].IsSmooth)
							Coords[SnapIndex-1] = EditPath.SmoothControlPoint(	SnapIndex-1,
																				FALSE,
																				TRUE,
																				FALSE,
																				OtherCoords[SnapToIndex+1]
																				);

						// Clear flag for SnapToCoords array
						SnapToLineOrCurve = FALSE;
					}
				}
				else
				{
					// Snapping curve at the end with the end of the other one
					if (OtherVerbs[SnapToIndex] == PT_BEZIERTO)
					{
						// Snapping curve at the end with curve at end of other one
						if (Flags[SnapIndex-1].IsSmooth)
							Coords[SnapIndex-1] = EditPath.SmoothControlPoint(	SnapIndex-1,
																				FALSE,
																				TRUE,
																				TRUE,
																				OtherCoords[SnapToIndex-3]
																				);
						// Init the SnapToCoords array
						if ((SnapToLineOrCurve = OtherFlags[SnapToIndex-1].IsSmooth))
						{
							SnapToCoords[0] = OtherCoords[SnapToIndex];
							SnapToCoords[1] = SnapToPath->InkPath.SmoothControlPoint(	SnapToIndex-1,
																						FALSE,
																						TRUE,
																						TRUE,
																						Coords[SnapIndex-3]
																						);
							SnapToCoords[2] = OtherCoords[SnapToIndex-2];
							SnapToCoords[3] = OtherCoords[SnapToIndex-3];
						}
					}
					else
					{
						// snapping curve at the end with line at end of other one
						if (Flags[SnapIndex-1].IsSmooth)
							Coords[SnapIndex-1] = EditPath.SmoothControlPoint(	SnapIndex-1,
																				FALSE,
																				TRUE,
																				FALSE,
																				OtherCoords[SnapToIndex-1]
																				);
						// Other is a line so clear the flag
						SnapToLineOrCurve = FALSE;
					}
				}

			}
			else
			{
				// Snapping a line at the end with something
				if (OtherVerbs[SnapToIndex] == PT_MOVETO)
				{
					// Snapping a line at the end to the start of the other one
					if (OtherVerbs[SnapToIndex+1] == PT_BEZIERTO)
					{
						// Snapping line at the end with curve at beginning
						// Nothing to smooth in the edit path, but we should set up SnapToCoords array
						if ((SnapToLineOrCurve = OtherFlags[SnapToIndex+1].IsSmooth))
						{
							SnapToCoords[0] = OtherCoords[SnapToIndex];
							SnapToCoords[1] = SnapToPath->InkPath.SmoothControlPoint(	SnapToIndex+1,
																						FALSE,
																						TRUE,
																						FALSE,
																						Coords[SnapIndex-1]
																						);
							SnapToCoords[2] = OtherCoords[SnapToIndex+2];
							SnapToCoords[3] = OtherCoords[SnapToIndex+3];
						}
					}
					{
						// Snapping line at the end with line at beginning
						// Just clear the flag
						SnapToLineOrCurve = FALSE;
					}
				}
				else
				{
					// Snapping line at the end with the end of the other one
					if (OtherVerbs[SnapToIndex] == PT_BEZIERTO)
					{
						// Snapping line at the end with curve at end of other one
						// Nothing to smooth in Editpath, but we have to do the array
						if ((SnapToLineOrCurve = OtherFlags[SnapToIndex-1].IsSmooth))
						{
							SnapToCoords[0] = OtherCoords[SnapToIndex];
							SnapToCoords[1] = SnapToPath->InkPath.SmoothControlPoint(	SnapToIndex-1,
																						FALSE,
																						TRUE,
																						FALSE,
																						Coords[SnapIndex-1]
																						);
							SnapToCoords[2] = OtherCoords[SnapToIndex-2];
							SnapToCoords[3] = OtherCoords[SnapToIndex-3];
						}
					}
					else
					{
						// snapping line at the end with line at end of other one
						// Just clear the flag
						SnapToLineOrCurve = FALSE;
					}
				}

			}
		}

	}

}



/********************************************************************************************

>	void OpNodePathEditBlob::RecalculatePaths( 	DocCoord Offset, 
												BOOL SnapEnds = FALSE,
												INT32 SnapIndex = 0 )

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/2000
	Inputs:		Offset - The Amount to move the selected blobs by before re-calcing everything
				SnapEnds is TRUE if the start and endpoint should snap (affects smoothing)
				SnapIndex is the index of the point that's being snapped, so we can only
				snap one subpath out of many
	Scope:		Private
	Purpose:	This goes through the path, moves all the selected coords by the offset and
				then performs some magic to smooth the rest of the path round the changes if
				it needs it.

********************************************************************************************/

void OpNodePathEditBlob::RecalculatePaths (Path* pEditPath, DocCoord Offset, BOOL SnapEnds , INT32 SnapIndex)
{
	//NodeListItem* pCurrentOrig = (NodeListItem*) OriginalPaths.GetHead ();
	//NodeListItem* pCurrentEdit = (NodeListItem*) EditPaths.GetHead ();

	//while (/*(pCurrentOrig) &&*/ (pCurrentEdit))
	//{
		//NodePath* pOrigPath = (NodePath*) (pCurrentOrig->pNode);
	//	Path* pEditPath = (Path*) (pCurrentEdit->pNode);
	
		// Get the array that I need
		PathFlags* Flags  = pEditPath->GetFlagArray();
		DocCoord*  Coords = pEditPath->GetCoordArray();

		INT32 NumCoords = pEditPath->GetNumCoords();

		// Go though all the points
		for (INT32 i=0; i<NumCoords; i++)
		{
			// and translate them by the drag offset if they are selected
			if (Flags[i].IsSelected)
				Coords[i].translate( Offset.x, Offset.y );
		}

		// And now smooth out the new path
		pEditPath->SmoothCurve(TRUE, SnapEnds, SnapIndex);

		//pCurrentOrig = (NodeListItem*) OriginalPaths.GetNext (pCurrentOrig);
	//	pCurrentEdit = (NodeListItem*) EditPaths.GetNext (pCurrentEdit);
	//}
}



/********************************************************************************************
>	void OpNodePathEditBlob::RenderDragBlobs( DocRect Rect, SPread* pSpread, BOOL bSolidDrag)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/93
	Purpose:	Renders the new version of the path to the window. It makes use of flags in
				the path to determine which segments of the path need to be rendered.
********************************************************************************************/
void OpNodePathEditBlob::RenderDragBlobs( DocRect Rect, Spread *pSpread, BOOL bSolidDrag )
{
	RenderPathEditBlobs(Rect,pSpread);
	if (EditObjChange.ChangeMask.EorBlobs)
		EditObjChange.RenderCurrentBlobs(OriginalPath,this,&EditPath,StartSpread,TRUE);
}



/********************************************************************************************
>	void OpNodePathEditBlob::RenderDraggingBlobs( DocRect Rect, Spread *pSpread )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/95
	Purpose:	Call this function to render all the blobs on screen from this operation
********************************************************************************************/
void OpNodePathEditBlob::RenderDraggingBlobs ( DocRect Rect, Spread *pSpread )
{
	RenderPathEditBlobs(Rect, pSpread);
}



/********************************************************************************************
>	void OpNodePathEditBlob::RenderDraggingBlobs ( Path* pEditPath, Spread *pSpread )

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/2000
	Purpose:	Call this function to render all the blobs on screen from this operation
********************************************************************************************/

void OpNodePathEditBlob::RenderDraggingBlobs ( Path* pEditPath, Spread *pSpread )
{
	RenderPathEditBlobs(pEditPath, pSpread);
}


void OpNodePathEditBlob::RenderPathEditBlobs( DocRect Rect, Spread *pSpread )
{
	// First we have to build a path to render
	Path RenderPath;
	if ( RenderPath.Initialise(24, 12) == FALSE )
	{
		// We did not get the memory, so don't draw anything
		return;
	}

	BOOL IsInCurve = FALSE;
	INT32 NumCoords = EditPath.GetNumCoords();
	for (INT32 i=0; i<NumCoords; i++)
	{
		// Dereference the pointers I need
		PathFlags* Flags  = EditPath.GetFlagArray();
		DocCoord*  Coords = EditPath.GetCoordArray();
		PathVerb*  Verbs  = EditPath.GetVerbArray();

		// We only consider the end points
		if (Flags[i].IsEndPoint)
		{
			if (IsInCurve)
			{
				// we are already adding to the curve, so see if we should continue adding
				if (Flags[i].NeedToRender)
				{
					switch (Verbs[i] & (~PT_CLOSEFIGURE))
					{
						case PT_MOVETO:
							RenderPath.InsertMoveTo(Coords[i]);
							break;

						case PT_LINETO:
							RenderPath.InsertLineTo(Coords[i]);
							break;

						case PT_BEZIERTO:
							RenderPath.InsertCurveTo(Coords[i-2], Coords[i-1], Coords[i]);
							break;

						default:
							break;
					}
				}
				else
				{
					IsInCurve = FALSE;
				}
			}
			else
			{
				// We are not currently adding to the render path, so start
				if (Flags[i].NeedToRender)
				{
					switch (Verbs[i] & (~PT_CLOSEFIGURE))
					{
					case PT_MOVETO:
						RenderPath.InsertMoveTo(Coords[i]);
						break;
					case PT_BEZIERTO:
						RenderPath.InsertMoveTo(Coords[i-3]);
						RenderPath.InsertCurveTo(Coords[i-2],Coords[i-1],Coords[i]);
						break;
					case PT_LINETO:
						RenderPath.InsertMoveTo(Coords[i-1]);
						RenderPath.InsertLineTo(Coords[i]);
						break;
					}
					IsInCurve = TRUE;
				}
			}
		}
	}

	// And if we've snapped to another curve
	if (SnapToLineOrCurve)
	{
		// We are snapping to another curve, so render the curve
		RenderPath.InsertMoveTo(SnapToCoords[0]);
		RenderPath.InsertCurveTo(SnapToCoords[1],SnapToCoords[2],SnapToCoords[3]);
	}
	
	// Make sure that there is something to draw
	if (RenderPath.GetNumCoords()==0)
		return;

	// And this bit actually draws the path we have just made
	RenderRegion* pRegion = DocView::RenderOnTop( NULL, StartSpread, ClippedEOR );
	while (pRegion != NULL)
	{
		pRegion->SetLineColour( COLOUR_XORNEW );	// Set the line colour 
		pRegion->DrawPath( &RenderPath );			// Draw an EORed version of how the path will turn out
		pRegion = DocView::GetNextOnTop( &Rect );	// Get the next render region
	}
}



/********************************************************************************************
>	void OpNodePathEditBlob::RenderPathEditBlobs( Path* pEditPath, Spread *pSpread )

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/2000
	Purpose:	Call this function to render all the blobs on screen from this operation
********************************************************************************************/

void OpNodePathEditBlob::RenderPathEditBlobs( Path* pEditPath, /*DocRect Rect,*/ Spread *pSpread )
{
	// First we have to build a path to render
	Path RenderPath;
	if ( RenderPath.Initialise(24, 12) == FALSE )
	{
		// We did not get the memory, so don't draw anything
		return;
	}

	DocRect Rect = pEditPath->GetBoundingRect();

	BOOL IsInCurve = FALSE;
	INT32 NumCoords = pEditPath->GetNumCoords();
	for (INT32 i=0; i<NumCoords; i++)
	{
		// Dereference the pointers I need
		PathFlags* Flags  = pEditPath->GetFlagArray();
		DocCoord*  Coords = pEditPath->GetCoordArray();
		PathVerb*  Verbs  = pEditPath->GetVerbArray();

		// We only consider the end points
		if (Flags[i].IsEndPoint)
		{
			if (IsInCurve)
			{
				// we are already adding to the curve, so see if we should continue adding
				if (Flags[i].NeedToRender)
				{
					switch (Verbs[i] & (~PT_CLOSEFIGURE))
					{
						case PT_MOVETO:
							RenderPath.InsertMoveTo(Coords[i]);
							break;

						case PT_LINETO:
							RenderPath.InsertLineTo(Coords[i]);
							break;

						case PT_BEZIERTO:
							RenderPath.InsertCurveTo(Coords[i-2], Coords[i-1], Coords[i]);
							break;

						default:
							break;
					}
				}
				else
				{
					IsInCurve = FALSE;
				}
			}
			else
			{
				// We are not currently adding to the render path, so start
				if (Flags[i].NeedToRender)
				{
					switch (Verbs[i] & (~PT_CLOSEFIGURE))
					{
					case PT_MOVETO:
						RenderPath.InsertMoveTo(Coords[i]);
						break;
					case PT_BEZIERTO:
						RenderPath.InsertMoveTo(Coords[i-3]);
						RenderPath.InsertCurveTo(Coords[i-2],Coords[i-1],Coords[i]);
						break;
					case PT_LINETO:
						RenderPath.InsertMoveTo(Coords[i-1]);
						RenderPath.InsertLineTo(Coords[i]);
						break;
					}
					IsInCurve = TRUE;
				}
			}
		}
	}

	// And if we've snapped to another curve
	if (SnapToLineOrCurve)
	{
		// We are snapping to another curve, so render the curve
		RenderPath.InsertMoveTo(SnapToCoords[0]);
		RenderPath.InsertCurveTo(SnapToCoords[1],SnapToCoords[2],SnapToCoords[3]);
	}
	
	// Make sure that there is something to draw
	if (RenderPath.GetNumCoords()==0)
		return;

	// And this bit actually draws the path we have just made
	RenderRegion* pRegion = DocView::RenderOnTop( NULL, StartSpread, ClippedEOR );
	while (pRegion != NULL)
	{
		pRegion->SetLineColour( COLOUR_XORNEW );	// Set the line colour 
		pRegion->DrawPath( &RenderPath );			// Draw an EORed version of how the path will turn out
		pRegion = DocView::GetNextOnTop( &Rect );	// Get the next render region
	}
}



/********************************************************************************************

>	BOOL OpNodePathEditBlob::Init()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpNodePathEditBlob::Init()
{
	return (RegisterOpDescriptor(	0, 
									_R(IDS_NODEPATH_EDIT),
									CC_RUNTIME_CLASS(OpNodePathEditBlob), 
									OPTOKEN_NODEPATH,
									OpNodePathEditBlob::GetState,
									0,										// help ID
									_R(IDBBL_NODEPATHOP),
									0 ) );									// bitmap ID */
}


/********************************************************************************************

>	OpState OpNodePathEditBlob::GetState(String_256* Description, OpDescriptor*)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the selector tool
	Returns:	The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/
OpState OpNodePathEditBlob::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}



/********************************************************************************************
>	void OpNodePathEditBlob::SetStatusLineHelp()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/95
	Purpose:	Updates the status line message to reflect the current situation
********************************************************************************************/
void OpNodePathEditBlob::SetStatusLineHelp()
{
	String_256 HelpString;
	if (EndSnapped)
		HelpString = String_256(_R(IDS_PATHDRAGCREATESHAPE));
	else
	{
		if (SnapToAnother)
			HelpString = String_256(_R(IDS_PATHDRAGJOINPATHS));
		else
			HelpString = String_256(_R(IDS_PATHDRAGFINISH));
	}
	GetApplication()->UpdateStatusBarText(&HelpString, FALSE);
}







/********************************************************************************************

>	void OpNodePathEditControlBlob::DoStartDragEdit(NodePath* OrigPath, DocCoord Anchor,
												    Spread *pSpread, INT32 ControlIndex)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		OrigPath - The original Node path object
				Anchor - The position of the mouse at the start of the Drag
				pSpread - the spread that the drag started on
				ControlIndex - The Coord number of the control point that we are dragging
	Purpose:	This is called when a Drag operation has been started on a control point
				of a path

********************************************************************************************/
void OpNodePathEditControlBlob::DoStartDragEdit(NodePath* OrigPath, DocCoord Anchor,
											    Spread *pSpread, INT32 ControlIndex)
{
	// We had better take a note of the starting point of the drag
	LastMousePos = Anchor;
	StartMousePos = Anchor;
	StartSpread  = pSpread;
	OriginalPath = OrigPath;
	Index = ControlIndex;
	BOOL ok = TRUE;

	PathFlags* Flags = OrigPath->InkPath.GetFlagArray();
	PathVerb* Verbs = OrigPath->InkPath.GetVerbArray();
	DocCoord* Coords = OrigPath->InkPath.GetCoordArray();

	// Find the index of the end point that this control point is attached to
	if (Flags[Index-1].IsEndPoint)
		EndPointIndex = Index-1;
	else
		EndPointIndex = Index+1;
	ConstrainPoint = Coords[EndPointIndex];
	ConstrainPrevPoint = ConstrainPoint;
	ConstrainNextPoint = ConstrainPoint;

	// Find the opposite index
	OppositeIndex = OrigPath->InkPath.FindOppositeControlPoint(Index);

	// We also need to make a version of the path that we can change
	if (ok)
		ok = BuildEditPath();

	// Create and send a change message about this path edit
	if (ok)
		ok = EditObjChange.ObjChangeStarting(OrigPath,this,&EditPath,StartSpread,TRUE) == CC_OK;

	if (ok)
		ok = CreateCursors();
	if (ok)
		ChangeCursor(pCrossHairCursor);

	// Since we're editing control points, we have to clear the IsSmooth flags on this,
	// the adjacent point and the endpoint
	if (ok)
	{
		Flags = EditPath.GetFlagArray();
		Verbs = EditPath.GetVerbArray();

		Flags[ControlIndex].IsSmooth = FALSE;
		if (OppositeIndex >= 0)
			Flags[OppositeIndex].IsSmooth = FALSE;
		Flags[EndPointIndex].IsSmooth = FALSE;
	}

	// Render the bits of the path that are different
	if (ok)
	{
		DocRect EditPathBBox = EditPath.GetBoundingRect();
		RenderPathEditControlBlobs(EditPathBBox, pSpread);
		DragStarted = TRUE;

		// And tell the Dragging system that we need drags to happen
		ok = StartDrag(DRAGTYPE_AUTOSCROLL, &EditPathBBox, &LastMousePos);
	}

	if (!ok)
	{
		InformError();
		FailAndExecute();
		End();
	}
}



/********************************************************************************************

>	void OpNodePathEditControlBlob::DragPointerMove( DocCoord PointerPos, 
												ClickModifiers ClickMods, Spread *pSpread,
												BOOL bSolidDrag)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods - Which key modifiers are being pressed
				pSpread - the spread that the cursor is over now
	Purpose:	This is called every time the mouse moves, during a drag.
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpNodePathEditControlBlob::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread *pSpread, BOOL bSolidDrag)
{
	EndSnapped = FALSE;		// Always false in this operation
	
	// If drag has moved onto a different spread, convert the coord to be relative to the
	// original spread.
	if (pSpread != StartSpread)
		PointerPos = MakeRelativeToSpread(StartSpread, pSpread, PointerPos);

	Path TempPath;

	// inform the parent a change is happening
	if (EditObjChange.ChangeMask.EorBlobs)
	{
		ChangeCode Chge=EditObjChange.RenderCurrentBlobs(OriginalPath,this,&EditPath,StartSpread,TRUE);
		if (Chge!=CC_OK)
			return;
		// Create a local copy of the edit path
		if (!TempPath.Initialise(EditPath.GetNumCoords(), 12))
			return;
		TempPath.CopyPathDataFrom(&EditPath);
	}

	// Optionally constrain the new position
	ERROR3IF(ConstrainPoint == DocCoord(-1,-1),"ConstrainPoint wasn't set");
	if (ClickMods.Constrain)
		DocView::ConstrainToAngle(ConstrainPoint, &PointerPos);
	DocView::SnapCurrent(StartSpread, &PointerPos);

	// Rub out the old EORed version of the path
	RenderPathEditControlBlobs( EditPath.GetBoundingRect(), StartSpread );

	// This is the bit where we go off and re-calculate the paths position,
	// based on how much the mouse has moved
	DocCoord Offset;
	Offset.x = PointerPos.x - LastMousePos.x;
	Offset.y = PointerPos.y - LastMousePos.y;
	RecalculatePath( Offset );

	if (EditObjChange.ChangeMask.EorBlobs)
	{
		ChangeCode Chge=EditObjChange.RenderChangedBlobs(OriginalPath,this,&EditPath,StartSpread,TRUE);
		if (Chge!=CC_OK)
		{
			// replace the old edit path
			EditPath.CopyPathDataFrom(&TempPath);
			RenderPathEditControlBlobs( EditPath.GetBoundingRect(), StartSpread );
			return;
		}
	}

	// Update the last mouse position and draw in the new version of the path
	LastMousePos = PointerPos;
	RenderPathEditControlBlobs( EditPath.GetBoundingRect(), StartSpread );

	// Now we want to update the Line tool infobar if the position of control point being 
	// dragged is being shown.
	PathFlags* Flags = EditPath.GetFlagArray();
//	DocCoord* Coords = EditPath.GetCoordArray();
	INT32  CurrentIndex;

	if (Flags[Index+1].IsEndPoint)
	{
		CurrentIndex = Index+1;
	}
	else
	{
		CurrentIndex = Index-1;
	}

	// We'll broadcast a message about the edit, thus hopping over the kernel/tool DLL boundary
	BROADCAST_TO_ALL(PathEditedMsg(&EditPath, pSpread, CurrentIndex));

	SetStatusLineHelp();
}



/********************************************************************************************
>	BOOL OpNodePathEditControlBlob::BuildEditPath()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> - later attacked by Peter
	Created:	17/02/94
	Returns:	TRUE if it managed to build the path, FALSE if it failed
	Purpose:	Builds a copy of the path that we can edit, without destroying the original
				data.  Also sets the NeedToRenderFlags for the EOR rendering during the drag.
				The parts of the path need to be rendered during the drag are the segment
				that contains this control point and the segment with the opposite control
				point if the rotate flag is set on the endpoint
	Errors:		If it runs out of memory then it will return FALSE
********************************************************************************************/
BOOL OpNodePathEditControlBlob::BuildEditPath()
{
	// Make a copy of the original path and clear its need to render flags
	const INT32 NumCoords = OriginalPath->InkPath.GetNumCoords();
	if (!EditPath.Initialise(NumCoords, 24))
		return FALSE;
	if (!EditPath.CopyPathDataFrom(&(OriginalPath->InkPath)))
		return FALSE;
	EditPath.ClearNeedToRender();

	PathFlags* Flags = EditPath.GetFlagArray();

	// Set the render flag on the endpoint after the dragged control point so that segment renders
	if (Flags[Index-1].IsEndPoint)
		Flags[Index+2].NeedToRender = TRUE;
	else
		Flags[Index+1].NeedToRender = TRUE;

	// Now do the same for the opposite control point, if the Rotate flag is set on the endpoint
	if (Flags[EndPointIndex].IsRotate && OppositeIndex>=0 && OppositeIndex<NumCoords)
	{	
		if (Flags[OppositeIndex-1].IsEndPoint)
			Flags[OppositeIndex+2].NeedToRender = TRUE;
		else
			Flags[OppositeIndex+1].NeedToRender = TRUE;
	}

	// Set the endpoints smooth flag to FALSE
	Flags[EndPointIndex].IsSmooth = FALSE;

	return TRUE;
}



/********************************************************************************************

>	void OpNodePathEditControlBlob::RecalculatePath( DocCoord Offset )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/93
	Inputs:		Offset - The Amount to move the selected blobs by before re-calcing everything
	Scope:		Private
	Purpose:	Will move the control point in question and then re-calculate the whole path
				setting the redraw flag where needed.

********************************************************************************************/

void OpNodePathEditControlBlob::RecalculatePath( DocCoord Offset )
{
	// Find the coords in the path
	DocCoord* Coords = EditPath.GetCoordArray();
	PathFlags* Flags = EditPath.GetFlagArray();
	INT32 NumCoords = EditPath.GetNumCoords();
	if (Flags[EndPointIndex].IsRotate && OppositeIndex >=0 && OppositeIndex < NumCoords)
	{
		EditPath.CalcRotate(Coords[EndPointIndex], &Coords[Index], &Coords[OppositeIndex], Offset);
	}
	else
	{
		Coords[Index].x += Offset.x;
		Coords[Index].y += Offset.y;
	}
}


/********************************************************************************************

>	virtual void OpNodePathEditControlBlob::RenderDragBlobs(DocRect Rect, Spread* pSpread, BOOL bSolidDrag)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Purpose:	Renders the new version of the path to the window. It calls to the
				base class function to render the path, then renders some blobs itself.

********************************************************************************************/

void OpNodePathEditControlBlob::RenderDragBlobs(DocRect Rect, Spread *pSpread, BOOL bSolidDrag)
{
	// Don't render anything if the last mouse position is the same as the start point
	if (LastMousePos == StartMousePos) return;

	// Now render the current edit path
	RenderPathEditControlBlobs(Rect,pSpread);

	// And report the change to our parents.
	if (EditObjChange.ChangeMask.EorBlobs)
		EditObjChange.RenderCurrentBlobs(OriginalPath,this,&EditPath,StartSpread,TRUE);
}



/********************************************************************************************
>	void OpNodePathEditControlBlob::RenderDraggingBlobs( DocRect Rect, Spread *pSpread )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/95
	Purpose:	Call this function to render all the blobs on screen from this operation
********************************************************************************************/
void OpNodePathEditControlBlob::RenderDraggingBlobs ( DocRect Rect, Spread *pSpread )
{
	RenderPathEditControlBlobs(Rect, pSpread);
}



/********************************************************************************************

>	void OpNodePathEditControlBlob::RenderPathEditControlBlobs( DocRect Rect, Spread* pSpread )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Inputs:		Rect	= a doc rect rectangle
				pSpread	= a pointer to a spread to render into
	Purpose:	Renders the new version of the path to the window. It calls to the
				base class function to render the path, then renders some blobs itself.

********************************************************************************************/

void OpNodePathEditControlBlob::RenderPathEditControlBlobs( DocRect Rect, Spread *pSpread )
{
	if (LastMousePos == StartMousePos)
		return;

	DocCoord* Coords = EditPath.GetCoordArray();
	PathFlags* Flags = EditPath.GetFlagArray();
//	PathVerb* Verbs = EditPath.GetVerbArray();
	INT32 NumCoords = EditPath.GetNumCoords();

//	BOOL PrevIsEndPoint;
	DocCoord StartCoord = Coords[0];

	// And this bit actually draws the path we have just made
	RenderRegion* pRegion = DocView::RenderOnTop( NULL, StartSpread, ClippedEOR );
	while ( pRegion )
	{
		OpNodePathEditBlob::RenderPathEditBlobs(Rect, pSpread);

		EditPath.DrawControlBlob(pRegion, Coords[Index]);
		EditPath.DrawControlLine(pRegion, Coords[Index], Coords[EndPointIndex]);
		if (Flags[EndPointIndex].IsRotate && OppositeIndex>=0 && OppositeIndex<NumCoords && Flags[Index].IsSelected)
		{
			
			EditPath.DrawControlBlob(pRegion, Coords[OppositeIndex]);
			EditPath.DrawControlLine(pRegion, Coords[OppositeIndex], Coords[EndPointIndex]);
		}			

		// Get the Next render region
		pRegion = DocView::GetNextOnTop( &Rect );
		
	}
}

/********************************************************************************************

>	BOOL OpNodePathEditControlBlob::Init()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpNodePathEditControlBlob::Init()
{
	PORTNOTETRACE("other","OpNodePathEditControlBlob::Init - do nothing");
	return (RegisterOpDescriptor(
									0, 
									_R(IDS_NODEPATH_EDIT),
									CC_RUNTIME_CLASS(OpNodePathEditControlBlob), 
									OPTOKEN_NODEPATH,
									OpNodePathEditControlBlob::GetState,
									0,											// help ID
									_R(IDBBL_NODEPATHOP),
									0 ) );										// bitmap ID
}


/********************************************************************************************

>	OpState OpNodePathEditControlBlob::GetState(String_256* Description, OpDescriptor*)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the selector tool
	Returns:	The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpNodePathEditControlBlob::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}



/********************************************************************************************
>	void OpNodePathEditControlBlob::SetStatusLineHelp()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/96
	Purpose:	Updates the status line message to reflect the current situation
********************************************************************************************/
void OpNodePathEditControlBlob::SetStatusLineHelp()
{
	String_256 HelpString(_R(IDS_PATHDRAGFINISHCONTROL));
	GetApplication()->UpdateStatusBarText(&HelpString, FALSE);
}


/********************************************************************************************

>	NodeGroup* OpNodePathEditBlob::GetGroupParentOfCurve()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/99
	inputs :    -
	outputs:    
	returns:    pointer to the first nodeblend in the selection if successful, 
				NULL otherwise
	Purpose:	To determine if the current selection is a blend on a curve


*********************************************************************************************/

NodeGroup* OpNodePathEditBlob::GetGroupParentOfCurve()
{	
	if (OriginalPath == NULL)
		return NULL;
	Node* pParent = OriginalPath->FindParent();

	if (pParent != NULL)
	{
		if (pParent->IS_KIND_OF(NodeBlend))
		{
			return (NodeGroup*)pParent;
		}
		else
			return NULL;
	}

	return NULL;

}



/********************************************************************************************

>	NodeGroup* OpNodePathEditBlob::GetGroupParentOfCurve(NodePath* pOrigPath)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> (based upon Diccon Yamanaka's)
	Created:	20/4/2000
	inputs :    -
	outputs:    
	returns:    pointer to the first nodeblend in the selection if successful, 
				NULL otherwise
	Purpose:	To determine if the current selection is a blend on a curve


*********************************************************************************************/

NodeGroup* OpNodePathEditBlob::GetGroupParentOfCurve(NodePath* pOrigPath)
{
	if (pOrigPath == NULL)
		return NULL;
	Node* pParent = pOrigPath->FindParent();

	if (pParent != NULL)
	{
		if (pParent->IS_KIND_OF(NodeBlend))
		{
			return (NodeGroup*)pParent;
		}
		else
			return NULL;
	}

	return NULL;

}


/********************************************************************************************

>	AttrBrushType* OpNodePathEditBlob::GetAppliedBrush()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/99
	inputs :    -
	outputs:    -
	returns:    pointer to the attrbrushtype that is applied to this nodepath, or NULL if there
				isn't one.  Note that if the applied brush is the default 'no brush' then we also
				return NULL
	Purpose:	To get the brush attribute that is applied to the nodepath we are editing


*********************************************************************************************/

AttrBrushType* OpNodePathEditBlob::GetAppliedBrush()
{
	if (OriginalPath == NULL)
		return NULL;

	NodeAttribute* pAttr = NULL;
	AttrBrushType* pAttrBrush = NULL;
	OriginalPath->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBrushType), &pAttr);
	
	if (pAttr != NULL)
	{
		// just check that our applied brush isn't the default
		if (((AttrBrushType*)pAttr)->GetBrushHandle() != BrushHandle_NoBrush)
			pAttrBrush = (AttrBrushType*)pAttr;
	}
	return pAttrBrush;
}


/********************************************************************************************

>	BOOL OpNodePathEditBlob::EditBrushAttribute(INT32 FirstIndex, INT32 LastIndex, AttrBrushType* pAttr)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/2000
	inputs :    FirstIndex - first changed index of the path
				LastIndex  - last changed index
				pAttrBrush - the attribute to change
	outputs:    -
	returns:    TRUE if all went well
	Purpose:	If we use the shape editor to edit a nodepath with an applied brush attribute
				and this brush attribute makes use of sampled pressure or time information then
				the brush needs to resample its data.  So here we will insert a few actions to
				make that happen.


*********************************************************************************************/

BOOL OpNodePathEditBlob::EditBrushAttribute(INT32 FirstIndex, INT32 LastIndex, AttrBrushType* pAttrBrush)
{
	if (FirstIndex < 0 || LastIndex < 0 || LastIndex <= FirstIndex || pAttrBrush == NULL)
	{
		//ERROR3("Invalid inputs to OpNodePathEditBlob::EditBrushAttribute");
		return FALSE;
	}
	
	// currently we only have pressure implemented, and if this attribute does not use
	// pressure then theres nothing for us to do
	CDistanceSampler* pDistSamp = pAttrBrush->GetPressureCache();
	if (pDistSamp == NULL)
		return TRUE;  // not an error 
	
	/* Basically what we want to do here is resample the data that applies to the changed 
	 section of the path, so that we have the same data applying over a different distance.
	 The way to implement this in an undoable way is this:
	 1) Find out the new length of the changed section of the path;
	 2) Ask the sampler to copy out the current data between the changed points;
	 4) Resample that data over the new length(the sampler can do this for us);
	 4) Create an action to undoably remove the original section of data from the attribute
	 5) Create an action to undoably insert the new resampled section;
	*/
	MILLIPOINT ChangedLength = GetLengthOfPathSection(&EditPath, FirstIndex, LastIndex);
//	MILLIPOINT OrigLength    = GetLengthOfPathSection(&(OriginalPath->InkPath), FirstIndex, LastIndex);

	// Get the distances to the changed indexes in the original path
	MILLIPOINT OrigStartDist = GetDistanceToPathIndex(&(OriginalPath->InkPath), FirstIndex);
	MILLIPOINT OrigEndDist   = GetDistanceToPathIndex(&(OriginalPath->InkPath), LastIndex);

	if (OrigStartDist == -1 || OrigEndDist == -1)
	{
		ERROR3("Error getting distance to index in OpNodePathEditBlob::EditBrushAttribute");
		return FALSE;
	}
	// we need another sampler to store the section data, needs to be allocated as we're putting
	// it into an action later
	CDistanceSampler* pSection = new CDistanceSampler;
	if (pSection == NULL)
		return FALSE;

	// Ask the sampler for its data section
	BOOL ok = pDistSamp->GetDataSection(OrigStartDist, OrigEndDist, pSection);
	if (!ok)
		return FALSE;
	
	// ask the section to resample itself
	if (!pSection->ReSample(ChangedLength))
	{
		delete pSection;
		return FALSE;
	}
	pSection->SetNumItemsFromArraySize();

	INT32 StartPressIndex = pDistSamp->GetInternalIndexFromDistance(OrigStartDist);
	INT32 EndPressIndex   = pDistSamp->GetInternalIndexFromDistance(OrigEndDist);
	INT32 NumIndexes = EndPressIndex - StartPressIndex;
	if (StartPressIndex == -1 || EndPressIndex == -1 || NumIndexes <= 0)
	{
		delete pSection;
		return FALSE;
	}

	// now make a removepoints action 
	RemovePressurePointsAction* pAction; 
	
	if (RemovePressurePointsAction::Init(this, &UndoActions, pAttrBrush, StartPressIndex, NumIndexes, NULL, &pAction) == AC_FAIL)
		return FALSE;

	// now insert our new points into the attribute
	AddPressurePointsAction* pAddAction;

	// find out how many points we're inserting
	size_t NumAddPoints = pSection->GetNumItems();
	
	if (AddPressurePointsAction::Init(this, &UndoActions, pAttrBrush, pSection, StartPressIndex, NumAddPoints,  &pAddAction) == AC_FAIL)
	{
		delete pSection;
		return FALSE;
	}
 
	return TRUE;
}




/********************************************************************************************

>	MILLIPOINT OpNodePathEditBlob::GetLengthOfPathSection(Path* pPath, INT32 FirstIndex, INT32 LastIndex)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/2000
	Inputs:		FirstIndex, LastIndex - the indexes marking the start and end of the section
				of path we are interested in
				pPath - the path we are interested in
	Returns:	The distance along the pPath between the two indexes, or -1 if something went wrong
	Purpose:	As above, if you want to know the distance between two points on the edit path
				then this function is for you.

	Notes:		I can't believe no-one has written a Path function to do this.  There must be 
				a better way to do this but currently I must get the two coordinates corresponding
				to the indexes, and call GetDistanceToPoint for both of them and do a subtraction.
				I will shift this into Path at some point but I don't want to hang around for
				the rebuild right now. 

********************************************************************************************/

MILLIPOINT OpNodePathEditBlob::GetLengthOfPathSection(Path* pPath, INT32 FirstIndex, INT32 LastIndex)
{
	ERROR2IF(pPath == NULL, -1, "Path is NULL in OpNodePathEditBlob::GetLengthOfPathSection");

	// find out how many elements there are in EditPath
	INT32 NumElements =	pPath->GetNumCoords();

	// check that our indexes are ok
	if (FirstIndex < 0 || LastIndex < 0 || LastIndex <= FirstIndex || FirstIndex >= NumElements
		|| LastIndex > NumElements)
	{
		ERROR3("Invalid inputs to OpNodePathEditBlob::GetLengthOfEditSection");
		return -1;
	}
	
	// get the coordinates
	DocCoord* pCoords = pPath->GetCoordArray();
	ERROR2IF(pCoords == NULL, -1, "Failed to get coord array in OpNodePathEditBlob::GetLengthOfEditSection");

	DocCoord FirstCoord = pCoords[FirstIndex];
	DocCoord LastCoord  = pCoords[LastIndex];

	// Now get the distances along the path to these points
	MILLIPOINT FirstDistance = -1;
	MILLIPOINT LastDistance  = -1;
	
	BOOL ok = pPath->GetDistanceToPoint(FirstCoord, &FirstDistance);
	if (ok)
		ok = pPath->GetDistanceToPoint(LastCoord, &LastDistance);

	MILLIPOINT SectionLength = -1;
	if (ok)
		SectionLength = LastDistance - FirstDistance;

	return SectionLength;
}


/********************************************************************************************

>	INT32 OpNodePathEditBlob::GetDistanceToPathIndex(Path* pPath, INT32 Index)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/2000
	Inputs:		pPath - the path containing the index
				Index - the index to which we wish to find the distance
	Returns:	The distance along pPath to Index, or -1 if theres a problem
	Purpose:	As above, 

	Notes:		Should really move this into Path at some point

********************************************************************************************/

INT32 OpNodePathEditBlob::GetDistanceToPathIndex(Path* pPath, INT32 Index)
{
	// some checks
	ERROR2IF(pPath == NULL, -1, "Path is NULL in OpNodePathEditBlob::GetDistanceToPathIndex");
	
	if (Index >= pPath->GetNumCoords())
	{
		ERROR3("Invalid index in OpNodePathEditBlob::GetDistanceToPathIndex");
		return -1;
	}

	// get the coordinates
	DocCoord* pCoords = pPath->GetCoordArray();
	ERROR2IF(pCoords == NULL, -1, "Failed to get coord array in OpNodePathEditBlob::GetLengthOfEditSection");

	DocCoord Coord = pCoords[Index];

	MILLIPOINT Distance = -1;
	if (!pPath->GetDistanceToPoint(Coord, &Distance))
		return -1;

	return Distance;
}


/********************************************************************************************

>	BOOL OpNodePathAddEndpoint::Init()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/94
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpNodePathAddEndpoint::Init()
{
	return (RegisterOpDescriptor(	0, 
									_R(IDS_NODEPATH_ADDENDPOINT),
									CC_RUNTIME_CLASS(OpNodePathAddEndpoint), 
									OPTOKEN_ADDENDPOINT,
									OpNodePathAddEndpoint::GetState,
									0,										// help ID
									_R(IDBBL_NODEPATHOP),
									0 ) );									// bitmap ID
}


/********************************************************************************************

>	OpState OpNodePathAddEndpoint::GetState(String_256* Description, OpDescriptor*)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/94
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the selector tool
	Returns:	The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpNodePathAddEndpoint::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}



/********************************************************************************************
>	void OpNodePathAddEndpoint::SetStatusLineHelp()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/95
	Purpose:	Updates the status line message to reflect the current situation
********************************************************************************************/
void OpNodePathAddEndpoint::SetStatusLineHelp()
{
	String_256 HelpString;
	if (EndSnapped)
		HelpString = String_256(_R(IDS_PATHDRAGCREATESHAPE));
	else
	{
		if (SnapToAnother)
			HelpString = String_256(_R(IDS_PATHDRAGJOINPATHS));
		else
			HelpString = String_256(_R(IDS_PATHDRAGADDSEGMENT));
	}

	GetApplication()->UpdateStatusBarText(&HelpString, FALSE);
}



/********************************************************************************************

>	void OpNodePathAddEndpoint::DoStartDragEdit(NodePath* OrigPath, 
												DocCoord Anchor, 
												Spread *pSpread, 
												INT32 PathIndex,
												BOOL CurveOrLine,
												BOOL CloseThePath = FALSE,
												BOOL SmoothOrCusp = TRUE)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com> (via Rik)
	Created:	17/6/94
	Inputs:		OrigPath - Pointer to the path we are about to edit
				Anchor - The position of the mouse at the start of the Drag
				pSpread - The spread that the path is on
				PathIndex is the index into the path of the element to which we're adding
				CurveOrLine is TRUE if we're adding a curve segment, FALSE for a line
				CloseThePath is true if this click is to close the path
				SmoothOrCusp is TRUE if the new point is smooth, FALSE if it is a cusp.
	Purpose:	This is called when a Drag operation has been started when the user wants
				to add a line or curve element to the start or end of an open subpath.
				Anchor is the point we are adding (that's where the user clicked) and
				PathIndex tells us the nearest endpoint in the path. The flag CloseThePath
				tells the operation if it should close the path and select both the start and 
				endpoints to drag around.

********************************************************************************************/

void OpNodePathAddEndpoint::DoStartDragEdit(NodePath* OrigPath, 
											DocCoord Anchor, 
											Spread *pSpread,
											ClickModifiers Mods, 
											INT32 PathIndex,
											BOOL CurveOrLine,
											BOOL CloseThePath,
											BOOL SmoothOrCusp)
{
	// Remember the index of the point to which we're adding this element
	InsertPosition = PathIndex;
	OriginalPath = OrigPath;

	// We had better take a note of the starting point of the drag
	StartMousePos = Anchor;
	LastMousePos = Anchor;
	StartSpread  = pSpread;
	IsPathClosing = CloseThePath;
	AddCurveFlag = CurveOrLine;
	AddSmoothFlag = SmoothOrCusp;
	
	// Now work out the position to constrain to.
	PathFlags*	Flags = OrigPath->InkPath.GetFlagArray();
	PathVerb*	Verbs = OrigPath->InkPath.GetVerbArray();
	DocCoord*	Coords = OrigPath->InkPath.GetCoordArray();
	if (Verbs[InsertPosition] == PT_MOVETO)
		ConstrainPoint = Coords[InsertPosition];
	else
	{
		INT32 Temp = InsertPosition;
		while (!Flags[Temp].IsEndPoint && (Temp < OrigPath->InkPath.GetNumCoords()))
			Temp++;
		ConstrainPoint = Coords[Temp];
	}
	ConstrainPrevPoint = ConstrainPoint;
	ConstrainNextPoint = ConstrainPoint;

	// Constrain the anchor and snap it to the grid
	ERROR3IF(ConstrainPoint == DocCoord(-1,-1),"DragConstrainPoint wasn't set");
	if (Mods.Constrain)
		DocView::ConstrainToAngle(ConstrainPoint, &LastMousePos);
	DocView::SnapCurrent(pSpread, &LastMousePos);

	BOOL ok = TRUE;
	
	// We also need to make a version of the path that we can change
	if (ok)
		ok = BuildEditPath(LastMousePos);
	UpdatePoint = DragPoint;

	// If the path is closing we don't want to start a drag as it leads to complications with snapping to ourselves
	if (CloseThePath)
	{
		if (ok)
			ok = Complete(StartMousePos);
	
		if (!ok)
		{
			InformError();
			FailAndExecute();
		}

		End();
		return;
	}
	else
	{
		// Create the drag cursors
		if (ok)
			ok = CreateCursors();

		// Render the bits of the path that are different
		if (ok)
		{
			DocRect EditPathBBox = EditPath.GetBoundingRect();
//			RenderPathEditBlobs(EditPathBBox, pSpread);

			// And tell the Dragging system that we need drags to happen
			ok = StartDrag(DRAGTYPE_AUTOSCROLL, &EditPathBBox, &LastMousePos);
		}

		if (ok)
			ChangeCursor(pCrossHairCursor);
	}

	if (!ok)
	{
		InformError();
		FailAndExecute();
		End();
	}
}

/********************************************************************************************

>	BOOL OpNodePathAddEndpoint::BuildEditPath(DocCoord NewPoint)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/94
	Input:		NewPoint is the point that's being inserted
	Returns:	TRUE if it managed to build the path, FALSE if it failed
	Purpose:	Builds a copy of the path that we can edit, without destroying the original
				data. This version has to add an extra element which we will be dragging 
				around. It will look at the member variable IsPathClosing to see if the path
				should be made to close.
	Errors:		If it runs out of memory then it will return FALSE

********************************************************************************************/

BOOL OpNodePathAddEndpoint::BuildEditPath(DocCoord NewPoint)
{
	// Find out how long the path is
	INT32 NumCoords = OriginalPath->InkPath.GetNumCoords();

	// Ask for a path with 24 free slots in it to be going on with and fail if we do not get it
	if (!EditPath.Initialise(NumCoords, 24))
		return FALSE;

	// Copy the path data from the original path to the editable one
	if (!EditPath.CopyPathDataFrom(&(OriginalPath->InkPath)))
		return FALSE;

	INT32 LastEndPoint = 0;				// The EndPoint before this one
	INT32 LastLastEndPoint = 0;			// The EndPoint before the last EndPoint
	BOOL SetNextEndPoint = FALSE;		// TRUE if we want to set the next EndPoint to render
	BOOL SetNextNextEndPoint = FALSE;	// TRUE if we want the one after the next one to render

	// Get pointers to the arrays that we are interested in
	PathFlags* Flags = EditPath.GetFlagArray();
	DocCoord* PathCoords = EditPath.GetCoordArray();
	PathVerb* PathVerbs = EditPath.GetVerbArray();

	// Clear all the selected bits in the path because we're adding a new point
	INT32					i;
	for ( i=0; i<NumCoords; i++)
	{
		Flags[i].IsSelected = FALSE;
	}


	// Now add the new element to the correct place in the path. This we get from the
	// member variable InsertPosition.

	if (!InsertElement(NewPoint, &EditPath, InsertPosition, &NewPointIndex))
		return FALSE;

	// Read the length of the path again
	NumCoords = EditPath.GetNumCoords();

	// Get array pointers again, because the paths have moved
	Flags = EditPath.GetFlagArray();
	PathCoords = EditPath.GetCoordArray();
	PathVerbs = EditPath.GetVerbArray();

	// Now check the IsPathClosing flag to see if we should make the path closed
	if (IsPathClosing)
	{
		// When closing the path, we have to snap the new endpoint to the opposite 
		// endpoint. Thus, we need to know if this element was added to the start or end
		// of the path. This we can discover by looking at the previous element to
		// the element at NewPointIndex. If that is a MoveTo then the new point is
		// at the start, and we should snap the MoveTo coordinate to the coord at
		// the end of the path, otherwise it's at the end, and we have to snap the
		// coords of the final element to be the same as the coords of the initial
		// moveto element.

		if (PathVerbs[NewPointIndex-1] == PT_MOVETO)
		{
			// Snap the MoveTo coords to the coords of the endpoint
			for (i = NewPointIndex; i < NumCoords && PathVerbs[i] != PT_MOVETO; i++ );
			// i now points one element above the endpoint of the path, so decrement
			i--;

			// Copy the coords of the endpoint to the moveto
			PathCoords[NewPointIndex-1] = PathCoords[i];

			// And set the closed bit
			PathVerbs[i] |= PT_CLOSEFIGURE;

			// And select the other endpoint so it drags as well
			Flags[i].IsSelected = TRUE;
		}
		else
		{
			// Snap the endpoint coords to those of the moveto at the start, but first
			// set the CloseFigure flag in the verb.
			
			INT32 EndCoord = NewPointIndex;
			
			if ((PathVerbs[NewPointIndex] & ~PT_CLOSEFIGURE) == PT_BEZIERTO)
				EndCoord = NewPointIndex + 2;

			PathVerbs[EndCoord] |= PT_CLOSEFIGURE;

			// scan back to the start of the path
			i = EndCoord;
			do
			{
				i--;
			} while (PathVerbs[i] != PT_MOVETO);

			PathCoords[EndCoord] = PathCoords[i];

			// And select the moveto as well
			Flags[i].IsSelected = TRUE;
		}
	}
	
	EditPath.SmoothCurve();

	// Go though all the coords, with scary amounts of looking back and forwards
	for (i=0; i<NumCoords; i++)
	{
		// Make all the flags FALSE by default
		Flags[i].NeedToRender = FALSE;

		if (Flags[i].IsEndPoint)
		{
			// if the endpoint 2 elements back was selected and the last element was smooth
			// then we need to mark this point for rendering
			if (SetNextNextEndPoint)
			{
				Flags[i].NeedToRender = TRUE;
				SetNextNextEndPoint = FALSE;
			}

			// We have found an Endpoint, do we want to mark this one as renderable
			if (SetNextEndPoint)
			{
				// As the last element was selected, this element needs to render
				Flags[i].NeedToRender = TRUE;
				SetNextEndPoint = FALSE;

				// If the smooth flag is set then the next item needs to render as well
				if (Flags[i].IsSmooth)
					SetNextNextEndPoint = TRUE;
			}

			// If its selected, then its renderable
			if (Flags[i].IsSelected)
			{
				Flags[i].NeedToRender = TRUE;
				Flags[LastEndPoint].NeedToRender = TRUE;

				// If the smooth flag is set then the re-draw area goes further
				if (Flags[LastEndPoint].IsSmooth)
					Flags[LastLastEndPoint].NeedToRender = TRUE;

				// Set the flag for the next endpoint
				SetNextEndPoint = TRUE;
			}

			// Make a note of the last endpoint, incase the next one is selected
			LastLastEndPoint = LastEndPoint;
			LastEndPoint = i;
		}
	}

	// It worked
	return TRUE;
}

/********************************************************************************************

>	void OpNodePathAddEndpoint::DragFinished(DocCoord Pos, 
											ClickModifiers Mods, 
											Spread* pSpread, 
											BOOL Success,
											BOOL bSolidDrag)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		Pos = mouse position at end of drag
				Mods = click modifiers (Adjust, etc.)
				pSpread points at the spread the drag finished in
				Success indicates whether the drag was aborted with Escape
	Outputs:	-
	Returns:	-
	Purpose:	This routine handles the end of the drag operation when adding an element
				to the end of a path. It will insert an element into the nodepath, then
				copy the contents of the temporary path into the nodepath
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpNodePathAddEndpoint::DragFinished(	DocCoord Pos, 
											ClickModifiers Mods, 
											Spread* pSpread, 
											BOOL Success,
											BOOL bSolidDrag)
{
	RenderPathEditBlobs( EditPath.GetBoundingRect(), StartSpread );
	EndDrag();
	DestroyCursors();
	if ( Success )
	{
		if (!Complete(Pos))
		{
			InformError();
			FailAndExecute();
		}
	}
	else
		FailAndExecute();

	End();	
	

}



/********************************************************************************************
>	BOOL OpNodePathAddEndpoint::Complete(DocCoord AddElementPoint)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> - From Jim code
	Created:	2/6/95
	Inputs:		AddElementPoint - the point at which the mouse finished
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	This routine does all the actions, with undo, for appending the additional
				endpoint onto the line, including the preparaty and concluding actions
	Errors:		-
	SeeAlso:	OpNodePathAddEndpoint::CompleteThisPath
********************************************************************************************/
BOOL OpNodePathAddEndpoint::Complete(DocCoord AddElementPoint)
{
	BeginSlowJob();
	BOOL ok = TRUE;
	
	// Start the sel operation, removes blobs at start and end
	if (ok)
		ok = DoStartSelOp(TRUE,TRUE);

	// Create and send a change message about this path edit so parents can refuse
	ObjChangeFlags cFlags;
	cFlags.TransformNode = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,OriginalPath,this);
	if (ok)
	{
		if (!OriginalPath->AllowOp(&ObjChange, TRUE))
		{
			FailAndExecute();
			return TRUE;
		}
	}

	// Do all the work involved in closing the path
	if (ok)
		ok = CompleteThisPath(AddElementPoint);

	// Inform all the parents of this node that it has been changed.
	if (ok)
	{
		ObjChange.Define(OBJCHANGE_FINISHED,ObjChangeFlags(),OriginalPath,this);
		ok = UpdateChangedNodes(&ObjChange);
	}

	return ok;
}



/********************************************************************************************
>	BOOL OpNodePathAddEndpoint::CompleteThisPath(DocCoord AddElementPoint)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> - From Jim code
	Created:	19/12/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	This routine does all the actions, with undo, for appending the additional
				endpoint onto the line.
	Errors:		FailAndExecute will have been called in the event of a failure
	SeeAlso:	OpNodePathAddEndpoint::Complete
********************************************************************************************/
BOOL OpNodePathAddEndpoint::CompleteThisPath(DocCoord AddElementPoint)
{
	BOOL ok = TRUE;

	// Force a re-draw of the place where the path used to be
	if (ok)
		ok = (RecalcBoundsAction::DoRecalc(this, &UndoActions, OriginalPath) != AC_FAIL);

//	// Store the paths sub-selection state
//	if (ok)
//		ok = (StorePathSubSelStateAction::DoRecord(this, &UndoActions, &OriginalPath->InkPath) != AC_FAIL);

	// Snap the endpoint that we are dragging with the other end of the sub-path
	if (ok && EndSnapped)
		SnapEndsTogether();

	// First thing to do is add a point into the path at the same place we added it
	// for the temporary path.
	if (ok)
		ok = InsertElement(AddElementPoint, &(OriginalPath->InkPath), InsertPosition, &NewPointIndex, TRUE, OriginalPath);

	// Copy the edited path back over the original path
	if (ok)
		ok = CopyEditedPathBack();

	if (IsPathClosing)
		EndSnapped = TRUE;
		
	// If the ends snapped, set the filled bit on the path
	if (ok)
		ok = FillPathIfEndsSnapped() && JoinWithOtherPath() ;

	if (ok)
	{
		// Update the bounding rectangle
		OriginalPath->InvalidateBoundingRect();

		// Announce this bounds change to the world
		GetApplication()->FindSelection()->UpdateBounds();

		// Force a redraw of the place where the path is now.
		ok = (RecordBoundsAction::DoRecord(this, &UndoActions, OriginalPath) != AC_FAIL);
	}

	// DY 9/99 are we editing a blend on a curve? If so we may wish to change the number of
	// steps in the blend
	NodeGroup* pParent = GetGroupParentOfCurve();
	
	if (pParent != NULL)
	{
		if (pParent->IS_KIND_OF(NodeBlend))
		{
			if (ok)
				InsertChangeBlendStepsAction((NodeBlend*)pParent);               
		}
	
	}
	ObjChangeFlags cFlags(FALSE,TRUE);
	ObjChangeParam ObjChange(OBJCHANGE_FINISHED,cFlags,NULL,this);
	ok = UpdateChangedNodes(&ObjChange);
	
	return ok;
}


/********************************************************************************************

>	BOOL OpNodePathAddEndpoint::EditBrushAttribute(INT32 FirstIndex, INT32 LastIndex, AttrBrushType* pAttr)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/2000
	inputs :    FirstIndex - first changed index of the path
				LastIndex  - the last changed index
				pAttrBrush - the attribute to change
	outputs:    -
	returns:    TRUE if all went well
	Purpose:	If we use the shape editor to edit a nodepath with an applied brush attribute
				and this brush attribute makes use of sampled pressure or time information then
				the brush needs to resample its data.  So here we will insert a few actions to
				make that happen.
	
	Notes:		Derived version does not do the remove points (as we are only adding here). Instead
				it makes itself some new data and inserts it at the insert point.

*********************************************************************************************/

BOOL OpNodePathAddEndpoint::EditBrushAttribute(INT32 FirstIndex, INT32 LastIndex, AttrBrushType* pAttrBrush)
{
	if (FirstIndex < 0 || LastIndex < 0 || LastIndex <= FirstIndex || pAttrBrush == NULL)
	{
		//ERROR3("Invalid inputs to OpNodePathEditBlob::EditBrushAttribute");
		return FALSE;
	}
	
	// currently we only have pressure implemented, and if this attribute does not use
	// pressure then theres nothing for us to do
	CDistanceSampler* pDistSamp = pAttrBrush->GetPressureCache();
	if (pDistSamp == NULL)
		return TRUE;  // not an error 
	
	// what we basically want to do here is to make a new section of data and insert ot
	// either at the beginning or the end

	MILLIPOINT SectionLength = GetLengthOfPathSection(&EditPath, FirstIndex, LastIndex);
	if (SectionLength == -1) // something went wrong
		return FALSE;
	// work out how many items we need
	INT32 NumItems = SectionLength / MIN_BRUSH_SPACING;

	// make a new CDistanceSampler
	CDistanceSampler* pNewSampler = new CDistanceSampler;
	if (pNewSampler == NULL)
		return FALSE;

	if (!pNewSampler->InitialiseData(NumItems))  // initialise the array 
	{
		delete pNewSampler;
		return FALSE;
	}

	// we are inserting either at the beginning or the end
	INT32 InsertIndex = 0;
	if (FirstIndex != 0)
		InsertIndex = LastIndex;

	// Get the distance to the insert index
	DocCoord* pCoord = EditPath.GetCoordArray();
	if (pCoord == NULL)
	{
		delete pNewSampler;
		return FALSE;
	}
	DocCoord InsertCoord = pCoord[InsertIndex];
	
	MILLIPOINT InsertDistance = -1;
	if (!EditPath.GetDistanceToPoint(InsertCoord, &InsertDistance))
	{
		delete pNewSampler;
		return FALSE;
	}

	// we want to get the pressure at this distance
	UINT32 InternalIndex = pDistSamp->GetInternalIndexFromDistance(InsertDistance);
	
	if (InternalIndex == (UINT32)-1) // we're over the end!
		InternalIndex = UINT32(pDistSamp->GetNumItems() - 1);

	pDistSamp->SetRetrievalSampleRate(1.0);
	CSampleItem TheItem;

	if (!pDistSamp->GetAt(InternalIndex, &TheItem))
	{
		ERROR3("Unable to retrieve pressure item in OpNodePathAddEndpoint::EditBrushAttribute"); 
		delete pNewSampler;
		return FALSE;
	}

	// whew, now we just have to fill up our new sampler with that item
	BOOL ok = pNewSampler->SetAt(0, TheItem);
	while (ok)
	{
		ok = pNewSampler->SetNext(TheItem);
		TRACEUSER( "Diccon", _T("Adding item pressure = %d\n"), TheItem.m_Pressure);
	}

	// now insert our new points into the attribute
	AddPressurePointsAction* pAddAction;

	// find out how many points we're inserting
	size_t NumAddPoints = pNewSampler->GetNumItems() + 1;
	
	if (AddPressurePointsAction::Init(this, &UndoActions, pAttrBrush, pNewSampler, InternalIndex, NumAddPoints,  &pAddAction) == AC_FAIL)
	{
		delete pNewSampler;
		return FALSE;
	}
 
	return TRUE;
}



/********************************************************************************************

>	void OpNodePathEditBlob::ChangeCursor(Cursor* cursor)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/9/94
	Inputs:		Pointer to the cursor to use now.
	Outputs:	-
	Returns:	-
	Purpose:	Changes the mouse pointer to a new shape.
	Errors:		-
	SeeAlso:	OpNodePathEditBlob::CreateCursors(), OpNodePathEditBlob::DestroyCursors()

********************************************************************************************/

void OpNodePathEditBlob::ChangeCursor(Cursor* cursor)
{
	if (cursor != MyCurrentCursor)
	{	// only change if this cursor is different from the current cursor
		if (MyCurrentCursor != NULL)
		{	// If one of our cursors is on the stack then get it off
			CursorStack::GPop(CurrentCursorID);	
		}
		MyCurrentCursor = cursor;
		CurrentCursorID = CursorStack::GPush(cursor);
	}
}	


/********************************************************************************************

>	BOOL OpNodePathEditBlob::CreateCursors()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/9/94
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if the cursors wern't created, TRUE if they were.
	Purpose:	Creates the cursor objects for the path operations.
	Errors:		-
	SeeAlso:	OpNodePathEditBlob::DestroyCursors()

********************************************************************************************/

BOOL OpNodePathEditBlob::CreateCursors()
{
	if (pMoveCursor == NULL)
	{	// If already created then don't create a new set.
		MyCurrentCursor = NULL;
		pMoveCursor = new Cursor(TOOLID_BEZTOOL, _R(IDC_MOVEBEZIERCURSOR));
		pCloseCursor = new Cursor(TOOLID_BEZTOOL, _R(IDC_CLOSEPATHCURSOR));
		pCrossHairCursor = new Cursor(TOOLID_BEZTOOL, _R(IDC_CROSSHAIRCURSOR));
		// See if any of them failed
		if ((!pMoveCursor || !pMoveCursor->IsValid())
				|| (!pCloseCursor || !pCloseCursor->IsValid())
				|| (!pCrossHairCursor || !pCrossHairCursor->IsValid()))
		{	
			// They did fail, so clean up
			TRACE( _T("Cursors not created in OpNodePathEditBlob::CreateCursors\n"));
			delete pMoveCursor;
			delete pCloseCursor;
			delete pCrossHairCursor;
			pMoveCursor = NULL;
			return FALSE;			
		}
	}
	return TRUE;
}



/********************************************************************************************

>	void OpNodePathEditBlob::DestroyCursors()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/9/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Deletes the path operation cursors.
	Errors:		-
	SeeAlso:	OpNodePathEditBlob::CreateCursors()

********************************************************************************************/

void OpNodePathEditBlob::DestroyCursors()
{
	if (pMoveCursor != NULL)
	{	// If one is NULL then the rest don't exist
		if (MyCurrentCursor != NULL)
		{
			CursorStack::GPop(CurrentCursorID);			
		}
		delete pMoveCursor;
		delete pCloseCursor;
		delete pCrossHairCursor;
		pMoveCursor = NULL;
		MyCurrentCursor = NULL;
		CurrentCursorID = 0;
	}
}



/********************************************************************************************

>	BOOL OpNodePathEditBlob::FillPathIfEndsSnapped()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/7/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Will look at the EndSnapped flag and set the IsFilled bit in the OriginalPath
				Builds undo information. Returns FALSE if it couldn't build the Undo info
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpNodePathEditBlob::FillPathIfEndsSnapped()
{
	// If the ends snapped, set the filled bit on the path
	if (EndSnapped && !(OriginalPath->InkPath.IsFilled))
	{
		ModifyFilledAction* pAction;
		if (ModifyFilledAction::Init(this, &UndoActions, TRUE, FALSE, OriginalPath, (Action**)(&pAction))== AC_FAIL)
			return FALSE;
		OriginalPath->InkPath.IsFilled = TRUE;
	}
	return TRUE;
}

/********************************************************************************************

>	BOOL OpNodePathEditBlob::FillPathIfEndsSnapped(NodePath* pEditPath)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> (based upon Jim)
	Created:	17/5/00
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Will look at the EndSnapped flag and set the IsFilled bit in the OriginalPath
				Builds undo information. Returns FALSE if it couldn't build the Undo info
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpNodePathEditBlob::FillPathIfEndsSnapped(NodePath* pOrigPath)
{
	// If the ends snapped, set the filled bit on the path
	if (EndSnapped && !(pOrigPath->InkPath.IsFilled))
	{
		ModifyFilledAction* pAction;
		if (ModifyFilledAction::Init(this, &UndoActions, TRUE, FALSE, pOrigPath, (Action**)(&pAction))== AC_FAIL)
			return FALSE;
		pOrigPath->InkPath.IsFilled = TRUE;
	}
	return TRUE;
}

/********************************************************************************************

>	BOOL OpNodePathAddEndpoint::InsertElement(	DocCoord NewPoint, 
												Path* DestPath, 
												INT32 InsPos,
												INT32* NewPosition,
												BOOL RecordUndo = FALSE,
												NodePath* UndoPath = NULL)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		NewPoint is the point that should be added
				DestPath is the path to which the element should be added
				InsPos is the index of the element which we're adding
				to (the first or last element in the path).
				RecordUndo is a flag telling the routine to build undo information (default FALSE)
				UndoPath is the path that's being operated on, for the undo recording
	Outputs:	NewPosition is the index of the element
				Also sets DragPoint
	Returns:	BOOL indicating success or failure
	Purpose:	Inserts a new element (either line or curve, depending in internal flags)
				into the given path. It will set the selected bit in the element.

				NOTE: This routine will move the path data around so any pointers to the arrays
				in the path should be re-read after any call to this routine.

	Errors:		If no memory available to insert the point
	SeeAlso:	-

********************************************************************************************/

BOOL OpNodePathAddEndpoint::InsertElement(	DocCoord NewPoint, 
											Path* DestPath, 
											INT32 InsPos,
											INT32* NewPosition,
											BOOL RecordUndo,
											NodePath* UndoPath)
{

	Action* UnAction;		// pointer to action that might be created
	ActionCode Act;			// Action code that might be used

	// There are two possibilities here - that we're at the start of a path, or at the
	// end. The end case is easier - we just add an element after the end element. For a
	// point added to the start, we have to change the MoveToCoordinates to be the point
	// clicked, and add an element which goes from there to the original MoveTo position.
	// If we add a CurveTo element, we have to smooth the coordinates before the drag starts.

	DestPath->SetPathPosition(InsPos);

	if (DestPath->GetVerb() == PT_MOVETO)
	{
		DocCoord tempcoord = DestPath->GetEndPoint(); 		// Get coord of moveto
		DestPath->FindNext();								// Skip to after the MoveTo
		*NewPosition = DestPath->GetPathPosition();			// remember index of point we're inserting

		// If we're undoing, create an action for this insert
		if (RecordUndo)
		{
			if (AddCurveFlag)
				Act = RemovePathElementAction::Init(this, &UndoActions, 3,*NewPosition, (Action**)(&UnAction));
			else
				Act = RemovePathElementAction::Init(this, &UndoActions, 1,*NewPosition, (Action**)(&UnAction));
			if (Act == AC_FAIL)
				return FALSE;

			((RemovePathElementAction*)UnAction)->RecordPath(UndoPath);
		}

		PathFlags tempflags;

		if (AddSmoothFlag)
		{
			tempflags.IsSmooth = TRUE;
			tempflags.IsRotate = TRUE;
		}
		else
		{
			tempflags.IsSmooth = TRUE;
			tempflags.IsRotate = FALSE;
		}
		
		if (AddCurveFlag)
		{
			// We insert a curve element with all three coords the same, since 
			// they will get smoothed
			if (!DestPath->InsertCurveTo(tempcoord,tempcoord,tempcoord,&tempflags))
				return FALSE;
		}
		else
		{
			if (!DestPath->InsertLineTo(tempcoord, &tempflags))
				return FALSE;
		}		

		// Read the pointers to the arrays.
		PathFlags* Flags = DestPath->GetFlagArray();
		DocCoord* PathCoords = DestPath->GetCoordArray();
		PathVerb* PathVerbs = DestPath->GetVerbArray();

		// Set the selected bit on the point we have to move (the MoveTo)
		if (RecordUndo)
		{
			DoChangeSelection(UndoPath, InsPos, TRUE);
			if (AddCurveFlag)
				DoChangeSelection(UndoPath, InsPos+1, TRUE);
		}
		else
		{
			Flags[InsPos].IsSelected = TRUE;	
			if (AddCurveFlag)
				Flags[InsPos+1].IsSelected = TRUE;
		}

		// Now to adjust the MoveTo coordinate, recording undo info if necessary

		if (RecordUndo)
		{
			Act = ModifyElementAction::Init(this, 
											&UndoActions,
											PathVerbs[InsPos],
											Flags[InsPos],
											PathCoords[InsPos],
											InsPos,
											UndoPath,
											(Action**)&UnAction);
		}

		PathCoords[InsPos] = NewPoint;

		// Make it smooth/rotate
		Flags[InsPos].IsSmooth = TRUE;
		Flags[InsPos].IsRotate = TRUE;

		DragPoint = InsPos;

		// Now smooth the path
//		DestPath->SmoothCurve();
	}
	else
	{
		// Adding to the end of a path is easier - insert an element after this one
		DestPath->FindNext();
		*NewPosition = DestPath->GetPathPosition();

		// If we're undoing, create an action for this insert
		if (RecordUndo)
		{
			if (AddCurveFlag)
				Act = RemovePathElementAction::Init(this, &UndoActions, 3,*NewPosition, (Action**)(&UnAction));
			else
				Act = RemovePathElementAction::Init(this, &UndoActions, 1,*NewPosition, (Action**)(&UnAction));
			if (Act == AC_FAIL)
				return FALSE;

			((RemovePathElementAction*)UnAction)->RecordPath(UndoPath);
		}

		PathFlags tempflags;
		if (AddSmoothFlag)
		{
			tempflags.IsSmooth = TRUE;
			tempflags.IsRotate = TRUE;
		}
		else
		{
			tempflags.IsSmooth = TRUE;
			tempflags.IsRotate = FALSE;
		}
		if (AddCurveFlag)
		{
			if (!DestPath->InsertCurveTo(NewPoint,NewPoint,NewPoint,&tempflags))
				return FALSE;
		}
		else
		{
			if (!DestPath->InsertLineTo(NewPoint,&tempflags))
				return FALSE;
		}

		// Re-read the pointers to the arrays, in case they've changed
		PathFlags* Flags = DestPath->GetFlagArray();
//		DocCoord* PathCoords = DestPath->GetCoordArray();
//		PathVerb* PathVerbs = DestPath->GetVerbArray();

		if (AddCurveFlag)
		{
			if (RecordUndo)
			{
				DoChangeSelection(UndoPath,(*NewPosition)+2, TRUE);		// Select the endpoint
				DoChangeSelection(UndoPath, (*NewPosition)+1, TRUE);	// Select the control point as well
			}
			else
			{
				Flags[(*NewPosition)+2].IsSelected = TRUE;
				Flags[(*NewPosition)+1].IsSelected = TRUE;
			}
			DragPoint = (*NewPosition)+2;
	//		DestPath->SmoothCurve();
		}
		else
		{
			if (RecordUndo)
				DoChangeSelection(UndoPath,(*NewPosition), TRUE);
			else
				Flags[(*NewPosition)].IsSelected = TRUE;
			DragPoint = *NewPosition;
		}
	}

	return TRUE;
}


/********************************************************************************************
>	OpToggleSmooth::OpToggleSmooth()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/94
	Purpose:	Constructor for toggle smooth/cusp operation
********************************************************************************************/
OpToggleSmooth::OpToggleSmooth()
{
	// Would initialise member variables here if there were any!
}



/********************************************************************************************
>	BOOL OpToggleSmooth::Init()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/94
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations
********************************************************************************************/
BOOL OpToggleSmooth::Init()
{
	return (RegisterOpDescriptor(	0, 
									_R(IDS_NODEPATH_EDIT),
									CC_RUNTIME_CLASS(OpToggleSmooth), 
									OPTOKEN_NODEPATH,
									OpToggleSmooth::GetState,
									0,									// help ID
									_R(IDBBL_NODEPATHOP),
									0 ) );								// bitmap ID
}



/********************************************************************************************
>	OpState OpToggleSmooth::GetState(String_256* Description, OpDescriptor*)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/94
	Outputs:	-
	Returns:	The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:	Find out the state of the operation at the specific time
********************************************************************************************/
OpState OpToggleSmooth::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;		// Ungreyed, unticked
	return Blobby;
}



/********************************************************************************************
>	void OpToggleSmooth::DoTogglePoint(NodePath* ThisNode, INT32 Index, Spread* pSpread, BOOL AllSmooth, BOOL DontMoveOnCusp)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>/Mike/Peter
	Created:	4/7/94
	Inputs:		ThisNode is the nodepath containing the point we want to toggle, if it
				is NULL then all selected paths should be scanned.
				Index is the index into the path of the point clicked - if part of a
				curve, it points at the endpoint, not the first control point.
				If Index is -1 then all selected endpoints on the path should be processed
				pSpread points at the spread containing the path
				AllSmooth is only needed if Index is -1.  If it is then AllSmooth is TRUE
				if all the points should be made smooth, otherwise they are made cusps.
				DontMoveOnCusp is TRUE if the control points shouldn't be moved when making a cusp
	Outputs:	-
	Returns:	-
	Purpose:	This operation will toggle the smoothness or otherwise of a point in a path.
				If the point is smooth, this operation will reset the smoothing bits on any 
				adjacent control points. It will also change their positions, making the point
				visibly a corner. If the point is not smooth, this operation will set the
				smoothing flags on the control points and perform a smoothing operation on the
				points.  If the path consists of a single curve element and the endpoint
				matches the start point, nothing is smoothed since we can't make any guesses
				as to where the control points go.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/
void OpToggleSmooth::DoTogglePoint(NodePath* ThisNode, INT32 Index, Spread* pSpread, BOOL AllSmooth, BOOL DontMoveOnCusp)
{
	if (!DoStartSelOp(TRUE,TRUE))
	{
		FailAndExecute();
		End();
		return;
	}

	// We need to make sure the node or nodes that will be effected will allow this op to happen
	ObjChangeFlags cFlags;
	cFlags.TransformNode = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);
	BOOL Failed = FALSE;

	if (ThisNode==NULL)
	{
		// If there's no node pointer then scan the selection!
		SelRange*	Selected = GetApplication()->FindSelection();
		Node*		pNode = Selected->FindFirst();
		while (pNode!=NULL && !Failed)
		{
			NodePath* pSelected = pNode->IsAnEditablePath();
			if ( (pSelected != NULL) && (pSelected->AllowOp(&ObjChange)) )
			{
				if (!TogglePathPoints(pSelected, Index, AllSmooth, DontMoveOnCusp))
				{
					InformError();
					Failed = TRUE;
				}
			}
			pNode = Selected->FindNext(pNode);
		}
	}
	else
	{
		if (ThisNode->AllowOp(&ObjChange))
		{
			if (!TogglePathPoints(ThisNode, Index, AllSmooth, DontMoveOnCusp))
			{
				InformError();
				Failed = TRUE;
			}
		}
	}

	// Update all the parents of the effected paths.
	if (!Failed)
	{
		ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
		Failed = !UpdateChangedNodes(&ObjChange);
	}

	if (Failed)
		FailAndExecute();

	End();
}



/********************************************************************************************
>	BOOL OpToggleSmooth::TogglePathPoints(NodePath* CurrentNode, INT32 Index, BOOL AllSmooth = FALSE)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/7/94
	Inputs:		ThisNode	is the nodepath containing the point we want to toggle.
				Index		is the index into the path of the point clicked - if part of a
							curve, it points at the endpoint, not the first control point.
							If Index is -1 then all selected endpoints on the path should be
							processed
				AllSmooth 	is only needed if Index is -1.  If it is then AllSmooth is TRUE
							if all the points should be made smooth, otherwise they are made cusps.
	Outputs:	-
	Returns:	FALSE if an error occured
	Purpose:	This operation will toggle the smoothness or otherwise of a point in a path.
				If the point is smooth, this operation will reset the smoothing bits on any 
				adjacent control points. It will also change their positions, making the point
				visibly a corner. If the point is not smooth, this operation will set the
				smoothing flags on the control points and perform a smoothing operation on the
				points.  If the path consists of a single curve element and the endpoint
				matches the start point, nothing is smoothed since we can't make any guesses
				as to where the control points go.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/
BOOL OpToggleSmooth::TogglePathPoints(NodePath* CurrentNode, INT32 Index, BOOL AllSmooth, BOOL DontMoveOnCusp)
{
	ERROR2IF(CurrentNode==NULL, FALSE, "Path pointer was NULL");

	PathFlags* Flags = NULL;
	PathVerb* Verbs = NULL;
	DocCoord* Coords = NULL;
	CurrentNode->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);
	ERROR2IF((Verbs==NULL) || (Coords==NULL) || (Flags==NULL), FALSE, "Path array pointer was NULL (panic!)");

	INT32 NumCoords = CurrentNode->InkPath.GetNumCoords();
	INT32 LowIndex  = (Index == -1) ? 0 : Index;
	INT32 HighIndex = (Index == -1) ? NumCoords-1 : Index;

	// Check to see if this is a single-segment closed curve, in which case we'll never
	// be able to smooth it. We can only be on the moveto...
	if ( (Verbs[0] == PT_MOVETO) && (Verbs[1] == PT_BEZIERTO) && (Verbs[3] & PT_CLOSEFIGURE) &&
		  (Coords[0] == Coords[3]) && (!Flags[0].IsRotate) )
		return TRUE;

	BOOL Success = TRUE;
	BOOL ChangedThisPath = FALSE;
	for (INT32 CurrentIndex = LowIndex; (CurrentIndex <= HighIndex) && Success; CurrentIndex++)
	{
		BOOL Test = FALSE;		// TRUE if this endpoint should be toggled
		CurrentNode->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);

		if (HighIndex != LowIndex)
		{
			if (Flags[CurrentIndex].IsEndPoint && Flags[CurrentIndex].IsSelected)
			{	// Force selected point to be either cusp or smooth
				if (AllSmooth)
					Test = !(Flags[CurrentIndex].IsRotate);
				else
					Test = Flags[CurrentIndex].IsRotate;
			}
			else
				Test = FALSE;
		}
		else
		{	// Toggle single point
			Test = Flags[CurrentIndex].IsEndPoint;
		}
		
		// Toggle the smooth state of this endpoint
		if (Test)
		{
			ERROR3IF(!Flags[CurrentIndex].IsEndPoint, "Attempting to toggle a control point");

			// Force a re-draw of the place where the path used to be
			if (Success && !ChangedThisPath)
			{
				Success = (RecalcBoundsAction::DoRecalc(this, &UndoActions, CurrentNode) != AC_FAIL);
//				// Store the paths sub-selection state
//				if (Success)
//					Success = (StorePathSubSelStateAction::DoRecord(this, &UndoActions, &CurrentNode->InkPath) != AC_FAIL);
			
				ChangedThisPath = TRUE;
			}

			// Now let's adjust the smoothness of this point.
			if (Success)
			{
				CurrentNode->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);
				if (Flags[CurrentIndex].IsRotate)
					Success = MakePointCusp(CurrentNode, CurrentIndex, DontMoveOnCusp);
				else
					Success = MakePointSmooth(CurrentNode, CurrentIndex);
			}
		}
	}

	// Now create a RecordBoundsAction so that the action list has bounds recalculation
	// at both ends which ensures undo and redo both work properly
	if (Success && ChangedThisPath)
	{
		CurrentNode->InvalidateBoundingRect();
		GetApplication()->FindSelection()->UpdateBounds();
		Success = (RecordBoundsAction::DoRecord(this, &UndoActions, CurrentNode) != AC_FAIL);
	}

	return Success;
}



/********************************************************************************************
>	BOOL OpToggleSmooth::MakePointCusp(NodePath* pPath, INT32 Index, BOOL DontMoveOnCusp)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> (made a function from Jim code)
	Created:	10/11/95
	Inputs:		pPath points to the path to modify
				Index - the index of the path to modify
				DontMoveOnCusp - TRUE if the control points should not move, FALSE to position
								 them 1/3 between prev/next endpoint
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Makes the specified endpoint in the path a cusp join.
	Errors:		Parameter checks.  Creates actions which may fail
	SeeAlso:	OpToggleSmooth::MakePointSmooth
********************************************************************************************/
BOOL OpToggleSmooth::MakePointCusp(NodePath* pPath, INT32 Index, BOOL DontMoveOnCusp)
{
	// Parameter checks
	ERROR2IF(pPath == NULL, FALSE, "Path pointer was NULL");
	ERROR2IF(Index < 0, FALSE, "Path index was negative");
	ERROR2IF(Index >= pPath->InkPath.GetNumCoords(), FALSE, "Path index off end of path");
	
	// Store the current state of the endpoint for undo then make it cusp
	ModifyElementAction* UnAction = NULL;
	PathFlags* Flags = NULL;
	PathVerb* Verbs = NULL;
	DocCoord* Coords = NULL;
	pPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);
	ERROR2IF((Verbs==NULL) || (Coords==NULL) || (Flags==NULL), FALSE, "Path array pointer was NULL (panic!)");
	ActionCode Act = ModifyElementAction::Init(this, &UndoActions,	Verbs[Index], Flags[Index],
																Coords[Index], Index, pPath, (Action**)&UnAction);
	if (Act == AC_FAIL)
		return FALSE;
	Flags[Index].IsSmooth = FALSE;			
	Flags[Index].IsRotate = FALSE;

	// Modify the previous control point (if there is one)
	INT32 PrevControl = pPath->InkPath.FindPrevControlPoint(Index);
	if (PrevControl != -1)
	{
		Act = ModifyElementAction::Init(this, &UndoActions,	Verbs[PrevControl],	Flags[PrevControl],
													Coords[PrevControl], PrevControl, pPath, (Action**)&UnAction);
		if (Act == AC_FAIL)
			return FALSE;

		// Move the control point if required and it is possible
		if (!DontMoveOnCusp && (!((Verbs[Index] == PT_MOVETO) && (Verbs[Index+1] == PT_BEZIERTO) &&
			(Verbs[Index+3] & PT_CLOSEFIGURE) && (Coords[Index] == Coords[Index+3]))))
		{
			Coords[PrevControl] = DocCoord::OneThird(Coords[PrevControl+1],Coords[PrevControl-2]);
		}
		Flags[PrevControl].IsSmooth = FALSE;
		Flags[PrevControl].IsRotate = FALSE;

		// Cope with the possibility that we have made the start/end of a closed
		// path a cusp - we can tell this if the endpoint associated with the 
		// previous control point is not the same as Index
		if (Index != PrevControl+1)
		{
			Act = ModifyElementAction::Init(this, &UndoActions, Verbs[PrevControl+1], Flags[PrevControl+1],
												Coords[PrevControl+1], PrevControl+1, pPath, (Action**)&UnAction);
			if (Act == AC_FAIL)
				return FALSE;
			Flags[PrevControl+1].IsSmooth = FALSE;
			Flags[PrevControl+1].IsRotate = FALSE;
		}
	}

	// Modify the next control point (if there is one)
	INT32 NextControl = pPath->InkPath.FindNextControlPoint(Index);
	if (NextControl != -1)
	{
		Act = ModifyElementAction::Init(this, &UndoActions, Verbs[NextControl], Flags[NextControl], 
													Coords[NextControl], NextControl, pPath, (Action**)&UnAction);
		if (Act == AC_FAIL)
			return FALSE;

		// Move the control point if required and it is possible
		// If this is a single-segment closed curve, in which case we'll never
		// be able to calculate sensible points. Just clear the rotate bits
		if (!DontMoveOnCusp && !((Verbs[Index] == PT_MOVETO) && (Verbs[Index+1] == PT_BEZIERTO) &&
									(Verbs[Index+3] & PT_CLOSEFIGURE) && (Coords[Index] == Coords[Index+3])))
		{
			Coords[NextControl] = DocCoord::OneThird(Coords[NextControl-1], Coords[NextControl+2]);
		}
		Flags[NextControl].IsSmooth = FALSE;
		Flags[NextControl].IsRotate = FALSE;
	}

	return TRUE;
}



/********************************************************************************************
>	BOOL OpToggleSmooth::MakePointCusp(NodePath* pPath, INT32 Index)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> (made a function from Jim code)
	Created:	10/11/95
	Inputs:		pPath points to the path to modify
				Index - the index of the path to modify
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Makes the specified endpoint in the path a smooth join.  Has to move the 
				control points around the endpoint.
	Errors:		Parameter checks.  Creates actions which may fail
	SeeAlso:	OpToggleSmooth::MakePointCusp
********************************************************************************************/
BOOL OpToggleSmooth::MakePointSmooth(NodePath* pPath, INT32 Index)
{
	// Parameter checks
	ERROR2IF(pPath == NULL, FALSE, "Path pointer was NULL");
	ERROR2IF(Index < 0, FALSE, "Path index was negative");
	ERROR2IF(Index >= pPath->InkPath.GetNumCoords(), FALSE, "Path index off end of path");
	
	// Store the current state of the endpoint for undo then make it smooth
	ModifyElementAction* UnAction = NULL;
	PathFlags* Flags = NULL;
	PathVerb* Verbs = NULL;
	DocCoord* Coords = NULL;
	pPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);
	ERROR2IF((Verbs==NULL) || (Coords==NULL) || (Flags==NULL), FALSE, "Path array pointer was NULL (panic!)");
	ActionCode Act = ModifyElementAction::Init(this, &UndoActions, Verbs[Index], Flags[Index],
																Coords[Index], Index, pPath, (Action**)&UnAction);
	if (Act == AC_FAIL)
		return FALSE;
	Flags[Index].IsSmooth = TRUE;			
	Flags[Index].IsRotate = TRUE;

	// Modify the previous control point (if there is one)
	INT32 PrevControl = pPath->InkPath.FindPrevControlPoint(Index);
	if (PrevControl != -1)
	{
		Act = ModifyElementAction::Init(this, &UndoActions, Verbs[PrevControl], Flags[PrevControl],
													Coords[PrevControl], PrevControl, pPath, (Action**)&UnAction);
		if (Act == AC_FAIL)
			return FALSE;
		Flags[PrevControl].IsSmooth = TRUE;
		Flags[PrevControl].IsRotate = TRUE;
		Coords[PrevControl] = pPath->InkPath.SmoothControlPoint(PrevControl);

		// if PrevControl+1 != Index we must be smoothing the joined ends
		// of a closed subpath, so we'll have to set the smoothing bits on this point too
		if (Index != PrevControl+1)
		{
			Act = ModifyElementAction::Init(this, &UndoActions, Verbs[PrevControl+1], Flags[PrevControl+1],
												Coords[PrevControl+1], PrevControl+1, pPath, (Action**)&UnAction);
			if (Act == AC_FAIL)
				return FALSE;
			Flags[PrevControl+1].IsSmooth = TRUE;
			Flags[PrevControl+1].IsRotate = TRUE;
		}
	}

	// Modify the next control point (if there is one)
	INT32 NextControl = pPath->InkPath.FindNextControlPoint(Index);
	if (NextControl != -1)
	{
		Act = ModifyElementAction::Init(this, &UndoActions, Verbs[NextControl], Flags[NextControl],
													Coords[NextControl], NextControl, pPath, (Action**)&UnAction);
		if (Act == AC_FAIL)
			return FALSE;
		Flags[NextControl].IsSmooth = TRUE;
		Flags[NextControl].IsRotate = TRUE;
		Coords[NextControl] = pPath->InkPath.SmoothControlPoint(NextControl);
	}

	return TRUE;
}



/********************************************************************************************

>	BOOL OpNewPath::Init()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/94
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpNewPath::Init()
{
	return (RegisterOpDescriptor(	0, 
									_R(IDS_NODEPATH_NEWPATH),
									CC_RUNTIME_CLASS(OpNewPath), 
									OPTOKEN_NODEPATH,
									OpNewPath::GetState,
									0,								// help ID
									_R(IDBBL_NODEPATHOP),
									0 ) );							// bitmap ID
}


/********************************************************************************************

>	OpState OpNewPath::GetState(String_256* Description, OpDescriptor*)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/94
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the selector tool
	Returns:	The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpNewPath::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}



/********************************************************************************************

>	void OpNewPath::DoStartDragEdit(DocCoord FirstPoint,
									DocCoord LastPoint, 
									Spread *pSpread,
									ClickModifiers Mods, 
									BOOL MakeCurve,
									BOOL MakeSmooth)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com> (via Rik)
	Created:	18/7/94
	Inputs:		FirstPoint is the initial point of the path
				LastPoint is the final point in the path
				pSpread points at the spread the path is on
				Mods - the click modifiers
				MakeCurve is TRUE if the new path should be a curve, FALSE for a line
				MakeSmooth is TRUE if the nee path should have rotate flags set, FALSE if not
	Purpose:	This is called when a Drag operation has been started when the user wants
				to add a new path. FirstPoint comes from the floating endpoint, and LastPoint
				comes from the new click position.

********************************************************************************************/

void OpNewPath::DoStartDragEdit(DocCoord FirstPoint, DocCoord LastPoint, Spread* pSpread, 
										ClickModifiers Mods, BOOL MakeCurve, BOOL MakeSmooth)
{
	ConstrainPoint = FirstPoint;
	ConstrainPrevPoint = FirstPoint;
	ConstrainNextPoint = FirstPoint;
	// Constrain the anchor and snap it to the grid
	ERROR3IF(ConstrainPoint == DocCoord(-1,-1),"DragConstrainPoint wasn't set");
	if (Mods.Constrain)
		DocView::ConstrainToAngle(ConstrainPoint, &LastPoint);
	DocView::SnapCurrent(pSpread, &LastPoint);

	// We had better take a note of the starting point of the drag
	StartMousePos = FirstPoint;
	LastMousePos = LastPoint;
	StartSpread  = pSpread;
	AddCurveFlag = MakeCurve;
	AddSmoothFlag = MakeSmooth;

	BOOL ok = CreateCursors();
	
	// We also need to make a version of the path that we can change
	if (ok)
		ok = BuildEditPath(FirstPoint, LastPoint);

	// Set UpdatePoint so the position of the dragged point is shown in the infobar
	if (AddCurveFlag)
		UpdatePoint = 3;
	else
		UpdatePoint = 1;

	// Start the drag
	if (ok)
	{
		// Render the bits of the path that are different
		DocRect EditPathBBox = EditPath.GetBoundingRect();
//		RenderPathEditBlobs(EditPathBBox, pSpread);

		// And tell the Dragging system that we need drags to happen
		ok = StartDrag(DRAGTYPE_AUTOSCROLL, &EditPathBBox, &LastMousePos);
	}

	if (!ok)
	{
		InformError();
		FailAndExecute();
		End();
	}
}

/********************************************************************************************

>	BOOL OpNewPath::BuildEditPath(DocCoord FirstPoint, DocCoord LastPoint)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/94
	Input:		FirstPoint is the start of the path
				Lastpoint is the end of the path
	Returns:	TRUE if it managed to build the path, FALSE if it failed
	Purpose:	Builds a new path consisting of one curve or line element.
	Errors:		If it runs out of memory then it will return FALSE

********************************************************************************************/

BOOL OpNewPath::BuildEditPath(DocCoord FirstPoint, DocCoord LastPoint)
{
	// We only need 4 elements in the path to begin with
	if (!EditPath.Initialise(4, 4))
		return FALSE;

	// Create the path.
	EditPath.InsertMoveTo(FirstPoint);
	if (AddCurveFlag)
		EditPath.InsertCurveTo(LastPoint, LastPoint, LastPoint);
	else
		EditPath.InsertLineTo(LastPoint);

	// Set the smoothing bits on all points
	PathFlags* Flags = EditPath.GetFlagArray();
	INT32 NumCoords = EditPath.GetNumCoords();
	for (INT32 i=0;i<NumCoords;i++)
	{
		Flags[i].IsSmooth = TRUE;
		Flags[i].IsRotate = AddSmoothFlag;
	}
	
	// Now select the endpoint and the previous control point
	if (AddCurveFlag)
	{
		Flags[2].IsSelected = Flags[3].IsSelected = TRUE;
		Flags[0].NeedToRender = Flags[3].NeedToRender = TRUE;
	}
	else
	{
		Flags[1].IsSelected = TRUE;
		Flags[0].NeedToRender = Flags[1].NeedToRender = TRUE;
	}
	
	EditPath.SmoothCurve();

	// It worked
	return TRUE;
}

/********************************************************************************************

>	void OpNewPath::DragFinished(	DocCoord Pos, 
											ClickModifiers Mods, 
											Spread* pSpread, 
											BOOL Success,
											BOOL bSolidDrag)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		Pos = mouse position at end of drag
				Mods = click modifiers (Adjust, etc.)
				pSpread points at the spread the drag finished in
				Success indicates whether the drag was aborted with Escape
	Outputs:	-
	Returns:	-
	Purpose:	This routine handles the end of the drag operation when adding a new
				path to the tree.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpNewPath::DragFinished(DocCoord Pos, ClickModifiers Mods, Spread* pSpread, BOOL Success, BOOL bSolidDrag)
{
	// Stop the drag
	RenderPathEditBlobs( EditPath.GetBoundingRect(), StartSpread );
	EndDrag();
	DestroyCursors();
	BeginSlowJob();

	BOOL ok = TRUE;
	NodePath* NewPath = NULL;
	Document* pDoc = GetWorkingDoc();
	ERROR3IF(pDoc == NULL, "Null Document while setting attributes for new NodePath");
	
	if (Success && (pDoc != NULL))
	{
		ok = DoStartSelOp(FALSE);
		
		// Create a path to hold the data
		NodePath* NewPath = new NodePath;
		ok = (NewPath != NULL);

		// Initialise the path
		if (ok)
			ok = NewPath->SetUpPath(24, 12);
		
		// Copy the data from the edit path to the new path
		if (ok)
			ok = NewPath->InkPath.CopyPathDataFrom(&EditPath);
		
		// Apply attributes to the new node
		if (ok)
			ok = pDoc->GetAttributeMgr().ApplyCurrentAttribsToNode((NodeRenderableInk*)NewPath);

		// Insert the new node into the tree
		if (ok)
			ok = DoInsertNewNode(NewPath, pSpread, TRUE);

		// Here we have to clear the floating endpoint condition that prevails in the line tool.
		// To avoid grief in tool DLL builds lets just broadcast a message saying an new
		// path has been created.  The line tool picks up this message and clears its floating endpoint 
		if (ok)
			BROADCAST_TO_ALL(NewPathCreatedMsg(NewPath, this, &UndoActions));
	}
	else
	{
		// Set up the flags that say it all went wrong
		FailAndExecute();
	}

	// Clean up after failure
	if (!ok)
	{
		if (NewPath != NULL)
		{
			NewPath->CascadeDelete();
			delete NewPath;
		}
		FailAndExecute();
	}

	End();		
}


/***********************************************************************************************

>	class JoinShapesBecomeA: public BecomeA

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/96
	Purpose:	This is the class that is passed to other nodes when the combine op gets them to 
				to turn into other types via their DoBecomeA() method

***********************************************************************************************/

class JoinShapesBecomeA : public BecomeA
{
CC_DECLARE_MEMDUMP(JoinShapesBecomeA);
public:
	JoinShapesBecomeA(NodePath* pThisJoinedPath) :
				BecomeA(BECOMEA_PASSBACK,CC_RUNTIME_CLASS(NodePath),NULL),
				pJoinedPath(pThisJoinedPath), pLastCreatedByNode(NULL), pLastAttrMap(NULL), pCreatedByAttrMap(NULL) {};

	~JoinShapesBecomeA();

	// This function should be called when Reason == BECOMEA_PASSBACK 
	virtual BOOL PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap);

	CCAttrMap* GetLastAttrMap();

private:
	NodePath* 			pJoinedPath;

	NodeRenderableInk*	pLastCreatedByNode;
	CCAttrMap*			pLastAttrMap;
	CCAttrMap*			pCreatedByAttrMap;
};

CC_IMPLEMENT_MEMDUMP(JoinShapesBecomeA,BecomeA);

/********************************************************************************************

>	JoinShapesBecomeA::~JoinShapesBecomeA()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/96
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	The default destructor.

				This will delete the attr map and the attrs within if it still has one

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

JoinShapesBecomeA::~JoinShapesBecomeA()
{
	if (pLastAttrMap != NULL)
	{
		pLastAttrMap->DeleteAttributes();
		delete pLastAttrMap;
	}

	if (pCreatedByAttrMap != NULL)
		delete pCreatedByAttrMap;
}

/********************************************************************************************

>	CCAttrMap* JoinShapesBecomeA::GetLastAttrMap()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/96
	Inputs:		
	Outputs:	-
	Returns:	ptr to an attr map, or NULL if it fails
	Purpose:	This returns a ptr to an attr map that contains ptrs to the attrs of the last selected object.

				This attr map is the one that's applied to the new joined path, using NodeRenderableInk::ApplyAttributes()

				Do not delete this map or any of the attrs within it.  

	Errors:		-
	SeeAlso:	NodeRenderableInk::ApplyAttributes()

********************************************************************************************/

CCAttrMap* JoinShapesBecomeA::GetLastAttrMap()
{
	CCAttrMap* pAttrMap = pLastAttrMap;

	if (pAttrMap == NULL)
	{
		if (pCreatedByAttrMap == NULL && pLastCreatedByNode != NULL)
		{
			pCreatedByAttrMap = new CCAttrMap(30);
			if (pCreatedByAttrMap != NULL)
			{
				if (!pLastCreatedByNode->FindAppliedAttributes(pCreatedByAttrMap))
				{
					delete pCreatedByAttrMap;
					pCreatedByAttrMap = NULL;
				}
			}
		}

		pAttrMap = pCreatedByAttrMap;
	}

	return pAttrMap;
}

/********************************************************************************************

>	BOOL JoinShapesBecomeA::PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/96
	Inputs:		pNewNode = the node created by the object being made into shapes
				pCreatedByNode = ptr to the node that created the new node
				pAttrMap = ptr to attrs (can be NULL)
	Outputs:	-
	Returns:	TRUE if the new path was used successfully, FALSE if op should be aborted
	Purpose:	This receives the paths from the selected objects and joins them to pJoinedPath.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL JoinShapesBecomeA::PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap)
{
	ERROR3IF(pJoinedPath == NULL,"NULL joined path");
	ERROR3IF(pNewNode == NULL,"NULL new path");
	if (pJoinedPath == NULL || pNewNode == NULL)
		return TRUE;

	if (!pNewNode->IsNodePath())
	{
		ERROR3("Received a node that's not a NodePath");
		return TRUE;
	}

	// If we have an attr map from the last call, delete it and all the attributes it contains
	if (pLastAttrMap != NULL)
	{
		pLastAttrMap->DeleteAttributes();
		delete pLastAttrMap;
	}

	// Update the 'last' vars
	pLastCreatedByNode  = pCreatedByNode;
	pLastAttrMap 		= pAttrMap;

	// Merge the new NodePath into our JoinedPath
	BOOL ok = pJoinedPath->InkPath.MergeTwoPaths(((NodePath*)pNewNode)->InkPath);
	
	// Don't need the supplied NodePath anymore
	pNewNode->CascadeDelete();
	delete pNewNode;
	pNewNode = NULL;

	return ok;
}


/********************************************************************************************

>	OpJoinShapes::OpJoinShapes() 

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpJoinShapes constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
            
OpJoinShapes::OpJoinShapes(): SelOperation()								
{                              
}

 /********************************************************************************************

>	BOOL OpJoinShapes::Init()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpJoinShapes initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpJoinShapes::Init()
{

	BTNOP( JOINSHAPEOP, OpJoinShapes, ARRANGE)
	return TRUE;
//	return (RegisterOpDescriptor(0,
 //							_R(IDS_JOINSHAPEOP),
//							CC_RUNTIME_CLASS(OpJoinShapes),
 //							OPTOKEN_NODEPATH,
 //							OpJoinShapes::GetState,
 //							0,	/* help ID */
 //							_R(IDBBL_NODEPATHOP),
 //							0	/* bitmap ID */)); 

}               
    
/********************************************************************************************

>	OpState	OpJoinShapes::GetState(String_256*, OpDescriptor*)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpJoinShapes
	Purpose:	For finding the OpJoinShapes's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpJoinShapes::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
	String_256 DisableReason; 

	// Ensure that a document exists
	if (Document::GetSelected() == NULL)
	{
		// There is no selected document
		OpSt.Greyed = TRUE;

		// Load reason why operation is disabled
		DisableReason = String_256(_R(IDS_NO_DOC));
	   	*UIDescription = DisableReason;	     
	   	return OpSt;                                 
	}

	OpSt.Greyed = FALSE;

	// Look at the current selection
	SelRange* Selected = GetApplication()->FindSelection();
	Node* pNode = Selected->FindFirst();
	BOOL SelectedInside = FALSE;
	BecomeA MyBecomeA(BECOMEA_TEST, CC_RUNTIME_CLASS(NodePath));
	MyBecomeA.ResetCount();
	
	while (pNode != NULL)
	{
		if (pNode->CanBecomeA(&MyBecomeA))
		{
			if (pNode->FindParentOfSelected() != NULL)
				SelectedInside = TRUE;
		}
		pNode = Selected->FindNext(pNode);
	}
	
	// The operation is disabled if there are less than one path to join
	if (MyBecomeA.GetCount() < 2)
	{
		OpSt.Greyed = TRUE;
		DisableReason = String_256(_R(IDS_JOIN_NEEDS_TWO_PATHS));
		*UIDescription = DisableReason;
	}
	else
	{
		// Greyed out if paths are selected inside.
		if (SelectedInside)
		{
			OpSt.Greyed = TRUE;
			DisableReason = String_256(_R(IDS_GREY_WHEN_SELECT_INSIDE));
			*UIDescription = DisableReason;
		}
	}
	
	return(OpSt);   
}

/********************************************************************************************

>	void OpJoinShapes::Do(OpDescriptor*)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com> (rewritten by Markn 12/1/96)
	Created:	20/7/94
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the Join shapes operation. 
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpJoinShapes::Do(OpDescriptor*)
{   
	BOOL ok = TRUE;
	
	// remember the selection before the operation
	if (ok)
		ok = DoStartSelOp(FALSE,FALSE);

	// Get the current selection
	SelRange* Selected = GetApplication()->FindSelection();

	// Create a new NodePath and insert it into the tree
	Node* pNode = Selected->FindFirst();
	NodePath* pJoinedPath = NULL;
	if (ok) (pJoinedPath = new NodePath);
	if (ok) ok = (pJoinedPath != NULL);
	if (ok) ok = pJoinedPath->SetUpPath();
	if (ok) ok = DoInsertNewNode(pJoinedPath,pNode,PREV,FALSE,FALSE,FALSE,FALSE);

	// JoinBecomeA will receive all the paths created by the selection and merge them into pJoinedPath
	JoinShapesBecomeA JoinBecomeA(pJoinedPath);

	// Now go through the selection, copying path data from selected paths into the new one
	pNode = Selected->FindFirst();
	while (ok && (pNode != NULL))
	{
		// Find the next node in the selection now, because this one might get hidden
		Node* pNextNode = Selected->FindNext(pNode);

		// Ask each node that can create a NodePath to do so via DoBecomeA.
		if (pNode != NULL && pNode->IsAnObject() && pNode->CanBecomeA(&JoinBecomeA))
			ok = ((NodeRenderableInk*)pNode)->DoBecomeA(&JoinBecomeA);

		// Next node
		pNode = pNextNode;
	}

	// Apply the attributes of the last node to generate a NodePath
	if (ok)
	{
		CCAttrMap* pAttrMap = JoinBecomeA.GetLastAttrMap();
		if (pAttrMap != NULL)
			pJoinedPath->ApplyAttributes(pAttrMap,TRUE);
	}

	// Hide all the selected nodes
	pNode = Selected->FindFirst();
	while (ok && pNode != NULL)
	{
		Node* pNextNode = Selected->FindNext(pNode);

		if (pNode != NULL)
			ok = DoHideNode(pNode,TRUE);

		// Next node
		pNode = pNextNode;
	}

	// Select the node, ensure that it's bounds are recalculated, and redraw it
	if (ok) pJoinedPath->Select(FALSE);
	if (ok) pJoinedPath->InvalidateBoundingRect();
	if (ok) ok = DoInvalidateNodeRegion(pJoinedPath,TRUE);

	// Inform any error that may occur
	if (!ok)
	{
		InformError();
		FailAndExecute();
	}

	// Tell the application to refresh the selection
	GetApplication()->UpdateSelection();
		
	End();
}			

/*	
void OpJoinShapes::Do(OpDescriptor*)
{   
	BOOL ok = TRUE;
	
	// remember the selection before the operation
	if (ok)
		ok = DoStartSelOp(FALSE,FALSE);

	// Now make shapes of the current selection
	SelRange* Selected = GetApplication()->FindSelection();
	if (ok)
		ok = DoMakeShapes(*Selected);
	
	// First find the *last* pathnode selected,  Selected inside paths are ignored
	NodePath* LastSelected = NULL;
	Node* pNode = Selected->FindFirst();
	INT32 count = 0;
	if (ok)
	{
		while (pNode != NULL)
		{
			if (IS_A(pNode,NodePath) && (pNode->FindParentOfSelected() == NULL))
			{
				LastSelected = (NodePath*) pNode;
				count++;
			}
			pNode = Selected->FindNext(pNode);
		}
	
		if ((LastSelected == NULL) || (count < 2))
		{
			FailAndExecute();
			End();
			return;
		}
	}

	// In order to maintain the attributes of at least one of the objects being joined
	// make a 'deep' copy of the last selected node which will contain any child
	// attributes. Then add all the other nodes to it.
	Node* pnp;				// Temp, for pointer to path (because NodeCopy takes a Node*)
	LastSelected->NodeCopy(&pnp);	// Make a deep copy of the last selected path
	NodePath* NewPath = (NodePath*)pnp;
	if (ok)
		ok = NewPath != NULL;

	// Now go through the selection, copying path data from selected paths into the new one
	// and hiding the nodes. 
	pNode = Selected->FindFirst();
	while (ok && (pNode != NULL))
	{
		// Find the next node in the selection, 'cos this one might get hidden
		Node* NextNode = Selected->FindNext(pNode);
		// For each node except LastSelected, merge the nodes
		if (IS_A(pNode,NodePath) && (pNode->FindParentOfSelected() == NULL) && (pNode != LastSelected))
		{
			if (!(NewPath->InkPath.MergeTwoPaths(((NodePath*)pNode)->InkPath)))
			{
				NewPath->CascadeDelete();
				delete NewPath;
				ok = FALSE;
			}

			// It's safe to hide the node if this isn't the LastSelected node
			if (ok && (pNode != (Node*)LastSelected))
			{
				if (!DoHideNode(pNode, TRUE))
				{
					NewPath->CascadeDelete();
					delete NewPath;
					ok = FALSE;
				}
			}

		}
		pNode = NextNode;
	}

	// Tell the application to refresh the selection
	GetApplication()->UpdateSelection();
		
	// Now NewPath contains all the selected paths, so we have to insert this node in the tree
	if (ok && !DoInsertNewNode(NewPath, LastSelected, NEXT, TRUE))
	{
		NewPath->CascadeDelete();
		delete NewPath;
		ok = FALSE;
	}

	// And hide the last selected node
	if (ok)
		ok = DoHideNode(LastSelected, TRUE);

	if (!ok)
	{
		InformError();
		FailAndExecute();
	}

	End();
}			
*/
    
/********************************************************************************************

>	OpBreakShapes::OpBreakShapes() 

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpBreakShapes constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
            
OpBreakShapes::OpBreakShapes(): SelOperation()								
{                              
}

 /********************************************************************************************

>	BOOL OpBreakShapes::Init()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpBreakShapes initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpBreakShapes::Init()
{

	BTNOP( BREAKSHAPEOP, OpBreakShapes, ARRANGE)
	return TRUE;
//	return (RegisterOpDescriptor(0,
 //							_R(IDS_JOINSHAPEOP),
//							CC_RUNTIME_CLASS(OpJoinShapes),
 //							OPTOKEN_NODEPATH,
 //							OpJoinShapes::GetState,
 //							0,	/* help ID */
 //							_R(IDBBL_NODEPATHOP),
 //							0	/* bitmap ID */)); 

}               
    
/********************************************************************************************

>	OpState	OpBreakShapes::GetState(String_256*, OpDescriptor*)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpBreakShapes
	Purpose:	For finding the OpBreakShapes's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpBreakShapes::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
	String_256 DisableReason; 

	// Ensure that a document exists
	if (Document::GetSelected() == NULL)
	{
		// There is no slected document
		OpSt.Greyed = TRUE;
		// Load reason why operation is disabled
		DisableReason = String_256(_R(IDS_NO_DOC));
	   	*UIDescription = DisableReason;	     
	   	return OpSt;                                 
	}


	OpSt.Greyed = FALSE;

	// Look at the current selection
	SelRange* Selected = GetApplication()->FindSelection();
	Node* pNode = Selected->FindFirst();

	// Go through the selection until we find a complex path that's not selected inside
	BOOL FoundComplex = FALSE;
	BOOL SelectedInside = FALSE;	// TRUE only if the last selected complex path is selected inside
	while (pNode)
	{
		if (pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodePath))
		{
			// Have we found a complex path?
			FoundComplex 	= (((NodePath*)pNode)->InkPath.IsComplexPath());

			// Set SelectedInside to TRUE if we find a complex path AND it's selected inside.  FALSE otherwise
			// This is important, otherwise the greying out code below won't work correctly.
			SelectedInside 	= (FoundComplex && (pNode->FindParentOfSelected() != NULL));

			// If we find a complex path that's not selected inside another node, so let op be doable
			if (FoundComplex && !SelectedInside)
				break;
		}
		pNode = Selected->FindNext(pNode);
	}
	
	// The operation is disabled if there are no complex paths selected

	if (!FoundComplex)
	{
		OpSt.Greyed = TRUE;
		DisableReason = String_256(_R(IDS_BREAK_NEEDS_COMPLEX));
		*UIDescription = DisableReason;
	}
	else
	{
		// Greyed out if paths are selected inside.
		if (SelectedInside)
		{
			OpSt.Greyed = TRUE;
			DisableReason = String_256(_R(IDS_GREY_WHEN_SELECT_INSIDE));
			*UIDescription = DisableReason;
		}
	}
	
	return(OpSt);   
}

/********************************************************************************************

>	void OpBreakShapes::Do(OpDescriptor*)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/94
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the Break shapes operation. 
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
	
void OpBreakShapes::Do(OpDescriptor*)
{   
	// Obtain the current selections 
	SelRange* Selected = GetApplication()->FindSelection();

	NodePath* NewPath;
	// Now, because we're going to be doing mad things to the selection, we have to make a list
	// of all the selected nodes, so that adding nodes into the tree won't confuse us

	List* NodeList = Selected->MakeListOfNodes();

	NodeListItem* CurItem = (NodeListItem*)(NodeList->GetHead());

	if (!CurItem)
		goto FailAndDeleteList;

	while(CurItem)
	{
		if ((CurItem->pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodePath)) 	&& 	// It's a NodePath 	AND
			(((NodePath*)(CurItem->pNode))->InkPath.IsComplexPath())			&&	// It's complex 	AND
			(CurItem->pNode->FindParentOfSelected() == NULL))						// It's not selected inside
		{
			// get a pointer to the NodePath
			NodePath* ThisPath = (NodePath*)(CurItem->pNode);
			
			// Start at the very beginning (a very good place to start)
			INT32 SubPathIndex = 0;
			INT32 NumCoords = ThisPath->InkPath.GetNumCoords();

			while (SubPathIndex < NumCoords)
			{
				// Create a new path, copy this subpath into it, and link it into the tree
				// Create the path
				NewPath = new NodePath;
				if (!NewPath)
				{
					goto FailAndDeleteList;
				}
				
				// Initialise the path
				if (!NewPath->SetUpPath(24,12))
				{
					InformError(_R(IDS_OUT_OF_MEMORY), _R(IDS_OK));
					goto FailAndDeleteListAndPath;
				}

				// Copy all attributes from the original shape to the new path
				Node* pAttr = ThisPath->FindFirstChild();
				while (pAttr != NULL)
				{
					if (pAttr->IsAnAttribute())
					{
						BOOL ok;
						Node* pAttrCopy;
						CALL_WITH_FAIL(pAttr->NodeCopy(&pAttrCopy), this, ok);
						if (!ok) 
						{
							goto FailAndDeleteListAndPath;
						}
						pAttrCopy->AttachNode(NewPath, FIRSTCHILD);
					}
					pAttr = pAttr->FindNext();
				}

				// Set the filled bit if the original path has the filled bit set
				if (ThisPath->InkPath.IsFilled)
					NewPath->InkPath.IsFilled = TRUE;
				
				// Now copy the current subpath into this object
				// Find the last element in the subpath
				INT32 EndOfSubPath = SubPathIndex;
				ThisPath->InkPath.FindEndOfSubPath(&EndOfSubPath);
				// Skip to the next element (this will give us the number of elements)
				ThisPath->InkPath.FindNext(&EndOfSubPath);

				// Now EndOfSubPath either points at the start of the next subpath, or 
				// the top of the path (i.e. the first free space).

				// Now copy it
				if (!(NewPath->InkPath.CopySectionFrom(ThisPath->InkPath, SubPathIndex, EndOfSubPath-SubPathIndex)))
				{
					InformError(_R(IDS_OUT_OF_MEMORY), _R(IDS_OK));
					goto FailAndDeleteListAndPath;
				}
				
				// Set the index to the start of the next subpath
				SubPathIndex = EndOfSubPath;

				// Now stick it in the tree
				if (!DoInsertNewNode(NewPath, ThisPath, NEXT, TRUE))
				{
					goto FailAndDeleteListAndPath;
				}

				// And that's it - no need to do anything more, just loop to the next subpath
			}

			// Now we've broken up this object, let's hide it
			if (!DoHideNode(ThisPath, TRUE))
				goto FailAndDeleteList;

		}
		CurItem = (NodeListItem*)(NodeList->GetNext(CurItem));
	}
	
	End();

	// delete the nodelist (and all the list items)
	while (!NodeList->IsEmpty())
		delete (NodeListItem*)(NodeList->RemoveHead());
	delete NodeList;

	return;

FailAndDeleteListAndPath:

	NewPath->CascadeDelete();
	delete NewPath;

FailAndDeleteList:
	while (!NodeList->IsEmpty())
		delete (NodeListItem*)(NodeList->RemoveHead());
	delete NodeList;
	FailAndExecute();
	End();
	return;
}



////////////////////////////////////////////////////////////////////////////////////////////
// OpDeletePoints
			

/********************************************************************************************
>	OpDeletePoints::OpDeletePoints() 

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/94
	Purpose:	OpDeletePoints constructor
********************************************************************************************/
OpDeletePoints::OpDeletePoints(): SelOperation()								
{                              
}


/********************************************************************************************
>	BOOL OpDeletePoints::Init()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpDeletePoints initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-
********************************************************************************************/
BOOL OpDeletePoints::Init()
{
	BTNOP( DELETEPOINTSOP, OpDeletePoints, EDIT)
	return TRUE;
}               


    
/********************************************************************************************

>	OpState	OpDeletePoints::GetState(String_256* UIDescription, OpDescriptor* pOpDesc)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com> - latterly Peter
	Created:	27/7/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpDeletePoints
	Purpose:	For finding the OpDeletePoints's state. 
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

OpState	OpDeletePoints::GetState(String_256* UIDescription, OpDescriptor* pOpDesc)
{
	// Look at the current selection
	SelRange* Selected = GetApplication()->FindSelection();
	Node* pNode = Selected->FindFirst();

	// We need to ask the effected nodes if they (and their parents) can handle this node being deleted
	ObjChangeParam ObjDelete(OBJCHANGE_STARTING, ObjChangeFlags(TRUE), NULL, NULL);
	ObjChangeParam ObjChange(OBJCHANGE_STARTING, ObjChangeFlags(), NULL, NULL);

	// Go through the selection until we find a selected point
	BOOL FoundSelected = FALSE;
	while (pNode != NULL)
	{
		if (IS_A(pNode, NodePath) || IS_A(pNode,NodeBlendPath))
		{
			if (((NodePath*)pNode)->InkPath.IsSubSelection())
			{
				// See if this will delete the node or not
				BOOL WillHide = WillDeleteEntirePath((NodePath*)pNode);

				// Check that we can do this op of the node
				BOOL Result = FALSE;
				if (WillHide)
					Result = pNode->AllowOp(&ObjDelete, FALSE);
				else
					Result = pNode->AllowOp(&ObjChange, FALSE);
				
				if (Result)
				{
					// We've found at least one node we can act upon
					FoundSelected = TRUE;	
					break;
				}
			}

		}
		pNode = Selected->FindNext(pNode);
	}
	
	OpState OpSt;
	// The operation is disabled if there are no complex paths selected
	if (!FoundSelected)
	{
		OpSt.Greyed = TRUE;
		*UIDescription = String_256(_R(IDS_NEEDS_SELECTED_POINT));
	}
	
	return(OpSt);   
}



/********************************************************************************************
>	void OpDeletePoints::Do(OpDescriptor*)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com> - latterly Peter
	Created:	22/7/94
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the Delete point(s) operation. 
	Errors:		-
	SeeAlso:	-
********************************************************************************************/
	
void OpDeletePoints::Do(OpDescriptor*)
{   
	// Obtain the current selections 
	SelRange* Selected = GetApplication()->FindSelection();
	Node* pNode = Selected->FindFirst();

	BOOL ok = DoStartSelOp(TRUE,TRUE);

	// We need to ask the effected nodes if they (and their parents) can handle this node being deleted
	ObjChangeFlags cFlags/*(TRUE)*/;
	cFlags.TransformNode = TRUE;
	ObjChangeParam ObjDelete(OBJCHANGE_STARTING,ObjChangeFlags(TRUE),NULL,this);
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

	// loop through the selection
	while (pNode != NULL)
	{
		// First see where the next node is, in case this one gets deleted
		Node* NextNode = Selected->FindNext(pNode);

		// Is the node of interest?
		// we're only interested in NodePaths which have selected points, and that will allow this op to happen
		BOOL DoThisPath = (IS_A(pNode, NodePath) || IS_A(pNode,NodeBlendPath)) && ((NodePath*)pNode)->InkPath.IsSubSelection();

		// Check with the path and it's parents
		BOOL WillDelete = FALSE;
		if (DoThisPath)
		{
			WillDelete = WillDeleteEntirePath((NodePath*)pNode);
			if (WillDelete)
				DoThisPath = pNode->AllowOp(&ObjDelete);
			else
				DoThisPath = pNode->AllowOp(&ObjChange);
		}

		// Do the delete if required
		if (DoThisPath && ok)
		{
			NodePath* ThisPath = (NodePath*)pNode;
			INT32 NumCoords = ThisPath->InkPath.GetNumCoords();
			PathVerb* Verbs = NULL;
			PathFlags* Flags = NULL;
			DocCoord* Coords = NULL;
			ThisPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);
			BOOL PathStillExists = TRUE;

			// Record the bounds and do a redraw
			ok = RecalcBoundsAction::DoRecalc(this, &UndoActions, ThisPath) != AC_FAIL ;

//			// Store the sub-selection state of the path
//			if (ok)
//				ok = (StorePathSubSelStateAction::DoRecord(this, &UndoActions, &ThisPath->InkPath) != AC_FAIL);

			// Simply hide all the path if all of it is to be deleted
			if (WillDelete)
			{
				if (ok)
					ok = DoHideNode(pNode, TRUE);
				PathStillExists = FALSE;
			}
					
			// We have to delete every selected endpoint in the path (if it still exists)
			while (ok && PathStillExists && ThisPath->InkPath.IsSubSelection())
			{
				// First refresh the pointers to the arrays
				ThisPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);
				NumCoords = ThisPath->InkPath.GetNumCoords();

				// Scan for a selected endpoint
				INT32	i;
				for ( i =0;i<NumCoords && (!Flags[i].IsSelected || !Flags[i].IsEndPoint);i++)
					;	// ; deliberate

				// Now, either i == NumCoords or it points at a selected endpoint
				// if i == NumCoords there weren't any selected endpoints, so clear
				// the selected bits on random control points
				if (i == NumCoords)
				{
					for (i=0;i<NumCoords;i++)
					{
						ok = ok && DoChangeSelection(ThisPath, i,FALSE);
					}
				}
				else
				{
					// i indexes into a selected endpoint, which is the one we have to delete.
					// Are we looking at a MoveTo, CurveTo or a LineTo?
					switch (Verbs[i] & ~PT_CLOSEFIGURE)
					{
						case PT_LINETO:
							ok = DeleteLineTo(ThisPath, i, &PathStillExists);
							break;
						case PT_BEZIERTO:
							ok = DeleteBezierTo(ThisPath, i, &PathStillExists);
							break;
						case PT_MOVETO:
							ok = DeleteMoveTo(ThisPath, i, &PathStillExists);
							break;
					}
				}
			}

			// now that we have deleted every selected point on the path we need to check to see if we are 
			// left with a closed path consisting of a moveto followed by a single line or a curve.
			// if we are then we should delete the node.
			if (ok && PathStillExists && !ThisPath->IsPathAllowable())
			{
				if (ok)
					ok = DoInvalidateNodeRegion(ThisPath,TRUE,TRUE);
				if (ok)
					ok = DoHideNode(ThisPath, TRUE);
				PathStillExists = FALSE;
			}

			// Having finished deleting, record the bounds of the new path
			// This is the partner to the RecalcBoundsAction at the start of this Op
			if (PathStillExists && ok)
			{
				ThisPath->InvalidateBoundingRect();
				ok = RecordBoundsAction::DoRecord(this, &UndoActions, ThisPath) != AC_FAIL; 
			}
		}
		pNode = NextNode;
	}

	// Update all the parents of this node
	if (ok)
	{
		ObjChange.Define(OBJCHANGE_FINISHED,ObjChangeFlags(),NULL,this);
		ok = UpdateChangedNodes(&ObjChange);
	}

	if (!ok)
	{
		InformError();
		FailAndExecute();
	}

	GetApplication()->UpdateSelection();

	End();
}



/********************************************************************************************

>	BOOL OpDeletePoints::DeleteLineTo(NodePath* pPath, INT32 Index, BOOL* PathExists)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> - from Jim code
	Created:	27/10/94
	Inputs:		pPath - Pointer to the NodePath to delete the endpoint from.
				Index - The number of the endpoint to delete
				PathExists - pointer to a BOOL for flaging that the whole path has been deleted.
	Outputs:	PathExists - If the entire path has been deleted, *DeletedPath is set to TRUE
	Returns:	FALSE if an error occured during deletion, otherwise TRUE for sucess.
	Purpose:	This is called by OpDeletePoints::Do in order to delete a PT_LINETO endpoint
				from a path.
	SeeAlso:	OpDeletePoints::Do. OpDeletePoints::DeleteMoveTo. OpDeletePoints::DeleteBezierTo
				

********************************************************************************************/
BOOL OpDeletePoints::DeleteLineTo(NodePath* pPath, INT32 Index, BOOL* PathExists)
{
	PathVerb* Verbs = pPath->InkPath.GetVerbArray();
	PathFlags* Flags = pPath->InkPath.GetFlagArray();
	DocCoord* Coords = pPath->InkPath.GetCoordArray();
	INT32 NumCoords = pPath->InkPath.GetNumCoords();
	PathVerb	TempVerb;
	PathFlags	TempFlags;
	DocCoord	EndPoint;

	// work differently depending on if this is the last point in the path
	if (Index+1 != NumCoords)
	{
		// See what the next element is
		switch (Verbs[Index+1] & ~PT_CLOSEFIGURE)
		{
		case PT_LINETO:
			return DoDeletePathSection(pPath, Index, 1) ;
			break;
		case PT_BEZIERTO:
			// for a line next to a bezier, delete the bezier segment
			// and change the line coordinate to go to the end of the bezier
			EndPoint = Coords[Index+3];	// Get coords of bez endpt
			TempVerb = Verbs[Index];
			TempFlags = Flags[Index];
			TempFlags.IsSelected = Flags[Index+3].IsSelected;

			// Change the LineTo element to go to the old end pos of the bezier
			// setting the selection status of the new point
			// Delete whole bezier section
			return (DoAlterPathElement(pPath, Index, EndPoint, TempFlags, TempVerb)
											 && DoDeletePathSection(pPath, Index+1, 3));
			break;
		case PT_MOVETO:
			// Next point is a MoveTo so this is the last point in a subpath
			// See if the path is closed
			if (Verbs[Index] & PT_CLOSEFIGURE)
			{
				// The subpath is closed, so do something clever (see below)
				// As a temporary measure to prevent an infinite loop we'll
				// deselect this point 
				return DoChangeSelection(pPath, Index, FALSE) ;
			}
			else
			{
				// This subpath is open, so we either just delete the element
				// or if the previous element is a MoveTo, we delete that as well
				if (Verbs[Index-1] == PT_MOVETO)
					return DoDeletePathSection(pPath, Index-1, 2);
				else
					return DoDeletePathSection(pPath, Index, 1);
			}
			break;
		default :
			ERROR2(FALSE, "Corrupted path found in DeleteLineTo");
		}
	}
	else
	{
		// Here, the point being deleted is the last point in the path
		// See if the path is being closed
		if (Verbs[Index] & PT_CLOSEFIGURE)
		{
			// We're deleting the last point of a closed path. HELP!
			// This entails deleting the corresponding MoveTo as well
			// Which is quite complicated. I'll do that in a minute.
			// For now, I'll just clear selection
			return DoChangeSelection(pPath, Index, FALSE);
		}
		else
		{
			// This is an open path, so the only worry is if this is the only
			// line element in the path
			if (Verbs[Index-1] == PT_MOVETO)
			{
				// Two possiblilities here, either this is the only path, or 
				// it's part of a subpath.

				if (NumCoords == 2)
				{
					// This is the only path, so delete the whole node
					*PathExists = FALSE;
					return (DoInvalidateNodeRegion(pPath,TRUE,TRUE) && DoHideNode(pPath, TRUE)) ;
				}
				else
				{
					// It's not the only subpath, so just delete these two elements
					return DoDeletePathSection(pPath, Index-1, 2);
				}
			}
			else
			{
				// It's not the only element in the subpath, so just delete it
				return DoDeletePathSection(pPath, Index, 1);
			}
		}
	}
}



/********************************************************************************************

>	BOOL OpDeletePoints::DeleteMoveTo(NodePath* pPath, INT32 Index, BOOL* PathExists)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> - from Jim code
	Created:	27/10/94
	Inputs:		pPath - Pointer to the NodePath to delete the endpoint from.
				Index - The number of the endpoint to delete
				PathExists - pointer to a BOOL for flaging that the whole path has been deleted.
	Outputs:	PathExists - If the entire path has been deleted, *DeletedPath is set to TRUE
	Returns:	FALSE if an error occured during deletion, otherwise TRUE for sucess.
	Purpose:	This is called by OpDeletePoints::Do in order to delete a PT_MOVETO endpoint
				from a path.
	SeeAlso:	OpDeletePoints::Do. OpDeletePoints::DeleteLineTo. OpDeletePoints::DeleteBezierTo
				

********************************************************************************************/
BOOL OpDeletePoints::DeleteMoveTo(NodePath* pPath, INT32 Index, BOOL* PathExists)
{
	PathVerb* Verbs = pPath->InkPath.GetVerbArray();
	PathFlags* Flags = pPath->InkPath.GetFlagArray();
	DocCoord* Coords = pPath->InkPath.GetCoordArray();
	INT32 NumCoords = pPath->InkPath.GetNumCoords();
	PathFlags	TempFlags;
	DocCoord	EndPoint;

	// I think this is slightly simpler than all the others (except for closed)
	// First see if this subpath is closed
	if (pPath->InkPath.IsSubPathClosed(Index))
	{
		// Great, the path is closed, so I'll have to do all sorts of difficult things
		// (yet again, see above)
		// or just deselect it, whichever's easier
		if (!DoChangeSelection(pPath, Index, FALSE))
			return FALSE;

		// Right, time to bite the bullet and do this.
		// This is actually the only occasion when this type of point can be
		// deleted - when a path is closed, the endpoint cannot be selected 
		// without the start point being selected, so we only have to cope with
		// this situation and ensure that we clear the selection bits
		// on the corresponding endpoints

		INT32 EndIndex = Index;
		pPath->InkPath.FindEndOfSubPath(&EndIndex);

		// There are situations where we have got a two element path, a MoveTo, then either
		// a LineTo or a BezierTo.  We must deal with the case of both endpoints being
		// selected and remove the entire sub-path.
		INT32 SubPathElements = EndIndex - Index;
		if (Verbs[EndIndex] == PT_BEZIERTO)
			SubPathElements += 2;

		if (((SubPathElements == 1) && (Flags[Index+1].IsSelected) ) ||
		    ((SubPathElements == 3) && (Flags[Index+3].IsSelected) && ((Verbs[Index+3] & ~PT_CLOSEFIGURE) == PT_BEZIERTO) ) )
		{
			if (SubPathElements == 1)
			{
				return (DoChangeSelection(pPath, Index+1, FALSE) &&
						DoDeletePathSection(pPath, Index, 2) );
			}
			else
			{
				return (DoChangeSelection(pPath, Index+3, FALSE) &&
						DoDeletePathSection(pPath, Index, 4) );
			}
		}

		// What we have to do here is find the last element, and the first element,
		// delete one of them (probably the first one but it might depend on what 
		// type of point they are)

		// Now EndIndex tells us where the end element is
		// Let's see what the two elements are
		PathVerb StartVerb = Verbs[Index+1] & ~PT_CLOSEFIGURE;
		PathVerb EndVerb = Verbs[EndIndex] & ~PT_CLOSEFIGURE;

		if 	( StartVerb == PT_BEZIERTO && EndVerb == PT_BEZIERTO )
		{
			// Both beziers. What we do here is remember the second
			// control point of the first bezier, as well as the endpoint,
			// then change the second control point and the endpoint
			// of the last bezier accordingly
			if ( ! (DoAlterPathElement(pPath, EndIndex+1, Coords[Index+2], Flags[Index+2], Verbs[Index+2]) &&
					// Alter the endpoint, keeping the same verb but changing the other elements
					DoAlterPathElement(pPath, EndIndex+2, Coords[Index+3], Flags[Index+3], Verbs[EndIndex+2]) &&
					// Now alter the MoveTo coordinates
					DoAlterPathElement(pPath, Index, Coords[Index+3], Flags[Index], Verbs[Index]) &&
					// And now delete the first curve element altogether
					DoDeletePathSection(pPath, Index+1, 3)
					) )
			{
				return FALSE;
			}

		}
		else if (StartVerb == PT_LINETO && EndVerb == PT_BEZIERTO)
		{
			// When one element is a line, we always keep the line
			// In this case, we have to change the coords of the 
			// moveto to be the same as the start of the final section,
			// then delete the final section
			if ( ! (DoAlterPathElement(pPath, Index, Coords[EndIndex-1], Flags[Index], Verbs[Index]) &&
					// Alter the element previous to the curve to have the close bit set
					DoAlterPathElement(pPath, EndIndex-1, Coords[EndIndex-1], Flags[EndIndex-1], Verbs[EndIndex-1] | PT_CLOSEFIGURE) &&
					// And delete the curve section
					DoDeletePathSection(pPath, EndIndex, 3)
					) )
			{
				return FALSE;
			}
		}
		else if (StartVerb == PT_BEZIERTO && EndVerb == PT_LINETO)
		{
			// Similar to the previous case, except the lineto is at the end
			// What we do here is set the lineto coord and the moveto coord
			// to the the endpoint of the first (curve) element, making sure the
			// lineto selection bit is clear.
			if ( ! (DoChangeSelection(pPath, EndIndex, Flags[Index+3].IsSelected) &&
					DoAlterPathElement(pPath, EndIndex, Coords[Index+3], Flags[EndIndex], Verbs[EndIndex]) &&
					DoAlterPathElement(pPath, Index, Coords[Index+3], Flags[Index], Verbs[Index]) &&
					// Now delete the curve element
					DoDeletePathSection(pPath, Index+1, 3)
					) )
			{
				return FALSE;
			}
		}
		else
		{
			// start and end must be linetos
			// Alter the final lineto and the moveto to have the coords of
			// the end of the first lineto
			if ( ! (DoAlterPathElement(pPath, Index, Coords[Index+1], Flags[Index], Verbs[Index]) &&
					DoAlterPathElement(pPath, EndIndex, Coords[Index+1], Flags[EndIndex], Verbs[EndIndex]) &&
					// make sure the endpoint isn't selected
					DoChangeSelection(pPath, EndIndex, Flags[Index+1].IsSelected) &&
					// delete the first lineto
					DoDeletePathSection(pPath, Index+1, 1)
					) )
			{
				return FALSE;
			}
		}			
	
		// Now recache the pointers
		Coords = pPath->InkPath.GetCoordArray();
		Verbs = pPath->InkPath.GetVerbArray();
		Flags = pPath->InkPath.GetFlagArray();

		// The last thing I have to do is check the selected status of the new endpoint
		// and select the MoveTo if it is selected, and deselect the moveto otherwise
		// Index is still valid for the start of the path
		EndIndex = Index;
		pPath->InkPath.FindEndOfSubPath(&EndIndex);
		BOOL IsSelected;
		if ((Verbs[EndIndex] & ~PT_CLOSEFIGURE) == PT_BEZIERTO)
			IsSelected = Flags[EndIndex+2].IsSelected;
		else
			IsSelected = Flags[EndIndex].IsSelected;

		// Change the selection of the MoveTo to match the selection
		// of the endpoint in this closed path
		return DoChangeSelection(pPath, Index, IsSelected);
	}
	else
	{
		// The path isn't closed. 
		// Are we looking at the start of a curve or a line?
		if ((Verbs[Index+1] & ~ PT_CLOSEFIGURE) == PT_BEZIERTO)
		{
			// We're deleting this bezier segment. We have to check if this is a lone element
			if (Index+4 == NumCoords || Verbs[Index+4] == PT_MOVETO)
			{
				// It's a lone element, so either delete the subpath or delete the whole node
				if (NumCoords == 4)
				{
					// It's all there is in the path, so delete the whole thing
					*PathExists = FALSE;
					return (DoInvalidateNodeRegion(pPath,TRUE,TRUE) && DoHideNode(pPath, TRUE)) ;
				}
				else
				{
					return DoDeletePathSection(pPath, Index, 4);
				}
			}
			else
			{
				// It's not a lone element, so delete the curveto, and change the MoveTo
				// so that it now moves to the endpoint of the curve
				EndPoint = Coords[Index+3];

				// Look at the selected bit of the end of the curve, and copy that
				// to the moveto, in case that point wanted deleting as well
				BOOL WasSelected = Flags[Index+3].IsSelected;

				// Delete the section containing the curve
				PathFlags TempFlags = Flags[Index];
				TempFlags.IsSelected = WasSelected;
				PathVerb TempVerb = Verbs[Index];
				if (!DoDeletePathSection(pPath, Index+1, 3))
					return FALSE;

				// Alter the MoveTo element so coords and flags are correct
				return DoAlterPathElement(pPath, Index, EndPoint, TempFlags, TempVerb);
			}
		}
		else
		{
			// Since we can't have a moveto following a moveto, the next element
			// has to be a path (let's check that with an ENSURE)
			ERROR2IF(((Verbs[Index+1] & ~PT_CLOSEFIGURE) != PT_LINETO), FALSE, "Badly formed path in DeletePoint");

			// Check if it's a single element
			if (Index+2 == NumCoords || Verbs[Index+2] == PT_MOVETO)
			{
				// It's a lone element so either delete the subpath or the whole path
				// See if this is the only subpath
				if (NumCoords == 2)
				{
					// This is the only element in the path, so hide the whole node
					*PathExists = FALSE;
					return (DoInvalidateNodeRegion(pPath,TRUE,TRUE) && DoHideNode(pPath, TRUE)) ;
				}
				else
				{
					// Just delete this subpath
					return DoDeletePathSection(pPath, Index, 2);
				}
			}
			else
			{
				// There is another element after this, so delete the line element
				// and alter the Moveto element so that it goes to where the endpoint
				// used to go
				EndPoint = Coords[Index+1];

				// Look at the selected bit of the line element
				BOOL WasSelected = Flags[Index+1].IsSelected;

				// Delete the section containing the line
				PathFlags TempFlags = Flags[Index];
				TempFlags.IsSelected = WasSelected;
				PathVerb TempVerb = Verbs[Index];
				if (!DoDeletePathSection(pPath, Index+1, 1))
					return FALSE;

				// Alter the MoveTo so that it goes to the position of the lineto, and it's selected
				// if necessary
				return DoAlterPathElement(pPath, Index, EndPoint, TempFlags, TempVerb);
			}
		}
	}
}




/********************************************************************************************
>	BOOL OpDeletePoints::DeleteBezierTo(NodePath* pPath, INT32 Index, BOOL* PathExists)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> - from Jim code
	Created:	27/10/94
	Inputs:		pPath - Pointer to the NodePath to delete the endpoint from.
				Index - The number of the endpoint to delete
				PathExists - pointer to a BOOL for flaging that the whole path has been deleted.
	Outputs:	PathExists - If the entire path has been deleted, *DeletedPath is set to TRUE
	Returns:	FALSE if an error occured during deletion, otherwise TRUE for sucess.
	Purpose:	This is called by OpDeletePoints::Do in order to delete a PT_BEZIERTO endpoint
				from a path.
	SeeAlso:	OpDeletePoints::Do. OpDeletePoints::DeleteMoveTo. OpDeletePoints::DeleteLineTo
********************************************************************************************/
BOOL OpDeletePoints::DeleteBezierTo(NodePath* pPath, INT32 Index, BOOL* PathExists)
{
	PathVerb* Verbs = pPath->InkPath.GetVerbArray();
//	PathFlags* Flags = pPath->InkPath.GetFlagArray();
//	DocCoord* Coords = pPath->InkPath.GetCoordArray();
	INT32 NumCoords = pPath->InkPath.GetNumCoords();
	PathFlags	TempFlags;
	DocCoord	EndPoint;

	// See if this is the end of the path
	if (Index+1 != NumCoords)
	{
		// See what the next element is
		switch (Verbs[Index+1] & ~ PT_CLOSEFIGURE)
		{
		case PT_LINETO:
			// A curve followed by a line. Just delete the curve.
			return DoDeletePathSection(pPath, Index-2, 3);
			break;
		case PT_BEZIERTO:
			// A curve followed by a curve. First delete the second control point,
			// the endpoint, and the first control point of the next (curve) element
			// Then we should smooth it somehow (later)
			return DoDeletePathSection(pPath, Index-1, 3);
			break;
		case PT_MOVETO:
			// Tricky again, the next point is a MoveTo, so we have to check all the special
			// cases like closed paths, paths with single elements, etc.
			if (Verbs[Index] & PT_CLOSEFIGURE)
			{
				// The path is closed, it's fiddle time again (see above)
				// For now, deselect it
				return DoChangeSelection(pPath, Index, FALSE);
			}
			else
			{
				// At least the path isn't closed. We might safely be able to delete this
				// section, as long as the previous element isn't a MoveTo, in which case
				// we have to delete the whole subpath
				if (Verbs[Index-3] == PT_MOVETO)
				{
					// Delete the whole subpath
					return DoDeletePathSection(pPath, Index-3, 4);
				}
				else
				{
					// Only delete the curve section
					return DoDeletePathSection(pPath, Index-2, 3);
				}
			}
			break;
		default:
			ERROR2(FALSE,"Corrupt path found in DeleteBezierTo");
		}
	}
	else
	{
		// This is the case when we're at the end of the path
		// Is the path a closed one
		if (Verbs[Index] & PT_CLOSEFIGURE)
		{
			// the path is closed. This is still tricky (see above and above)
			// so trick that I'll just deselect it
			return DoChangeSelection(pPath, Index, FALSE);
		}
		else
		{
			// The path isn't closed, so check for this being the only element in
			// the path
			if (Verbs[Index-3] == PT_MOVETO)
			{
				// Either this could be one of many subpaths, or the only subpath
				if (NumCoords == 4)
				{
					// This is the only subpath, it only has one element, so delete
					// the whole path
					*PathExists = FALSE;
					return (DoInvalidateNodeRegion(pPath,TRUE,TRUE) && DoHideNode(pPath, TRUE));
				}
				else
				{
					// There are other subpaths, so just delete this one
					return DoDeletePathSection(pPath, Index-3, 4);
				}
			}
			else
			{
				// It's not the only element in the path so delete it
				return DoDeletePathSection(pPath, Index-2, 3);
			}
		}
	}
}



/********************************************************************************************
>	static BOOL OpDeletePoints::WillDeleteEntirePath(NodePath* pTreePath)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/04/95
	Inputs:		Points to a NodePath in the tree
	Outputs:	-
	Returns:	TRUE if a call to DeletePoints will result in the entire path being deleted
				FALSE if some of the path will remain in the document
	Purpose:	Sees if some of the path will remain after a delete points operation
	Errors:		-
	SeeAlso:	OpDeletePoints::GetState
********************************************************************************************/
BOOL OpDeletePoints::WillDeleteEntirePath(NodePath* pTreePath)
{   
	// If the path has no selected endpoints then it won't be deleted
	if (!pTreePath->InkPath.IsSubSelection())
		return FALSE;

 	// Get the path pointers;
 	INT32 NumCoords = pTreePath->InkPath.GetNumCoords();
	PathVerb* Verbs = NULL;
	DocCoord* Coords = NULL;
	PathFlags* Flags = NULL;
	pTreePath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);
	BOOL PathStillExists = TRUE;

	// We need to scan all the endpoints.  If there are ALL selected then we can 
	// just delete the whole path.
	INT32 loop = 0;
	BOOL Selected = TRUE;
	while (Selected && (loop < NumCoords))
	{
		if (Flags[loop].IsEndPoint && !Flags[loop].IsSelected)
			Selected = FALSE;
		loop++;
	}
	if (Selected)
		return TRUE;

	// Make a copy of the path before testing the delete
	NodePath* pPath = (NodePath*)pTreePath->SimpleCopy();
	if (pPath == NULL)
		return FALSE;

	// We have to delete every selected endpoint in the path
	while (PathStillExists)
	{
		// First refresh the pointers to the arrays
		NumCoords = pPath->InkPath.GetNumCoords();
		pPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);

		// Scan for a selected endpoint
		INT32			i;
		for ( i =0;i<NumCoords && (!Flags[i].IsSelected || !Flags[i].IsEndPoint);i++)
			;	// ; deliberate

		// break out if we didn't find an endpoint
		if (i == NumCoords)
			break;

		// i indexes into a selected endpoint which we have to delete.
		switch (Verbs[i] & ~PT_CLOSEFIGURE)
		{
			case PT_LINETO:
				TryDeleteLineTo(pPath, i, &PathStillExists);
				break;
			case PT_BEZIERTO:
				TryDeleteBezierTo(pPath, i, &PathStillExists);
				break;
			case PT_MOVETO:
				TryDeleteMoveTo(pPath, i, &PathStillExists);
				break;
			default:
				ERROR3("What was that path verb?");
		}
	}

	// now that we have deleted every selected point on the path we need to check 
	// the path to see if is still valid
	if (PathStillExists)
		PathStillExists = pPath->IsPathAllowable();

	delete pPath;

	return !PathStillExists;
}

         

/********************************************************************************************
>	static BOOL OpDeletePoints::TryDeleteLineTo(NodePath* pPath, INT32 Index, BOOL* PathExists)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> - from Jim code
	Created:	29/04/95
	Inputs:		pPath - Pointer to the NodePath to delete the endpoint from.
				Index - The number of the endpoint to delete
				PathExists - pointer to a BOOL for flaging that the whole path has been deleted.
	Outputs:	PathExists - If the entire path has been deleted, *DeletedPath is set to TRUE
	Returns:	FALSE if an error occured during deletion, otherwise TRUE for sucess.
	Purpose:	This is called by OpDeletePoints::WillDeletePath in order to delete a
				PT_LINETO endpoint from a path.
	SeeAlso:	OpDeletePoints::WillDeletePath. OpDeletePoints::DeleteMoveTo.
				OpDeletePoints::DeleteBezierTo
********************************************************************************************/
BOOL OpDeletePoints::TryDeleteLineTo(NodePath* pPath, INT32 Index, BOOL* PathExists)
{
	// Get the path pointers
	INT32 NumCoords = pPath->InkPath.GetNumCoords();
	PathVerb* Verbs = NULL;
	DocCoord* Coords = NULL;
	PathFlags* Flags = NULL;
	pPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);
	
	// work differently depending on if this is the last point in the path
	if (Index+1 != NumCoords)
	{
		// See what the next element is
		switch (Verbs[Index+1] & ~PT_CLOSEFIGURE)
		{
			case PT_LINETO:
				// Delete the current line, createing a line from Index-1 to Index +1
				return (pPath->InkPath.DeleteSection(Index, 1));
				break;
			case PT_BEZIERTO:
				// Delete the folloing curve, creating a line from Index-1 to Index+3
				Flags[Index].IsSelected = Flags[Index+3].IsSelected;
				Coords[Index] = Coords[Index+3];
				return pPath->InkPath.DeleteSection(Index+1, 3);
				break;
			case PT_MOVETO:
				// Next point is a MoveTo so this is the last point in a subpath
				if (Verbs[Index] & PT_CLOSEFIGURE)
				{
					// Subpath is closed, leave it it to the next moveto
					Flags[Index].IsSelected = FALSE;
					return TRUE;
				}
				else
				{
					// This subpath is open, so we either just delete the element
					// or if the previous element is a MoveTo, we delete that as well
					if (Verbs[Index-1] == PT_MOVETO)
						return (pPath->InkPath.DeleteSection(Index-1, 2));
					else
						return (pPath->InkPath.DeleteSection(Index, 1));
				}
				break;
			default :
				ERROR2(FALSE, "Corrupted path found in DeleteLineTo");
		}
	}
	else
	{
		// Here, the point being deleted is the last point in the path
		// See if the path is closed
		if (Verbs[Index] & PT_CLOSEFIGURE)
		{
			// Just deselect the last point
			Flags[Index].IsSelected = FALSE;
			return TRUE;
		}
		else
		{
			if (Verbs[Index-1] == PT_MOVETO)
			{
				if (NumCoords == 2)
				{
					// This is the only path, so delete the whole node
					*PathExists = FALSE;
					return TRUE;
				}
				else
				{
					// It's not the only subpath, so just delete these two elements
					return (pPath->InkPath.DeleteSection(Index-1, 2));
				}
			}
			else
			{
				// It's not the only element in the subpath, so just delete it
				return (pPath->InkPath.DeleteSection(Index, 1));
			}
		}
	}
}



/********************************************************************************************
>	BOOL OpDeletePoints::TryDeleteMoveTo(NodePath* pPath, INT32 Index, BOOL* PathExists)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> - from Jim code
	Created:	27/10/94
	Inputs:		pPath - Pointer to the NodePath to delete the endpoint from.
				Index - The number of the endpoint to delete
				PathExists - pointer to a BOOL for flaging that the whole path has been deleted.
	Outputs:	PathExists - If the entire path has been deleted, *DeletedPath is set to TRUE
	Returns:	FALSE if an error occured during deletion, otherwise TRUE for sucess.
	Purpose:	This is called by OpDeletePoints::WillDeletePath in order to delete a PT_MOVETO endpoint
				from a path.
	SeeAlso:	OpDeletePoints::WillDeletePath. OpDeletePoints::DeleteLineTo. OpDeletePoints::DeleteBezierTo
********************************************************************************************/
BOOL OpDeletePoints::TryDeleteMoveTo(NodePath* pPath, INT32 Index, BOOL* PathExists)
{
	// Get the path pointers
	INT32 NumCoords = pPath->InkPath.GetNumCoords();
	PathVerb* Verbs = NULL;
	DocCoord* Coords = NULL;
	PathFlags* Flags = NULL;
	pPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);

	// See if this subpath is closed
	if (pPath->InkPath.IsSubPathClosed(Index))
	{
		// Deselect it
		Flags[Index].IsSelected = FALSE;

		// Get the index of the other end of the subpath
		INT32 EndIndex = Index;
		pPath->InkPath.FindEndOfSubPath(&EndIndex);

		// There are situations where we have got a two element path, a MoveTo, then either
		// a LineTo or a BezierTo.  We must deal with the case of both endpoints being
		// selected and remove the entire sub-path.
		INT32 SubPathElements = EndIndex - Index;
		if (Verbs[EndIndex] == PT_BEZIERTO)
			SubPathElements += 2;

		if (((SubPathElements == 1) && (Flags[Index+1].IsSelected) ) ||
		    ((SubPathElements == 3) && (Flags[Index+3].IsSelected) && ((Verbs[Index+3] & ~PT_CLOSEFIGURE) == PT_BEZIERTO) ) )
		{
			if (SubPathElements == 1)
			{
				Flags[Index+1].IsSelected = FALSE;
				return (pPath->InkPath.DeleteSection(Index, 2));
			}
			else
			{
				Flags[Index+3].IsSelected = FALSE;
				return (pPath->InkPath.DeleteSection(Index, 4));
			}
		}

		// Now EndIndex tells us where the end element is
		PathVerb StartVerb = Verbs[Index+1] & ~PT_CLOSEFIGURE;
		PathVerb EndVerb = Verbs[EndIndex] & ~PT_CLOSEFIGURE;

		if 	( StartVerb == PT_BEZIERTO && EndVerb == PT_BEZIERTO )
		{
			// Both beziers. What we do here is remember the second
			// control point of the first bezier, as well as the endpoint,
			// then change the second control point and the endpoint
			// of the last bezier accordingly
			Coords[EndIndex+1] = Coords[Index+2];
			Flags[EndIndex+1] = Flags[Index+2];
			Verbs[EndIndex+1] = Verbs[Index+2];

			Coords[EndIndex+2] = Coords[Index+3];
			Flags[EndIndex+2] = Flags[Index+3];

			Coords[Index] = Coords[Index+3];

			return pPath->InkPath.DeleteSection(Index+1, 3);
		}
		else if (StartVerb == PT_LINETO && EndVerb == PT_BEZIERTO)
		{
			// When one element is a line, we always keep the line
			// In this case, we have to change the coords of the 
			// moveto to be the same as the start of the final section,
			// then delete the final section
			Coords[Index] = Coords[EndIndex-1];

			Verbs[EndIndex-1] = Verbs[EndIndex-1] | PT_CLOSEFIGURE;

			Coords[Index] = Coords[Index+3];

			return pPath->InkPath.DeleteSection(EndIndex, 3);
		}
		else if (StartVerb == PT_BEZIERTO && EndVerb == PT_LINETO)
		{
			// Similar to the previous case, except the lineto is at the end
			// What we do here is set the lineto coord and the moveto coord
			// to the the endpoint of the first (curve) element, making sure the
			// lineto selection bit is clear.
			Flags[EndIndex].IsSelected = Flags[Index+3].IsSelected;

			Coords[EndIndex] = Coords[Index+3];

			Coords[Index] = Coords[Index+3];

			return pPath->InkPath.DeleteSection(Index+1, 3);
		}
		else
		{
			// start and end must be linetos
			// Alter the final lineto and the moveto to have the coords of
			// the end of the first lineto
			Coords[Index] = Coords[Index+1];

			Coords[EndIndex] = Coords[Index+1];

			Flags[EndIndex].IsSelected = Flags[Index+1].IsSelected;

			return pPath->InkPath.DeleteSection(Index+1, 1);
		}			
	
		// Recache the pointers
		pPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);

		// The last thing I have to do is check the selected status of the new endpoint
		// and select the MoveTo if it is selected, and deselect the moveto otherwise
		// Index is still valid for the start of the path
		EndIndex = Index;
		pPath->InkPath.FindEndOfSubPath(&EndIndex);
		BOOL IsSelected;
		if ((Verbs[EndIndex] & ~PT_CLOSEFIGURE) == PT_BEZIERTO)
			IsSelected = Flags[EndIndex+2].IsSelected;
		else
			IsSelected = Flags[EndIndex].IsSelected;

		// Change the selection of the MoveTo to match the selection
		// of the endpoint in this closed path
		Flags[Index].IsSelected = IsSelected;
	}
	else
	{
		// The path isn't closed. 
		// Are we looking at the start of a curve or a line?
		if ((Verbs[Index+1] & ~ PT_CLOSEFIGURE) == PT_BEZIERTO)
		{
			// We're deleting this bezier segment. We have to check if this is a lone element
			if (Index+4 == NumCoords || Verbs[Index+4] == PT_MOVETO)
			{
				// It's a lone element, so either delete the subpath or delete the whole node
				if (NumCoords == 4)
				{
					*PathExists = FALSE;
					return TRUE;
				}
				else
					return pPath->InkPath.DeleteSection(Index, 4); 
			}
			else
			{
				Coords[Index] = Coords[Index+3];
				Flags[Index].IsSelected = Flags[Index+3].IsSelected;
				
				return pPath->InkPath.DeleteSection(Index+1, 3);
			}
		}
		else
		{
			// Since we can't have a moveto following a moveto, the next element
			// has to be a path (let's check that with an ENSURE)
			ERROR2IF(((Verbs[Index+1] & ~PT_CLOSEFIGURE) != PT_LINETO), FALSE, "Badly formed path in DeletePoint");

			// Check if it's a single element
			if (Index+2 == NumCoords || Verbs[Index+2] == PT_MOVETO)
			{
				// It's a lone element so either delete the subpath or the whole path
				// See if this is the only subpath
				if (NumCoords == 2)
				{
					*PathExists = FALSE;
					return TRUE;
				}
				else
					return pPath->InkPath.DeleteSection(Index, 2);
			}
			else
			{
				// There is another element after this, so delete the line element
				// and alter the Moveto element so that it goes to where the endpoint
				// used to go
				Coords[Index] = Coords[Index+1];
				Flags[Index].IsSelected = Flags[Index+1].IsSelected;

				return pPath->InkPath.DeleteSection(Index+1, 1);
			}
		}
	}
}



/********************************************************************************************
>	BOOL OpDeletePoints::TryDeleteBezierTo(NodePath* pPath, INT32 Index, BOOL* PathExists)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> - from Jim code
	Created:	27/10/94
	Inputs:		pPath - Pointer to the NodePath to delete the endpoint from.
				Index - The number of the endpoint to delete
				PathExists - pointer to a BOOL for flaging that the whole path has been deleted.
	Outputs:	PathExists - If the entire path has been deleted, *DeletedPath is set to TRUE
	Returns:	FALSE if an error occured during deletion, otherwise TRUE for sucess.
	Purpose:	This is called by OpDeletePoints::WillDeletePath in order to delete a
				PT_BEZIERTO endpoint from a path.
	SeeAlso:	OpDeletePoints::WillDeletePath. OpDeletePoints::DeleteMoveTo.
				OpDeletePoints::DeleteLineTo
********************************************************************************************/
BOOL OpDeletePoints::TryDeleteBezierTo(NodePath* pPath, INT32 Index, BOOL* PathExists)
{
	PathVerb* Verbs = pPath->InkPath.GetVerbArray();
	PathFlags* Flags = pPath->InkPath.GetFlagArray();
//	DocCoord* Coords = pPath->InkPath.GetCoordArray();
	INT32 NumCoords = pPath->InkPath.GetNumCoords();
	PathFlags	TempFlags;
	DocCoord	EndPoint;

	// See if this is the end of the path
	if (Index+1 != NumCoords)
	{
		// See what the next element is
		switch (Verbs[Index+1] & ~PT_CLOSEFIGURE)
		{
			case PT_LINETO:
				// A curve followed by a line. Just delete the curve.
				return pPath->InkPath.DeleteSection(Index-2, 3);
				break;
			case PT_BEZIERTO:
				// A curve followed by a curve. First delete the second control point,
				// the endpoint, and the first control point of the next (curve) element
				return pPath->InkPath.DeleteSection(Index-1, 3);
				break;
			case PT_MOVETO:
				// Tricky again, the next point is a MoveTo, so we have to check all the special
				// cases like closed paths, paths with single elements, etc.
				if (Verbs[Index] & PT_CLOSEFIGURE)
				{
					// The path is closed, For now, deselect it
					Flags[Index].IsSelected = FALSE;
					return TRUE;
				}
				else
				{
					// We might safely be able to delete this
					// section, as long as the previous element isn't a MoveTo, in which case
					// we have to delete the whole subpath
					if (Verbs[Index-3] == PT_MOVETO)
					{
						// Delete the whole subpath
						return pPath->InkPath.DeleteSection(Index-3, 4);
					}
					else
					{
						// Only delete the curve section
						return pPath->InkPath.DeleteSection(Index-2, 3);
					}
				}
				break;
			default:
				ERROR2(FALSE,"Corrupt path found in DeleteBezierTo");
		}
	}
	else
	{
		// This is the case when we're at the end of the path
		if (Verbs[Index] & PT_CLOSEFIGURE)
		{
			// the path is closed. This is still tricky (see above and above)
			Flags[Index].IsSelected = FALSE;
			return TRUE;
		}
		else
		{
			// The path isn't closed, so check for this being the only element in the path
			if (Verbs[Index-3] == PT_MOVETO)
			{
				// Either this could be one of many subpaths, or the only subpath
				if (NumCoords == 4)
				{
					*PathExists = FALSE;
					return TRUE;
				}
				else
				{
					// There are other subpaths, so just delete this one
					return pPath->InkPath.DeleteSection(Index-3, 4);
				}
			}
			else
			{
				// It's not the only element in the path so delete it
				return pPath->InkPath.DeleteSection(Index-2, 3);
			}
		}
	}
}



/////////////////////////////////////////////////////////////////////////////////////////////
// OpReshapeOrAddPoint


/********************************************************************************************

>	void OpReshapeOrAddPoint::DoStartDragEdit(NodePath* OrigPath, DocCoord Anchor,
												    Spread *pSpread, INT32 ControlIndex,
												    double pdist)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		OrigPath - The original Node path object
				Anchor - The position of the mouse at the start of the Drag
				pSpread - the spread that the drag started on
				ControlIndex - The Coord number of the control point that we are dragging
							   this is the index of the first element of the segment plus 1
				pdist - parametric distance along the curve we're dragging from (0 to 1)
	Purpose:	This is called when a Drag operation has been started on the middle of a path.
				It will try and reshape the curve as it's dragging according to the maths in the 
				function RecalculatePath
	SeeAlso:	OpReshapeOrAddPoint::RecalculatePath
				

********************************************************************************************/

void OpReshapeOrAddPoint::DoStartDragEdit(NodePath* OrigPath, DocCoord Anchor,
											    Spread *pSpread, INT32 SegmentIndex, double pdist)
{
	// We had better take a note of the starting point of the drag
	LastMousePos = Anchor;
	StartMousePos = Anchor;
	FurthestPoint = Anchor;
	StartSpread  = pSpread;
	OriginalPath = OrigPath;
	Index = SegmentIndex;
	paramdist = pdist;

	CreateCursors();

	// We also need to make a version of the path that we can change
	if (!BuildEditPath())
	{
		// Failed to get the memory that we needed to copy the path, so moan
		TRACEUSER( "Jim", _T("Failed to get the mem to copy the path data for editing\n") );
		
		// Inform the person doing the clicking that life is not looking so good
		InformError( _R(IDS_OUT_OF_MEMORY), _R(IDS_OK) );
		End();
		return;
	}

	// Render the bits of the path that are different
	DocRect EditPathBBox = EditPath.GetBoundingRect();
	RenderPathEditBlobs(EditPathBBox, pSpread);

	// Record the original positions of the two control points
	DocCoord* Coords = EditPath.GetCoordArray();
	PathVerb* Verbs = EditPath.GetVerbArray();
	if (Verbs[Index] == PT_BEZIERTO)
	{
		OrigControl1 = Coords[Index];
		OrigControl2 = Coords[Index+1];
	}
	else
	{
		ERROR3("Control Point wasn't a control point!");
	}

	// And tell the Dragging system that we need drags to happen
	StartDrag(DRAGTYPE_AUTOSCROLL, &EditPathBBox, &LastMousePos);

	SetStatusLineHelp();
}



/********************************************************************************************

>	void OpReshapeOrAddPoint::DragPointerMove( DocCoord PointerPos, 
												ClickModifiers ClickMods, Spread *pSpread, BOOL bSolidDrag)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods - Which key modifiers are being pressed
				pSpread - the spread that the cursor is over now
	Purpose:	This is called every time the mouse moves, during a drag.
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpReshapeOrAddPoint::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread *pSpread, BOOL bSolidDrag)
{
	EndSnapped = FALSE;		// Always false in this operation
	
	// If drag has moved onto a different spread, convert the coord to be relative to the
	// original spread.
	if (pSpread != StartSpread)
		PointerPos = MakeRelativeToSpread(StartSpread, pSpread, PointerPos);

	// Now snap it to the current grid
	DocView::SnapCurrent(StartSpread,&PointerPos);

	// Rub out the old EORed version of the path
	RenderPathEditBlobs( EditPath.GetBoundingRect(), StartSpread );

	// This is the bit where we go off and re-calculate the paths position,
	// based on how much the mouse has moved
	DocCoord Offset;
	Offset.x = PointerPos.x - StartMousePos.x;
	Offset.y = PointerPos.y - StartMousePos.y;
	RecalculatePath( Offset );

	// Compare this point with the furthest point so far
	if (PointerPos.Distance(StartMousePos) > FurthestPoint.Distance(StartMousePos))
	{
		FurthestPoint.x = PointerPos.x;
		FurthestPoint.y = PointerPos.y;
	}

	// Draw in the new version of the path and update the Last Mouse Position
	LastMousePos = PointerPos;
	RenderPathEditBlobs( EditPath.GetBoundingRect(), StartSpread );

	SetStatusLineHelp();
}


/********************************************************************************************

>	void OpReshapeOrAddPoint::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, BOOL Success, BOOL bSolidDrag)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/94
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickMods - the key modifiers being pressed
				Success - TRUE if the drag was terminated properly, FALSE if it
				was ended with the escape key being pressed
	Purpose:	This is called when a drag operation finishes.
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpReshapeOrAddPoint::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
									   Spread* pSpread, BOOL Success, BOOL bSolidDrag)
{
	RenderPathEditBlobs( EditPath.GetBoundingRect(), StartSpread );
	EndDrag();
	DestroyCursors();
	BeginSlowJob();
	
	BOOL Worked = TRUE;

	if ( Success )
	{
		ObjChangeFlags cFlags;
		cFlags.TransformNode = TRUE;
		ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,OriginalPath,this);

		Worked = DoStartSelOp(TRUE,TRUE) ;

		// Create and send a change message about this path edit
		if (Worked)
		{
			Worked = OriginalPath->AllowOp(&ObjChange) ;
		}

		// If the furthest mouse position is close to the start, add a point instead
		if (Worked)
		{
//			// Store the paths sub-selection state
//			if (Worked)
//				Worked = (StorePathSubSelStateAction::DoRecord(this, &UndoActions, &OriginalPath->InkPath) != AC_FAIL);

			// Due to popular request, I've changed this test so that moving by even a pixel
			// will reshape the line, so a single click has to be a single click to add a 
			// point.
			if (StartMousePos == FurthestPoint)
			{
				Worked = DragFinishedAddPoint(); 
			}
			else
			{
				BOOL Optimise = CopyNeedToRenderFlags() && !(EndSnapped || SnapToAnother);

				// Check we are dealing with a node path. If not, we shouldn't optimise the redraw.
				// Really we should ask the node whether it can cope with optimised redraw. This means
				// whether its correct simply to redraw the changed rectangular section around the moved
				// coordinate. For envelope and perspective moulds whose shapes are govened by derived
				// path objects its not correct to redraw optimally as this can result in sections of the
				// object being left undrawn when the whole surface / contents have changed.

				if (!IS_A(OriginalPath,NodePath))
					Optimise = FALSE;

				// Force a re-draw of the place where the path used to be
				if (RecalcBoundsAction::DoRecalc(this, &UndoActions, OriginalPath, Optimise) == AC_FAIL)
					Worked = FALSE;

				if (Worked && HaveMadeCurve)
				{
					Worked = DragFinishedReshapeLine();
				}

				if (Worked)
				{
					// The drag was a real drag (man) so replace the edited path
					// Go and copy the edited path back over the original path
					if (!CopyEditedPathBack())
					{
//						if (IsUserName("Jim")) TRACE( _T("Failed to copy the edited path back to the original") );
						Error::SetError( _R(IDS_OUT_OF_MEMORY) );
						Worked = FALSE;
					}
				}
		
				// Recalculate the path's bounding box
				OriginalPath->InvalidateBoundingRect();
		
				// tell the world that something in the selection has changed 
				// so that selection bounds are updated
				GetApplication()->UpdateSelection();

				// Force a redraw of the place where the path is now.
				if (Worked && (RecordBoundsAction::DoRecord(this, &UndoActions, OriginalPath, Optimise) == AC_FAIL))
				{
					Worked = FALSE;
				}
			}
		}

		// Inform all the parents of this node that it has been changed.
		if (Worked)
		{
			ObjChange.Define(OBJCHANGE_FINISHED,cFlags,OriginalPath,this);
			Worked = UpdateChangedNodes(&ObjChange);
		}
	}
	else
		FailAndExecute();

	if (!Worked)
	{
//		InformError();
		FailAndExecute();
	}
	End();
}



/********************************************************************************************

>	BOOL OpReshapeOrAddPoint::DragFinishedAddPoint( )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95
	Inputs:		-
	Outputs:	Changes OriginalPath, adding a point when the drag started, creating two new
				path segments, one either side.  Also builds undo information into the operation
				history.
	Returns:	TRUE/FALSE for success/failure.
	Purpose:	This is called when a drag on a path segment finishes and we want to add a new
				point on the segment.
	SeeAlso:	OpReshapeOrAddPoint::DragFinished

********************************************************************************************/
BOOL OpReshapeOrAddPoint::DragFinishedAddPoint( )
{
	// First, find the closest point, the element it's at, and go from there
	INT32 SplitElement = 0;
	PathVerb	NewVerbs[6];
	DocCoord	NewCoords[6];
	PathFlags	NewFlags[6];
	for (INT32 j=0;j<6;j++)
	{
		NewCoords[j].x = 0;
		NewCoords[j].y = 0;
	}
	UINT32 NumElements = 0;
	if (OriginalPath->InkPath.SplitAtPoint(StartMousePos, &SplitElement, &NumElements, NewVerbs, NewCoords))
	{
		// The path was split, so we know where, and how, so let's party on the path
		PathVerb* Verbs = NULL;
		PathFlags* Flags = NULL;
		DocCoord* Coords = NULL;
		OriginalPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);
		ERROR2IF((Verbs == NULL) || (Flags == NULL) || (Coords == NULL), FALSE, "Path array pointer was NULL");
//		UINT32 NumCoords = OriginalPath->InkPath.GetNumCoords();

		// Before we go any further we want to deselect all current points
//		for (UINT32 loop = 0; loop < NumCoords; loop++)
//		{
//			if (!DoChangeSelection(OriginalPath, loop, FALSE))
//				return FALSE;
//		}
		OriginalPath->InkPath.ClearSubSelection();

		INT32 NumToChange = 0;
		// We're adding something, either a line or curve - check which
		if ((Verbs[SplitElement] & ~PT_CLOSEFIGURE) == PT_BEZIERTO)
		{
			// Adding a curve - number to change = 6
			NumToChange = 6;
			// Initialise the flags appropriately
			INT32 i;
			for (i=0;i<3;i++)
			{
				NewFlags[i] = Flags[SplitElement+i];
				NewFlags[i+3] = Flags[SplitElement+i];
			}
			for (i = 1; i <=3; i++)
			{
	 			NewFlags[i].IsSelected = TRUE;		
				NewFlags[i].IsSmooth = FALSE;
				NewFlags[i].IsRotate = TRUE;
			}
			NewFlags[0].IsSmooth = Flags[SplitElement].IsSmooth;
			NewFlags[0].IsRotate = Flags[SplitElement].IsRotate;
			NewFlags[4].IsSmooth = Flags[SplitElement+1].IsSmooth;
			NewFlags[4].IsRotate = Flags[SplitElement+1].IsRotate;
			NewFlags[5].IsSmooth = Flags[SplitElement+2].IsSmooth;
			NewFlags[5].IsRotate = Flags[SplitElement+2].IsRotate;

			if (Verbs[SplitElement+2] & PT_CLOSEFIGURE)
				NewVerbs[5] |= PT_CLOSEFIGURE;
		}
		else
		{
			NumToChange = 2;
			NewFlags[0] = NewFlags[1] = Flags[SplitElement];
			NewFlags[0].IsSmooth = NewFlags[0].IsRotate = FALSE;
			NewFlags[0].IsSelected = TRUE;
			if (Verbs[SplitElement] & PT_CLOSEFIGURE)
				NewVerbs[1] |= PT_CLOSEFIGURE;
		}

		// NumToChange is the number of elements that want changing

		Action* UnAction;
		ActionCode Act = RemovePathElementAction::Init(this, &UndoActions, NumToChange / 2, SplitElement, (Action**)(&UnAction));
		if (Act == AC_FAIL)
		{
			return FALSE;
		}
		((RemovePathElementAction*)UnAction)->RecordPath(OriginalPath);
		PathFlags tempflags;
		tempflags.IsSmooth = TRUE;
		tempflags.IsRotate = TRUE;
		tempflags.IsSelected = TRUE;

		if (NumToChange == 6)
		{
			OriginalPath->InkPath.SetPathPosition(SplitElement);
			if (!(OriginalPath->InkPath.InsertCurveTo(NewCoords[0], NewCoords[1], NewCoords[2], &tempflags)))
			{
				return FALSE;
			}
		}
		else
		{
			if (!(OriginalPath->InkPath.InsertLineTo(NewCoords[0], &tempflags)))
			{
				return FALSE;
			}
		}
		
		// Refresh the path pointers in case things have moved
		OriginalPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);

		// Build an action to record the changes we're about to make to the path
		ModifyPathAction* ModAction;
		Act = ModifyPathAction::Init(this, &UndoActions, NumToChange, (Action**)(&ModAction));
		if (Act == AC_FAIL)
		{
			return FALSE;
		}
		
		if ((Act!=AC_NORECORD) && (ModAction!=NULL))
		{
			PathVerb* ChangedVerbs;
			PathFlags* ChangedFlags;
			DocCoord* ChangedCoords;
			INT32* ChangedIndices;

			ALLOC_WITH_FAIL(ChangedVerbs, (PathVerb*) CCMalloc(NumToChange * sizeof(PathVerb)), this);
			ALLOC_WITH_FAIL(ChangedFlags, (PathFlags*) CCMalloc(NumToChange* sizeof(PathFlags)), this);
			ALLOC_WITH_FAIL(ChangedCoords, (DocCoord*) CCMalloc(NumToChange* sizeof(DocCoord)), this);
			ALLOC_WITH_FAIL(ChangedIndices, (INT32*) CCMalloc(NumToChange* sizeof(INT32)), this);

			if (!ChangedVerbs || !ChangedFlags || !ChangedCoords || !ChangedIndices)
			{
				if (ChangedVerbs) CCFree(ChangedVerbs);
				if (ChangedFlags) CCFree(ChangedFlags);
				if (ChangedCoords) CCFree(ChangedCoords);
				if (ChangedIndices) CCFree(ChangedIndices);
				return FALSE;
			}

			// Now record the arrays...
			for (INT32 i=0; i<NumToChange; i++)
			{
				ChangedIndices[i]	= SplitElement+i;
				ChangedVerbs[i]		= Verbs[SplitElement+i];
				ChangedFlags[i] 	= Flags[SplitElement+i];
				ChangedCoords[i]	= Coords[SplitElement+i];
			}
			
			ModAction->StoreArrays(ChangedVerbs, ChangedFlags, ChangedCoords, ChangedIndices, OriginalPath);
		}

		// Reread the pointers, in case they've changed
		OriginalPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);

		// Now that the undo rigmarole has been done, let's change the path
		for (INT32 i=0; i<NumToChange; i++)
		{
			Verbs[SplitElement+i] = NewVerbs[i];
			Flags[SplitElement+i] = NewFlags[i];
			Coords[SplitElement+i] = NewCoords[i];
		}

		GetApplication()->FindSelection()->UpdateBounds();
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}



/********************************************************************************************

>	BOOL OpReshapeOrAddPoint::DragFinishedReshapeLine( )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95
	Inputs:		-
	Outputs:	Changes OriginalPath, converting the dragged line segment into a curve, then
				reshaping it as required by the drag.
	Returns:	TRUE/FALSE for success/failure.
	Purpose:	This is called when a drag on a path line segment finishes and we want to
				reshape it
	SeeAlso:	OpReshapeOrAddPoint::DragFinished, OpMakeSegmentsCurves::CarryOut

********************************************************************************************/
BOOL OpReshapeOrAddPoint::DragFinishedReshapeLine( )
{
	return OpMakeSegmentsCurves::CarryOut(Index, Index-1, OriginalPath, this, &UndoActions);
}



/********************************************************************************************
>	BOOL OpReshapeOrAddPoint::BuildEditPath()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> - later attacked by Peter
	Created:	17/02/94
	Returns:	TRUE if it managed to build the path, FALSE if it failed
	Purpose:	Builds a copy of the path that we can edit, without destroying the original
				data.  If the dragged segment is a line then it is converted to a curve segment
				Also sets the NeedToRenderFlags for the EOR rendering during the drag.
				The parts of the path need to be rendered during the drag is the segment
				being dragged and the next and previous segmetns if the rotate flags are on
				the endpoints at each end of the dragged segment
	Errors:		If it runs out of memory then it will return FALSE
********************************************************************************************/
BOOL OpReshapeOrAddPoint::BuildEditPath()
{
	// Make a copy of the path
	UINT32 NumCoords = OriginalPath->InkPath.GetNumCoords();
	if (!EditPath.Initialise(NumCoords, 24))
		return FALSE;
	if (!EditPath.CopyPathDataFrom(&(OriginalPath->InkPath)))
		return FALSE;

	// If the drag segment is a line then convert it to a curve
	PathFlags* Flags;
	PathVerb* Verbs;
	DocCoord* Coords;
	EditPath.GetPathArrays(&Verbs, &Coords, &Flags);
	if ((Verbs[Index] & ~PT_CLOSEFIGURE) == PT_LINETO)
	{
		const DocCoord Start = Coords[Index-1];
		const DocCoord End = Coords[Index];
		BOOL IsClosed = OriginalPath->InkPath.GetVerbArray()[Index] & PT_CLOSEFIGURE;
		BOOL IsSelected = OriginalPath->InkPath.GetFlagArray()[Index].IsSelected;

		// Insert a curve segment
		DocCoord OneThird((Start.x+(End.x-Start.x)/3), (Start.y+(End.y-Start.y)/3) );
		DocCoord TwoThird((End.x+(Start.x-End.x)/3), (End.y+(Start.y-End.y)/3) );
		EditPath.SetPathPosition(Index);
		if (!EditPath.InsertCurveTo(OneThird, TwoThird, End))
			return FALSE;

		// Remove the line segment
		if (!EditPath.DeleteSection(Index+3, 1))
			return FALSE;

		// Copy the selection and closure state of the LineTo
		EditPath.GetPathArrays(&Verbs, &Coords, &Flags);
		Flags[Index+2].IsSelected = IsSelected;
		if (IsClosed)
			Verbs[Index+2] = Verbs[Index+2] | PT_CLOSEFIGURE; 

		// We need to recalc paramdist now
		HaveMadeCurve = TRUE;
		INT32 tempel;
		EditPath.SqrDistanceToPoint(StartMousePos, &tempel, &paramdist);
	}

	// Clear the NeedToRender flags
	EditPath.ClearNeedToRender();

	// Set the render flag on this segment
	EditPath.GetPathArrays(&Verbs, &Coords, &Flags);
	if ((Verbs[Index] & ~PT_CLOSEFIGURE) == PT_BEZIERTO)
		Flags[Index+2].NeedToRender = TRUE;
	else
		Flags[Index].NeedToRender = TRUE;

	// Clear the smooth bit on the endpoints of this curve
	Flags[Index-1].IsSmooth = FALSE;
	Flags[Index+2].IsSmooth = FALSE;

	// Check before setting the NeedToRender on the adjacent segments as well
	if ((Verbs[Index] & ~PT_CLOSEFIGURE) == PT_BEZIERTO)
	{
		INT32 Opposite = EditPath.FindOppositeControlPoint(Index);
		if ( (Opposite>=0) && (Flags[Opposite+1].IsRotate || Flags[Index-1].IsRotate) )
			Flags[Opposite+1].NeedToRender = TRUE;

		Opposite = EditPath.FindOppositeControlPoint(Index+1);
		if ( (Opposite>=0) && (Flags[Opposite-1].IsRotate || Flags[Index+2].IsRotate) )
			Flags[Opposite+2].NeedToRender = TRUE;
	}

	// It worked
	return TRUE;
}



/********************************************************************************************

>	void OpReshapeOrAddPoint::RecalculatePath( DocCoord Offset )

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/94
	Inputs:		Offset - The Amount the pointer has moved from the *original* position (i.e. not
						the last position).
	Scope:		Private
	Purpose:	Will reshape a curve segment using a bit of voodoo maths  

				Assuming the mouse was originally clicked on the path a certain distance along it
				(say 1/4 of the whole curve). The parametric distance would thus be 0.25 - this is the 
				distance that controls the first control point. 1-0.25 controls the second control point.

				Given:	(px,py) = current pointer position
						(ox,oy) = original pointer position
						(x0,y0) = first control point
						(x1,y1) = second control point
						(x2,y2) = endpoint of curve
						pdist = parametric distance

				then:	newx0 = x0+(px-ox)*3/pdist
						newy0 = y0+(py-oy)*3/pdist
						newx1 = x1+(px-ox)*3/(1-pdist)
						newy1 = y1+(py-oy)*3/(1-pdist)

				Disclaimer: This maths was plucked out of the blue, and so might not be the best way 
				of doing it.

********************************************************************************************/

void OpReshapeOrAddPoint::RecalculatePath( DocCoord Offset )
{
	// Don't recalculate if the segment is a straight line...
	const double factor = 0.656875;
	PathVerb* Verbs = EditPath.GetVerbArray();
	DocCoord* Coords = EditPath.GetCoordArray();
	PathFlags* Flags = EditPath.GetFlagArray();
	if (Verbs[Index] == PT_BEZIERTO)		// first ctrl pt won't have closefigure set
	{
		// We have to calculate by how much to move the two control points on the curve,
		// and if the endpoints are set to rotate, we have to rotate the opposite control
		// points as well

		// Since the routine CalcRotate will change the *current* coords of the control point,
		// and we calculate the new coords from the *original* control point (to avoid accumulated
		// inaccuracies) we have to generate an offset that CalcRotate can use.
		
		DocCoord ControlOffset;
		INT32 OppositeIndex = EditPath.FindOppositeControlPoint(Index);
		ControlOffset.x = (INT32)(OrigControl1.x + (Offset.x) * factor / paramdist);
		ControlOffset.y = (INT32)(OrigControl1.y + (Offset.y) * factor / paramdist);

		ControlOffset.x -= Coords[Index].x;
		ControlOffset.y -= Coords[Index].y;

		if (Flags[Index-1].IsRotate && OppositeIndex >= 0)
		{
			EditPath.CalcRotate(Coords[Index-1], &Coords[Index], &Coords[OppositeIndex], ControlOffset);
		}
		else
		{
			Coords[Index].x += ControlOffset.x;
			Coords[Index].y += ControlOffset.y;
		}

		// We need to clear the smooth flags off the endpoints and control points around where
		// the path is being dragged.  We know Index-1 to Index+2 are Beziers.
		if ((Index > 1) && ((Verbs[Index-1] & ~PT_CLOSEFIGURE) == PT_BEZIERTO) )
			Flags[Index-2].IsSmooth = FALSE;
		Flags[Index-1].IsSmooth = FALSE;
		Flags[Index].IsSmooth = FALSE;
		Flags[Index+1].IsSmooth = FALSE;
		Flags[Index+2].IsSmooth = FALSE;
		if ((Index < EditPath.GetNumCoords()-3) && ((Verbs[Index+3] & ~PT_CLOSEFIGURE) == PT_BEZIERTO) )
			Flags[Index+3].IsSmooth = FALSE;

		// If the index was at the very start or end of the path then we also need to turn off the
		// rotate flags so future path extension is correct
		if (Index == 1)

		{
			Flags[Index].IsRotate = FALSE;
			Flags[Index-1].IsRotate = FALSE;
		}												  
		if (Index == EditPath.GetNumCoords()-3)
		{
			Flags[Index+1].IsRotate = FALSE;
			Flags[Index+2].IsRotate = FALSE;
		}
	
		ControlOffset.x = (INT32)(OrigControl2.x + (Offset.x) * factor / (1.0-paramdist));
		ControlOffset.y = (INT32)(OrigControl2.y + (Offset.y) * factor / (1.0-paramdist));

		ControlOffset.x -= Coords[Index+1].x;
		ControlOffset.y -= Coords[Index+1].y;
		OppositeIndex = EditPath.FindOppositeControlPoint(Index+1);

		if (Flags[Index+2].IsRotate && OppositeIndex >= 0)
		{
			EditPath.CalcRotate(Coords[Index+2], &Coords[Index+1], &Coords[OppositeIndex], ControlOffset);
		}
		else
		{
			Coords[Index+1].x += ControlOffset.x;
			Coords[Index+1].y += ControlOffset.y;
		}
	}
}


/********************************************************************************************

>	BOOL OpReshapeOrAddPoint::Init()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpReshapeOrAddPoint::Init()
{
	return (RegisterOpDescriptor(
									0, 
									_R(IDS_NODEPATH_EDIT),
									CC_RUNTIME_CLASS(OpReshapeOrAddPoint), 
									OPTOKEN_NODEPATH,
									OpReshapeOrAddPoint::GetState,
									0,										// help ID
									_R(IDBBL_NODEPATHOP),
									0 ) ); 									// bitmap ID
}




/********************************************************************************************

>	OpReshapeOrAddPoint::OpReshapeOrAddPoint()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/03/95
	Purpose:	Constructor to initialise the members variables

********************************************************************************************/

OpReshapeOrAddPoint::OpReshapeOrAddPoint() : OpNodePathEditBlob()
{
	Index = -1;				
	paramdist = -1;		
	HaveMadeCurve = FALSE;		
};



/********************************************************************************************

>	OpState OpReshapeOrAddPoint::GetState(String_256* Description, OpDescriptor*)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the selector tool
	Returns:	The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpReshapeOrAddPoint::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}


/********************************************************************************************
>	void OpReshapeOrAddPoint::SetStatusLineHelp()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/96
	Purpose:	For getting a message for the status line during drag operations
********************************************************************************************/
void OpReshapeOrAddPoint::SetStatusLineHelp()
{
	String_256 HelpString;
	if (StartMousePos == FurthestPoint)
		HelpString = String_256(_R(IDS_PATHDRAGADDREFORM));
	else
		HelpString = String_256(_R(IDS_PATHDRAGFINISHREFORM));

	GetApplication()->UpdateStatusBarText(&HelpString, FALSE);
}





///////////////////////////////////////////////////////////////////////////////////////////////
// The ModifyPathAction class																 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	ModifyPathAction::ModifyPathAction()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action to undo path modification
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ModifyPathAction::ModifyPathAction()
{
	ChangedVerbs = NULL;
	ChangedFlags = NULL;
	ChangedCoords = NULL;
	ChangedIndices = NULL;
}


/********************************************************************************************

>	ActionCode ModifyPathAction::Init(  Operation* pOp,
										ActionList* pActionList,
										INT32 NumChangedElements,
										Action** NewAction)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/5/94
	Inputs:		pOp is the pointer to the operation to which this action belongs
				pActionList is the action list to which this action should be added
				NumChangedElements is the number of elements in the path that are going to
				change. This tells the action how much memory the total action is going to take
	Outputs:	NewAction is a pointer to a pointer to an action, allowing the function to return
				a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/



ActionCode ModifyPathAction::Init(  Operation* pOp,
									ActionList* pActionList,
									INT32 NumChangedElements,
									Action** NewAction)
{
	UINT32 ActSize = sizeof(ModifyPathAction) +
					NumChangedElements * (sizeof(PathVerb)+sizeof(PathFlags)+sizeof(DocCoord)+sizeof(INT32));

	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(ModifyPathAction), NewAction);
	if ((Ac==AC_OK) && (*NewAction != NULL))
		((ModifyPathAction*)*NewAction)->NumElements = NumChangedElements;

//	if (IsUserName("Jim")) TRACE( _T("Creating ModifyPathAction"));

	return Ac;
}

/********************************************************************************************

>	void ModifyPathAction::StoreArrays(PathVerb* Verbs, PathFlags* Flags, DocCoord* Coords, 
										INT32* Indices, NodePath* WhichPath)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/5/94
	Inputs:		Verbs is a pointer to an array of path verbs
				Flags is a pointer to an array of path flags
				Coords is a pointer to an array of DocCoords
				Indices is a pointer to an array of indices into the path
	Outputs:	-
	Returns:	-
	Purpose:	This function initialises the array pointers in this action. Note that the 
				variable NumElements is initialised in the Init function
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void ModifyPathAction::StoreArrays(PathVerb* Verbs, PathFlags* Flags, DocCoord* Coords, 
									INT32* Indices, NodePath* WhichPath)
{
	ChangedPath = WhichPath;
	ChangedVerbs = Verbs;
	ChangedFlags = Flags;
	ChangedCoords = Coords;
	ChangedIndices = Indices;
}



/********************************************************************************************

>	ActionCode ModifyPathAction::Execute()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/94
	Inputs:		-
	Outputs:	-
	Returns:	Action code, one of AC_OK, AC_NORECORD or AC_FAIL.
	Purpose:	This is the virtual function that is called when the action is executed
				by the Undo/Redo system. This is the function that actually undoes the modifypath
				action by replacing the indicated sections of the path with data from itself, and
				records redo information from the current state of the path.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ActionCode ModifyPathAction::Execute()
{
	// Here we're undoing the modify, so we have to write the list of changed array entries
	// back into the path. We also have to build undo information for the redo. Usefully,
	// We can use the existing arrays, swapping the data in the undo action for the data 
	// in the path, then passing the arrays on to the redo record. We have to make sure 
	// we don't try and de-allocate the arrays after we've done this, so we'll put a check in
	// the destructor for null pointers
	
	ModifyPathAction* ModAction;
	
	ActionCode Act;
	Act = ModifyPathAction::Init(pOperation, pOppositeActLst, NumElements, (Action**)(&ModAction));
	if (Act == AC_FAIL)
		return AC_FAIL;

	// Force a re-draw of the place where the path used to be
	Document* pDoc = GetWorkingDoc();
	ERROR2IF( pDoc == NULL, AC_FAIL, "There was no current document when undoing modifypath" );
	Spread* pSpread = ChangedPath->FindParentSpread();
	DocRect Invalid = ChangedPath->GetUnionBlobBoundingRect();
	pDoc->ForceRedraw( pSpread, Invalid, FALSE, ChangedPath );

	// Get pointers to all the arrays of the path

	PathVerb* Verbs = ChangedPath->InkPath.GetVerbArray();
	DocCoord* Coords = ChangedPath->InkPath.GetCoordArray();
	PathFlags* Flags = ChangedPath->InkPath.GetFlagArray();

	// Now modify the data, swapping data from the two sets of arrays

	PathVerb TempVerb;
	DocCoord TempCoord;
	PathFlags TempFlag;
	
	for (INT32 i=0;i<NumElements;i++)
	{
		TempVerb = Verbs[ChangedIndices[i]];
		TempCoord = Coords[ChangedIndices[i]];
		TempFlag = Flags[ChangedIndices[i]];
		Verbs[ChangedIndices[i]] = ChangedVerbs[i];
		Coords[ChangedIndices[i]] = ChangedCoords[i];
		Flags[ChangedIndices[i]] = ChangedFlags[i];
		ChangedVerbs[i] = TempVerb;
		ChangedCoords[i] = TempCoord;
		ChangedFlags[i] = TempFlag;

	}

	// Now, the path in the document has been changed, the arrays now contain what was in the path
	// so we tell the action where the arrays are.

	if (Act==AC_OK && ModAction!=NULL)
		ModAction->StoreArrays(ChangedVerbs, ChangedFlags, ChangedCoords, ChangedIndices, ChangedPath);

	// And set the array pointers to NULL so the destructor doesn't try and deallocate them

	ChangedVerbs = NULL;
	ChangedCoords = NULL;
	ChangedFlags = NULL;
	ChangedIndices = NULL;

	ChangedPath->InvalidateBoundingRect();

	// Now cause a redraw of the new paths position
	Invalid = ChangedPath->GetUnionBlobBoundingRect();
	pDoc->ForceRedraw( pSpread, Invalid, FALSE, ChangedPath );

	return Act;
}




ModifyPathAction::~ModifyPathAction()
{
	if (ChangedVerbs)
		CCFree(ChangedVerbs);
	if (ChangedCoords)
		CCFree(ChangedCoords);
	if (ChangedFlags)
		CCFree(ChangedFlags);
	if (ChangedIndices)
		CCFree(ChangedIndices);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// The RemovePathElementAction class														 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	RemovePathElementAction::RemovePathElementAction()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action to undo path element insertion
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

RemovePathElementAction::RemovePathElementAction()
{
}


/********************************************************************************************

>	ActionCode RemovePathElementAction::Init(  Operation* pOp,
										ActionList* pActionList,
										INT32 NumChangedElements,
										INT32 ChangedIndex,
										Action** NewAction)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		pOp is the pointer to the operation to which this action belongs
				pActionList is the action list to which this action should be added
				NumChangedElements is the number of elements in the path that are going to
				be removed.
				ChangedIndex is the index into the first element to be deleted
	Outputs:	NewAction is a pointer to a pointer to an action, allowing the function to return
				a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of the action to remove elements from a 
				path. If there is no room in the undo buffer (which is determined by the base class Init 
				function called within) the function will either return AC_NO_RECORD which means the 
				operation can continue, but no undo information needs to be stored, or AC_OK which means 
				the operation should continue AND record undo information. If the function returns AC_FAIL, 
				there was not enough memory to record the undo information, and the user has decided not 
				to continue with the operation.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/



ActionCode RemovePathElementAction::Init(  Operation* pOp,
									ActionList* pActionList,
									INT32 NumChangedElements,
									INT32 ChangedIndex,
									Action** NewAction)
{
	UINT32 ActSize = sizeof(RemovePathElementAction) +
					NumChangedElements * (sizeof(PathVerb)+sizeof(PathFlags)+sizeof(DocCoord));

	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(RemovePathElementAction), NewAction);

	if ((Ac == AC_OK) && (*NewAction != NULL))
	{
		((RemovePathElementAction*)*NewAction)->NumElements = NumChangedElements;
		((RemovePathElementAction*)*NewAction)->FirstChangedIndex = ChangedIndex;
	}

	return Ac;
}

/********************************************************************************************

>	void RemovePathElementAction::RecordPath(NodePath* WhichPath)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		WhichPath is the path that's being edited
	Outputs:	-
	Returns:	-
	Purpose:	This function records a pointer to the path. That's all.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/



void RemovePathElementAction::RecordPath(NodePath* WhichPath)
{
	ChangedPath = WhichPath;
}

ActionCode RemovePathElementAction::Execute()
{
	// Here we're undoing the insert, so we have to remove the elements that were inserted.
	// At the same time, we have to create a redo operation which will end up being an
	// InsertPathElementAction.
	
	InsertPathElementAction* ModAction;
	
	ActionCode Act;
	Act = InsertPathElementAction::Init(pOperation, pOppositeActLst, NumElements, FirstChangedIndex,
											(Action**)(&ModAction));
	if (Act == AC_FAIL)
		return AC_FAIL;

	// Force a re-draw of the place where the path used to be
	Document* pDoc = GetWorkingDoc();
	ERROR2IF( pDoc == NULL, AC_FAIL, "There was no current document when undoing modifypath" );
	Spread* pSpread = ChangedPath->FindParentSpread();
	DocRect Invalid = ChangedPath->GetUnionBlobBoundingRect();
	pDoc->ForceRedraw( pSpread, Invalid, FALSE, ChangedPath );

	if ((Act!=AC_NORECORD) && (ModAction!=NULL))
	{
		// I have to claim some memory to store the elements I'm deleting...

		PathVerb* ChangedVerbs;
		DocCoord* ChangedCoords;
		PathFlags* ChangedFlags;

		ALLOC_WITH_FAIL(ChangedVerbs,(PathVerb*) CCMalloc(NumElements * sizeof(PathVerb)),pOperation);
		ALLOC_WITH_FAIL(ChangedCoords,(DocCoord*) CCMalloc(NumElements * sizeof(DocCoord)),pOperation);
		ALLOC_WITH_FAIL(ChangedFlags,(PathFlags*) CCMalloc(NumElements * sizeof(PathFlags)),pOperation);

		if (!ChangedVerbs || !ChangedCoords || !ChangedFlags)
		{
			if (ChangedVerbs) CCFree(ChangedVerbs);
			if (ChangedCoords) CCFree(ChangedCoords);
			if (ChangedFlags) CCFree(ChangedFlags);
			return AC_FAIL;
		}

		// Get pointers to all the arrays of the path
		PathVerb* Verbs = ChangedPath->InkPath.GetVerbArray();
		DocCoord* Coords = ChangedPath->InkPath.GetCoordArray();
		PathFlags* Flags = ChangedPath->InkPath.GetFlagArray();

		// Now copy the data from the path into the arrays
		for (INT32 i=0;i<NumElements;i++)
		{
			ChangedVerbs[i] = Verbs[FirstChangedIndex+i];
			ChangedCoords[i] = Coords[FirstChangedIndex+i];
			ChangedFlags[i] = Flags[FirstChangedIndex+i];
		}

		// Now pass these arrays to the Insert action
		ModAction->RecordPath(ChangedVerbs, ChangedFlags,ChangedCoords, ChangedPath);
	}

	// Now we've recorded the data we're about to delete, let's delete it
	ChangedPath->InkPath.DeleteSection(FirstChangedIndex, NumElements);

	ChangedPath->InvalidateBoundingRect();

	// Now cause a redraw of the new path
	Invalid = ChangedPath->GetUnionBlobBoundingRect();
	pDoc->ForceRedraw( pSpread, Invalid, FALSE, ChangedPath );

	return Act;
}



RemovePathElementAction::~RemovePathElementAction()
{
//	if (ChangedVerbs)
//		CCFree(ChangedVerbs);
//	if (ChangedCoords)
//		CCFree(ChangedCoords);
//	if (ChangedFlags)
//		CCFree(ChangedFlags);
//	if (ChangedIndices)
//		CCFree(ChangedIndices);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// The InsertPathElementAction class														 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	InsertPathElementAction::InsertPathElementAction()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action to undo path modification
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

InsertPathElementAction::InsertPathElementAction()
{
	ChangedVerbs = NULL;
	ChangedFlags = NULL;
	ChangedCoords = NULL;
}


/********************************************************************************************

>	ActionCode InsertPathElementAction::Init(  Operation* pOp,
										ActionList* pActionList,
										INT32 NumChangedElements,
										INT32 ChangedIndex,
										Action** NewAction)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/5/94
	Inputs:		pOp is the pointer to the operation to which this action belongs
				pActionList is the action list to which this action should be added
				NumChangedElements is the number of elements in the path that are going to
				change. This tells the action how much memory the total action is going to take
	Outputs:	NewAction is a pointer to a pointer to an action, allowing the function to return
				a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/



ActionCode InsertPathElementAction::Init(  Operation* pOp,
									ActionList* pActionList,
									INT32 NumChangedElements,
									INT32 ChangedIndex,
									Action** NewAction)
{
	UINT32 ActSize = sizeof(InsertPathElementAction) +
					NumChangedElements * (sizeof(PathVerb)+sizeof(PathFlags)+sizeof(DocCoord));

	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(InsertPathElementAction), NewAction);
	if ((Ac == AC_OK) && (*NewAction != NULL))
	{
		((InsertPathElementAction*)*NewAction)->NumElements = NumChangedElements;
		((InsertPathElementAction*)*NewAction)->InsertionPoint = ChangedIndex;
	}

	return Ac;
}

/********************************************************************************************

>	void InsertPathElementAction::RecordPath(PathVerb* Verbs, PathFlags* Flags, DocCoord* Coords, 
										 NodePath* WhichPath)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/6/94
	Inputs:		Verbs is a pointer to an array of path verbs
				Flags is a pointer to an array of path flags
				Coords is a pointer to an array of DocCoords
				WhichPath is the path that's being operated on
	Outputs:	-
	Returns:	-
	Purpose:	This function initialises the array pointers in this action. Note that the 
				variables NumElements and InsertionPoint are initialised in the Init function
	Errors:		-
	SeeAlso:	-

********************************************************************************************/



void InsertPathElementAction::RecordPath(PathVerb* Verbs, PathFlags* Flags, DocCoord* Coords, 
										NodePath* WhichPath)
{
	ChangedPath = WhichPath;
	ChangedVerbs = Verbs;
	ChangedFlags = Flags;
	ChangedCoords = Coords;
}

ActionCode InsertPathElementAction::Execute()
{
	// Here we're undoing the modify, so we have to write the list of changed array entries
	// back into the path. We also have to build undo information for the redo. Usefully,
	// We can use the existing arrays, swapping the data in the undo action for the data 
	// in the path, then passing the arrays on to the redo record. We have to make sure 
	// we don't try and de-allocate the arrays after we've done this, so we'll put a check in
	// the destructor for null pointers
	
	RemovePathElementAction* ModAction;
	
	ActionCode Act;
	Act = RemovePathElementAction::Init(pOperation, pOppositeActLst, NumElements, InsertionPoint, (Action**)(&ModAction));
	if (Act == AC_FAIL)
		return AC_FAIL;

	// Tell the operation where the path is
	if (ModAction!=NULL)
		ModAction->RecordPath(ChangedPath);

	// Force a re-draw of the place where the path used to be
	Document* pDoc = GetWorkingDoc();
	ERROR2IF( pDoc == NULL, AC_FAIL, "There was no current document when undoing modifypath" );
	Spread* pSpread = ChangedPath->FindParentSpread();
	DocRect Invalid = ChangedPath->GetUnionBlobBoundingRect();
	pDoc->ForceRedraw( pSpread, Invalid, FALSE, ChangedPath );

	// See if we can open the path up
	if (!ChangedPath->InkPath.InsertSection(InsertionPoint,
											NumElements))	// WARNING! This routine will claim memory
		return AC_FAIL;										// in a way the undo system can't track, so it
															// won't be able to warn the user that memory is
															// tight and would they prefer not to do this operation

	// Get pointers to all the arrays of the path
	PathVerb* Verbs = ChangedPath->InkPath.GetVerbArray();
	DocCoord* Coords = ChangedPath->InkPath.GetCoordArray();
	PathFlags* Flags = ChangedPath->InkPath.GetFlagArray();

	// Now insert the data from the arrays in the action record

	for (INT32 i=0;i<NumElements;i++)
	{
		Verbs[i+InsertionPoint] = ChangedVerbs[i];
		Coords[i+InsertionPoint] = ChangedCoords[i];
		Flags[i+InsertionPoint] = ChangedFlags[i];
	}

  	ChangedPath->InvalidateBoundingRect();

	Invalid = ChangedPath->GetUnionBlobBoundingRect();
	pDoc->ForceRedraw( pSpread, Invalid, FALSE, ChangedPath );

	return Act;
}

InsertPathElementAction::~InsertPathElementAction()
{
	if (ChangedVerbs)
		CCFree(ChangedVerbs);
	if (ChangedCoords)
		CCFree(ChangedCoords);
	if (ChangedFlags)
		CCFree(ChangedFlags);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// The ModifyElementAction class																 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	ModifyElementAction::ModifyElementAction()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action to undo path element modification
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ModifyElementAction::ModifyElementAction()
{
	ChangedPath = NULL;
}


/********************************************************************************************

>	ActionCode ModifyElementAction::Init(  Operation* pOp,
										ActionList* pActionList,
										PathVerb Verb, 
										PathFlags Flags, 
										DocCoord Coord, 
										INT32 Index, 
										NodePath* WhichPath,
										Action** NewAction)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Inputs:		pOp is the pointer to the operation to which this action belongs
				pActionList is the action list to which this action should be added
				Verb is the verb of the path that we're changing
				Flags are the flags of the changed element
				DocCoord is the coords of the element
				Index is the index into the path of the element
	Outputs:	NewAction is a pointer to a pointer to an action, allowing the function to return
				a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/



ActionCode ModifyElementAction::Init(  Operation* pOp,
									ActionList* pActionList,
									PathVerb Verb, 
									PathFlags Flags, 
									DocCoord Coord, 
									INT32 Index, 
									NodePath* WhichPath,
									Action** NewAction)
{
	UINT32 ActSize = sizeof(ModifyElementAction);

	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(ModifyElementAction), NewAction);

	if ((Ac == AC_OK) && (*NewAction != NULL))
	{
		((ModifyElementAction*)*NewAction)->ChangedVerb = Verb;
		((ModifyElementAction*)*NewAction)->ChangedFlags = Flags;
		((ModifyElementAction*)*NewAction)->ChangedCoord = Coord;
		((ModifyElementAction*)*NewAction)->ChangedIndex = Index;
		((ModifyElementAction*)*NewAction)->ChangedPath = WhichPath;
	}

	return Ac;
}

ActionCode ModifyElementAction::Execute()
{
	// Here we're undoing the modify, so we have to change the element in the path,
	// recording redo information at the same time.
	
	ModifyElementAction* ModAction;
	
	// Get pointers to all the arrays of the path

	PathVerb* Verbs = ChangedPath->InkPath.GetVerbArray();
	DocCoord* Coords = ChangedPath->InkPath.GetCoordArray();
	PathFlags* Flags = ChangedPath->InkPath.GetFlagArray();

	// Create a redo action for this action, which is also a ModifyElementAction

	ActionCode Act;
	Act = ModifyElementAction::Init(pOperation, 
									pOppositeActLst, 
									Verbs[ChangedIndex],
									Flags[ChangedIndex],
									Coords[ChangedIndex],
									ChangedIndex,
									ChangedPath,
									(Action**)(&ModAction));
	if (Act == AC_FAIL)
		return AC_FAIL;

	// Force a re-draw of the place where the path used to be
	Document* pDoc = GetWorkingDoc();
	ERROR2IF( pDoc == NULL, AC_FAIL, "There was no current document when undoing modifypath" );
	Spread* pSpread = ChangedPath->FindParentSpread();
	DocRect Invalid = ChangedPath->GetUnionBlobBoundingRect();
	pDoc->ForceRedraw( pSpread, Invalid, FALSE, ChangedPath );

	Verbs[ChangedIndex] = ChangedVerb;
	Flags[ChangedIndex] = ChangedFlags;
	Coords[ChangedIndex] = ChangedCoord;

  	ChangedPath->InvalidateBoundingRect();
	Invalid = ChangedPath->GetUnionBlobBoundingRect();
	pDoc->ForceRedraw( pSpread, Invalid, FALSE, ChangedPath );

	return Act;
}

ModifyElementAction::~ModifyElementAction()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////
// The ModifyFlagsAction class																 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	ModifyFlagsAction::ModifyFlagsAction()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action to undo path flags modification
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ModifyFlagsAction::ModifyFlagsAction()
{
	ChangedPath = NULL;
}


/********************************************************************************************

>	ActionCode ModifyFlagsAction::Init(  Operation* pOp,
										ActionList* pActionList,
										PathFlags Flags, 
										INT32 Index, 
										NodePath* WhichPath,
										Action** NewAction)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		pOp is the pointer to the operation to which this action belongs
				pActionList is the action list to which this action should be added
				Flags are the flags of the changed element
				Index is the index into the path of the element
	Outputs:	NewAction is a pointer to a pointer to an action, allowing the function to return
				a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/



ActionCode ModifyFlagsAction::Init(  Operation* pOp,
									ActionList* pActionList,
									PathFlags Flags, 
									INT32 Index, 
									NodePath* WhichPath,
									Action** NewAction)
{
	UINT32 ActSize = sizeof(ModifyFlagsAction);

	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(ModifyFlagsAction), NewAction);
	if ((Ac==AC_OK) && (*NewAction != NULL))
	{
		((ModifyFlagsAction*)*NewAction)->ChangedFlags = Flags;
		((ModifyFlagsAction*)*NewAction)->ChangedIndex = Index;
		((ModifyFlagsAction*)*NewAction)->ChangedPath = WhichPath;
	}
//	if (IsUserName("Jim")) TRACE( _T("Creating ModifyElementAction"));

	return Ac;
}

ActionCode ModifyFlagsAction::Execute()
{
	// Here we're undoing the modify, so we have to change the element in the path,
	// recording redo information at the same time.
	
	ModifyFlagsAction* ModAction;
	
	// Get pointers to all the arrays of the path

	PathFlags* Flags = ChangedPath->InkPath.GetFlagArray();

	// Create a redo action for this action, which is also a ModifyElementAction

	ActionCode Act;
	Act = ModifyFlagsAction::Init(pOperation, 
									pOppositeActLst, 
									Flags[ChangedIndex],
									ChangedIndex,
									ChangedPath,
									(Action**)(&ModAction));
	if (Act == AC_FAIL)
		return AC_FAIL;

	// Force a re-draw of the place where the path used to be
	Document* pDoc = GetWorkingDoc();
	ERROR2IF( pDoc == NULL, AC_FAIL, "There was no current document when undoing modifypath" );
	Spread* pSpread = ChangedPath->FindParentSpread();
	DocRect Invalid = ChangedPath->GetUnionBlobBoundingRect();
	pDoc->ForceRedraw( pSpread, Invalid, FALSE, ChangedPath );

	Flags[ChangedIndex] = ChangedFlags;

	Invalid = ChangedPath->GetUnionBlobBoundingRect();
	pDoc->ForceRedraw( pSpread, Invalid, FALSE, ChangedPath );

	return Act;
}

ModifyFlagsAction::~ModifyFlagsAction()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////
// The ModifyFilledAction class																 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	ModifyFilledAction::ModifyFilledAction()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action to undo path Filled flag
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ModifyFilledAction::ModifyFilledAction()
{
	UndoFilled = TRUE;
	OldFilledState = FALSE;
}


/********************************************************************************************

>	ActionCode ModifyFilledAction::Init(  Operation* pOp,
										ActionList* pActionList,
										BOOL UndoFillOrStroke,
										BOOL FilledFlag,
										NodePath* WhichPath,
										Action** NewAction)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/7/94
	Inputs:		pOp is the pointer to the operation to which this action belongs
				pActionList is the action list to which this action should be added
				UndoFillOrStroke is TRUE if this action should undo a fill, otherwise a stroke
				FilledFlag is the state of the flag that must be replaced when undoing
				WhichPath is the path whose flag we're modifying
	Outputs:	NewAction is a pointer to a pointer to an action, allowing the function to return
				a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/



ActionCode ModifyFilledAction::Init(Operation* pOp,
									ActionList* pActionList,
									BOOL UndoFillOrStroke,
									BOOL FilledFlag,
									NodePath* WhichPath,
									Action** NewAction)
{
	UINT32 ActSize = sizeof(ModifyFilledAction);

	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(ModifyFilledAction), NewAction);
	if ((Ac==AC_OK) && (*NewAction != NULL))
	{
		((ModifyFilledAction*)*NewAction)->UndoFilled = UndoFillOrStroke;
		((ModifyFilledAction*)*NewAction)->OldFilledState = FilledFlag;
		((ModifyFilledAction*)*NewAction)->ChangedPath = WhichPath;
	}
//	if (IsUserName("Jim")) TRACE( _T("Creating ModifyElementAction"));

	return Ac;
}

ActionCode ModifyFilledAction::Execute()
{
	// Here we're undoing the modify, so we have to change the element in the path,
	// recording redo information at the same time.
	
	ModifyFilledAction* ModAction;
	
	// Create a redo action for this action, which is also a ModifyElementAction

	BOOL CurState;
	if (UndoFilled)
		CurState = ChangedPath->InkPath.IsFilled;
	else
		CurState = ChangedPath->InkPath.IsStroked;
	
	ActionCode Act;
	Act = ModifyFilledAction::Init(pOperation, 
									pOppositeActLst, 
									UndoFilled,
									CurState,
									ChangedPath,
									(Action**)(&ModAction));
	if (Act == AC_FAIL)
		return AC_FAIL;

	// Force a re-draw of the place where the path used to be
	Document* pDoc = GetWorkingDoc();
	ERROR2IF( pDoc == NULL, AC_FAIL, "There was no current document when undoing modifypath" );
	Spread* pSpread = ChangedPath->FindParentSpread();
	DocRect Invalid = ChangedPath->GetUnionBlobBoundingRect();
	pDoc->ForceRedraw( pSpread, Invalid, FALSE, ChangedPath );

	if (UndoFilled)
		ChangedPath->InkPath.IsFilled = OldFilledState;
	else
		ChangedPath->InkPath.IsStroked = OldFilledState;

	Invalid = ChangedPath->GetUnionBlobBoundingRect();
	pDoc->ForceRedraw( pSpread, Invalid, FALSE, ChangedPath );

	return Act;
}

ModifyFilledAction::~ModifyFilledAction()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////
// The RecalcBoundsAction class																 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	RecalcBoundsAction::RecalcBoundsAction()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action to recalculate bounds of a node
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
RecalcBoundsAction::RecalcBoundsAction()
{
}



/********************************************************************************************

>	ActionCode RecalcBoundsAction::Init(  Operation* pOp,
										ActionList* pActionList,
										NodeRenderableBounded* WhichNode,
										Action** NewAction)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/94
	Inputs:		pOp is the pointer to the operation to which this action belongs
				pActionList is the action list to which this action should be added
				WhichNode is the object whose bounds we want to recalculate
	Outputs:	NewAction is a pointer to a pointer to an action, allowing the function to return
				a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/
ActionCode RecalcBoundsAction::Init(  Operation* pOp,
									ActionList* pActionList,
									NodeRenderableBounded* WhichNode,
									Action** NewAction)
{
	UINT32 ActSize = sizeof(RecalcBoundsAction);

	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(RecalcBoundsAction), NewAction);
	if ((Ac == AC_OK) && (*NewAction != NULL))
	{
		((RecalcBoundsAction*)*NewAction)->ChangedNode = WhichNode;
		((RecalcBoundsAction*)*NewAction)->OldBounds = WhichNode->GetBoundingRect();
	}

	return Ac;
}



/********************************************************************************************

>	ActionCode RecalcBoundsAction::DoRecord(Operation* pOp,
											ActionList* pActionList,
											NodeRenderableBounded* WhichNode,
											BOOL OptimiseRedraw = FALSE)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Inputs:		pOp is the currently running operation
				pActionList is a pointer ot the action list to which the action should be appended
				WhichNode is the node we're dealing with here
				OptimiseRedraw : If this is TRUE and WhichNode is a NodePath then the NeedToRender
								 flags are used to determine the areas of the screen to redraw
	Outputs:	-
	Returns:	Action code which indicates success or failure to create the action
	Purpose:	This static function makes it a little easier to use this action. It creates an instance
				of this action and appends it to the action list. Then it invalidates the current
				view according to the blob rectangle of the object.
				This function should be called before you do anything to change the bounding box of an
				object. It will invalidate the view (including blobs) allowing you to manipulate the object
				to your heart's content. In order to keep undo and redo running, make sure that after you 
				have finished manipulating the object, you call RecordBoundsAction::DoRecord which will not
				only invalidate the new bounds rectangle of the object (including blobs) but will also
				make sure that there are the correct actions in the undo list.
	Errors:		-
	SeeAlso:	RecordBoundsAction::DoRecalc

********************************************************************************************/
ActionCode RecalcBoundsAction::DoRecalc(Operation* pOp,
										ActionList* pActionList,
										NodeRenderableBounded* WhichNode,
										BOOL OptimiseRedraw)
{
	ERROR3IF(OptimiseRedraw && !WhichNode->IsNodePath(),"Can't optimise redraw for a non-path object");

	RecalcBoundsAction* RecAction;
	ActionCode Act = RecalcBoundsAction::Init(pOp, pActionList, WhichNode, (Action**)&RecAction);
	if (Act != AC_FAIL)
	{
		// Force a re-draw of the place where the path used to be
		Document* pDoc = pOp->GetWorkingDoc();
		ERROR2IF( pDoc == NULL, AC_FAIL, "There was no current document in RecordBoundsAction::DoRecord" );
		Spread* pSpread = WhichNode->FindParentSpread();
		DocRect Invalid;

		((NodePath*)WhichNode)->ReleaseCached();
		
		// Do an optimised redraw if possible
		if (WhichNode->IsNodePath() && OptimiseRedraw)
			OptimiseRedraw = CauseOptimisedRedraw((NodePath*)WhichNode, pDoc, pSpread);
		else
			OptimiseRedraw = FALSE;

		if (!OptimiseRedraw)
			pDoc->ForceRedraw( pSpread, WhichNode->GetUnionBlobBoundingRect());
	}
	return Act;
}



/********************************************************************************************

>	ActionCode RecalcBoundsAction::Execute()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/94
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, either AC_OK, AC_NORECORD or AC_FAIL
	Purpose:	Will replace the bounding box of an object, recording the existing bounds
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
ActionCode RecalcBoundsAction::Execute()
{
	// This is undoing a bounds recalculation
	
	RecordBoundsAction* ReAction;
	
	// Create a redo action for this action, which is also a RecalcBoundsAction

	ActionCode Act;
	Act = RecordBoundsAction::Init(pOperation, 
									pOppositeActLst, 
									ChangedNode,
									(Action**)(&ReAction));
	if (Act == AC_FAIL)
		return AC_FAIL;

	// Force a re-draw of the place where the path used to be
	Document* pDoc = GetWorkingDoc();
	ERROR2IF( pDoc == NULL, AC_FAIL, "There was no current document when undoing RecalcBounds" );
	Spread* pSpread = ChangedNode->FindParentSpread();
	
	if (pSpread != NULL)
	{
		DocRect Invalid = ChangedNode->GetUnionBlobBoundingRect();
		pDoc->ForceRedraw( pSpread, Invalid, FALSE, ChangedNode );

		// Mark the bounding rect as invalid
		ChangedNode->InvalidateBoundingRect();
	}
	
	return Act;
}

RecalcBoundsAction::~RecalcBoundsAction()
{
}



/********************************************************************************************

>	static BOOL RecalcBoundsAction::CauseOptimisedRedraw(NodePath* pChangedPath, Document* pDoc, Spread* pSpread)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/04/95
	Inputs:		pChangedPath - points to the path to cause a redraw of
				pDoc - points to the document containing the path
				pSpread - points to the spread containing the path
	Outputs:	-
	Returns:	TRUE if the optimised redraw succeded,
				FALSE if the entire shape should be redrawn (an error occured)
	Purpose:	Called by DoRecord and DoRecalc to only invalidate the areas of the screen 
				covered by the parts of the path that have changed.  These will have the
				NeedToRender flag set.
	Errors:		-
	SeeAlso:	RecordBoundsAction::DoRecord, RecalcBoundsAction::DoRecalc

********************************************************************************************/
BOOL RecalcBoundsAction::CauseOptimisedRedraw(NodePath* pChangedPath, Document* pDoc, Spread* pSpread)
{
	// If the shape is filled then we can't optimise
	if (pChangedPath->InkPath.IsFilled)
		return FALSE;

	// Build an attribute map
	CCAttrMap AttribMap(30);
	if (!pChangedPath->FindAppliedAttributes(&AttribMap)) 
		return FALSE;

	// Get the current linewidth applied to the NodePath
	MILLIPOINT LineWidth = 0;
	void* pLineWidth = NULL;
	AttribMap.Lookup( CC_RUNTIME_CLASS(AttrLineWidth), pLineWidth );
	ERROR3IF(pLineWidth == NULL, "Did not find line width attribute");
	if (pLineWidth != NULL) 
		LineWidth = ((AttrLineWidth*)pLineWidth)->Value.LineWidth;
	else
		return FALSE;

	// Get the current dash pattern applied to the NodePath
	// Can't optimise if it's a dashed line - the pattern shifts around
	void* pDashPat = NULL;
	AttribMap.Lookup( CC_RUNTIME_CLASS(AttrDashPattern), pDashPat );
	ERROR3IF(pDashPat == NULL, "Did not find dash pattern attribute");
	if ((pDashPat == NULL) || (((AttrDashPattern*)pDashPat)->Value.DashPattern.Elements != 0) )
		return FALSE;

	// Get the current mitre limit applied to the NodePath
	MILLIPOINT MitreLimit = 0;
	void* pMitreAttr = NULL;
	AttribMap.Lookup( CC_RUNTIME_CLASS(AttrMitreLimit),pMitreAttr);
	ERROR3IF(pMitreAttr == NULL, "Did not find mitre attribute");
	if (pMitreAttr != NULL) 
		MitreLimit = ((AttrMitreLimit*)pLineWidth)->Value.MitreLimit;
	else
		return FALSE;


	// See if theres a brush attribute applied to the nodepath
	AttrBrushType* pAttrBrush = (AttrBrushType*) pChangedPath->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBrushType));
	if (pAttrBrush != NULL && pAttrBrush->GetBrushHandle() != BrushHandle_NoBrush)
	{
		pAttrBrush->FlushCache();
		DocRect Redraw = pAttrBrush->GetAttrBoundingRect(pChangedPath);
		pDoc->ForceRedraw(pSpread, Redraw, FALSE, pChangedPath);
	}

	// Get the arrowheads applied to the NodePath
	AttrEndArrow* pEndArrow = (AttrEndArrow*) pChangedPath->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrEndArrow));
	AttrStartArrow* pStartArrow = (AttrStartArrow*) pChangedPath->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStartArrow));

	// redraw the areas covered by the arrowheads
	// PABODGE - only redraw if the arrohead has moved
	if (pEndArrow != NULL)
	{
		DocRect Redraw = pEndArrow->GetAttrBoundingRect(pChangedPath, &AttribMap);
		pDoc->ForceRedraw( pSpread, Redraw, FALSE, pChangedPath );
	}
	if (pStartArrow != NULL)
	{
		DocRect Redraw = pStartArrow->GetAttrBoundingRect(pChangedPath, &AttribMap);
		pDoc->ForceRedraw( pSpread, Redraw, FALSE, pChangedPath );
	}

	// PABODGE - should only inflate by mitre limit if mitre join applied!
	MILLIPOINT InflationSize = LineWidth + MitreLimit*4;
	const INT32 NumPoints = pChangedPath->InkPath.GetNumCoords();
	INT32 loop = 0;
	INT32 Previous = 0;
	PathVerb* Verbs = NULL;
	DocCoord* Coords = NULL;
	PathFlags* Flags = NULL;
	pChangedPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);

	if (pChangedPath->InkPath.FindNextEndPoint(&loop))
	{
		while (loop < NumPoints)
		{
			// If the need to render flag is set then cause a redraw of this segment
			if (Flags[loop].NeedToRender)
			{
				// Get the size of the redraw rect
				DocRect Invalid(Coords[Previous], Coords[Previous]);
				while (Previous < loop)
				{
					Previous++;
					Invalid.IncludePoint(Coords[Previous]);
				}
	
				// Do the redraw
				Invalid.Inflate(InflationSize);
				pDoc->ForceRedraw( pSpread, Invalid, FALSE, pChangedPath );
			}

			// Point to the next endpoint
			Previous = loop;
			if (!pChangedPath->InkPath.FindNextEndPoint(&loop))
				break;
		}
	}
	else
		return FALSE;

	return TRUE;
}



///////////////////////////////////////////////////////////////////////////////////////////////
// The RecordBoundsAction class																 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	RecordBoundsAction::RecordBoundsAction()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action to recalculate bounds of a node
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
RecordBoundsAction::RecordBoundsAction()
{
}



/********************************************************************************************

>	ActionCode RecordBoundsAction::Init(  Operation* pOp,
										ActionList* pActionList,
										NodeRenderableBounded* WhichNode,
										Action** NewAction)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/94
	Inputs:		pOp is the pointer to the operation to which this action belongs
				pActionList is the action list to which this action should be added
				WhichNode is the object whose bounds we want to recalculate
	Outputs:	NewAction is a pointer to a pointer to an action, allowing the function to return
				a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/
ActionCode RecordBoundsAction::Init(  Operation* pOp,
									ActionList* pActionList,
									NodeRenderableBounded* WhichNode,
									Action** NewAction)
{
	UINT32 ActSize = sizeof(RecordBoundsAction);

	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(RecordBoundsAction), NewAction);
	if ((Ac == AC_OK) && (*NewAction != NULL))
		((RecordBoundsAction*)*NewAction)->ChangedNode = WhichNode;

	return Ac;
}



/********************************************************************************************

>	ActionCode RecordBoundsAction::DoRecord(Operation* pOp,
											ActionList* pActionList,
											NodeRenderableBounded* WhichNode,
											BOOL OptimiseRedraw = FALSE)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Inputs:		pOp is the currently running operation
				pActionList is a pointer ot the action list to which the action should be appended
				WhichNode is the node we're dealing with here
				OptimiseRedraw : If this is TRUE and WhichNode is a NodePath then the NeedToRender
								 flags are used to determine the areas of the screen to redraw
	Outputs:	-
	Returns:	Action code which indicates success or failure to create the action
	Purpose:	This static function makes it a little easier to use this action. It creates an instance
				of this action and appends it to the action list. Then it invalidates the current
				view according to the blob rectangle of the object.
	Errors:		ERROR2 if the operation dosen't have a document
				ERROR3 if you attempt to optimise for a non path object
	SeeAlso:	RecalcBoundsAction::DoRecalc

********************************************************************************************/
ActionCode RecordBoundsAction::DoRecord(Operation* pOp,
										ActionList* pActionList,
										NodeRenderableBounded* WhichNode,
										BOOL OptimiseRedraw)
{
	ERROR3IF(OptimiseRedraw && !WhichNode->IsNodePath(),"Can't optimise redraw for a non-path object");

	RecordBoundsAction* RecAction;
	ActionCode Act = RecordBoundsAction::Init(pOp, pActionList, WhichNode, (Action**)&RecAction);
	if (Act != AC_FAIL)
	{
		// Force a re-draw of the place where the path used to be
		Document* pDoc = pOp->GetWorkingDoc();
		ERROR2IF( pDoc == NULL, AC_FAIL, "There was no current document in RecordBoundsAction::DoRecord" );

		Spread* pSpread = WhichNode->FindParentSpread();

		// Do an optimised redraw if possible
		if (WhichNode->IsNodePath() && OptimiseRedraw)
			OptimiseRedraw = RecalcBoundsAction::CauseOptimisedRedraw((NodePath*)WhichNode, pDoc, pSpread);
		else
			OptimiseRedraw = FALSE;

		if (!OptimiseRedraw)
			pDoc->ForceRedraw( pSpread, WhichNode->GetUnionBlobBoundingRect(), FALSE, WhichNode);
	}
	return Act;
}



/********************************************************************************************

>	ActionCode RecordBoundsAction::Execute()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/94
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, either AC_OK, AC_NORECORD or AC_FAIL
	Purpose:	Will replace the bounding box of an object, recording the existing bounds
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
ActionCode RecordBoundsAction::Execute()
{
	// This is undoing a bounds recalculation
	
	RecalcBoundsAction* ReAction;
	
	// Create a redo action for this action, which is also a RecalcBoundsAction

	ActionCode Act;
	Act = RecalcBoundsAction::Init(pOperation, 
									pOppositeActLst, 
									ChangedNode,
									(Action**)(&ReAction));
	if (Act == AC_FAIL)
		return AC_FAIL;

	// Force a re-draw of the place where the path used to be
	Document* pDoc = GetWorkingDoc();
	ERROR2IF( pDoc == NULL, AC_FAIL, "There was no current document when undoing RecalcBounds" );
	Spread* pSpread = ChangedNode->FindParentSpread();
	
	if (pSpread != NULL)
	{
		DocRect Invalid = ChangedNode->GetUnionBlobBoundingRect();
		pDoc->ForceRedraw( pSpread, Invalid, FALSE, ChangedNode );
	}

	return Act;
}



RecordBoundsAction::~RecordBoundsAction()
{
}





/********************************************************************************************

>	SavePathArraysAction::SavePathArraysAction()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Purpose:	Constructor for the action to store a path on the undo

********************************************************************************************/

SavePathArraysAction::SavePathArraysAction()
{
	ChangedPath   = NULL;
	ChangedVerbs  = NULL;
	ChangedFlags  = NULL;
	ChangedCoords = NULL;
}


/********************************************************************************************

>	SavePathArraysAction::~SavePathArraysAction()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Purpose:	destructor for the action to store a path on the undo

********************************************************************************************/

SavePathArraysAction::~SavePathArraysAction()
{
	if (ChangedVerbs)
		CCFree(ChangedVerbs);
	if (ChangedCoords)
		CCFree(ChangedCoords);
	if (ChangedFlags)
		CCFree(ChangedFlags);
}


/********************************************************************************************

>	ActionCode SavePathArraysAction::DoRecord(Operation* pOp,
											  ActionList* pActionList,
											  Path* pPath)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Inputs:		pOp is the currently running operation
				pActionList = a pointer ot the action list to which the action should be appended
				pPath		= a pointer to the path whose data will be saved.
	Outputs:	NewAction	= a pointer to a pointer to an action, allowing the function 
							  to return	a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Use this function to get the SavePathArraysAction going. It is far simpler
				and less error prone than calling SavePathArraysAction::Init directly.
	Errors:		-
	SeeAlso:	For a full explanation of this action see SavePathArraysAction::Init()

********************************************************************************************/

ActionCode SavePathArraysAction::DoRecord(Operation* pOp, ActionList* pActionList, Path* pPath)
{
	SavePathArraysAction* SaveAction;
	ActionCode Act = SavePathArraysAction::Init(pOp, pActionList, pPath, (Action**)&SaveAction, TRUE);
	return Act;
}



/********************************************************************************************

>	ActionCode SavePathArraysAction::Init(Operation* pOp,
										  ActionList* pActionList,
										  Path* pPath,
										  Action** NewAction,
										  BOOL CreateArrays = TRUE)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Inputs:		pOp 		= a pointer to the operation to which this action belongs
				pActionList = the action list to which this action should be added
				pPath		= a pointer to the path whose data will be saved.
				CreateArrays= a boolean which should always be TRUE except for when this
							  action is being executed by the undo/redo system.
	Outputs:	NewAction	= a pointer to a pointer to an action, allowing the function 
							  to return	a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	The actions static init function.
				The purpose of this function is to create instances of SavePathsArrayAction.
				Hence it static nature. It seems strange that you can call a function inside
				a class	to create another instance of that class, but thats what really happens
				here. We pass back a pointer to the action if we've succeeded in creating
				it.
				This particular action init function also creates some external arrays to
				save a paths data in. You need to call the function with CreateArrays set
				to TRUE, to get it to save the paths arrays. It needs this parameter because
				the very same function is called within the execution of a previously created
				savepath action which incidently sets CreateArrays to FALSE.
				(You can get a serious headache thinking about this one).
				The idea is really to create the save arrays once, when this init function
				is called and swap them between undo and redo actions, killing them off only
				when the last thread is destructed.
	SeeAlso:	SavePathArraysAction::DoRecord()

********************************************************************************************/

ActionCode SavePathArraysAction::Init(Operation* pOp,
									  ActionList* pActionList,
									  Path* pPath,
									  Action** NewAction,
									  BOOL CreateArrays)
{
	ERROR1IF(pPath==NULL,AC_FAIL,"SavePathArraysAction::Init() passed a NULL path pointer");

	UINT32 NumElements = pPath->GetNumCoords();
	UINT32 ActSize = sizeof(SavePathArraysAction) + NumElements*(sizeof(PathVerb) + 
								   								sizeof(PathFlags) +
								   								sizeof(DocCoord) );

	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(SavePathArraysAction), NewAction);

	SavePathArraysAction* CreatedAction = (SavePathArraysAction*)(*NewAction);
	if (CreatedAction!=NULL)
	{
		CreatedAction->ChangedPath = pPath;
		if (CreateArrays)
			CreatedAction->SetUpArrays(pPath,pOp);
	}
	return Ac;
}



/********************************************************************************************

>	ActionCode SavePathArraysAction::SetUpArrays(Path* pPath, Operation* pOp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Inputs:		pPath 	= pointer to the path to save
				pOp		= pointer to the current operation
	Returns:	one of AC_OK, AC_NORECORD, AC_FAIL
	Purpose:	

********************************************************************************************/

ActionCode SavePathArraysAction::SetUpArrays(Path* pPath, Operation* pOp)
{
	if (ChangedFlags!=NULL || ChangedVerbs!=NULL ||	ChangedCoords!=NULL)
	{
		ENSURE(TRUE,"SavePathArraysAction::SetUpArrays() failed");
		return AC_FAIL;
	}

	if (!SavePathArrays(pPath, pOp, &ChangedFlags, &ChangedVerbs, &ChangedCoords))
		return AC_FAIL;

	return AC_OK;
}



/********************************************************************************************

>	ActionCode SavePathArraysAction::Execute()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Inputs:		-
	Returns:	one of AC_OK, AC_NORECORD, AC_FAIL
	Purpose:	This function executes the SavePath action which will create an opposite
				action record dependent on whether we're undoing or redoing. It will swap
				the saved information about the path between the executing action record
				and the arrays of the path.

********************************************************************************************/

ActionCode SavePathArraysAction::Execute()
{
	// first try to create an opposite action
	SavePathArraysAction* SaveAction;
	
	ActionCode Act;
	Act = SavePathArraysAction::Init(pOperation, pOppositeActLst, ChangedPath, (Action**)(&SaveAction), FALSE);
	if (Act == AC_FAIL)
		return AC_FAIL;

	// swap over the path data between this action record and the path.
	SwapPathArrays();

	// and update the new actions array pointers
	if (SaveAction!=NULL)
	{
		SaveAction->ChangedFlags = ChangedFlags;
		SaveAction->ChangedVerbs = ChangedVerbs;
		SaveAction->ChangedCoords = ChangedCoords;

		// make sure we clear these pointer vals before the destructor does
		// as we're sharing the arrays.
		ChangedVerbs  = NULL;
		ChangedCoords = NULL;
		ChangedFlags  = NULL;
	}

	return Act;
}


/********************************************************************************************

>	BOOL SavePathArraysAction::SavePathArrays(	Path* pInputPath,
												Operation* pOp,
						 						PathFlags** pFlags,
						 						PathVerb** pVerbs,
						 						DocCoord** pCoords)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Inputs:		pPath 	= a pointer to the path to save
				pOp		= the current operation pointer
	Outputs		pFlags points to a copied flags array, set to NULL if failed
				pVerbs points to a copied verb array, set to NULL if failed
				pCoords points to a copied coord array, set to NULL if failed
	Returns:	TRUE if the paths arrays have been saved
				FALSE if not
	Purpose:	Creates three arrays and copies the input paths verbs, coords and flags
				into these. If successfull it will return pointers to these arrays in
				pFlags, pVerbs and pCoords.
														  
********************************************************************************************/

BOOL SavePathArraysAction::SavePathArrays(	Path* pPath, 
											Operation* pOp,
											PathFlags** pFlags,
											PathVerb** pVerbs,
											DocCoord** pCoords)
{	
	UINT32 NumElements = pPath->GetNumCoords();
	PathFlags* DFlags;
	PathVerb* DVerbs;
	DocCoord* DCoords;

	ALLOC_WITH_FAIL(DVerbs,(PathVerb*) CCMalloc(NumElements * sizeof(PathVerb)),pOp);
	ALLOC_WITH_FAIL(DCoords,(DocCoord*) CCMalloc(NumElements * sizeof(DocCoord)),pOp);
	ALLOC_WITH_FAIL(DFlags,(PathFlags*) CCMalloc(NumElements * sizeof(PathFlags)),pOp);

	if (!DVerbs || !DCoords || !DFlags)
	{
		if (DVerbs)  CCFree(DVerbs);
		if (DCoords) CCFree(DCoords);
		if (DFlags)  CCFree(DFlags);

		*pFlags=NULL;
		*pVerbs=NULL;
		*pCoords=NULL;

		return FALSE;
	}

	// Now copy the data from the path into the arrays
	DocCoord*  SCoords = pPath->GetCoordArray();
	PathFlags* SFlags  = pPath->GetFlagArray();
	PathVerb*  SVerbs  = pPath->GetVerbArray();

	// Copy all the data
	memmove((void*)(DCoords), (void*)(SCoords), NumElements*sizeof(DocCoord));
	memmove((void*)(DVerbs),  (void*)(SVerbs),  NumElements*sizeof(PathVerb));
	memmove((void*)(DFlags),  (void*)(SFlags),  NumElements*sizeof(PathFlags));

	*pFlags = DFlags;
	*pVerbs = DVerbs;
	*pCoords = DCoords;

	return TRUE;
}


/********************************************************************************************

>	void SavePathArraysAction::SwapPathArrays()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Inputs:		-
	Returns:	-
	Purpose:	Swaps the path array data with that saved in the action record.

********************************************************************************************/

void SavePathArraysAction::SwapPathArrays()
{
	INT32 NumElements = ChangedPath->GetNumCoords();
	PathVerb*  Verbs  = ChangedPath->GetVerbArray();
	DocCoord*  Coords = ChangedPath->GetCoordArray();
	PathFlags* Flags  = ChangedPath->GetFlagArray();

	PathVerb TempVerb;
	DocCoord TempCoord;
	PathFlags TempFlag;
	
	for (INT32 i=0; i<NumElements; i++)
	{
		TempVerb=Verbs[i];	 Verbs[i]=ChangedVerbs[i];	 ChangedVerbs[i]=TempVerb;
		TempCoord=Coords[i]; Coords[i]=ChangedCoords[i]; ChangedCoords[i]=TempCoord;
		TempFlag=Flags[i];	 Flags[i]=ChangedFlags[i];	 ChangedFlags[i]=TempFlag;
	}
}



/********************************************** StorePathSubSelStateAction ******************************************/



///********************************************************************************************
//>	StorePathSubSelStateAction::StorePathSubSelStateAction()
//
//	Author:		Peter
//	Created:	9/11/95
//	Purpose:	Constructor.  Initalises member variables
//********************************************************************************************/
//StorePathSubSelStateAction::StorePathSubSelStateAction()
//{
//	pStatePath = NULL;
//	pIndexArray = NULL;
//	NumStoredIndexes = 0;
//	RecordingSelected = TRUE;
//}
//
//
//
///********************************************************************************************
//>	StorePathSubSelStateAction::~StorePathSubSelStateAction()
//
//	Author:		Peter
//	Created:	9/11/95
//	Purpose:	Destructor.  Frees claimed memory
//********************************************************************************************/
//StorePathSubSelStateAction::~StorePathSubSelStateAction()
//{
//	if (pIndexArray != NULL)
//		CCFree(pIndexArray);
//}
//
//
//
///********************************************************************************************
//>	static ActionCode StorePathSubSelStateAction::DoRecord(Operation* pOp,
//															ActionList* pActionList,
//															Path* pPath)
//	Author:		Peter
//	Created:	9/11/95
//	Inputs:		pOp 		= a pointer to the current operation
//				pActionList = a pointer to the action list to which the action should be appended
//				pPath		= a pointer to the path whose data will be saved.
//	Outputs:	Sets up action, recording the sub-selection state.
//	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
//	Purpose:	Use this function to the sub-selection state of a path in your operation.
//				It's a wrapper around StorePathSubSelStateAction::Init.
//	Errors:		-
//	SeeAlso:	StorePathSubSelStateAction::Init()
//********************************************************************************************/
//ActionCode StorePathSubSelStateAction::DoRecord(Operation* pOp, ActionList* pActionList, Path* pPath)
//{
//	StorePathSubSelStateAction* SaveAction = NULL;
//	return StorePathSubSelStateAction::Init(pOp, pActionList, pPath, (Action**)&SaveAction);
//}
//
//
//
///********************************************************************************************
//>	static ActionCode StorePathSubSelStateAction::Init(Operation* pOp,
//													  ActionList* pActionList,
//													  Path* pPath,
//													  Action** NewAction)
//	Author:		Peter
//	Created:	9/11/95
//	Inputs:		pOp 		= a pointer to the operation to which this action belongs
//				pActionList = the action list to which this action should be added
//				pPath		= a pointer to the path whose data will be saved.
//	Outputs:	The action is created, the path state is stored and the action is inserted
//				into the actionlist
//	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
//	Purpose:	The purpose of this function is to create instances of StorePathSubSelStateAction.
//				The action stores the sub-selection state of the given path.  It either stores
//				the indexes of the selected endpoints, or the unselected ones, depending on
//				which will use less memory.
//				NOTE: Don't call this function directly - call DoRecord() 
//	SeeAlso:	StorePathSubSelStateAction::DoRecord()
//********************************************************************************************/
//ActionCode StorePathSubSelStateAction::Init(Operation* pOp, ActionList* pActionList, Path* pPath, Action** NewAction)
//{
//	ERROR2IF((pPath==NULL) || (pOp==NULL) || (pActionList==NULL), AC_FAIL, "StorePathSubSelStateAction::Init() passed a NULL pointer");
//
//	// Work out how much memory the action array will require
//	const UINT32 NumElements = pPath->GetNumCoords();		// Number of elements in the path
//	PathFlags* pFlags = pPath->GetFlagArray();			// Pointer to the element flags
//	UINT32 SelectedPoints = 0;							// Count of selected endpoints
//	UINT32 UnselectedPoints = 0;							// Count of unselected endpoints.
//	for (UINT32 loop = 0; loop<NumElements; loop++)
//	{
//		if (pFlags[loop].IsEndPoint)
//		{
//			if (pFlags[loop].IsSelected)
//				SelectedPoints++;
//			else
//				UnselectedPoints++;
//		}
//	}
//	const UINT32 ArraySize = sizeof(UINT32) * __min(SelectedPoints,UnselectedPoints);
//
//	// Create the action object
//	UINT32 ActSize = sizeof(SavePathArraysAction) + ArraySize;
//	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(StorePathSubSelStateAction), NewAction);
//
//	// Complete the setup of the action
//	if ((Ac == AC_OK) && (*NewAction != NULL))
//	{
//		StorePathSubSelStateAction* CreatedAction = (StorePathSubSelStateAction*)(*NewAction);
//		CreatedAction->pStatePath = pPath;
//		CreatedAction->RecordingSelected = (SelectedPoints < UnselectedPoints);
//		CreatedAction->NumStoredIndexes = __min(SelectedPoints,UnselectedPoints);
//
//	 	// Store the indexes of the selected or unselected points
//		if (ArraySize > 0)
//		{
//			// Claim memory for the array (theoritically this wont fail as Init ensured there was enough)
//			ALLOC_WITH_FAIL(CreatedAction->pIndexArray, ((UINT32*)CCMalloc(ArraySize)), pOp);
//			if (CreatedAction->pIndexArray != NULL)
//			{
//				pFlags = pPath->GetFlagArray();			// Recache flag array pointer (it may have moved)
//				UINT32 ArrayIndex = 0;
//
//				// Store the index of the endpoints whose selection state matches what we are storing
//				for (UINT32 loop = 0; loop<NumElements; loop++)
//				{
//					if (pFlags[loop].IsEndPoint && (pFlags[loop].IsSelected == CreatedAction->RecordingSelected) )
//						CreatedAction->pIndexArray[ArrayIndex++] = loop;
//				}
//			}
//			else
//				CreatedAction->RecordingSelected = FALSE;	// Deselect the lot
//		}
//	}
//
//	return Ac;
//}
//
//
//
///********************************************************************************************
//>	ActionCode StorePathSubSelStateAction::Execute()
//
//	Author:		Peter
//	Created:	9/11/95
//	Inputs:		-
//	Returns:	one of AC_OK, AC_NORECORD, AC_FAIL
//	Purpose:	This function creates a new action to store the path sub-selection state
//				the sets the sub-selection state to the one in the stored arrays
//********************************************************************************************/
//ActionCode StorePathSubSelStateAction::Execute()
//{
//	ERROR2IF(pStatePath == NULL, AC_FAIL, "Path pointer was NULL");
//
//	// first try to create an opposite action
//	StorePathSubSelStateAction* SaveAction;
//	ActionCode Act = StorePathSubSelStateAction::Init(pOperation, pOppositeActLst, pStatePath, (Action**)(&SaveAction));
//	if (Act == AC_FAIL)
//		return AC_FAIL;
//
//	// now set the path sub-selection state to what we have recorded.
//	const UINT32 NumElements = pStatePath->GetNumCoords();	// Number of elements in the path
//	PathFlags* pFlags = pStatePath->GetFlagArray();			// Pointer to the element flags
//	if (pIndexArray == NULL)
//	{
//		// Set all the endpoints to the stored selection state
//		if (RecordingSelected)
//			pStatePath->ClearSubSelection();
//		else
//			pStatePath->SetAllSubSelection();
//	}
//	else
//	{
//		// Set indexed endpoints to the stored selection state, and the others to the opposite
//		UINT32 ArrayIndex = 0;
//		for (UINT32 loop = 0; loop<NumElements; loop++)
//		{
//			if (pFlags[loop].IsEndPoint)
//			{
//				if ((ArrayIndex < NumStoredIndexes) && (pIndexArray[ArrayIndex] == loop))
//				{
//					pFlags[loop].IsSelected = RecordingSelected;
//					ArrayIndex ++;
//				}
//				else
//					pFlags[loop].IsSelected = !RecordingSelected;
//			}
//		}
//	}
//
//	// Clean up the control points selection state
//	pStatePath->EnsureSelection(TRUE);
//
//	return Act;
//}






/********************************************************************************************
>	BOOL OpCloseNodePaths::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95
	Returns:	TRUE if all went OK, FALSE if initalisation failed
	Purpose:	Adds the operation to the list of all known operations
********************************************************************************************/
BOOL OpCloseNodePaths::Init()
{
	const INT32 HID_AUTOCLOSEPATHS = 0;
	BTNOP( AUTOCLOSEPATHS, OpCloseNodePaths, ARRANGE)
	return TRUE;											 
}



/********************************************************************************************
>	OpState OpCloseNodePaths::GetState(String_256* Description, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95
	Outputs:	-
	Returns:	The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:	Find out the state of the operation at the specific time.  Greyed unless there
				is an open path with the start or end selected
********************************************************************************************/
OpState OpCloseNodePaths::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby(FALSE, TRUE);		// unticked and greyed

	// Get the selection range and the first selected object
	SelRange* pSelection = GetApplication()->FindSelection();
	ERROR2IF(pSelection == NULL, Blobby, "No SelRange!");
	Node* pSelNode = pSelection->FindFirst();

	// Test all selected objects
	while (pSelNode != NULL)
	{
		if (pSelNode->IsNodePath())
		{
			if (IsThisPathCloseable((NodePath*)pSelNode))
			{
				Blobby.Greyed = FALSE;
				break;
			}
		}

		pSelNode = pSelection->FindNext(pSelNode);
	}

	return Blobby;
}



/********************************************************************************************
>	virtual void OpCloseNodePaths::DoWithParam(OpDescriptor* pOpDesc, OpParam* pOpParam)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95
	Inputs:		pOpParam->Param1 = TRUE = close with curve, FALSE = close with line
				pOpParam->Param2 = TRUE = close with smooth, FALSE = close with cusp
	Outputs:	-
	Returns:	-
	Purpose:	Closes all the selected open paths whose ends are selected
********************************************************************************************/
void OpCloseNodePaths::DoWithParam(OpDescriptor* pOpDesc, OpParam* pOpParam)
{
	BeginSlowJob();

	BOOL ok = TRUE;
	AddCurveFlag = pOpParam->Param1;
	AddSmoothFlag = pOpParam->Param2;
	IsPathClosing = TRUE;
	
	// Start the sel operation
	if (ok)
		ok = DoStartSelOp(TRUE, TRUE);

	// Get the selection range and the first selected object
	SelRange* pSelection = GetApplication()->FindSelection();
	Node* pSelNode = pSelection->FindFirst();

	// Test all selected objects
	while (ok && (pSelNode != NULL))
	{
		if (pSelNode->IsNodePath())
		{
			if (IsThisPathCloseable((NodePath*)pSelNode))
			{
				NodePath *pPath = (NodePath*)pSelNode;
//				BOOL skip = FALSE;

				// Create and send a change message about this path edit
				ObjChangeParam ObjChange(OBJCHANGE_STARTING,ObjChangeFlags(), pPath, this);
				if (pPath->AllowOp(&ObjChange))
					ok = CloseThisPath(pPath);
			}
		}

		pSelNode = pSelection->FindNext(pSelNode);
	}

	// Inform all the parents of this node that it has been changed.
	if (ok)
	{
		ObjChangeParam ObjFinished(OBJCHANGE_FINISHED, ObjChangeFlags(), NULL, this);
		ok = UpdateChangedNodes(&ObjFinished);
	}

	if (!ok)
	{
		FailAndExecute();
		InformError();
	}

	End();
}



/********************************************************************************************
>	BOOL OpCloseNodePaths::CloseThisPath(NodePath* pPath)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95
	Inputs:		pPath - the path to close
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Sets up the member variables and then calls OpNodePathAddEndpoint::CompleteThisPath
				to do all the work in closing the path
********************************************************************************************/
BOOL OpCloseNodePaths::CloseThisPath(NodePath* pPath)
{
	PathFlags* Flags = NULL;
	PathVerb* Verbs = NULL;
	DocCoord* Coords = NULL;
	pPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);
	ERROR2IF( (Flags==NULL) || (Verbs==NULL) || (Coords==NULL), FALSE, "Path array pointer was NULL");
	INT32 NumCoords = pPath->InkPath.GetNumCoords();

	// Setup all member variables relating to this path
	InsertPosition = NumCoords-1;
	OriginalPath = pPath;
	StartMousePos = Coords[NumCoords-1];
	LastMousePos = Coords[NumCoords-1];
	StartSpread = pPath->FindParentSpread();
	UpdatePoint = -1;
	ERROR2IF( StartSpread==NULL, FALSE, "Path was not on a spread");
	
	// We also need to make a version of the path that we can change
	BOOL ok = BuildEditPath(LastMousePos);

	if (ok)
		ok = CompleteThisPath(StartMousePos);

	EditPath.ClearPath();
	
	return ok;
}



/********************************************************************************************
>	BOOL OpCloseNodePaths::IsThisPathCloseable(NodePath* pPath)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95
	Inputs:		pPath - a pointer to a path
	Outputs:	-
	Returns:	TRUE if the path can be autoclosed, FALSE if not
	Purpose:	Finds out wether a path can be autoclosed.  It can if it is not closed and
				the first or last endpoints in the last sub-path are selected.
********************************************************************************************/
BOOL OpCloseNodePaths::IsThisPathCloseable(NodePath* pPath)
{
	BOOL Closeable = FALSE;
	PathFlags* Flags = NULL;
	PathVerb* Verbs = NULL;
	pPath->InkPath.GetPathArrays(&Verbs, NULL, &Flags);
	if ((Flags==NULL) || (Verbs==NULL))
	{
		ERROR3("Path array pointer was NULL");
		return FALSE;
	}
	INT32 NumCoords = pPath->InkPath.GetNumCoords();

	// See if the end of the (last sub-path of the) path is open
	if (!(Verbs[NumCoords-1] & PT_CLOSEFIGURE))
	{
		// Get the coord of the start of the sub-path
		INT32 SubPathStart = NumCoords-1;
		pPath->InkPath.FindStartOfSubPath(&SubPathStart);

		// Is the path longer than one line and is the first or last point selected?
		if (SubPathStart < (NumCoords-2))
			Closeable = (Flags[NumCoords-1].IsSelected || Flags[SubPathStart].IsSelected);
	}

	return Closeable;
}


