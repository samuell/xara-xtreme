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
// The Grad Fill creation and Editing operations

/********************************************************************************************

	NOTE: 

	OpCreateFill is no longer used.
	OpEditFill, is now used for fill Creation and Editing.

********************************************************************************************/

/*
*/


#include "camtypes.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "opgrad.h"
#include "filltool.h"
//#include "will.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "attr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "lineattr.h"
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "rndrgn.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "stockcol.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "paths.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "blobs.h"
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "toollist.h"
//#include "ink.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "bitmpinf.h"
#include "progress.h"
#include "ndoptmz.h"
#include "csrstack.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "objchge.h"
#include "keypress.h"
#include "nodeblnd.h"
#include "fillramp.h"

//#include "will2.h"
#include "nodecont.h"

#ifndef STANDALONE
CC_IMPLEMENT_DYNCREATE( OpCreateFill, Operation )
CC_IMPLEMENT_DYNCREATE( OpEditFill,   OpApplyAttribToSelected )
CC_IMPLEMENT_DYNCREATE( RestoreFillRampAction,   Action)
#endif
//CC_IMPLEMENT_DYNCREATE( OpMutateFill, Operation )
//CC_IMPLEMENT_DYNCREATE( OpChangeFillColour, SelOperation )
//CC_IMPLEMENT_DYNCREATE( OpChangeSelectionColours, SelOperation )
//CC_IMPLEMENT_DYNCREATE( ModifyFillCoordsAction, Action)
//CC_IMPLEMENT_DYNCREATE( ModifyFillColoursAction, Action)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

// Fill editing preferences
BOOL 	OpEditFill::InteractiveDragUpdate 	= TRUE;
BOOL 	OpEditFill::ContinuousEOR 			= TRUE;
UINT32 	OpEditFill::IdleFillDelay 			= 200;	// Fill redraw delay (used to be 300 in Xara X release)

BOOL OpEditFill::CreateFill = FALSE;

static BOOL EditFinished = FALSE;

//  some pointers that are useful within this stuff ....

static NodeRenderableInk* pParentOfFill = NULL;
static AttrFillGeometry* pTheFill = NULL;

#ifndef STANDALONE

/********************************************************************************************

>	OpEditFill::OpEditFill()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/94
	Purpose:	Dummy Contrustor

********************************************************************************************/

OpEditFill::OpEditFill()
{
	// Initialise everything
	AlwaysFail 			= TRUE;
	ApplyAtEnd 			= TRUE;
	ForceAspectLock 	= FALSE;
	CreateFill 			= FALSE;
	IsRequired 			= TRUE;
	ShouldApplyNewFill 	= FALSE;
	CheckForCompound 	= TRUE;
	CallAllowOp	 		= FALSE;

	InvalidateAll 		= TRUE;
	InvalidateCompound 	= FALSE;

	DragIsIdle 			= FALSE;
	DoneIdleRedraw 		= FALSE;

	ShowDragBlobs		= TRUE;
	DontDrawBlobs 		= FALSE;

	EditFinished 		= FALSE;

	AspectRatio = 1;
}

/********************************************************************************************

>	void OpEditFill::DoCreate(DocCoord &Start, DocCoord &End, Spread* pSpread, AttrFillGeometry* pGrad)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		Start - The starting position of the drag
				pSpread - The spread that the drag started over
				pElip - The NodeGradFill that is being edited
	Purpose:	-

********************************************************************************************/

void OpEditFill::DoCreate(DocCoord &Start, Spread* pSpread, AttrFillGeometry* pGrad)
{
	ERROR3IF(pGrad == NULL, "Fill pointer is NULL in OpEditFill::DoCreate()");

	if (pGrad == NULL)
	{
		// Nothing to create !!
		FailAndExecute();
		End();
		return;
	}

	// DMc - do the attribute to change
	m_pAttr = (NodeAttribute *)pGrad;

	AlwaysFail = FALSE;
	
	// DMc - set this to TRUE
	ApplyAtEnd = FALSE;
	ForceAspectLock = TRUE;
	CreateFill = TRUE;

	// Get a description of the attribute being applied so that we can use it to create the 
	// undo string. 
	UndoAttribStrID = pGrad->GetAttrNameID();
	AttrFillGeometry::HitList.DeleteAll();

	SelRange* pSel = GetApplication()->FindSelection(); 
	if (pSel && pSel->Count() > 0)
	{
		// Check it's ok to apply to this object
		ObjChangeFlags cFlags;
		cFlags.Attribute = TRUE;
		ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);
ObjChange.SetRetainCachedData(TRUE);		// Don't let nodes releasecached data - we will handle that!
		if (!pSel->AllowOp(&ObjChange))
		{
			delete pGrad;
			FailAndExecute();
			End();
			return;
		}
	}

	AttrFillGeometry* pFillToEdit = pGrad;

	// Now check to make sure someone in the selection will accept this attribute
	if (AttributeManager::CanBeAppliedToSelection(pGrad, &AttrGroups))
	{
		// We're gunna apply to at least one object
		IsRequired = TRUE;

		// Flag that we need to apply the fill, either when we try
		// and do an interective redraw, or at the end
		ShouldApplyNewFill = TRUE;

		// Remove all the existing selection fill blobs
		DocRect SelBounds = pSel->GetBlobBoundingRect();
		RenderInitSelectionBlobs(pSel, SelBounds, pSpread);
	}
	else
	{
		// This attribute is not required by any of the selection,
		// but we will continue, without actually applying anything,
		// and set the 'current' attribute instead
		IsRequired = FALSE;
		AttributeManager::pLastNodeAppliedTo = NULL;
	}

	// Snap the actual mouse position to the grid if needed
	DocView::SnapSelected(pSpread, &Start);

	// Initialise the Control Points
	pFillToEdit->SetStartPoint(&Start);
	pFillToEdit->SetEndPoint(&Start);
	pFillToEdit->SetEndPoint2(&Start);

	// Call the edit code
	DoDrag(Start, pSpread, pFillToEdit, FILLCONTROL_ENDPOINT);
}

/********************************************************************************************

>	void OpEditFill::DoDrag(DocCoord &Start, DocCoord &End, Spread* pSpread, AttrFillGeometry* pGrad, INT32 BlobHit)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		Start - The starting position of the drag
				pSpread - The spread that the drag started over
				pElip - The NodeGradFill that is being edited
	Purpose:	-
	Notes:		Phil, 16-09-2005
				This code relies on the attributea pplication code using DoHideNode and DoShowNode
				to record changes to the attributes. By recording the attributes in this way when
				they are first applied, the undo history contains pointers to them and the 
				dragging code can then modify those attributes in the knowledge that the op
				history is now pointing at the modified attributes.
				It should really be changed to use OpApplyAttrInteractive. That would reduce
				the complexity of the following functions enormously and would be more
				consistent, smaller and possibly faster.
				There are too many different ways of "solid dragging" things and they should all:
				* Record current tree state at start of drag
				* Modify tree during drag
				* Produce permanent Undo information at the end, using the same logic as would
				  be used in the simple, parametric or outline drag case

********************************************************************************************/

void OpEditFill::DoDrag(DocCoord &Start, Spread* pSpread, AttrFillGeometry* pGrad, FillControl ControlHit)
{
	ERROR3IF(ControlHit > NUMCONTROLPOINTS && 
		!ISA_RAMPINDEX(ControlHit), "Invalid control point in OpEditFill::DoDrag()");
	ERROR3IF(pGrad == NULL, "Fill pointer is NULL in OpEditFill::DoDrag()");

	if (pGrad == NULL)
	{
		// Nothing to edit !!
		FailAndExecute();
		End();
		return;
	}

	// Various starting positions
	StartSpread = pSpread;
	DragControl = ControlHit;

	// Keep a copy of the old fill, so we can put it back later if necessary
	CCRuntimeClass* ObjectType = pGrad->GetRuntimeClass();
	FillClone = (AttrFillGeometry*)ObjectType->CreateObject();
	*FillClone = *pGrad;

	// and make another copy to actually edit
	ObjectType = pGrad->GetRuntimeClass();
	GradFill = (AttrFillGeometry*)ObjectType->CreateObject();
	*GradFill = *pGrad;

	pParentOfFill = (NodeRenderableInk*) pGrad->FindParent ();
	pTheFill = pGrad;

	if (CreateFill)
		delete pGrad;	// Not needed any more

	AspectRatio = FindAspectRatio();

	AnchorPos = Start;
	LastMousePosition = Start;

	DontDrawBlobs = FALSE;
	DragIsIdle = FALSE;
	DoneIdleRedraw = FALSE;

//	ShowDragBlobs = ContinuousEOR;
	ShowDragBlobs = !DocView::SolidDragging;

	if (!IsRequired)			// Always show blobs if we're gunna
		ShowDragBlobs = TRUE;	// set the current attribute

	AttrFillGeometry::DraggedFill = NULL;
	AttrFillGeometry::EditedFill  = GradFill;

	// And tell the Dragging system that we need drags to happen
	StartDrag(DRAGTYPE_AUTOSCROLL, NULL, NULL, TRUE, TRUE);			// We support solid dragging
}
	
/********************************************************************************************

>	void OpEditFill::DragPointerMove(DocCoord PointerPos, ClickModifiers ClickMods,
										Spread* pSpread, BOOL bSolidDrag)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		PointerPos - The current position of the mouse
				ClickMods - Which buttons and modifiers are held down
				pSpread - The Spread that the mouse is over now.
	Purpose:	Recalculates the GradFill according to the new position of the corner that
				is being dragged, EORs it to the screen and keeps the Parallelogram up
				to date.

********************************************************************************************/

void OpEditFill::DragPointerMove(DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL bSolidDrag)
{
	if (ISA_RAMPINDEX(DragControl) && !bSolidDrag)
	{
		GradFill->DisableRampRedraw();
	}
	
	if (EditFinished)
		return;

	if (ForceAspectLock)
		ClickMods.Adjust = TRUE;

	DragIsIdle = FALSE;
	DoneIdleRedraw = FALSE;

	// Snap the actual mouse position to the grid if needed
	DocView::SnapSelected(pSpread, &PointerPos);

	if (AttrFillGeometry::DraggedFill == NULL)
	{
		double APixel = (DocView::GetSelected()->GetScaledPixelWidth()).MakeDouble();

		// If the pointer has moved less than a few pixels, then do nothing
		if (PointerPos.Distance(AnchorPos) <= APixel*2)
		{
			if (!(ISA_RAMPINDEX(DragControl)))
			{
				return;
			}
		}

		// This is the first time the pointer has moved ....
		if (!CreateFill)
		{
			// If we are editing a fill, the check with it's parent
			// to make sure it's ok (Mould's will say 'no').

			ListItem* pAttrPtr = AttrFillGeometry::HitList.GetHead();
			AttrFillGeometry* pGrad = (AttrFillGeometry*)((NodeAttributePtrItem*)pAttrPtr)->NodeAttribPtr;

			if (pGrad != NULL)
			{
				Node* pParent = pGrad->FindParent();

				if (pParent != NULL)
				{
					// Check it's ok to edit this fill
					ObjChangeFlags cFlags;
					cFlags.Attribute = TRUE;
					ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);
ObjChange.SetRetainCachedData(TRUE);

					if (!pParent->AllowOp(&ObjChange))
					{
						// Parent said 'no', so abort the edit
	
						EndDrag();
						delete FillClone;
						delete GradFill;

						FailAndExecute();
						End();
						return;
					}
				}
			}
		}

		AttrFillGeometry::EditedFill  = GradFill;

		AttrFillGeometry::LastRenderedStartBlob = DocCoord(0,0);
		AttrFillGeometry::LastRenderedEndBlob = DocCoord(0,0);
		AttrFillGeometry::LastRenderedEnd2Blob = DocCoord(0,0);
		AttrFillGeometry::LastRenderedEnd3Blob = DocCoord(0,0);

		// First Rub out the old Fill blobs
		RenderInitBlobs(GradFill->GetBlobBoundingRect(), StartSpread, bSolidDrag);

		GradFill->SetBlobState(DragControl, TRUE);

		// And stop any more from being drawn for now
		AttrFillGeometry::DraggedFill = GradFill;

		LastDragRect = GradFill->GetBlobBoundingRect();
		DontDrawBlobs = !ShowDragBlobs;

		// Now draw in the new 'Drag' blobs
		RenderDragBlobs(LastDragRect, StartSpread, bSolidDrag);

		// Update the status line to show we are dragging a fill
		String_256 DragText;
		GetStatusLineText(&DragText, pSpread, PointerPos, ClickMods);
		GetApplication()->UpdateStatusBarText(&DragText);
	}

	// If the mouse is in a different position then do something
	if (PointerPos != LastMousePosition)
	{
		// First Rub out the old Drag blobs
		RenderDragBlobs(LastDragRect, StartSpread, bSolidDrag);

		// Make sure that the coords are relative to the correct spread
		if (pSpread != StartSpread)
			PointerPos = MakeRelativeToSpread(StartSpread, pSpread, PointerPos);

		// Update the last mouse position and re-calc the bounding rect
		GradFill->OnControlDrag(PointerPos, DragControl, ClickMods);

		LastMousePosition = PointerPos;

		// If we are creating a fill, then we'll try and create it with the
		// correct aspect ratio
		if (ForceAspectLock)
			GradFill->SetAspectRatio(AspectRatio);

		LastDragRect = GradFill->GetBlobBoundingRect();
		DontDrawBlobs = !ShowDragBlobs;

		// Render 'On' the blobs in the new position
		RenderDragBlobs(LastDragRect, StartSpread, bSolidDrag);

		if (DontDrawBlobs)
			RedrawFills(ClickMods);		// We're in 'Solid update' mode
	}

	if (ISA_RAMPINDEX(DragControl))
	{
		GradFill->EnableRampRedraw ();
	}
}

/********************************************************************************************

>	double OpEditFill::FindAspectRatio()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95
	Purpose:	Find the aspect ratio of the bitmap we are creating.

********************************************************************************************/

double OpEditFill::FindAspectRatio()
{
	// Are we creating a Bitmap fill ?
	if (!GradFill->IsABitmapFill())
		return 1;

	SelRange* pSel = GetApplication()->FindSelection();

	NodeAttribute* pCommonAttr;
	SelRange::CommonAttribResult Result;
	
	// Find the common attr in the selection
	Result = pSel->FindCommonAttribute(GradFill->GetAttributeType(), &pCommonAttr);

	KernelBitmap* Bmp = NULL;

	if (Result == SelRange::ATTR_COMMON)
	{
		// There is a common attr, so if it's a Bitmap Fill,
		// then use that bitmaps aspect ratio
		AttrFillGeometry* pFill = (AttrFillGeometry*)pCommonAttr;

		if (pFill->IsABitmapFill())
			Bmp = pFill->GetBitmap();

		GradFill->AttachBitmap(Bmp);
	}
	else
	{
		// Otherwise just use the default
		Bmp = GradFill->GetBitmap();
	}

	if (Bmp == NULL)
		return 1;

	BitmapInfo Info;

	if (Bmp->ActualBitmap &&
		Bmp->ActualBitmap->GetInfo( &Info ))
	{
		INT32 BitmapWidth  = Info.RecommendedWidth;
		INT32 BitmapHeight = Info.RecommendedHeight;

		// Calculate the aspect ratio of the bitmap
		return double(BitmapHeight)/double(BitmapWidth);
	}

	// Default to square
	return 1;
}

