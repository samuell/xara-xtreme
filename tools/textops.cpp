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
// Implementation of the text tool

// This file contains the implementation of the following classes 

// class OpTextUndoable: 	All Undoable text operations are derived from this base class. It
//						 	currently provides a set of useful DO functions for text ops.
// class OpCreateTextObject:This is the op which gets executed to create a text object
// class OpTextFormat:		This op groups together those text operations which will cause
//							the TextStory to be formatted.
// class OpFitTextToCurve:	Self explanatory

// class OpTextCaret:		Derived from Operation. This Op groups together all caret operations.


/*
*/

#include "camtypes.h"
#include "textops.h"	

#include "dlgmgr.h"
// Resource files
//#include "markg.h"	

// Code headers
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "blobs.h"
#include "clipint.h"
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "invalid.h"
#include "layer.h"
#include "nodetext.h"
#include "nodetxtl.h"
#include "nodetxts.h"
#include "objchge.h"
//#include "ops.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "paths.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "progress.h"
#include "textacts.h"
#include "textfuns.h"
#include "textinfo.h"
#include "texttool.h"
//#include "trans2d.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "peter.h"
//#include "simon.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "ndtxtpth.h"
#include "cutop.h"
//#include "will.h"
#include "fontman.h"
//#include "barsdlgs.h"			// for the fit txt to curve bar control
#include "bubbleid.h"
//#include "nodecomp.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "slicehelper.h"
//#include "becomea.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "opliveeffects.h"
#include "ophist.h"
#include "nodedoc.h"
#include "nodepath.h"

extern void Beep();

DECLARE_SOURCE( "$Revision$" );


// CC_IMPLEMENTS for all classes
#if !defined(EXCLUDE_FROM_RALPH)
// Base class
CC_IMPLEMENT_DYNCREATE(OpTextUndoable, SelOperation)

// Undoable ops
CC_IMPLEMENT_DYNCREATE(OpDeleteTextStory, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpCreateTextObject, OpTextUndoable)
CC_IMPLEMENT_DYNCREATE(OpTextFormat, OpTextUndoable)
CC_IMPLEMENT_DYNCREATE(OpFitTextToCurve, OpTextUndoable)
CC_IMPLEMENT_DYNCREATE(OpTextKern, OpTextUndoable)
CC_IMPLEMENT_DYNCREATE(OpTextAutoKern, OpTextUndoable)
CC_IMPLEMENT_DYNCREATE(OpReverseStoryPath, OpTextUndoable)

// Class city for dragging indent blobs on stories
CC_IMPLEMENT_DYNAMIC(OpDragStoryIndent, OpTextUndoable)
CC_IMPLEMENT_DYNAMIC(OpDragStoryPathIndent, OpDragStoryIndent)
CC_IMPLEMENT_DYNAMIC(OpDragStoryNonPathIndent, OpDragStoryIndent)
CC_IMPLEMENT_DYNCREATE(OpDragStoryPathLeftIndent, OpDragStoryPathIndent)
CC_IMPLEMENT_DYNCREATE(OpDragStoryPathRightIndent, OpDragStoryPathIndent)
CC_IMPLEMENT_DYNCREATE(OpDragStoryNonPathLeftIndent, OpDragStoryNonPathIndent)
CC_IMPLEMENT_DYNCREATE(OpDragStoryNonPathRightIndent, OpDragStoryNonPathIndent)

CC_IMPLEMENT_DYNCREATE(OpTextPaste, OpPaste)
CC_IMPLEMENT_DYNCREATE(OpTogglePrintTextAsShapes, Operation)
CC_IMPLEMENT_DYNCREATE(OpTextCaret, Operation)
CC_IMPLEMENT_DYNCREATE(OpTextSelection, Operation)
#endif
CC_IMPLEMENT_DYNCREATE(OpApplyGlobalAffect, Operation)
#if !defined(EXCLUDE_FROM_RALPH)
CC_IMPLEMENT_DYNCREATE(OpApplyJustificationToStory, Operation)
CC_IMPLEMENT_DYNCREATE(OpApplyLeftJustifyToStory, OpApplyJustificationToStory)
CC_IMPLEMENT_DYNCREATE(OpApplyCentreJustifyToStory, OpApplyJustificationToStory)
CC_IMPLEMENT_DYNCREATE(OpApplyRightJustifyToStory, OpApplyJustificationToStory)
CC_IMPLEMENT_DYNCREATE(OpApplyFullJustifyToStory, OpApplyJustificationToStory)
#endif
CC_IMPLEMENT_DYNCREATE(OpAffectFontChange, OpApplyGlobalAffect)

CC_IMPLEMENT_DYNCREATE(PrePostTextAction, Action)
CC_IMPLEMENT_DYNCREATE(ToggleAutoKerningAction, Action)
#if !defined(EXCLUDE_FROM_RALPH)
CC_IMPLEMENT_DYNCREATE(PositionCaretAction, Action)
CC_IMPLEMENT_DYNCREATE(StorePathIndentAction, Action)
#endif

// Must come after the last CC_IMPLEMENT.. macro
#define new CAM_DEBUG_NEW     


// The following define attempts to put in a fix to allow pasted in unformatted text
// to take on the attributes of the current Caret position...
// (Doesn't work reliably...)
//#define PHIL_ATTRS_FIX

// Define this to ensure that the caret is always selected, even when other characters
// are selected. This allows the caret to take part in attribute applciation and
// optimisation correctly
#define SELECT_CARET_AND_CHARS

#if !defined(EXCLUDE_FROM_RALPH)
// statics
VerticalInsetStore OpTextCaret::VertInset;

	
/////////////////////////////////////////////////////////////////////////////////////////////
// OpTextUndoable methods

/********************************************************************************************
>	OpTextUndoable::OpTextUndoable()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/3/95
	Purpose:	OpTextUndoable constructor
********************************************************************************************/

OpTextUndoable::OpTextUndoable()
{
}


/********************************************************************************************
>	virtual BOOL OpTextUndoable::DoStartTextOp(TextStory* pTxtStory, BOOL StartBlobs=TRUE, BOOL EndBlobs=TRUE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/3/95
	Inputs:		pTextStory - The TextStory to operate on
				StartBlobs - wether to render object blobs at the start of the op
				EndBlobs   - wether to render object blobs at the end of the op
	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!)
	Purpose:	This function should get called at the start of each undoable text operation.
				All common actions are done here
********************************************************************************************/

BOOL OpTextUndoable::DoStartTextOp(TextStory* pTxtStory, BOOL StartBlobs, BOOL EndBlobs)
{
//	BeginSlowJob();		// text ops only work on one object at a time so are never slow!?

	// Record the selection state
	return DoStartSelOp(StartBlobs, EndBlobs);
}


/////////////////////////////////////////////////////////////////////////////////////////////
// OpCreateTextObject methods

/********************************************************************************************
>	OpCreateTextObject::OpCreateTextObject()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/02/95						  
	Purpose:	OpCreateTextObject constructor
********************************************************************************************/

OpCreateTextObject::OpCreateTextObject()
{
}


/********************************************************************************************
>	void OpCreateTextObject::DoImmediate( Spread* pSpread, DocCoord Anchor, NodePath* pPath,
					ClickModifiers ClickMods, DocCoord StoryWidth = DocCoord(MinDocCoord,MinDocCoord))
    
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> (From Peter code) 
	Created:	09/03/95
	Inputs:		pSpread - The spread that the click happened on
				The Anchor and pPath inputs are mutually exclusive. if one is specified
				the other should not be. 
				When the user clicks on a spread, the Anchor will be the click pos
				When the user clicks on a path the pPath will be a pointer to this path.
				ClickMods - the current click modifiers
				StoryWidth - Another point specifing width and angle of story.
							Specify MinDocCoord,MinDocCoord for a non-wrapping story
	Purpose:	This function starts and ends the operation of inserting a text object onto
				a spread. It is the DO function.
********************************************************************************************/
void OpCreateTextObject::DoImmediate( Spread* pSpread, DocCoord Anchor, NodePath* pPath,
										ClickModifiers ClickMods, DocCoord StoryWidth)
{
	// Snap the click point
	DocView::SnapCurrent(pSpread, &Anchor);
	
	// create the text object
	TextStory* pTextStory = NULL; 
	ALLOC_WITH_FAIL(pTextStory, (TextStory::CreateTextObject(Anchor)), this); 
	BOOL ok = (pTextStory != NULL);

	// Create a word-wrapping story on a path
	if (ok && pPath==NULL && StoryWidth!=DocCoord(MinDocCoord,MinDocCoord))
	{
		// Set the wrapping width
		pTextStory->SetWordWrapping(TRUE);
		pTextStory->SetStoryWidth((MILLIPOINT)Anchor.Distance(StoryWidth));

		// Set the angle on the story too.
		double StoryRot = atan2((double)StoryWidth.y-Anchor.y, (double)StoryWidth.x-Anchor.x)*(180/PI);

		// Construct a rotation matrix and apply to the story
		Trans2DMatrix StoryRotMat(Matrix(Anchor, StoryRot));
		pTextStory->Transform(StoryRotMat);
	}

	// Start the operation
	if (ok)
		ok = DoStartSelOp(FALSE, TRUE, TRUE, FALSE);

	// Hide any existing empty text story
	if (ok)
		ok = OpDeleteTextStory::RemoveEmptyFocusStory(this);

	// Apply the current attributes to the TextStory
	if (ok)
		ok = pOurDoc->GetAttributeMgr().ApplyCurrentAttribsToNode((NodeRenderableInk*)pTextStory);

	// Add it to the document and invalidate the region
	if (ok)
	{
		if (pPath == NULL)
			ok = DoInsertNewNode(pTextStory, pSpread, FALSE, TRUE, FALSE);
		else
			ok = DoInsertNewNode(pTextStory, pPath, NEXT, FALSE, TRUE, FALSE);
	}

	// Move the path into it if required
	if (ok && pPath!=NULL)
	{
		// Work out how far along the line the click was, CTRL is position at left
		MILLIPOINT KernDist = 0;
		if (!ClickMods.Constrain)
		{
			double dist = 0.0;
			ok = pPath->InkPath.DistanceTo(Anchor, &dist);
			if (ok)
				KernDist = (MILLIPOINT)dist;
		}

		// If SHIFT click or path is closed then we don't want the story to wrap
		BOOL PathIsClosed = pPath->InkPath.IsSubPathClosed(pPath->InkPath.GetNumCoords()-1);
		if (ClickMods.Adjust || PathIsClosed)
		{
			pTextStory->SetWordWrapping(FALSE);
			pTextStory->SetLeftIndent(0);

			// Old style indent via a kern at the start
			// Get the size of an em at the caret
			CaretNode* pCaret = pTextStory->GetCaret();
			if (ok && (pCaret != NULL) && (KernDist > 0))
			{
				FormatRegion FRegion;
				ok = FRegion.Init(pCaret);

				CharMetrics CharMet;
				if (ok)
					ok = FRegion.GetCharMetrics(&CharMet, FONTEMCHAR);

				// Insert a kern at the start of the text
				if (ok)
				{
					KernDist = (INT32)((KernDist / (double)CharMet.FontEmWidth)*1000);
					KernCode* pKern = new KernCode();
					if (pKern == NULL)
						ok = FALSE;
					else
					{
						DocCoord kd(KernDist,0);
						pKern->SetValue(kd);
						ok = DoInsertNewNode(pKern,pTextStory->FindFirstChild(CC_RUNTIME_CLASS(TextLine)),
																				FIRSTCHILD,FALSE,FALSE,FALSE);
						// Also insert a zero kern so the caret appears in the correct
						if (ok)
						{
							KernCode* pKern = new KernCode();
							if (pKern == NULL)
								ok = FALSE;
							else
							{
								DocCoord kd(0,0);
								pKern->SetValue(kd);
								ok = DoInsertNewNode(pKern,pTextStory->FindFirstChild(CC_RUNTIME_CLASS(TextLine)),
																					FIRSTCHILD,FALSE,FALSE,FALSE);
							}
						}
					}
				}	 
			}		
		}
		else
		{
			pTextStory->SetWordWrapping(TRUE);
			pTextStory->SetLeftIndent(KernDist);
		}


		//Localise attributes of the story
		if (ok)	ok = DoLocaliseCommonAttributes(pTextStory); 

		// Move the path into the story
		if (ok) ok = DoMoveNode(pPath, pTextStory, FIRSTCHILD);
		if (ok)	pPath->DeSelect(FALSE);

		// Factor back out the common attributes
		if (ok) ok = DoFactorOutCommonChildAttributes(pTextStory);
	}

	// Mark the document as modified (as we don't have an opDescriptor)
	if (ok)
		pOurDoc->SetModified(TRUE);	

	// Now format the new story to position the caret
	if (ok)
		ok = PrePostTextAction::DoFormatStory(this, pTextStory);

	// Select the caret and give it the focus
	if (ok)
	{
		// Only select caret if we added to ok layer
		Layer* pLayer = (Layer*)pTextStory->FindParent(CC_RUNTIME_CLASS(Layer));
		if ((pLayer != NULL) && pLayer->IsVisible() && !pLayer->IsLocked())
		{
			pTextStory->GetCaret()->Select(TRUE);
			TextStory::SetFocusStory(pTextStory);
			DialogManager::DefaultKeyboardFocus();		// Gives input focus to mainframe
		}
	}

	// Clear any failure
	if (!ok)
	{
		InformError();
		FailAndExecute();
	}

	End(); 
}



/********************************************************************************************
>	void OpCreateTextObject::DoDrag( Spread* pSpread, DocCoord Anchor, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Inputs:		pSpread - The spread that the drag was started on
				Anchor - starting position of the drag
				ClickMods - the starting click modifiers.
	Purpose:	Call to start a drag operation to create a wrapping story with a width.
********************************************************************************************/
void OpCreateTextObject::DoDrag( Spread* pSpread, DocCoord Anchor, ClickModifiers ClickMods)
{
	// Snap the starting point
	DocView::SnapCurrent(pSpread, &Anchor);

	// Init member vars
	m_AnchorPoint = Anchor;
	m_CurrentOtherEnd = Anchor;
	m_pStartSpread = pSpread;

	// Eor on the drag blobs for the first time
	DocRect EditPathBBox = m_pStartSpread->GetBoundingRect();
	RenderDragBlobs(EditPathBBox, m_pStartSpread, FALSE);

	// And tell the Dragging system that we need drags to happen
	StartDrag( DRAGTYPE_AUTOSCROLL, NULL, &m_AnchorPoint, FALSE );
}



/********************************************************************************************
>	void OpCreateTextObject::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL bSolidDrag)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Inputs:		PointerPos - the current mouse position
				ClickMods - the current click modifiers.
				pSpread - the spread the pointer is over.
	Purpose:	Called after the user moves the mouse during a drag operation.  Updates the
				on screen drag indication
********************************************************************************************/
void OpCreateTextObject::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL bSolidDrag)
{
	// Rub out the old drag blob
	DocRect EditPathBBox = m_pStartSpread->GetBoundingRect();
	RenderDragBlobs(EditPathBBox, m_pStartSpread, bSolidDrag);

	// Should we ever have multiple spreads this code will prove useful
	if (pSpread != m_pStartSpread)
		PointerPos = MakeRelativeToSpread(m_pStartSpread, pSpread, PointerPos);

	// Constrain options
	// none - horizontal story
	// SHIFT - unconstrained
	// CTRL - angle constrain
	if (!ClickMods.Adjust && !ClickMods.Constrain)
		PointerPos.y = m_AnchorPoint.y;
	if (ClickMods.Constrain)
		DocView::ConstrainToAngle(m_AnchorPoint, &PointerPos);
	DocView::SnapCurrent(pSpread, &PointerPos);

	m_CurrentOtherEnd = PointerPos;

	// Put the drag blobs back on in the new position
	RenderDragBlobs(EditPathBBox, m_pStartSpread, bSolidDrag);

	// Set the status line to be helpful
	String_256 HelpString(_R(IDS_TEXTTOOL_DRAGCREATEPARA));
	GetApplication()->UpdateStatusBarText(&HelpString);
}



/********************************************************************************************
>	void OpCreateTextObject::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL Success, BOOL bSolidDrag)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Inputs:		PointerPos - the current mouse position
				ClickMods - the current click modifiers.
				pSpread - the spread the pointer is over.
				Success - FALSE if the user has aborted the drag.
	Purpose:	Called at the end of a drag operation.  Creates a text story if the user did
				not cancel the drag.
********************************************************************************************/
void OpCreateTextObject::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL Success, BOOL bSolidDrag)
{
	EndDrag();

	// Take off the drag blobs
	DocRect EditPathBBox = m_pStartSpread->GetBoundingRect();
	RenderDragBlobs(EditPathBBox, m_pStartSpread, bSolidDrag);

	if (Success)
	{
		// If the drag ended (too) close to the start then we don't want to create a wrapping story
		DocRect PointerBlobRect;
		GetApplication()->GetBlobManager()->GetBlobRect(m_AnchorPoint, &PointerBlobRect);

		// Call the CreateImmediate function to do all the work.
		if (PointerBlobRect.ContainsCoord(m_CurrentOtherEnd))
			DoImmediate(m_pStartSpread, m_AnchorPoint, NULL, ClickMods);
		else
			DoImmediate(m_pStartSpread, m_AnchorPoint, NULL, ClickMods, m_CurrentOtherEnd);
	}
}



/********************************************************************************************
>	virtual void OpCreateTextObject::RenderDragBlobs( DocRect Rect, Spread* pSpread, BOOL bSolidDrag )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Inputs:		Rect - The region that needs the blobs to be drawn
				pSpread - The spread that the drawing will happen on
	Purpose:	Draws the drag blobs, in this case a line from the start to the current pos.
********************************************************************************************/
void OpCreateTextObject::RenderDragBlobs(DocRect EditPathBBox, Spread* pSpread, BOOL bSolidDrag)
{
	// If being called from DocView::RenderView, then the spread could be wrong - so
	// convert the rectangle if necessary.
	if (pSpread != m_pStartSpread)
	{
		EditPathBBox.lo = MakeRelativeToSpread(m_pStartSpread, pSpread, EditPathBBox.lo);
		EditPathBBox.hi = MakeRelativeToSpread(m_pStartSpread, pSpread, EditPathBBox.hi);
	}

	// start a rendering loop
	RenderRegion* pRegion = DocView::RenderOnTop(NULL, pSpread, ClippedEOR);
	while (pRegion)
	{
		// Draw the line 
		pRegion->SetFillColour(COLOUR_NONE);
		pRegion->SetLineColour(COLOUR_XORNEW);
		pRegion->DrawLine(m_AnchorPoint, m_CurrentOtherEnd);

		// Draw a box at the anchor
		pRegion->SetLineColour(COLOUR_BEZIERBLOB);
		pRegion->SetFillColour(COLOUR_BEZIERBLOB);
		pRegion->DrawBlob(m_AnchorPoint, BT_UNSELECTED);

		// Get the Next render region
		pRegion = DocView::GetNextOnTop(NULL);
	}
}



/********************************************************************************************
>	void OpCreateTextObject::GetOpName(String_256* OpName)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/95
	Outputs:	OpName: The name of the operation (undo / redo string)
	Purpose:	Returns the name of the operation
********************************************************************************************/

void OpCreateTextObject::GetOpName(String_256* OpName)
{
	*OpName = String_256(_R(IDS_CREATETEXTOBJECT));
}


/////////////////////////////////////////////////////////////////////////////////////////////
// OpTextFormat methods

/********************************************************************************************
>	OpTextFormat::OpTextFormat()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/02/95
	Purpose:	OpTextFormat constructor
********************************************************************************************/

OpTextFormat::OpTextFormat()
{
}


/********************************************************************************************
>	BOOL OpTextFormat::DoDeleteSelection(TextStory* pTextStory, BOOL ReAttachAttributes,
											BOOL* AllowOpRefused = NULL)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/95
	Inputs:		pTextStory         - The TextStory to operate on
				ReAttachAttributes - TRUE => reattach caret's attributes after deletion
	Outputs:	AllowOpRefused     - TRUE => AllowOp said we could not delete
	Returns:	FALSE if failed
	Purpose:	Delete any sub-selection within pTextStory (exc caret)
********************************************************************************************/

BOOL OpTextFormat::DoDeleteSelection(TextStory* pTextStory, BOOL ReAttachAttributes, BOOL* AllowOpRefused)
{
	if (AllowOpRefused != NULL)
		*AllowOpRefused = FALSE;

	BOOL ok = TRUE;
	TextStory* pStory = TextStory::GetFocusStory();
	if (pStory != NULL)
	{
		BOOL DontDelete = FALSE;
		BOOL FoundDeletable = FALSE;
		
		// Call AllowOp on all the selected characters.
		VisibleTextNode* pChar = pStory->FindFirstVTN();
		ok = (pChar != NULL);
		ObjChangeParam ObjEdit(OBJCHANGE_STARTING,ObjChangeFlags(TRUE),NULL,this);
		while (ok && !DontDelete && (pChar != NULL))
		{
			if (pChar->IsSelected() && !pChar->IsACaret())
			{
				FoundDeletable = TRUE;
				DontDelete = !(pChar->AllowOp(&ObjEdit));
			}
			pChar = pChar->FindNextVTNInStory();
		}

		if (DontDelete && (AllowOpRefused != NULL))
			*AllowOpRefused = TRUE;

		if (FoundDeletable && !DontDelete)
		{
			// Get the story to delete the selection - it does all attribute localising and factoring out
			if (ok && !DontDelete)
				ok = pStory->DeleteSelectedText(this);

			CaretNode* pCaret = pStory->GetCaret();
			if (ok && !DontDelete && (pCaret!=NULL))
				pCaret->SetSelected(TRUE);

			// Attach new attributes to the caret
			if (ok && ReAttachAttributes)
				ok = pStory->AttachCaretAttributes();

			// Mark the document as modified (as we don't have an opDescriptor)
			if (GetWorkingDoc() != NULL)
				GetWorkingDoc()->SetModified(TRUE);
		}
	}

	return ok;
}


/********************************************************************************************
>	void OpTextFormat::DoInsertChar(WCHAR ch, InsertStatus InsStatus)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/96
	Inputs:		ch        - The character to insert at the caret position
				InsStatus - not yet implemented
	Purpose:	see DoInsertCharHelper()
********************************************************************************************/

void OpTextFormat::DoInsertChar(WCHAR ch, OpTextFormat::InsertStatus InsStatus)
{
	if (!DoInsertCharHelper(ch))
		FailAndExecute();
	End();
}


/********************************************************************************************
>	BOOL OpTextFormat::DoInsertChar(WCHAR ch)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/96
	Inputs:		ch - The character to insert at the caret position
	Purpose:	Inserts a new character to the left of the caret
				deleting any selection in the story that exists
********************************************************************************************/

BOOL OpTextFormat::DoInsertCharHelper(WCHAR ch)
{
	// get ptrs to story and caret
	TextStory* pStory = TextStory::GetFocusStory();
	ERROR2IF(pStory==NULL,FALSE,"OpTextFormat::DoInsertCharHelper() - no focus story");
	CaretNode* pCaret = pStory->GetCaret();
	ERROR2IF(pCaret==NULL,FALSE,"OpTextFormat::DoInsertCharHelper() - Story has no caret");

	String_256 MasterText = pStory->GetStoryAsString();

	// start the text op (must be done before AllopOp() which may insert actions)
	BOOL ok = DoStartTextOp(pStory);

	// ask caret if it and it's parents allow char to be inserted (allows blend to remap)
	ObjChangeParam ObjInsert(OBJCHANGE_STARTING,ObjChangeFlags(),NULL,this);
	if (ok) ok = pCaret->AllowOp(&ObjInsert);

	// delete any sub-selection if it exists
	if (ok) ok = DoDeleteSelection(pStory,TRUE);

	// Create a new text character node, and insert into tree, and apply caret attrs to it
	TextChar* pNewChar = NULL;
	if (ok) { ALLOC_WITH_FAIL(pNewChar, (new TextChar()) , this); }
	if (ok) ok = (pNewChar!=NULL);
	if (ok) pNewChar->SetUnicodeValue(ch); 
	if (ok) ok = pNewChar->DoInsertNewNode(this,pCaret,PREV);
	if (ok) ok = pCaret->DoApplyAttrsTo(this,pNewChar);

	// Mark the document as modified (as we don't have an opDescriptor)
	if (ok && GetWorkingDoc()!=NULL)
		GetWorkingDoc()->SetModified(TRUE);	

	// update other textstories that are dependant on this one
	SliceHelper::OnTextStoryChanged(pStory, this, &ObjInsert, MasterText);
	
	// Update all the changed nodes
	ObjChangeParam ObjChange(OBJCHANGE_FINISHED,ObjChangeFlags(),NULL,this);
	if (ok) ok = UpdateChangedNodes(&ObjChange);

	if (ok) pCaret->ScrollToShow();

	return ok;
}


/********************************************************************************************
>	void OpTextFormat::DoDeleteChar(OpTextFormat::DeleteType DelTyp)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com> (originally Peter)
	Created:	12/2/96
	Inputs:		DelTyp - DELETE    => delete char to right of caret
						 BACKSPACE => delete char to left  of caret
	Purpose:	If there is no sub-selection the char to the right/left of the caret is deleted
				else sub-selection is deleted and caret placed to left of 1st selected char
********************************************************************************************/

