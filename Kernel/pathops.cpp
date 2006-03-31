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
// Home of additional path operations - pathedit.cpp is getting too big

/*
*/

#include "camtypes.h"
#include "pathops.h"
								
#include "app.h"
#include "document.h"
#include "docview.h"
#include "nodepath.h"				  						
#include "pathedit.h"
#include "objchge.h"
//#include "peter.h"
#include "progress.h"
#include "fixmem.h"


DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNCREATE( OpBaseConvertPathSegment, SelOperation)
CC_IMPLEMENT_DYNCREATE( OpMakeSegmentsCurves, OpBaseConvertPathSegment)
CC_IMPLEMENT_DYNCREATE( OpMakeSegmentsLines, OpBaseConvertPathSegment)
CC_IMPLEMENT_DYNCREATE( OpReversePath, SelOperation)
CC_IMPLEMENT_DYNCREATE( OpMovePathPoint, TransOperation)
CC_IMPLEMENT_DYNCREATE( OpMenuSelectPathPoints, Operation)
CC_IMPLEMENT_DYNCREATE( OpSelectAllPathPoints, OpMenuSelectPathPoints)
CC_IMPLEMENT_DYNCREATE( OpDeSelectAllPathPoints, OpMenuSelectPathPoints)

CC_IMPLEMENT_DYNAMIC( NewPathCreatedMsg, Msg)
CC_IMPLEMENT_DYNAMIC( PathEditedMsg, Msg)
CC_IMPLEMENT_MEMDUMP( MovePointsParams, OpParam)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW



/********************************************************************************************
>	NewPathCreatedMsg::NewPathCreatedMsg(NodePath* pPath, Operation* CurOp, ActionList* Undos) 

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/94
	Inputs:		pPath - pointer to the newly created path
				CurOp - pointer to the currently active operation
				Undos - pointer to the operations undo list
	Purpose:	initilise a NewPathCreatedMsg
	SeeAlso:	NewPathCreatedMsg
********************************************************************************************/
NewPathCreatedMsg::NewPathCreatedMsg(NodePath* pPath, Operation* CurOp, ActionList* Undos)							
{                              
	CurrentOp = CurOp;
	NewPath = pPath;	
	UndoActs = Undos;
}



/********************************************************************************************
>	PathEditedMsg::PathEditedMsg(Path* pPath, Spread* pSpread, INT32 Index)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/12/94
	Inputs:		pPath - pointer to the edited path
				pSpread - pointer to the spread containing the path
				Index - the edited endpoint
	Purpose:	initilise a PathEditedMsg
	SeeAlso:	PathEditedMsg
********************************************************************************************/
PathEditedMsg::	PathEditedMsg(Path* pPath, Spread* pSpread, INT32 Index)
							
{                              
	EditPath = pPath;
	EditSpread = pSpread;
	EndPoint = Index;
}



/********************************************************************************************
>	OpBaseConvertPathSegment::OpBaseConvertPathSegment() 

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/95
	Purpose:	OpBaseConvertPathSegment constructor - does nothing itself
********************************************************************************************/
OpBaseConvertPathSegment::OpBaseConvertPathSegment(): SelOperation()								
{                              
}



/********************************************************************************************
>	OpState	OpBaseConvertPathSegment::BaseGetState(INT32 PathType)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/95
	Inputs:		Type of the path that ends a segment
	Returns:	The tick/grey state of OpBaseConvertPathSegment and derived classes
	Purpose:	For finding the state of this op
	SeeAlso:	OpState
********************************************************************************************/
OpState	OpBaseConvertPathSegment::BaseGetState(INT32 PathType)
{
	OpState OpSt;

#ifndef STANDALONE

 	SelRange*	Selected = GetApplication()->FindSelection();

	if ((Document::GetSelected() == NULL) || (Selected == NULL) )
	{	// There is no selected document or selrange is invalid
		OpSt.Greyed = TRUE;
	   	return OpSt;                                 
	}

	Node*	pNode = Selected->FindFirst();
	BOOL	FoundSegment = FALSE;
	BOOL	AllConverted = TRUE;
	BOOL	PrevSelected = FALSE;

	while ((pNode != NULL) && AllConverted)
	{
		if (pNode->IsNodePath() && ((NodePath*)pNode)->IsPathAllowable())
		{
			Path* ThisPath = &(((NodePath*)pNode)->InkPath);
			PathFlags* Flags = ThisPath->GetFlagArray();
			PathVerb* Verbs = ThisPath->GetVerbArray();
			INT32 UsedSlots = ThisPath->GetNumCoords();
			PrevSelected = FALSE;

			for (INT32 i=0; i<UsedSlots; i++)
			{
				if (Flags[i].IsEndPoint)
				{
					if (Flags[i].IsSelected)
					{
						if (PrevSelected && ((Verbs[i] & ~PT_CLOSEFIGURE) != PT_MOVETO) )
						{
							FoundSegment = TRUE;
							if ((Verbs[i] & ~PT_CLOSEFIGURE) != PathType)
								AllConverted = FALSE;
						}
						PrevSelected = TRUE;
					}
					else							
					{
						PrevSelected = FALSE;
					}
				}
			}
		}
		pNode = Selected->FindNext(pNode);
	}

	OpSt.Greyed = !FoundSegment;
	OpSt.Ticked = AllConverted && FoundSegment;

#endif	// #ifdef STANDALONE

	return OpSt;
}



