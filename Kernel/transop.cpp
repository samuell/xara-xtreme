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
/*
// */

#include "camtypes.h"

#include "app.h"
#include "transop.h" 
#include "document.h"
#include "selstate.h"
#include "ensure.h"
#include "trans2d.h"
#include "ink.h"
#include "spread.h"
#include "layer.h"
//#include "progress.h"
#include "selector.h"
#include "objchge.h"
#include "keypress.h"
//#include "rik.h"
//#include "clikdrag.h"
#include "cutop.h"
//#include "mario.h"
#include "docview.h"
#include "nodecont.h"
//#include "dragtool.h"	// for DragTool.

//Matt 6/11/00 - I only wanted a function from the next line...
#include "slicehelper.h"	//For helper functions

//But I had to include all of these to get it to work!!...
#include "cxfrech.h"		//For CamelotRecordHandler
#include "userattr.h"		//For UserAttr
#include "tmpltatr.h"		//For TemplateAttribute
#include "ophist.h"

//Graham 30/9/96: Need to maintain a node list for Drop Copy
#include "list.h"
#include "range.h"

// declare this files version number to the program and give our name in memory dumps
DECLARE_SOURCE("$Revision$");
CC_IMPLEMENT_DYNCREATE(TransOperation, UndoableOperation)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

// Control how the dragged copies of the selection are modified during solid dragging
// 1 = delete and recopy objects (fails when background render suspends on object that gets deleted)
// 2 = copy object contents (fails when the two trees get mismatched)
// 3 = transform copies back to origin (fails when the scale gets very small, matrix inversion fails, errors get magnified horribly)
// 4 = Combined: transform most of the time, but if scale gets very small fall back to delete and recopy

#define SOLID_TRANSFORM_METHOD 4

/********************************************************************************************

	Preference:	ClickWhileDragFunc
	Section:	Mouse
	Range:		0 to 3
	Purpose:	Determine the function of a. clicking while dragging and b. the Num +
				key while dragging.

				0,1 - Leave copy
				2,3 - Drop copy

				At present, both a. and b. have the same function. However, this
				preference is set up with four values so that each of a. and b.
				could have either of the two functions.

********************************************************************************************/
INT32 TransOperation::ClickWhileDragFunc = 3;
                                  

/********************************************************************************************

>	TransOperation::TransOperation()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/94
	Purpose:	TransOperation constructor, calls the constructor of the base  class

********************************************************************************************/

TransOperation::TransOperation(): UndoableOperation()
{
	// Give all the flags default values
	LockAspect = FALSE;
	LeaveCopy = TRUE;

	// The scale factor is used to scale the line widths and by default it is ignored
	ScaleLines = FALSE;
	CanScaleLines = TRUE;
	TransFills = TRUE;

	StatusHelpID = 0;
	StatusHelpID2=0;

	//Graham: We want this drag to transform the objects when it ends
	AbortTransAtEnd=FALSE;

	ObjectUnderCursor = NULL;
	SelState = NULL;
	RecordTwice = FALSE;

	m_pSelList = NULL;
	m_pDraggedRange = NULL;
	m_pTransformRange = NULL;
	m_bFirstRedraw = FALSE;

	m_PrevTransform.SetTransform();

	m_bShowDraggedOutlines = FALSE;
	m_bShowOriginalOutlines = FALSE;
	m_bRangeCacheTransformed = TRUE;
	m_bRangeFullyTransformed = TRUE;
}

/********************************************************************************************

>	TransOperation::~TransOperation()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/94
	Purpose:	TransOperation destructor 

********************************************************************************************/

TransOperation::~TransOperation()
{
	if (m_pSelList)
	{
		m_pSelList->DeleteAll();
		delete m_pSelList;
		m_pSelList = NULL;
	}

	if (m_pDraggedRange)
	{
		delete m_pDraggedRange;
		m_pDraggedRange = NULL;
	}

	if (m_pTransformRange)
	{
		delete m_pTransformRange;
		m_pTransformRange = NULL;
	}

}

/********************************************************************************************

>	BOOL TransOperation::DoStartTransOp(BOOL RecordSelTwice, Node* NodeToTransform = NULL,
										Range* RangeToTransform = NULL)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/94
	Inputs:		RecordSelTwice - TRUE if the selection should be recorded twice (once at
				the start of the operation and once at the end). If it is FALSE the selection
				is only recorded at the start of the operation. This should only be set to TRUE
				if the selection is different at the end of the operation (This is quite rare)
				
				NodeToTransform: Specifies the object that will be transformed. 
				The default NULL value specifies that the entire selection will be transformed.
				
				RangeToTransform: Specifies the range of objects to use.
				The default NULL value specifies that the entire selection is the range.
				At present only used when NodeToTransform is NULL. Used by page resizing to
				move all the objects on the page regardless of selection. Hence, it does not
				call the AllowOp if the range is selected as AllowOp errors if no selection on
				things like NodeBlend and NodeText.
			

	Returns:	TRUE if successful, FALSE if memory ran out (call End)
	Purpose:	This function must be called by all TransOperations. It does the following:
				Records the current selection status  
				Invalidates the rectangle covered by the selection and its blobs. 
	SeeAlso:	TransOperation::End()

********************************************************************************************/

BOOL TransOperation::DoStartTransOp(BOOL RecordSelTwice, Node* NodeToTransform,
									Range* RangeToTransform)
{
	// Create a SelectionState object
	ALLOC_WITH_FAIL(SelState, (new SelectionState()), this); 
	if (SelState == NULL)
	{
		return FALSE; // Failure 
	}

	// We have managed to create a SelectionState instance, now lets try and 
	// record the current selections 

	BOOL Success;
	
	CALL_WITH_FAIL(SelState->Record(), this, Success)

	if (!Success)  // We failed to allocate enough memory to store the selection state
	{
		// There was insufficient memory to record the selections 
		delete SelState; 			// Delete the selection state 
		SelState = NULL; 
		return FALSE; 
	}
	
	// We have successfully managed to create a Selection state, create an action 
	// to restore the selections when executed 

	// Recorded the current selection state ok
	RestoreSelectionsAction* RestoreAct;  
   	ActionCode ActCode;
	RecordTwice = RecordSelTwice;

	// Attempt to initialise the action    
    ActCode = RestoreSelectionsAction::Init(this,                    
							     			&UndoActions,  
							     			SelState, 
											!RecordTwice,	// # Toggle
											FALSE,			// This action does not	restore the sels
											 				// restore the selection state but
											 				// its twin will
											!RecordTwice,	// The SelState is shared
											FALSE,			// Don't draw the selection blobs
											FALSE,
											FALSE,			// End restore					
					 			 			( Action**)(&RestoreAct)); 

	if (ActCode == AC_FAIL)
	{
		delete SelState; // We won't be needing this 
		return FALSE;  
	}

	// Create our local transform range
	SetTransformRange(RangeToTransform, NodeToTransform);

	// Ask the range whether it's OK to do the transform...
	// Get an ObjChangeParam ready, so we can ask op permission from nodes.
	ObjChangeFlags cFlags(FALSE,FALSE,FALSE,FALSE,FALSE,TRUE);	// flag this is a transform
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);
	BOOL bOK = m_pTransformRange->AllowOp(&ObjChange);
	if (!bOK)
		return FALSE;

	// Invalidate the regions covered by all the selected objects and their blobs 
//	return (DoInvalidateNodesRegions(*m_pTransformRange, TRUE)); 
	bOK = DoSmartInvalidate();
	return bOK;
}
 


/********************************************************************************************

>	BOOL TransOperation::DoEndTransOp() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/94
	Purpose:	This function gets called automatically by End()
				It Invalidates the rectangle covered by the selection and its blobs	
	Scope:		protected

********************************************************************************************/

BOOL TransOperation::DoEndTransOp() 
{
	// If this is a translation and the nodes all managed to transform their cached
	// info successfully then we should avoid forcing them to recache now.
	// So we must do some cleverer recaching
	BOOL bOK = DoSmartInvalidate();
	if (!bOK) return FALSE;

	// If we want to record the selection again, then build a new Selection State object
	// and record the selection into it.
	if (RecordTwice)
	{
		// Create a SelectionState object
		ALLOC_WITH_FAIL(SelState, (new SelectionState()), this); 
		if (SelState == NULL)
		{
			return FALSE; // Failure 
		}

		// We have managed to create a SelectionState instance, now lets try and 
		// record the current selections 

		BOOL Success; 
	
		CALL_WITH_FAIL(SelState->Record(), this, Success)

		if (!Success)  // We failed to allocate enough memory to store the selection state
		{
			// There was insufficient memory to record the selections 
			delete SelState; 			// Delete the selection state 
			SelState = NULL; 
			return FALSE; 
		}
	
		// We have successfully managed to create a Selection state, create an action 
		// to restore the selections when executed 
	}

	// Recorded the current selection state ok
	RestoreSelectionsAction* RestoreAct;  
   	ActionCode ActCode;

	// The SelState should have been created in the DoStartSelectionOp method. 
	// maybe it was never called !!!
   	ERROR2IF(SelState==NULL, FALSE, "We don't have a selection state");

	// Attempt to initialise the action    
    ActCode = RestoreSelectionsAction::Init(this,                    
							     			&UndoActions,  
							     			SelState, 
											!RecordTwice,	// Toggle
											TRUE,			// This action does not	restore the sels
											 				// restore the selection state but
											 				// its twin will
											!RecordTwice,	// The SelState is shared
											FALSE,			// Don't draw the selection blobs
											FALSE,
											TRUE,			// End restore					
					 			 			( Action**)(&RestoreAct)); 

	// See if it all worked
	if (ActCode == AC_FAIL)
	{
		// We won't be needing this 
		delete SelState;
		return FALSE;  
	}

	// Inform all changed nodes that we have finished
	ObjChangeFlags cFlags;
	ObjChangeParam ObjChange(OBJCHANGE_FINISHED,cFlags,NULL,this);
	ObjChange.SetRetainCachedData(Transform.IsTranslation() && m_bRangeCacheTransformed);	// Same condition in DoSmartInvalidate
	BOOL ok = UpdateChangedNodes(&ObjChange);

	// BODGE TEXT - quick (harmless?) fix for bug 'transforming 2 selected text stories => sel blobs incorrect'
	// it is actually due to the fact that update change nodes is not called before the sel bounds are recached
	SelRange* pSelRange = Camelot.FindSelection();
	if (pSelRange != NULL)
	{
		BOOL bOldValue = pSelRange->SetPromoteToParent(TRUE);

		pSelRange->UpdateBounds();

		// Try to make sure that the spread's pasteboard is big enough to include the transformed 
		// objects. This is very quick if the pasteboard is big enough already.
		Node *Bob = pSelRange->FindFirst();
		if (Bob != NULL)
		{
			Spread *pSpread = Bob->FindParentSpread();
			
			DocRect IncludeRect = pSelRange->GetBoundingRect();
			pSpread->ExpandPasteboardToInclude(IncludeRect);
		}

		pSelRange->SetPromoteToParent(bOldValue);
	}

	return ok;
}