void OpTextFormat::DoDeleteChar(OpTextFormat::DeleteType DelTyp)
{
	TextStory* pStory = TextStory::GetFocusStory();
	if (pStory==NULL)
		End();	// no focus story so normal node deletion used which does not restore a caret

	String_256 MasterText = pStory->GetStoryAsString();

	BOOL ok = DoStartTextOp(pStory);

	BOOL DeletingAtCaret    = FALSE;	// assume not deleting at caret
	BOOL AttachAttrsToCaret = TRUE;		// assume attach carets from char to left after deletion

	// If just the caret is selected then select the character to the left/right
	CaretNode* pCaret = pStory->GetCaret();
	ERROR3IF(pCaret==NULL,"OpTextFormat::DoDeleteChar() - Focus story had no caret");
	VisibleTextNode* pForcedSel = NULL;
	// Caret is always selected, so test for subselection using selection end pointer
	if (ok && pCaret!=NULL && pStory->GetSelectionEnd()==NULL)
	{
		DeletingAtCaret = TRUE;
		if (DelTyp==OpTextFormat::DEL)
		{
			pForcedSel = pCaret->FindNextVTNInStory();
			ERROR3IF(pForcedSel==NULL,"No VisibleTextNode following the caret");
			if (pForcedSel!=NULL && pForcedSel==pStory->FindLastVTN())
				pForcedSel = NULL;		// ensure last EOL in story is not deleted
		}
		else
		{
			pForcedSel = pCaret->FindPrevVTNInStory();
			if (pForcedSel!=NULL && !pForcedSel->IsAnEOLNode())
			{
				pStory->AttachCaretAttributes(); // ensure caret has attrs of node to be deleted
				AttachAttrsToCaret = FALSE;
			}
					
		}

		// if we have a char to delete, select the char to delete
		if (pForcedSel!=NULL)
		{
			pForcedSel->SetSelected(TRUE);
			pCaret->SetSelected(FALSE);
		}
	}

	// do the deletion
	if (!DeletingAtCaret || pForcedSel!=NULL)
	{
		// Delete the selected character(s)
		BOOL AllowOpRefused = TRUE;
		ok = DoDeleteSelection(pStory, AttachAttrsToCaret, &AllowOpRefused);

		// if we were deleting at the caret, restore the selection
		if (ok && AllowOpRefused && pForcedSel!=NULL)
		{
			pForcedSel->SetSelected(FALSE);
			pCaret->SetSelected(TRUE);
		}

		// Mark the document as modified (as we don't have an opDescriptor)
		if (GetWorkingDoc()!=NULL)
			GetWorkingDoc()->SetModified(TRUE);	
		
		ObjChangeFlags cFlags; // use these so we have some objchange params to pass to the ontextstory changed fn
		ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

		if (ok)
		{
			// update other textstories that are dependant on this one
			SliceHelper::OnTextStoryChanged(pStory, this, &ObjChange, MasterText);
		}

		ObjChange.Define(OBJCHANGE_FINISHED,ObjChangeFlags(TRUE),NULL,this);


		if (ok) ok = UpdateChangedNodes(&ObjChange);

		if (ok) pCaret->ScrollToShow();
	}

	if (!ok)
	{
		InformError();
		FailAndExecute();
	}

	End();  
}	


/********************************************************************************************
>	BOOL OpTextFormat::DoReturn(OpTextFormat::InsertStatus InsStatus)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/96
	Inputs:		InsStatus - insert or overwrite mode (not yet used)
	Returns:	FALSE if fails
	Purpose:	Break the text line at the caret with a hard EOL
				For simplicity, a new line is created the word wrapper tidies up
********************************************************************************************/

BOOL OpTextFormat::DoReturn(OpTextFormat::InsertStatus InsStatus)
{
	ERROR2IF(InsStatus!=OpTextFormat::INSERT,FALSE,"OpTextFormat::DoReturn() - only supports insert mode");
	UndoableOperation* pUndoOp = this;

	// Get pointers to focus story, caret, caret line and caret line EOL
	TextStory* pStory = TextStory::GetFocusStory();
	ERROR2IF(pStory==NULL, FALSE, "OpTextFormat::DoReturn() - No focus story");
	CaretNode* pCaret = pStory->GetCaret();
	ERROR2IF(pCaret==NULL, FALSE, "OpTextFormat::DoReturn() - Focus story had no caret");
	TextLine* pCaretLine = pCaret->FindParentLine();
	ERROR2IF(pCaretLine==NULL,FALSE,"OpTextFormat::DoReturn() - caret has no parent");

	String_256 MasterText = pStory->GetStoryAsString();

	// start the text op (must be done before AllopOp() which may insert actions)
	BOOL ok = DoStartTextOp(pStory);

	// see if the op is allowed
	ObjChangeParam ObjParam(OBJCHANGE_STARTING, ObjChangeFlags(), NULL, pUndoOp);
	if (pCaret->AllowOp(&ObjParam)==FALSE)
		return TRUE;

	// delete any sub-selection if it exists, then get ptr to last VTN on caret line
	if (ok) ok = DoDeleteSelection(pStory, TRUE);

	// After deleting the selection, the caret line may have been deleted
	// so we need to get it again in it's new location
	pCaret = pStory->GetCaret();
	ERROR2IF(pCaret==NULL, FALSE, "OpTextFormat::DoReturn() - Focus story had no caret");
	pCaretLine = pCaret->FindParentLine();
	ERROR2IF(pCaretLine==NULL,FALSE,"OpTextFormat::DoReturn() - caret has no parent");

	VisibleTextNode* pLastCaretLineVTN = pCaretLine->FindLastVTN();
	ERROR2IF(pLastCaretLineVTN==NULL,FALSE,"OpTextFormat::DoReturn() - caret line has no VTN");

	// insert a new EOL on src line before caret with caret's attributes
	EOLNode* pNewEOL = NULL;
	if (ok) pNewEOL = new EOLNode;
	if (ok) ok = (pNewEOL!=NULL);
	if (ok) ok = pNewEOL->DoInsertNewNode(pUndoOp,pCaret,PREV);
	if (ok) ok = pCaret->DoApplyAttrsTo(pUndoOp,pNewEOL);

	// insert a new line after caret line
	TextLine* pNewLine = NULL;
	if (ok) pNewLine = new TextLine;
	if (ok) ok = (pNewLine!=NULL);
	if (ok) ok = pNewLine->DoInsertNewNode(pUndoOp,pCaretLine,NEXT);

	// and move all nodes from the caret upto end of line to the new line
	if (ok) ok = pCaretLine->DoLocaliseCommonAttributes(pUndoOp,FALSE,FALSE,NULL);
	if (ok) ok = pCaret->DoMoveNodes(pUndoOp,pLastCaretLineVTN,pNewLine,FIRSTCHILD);
	if (ok) ok = pCaretLine->DoFactorOutCommonChildAttributes(pUndoOp,FALSE,NULL);
	if (ok) ok = pNewLine->DoFactorOutCommonChildAttributes(pUndoOp,FALSE,NULL);

	// update other textstories that are dependant on this one
	SliceHelper::OnTextStoryChanged(pStory, this, &ObjParam, MasterText);

	// Update all the changed nodes
	ObjChangeParam ObjChange(OBJCHANGE_FINISHED,ObjChangeFlags(),NULL,pUndoOp);
	if (ok) ok = UpdateChangedNodes(&ObjChange);

	if (ok) pCaret->ScrollToShow();

	if (!ok) FailAndExecute();
	End();

	return ok;
}

BOOL OpTextFormat::DoTab()
{
	UndoableOperation* pUndoOp = this;

	// Get pointers to focus story, caret, caret line and caret line EOL
	TextStory* pStory = TextStory::GetFocusStory();
	ERROR2IF(pStory==NULL, FALSE, "OpTextFormat::DoTab() - No focus story");
	CaretNode* pCaret = pStory->GetCaret();
	ERROR2IF(pCaret==NULL, FALSE, "OpTextFormat::DoTab() - Focus story had no caret");
	TextLine* pCaretLine = pCaret->FindParentLine();
	ERROR2IF(pCaretLine==NULL,FALSE,"OpTextFormat::DoTab() - caret has no parent");

	// start the text op (must be done before AllopOp() which may insert actions)
	BOOL ok = DoStartTextOp(pStory);

	// see if the op is allowed
	ObjChangeParam ObjParam(OBJCHANGE_STARTING, ObjChangeFlags(), NULL, pUndoOp);
	if (pCaret->AllowOp(&ObjParam)==FALSE)
		return TRUE;

	// delete any sub-selection if it exists, then get ptr to last VTN on caret line
	if (ok) ok = DoDeleteSelection(pStory, TRUE);

	// After deleting the selection, the caret line may have been deleted
	// so we need to get it again in it's new location
	pCaret = pStory->GetCaret();
	ERROR2IF(pCaret==NULL, FALSE, "OpTextFormat::DoTab() - Focus story had no caret");
	pCaretLine = pCaret->FindParentLine();
	ERROR2IF(pCaretLine==NULL,FALSE,"OpTextFormat::DoTab() - caret has no parent");

	VisibleTextNode* pLastCaretLineVTN = pCaretLine->FindLastVTN();
	ERROR2IF(pLastCaretLineVTN==NULL,FALSE,"OpTextFormat::DoTab() - caret line has no VTN");

	// insert a new Tab node before caret with caret's attributes
	HorizontalTab* pTab = NULL;
	if (ok) pTab = new HorizontalTab;
	if (ok) ok = (pTab != NULL);
	if (ok) ok = pTab->DoInsertNewNode(pUndoOp,pCaret,PREV);
	if (ok) ok = pCaret->DoApplyAttrsTo(pUndoOp,pTab);

	// update other textstories that are dependant on this one
	//##SliceHelper::OnTextStoryChanged(pStory, this, &ObjParam, MasterText);

	// Update all the changed nodes
	ObjChangeParam ObjChange(OBJCHANGE_FINISHED,ObjChangeFlags(),NULL,pUndoOp);
	if (ok) ok = UpdateChangedNodes(&ObjChange);

	if (ok) pCaret->ScrollToShow();

	if (!ok) FailAndExecute();
	End();

	return ok;
}

/********************************************************************************************
>	void OpTextFormat::DoSwapCase()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/04/95
	Purpose:	The character to the right of the character has its case swapped, and the caret
				is moved right one character
********************************************************************************************/

void OpTextFormat::DoSwapCase()
{
	// Get pointers
	TextStory* pStory = TextStory::GetFocusStory();
	ERROR3IF(pStory == NULL, "You attempted to swap a character without a focus story");
	CaretNode* pCaret = NULL;
	if (pStory != NULL)
	 	pCaret = pStory->GetCaret();
	ERROR3IF(pCaret == NULL, "The TextStory didn't have a caret!");
	if (pCaret == NULL)
		return;

	// If there is a sub-selection then don't do anything
	if (pStory->GetSelectionEnd() != NULL)
	{
		Beep();
		return;
	}

	// Get a pointer to the character to the right of the caret
	VisibleTextNode* pSwapChar = NULL;
	pSwapChar = pCaret->FindNextVTNInStory();

	BOOL ok = TRUE;

	if (pSwapChar != NULL)
	{
		ok = DoStartTextOp(pStory);

		// Tell parent nodes about the op so the story reformats
		if (ok)
		{
			ObjChangeParam ObjInsert(OBJCHANGE_STARTING,ObjChangeFlags(),NULL,this);
			if (pSwapChar->AllowOp(&ObjInsert)==FALSE)
				pSwapChar=NULL;
		}

		// Swap the case of the found character
		if ((pSwapChar != NULL) && ok)
		{
			if (IS_A(pSwapChar,TextChar))
			{
				TextChar* pSwapTextChar = (TextChar*)pSwapChar;
				WCHAR CharValue = pSwapTextChar->GetUnicodeValue();
				CharCase Result = TextManager::ProcessCharCase(&CharValue, Swap);

				ok = (Result != Failed);

				if (ok && (Result != UnknownType))
				{
					// Store the characters old UniCode value
					ok = StoreCharCodeAction::DoStoreCharacterCode( this, &UndoActions, pSwapTextChar);

					// Set it to the new value
					if (ok)
						pSwapTextChar->SetUnicodeValue(CharValue);
				}
			}

			// advance the caret to the next character
			if (ok)
			{
				pStory->MoveCaretRightAChar();
				pStory->GetCaret()->HasMoved();
				pStory->GetCaret()->ScrollToShow();
			}
		}
   	}

	// Update all the changed nodes, i.e tell all the parents of the children that have been effected
	if (ok)
	{
		ObjChangeParam ObjChange(OBJCHANGE_FINISHED,ObjChangeFlags(),NULL,this);
		ok = UpdateChangedNodes(&ObjChange);
	}

	if (!ok)
	{
		InformError();
		FailAndExecute();
	}

	End();
}


/********************************************************************************************
>	void OpTextFormat::GetOpName(String_256* OpName)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/95
	Outputs:	OpName: The name of the operation (undo / redo string)
	Purpose:	Returns the name of the operation
********************************************************************************************/

void OpTextFormat::GetOpName(String_256* OpName)
{
	*OpName = String_256(_R(IDS_TYPING));
}


/////////////////////////////////////////////////////////////////////////////////////////////
// PositionCaretAction

/********************************************************************************************
>	PositionCaretAction::PositionCaretAction()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/95
	Purpose:	Constructor for PositionCaretAction.  Initialises the member variables  
********************************************************************************************/

PositionCaretAction::PositionCaretAction()
{
	pTextStory = NULL;
	pNearNode = NULL;
	AttachDirection = NEXT;
}


/********************************************************************************************
>	PositionCaretAction::~PositionCaretAction()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/95
	Purpose:	Destructor for PositionCaretAction.  
********************************************************************************************/

PositionCaretAction::~PositionCaretAction()
{
}


/********************************************************************************************
>	ActionCode PositionCaretAction::Execute()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/95
	Returns:	ActionCode, either AC_OK, AC_NORECORD or AC_FAIL
	Purpose:	This action inserts the opposite PositionCaretAction for redo purposes, then
				moves the CaretNode within the TextStory.
********************************************************************************************/

ActionCode PositionCaretAction::Execute()
{
	ERROR2IF(pTextStory==NULL,AC_FAIL,"PositionCaretAction::Execute() - pTextStory==NULL");
	ERROR2IF( pNearNode==NULL,AC_FAIL,"PositionCaretAction::Execute() - pNearNode==NULL");
	CaretNode* pCaret = pTextStory->GetCaret();
	ERROR2IF(    pCaret==NULL,AC_FAIL,"PositionCaretAction::Execute() - pCaret==NULL");
	
	// create redo action to position caret where it is currently
	BaseTextClass*      pReDoAttachNode = NULL;
	AttachNodeDirection ReDoAttachDir   = NEXT;
	if (!GetCaretNeighbour(pTextStory, &pReDoAttachNode, &ReDoAttachDir))
		return AC_FAIL;
	if (PositionCaretAction::Init(pOperation,pOppositeActLst,pTextStory,pReDoAttachNode,ReDoAttachDir)==AC_FAIL)
		return AC_FAIL;

	// ensure FIRSTCHILD actually means first child object
	Node*               pAttachNode = pNearNode;
	AttachNodeDirection  AttachDir  = AttachDirection;
	if (AttachDirection==FIRSTCHILD)
		pNearNode->GetAttachNodeAndDirectionToAttachFirstChildObject(&pAttachNode,&AttachDir);

	// Move caret, deselect in old pos and reselect in new pos to maintain ParentOfSelected
	BOOL CaretWasSelected = pCaret->IsSelected();
	if (CaretWasSelected)
		pCaret->SetSelected(FALSE);
	pCaret->MoveNode(pAttachNode, AttachDir);
	if (CaretWasSelected)
		pCaret->SetSelected(TRUE);

	pTextStory->AttachCaretAttributes();			// ensure caret has attrs of its context
	pCaret->ScrollToShow();							// ensure caret on screen

	return AC_OK;
}


/********************************************************************************************
>	static ActionCode PositionCaretAction::Init(Operation* pOp,
												ActionList* pActionList,
												TextStory* pStory,
												BaseTextClass* pAttachNode,
												AttachNodeDirection Direction)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/95
	Inputs:		pOp is the pointer to the operation to which this action belongs
				pActionList is the action list to which this action should be added
				pStory - pointer to the text story to apply this action to
				pAttachNode is the node the caret should be attached to
				Direction is the direction of the attachment (either NEXT or PREV)
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Call this function to cause the move of caret a TextStory during an
				undo/redo set of operations.  In your Do function move the caret, then
				call this function to insert an undo record for that deletion.
	SeeAlso:	Action::Init(), PositionCaretAction::Execute()
********************************************************************************************/

ActionCode PositionCaretAction::Init(Operation* pOp,
									ActionList* pActionList,
									TextStory* pStory,
									BaseTextClass* pAttachNode,
									AttachNodeDirection Direction)
{
	UINT32 ActSize = sizeof(PositionCaretAction);

	PositionCaretAction* pNewAction = NULL;

	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(PositionCaretAction), (Action**)&pNewAction);
	if ((Ac == AC_OK) && (pNewAction != NULL))
	{
		ERROR2IF(pStory == NULL, AC_FAIL, "TextStory pointer was NULL");
		ERROR2IF(pAttachNode == NULL, AC_FAIL, "Attachment node pointer was NULL");
		pNewAction->pTextStory = pStory;
		pNewAction->pNearNode = pAttachNode;
		pNewAction->AttachDirection = Direction;
	}

	return Ac;
}


/********************************************************************************************
>	static BOOL PositionCaretAction::DoStoreCaretPosition(Operation* pOp, TextStory* pStory)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com> (originally Peter)
	Created:	15/2/96
	Inputs:		pOp    - operation to which this action belongs (NULL if not undoable)
				pStory - pointer to textstory to which this action applies
	Returns:	FALSE if fails
	Purpose:	Call this function to restore the position of the caret in an undo list
********************************************************************************************/

BOOL PositionCaretAction::DoStoreCaretPosition(Operation* pOp, TextStory* pStory)
{
	ERROR2IF(pStory==NULL,FALSE,"PositionCaretAction::DoStoreCaretPosition() - pStory==NULL");
	ERROR2IF(pOp   ==NULL,FALSE,"PositionCaretAction::DoStoreCaretPosition() - pOp==NULL");

	// Get a pointer to the node to reattach the caret to
	CaretNode* pCaret = pStory->GetCaret();
	ERROR2IF(pCaret==NULL,FALSE,"PositionCaretAction::DoStoreCaretPosition() - pCaret==NULL");

	ActionList* pActionList = pOp->GetUndoActionList();
	ERROR2IF(pActionList==NULL,FALSE,"PositionCaretAction::DoStoreCaretPosition() - pActionList==NULL");

	// get a neighbour for the caret, and its direction
	BaseTextClass* pReattachNode = NULL;
	AttachNodeDirection AttachDir = NEXT;
	if (GetCaretNeighbour(pStory, &pReattachNode, &AttachDir)==FALSE)
		return FALSE;

	// Create an action for this
	if (PositionCaretAction::Init(pOp, pActionList, pStory, pReattachNode, AttachDir)==AC_FAIL)
		return FALSE;

	return TRUE;
}


/********************************************************************************************
>	BOOL PositionCaretAction::GetCaretNeighbour(TextStory* pStory, BaseTextClass** ppNeighbour,
												AttachNodeDirection* pDirection)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/95
	Inputs:		pStory      - points the TextStory with the caret
	Outputs:	ppNeighbour - ptr to node the caret should be attached to 
				pDirection  - direction of the attachment
	Returns:	FALSE if fails
	Purpose:	Gets a pointer to the node that execute function should attach the caret to.
********************************************************************************************/