/********************************************************************************************
>	void OpBaseConvertPathSegment::Do(OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/95
	Inputs:		OpDescriptor (unused)
	Purpose:	Performs the common SelRange scanning part of converting path segments.  Calls
				the ProcessSegment function in derived classes to do the actual processing
	Errors:		-
	SeeAlso:	-
********************************************************************************************/
void OpBaseConvertPathSegment::Do(OpDescriptor*)
{   
#ifndef STANDALONE

	// Obtain the current selections and the first node in the selection
	SelRange* Selected = GetApplication()->FindSelection();
	BOOL ok = (Selected != NULL);

	// Start the op
	BeginSlowJob();
	if (ok)
		ok = DoStartSelOp(TRUE,TRUE);

	// Check with the selrange it is ok to run this op
	ObjChangeFlags cFlags;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);
	if (ok)
	{
		if (!Selected->AllowOp(&ObjChange))
		{
			EndSlowJob();
			FailAndExecute();
			End();
			return;
		}
	}

	Node* pNode = Selected->FindFirst();
	NodePath* ThisPath = NULL;

	while (ok && (pNode != NULL))
	{	// we're only interested in NodePaths which have selected points
		BOOL DoThisNode = pNode->IsNodePath();
		if (DoThisNode)
			DoThisNode = (((NodePath*)pNode)->InkPath.IsSubSelection());
		if (DoThisNode)
			DoThisNode = (((NodePath*)pNode)->IsPathAllowable());

		if 	( DoThisNode )
		{
			// for convenience, cast the pointer to a pointer to a NodePath
			ThisPath = (NodePath*)pNode;

			// First get pointers to the arrays
			PathVerb* Verbs = NULL;
			PathFlags* Flags = NULL;
			DocCoord* Coords = NULL;
			ThisPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);
			INT32 NumCoords = ThisPath->InkPath.GetNumCoords();
			BOOL PrevSelected = FALSE;
			INT32 PrevPos = 0;

			// Set the NeedToRender flags
			INT32		loop;
			for (loop = 0; loop < NumCoords; loop++)
			{
				if (Flags[loop].IsEndPoint && Flags[loop].IsSelected)
					Flags[loop].NeedToRender = TRUE;
				else
					Flags[loop].NeedToRender = FALSE;
			}

			// Force a re-draw of the place where the path used to be
			if (ok)
				ok = (RecalcBoundsAction::DoRecalc(this, &UndoActions, ThisPath, TRUE) != AC_FAIL);

			// loop through the points
			for (loop = 0; loop<NumCoords; loop++)
			{
				if (Flags[loop].IsEndPoint)
				{	// its an endpoint
					if (Flags[loop].IsSelected)
					{	// which is selected
						if (PrevSelected && ((Verbs[loop] & ~PT_CLOSEFIGURE) == GetProcessPathType()) )
						{	//  and the previous was selected and it's a processable segment
							if (ok)
								ok = ProcessSegment(ThisPath, &loop, PrevPos);
							// Recache array pointers
							ThisPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);
							NumCoords = ThisPath->InkPath.GetNumCoords();
							Flags[loop].NeedToRender = TRUE;
						}
						PrevSelected = TRUE;
						PrevPos = loop;
					}
					else
						PrevSelected = FALSE;
				}
			}

			// Having finished processing this path go round and smooth it.
			DocCoord NewCoord;
			for (loop = 0; (ok && (loop < NumCoords)); loop++)
			{
				if (Verbs[loop] == PT_BEZIERTO && !(Flags[loop].IsEndPoint) && Flags[loop].IsSmooth)
				{
					NewCoord = ThisPath->InkPath.SmoothControlPoint(loop);
					if (ok && (NewCoord != Coords[loop]))
					{
						ok = DoAlterPathElement(ThisPath, loop, NewCoord, Flags[loop], Verbs[loop], FALSE);
						ThisPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);
					}
				}
			}

			ThisPath->InvalidateBoundingRect();

			// Force a redraw of the place where the path is now.
			if (ok)
				ok = (RecordBoundsAction::DoRecord(this, &UndoActions, ThisPath, TRUE) != AC_FAIL);
		}
		pNode = Selected->FindNext(pNode);
	}

	if (ok)
	{
		ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
		if (!UpdateChangedNodes(&ObjChange))
		{
			FailAndExecute();
			End();
			return;
		}
	}

	EndSlowJob();

	if (!ok)
	{	
		FailAndExecute();
		InformError();
	}