/********************************************************************************************

>	BOOL TransOperation::DoSmartInvalidate()


	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/12/2004
	Purpose:	Invalidate the transform range of nodes but be careful
				about releasing cache info so that translations can retain cached info
				if possible

********************************************************************************************/

BOOL TransOperation::DoSmartInvalidate()
{
	// Issue undoable redraws...
	if (m_pTransformRange && !DoInvalidateNodesRegions(*m_pTransformRange, TRUE, FALSE, FALSE, FALSE))
		return FALSE;

	// Conditionally update cached info
	Node* pNode = m_pTransformRange->FindFirst();
	while (pNode)
	{
		if (pNode->IsBounded())
		{
			NodeRenderableBounded* pBoundNode = (NodeRenderableBounded*)pNode;

			// If we have transformed all cached data successfully
			// And 
			//		We are translating (in which case the cached data will remain usable)
			//		Or
			//		The node was Direct (in which case the cached data comes from outside the tree where it can't have been affected by the transform)
			// Then
			//		Don't force the recacheing of the data for this node, just its parents
			if (m_bRangeCacheTransformed && (Transform.IsTranslation() || pBoundNode->HasCachedDirectBitmap()))
				pBoundNode->ReleaseCached(TRUE, FALSE, FALSE, TRUE);	// Parents and derived data only
			else
				pBoundNode->ReleaseCached(TRUE, TRUE, TRUE, TRUE);		// Parents, children, self and derived data
		}

		pNode = m_pTransformRange->FindNext(pNode);
	}

	return TRUE;
}

	
	
	
/********************************************************************************************

>	virtual void TransOperation::End()


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/94
	Purpose:	Calls DoEndTransOp(), then Operation::End()

********************************************************************************************/

void TransOperation::End()
{
	if (OpStatus == DO && (!OpFlags.Failed))
	{
		// We don't want to perform this if we are undoing or redoing do we
		DoEndTransOp();
	}

	Operation::End(); 
}







/********************************************************************************************

>	void TransOperation::DoWithParam(OpDescriptor* pDescriptor, OpParam* pOpParam)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		pDescriptor - The operations descriptor
				pOpParam - The parameters for this operation
	Purpose:	Does an immediate version of the transform. This attempts to transform the
				selection and build all the neccesary Undo information. Derived class only
				need overide InitTransformImmediate() to set up the specifics of there own
				transform unless they need to do something not covered by this base class
				version

********************************************************************************************/

void TransOperation::DoWithParam(OpDescriptor* /*pDescriptor*/, OpParam* pOpParam)
{
	// Get at my data
	TransformData* TransData = (TransformData *)(void *)(pOpParam->Param1);

	// copy across all the relavent bits of data for the transform
	CentreOfTrans 	= TransData->CentreOfTrans;
	LockAspect 		= TransData->LockAspect;
	LeaveCopy 		= TransData->LeaveCopy;
	ScaleLines 		= TransData->ScaleLines;
	TransFills 		= TransData->TransFills;

	SetTransformRange(TransData->pRange, NULL);

	// Where was this operation started from, and are we interested
	SetStartBlob(TransData->StartBlob);

	// Allow the specific transforms to set up anything that they may need to set
	InitTransformImmediate(pOpParam);

	// Build the transform matrix
	BuildMatrix();

	// Build the undo if there was a selection
	if (!CompleteTransformation())
		FailAndExecute();

	// mark the selection cache as invalid (bounding rects etc will no longer be correct)
	m_pTransformRange->Update();

	// End the operation
	End();	
}




/********************************************************************************************

>	virtual void TransOperation::DragStarted(TransformData* TransData, SelectorTool* pTool,
								 TransformBoundingData* pBounds, DocCoord ClickPos, Spread* pSpread,
								 ClickModifiers ClickMods,
								 DocCoord ClickOffset, 
								 Node* NodeToTransform = NULL,
								 DragType dragtype = DRAGTYPE_AUTOSCROLL,
								 BOOL bSolidDrag)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		TransData - Extra data about the transform including info about the centre
				of transformation and several flags that may effect the behaviour of the
				operation
				pTool - A pointer to the selector tool so we can call it back (Can be NULL)
				pBounds - A pointer to the structure that contains all the bounds info about
				the current selection (Only required of pTool specified)
				ClickPos - The position of the click that is initiating this drag
				pSpread - The spread that the drag is starting on
				ClickMods - The click modifiers that were active at the start of the drag
				ClickOffset - The amount to offset the drag by as the Bounds Blobs are not
				on the exact corner of the selection, but further away.
				NodeToTransform - Specifies the object to transform (must be selected). 
				If NULL the applications selection is used.
				dragtype  ---  what to do if the user drags the mouse beyond the view
				window, by default scroll the view. 
	Purpose:	Starts an interactive drag version of the transformation operation.

********************************************************************************************/

/********************************************************************************************

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/1999
	Purpose:	Changes made: pTool is now of type DragTool*, instead of SelectorTool*.

********************************************************************************************/

void TransOperation::DragStarted(TransformData* TransData, DragTool* pTool,
								 TransformBoundingData* pBounds, DocCoord ClickPos, Spread* pSpread,
								 ClickModifiers ClickMods,
								 DocCoord ClickOffset, 
								 Node* NodeToTransform,
								 DragType dragtype,
								 BOOL bSolidDrag)
{
	// Ensure all the data is acceptable
	ENSURE( TransData!=NULL, "The Transform Data was NULL in DragStarted");

	// What is the node that we should be concentrating on in this transform?
	// (even if many nodes are being transformed)
	// If we haven't been told of a specific node and the drag pos is not offset
	// Then we can try to look for one directly under the cursor.
	// Phil says: This is horrible because
	//   (a) the caller already knows this info
	//   (b) the primary dragged node might not be the simple one under the pointer
	ObjectUnderCursor = NodeToTransform;
	if (ObjectUnderCursor == NULL && !ShouldPointerBeOffset())
	{
		ObjectUnderCursor = NodeRenderableInk::FindSimpleAtPoint(pSpread, ClickPos);
	}

	// Copy all the data that we will want to be keeping
	StartSpread = pSpread;
	CurrentSpread = pSpread;
	m_pDragSpread = pSpread;
	RawStartPos = ClickPos;
	StartPos = ClickPos;
	MagStartPos = ClickPos;
	LastRawPos = ClickPos;
	Offset = ClickOffset;
	MouseHasMoved = FALSE;
	pSelTool = pTool;
	MagneticGripPoint = FALSE;

	// Process raw mouse coordinate according to Offset distances...
	if (ShouldPointerBeOffset())
	{
		StartPos.translate(Offset.x,Offset.y);
		MagStartPos = StartPos;
	}
	else if (ObjectUnderCursor != NULL)
	{
		// If the click position is the position we intend to drag (ie. not offset)
		// and we have been informed about the node that position refers to
		// then we can try magnetic snapping to get a more accurate start pos
		MagneticGripPoint = DocView::SnapToMagneticNode(pSpread, ObjectUnderCursor, &MagStartPos);
	}

	// Get the information about the current size, position etc of the selection
	if (pBounds != NULL)
	{
		BoundingData.x = pBounds->x;
		BoundingData.y = pBounds->y;
		BoundingData.Width = pBounds->Width;
		BoundingData.Height = pBounds->Height;
		BoundingData.XScale = pBounds->XScale;
		BoundingData.YScale = pBounds->YScale;
		BoundingData.Rotation = pBounds->Rotation;
		BoundingData.Shear = pBounds->Shear;

		// Get the bounding rectangle into an array ready for quick transformation
		OriginalBounds[0].x = BoundingData.x;
		OriginalBounds[0].y = BoundingData.y;
		OriginalBounds[1].x = BoundingData.x+BoundingData.Width;
		OriginalBounds[1].y = BoundingData.y;
		OriginalBounds[2].x = BoundingData.x+BoundingData.Width;
		OriginalBounds[2].y = BoundingData.y+BoundingData.Height;
		OriginalBounds[3].x = BoundingData.x;
		OriginalBounds[3].y = BoundingData.y+BoundingData.Height;

		// Work out the centre of the selection
		BoundingCentre.x = BoundingData.x + (BoundingData.Width/2);
		BoundingCentre.y = BoundingData.y + (BoundingData.Height/2);

	}
	m_AccuracyTestRect = DocRect(OriginalBounds[0].x, OriginalBounds[0].y, OriginalBounds[2].x, OriginalBounds[2].y);

	// Get the info out of the structure
	OriginalCentre	= TransData->CentreOfTrans;
	LockAspect 		= TransData->LockAspect;
	LeaveCopy 		= TransData->LeaveCopy;
	ScaleLines 		= TransData->ScaleLines;
	TransFills 		= TransData->TransFills;

	BoundingData.XYChanged = FALSE;
	BoundingData.WHChanged = FALSE;
	BoundingData.ScaleChanged = FALSE;
	BoundingData.RotateChanged = FALSE;
	BoundingData.ShearChanged = FALSE;
	BoundingData.TransFillsChanged = FALSE;
	BoundingData.LeaveCopyChanged = FALSE;
	BoundingData.ScaleLinesChanged = FALSE;

	// This section allows certain values passed into this func to be toggled
	// if their keyboard short-cuts are being pressed (markn 25/7/95)

//	BOOL FlagsChanged = FALSE;

	//Graham 28/6/96: If the + key is pressed do the following:
	//IF its function is to set the LeaveCopy flag, do it
	//IF its function is to drop a copy, do nothing
	if (KeyPress::IsKeyPressed(CAMKEY(NUMPAD_ADD)))
	{
		if (ClickWhileDragFunc>1) LeaveCopy = !LeaveCopy;
	}

	if (KeyPress::IsKeyPressed(CAMKEY(NUMPAD_DIVIDE)))
	{
		ScaleLines = !ScaleLines;
	}

	if (KeyPress::IsKeyPressed(CAMKEY(NUMPAD_SUBTRACT)))
	{
		TransFills = !TransFills;
	}

	if (ClickMods.Menu)
	{
		AbortTransAtEnd = TRUE;
		StatusHelpID=_R(IDS_ABORT_TRANSFORMATION1);
	}
	
	BoundingData.TransFills = TransFills;
	BoundingData.ScaleLines = ScaleLines;
	BoundingData.LeaveCopy = LeaveCopy;

	// By default we will use the proper centre of transform. Adjust may change this later
	CentreOfTrans = OriginalCentre;
	
	// Where was this operation started from, and are we interested
	SetStartBlob(TransData->StartBlob);

	// Set up the rendering matrix parameters
	InitTransformOnDrag(GetStartPos(), ClickMods);

	// Use the SliceHelper function to help the user to select buttons & bars... - Matt 9/11/2000
	SliceHelper::ModifySelectionToContainWholeButtonElements();

	// Go find the selection and make sure that there really is something in it
	SetTransformRange(TransData->pRange, NodeToTransform);

	// Mike 01/02/95 - Change the range control to include NoneRenderables now
	RangeControl TransFlags = m_pTransformRange->GetRangeControlFlags();
	TransFlags.IgnoreNoneRenderable=TRUE;

	//Include invisible layers in the selection... Matt 9/11/2000
	TransFlags.IgnoreInvisibleLayers = TRUE;

	m_pTransformRange->SetRangeControl(TransFlags);

	Node* pFirstNode = m_pTransformRange->FindFirst();
	ENSURE(pFirstNode!=NULL, "There was no selection to transform!");
	if (pFirstNode==NULL)
		return;

	// Reset the EORed stuff
	m_pTransformRange->ResetXOROutlineRenderer();

	// Build the transformation matrix
	BuildMatrix();
	OriginalTransform = Transform;

	// Setup the solid dragging flag for this transform drag instance
	bSolidDrag = DocView::SolidDragging;

	// Hide the current selection and make a copy of it
	// Change the range flags before transforming to implement DMc's
	// horrible PromoteToParent bodge for his nasty objects
//	BOOL oldPromoteVal = TransFlags.PromoteToParent;
	TransFlags.PromoteToParent = TRUE;
	m_pTransformRange->SetRangeControl(TransFlags);

	m_pDraggedRange = m_pTransformRange->CloneNodes(DocView::SolidDragTimeLimit, AbortTransAtEnd);
	m_bFirstRedraw = TRUE;

	BOOL bEnableSolidDragging = TRUE;
	if (m_pDraggedRange==NULL)
	{
		bSolidDrag = FALSE;
		bEnableSolidDragging = FALSE;
	}
	else
	{
		m_pDraggedRange->SetDraggedNodes(TRUE);
		bEnableSolidDragging = TRUE;
	}

	// Start XOR rendering going. We find the (ink) object which was clicked on, and
	// pass that in the the renderer, so that it can ensure that object outline is
	// rendered first (this allows the user to drag a small object interactively rather
	// than having to wait for the entire image to XOR onto screen before they can see
	// the precise object they are dragging)
	if (!bSolidDrag)
	{
		m_pTransformRange->SetRenderable(TRUE);
		if (m_pDraggedRange) m_pDraggedRange->SetRenderable(FALSE);
		m_pTransformRange->RenderXOROutlinesOn(NULL, CurrentSpread, &Transform, ObjectUnderCursor);
	}
	else
	{
		// Hide the selection blobs, hide the selection, show the drag selection
		m_timeIdle.Sample();
		if (!AbortTransAtEnd) m_pTransformRange->SetRenderable(FALSE);
		if (m_pDraggedRange) m_pDraggedRange->SetRenderable(TRUE);
		if (pSelTool) Camelot.GetBlobManager()->RenderToolBlobsOff(pSelTool, CurrentSpread, NULL);
		Camelot.GetBlobManager()->BlobRenderingOff(TRUE);

		// Do we need to show the original outlines right away?
		if (DocView::IdleDragDelay==0 || DocView::OriginOutlineShowAlways)
		{
			m_pTransformRange->RenderXOROutlinesOn(NULL, CurrentSpread, &OriginalTransform, NULL);
			m_bShowOriginalOutlines = TRUE;
		}
	}

	// And tell the Dragging system that we need drags to happen
	StartDrag(dragtype, NULL, &StartPos, FALSE, bEnableSolidDragging);

	// Now that everything is set up, show status help message
	// (FigureStatusText will now have all the info it needs to provide correct feedback)
	if (StatusHelpID!=0)
	{
		String_256 Text;
		FigureStatusText(&Text);
		GetApplication()->UpdateStatusBarText(&Text);
	}
}