/********************************************************************************************

>	void OpEditFill::RedrawFills(ClickModifiers& ClickMods)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95
	Purpose:	Forces a redraw on the edited fills.

********************************************************************************************/

void OpEditFill::RedrawFills(ClickModifiers& ClickMods)
{
	if (ClickMods.Alternative1)
		return;			// Alt key stops interactive updates

	// Prevent slow renderers from doing their stuff while we try to
	// show the results of the feather op interactively.
	Operation::SetQuickRender(TRUE);

	if (ShouldApplyNewFill)
	{
		// If we haven't applied the new fill yet, then we should
		// do so now.
		ApplyNewFill();
	}

	if (CheckForCompound)
	{
		// If we haven't already done so, we check to see if we need
		// to invalidate any compound parents.
		CheckForCompoundRedraw();
	}

	if (!AttrFillGeometry::HitList.IsEmpty())
	{
		DocView* pSelView = DocView::GetSelected();

		// Scan the List of Fills
		ListItem* pAttrPtr = AttrFillGeometry::HitList.GetHead();

		DocRect SelBounds = GetApplication()->FindSelection()->GetBoundingRect();

		while (pAttrPtr != NULL)
		{
			AttrFillGeometry* pAttr = (AttrFillGeometry*)((NodeAttributePtrItem*)pAttrPtr)->NodeAttribPtr;
			ERROR3IF(pAttr->IsADefaultAttr(), "Default attr in hit list");

			if (pAttr != GradFill)
			{
				if (ISA_RAMPINDEX(DragControl))
				{
					AttrFillGeometry* CurrentFill = AttrFillGeometry::DraggedFill;
					
					AttrFillGeometry::DraggedFill = pAttr;
					DocRect Current = pAttr->GetBlobBoundingRect ();
					SelBounds = SelBounds.Union (Current);

					AttrFillGeometry::DraggedFill = CurrentFill;
				}
				
				// Update the control points on the other fills
				pAttr->SetStartPoint(GradFill->GetStartPoint());
				pAttr->SetEndPoint(GradFill->GetEndPoint());
				pAttr->SetEndPoint2(GradFill->GetEndPoint2());
				pAttr->SetColourRamp(GradFill->GetColourRamp());
				//pAttr->DisableRampRedraw ();

				pAttr->RecalcFractal();
			}

			if (!InvalidateAll)
			{
				// Force the fills parent object to redraw
				NodeRenderableInk* pParent = (NodeRenderableInk*)pAttr->FindParent();
				ERROR3IF(pParent==NULL, "Can't find parent of edited attribute");
				if (pParent && pAttr->IsEffectAttribute())	// IsValidEffectAttr?
				{
//					Node* pGrandParent = pParent->FindParent();
//					if (pGrandParent && pGrandParent->IsBounded())
//						((NodeRenderableBounded*)pGrandParent)->ReleaseCached(TRUE, FALSE);	// Parents only
					pParent->ReleaseCached(TRUE, FALSE, FALSE, TRUE);	// Parents and derived data only
				}
				else
					pParent->ReleaseCached();
				pSelView->ForceRedraw(StartSpread, pParent->GetBoundingRect(), FALSE, pAttr);
				GetApplication ()->ServiceRendering ();
			}

			pAttrPtr = AttrFillGeometry::HitList.GetNext(pAttrPtr);
		}

		if (InvalidateAll)
		{
			// Either there were too many objects to invalidate individually,
			// or there is a Compound Parent to invalidate
			if (InvalidateCompound)
			{
				pAttrPtr = AttrFillGeometry::HitList.GetHead();
				AttrFillGeometry* pGrad = (AttrFillGeometry*)((NodeAttributePtrItem*)pAttrPtr)->NodeAttribPtr;

				if (CallAllowOp && !pGrad->IsEffectAttribute())
				{
					// For a blend, we need to make sure it re-blends itself,
					// so we need to call AllowOp on it ...

					ObjChangeFlags cFlags;
					cFlags.Attribute = TRUE;		// Needed to make Blends re-calc themselves

					ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL, this);

					pGrad->AllowOp(&ObjChange);

					ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL, this);
					UpdateAllChangedNodes(&ObjChange);
				}

				if (ISA_RAMPINDEX (DragControl))
				{
					GradFill->DisableBoundsRedraw();
				}

				NodeRenderableInk* pParent = (NodeRenderableInk*)pCompound;
				ERROR3IF(pParent==NULL, "Can't find parent of edited attribute");
				if (pParent && pGrad->IsEffectAttribute())	// IsValidEffectAttr?
				{
//					Node* pGrandParent = pParent->FindParent();
//					if (pGrandParent && pGrandParent->IsBounded())
//						((NodeRenderableBounded*)pGrandParent)->ReleaseCached(TRUE, FALSE);	// Parents only
					pParent->ReleaseCached(TRUE, FALSE, FALSE, TRUE);	// Parents and derived data only
				}
				else
					pParent->ReleaseCached();
				pSelView->ForceRedraw(StartSpread, pParent->GetBoundingRect(), FALSE, pGrad);
				GetApplication ()->ServiceRendering();

				if (ISA_RAMPINDEX (DragControl))
				{
					GradFill->EnableBoundsRedraw();
				}
			}
			else
			{			
				if (ISA_RAMPINDEX (DragControl))
				{
					GradFill->DisableBoundsRedraw();
				}
				
				// Oh sod it. Just invalidate the whole blinking lot of them,
				// and be done with it.
				//DocRect SelBounds = GetApplication()->FindSelection()->GetBoundingRect();

				// Scan the List of Fills and ensure that none of them are cached...
				ListItem* pAttrPtr = AttrFillGeometry::HitList.GetHead();
				Node* pBackMost = NULL;
				while (pAttrPtr != NULL)
				{
					AttrFillGeometry* pAttr = (AttrFillGeometry*)((NodeAttributePtrItem*)pAttrPtr)->NodeAttribPtr;

					// Force the fills parent object to redraw
					NodeRenderableInk* pParent = (NodeRenderableInk*)pAttr->FindParent();
					ERROR3IF(pParent==NULL, "Can't find parent of edited attribute");
					if (pParent && pAttr->IsEffectAttribute())	// IsValidEffectAttr?
					{
//						Node* pGrandParent = pParent->FindParent();
//						if (pGrandParent && pGrandParent->IsBounded())
//							((NodeRenderableBounded*)pGrandParent)->ReleaseCached(TRUE, FALSE);	// Parents only
						pParent->ReleaseCached(TRUE, FALSE, FALSE, TRUE);	// Parents and derived data only
					}
					else
						pParent->ReleaseCached();

					// Work out the backmost attr
					if (pBackMost==NULL || pAttr->IsUnder(pBackMost))
						pBackMost = pAttr;

					pAttrPtr = AttrFillGeometry::HitList.GetNext(pAttrPtr);
				}

				pSelView->ForceRedraw(StartSpread, SelBounds, FALSE, pBackMost);

				GetApplication()->ServiceRendering();

				if (ISA_RAMPINDEX (DragControl))
				{
					GradFill->EnableBoundsRedraw();
				}
			}
		}
	}

	// Re-enable slow renderers
	Operation::SetQuickRender(FALSE);
}

/********************************************************************************************

>	void OpEditFill::CheckForCompoundRedraw()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95
	Purpose:	Check to see if the redraw needs to invalidate a compound parent.
				(eg. a blend).

********************************************************************************************/

void OpEditFill::CheckForCompoundRedraw()
{
	CheckForCompound = FALSE;

	pFillsDoc = StartSpread->FindOwnerDoc();

	if (AttrFillGeometry::HitList.GetCount() == GetApplication()->FindSelection()->Count() ||
		AttrFillGeometry::HitList.GetCount() > 10)
	{
		// If we are dragging all selected fills, or if there are
		// more than 10 of them, then we'll just invalidate the 
		// whole selection, 'cus it's much quicker
		InvalidateAll = TRUE;
	}
	else
	{
		// We will invalidate just the edited fills
		InvalidateAll = FALSE;
	}

 	InvalidateCompound = FALSE;

	if (AttrFillGeometry::HitList.GetCount() == 1)
	{
		// If just one Fill is being dragged, then check to see if
		// it's a 'Select Inside' of a Compound object.

		// If it is, then we'll need to update the whole compound parent.

		ListItem* pAttrPtr = AttrFillGeometry::HitList.GetHead();
		AttrFillGeometry* pGrad = (AttrFillGeometry*)((NodeAttributePtrItem*)pAttrPtr)->NodeAttribPtr;

		Node* pParent = pGrad->FindParent();
		if (pParent == NULL)
		{
			InvalidateAll = TRUE;
			return;
		}
	
		if (pParent->IsCompound())
		{
			// The parent is compound, so we'll just update that
		 	InvalidateAll = TRUE;
		 	InvalidateCompound = TRUE;
			pCompound = pParent;
		}

		// Is the parent a blend object, or is there one
		// further up the tree ?
		while (pParent != NULL && !pParent->IsLayer())
		{
			if (pParent->IS_KIND_OF(NodeBlend) || pParent->ShouldITransformWithChildren())
			{
				// Found a blend, so we'll have to invalidate it
				// each time we want to re-draw
			 	InvalidateAll = TRUE;
			 	InvalidateCompound = TRUE;
				pCompound = pParent;

				// We will call allowop, to invalidate the blend
				CallAllowOp = TRUE;

				// Don't break out yet, 'cus we need to invalidate
				// the highest blend in the tree
			}
			
			pParent = pParent->FindParent();
		}
	}
}

/********************************************************************************************

>	void OpEditFill::DragPointerIdle(DocCoord PointerPos, ClickModifiers ClickMods,
										Spread* pSpread, BOOL bSolidDrag)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		PointerPos - The current position of the mouse
				ClickMods - Which buttons and modifiers are held down
				pSpread - The Spread that the mouse is over now.
	Purpose:	Recalculates the GradFill according to the new position of the corner that
				is being dragged, EORs it to the screen and keeps the Parallelogram up
				to date.

********************************************************************************************/

void OpEditFill::DragPointerIdle(DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL bSolidDrag)
{	
	if (!DragIsIdle)
	{
		// The Mouse has just become idle, so we'll reset the timer.
		DragIsIdle = TRUE;
		DoneIdleRedraw = FALSE;
		Time.Sample();
	}		
	else
	{
		// Has the Mouse been idle for long enough ?
		if (Time.Elapsed(IdleFillDelay))
		{
			if (DontDrawBlobs)
			{
				// If we are not redrawing the EOR blobs constantly,
				// then we will stick them back on when the mouse
				// has been idle for a while.
				DontDrawBlobs = FALSE;
				DragIsIdle = FALSE;
				RenderDragBlobs(LastDragRect, StartSpread, bSolidDrag);
			}
			else
			{
				// If we ARE redrawing the blobs, then we'll try and redraw
				// the fills when the mouse has been idle for a while.
				if (InteractiveDragUpdate && !DoneIdleRedraw)
				{
					RedrawFills(ClickMods);
					DoneIdleRedraw = TRUE;
				}
			}
		}
	}
}

/********************************************************************************************

>	BOOL OpEditFill::DragKeyPress(KeyPress* pKeyPress, BOOL bSolidDrag)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/95
	Inputs:		pKeyPress - Object describing the nature of the keypress.
	Purpose:	Allows the Fill Edit operation to respond to key presses.

********************************************************************************************/

BOOL OpEditFill::DragKeyPress(KeyPress* pKeyPress, BOOL bSolidDrag)
{
	if (pKeyPress->GetVirtKey() == CAMKEY(TAB) && 
		pKeyPress->IsPress() &&
		!pKeyPress->IsRepeat())
	{
		// Toggle the continuous EOR flag
		ContinuousEOR = !ContinuousEOR;
		ShowDragBlobs = ContinuousEOR;

		String_256 DragText;
		ClickModifiers ClickMods;

		// Now update the Status line to show the mode change
		GetStatusLineText(&DragText, StartSpread, LastMousePosition, ClickMods);
		GetApplication()->UpdateStatusBarText(&DragText);
		
		return TRUE;
	}
	return FALSE;
}




/********************************************************************************************

>	void OpEditFill::DragModeChanged()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/12/2003
	Inputs:		pKeyPress - pointer to a keypress object
	Returns:	TRUE if it handled the keypress, FALSE otherwise
	Purpose:	Find out if any of the key modifiers are being pressed when the mouse is
				not moving.

********************************************************************************************/

void OpEditFill::DragModeChanged(BOOL bSolidDrag)
{
/*	DocView* pDocView = DocView::GetSelected();
	BlobManager* pBlobManager = Camelot.GetBlobManager();
	ENSURE(pBlobManager, "Can't get BlobManager");

	// Toggle the continuous EOR flag
	ContinuousEOR = !bSolidDrag;
	ShowDragBlobs = ContinuousEOR;

	String_256 DragText;
	ClickModifiers ClickMods;

	// Now update the Status line to show the mode change
	GetStatusLineText(&DragText, StartSpread, LastMousePosition, ClickMods);
	GetApplication()->UpdateStatusBarText(&DragText);
*/
}




/********************************************************************************************

>	void OpEditFill::DragFinished(DocCoord PointerPos, ClickModifiers ClickMods,
									Spread* pSpread, BOOL Success, BOOL bSolidDrag)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		PointerPos - The current position of the mouse
				ClickMods - Which buttons and modifiers are held down
				pSpread - The Spread that the mouse is over now.
				Success - TRUE if the drag was completed sucessfully.
	Purpose:	If the drag was a success then a copy of the original node in the tree is
				created and updated GradFill built. The original NodeGradFill is hidden and
				the new one if inserted into the tree. If any of these things fail then
				the operation will fail.

********************************************************************************************/