#endif

	End();
}



/********************************************************************************************

>	OpMakeSegmentsLines::OpMakeSegmentsLines() 

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Purpose:	OpMakeSegmentsLines constructor - does nothing itself

********************************************************************************************/
OpMakeSegmentsLines::OpMakeSegmentsLines()
{                              
}



 /********************************************************************************************

>	BOOL OpMakeSegmentsLines::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpMakeSegmentsLines initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/
BOOL OpMakeSegmentsLines::Init()
{
	return (RegisterOpDescriptor(	0, 
									_R(IDS_MAKELINES),
									CC_RUNTIME_CLASS(OpMakeSegmentsLines), 
									OPTOKEN_MAKELINESOP,
									OpMakeSegmentsLines::GetState,
									0,	/* help ID */
									_R(IDBBL_MAKELINES),
									0	/* bitmap ID */));
}               



OpState OpMakeSegmentsLines::GetState(String_256*, OpDescriptor*)
{
	return OpBaseConvertPathSegment::BaseGetState(PT_LINETO);
}



/********************************************************************************************
>	BOOL OpMakeSegmentsLines::ProcessSegment(NodePath* pPath, INT32* Index, INT32 PrevIndex)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/95
	Inputs:		pPath - points to the path to process
				Index - index of the PT_BEZIERTO segment to change into lines
				PrevIndex - index of the previois endpoint (other end of the segment to process)
	Outputs:	Index is changed to point to the new index of the end of the segment
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Performs the make selected segments into lines operation. 
	Errors:		-
	SeeAlso:	OpBaseConvertPathSegment::Do
********************************************************************************************/
BOOL OpMakeSegmentsLines::ProcessSegment(NodePath* pPath, INT32* Index, INT32 PrevIndex)
{
	// Get the path pointers
	PathVerb* Verbs;
	PathFlags* Flags;
	DocCoord* Coords;
	pPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);

	// Quick check
	ERROR2IF(((Verbs[*Index] & ~PT_CLOSEFIGURE) != PT_BEZIERTO), FALSE, "Unknown segment encountered");
	ERROR2IF(((PrevIndex+3) != *Index), FALSE, "Not two points between segment start and end");

//	BOOL ok = TRUE;

	DocCoord EndCoord = Coords[*Index];
	PathFlags EndFlags = Flags[*Index];
	PathVerb NewEndVerb = Verbs[*Index];
	NewEndVerb = (NewEndVerb == PT_BEZIERTO) ? PT_LINETO : PT_LINETO | PT_CLOSEFIGURE;

	BOOL DoneOK = TRUE;

	// Insert a line segment
	if (DoneOK)
		DoneOK = DoInsertPathElement(pPath, PrevIndex, EndCoord, EndFlags, NewEndVerb, FALSE);

	// Remove the curve segment
	if (DoneOK)
		DoneOK = DoDeletePathSection(pPath, PrevIndex+2, 3, FALSE);

	*Index = PrevIndex+1;

	return DoneOK;
}



/********************************************************************************************

>	OpMakeSegmentsCurves::OpMakeSegmentsCurves() 

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Purpose:	OpMakeSegmentsCurves constructor - does nothing itself

********************************************************************************************/
OpMakeSegmentsCurves::OpMakeSegmentsCurves()
{                              
}



/********************************************************************************************

>	BOOL OpMakeSegmentsCurves::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpMakeSegmentsCurves initialiser method.
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/
BOOL OpMakeSegmentsCurves::Init()
{
	return (RegisterOpDescriptor(	0, 
									_R(IDS_MAKECURVES),
									CC_RUNTIME_CLASS(OpMakeSegmentsCurves), 
									OPTOKEN_MAKECURVESOP,
									OpMakeSegmentsCurves::GetState,
									0,	/* help ID */
									_R(IDBBL_MAKELINES),
									0	/* bitmap ID */));
}               



OpState OpMakeSegmentsCurves::GetState(String_256*, OpDescriptor*)
{
	  return OpBaseConvertPathSegment::BaseGetState(PT_BEZIERTO);
}
    


/********************************************************************************************
>	BOOL OpMakeSegmentsCurves::ProcessSegment(NodePath* pPath, INT32* Index, INT32 PrevIndex)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/95
	Inputs:		pPath - points to the path to process
				Index - index of the PT_LINETO segment to change into lines
				PrevIndex - index of the previous endpoint (other end of the segment to process)
	Outputs:	Index is changed to point to the new index of the end of the segment
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Performs the make selected segments into lines operation. 
	Errors:		-
	SeeAlso:	OpBaseConvertPathSegment::Do
********************************************************************************************/
BOOL OpMakeSegmentsCurves::ProcessSegment(NodePath* pPath, INT32* Index, INT32 PrevIndex)
{
	// Get the path pointers
	PathVerb* Verbs;
	PathFlags* Flags;
	DocCoord* Coords;
	pPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);

	// Quick check
	ERROR2IF(((Verbs[*Index] & ~PT_CLOSEFIGURE) != PT_LINETO), FALSE, "Unknown segment encountered");
	ERROR2IF(((PrevIndex+1) != *Index), FALSE, "Points between segment start and end");

	BOOL ok = CarryOut(*Index, PrevIndex, pPath, this, &UndoActions);
	*Index = PrevIndex + 3;

	return ok;
}