/********************************************************************************************

>	BOOL TransOperation::ShouldPointerBeOffset()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/94
	Returns:	TRUE if the transform wants the mouse pointer to be offsets to the edge of
				the selection, FALSE if it wants it left alone.
	Purpose:	Allows the transforms to decide if they want the mouse position to be
				moved for them to the edge of the selection.

********************************************************************************************/

BOOL TransOperation::ShouldPointerBeOffset()
{
	return TRUE;
}



/********************************************************************************************

>	DocCoord TransOperation::GetStartPos()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/04/95
	Returns:	DocCoord of start of drag.
	Purpose:	Get the raw start position or the magnetically snapped one depending on
				the state of the magnetic switch in the drag view.
				This function must be called instead of accessing StartPos directly in any
				place where the magnetically snapped start position is going to be used.
				It does not need to be used when magnetic snapping is not an issue. Then,
				StartPos can simply be read directly.

********************************************************************************************/

DocCoord TransOperation::GetStartPos()
{
	DocView* pDocView = DocView::GetSelected();
	if (pDocView && pDocView->GetSnapToObjectsState())
		return MagStartPos;
	else
		return StartPos;
}

/********************************************************************************************

>	void TransOperation::DragPointerMove(DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL bSolidDrag)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		PointerPos - The current position of the mouse
				ClickMods - Which of the modifiers are held down
				pSpread - The spread that the mouse is over
	Purpose:	Handles the event of the mouse moving during a drag. This function will try
				to undraw all the eored data from the screen and start drawing it in again
				in a new location.

********************************************************************************************/

void TransOperation::DragPointerMove(DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL bSolidDrag)
{
	// Has the mouse position actually moved? Check raw PointerPos against raw LastPos
	if (PointerPos != LastRawPos)
	{
		// First off, we will note the fact that the mouse has moved
		MouseHasMoved = TRUE;
		RawPos = PointerPos;

		// Right, the mouse pos changed, so go change the transform...
		if (bSolidDrag)
			DragSolidChanged(PointerPos, ClickMods, pSpread);
		else
			DragInputChanged(PointerPos, ClickMods, pSpread);

		// Preserve last raw mouse position
		LastRawPos = RawPos;
	}
}



/********************************************************************************************

>	void TransOperation::DragInputChanged(DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		PointerPos - The current position of the mouse
				ClickMods - Which of the modifiers are held down
				pSpread - The spread that the mouse is over
	Purpose:	Deals with any input changing during a transform drag. This mostly means
				the mouse position changing but it is also called when the click modifiers
				change or potentially any other input.

********************************************************************************************/

void TransOperation::DragInputChanged(DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread)
{
	// Process raw mouse coordinate according to Offset distances...
	if (ShouldPointerBeOffset())
		PointerPos.translate(Offset.x,Offset.y);

	// Rub all that old EORed stuff out, 'cos we sure don't want it now.
	m_pTransformRange->RenderXOROutlinesOff(NULL, CurrentSpread, &Transform);

	// Constrain the mouse position if required (45 degrees until we get a better system)
	if (ClickMods.Constrain)
		ConstrainDrag(&PointerPos);

	// if the adjust button is down, we will use the centre
	// of the selection as the centre of transform
	if (ClickMods.Adjust)
		CentreOfTrans = BoundingCentre;
	else
		CentreOfTrans = OriginalCentre;

	// rebuild the transform matrix
	UpdateTransformOnDrag(PointerPos, pSpread, ClickMods);
	BuildMatrix();

	// Draw the fabby new EORed stuff to the screen
	m_pTransformRange->RenderXOROutlinesOn(NULL, CurrentSpread, &Transform, ObjectUnderCursor);

	UpdateTransformBoundingData();
}



/********************************************************************************************

>	void TransOperation::DragSolidChanged(DocCoord PointerPos,
										  ClickModifiers ClickMods,
										  Spread* pSpread,
										  BOOL bForceRecopy = FALSE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/2003
	Inputs:		PointerPos - The current position of the mouse
				ClickMods - Which of the modifiers are held down
				pSpread - The spread that the mouse is over
	Purpose:	Deals with any input changing during a solid transform drag. This mostly means
				the mouse position changing but it is also called when the click modifiers
				change or potentially any other input.

********************************************************************************************/

void TransOperation::DragSolidChanged(DocCoord PointerPos,
										  ClickModifiers ClickMods,
										  Spread* pSpread,
										  BOOL bForceRecopy)
{
	DocCoord modPointerPos = PointerPos;
	DocView* pDocView = DocView::GetSelected();

	m_timeIdle.Sample();

	if (m_bShowDraggedOutlines)
	{
		m_pDraggedRange->RenderXOROutlinesOff(NULL, CurrentSpread, &OriginalTransform);
		m_bShowDraggedOutlines = FALSE;
	}

	if (!DocView::OriginOutlineShowAlways)
	{
		if (m_bShowOriginalOutlines && TransformOverlapsOriginal())
		{
			m_pTransformRange->RenderXOROutlinesOff(NULL, CurrentSpread, &OriginalTransform);
			m_bShowOriginalOutlines = FALSE;
		}
	}

	// Process raw mouse coordinate according to Offset distances...
	if (ShouldPointerBeOffset())
		modPointerPos.translate(Offset.x,Offset.y);

	// Constrain the mouse position if required (45 degrees until we get a better system)
	if (ClickMods.Constrain)
		ConstrainDrag(&modPointerPos);

	// if the adjust button is down, we will use the centre
	// of the selection as the centre of transform
	if (ClickMods.Adjust)
		CentreOfTrans = BoundingCentre;
	else
		CentreOfTrans = OriginalCentre;

	// rebuild the transform matrix
	UpdateTransformOnDrag(modPointerPos, pSpread, ClickMods);
	BuildMatrix();

	// Actually move the dragged objects here
	// Start a timer covering both the transform and the render...
	MonotonicTime timeRender;
	{
		SolidDragTransform(bForceRecopy);

		// Tell the dragging tool about the changes
		UpdateTransformBoundingData();

		// Render the tree and DON'T do any EOR rendering!
		// Call ServiceRendering to do a render directly within this function (without waiting
		// for the idle processor to do it)
		pDocView->FlushRedraw();
		GetApplication()->ServiceRendering();
	}
	// If render took more than 150 ms (SolidDragTimeLimit), fall back
	// NOTE! This is clever, the 150ms time will only be exceeded if a single object takes that
	// INT32 to render. Otherwise the background rendering timeslice will suspend rendering
	// and return. So background rendering will cope with complex selections but this clause
	// will fire if certain objects are taking too long.
	if (timeRender.Elapsed(DocView::SolidDragTimeLimit))
	{
		TRACEUSER( "Phil", _T("TOO COMPLEX TO DRAG SOLIDLY!\n"));
		pDocView->SetSolidDragState(FALSE);		// Turn off solid dragging in this drag op
		DragModeChanged(FALSE);
	}

	// Draw the EORed stuff to the screen if we need to...
	if (!DocView::OriginOutlineShowAlways)
	{
		if (!m_bShowOriginalOutlines && DocView::OriginOutlineShowNonOverlap && !TransformOverlapsOriginal())
		{
			m_pTransformRange->RenderXOROutlinesOn(NULL, CurrentSpread, &OriginalTransform, NULL);
			m_bShowOriginalOutlines = TRUE;
		}
	}
}