void OpEditFill::DragFinished(DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL Success, BOOL bSolidDrag)
{	
	EditFinished = TRUE;
	
	double APixel;

	// Flag to say if everything has worked
	BOOL IsOk = FALSE;

	// First Rub out the old Drag blobs
	if (!DontDrawBlobs && AttrFillGeometry::DraggedFill != NULL)
	{
		if (!(ISA_RAMPINDEX(DragControl)))
		{
			RenderDragBlobs(GradFill->GetBlobBoundingRect(), StartSpread, bSolidDrag);
		}
		else
		{
			if (ISA_RAMPINDEX (DragControl))
			{
				GradFill->DisableBoundsRedraw ();
			}
			RenderDragBlobs(GradFill->GetBlobBoundingRect(), StartSpread, bSolidDrag);
			if (ISA_RAMPINDEX (DragControl))
			{
				GradFill->EnableBoundsRedraw ();
			}
		}
	}

	DontDrawBlobs = FALSE;
	BOOL HaveFailed = FALSE;

	Document* pDoc = Document::GetSelected();

	// check for a fill ramp drag
	if (ISA_RAMPINDEX(DragControl))
	{
		if (Success)
		{
			if (GradFill->GetColourRamp())
			{
				GradFill->GetColourRamp()->SortRamp();
			}
		}
	}

	// End the Drag                             
	if (!EndDrag())
	{
		delete FillClone;
		delete GradFill;
		goto EndOperation;
	}

	if (*GradFill->GetStartPoint() == *GradFill->GetEndPoint())
		Success = FALSE;
	
	APixel = (DocView::GetSelected()->GetScaledPixelWidth()).MakeDouble();

	// If the pointer has moved less than a few pixels, then just do a click
	if (PointerPos.Distance(AnchorPos) <= APixel*2)
		Success = FALSE;

	// If PointerPos is equal to AnchorPos, then it must have just been a click
	if (!Success || (PointerPos == AnchorPos))
	{
		if (AttrFillGeometry::DraggedFill)
		{
			// We must have actually dragged a bit,
			// so make sure we tidy everying, and then render
			// the old fill blobs back on
			AttrFillGeometry::DraggedFill = NULL;
			AttrFillGeometry::EditedFill = NULL;

			// Failed, so put all the fills back like they were,
			// if we have changed them.
			if (!(*GradFill == *FillClone))
			{
				*GradFill = *FillClone;

				ListItem* pAttrPtr = AttrFillGeometry::HitList.GetHead();
				while (pAttrPtr != NULL)
				{
					AttrFillGeometry* pAttr = (AttrFillGeometry*)((NodeAttributePtrItem*)pAttrPtr)->NodeAttribPtr;
ERROR3IF(pAttr->IsADefaultAttr(), "Default attr in hit list");

					pAttr->SetStartPoint(FillClone->GetStartPoint());
					pAttr->SetEndPoint(FillClone->GetEndPoint());
					pAttr->SetEndPoint2(FillClone->GetEndPoint2());

					NodeRenderableInk* pParent = (NodeRenderableInk*)pAttr->FindParent();
					ERROR3IF(pParent==NULL, "Can't find parent of edited attribute");
					if (pParent && pAttr->IsEffectAttribute())	// IsValidEffectAttr?
					{
//						Node* pGrandParent = pParent->FindParent();
//						if (pGrandParent && pGrandParent->IsBounded())
//							((NodeRenderableBounded*)pGrandParent)->ReleaseCached(TRUE, FALSE);	// Parents only
						pParent->ReleaseCached(TRUE, FALSE, FALSE, TRUE);	// Parents and derived data only
					}
					else
						pParent->ReleaseCached();

					pAttrPtr = AttrFillGeometry::HitList.GetNext(pAttrPtr);
				}

				// Redraw the old fills back
				DocRect SelBounds = GetApplication()->FindSelection()->GetBlobBoundingRect();

				DocView::GetSelected()->ForceRedraw(StartSpread, SelBounds);
			}

		}
		else
		{
			// We didn't drag anything, so it must have just been a click
			if (ClickMods.Adjust && ClickMods.Constrain)
			{
				// Cntrl/Shift/Click will select all Start or End blobs
				SelectAllBlobs();
			}

			// Tell the world that a fill blob has been selected
			BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::COLOURATTCHANGED)); 
		}

		AttrFillGeometry::LastRenderedMesh = NULL;
		AttrFillGeometry::HitList.DeleteAll();

		delete FillClone;
		delete GradFill;
		goto EndOperation; 		// This will fail the op
	}

	// We have edited the fill ok, so we'll draw the blobs back on
	AttrFillGeometry::DraggedFill = NULL;

	if (IsRequired)
		RenderFinalBlobs(GetDragBlobRect(), StartSpread, bSolidDrag);

	if (AlwaysFail)
	{
		// If we just edited a fill, then we don't want any undo info,
		// so we'll fail the op now (and discard the Actions), before we
		// apply the edited fill
		FailAndExecute();
		HaveFailed = TRUE;	// no need to fail at the end
	}

	// if the drag was a sucess then make a grad fill
	if (Success && (!GetDragBlobRect().IsEmpty()))
	{
		AttrFillGeometry::EditedFill = NULL;
		AttrFillGeometry::DraggedFill = NULL;
	
		if (ApplyAtEnd)
		{
			// Update all the edited fills
			ApplyEditedFill();

			delete FillClone;
			delete GradFill;
		}
		else
		{
			if (ShouldApplyNewFill)
			{
				ApplyNewFill();
			}

			BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::COLOURATTCHANGED)); 
			BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::NONCOLOURATTCHANGED)); 

			SelRange *Selection = Camelot.FindSelection();
			DocRect Bounds;
			if (Selection != NULL && Selection->Count()>0)
				Bounds = Selection->GetBoundingRect();
			else
				Bounds = GetDragBlobRect();

			GradFill->SetBoundingRect(Bounds);

			// Ensure we update all the fills
			if (!AttrFillGeometry::HitList.IsEmpty())
			{
				ListItem* pAttrPtr = AttrFillGeometry::HitList.GetHead();

				while (pAttrPtr != NULL)
				{
					AttrFillGeometry* pAttr = (AttrFillGeometry*)((NodeAttributePtrItem*)pAttrPtr)->NodeAttribPtr;
ERROR3IF(pAttr->IsADefaultAttr(), "Default attr in hit list");

					// Update the control points on the other fills
					pAttr->SetStartPoint(GradFill->GetStartPoint());
					pAttr->SetEndPoint(GradFill->GetEndPoint());
					pAttr->SetEndPoint2(GradFill->GetEndPoint2());

					// Give the attribute a bounding rect
					pAttr->SetBoundingRect(Bounds);

					// We need to ensure that we force a redraw at the end of the drag
					// because redraws during the drag will have been done with QuickRender turned
					// on and so some slow objects may not have rendered themselves
					// fully.
					if (bSolidDrag)
					{
						Node* pParent = pAttr->FindParent();
						if (pParent && pParent->IsBounded())
						{
((NodeRenderableBounded*)pParent)->ReleaseCached(TRUE, FALSE, !pAttr->IsEffectAttribute(), TRUE);
							DoInvalidateNodeRegion((NodeRenderableBounded*)pParent, FALSE, TRUE, FALSE, FALSE);	// Do not recache
							// (SelOperation::DoApply will have called ReleaseCached as needed)
						}
					}

					pAttrPtr = AttrFillGeometry::HitList.GetNext(pAttrPtr);
				}
			}
			AttributeManager& AttrMgr = pDoc->GetAttributeMgr();

			if (AttrMgr.WeShouldMakeAttrCurrent(IsRequired,GradFill,&AttrGroups))
			{
				AttributeManager::UpdateCurrentAttr(GradFill, TRUE, &AttrGroups);
			}

			delete FillClone;
			delete GradFill;
		}

		IsOk = TRUE;
	}

EndOperation:

	// DMc - should inform all nodes at the end of a drag too
	AttrFillGeometry::EditedFill = NULL;
	AttrFillGeometry::DraggedFill = NULL;
	AttrFillGeometry::HitList.DeleteAll();
	CreateFill = FALSE;

	// Don't forget the AttrGroups List
	AttrGroups.DeleteAll();

	// If something went wrong, then fail (if we haven't already)
	if (!IsOk && !HaveFailed)
	{
		FailAndExecute();
	}

	// always call end
	End();
}

/********************************************************************************************

>	BOOL OpEditFill::ApplyNewFill()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/95
	Returns:	TRUE if fill was applied ok.
	Purpose:	Applies the newly created fill to the selection.

********************************************************************************************/

BOOL OpEditFill::ApplyNewFill()
{
	ShouldApplyNewFill = FALSE;

	SelRange* pSel = GetApplication()->FindSelection();
	Range SelRng(*pSel);
	// >>Webster
	BOOL bShowProgress = TRUE;
#ifndef EXCLUDE_FROM_XARALX	
	if (GetKeyState(CAMKEY(LBUTTON)) & 0x80)
		bShowProgress = FALSE;
#endif
	// << Webster

	if (bShowProgress)
		BeginSlowJob(-1, FALSE);

	ObjChangeFlags cFlags;
	cFlags.Attribute = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);
	ObjChange.SetRetainCachedData(TRUE);

	if (!pSel->AllowOp(&ObjChange))
	{
		if (bShowProgress)
			EndSlowJob();
		return FALSE;
	}

	DoStartSelOp(FALSE,FALSE, TRUE,TRUE);

	// Before we apply the attribute to the selection we must localise all attributes
	// with the same type that we are going to apply. If we don't do this then the
	// tree will be left in an invalid state. 

	AttrTypeSet AttrTypes; 
	AttrTypes.AddToSet((GradFill->GetAttributeType())); 

	// Invalidate before
	if (!DoInvalidateRegions(&SelRng, 
							 GradFill, 
							 TRUE,
							 NULL,
							 FALSE))  //Mutators have to include bounds
	{
		if (bShowProgress)
			EndSlowJob();
		return FALSE;
	}
	
	if (!DoLocaliseForAttrChange(&SelRng, &AttrTypes))
	{
		if (bShowProgress)
			EndSlowJob();
		return FALSE;
	}

	// Apply the new fill to the selection
	//GradFill->SetColourRamp (
	
	//pAttr->SetColourRamp ((ColourRamp*) GradFill->GetFillRamp ());
	ApplyToSelection(GradFill, TRUE);

	// Having applied the attributes, we  must now try and factor out the newly 
	// applied attributes
	if (!DoFactorOutAfterAttrChange(&SelRng, 
									&AttrTypes))
	{
		if (bShowProgress)
			EndSlowJob();
		return FALSE;
	}

	// Invalidate after
	if (!DoInvalidateRegions(&SelRng, 
							 GradFill, 
							 TRUE,
							 NULL,
							 FALSE))  //Mutators have to include bounds
	{
		if (bShowProgress)
			EndSlowJob();
		return FALSE;
	}

	AttrTypes.DeleteAll();

	if (Document::GetSelected())
		Document::GetSelected()->SetModified(TRUE);

	ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
ObjChange.SetRetainCachedData(TRUE);	// Tell TextStory not to invalidate everything in OnChildChange
	if (!UpdateChangedNodes(&ObjChange))
	{
		if (bShowProgress)
			EndSlowJob();
		return FALSE;
	}

	// Get the 'actual' fills back, so we can continue to drag them
	AttrFillGeometry* pFill = AttrFillGeometry::FindFirstSelectedAttr(GradFill->GetAttributeType());
//	AttrFillGeometry* pFillToEdit = pFill;

	// Build a list of them
	while (pFill)
	{
		BOOL InList = FALSE;

		// We may have already hit this attribute if the selection is
		// inside a parent and have common attributes, so we need to
		// check and make sure this attribute is NOT in the list
		// already.

		if (!AttrFillGeometry::HitList.IsEmpty())
		{
			ListItem* pItem = AttrFillGeometry::HitList.GetHead();

			while (pItem)
			{
				NodeAttributePtrItem* NodePtr = (NodeAttributePtrItem*)pItem;

				if (NodePtr->NodeAttribPtr == pFill)
				{
					// Ignore this one, we've hit it already
				 	InList = TRUE;
					break;
				}

				pItem = AttrFillGeometry::HitList.GetNext(pItem);
			}
		}

		if (!InList)
		{
			// Add this to the list of fills to edit
			ListItem* pAttr = new NodeAttributePtrItem;
			if (pAttr == NULL)
			{
				if (bShowProgress)
					EndSlowJob();
				return FALSE;
			}

			ERROR3IF(pFill->IsADefaultAttr(), "Default attr in hit list");
			((NodeAttributePtrItem*)pAttr)->NodeAttribPtr = pFill;

			AttrFillGeometry::HitList.AddTail(pAttr);
		}

		pFill = AttrFillGeometry::FindNextSelectedAttr(GradFill->GetAttributeType());
	}

	if (bShowProgress)
		EndSlowJob();

	return TRUE;
}

/********************************************************************************************

>	BOOL OpEditFill::ApplyEditedFill()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/95
	Returns:	TRUE if fill was applied ok.
	Purpose:	Applies the edited fill, back to the editing objects.

********************************************************************************************/

BOOL OpEditFill::ApplyEditedFill()
{
	// Put all the fills back like they were so undo will work
	ListItem* pAttrPtr = AttrFillGeometry::HitList.GetHead();
	while (pAttrPtr != NULL)
	{
		AttrFillGeometry* pAttr = (AttrFillGeometry*)((NodeAttributePtrItem*)pAttrPtr)->NodeAttribPtr;

		if (pAttr != GradFill)
		{
			pAttr->SetStartPoint(FillClone->GetStartPoint());
			pAttr->SetEndPoint(FillClone->GetEndPoint());
			pAttr->SetEndPoint2(FillClone->GetEndPoint2());
			pAttr->SetColourRamp(FillClone->GetColourRamp());
		}

		pAttrPtr = AttrFillGeometry::HitList.GetNext(pAttrPtr);
	}

	SelRange *Selection = Camelot.FindSelection();

	DocRect Bounds;
	if (Selection != NULL && Selection->Count()>0)
		Bounds = Selection->GetBoundingRect();
	else
		Bounds = GetDragBlobRect();

	// Give the attribute a bounding rect
	GradFill->SetBoundingRect(Bounds);

	BeginSlowJob(-1, FALSE);
	AttributeManager::SendMessages = FALSE;

	// Now replace all the edited attributes with the new one
	AttributeManager::ReplaceAttributes(GradFill, &AttrFillGeometry::HitList);
	AttrFillGeometry::HitList.DeleteAll();

//	BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::COLOURATTCHANGED)); 
//	BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::NONCOLOURATTCHANGED)); 
	AttributeManager::SendMessages = TRUE;
	EndSlowJob();

	return TRUE;
}

/********************************************************************************************

>	void OpEditFill::SelectAllBlobs()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/95
	Purpose:	Selects all the Start or End fill blobs.

********************************************************************************************/

void OpEditFill::SelectAllBlobs()
{
	// It's a 'Select all Blobs' thingy
	AttrFillGeometry::LastRenderedStartBlob = DocCoord(0,0);
	AttrFillGeometry::LastRenderedEndBlob = DocCoord(0,0);
	AttrFillGeometry::LastRenderedEnd2Blob = DocCoord(0,0);

	// Start again at the beginning
	AttrFillGeometry* pSelAttr = AttrFillGeometry::FindFirstSelectedAttr(FillClone->GetAttributeType());
	while (pSelAttr != NULL)
	{
		if (pSelAttr)
		{
			pSelAttr->SelectBlob(DragControl);
			pSelAttr->DeselectAllBut(DragControl);
		}

		// Move onto the next.
		pSelAttr = AttrFillGeometry::FindNextSelectedAttr(FillClone->GetAttributeType());
	}					
}