/********************************************************************************************

>	static BOOL OpMakeSegmentsCurves::CarryOut(INT32 Count, INT32 PrevPos, NodePath* ThisPath, 
												UndoableOperation* pOp, ActionList* pActions)
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/10/94
	Inputs:		Count - pointer to the index of the PT_LINETO endpoint
				PrevPos - pointer to the index of the previous endpoint
				ThisPath - pointer to the path with the segment to alter
				pOP - pointer to the operation to attach the actions to
				pActions - points to the action list of the operation
	Returns:	TRUE if sucessful, FALSE if any of the actions failed
	Purpose:	Performs the actions of the make selected segments into lines operation. 
	Errors:		Checks the pointers passed for NULL
				Checks the path is valid before altering it
	SeeAlso:	OpMakeSegmentsCurves::Do

********************************************************************************************/

BOOL OpMakeSegmentsCurves::CarryOut(INT32 Count, INT32 PrevPos, NodePath* ThisPath, 
										UndoableOperation* pOp, ActionList* pActions)
{
#ifndef STANDALONE

	ERROR2IF(pOp == NULL,FALSE, "Operation pointer was NULL");
	ERROR2IF(ThisPath == NULL,FALSE, "Path pointer was NULL");

	DocCoord* Coords = ThisPath->InkPath.GetCoordArray();
//	PathFlags* Flags = ThisPath->InkPath.GetFlagArray();
	PathVerb* Verbs = ThisPath->InkPath.GetVerbArray();

	DocCoord	First;
	DocCoord	Second;
	PathFlags	NewFlags;
	NewFlags.IsEndPoint = FALSE; 
	NewFlags.NeedToRender = FALSE; 
	NewFlags.IsSelected = TRUE;
	NewFlags.IsSmooth = TRUE;
	NewFlags.IsRotate = TRUE;

//	By default position the control points one-third of the way along the line between the
//	first and second endpoints (changing the line into an identical line).  At the end of the 
//	make curves operation we can go round and smooth the entire path.
	First.x = Coords[PrevPos].x-(Coords[PrevPos].x-Coords[Count].x)/3;	  
	First.y = Coords[PrevPos].y-(Coords[PrevPos].y-Coords[Count].y)/3;
	Second.x = Coords[PrevPos].x-((Coords[PrevPos].x-Coords[Count].x)/3)*2;	  
	Second.y = Coords[PrevPos].y-((Coords[PrevPos].y-Coords[Count].y)/3)*2;

	PathVerb EndVerb = Verbs[Count];
	DocCoord EndCoord = Coords[Count];

	ERROR2IF((EndVerb & ~PT_CLOSEFIGURE) != PT_LINETO, FALSE, "Attempt to convert a non-line to a curve!");

	EndVerb = ((EndVerb & PT_CLOSEFIGURE) | PT_BEZIERTO) ;

	// After any of these actions the path must be OK.  
	BOOL DoneOK  = TRUE;

	// Insert a curve into the path
	if (DoneOK)
	{
		if ((PrevPos+2) >= ThisPath->InkPath.GetNumCoords())
		{
			DoneOK = ThisPath->InkPath.AddCurveTo(First, Second, EndCoord, &NewFlags);
		}
		else
		{
			ThisPath->InkPath.SetPathPosition(PrevPos+2);
			DoneOK = ThisPath->InkPath.InsertCurveTo(First, Second, EndCoord, &NewFlags);
		}
	}

	// Correct the end verb
	if (DoneOK)
	{
		Verbs = ThisPath->InkPath.GetVerbArray();
		Verbs[PrevPos+4] = EndVerb;
	}

	// Now insert an action to remove the curve
	if (DoneOK)
	{
		Action* UnAction;			
		ActionCode Act;				
		Act = RemovePathElementAction::Init(pOp, pActions, 3, PrevPos+2, (Action**)(&UnAction));
		if (Act == AC_OK)
			((RemovePathElementAction*)UnAction)->RecordPath(ThisPath);
		DoneOK = !(Act == AC_FAIL);
	}

	// Finally delete the line
	if (DoneOK)
		DoneOK = pOp->DoDeletePathSection(ThisPath, PrevPos+1, 1, FALSE);

	return DoneOK;

#else
	return TRUE;
#endif
}



