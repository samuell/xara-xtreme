// $Id: opnudge.cpp 662 2006-03-14 21:31:49Z alex $
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
// Implementation of the nudge ops

/*
*/

#include "camtypes.h"
#include "opnudge.h"
#include "app.h"
//#include "markn.h"
//#include "resource.h"
#include "range.h"
#include "spread.h"
#include "document.h"
#include "docview.h"
#include "ophist.h"


//Matt 11/11/00 - I only wanted a function from the next line...
#include "slicehelper.h"	//For helper functions

//But I had to include all of these to get it to work!!...
#include "cxfrech.h"		//For CamelotRecordHandler
#include "userattr.h"		//For UserAttr
#include "tmpltatr.h"		//For TemplateAttribute


DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNCREATE(OpNudge,OpTranslateTrans)

/*********************************************************************************************

	Preference:	StepSize
	Section:	Nudge
	Range:		0 to UINT_MAX
	Purpose:	This is the users nudge step size setting. It is the distance in millipoints
				that the nudge operation should move the object.  Holding down the CTRL key
				multiplies the nudge distance by 5, SHIFT multiplies it by 10.
	SeeAlso:	

**********************************************************************************************/ 

#define new CAM_DEBUG_NEW

// WEBSTER - markn 6/12/96
// Changed the default nudge size to a round number of pixels
#ifndef WEBSTER
MILLIPOINT 	OpNudge::NudgeStep	= 2835;	// The default size of a single nudge (1mm)
#else
MILLIPOINT 	OpNudge::NudgeStep	= 3750;	// The default size of a single nudge (5 pixels)
#endif // WEBSTER