/********************************************************************************************

>	DocRect OpEditFill::GetDragBlobRect()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Returns:	The bounding rect of the NodeGradFill parallelogram
	Purpose:	This function finds the bounding rect that the parallelogram of the 
				NodeGradFill we are using for EOR renderings parallelogram fits in.

********************************************************************************************/

DocRect OpEditFill::GetDragBlobRect()
{
	return GradFill->GetBlobBoundingRect();
}

/********************************************************************************************

>	void OpEditFill::RenderDragBlobs(DocRect Rect, Spread* pSpread, BOOL bSolidDrag)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		Rect - the rect that needs redrawing
				pSpread  - the spread that is being rendered
	Purpose:	Renders the GradFill as it will look if the drag were to end. If we failed
				to create the GradFill earlier, it will draw a bounding rect

********************************************************************************************/

void OpEditFill::RenderDragBlobs(DocRect Rect, Spread* pSpread, BOOL bSolidDrag)
{
	if (DontDrawBlobs)
		return;

	// If being called from DocView::RenderView, then the spread could be wrong - so
	// convert the rectangle if necessary.
	if (pSpread != StartSpread)
	{
		Rect.lo = MakeRelativeToSpread(StartSpread, pSpread, Rect.lo);
		Rect.hi = MakeRelativeToSpread(StartSpread, pSpread, Rect.hi);
	}

	AttrFillGeometry::EditedFill  = NULL;

	RenderRegion* pRegion = DocView::RenderOnTop(&Rect, StartSpread, UnclippedEOR);
	while (pRegion)
	{
		GradFill->RenderFillBlobs(pRegion);

		// Get the Next render region
		pRegion = DocView::GetNextOnTop(&Rect);
	}

	AttrFillGeometry::EditedFill  = GradFill;

	// Bodge to stop fill meshes EOR each other out.
	AttrFillGeometry::LastRenderedMesh = NULL;
}

/********************************************************************************************

>	void OpEditFill::RenderSelectionFillBlobs(SelRange* pSel, DocRect Rect, Spread* pSpread)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/6/95
	Inputs:		Rect - the rect that needs redrawing
				pSpread  - the spread that is being rendered
	Purpose:	Renders the GradFill as it will look if the drag were to end. If we failed
				to create the GradFill earlier, it will draw a bounding rect

********************************************************************************************/

void OpEditFill::RenderSelectionFillBlobs(SelRange* pSel, DocRect Rect, Spread* pSpread)
{
	// Render all the selection fill blobs
	RenderRegion* pRegion = DocView::RenderOnTop(&Rect, pSpread, UnclippedEOR);
	while (pRegion)
	{
		Node* pNode	= pSel->FindFirst();
	
		while (pNode)
		{
			((NodeRenderable*)pNode)->RenderAppliedFillBlobs(pRegion);
			pNode = pSel->FindNext(pNode);
		}

		// Get the Next render region
		pRegion = DocView::GetNextOnTop(&Rect);
	}

	AttrFillGeometry::LastRenderedMesh = NULL;
}

/********************************************************************************************

>	void OpEditFill::RenderInitSelectionBlobs(SelRange* pSel, DocRect Rect, Spread* pSpread)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/6/95
	Inputs:		Rect - the rect that needs redrawing
				pSpread  - the spread that is being rendered
	Purpose:	Renders the GradFill as it will look if the drag were to end. If we failed
				to create the GradFill earlier, it will draw a bounding rect

********************************************************************************************/

void OpEditFill::RenderInitSelectionBlobs(SelRange* pSel, DocRect Rect, Spread* pSpread)
{
	// EOR on in all the render regions that are still rendering,
	// so that the Blob rendering when the region is finished, 
	// will put them back
	RenderRegionList* pRegionList = GetApplication()->GetRegionList();
	
	if (!pRegionList->IsEmpty())
	{
		RenderRegion* pRegion = (RenderRegion*)pRegionList->GetHead();	
		
		while (pRegion)
		{
			// Check the RenderRegion is for the same spread.
			if (pRegion->GetRenderSpread() == pSpread &&
				(pRegion->IsInkRenderStarted || pRegion->NeedsOSPaper))
			{
				// Render the blobs 'clipped' to this Render Region.
				DocRect ClipRect = pRegion->GetRegionRect();
		
				if (ClipRect.IsIntersectedWith(Rect))
				{
					ClipRect = ClipRect.Intersection(Rect);
					RenderSelectionFillBlobs(pSel, ClipRect, pSpread);
				}
			}

			// Get the Next render region
			pRegion = (RenderRegion*)pRegionList->GetNext(pRegion);
		}
	}

	// Render the blobs 'Off'
	RenderSelectionFillBlobs(pSel, Rect, pSpread);
}

/********************************************************************************************

>	void OpEditFill::RenderFinalBlobs(DocRect Rect, Spread* pSpread, BOOL bSolidDrag)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/6/95
	Inputs:		Rect - the rect that needs redrawing
				pSpread  - the spread that is being rendered
	Purpose:	Renders the GradFill as it will look if the drag were to end. If we failed
				to create the GradFill earlier, it will draw a bounding rect

********************************************************************************************/

void OpEditFill::RenderFinalBlobs(DocRect Rect, Spread* pSpread, BOOL bSolidDrag)
{
	// Render the blobs 'On'
	RenderDragBlobs(Rect, pSpread, bSolidDrag);

	// Now EOR off in all the render regions that are still rendering,
	// so that the Blob rendering when the region is finished, 
	// will put them back
	RenderRegionList* pRegionList = GetApplication()->GetRegionList();
	
	if (!pRegionList->IsEmpty())
	{
		RenderRegion* pRegion = (RenderRegion*)pRegionList->GetHead();	
		
		while (pRegion)
		{
			// Check the RenderRegion is for the same spread.
			if (pRegion->GetRenderSpread() == pSpread &&
				(pRegion->IsInkRenderStarted || pRegion->NeedsOSPaper))
			{
				// Render the blobs 'clipped' to this Render Region.
				DocRect ClipRect = pRegion->GetRegionRect();
		
				if (ClipRect.IsIntersectedWith(Rect))
				{
					ClipRect = ClipRect.Intersection(Rect);
					RenderDragBlobs(ClipRect, pSpread, bSolidDrag);
				}
			}

			// Get the Next render region
			pRegion = (RenderRegion*)pRegionList->GetNext(pRegion);
		}
	} 
}

/********************************************************************************************

>	void OpEditFill::RenderInitBlobs(DocRect Rect, Spread* pSpread, BOOL bSolidDrag)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/6/95
	Inputs:		Rect - the rect that needs redrawing
				pSpread  - the spread that is being rendered
	Purpose:	Renders the GradFill as it will look if the drag were to end. If we failed
				to create the GradFill earlier, it will draw a bounding rect

********************************************************************************************/

void OpEditFill::RenderInitBlobs(DocRect Rect, Spread* pSpread, BOOL bSolidDrag)
{
	// Now EOR off in all the render regions that are still rendering,
	// so that the Blob rendering when the region is finished, 
	// will put them back
	RenderRegionList* pRegionList = GetApplication()->GetRegionList();
	
	if (!pRegionList->IsEmpty())
	{
		RenderRegion* pRegion = (RenderRegion*)pRegionList->GetHead();	
		
		while (pRegion)
		{
			// Check the RenderRegion is for the same spread.
			if (pRegion->GetRenderSpread() == pSpread &&
				(pRegion->IsInkRenderStarted || pRegion->NeedsOSPaper))
			{
				// Render the blobs 'clipped' to this Render Region.
				DocRect ClipRect = pRegion->GetRegionRect();
		
				if (ClipRect.IsIntersectedWith(Rect))
				{
					ClipRect = ClipRect.Intersection(Rect);
					RenderDragBlobs(ClipRect, pSpread, bSolidDrag);
				}
			}

			// Get the Next render region
			pRegion = (RenderRegion*)pRegionList->GetNext(pRegion);
		}
	}

	// Render the blobs 'On'
	RenderDragBlobs(Rect, pSpread, bSolidDrag);
}

/********************************************************************************************

> 	BOOL OpEditFill::GetStatusLineText(String_256* pText, Spread* pSpread, DocCoord DocPos, ClickModifiers Mods)

    Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:	29/1/95
	Inputs:		pSpread - pioter to spread mouse is over
				DocPos  - position of mouse in doc
				Mods	- click modifiers
	Outputs:	pText	- buffer for StatusLine text
    Returns:   	FALSE if fails
    Purpose:    Gets the status text during the editing of a fill.

********************************************************************************************/

BOOL OpEditFill::GetStatusLineText(String_256* pText, Spread* pSpread, 
									DocCoord DocPos, ClickModifiers ClickMods)
{
	if (GradFill == NULL)
	{
		*pText="";		// We don't seem to be doing anything at the moment !
		return TRUE;
	}

	pText->Load(_R(IDS_K_OPGRAD_EDITING));

	String_256 FillName;
	if (FillName.Load(GradFill->GetAttrNameID()))
	{
		// Add the description of the attribute onto the base string
		*pText += FillName;

		if (ContinuousEOR)
		{
			*pText += String_256(_R(IDS_K_OPGRAD_IMMEDIATE));
		}
		else
		{
			if (InteractiveDragUpdate)
			{
				*pText += String_256(_R(IDS_K_OPGRAD_BACKGROUND));
			}
			else
			{
				*pText += String_256(_R(IDS_K_OPGRAD_NOUPDATE));
			}
		}

		return TRUE;
	}

	return FALSE;
}

/********************************************************************************************

>	BOOL OpEditFill::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Returns:	TRUE if it works, FALSE if not
	Purpose:	Register this operation into the list of all operations

********************************************************************************************/

BOOL OpEditFill::Init()
{
	return (RegisterOpDescriptor(   0, 
									_R(IDS_EDITFILLOP),
									CC_RUNTIME_CLASS(OpEditFill), 
									OPTOKEN_EDITFILL,
									OpEditFill::GetState,
									0,	/* help ID */
									0,
									0	/* bitmap ID */));
}

/********************************************************************************************

>	OpState OpEditFill::GetState(String_256* Description, OpDescriptor*)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		Description - the description of the operation
	Returns:	The State of the operation
	Purpose:	Used for greying etc of an operation.

********************************************************************************************/

OpState OpEditFill::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}






/********************************************************************************************

>	OpChangeFillColour::OpChangeFillColour()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/94
	Purpose:	Dummy Contrustor

********************************************************************************************/

/*
OpChangeFillColour::OpChangeFillColour()
{
}
*/



/********************************************************************************************

>	void OpChangeFillColour::Do(AttrFillGeometry* pGrad, DocColour &StartColour)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		-
	Purpose:	-

********************************************************************************************/

/*
void OpChangeFillColour::Do( DocColour &StartColour, DocColour &EndColour,
								AttrFillGeometry* GradFill )
{
	DoStartSelOp(FALSE, FALSE);

	ModifyFillColoursAction* ModAction;
	
	ActionCode Act;
	Act = ModifyFillColoursAction::Init(this, &UndoActions, 2, (Action**)(&ModAction));
	if (Act == AC_FAIL)
	{
		FailAndExecute();
		End();
		return;
	}
			
	DocColour* OldColours;
	BOOL* OldSelect;

	ALLOC_WITH_FAIL(OldColours, (DocColour*) CCMalloc(2 * sizeof DocColour), this);
	if (!OldColours)
	{
		FailAndExecute();
		End();
		return;
	}

	// Strangeness
	new(&OldColours[0]) DocColour;
	new(&OldColours[1]) DocColour;

	OldColours[0] = (GradFill->GetAttributeValue())->Colour;
	OldColours[1] = (GradFill->GetAttributeValue())->EndColour;

	ALLOC_WITH_FAIL(OldSelect, (BOOL*) CCMalloc(2 * sizeof BOOL), this);
	if (!OldSelect)
	{
		CCFree(OldColours);
		FailAndExecute();
		End();
		return;
	}

	GradFill->GetSelectionState(OldSelect, 2);

	ModAction->StoreChanges(GradFill, OldColours, OldSelect);

	(GradFill->GetAttributeValue())->Colour    = StartColour;
	(GradFill->GetAttributeValue())->EndColour = EndColour;

	DocView* pDocView = DocView::GetCurrent();
	ENSURE( pDocView != NULL, "There was no current document when undoing ModifyFillColours" );

	Node* ParentNode = GradFill->FindParent();
	DoInvalidateNodeRegion((NodeRenderableInk*)ParentNode, TRUE);

	// always call end
	End();
}
*/

/********************************************************************************************

>	BOOL OpChangeFillColour::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Returns:	TRUE if it works, FALSE if not
	Purpose:	Register this operation into the list of all operations

********************************************************************************************/

/*
BOOL OpChangeFillColour::Init()
{
	return (RegisterOpDescriptor(   0, 
									_R(IDS_CHANGECOLOP),
									CC_RUNTIME_CLASS(OpChangeFillColour), 
									OPTOKEN_CHANGEFILL,
									OpChangeFillColour::GetState,
									0,	// help ID 
									0,
									0	// bitmap ID
								));
}
*/



/********************************************************************************************

>	OpState OpChangeFillColour::GetState(String_256* Description, OpDescriptor*)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		Description - the description of the operation
	Returns:	The State of the operation
	Purpose:	Used for greying etc of an operation.

********************************************************************************************/

/*
OpState OpChangeFillColour::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}
*/





/********************************************************************************************

>	OpChangeSelectionColours::OpChangeSelectionColours()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/94
	Purpose:	Dummy Contrustor

********************************************************************************************/

/*
OpChangeSelectionColours::OpChangeSelectionColours()
{
}
*/

/********************************************************************************************

>	void OpChangeSelectionColours::Do(DocColour &NewColour)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		-
	Purpose:	-

********************************************************************************************/
/*
BOOL OpChangeSelectionColours::Do( DocColour &NewColour )
{
	DoStartSelOp(FALSE, FALSE);

	BOOL ColourChanged = FALSE;

	Node *SelectedNode = NULL;
	SelRange *Selection = Camelot.FindSelection();

	if (Selection != NULL)
	{
		// Scan though all the Nodes in the Selection (and their Children)
		SelectedNode = Selection->FindFirst();
		while (SelectedNode != NULL)
		{
			// Is it a Fill Attribute ?
			if ( SelectedNode->IsKindOf(CC_RUNTIME_CLASS(AttrFillGeometry)) )
			{
				// Yes, so lets ask if it has any Control Points Selected ?
				if ( ((AttrFillGeometry*)SelectedNode)->ChangeControlPointColour( NewColour, this ) )
				{
					if (!ColourChanged)
					{
	 					CCRuntimeClass* CurrentAttribGroup = 
 							Tool::GetCurrent()->Parent->ToolInfo.CurrentAttributeGroup;

						Document* CurrentDoc = Document::GetSelected();

		  				Node* AttribClone;
		  				SelectedNode->NodeCopy(&AttribClone); 

						CurrentDoc->GetAttributeMgr()
							.UpdateCurrentAttribute(CurrentAttribGroup, (NodeAttribute*)AttribClone);
					}

					Node* ParentNode = SelectedNode->FindParent();
					DoInvalidateNodeRegion((NodeRenderableInk*)ParentNode, TRUE);

					// Yep, Someone has changed their colour.
					ColourChanged = TRUE;

				}
			}
			SelectedNode = Selection->FindNextAndChild(SelectedNode);
		}
	}

	if (!ColourChanged)
		FailAndExecute();

	// always call end
	End();

	return ColourChanged;
}
*/