/********************************************************************************************

>	OpMovePathPoint::OpMovePathPoint() 

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/94
	Purpose:	OpMovePathPoint constructor - does nothing itself

********************************************************************************************/
OpMovePathPoint::OpMovePathPoint(): TransOperation()								
{                              
}



 /********************************************************************************************

>	BOOL OpMovePathPoint::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/94
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpMovePathPoint initialiser method.
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/
BOOL OpMovePathPoint::Init()
{
	return (RegisterOpDescriptor(	0, 
									_R(IDS_MOVEPATHPOINT),
									CC_RUNTIME_CLASS(OpMovePathPoint), 
									OPTOKEN_MOVEPATHPOINT,
									OpMovePathPoint::GetState,
									0,	/* help ID */
									_R(IDBBL_MOVEPATHPOINT),
									0	/* bitmap ID */));
}               
    


/********************************************************************************************

>	OpState	OpMovePathPoint::GetState(String_256*, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/94
	Returns:	The state of the OpMovePathPoint - always available
	Purpose:	For finding the OpCyclePathSelectionFoward's state.  

********************************************************************************************/
OpState	OpMovePathPoint::GetState(String_256* UIDescription, OpDescriptor* fred)
{
	OpState OpSt;

	return(OpSt);   
}



/********************************************************************************************

>	void OpMovePathPoint::DoWithParam(OpDescriptor*, OpParam* Params)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Inputs:		OpDescriptor (unused)
				Params - pointer to the OpParam block (should be a MovePointsParams object)
	Purpose:	An operation to position elements in a path (in an undoable kind of way).
	Errors:		if Params contains a NULL pointer to a path.
				if Params contains a NULL pointer to the changes data.

********************************************************************************************/
void OpMovePathPoint::DoWithParam(OpDescriptor* Fred, OpParam* Params)
{
	// Cast parameter block to our type
	MovePointsParams*	MyParams = (MovePointsParams*)Params;

	ERROR3IF(MyParams == NULL, "Parameter block pointer was NULL");
	ERROR3IF(MyParams->ChangesCount == 0, "No elements to alter!");
	ERROR3IF(MyParams->PathChanges == NULL, "No changes data supplied (NULL pointer passed)");
	ERROR3IF(MyParams->PathToEdit == NULL, "No changes path supplied (NULL pointer passed)");

	// Do the above tests again for the retail build
	if ( (MyParams == NULL) || (MyParams->ChangesCount == 0) || (MyParams->PathChanges == NULL)
																|| (MyParams->PathToEdit == NULL) )
	{
		End();
		return;
	}

	// Get pointers to the path data.
	PathFlags*	Flags = MyParams->PathToEdit->InkPath.GetFlagArray();
	PathVerb*	Verbs = MyParams->PathToEdit->InkPath.GetVerbArray();
	DocCoord*	Coords= MyParams->PathToEdit->InkPath.GetCoordArray();
	INT32		NumElements = MyParams->PathToEdit->InkPath.GetNumCoords();
	BOOL		NotFailed = TRUE;

	// Tell the operation system to start.
	if (!DoStartTransOp(FALSE,MyParams->PathToEdit))
	{
		FailAndExecute();
		End();								
		return;
	}

	for (INT32 Loop = 0; (Loop < MyParams->ChangesCount) && NotFailed; Loop++)
	{
		// Get the index of the point to edit
		INT32 Index = MyParams->PathChanges[Loop].Element;
		DocCoord NewCoord = MyParams->PathChanges[Loop].Coordinate;

		ERROR3IF(((Index >= NumElements) || (Index < 0)), "Invalid index into path (either -ve or off end of path)");
		if ((Index >= NumElements) || (Index < 0))
		{
			FailAndExecute();
			End();								
			return;
		}

		// Find the start and end of the subpath containing index
		INT32 EndOfSubPathIndex = Index;
		MyParams->PathToEdit->InkPath.FindEndElOfSubPath(&EndOfSubPathIndex);
		INT32 StartOfSubPathIndex = EndOfSubPathIndex;
		while ((Verbs[StartOfSubPathIndex] != PT_MOVETO) && (StartOfSubPathIndex > 0))
			StartOfSubPathIndex--;
		BOOL IsSubPathClosed = (Verbs[EndOfSubPathIndex] & PT_CLOSEFIGURE);

		// Move the point to the required coordinate
		NotFailed = DoAlterPathElement(MyParams->PathToEdit, Index, NewCoord, Flags[Index], Verbs[Index]);
	
		// If we have just moved one endpoint of a closed path we will have to move the
		// other point too!
		if ( NotFailed && IsSubPathClosed )
		{
			if (Index == EndOfSubPathIndex)
			{
				NotFailed = DoAlterPathElement(MyParams->PathToEdit, StartOfSubPathIndex, NewCoord,
												Flags[StartOfSubPathIndex], Verbs[StartOfSubPathIndex]);
			}
			if (Index == StartOfSubPathIndex)
			{
				NotFailed = DoAlterPathElement(MyParams->PathToEdit, EndOfSubPathIndex, NewCoord,
													Flags[EndOfSubPathIndex], Verbs[EndOfSubPathIndex]);
			}
		}

		// Now if we have just moved a Bezier control point then we need to clear the
		// smooth and rotate flags from the associated endpoint.  We are lucky in that a 
		// control point's endpoint is at either +1 or -1 elements away from the control point.
		if (NotFailed && ( (Verbs[Index] == PT_BEZIERTO) && !Flags[Index].IsEndPoint) )
		{
			INT32 BezEndpoint = -1;

			if ((Verbs[Index+1] & ~PT_CLOSEFIGURE) == PT_BEZIERTO)
				BezEndpoint = Index + 1;
			else
			{
				ERROR3IF(((Verbs[Index-1] & ~PT_CLOSEFIGURE) != PT_BEZIERTO),"Invalid path detected");
				if ((Verbs[Index-1] & ~PT_CLOSEFIGURE) == PT_BEZIERTO)
					BezEndpoint = Index - 1;
				else
					NotFailed = FALSE;
			}

			// Having found the endpoint attack its flags.
			if (NotFailed)
			{
				NotFailed = ClearSmoothAndRotate(MyParams->PathToEdit,BezEndpoint);
			}

			// And the other control point if this is a closed path
			if (NotFailed && IsSubPathClosed)
			{
			 	if ( (Index == EndOfSubPathIndex-1) || (Index == EndOfSubPathIndex+1) )
					NotFailed = ClearSmoothAndRotate(MyParams->PathToEdit,StartOfSubPathIndex);
			 	if ( (Index == StartOfSubPathIndex-1) || (Index == StartOfSubPathIndex+1) )
					NotFailed = ClearSmoothAndRotate(MyParams->PathToEdit,EndOfSubPathIndex);
			}

			// Clear them from the moved control point
			if (NotFailed)
			{
				NotFailed = ClearSmoothAndRotate(MyParams->PathToEdit,Index);
			}

			// Also clear the flags from the opposite control point
			if (NotFailed)
			{
				INT32 OtherControlPoint = MyParams->PathToEdit->InkPath.FindOppositeControlPoint(Index);
				if (OtherControlPoint != -1)
				{
					NotFailed = ClearSmoothAndRotate(MyParams->PathToEdit,OtherControlPoint);
				}
			}
		}
	}

	// Now run along the path and smooth it.
	if (NotFailed)
	{
		DocCoord NewCoord;
		for (INT32 i = 0; (i < NumElements) && NotFailed; i++)
		{
			if (Verbs[i] == PT_BEZIERTO && !(Flags[i].IsEndPoint) && Flags[i].IsSmooth)
			{
				NewCoord = MyParams->PathToEdit->InkPath.SmoothControlPoint(i);
				if (NewCoord != Coords[i])
					NotFailed = DoAlterPathElement(MyParams->PathToEdit, i, NewCoord, Flags[i], Verbs[i]);
			}
		}
	}

	if (NotFailed)
		GetApplication()->FindSelection()->UpdateBounds();

	if (!NotFailed)
	{
		InformError();
		FailAndExecute();
	}

	End();
}