/********************************************************************************************

>	void TransOperation::DragPointerIdle(DocCoord, ClickModifiers, Spread*, BOOL bSolidDrag)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/94
	Inputs:		None Used
	Purpose:	This function is called during the drag when the mouse is not moving.
				It makes use of this idle time to draw a few more shapes from the selection
				to the screen. Thus, if you are tranforming a selection of objects you will
				see more detail when the mouse stops moving.

********************************************************************************************/

void TransOperation::DragPointerIdle(DocCoord Coord, ClickModifiers, Spread*, BOOL bSolidDrag)
{
	// Draw a few more outlines
	if (!bSolidDrag)
		m_pTransformRange->RenderXOROutlinesOn(NULL, CurrentSpread, &Transform, ObjectUnderCursor);
	else
	{
		if (m_timeIdle.Elapsed(DocView::IdleDragDelay))
		{
			// Assume OriginalTransform is Identity.
			m_bShowOriginalOutlines = TRUE;
			m_pTransformRange->RenderXOROutlinesOn(NULL, CurrentSpread, &OriginalTransform, ObjectUnderCursor);
		}

		if (m_timeIdle.Elapsed(DocView::IdleDragDelay2))
		{
			// Don't show the outlines at the dragged position if that position is the original
			// position
			if (!(Transform==OriginalTransform))
			{
				// We need to ensure that the dragged objects have actually been transformed
				// (Their cached containers might have been dragged instead to save time)
				// Would be nice to be able to test whether this recreation of the dragged
				// objects is really necessary here but I can't think of a decent way right now
				if (!m_bRangeFullyTransformed)
				{
					SolidDragTransform(TRUE, FALSE);
					ERROR3IF(!this->m_bRangeFullyTransformed, "Failed to transform shapes in DragPointerIdle");
				}

				m_bShowDraggedOutlines = TRUE;
				if (m_pDraggedRange)
				{
					m_pDraggedRange->RenderXOROutlinesOn(NULL, CurrentSpread, &OriginalTransform, NULL);
				}
			}
		}
	}
}




/********************************************************************************************

>	void TransOperation::DragFinished(	DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread,
									BOOL Worked, BOOL bSolidDrag)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickMods - the keyboard modifiers in place
				pSpread - the spread that was under the mouse
				Worked - TRUE if the drag was completed successfully
	Purpose:	Marks the end of the drag. It is at this point that all the transformations
				should be applied to the selection if everything worked.

********************************************************************************************/

void TransOperation::DragFinished(DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread,
								  BOOL Success, BOOL bSolidDrag)
{
	// By default assume it will not work
	BOOL Worked = FALSE;

	// Deregister our idle event handler. We don't want it to keep on rendering!
	GetApplication()->RemoveIdleProcessor(IDLEPRIORITY_HIGH, this);

	// Process raw mouse coordinate according to Offset distances...
	if (ShouldPointerBeOffset()) PointerPos.translate(Offset.x,Offset.y);

	// Find out how well the drag went . . .
	DragEndType HowEnded;
	if (!Success)
	{
		HowEnded = DT_CANCELLED;
	}
	else if (MouseHasMoved)
	{
		HowEnded = (PointerPos == RawStartPos) ? DT_MOVEDTOSAME : DT_MOVEDTODIFF;
	}
	else
	{
		HowEnded = DT_DIDNOTMOVE;
	}

	// Get rid of the temporary solid drag objects...
	DocView* pDocView = GetWorkingDocView();
	if (m_pDraggedRange)
	{
		// If we're not going to transform objects to this position
		// then force a redraw to remove them from the view
		if (AbortTransAtEnd || !Success)
			m_pDraggedRange->ForceRedrawView(pDocView, TRUE);
		else
		// Bodge to ensure that scaled cached data is fully wiped (because
		// the scaled cached data may be bigger than the actual scaled vector data)
		if (HowEnded==DT_MOVEDTODIFF && m_bRangeCacheTransformed && !Transform.IsTranslation())
			m_pDraggedRange->ForceRedrawView(pDocView, TRUE);

		m_pDraggedRange->DeleteNodes();
		delete m_pDraggedRange;
		m_pDraggedRange = NULL;
	}
	m_pTransformRange->SetRenderable(TRUE);

	// Rub all that old EORed stuff out, 'cos we sure don't want it now.
	if (!bSolidDrag)
		m_pTransformRange->RenderXOROutlinesOff(NULL, CurrentSpread, &Transform);
	else
	{
		if (m_bShowOriginalOutlines)
			m_pTransformRange->RenderXOROutlinesOff(NULL, CurrentSpread, &OriginalTransform);

		if (!Success)
			m_pTransformRange->ForceRedrawView(pDocView, TRUE, FALSE, TRUE);	// Don't recache objects - they haven't changed
	}
	pDocView->FlushRedraw();

	// Mike 01/02/95 - Change the range control to include NoneRenderables now
	RangeControl TransFlags = m_pTransformRange->GetRangeControlFlags();
	TransFlags.IgnoreNoneRenderable=FALSE;

	m_pTransformRange->SetRangeControl(TransFlags);

	// Check to see if the operation was a success and if the tool wants me to continue.
	BOOL fToolDragOK = TRUE;

#ifndef STANDALONE
	if (pSelTool != NULL) fToolDragOK = pSelTool->DragFinished(HowEnded);
#endif

	//Graham 30/9/96: This code is a bit of a bodge to fix something before the deadline
	//Do we need to change the selection?
	if (!lstNodesToSelect.IsEmpty()) 
	{
		//Yes. So first get rid of the current selection 
		NodeRenderableInk::DeselectAll(!bSolidDrag, TRUE);

		//And now we need to go through the list of nodes to select, selecting each one
		//in turn.
		NodeListItem* pThisItem=((NodeListItem*)lstNodesToSelect.GetHead());

		ERROR3IF(pThisItem==NULL, "TransOp::EndDrag - Node list has no head!");

		while (pThisItem!=NULL)
		{
			//Select the node
			Node* pThisNode=pThisItem->pNode;

			ERROR3IF(pThisNode==NULL, "TransOp::EndDrag - Node list item is empty!");

			pThisNode->SetSelected(TRUE);

			NodeRenderableBounded* pThisBoundedNode=((NodeRenderableBounded*) pThisNode);

			if (pThisBoundedNode)
				DoInvalidateNodeRegion(pThisBoundedNode, TRUE, FALSE);

			//And go on to the next node in the list.
			pThisItem=(NodeListItem*) lstNodesToSelect.GetNext(pThisItem);
		}

		//And get rid of the items in the list now we've used them
		lstNodesToSelect.DeleteAll();
	}


	//Graham: If AbortTransAtEnd is set, we don't want to complete the transformation
	if (fToolDragOK && Success && HowEnded!=DT_DIDNOTMOVE && !AbortTransAtEnd)
	{
		// Try to build all the undo information
	 	Worked = CompleteTransformation();

		if (CanChangeSpread())
			Document::SetSelectedViewAndSpread(NULL, NULL, m_pDragSpread);

	 	// mark the selection cache as invalid (bounding rects etc will no longer be correct)
		m_pTransformRange->Update();
	}

	// End the drag
	EndDrag();

	// Use the SliceHelper function to restore the selection back to how it was if it had been modified...
	SliceHelper::RestoreSelection();

	// If there was a problem, fail
	if (!Worked)
		FailAndExecute();

	// DON'T delete these ranges because they are used inside the End() function
	// Rely on ~Transop to get rid of them
/*	// tidy up member vars
	if (m_pTransformRange)
	{
		delete m_pTransformRange;
		m_pTransformRange = NULL;
	}

	if (m_pDraggedRange)
	{
		delete m_pDraggedRange;
		m_pDraggedRange = NULL;
	}
*/

	// Preserve some data items we need to use below, after "this" has been deleted...
	DragTool* pSelToolCopy = pSelTool;
	Spread* pSpreadCopy = CurrentSpread;
	// pDocview was gained locally in the code above
	
	// End the operation (and Delete ourselves!)
	End();

	// The following code may run when "this" has been deleted!
	// (It has to because we don't want to re-enable blob rendering until End() has done its stuff)
	// Be VERY careful here!

	// Now turn selection blobs back on
	Camelot.GetBlobManager()->BlobRenderingOn(FALSE);
	if (bSolidDrag && pSelToolCopy)
	{
		// Note that this function call renders blobs into all current RenderRegions
		// so that they are back in sync with the selection state
		Camelot.GetBlobManager()->RenderToolBlobsOn(pSelToolCopy, pSpreadCopy, NULL);
	}

}



/********************************************************************************************

>	BOOL TransOperation::DragKeyPress(KeyPress* pKeyPress, BOOL bSolidDrag)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/95
	Inputs:		pKeyPress - pointer to a keypress object
	Returns:	TRUE if it handled the keypress, FALSE otherwise
	Purpose:	Find out if any of the key modifiers are being pressed when the mouse is
				not moving.

********************************************************************************************/