/********************************************************************************************

>	BOOL OpChangeSelectionColours::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Returns:	TRUE if it works, FALSE if not
	Purpose:	Register this operation into the list of all operations

********************************************************************************************/
/*
BOOL OpChangeSelectionColours::Init()
{
	return (RegisterOpDescriptor(   0, 
									_R(IDS_CHANGECOLOP),
									CC_RUNTIME_CLASS(OpChangeSelectionColours), 
									OPTOKEN_SELECTCOLS,
									OpChangeSelectionColours::GetState,
									0,	// help ID
									0,
									0	// bitmap ID
								));
}
*/



/********************************************************************************************

>	OpState OpChangeSelectionColours::GetState(String_256* Description, OpDescriptor*)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		Description - the description of the operation
	Returns:	The State of the operation
	Purpose:	Used for greying etc of an operation.

********************************************************************************************/
/*
OpState OpChangeSelectionColours::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////
// The ModifyFillCoordsAction class																 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	ModifyFillCoordsAction::ModifyFillCoordsAction()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action to undo fill modification
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
/*
ModifyFillCoordsAction::ModifyFillCoordsAction()
{
	ChangedCoords = NULL;
	SelectionState = NULL;
}
*/

/********************************************************************************************

>	ActionCode ModifyFillCoordsAction::Init(  Operation* pOp,
										ActionList* pActionList,
										Action** NewAction)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/94
	Inputs:		pOp is the pointer to the operation to which this action belongs
				pActionList is the action list to which this action should be added
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


/*
ActionCode ModifyFillCoordsAction::Init(  Operation* pOp,
									ActionList* pActionList,
									INT32 NumChanged,
									Action** NewAction)
{
	UINT32 ActSize = sizeof(ModifyFillCoordsAction) + (NumChanged * sizeof(DocCoord)) + (NumChanged * sizeof(BOOL));
	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(ModifyFillCoordsAction), NewAction);

	((ModifyFillCoordsAction*)*NewAction)->NumChangedCoords = NumChanged;

	if (IsUserName("Will")) TRACE( _T("Creating ModifyFillCoordsAction"));

	return Ac;
}
*/

/********************************************************************************************

>	void ModifyFillCoordsAction::StoreChanges(DocCoord* Coords, DocColour* Colours)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/94
	Inputs:		Coords is a pointer to an array of DocCoords
				Colours is a pointer to an array of DocColours
	Outputs:	-
	Returns:	-
	Purpose:	This function initialises the array pointers in this action. Note that the 
				variable NumElements is initialised in the Init function
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

/*
void ModifyFillCoordsAction::StoreChanges(AttrFillGeometry* Fill, DocCoord* Coords, BOOL* SelState)
{
	ChangedFill	= Fill;
	ChangedCoords = Coords;
	SelectionState = SelState;
}
*/

/********************************************************************************************

>	ActionCode ModifyFillCoordsAction::Execute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/94
	Inputs:		-
	Outputs:	-
	Returns:	Action code, one of AC_OK, AC_NORECORD or AC_FAIL.
	Purpose:	This is the virtual function that is called when the action is executed
				by the Undo/Redo system. This is the function that actually undoes the 
				ModifyFillCoords action by changing the attribute values, and
				records redo information from the current values.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

/*
ActionCode ModifyFillCoordsAction::Execute()
{

	ModifyFillCoordsAction* ModAction;
	
	ActionCode Act;
	Act = ModifyFillCoordsAction::Init(pOperation, pOppositeActLst, NumChangedCoords, (Action**)(&ModAction));
	if (Act == AC_FAIL)
		return AC_FAIL;

	DocView* pDocView = DocView::GetCurrent();
	ENSURE( pDocView != NULL, "There was no current document when undoing ModifyFillCoords" );

	DocCoord StartPointTemp = ChangedCoords[0];
	DocCoord EndPointTemp   = ChangedCoords[1];
	DocCoord EndPoint2Temp  = ChangedCoords[2];

	BOOL StartSelTemp = SelectionState[0];
	BOOL EndSelTemp   = SelectionState[1];
	BOOL End2SelTemp  = SelectionState[2];

	ChangedCoords[0] = (ChangedFill->GetAttributeValue())->StartPoint;
	ChangedCoords[1] = (ChangedFill->GetAttributeValue())->EndPoint;
	ChangedCoords[2] = (ChangedFill->GetAttributeValue())->EndPoint2;

	BOOL CurrentSelState[3];
	ChangedFill->GetSelectionState(CurrentSelState, 3);

	SelectionState[0] = CurrentSelState[0];
	SelectionState[1] = CurrentSelState[1];
	SelectionState[2] = CurrentSelState[2];

	CurrentSelState[0] = StartSelTemp;
	CurrentSelState[1] = EndSelTemp;
	CurrentSelState[2] = End2SelTemp;

	ChangedFill->SetSelectionState(CurrentSelState, 3);

	(ChangedFill->GetAttributeValue())->StartPoint = StartPointTemp;
	(ChangedFill->GetAttributeValue())->EndPoint = EndPointTemp;
	(ChangedFill->GetAttributeValue())->EndPoint2 = EndPoint2Temp;

	ModAction->StoreChanges(ChangedFill, ChangedCoords, SelectionState);

	ChangedCoords = NULL;
	SelectionState = NULL;

	Node* ParentNode = ChangedFill->FindParent();
	((UndoableOperation*)pOperation)->
			DoInvalidateNodeRegion((NodeRenderableInk*)ParentNode, TRUE);

	return Act;
}

ModifyFillCoordsAction::~ModifyFillCoordsAction()
{
	if (ChangedCoords)
		CCFree(ChangedCoords);

	if (SelectionState)
		CCFree(SelectionState);
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////
// The ModifyFillColoursAction class																 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	ModifyFillColoursAction::ModifyFillColoursAction()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action to undo fill modification
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
/*
ModifyFillColoursAction::ModifyFillColoursAction()
{
	ChangedColours = NULL;
	SelectionState = NULL;
}
*/

/********************************************************************************************

>	ActionCode ModifyFillColoursAction::Init(  Operation* pOp,
										ActionList* pActionList,
										Action** NewAction)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/94
	Inputs:		pOp is the pointer to the operation to which this action belongs
				pActionList is the action list to which this action should be added
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

/*
ActionCode ModifyFillColoursAction::Init(  Operation* pOp,
									ActionList* pActionList,
									INT32 NumChanged,
									Action** NewAction)
{
	UINT32 ActSize = sizeof(ModifyFillColoursAction) + (NumChanged * sizeof(DocColour)) + (NumChanged * sizeof(BOOL));
	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(ModifyFillColoursAction), NewAction);

	((ModifyFillColoursAction*)*NewAction)->NumChangedColours = NumChanged;

	if (IsUserName("Will")) TRACE( _T("Creating ModifyFillColoursAction"));

	return Ac;
}
*/

/********************************************************************************************

>	void ModifyFillColoursAction::StoreChanges(AttrFillGeometry* Fill, DocColour* Colours)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/94
	Inputs:		Colours is a pointer to an array of DocColours
	Outputs:	-
	Returns:	-
	Purpose:	This function initialises the array pointers in this action. Note that the 
				variable NumElements is initialised in the Init function
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

/*
void ModifyFillColoursAction::StoreChanges(AttrFillGeometry* Fill, DocColour* Colours, BOOL* SelState)
{
	ChangedFill	= Fill;
	ChangedColours = Colours;
	SelectionState = SelState;
}
*/

/********************************************************************************************

>	ActionCode ModifyFillColoursAction::Execute()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/94
	Inputs:		-
	Outputs:	-
	Returns:	Action code, one of AC_OK, AC_NORECORD or AC_FAIL.
	Purpose:	This is the virtual function that is called when the action is executed
				by the Undo/Redo system. This is the function that actually undoes the 
				ModifyFillColours action by changing the attribute values, and
				records redo information from the current values.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

/*
ActionCode ModifyFillColoursAction::Execute()
{

	ModifyFillColoursAction* ModAction;
	
	ActionCode Act;
	Act = ModifyFillColoursAction::Init(pOperation, pOppositeActLst, NumChangedColours, (Action**)(&ModAction));
	if (Act == AC_FAIL)
		return AC_FAIL;

	DocView* pDocView = DocView::GetCurrent();
	ENSURE( pDocView != NULL, "There was no current document when undoing ModifyFillColours" );

	DocColour StartTemp = ChangedColours[0];
	DocColour EndTemp   = ChangedColours[1];

	BOOL StartSelTemp = SelectionState[0];
	BOOL EndSelTemp   = SelectionState[1];

	ChangedColours[0] = (ChangedFill->GetAttributeValue())->Colour;
	ChangedColours[1] = (ChangedFill->GetAttributeValue())->EndColour;

	BOOL CurrentSelState[2];
	ChangedFill->GetSelectionState(CurrentSelState, 2);

	SelectionState[0] = CurrentSelState[0];
	SelectionState[1] = CurrentSelState[1];

	CurrentSelState[0] = StartSelTemp;
	CurrentSelState[1] = EndSelTemp;

	ChangedFill->SetSelectionState(CurrentSelState, 2);

	(ChangedFill->GetAttributeValue())->Colour = StartTemp;
	(ChangedFill->GetAttributeValue())->EndColour = EndTemp;

	ModAction->StoreChanges(ChangedFill, ChangedColours, SelectionState);

	ChangedColours = NULL;
	SelectionState = NULL;

	Node* ParentNode = ChangedFill->FindParent();
	((UndoableOperation*)pOperation)->
			DoInvalidateNodeRegion((NodeRenderableInk*)ParentNode, TRUE);

	return Act;
}

ModifyFillColoursAction::~ModifyFillColoursAction()
{
	if (ChangedColours)
	{
		for (INT32 i=0; i < NumChangedColours; i++)
		{
			ChangedColours[i].~DocColour();
		}
		CCFree(ChangedColours);
	}

	if (SelectionState)
		CCFree(SelectionState);
}
*/


/********************************************************************************************

>	OpCreateFill::OpCreateFill()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Purpose:	Dummy Constructor

********************************************************************************************/

OpCreateFill::OpCreateFill()
{
	GradFill = NULL;
}

/********************************************************************************************

>	void OpCreateFill::DoDrag( AttrFillGeometry* Fill, Spread* pSpread, DocCoord Anchor)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		pSpread - The Spread the drag is starting on
				Anchor - The coord of the start of the drag
	Purpose:	Starts the drag and records the initial position of the mouse etc.
				It also snaps the start position to the grid if it needs to.

********************************************************************************************/

void OpCreateFill::DoDrag( AttrFillGeometry* Fill, Spread* pSpread, DocCoord Anchor)
{
	DocView::SnapSelected(pSpread, &Anchor);

	// We had better take a note of the starting point of the drag
	StartSpread = pSpread;
	StartPoint = Anchor;
	
	// init the old and the new mouse position needed in drawing the rubber box
	LastMousePosition = Anchor;

	GradFill = Fill;

	GradFill->SetStartPoint(&Anchor);
	GradFill->SetEndPoint(&Anchor);
	
	if (GradFill->GetEndPoint2() != NULL)
    	GradFill->SetEndPoint2(&Anchor);

	// Remove other fill meshs whilst we are dragging	
	BlobStyle MyBlobs;
	MyBlobs.Fill = TRUE;
	(Camelot.GetBlobManager())->RemoveInterest(MyBlobs);

	AttrFillGeometry::DraggedFill = GradFill;

	// And tell the Dragging system that we need drags to happen
//	StartDrag( DRAGTYPE_AUTOSCROLL );
	StartDrag(DRAGTYPE_AUTOSCROLL, NULL, NULL, TRUE, TRUE);			// We support solid dragging
}	

/********************************************************************************************

>	void OpCreateFill::DragPointerMove(	DocCoord PointerPos, ClickModifiers ClickMods,
											Spread* pSpread, BOOL bSolidDrag)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		PointerPos - The current mouse position
				ClickMods - Which modifier keys were being pressed at the time
				pSpread - The Spread that the mouse is over now
	Purpose:	re-draws the drag blobs as the mouse moves about the document.

********************************************************************************************/

void OpCreateFill::DragPointerMove(DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL bSolidDrag)
{
	// Snap the actual mouse position to the grid if needed
	DocView::SnapSelected(pSpread, &PointerPos);

	// if we need to constrain the drag, then make our GradFill into a circle
	if (ClickMods.Constrain)
		DocView::ConstrainToAngle(StartPoint, PI/4, &PointerPos);

	// If the mouse is in a different position then do something
	if (PointerPos != LastMousePosition)
	{
		// First Rub out the old Drag blobs
		DocRect BoundingRect = GetDragBlobRect();
		RenderDragBlobs(BoundingRect, StartSpread, bSolidDrag);

		// Make sure that the coords are relative to the coorect spread
		if (pSpread != StartSpread)
			PointerPos = MakeRelativeToSpread(StartSpread, pSpread, PointerPos);

		// Update the last mouse position and re-calc the bounding rect
		LastMousePosition = PointerPos;

		DocCoord Start	= StartPoint;
		DocCoord End    = PointerPos;
		DocCoord End2   = MakeLineAtAngle(StartPoint, PointerPos);

		// If we are creating a Bitmap Fill, then try to create it 
		// with the correct aspect ratio 
		if (GradFill->GetBitmap() != NULL)
		{
			BitmapInfo Info;
  			KernelBitmap* Bmp = GradFill->GetBitmap();

			AttrFillGeometry* Attr = AttrFillGeometry::FindFirstSelectedAttr(GradFill->GetAttributeType());

			while (Attr != NULL)
			{
				if (Attr->GetBitmap() != NULL)
				{
					Bmp = Attr->GetBitmap();
					break; 
				}
				
				Attr = AttrFillGeometry::FindNextSelectedAttr(GradFill->GetAttributeType());
			}

			// if possible we base our default size on the bitmaps preferred size
			if (Bmp && Bmp->ActualBitmap)
			{
				if (Bmp->ActualBitmap->GetInfo( &Info ))
				{
					INT32 BitmapWidth  = Info.RecommendedWidth;
					INT32 BitmapHeight = Info.RecommendedHeight;

					// Calculate the spect ratio of the bitmap
					double Ratio = double(BitmapHeight)/double(BitmapWidth);

					End2 = MakeLineAtAngle(Start, End, 90, INT32(Start.Distance(End) * Ratio));
				}
			}
		}

		if (GradFill->IsKindOf(CC_RUNTIME_CLASS(AttrBitmapFill)))
		{
			// Get the Real Control Points
			// (The user only sees the virtual points)
        	GetBitmapRealPoints(Start, End, End2,
								&Start, &End, &End2);
		}

		// Update the new Fill's points
		GradFill->SetStartPoint(&Start);
		GradFill->SetEndPoint(&End);
		if (GradFill->GetEndPoint2() != NULL)
			GradFill->SetEndPoint2(&End2);

		BoundingRect = GetDragBlobRect();

		// Draw the Fill Mesh
		RenderDragBlobs(BoundingRect, StartSpread, bSolidDrag);
	}
}