/********************************************************************************************

>	BOOL OpMovePathPoint::ClearSmoothAndRotate(NodePath* pPath, INT32 PathIndex)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/04/95
	Inputs:		pPath - points to the path to edit
				PathIndex - the index of the path element to clear the flags 
   	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	This function calls DoAlterPathElement to clear the Smooth and Rotate flags
				from the specified path endpoint
	Errors:		UndoableOperation::DoAlterPathElement

********************************************************************************************/

BOOL OpMovePathPoint::ClearSmoothAndRotate(NodePath* pPath, INT32 PathIndex)
{
	PathFlags*	Flags = pPath->InkPath.GetFlagArray();
	PathVerb*	Verbs = pPath->InkPath.GetVerbArray();
	DocCoord*	Coords= pPath->InkPath.GetCoordArray();

	PathFlags tFlags = Flags[PathIndex];
	tFlags.IsSmooth = FALSE;
	tFlags.IsRotate = FALSE;
	return DoAlterPathElement(pPath, PathIndex, Coords[PathIndex], tFlags, Verbs[PathIndex]);
}


/********************************************************************************************

>	MovePointsParams::MovePointsParams(NodePath* pPath, ElementCoord* ChangeData, INT32 NumChanges)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		pPath - Pointer to a path
				ChangeData - pointer to a block describing changes to be made to the path.
				NumChanges - the number of change data items in the change block
	Purpose:	Constructor for the parameter class to be passed to OpMovePathPoint::DoWithParam
	SeeAlso:	OpMovePathPoint::DoWithParam
	Errors:		

********************************************************************************************/
MovePointsParams::MovePointsParams(NodePath* pPath, ElementCoord* ChangeData, INT32 NumChanges) : OpParam( INT32(0), INT32(0) ) \
{ 
	PathToEdit = pPath;
	PathChanges = ChangeData;
	ChangesCount = NumChanges;
};



/////////////////////////////////////////////////////////////////////////////////////////////
// OpSelectPathPoints