BOOL PositionCaretAction::GetCaretNeighbour(TextStory* pStory, BaseTextClass** ppNeighbour,
											AttachNodeDirection* pDirection)
{
	ERROR2IF(     pStory== NULL,FALSE,"PositionCaretAction::GetCaretNeighbour() - pStory==NULL");
	ERROR2IF(ppNeighbour== NULL,FALSE,"PositionCaretAction::GetCaretNeighbour() - ppNeighbour==NULL");
	ERROR2IF( pDirection== NULL,FALSE,"PositionCaretAction::GetCaretNeighbour() - pDirection==NULL");

	// Get a pointer to the node to reattach the caret to
	CaretNode* pCaret = pStory->GetCaret();
	ERROR2IF(pCaret==NULL,FALSE,"PositionCaretAction::GetCaretNeighbour() - story has no caret");
	*ppNeighbour = pCaret->FindPrevVTNInLine();
	*pDirection  = NEXT;
	if (*ppNeighbour==NULL)
	{
		*ppNeighbour = pCaret->FindNextVTNInLine();
		*pDirection  = PREV;
	}
	if (*ppNeighbour==NULL)
	{
		*ppNeighbour = pCaret->FindParentLine();
		ERROR2IF(*ppNeighbour==NULL,FALSE,"PositionCaretAction::GetCaretNeighbour() - caret has no parent TExtLine");
		*pDirection  = FIRSTCHILD;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// OpTextKern methods

/********************************************************************************************
>	OpTextKern::OpTextKern() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/03/95
	Purpose:	OpTextKern constructor
********************************************************************************************/
            
OpTextKern::OpTextKern(): OpTextUndoable()								
{                              
}


/********************************************************************************************
>	BOOL OpTextKern::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/03/95
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpTextKern initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
********************************************************************************************/

BOOL OpTextKern::Init()
{
	return (RegisterOpDescriptor(0,
 							_R(IDS_KERNOP),
							CC_RUNTIME_CLASS(OpTextKern),
 							OPTOKEN_KERNTEXT,
 							OpTextKern::GetState,
 							0,							// help ID 
 							0,							// Bubble help
 							0,
 							0,
							SYSTEMBAR_ILLEGAL,			// For now !
							TRUE, 						// Receive messages
							FALSE,
							FALSE,
							0,
							GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION | DONT_GREY_WHEN_SELECT_INSIDE
	       )); 
}               

    
/********************************************************************************************
>	OpState	OpTextKern::GetState(String_256*, OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/03/95
	Returns:	The state of the OpTextKern
	Purpose:	For finding OpTextKern's state. The Op is greyed if there is no selected
				caret in the focus story.
********************************************************************************************/

OpState	OpTextKern::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
	OpSt.Greyed = TRUE;

	TextStory* pStory = TextStory::GetFocusStory();
	if (pStory != NULL)
	{
		CaretNode* pCaret = pStory->GetCaret();
		if (pCaret != NULL) 
		{
			if (pCaret->IsSelected() && pStory->GetSelectionEnd() == NULL)
				OpSt.Greyed = FALSE;
		}
	}

	return OpSt;   
}


/********************************************************************************************
>	void OpTextKern::DoWithParam(OpDescriptor*, OpParam* pOpParam)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/03/95
	Inputs:		OpDescriptor (unused)
				pOpParam->Param1 = XKernValue
				pOpParam->Param2 = YKernValue
	Purpose:	If the Caret is selected in the focus story then the kern is inserted to the 
				right of the caret. 
				If there is a sub-selection then a kern is inserted before the first selected
				character and a negative kern after the last selected character.  
	SeeAlso:	OpTextKern::DoInsertKernWithMerge
********************************************************************************************/
	
void OpTextKern::DoWithParam(OpDescriptor*, OpParam* pOpParam)
{   
	// Extract the parameter
	DocCoord KernValue(pOpParam->Param1, pOpParam->Param2);

	// get the focus story and caret
	TextStory* pFocusStory=TextStory::GetFocusStory();
	CaretNode* pCaret = NULL;
	ERROR3IF(pFocusStory==NULL,"OpTextKern::DoWithParam() - The focus story is missing");
	if (pFocusStory != NULL)
		pCaret = pFocusStory->GetCaret();
	ERROR3IF(pCaret==NULL,"OpTextKern::DoWithParam() - The text story's caret is missing");
	BOOL ok = ((pFocusStory != NULL) && (pCaret != NULL));

	// Start the TextOp - removes blobs and restores them at the end for us
	if (ok)
		ok = DoStartTextOp(pFocusStory);
	
	// if no selection insert kern before caret (else insert one at start and reverse at end of selection)
	if ( ok && (pFocusStory->GetSelectionEnd() == NULL))
	{
		ok = DoInsertKernWithMerge(pCaret, KernValue);
	}

	// Update all the changed nodes, i.e tell all the parents of the children that have been effected
	if (ok)
	{
		ObjChangeParam ObjChange(OBJCHANGE_FINISHED,ObjChangeFlags(),NULL,this);
		ok = UpdateChangedNodes(&ObjChange);
	}

	GetApplication()->UpdateSelection();

	if (!ok)
		FailAndExecute();

	End(); 
}			


/********************************************************************************************
>	BOOL OpTextKern::DoInsertKernWithMerge(VisibleTextNode* TTContextNode, DocCoord& Value)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/95
	Inputs:		TTContextNode: A kern will be Inserted before this node
				Value:		   The kern's X,Y offset value
	Returns:	TRUE if it worked
				FALSE otherwise, Call End()
	Purpose:	This Do function inserts a Kern before TTContextNode and merges adjacent
				kerns.
	SeeAlso:	OpTextKern::DoWithParam
********************************************************************************************/

BOOL OpTextKern::DoInsertKernWithMerge(VisibleTextNode* TTContextNode, DocCoord& Value)
{
	ERROR2IF(TTContextNode == NULL, FALSE, "TTContextNode is NULL");
	DocCoord KernValue = Value;
	AbstractTextChar* pPrevATC = (AbstractTextChar*)TTContextNode->FindPrevious(CC_RUNTIME_CLASS(AbstractTextChar));
	AbstractTextChar* pNextATC = (AbstractTextChar*)TTContextNode->FindNext(CC_RUNTIME_CLASS(AbstractTextChar));
	BOOL ok = TRUE;

	// if there is a kern to the right hide it and merge its value into the new one
	ObjChangeParam ObjDelete(OBJCHANGE_STARTING,ObjChangeFlags(TRUE),NULL,this);
	if (ok && (pNextATC!=NULL) && IS_A(pNextATC, KernCode))
	{
		KernValue = KernValue + ((KernCode*)pNextATC)->GetValue();
		ok = pNextATC->AllowOp(&ObjDelete);
		if (ok)
			ok = DoHideNode(pNextATC, TRUE);
	}

	// Hide the old kern code if the new values are zero
	if (ok && (pPrevATC!=NULL) && IS_A(pPrevATC, KernCode) && (KernValue.x==0) && (KernValue.y==0))
	{
		ok = pPrevATC->AllowOp(&ObjDelete);
		if (ok)
			ok = DoHideNode(pPrevATC, TRUE);
		pPrevATC = NULL;
	}

	// if the new kern will have an effect, set its value and insert it into the tree
	if (ok && (KernValue.x!=0 || KernValue.y!=0))
	{
		// Create a new kern if required, otherwise update the existing one
		if ((pPrevATC == NULL) || !IS_A(pPrevATC,KernCode) )
		{
			// Create the new Kern code
			KernCode* pKernCode = NULL;
			ALLOC_WITH_FAIL(pKernCode, (new KernCode()), this);
			ok = (pKernCode != NULL);

			// Now copy the attributes from the previous character to the kern
			if (ok)
			{
				AbstractTextChar* pAttrChar = pPrevATC;
				if (pAttrChar == NULL)
					pAttrChar = pNextATC;
				if (pAttrChar != NULL)
					ok = pAttrChar->CopyChildrenTo(pKernCode);
				// BODGE TEXT - should only copy children attributes
			}

			// Insert it in the tree
			if (ok)
			{
				pKernCode->SetValue(KernValue);
				ObjChangeParam ObjInsert(OBJCHANGE_STARTING,ObjChangeFlags(),NULL,this);
				ok = TTContextNode->AllowOp(&ObjInsert);
				if (ok)
					ok = pKernCode->DoInsertNewNode(this,TTContextNode,PREV);
			}
		}
		else
		{
			// Call AllowOp to mark the kern code as changed
			ObjChangeParam ObjEdit(OBJCHANGE_STARTING,ObjChangeFlags(),NULL,this);
			if (pPrevATC->AllowOp(&ObjEdit))
			{
				// Store the current code value
				ok = StoreKernCodeAction::DoStoreKernValue(this, &UndoActions, (KernCode*)pPrevATC);

				// set it to the new value
				if (ok)
					((KernCode*)pPrevATC)->SetValue(KernValue);
			}
		}
	}

	return ok;
}	


/********************************************************************************************
>	virtual void OpTextKern::PerformMergeProcessing()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/95
	Purpose:	This virtual function gets called to allow for the merging of text kern ops
	Errors:		[Not allowed to fail!]
	SeeAlso:	UndoableOperation::PerformMergeProcessing
********************************************************************************************/

void OpTextKern::PerformMergeProcessing()
{	 
	// Obtain a pointer to the operation history for the current document
	OperationHistory* pOpHist = &pOurDoc->GetOpHistory();
	ERROR3IF(pOpHist->FindLastOp() != this, "Last Op should be this op"); 
	
	// Is operation performed before this a OpTextKern?
	Operation* pPrevOp = pOpHist->FindPrevToLastOp();

	if ((pPrevOp != NULL) && IS_A(pPrevOp, OpTextKern))
	{
		// We can merge with the previous kern if the two ops both have a StoreKernCodeAction
		// And they both reference the same (document) kern code
		// We also need to access the invalid rectangles within the action list
		StoreKernCodeAction* pThisStoreKern = (StoreKernCodeAction*)GetUndoActionList()->FindActionOfClass(CC_RUNTIME_CLASS(StoreKernCodeAction)); 	
		StoreKernCodeAction* pPrevStoreKern = (StoreKernCodeAction*)pPrevOp->GetUndoActionList()->FindActionOfClass(CC_RUNTIME_CLASS(StoreKernCodeAction));
		InvalidateArbitaryAction* pThisInvalidRect = (InvalidateArbitaryAction*)GetUndoActionList()->FindActionOfClass(CC_RUNTIME_CLASS(InvalidateArbitaryAction));
		InvalidateArbitaryAction* pPrevInvalidRect = (InvalidateArbitaryAction*)pPrevOp->GetUndoActionList()->FindActionOfClass(CC_RUNTIME_CLASS(InvalidateArbitaryAction));
	
		if ((pThisStoreKern != NULL) && (pPrevStoreKern != NULL) && 
				(pThisInvalidRect != NULL) && (pPrevInvalidRect != NULL) && 
				(pThisStoreKern->GetKernCode() == pPrevStoreKern->GetKernCode()) &&
				(pThisInvalidRect->GetSpread() == pPrevInvalidRect->GetSpread()))
		{
			// In order to merge these two ops just throw away this op!
			// The kern code already has the new value, on undo the (correct) stored value is restored
			// However, we must merge the two redraw rectangles
			DocRect AllInvalid = pThisInvalidRect->GetRect().Union(pPrevInvalidRect->GetRect());
			pPrevInvalidRect->SetRect(AllInvalid);

			// This op is no longer required, so let's vape it
			pOpHist->DeleteLastOp(); 
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// OpTextAutoKern methods

/********************************************************************************************
>	OpTextAutoKern::OpTextAutoKern() 

	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/2000
	Purpose:	OpTextAutoKern constructor
********************************************************************************************/
OpTextAutoKern::OpTextAutoKern(): OpTextUndoable()								
{                              
}


/********************************************************************************************
>	BOOL OpTextAutoKern::Init()

	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/2000
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpTextKern initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
********************************************************************************************/
BOOL OpTextAutoKern::Init()
{
	return (RegisterOpDescriptor(0,									// Tool ID
 							_R(IDS_AUTOKERNOP),							// Resource ID
							CC_RUNTIME_CLASS(OpTextAutoKern),		// Runtime class
 							OPTOKEN_AUTOKERNTEXT,					// Token string
 							OpTextAutoKern::GetState,				// Get state function
 							0,										// Help ID
 							0,										// Bubble help
 							0,										// Resource ID
 							0,										// Control ID
							SYSTEMBAR_ILLEGAL,						// System bar group ID
							TRUE,									// Receive messages
							FALSE,									// Smart
							FALSE,									// Clean
							0,										// OneOpenInstID
							GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION | DONT_GREY_WHEN_SELECT_INSIDE	// Auto state flags
	       )); 
}               

    
/********************************************************************************************
>	OpState	OpTextAutoKern::GetState(String_256*, OpDescriptor*)

	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/2000
	Returns:	The state of the OpTextKern
	Purpose:	For finding OpTextKern's state. The Op is greyed if there is no selected
				caret in the focus story.
********************************************************************************************/
OpState	OpTextAutoKern::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;

	if (TextStory::GetFocusStory())
		OpSt.Greyed = FALSE;
	else
		OpSt.Greyed = TRUE;

	return OpSt;   
}


/********************************************************************************************
>	void OpTextAutoKern::DoWithParam(OpDescriptor*, OpParam* pOpParam)

	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/2000
	Inputs:		OpDescriptor (unused)
				pOpParam->Param1 = Autokerning?
				pOpParam->Param2 = (unused)
	Purpose:	Set the selected story to use/not use auto kerning
********************************************************************************************/
void OpTextAutoKern::DoWithParam(OpDescriptor*, OpParam* pOpParam)
{
	bool NewIsAutoKerning = pOpParam->Param1 != 0; // avoids warning casting to bool

	TextStory *pTextStory = TextStory::GetFocusStory();
	BOOL ok = pTextStory != 0;

	// causes shadows, contours & bevels to regenerate.
	ObjChangeFlags ChangeFlags;
	ChangeFlags.RegenerateNode = TRUE;

	// Start the TextOp - removes blobs and restores them at the end for us
	if (ok) ok = DoStartTextOp(pTextStory);

	if (ok)
	{
		ObjChangeParam ObjChange(OBJCHANGE_STARTING, ChangeFlags, pTextStory, this);
		ok = pTextStory->AllowOp(&ObjChange);
	}

	if (ok) ok = DoInvalidateNodeRegion(pTextStory, TRUE, FALSE);

	if (ok) ok = ToggleAutoKerningAction::Init(this, &UndoActions, pTextStory, NewIsAutoKerning) != AC_FAIL;

	if (ok) ok = DoInvalidateNodeRegion(pTextStory, TRUE, FALSE);

	// Inform all changed nodes that we have finished
	// Update all the changed nodes, i.e tell all the parents of the children that have been affected
	if (ok)
	{
		ObjChangeParam ObjChange(OBJCHANGE_FINISHED, ChangeFlags, pTextStory, this);
		ok = UpdateChangedNodes(&ObjChange);
	}

	if (!ok)
		FailAndExecute();

	End();
}


/////////////////////////////////////////////////////////////////////////////////////////////
// OpFitTextToCurve methods
            
/********************************************************************************************
>	OpFitTextToCurve::OpFitTextToCurve() 

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/03/95
	Purpose:	OpFitTextToCurve constructor
********************************************************************************************/
            
OpFitTextToCurve::OpFitTextToCurve(): OpTextUndoable()								
{                              
}


/********************************************************************************************
>	BOOL OpFitTextToCurve::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/03/95
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpFitTextToCurve initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
********************************************************************************************/

BOOL OpFitTextToCurve::Init()
{
	return (RegisterOpDescriptor(0,
 							_R(IDS_FITTEXTTOPATHOP),
							CC_RUNTIME_CLASS(OpFitTextToCurve),
 							OPTOKEN_FITTEXTTOPATH,
 							OpFitTextToCurve::GetState,
 							0,							// help ID 
 							_R(IDBBL_FITTEXTTOCURVE),		// Bubble help
 							_R(IDD_BARCONTROLSTORE),
 							_R(IDC_FITTEXTTOCURVE),
							SYSTEMBAR_EDIT,				// Bar ID
							TRUE, 						// Receive messages
							FALSE,
							FALSE,
							0,
							GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION | DONT_GREY_WHEN_SELECT_INSIDE
	       )); 
}               

    
/********************************************************************************************
>	OpState	OpFitTextToCurve::GetState(String_256*, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/03/95
	Returns:	The state of the OpFitTextToCurve
	Purpose:	For finding OpFitTextToCurve's state.  It is greyed unless there is just one
				path and one text line selected. 
********************************************************************************************/

OpState	OpFitTextToCurve::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;

	NodeRenderableInk* pPath = NULL;
	TextStory* pStory = NULL;
	UINT32 IDS = 0;

	// BODGE TEXT remove should work with a story with a caret on a path
	// Grey the op if we can't do it
	if (!FindPathAndText(&pPath, &pStory, NULL, &IDS))
	{
		OpSt.Greyed = TRUE;
		if (IDS == 0)
			IDS = _R(IDS_NOTPATHANDTEXTSEL);	
	}
	else
	{
		// Set the UI string for fit text / remove text
		if (pPath == NULL)
			IDS = _R(IDS_REMOVETEXTFROMPATHOP);
	}

	if (IDS != 0)
    	*UIDescription = String_256(IDS);			

	return(OpSt);   
}


/********************************************************************************************
>	void OpFitTextToCurve::Do(OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/03/95
	Inputs:		OpDescriptor (unused)
	Purpose:	move selected path into selected text story or remove the path from the story
********************************************************************************************/

void OpFitTextToCurve::Do(OpDescriptor*)
{   
	// get ptrs to path and text story
	NodeRenderableInk* pObject = NULL;
	TextStory* pStory = NULL;
	UINT32 dummy = 0;
	BOOL ok=FindPathAndText(&pObject, &pStory, this, &dummy);

	// Start the op by saving the selection state
	if (ok)	ok = DoStartSelOp(TRUE, TRUE);

	BeginSlowJob();		// Ended by Operation::End();

	// Inform the story so it reformats
	if (ok)
	{
		ObjChangeParam ObjInsert(OBJCHANGE_STARTING,ObjChangeFlags(FALSE,FALSE,FALSE,FALSE,FALSE,TRUE),NULL,this);
		ok = pStory->AllowOp(&ObjInsert);
	}

	// Inform the path if we are going to move it into the story
	if (ok && (pObject != NULL))
	{
		ObjChangeParam ObjDelete(OBJCHANGE_STARTING,ObjChangeFlags(TRUE),NULL,this);
		ObjChangeParam ObjMove(OBJCHANGE_STARTING,ObjChangeFlags(FALSE, FALSE, TRUE),NULL,this);
		if (!IS_A(pObject,NodePath))
			ok = pObject->AllowOp(&ObjDelete);
		else
			ok = pObject->AllowOp(&ObjMove);
	}

	// Stop now if the AllowOps failed
	if (!ok)
	{
		FailAndExecute();
		End();
		return;
	}
		
	// Localise attributes of the story
	if (ok)	ok = DoLocaliseCommonAttributes(pStory); 

	// we need to remove any controller nodes from the path, and
	// from the text
	// so we can add them later
	//
	// New method (Phil 07/08/2005):
	// If we have pObject we are always going to end up with a text story containing a NodePath
	// That text story may be inside a stack of contollers/effects or not
	// So, one stack of controllers/effects may need to be removed
	//
	if (ok && pObject && pStory)
	{
		Node* pCompoundAboveObj = NULL;
		Node* pCompoundAboveText = NULL;

PORTNOTE("other", "Removed live effects usage from text");
#ifndef EXCLUDE_FROM_XARALX
		// Find controller/effect stacks above the object and the text story
		ListRange* pStack = EffectsStack::GetEffectsStackFromNode(pObject, FALSE, TRUE, TRUE);
		if (pStack)
		{
			pCompoundAboveObj = pStack->FindLast();
			delete pStack;
		}

		pStack = EffectsStack::GetEffectsStackFromNode(pStory, FALSE, TRUE, TRUE);
		if (pStack)
		{
			pCompoundAboveText = pStack->FindLast();
			delete pStack;
		}

		// If both things have compound stacks get rid of the one on the object
		if (pCompoundAboveObj!=NULL && pCompoundAboveText!=NULL)
		{
			OpLiveEffect::DoDeleteAllPostProcessors(this, pObject, TRUE, TRUE);
			pCompoundAboveObj = NULL;
		}
#endif

		// We want to translate the story so it sits near the start of the path
		// so that attributes and effects are in roughly the right position.
		// Do it before the text node is moved inside the object's effects stack
		// because DoTransformNode transforms all applied effects.
		DocRect ObjectBounds = pObject->GetBoundingRect();
		DocRect StoryBounds = pStory->GetBoundingRect();

		Trans2DMatrix* pTransMat = new Trans2DMatrix(Matrix(ObjectBounds.lo.x-StoryBounds.lo.x, ObjectBounds.lo.y-StoryBounds.lo.y));
		ok = (pTransMat!=NULL);
		if (ok)
			ok = DoTransformNode(pStory, pTransMat);

		// If the object has a compound stack but the text doesn't then we want the text
		// to take on the effects of the object
		if (pCompoundAboveObj!=NULL && pCompoundAboveText==NULL)
		{
			DoMoveNode(pStory, pObject, NEXT);
		}
	}

	if (pObject == NULL)
	{
		// If the text story is already fitted to a path
		// Then remove the path...
		///
		if (ok)	ok = MatrixFitToPathAction::DoMatrixRemoveFromPath(this, &UndoActions, pStory);

		// Remove the path from the story
		pObject = pStory->GetTextPath();
		if (pObject != NULL)
		{
			// set story width to path length (minus physical indents), this is the simplest way
			// BODGE WORDWRAP - does this need to account for x scaling due to matrix?
			TextStoryInfo StoryInfo;
			if (ok) ok = pStory->CreateUntransformedPath(&StoryInfo);
			if (ok)
			{
				delete StoryInfo.pPath;
				if (StoryInfo.WordWrapping)
					pStory->SetStoryWidth(StoryInfo.PathLength - StoryInfo.LeftPathIndent - StoryInfo.RightPathIndent);

				// Move path outside text object (but not outside any controllers that may be applied)
				ok = DoMoveNode(pObject, pStory, NEXT);
			}

			if (ok) pObject->SetSelected(FALSE);
			if (ok) pStory->SetSelected(TRUE);
		}
	}
	else
	{
		// move the node into the TextStory
		if (ok)	pObject->SetSelected(FALSE);
		if (ok)	ok = DoMoveNode(pObject,pStory,FIRSTCHILD);

		// convert the object to a path if it isn't
		if (ok && !IS_A(pObject,NodePath))
		{
			Range pThing(pObject, pObject, RangeControl(TRUE, TRUE));
			ok = DoMakeShapes(pThing);
		}

		if (ok) ok = MatrixRemoveFromPathAction::DoMatrixFitToPath(this, &UndoActions, pStory);
		if (ok)	pStory->SetSelected(TRUE);
	}

	// Factor back out the story attributes
	if (ok) ok = DoFactorOutCommonChildAttributes(pStory);
	
	// Tell the parents of the edited nodes
	if (ok)
	{
		ObjChangeParam ObjChange(OBJCHANGE_FINISHED,ObjChangeFlags(FALSE,FALSE,FALSE,FALSE,FALSE,TRUE),NULL,this);
		ok = UpdateChangedNodes(&ObjChange);
	}

	if (!ok)
	{
		FailAndExecute();
		InformError();
	}

 	End();
}		

/********************************************************************************************
>	BOOL OpFitTextToCurve::FindPathAndText(NodeRenderableInk** pPath, TextStory** pStory, UndoableOperation* pOp, UINT32* ID)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/03/95
	Inputs:		pPath: (See below)
				pStory: (See below)
				pOp: the paramater to pass to ObjChangeParam
	Outputs:	pPath points to the path to fit the text to (is NULL is pStory has a path)
				pStory points to the text story to use (is NULL if no stories found)
	Returns:	TRUE if there is a selected path and text story OR a selected story with a path
				FALSE if there isn't
	Purpose:	Gets the text story and path to fit.  Used by the Do and the GetState functions
	SeeAlso:	OpFitTextToCurve::Do, OpFitTextToCurve::GetState
********************************************************************************************/

BOOL OpFitTextToCurve::FindPathAndText(NodeRenderableInk** pPath, TextStory** pStory, UndoableOperation* pOp, UINT32* ID)
{
	OpState OpSt;

//	Range* pSelRange = GetApplication()->FindSelection(); 
	*pStory = NULL;
	*pPath= NULL;

	// Set up the ObjChangeParam so we can ask the selected nodes if they mind being deleted
	ObjChangeParam ObjDeleteChange(OBJCHANGE_STARTING,ObjChangeFlags(TRUE),NULL,pOp);
	ObjChangeParam ObjNotChange(OBJCHANGE_STARTING,ObjChangeFlags(FALSE),NULL,pOp);

	BOOL FoundOnePath = FALSE;
	BOOL FoundOneTextStory = FALSE;
	BOOL FoundMultiple = FALSE;
	BOOL StoryHadPath = FALSE;

	SelRange* Selected = GetApplication()->FindSelection();
	Node* pNode = Selected->FindFirst();

	BOOL bFoundTextStoryWithController = FALSE;
	BOOL bFoundPathWithController = FALSE;
	
	BecomeA TestBecomeAPath(BECOMEA_TEST, CC_RUNTIME_CLASS(NodePath));

	while (pNode && !FoundMultiple)
	{
		// check for needs parent nodes too
		if (pNode->NeedsParent(NULL))
		{
			// find out if we have a base text class node here, in this hierarchy
			*pStory = (TextStory *)pNode->FindNext(CC_RUNTIME_CLASS(BaseTextClass));

			if ((*pStory) == NULL)
			{
				*pStory = (TextStory *)pNode->FindPrevious(CC_RUNTIME_CLASS(BaseTextClass));
			}

			// have we found the text story ? then dont bother trying to find it again
			if ((*pStory) != NULL)
				bFoundTextStoryWithController = TRUE;

			// have we found an object though ?
			if (!bFoundTextStoryWithController)
			{
				*pPath = (NodeRenderableInk *)pNode->FindNext(CC_RUNTIME_CLASS(NodeRenderableInk));

				if ((*pPath) == NULL)
				{
					*pPath = (NodeRenderableInk *)pNode->FindPrevious(CC_RUNTIME_CLASS(NodeRenderableInk));
				}

				if ((*pPath) != NULL && (*pPath)->NeedsParent(NULL))
				{
					*pPath = NULL;
				}
			}

			if ((*pPath) != NULL)
			{
				bFoundPathWithController = TRUE;
			}
		}

		// check for controller nodes selected with text children
		if (pNode->IsCompound())
		{
			if (pNode->IsController())
			{
				// try to find the text node
				*pStory = (TextStory *)pNode->FindFirstChild(CC_RUNTIME_CLASS(BaseTextClass));
				
				if ((*pStory) != NULL)				
					bFoundTextStoryWithController = TRUE;

				if (!bFoundTextStoryWithController)
				{
					// try to find the shape node
					*pPath = (NodeRenderableInk *)pNode->FindFirstChild(CC_RUNTIME_CLASS(NodeRenderableInk));

					while (*pPath && (*pPath)->NeedsParent(NULL))
					{
						*pPath = (NodeRenderableInk *)(*pPath)->FindNext(CC_RUNTIME_CLASS(NodeRenderableInk));
					}
				}
			}

			if ((*pPath) != NULL)
			{
				bFoundPathWithController = TRUE;
			}
		}

		if ((pNode->IS_KIND_OF(BaseTextClass) || bFoundTextStoryWithController)
			&& (*pStory) != pNode) // make sure we won't discount a node we've already found
		{
			// only do this once
			bFoundTextStoryWithController = FALSE;
			
			TextStory* pTS = NULL;
			
			if (*(pStory) == NULL)
			{
				if (IS_A(pNode,TextStory))
					pTS = (TextStory*)pNode;
				else
					pTS = (TextStory*)pNode->FindParent(CC_RUNTIME_CLASS(TextStory));
			}
			else
			{
				pTS = *pStory;
			}

			if (pTS != NULL)
			{
				if (FoundOneTextStory && (pTS != *pStory))
					FoundMultiple = TRUE;
				else
				{
					// We should check here to see if the TextStory is already on a path
					if (pTS->AllowOp(&ObjNotChange,FALSE))
					{
						FoundOneTextStory = TRUE;
						*pStory = pTS;
						if ((*pStory)->GetTextPath() != NULL)
						{
							FoundOnePath = TRUE;
							StoryHadPath = TRUE;
						}
					}
				}
			}
		}
		else if (pNode->IsAnObject() &&
				!pNode->IsCompound() &&
				((NodeRenderableInk*)pNode)->CanBecomeA(&TestBecomeAPath) ||
				bFoundPathWithController && (*pPath) != pNode
				)
		{
			if (FoundOnePath)
				FoundMultiple = TRUE;
			else
			{
				if (bFoundPathWithController)
				{
					if (!(*pPath)->AllowOp(&ObjDeleteChange, FALSE))
					{
						*pPath = NULL;
					}
					else
					{
						FoundOnePath = TRUE;
					}
				}
				else if (pNode->AllowOp(&ObjDeleteChange,FALSE))
				{
					FoundOnePath = TRUE;
					*pPath = (NodePath*)pNode;
				}
			}
		}

		pNode = Selected->FindNext(pNode);
	}

	// If there's a sub-selection the use the focus story
	if (!FoundOneTextStory && (TextStory::GetFocusStory() != NULL))
		FoundOneTextStory = TRUE;

	*ID = ObjDeleteChange.GetReasonForDenial();

	if (StoryHadPath)
		*pPath = NULL;

	return !FoundMultiple && ((FoundOnePath && FoundOneTextStory) || (StoryHadPath && FoundOneTextStory));
}


/////////////////////////////////////////////////////////////////////////////////////////////
// OpReverseStoryPath

/********************************************************************************************
>	static BOOL OpReverseStoryPath::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Returns:	TRUE/FALSE for successful initialisation of this operation
	Purpose:	Registers the ReversStoryPath operation
********************************************************************************************/

BOOL OpReverseStoryPath::Init()
{
	return (RegisterOpDescriptor(0,
 							_R(IDS_REVERSEPATHOP),
							CC_RUNTIME_CLASS(OpReverseStoryPath),
 							OPTOKEN_REVERSESTORYPATH,
 							OpReverseStoryPath::GetState)); 
}


/********************************************************************************************
>	static OpState OpReverseStoryPath::GetState(String_256*, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Inputs:		(unused)
	Returns:	The tick/grey state of this operation
	Purpose:	The op is greyed if there are no selected "Text on a path" TextStories
********************************************************************************************/

OpState OpReverseStoryPath::GetState(String_256*, OpDescriptor*)
{
	OpState State;
	State.Greyed = FALSE;

	SelRange* Selection = GetApplication()->FindSelection();
	Node* pNode = Selection->FindFirst();

	while (pNode != NULL)
	{
		if (IS_A(pNode,TextStory))
		{
			TextStory* pTS = (TextStory*)pNode;
			if (pTS->GetTextPath()==NULL /*|| pTS->IsWordWrapping()*/)
			{
				State.Greyed = TRUE;
				break;
			}
		}

		pNode = Selection->FindNext(pNode);
	}

	return State;
}


/********************************************************************************************
>	virtual void OpReverseStoryPath::Do(OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Inputs:		(Unused)
	Purpose:	Called when the user selects the "Reverse TextStory path" option.  Reverses
				the path for all selected TextStories on a path, so it (and the text) goes
				the other way
********************************************************************************************/

void OpReverseStoryPath::Do(OpDescriptor*)
{
	SelRange* pSelection = GetApplication()->FindSelection();
	Node* pNode = pSelection->FindFirst();
	BOOL ok = TRUE;

	if (ok && (pNode != NULL))
		ok = DoStartTextOp(NULL);

	while (ok && (pNode != NULL))
	{
		if (IS_A(pNode,TextStory))
		{
			NodePath* pPath = ((TextStory*)pNode)->GetTextPath();

			if (pPath != NULL)
			{
				ObjChangeParam ObjEdit(OBJCHANGE_STARTING,ObjChangeFlags(),NULL,this);
				if (pNode->AllowOp(&ObjEdit))
					ok = ReversePathAction::DoReversePath(this, &UndoActions, (TextStory*)pNode); 
			}
		}

		pNode = pSelection->FindNext(pNode);
	}

	// Tell the parents of the edited nodes
	if (ok)
	{
		ObjChangeParam ObjChange(OBJCHANGE_FINISHED,ObjChangeFlags(),NULL,this);
		ok = UpdateChangedNodes(&ObjChange);
	}

	// Do a SelChange so the text tool updates it's blobs
	if (ok)
		pSelection->Update();


	if (!ok)
	{
		FailAndExecute();
		InformError();
	}

	End();
}

#endif

/////////////////////////////////////////////////////////////////////////////////////////////
// The PrePostTextAction class

/********************************************************************************************
>	PrePostTextAction::PrePostTextAction()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/94
	Purpose:	Constructor for the text reformat action
********************************************************************************************/

PrePostTextAction::PrePostTextAction()
{
	HasEffect            = TRUE;
	pTextStoryToReformat = NULL;
	CaretContextNode     = NULL;
	CaretAttachDir       = PREV;
}


/********************************************************************************************
>	PrePostTextAction::PrePostTextAction()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/94
	Purpose:	Destructor for the text reformat action
********************************************************************************************/

PrePostTextAction::~PrePostTextAction()
{
}


/********************************************************************************************
>	static ActionCode PrePostTextAction::Init(Operation* pOp,
											  ActionList* pActionList,
											  TextStory* pStory,
											  BOOL ThisActionReformats,
											  Action** NewAction)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95
	Inputs:		pOp is the pointer to the operation to which this action belongs
				pActionList is the action list to which this action should be added
				pStory - pointer to the text story to reformat
				ThisActionReformats -   TRUE if this action will cause a reformat
										FALSE if it is dormant
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action.  There should
				be two actions in the ActionList for each operation, one at the start lying
				idle, and the one at the end of the action list causing a reformat.  The
				execute function should toggle the idle state of the action.  When inserting
				actions into the list in the Do function of your operation the first one should
				be active and the second one idle.
	SeeAlso:	Action::Init(), PrePostTextAction::Execute()
********************************************************************************************/

ActionCode PrePostTextAction::Init(	Operation* pOp,
									ActionList* pActionList,
									TextStory* pStory,
									BOOL ThisActionReformats,
									BOOL ToTail)
{
	UINT32 ActSize = sizeof(PrePostTextAction);
	PrePostTextAction* pNewAction;
	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(PrePostTextAction), (Action**)&pNewAction);

	if (Ac==AC_OK && pNewAction!=NULL)
	{
		pNewAction->m_bToTail = ToTail;
		
		pNewAction->HasEffect            = ThisActionReformats;
		pNewAction->pTextStoryToReformat = pStory;

		// Now store a Context node	and direction so the caret's position can be restored
		// must be on the same line, otherwise losts of grief!
		CaretNode* pCaret = pStory->GetCaret();
		ERROR2IF(pCaret==NULL,AC_FAIL,"PrePostTextAction::Init() - story has no caret");
		pNewAction->CaretAttachDir   = PREV;
		pNewAction->CaretContextNode = pCaret->FindNextVTNInLine();
		if (pNewAction->CaretContextNode==NULL)
		{
			pNewAction->CaretAttachDir   = NEXT;
			pNewAction->CaretContextNode = pCaret->FindPrevVTNInLine();
			ERROR2IF(pNewAction->CaretContextNode==NULL,AC_FAIL,"PrePostTextAction::Init() - could find no context node for caret!");
		}

		// reformat the text (if the tail flag is set)
		if (ToTail)
		{
			pStory->FlagNodeAndDescendantsModifiedByOpAndParentsHaveDescendantModifiedByOp();
			pStory->FormatAndChildren(NULL,FALSE);

			// remove the new action from the list, and add it to the head of
			// the action list
			pActionList->RemoveItem(pNewAction);
			pActionList->AddHead(pNewAction);
		}
	}

	return Ac;
}


/********************************************************************************************
>	static BOOL PrePostTextAction::DoFormatStory(UndoableOperation* pUndoOp, TextStory* pStory)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/95
	Inputs:		pUndoOp - pointer to an undoable operation
				pStory  - points to the story to reform
	Returns:	FALSE if fails
	Purpose:	Calls FormatAndChildren() then inserts a dormant action to reformat the story.
********************************************************************************************/

BOOL PrePostTextAction::DoFormatStory(UndoableOperation* pUndoOp, TextStory* pStory,
									  BOOL ToTail)
{
	ERROR2IF(pUndoOp==NULL,FALSE,"PrePostTextAction::DoFormatStory() - NULL operation pointer");
	ERROR2IF( pStory==NULL,FALSE,"PrePostTextAction::DoFormatStory() - NULL story pointer");
	
	// Format the story
	BOOL ok = pStory->FormatAndChildren(pUndoOp,!ToTail);

	// Insert dormant reformat action
	ActionList* pActionList = pUndoOp->GetUndoActionList();
	ERROR2IF(pActionList==NULL,FALSE,"PrePostTextAction::DoFormatStory() - pActionList==NULL");
	if (ok) ok = (Init(pUndoOp, pActionList, pStory, FALSE, ToTail) != AC_FAIL);

	return ok;
}


/********************************************************************************************
>	ActionCode PrePostTextAction::Execute()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95
	Returns:	ActionCode, either AC_OK, AC_NORECORD or AC_FAIL
	Purpose:	If this action is active then it calls the text story to reformat itself.  In
				both cases (active and idle) the opposite action created has the opposite state.
	Errors:		ERROR3 if the text story pointer is NULL
********************************************************************************************/

ActionCode PrePostTextAction::Execute()
{
	ERROR3IF(pTextStoryToReformat == NULL, "The TextStory pointer is NULL");
	
	// Create a redo action for this action, which is also a PrePostTextAction
	ActionCode Act;
	Act = PrePostTextAction::Init(pOperation, 
								  pOppositeActLst, 
								  pTextStoryToReformat,
								  !HasEffect,
								  m_bToTail);		  // Toggle
	
	if (Act == AC_FAIL)
		return AC_FAIL;

	if (pTextStoryToReformat == NULL)
		return Act;

	if (pTextStoryToReformat->FindParent() == NULL) // The TextStory is not in the tree, 
												  // it's probably hidden
		return Act;

	// Cause the reformat if required.
	if (HasEffect && pTextStoryToReformat!=NULL)
	{
		// Restore the caret first
		pTextStoryToReformat->MoveCaretToCharacter(CaretContextNode, CaretAttachDir);

		if (!pTextStoryToReformat->FormatAndChildren(NULL,FALSE,FALSE))
			return AC_FAIL;
	}
	return Act;
}

#if !defined(EXCLUDE_FROM_RALPH)

/////////////////////////////////////////////////////////////////////////////////////////////
// OpTextCaret methods

// Word movement rules - taken from Word 6
// Cursor left, right, up and down simply moves the caret.
// CTRL- left & right always puts caret at start of previous/next word.
// CTRL- up & down moves to start and end of paragraph.
// Home and end for start & end of line.  CTRL for start/end of story
// Movement with SHIFT always selects.

/********************************************************************************************
>	void OpTextCaret::DoMoveCaretLeft(BOOL WithSelect, BOOL AWord)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95 - rewritten 29/8/95
	Inputs:		WithSelect - movement should select words
				AWord - move the caret a word at a time
	Purpose:	Cursor left key operation - see movement rules above
	Errors:		Story and caret pointers are checked for NULL
********************************************************************************************/

void OpTextCaret::DoMoveCaretLeft(BOOL WithSelect, BOOL AWord)
{
	// Initialise
	BOOL Success = TRUE;
	if (!PreCaretMove())
	{
		Success = FALSE;
		goto DoneMovement;
	}

	if (WithSelect)
	{
		BOOL MoveSelection = TRUE;
		VisibleTextNode* pSelEnd = pCaret;
		VisibleTextNode* pSelStart = pOtherSelEnd;

		// Get the new selection end.  Move the current end to the left, either by one word or one character
		if (AWord)						   
		{
			// Special case so we skip back over selected EOLs like Word
			if (SelToRightOfCaret && pSelStart->IsAnEOLNode())
			{
				pSelStart = pSelStart->FindPrevVTNInStory();
				MoveSelection = FALSE;
			}
			else
				pSelStart = pStory->GetPrevWordChar(pSelStart);
		}
		else
		{
			VisibleTextNode* pTemp = pSelStart->FindPrevVTNInStory();
			if (pTemp != NULL)
				pSelStart = pTemp;
		}

		ERROR3IF(pSelStart == NULL, "Failed to find selection start");
		ERROR3IF(pSelEnd == NULL, "Failed to find selection end (where's the caret!)");
		VisibleTextNode* pScrollTo = pSelStart;
		if ((pSelStart != NULL) && (pSelEnd != NULL))
		{
			// Ensure that start is before end.  For left movement see if the character
			// before the new start is selected and swap if it is
			VisibleTextNode* pPrev = pSelStart->FindPrevVTNInStory();
			if ((pPrev != NULL) && (pSelStart != pSelEnd) && (pPrev->IsSelected() || pPrev->IsACaret()))
			{
				VisibleTextNode* pTemp = pSelEnd;
				pSelEnd = pSelStart;
				pSelStart = pTemp;
				pTemp = pSelEnd->FindPrevVTNInStory();
				if (AWord && MoveSelection && (pTemp != NULL) && !pSelEnd->IsAnEOLNode())
					pSelEnd = pTemp;
				pScrollTo = pSelEnd;
			}

			// Select the characters and scroll to show moving end
			pScrollTo->ScrollToShow();		
			OpTextCaret::SelectStoryCharacters(pSelStart, pSelEnd, pStory, pCaret);
		}
	}
	else
	{
		// If there is a selected region move caret to its end, else move caret
		if (pStory->GetSelectionEnd() != NULL)
			MoveCaretToSelectionEnd(TRUE);
		else
		{
			if (pCaret->FindPrevVTNInStory() != NULL)
			{
				NodeRenderableInk::DeselectAll(TRUE, FALSE);
				if (AWord)
					Success = pStory->MoveCaretLeftAWord();
				else
					Success = pStory->MoveCaretLeftAChar();
				pCaret->ScrollToShow();
				pCaret->Select(TRUE);
			}
		}
	}

DoneMovement:
	if (!Success)
	{
		FailAndExecute();
		InformError();
	}
	
	CompleteOperation();
}


/********************************************************************************************
>	void OpTextCaret::DoMoveCaretRight(BOOL AWord = FALSE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Inputs:		pTextStory: The TextStory to perform the operation on
				AWord: 		Move the caret a word at a time
	Purpose:	Cursor right key operation. If the caret is selected then the caret moves
				one position to the right.  If there is a sub-selection then the caret will
				move to the right of the last selected character and become selected. The
				subselection is cleared.
********************************************************************************************/

void OpTextCaret::DoMoveCaretRight(BOOL WithSelect, BOOL AWord)
{
	BOOL Success = TRUE;
	if (!PreCaretMove())
	{
		Success = FALSE;
		goto DoneMovement;
	}

	if (WithSelect)
	{
		VisibleTextNode* pSelEnd = pCaret;
		VisibleTextNode* pSelStart = pOtherSelEnd;

		// Get the new selection end.  Move the current end to the right, either by one word or one character
		if (AWord)
		{
			// If the sel end is at the end of a line then skip to the start of the new line
			// then to the end of the first word on the line
			if (pSelStart->IsAnEOLNode() && SelToRightOfCaret)
				pSelStart = pStory->GetNextWordChar(pSelStart);
			pSelStart = pSelStart->FindNextVTNInStory();
			pSelStart = pStory->GetNextWordChar(pSelStart);
		}
		else
			pSelStart = pSelStart->FindNextVTNInStory();

		ERROR3IF(pSelStart == NULL, "Failed to find slection start");
		ERROR3IF(pSelEnd == NULL, "Failed to find slection end (where's the caret!)");
		VisibleTextNode* pScrollTo = pSelStart;
		if ((pSelStart != NULL) && (pSelEnd != NULL))
		{
			// Ensure that start is before end.  For right movement see if the character
			// after the new start is selected, swap if it isn't
			VisibleTextNode* pNext = pSelStart->FindNextVTNInStory();
			if ((pNext == NULL) || (pNext != NULL) && (pSelStart != pSelEnd) && (pNext != pSelEnd) && !pNext->IsSelected())
			{
				VisibleTextNode* pTemp = pSelEnd;
				pSelEnd = pSelStart;
				pSelStart = pTemp;
				if (AWord && (pSelEnd->FindPrevVTNInStory() != NULL) && (pSelEnd->FindNext() != pStory->FindLastVTN()))
					pSelEnd = pSelEnd->FindPrevVTNInStory();
				pScrollTo = pSelEnd;
			}

			// Select the characters and scroll to show moving end
			pScrollTo->ScrollToShow();		
			OpTextCaret::SelectStoryCharacters(pSelStart, pSelEnd, pStory, pCaret);
		}
	}
	else
	{
		// If there is a selected region move caret to its end, else move caret
		if (pStory->GetSelectionEnd() != NULL)
			MoveCaretToSelectionEnd(FALSE);
		else
		{
			// Do simple caret moving
			if (pCaret->FindNextVTNInStory() != pStory->FindLastVTN())
			{
				NodeRenderableInk::DeselectAll(TRUE,FALSE);
				if (AWord)
					pStory->MoveCaretRightAWord();
				else
					pStory->MoveCaretRightAChar();
				pCaret->ScrollToShow();
				pCaret->Select(TRUE);
			}
		}
	}

DoneMovement:
	if (!Success)
	{
		FailAndExecute();
		InformError();
	}
	
	CompleteOperation();
}
	

/********************************************************************************************
>	void OpTextCaret::DoMoveCaretUp(BOOL WithSelect)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95
	Inputs:		WithSelect - TRUE if characters should be selected
	Purpose:	Moves the caret up one line
********************************************************************************************/

void OpTextCaret::DoMoveCaretUp(BOOL WithSelect)
{
	BOOL Success = TRUE;
	if (!PreCaretMove())
	{
		Success = FALSE;
		goto DoneMovement;
	}
		
	if (WithSelect)
	{
		VisibleTextNode* pSelEnd = pCaret;
		VisibleTextNode* pSelStart = pOtherSelEnd;

		// Move the current selection end up a line
		BOOL SelToLeftOfNewEnd = TRUE;
		pSelStart = GetCharacterLineChange(pSelStart, SelToRightOfCaret, &SelToLeftOfNewEnd, TRUE);

		if (pSelStart == NULL)
		{
			DoMoveCaretHome(TRUE, TRUE);
			return; // We have already End()ed
		}
		else
		{
			ERROR3IF(pSelEnd == NULL, "Failed to find slection end (where's the caret!)");
			if ((pSelStart != NULL) && (pSelEnd != NULL))
			{
				if (!SelToLeftOfNewEnd && (pSelStart->FindNextVTNInStory() != NULL))
					pSelStart = pSelStart->FindNextVTNInStory();
				VisibleTextNode* pScrollTo = pSelStart;
				// Ensure that start is before end.  For up movement see if the character
				// before the new start is selected and swap if it is
				VisibleTextNode* pPrev = pSelStart->FindPrevVTNInStory();
				if ((pPrev != NULL) && (pSelStart != pSelEnd) && (pPrev->IsSelected() || pPrev->IsACaret()))
				{
					VisibleTextNode* pTemp = pSelEnd;
					pSelEnd = pSelStart;
					pSelStart = pTemp;
					if (pSelEnd->FindPrevVTNInStory() != NULL)
						pSelEnd = pSelEnd->FindPrevVTNInStory();
					pScrollTo = pSelEnd;
				}

				// Select the characters and scroll to show moving end
				pScrollTo->ScrollToShow();		
				OpTextCaret::SelectStoryCharacters(pSelStart, pSelEnd, pStory, pCaret);
			}
		}
	}
	else
	{
		if (pStory->GetSelectionEnd() != NULL)
			MoveCaretToSelectionEnd(TRUE);
		else
		{
			BOOL ToLeft = FALSE;
			VisibleTextNode* pNewPos = GetCharacterLineChange(pCaret, TRUE, &ToLeft, TRUE);

			if (pNewPos != NULL)
			{
				NodeRenderableInk::DeselectAll(TRUE,FALSE);
				pStory->MoveCaretToCharacter(pNewPos, ToLeft?PREV:NEXT);
				pCaret->ScrollToShow();
				pCaret->Select(TRUE);
			}
		}
	}

DoneMovement:
	if (!Success)
	{
		FailAndExecute();
		InformError();
	}
	
	CompleteOperation();
}


/********************************************************************************************
>	void OpTextCaret::DoMoveCaretDown(BOOL WithSelect)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95
	Inputs:		WithSelect - TRUE if characters should be selected
				Scroll - TRUE to scroll the screen
	Purpose:	Moves the caret down one line, dealing with selection if needed
********************************************************************************************/

void OpTextCaret::DoMoveCaretDown(BOOL WithSelect)
{
	BOOL Success = TRUE;
	if (!PreCaretMove())
	{
		Success = FALSE;
		goto DoneMovement;
	}
		
	if (WithSelect)
	{
		// get the 'other' end of the currect selection (the caret is at non-moving end)
		VisibleTextNode* pSelEnd = pCaret;
		VisibleTextNode* pSelStart = pOtherSelEnd;
		VisibleTextNode* pOldSelStart = pSelStart;
		BOOL SelToLeftOfNewEnd = TRUE;
		pSelStart = GetCharacterLineChange(pSelStart, SelToRightOfCaret, &SelToLeftOfNewEnd, FALSE);

		if (pSelStart == NULL)
		{
			DoMoveCaretEnd(TRUE, TRUE);
			return; // We have already End()ed
		}
		else
		{
			ERROR3IF(pSelEnd == NULL, "Failed to find selection end (where's the caret!)");
			if ((pSelStart != NULL) && (pSelEnd != NULL))
			{
				if (!SelToLeftOfNewEnd && (pSelStart->FindNextVTNInStory() != NULL))
					pSelStart = pSelStart->FindNextVTNInStory();
				VisibleTextNode* pScrollTo = pSelStart;

				// Ensure that start is before end.  For down movement see if the character
				// after the new start is selected, swap if it isn't
				VisibleTextNode* pNext = pSelStart->FindNextVTNInStory();
				if ((pNext == NULL) || ((pNext != NULL) && (pSelStart != pSelEnd) && (pNext != pSelEnd) && !pNext->IsSelected()))
				{
					VisibleTextNode* pTemp = pSelEnd;
					pSelEnd = pSelStart;
					pSelStart = pTemp;
					VisibleTextNode *pPrev = pSelEnd->FindPrevVTNInStory();
					if ((pPrev != NULL) && (pPrev != pOldSelStart))
						pSelEnd = pPrev;
					pScrollTo = pSelEnd;
				}

				if (pScrollTo->IsAnEOLNode() && (pScrollTo->FindNextVTNInStory() != NULL))
					pScrollTo = pScrollTo->FindNextVTNInStory();

				// Select the characters and scroll to show moving end
				pScrollTo->ScrollToShow();		
				OpTextCaret::SelectStoryCharacters(pSelStart, pSelEnd, pStory, pCaret);
			}
		}
	}
	else
	{
		if (pStory->GetSelectionEnd() != NULL)
			MoveCaretToSelectionEnd(FALSE);
		else
		{
			BOOL ToLeft = FALSE;
			VisibleTextNode* pNewPos = GetCharacterLineChange(pCaret, TRUE, &ToLeft, FALSE);

			if (pNewPos != NULL)
			{
				NodeRenderableInk::DeselectAll(TRUE,FALSE);
				pStory->MoveCaretToCharacter(pNewPos, ToLeft?PREV:NEXT);
				pCaret->ScrollToShow();
				pCaret->Select(TRUE);
			}
		}
	}

DoneMovement:
	if (!Success)
	{
		FailAndExecute();
		InformError();
	}
	
	CompleteOperation();
}


/********************************************************************************************
>	void OpTextCaret::DoMoveCaretHome(BOOL WithSelect, BOOL ToStoryStart)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Inputs:		WithSelect - true if characters from the current caret position to the home
				position should be selected
	Purpose:	Moves the caret to the left of the first character on the line, or to the 
				start of the TextStory
********************************************************************************************/

void OpTextCaret::DoMoveCaretHome(BOOL WithSelect, BOOL ToStoryStart)
{
	if (!PreCaretMove())
	{
		FailAndExecute();
		InformError();
		End();
		return;
	}

	VisibleTextNode* pMoveChar = pStory->GetSelectionEnd();
	if (ToStoryStart)
		pMoveChar = pStory->FindFirstVTN();
	else
	{
		if (pMoveChar == NULL)
			pMoveChar = GetLineStartCharacter(pCaret);
		else
			pMoveChar = GetLineStartCharacter(pMoveChar);
	}

	if (pMoveChar == NULL)
		return ;

	if (WithSelect)
	{
		// Get a pointer to the first and last selectable chars
		VisibleTextNode* pSelEndChar = pCaret;
		VisibleTextNode* pSelStartChar = (VisibleTextNode*)pMoveChar;
		if ((pSelEndChar != NULL) && (pSelStartChar != NULL))
		{
			// Its possible that the end character is in front of the start
			if (pSelStartChar != pSelEndChar)
			{
				VisibleTextNode* pChar = pSelEndChar;
				while (pChar != NULL)
				{
					if (pChar == pSelStartChar)
					{
						pSelStartChar = pCaret->FindNextVTNInStory();
						pSelEndChar = pChar->FindPrevVTNInStory();
						break;
					}

					pChar = pChar->FindNextVTNInStory();
				}

				if (pChar==NULL)
					pSelEndChar = pCaret->FindPrevVTNInStory();
			}				

			if (pSelStartChar != NULL)
				pSelStartChar->ScrollToShow();
		 	SelectStoryCharacters(pSelStartChar, pSelEndChar, pStory, pCaret);
		}
	}
	else
	{
		NodeRenderableInk::DeselectAll(TRUE,FALSE);
		if (pMoveChar != pCaret)
			pStory->MoveCaretToCharacter(pMoveChar, PREV);
		pCaret->ScrollToShow();
		pCaret->Select(TRUE);
	}

	CompleteOperation();
}


/********************************************************************************************
>	void OpTextCaret::DoMoveCaretEnd(BOOL WithSelect, BOOL ToStoryEnd)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Inputs:		WithSelect - true if characters from the current caret position to the end
				of the line should be selected
	Purpose:	Moves the caret to the right of the last character on the line
********************************************************************************************/

void OpTextCaret::DoMoveCaretEnd(BOOL WithSelect, BOOL ToStoryEnd)	
{
	if (!PreCaretMove())
	{
		FailAndExecute();
		InformError();
		End();
		return;
	}

	VisibleTextNode* pMoveChar = pStory->GetSelectionEnd();
	if (ToStoryEnd)
		pMoveChar = pStory->FindLastVTN();
	else
	{
		if (pMoveChar == NULL)
			pMoveChar = GetLineEndCharacter(pCaret);
		else
			pMoveChar = GetLineEndCharacter(pMoveChar);
	}

	if (pMoveChar == NULL)
		return ;

	if (WithSelect)
	{
		// Get a pointer to the first and last selectable chars
		VisibleTextNode* pSelStartChar = pCaret;
		VisibleTextNode* pSelEndChar = pMoveChar->FindPrevVTNInStory();
		if (pSelEndChar == NULL)
			pSelEndChar = pMoveChar;

		if (pSelStartChar != pSelEndChar)
		{
			// Its possible that the end character is in front of the start
			VisibleTextNode* pChar = pSelEndChar;
			while (pChar != NULL)
			{
				if (pChar == pSelStartChar)
				{
					pSelStartChar = pSelEndChar->FindNextVTNInStory();
					pSelEndChar = pCaret->FindPrevVTNInStory();
					break;
				}

				pChar = pChar->FindNextVTNInStory();
			}
			if (pChar == NULL)
				pSelStartChar = pCaret->FindNextVTNInStory();
		}

		if (pSelEndChar != NULL)
			pSelEndChar->ScrollToShow();

	 	SelectStoryCharacters(pSelStartChar, pSelEndChar, pStory, pCaret);
	}
	else
	{
		NodeRenderableInk::DeselectAll(TRUE,FALSE);
		if (pMoveChar != pCaret)
		{
			if (pMoveChar->IsAnEOLNode())
				pStory->MoveCaretToCharacter(pMoveChar, PREV);
			else
				pStory->MoveCaretToCharacter(pMoveChar, NEXT);
		}
		pCaret->ScrollToShow();
		pCaret->Select(TRUE);
	}

	CompleteOperation();
}


/********************************************************************************************
>	BOOL OpTextCaret::PreCaretMove()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/08/95
	Returns:	TRUE for success, FALSE if op should not continue
	Purpose:	Carries out common pre operation code for caret movement ops
	Errors:		Checks story and caret pointers for NULL
********************************************************************************************/

BOOL OpTextCaret::PreCaretMove()
{
	// Pointer checks on the ops member variables
	ERROR2IF(pStory == NULL, FALSE, "(OpTextCaret::DoMoveCaretLeft) pStory was NULL");
	ERROR2IF(pCaret == NULL, FALSE, "(OpTextCaret::DoMoveCaretLeft) pCaret was NULL");

	GetApplication()->FindSelection()->SetGag(TRUE);

	return TRUE;
}


/********************************************************************************************
>	void OpTextCaret::DoSelectWordAtCaret()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/04/95
	Purpose:	Moves the caret to the start of the word that is is currently at, then selects
				the characters to the end of the word 
	Errors:		Checks story and caret pointers for NULL
********************************************************************************************/

void OpTextCaret::DoSelectWordAtCaret()
{
	// Validate the pointers
	if (!PreCaretMove())
	{
		InformError();
		FailAndExecute();
		End();
		return;
	}
	
	NodeRenderableInk::DeselectAll(TRUE,FALSE);
	DialogManager::DefaultKeyboardFocus();

	// find the start of the previous word and move the caret there
	VisibleTextNode* pPrevWordStart = pStory->GetPrevWordChar(pCaret);
	if (pPrevWordStart==NULL)
	{
		ERROR3("OpTextCaret::DoSelectWordAtCaret() - failed to find start of previous word!");
		return;
	}
	if (pPrevWordStart!=pCaret)
		pStory->MoveCaretToCharacter(pPrevWordStart, PREV);

	// find the start of the next word
	VisibleTextNode* pNextWordStart = pStory->GetNextWordChar(pPrevWordStart);
	if (pNextWordStart==NULL)
	{
		ERROR3("OpTextCaret::DoSelectWordAtCaret() - failed to find start of next word!");
		return;
	}

#ifdef SELECT_CARET_AND_CHARS
	// Select the caret
	pCaret->Select(TRUE);
#endif

	// select all chars between the start of the last word to the start of the next word
	VisibleTextNode* pVTN = pPrevWordStart;
	while (pVTN!=NULL && pVTN!=pNextWordStart)
	{
		if (!pVTN->IsAnEOLNode())
			pVTN->Select(TRUE);
		pVTN = pVTN->FindNextVTNInStory();
		ERROR3IF(pVTN==NULL,"OpTextCaret::DoSelectWordAtCaret() - never found pNextWordStart");
	}

	GetApplication()->FindSelection()->SetGag(FALSE);
	CompleteOperation();
}

  
/********************************************************************************************
>	OpTextCaret::OpTextCaret()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/94
	Purpose:	OpTextCaret constructor.  Sets the member variables
********************************************************************************************/

OpTextCaret::OpTextCaret()
{
	// Get pointers to the focus story.  There should be one!
	pStory = TextStory::GetFocusStory();
	ERROR3IF(pStory == NULL, "OpTextCaret created without a focus story");

	// Get pointer to caret and other end of selection
	if (pStory != NULL)
	{
		pCaret = pStory->GetCaret();
		pOtherSelEnd = pStory->GetSelectionEnd(&SelToRightOfCaret);

		// If no selection then set pointer to the caret
		if (pOtherSelEnd == NULL)
			pOtherSelEnd = pCaret;
	}
	else
	{
		pCaret = NULL;
		pOtherSelEnd = NULL;
	}

	// Reset the vertical move inset
	PreviousVerticalInset = VertInset;
	VertInset.LastInset = VERTICALMOVEINSET_NONE;
	VertInset.HoldingInset = VERTICALMOVEINSET_NONE;
}


/********************************************************************************************
>	OpTextCaret::MoveCaretToSelectionEnd(BOOL MoveToLeft)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/95
	Inputs:		MoveToLeft, TRUE if caret should go to left side of selection, FALSE for right side
	Purpose:	This function should be called to move the caret to the end of the selected text.
				It also selectes the caret, deselecting the other characters
********************************************************************************************/

void OpTextCaret::MoveCaretToSelectionEnd(BOOL MoveToLeft)
{
	ERROR3IF(pStory == NULL, "pStory was NULL");
	ERROR3IF(pCaret == NULL, "pCaret was NULL");

	if ((pStory != NULL) && (pCaret != NULL) )
	{
		BOOL SelToRightOfCaret = TRUE;
		VisibleTextNode* pSelEnd = pStory->GetSelectionEnd(&SelToRightOfCaret);
		ERROR3IF(pSelEnd == NULL, "Who cleared the selection before calling MoveCaretToSelectionEnd?");

		// Reposition the caret at right end of the selected region
		NodeRenderableInk::DeselectAll(TRUE,FALSE);
		if ((pSelEnd != NULL) && (MoveToLeft != SelToRightOfCaret))
		{
			if (pSelEnd->IsAnEOLNode())
				pStory->MoveCaretToCharacter(pSelEnd, PREV);
			else
				pStory->MoveCaretToCharacter(pSelEnd, MoveToLeft?PREV:NEXT);
		}
		pCaret->ScrollToShow();
		pCaret->Select(TRUE);
	}
}


/********************************************************************************************
>	OpTextCaret::CompleteOperation()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/95
	Purpose:	This function performs common things done at the end of all operations
				It calls the End() method so don't do anything else except return 
				after calling it.
********************************************************************************************/

void OpTextCaret::CompleteOperation()
{
	GetApplication()->UpdateSelection();

	End();
}



/********************************************************************************************
>	static void OpTextCaret::SelectStoryCharacters(VisibleTextNode* pFirstToSel, VisibleTextNode* pLastToSel,
													TextStory* pStory, CaretNode* pCaret = NULL);

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/95
	Inputs:		pFirstToSel - points to the first character to select
				pLastToSel - points to the last character to select
				pStory - points to the story to operate in
				pCaret - points to the caret in the TextStory, pass in NULL if you don't have it
	Purpose:	This function selects characters between the two pointers (inclusive), deselecting
				all other characters in the story.
********************************************************************************************/

void OpTextCaret::SelectStoryCharacters(VisibleTextNode* pFirstToSel, VisibleTextNode* pLastToSel,
															TextStory* pStory, CaretNode* pCaret)
{
	ERROR3IF(pStory == NULL, "Story pointer was NULL");
	if ((pFirstToSel == NULL) || (pLastToSel == NULL) || (pStory == NULL))
		return;

	// Get required pointers
	if (pCaret == NULL)
		pCaret = pStory->GetCaret();

	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr == NULL)
		return;

	// Adjust the selection bounds so caret is not included.
	if (pFirstToSel != pLastToSel)
	{
		if (pFirstToSel->IsACaret())
			pFirstToSel = pFirstToSel->FindNextVTNInStory();
		else if (pLastToSel->IsACaret())
			pLastToSel = pLastToSel->FindPrevVTNInStory();
	}

	// Check that we haven't just swapped the start and end
	if (pLastToSel == pFirstToSel->FindPrevVTNInStory())
	{
		VisibleTextNode* pTemp = pFirstToSel;
		pFirstToSel = pLastToSel;
		pLastToSel = pTemp;
	}

	Spread* pSpread = pStory->FindParentSpread();

	// We only need to use the Blob manager, if there are some pending render regions
	RenderRegionList* pRegionList = GetApplication()->GetRegionList();
	BOOL UseBlobMgr = !pRegionList->IsEmpty();

	RenderRegion* pRegion =	NULL;

	// Select characters using just one render region
	if (!UseBlobMgr)
		pRegion = DocView::RenderOnTop(NULL, pSpread, ClippedEOR );

	while ( UseBlobMgr || pRegion != NULL )
	{
		// Loop through all the characters in the story
		BOOL Select = FALSE;
		VisibleTextNode* pChar = pStory->FindFirstVTN();
		VisibleTextNode* pNextChar;
		while (pChar != NULL)
		{
			if (pChar == pFirstToSel)
				Select = TRUE;

			// We don't want the very last character to be selectable
			pNextChar = pChar->FindNextVTNInStory();
			if (pNextChar == NULL)
				Select = FALSE;

			if (Select)
			{
				if (!pChar->IsSelected())
				{
					pChar->SetSelected(TRUE);

					if (UseBlobMgr)
						pBlobMgr->RenderObjectBlobsOn(NULL, pSpread, pChar);
					else
						pChar->RenderObjectBlobs(pRegion);
				}
			}
			else
			{
				if (pChar->IsSelected())
				{
					if (UseBlobMgr)
						pBlobMgr->RenderObjectBlobsOff(NULL, pSpread, pChar);
					else
						pChar->RenderObjectBlobs(pRegion);

					pChar->SetSelected(FALSE);
				}
			}

			if (pChar == pLastToSel)
				Select = FALSE;

			pChar = pNextChar;

		}

		// Get the Next render region
		if (UseBlobMgr)
			UseBlobMgr = FALSE;
		else
			pRegion = DocView::GetNextOnTop(NULL);
	}

	GetApplication()->UpdateSelection();

#ifdef SELECT_CARET_AND_CHARS
	// Ensure the caret is always selected
	pCaret->Select(TRUE);
#else
	// Now that we have finished selecting characters, select the caret if there is no selected chars
	if (pStory->GetSelectionEnd() == NULL)
		pCaret->Select(TRUE);
#endif

}