/********************************************************************************************

>	void OpCreateFill::DragFinished(DocCoord PointerPos, ClickModifiers, Spread* pSpread,
										BOOL Success, BOOL bSolidDrag)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/94
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickModifiers - Not Used
				pSpread - The spread that the drag ended over
				Success - TRUE if the drag worked
	Purpose:	Called when the drag ends. This function rubs out the drag blobs and then
				builds a new GradFill Node and adds it to the end of the tree.

********************************************************************************************/

void OpCreateFill::DragFinished(DocCoord PointerPos, ClickModifiers, Spread* pSpread, BOOL Success, BOOL bSolidDrag)
{
	// Put the hour glass up as we have to
	BeginSlowJob();

	DocRect DragRect;

	// First Rub out the old Drag blobs
	DocRect	BoundingRect = GetDragBlobRect();
	RenderDragBlobs(BoundingRect, StartSpread, bSolidDrag);

	// Get the scaled pixel size for the view
	FIXED16 ScaledPixelWidth,
			ScaledPixelHeight;
	GetWorkingView()->GetScaledPixelSize(&ScaledPixelWidth, &ScaledPixelHeight);

	INT32 PixWidth 	= ScaledPixelWidth.MakeLong();
	INT32 PixHeight	= ScaledPixelHeight.MakeLong();

	// Flag to say if everything has worked
	BOOL IsOk = FALSE;

	// End the Drag                             
	if (!EndDrag())
		goto EndOperation;

	if (PointerPos == StartPoint)
		goto EndOperation;

	if (*GradFill->GetStartPoint() == *GradFill->GetEndPoint())
		goto EndOperation;

	DragRect.IncludePoint(StartPoint);
	DragRect.IncludePoint(PointerPos);

	if (DragRect.Width() < PixWidth*2 && DragRect.Height() < PixHeight*2)
	{
		TRACEUSER( "Will", _T("Drag rect too small\n"));
		goto EndOperation;
	}

	// if the drag was a sucess then make a grad fill
	if (Success && (!DragRect.IsEmpty()))
	{
		if (GradFill != NULL)
		{				
			 CCRuntimeClass* CurrentAttribGroup = 
		 		Tool::GetCurrent()->Parent->m_ToolInfo.CurrentAttributeGroup;

			Document* CurrentDoc = Document::GetSelected();

			AttrFillGeometry* CurrentAttr = (AttrFillGeometry*)(CurrentDoc->GetAttributeMgr()
					.GetCurrentAttribute(CurrentAttribGroup, GradFill->GetAttributeType()));

			// Is there a current attribute of this type ?
			if (CurrentAttr != NULL)
			{
				BOOL Locked = ((FillGeometryAttribute*)GradFill->GetAttributeValue())->IsAspectLocked();
				
				// Use the current Attributes Colours
				*((FillGeometryAttribute*)GradFill->GetAttributeValue()) = 
							*((FillGeometryAttribute*)CurrentAttr->GetAttributeValue());

				((FillGeometryAttribute*)GradFill->GetAttributeValue())->SetAspectLock(Locked);

				DocCoord Start	= StartPoint;
				DocCoord End    = LastMousePosition;
				DocCoord End2   = MakeLineAtAngle(StartPoint, LastMousePosition);

				// Ensure the Aspect ratio is correct for a new
				// bitmap fill
				if (GradFill->GetBitmap() != NULL)
				{
					BitmapInfo Info;
		  			KernelBitmap* Bmp = GradFill->GetBitmap();

					AttrFillGeometry* Attr = AttrFillGeometry::FindFirstSelectedAttr(GradFill->GetAttributeType());

					while (Attr != NULL)
					{
						if (Attr->GetBitmap() != NULL)
						{
							Bmp = Attr->GetBitmap();
							break; 
						}
				
						Attr = AttrFillGeometry::FindNextSelectedAttr(GradFill->GetAttributeType());
					}

					// if possible we base our default size on the bitmaps preferred size
					if (Bmp && Bmp->ActualBitmap)
					{
						if (Bmp->ActualBitmap->GetInfo( &Info ))
						{
							INT32 BitmapWidth  = Info.RecommendedWidth;
							INT32 BitmapHeight = Info.RecommendedHeight;

							// Calculate the spect ratio of the bitmap
							double Ratio = double(BitmapHeight)/double(BitmapWidth);

							End2 = MakeLineAtAngle(Start, End, 90, INT32(Start.Distance(End) * Ratio));
						}
					}
				}

				if (GradFill->IsKindOf(CC_RUNTIME_CLASS(AttrBitmapFill)))
				{
					// Get the Real Control Points
					// (The user only sees the virtual points)
		        	GetBitmapRealPoints(Start, End, End2,
										&Start, &End, &End2);
				}

				// Set the new fills points
				GradFill->SetStartPoint(&Start);
				GradFill->SetEndPoint(&End);

				if (GradFill->GetEndPoint2() != NULL)
					GradFill->SetEndPoint2(&End2);
			}
			else
			{
				// No current fill so use default colours
				*GradFill->GetStartColour()	= DocColour(COLOUR_WHITE);
				*GradFill->GetEndColour() 	= DocColour(COLOUR_BLACK);
			}

			if (GradFill->GetAttributeType() == CC_RUNTIME_CLASS(AttrTranspFillGeometry))
			{
				UINT32 Start = 0;
				UINT32 End = 255;
	
				GradFill->SetStartTransp(&Start);
				GradFill->SetEndTransp(&End);
			}

			SelRange *Selection = Camelot.FindSelection();
			DocRect Bounds;
			if (Selection != NULL && Selection->Count()>0)
				Bounds = Selection->GetBoundingRect();
			else
				Bounds = GetDragBlobRect();

			// Give the attribute a bounding rect
			GradFill->SetBoundingRect(Bounds);

			GradFill->SelectBlob(FILLCONTROL_ENDPOINT);

			// And pass it to the attribute manager to sort the rest out
			AttributeManager::SendMessages = FALSE;
			AttributeManager::AttributeSelected(GradFill); 
			AttributeManager::SendMessages = TRUE;

			IsOk = TRUE;
		}
	}

EndOperation:
	// If something went wrong, then fail
	if (!IsOk)
	{
		delete GradFill;
		FailAndExecute();
	}

	AttrFillGeometry::DraggedFill = NULL;

	// Turn the fill meshes back on
	BlobStyle MyBlobs;
	MyBlobs.Fill = TRUE;
	(Camelot.GetBlobManager())->AddInterest(MyBlobs);

	AttrFillGeometry::SelectionCount = AttrFillGeometry::CountSelectionControlPoints();
	// We've probably changed a Colour
	BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::COLOURATTCHANGED)); 
	// and the Attrib may have changed shape or summit.	Who knows ?
	BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::NONCOLOURATTCHANGED)); 

	// always call end
	End();
}

/********************************************************************************************

>	void OpCreateFill::RenderDragBlobs(DocRect Rect, Spread* pSpread, BOOL bSolidDrag)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		Rect - The rectangle that needs to be redrawn
				pSpread - the spread that it should be drawn on
	Purpose:	Renders the EOR blobs during a drag. It can also get called from OnDraw()

********************************************************************************************/

void OpCreateFill::RenderDragBlobs(DocRect Rect, Spread* pSpread, BOOL bSolidDrag)
{
	// If being called from DocView::RenderView, then the spread could be wrong - so
	// convert the rectangle if necessary.
	if (pSpread != StartSpread)
	{
		Rect.lo = MakeRelativeToSpread(StartSpread, pSpread, Rect.lo);
		Rect.hi = MakeRelativeToSpread(StartSpread, pSpread, Rect.hi);
	}

	RenderRegion* pRegion = DocView::RenderOnTop(&Rect, StartSpread, UnclippedEOR);
	while (pRegion)
	{
		GradFill->RenderFillBlobs(pRegion);

		// Get the Next render region
		pRegion = DocView::GetNextOnTop(&Rect);
	}

	// Bodge to stop fill meshes EOR each other out.
	AttrFillGeometry::LastRenderedMesh = NULL;
}

/********************************************************************************************

>	DocRect OpCreateFill::GetDragBlobRect()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Returns:	The bounding rect of the NodeGradFill parallelogram
	Purpose:	This function finds the bounding rect that the parallelogram of the 
				NodeGradFill we are using for EOR renderings parallelogram fits in.

********************************************************************************************/

DocRect OpCreateFill::GetDragBlobRect()
{
	return GradFill->GetBlobBoundingRect();
}

/********************************************************************************************

>	BOOL OpCreateFill::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Returns:	TRUE if the operation was initialised sucessfully
	Purpose:	Declare the operations OpDescriptor

********************************************************************************************/

BOOL OpCreateFill::Init()
{
	return (RegisterOpDescriptor(   0, 
									_R(IDS_CREATEFILLOP),
									CC_RUNTIME_CLASS(OpCreateFill), 
									OPTOKEN_GRADFILL,
									OpCreateFill::GetState,
									0,	/* help ID */
									0,
									0	/* bitmap ID */));
}

/********************************************************************************************

>	OpState OpCreateFill::GetState(String_256* Description, OpDescriptor*)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/94
	Inputs:		Description - The Description of the operation
	Purpose:	Finds out the operations current state for greying etc in menus

********************************************************************************************/

OpState OpCreateFill::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}

/********************************************************************************************

> 	BOOL OpCreateFill::GetStatusLineText(String_256* pText, Spread* pSpread, DocCoord DocPos, ClickModifiers Mods)

    Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
    Created:	29/1/95
	Inputs:		pSpread - pioter to spread mouse is over
				DocPos  - position of mouse in doc
				Mods	- click modifiers
	Outputs:	pText	- buffer for StatusLine text
    Returns:   	FALSE if fails
    Purpose:    Gets the status text during the creation of a fill.

********************************************************************************************/

BOOL OpCreateFill::GetStatusLineText(String_256* pText, Spread* pSpread, 
										DocCoord DocPos, ClickModifiers ClickMods)
{
	if (GradFill == NULL)
	{
		*pText="";		// We don't seem to be doing anything at the moment !
		return TRUE;
	}

	pText->Load(_R(IDS_K_OPGRAD_CREATENEW));

	String_256 FillName;
	if (FillName.Load(GradFill->GetAttrNameID()))
	{
		// Add the description of the attribute onto the base string
		*pText += FillName;

		return TRUE;
	}

	return FALSE;
}


#endif    // STANDALONE






/********************************************************************************************	

	These are some Global Functions, used by the Grad Fill stuff to render it's fill mesh
	bits.  They are only tempory.

********************************************************************************************/

/********************************************************************************************

>	void MakeMeshArrow(Path* ArrowPath, DocCoord &Start, DocCoord &End, DocCoord* LineEnd)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/07/94
	Inputs:		-
	Returns:	-
	Purpose:	Makes an Arrow for the Fill Mesh.
				This is a Tempory Function, used until ArrowHeads work properly. 

********************************************************************************************/

void MakeMeshArrow(Path* ArrowPath, DocCoord &Start, DocCoord &End, DocCoord* LineEnd)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// First we Must caculate the angle of the arrow from the line angle
	const INT32 dx = Start.x - End.x;
	const INT32 dy = Start.y - End.y;

	if ( (dy==0) && (dx==0) )
	{
		ArrowPath->InsertMoveTo(End);
		ArrowPath->InsertLineTo(End);
		ArrowPath->TryToClose();

		*LineEnd = End;
		return;								// draw nothing as too small
	}

	double ArrowAngle;

	if ( (dx>=0) && (dy>=0) )
		ArrowAngle = PI - atan2( (double)dx, (double)dy );
	else if ( (dx<0) && (dy>=0) )
		ArrowAngle = -PI + atan2( (double)-dx, (double)dy );
	else if ( (dx<0) && (dy<0) )
		ArrowAngle = -atan2( (double)-dx, (double)-dy );
	else if ( (dx>=0) && (dy<0) )
		ArrowAngle = atan2( (double)dx, (double)-dy );
	else
	{
		TRACE( _T("invalid angle on arrowhead\n"));		// this should be impossible
		return;
	}

	ANGLE RotateAngle = 360 * (ArrowAngle / (2*PI));	// Convert from radians to degrees

    Matrix ArrowMatrix = Matrix(RotateAngle);			// Rotate it
    ArrowMatrix *= Matrix(End);							// Move it to the line point

	const UINT32 PointCount = 4;
	Coord ArrowPts[PointCount];

	INT32 BlobSize = (Camelot.GetBlobManager())->GetBlobSize();

	INT32 ArrowWidth  = (BlobSize  * 3)/2;
	INT32 ArrowHeight = (BlobSize  * 3)/2;
    
	ArrowPts[0] = Coord(             0,            0);
	ArrowPts[1] = Coord( -ArrowWidth/2, -ArrowHeight);
	ArrowPts[2] = Coord(  ArrowWidth/2, -ArrowHeight);
	ArrowPts[3] = Coord(             0, -ArrowHeight);

	ArrowMatrix.transform(ArrowPts, PointCount);

	ArrowPath->InsertMoveTo(DocCoord(ArrowPts[0].x, ArrowPts[0].y));
	ArrowPath->InsertLineTo(DocCoord(ArrowPts[1].x, ArrowPts[1].y));
	ArrowPath->InsertLineTo(DocCoord(ArrowPts[2].x, ArrowPts[2].y));
	ArrowPath->InsertLineTo(DocCoord(ArrowPts[0].x, ArrowPts[0].y));
	ArrowPath->TryToClose();

	*LineEnd = DocCoord(ArrowPts[3].x, ArrowPts[3].y);
#endif
}

/********************************************************************************************

>	void MakeMeshDottedLine(Path* ArrowPath, DocCoord &Start, DocCoord &End)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/94
	Inputs:		-
	Returns:	-
	Purpose:	Makes a Dotted Line for the Fill Mesh. 
				This is a Tempory Function, used until Stroke Providers work properly. 

********************************************************************************************/

void MakeMeshDottedLine(Path* ArrowPath, DocCoord &Start, DocCoord &End)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ArrowPath->InsertMoveTo(Start);
	ArrowPath->InsertLineTo(End);
#endif
}

/********************************************************************************************

>	void MakeMeshSemiCircle(Path* EllipsePath, DocCoord Start, DocCoord End, DocCoord ArrowPoint)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/94
	Inputs:		-
	Returns:	-
	Purpose:	Make a Semi-Circle for the Conical Fill Mesh. 

********************************************************************************************/