BOOL TransOperation::DragKeyPress(KeyPress* pKeyPress, BOOL bSolidDrag)
{
	// Need something to see if there was a usful keypress
	BOOL KeyRecognised = FALSE;

	// and something to keep them in
	ClickModifiers ClickMods;
	ClickMods.Adjust = FALSE;
	ClickMods.Menu = FALSE;
	ClickMods.Constrain = FALSE;
	ClickMods.Alternative1 = FALSE;
	ClickMods.Alternative2 = FALSE;
	ClickMods.Pressure = 0;

	if (pKeyPress->IsPress() && !pKeyPress->IsRepeat())	// I.e. is this a non-auto-repeated key-down event?
	{
		switch (pKeyPress->GetVirtKey())
		{
			// Numeric + key
			case CAMKEY(NUMPAD_ADD):
			{
				// Markn 31/5/95: Only allow the LeaveCopy flag to toggled if select-inside is NOT present
				if (!m_pTransformRange->ContainsSelectInside())
				{
					// Graham 24/6/96: The '+' key's function depends on a preference
					// DragCopyAction() will perform whatever operation
					// is assigned to the + key by this preference.

					KeyRecognised = DragCopyAction(bSolidDrag);
				}
				break;
			}

			// Numeric / key
			case CAMKEY(NUMPAD_DIVIDE):
			{
					ScaleLines = !ScaleLines;
					BoundingData.ScaleLinesChanged = TRUE;
					KeyRecognised = TRUE;
			}

			// Numeric - key
			case CAMKEY(NUMPAD_SUBTRACT):
			{
					TransFills = !TransFills;
					BoundingData.TransFillsChanged = TRUE;
					KeyRecognised = TRUE;
			}
		}

		// If key recognised in the first part of this routine then it will have changed the status help...
		if (KeyRecognised)
		{
			BoundingData.ScaleLines = ScaleLines;
			BoundingData.LeaveCopy = LeaveCopy;
			BoundingData.TransFills = TransFills;

			String_256 Text;
			FigureStatusText(&Text);
			GetApplication()->UpdateStatusBarText(&Text);
		}
	}

	// If a modifier key has changed state, or the key pressed was one that we recognise,
	// update the drag data immediately
	if (KeyPress::ModifierChanged() || KeyRecognised)
	{
		// Set up the click modifier
		ClickMods.Constrain = pKeyPress->IsConstrainPressed();
		ClickMods.Adjust = pKeyPress->IsAdjustPressed();
		ClickMods.Alternative1 = pKeyPress->IsAlternativePressed();

		// Click modifiers changed (sort of) so go change the transform...
//		DragInputChanged(LastRawPos, ClickMods, CurrentSpread);
		// Right, the mouse pos changed, so go change the transform...
		if (bSolidDrag)
			DragSolidChanged(LastRawPos, ClickMods, CurrentSpread, KeyRecognised);
		else
			DragInputChanged(LastRawPos, ClickMods, CurrentSpread);

		// Tell the keypress people that we used the keypress
		KeyRecognised = TRUE;
	}

	// Did or did not use the click
	return KeyRecognised; 
}




/********************************************************************************************

>	void TransOperation::DragModeChanged()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/95
	Inputs:		pKeyPress - pointer to a keypress object
	Returns:	TRUE if it handled the keypress, FALSE otherwise
	Purpose:	Find out if any of the key modifiers are being pressed when the mouse is
				not moving.

********************************************************************************************/

void TransOperation::DragModeChanged(BOOL bSolidDrag)
{
	DocView* pDocView = DocView::GetSelected();
	BlobManager* pBlobManager = Camelot.GetBlobManager();
	ENSURE(pBlobManager, "Can't get BlobManager");

	if (bSolidDrag)
	{
		// The drag rendering mode has just become solid
		// Hide the original objects and show the dragged objects
		m_pTransformRange->SetRenderable(FALSE);
		m_pTransformRange->ForceRedrawView(pDocView, TRUE, FALSE, TRUE);	// Don't recache objects - they haven't changed
		if (m_pDraggedRange)
		{
			m_bFirstRedraw = TRUE;
			// Actually move the dragged objects here
			SolidDragTransform(FALSE);

			m_pDraggedRange->SetRenderable(TRUE);
//			m_pDraggedRange->ForceRedrawView(pDocView);

			m_pTransformRange->RenderXOROutlinesOff(NULL, CurrentSpread, &Transform);
			if (m_bShowOriginalOutlines)
				m_pTransformRange->RenderXOROutlinesOn(NULL, CurrentSpread, &OriginalTransform, ObjectUnderCursor);

			if (pSelTool) pBlobManager->RenderToolBlobsOff(pSelTool, CurrentSpread, NULL);
			pBlobManager->BlobRenderingOff(FALSE);
		}
	}
	else
	{
		// The drag rendering mode has just become outline
		// Show the original objects and hide the dragged objects
		m_pTransformRange->SetRenderable(TRUE);
		m_pTransformRange->ForceRedrawView(pDocView, TRUE, FALSE, TRUE);	// Don't recache objects - they haven't changed
		if (m_pDraggedRange)
		{
			pBlobManager->BlobRenderingOn(FALSE);
			if (pSelTool) pBlobManager->RenderToolBlobsOn(pSelTool, CurrentSpread, NULL);

			if (m_bShowOriginalOutlines)
				m_pTransformRange->RenderXOROutlinesOff(NULL, CurrentSpread, &OriginalTransform);
			m_pTransformRange->RenderXOROutlinesOn(NULL, CurrentSpread, &Transform, ObjectUnderCursor);

			m_pDraggedRange->SetRenderable(FALSE);
			m_pDraggedRange->ForceRedrawView(pDocView, TRUE);
		}
	}

	// Show status help message
	if (StatusHelpID!=0)
	{
		String_256 Text;
		FigureStatusText(&Text);
		GetApplication()->UpdateStatusBarText(&Text);
	}

	// Render any invalidated regions
	// Call ServiceRendering to do a render directly within this function (without waiting
	// for the idle processor to do it)
	pDocView->FlushRedraw();
	GetApplication()->ServiceRendering();
}




/********************************************************************************************

	void TransOperation::OnClickWhileDragging(OilCoord PointerPos, ClickType Click, ClickModifiers Mods, BOOL bSolidDrag)
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/6/96
	Inputs:		OilCoord PointerPos:	position of click
				ClickType Click:		single, double or triple
				ClickModifiers Mods:	whether shift, ctrl etc was pressed during the click
	Outputs:	-
	Returns:	-
	Purpose:	This virtual function handles clicks with one mouse button while
				another mouse button is dragging. For example, the user drags with the
				left mouse button and clicks the right button during the drag.

				This case of the virtual function overrides the empty virtual function
				Operation::OnClickWhileDragging.

				This function calls DragCopyAction to find out what the function
				assigned to Clicks-While-Dragging is.

				Feel free to add other overriding functions to handle clicks-while-
				dragging for other operations.

	Errors:		None. 
	SeeAlso:	ClickModifiers::ClickWhileDrag (member variable); DocView::OnClick;
				ScreenView::HandleDragEvent; ScreenView::HandleButtonUp;
				TransOperation::OnClickWhileDragging

********************************************************************************************/

void TransOperation::OnClickWhileDragging(OilCoord PointerPos, ClickType Click, ClickModifiers Mods, BOOL bSolidDrag)
{
	//If Click is an up-click, we are not interested in this particular case
	if (Click==CLICKTYPE_UP) return;

	// Karim 21/06/2000		similarly to the way we handle tapping '+' while dragging,
	//						we only acknowledge the click if select-inside is not present.
	if (!m_pTransformRange->ContainsSelectInside())
	{
		//Do whatever the ClickWhileDragging preference says we should do
		DragCopyAction(bSolidDrag);
	}

	//And update the status bar
	String_256 Text;
	FigureStatusText(&Text);
	GetApplication()->UpdateStatusBarText(&Text);
}


/********************************************************************************************

>	void TransOperation::RenderDragBlobs( DocRect ClipRect, Spread *pSpread, BOOL bSolidDrag )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/93
	Inputs:		ClipRect - NULL, or the region that needs to be redrawn
				pSpread - The spread to render to
	Purpose:	Draws the EORed stuff ("blobs") to the screen during a drag.
	SeeAlso:	Operation::RenderDragBlobs

********************************************************************************************/

void TransOperation::RenderDragBlobs(DocRect ClipRect, Spread *pSpread, BOOL bSolidDrag)
{
	// If solid dragging is turned on, only render any if we have turned them on!
	if (bSolidDrag)
	{
		if (m_bShowOriginalOutlines)
			m_pTransformRange->RenderXOROutlinesToCatchUp(&ClipRect, CurrentSpread, &OriginalTransform);
		if (m_bShowDraggedOutlines)
			m_pDraggedRange->RenderXOROutlinesToCatchUp(&ClipRect, CurrentSpread, &OriginalTransform);
		return;
	}

	// Build the transform matrix
	BuildMatrix();

	// draw the blobs for the specific region up to the same level as everywhere else
	if (pSpread==CurrentSpread)
		m_pTransformRange->RenderXOROutlinesToCatchUp(&ClipRect, CurrentSpread, &Transform);
}





/********************************************************************************************

>	BOOL TransOperation::Declare()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Returns:	TRUE if it worked, FALSE otherwise
	Purpose:	

********************************************************************************************/

BOOL TransOperation::Declare()
{
	return TRUE;
}

/********************************************************************************************

>	static BOOL TransOperation::DeclarePrefs()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/96
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Declares any preferences that the class needs to declare

********************************************************************************************/

BOOL TransOperation::DeclarePrefs()
{
	GetApplication()->DeclarePref( _T("Mouse"), _T("ClickWhileDragFunc"), &ClickWhileDragFunc, 0, 3 );

	return TRUE;
}




/********************************************************************************************

>	OpState TransOperation::GetState(String_256* Description, OpDescriptor*)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	

********************************************************************************************/

OpState TransOperation::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	return Blobby;
}




/********************************************************************************************

>	void TransOperation::InitTransformImmediate(OpParam*)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/94
	Inputs:		OpParam - The operations parameters
	Purpose:	Allows a transformation to set itself up ready for an immediate transform.
				If your class wants to support immediate transforms it should overide this
				function or the DoWithParam function. This base class version of the function
				does nothing.

********************************************************************************************/

void TransOperation::InitTransformImmediate(OpParam*)
{
}


/********************************************************************************************

>	virtual void TransOperation::InitTransformOnDrag(DocCoord PointerPos, ClickModifiers ClickMods)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		PointerPos - The position of the mouse at the start of the drag
				ClickMods - the keyboard modifiers that were active at the start of the drag
	Purpose:	Sets up the parameters needed to build the transform matrix at the start
				of the drag. This base class version of this function does nothing.

********************************************************************************************/