/********************************************************************************************
>	static BOOL	OpMenuSelectPathPoints::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/04/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Initialises the two path select points operations
	SeeAlso:	OpSelectAllPathPoints, OpDeSelectAllPathPoints
********************************************************************************************/
BOOL OpMenuSelectPathPoints::Init()	
{
	BOOL ok = RegisterOpDescriptor(	0, 
									_R(IDS_SELECTALLPATHPOINTS),
									CC_RUNTIME_CLASS(OpSelectAllPathPoints), 
									OPTOKEN_SELECTALLPATHPOINTS,
									OpMenuSelectPathPoints::GetState);

	if (ok)
		ok = RegisterOpDescriptor(	0, 
									_R(IDS_DESELECTALLPATHPOINTS),
									CC_RUNTIME_CLASS(OpDeSelectAllPathPoints), 
									OPTOKEN_DESELECTALLPATHPOINTS,
									OpMenuSelectPathPoints::GetState);

	return ok;
}



/********************************************************************************************
>	static OpState OpMenuSelectPathPoints::GetState(String_256*, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/04/95
	Inputs:		(Unused)
	Outputs:	-
	Returns:	The state of the operation
	Purpose:	For getting the start of the operation
	SeeAlso:	-
********************************************************************************************/
OpState OpMenuSelectPathPoints::GetState(String_256*, OpDescriptor*)
{
	OpState Banana;

	return Banana;		// Mmmm.... Banana.....
}



/********************************************************************************************
>	BOOL OpMenuSelectPathPoints::DoAction(BOOL SelectPoints)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/04/95
	Inputs:		SelectPoints - TRUE to select all the points in the path
							 - FALSE to deslect all the points in the path
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure		    
	Purpose:	Common code for selecting or deselecting all the points in all the selected
				paths.
	SeeAlso:	-
********************************************************************************************/
BOOL OpMenuSelectPathPoints::DoAction(BOOL SelectPoints)
{
	SelRange* pSelection = GetApplication()->FindSelection();
	Node* pNode = pSelection->FindFirst();

	// Cycle through all the selected paths
	while (pNode != NULL)
	{
		if (pNode->IsNodePath())
		{
			NodePath* pPath = (NodePath*)pNode;
			Spread* pSpread = pNode->FindParentSpread();
			ERROR2IF(pSpread == NULL, FALSE, "NodePath didn't have a parent spread");

			// Remove the blobs from the path
			RenderRegion* pRegion = DocView::RenderOnTop(NULL, pSpread, ClippedEOR);
			while (pRegion != NULL)
			{
				pPath->RenderObjectBlobs(pRegion);
				pRegion = DocView::GetNextOnTop(NULL);
			}
			
			// Set the selection state
			const INT32 NumCoords = pPath->InkPath.GetNumCoords();
			for (INT32 loop = 0; loop<NumCoords; loop++)
			{
				pPath->InkPath.GetFlagArray()[loop].IsSelected = SelectPoints;
			}

			// Put the blobs back on the path
			pRegion = DocView::RenderOnTop(NULL, pSpread, ClippedEOR);
			while (pRegion != NULL)
			{
				pPath->RenderObjectBlobs(pRegion);
				pRegion = DocView::GetNextOnTop(NULL);
			}

			GetApplication()->UpdateSelection();
		}

	 	pNode = pSelection->FindNext(pNode);
	}

	return TRUE;
}	



/********************************************************************************************
>	void OpSelectAllPathPoints::Do(OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/04/95
	Inputs:		Unused
	Outputs:	-
	Returns:	-	    
	Purpose:	Selects all the points on the selected paths
	SeeAlso:	-
********************************************************************************************/
void OpSelectAllPathPoints::Do(OpDescriptor*)
{
	if (!DoAction(TRUE))
		InformError();

	End();
}



/********************************************************************************************
>	void OpDeSelectAllPathPoints::Do(OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/04/95
	Inputs:		Unused
	Outputs:	-
	Returns:	-	    
	Purpose:	Deselects all the points on the selected paths
	SeeAlso:	-
********************************************************************************************/
void OpDeSelectAllPathPoints::Do(OpDescriptor*)
{
	if (!DoAction(FALSE))
		InformError();

	End();
}



/********************************************************************************************

>	OpReversePath::OpReversePath()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/2000
	Inputs:		-
	Purpose:	Construct an object of this type.

********************************************************************************************/

OpReversePath::OpReversePath()
{
}



/********************************************************************************************

>	OpReversePath::~OpReversePath()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/2000
	Inputs:		-
	Purpose:	Destruct an object of this type.

********************************************************************************************/

OpReversePath::~OpReversePath()
{
}



/********************************************************************************************

>	OpReversePath::Init()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/2000
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpMakeSegmentsLines initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.

********************************************************************************************/