/********************************************************************************************
>	VisibleTextNode* OpTextCaret::GetCharacterLineChange(	VisibleTextNode* pCharacter,
															BOOL ToLeftOfChar,
															BOOL* NewToLeft,
															BOOL MoveUp)
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/04/95
	Inputs:		pCharacter - points to the character to look from
				ToLeftOfChar - TRUE/FALSE if current pos is to left/right of pCharacter
				NewToLeft - pointer to a BOOL to output a return value
				MoveUp - TRUE to look up a line, FALSE to look down
	Outputs:	ToLeft - TRUE to position the caret to the left of the return character
						 FALSE to position the caret to the right of the return character
	Returns:	A pointer to the character on the line above/below pCharacter, or NULL if there
				isn't a character.
	Purpose:	This function returns a pointer to the character on the line above or below
				the given character or position.  If there is a previous vertical movement
				position then this is used, rather then the supplied character.
********************************************************************************************/

VisibleTextNode* OpTextCaret::GetCharacterLineChange(VisibleTextNode* pCharacter, BOOL ToLeftOfChar,
														BOOL* NewToLeft,  BOOL MoveUp)
{
	ERROR2IF(pCharacter == NULL, NULL, "NULL entry param");

	// Get a pointer to the current and next TextLine
	TextLine* pCurrentLine = pCharacter->FindParentLine();
	ERROR2IF(pCurrentLine == NULL, NULL, "Character didn't have a parent text line");
	TextLine* pNewLine = NULL;
	if (MoveUp)
		pNewLine = pCurrentLine->FindPrevLine();
	else
		pNewLine = pCurrentLine->FindNextLine();

	// If there is a new line then find the character on it
	VisibleTextNode* pResult = NULL;
	if (pNewLine != NULL)
	{
		// Get the current distance along the line, using the previous vertical move if there was one
		MILLIPOINT CurrentDistance = pCharacter->CalcCharDistAlongLine(ToLeftOfChar);
		if (PreviousVerticalInset.HoldingInset != VERTICALMOVEINSET_NONE &&
			PreviousVerticalInset.LastInset == CurrentDistance &&
			CurrentDistance != -1)
		{
			CurrentDistance = PreviousVerticalInset.HoldingInset;
		}
		else
		{
			PreviousVerticalInset.HoldingInset = VERTICALMOVEINSET_NONE;
		}

		// Get the character that distance along the new line
		if (CurrentDistance != -1)
			pResult = pNewLine->FindCharAtDistAlongLine(CurrentDistance, NewToLeft);

		// Update the vertical move memory
		if (pResult != NULL)
		{
			if (PreviousVerticalInset.HoldingInset != VERTICALMOVEINSET_NONE)
				VertInset.HoldingInset = PreviousVerticalInset.HoldingInset;
			else
				VertInset.HoldingInset = CurrentDistance;
			VertInset.LastInset = pResult->CalcCharDistAlongLine(!*NewToLeft);
		}
	}

	return pResult;
}