void TransOperation::InitTransformOnDrag(DocCoord PointerPos, ClickModifiers ClickMods)
{
	// Overide this function if your transformation operation needs to do anything
	// set up the matrix builder in a drag version.
}



/********************************************************************************************

>	virtual void TransOperation::UpdateTransformOnDrag(DocCoord PointerPos, Spread*,
									ClickModifiers&)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		PointerPos - The position of the mouse
				Spread - The spread that the mouse is over right now
				ClickModifiers - The current click modifiers
	Purpose:	This function re-calculates the parameters specific to the transform based
				on the mouse position. This is only used in interactive drag style transforms

********************************************************************************************/

void TransOperation::UpdateTransformOnDrag(DocCoord, Spread*, ClickModifiers&)
{
	// Overide this function if you need to do anything during a mouse move
}


/********************************************************************************************

>	virtual BOOL TransOperation::CanChangeSpread()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/June/2006
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this transform allows the drag to be transferred to another spread
	Purpose:	Tell the baseclass functions whether to draw drag feedback only on the start
				spread or to allow drag rendering to be done on other spreads too.

********************************************************************************************/

BOOL TransOperation::CanChangeSpread()
{
	// Overide this function if you allow your transform to cross spread boundaries
	return FALSE;
}


/********************************************************************************************

>	virtual void TransOperation::BuildMatrix()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Purpose:	This function should build the approprate transform matrix based on all the
				params that are available. This base class version of the function does not
				build a matrix and should not be called, as a derived class version of it
				should exist.

********************************************************************************************/

void TransOperation::BuildMatrix()
{
	ENSURE(FALSE, "Base class version of BuildMatrix() called. A derived class version must exist");
	
	// Sets the transform matrix to be the identity matrix in case of an emergency.
	Transform = Matrix();
}


/********************************************************************************************

>	void TransOperation::ConstrainDrag(DocCoord* PointerPos)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		PointerPos - The position of the mouse right about now
	Purpose:	Gives the operation the chance to constrain the mouses movement if it is
				relavent. If it is not, then do not overide this function.

********************************************************************************************/

void TransOperation::ConstrainDrag(DocCoord* PointerPos)
{
	// By default there is no constrain action
	// Write you own version of this function if you want your transformation to be constrained
}


/********************************************************************************************

>	void TransOperation::SetStartBlob(INT32 StartBlob)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/7/94
	Inputs:		StartBlob  - the Number of the blob that the operation was initiated on,
				if applicabale. They are numbered as follows :-
				MonoOn
				1  2  3
				4     5
				6  7  8
				MonoOff
				All operations that were not started from one of the 8 blobs should use 0
	Purpose:	Allows the operations to know how it was started. Some operation do
				different things depending on if they are going vertically or horizontally
				and this will allow you to figure that out. Overide this function if you
				need to deal with this situation (ie the Shear operation). The base class
				version does nothing.
				This function is called from the DragStarted() function.

********************************************************************************************/

void TransOperation::SetStartBlob(INT32 StartBlob)
{
}



/********************************************************************************************

>	void TransOperation::UpdateTransformBoundingData()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/94
	Purpose:	Should update the information in the TransformBoundingData object of the
				class and tell the Selector tool using the DragMove function. This is the
				base class version of the function that just tells the tool but does not
				change any of the data. This function is called at the end of the
				DragPointerMove function. It is possible for your transform to simple change
				the data in the BoundingData var as it goes and leave this base class version
				of the function to tell the tool about the changes

********************************************************************************************/

void TransOperation::UpdateTransformBoundingData()
{
#ifndef STANDALONE
	// Tell the tool about the current transform bounding data
	if (pSelTool != NULL)
	{
		pSelTool->DragMove(&BoundingData);
	}
#endif
}

/********************************************************************************************

>	void TransOperation::ComputeNewBounds()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/9/94
	Purpose:	Applies the current transformation matrix to the original bounding box data
				to compute the new bounds of the objects. It also notes that the bounds may 
				be severely distorted by the transform and so it sorts and recombines the
				output coordinates to construct a new bounding rectangle.

********************************************************************************************/

void TransOperation::ComputeNewBounds()
{
	// Transform original bounding coordinates
	Coord NewBounds[4];
	Transform.transform(NewBounds,OriginalBounds,4);

	// Now recombine the output coords to see where the new bottom-left and top-right coords
	// really are...
	INT32 lox = NewBounds[0].x;
	INT32 loy = NewBounds[0].y;
	INT32 hix = NewBounds[0].x;
	INT32 hiy = NewBounds[0].y;
	UINT32 i;
	for(i=1;i!=4;i++)
	{
		if (NewBounds[i].x < lox) lox=NewBounds[i].x;
		if (NewBounds[i].y < loy) loy=NewBounds[i].y;
		if (NewBounds[i].x > hix) hix=NewBounds[i].x;
		if (NewBounds[i].y > hiy) hiy=NewBounds[i].y;
	}

	// Now update the bounds structure...
	if (lox!=BoundingData.x || loy!=BoundingData.y)
	{
		// One or both of x or y have changed so update them and set the changed flag
		BoundingData.x=lox;
		BoundingData.y=loy;
		BoundingData.XYChanged = TRUE;
	}

	// Compute width and height
	hix = hix-lox;
	hiy = hiy-loy;
	if (hix!=BoundingData.Width || hiy!=BoundingData.Height)
	{
		// One or both of w or h have changed so update them and set the changed flag
		BoundingData.Width=hix;
		BoundingData.Height=hiy;
		BoundingData.WHChanged = TRUE;
	}

}

/********************************************************************************************

>	BOOL TransOperation::CompleteTransformation()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Returns:	TRUE if it worked
	Purpose:	Builds the actual Undo information etc for the transformation as well as
				actually performing the transformation itself. This should be called by
				both DragFinished() and DoWithParams().

********************************************************************************************/

BOOL TransOperation::CompleteTransformation()
{
	// Build the appropriate transform object to transform the selection
	Trans2DMatrix* Trans; 
	ALLOC_WITH_FAIL(Trans, new Trans2DMatrix(Transform), this);  

	// See if the memory allocation worked
	if (Trans == NULL)
		return FALSE;

	// Ideally we should put the hourglass on here, it will be turned off in the End method
PORTNOTE("other", "Use of BeginSlowJob removed in TransOperation::CompleteTransformation()");
//	BeginSlowJob();

	// Set the flags in the transform according to the flags set here
	Trans->TransFills = TransFills;
	Trans->TransLines = ScaleLines && CanScaleLines;
	Trans->bHaveTransformedAllCached = TRUE;			// having transformed none we can say all caches have been transformed
	Trans->bHaveTransformedAllChildren = TRUE;			// having transformed none we can say all children have been transformed
	Trans->bTransformYourChildren = TRUE;				// The transform does not have to transform all its data
	m_bRangeCacheTransformed = TRUE;
	m_bRangeFullyTransformed = TRUE;

	// if we need to leave a copy of the selection behind, then do it
	// We HAVE to do this before we call DoStartTransOp() as it builds a
	// run length coding of the selection, which is messed up if we insert
	// extra nodes into the tree. 
	//
	// Markn 31/5/95: Only allow LeaveCopy if select-inside is NOT present
	// JustinF: 6/10/96 only do this if transforming the selection, not TransformRange!
	if (m_pTransformRange && LeaveCopy && !m_pTransformRange->ContainsSelectInside())
	{
		if (!LeaveCopyOfSelection())
			return FALSE;
	}

  	// Start the selection operation which records the current selection status and 
	// invalidates the rectangle covered by the selection and its blobs. 
	BOOL RecordSelectionTwice = (CanChangeSpread()) && (StartSpread!=CurrentSpread);

	// If the Selection only contains a single object then we pass this object as
	// a parameter to DoStartTransOp. Otherwise the routine assumes that the 
	// entire selection will be transformed.
	// JustinF: 7/10/96 only do this if transforming the selection, not TransformRange!
	if (SelState==NULL)
	{
		if (!DoStartTransOp(RecordSelectionTwice, NULL, m_pTransformRange))
			return FALSE; 
	}

	// Scan the selection, and transform each selected object.
	// Karim 29/06/2000 - modified so that the range's PromoteToParent flag is not ruined.
	BOOL TransOK = DoTransformNodes(*m_pTransformRange, Trans);
	if (!TransOK)
		return FALSE;

	// Move the selection to a new spread if it should be
	if (RecordSelectionTwice)
	{
		// Move the selection to a new spread here
		if (!DoMoveNodes(*m_pTransformRange, CurrentSpread->FindActiveLayer(), LASTCHILD))
			return FALSE;

		Document::GetSelected()->ResetInsertionPosition();
	}
	
	// Change the bounds then ripple them up the tree
	//SCANRANGEFORCLASS(Selection, UpdateInkBoundingRect(), NodeRenderableInk);
 	//Selection->UpdateParentBoundsOfSelection(); 

	m_bRangeCacheTransformed = Trans->bHaveTransformedAllCached;
	m_bRangeFullyTransformed = Trans->bHaveTransformedAllChildren;

	// It all worked, so say so.
	return TRUE;
}



/********************************************************************************************

>	BOOL TransOperation::SolidDragTransform(BOOL bForceRecopy = FALSE, BOOL bForceRedraw = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/2003
	Returns:	TRUE if it worked
	Purpose:	Transform the selection into the drag range.

********************************************************************************************/