void MakeMeshSemiCircle(Path* EllipsePath, DocCoord Start, DocCoord End, DocCoord ArrowPoint)
{
#if !defined(EXCLUDE_FROM_RALPH)
	End  = MakeLineAtAngle(Start, End, 180);
	DocCoord End2 = MakeLineAtAngle(Start, End);

	ANGLE LineAngle = CalcLineAngle(Start, End);
	Matrix Rotate = Matrix(-LineAngle);

	End.translate(-Start.x, -Start.y);
	End2.translate(-Start.x, -Start.y);

	Rotate.transform(&End);
	Rotate.transform(&End2);

	End.translate(Start.x, Start.y);
	End2.translate(Start.x, Start.y);

	DocCoord Parallel[4];

	INT32 dx = End2.x - Start.x;
	INT32 dy = Start.y - End.y;

	// Copy the rectangle into the parallelogram
	Parallel[0] = DocCoord(Start.x - dx, Start.y - dy);
	Parallel[1] = DocCoord(Start.x + dx, Start.y - dy);
	Parallel[2] = DocCoord(Start.x + dx, Start.y + dy);
	Parallel[3] = DocCoord(Start.x - dx, Start.y + dy);

	// Get an array to put the 12 different coords needed to specify an ellipse
	DocCoord NewCoords[12];

	// Calculate the 3 coordinates along each side of the parallelogram
	CalcMeshEllipseEdge(Parallel[0], Parallel[1], &NewCoords[11], &NewCoords[0], &NewCoords[1]);
	CalcMeshEllipseEdge(Parallel[1], Parallel[2], &NewCoords[2], &NewCoords[3], &NewCoords[4]);
	CalcMeshEllipseEdge(Parallel[2], Parallel[3], &NewCoords[5], &NewCoords[6], &NewCoords[7]);
	CalcMeshEllipseEdge(Parallel[3], Parallel[0], &NewCoords[8], &NewCoords[9], &NewCoords[10]);

	Matrix Rotate2 = Matrix(LineAngle);

	for (INT32 i=0; i<12; i++)
	{
		NewCoords[i].translate(-Start.x, -Start.y);
		Rotate2.transform(&NewCoords[i]);
		NewCoords[i].translate(Start.x, Start.y);
	}

	// build a path
	EllipsePath->ClearPath();
	EllipsePath->FindStartOfPath();
			
	// Start at bottom left corner
//	EllipsePath->InsertMoveTo(NewCoords[0]);
//	EllipsePath->InsertCurveTo(NewCoords[1], NewCoords[2], NewCoords[3]);
//	EllipsePath->InsertCurveTo(NewCoords[4], NewCoords[5], NewCoords[6]);

	EllipsePath->InsertMoveTo(ArrowPoint);
	EllipsePath->InsertCurveTo(NewCoords[7], NewCoords[8], NewCoords[9]);
	EllipsePath->InsertCurveTo(NewCoords[10], NewCoords[11], NewCoords[0]);

	// Close the path properly
//	EllipsePath->CloseSubPath();
#endif
}

/********************************************************************************************

>	DocRect GetMeshSemiCircleBounds(DocCoord Start, DocCoord End, DocCoord EndPoint)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/94
	Inputs:		-
	Returns:	-
	Purpose:	Gets the Bounding rect of the Conical Fill Mesh. 

********************************************************************************************/

DocRect GetMeshSemiCircleBounds(DocCoord Start, DocCoord End, DocCoord EndPoint)
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocCoord End2 = MakeLineAtAngle(Start, End);

	ANGLE LineAngle = CalcLineAngle(Start, End);
	Matrix Rotate = Matrix(-LineAngle);

	End.translate(-Start.x, -Start.y);
	End2.translate(-Start.x, -Start.y);

	Rotate.transform(&End);
	Rotate.transform(&End2);

	End.translate(Start.x, Start.y);
	End2.translate(Start.x, Start.y);

	DocCoord Parallel[4];

	INT32 dx = End2.x - Start.x;
	INT32 dy = Start.y - End.y;

	// Copy the rectangle into the parallelogram
	Parallel[0] = DocCoord(Start.x - dx, Start.y - dy);
	Parallel[1] = DocCoord(Start.x + dx, Start.y - dy);
	Parallel[2] = DocCoord(Start.x + dx, Start.y + dy);
	Parallel[3] = DocCoord(Start.x - dx, Start.y + dy);

	// Get an array to put the 12 different coords needed to specify an ellipse
	DocCoord NewCoords[12];

	// Calculate the 3 coordinates along each side of the parallelogram
	CalcMeshEllipseEdge(Parallel[0], Parallel[1], &NewCoords[11], &NewCoords[0], &NewCoords[1]);
	CalcMeshEllipseEdge(Parallel[1], Parallel[2], &NewCoords[2], &NewCoords[3], &NewCoords[4]);
	CalcMeshEllipseEdge(Parallel[2], Parallel[3], &NewCoords[5], &NewCoords[6], &NewCoords[7]);
	CalcMeshEllipseEdge(Parallel[3], Parallel[0], &NewCoords[8], &NewCoords[9], &NewCoords[10]);

	Matrix Rotate2 = Matrix(LineAngle);

	for (INT32 i=0; i<12; i++)
	{
		NewCoords[i].translate(-Start.x, -Start.y);
		Rotate2.transform(&NewCoords[i]);
		NewCoords[i].translate(Start.x, Start.y);
	}

	DocRect Bounds = DocRect(EndPoint, EndPoint);
	Bounds.IncludePoint(NewCoords[6]);
	Bounds.IncludePoint(NewCoords[7]);
	Bounds.IncludePoint(NewCoords[8]);
	Bounds.IncludePoint(NewCoords[9]);
	Bounds.IncludePoint(NewCoords[10]);
	Bounds.IncludePoint(NewCoords[11]);

	return Bounds;
#else
	return DocRect(0,0,0,0);
#endif
}

/********************************************************************************************

>	void MakeMeshEllipse(Path* EllipsePath, DocCoord Start, DocCoord End, DocCoord End2)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/94
	Inputs:		-
	Returns:	-
	Purpose:	Makes an Elliptical Fill Mesh. 

********************************************************************************************/

void MakeMeshEllipse(Path* EllipsePath, DocCoord Start, DocCoord End, DocCoord End2)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (End2 == DocCoord(0,0))
	{
		End2 = MakeLineAtAngle(Start, End);
	}

	ANGLE LineAngle = CalcLineAngle(Start, End);
	Matrix Rotate = Matrix(-LineAngle);

	End.translate(-Start.x, -Start.y);
	End2.translate(-Start.x, -Start.y);

	Rotate.transform(&End);
	Rotate.transform(&End2);

	End.translate(Start.x, Start.y);
	End2.translate(Start.x, Start.y);

	DocCoord Parallel[4];

	INT32 dx = End2.x - Start.x;
	INT32 dy = Start.y - End.y;

	// Copy the rectangle into the parallelogram
	Parallel[0] = DocCoord(Start.x - dx, Start.y - dy);
	Parallel[1] = DocCoord(Start.x + dx, Start.y - dy);
	Parallel[2] = DocCoord(Start.x + dx, Start.y + dy);
	Parallel[3] = DocCoord(Start.x - dx, Start.y + dy);

	// Get an array to put the 12 different coords needed to specify an ellipse
	DocCoord NewCoords[12];

	// Calculate the 3 coordinates along each side of the parallelogram
	CalcMeshEllipseEdge(Parallel[0], Parallel[1], &NewCoords[11], &NewCoords[0], &NewCoords[1]);
	CalcMeshEllipseEdge(Parallel[1], Parallel[2], &NewCoords[2], &NewCoords[3], &NewCoords[4]);
	CalcMeshEllipseEdge(Parallel[2], Parallel[3], &NewCoords[5], &NewCoords[6], &NewCoords[7]);
	CalcMeshEllipseEdge(Parallel[3], Parallel[0], &NewCoords[8], &NewCoords[9], &NewCoords[10]);

	Matrix Rotate2 = Matrix(LineAngle);

	for (INT32 i=0; i<12; i++)
	{
		NewCoords[i].translate(-Start.x, -Start.y);
		Rotate2.transform(&NewCoords[i]);
		NewCoords[i].translate(Start.x, Start.y);
	}

	// build a path
	EllipsePath->ClearPath();
	EllipsePath->FindStartOfPath();
			
	// Start at bottom left corner
	EllipsePath->InsertMoveTo(NewCoords[0]);
	EllipsePath->InsertCurveTo(NewCoords[1], NewCoords[2], NewCoords[3]);
	EllipsePath->InsertCurveTo(NewCoords[4], NewCoords[5], NewCoords[6]);
	EllipsePath->InsertCurveTo(NewCoords[7], NewCoords[8], NewCoords[9]);
	EllipsePath->InsertCurveTo(NewCoords[10], NewCoords[11], NewCoords[0]);

	// Close the path properly
	EllipsePath->CloseSubPath();
#endif
}

/********************************************************************************************

>	void CalcMeshEllipseEdge(const DocCoord& P0, const DocCoord& P1, DocCoord* NewCoordA,
								  DocCoord* NewCoordB, DocCoord* NewCoordC)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/94
	Inputs:		-
	Returns:	-
	Purpose:	Used when making an Elliptical Fill Mesh. 

********************************************************************************************/

void CalcMeshEllipseEdge(const DocCoord& P0, const DocCoord& P1, DocCoord* NewCoordA,
								  DocCoord* NewCoordB, DocCoord* NewCoordC)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Do the maths. This is described in the document camelot\docs\ellipse.doc
	// Find the length of the first side (pythag)
	INT32 dx = P1.x - P0.x;
	INT32 dy = P1.y - P0.y;
	//double SideLength = sqrt(dx*dx + dy*dy);
	//ENSURE(SideLength!=0.0, "CalcMeshEllipseEdge was thinking of doing a div 0!");

	// This would be different if we were calculating a rounded rectangle
	//double ArcRadius = SideLength / 2.0;
	//double ControlDist = ArcRadius/(SideLength*2.2336);
	// This is equivalent to the above 2 lines, only less maths is needed
	// (ie none as the compiler should do it)
	double ControlDist = 1.0 / (2 * 2.2336);

	// Find the control point closest to P0
	NewCoordA->x = (INT32) (P0.x + (dx * ControlDist));
	NewCoordA->y = (INT32) (P0.y + (dy * ControlDist));

	// Find the mid point of the side, for the middle control point
	NewCoordB->x = (INT32) (P0.x + (dx / 2));
	NewCoordB->y = (INT32) (P0.y + (dy / 2));

	// Find the Control point closest to p1
	NewCoordC->x = (INT32) (P1.x - dx * ControlDist);
	NewCoordC->y = (INT32) (P1.y - dy * ControlDist);
#endif
}

/********************************************************************************************

>	DocRect GetMeshEllipseBounds(DocCoord Start, DocCoord End, DocCoord End2)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/94
	Inputs:		-
	Returns:	-
	Purpose:	Gets the Bounding rect of the Radial Fill Mesh. 

********************************************************************************************/

DocRect GetMeshEllipseBounds(DocCoord Start, DocCoord End, DocCoord End2)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (End2 == DocCoord(0,0))
	{
		End2 = MakeLineAtAngle(Start, End);
	}

	ANGLE LineAngle = CalcLineAngle(Start, End);
	Matrix Rotate = Matrix(-LineAngle);

	End.translate(-Start.x, -Start.y);
	End2.translate(-Start.x, -Start.y);

	Rotate.transform(&End);
	Rotate.transform(&End2);

	End.translate(Start.x, Start.y);
	End2.translate(Start.x, Start.y);

	DocCoord Parallel[4];

	INT32 dx = End2.x - Start.x;
	INT32 dy = Start.y - End.y;

	// Copy the rectangle into the parallelogram
	Parallel[0] = DocCoord(Start.x - dx, Start.y - dy);
	Parallel[1] = DocCoord(Start.x + dx, Start.y - dy);
	Parallel[2] = DocCoord(Start.x + dx, Start.y + dy);
	Parallel[3] = DocCoord(Start.x - dx, Start.y + dy);

	Matrix Rotate2 = Matrix(LineAngle);

	for (INT32 i=0; i<4; i++)
	{
		Parallel[i].translate(-Start.x, -Start.y);
		Rotate2.transform(&Parallel[i]);
		Parallel[i].translate(Start.x, Start.y);
	}

	DocRect Bounds = DocRect(Parallel[0], Parallel[0]);
	Bounds.IncludePoint(Parallel[1]);
	Bounds.IncludePoint(Parallel[2]);
	Bounds.IncludePoint(Parallel[3]);

	return Bounds;
#else
	return DocRect(0,0,0,0);
#endif
}

/********************************************************************************************

>	void MakePerpendicularMeshLines(DocCoord Start, DocCoord End, 
										DocRect Bounds, DocCoord* PointArray)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/94
	Inputs:		-
	Returns:	-
	Purpose:	Makes lines perpendicular to a fill line. 

********************************************************************************************/

void MakePerpendicularMeshLines(DocCoord Start, DocCoord End, 
										DocRect Bounds, DocCoord* PointArray)
{
#if !defined(EXCLUDE_FROM_RALPH)
	INT32 Infinity = max(Bounds.Width(), Bounds.Height());

	PointArray[0] = MakeLineAtAngle(Start, End,  90, Infinity);
	PointArray[1] = MakeLineAtAngle(Start, End, -90, Infinity);
	PointArray[2] = MakeLineAtAngle(End, Start,  90, Infinity);
	PointArray[3] = MakeLineAtAngle(End, Start, -90, Infinity);
#endif
}

/********************************************************************************************

>	DocCoord MakeLineAtAngle(DocCoord Start, DocCoord End, ANGLE Offset, INT32 Length)
									DocRect Bounds, DocCoord* PointArray)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/94
	Inputs:		-
	Returns:	-
	Purpose:	Make a line at a givel angle to another line. 

********************************************************************************************/