/********************************************************************************************
>	VisibleTextNode* OpTextCaret::GetLineStartCharacter(VisibleTextNode* pCharacter)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/04/95
	Inputs:		pCharacter - points to the character to look from
	Returns:	A pointer to the first VisibleTextNode character on the line pCharacter is on,
				NULL if there is isn't one (this is BAD!)
	Purpose:	This function returns a pointer to the VisibleTextNode at the start of a line
********************************************************************************************/

VisibleTextNode* OpTextCaret::GetLineStartCharacter(VisibleTextNode* pCharacter)
{
	ERROR2IF(pCharacter == NULL, NULL, "pCharacter was NULL");
	VisibleTextNode* pResult = NULL;
	
	TextLine* pParent = pCharacter->FindParentLine();
	ERROR2IF(pParent == NULL, NULL, "Character didn't have a parent TextLine");
	pResult = pParent->FindFirstVTN();

	ERROR2IF(pResult == NULL, NULL, "Line didn't have a VTN");

	return pResult;
}


/********************************************************************************************
>	VisibleTextNode* OpTextCaret::GetLineEndCharacter(VisibleTextNode* pCharacter)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/04/95
	Inputs:		pCharacter - points to the character to look from
	Returns:	A pointer to the last VisibleTextNode character on the line pCharacter is on,
				NULL if there is isn't one (this is BAD!)
	Purpose:	This function returns a pointer to the VisibleTextNode at the end of a line,
				this should be an EOLNode.
********************************************************************************************/

VisibleTextNode* OpTextCaret::GetLineEndCharacter(VisibleTextNode* pCharacter)
{
	ERROR2IF(pCharacter == NULL, NULL, "pCharacter was NULL");
	VisibleTextNode* pResult = NULL;
	
	TextLine* pParent = pCharacter->FindParentLine();
	ERROR2IF(pParent == NULL, NULL, "Character didn't have a parent TextLine");
	pResult = pParent->FindLastVTN();

	ERROR2IF(pResult == NULL, NULL, "Line didn't have a VTN");

	return pResult;
}


///////////////////////////////////////////////////////////////////////////////////////////
//  The OpTextSelection Operation

/********************************************************************************************
>	OpTextSelection::OpTextSelection()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/04/95
	Purpose:	Constructor.  Initialises members
********************************************************************************************/

OpTextSelection::OpTextSelection()
{
	StartSpread = NULL;
	pSelectionStory = NULL;
}


/********************************************************************************************
>	BOOL OpTextSelection::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/94
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations
********************************************************************************************/

BOOL OpTextSelection::Init()
{
	return (RegisterOpDescriptor(0,
		 							_R(IDS_OPTEXTSELECTION),
									CC_RUNTIME_CLASS(OpTextSelection),
		 							OPTOKEN_TEXTSELECTION,
		 							OpTextSelection::GetState,
		 							0,							// help ID 
		 							0,							// Bubble help
		 							0,
		 							0,
									SYSTEMBAR_ILLEGAL,			// For now !
									TRUE, 						// Receive messages
									FALSE,
									FALSE,
									0,
									GREY_WHEN_NO_CURRENT_DOC
	       )); 
}               


/********************************************************************************************
>	OpState OpTextSelection::GetState(String_256* Description, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/6/94
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the push tool
	Returns:	The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:	Find out the state of the operation at the specific time
********************************************************************************************/

OpState OpTextSelection::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}


/********************************************************************************************
>	BOOL OpTextSelection::DoDrag( DocCoord Anchor, Spread *pSpread, ClickModifiers ClickMods,
									Node* pClickNode)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/04/95
	Inputs:		Anchor 	- The position of the mouse at the start of the Drag
				pSpread - The spread that the drag started on
				ClickMods are the modifiers at the start of the operaration
				pClickNode - the node the click happened on
	Purpose:	Initially inserts the caret into the TextStory at the position of the click
********************************************************************************************/

BOOL OpTextSelection::DoDrag( DocCoord Anchor, Spread *pSpread, ClickModifiers ClickMods,
									Node* pClickNode)
{
	// Validate clicked character and story
	ERROR2IF(pClickNode == NULL, FALSE, "Pointer to clicked node was NULL!");
	ERROR2IF(!pClickNode->IsAVisibleTextNode(), FALSE, "Clicked node was not a text character");
	VisibleTextNode* pClickChar = (VisibleTextNode*)pClickNode;
	pSelectionStory = (TextStory*)(pClickNode->FindParent(CC_RUNTIME_CLASS(TextStory)));
	ERROR2IF(pSelectionStory == NULL, FALSE, "Clicked character didn't have a parent TextStory");
	pCaret = pSelectionStory->GetCaret();
	ERROR2IF(pCaret == NULL, FALSE, "TextStory didn't have a caret");

	// Record the initial parameters for later use
	StartPoint = Anchor;
	StartSpread	= pSpread;

	BOOL Success = TRUE;

	// Give the mainframe input focus
	DialogManager::DefaultKeyboardFocus();

	if (ClickMods.Adjust && !ClickMods.Constrain)
	{
	 	// SHIFT click to extend selection
		Success = ExtendSelection(pClickChar, Anchor);
		End();
		return Success;
	}
	else
	{
		if (ClickMods.Constrain)
		{
			// CTRL click to select line
			Success = InitialCaretPosition(pClickChar) && SelectLine();
			End();
			return Success;
		}
		else
		{
			// Normal click to position caret and then drag selection
			Success = InitialCaretPosition(pClickChar);
		}
	}

	if (Success)
		Success = StartDrag( DRAGTYPE_AUTOSCROLL );

	return Success;
}


/********************************************************************************************
>	void OpTextSelection::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
								  Spread *pSpread, BOOL bSolidDrag)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/04/95
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods - Which key modifiers are being pressed
				pSpread - The spread that the mouse is moving over
	Purpose:	Adds or removes characters from the text story selection
********************************************************************************************/

void OpTextSelection::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
										  Spread *pSpread, BOOL bSolidDrag)
{
	ERROR3IF(((pSelectionStory == NULL) || (pCaret == NULL)), "A pointer was NULL");
	BOOL Success = TRUE;

	// If drag has moved onto a different spread, convert the coord to be relative to the
	// original spread.
	if (pSpread != StartSpread)
		PointerPos = MakeRelativeToSpread(StartSpread, pSpread, PointerPos);

	// Do different things depending wether you are on a path or not
	VisibleTextNode* pFirstToSel = NULL;
	VisibleTextNode* pLastToSel = NULL;
	BOOL Ignore = FALSE;
	if (pSelectionStory->GetTextPath() != NULL)
		Success = GetNewSelEndsOnPath(PointerPos, &pFirstToSel, &pLastToSel, &Ignore);
	else
		Success = GetNewSelEndsInStory(PointerPos, &pFirstToSel, &pLastToSel);

	// Run through all the characters, selecting and deselecting
	if (!Success || (pFirstToSel == NULL) || (pLastToSel == NULL))
	{
		if ((!pCaret->IsSelected() || (pSelectionStory->GetSelectionEnd() != NULL)) && !Ignore)
		{
			NodeRenderableInk::DeselectAll(TRUE,FALSE);
			pCaret->Select(TRUE);
		}
	}
	else
	{
		OpTextCaret::SelectStoryCharacters(pFirstToSel, pLastToSel, pSelectionStory, pCaret);
	}

	if (!Success)
	{
		InformError();
		FailAndExecute();
		EndDrag();
		End();
	}
}


/********************************************************************************************
>	void OpTextSelection::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods,
							   Spread *pSpread, BOOL Success, BOOL bSolidDrag)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/6/94
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickMods - the key modifiers being pressed
				pSpread - The spread that the drag finished on
				Success - TRUE if the drag was terminated properly, FALSE if it
				was ended with the escape key being pressed
	Purpose:	Handles the drag finishing.  Clears the selection if ESCAPE was pressed
********************************************************************************************/

void OpTextSelection::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods,
							   Spread *pSpread, BOOL Success, BOOL bSolidDrag)
{
	BOOL Worked = TRUE;
	
	// End the Drag
	if (Worked)
		Worked = EndDrag();

	// Select the caret if they pressed ESCAPE to end the drag, or there is no remaing selection
	BOOL Direction;
	if ((!Success) || (pSelectionStory->GetSelectionEnd(&Direction) == NULL) )
	{
		if ((pSelectionStory->GetCaret() != NULL) && !pSelectionStory->GetCaret()->IsSelected())
		{
			NodeRenderableInk::DeselectAll(TRUE,FALSE);
			pSelectionStory->GetCaret()->Select(TRUE);
		}
	}

	GetApplication()->FindSelection()->Update();

	End();
}


/********************************************************************************************
>	BOOL OpTextSelection::GetNewSelEndsOnPath(DocCoord Point, VisibleTextNode** pSelStart,
													VisibleTextNode** pSelEnd, BOOL* pIgnore)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/05/95
	Inputs:		Point - the current pointer position
	Outputs:	pSelStart - points to the first character to select (or NULL)
				pSelEnd - points to the last character to select (or NULL)
				pIgnore - TRUE if the pointer was not over any text
	Returns:	TRUE/FALSE for Success/Failure
	Purpose:	For finding the bounds of the selection when the TextStory is on a path
	Errors:		Checks on the story
	SeeAlso:	OpTextSelection::GetNewSelEndsInStory
********************************************************************************************/

BOOL OpTextSelection::GetNewSelEndsOnPath(DocCoord Point, VisibleTextNode** pSelStart, VisibleTextNode** pSelEnd, BOOL* pIgnore)
{
	// Find the first character in the story
	ERROR2IF(((pSelectionStory == NULL) || (pCaret == NULL)), FALSE, "Story or caret pointer was NULL");
	VisibleTextNode* pFirstChar = pSelectionStory->FindFirstVTN();
	ERROR2IF(pFirstChar == NULL, FALSE, "Selection Story had no characters");

	// Find the character the pointer is currently over
	BOOL Success = TRUE;
	DocRect CharBoundsInDoc;
	DocRect CharBoundsInChar;
	BOOL ToLeftOfCaret = TRUE;
	OpTextSelection::SelectionPos CharSelPos = NOT_NEAR;
	VisibleTextNode* pCurrent = pFirstChar;
	VisibleTextNode* pChar = NULL;
	while ((pCurrent != NULL) && Success)
	{		
		if (pCurrent->IsAnAbstractTextChar())
		{
			// Do a test on the bounds of the character matrix
			Success = ((AbstractTextChar*)pCurrent)->GetMetricsRectBounds(&CharBoundsInDoc);

			// Now test the metric rect of the character
			if (Success)
			{
				CharSelPos = IsClickToLeftHalfOfChar((AbstractTextChar*)pCurrent, Point, FALSE);

				if (CharSelPos != NOT_NEAR)
				{
					pChar = (AbstractTextChar*)pCurrent;
					break;
				}
			}
		}

		if (pCurrent == (VisibleTextNode*)pCaret)
			ToLeftOfCaret = FALSE;

		pCurrent = pCurrent->FindNextVTNInStory();
	}

	// Generate first and last selection characters
	if (Success && (pChar != NULL))
	{
		*pIgnore = FALSE;
		Success = GetNewSelBoundsChars((CharSelPos == TO_LEFT), ToLeftOfCaret, pChar, pSelStart, pSelEnd);
	}
	else
	{
		*pIgnore = TRUE;
		pSelStart = NULL;
		pSelEnd = NULL;
	}

	return Success;
}


/********************************************************************************************
>	BOOL OpTextSelection::GetNewSelEndsInStory(DocCoord Point, VisibleTextNode** pSelStart, VisibleTextNode* pSelEnd)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/05/95
	Inputs:		Point - the current pointer position
	Outputs:	pSelStart - points to the first character to select (or NULL)
				pSelEnd - points to the last character to select (or NULL)
	Returns:	TRUE/FALSE for Success/Failure
	Purpose:	For finding the bounds of the selection when the TextStory is not on a path
	Errors:		Checks on the story
	SeeAlso:	OpTextSelection::GetNewSelEndsOnPath
********************************************************************************************/

BOOL OpTextSelection::GetNewSelEndsInStory(DocCoord Point, VisibleTextNode** pSelStart, VisibleTextNode** pSelEnd)
{
	// Check members
	ERROR2IF(((pSelectionStory == NULL) || (pCaret == NULL)), FALSE, "Story or caret pointer was NULL");

	BOOL ToLeftPosChar = TRUE;
	VisibleTextNode* pPointerChar = NULL;
	if (!pSelectionStory->GetCharacterAtPoint(FALSE, Point, &pPointerChar, &ToLeftPosChar))
		return FALSE;
	ERROR2IF(pPointerChar == NULL, FALSE, "GetCharacterAtPoint failed to find a character");

	// Now scan through the Story to see wether the char is to the left or right of the caret
	VisibleTextNode* pChar = pSelectionStory->FindFirstVTN();
	ERROR2IF(pChar == NULL, FALSE, "Selection Story had no characters");
	BOOL ToLeftOfCaret = TRUE;
	while (pChar != NULL)
	{
		if (pChar == pCaret)
		{
			ToLeftOfCaret = FALSE;
			break;
		}

		if (pChar == pPointerChar)
		{
			break;
		}

		pChar = pChar->FindNextVTNInStory();
	}

	// Generate first and last selection characters
	return GetNewSelBoundsChars(ToLeftPosChar, ToLeftOfCaret, pPointerChar, pSelStart, pSelEnd);
}


/********************************************************************************************
>	BOOL OpTextSelection::GetNewSelBoundsChars( BOOL ToLeftOfChar,
												BOOL ToLeftOfCaret,
												VisibleTextNode* pFoundChar,
												VisibleTextNode** pSelStart,
												VisibleTextNode** pSelEnd)
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/05/95
	Inputs:		ToLeftOfChar - TRUE if the position is to the left of pFoundChar, FALSE to the right
				ToLeftOfCaret - TRUE if pFoundChar comes before the caret in the story
				pFoundChar - the character that is to become one end of the selection,
				the caret is at the other end
	Outputs:	pSelStart - points to the first character to select (or NULL)
				pSelEnd - points to the last character to select (or NULL)
	Returns:	TRUE/FALSE for Success/Failure
	Purpose:	Having found the character that will form the other end of the story selection,
				call this function to find the first and last chars to select.
	Errors:		Assorted checks on pointers
********************************************************************************************/

BOOL OpTextSelection::GetNewSelBoundsChars(BOOL ToLeftOfChar, BOOL ToLeftOfCaret, VisibleTextNode* pFoundChar,
										VisibleTextNode** pSelStart, VisibleTextNode** pSelEnd)
{
	ERROR2IF(((pFoundChar == NULL) || (pCaret == NULL)), FALSE, "NULL Pointer found");
	*pSelStart = NULL;
	*pSelEnd = NULL;

	// Work out wether to include the character in the selection
	if (ToLeftOfCaret)
	{
		// Selection goes from the left/right of pFoundChar to left of caret
		if (ToLeftOfChar)
			*pSelStart = pFoundChar;
		else
		{
			*pSelStart = pFoundChar->FindNextVTNInStory();
			if (*pSelStart == pCaret)
				*pSelStart = NULL;
		}

		*pSelEnd = pCaret->FindPrevVTNInStory();
	}
	else
	{
		// Selection goes from right of caret to left/right of pChar
		if (!ToLeftOfChar)
			*pSelEnd = pFoundChar;
		else
		{
			*pSelEnd = pFoundChar->FindPrevVTNInStory();
			if (*pSelEnd == pCaret)
				*pSelEnd = NULL;
		}

		*pSelStart = pCaret->FindNextVTNInStory();
	}

	return TRUE;
}


/********************************************************************************************
>	BOOL OpTextSelection::InitialCaretPosition(VisibleTextNode* pSelectChar)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/04/95
	Inputs:		pSelectChar - points to the character that was clicked on
	Outputs:	TransStartPoint is made character relative.
	Returns:	TRUE/FALSE for Success/Failure
	Purpose:	The caret is positioned either to the left or right of the character clicked
				upon.
********************************************************************************************/

BOOL OpTextSelection::InitialCaretPosition(VisibleTextNode* pSelectChar)
{
	BOOL Success = TRUE;
	
	// Deselect ALL objects in the tree
	NodeRenderableInk::DeselectAll(TRUE,FALSE);
	if (pSelectionStory!=TextStory::GetFocusStory())
		if (!OpDeleteTextStory::RemoveEmptyFocusStory())
			return FALSE;

	// Position the caret to the left or right of the character, depending on the click
	OpTextSelection::SelectionPos ClickPos = TO_LEFT;
	if (pSelectChar->IsAnAbstractTextChar())
		ClickPos = IsClickToLeftHalfOfChar((AbstractTextChar*)pSelectChar, StartPoint, TRUE);
	ERROR3IF(ClickPos == NOT_NEAR,"Selection point was not near the character claimed");
	if (ClickPos == TO_LEFT)
		pSelectionStory->MoveCaretToCharacter(pSelectChar, PREV);
	if (ClickPos == TO_RIGHT)
		pSelectionStory->MoveCaretToCharacter(pSelectChar, NEXT);

	// Select the new Caret
	TextStory::SetFocusStory(pSelectionStory);
	pSelectionStory->GetCaret()->HasMoved();
	pSelectionStory->GetCaret()->ScrollToShow();
	pSelectionStory->GetCaret()->Select(TRUE);

	return Success;
}																			  


/********************************************************************************************
>	SelectionPos OpTextSelection::IsClickToLeftHalfOfChar(AbstractTextChar* pClickChar,
														DocCoord ClickPoint, BOOL MustChoose)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/04/95
	Inputs:		pClickChar - points to the character clicked on
				ClickPoint - the actual position of the click
				MustChose - TRUE if left or right should be returned, ignoring NOT_NEAR
	Returns:	Wether the click is to the left (TO_LEFT), to the right (TO_RIGHT), or not
				near the character (NOT_NEAR)
	Purpose:	To see if a click is to the left or right of the middle of a character
********************************************************************************************/