/********************************************************************************************

>	static BOOL OpNudge::Init()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpNudge initialiser method.  Inits all the nudge OpDescriptors and reads the
				NudgeStep from the preferences file.
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

// This macro gives a short-hand way of registering all the nudge opdescriptors
// It may have to be updated if we every want to put nudging on buttons

#define REG_NUDGE_OP(NAME)\
	{\
		BOOL Blobby = RegisterOpDescriptor(\
						0,\
						_R(IDS_NUDGE),\
						CC_RUNTIME_CLASS(OpNudge),\
						OPTOKEN_ ## NAME,\
						OpNudge::GetState,\
						0,\
						0,\
						0,\
						0,\
						SYSTEMBAR_ILLEGAL,\
						TRUE,\
						FALSE,\
						FALSE,\
 					  	0,\
 					  	0 \
 					  );\
		ERROR1IF(!Blobby, FALSE, _R(IDS_OUT_OF_MEMORY));\
 	}\

// Finally, OpNudge::Init()

BOOL OpNudge::Init()
{
	REG_NUDGE_OP(NUDGEUP1);
	REG_NUDGE_OP(NUDGEUP5);
	REG_NUDGE_OP(NUDGEUP10);
	REG_NUDGE_OP(NUDGEUPFIFTH);
	REG_NUDGE_OP(NUDGEUPPIXEL1);
	REG_NUDGE_OP(NUDGEUPPIXEL10);

	REG_NUDGE_OP(NUDGEDOWN1);
	REG_NUDGE_OP(NUDGEDOWN5);
	REG_NUDGE_OP(NUDGEDOWN10);
	REG_NUDGE_OP(NUDGEDOWNFIFTH);
	REG_NUDGE_OP(NUDGEDOWNPIXEL1);
	REG_NUDGE_OP(NUDGEDOWNPIXEL10);

	REG_NUDGE_OP(NUDGELEFT1);
	REG_NUDGE_OP(NUDGELEFT5);
	REG_NUDGE_OP(NUDGELEFT10);
	REG_NUDGE_OP(NUDGELEFTFIFTH);
	REG_NUDGE_OP(NUDGELEFTPIXEL1);
	REG_NUDGE_OP(NUDGELEFTPIXEL10);

	REG_NUDGE_OP(NUDGERIGHT1);
	REG_NUDGE_OP(NUDGERIGHT5);
	REG_NUDGE_OP(NUDGERIGHT10);
	REG_NUDGE_OP(NUDGERIGHTFIFTH);
	REG_NUDGE_OP(NUDGERIGHTPIXEL1);
	REG_NUDGE_OP(NUDGERIGHTPIXEL10);

	//Camelot.DeclareSection(TEXT("Nudge"), 1);
	//Camelot.DeclarePref(TEXT("Nudge"), TEXT("StepSize"), (UINT32*)&OpNudge::NudgeStep, 0, UINT_MAX);

	ERROR2IF(OpNudge::NudgeStep<0, FALSE, "Preference nudge step was negative");

	return (TRUE);
}               
    
/********************************************************************************************

>	static OpState OpNudge::GetState(String_256*, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/94
	Inputs:		ptr to a string to place a description of what went wrong (if it did!)
	Outputs:	-
	Returns:	The state of the OpNudge operation
	Purpose:	For finding OpNudge's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpNudge::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;

	SelRange* pSelRange = GetApplication()->FindSelection();
	if (pSelRange != NULL && pSelRange->FindFirst() != NULL)
		OpSt.Greyed = FALSE;
	else
		OpSt.Greyed = TRUE;

	return (OpSt);   
}

/********************************************************************************************

>	void OpNudge::Do(OpDescriptor* pOpDesc)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/94
	Inputs:		pOpDesc = ptr to the op descriptor
	Outputs:	-
	Returns:	-
	Purpose:	The nudge op's Do() function.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void OpNudge::Do(OpDescriptor* pOpDesc)
{  
// Determine the nudge factors based on the OpDescriptor used to invoke the nudge operation
		String OpToken = pOpDesc->Token; 

//Get scaled pixel size 
	FIXED16 ScaledPixelWidth, ScaledPixelHeight;
	GetWorkingView()->GetScaledPixelSize(&ScaledPixelWidth, &ScaledPixelHeight) ;
	PixelNudge=ScaledPixelWidth.MakeDouble() ;


if		(OpToken == String(OPTOKEN_NUDGEUP1))				{ X_NudgeFactor = 0;	Y_NudgeFactor = 1;		flag=TRUE; }
else if (OpToken == String(OPTOKEN_NUDGEUP5))				{ X_NudgeFactor = 0;	Y_NudgeFactor = 5;		flag=TRUE; }
else if (OpToken == String(OPTOKEN_NUDGEUP10))				{ X_NudgeFactor = 0;	Y_NudgeFactor = 10;		flag=TRUE; }
else if (OpToken == String(OPTOKEN_NUDGEUPFIFTH))			{ X_NudgeFactor = 0;	Y_NudgeFactor = 0.2;	flag=TRUE; }
else if (OpToken == String(OPTOKEN_NUDGEDOWN1))				{ X_NudgeFactor = 0;	Y_NudgeFactor = -1;		flag=TRUE; }
else if (OpToken == String(OPTOKEN_NUDGEDOWN5))				{ X_NudgeFactor = 0;	Y_NudgeFactor = -5;		flag=TRUE; }
else if (OpToken == String(OPTOKEN_NUDGEDOWN10))			{ X_NudgeFactor = 0;	Y_NudgeFactor = -10;	flag=TRUE; }
else if (OpToken == String(OPTOKEN_NUDGEDOWNFIFTH))			{ X_NudgeFactor = 0;	Y_NudgeFactor = -0.2;	flag=TRUE; }
else if (OpToken == String(OPTOKEN_NUDGELEFT1))				{ X_NudgeFactor = -1;	Y_NudgeFactor = 0;		flag=TRUE; }
else if (OpToken == String(OPTOKEN_NUDGELEFT5))				{ X_NudgeFactor = -5;	Y_NudgeFactor = 0;		flag=TRUE; }
else if (OpToken == String(OPTOKEN_NUDGELEFT10))			{ X_NudgeFactor = -10;	Y_NudgeFactor = 0;		flag=TRUE; }
else if (OpToken == String(OPTOKEN_NUDGELEFTFIFTH))			{ X_NudgeFactor = -0.2;	Y_NudgeFactor = 0;		flag=TRUE; }
else if (OpToken == String(OPTOKEN_NUDGERIGHT1))			{ X_NudgeFactor = 1;	Y_NudgeFactor = 0;		flag=TRUE; }
else if (OpToken == String(OPTOKEN_NUDGERIGHT5))	 		{ X_NudgeFactor = 5; 	Y_NudgeFactor = 0;		flag=TRUE; }
else if (OpToken == String(OPTOKEN_NUDGERIGHT10))			{ X_NudgeFactor = 10;	Y_NudgeFactor = 0;		flag=TRUE; }
else if (OpToken == String(OPTOKEN_NUDGERIGHTFIFTH))		{ X_NudgeFactor = 0.2;	Y_NudgeFactor = 0;		flag=TRUE; }

else if (OpToken == String(OPTOKEN_NUDGEUPPIXEL1)) 			{ X_NudgeFactor = 0;	Y_NudgeFactor = 1;		flag=FALSE; }
else if (OpToken == String(OPTOKEN_NUDGEUPPIXEL10))			{ X_NudgeFactor = 0;	Y_NudgeFactor = 10;		flag=FALSE; }
else if (OpToken == String(OPTOKEN_NUDGEDOWNPIXEL1))		{ X_NudgeFactor = 0;	Y_NudgeFactor = -1;		flag=FALSE; }
else if (OpToken == String(OPTOKEN_NUDGEDOWNPIXEL10))		{ X_NudgeFactor = 0;	Y_NudgeFactor = -10;	flag=FALSE; }
else if (OpToken == String(OPTOKEN_NUDGELEFTPIXEL1))		{ X_NudgeFactor = -1;	Y_NudgeFactor = 0;		flag=FALSE; }
else if (OpToken == String(OPTOKEN_NUDGELEFTPIXEL10))		{ X_NudgeFactor = -10;	Y_NudgeFactor =	0;		flag=FALSE; }
else if (OpToken == String(OPTOKEN_NUDGERIGHTPIXEL1))		{ X_NudgeFactor = 1;	Y_NudgeFactor = 0;		flag=FALSE;	}
else if (OpToken == String(OPTOKEN_NUDGERIGHTPIXEL10))		{ X_NudgeFactor = 10;	Y_NudgeFactor = 0;		flag=FALSE; }

else
	{
		ERROR3("Unknown type of nudge");
	}



Range* Selection = GetApplication()->FindSelection();
RangeControl TransFlags = Selection->GetRangeControlFlags();
TransFlags.IgnoreNoneRenderable=TRUE;
TransFlags.IgnoreInvisibleLayers = TRUE;
Selection->SetRangeControl(TransFlags);
SliceHelper::ModifySelectionToContainWholeButtonElements();



// Setup the Offset DocCoord to contain the X and Y translation values
DocCoord Offset;

	if	(flag)
		{
			(Offset.x) = (INT32) GetXNudgeDisplacement();
			(Offset.y) = (INT32) GetYNudgeDisplacement();
		}
	else if	(!flag)
		{
			(Offset.x) = (INT32)GetXPixelDisplacement();
			(Offset.y) = (INT32)GetYPixelDisplacement(); 
		}

	// Will the nudge send the selection beyond the spread bounds?
	// If so, fail and return now
if (!IsNudgeOK(Offset.x,Offset.y)) 
	{
		SliceHelper::RestoreSelection();
		InformWarning(_R(IDS_NUDGE_OUTOFBOUNDS));
		FailAndExecute();
		End();	
		return;
	}

	TransformData 	TransData;

	// Set up the transform data
	TransData.CentreOfTrans.x 	= 0;
	TransData.CentreOfTrans.y 	= 0;
	TransData.StartBlob 	  	= 0;
	TransData.LockAspect 		= TRUE;
	TransData.LeaveCopy  		= FALSE;
	TransData.ScaleLines 		= FALSE;
	TransData.TransFills 		= TRUE;
	TransData.pRange = 0;

	// Call OpTranslateTrans::DoWithParams() with a ptr to the transform data and a ptr to a DocCoord
	// that specs the X and Y offsets of the translation
	OpParam param( &TransData, &Offset );
	DoWithParam( pOpDesc, &param );

	SliceHelper::RestoreSelection();
}

/********************************************************************************************

>	BOOL OpNudge::IsNudgeOK(BOOL dx,BOOL dy)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/96
	Inputs:		dx = the amount to nudge in the X direction
				dy = the amount to nudge in the Y direction
	Outputs:	-
	Returns:	FALSE if nudge will send the selection out of bounds, TRUE if OK
	Purpose:	A validity check function the tests to see if the nudge will send the nudge
				beyond the spread bounds
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpNudge::IsNudgeOK(BOOL dx,BOOL dy)
{
	// Get the selection
	SelRange* pSel = GetApplication()->FindSelection();
	ERROR2IF(pSel == NULL,FALSE,"Awooga, NULL sel range");

	// Find out the bounding rectangle of the selection
	DocRect BoundingRect = pSel->GetBoundingRect();

	// Find out the Pasteboard rect
	DocRect PasteRect;
	Spread* pSpread = pOurDoc->GetSelectedSpread();
	if (pSpread==NULL)
		PasteRect = BoundingRect;
	else
	{
		// Try to make the pasteboard grow if necessary to include the new object position
		// This is very quick if the pasteboard is large enough already.
		pSpread->ExpandPasteboardToInclude(BoundingRect);

		// And re-read the bounding rectangle of the selection, as the pasteboard resize may have
		// caused the origin of our entire coordinate space to have moved! Argh!
		BoundingRect = pSel->GetBoundingRect();
//		BoundingRect.Translate(dx,dy);

		// This is in Document Coords, so we need to convert it
		PasteRect = pSpread->GetPasteboardRect();
		pSpread->DocCoordToSpreadCoord(&PasteRect);
	}

	// Assume the nudge will be OK.
	BOOL NudgeOK = TRUE;

	if (PasteRect.ContainsRect(BoundingRect))
	{
		// Untranslated bounds fit inside pasteboard rect, so we must complain if the bounds
		// do not fit *after* translation

		// Translate the bounds by the nudge values
		BoundingRect.Translate(dx,dy);

		// Do the translated bounds still fit entirely in the pasteboard rect?
		NudgeOK = PasteRect.ContainsRect(BoundingRect);
	}
	else
	{
		// The original bounds overlap the pasteboard rect, so we must complain if the user
		// nudges the bounds completely out of sight

		if (PasteRect.IsIntersectedWith(BoundingRect))
		{
			// The original bounds intersect with the pasteboard rect
			BoundingRect.Translate(dx,dy);

			// Only allow the nudge if the translated bounds still overlap with the spread.
			NudgeOK = PasteRect.IsIntersectedWith(BoundingRect);
		}
	}

	// If the nudge is OK, we may need to scroll the DocView?

/*	Jim, 12/9/96 - removed this because we don't want to scroll to show opn nudges

	if (NudgeOK)
	{
		DocCoord Coord(0,0);

		// If nudging left or right, pick the min or max X coord
		if (dx != 0)
		{
			if (dx < 0)
				Coord.x = BoundingRect.lox;
			else
				Coord.x = BoundingRect.hix;
		}

		// If nudging up or down, pick the max or min Y coord
		if (dy != 0)
		{
			if (dy < 0)
				Coord.y = BoundingRect.loy;
			else
				Coord.y = BoundingRect.hiy;
		}

		// If we have picked a coord, ensure that this coord is visible in the selected spread
		// of the selected DocView
		if (Coord != DocCoord(0,0))
		{
			DocView* pDocView = DocView::GetSelected();
			if (pDocView != NULL)
				pDocView->ScrollToShow(&Coord);
		}
	}
*/
	return NudgeOK;
}


