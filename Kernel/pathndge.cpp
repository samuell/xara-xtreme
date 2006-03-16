// $Id: pathndge.cpp 662 2006-03-14 21:31:49Z alex $
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
// Implementation of the path nudge ops

/*
*/

#include "camtypes.h"
#include "pathndge.h"

#include "app.h"
//#include "markn.h"
#include "nodepath.h"
#include "opnudge.h"
#include "trans2d.h"
#include "pathedit.h"
#include "docview.h"
#include "document.h"
#include "ophist.h"

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNCREATE(OpPathNudge,TransOperation)


/********************************************************************************************

>	void IMPLEMENT_OPPATHNUDGE_CLASS(DIR,X_NUDGE_FACTOR,Y_NUDGE_FACTOR,UNDO_IDS)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> (via MarkN)
	Created:	14/10/94
	Inputs:		DIR 			= String postfix specifying the nudge direction and magnitude, e.g. Up1
				X_NUDGE_FACTOR 	= signed INT32 giving the x nudge magnitude and direction
				Y_NUDGE_FACTOR 	= y version of above
				UNDO_IDS		= IDS of undo string to append to the undo/redo men items
	Outputs:	-
	Returns:	
	Purpose:	The macro IMPLEMENT_OPPATHNUDGE_CLASS(DIR,X_NUDGE_FACTOR,Y_NUDGE_FACTOR,UNDO_IDS) implements
				the OpNudgeX Do() function.
				E.g. OpNudgeUp1

				Firstly, CC_IMPLEMENT_DYNCREATE(OpNudgeUp1,OpNudge) is done

				Then itsets the OpNudge protected member vars as follows-
					X_NudgeFactor = 0; 					// I.e. don't nudge in the X dir
					Y_NudgeFactor = 1; 					// I.e. Nudge up by 1 nudge step 
					NudgeUndoIDS  = _R(IDS_UNDO_NUDGE_UP); 	// This string appended to the undo/redo menu items

				It then calls OpNudge::Do() which uses the nudge factors to determine nudging magnitude and
				direction

	Errors:		-
	SeeAlso:	IMPLEMENT_OPNUDGE_CLASS

********************************************************************************************/