OpTextSelection::SelectionPos OpTextSelection::IsClickToLeftHalfOfChar(AbstractTextChar* pClickChar,
															DocCoord ClickPoint, BOOL MustChoose)
{
	ERROR3IF(pSelectionStory == NULL, "pSelectionStory is NULL");
	ERROR3IF(pClickChar == NULL, "pClickChar is NULL");

	SelectionPos Result = NOT_NEAR;

 	if ((pSelectionStory != NULL) && (pClickChar != NULL))
 	{
	 	// Transform the mouse position through the two matrices so it is character relative
		Matrix CharMat(0,0);
		if (pClickChar->GetStoryAndCharMatrix(&CharMat)!=FALSE)
			CharMat=CharMat.Inverse();
		else
			ERROR3("GetStoryAndCharMatrix() failed");

		// Transform the mouse click position into character space
		CharMat.transform(&ClickPoint);

		// Get the character rectangle
		DocRect CharRect;
		if (pClickChar->GetMetricsRect(&CharRect))
		{
			if (CharRect.ContainsCoord(ClickPoint) || MustChoose)
			{
				if (ClickPoint.x > ((CharRect.lo.x+CharRect.hi.x)/2))
					Result = TO_RIGHT;
				else
					Result = TO_LEFT;
			}
		}
		else
			ERROR3("GetMetricsRectInStory failed");
	}

	return Result;
}


/********************************************************************************************
>	BOOL OpTextSelection::SelectLine()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/04/95
	Returns:	TRUE/FALSE for Success/Failure
	Purpose:	The caret is positioned at the start of the line and all AbstractCharacters on
				the line are selected.
	SeeAlso:	OpTextSelection::DoSelectTextLine
********************************************************************************************/

BOOL OpTextSelection::SelectLine()
{
	ERROR2IF(pSelectionStory == NULL, FALSE, "Selection story was NULL");
	CaretNode* pCaret = pSelectionStory->GetCaret();
	ERROR2IF(pCaret == NULL, FALSE, "Selection story didn't have a caret");
	TextLine* pLine = pCaret->FindParentLine();
	ERROR2IF(pLine == NULL, FALSE, "Caret didn't have a parent TextLine");

	// Move the caret to the start of the line
	NodeRenderableInk::DeselectAll(TRUE,FALSE);
	pSelectionStory->MoveCaretToStartOfLine();

	// Get pointer to first and last char in line
	VisibleTextNode* pFirstChar = pCaret->FindNextVTNInStory();
	VisibleTextNode* pLastChar = pLine->FindLastVTN();

	// Select the characters
	OpTextCaret::SelectStoryCharacters(pFirstChar, pLastChar, pSelectionStory, pCaret);

	return TRUE;
}


/********************************************************************************************
>	BOOL OpTextSelection::ExtendSelection(VisibleTextNode* pClickChar)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/04/95
	Inputs:		pClickChar - the character that was clicked on
	Returns:	TRUE/FALSE for Success/Failure
	Purpose:	If the story clicked on was not the focus story the the caret is positioned
				at the click point
				If the story clicked on was the focus story, but there was no text selection
				then a selection is created between the caret and the click point
				If the story clicked on was the focus story and there was no text selection
				the selection is extended, using the caret as the anchor point.
********************************************************************************************/

BOOL OpTextSelection::ExtendSelection(VisibleTextNode* pClickChar, DocCoord ClickPoint)
{
	BOOL Success = TRUE;
	
	// Is the selection story the one with the input focus?
	if (pSelectionStory != TextStory::GetFocusStory())
	{
		Success = InitialCaretPosition(pClickChar);
	}
	else
	{
//		BOOL CurrentForwardsDirection = TRUE;
		BOOL NewForwardsDirection = TRUE;
//		VisibleTextNode* pSelEnd = pSelectionStory->GetSelectionEnd(&CurrentForwardsDirection);
		CaretNode* pCaret = pSelectionStory->GetCaret();
		ERROR2IF(pCaret == NULL, FALSE, "Caret not found");
		
		// Scan from the caret to the right to find the direction of pClickChar
		// BODGE TEXT - should be a formatty thing, rather than scanning chars
		VisibleTextNode* pChar = pCaret->FindNextVTNInStory();
		while ((pChar != NULL) && (pChar != pClickChar))
		{
			pChar = pChar->FindNextVTNInStory();
		}
		if (pChar != NULL)
			NewForwardsDirection = TRUE;		// The clicked char is to the right of the caret
		else
			NewForwardsDirection = FALSE;		// The clicked char is to the left of the caret

		// Get the selection end char
		VisibleTextNode* pNewSelEndChar = pClickChar;
		SelectionPos ClickPos = TO_LEFT;
		if (pClickChar->IsAnAbstractTextChar())
			ClickPos = IsClickToLeftHalfOfChar((AbstractTextChar*)pClickChar, ClickPoint, TRUE);
		if (NewForwardsDirection && (ClickPos==TO_LEFT))
		{
			// The selection goes from the caret to the left of the click char
			pNewSelEndChar = pClickChar->FindPrevVTNInStory();
		}
		if (!NewForwardsDirection && (ClickPos==TO_RIGHT))
		{
			// The selection goes from the caret to the right of the click char
			pNewSelEndChar = pClickChar->FindNextVTNInStory();
		}
		if (pNewSelEndChar->IsACaret())
			pNewSelEndChar = NULL;

		// Get the selection start character
		VisibleTextNode* pSelStartChar = NULL;
		if (NewForwardsDirection)
			pSelStartChar = pCaret->FindNextVTNInStory();
		else
		{
			pSelStartChar = pNewSelEndChar;
			pNewSelEndChar = pCaret->FindPrevVTNInStory();
		}

	  	// Do the selection
		if ((pSelStartChar == NULL) || (pNewSelEndChar == NULL))
		{
			NodeRenderableInk::DeselectAll(TRUE,FALSE);
			pCaret->Select(TRUE);
		}
		else
		{
			OpTextCaret::SelectStoryCharacters(pSelStartChar, pNewSelEndChar, pSelectionStory, pCaret);
			pNewSelEndChar->ScrollToShow();
		}
	}

	GetApplication()->UpdateSelection();
	return Success;
}


/********************************************************************************************
>	BOOL OpTextSelection::DoSelectAllText(TextStory* pStory)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/95
	Inputs:		pStory - points to a text story
	Returns:	TRUE/FALSE for Success/Failure
	Purpose:	Selects all characters in the given story
	Errors:		Checks on story pointer and the caret in the story
	SeeAlso:	OpTextSelection::DoSelectLineText
********************************************************************************************/

BOOL OpTextSelection::DoSelectAllText(TextStory* pStory)
{
	ERROR2IF(pStory == NULL, FALSE, "Story pointer was NULL");
	CaretNode* pCaret = pStory->GetCaret();
	ERROR2IF(pCaret == NULL, FALSE, "Story didn't have a caret");
	BOOL ok = TRUE;

	NodeRenderableInk::DeselectAll(TRUE, FALSE);

	// Move the caret to the start of the text story
	VisibleTextNode* pStartChar = pStory->FindFirstVTN();
	if (ok && (pStartChar != pCaret))
		ok = pStory->MoveCaretToCharacter(pStartChar, PREV);
	
	// Get the first and last char in story
	pStartChar = pStory->FindFirstVTN();
	VisibleTextNode* pEndChar = pStory->FindLastVTN();
	if ((pStartChar == NULL) || (pEndChar == NULL))
		ok = FALSE;
	
	if (ok)
	{
		OpTextCaret::SelectStoryCharacters(pStartChar, pEndChar, pStory, pCaret);
	}
	else
	{
		FailAndExecute();
		InformError();
	}

	End();
	
	return ok;
}


/********************************************************************************************
>	BOOL OpTextSelection::DoSelectLineText()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/95
	Inputs:		-
	Returns:	TRUE/FALSE for Success/Failure
	Purpose:	Selects all VTN's in the given text line, clearing all other selections
	Errors:		Check on line pointer
	SeeAlso:	OpTextSelection::DoSelectAllText
********************************************************************************************/

BOOL OpTextSelection::DoSelectLineText()
{
	// Set pointers
	pSelectionStory = TextStory::GetFocusStory();;
	ERROR2IF(pSelectionStory == NULL, FALSE, "No focus text story");
	pCaret = pSelectionStory->GetCaret();
	ERROR2IF(pCaret == NULL, FALSE, "TextStory didn't have a caret");

	BOOL ok = SelectLine();

	if (!ok)			   
	{
		FailAndExecute();
		InformError();
	}

	End();
	
	return ok;
}


/********************************************************************************************
>	BOOL OpDeleteTextStory::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/04/95
	Returns:	TRUE/FALSE for Success/Failure
	Purpose:	Initialalises the DeleteATextStory operation
********************************************************************************************/

BOOL OpDeleteTextStory::Init()
{
	return RegisterOpDescriptor(0,
 								_R(IDS_DELETETEXTSTORY),
								CC_RUNTIME_CLASS(OpDeleteTextStory),
 								OPTOKEN_DELETESTORY,
 								OpDeleteTextStory::GetState);
}               


/********************************************************************************************
>	OpState	OpDeleteTextStory::GetState(String_256*, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/04/95
	Returns:	The state of the OpDeleteTextStory
	Purpose:	For finding OpDeleteTextStory's state. Always available
********************************************************************************************/

OpState	OpDeleteTextStory::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;

	return OpSt;
}



/********************************************************************************************
>	OpState	OpDeleteTextStory::GetState(DoWithParam(OpDescriptor* pOpDesc, OpParam* pParam))

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/04/95
	Inputs:		pOpDesc - points to the OpDescriptor (unused)
				pParam - Param1 points to the TextStory to delete 
	Purpose:	For finding OpDeleteTextStory's state. Always available
********************************************************************************************/

void OpDeleteTextStory::DoWithParam(OpDescriptor* pOpDesc, OpParam* pParam)
{
	TextStory* pStory = (TextStory*) (void *) pParam->Param1;
	if (pStory == NULL)
	{
		ERROR3("Story pointer was NULL");
		return;
	}
	ERROR3IF(!IS_A(pStory,TextStory),"Story pointer didn't point to a TextStory!");

	BOOL ok = DoStartSelOp(TRUE, TRUE);

	if (ok)
		ok = DoActions(this, pStory);

	if (ok)
	{
		ObjChangeParam ObjChange(OBJCHANGE_FINISHED,ObjChangeFlags(TRUE), pStory, this);
		ok = UpdateChangedNodes(&ObjChange);
	}
		
	if (!ok)
	{
		FailAndExecute();
		InformError();
	}

	End();
}


/********************************************************************************************
>	static BOOL OpDeleteTextStory::DoActions(UndoableOperation* pOp, TextStory* pEmptyStory)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/05/95
	Inputs:		pOp - points to the current operation
				pEmptyStory - points to the text story to delete
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Hides an empty text story, inserting the required undo actions into the operation
	Errors:		Checks on the arguments
********************************************************************************************/

BOOL OpDeleteTextStory::DoActions(UndoableOperation* pOp, TextStory* pEmptyStory)
{
	ERROR2IF(pEmptyStory == NULL, FALSE, "Story pointer was NULL");

	BOOL ok = TRUE;

	ObjChangeParam ObjDeleteChange(OBJCHANGE_STARTING, ObjChangeFlags(TRUE), pEmptyStory, pOp);
	if (ok && pEmptyStory->AllowOp(&ObjDeleteChange))
	{
		if (pEmptyStory->GetCaret() != NULL)
			pEmptyStory->GetCaret()->DeSelect(TRUE);

		if (ok)
			ok = pOp->DoInvalidateNodeRegion(pEmptyStory, TRUE, FALSE);

		// If the story is on a path then we want the path to remain
		if (ok && (pEmptyStory->GetTextPath() != NULL))
		{
			NodePath* pTextPath = pEmptyStory->GetTextPath();

			//Localise attributes of the story
			if (ok)	ok = pOp->DoLocaliseCommonAttributes(pEmptyStory); 
			if (ok) ok = pOp->DoMoveNode(pTextPath,pEmptyStory,NEXT);
			if (ok) pTextPath->DeSelect(FALSE);

			// Factor back out the common attributes
			if (ok)
				ok = pOp->DoFactorOutCommonChildAttributes(pEmptyStory);
		}

		// Hide the story
		if (ok)
			ok = pOp->DoHideNode(pEmptyStory, TRUE);

		if (ok && (TextStory::GetFocusStory() == pEmptyStory))
			TextStory::SetFocusStory(NULL);

		GetApplication()->UpdateSelection();
	}

	return ok;
}


/********************************************************************************************
>	static BOOL OpDeleteTextStory::RemoveEmptyFocusStory(UndoableOperation* pOp = NULL)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/95
	Inputs:		pOp - points to the current operation (if any)
	Returns:	TRUE/FALSE for success/failure
	Purpose:	If the focus story has no characters in it and the caret in it is not selected
				then the story is hidden and the focus story is cleared.
********************************************************************************************/

BOOL OpDeleteTextStory::RemoveEmptyFocusStory(UndoableOperation* pOp)
{
	BOOL ok = TRUE;
	TextStory* pTS = TextStory::GetFocusStory();
	CaretNode* pCaret = NULL;
	if (pTS != NULL)
		pCaret = pTS->GetCaret();
	if ((pTS != NULL) && (pCaret != NULL))
	{
		// Get a pointer to the first character in the story
		VisibleTextNode* pFirstChar = pTS->FindFirstVTN();
		if (pFirstChar == NULL)
			InformError();
		else
		{
			if (!IS_A(pFirstChar,TextChar))
				pFirstChar = pFirstChar->FindNextTextCharInStory();
		}

		// Remove the story if it contains no text chars
		if (pFirstChar == NULL)
		{
			// If no op is running then invoke on to do the delete, otherwise append delete actions to running op 
			if (pOp == NULL)
			{
				OpDescriptor* OpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpDeleteTextStory));
				if (OpDesc != NULL)
				{
					OpParam p((void*)pTS,0);
					OpDesc->Invoke(&p);
				}
				else
					ok = FALSE;
			}
			else
				ok = OpDeleteTextStory::DoActions(pOp, pTS);
		}
	}

	return ok;
}