DocCoord MakeLineAtAngle(DocCoord Start, DocCoord End, ANGLE Offset, INT32 Length)
{
	const INT32 dx = End.x - Start.x;
	const INT32 dy = End.y - Start.y;

	if ( (dy==0) && (dx==0) )
	{
		return End;
	}

	DocCoord End2;

	if (Length == 0 && Offset == ANGLE(90))
	{
		End2.x = Start.x - (End.y - Start.y);
		End2.y = Start.y + (End.x - Start.x);

		return End2;
	}
	else if (Length == 0 && Offset == ANGLE(-90))
	{
		End2.x = Start.x + (End.y - Start.y);
		End2.y = Start.y - (End.x - Start.x);

		return End2;
	}
	else if (Length == 0 && (Offset == ANGLE(180) || Offset == ANGLE(-180)))
	{
		End2.x = Start.x - (End.x - Start.x);
		End2.y = Start.y - (End.y - Start.y);

		return End2;
	}

	double LineAngle=0;

	if ( (dx>=0) && (dy>=0) )
		LineAngle = PI - atan2( (double)dx, (double)dy );
	else if ( (dx<0) && (dy>=0) )
		LineAngle = -PI + atan2( (double)-dx, (double)dy );
	else if ( (dx<0) && (dy<0) )
		LineAngle = -atan2( (double)-dx, (double)-dy );
	else if ( (dx>=0) && (dy<0) )
		LineAngle = atan2( (double)dx, (double)-dy );

	ANGLE RotateAngle = 360 * (LineAngle / (2*PI));	// Convert from radians to degrees

	double Length1 = Length;
   	double Length2 = Start.Distance(End);

	if ( INT32(Length2) == 0)
		return End;

	if ( INT32(Length1) == 0)
		Length1 = Length2;

	FIXED16 LineScale = FIXED16(Length1/Length2);

	Matrix Rotate1 = Matrix(-RotateAngle);
	Matrix Rotate2 = Matrix(RotateAngle + Offset);
	Matrix Scale = Matrix(LineScale, LineScale);

	End2 = End;

	End2.translate(-Start.x, -Start.y);
	Rotate1.transform(&End2);
	Scale.transform(&End2);
	Rotate2.transform(&End2);
	End2.translate(Start.x, Start.y);

	return End2;
}

/********************************************************************************************

>	ANGLE CalcLineAngle(DocCoord Start, DocCoord End)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/94
	Inputs:		-
	Returns:	-
	Purpose:	Calculates the Angle of a Line. 0 is Straight Down.

********************************************************************************************/

ANGLE CalcLineAngle(DocCoord Start, DocCoord End)
{
	const INT32 dx = End.x - Start.x;
	const INT32 dy = End.y - Start.y;

	if ( (dy==0) && (dx==0) )
	{
		return 0;
	}

	double LineAngle=0;

	if ( (dx>=0) && (dy>=0) )
		LineAngle = PI - atan2( (double)dx, (double)dy );
	else if ( (dx<0) && (dy>=0) )
		LineAngle = -PI + atan2( (double)-dx, (double)dy );
	else if ( (dx<0) && (dy<0) )
		LineAngle = -atan2( (double)-dx, (double)-dy );
	else if ( (dx>=0) && (dy<0) )
		LineAngle = atan2( (double)dx, (double)-dy );

	return 360 * (LineAngle / (2*PI));	// Convert from radians to degrees
}

/********************************************************************************************

>	DocCoord CentreOf(DocRect Rect)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/94
	Inputs:		-
	Returns:	-
	Purpose:	Calculates the centre of a rectangle.

********************************************************************************************/

DocCoord CentreOf(DocRect Rect)
{
	return DocCoord(Rect.lo.x + (Rect.Width()/2), Rect.lo.y + (Rect.Height()/2));
}

/********************************************************************************************

>	void MakeMeshParallelagram(Path* ParaPath, DocCoord Start, DocCoord End, DocCoord End2)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/94
	Inputs:		-
	Returns:	-
	Purpose:	Makes a Parallelagram for the Bitmap fill mesh.

********************************************************************************************/

void MakeMeshParallelagram(Path* ParaPath, DocCoord Start, DocCoord End, DocCoord End2)
{
#if !defined(EXCLUDE_FROM_RALPH)
	INT32 dx1 = (End2.x - Start.x);
	INT32 dy1 = (End2.y - Start.y);

	INT32 dx2 = (End.x - Start.x);
	INT32 dy2 = (Start.y - End.y);

	// build a path
	ParaPath->ClearPath();
	ParaPath->FindStartOfPath();
			
	ParaPath->InsertMoveTo(DocCoord(Start.x + dx1 + dx2, Start.y + dy1 - dy2));
	ParaPath->InsertLineTo(DocCoord(Start.x - dx1 + dx2, Start.y - dy1 - dy2));
	ParaPath->InsertLineTo(DocCoord(Start.x - dx1 - dx2, Start.y - dy1 + dy2));
	ParaPath->InsertLineTo(DocCoord(Start.x + dx1 - dx2, Start.y + dy1 + dy2));
	ParaPath->InsertLineTo(DocCoord(Start.x + dx1 + dx2, Start.y + dy1 - dy2));
#endif
}

/********************************************************************************************

>	void GetMeshParallelagram(DocCoord Start, DocCoord End, DocCoord End2,
							DocCoord* P1, DocCoord* P2, DocCoord* P3, DocCoord* P4)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/94
	Inputs:		-
	Returns:	-
	Purpose:	Makes a Parallelagram for the Bitmap fill mesh.
				Not used any more.

********************************************************************************************/

void GetMeshParallelagram(DocCoord Start, DocCoord End, DocCoord End2,
							DocCoord* P1, DocCoord* P2, DocCoord* P3, DocCoord* P4)
{
#if !defined(EXCLUDE_FROM_RALPH)
	INT32 dx1 = (End2.x - Start.x);
	INT32 dy1 = (End2.y - Start.y);

	INT32 dx2 = (End.x - Start.x);
	INT32 dy2 = (Start.y - End.y);

	*P1 = DocCoord(Start.x + dx1 + dx2, Start.y + dy1 - dy2);
	*P2 = DocCoord(Start.x - dx1 + dx2, Start.y - dy1 - dy2);
	*P3 = DocCoord(Start.x - dx1 - dx2, Start.y - dy1 + dy2);
	*P4 = DocCoord(Start.x + dx1 - dx2, Start.y + dy1 + dy2);
#endif
}

/********************************************************************************************

>	DocRect GetParallelagramBounds(DocCoord Start, DocCoord End, DocCoord End2)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/94
	Inputs:		-
	Returns:	-
	Purpose:	Gets the Bounding rect of the Bitmap Fill Mesh. 

********************************************************************************************/

DocRect GetParallelagramBounds(DocCoord Start, DocCoord End, DocCoord End2)
{
	DocCoord P1(0,0);
	DocCoord P2;
	DocCoord P3;
	DocCoord P4;

	GetMeshParallelagram(Start, End, End2, &P1, &P2, &P3, &P4);

	DocRect Bounds = DocRect(P1, P1);

	Bounds.IncludePoint(P2);
	Bounds.IncludePoint(P3);
	Bounds.IncludePoint(P4);
	
	return Bounds;
}

/********************************************************************************************

>	void GetBitmapVirtualPoints(DocCoord Start, DocCoord End, DocCoord End2,
							DocCoord* P1, DocCoord* P2, DocCoord* P3)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/94
	Inputs:		-
	Returns:	-
	Purpose:	Calculates the Virtual points of a bitmap from the actual bitmap control
				points given. 

********************************************************************************************/

void GetBitmapVirtualPoints(DocCoord Start, DocCoord End, DocCoord End2,
							DocCoord* P1, DocCoord* P2, DocCoord* P3)
{
#if !defined(EXCLUDE_FROM_RALPH)
	double dx1 = (End2.x - Start.x);
	double dy1 = (End2.y - Start.y);

	double dx2 = (End.x - Start.x);
	double dy2 = (Start.y - End.y);

	if (P1 != NULL && (Start != DocCoord(0,0)))
    	*P1 = DocCoord(INT32(double(Start.x) + dx1/2 + dx2/2), INT32(double(Start.y) + dy1/2 - dy2/2));
	if (P2 != NULL && (End != DocCoord(0,0)))
		*P2 = DocCoord(INT32(double(Start.x) + dx1/2 + dx2),   INT32(double(Start.y) + dy1/2 - dy2));
	if (P3 != NULL && (End2 != DocCoord(0,0)))
		*P3 = DocCoord(INT32(double(Start.x) + dx1   + dx2/2), INT32(double(Start.y) + dy1   - dy2/2));
#endif
}

/********************************************************************************************

>	void GetBitmapRealPoints(DocCoord Start, DocCoord End, DocCoord End2,
							DocCoord* P1, DocCoord* P2, DocCoord* P3)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/07/94
	Inputs:		-
	Returns:	-
	Purpose:	Calculates the Real Control points of a bitmap from the vitual points given. 

********************************************************************************************/

void GetBitmapRealPoints(DocCoord Start, DocCoord End, DocCoord End2,
							DocCoord* P1, DocCoord* P2, DocCoord* P3)
{
#if !defined(EXCLUDE_FROM_RALPH)
	INT32 dx1 = (End2 == DocCoord(0,0)) ? 0 : (End2.x - Start.x);
	INT32 dy1 = (End2 == DocCoord(0,0)) ? 0 : (End2.y - Start.y);

	INT32 dx2 = (End.x - Start.x);
	INT32 dy2 = (Start.y - End.y);

	if (P1 != NULL && (Start != DocCoord(0,0)))
		*P1 = DocCoord(Start.x - dx1 - dx2, Start.y - dy1 + dy2);
	if (P2 != NULL && (End != DocCoord(0,0)))
		*P2 = DocCoord(Start.x - dx1 + dx2, Start.y - dy1 - dy2);
	if (P3 != NULL && (End2 != DocCoord(0,0)))
		*P3 = DocCoord(Start.x + dx1 - dx2, Start.y + dy1 + dy2);
#endif
}

/********************************************************************************************

>	void IncludeArrowHead(DocRect* Bounds, DocCoord Start, DocCoord End)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/9/94
	Inputs:		-
	Returns:	-
	Purpose:	Includes the Arrow head in the bounds calculations.

********************************************************************************************/

void IncludeArrowHead(DocRect* Bounds, DocCoord Start, DocCoord End)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// First we Must caculate the angle of the arrow from the line angle
	const INT32 dx = Start.x - End.x;
	const INT32 dy = Start.y - End.y;

	if ( (dy==0) && (dx==0) )
	{
		return;								// draw nothing as too small
	}

	double ArrowAngle;

	if ( (dx>=0) && (dy>=0) )
		ArrowAngle = PI - atan2( (double)dx, (double)dy );
	else if ( (dx<0) && (dy>=0) )
		ArrowAngle = -PI + atan2( (double)-dx, (double)dy );
	else if ( (dx<0) && (dy<0) )
		ArrowAngle = -atan2( (double)-dx, (double)-dy );
	else if ( (dx>=0) && (dy<0) )
		ArrowAngle = atan2( (double)dx, (double)-dy );
	else
	{
		TRACE( _T("invalid angle on arrowhead\n"));		// this should be impossible
		return;
	}

	ANGLE RotateAngle = 360 * (ArrowAngle / (2*PI));	// Convert from radians to degrees

    Matrix ArrowMatrix = Matrix(RotateAngle);			// Rotate it
    ArrowMatrix *= Matrix(End);							// Move it to the line point

	const UINT32 PointCount = 3;
	Coord ArrowPts[PointCount];

	INT32 BlobSize = (Camelot.GetBlobManager())->GetBlobSize();

	INT32 ArrowWidth  = (BlobSize  * 3)/2;
	INT32 ArrowHeight = (BlobSize  * 3)/2;
    
	ArrowPts[0] = Coord(             0,            0);
	ArrowPts[1] = Coord( -ArrowWidth/2, -ArrowHeight);
	ArrowPts[2] = Coord(  ArrowWidth/2, -ArrowHeight);

	ArrowMatrix.transform(ArrowPts, PointCount);

	Bounds->IncludePoint(DocCoord(ArrowPts[0].x, ArrowPts[0].y));
	Bounds->IncludePoint(DocCoord(ArrowPts[1].x, ArrowPts[1].y));
	Bounds->IncludePoint(DocCoord(ArrowPts[2].x, ArrowPts[2].y));
#endif
}

/********************************************************************************************

>	BOOL AreLinesPerpendicular(DocCoord* Start, DocCoord* End, DocCoord* End2)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95
	Inputs:		-
	Returns:	-
	Purpose:	Tests to see if two fill control lines are perpendicular or not.
				Uses the 'Similar Triangles' method, but allows a 5 millipoint error.

********************************************************************************************/

BOOL AreLinesPerpendicular(DocCoord* Start, DocCoord* End, DocCoord* End2)
{
	INT32 dx1 = (*End).x - (*Start).x;
	INT32 dy1 = (*End).y - (*Start).y;

	INT32 dx2 = (*End2).x - (*Start).x;
	INT32 dy2 = (*End2).y - (*Start).y;

	if ( (ABS(dx1 - dy2)) > 5 )	// Allow a few millipoints error
		return FALSE;

	if ( (ABS(dy1 + dx2)) > 5 )	// Allow a few millipoints error
		return FALSE;
	
	return TRUE;
}

////////////////////////////////////////////////////////////
// RestoreFillRampAction class
/********************************************************************************************

>	RestoreFillRampAction::RestoreFillRampAction()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/10/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

RestoreFillRampAction::RestoreFillRampAction()
{
	m_pAttr  = NULL;
}


/********************************************************************************************

>	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							AttrFillGeometry* pFill,
							RestoreFillRampAction** NewAction,
	

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pFill			= ptr to the fill geometry attribute to store its ramp
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
								  a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Stores the fill ramp of the given attribute for restoring on undo
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/



ActionCode RestoreFillRampAction::Init( Operation* pOp,
							ActionList* pActionList,
							AttrFillGeometry* pFill,
							RestoreFillRampAction** NewAction)
{
	UINT32 ActSize = sizeof(RestoreFillRampAction);

	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,
		CC_RUNTIME_CLASS(RestoreFillRampAction),
		(Action**)NewAction);

//	Document * pDoc = Document::GetCurrent();	

	DocRect dr;

	if (Ac != AC_FAIL)
	{
		if (pFill->GetColourRamp() != NULL)
		{
			(*NewAction)->m_LastRamp = *pFill->GetColourRamp();
		}		
		// bugfix #6835 (Marc 22/09/04) - make sure we store the fill even if there is no ramp
		(*NewAction)->m_pAttr    = pFill;
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode RestoreFillRampAction::Execute();

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/96
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the num blend steps in pThisNodeBlend to OldNumSteps,
				after creating another action to record the current num steps of pThisNodeBlend
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode RestoreFillRampAction::Execute()
{
	ActionCode Act;
	RestoreFillRampAction* pAction;

	Act = RestoreFillRampAction::Init(	pOperation, 
										pOppositeActLst,
										m_pAttr,
										&pAction
										);

	if (Act != AC_FAIL)
	{
		Document * pDoc = Document::GetCurrent();
		
		if (pDoc)
		{
			/*
			DocRect dr = pController->GetBoundingRect();
			
			pDoc->ForceRedraw(pController->FindParentSpread(), 
				dr, FALSE);
				*/

			// CGS - we need to check for a NULL colour ramp - cause we may have deleted it!
			
			ColourRamp* ColRamp = m_pAttr->GetColourRamp();

			if (ColRamp)
			{
				*m_pAttr->GetColourRamp() = m_LastRamp;
			}
			else
			{
				// if it is NULL, then we need to recreate one BEFORE making the above call!

				ColRamp = new ColourRamp ();
				m_pAttr->SetColourRamp (ColRamp);
				*m_pAttr->GetColourRamp() = m_LastRamp;
			}
		}
	}
	
	return Act;
}

RestoreFillRampAction::~RestoreFillRampAction()
{
}