#define IMPLEMENT_OPPATHNUDGE_CLASS(DIR,X_NUDGE_FACTOR,Y_NUDGE_FACTOR,UNDO_IDS) \
	CC_IMPLEMENT_DYNCREATE(OpPathNudge ## DIR,OpPathNudge) \
	void OpPathNudge ## DIR::Do(OpDescriptor* pOpDesc) \
	{ \
		X_NudgeFactor = X_NUDGE_FACTOR; \
		Y_NudgeFactor = Y_NUDGE_FACTOR; \
		NudgeUndoIDS = UNDO_IDS; \
		OpPathNudge::Do(pOpDesc); \
	} \

IMPLEMENT_OPPATHNUDGE_CLASS (Up1,0,1,_R(IDS_UNDO_NUDGE_UP))
IMPLEMENT_OPPATHNUDGE_CLASS (Up5,0,5,_R(IDS_UNDO_NUDGE_UP))
IMPLEMENT_OPPATHNUDGE_CLASS (Up10,0,10,_R(IDS_UNDO_NUDGE_UP))
IMPLEMENT_OPPATHNUDGE_CLASS	(UpFifth,0,0.2,_R(IDS_UNDO_NUDGE_UP))
IMPLEMENT_OPPATHNUDGE_CLASS	(UpPixel1,0,1,_R(IDS_UNDO_NUDGE_UP))
IMPLEMENT_OPPATHNUDGE_CLASS	(UpPixel10,0,10,_R(IDS_UNDO_NUDGE_UP))


IMPLEMENT_OPPATHNUDGE_CLASS (Down1,0,-1,_R(IDS_UNDO_NUDGE_DOWN))
IMPLEMENT_OPPATHNUDGE_CLASS (Down5,0,-5,_R(IDS_UNDO_NUDGE_DOWN))
IMPLEMENT_OPPATHNUDGE_CLASS (Down10,0,-10,_R(IDS_UNDO_NUDGE_DOWN))
IMPLEMENT_OPPATHNUDGE_CLASS	(DownFifth,0,-0.2,_R(IDS_UNDO_NUDGE_DOWN))
IMPLEMENT_OPPATHNUDGE_CLASS	(DownPixel1	,0,-1,_R(IDS_UNDO_NUDGE_DOWN))
IMPLEMENT_OPPATHNUDGE_CLASS	(DownPixel10,0,-10,_R(IDS_UNDO_NUDGE_DOWN))

IMPLEMENT_OPPATHNUDGE_CLASS (Left1,-1,0,_R(IDS_UNDO_NUDGE_LEFT))
IMPLEMENT_OPPATHNUDGE_CLASS (Left5,-5,0,_R(IDS_UNDO_NUDGE_LEFT))
IMPLEMENT_OPPATHNUDGE_CLASS (Left10,-10,0,_R(IDS_UNDO_NUDGE_LEFT))
IMPLEMENT_OPPATHNUDGE_CLASS (LeftFifth,-0.2,0,_R(IDS_UNDO_NUDGE_LEFT))
IMPLEMENT_OPPATHNUDGE_CLASS	(LeftPixel1	,-1,0,_R(IDS_UNDO_NUDGE_LEFT))
IMPLEMENT_OPPATHNUDGE_CLASS	(LeftPixel10,-10,0,_R(IDS_UNDO_NUDGE_LEFT))


IMPLEMENT_OPPATHNUDGE_CLASS (Right1,1,0,_R(IDS_UNDO_NUDGE_RIGHT))
IMPLEMENT_OPPATHNUDGE_CLASS (Right5,5,0,_R(IDS_UNDO_NUDGE_RIGHT))
IMPLEMENT_OPPATHNUDGE_CLASS (Right10,10,0,_R(IDS_UNDO_NUDGE_RIGHT))
IMPLEMENT_OPPATHNUDGE_CLASS	(RightFifth,0.2,0,_R(IDS_UNDO_NUDGE_RIGHT))
IMPLEMENT_OPPATHNUDGE_CLASS	(RightPixel1,1,0,_R(IDS_UNDO_NUDGE_RIGHT))
IMPLEMENT_OPPATHNUDGE_CLASS	(RightPixel10,10,0,_R(IDS_UNDO_NUDGE_RIGHT))

#define new CAM_DEBUG_NEW

MILLIPOINT 	OpPathNudge::NudgeStep	 = 2835;	// The default size of a single nudge (1mm)
double		OpPathNudge::X_NudgeFactor = 1;		// Distance nudged in x = NudgeStep * X_NudgeFactor;
double		OpPathNudge::Y_NudgeFactor = 0;		// Distance nudged in y = NudgeStep * Y_NudgeFactor;


/********************************************************************************************

>	void OpPathNudge::Do(OpDescriptor* pOpDesc)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/94
	Inputs:		pOpDesc = ptr to the op descriptor
	Purpose:	The path nudge op's Do() function.
	SeeAlso:	OpNudge::Do()

********************************************************************************************/

void OpPathNudge::Do(OpDescriptor* pOpDesc)
{
	NudgeStep = OpNudge::GetNudgeStep();
	
	//	Get scaled pixel size 
	FIXED16 ScaledPixelWidth, ScaledPixelHeight;
	GetWorkingView()->GetScaledPixelSize(&ScaledPixelWidth, &ScaledPixelHeight) ;
	PixelNudge=ScaledPixelWidth.MakeDouble() ;

	// Determine the nudge factors based on the OpDescriptor used to invoke the nudge operation
	String OpToken = pOpDesc->Token; 

if		(OpToken == String(OPTOKEN_NUDGEUPPIXEL1))		{ PixelNudgeClass = TRUE; }
else if	(OpToken == String(OPTOKEN_NUDGEUPPIXEL10))		{ PixelNudgeClass = TRUE; }

else if	(OpToken == String(OPTOKEN_NUDGEDOWNPIXEL1))	{ PixelNudgeClass = TRUE; }
else if	(OpToken == String(OPTOKEN_NUDGEDOWNPIXEL10))	{ PixelNudgeClass = TRUE; }

else if	(OpToken == String(OPTOKEN_NUDGELEFTPIXEL1))	{ PixelNudgeClass = TRUE; }
else if	(OpToken == String(OPTOKEN_NUDGELEFTPIXEL10))	{ PixelNudgeClass = TRUE; }

else if	(OpToken == String(OPTOKEN_NUDGERIGHTPIXEL1))	{ PixelNudgeClass = TRUE; }
else if	(OpToken == String(OPTOKEN_NUDGERIGHTPIXEL10))	{ PixelNudgeClass = TRUE; }

else if	(OpToken == String(OPTOKEN_NUDGEUP1))			{ PixelNudgeClass = FALSE; }
else if	(OpToken == String(OPTOKEN_NUDGEUP10))			{ PixelNudgeClass = FALSE; }

else if	(OpToken == String(OPTOKEN_NUDGEDOWN1))			{ PixelNudgeClass = FALSE; }
else if	(OpToken == String(OPTOKEN_NUDGEDOWN10))		{ PixelNudgeClass = FALSE; }

else if	(OpToken == String(OPTOKEN_NUDGELEFT1))			{ PixelNudgeClass = FALSE; }
else if	(OpToken == String(OPTOKEN_NUDGELEFT10))		{ PixelNudgeClass = FALSE; }

else if	(OpToken == String(OPTOKEN_NUDGERIGHT1))		{ PixelNudgeClass = FALSE; }
else if	(OpToken == String(OPTOKEN_NUDGERIGHT10))		{ PixelNudgeClass = FALSE; }
	
	
	
	// Setup the Offset DocCoord to contain the X and Y translation values		
	DocCoord Offset;

if (PixelNudgeClass)
{
	Offset.x=(INT32)GetXPixelDisplacement();
	Offset.y=(INT32)GetYPixelDisplacement();
}

else if (!PixelNudgeClass)
{
	//TypeCast from double to INT32
	Offset.x=(INT32) GetXNudgeDisplacement();
	Offset.y=(INT32) GetYNudgeDisplacement();
}


	SelRange*	Selection = GetApplication()->FindSelection();
	Node*		pNode = Selection->FindFirst();
	NodePath*	ThisPath = NULL;
	BOOL		HaveNudged = FALSE;

	DocRect BoundingBox;
	BOOL BoundingBoxValid = FALSE;

	PathVerb*	Verbs = NULL;
	PathFlags*	Flags = NULL;
	DocCoord*	Coords = NULL;
	INT32 NumCoords = 0;
	BOOL ok = TRUE;
	
	// Only start an undo record if there is work to do!
	if (pNode != NULL)
		ok = DoStartTransOp(FALSE, NULL);

	while (ok && pNode!=NULL)
	{
		//scan tree for all selected paths
		if 	( IS_A(pNode,NodePath) && (((NodePath*)pNode)->InkPath.IsSubSelection()) )
		{
			// for convenience, cast the pointer to a pointer to a NodePath
			ThisPath = (NodePath*)pNode;

			// First get pointers to the arrays
			ThisPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);
			NumCoords = ThisPath->InkPath.GetNumCoords();
			BOOL ok = TRUE;

			for (INT32 i=0; i<NumCoords && ok; i++)
			{
				//  scan path for all selected endpoints
				if (Flags[i].IsEndPoint && Flags[i].IsSelected)
				{	
					DocCoord NewCoord = Coords[i] + Offset;
					ok = DoAlterPathElement(ThisPath, i, NewCoord, Flags[i], Verbs[i]);
					HaveNudged = TRUE;

					if (BoundingBoxValid)
					{
						BoundingBox.IncludePoint(NewCoord);
					}
					else
					{
						BoundingBox = DocRect(NewCoord, NewCoord);
						BoundingBoxValid = TRUE;
					}

					// Also need to nudge the associated control points if the rotate flag is set
					// Jim 31/8/96 - Changed to nudge control points always, not just when rotate flag set
					// Exeption Don't do the selected moveto in a closed subpath - the end of the 
					// will alsobe selected, that will handle this situation
					if (!(Verbs[i]== PT_MOVETO && ThisPath->InkPath.IsSubPathClosed(i)))
					{
						INT32 PrevIndex = ThisPath->InkPath.FindPrevControlPoint(i);
						if (PrevIndex!=-1 && ok)
						{
							DocCoord NewCoord = Coords[PrevIndex] + Offset;
							ok = DoAlterPathElement(ThisPath, PrevIndex, NewCoord, Flags[PrevIndex], Verbs[PrevIndex]);
						}

						INT32 NextIndex = ThisPath->InkPath.FindNextControlPoint(i);
						if (NextIndex!=-1 && ok)
						{
							DocCoord NewCoord = Coords[NextIndex] + Offset;
							ok = DoAlterPathElement(ThisPath, NextIndex, NewCoord, Flags[NextIndex], Verbs[NextIndex]);
						}
					}
				}
			}
		}

		pNode = Selection->FindNext(pNode);
	}

	if (ok && !HaveNudged)
	{
		// No points were selected so we need to move all the selected paths
		pNode = Selection->FindFirst();

		while (ok && pNode!=NULL)
		{
			if (IS_A(pNode,NodePath))
			{
				// Construct the translation matrix
				Trans2DMatrix* TransMat = new Trans2DMatrix(Offset.x, Offset.y);

				// And apply it to the path
				ok = DoTransformNode((NodeRenderableInk*)pNode, TransMat);
			}
			pNode = Selection->FindNext(pNode);
		}

		if (ok)
		{
			BoundingBox = Selection->GetBoundingRect();
			BoundingBoxValid = TRUE;
		}
	}
	
	if (ok)
	{
		if (BoundingBoxValid)
		{
			DocView* pDocView = DocView::GetSelected();
			if (pDocView != NULL)
			{
				pDocView->ScrollToShow(&BoundingBox, Offset);
			}
		}
	}
	else
	{
		InformError();
		FailAndExecute();
	}

	End();
}