BOOL TransOperation::SolidDragTransform(BOOL bForceRecopy, BOOL bForceRedraw)
{
	BOOL TransOK = FALSE;

	if (m_pDraggedRange==NULL)
		return FALSE;

	// Build the appropriate transform object to transform the selection
	Trans2DMatrix* Trans; 
	Trans = new Trans2DMatrix(Transform);

	// See if the memory allocation worked
	if (Trans == NULL)
		return FALSE;

//TRACEUSER( "Phil", _T("SolidDragTransform hscale = %f, %f\n"), Trans->GetScale().MakeDouble(), Trans->GetAspect().MakeDouble());

	// Set the flags in the transform according to the flags set here
	Trans->TransFills = TransFills;
	Trans->TransLines = ScaleLines && CanScaleLines;

	// Get rid of the last set of dragged objects and make a new copy of the selection
	// Note! This would be much faster if just the coords of the drag selection could be transformed!
	// Redraw just the current view
	m_pDraggedRange->ForceRedrawView(GetWorkingDocView(), TRUE, m_bFirstRedraw);
	m_bFirstRedraw = FALSE;
	BOOL bQualNotSolid = !(GetWorkingDocView()->RenderQuality.GetFillQuality() > Quality::Solid);

#if SOLID_TRANSFORM_METHOD == 1
	// Update the dragged objects the dumb way - by deleting and recreating them
	m_pDraggedRange->DeleteNodes();
	delete m_pDraggedRange;

	m_pDraggedRange = m_pTransformRange->CloneNodes(DocView::SolidDragTimeLimit);

	if (m_pDraggedRange==NULL)
		return FALSE;

	m_pDraggedRange->SetDraggedNodes(TRUE);
	m_pDraggedRange->SetRenderable(TRUE);

	// Scan the selection, and transform each selected object.
	Trans->bSolidDrag = TRUE;
	Trans->bHaveTransformedAllCached = TRUE;			// having transformed none we can say all caches have been transformed
	Trans->bHaveTransformedAllChildren = TRUE;			// having transformed none we can say all caches have been transformed
	Trans->bTransformYourChildren = bQualNotSolid;		// The transform does not have to transform all its data
	TransOK = m_pDraggedRange->TransformNodes(Trans);
	m_bRangeCacheTransformed = Trans->bHaveTransformedAllCached;
	m_bRangeFullyTransformed = Trans->bHaveTransformedAllChildren;

#endif
#if SOLID_TRANSFORM_METHOD == 2
	// Update the dragged objects the smart way by copying the contents of the selection
	// into the dragged nodes
	m_pTransformRange->CopyNodesContents(m_pDraggedRange, TRUE, TRUE);

	// Scan the selection, and transform each selected object.
	Trans->bSolidDrag = TRUE;
	Trans->bHaveTransformedAllCached = TRUE;			// having transformed none we can say all caches have been transformed
	Trans->bHaveTransformedAllChildren = TRUE;			// having transformed none we can say all caches have been transformed
	Trans->bTransformYourChildren = bQualNotSolid;		// The transform does not have to transform all its data
	TransOK = m_pDraggedRange->TransformNodes(Trans);
	m_bRangeCacheTransformed = Trans->bHaveTransformedAllCached;
	m_bRangeFullyTransformed = Trans->bHaveTransformedAllChildren;

#endif
#if SOLID_TRANSFORM_METHOD == 3
	// Move objects back to original position simply by premultiplying the transformation
	// matrix by the inverse of the previous transform
	m_PrevTransform.Invert();
	m_PrevTransform *= *Trans;

	// Scan the selection, and transform each selected object.
	m_PrevTransform.bSolidDrag = TRUE;
	m_PrevTransform.bHaveTransformedAllCached = TRUE;			// having transformed none we can say all caches have been transformed
	m_PrevTransform.bHaveTransformedAllChildren = TRUE;			// having transformed none we can say all caches have been transformed
	m_PrevTransform.bTransformYourChildren = bQualNotSolid;		// The transform does not have to transform all its data
	TransOK = m_pDraggedRange->TransformNodes(&m_PrevTransform);
	m_bRangeCacheTransformed = Trans->bHaveTransformedAllCached;
	m_bRangeFullyTransformed = Trans->bHaveTransformedAllChildren;
	m_PrevTransform = *Trans;										// Store previous transform
#endif
#if SOLID_TRANSFORM_METHOD == 4
	// If the dragged objects have become innacurate compared to the transformed
	// originals then we must ReCopy, else we can continue to transform the dragged
	// objects
	//
	// Get width/height of dragged objects
	// Calc width height of transformed original bounds
	// If the two differ by more than half a pixel
	// Recopy
	//
	DocRect tr = DocRect(OriginalBounds[0].x, OriginalBounds[0].y, OriginalBounds[2].x, OriginalBounds[2].y);
	m_PrevTransform.Transform((DocCoord*)&tr, 2);
	//TRACEUSER( "Phil", _T("M4 Test %d %d\n"), abs(tr.lox-m_AccuracyTestRect.lox), abs(tr.loy-m_AccuracyTestRect.loy));

	// We may need to move the dragged range to a new spread if the transform requires it...
	if (CanChangeSpread() && (m_pDragSpread!=CurrentSpread))
	{
		// Move m_DraggedRange to the new spread
		//TRACEUSER("Phil", _T("Move m_Dragged range to new spread!\n"));
		bForceRecopy = TRUE;
	}

	if( bForceRecopy || 
		abs( tr.lo.x - m_AccuracyTestRect.lo.x ) > 10 ||
		abs( tr.lo.y - m_AccuracyTestRect.lo.y ) > 10 || 
		abs( tr.hi.x - m_AccuracyTestRect.hi.x ) > 10 || 
		abs( tr.hi.y - m_AccuracyTestRect.hi.y ) > 10 )
	{
		//TRACEUSER( "Phil", _T("M4 Recopy %f\n"), Trans->GetScale().MakeDouble());
		// Update the dragged objects the dumb way - by deleting and recreating them
		m_pDraggedRange->DeleteNodes();
		delete m_pDraggedRange;

		Layer* pTargetLayer = NULL;
		if (CurrentSpread!=m_pDragSpread)
			pTargetLayer = CurrentSpread->FindActiveLayer();	// This logic must match the final copy done in DragFinished

		m_pDraggedRange = m_pTransformRange->CloneNodes(DocView::SolidDragTimeLimit, FALSE, FALSE, pTargetLayer);

		if (m_pDraggedRange==NULL)
			return FALSE;

		m_pDraggedRange->SetDraggedNodes(TRUE);
		m_pDraggedRange->SetRenderable(TRUE);
		m_pDragSpread = CurrentSpread;

		// Scan the selection, and transform each selected object.
		Trans->bSolidDrag = TRUE;
		Trans->bHaveTransformedAllCached = TRUE;
		Trans->bHaveTransformedAllChildren = TRUE;
		Trans->bTransformYourChildren = bForceRecopy | bQualNotSolid;		// if forced recopy then also force transformation of everything
		TransOK = m_pDraggedRange->TransformNodes(Trans);
		m_bRangeCacheTransformed = Trans->bHaveTransformedAllCached;
		m_bRangeFullyTransformed = Trans->bHaveTransformedAllChildren;

		// Reset accuracy test rect because we have just reset the dragged objects
		m_AccuracyTestRect = DocRect(OriginalBounds[0].x, OriginalBounds[0].y, OriginalBounds[2].x, OriginalBounds[2].y);
		Trans->Transform((DocCoord*)&m_AccuracyTestRect, 2);
	}
	else
	{
		//TRACEUSER( "Phil", _T("M4 Transform\n"));
		// Move objects back to original position simply by premultiplying the transformation
		// matrix by the inverse of the previous transform
		m_PrevTransform.Invert();
		m_PrevTransform *= *Trans;

		// Scan the selection, and transform each selected object.
		m_PrevTransform.bSolidDrag = TRUE;
		m_PrevTransform.bHaveTransformedAllCached = TRUE;			// having transformed none we can say all caches have been transformed
		m_PrevTransform.bHaveTransformedAllChildren = TRUE;			// having transformed none we can say all caches have been transformed
		m_PrevTransform.bTransformYourChildren = bQualNotSolid;		// The transform does not have to transform all its data
		TransOK = m_pDraggedRange->TransformNodes(&m_PrevTransform);
		m_bRangeCacheTransformed = m_PrevTransform.bHaveTransformedAllCached;
		m_bRangeFullyTransformed = m_PrevTransform.bHaveTransformedAllChildren;

		// Transform accuracy test rect in the same way we have just done for the dragged objects
		// to track what's hapenning to them...
		m_PrevTransform.Transform((DocCoord*)&m_AccuracyTestRect, 2);
	}

	m_PrevTransform = *Trans;

#endif

	if (m_pDraggedRange && bForceRedraw)
		m_pDraggedRange->ForceRedrawView(GetWorkingDocView(), TRUE);

	if (!TransOK)
		return FALSE;

	delete Trans;

	return TRUE;
}




/********************************************************************************************

>	BOOL TransOperation::LeaveCopyOfSelection()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/7/94
	Returns:	TRUE if it worked, FALSE if it did not
	Purpose:	If the flag LeaveCopy is set (ie we have been instructed to leave a copy of
				the selection in its original location) then this function is called. It
				makes a copy of the current selection and places them in the tree.
	SeeAlso:	TransOperation::CompleteTransformation()

********************************************************************************************/

BOOL TransOperation::LeaveCopyOfSelection()
{
	// we have to go through the current selection and make copies of all the nodes
	Node* pNode = m_pTransformRange->FindFirst();
	while (pNode!=NULL)
	{
		// Make sure that the node is bounded
		ENSURE(pNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded)),
				"Selected node was not bounded");

		// Go and find the next selected node before we do any insertions
		Node* pNext = m_pTransformRange->FindNext(pNode);

		// Make a copy of this node and return FALSE if it failed
		Node* pCopy;
		if (!pNode->NodeCopy(&pCopy))
			return FALSE;

		// Ensure that the copy is not selected
		pCopy->SetSelected(FALSE);

		// Insert the new node into the tree just after the original,
		// without effecting the selection, or forcing a redraw.
		if (!DoInsertNewNode((NodeRenderableBounded*)pCopy, pNode, PREV,
							 FALSE, FALSE, FALSE))
			return FALSE;

		// Find the next node to copy
		pNode = pNext;
	}

	// All worked
	return TRUE;	
}



/*****************************************************************************
>	BOOL TransOperation::GetStatusLineText(String_256* pText, Spread*, DocCoord, ClickModifiers)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/02/95
	Inputs:		Spread*, DocCoord, ClickModifiers - not used
	Outputs:	pText - 
	Returns:	TRUE if returning valid text
	Purpose:	get status line hlep for drag op
	Errors:		this==NULL
*****************************************************************************/

BOOL TransOperation::GetStatusLineText(String_256* pText, Spread*, DocCoord, ClickModifiers)
{
	ERROR2IF(this==NULL, FALSE,"TransOperation::GetStatusLineText() - this==NULL");
	ERROR2IF(pText==NULL,FALSE,"TransOperation::GetStatusLineText() - pText==NULL");
	
	FigureStatusText(pText);
	return TRUE;
}



/*****************************************************************************
>	void TransOperation::FigureStatusText()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/02/95
	Inputs:		-
	Outputs:	pText - 
	Returns:	-
	Purpose:	Compute help text for this transform
	Errors:		this==NULL
*****************************************************************************/