void OpDeleteTextStory::PerformMergeProcessing()
{
	MergeWithPrevious();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Justification operations

/********************************************************************************************
>	static BOOL OpApplyJustificationToStory::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/95
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Initialises all the justification options on the TextStory pop-up menu
********************************************************************************************/

BOOL OpApplyJustificationToStory::Init()
{
	BOOL ok = TRUE;

	ok = ok && RegisterOpDescriptor(0,
 									_R(IDS_APPLYLEFTJUSTIFY),
									CC_RUNTIME_CLASS(OpApplyLeftJustifyToStory),
 									OPTOKEN_APPLYLEFTJUSTIFY,
		 							OpApplyJustificationToStory::GetState);
	ok = ok && RegisterOpDescriptor(0,
 									_R(IDS_APPLYCENTREJUSTIFY),
									CC_RUNTIME_CLASS(OpApplyCentreJustifyToStory),
 									OPTOKEN_APPLYCENTREJUSTIFY,
		 							OpApplyJustificationToStory::GetState);
	ok = ok && RegisterOpDescriptor(0,
 									_R(IDS_APPLYRIGHTJUSTIFY),
									CC_RUNTIME_CLASS(OpApplyRightJustifyToStory),
 									OPTOKEN_APPLYRIGHTJUSTIFY,
		 							OpApplyJustificationToStory::GetState);
	ok = ok && RegisterOpDescriptor(0,
 									_R(IDS_APPLYFULLJUSTIFY),
									CC_RUNTIME_CLASS(OpApplyFullJustifyToStory),
 									OPTOKEN_APPLYFULLJUSTIFY,
		 							OpApplyJustificationToStory::GetState);

	return ok;
}


/********************************************************************************************
>	static OpState	OpApplyJustificationToStory::GetState(String_256*, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/95
	Returns:	Tick/Grey state for the op
	Purpose:	Returns the greying state of the operation, greyed if there are no text
				stories selected.
********************************************************************************************/

OpState OpApplyJustificationToStory::GetState(String_256* pUIDesc, OpDescriptor* pOpDesc)
{
	OpState OpSt;

	SelRange* pSelection = GetApplication()->FindSelection();
	Node* pNode = pSelection->FindFirst();
	
	// Search the SelRange for a selected TextStory
	BOOL FoundSelStory = FALSE;
	while (pNode != NULL)
	{
		if (IS_A(pNode, TextStory))
		{
			FoundSelStory = TRUE;
			break;
		}

		pNode = pSelection->FindNext(pNode);
	}

	OpSt.Greyed = !FoundSelStory;

	return OpSt;
}		


/********************************************************************************************
>	virtual void OpApplyLeftJustifyToStory::Do(OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/95
	Inputs:		OpDesc - unused
	Purpose:	Applies left justification to selected text stories.
********************************************************************************************/

void OpApplyLeftJustifyToStory::Do(OpDescriptor* pOpDesc)
{
	AttrTxtJustification* ApplyAttrib = new AttrTxtJustification();

	if (ApplyAttrib != NULL)
	{
		ApplyAttrib->Value.justification = JLEFT;
		AttributeManager::AttributeSelected(ApplyAttrib, NULL);
	}
}


/********************************************************************************************
>	virtual void OpApplyCentreJustifyToStory::Do(OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/95
	Inputs:		OpDesc - unused
	Purpose:	Applies centre justification to selected text stories.
********************************************************************************************/

void OpApplyCentreJustifyToStory::Do(OpDescriptor* pOpDesc)
{
	AttrTxtJustification* ApplyAttrib = new AttrTxtJustification();

	if (ApplyAttrib != NULL)
	{
		ApplyAttrib->Value.justification = JCENTRE;
		AttributeManager::AttributeSelected(ApplyAttrib, NULL);
	}
}


/********************************************************************************************
>	virtual void OpApplyRightJustifyToStory::Do(OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/95
	Inputs:		OpDesc - unused
	Purpose:	Applies right justification to selected text stories.
********************************************************************************************/

void OpApplyRightJustifyToStory::Do(OpDescriptor* pOpDesc)
{
	AttrTxtJustification* ApplyAttrib = new AttrTxtJustification();

	if (ApplyAttrib != NULL)
	{
		ApplyAttrib->Value.justification = JRIGHT;
		AttributeManager::AttributeSelected(ApplyAttrib, NULL);
	}
}


/********************************************************************************************
>	virtual void OpApplyFullJustifyToStory::Do(OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/95
	Inputs:		OpDesc - unused
	Purpose:	Applies full justification to selected text stories.
********************************************************************************************/

void OpApplyFullJustifyToStory::Do(OpDescriptor* pOpDesc)
{
	AttrTxtJustification* ApplyAttrib = new AttrTxtJustification();

	if (ApplyAttrib != NULL)
	{
		ApplyAttrib->Value.justification = JFULL;
		AttributeManager::AttributeSelected(ApplyAttrib, NULL);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////
// OpTextPaste

/********************************************************************************************
>	OpTextPaste::OpTextPaste() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/05/95
	Purpose:	Constructor for paste text operation
********************************************************************************************/

OpTextPaste::OpTextPaste()
{
	// Dummy constructor
}


/********************************************************************************************
>	BOOL OpTextPaste::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/01/95
	Returns:	TRUE if the op is correctly registered.
	Purpose:	Registers the specilised text paste operation
********************************************************************************************/

BOOL OpTextPaste::Init()
{
	return RegisterOpDescriptor(0, 
								_R(IDS_TEXTPASTEOP),
								CC_RUNTIME_CLASS(OpTextPaste),
								OPTOKEN_TEXTPASTE, 
								OpTextPaste::GetState,
								0,
								_R(IDBBL_TEXTPASTEOP));
}


/********************************************************************************************
>	OpState OpTextPaste::GetState(String_256*, OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Returns:	Returns a default OpState.
	Purpose:	Controls whether the text paste operation is available or not.
				Currently, is always available.
********************************************************************************************/

OpState OpTextPaste::GetState(String_256*, OpDescriptor*)
{
	OpState os;
	return os;
}


/********************************************************************************************
> void OpTextPaste::Do(OpDescriptor* pOpDesc)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	21/7/96
	Inputs:		pOpDesc - unused
	Purpose:	see OpTextPaste::DoPasteTextHelper()
********************************************************************************************/

void OpTextPaste::Do(OpDescriptor*)
{
	BeginSlowJob();
	if (!DoPasteTextHelper())
		FailAndExecute();
	End();
}


/********************************************************************************************
> BOOL OpTextPaste::DoPasteTextHelper()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	21/7/96
	Returns:	FALSE if fails
	Purpose:	If there is a single TextStory on clipboard and a focus story in the doc,
				the story on the clipboard is pasted at caret in focus story,
				else the normal paste method is used
********************************************************************************************/

BOOL OpTextPaste::DoPasteTextHelper()
{
	// get a ptr to the clipbaord and prepare it for a paste
	BOOL ExternalData = FALSE;
	if (!InternalClipboard::PrepareForPaste(&ExternalData)) return FALSE;

	// determine how many objects and text stories on the clipboard, keeping ptr to 1st text story
	INT32 NumObjects = 0;
	INT32 NumStories = 0;
	TextStory* pFirstStory = NULL;
	Node* pNode = InternalClipboard::GetInsertionLayer()->FindFirstChild();
	while (pNode!=NULL)
	{
		NumObjects += 1;
		if (IS_A(pNode,TextStory))
		{
			NumStories += 1;
			if (pFirstStory==NULL)
				pFirstStory = (TextStory*)pNode;
		}
		pNode = pNode->FindNext();
	}

	// if no focus story or not a single text story on the clipboard, do a normal paste op
	TextStory* pFocusStory = TextStory::GetFocusStory();
	if (NumStories!=1 || NumObjects!=1 || pFocusStory==NULL)
		return DoPasteStandard(FALSE,ExternalData);

	if (!DoStartSelOp(TRUE,TRUE))  
		return FALSE;

	// create a range of selected nodes in the focus story and call AllowOp() on them
	VisibleTextNode* pFirstVTN = pFocusStory->FindFirstVTN();
	VisibleTextNode* pLastVTN  = pFocusStory->FindLastVTN();
	ERROR2IF(pFirstVTN==NULL || pLastVTN==NULL, FALSE,"OpTextPaste::PasteTextHelper() - Focus story has no VTN");
	Range pFocusStorySelection(pFirstVTN,pLastVTN,RangeControl(TRUE));
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,ObjChangeFlags(),NULL,this);
	if (!pFocusStorySelection.AllowOp(&ObjChange))
		//ERROR2(FALSE,"OpTextPaste::PasteTextHelper() - op should have been greyed!");
		return DoPasteStandard(FALSE,ExternalData); // doesn't allow me to paste in - but we need to do something! so we do a general paste to app.

	String_256 MasterText = pFocusStory->GetStoryAsString();

	// delete any selected text
	if (!pFocusStory->DeleteSelectedText(this))
		return FALSE;

	// After op ends need to inform all DocComponents in dest doc of the outcome
	BaseDocument* pDoc = pFocusStory->FindOwnerDoc();
	ERROR2IF(pDoc==NULL,FALSE,"OpTextPaste::PasteTextHelper() - Focus story has no parent document!");

	// Ensure that EndComponentCopy and/or AbortComponentCopy will be called when the Op ends
	InformDocComponentsOfOperationsOutcome(pDoc); 

	// paste story into focus story at the caret, ensuring dest doc component data up to date
	BOOL ok = TRUE;
	CALL_WITH_FAIL((pDoc->StartComponentCopy()),this, ok)
	if (!ok) return FALSE;
	Range PasteRange;
	if (!DoPasteText(pFirstStory,pFocusStory,&PasteRange))
		return FALSE;
	CALL_WITH_FAIL((PasteRange.CopyComponentDataToDoc(InternalClipboard::Instance(), pDoc)), this, ok);
	if (!ok) return FALSE;
	
	// inform the clipboard the paste has finished
	if (!InternalClipboard::PasteCompleted()) return FALSE;

	CaretNode* pCaret = pFocusStory->GetCaret();
	if (!DoSelectNode(pCaret,NULL)) return FALSE;

	// update other textstories that are dependant on this one
	SliceHelper::OnTextStoryChanged(pFocusStory, this, &ObjChange, MasterText);
		
	// Update all the changed nodes, i.e. tell all the parents of the children that have been effected
	ObjChange.Define(OBJCHANGE_FINISHED,ObjChangeFlags(),NULL,this);
	if (!UpdateChangedNodes(&ObjChange))
		return FALSE;

	// Make sure the caret is still in view
	pCaret->ScrollToShow();
	return TRUE;
}


/********************************************************************************************
>	BOOL OpTextPaste::DoPasteText(TextStory *pPasteStory, TextStory *pFocusStory, Range* pPasteRange)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	21/7/96
	Inputs:		pPasteStory - story to paste
				pFocusStory - story to paste into
	Outputs:	pPasteRange - Range of nodes pasted into focus story
	Returns:	FALSE if fails
	Purpose:	Paste a story into the Focus story at the caret
********************************************************************************************/

BOOL OpTextPaste::DoPasteText(TextStory* pPasteStory, TextStory* pFocusStory, Range* pPasteRange)
{
	BaseDocument* pDoc = pFocusStory->FindOwnerDoc();
	ERROR2IF(pPasteStory==NULL, FALSE, "Null paste story passed to PasteText()");
	ERROR2IF(pFocusStory==NULL, FALSE, "Null focus story passed to PasteText()");
	UndoableOperation* pUndoOp = this;

	// get some useful pointers ...
	CaretNode* pCaret = pFocusStory->GetCaret();
	ERROR2IF(pCaret==NULL,FALSE,"OpTextPaste::DoPasteText() - focus story has no caret!");
	TextLine* pCaretLine = pCaret->FindParentLine();
	ERROR2IF(pCaretLine==NULL,FALSE,"OpTextPaste::DoPasteText() - caret in dest story has no parent TextLine");
	TextLine* pFirstSrcLine = pPasteStory->FindFirstLine();
	ERROR2IF(pFirstSrcLine==NULL,FALSE,"OpTextPaste::DoPasteText() - story to paste has no lines");
	VisibleTextNode* pCaretLineLastVTN = pCaretLine->FindLastVTN();
	ERROR2IF(pCaretLineLastVTN==NULL,FALSE,"OpTextPaste::DoPasteText() - Caret line has no VTN!");
	
	// get the set of non line level attrs on both stories
	AttrTypeSet NonLineLevelAttrs;
	BOOL    ok = pPasteStory->AddNonLineLevelDescendantAttrsToSet(&NonLineLevelAttrs);
	if (ok) ok = pFocusStory->AddNonLineLevelDescendantAttrsToSet(&NonLineLevelAttrs);

	// ptrs to create a range of node pasted
	VisibleTextNode* pFirstPastedVTN = NULL;
	VisibleTextNode* pLastPastedVTN  = NULL;

	// copy all VTN on the first line to be pasted before the caret, noting the first copied
	// - Localise/FactorOut to/from chars global as attrs on whole story may change
	// - Localise/FactorOut to/from chars excludes LineLevelAttrs ensuring LLA merged correctly
	// - Localise/FactorOut to/from chars on clipboard not done undoably
	if (ok) ok = pCaretLine->DoLocaliseCommonAttributes(pUndoOp,FALSE,TRUE,&NonLineLevelAttrs);
	if (ok) ok = pFirstSrcLine->DoLocaliseCommonAttributes(NULL,FALSE,TRUE,&NonLineLevelAttrs);
	VisibleTextNode* pSrcVTN = pFirstSrcLine->FindFirstVTN();
	while (ok && pSrcVTN!=NULL)
	{
		if (!pSrcVTN->IsACaret())	// don't paste caret
		{
			VisibleTextNode* pVTNCopy = NULL;
			CALL_WITH_FAIL(pSrcVTN->NodeCopy((Node**)&pVTNCopy), pUndoOp, ok);
			if (ok) pVTNCopy->ClearSelection(TRUE);		// ensure new node not selected
			if (ok) ok = pVTNCopy->DoInsertNewNode(pUndoOp,pCaret,PREV);

			if (ok)
			{
				// Ensure component references are up to date /before/ we start to optimise
				// attributes!
				Node* pNode = pVTNCopy->FindFirstDepthFirst();
				while (ok && pNode && pNode!=pVTNCopy)
				{
					if (pNode->IsRenderable())
						ok = ((NodeRenderable*)pNode)->CopyComponentData(InternalClipboard::Instance(), pDoc);

					pNode = pNode->FindNextDepthFirst(pVTNCopy);
				}
				if (!ok)
					pDoc->AbortComponentCopy(); // Cancel all data which has been copied
			}

#ifdef PHIL_ATTRS_FIX
			// Apply any missing attributes to this node (leaving original copies alone)
			if (ok) ok = pCaret->DoApplyAttrsTo(pUndoOp, pVTNCopy, FALSE);
#endif

			if (pFirstPastedVTN==NULL)
				pFirstPastedVTN = pVTNCopy;
		}
		pSrcVTN = pSrcVTN->FindNextVTNInLine();
	}
	if (ok) ok = pFirstSrcLine->DoFactorOutCommonChildAttributes(NULL,TRUE,&NonLineLevelAttrs);
	if (ok) ok = pCaretLine->DoFactorOutCommonChildAttributes(pUndoOp,TRUE,&NonLineLevelAttrs);

	// then move all nodes from caret to last VTN on line to a new line after the caret line
	// - Localise/FactorOut to/from chars local as attrs on story unchanged	(only moving nodes)
	// - Localise/FactorOut to/from chars includes LineLevelAttrs as new line needs them
	TextLine* pNewLine = NULL;
	if (ok) pNewLine = new TextLine;
	if (ok) ok = (pNewLine!=NULL);
	if (ok) ok = pNewLine->DoInsertNewNode(pUndoOp,pCaretLine,NEXT);
	if (ok) ok = pCaretLine->DoLocaliseCommonAttributes(pUndoOp,FALSE,FALSE,NULL);
	if (ok) ok = pCaret->DoMoveNodes(pUndoOp,pCaretLineLastVTN,pNewLine,FIRSTCHILD);
	if (ok) ok = pCaretLine->DoFactorOutCommonChildAttributes(pUndoOp,FALSE,NULL);
	if (ok) ok = pNewLine->DoFactorOutCommonChildAttributes(pUndoOp,FALSE,NULL);
	pCaretLine = pNewLine;

	// insert all remaining lines to be pasted before the (new) caret line
	// - Localise/FactorOut to/from lines global as attrs on whole story may change (inc LLA)
	// - Localise/FactorOut to/from lines on clipboard not done undoably
	TextLine* pSrcLine = pFirstSrcLine->FindNextLine();
	if (pSrcLine!=NULL)
	{
		if (ok) ok = pFocusStory->DoLocaliseCommonAttributes(pUndoOp,FALSE,TRUE,NULL);
		if (ok) ok = pPasteStory->DoLocaliseCommonAttributes(NULL,FALSE,TRUE,NULL);
		while (ok && pSrcLine!=NULL)
		{
			TextLine* pLineCopy = NULL;
			CALL_WITH_FAIL(pSrcLine->NodeCopy((Node**)&pLineCopy), pUndoOp, ok);
			pLineCopy->ClearSelection(TRUE);		// ensure new node not selected

#ifndef PHIL_ATTRS_FIX
			CaretNode* pCaret = ((TextLine*)pLineCopy)->FindCaret();
			if (pCaret!=NULL)	// remove any caret pasted
			{
				pCaret->CascadeDelete();
				delete pCaret;
			}
			if (ok) ok = pLineCopy->DoInsertNewNode(pUndoOp,pCaretLine,PREV);
#else
			CaretNode* pCopyCaret = ((TextLine*)pLineCopy)->FindCaret();
			if (pCopyCaret!=NULL)	// remove any caret pasted
			{
				pCopyCaret->CascadeDelete();
				delete pCopyCaret;
				pCopyCaret = NULL;
			}
			if (ok) ok = pLineCopy->DoInsertNewNode(pUndoOp,pCaretLine,PREV);

			// Apply any missing attributes to the nodes in this line (leaving original copies alone)
			VisibleTextNode* pLineNode = pLineCopy->FindFirstVTN();
			if (ok) ok = pLineCopy->DoLocaliseCommonAttributes(pUndoOp,FALSE,FALSE,NULL);
			if (ok) ok = pCaretLine->DoLocaliseCommonAttributes(pUndoOp,FALSE,FALSE,NULL);
			while (ok && pLineNode)
			{
				if (ok) ok = pCaret->DoApplyAttrsTo(pUndoOp, pLineNode, FALSE);
				pLineNode = pLineNode->FindNextVTNInLine();
			}
			if (ok) ok = pCaretLine->DoFactorOutCommonChildAttributes(pUndoOp,FALSE,NULL);
			if (ok) ok = pLineCopy->DoFactorOutCommonChildAttributes(pUndoOp,FALSE,NULL);
#endif
			pSrcLine = pSrcLine->FindNextLine();
		}
		if (ok) ok = pFocusStory->DoFactorOutCommonChildAttributes(pUndoOp,TRUE,NULL);
		if (ok) ok = pPasteStory->DoFactorOutCommonChildAttributes(NULL,TRUE,NULL);
	}

	// if last EOL pasted virtual, delete it and append (new) caret line to prev line
	// - Localise/FactorOut to/from chars local as attrs on story unchanged	(only moving nodes)
	// - Localise/FactorOut to/from chars excludes LineLevelAttrs ensuring LLA merged correctly
	if (ok)
	{
		TextLine* pLastPastedLine = pCaretLine->FindPrevLine();
		ERROR2IF(pLastPastedLine==NULL,FALSE,"OpTextPaste::DoPasteText() - could not find last line pasted!");
		EOLNode* pLastPastedEOL = pLastPastedLine->FindEOLNode();
		ERROR2IF(pLastPastedEOL==NULL,FALSE,"OpTextPaste::DoPasteText() - story to paste has no final EOL");
		pLastPastedVTN = pLastPastedEOL;
		if (pLastPastedEOL->IsVirtual())
		{
			pLastPastedVTN = pLastPastedVTN->FindPrevVTNInStory();
			ERROR2IF(pLastPastedVTN==NULL,FALSE,"OpTextPaste::DoPasteText() - no chars on pasted line which ended in a virtual EOL");
			if (ok) ok = pLastPastedLine->DoLocaliseCommonAttributes(pUndoOp,FALSE,FALSE,&NonLineLevelAttrs);
			if (ok) ok =      pCaretLine->DoLocaliseCommonAttributes(pUndoOp,FALSE,FALSE,&NonLineLevelAttrs);
			if (ok) ok = pLastPastedEOL->DoHideNode(pUndoOp);
			if (ok) ok = pCaret->DoMoveNodes(pUndoOp,pCaretLineLastVTN,pLastPastedLine,LASTCHILD);
			if (ok) ok = pCaretLine->DoHideNode(pUndoOp);	// delete the empty line
			if (ok) ok = pLastPastedLine->DoFactorOutCommonChildAttributes(pUndoOp,FALSE,&NonLineLevelAttrs);
		}
	}

	// create a range of nodes which have been pasted
	if (ok)
	{
		Range r(pFirstPastedVTN, pLastPastedVTN, RangeControl(TRUE,TRUE,FALSE));
		*pPasteRange = r;
	}
	return ok;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// OpTogglePrintTextAsShapes

/********************************************************************************************
>	BOOL OpTogglePrintTextAsShapes::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/01/95
	Returns:	TRUE if the op is correctly registered.
	Purpose:	Registers this op
********************************************************************************************/

BOOL OpTogglePrintTextAsShapes::Init()
{
	return RegisterOpDescriptor(0, 
								_R(IDS_OPTOGGLEPRINTTEXTASCURVES),
								CC_RUNTIME_CLASS(OpTogglePrintTextAsShapes),
								OPTOKEN_TOGGLEPRINTASSHAPES, 
								OpTogglePrintTextAsShapes::GetState);
}


/********************************************************************************************
>	OpState OpTogglePrintTextAsShapes::GetState(String_256*, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/05/95
	Inputs:		Unused
	Returns:	Returns the state of the operation, greying and ticking
	Purpose:	The state of the operation
********************************************************************************************/

OpState OpTogglePrintTextAsShapes::GetState(String_256*, OpDescriptor*)
{
	OpState os;

	SelRange* pSelection = GetApplication()->FindSelection();
	TextStory* pStory = (TextStory*)pSelection->FindFirstObject(CC_RUNTIME_CLASS(TextStory));

	if (pStory != NULL)
	{
		os.Greyed = FALSE;
		os.Ticked = pStory->IsPrintingAsShapes();
	}
	else
	{
		os.Greyed = TRUE;
		os.Ticked = FALSE;
	}

	return os;
}


/********************************************************************************************
>	virtual void OpTogglePrintTextAsShapes::Do(OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/05/95
	Inputs:		Unused
	Purpose:	Toggles the state of PrintAsShapes flag of the first selected TextStory, and
				sets the state of the remaining selected stories to the same.
********************************************************************************************/

void OpTogglePrintTextAsShapes::Do(OpDescriptor*)
{
	SelRange* pSelection = GetApplication()->FindSelection();
	TextStory* pStory = (TextStory*)pSelection->FindFirstObject(CC_RUNTIME_CLASS(TextStory));

	if (pStory != NULL)
	{
		BOOL NewState = !pStory->IsPrintingAsShapes();

		while (pStory != NULL)
		{
			pStory->SetPrintingAsShapes(NewState);

			pStory = (TextStory*)pSelection->FindNextObject(CC_RUNTIME_CLASS(TextStory), pStory);
		}
	}
}


/********************************************************************************************
>	OpDragStoryIndent::OpDragStoryIndent()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95
	Purpose:	Constructor - inits member variables
********************************************************************************************/

OpDragStoryIndent::OpDragStoryIndent()
{
	mpStartSpread = NULL;
	mpDragStory = NULL;
	mpStoryPath = NULL;
}


/********************************************************************************************
>	OpDragStoryIndent::~OpDragStoryIndent()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95
	Purpose:	Destructor
********************************************************************************************/

OpDragStoryIndent::~OpDragStoryIndent()
{

}


/********************************************************************************************
>	OpState OpDragStoryIndent::GetState(String_256* Description, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95
	Inputs:		unused
	Returns:	Current state of the operation
	Purpose:	Get state function.  Never ticked, Never greyed.
********************************************************************************************/

OpState OpDragStoryIndent::GetState(String_256* Description, OpDescriptor*)
{
	OpState apple;
	return apple;
}


/********************************************************************************************
>	BOOL OpDragStoryIndent::DoDrag( DocCoord Anchor, Spread* pSpread, ClickModifiers ClickMods, TextStory* pStory)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95
	Inputs:		Anchor - the start point of the drag
				pSpread - the spread Anchor is on
				ClickMods - the click modifiers at the start of the drag
				pStory - pointer to the text story to edit.
	Returns:	TRUE if the drag started ok, FALSE if an error occured
	Purpose:	Starts a drag operation to move the path indent position for a text story
********************************************************************************************/
BOOL OpDragStoryIndent::DoDrag( DocCoord Anchor, Spread* pSpread, ClickModifiers ClickMods, TextStory* pStory)
{
	ERROR2IF((pSpread == NULL) || (pStory == NULL), FALSE, "NULL entry param");
	ERROR3IF(pSpread != pStory->FindParentSpread(), "Spread pointer didn't point to the spread the story was on. Is this bad?");
	BOOL ok = TRUE;

	// Record the initial parameters for later use
	mStartPoint = Anchor;
	mpStartSpread = pSpread;
	mpDragStory = pStory;
 	mpStoryPath = pStory->GetTextPath();

	// Get the initial indent along the line
	if (ok)
	{
		SetCurrentPos(mStartPoint, ClickMods);
		mStartPoint = mCurrentBlobPos;
	}

	// Put the drag blobs on for the first time
	DocRect Empty;
	Empty.MakeEmpty();
	RenderDragBlobs(Empty, mpStartSpread, FALSE);
	
	// Start the drag going
	if (ok)
		ok = StartDrag( DRAGTYPE_AUTOSCROLL );

	return ok;
}


/********************************************************************************************
>	virtual void OpDragStoryIndent::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread *pSpread, BOOL bSolidDrag)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95
	Inputs:		PointerPos - the new mouse position
				ClickMods - the current click modifiers
				pSpread - the spread the mouse position is on
	Purpose:	Starts a drag operation to move the path indent position for a text story
********************************************************************************************/
void OpDragStoryIndent::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread *pSpread, BOOL bSolidDrag)
{
	// Salary check
	if ((mpStartSpread == NULL) || (mpDragStory == NULL))
	{
		ERROR3("A pointer was NULL");
		return;
	}

	// If drag has moved onto a different spread, convert the coord to be relative to the original spread.
	if (pSpread != mpStartSpread)
		PointerPos = MakeRelativeToSpread(mpStartSpread, pSpread, PointerPos);

	// Take the previous blobs off
	DocRect Empty;
	Empty.MakeEmpty();
	RenderDraggingBlobs(Empty, mpStartSpread);

	// Get the new distance along the path
	SetCurrentPos(PointerPos, ClickMods);

	// Put the blob back on
	RenderDraggingBlobs(Empty, mpStartSpread);

	// Set the status line to be helpful
	if (mpStoryPath==NULL)
	{
		String_256 HelpString(_R(IDS_TEXTTOOL_DRAGSTORYHANDLE));
		GetApplication()->UpdateStatusBarText(&HelpString);
	}
}


/********************************************************************************************
>	virtual void OpDragStoryIndent::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, Spread *pSpread, BOOL Success, BOOL bSolidDrag)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95
	Inputs:		PointerPos - the mouse position at the end of the drag
				ClickMods - the current click modifiers
				pSpread - the spread the mouse position is on
				Success - FALSE if the drag was aborted by the user
	Purpose:	Completes the drag operation, stores the new path indent in the story
********************************************************************************************/
void OpDragStoryIndent::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, Spread *pSpread, BOOL Success, BOOL bSolidDrag)
{
	BOOL ok = TRUE;

	// Take the drag blob off
	DocRect Empty;
	Empty.MakeEmpty();
	RenderDragBlobs(Empty, mpStartSpread, bSolidDrag);

	BeginSlowJob();
	EndDrag();

	if (Success)
	{
		if (ok)
			ok = DoStartTextOp(mpDragStory, TRUE, TRUE);

		// Ask the story if it minds being altered (also does reformatting for us)
		if (ok)
		{
			ObjChangeParam ObjInsert(OBJCHANGE_STARTING, ObjChangeFlags(), NULL, this);
			ok = mpDragStory->AllowOp(&ObjInsert);
		}

		// Store the current indent in undo history
		if (ok)
			ok = StorePathIndentAction::DoStoreIndent(this, &UndoActions, mpDragStory);

		if (ok)
			ok = DoIndentChange();

		GetApplication()->FindSelection()->Update();

		// Tell the story we have finished fiddling with it (reformats for us)
		if (ok)
		{
			ObjChangeParam ObjChange(OBJCHANGE_FINISHED, ObjChangeFlags(), NULL, this);
			ok = UpdateChangedNodes(&ObjChange);
		}
	}
		
	if (!ok)
		FailAndExecute();
	
	End();
}


/********************************************************************************************
>	virtual void OpDragStoryIndent::RenderDragBlobs(DocRect ClipRect, Spread* pSpread, BOOL bSolidDrag)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95
	Inputs:		ClipRect - a clipping rectangle
				pSpread - the spread to render on
	Purpose:	Renders the EOR drag blobs, either putting them on or taking them off.
				Also renders the story's object blobs
********************************************************************************************/
void OpDragStoryIndent::RenderDragBlobs(DocRect ClipRect, Spread* pSpread, BOOL bSolidDrag)
{
	DocCoord OtherIndent = GetOtherIndent();
		
	if (ClipRect.IsEmpty())
		TextToolBlobPosList::RenderBlob(pSpread, NULL, OtherIndent);
	else
		TextToolBlobPosList::RenderBlob(pSpread, &ClipRect, OtherIndent);

	RenderDraggingBlobs(ClipRect, pSpread);
}


/********************************************************************************************
>	void OpDragStoryIndent::RenderDraggingBlobs(DocRect ClipRect, Spread* pSpread)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95
	Inputs:		ClipRect - a clipping rectangle
				pSpread - the spread to render on
	Outputs:	-
	Returns:	-
	Purpose:	Renders the EOR drag blob for the margin being dragged.
********************************************************************************************/
void OpDragStoryIndent::RenderDraggingBlobs(DocRect ClipRect, Spread* pSpread)
{
	if (ClipRect.IsEmpty())
		TextToolBlobPosList::RenderBlob(pSpread, NULL, mCurrentBlobPos);
	else
		TextToolBlobPosList::RenderBlob(pSpread, &ClipRect, mCurrentBlobPos);

	// Render a line connecting the blobs if not on a path
	if (mpStoryPath==NULL)
	{
		DocRect* pClipper = &ClipRect;
		if (ClipRect.IsEmpty())
			pClipper = NULL;
	
		RenderRegion* pRegion = DocView::RenderOnTop(pClipper, pSpread, ClippedEOR);
		while (pRegion != NULL)
		{
			// Draw a Cross Hair
			pRegion->SetLineColour(COLOUR_XOREDIT);
			pRegion->SetFillColour(COLOUR_TRANS);
			pRegion->DrawLine(mCurrentBlobPos, GetOtherIndent());

			// Get the next region in the list
			pRegion = DocView::GetNextOnTop(pClipper);
		}
	}
}



/********************************************************************************************
>	BOOL OpDragStoryIndent::GetLeftHandLength(MILLIPOINT* pResult)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/9/96
	Inputs:		-
	Outputs:	pResult - Length along the path from the start
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Gets the length along the path from the start of the path to the current pos
********************************************************************************************/
BOOL OpDragStoryIndent::GetLeftHandLength(MILLIPOINT* pResult)
{
	return GetLeftHandLength(mCurrentBlobPos, mpStoryPath, pResult);
}


	
/********************************************************************************************
>	BOOL OpDragStoryIndent::GetRightHandLength(MILLIPOINT* pResult)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/9/96
	Inputs:		-
	Outputs:	TRUE/FALSE for success/failure
	Returns:	Length along the path from the end
	Purpose:	Gets the length along the path from the end of the path to the current pos
********************************************************************************************/
BOOL OpDragStoryIndent::GetRightHandLength(MILLIPOINT* pResult)
{
	return GetRightHandLength(mCurrentBlobPos, mpStoryPath, pResult);
}



/********************************************************************************************
>	BOOL OpDragStoryIndent::GetLeftHandLength(DocCoord Coord, NodePath* pPath, MILLIPOINT* pResult)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/9/96
	Inputs:		Coord - coord of point
				pPath - pointer to a path
	Outputs:	pResult - Length along the path from the start to Coord
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Gets the length along the path from the start of the path to the given point
********************************************************************************************/
BOOL OpDragStoryIndent::GetLeftHandLength(DocCoord Coord, NodePath* pPath, MILLIPOINT* pResult)
{
	ERROR2IF(pResult==NULL, FALSE, "NULL output parameter");
	*pResult = 0;
	ERROR2IF(pPath==NULL, FALSE, "NULL path pointer");

	return pPath->InkPath.GetDistanceToPoint(Coord, pResult);
}


	
/********************************************************************************************
>	BOOL OpDragStoryIndent::GetRightHandLength(DocCoord Coord, NodePath* pPath, MILLIPOINT* pResult)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/9/96
	Inputs:		Coord - coord of point
				pPath - pointer to a path
	Outputs:	pResult - Length along the path from the end to Coord
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Gets the length along the path from the end of the path to the given point
********************************************************************************************/
BOOL OpDragStoryIndent::GetRightHandLength(DocCoord Coord, NodePath* pPath, MILLIPOINT* pResult)
{
	ERROR2IF(pResult==NULL, FALSE, "NULL output parameter");
	*pResult = 0;
	ERROR2IF(pPath==NULL, FALSE, "NULL path pointer");

	// Get the length of the whole path
	MILLIPOINT TotalLength = (MILLIPOINT)(pPath->InkPath.GetPathLength());

	// Get the distance from the start of the path to where the drag ended
	MILLIPOINT Distance = 0;
	BOOL ok = pPath->InkPath.GetDistanceToPoint(Coord, &Distance);

	*pResult = TotalLength - Distance;
		
	return ok;
}



/********************************************************************************************
>	BOOL OpDragStoryPathLeftIndent::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Returns:	TRUE for succesful init, FALSE if init failed
	Purpose:	Initialises the operation; registers its op descriptor
********************************************************************************************/
BOOL OpDragStoryPathLeftIndent::Init()
{
	return (RegisterOpDescriptor(	0,
		 							_R(IDS_OPTEXT_DRAGINDENT),
									CC_RUNTIME_CLASS(OpDragStoryPathLeftIndent),
		 							OPTOKEN_TEXTDRAGLEFTPATHINDENT,
		 							OpDragStoryIndent::GetState,
		 							0,							// help ID 
		 							0,							// Bubble help
		 							0,
		 							0,
									SYSTEMBAR_ILLEGAL,			// For now !
									TRUE, 						// Receive messages
									FALSE,
									FALSE,
									0,
									GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION | DONT_GREY_WHEN_SELECT_INSIDE
	       )); 
}


/********************************************************************************************
>	BOOL OpDragStoryPathRightIndent::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Returns:	TRUE for succesful init, FALSE if init failed
	Purpose:	Initialises the operation; registers its op descriptor
********************************************************************************************/
BOOL OpDragStoryPathRightIndent::Init()
{
	return (RegisterOpDescriptor(	0,
		 							_R(IDS_OPTEXT_DRAGINDENT),
									CC_RUNTIME_CLASS(OpDragStoryPathRightIndent),
		 							OPTOKEN_TEXTDRAGRIGHTPATHINDENT,
		 							OpDragStoryIndent::GetState,
		 							0,							// help ID 
		 							0,							// Bubble help
		 							0,
		 							0,
									SYSTEMBAR_ILLEGAL,			// For now !
									TRUE, 						// Receive messages
									FALSE,
									FALSE,
									0,
									GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION | DONT_GREY_WHEN_SELECT_INSIDE
	       )); 
}


/********************************************************************************************
>	BOOL OpDragStoryNonPathLeftIndent::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Returns:	TRUE for succesful init, FALSE if init failed
	Purpose:	Initialises the operation; registers its op descriptor
********************************************************************************************/
BOOL OpDragStoryNonPathLeftIndent::Init()
{
	return (RegisterOpDescriptor(	0,
		 							_R(IDS_OPTEXT_DRAGINDENT),
									CC_RUNTIME_CLASS(OpDragStoryNonPathLeftIndent),
		 							OPTOKEN_TEXTDRAGLEFTNONPATHINDENT,
		 							OpDragStoryIndent::GetState,
		 							0,							// help ID 
		 							0,							// Bubble help
		 							0,
		 							0,
									SYSTEMBAR_ILLEGAL,			// For now !
									TRUE, 						// Receive messages
									FALSE,
									FALSE,
									0,
									GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION | DONT_GREY_WHEN_SELECT_INSIDE
	       )); 
}


/********************************************************************************************
>	BOOL OpDragStoryNonPathRightIndent::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Returns:	TRUE for succesful init, FALSE if init failed
	Purpose:	Initialises the operation; registers its op descriptor
********************************************************************************************/
BOOL OpDragStoryNonPathRightIndent::Init()
{
	return (RegisterOpDescriptor(	0,
		 							_R(IDS_OPTEXT_DRAGINDENT),
									CC_RUNTIME_CLASS(OpDragStoryNonPathRightIndent),
		 							OPTOKEN_TEXTDRAGRIGHTNONPATHINDENT,
		 							OpDragStoryIndent::GetState,
		 							0,							// help ID 
		 							0,							// Bubble help
		 							0,
		 							0,
									SYSTEMBAR_ILLEGAL,			// For now !
									TRUE, 						// Receive messages
									FALSE,
									FALSE,
									0,
									GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION | DONT_GREY_WHEN_SELECT_INSIDE
	       )); 
}



/********************************************************************************************
>	BOOL OpDragStoryPathLeftIndent::DoIndentChange()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Called to perform the change to the story required at the completion of the
				blob drag
********************************************************************************************/
BOOL OpDragStoryPathLeftIndent::DoIndentChange()
{
	ERROR2IF(mpDragStory==NULL, FALSE, "NULL story pointer");

	MILLIPOINT Distance = 0;
	BOOL ok = TRUE;

	if (mpDragStory->IsTextOnPathReversed())
		ok = GetRightHandLength(&Distance);
	else
		ok = GetLeftHandLength(&Distance);

	if (ok)
		mpDragStory->SetLeftIndent(Distance);

	return ok;
}


/********************************************************************************************
>	BOOL OpDragStoryPathRightIndent::DoIndentChange()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Called to perform the change to the story required at the completion of the
				blob drag
********************************************************************************************/
BOOL OpDragStoryPathRightIndent::DoIndentChange()
{
	ERROR2IF(mpDragStory==NULL, FALSE, "NULL story pointer");

	MILLIPOINT Distance = 0;
	BOOL ok = TRUE;

	if (mpDragStory->IsTextOnPathReversed())
		ok = GetLeftHandLength(&Distance);
	else
		ok = GetRightHandLength(&Distance);

	if (ok)
		mpDragStory->SetRightIndent(Distance);

	return ok;
}


/********************************************************************************************
>	BOOL OpDragStoryNonPathLeftIndent::DoIndentChange()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Called to perform the change to the story required at the completion of the
				blob drag
********************************************************************************************/
BOOL OpDragStoryNonPathLeftIndent::DoIndentChange()
{
	// Set the story width
	DocCoord LeftIndentPos = GetThisIndent();
	DocCoord RightIndentPos = GetOtherIndent();
	mpDragStory->SetStoryWidth((INT32)mCurrentBlobPos.Distance(RightIndentPos));

	// We also want to rotate about the right indent position
	double OldAngle = atan2((double)(LeftIndentPos.y-RightIndentPos.y), (double)(LeftIndentPos.x-RightIndentPos.x));
	double NewAngle = atan2((double)(mCurrentBlobPos.y-RightIndentPos.y), (double)(mCurrentBlobPos.x-RightIndentPos.x));
	ANGLE RotAngle = (NewAngle-OldAngle)*(180/PI);
	Matrix RMatrix(RightIndentPos, RotAngle);
	Trans2DMatrix* pTransMat = new Trans2DMatrix(RMatrix);
	BOOL ok = pTransMat!=NULL;
	if (ok)
		ok = DoTransformNode(mpDragStory, pTransMat);

	// Now we need to translate the story so the left blob is where the user ended the drag
	if (ok)
	{
		LeftIndentPos = GetThisIndent();
		DocCoord TransOffset = mCurrentBlobPos - LeftIndentPos;
		Matrix TMatrix(TransOffset.x, TransOffset.y);
		Trans2DMatrix* pTransMat = new Trans2DMatrix(TMatrix);
		ok = pTransMat!=NULL;
		if (ok)
			ok = DoTransformNode(mpDragStory, pTransMat);
	}

	return ok;
}


/********************************************************************************************
>	BOOL OpDragStoryNonPathRightIndent::DoIndentChange()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Called to perform the change to the story required at the completion of the
				blob drag
********************************************************************************************/
BOOL OpDragStoryNonPathRightIndent::DoIndentChange()
{
	// Set the story width
	mpDragStory->SetStoryWidth((INT32)mCurrentBlobPos.Distance(GetOtherIndent()));

	// Rotate the story about the left indent pos.
	DocCoord LeftIndentPos = GetOtherIndent();
	DocCoord OldRightIndentPos = GetThisIndent();

	double OldAngle = atan2((double)(OldRightIndentPos.y-LeftIndentPos.y), (double)(OldRightIndentPos.x-LeftIndentPos.x));
	double NewAngle = atan2((double)(mCurrentBlobPos.y-LeftIndentPos.y), (double)(mCurrentBlobPos.x-LeftIndentPos.x));
	ANGLE RotAngle = (NewAngle-OldAngle)*(180/PI);

	Matrix RotMatrix(LeftIndentPos, RotAngle);

	// Must not create a transform matrix on the stack as the operation keeps a pointer to it!
	Trans2DMatrix* pTransMat = new Trans2DMatrix(RotMatrix);

	if (pTransMat == NULL)
		return FALSE;
	else
		return DoTransformNode(mpDragStory, pTransMat);
}



/********************************************************************************************
>	DocCoord OpDragStoryPathLeftIndent::GetThisIndent()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Returns:	The coordinate of the indent begin dragged (where it was before the drag started)
	Purpose:	Gets the original position of the dragged indent
********************************************************************************************/
DocCoord OpDragStoryPathLeftIndent::GetThisIndent()
{
	return mpDragStory->GetLeftIndentPos();
}


/********************************************************************************************
>	DocCoord OpDragStoryPathRightIndent::GetThisIndent()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Returns:	The coordinate of the indent begin dragged (where it was before the drag started)
	Purpose:	Gets the original position of the dragged indent
********************************************************************************************/
DocCoord OpDragStoryPathRightIndent::GetThisIndent()
{
	return mpDragStory->GetRightIndentPos();
}


/********************************************************************************************
>	DocCoord OpDragStoryNonPathLeftIndent::GetThisIndent()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Returns:	The coordinate of the indent begin dragged (where it was before the drag started)
	Purpose:	Gets the original position of the dragged indent
********************************************************************************************/
DocCoord OpDragStoryNonPathLeftIndent::GetThisIndent()
{
	const Matrix* pStoryMatrix = mpDragStory->GetpStoryMatrix();
	DocCoord LeftIndent(0,0);
	pStoryMatrix->transform(&LeftIndent);

	return LeftIndent;
}


/********************************************************************************************
>	DocCoord OpDragStoryNonPathRightIndent::GetThisIndent()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Returns:	The coordinate of the indent begin dragged (where it was before the drag started)
	Purpose:	Gets the original position of the dragged indent
********************************************************************************************/
DocCoord OpDragStoryNonPathRightIndent::GetThisIndent()
{
	const Matrix* pStoryMatrix = mpDragStory->GetpStoryMatrix();
	DocCoord RightIndent(mpDragStory->GetStoryWidth(),0);
	pStoryMatrix->transform(&RightIndent);

	return RightIndent;
}



/********************************************************************************************
>	DocCoord OpDragStoryPathLeftIndent::GetOtherIndent()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Returns:	The coordinate of the 'other' indent, not the one being dragged
	Purpose:	Gets the original position of the stationary indent
********************************************************************************************/
DocCoord OpDragStoryPathLeftIndent::GetOtherIndent()
{
	return mpDragStory->GetRightIndentPos();
}


/********************************************************************************************
>	DocCoord OpDragStoryPathRightIndent::GetOtherIndent()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Returns:	The coordinate of the 'other' indent, not the one being dragged
	Purpose:	Gets the original position of the stationary indent
********************************************************************************************/
DocCoord OpDragStoryPathRightIndent::GetOtherIndent()
{
	return mpDragStory->GetLeftIndentPos();
}


/********************************************************************************************
>	DocCoord OpDragStoryNonPathLeftIndent::GetOtherIndent()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Returns:	The coordinate of the 'other' indent, not the one being dragged
	Purpose:	Gets the original position of the stationary indent
********************************************************************************************/
DocCoord OpDragStoryNonPathLeftIndent::GetOtherIndent()
{
	const Matrix* pStoryMatrix = mpDragStory->GetpStoryMatrix();
	DocCoord RightIndent(mpDragStory->GetStoryWidth(),0);
	pStoryMatrix->transform(&RightIndent);

	return RightIndent;
}


/********************************************************************************************
>	DocCoord OpDragStoryNonPathRightIndent::GetOtherIndent()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Returns:	The coordinate of the 'other' indent, not the one being dragged
	Purpose:	Gets the original position of the stationary indent
********************************************************************************************/
DocCoord OpDragStoryNonPathRightIndent::GetOtherIndent()
{
	const Matrix* pStoryMatrix = mpDragStory->GetpStoryMatrix();
	DocCoord LeftIndent(0,0);
	pStoryMatrix->transform(&LeftIndent);

	return LeftIndent;
}



/********************************************************************************************
>	DocCoord OpDragStoryPathLeftIndent::InternalConstrain(DocCoord Current, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Inputs:		Current - the current mouse position
				ClickMods - the current click modifiers
	Returns:	The suitably constrained mouse position
	Purpose:	Constrains the drag of the left indent on the path
********************************************************************************************/
DocCoord OpDragStoryPathLeftIndent::InternalConstrain(DocCoord Current, ClickModifiers ClickMods)
{
	ERROR2IF(mpDragStory==NULL || mpStoryPath==NULL, DocCoord(0,0), "NULL pointer");

	DocView::SnapCurrent(mpStartSpread, &Current);

	INT32 NearEl=0;
	double mu=0.0;
	mpStoryPath->InkPath.SqrDistanceToPoint(Current, &NearEl, &mu);
	DocCoord LinePoint = mpStoryPath->InkPath.ClosestPointTo(mu, NearEl);

	// We need to stop the left hand blob going past the right hand one.
	MILLIPOINT TotalLineLength = (MILLIPOINT)mpStoryPath->InkPath.GetPathLength();
	MILLIPOINT LeftIndentLength = 0;
	BOOL ok = TRUE;
	if (mpDragStory->IsTextOnPathReversed())
		ok = GetRightHandLength(LinePoint, mpStoryPath, &LeftIndentLength);
	else
		ok = GetLeftHandLength(LinePoint, mpStoryPath, &LeftIndentLength);

	if (((TotalLineLength-LeftIndentLength) < mpDragStory->GetRightIndent()) && ok) 
		return mpDragStory->GetRightIndentPos();
	else
		return LinePoint;
}


/********************************************************************************************
>	DocCoord OpDragStoryPathRightIndent::InternalConstrain(DocCoord Current, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Inputs:		Current - the current mouse position
				ClickMods - the current click modifiers
	Returns:	The suitably constrained mouse position
	Purpose:	Constrains the drag of the right indent on the path
********************************************************************************************/
DocCoord OpDragStoryPathRightIndent::InternalConstrain(DocCoord Current, ClickModifiers ClickMods)
{
	DocView::SnapCurrent(mpStartSpread, &Current);
	
	INT32 NearEl=0;
	double mu=0.0;
	mpStoryPath->InkPath.SqrDistanceToPoint(Current, &NearEl, &mu);
	DocCoord LinePoint = mpStoryPath->InkPath.ClosestPointTo(mu, NearEl);

	// We need to stop the right hand blob going past the left hand one.
	MILLIPOINT TotalLineLength = (MILLIPOINT)mpStoryPath->InkPath.GetPathLength();
	MILLIPOINT RightIndentLength = 0;
	BOOL ok = TRUE;
	if (mpDragStory->IsTextOnPathReversed())
		ok = GetLeftHandLength(LinePoint, mpStoryPath, &RightIndentLength);
	else
		ok = GetRightHandLength(LinePoint, mpStoryPath, &RightIndentLength);

	if (((TotalLineLength-RightIndentLength) < mpDragStory->GetLeftIndent()) && ok) 
		return mpDragStory->GetLeftIndentPos();
	else
		return LinePoint;
}


/********************************************************************************************
>	DocCoord OpDragStoryNonPathIndent::InternalConstrain(DocCoord Current, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Inputs:		Current - the current mouse position
				ClickMods - the current click modifiers
	Returns:	The suitably constrained mouse position
	Purpose:	Constrains the drag of indents not on a path
********************************************************************************************/
DocCoord OpDragStoryNonPathIndent::InternalConstrain(DocCoord Current, ClickModifiers ClickMods)
{
	// By default drag along the baseline
	if (!ClickMods.Adjust && !ClickMods.Constrain)
	{
		const double OrigLength = GetThisIndent().Distance(GetOtherIndent());
		double Hypot = GetOtherIndent().Distance(Current);
		double Oppo = DocCoord::DistanceFromLine(GetThisIndent(), GetOtherIndent(), Current);
		double Adj = sqrt(Hypot*Hypot - Oppo*Oppo);
		double Ratio = 1.0;
		if (OrigLength != 0.0)
			Ratio = Adj/(OrigLength);

		const DocCoord JoinPoint = GetOtherIndent(); 
		const DocCoord OtherJoinEnd = GetThisIndent(); 
		const DocCoord cCurrent = Current; 

		if (DocCoord::IsJoinAcute(&JoinPoint, &OtherJoinEnd, &cCurrent))
			return DocCoord::PositionPointFromRatio(JoinPoint, OtherJoinEnd, Ratio);
		else
			return DocCoord::PositionPointFromRatio(JoinPoint, OtherJoinEnd, -Ratio);
	}
	else
	{
		// CTRL is angle constrain, // SHIFT is unconstrained
		if (ClickMods.Constrain)
		{
			DocCoord Anchor = GetOtherIndent();
			DocView::ConstrainToAngle(Anchor, &Current);
		}

		DocView::SnapCurrent(mpStartSpread, &Current);
	}

	return Current;
}



/********************************************************************************************
>	BOOL OpDragStoryPathIndent::SetCurrentPos(DocCoord MousePos, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Inputs:		MousePos - a coordinate
				ClickMods - the current click modifiers
	Outputs:	Member variable mCurrentBlobPos is set from MousePos
	Returns:	TRUE/FALSE for success failure
	Purpose:	Given a position works out the nearest position on the path to it, and sets
				mCurrentBlobPos to it.
********************************************************************************************/
BOOL OpDragStoryPathIndent::SetCurrentPos(DocCoord MousePos, ClickModifiers ClickMods)
{
	ERROR2IF(mpStoryPath==NULL, FALSE, "NULL story pointer");

	mCurrentBlobPos = InternalConstrain(MousePos, ClickMods);

	return TRUE;
}


/********************************************************************************************
>	BOOL OpDragStoryNonPathIndent::SetCurrentPos(DocCoord MousePos, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95
	Inputs:		MousePos - a coordinate
				ClickMods - the current click modifiers
	Outputs:	Member variable mCurrentBlobPos is set from MousePos
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Given a position sets mCurrentBlobPos to it.
********************************************************************************************/
BOOL OpDragStoryNonPathIndent::SetCurrentPos(DocCoord MousePos, ClickModifiers ClickMods)
{
	ERROR3IF(mpStoryPath!=NULL, "Story is on a path, you don't want to use this function");

	mCurrentBlobPos = InternalConstrain(MousePos, ClickMods);

	return TRUE;
}













/********************************************************************************************
>	StorePathIndentAction::StorePathIndentAction()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95
	Purpose:	Constructor for StorePathIndentAction.  Initialises the member variables  
********************************************************************************************/

StorePathIndentAction::StorePathIndentAction()
{
	mpTextStory = NULL;
	mLeftIndentLength = 0;
	mRightIndentLength = 0;
	mStoryWidth = 0;
}


/********************************************************************************************
>	StorePathIndentAction::~StorePathIndentAction()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95
	Purpose:	Destructor for StorePathIndentAction.  
********************************************************************************************/

StorePathIndentAction::~StorePathIndentAction()
{
}


/********************************************************************************************
>	ActionCode StorePathIndentAction::Execute()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/18/95
	Returns:	ActionCode, either AC_OK, AC_NORECORD or AC_FAIL
	Purpose:	This action inserts the opposite StorePathIndentAction for redo purposes, then
				set the indent in the TextStory to the stored value.
	Errors:		Checks on pointers
********************************************************************************************/

ActionCode StorePathIndentAction::Execute()
{
	// Salary checks
	ERROR2IF(mpTextStory == NULL, AC_FAIL, "The TextStory pointer is NULL");
	
	// Create a redo action for this action, which is also a StorePathIndentAction
	ActionCode Act;
	Act = StorePathIndentAction::Init(pOperation, 
										pOppositeActLst, 
										mpTextStory,
									    mpTextStory->GetLeftIndent(),
									    mpTextStory->GetRightIndent(),
										mpTextStory->GetStoryWidth());

	// Now undo
	if (Act != AC_FAIL)
	{
		mpTextStory->SetLeftIndent(mLeftIndentLength);
		mpTextStory->SetRightIndent(mRightIndentLength);
		mpTextStory->SetStoryWidth(mStoryWidth);
	}

	return Act;
}


/********************************************************************************************
>	static ActionCode StorePathIndentAction::Init(Operation* pOp,
												ActionList* pActionList,
												TextStory* pStory,
												MILLIPOINT LeftIndent,
												MILLIPOINT RightIndent
												MILLIPOINT StoryWidth)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95
	Inputs:		pOp is the pointer to the operation to which this action belongs
				pActionList is the action list to which this action should be added
				pStory - pointer to the text story to apply this action to
				LeftIndent - the left path indent value
				RightIndent - the right path indent value
				StoryWidth - the width of the story
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Sets up the action to store a text story path indent
	SeeAlso:	Action::Init(), StorePathIndentAction::Execute()
********************************************************************************************/

ActionCode StorePathIndentAction::Init(Operation* pOp,
									ActionList* pActionList,
									TextStory* pStory,
									MILLIPOINT LeftIndent,
									MILLIPOINT RightIndent,
									MILLIPOINT StoryWidth)
{
	// Salary check
	ERROR2IF(pStory == NULL, AC_FAIL, "TextStory pointer was NULL");

	// Create the new action
	UINT32 ActSize = sizeof(StorePathIndentAction);
	StorePathIndentAction* pNewAction = NULL;
	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(StorePathIndentAction), (Action**)&pNewAction);

	// Fill in its data
	if ((Ac == AC_OK) && (pNewAction != NULL))
	{
		pNewAction->mpTextStory = pStory;
		pNewAction->mLeftIndentLength = LeftIndent;
		pNewAction->mRightIndentLength = RightIndent;
		pNewAction->mStoryWidth = StoryWidth;
	}

	return Ac;
}