/********************************************************************************************

>	void OpPathNudge::GetOpName(String_256* OpName) 

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/9/94
	Outputs:	The undo string for the operation
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
	SeeAlso:	OpNudge::GetOpName()

********************************************************************************************/

void OpPathNudge::GetOpName(String_256* OpName) 
{ 
	*OpName = String_256(NudgeUndoIDS);
}  



/********************************************************************************************
>	virtual void OpPathNudge::PerformMergeProcessing()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/1/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	If the previous operation was also a path nudge the the undo actions are merged
				to form one undo step.
	SeeAlso:	UndoableOperation::PerformMergeProcessing
********************************************************************************************/
void OpPathNudge::PerformMergeProcessing()
{
	if (pOurDoc==NULL)
	{
		ERROR3("This operation had no document pointer"); 
		return;
	}

	// Obtain a pointer to the operation history for the current document
	OperationHistory* pOpHist = &pOurDoc->GetOpHistory();
	if (pOpHist==NULL)
	{
		ERROR3("This document has no operation history"); 
		return;
	}
	ERROR3IF(pOpHist->FindLastOp() != this, "Last Op should be this op"); 
	
	// Is operation performed before this a path nudge?  Attempt a merge if so
	Operation* pPrevOp = pOpHist->FindPrevToLastOp();
	if ((pPrevOp != NULL) && pPrevOp->IS_KIND_OF(OpPathNudge))
	{
		BOOL PerformMerge = TRUE;
		
		// Count the number of actions.  They must be the same to merge.
		UINT32 NumPrevActions = pPrevOp->GetUndoActionList()->GetCount();
		UINT32 NumOurActions = GetUndoActionList()->GetCount();
		PerformMerge = (NumPrevActions == NumOurActions);

 		// Compare the actions to check we are operating on the same points on the same paths
		if (PerformMerge)
		{
			Action* pPrevAction = (Action*)pPrevOp->GetUndoActionList()->GetHead();
			Action* pOurAction = (Action*)GetUndoActionList()->GetHead();

			while (PerformMerge && pPrevAction!=NULL && pOurAction!=NULL)
			{
				// Actions must be of the same class
				PerformMerge = (pPrevAction->GetRuntimeClass() == pOurAction->GetRuntimeClass());

				// ModifyElementActions must reference the same path and index
				if (PerformMerge &&	IS_A(pPrevAction, ModifyElementAction))
				{
					const ModifyElementAction* pPrevModify = (ModifyElementAction*)pPrevAction;
					const ModifyElementAction* pOurModify = (ModifyElementAction*)pOurAction;

					PerformMerge = (pPrevModify->GetActionPath()==pOurModify->GetActionPath() &&
										pPrevModify->GetActionIndex()==pOurModify->GetActionIndex());
				}
								
				// If the nudge has used TransformNodeActions don't merge to avoid
				// redraw problems
				if (PerformMerge &&	IS_A(pPrevAction, TransformNodeAction))
					PerformMerge = FALSE;

				pPrevAction = (Action*)pPrevOp->GetUndoActionList()->GetNext(pPrevAction);
				pOurAction = (Action*)GetUndoActionList()->GetNext(pOurAction);
			}

			// Perform the merge
			if (PerformMerge)
			{
				// In order to merge these two ops we can just throw away this op!
				pOpHist->DeleteLastOp(); 
			}
		}
	}
}