void TransOperation::FigureStatusText(String_256* pText)
{
	pText->Empty();

	if (StatusHelpID!=0)
		pText->Load(StatusHelpID);

	// Get info about current drag mode and show the user
	DocView* pDocView = GetWorkingDocView();
	if (pDocView && pDocView->GetCurrentSolidDragAbility())
	{
		if (pDocView->GetCurrentSolidDragState())
			*pText += String_256(_R(IDS_TAB_SOLID_OFF));
		else
			*pText += String_256(_R(IDS_TAB_SOLID_ON));
	}

	// Is the click-while-dragging preference is set for dropping copies?
	if (ClickWhileDragFunc>1) 
	{
		//Yes. Show the "dropping copies" status bar text
		*pText += String_256(_R(IDS_DROP_COPY));
	}
	else
	{
		//Otherwise, the preference is set so that clicking while dragging
		//sets the LeaveCopy flag. Show the "leaving copies" status bar text.
		if (LeaveCopy)
			*pText += String_256(_R(IDS_LEAVE_COPY_ON_B));
		else
			*pText += String_256(_R(IDS_LEAVE_COPY_OFF_B));
	}
	
	if (TransFills)
		*pText += String_256(_R(IDS_TRANSFILLS_ON));
	else
		*pText += String_256(_R(IDS_TRANSFILLS_OFF));

	
	if (StatusHelpID2!=0)
		*pText += String_256(StatusHelpID2);


	if (CanScaleLines)
	{
		if (ScaleLines)
			*pText += String_256(_R(IDS_SCALELINES_ON));
		else
			*pText += String_256(_R(IDS_SCALELINES_OFF));
	}

}

/********************************************************************************************

>>>	BOOL DragCopyAction(BOOL bSolidDrag)
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/96
	Inputs:		None
	Outputs:	-
	Returns:	FALSE if the function it called did not work.
	Purpose:	OnKeyPress and OnClickWhileDragging call this function when Num + is pressed
				during a drag or when the user clicks a mouse button while dragging with 
				another button.

				These two events have the same effect but that effect is set by a user 
				preference. This function looks at that preference and calls the 
				appropriate function.				

	Errors:		None. 
	SeeAlso:	TransOperation::OnClickWhileDragging; TransOperation::DropCopy; 
				TransOperation::SetLeaveCopy;

********************************************************************************************/
BOOL TransOperation::DragCopyAction(BOOL bSolidDrag)
{
	//Call the function appropriate to the preference assigned to the key pressed.
	
	if (ClickWhileDragFunc > 1)
	{
		return DropCopy(bSolidDrag);
	}
	else
	{
		return SetLeaveCopy();
	}
}

/********************************************************************************************

>>>	BOOL DropCopy(BOOL bSolidDrag)
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/96
	Inputs:		None.
	Outputs:	-
	Returns:	FALSE if it did not work due to lack of memory
	Purpose:	This function drops a copy of the dragged object during the drag,
				appropriately translated or transformed.

				Because the user has dropped objects during the drag, we assume he does not
				want to transform the original objects at the end of the drag. That is, the user
				has used this drag to copy objects, not to move them. So we set the
				AbortTransAtEnd flag.

	Errors:		One memory error if no space for an error
	SeeAlso:	TransOperation::DragCopyAction; OpCopyAndTransform::Do

********************************************************************************************/
BOOL TransOperation::DropCopy(BOOL bSolidDrag)
{
	//Because we are dropping a copy, we take that as a signal the user doesn't
	//want the drag to transform the selection. Setting this flag will abort the 
	//transformation when the drag ends.
	AbortTransAtEnd=TRUE;

	//Prepare the Trans2DMatrix to send to the CopyAndTransform operation
	Trans2DMatrix MatrixToSend(Transform);
	 
	MatrixToSend.TransFills = TransFills;
	MatrixToSend.TransLines = ScaleLines && CanScaleLines;

	//Create a new CopyAndTransform operation with that matrix
	OpCopyAndTransform* pOpCopyAndTransform = new OpCopyAndTransform(MatrixToSend);

	ERROR1IF(pOpCopyAndTransform==NULL, FALSE, _R(IDE_NOMORE_MEMORY));

	// DMc - store the original selection
	
	// DMc - build a list of the original selection, so we can reselect them later
	Range Sel(*(GetApplication()->FindSelection()));
	
	List * pSelList = Sel.MakeListOfNodes(FALSE);

	//Do the operation
	pOpCopyAndTransform->Do(NULL);

	// Make sure the dragged objects are on top by removing them and then putting them back on top
	// (Could just move them in the tree but there's no routine to do that at the moment...)
	if (m_pDraggedRange)
	{
		Range* pNewDraggedRange = m_pDraggedRange->CloneNodes(DocView::SolidDragTimeLimit, TRUE);
		if (pNewDraggedRange)
		{
			if (bSolidDrag)
			{
				pNewDraggedRange->SetRenderable(TRUE);
			}
			else
			{
				pNewDraggedRange->SetRenderable(FALSE);
			}
			pNewDraggedRange->SetDraggedNodes(TRUE);
			m_pDraggedRange->DeleteNodes();
			delete m_pDraggedRange;
			m_pDraggedRange = pNewDraggedRange;
		}
	}

	// DMc - restore the selection afterwards
	// restore the selection back to what it was before we entered the operation
	if (pSelList)
	{
		NodeRenderableInk::DeselectAll(FALSE, FALSE);
		
		// get back the original selection
		NodeListItem * pItem = (NodeListItem *)pSelList->GetHead();
		
		while (pItem)
		{
			pItem->pNode->SetSelected(TRUE);
			
			pItem = (NodeListItem *)pSelList->GetNext(pItem);
		}
		
		pSelList->DeleteAll();
		delete pSelList;
		GetApplication()->UpdateSelection();	
	}

	//Change the initial status bar message to read "Selection will not move after drag"
	StatusHelpID=_R(IDS_ABORT_TRANSFORMATION1);

	// Show the original objects if we are solid dragging, since they will be left behind
	// now that we have dropped a copy
	DocView* pDocView = GetWorkingDocView();
	m_pTransformRange->SetRenderable(TRUE);
	m_pTransformRange->ForceRedrawView(pDocView, TRUE, FALSE, TRUE);	// Don't recache objects - they haven't changed
	if (m_bShowOriginalOutlines)
	{
		m_pTransformRange->RenderXOROutlinesOff(NULL, CurrentSpread, &OriginalTransform);
		m_bShowOriginalOutlines = FALSE;
	}
	pDocView->FlushRedraw();
	GetApplication()->ServiceRendering();

	//StatusHelpID2 contains all the information on SHIFT-dragging, CTRL-dragging etc.
	//This does not change when a copy is dropped, so the variable stays the same.
	//And return to say it worked
	return TRUE;
}

/********************************************************************************************

>>>	BOOL SetLeaveCopy ()
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/96
	Inputs:		None
	Outputs:	-
	Returns:	TRUE.
	Purpose:	Sets the LeaveCopy flag and the corresponding flags in BoundingData.	
	Errors:		None. 
	SeeAlso:	TransOperation::DragCopyAction

********************************************************************************************/
BOOL TransOperation::SetLeaveCopy ()
{
	//If SelectInside is off, set the LeaveCopy flag and corresponding BoundingData flags.
		if (!m_pTransformRange->ContainsSelectInside())
		{
			LeaveCopy = !LeaveCopy;
			BoundingData.LeaveCopy = LeaveCopy;
			BoundingData.LeaveCopyChanged = TRUE;
		}

	return TRUE;
}

/********************************************************************************************

  >>>	void TransOperation::SelectNodeAfterDrag (Node* NodeToSelect)
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/96
	Inputs:		The node we want to select after the drag
	Purpose		Adds the node to list of nodes to select after the drag

********************************************************************************************/
void TransOperation::SelectNodeAfterDrag (Node* NodeToSelect)
{
	NodeListItem* pnliNode=new NodeListItem(NodeToSelect);

	lstNodesToSelect.AddTail((ListItem*) pnliNode);
}

/********************************************************************************************

  >>>	void ClearNodesToSelect ()
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/96
	Purpose		Empties the node list and deletes all the items in it.

********************************************************************************************/
void TransOperation::ClearNodesToSelect ()
{
	lstNodesToSelect.DeleteAll();
}




/********************************************************************************************

>	BOOL TransOperation::SetTransformRange(Range* pRange, Node* pNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/12/2003
	Returns:	-
	Purpose:	Sets the m_pTransformRange member to be a local range created either
				as a copy of the supplied range or a range created to represent the node
				or a copy of the selection range.

********************************************************************************************/
BOOL TransOperation::SetTransformRange(Range* pTransformRange, Node* pTransformNode)
{
	// If we have a good range and we're being given the same range then do nothing
	if (m_pTransformRange!=NULL && pTransformRange==m_pTransformRange)
		return TRUE;

	// If we have a good range and the caller is giving us nothing then do nothing
	if (m_pTransformRange && pTransformRange==NULL && pTransformNode==NULL)
		return TRUE;

	if (pTransformNode)
	{
		// Only transforming single node.  First check it's correct type
		ERROR3IF(!pTransformNode->IS_KIND_OF(NodeRenderableBounded), "Should be bounded");

		m_pTransformRange = new Range(pTransformNode,
									  pTransformNode,
									  RangeControl(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE));
		ERROR2IF(m_pTransformRange==NULL, FALSE, "Can't create simple range");
	}
	else
	{
		if (pTransformRange)
		{
			m_pTransformRange = new Range(*pTransformRange);
			ERROR2IF(m_pTransformRange == NULL, FALSE, "No supplied range");
		}
		else
		{
			m_pTransformRange = new Range(*(GetApplication()->FindSelection()));
			ERROR2IF(m_pTransformRange == NULL, FALSE, "No selected range");
			m_pTransformRange->SetPromoteToParent(TRUE);
		}
	}

	// If this goes off there's a logical error in the above code that works
	// out the range to transform.
	ERROR3IF(!m_pTransformRange, "No range to transform in TransOperation::DoStartTransOp");
	return (m_pTransformRange!=NULL);
}




/********************************************************************************************

>	BOOL TransOperation::TransformOverlapsOriginal()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/03/2004
	Returns:	TRUE if the original bounds overlap the transformed bounds
	Purpose:	Check whether transform objects overlap originals

********************************************************************************************/
BOOL TransOperation::TransformOverlapsOriginal()
{
	DocRect TransformBounds(BoundingData.x, BoundingData.y,
							BoundingData.x+BoundingData.Width, BoundingData.y+BoundingData.Height
							);
	DocRect OrigBounds(OriginalBounds[0].x, OriginalBounds[0].y, OriginalBounds[2].x, OriginalBounds[2].y);

	return (TransformBounds.IsIntersectedWith(OrigBounds));
}