/********************************************************************************************
>	static BOOL StorePathIndentAction::DoStoreIndent(Operation* pOp,
													 ActionList* pActionList,
													 TextStory* pStory)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95
	Inputs:		pOp is the pointer to the operation to which this action belongs
				pActionList is the action list to which this action should be added
				pStory - pointer to the text story to apply this action to
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Call this function to store the current text story path indent value before
				changing it
	SeeAlso:	StorePathIndentAction::Init()
********************************************************************************************/

BOOL StorePathIndentAction::DoStoreIndent( Operation* pOp, ActionList* pActionList, TextStory* pStory)
{
	// Salary check
	ERROR2IF(pStory == NULL, FALSE, "The TextStory pointer is NULL");
	
	// Create an action for this
	ActionCode Act;
	Act = StorePathIndentAction::Init(pOp, pActionList, pStory, pStory->GetLeftIndent(), pStory->GetRightIndent(), pStory->GetStoryWidth());

	return (Act != AC_FAIL);
}


#endif


/////////////////////////////////////////////////////////////////////////////////////////////
// OpApplyGlobalAffect

/********************************************************************************************
>	void OpApplyGlobalAffect::DoAffectChange(ObjChangeParam* pObjChange, Document* pSearchDoc=NULL)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/01/96
	Inputs:		pObjChange	= a pointer to a user change parameter
	Purpose:	Scans through all object nodes of all object trees in all documents, passing
				the user object change parameter to every node it comes accross. The tree
				scans are depth first. This is a useful operation for other ops to derive
				themselves off.
********************************************************************************************/

void OpApplyGlobalAffect::DoAffectChange(ObjChangeParam* pObjChange, Document* pSearchDoc)
{
	// Get a pointer to the StartDocument 
	ERROR3IF(pObjChange == NULL, "DoAffectChange called with a NULL pObjChange param");
	if (pObjChange==NULL)
		return;

	Document* pDocument = pSearchDoc;
	if (!pDocument)
		pDocument = (Document*) Camelot.Documents.GetHead();

#if !defined(EXCLUDE_FROM_RALPH)
	// render blobs off (only in selected doc)
	if (Camelot.FindSelection()!=NULL)
	{
		Node* SelNode = Camelot.FindSelection()->FindFirst();
		if (SelNode!=NULL)
		{
			Spread* SelSpread = (Spread*)SelNode->FindParent(CC_RUNTIME_CLASS(Spread));
			if (SelSpread!=NULL)
				Camelot.GetBlobManager()->RenderOff(NULL, SelSpread);
		}
	}
#endif

	// scan through all documents
	while (pDocument != NULL)
	{
  		// Get a pointer to first node
		Node* pNode = Node::DocFindFirstDepthFirst(pDocument);
		// Scan through the tree, on a depth first traversal affecting all nodes.
		while (pNode != NULL)
		{
			AffectNode(pDocument, pNode, pObjChange);
			// Move onto the next node in the tree
			pNode = pNode->DocFindNextDepthFirst(); 
		}
		// make sure we update all of this document
		pDocument->FlushRedraw();
		
		if (pSearchDoc==NULL)
			pDocument = (Document*) Camelot.Documents.GetNext(pDocument);
		else
			pDocument = NULL;
	}

	// Now for the important bit, make sure we update all the changed nodes.
	pObjChange->Define(OBJCHANGE_FINISHED,pObjChange->GetChangeFlags(),NULL,NULL);
	UpdateAllChangedNodes(pObjChange);

#if !defined(EXCLUDE_FROM_RALPH)
	// render blobs back on again!
	if (Camelot.FindSelection()!=NULL)
	{
		Node* SelNode = Camelot.FindSelection()->FindFirst();
		if (SelNode!=NULL)
		{
			Spread* SelSpread = (Spread*)SelNode->FindParent(CC_RUNTIME_CLASS(Spread));
			if (SelSpread!=NULL)
				Camelot.GetBlobManager()->RenderOff(NULL, SelSpread);
		}
	}
#endif
}


/********************************************************************************************
>	virtual void OpApplyGlobalAffect::AffectNode(Document* pDocument,
												Node* pNode,
												ObjChangeParam* pObjChange)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/01/96
	Inputs:		pDocument	= a pointer to the document currently being scanned
				pNode 		= a pointer to the node the scan is at (going up the tree)
				pObjChange	= a pointer to a user change parameter
	Purpose:	This function checks for text story nodes. If the entry node is one, the
				node will be reformated and redrawn.
********************************************************************************************/

void OpApplyGlobalAffect::AffectNode(Document* pDocument, Node* pNode, ObjChangeParam* pObjChange)
{
}


/********************************************************************************************
>	BOOL OpAffectFontChange::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/01/96
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpApplyGlobalAffect initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
********************************************************************************************/

BOOL OpAffectFontChange::Init()
{
	return (RegisterOpDescriptor(	0,
	 								0,
									CC_RUNTIME_CLASS(OpAffectFontChange),
	 								OPTOKEN_AFFECTFONTCHANGE,
	 								OpAffectFontChange::GetState,
	 								0,	/* help ID */
	 								0,
	 								0	/* bitmap ID */));
}               
    

/********************************************************************************************
>	OpState	OpAffectFontChange::GetState(String_256*, OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/01/96
	Returns:	The state of the OpApplyGlobalAffect operation
	Purpose:	For finding the OpApplyGlobalAffect's state. 
********************************************************************************************/

OpState	OpAffectFontChange::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
 	return (OpSt);
}


/********************************************************************************************
>	void OpAffectFontChange::DoAffectFontChange()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/01/96
	Inputs:		-
	Purpose:	Scans all documents, asking all text objects to reformat themselves on
				this font change. This is performed as a none undoable operation so that
				the allow op system can be used to update all parents.
********************************************************************************************/

void OpAffectFontChange::DoAffectFontChange()
{
	ObjChangeFlags cFlags;
	cFlags.Attribute = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);

	BeginSlowJob();
	DoAffectChange(&ObjChange);
    EndSlowJob();
	End();
}


/********************************************************************************************
>	void OpAffectFontChange::AffectNode(Document* pDocument,
										Node* pNode,
										ObjChangeParam* pObjChange)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/96
	Inputs:		pDocument	= a pointer to the document currently being scanned
				pNode 		= a pointer to the node the scan is at (going up the tree)
				pObjChange	= a pointer to a user change parameter
	Purpose:	Throw away undo/redo history unless the document is known not to be affected
				by the font substitution (this can only be ascertained by the font gallery)
				Also, call AllowOp() on TextStories to let them know of the potential change
********************************************************************************************/

void OpAffectFontChange::AffectNode(Document* pDocument, Node* pNode, ObjChangeParam* pObjChange)
{
	// throw away undo for all docs
	if (IS_A(pNode,NodeDocument))
	{
		pDocument->GetOpHistory().DeleteUndoableOps();
		pDocument->GetOpHistory().DeleteRedoableOps();
	}

	if (IS_A(pNode,TextStory))
		((TextStory*)pNode)->AllowOp(pObjChange);
}

// ------------------------------------------------------------------------------------------
// ToggleAutoKerningAction

/********************************************************************************************

>	ToggleAutoKerningAction::ToggleAutoKerningAction()

	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/2000
	Purpose:	Constructor for the action

********************************************************************************************/
ToggleAutoKerningAction::ToggleAutoKerningAction()
{
	pTextStory			= 0;
	OldIsAutoKerning	= FALSE;
}


/********************************************************************************************

>	ActionCode ToggleAutoKerningAction::Init(	Operation*	pOp,
												ActionList*	pActionList,
												TextStory*	pStory,
												bool		NewIsAutoKerning);

	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/2000
	Inputs:		pOp  				= ptr to the operation to which this action belongs
				pActionList 		= ptr to action list to which this action should be added
				pStory				= ptr to TextStory to change 
				NewIsAutoKerning	= as the name says
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.
	SeeAlso:	Action::Init()

********************************************************************************************/
ActionCode ToggleAutoKerningAction::Init(Operation*	pOp,
										ActionList*	pActionList,
										TextStory*	pStory,
										BOOL		NewIsAutoKerning)
{
	ERROR2IF(pStory==0,AC_FAIL,"pStory was NULL");

	UINT32 ActSize = sizeof(ToggleAutoKerningAction);

	ToggleAutoKerningAction* pNewAction = NULL;

	ActionCode Ac = Action::Init(	pOp,
									pActionList,
									ActSize,
									CC_RUNTIME_CLASS(ToggleAutoKerningAction),
									(Action**)&pNewAction);

	if ((Ac != AC_FAIL) && (pNewAction != NULL))
	{
		ERROR2IF(pStory == NULL,AC_FAIL,"TextStory pointer was NULL");
		pNewAction->pTextStory			= pStory;
		pNewAction->OldIsAutoKerning	= !NewIsAutoKerning;

		pStory->SetAutoKerning(NewIsAutoKerning);
		pStory->FlagNodeAndDescendantsModifiedByOpAndParentsHaveDescendantModifiedByOp();
		pStory->FormatAndChildren(NULL,FALSE);
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode ToggleAutoKerningAction::Execute();

	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/2000
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  Uses Init() to undo/redo the action.
	SeeAlso:	Action::Init()

********************************************************************************************/
ActionCode ToggleAutoKerningAction::Execute()
{
	ActionCode Act;

	Act = ToggleAutoKerningAction::Init(pOperation, 
										pOppositeActLst,
										pTextStory,
										OldIsAutoKerning);

	return Act;
}