BOOL OpReversePath::Init()
{
	return (RegisterOpDescriptor(	0, 
									_R(IDS_MAKEREVERSE),
									CC_RUNTIME_CLASS(OpReversePath), 
									OPTOKEN_REVERSEPATH,
									OpReversePath::GetState,
									0,	/* help ID */
									_R(IDBBL_MAKEREVERSE),
									0	/* bitmap ID */));
}               



/********************************************************************************************

>	OpState OpReversePath::GetState (String_256* Description, OpDescriptor*)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/2000
	Returns:	Ungreyed, Unticked
	Purpose:	Determines the state of the op.

********************************************************************************************/

OpState OpReversePath::GetState (String_256* Description, OpDescriptor*)
{
	OpState OpSt;

 	SelRange*	Selected = GetApplication ()->FindSelection ();

	if ((Document::GetSelected () == NULL) || (Selected == NULL) )
	{	// There is no selected document or selrange is invalid 
		OpSt.Greyed = TRUE;
	   	return (OpSt);
	}

	if (Selected->Count () == 0)	// if there ain't no selection - return cause we ain't
									// going to be reversing anything
	{
		OpSt.Greyed = TRUE;
	   	return (OpSt);
	}

	Node* pNode = Selected->FindFirst ();

	// scan the selection and if even a single DoThisNode fails, then return that the op
	// is greyed immediately ....

	while (pNode != NULL)
	{	// we're only interested in NodePaths which have selected points
		BOOL DoThisNode = pNode->IsNodePath ();
		//if (DoThisNode)
		//	DoThisNode = (((NodePath*)pNode)->InkPath.IsSubSelection());
		if (DoThisNode)
			DoThisNode = (((NodePath*)pNode)->IsPathAllowable ());

		if (DoThisNode)
		{
			pNode = Selected->FindNext (pNode);
		}
		else
		{
			pNode = NULL;
			OpSt.Greyed = TRUE;
		}
	}

	return (OpSt);
}



/********************************************************************************************

>	void OpReversePath::Do (OpDescriptor*)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/2000
	Inputs:		OpDescriptor (unused)
	Purpose:	Does the work of reversing the paths.

********************************************************************************************/

void OpReversePath::Do (OpDescriptor*)
{
	// Obtain the current selections and the first node in the selection
	SelRange* Selected = GetApplication()->FindSelection();
	BOOL ok = (Selected != NULL);

	// Start the op
	BeginSlowJob();
	if (ok)
		ok = DoStartSelOp(TRUE,TRUE);

	// Check with the selrange it is ok to run this op
	ObjChangeFlags cFlags;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);
	if (ok)
	{
		if (!Selected->AllowOp(&ObjChange))
		{
			EndSlowJob();
			FailAndExecute();
			End();
			return;
		}
	}

	Node* pNode = Selected->FindFirst();
	NodePath* ThisPath = NULL;

	//Document* pDocument = GetWorkingDoc();

	while (ok && (pNode != NULL))
	{	// we're only interested in NodePaths which have selected points
		BOOL DoThisNode = pNode->IsNodePath();
		//if (DoThisNode)
		//	DoThisNode = (((NodePath*)pNode)->InkPath.IsSubSelection());
		if (DoThisNode)
			DoThisNode = (((NodePath*)pNode)->IsPathAllowable());

		if 	( DoThisNode )
		{
			// for convenience, cast the pointer to a pointer to a NodePath
			ThisPath = (NodePath*)pNode;

			// First get pointers to the arrays
			PathVerb* Verbs = NULL;
			PathFlags* Flags = NULL;
			DocCoord* Coords = NULL;
			ThisPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);
			INT32 NumCoords = ThisPath->InkPath.GetNumCoords();
//			BOOL PrevSelected = FALSE;
//			INT32 PrevPos = 0;

			ObjChangeFlags cFlags;
			cFlags.TransformNode = TRUE;
			ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,ThisPath,this);
			if (!ThisPath->AllowOp(&ObjChange, TRUE))
			{
				return;
			}

			// Set the NeedToRender flags
			for (INT32 loop = 0; loop < NumCoords; loop++)
			{
				if (Flags[loop].IsEndPoint && Flags[loop].IsSelected)
					Flags[loop].NeedToRender = TRUE;
				else
					Flags[loop].NeedToRender = FALSE;
			}

			// Force a re-draw of the place where the path used to be
			if (ok)
				ok = (RecalcBoundsAction::DoRecalc(this, &UndoActions, ThisPath, TRUE) != AC_FAIL);

			DoReversePath (ThisPath);

			// Force a redraw of the place where the path is now.
			if (ok)
				ok = (RecordBoundsAction::DoRecord(this, &UndoActions, ThisPath, TRUE) != AC_FAIL);
		}
		pNode = Selected->FindNext(pNode);
	}

	if (ok)
	{
		ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
		if (!UpdateChangedNodes(&ObjChange))
		{
			FailAndExecute();
			End();
			return;
		}
	}

	EndSlowJob();

	if (!ok)
	{	
		FailAndExecute();
		InformError();
	}

	End();
}