/********************************************************************************************

>	void OpNudge::GetOpName(String_256* OpName) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/9/94
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
					
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpNudge::GetOpName(String_256* OpName) 
{ 
	*OpName = String_256(_R(IDS_UNDO_NUDGE));
}  


/********************************************************************************************

>	virtual void OpNudge::PerformMergeProcessing() 


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This virtual function gets called to allow for the merging of nudge operations
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpNudge::PerformMergeProcessing()
{	 
	// Obtain a pointer to the operation history for the current document
	OperationHistory* pOpHist = &pOurDoc->GetOpHistory();

	// Ensure that we are the last operation added to the operation history
	// Note cannot be an ERROR2 cos this function cannot fail. 
	ERROR3IF(pOpHist->FindLastOp() != this, "Last Op should be this op"); 
	
	// OK lets see if the operation performed before this was an OpNudge operation
	Operation* pPrevOp = pOpHist->FindPrevToLastOp();

	if (pPrevOp != NULL)   // Check if there was a previous op
	{	
	
		if (IS_A(pPrevOp, OpNudge))
		{
			// Yes it was
			// We can merge this op with pPrevOp if they both apply to the same set of objects
			// This will be TRUE is their SelectionStates are the same. 
			RestoreSelectionsAction* pRestoreSelAct = (RestoreSelectionsAction*)
				GetUndoActionList()->FindActionOfClass(CC_RUNTIME_CLASS(RestoreSelectionsAction)); 	
			ERROR3IF(pRestoreSelAct == NULL, "This op should have a RestoreSelectionsAction");
			SelectionState* ThisOpsSelection = pRestoreSelAct->GetSelState();
		
			pRestoreSelAct = (RestoreSelectionsAction*)
				pPrevOp->GetUndoActionList()->FindActionOfClass(CC_RUNTIME_CLASS(RestoreSelectionsAction)); 	
			ERROR3IF(pRestoreSelAct == NULL, "OpNudge op should have a RestoreSelectionsAction");
			SelectionState* LastOpsSelection = pRestoreSelAct->GetSelState();
 
			if ((*ThisOpsSelection) == (*LastOpsSelection))
			{

				// scan to see if either of these ops hides a node
				// if either do then we cannot merge them together
				// this can happen if perhaps the extending definitions
				// were changed by the nudge (sjk 27-7-00)
				if (DoesActionListHideNodes(pPrevOp) || DoesActionListHideNodes(this) )
					return;

				// this op can be merged into the previous op, we simply need to combine the 
				// TransformNodeActions
				TransformNodeAction* pTransNdAct = (TransformNodeAction*)
					GetUndoActionList()->FindActionOfClass(CC_RUNTIME_CLASS(TransformNodeAction)); 
				ERROR3IF(pTransNdAct == NULL, "This op should have a TransformNodeAction"); 
				
				TransformNodeAction* pLastOpsTransNdAct = (TransformNodeAction*)
					pPrevOp->GetUndoActionList()->FindActionOfClass(CC_RUNTIME_CLASS(TransformNodeAction)); 
				ERROR3IF(pLastOpsTransNdAct == NULL,"OpNudgeOp should have a TransformNodeAction"); 

				pLastOpsTransNdAct->CombineWith(pTransNdAct); 

				// This op is no longer required, so let's vape it
				pOpHist->DeleteLastOp(); 
			}
		}
	}
	return;    	
}

/********************************************************************************************

>	BOOL OpNudge::DoesActionListHideNodes(OpNudge * pOp)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/00
	Inputs:		ptr to the (nudge) operation
	Outputs:	-
	Returns:	TRUE if the action list for the op does a HideNodesAction
	Purpose:	To disallow the merging of nudge operations if a HideNodesAction was added
				to this nudge op, as this wouldn't then undo correctly.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL OpNudge::DoesActionListHideNodes(Operation * pOp)
{
	ActionList * pActions = pOp->GetUndoActionList();
	ListItem* CurrentAction = pActions->GetHead();

	while (CurrentAction != NULL) // For each action in the list
	{
		if (IS_A(CurrentAction, HideNodeAction))
			return TRUE;

		// Get next action 
		CurrentAction = pActions->GetNext(CurrentAction);
	}

	return FALSE;
}

