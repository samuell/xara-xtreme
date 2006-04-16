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
// sgdrag.cpp - Drag Manager classes (information, targets) for supergallery scollbars
//				and generic supergallery item dragging (for rearrangement only)


/*
*/


#include "camtypes.h"

#include "fixmem.h"
//#include "galres.h"
//#include "galstr.h"
//#include "resource.h"
#include "sgallery.h"
#include "sgdrag.h"
#include "sgtree.h"
#include "oilcoord.h"


DECLARE_SOURCE("$Revision$");


CC_IMPLEMENT_DYNAMIC(SGScrollDragTarget, KernelDragTarget)
CC_IMPLEMENT_DYNCREATE(SGScrollDragInfo, DragInformation)

CC_IMPLEMENT_DYNAMIC(SGListDragTarget, KernelDragTarget)
CC_IMPLEMENT_DYNCREATE(SGListDragInfo, DragInformation)



#define new CAM_DEBUG_NEW



const INT32 AUTOREPEATTIME = 150;			// Auto-repeat a maximum of ~7 times per second
											// (This is the delay, in milliseconds)


/********************************************************************************************

>	SGScrollDragTarget::SGScrollDragTarget(DialogOp *TheDialog, CGadgetID TheGadget = NULL)
	 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/2/95
	Inputs:		TheDialog - The kernel dialog in which the target exists
				TheGadget - The gadget within that dialogue which is the target

	Purpose:	Constructor

********************************************************************************************/

SGScrollDragTarget::SGScrollDragTarget(DialogOp *TheDialog, CGadgetID TheGadget)
					: KernelDragTarget(TheDialog, TheGadget)
{
	Timer.Sample();			// Remember the time at which we were created, for autorepeat

	IWantAllEvents = TRUE;	// Ask for all events to be given to us, even if the pointer
							// strays out of our target window area.
}



/********************************************************************************************

	BOOL SGScrollDragTarget::ProcessEvent(DragEventType Event, DragInformation *pDragInfo,
										OilCoord *pMousePos, KeyPress* pKeyPress)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/2/95		  
	Inputs:		Event - Indicates what has happened
				pDragInfo - points to drag information describing this drag. This
				should be an SGScrollDragInfo or something has gone seriously wrong.
				pMousePos - points to information on the current mouse position, in OIL coords
				pKeyPress - NULL, or if for a keypress event, keypress information

	Returns:	TRUE to claim the event, FALSE to let it through to other targets
				This particular handler always returns TRUE

	Purpose:	Event Handler for SuperGallery scrollbar drag events

********************************************************************************************/

BOOL SGScrollDragTarget::ProcessEvent(DragEventType Event, DragInformation *pDragInfo,
										OilCoord *pMousePos, KeyPress* pKeyPress)
{
	// Weed out events that we are not interested in
	switch(Event)
	{
		case DRAGEVENT_ABORT:
		case DRAGEVENT_COMPLETED:
			// Just claim the event as ours.
			// We don't get these events unless the pointer is over us, so we don't do
			// anything here - see the deinitialise handler
			return(TRUE);

		case DRAGEVENT_INITIALISE:
		case DRAGEVENT_DEINITIALISE:
		case DRAGEVENT_MOUSEMOVED:
		case DRAGEVENT_MOUSESTOPPED:
		case DRAGEVENT_MOUSEIDLE:
			// We want these events, so break to drop through and process them
			break;

		default:
			// An unknown/unwanted event type - it could be dangerous to do something on
			// an unknown event, so we won't!
			return(TRUE);
	}

	// Cast the draginfo into the correct type
	SGScrollDragInfo *DragInfo = (SGScrollDragInfo *) pDragInfo;

	// FALSE if we aren't dragging, but just handling auto-repeat
	BOOL NotAutoRepeat = (DragInfo->GetDragType() == SGDRAG_SAUSAGE);

	SGDisplayRootScroll *Root	= DragInfo->GetDragRootNode();
	SGMiscInfo *MiscInfo		= DragInfo->GetDragMiscInfo();
	SuperGallery *ParentGallery = Root->GetParentGallery();

	// Convert the OilCoord into a DocCoord, and convert into displaylist coords
	DocCoord MousePos(pMousePos->x, pMousePos->y);
	ParentGallery->ConvertToVirtualCoords(MiscInfo, &MousePos);

	// And calculate the relevant rectangles...
	DocRect UpButton;
	DocRect DownButton;
	DocRect SausageRect;
	DocRect PageUp;
	DocRect PageDown;
	DocRect ScrollRect;
	Root->CalculateScrollRects(MiscInfo, TRUE,
								 &UpButton, &DownButton, &SausageRect,
								 &PageUp, &PageDown, &ScrollRect);

	if (Event == DRAGEVENT_MOUSEMOVED && NotAutoRepeat)
	{
		// Dragging the scroll sausage

		// Get the offset - the scroll sausage should try to move this point of the
		// sausage (measured in millipoints from the top of the sausage) to lie under
		// the mouse pointer position if possible.
		INT32 AnchorOffset = DragInfo->GetDragAnchorOffset();

		INT32 ScrollWindowTo;
		
		if (SausageRect.Height() <= SausageRect.Width())
		{
			// The sausage is a minimum-size square, so now does not proportionally represent
			// the displayed area of the window. Thus, we need to use the top position of the
			// bar in (barheight - sausageheight) as a fractional scroll between 0 and 
			// (ScrollExtent - WindowHeight)
			ScrollWindowTo = (INT32) (
									( ((double) (ScrollRect.hi.y - (MousePos.y + AnchorOffset))) / 
										(double) (ScrollRect.Height() - SausageRect.Height()) ) *
										 	(double) (Root->GetCachedListExtent() - MiscInfo->WindowHeight)
									);
		}
		else
		{
			// The bar represents the fraction of visible area to extent, so we work out the
			// fractional position of the top of the bar, and that gives the position to scroll
			// the top of the displayed area to.
			ScrollWindowTo = (INT32) (
									( ((double) (ScrollRect.hi.y - (MousePos.y + AnchorOffset))) / 
										(double) ScrollRect.Height() ) *
									 		(double) Root->GetCachedListExtent()
									);
		}

		Root->SetScrollOffset(ScrollWindowTo, MiscInfo);
		return(TRUE);
	}


	// The drag must therefore be to implement simple AutoRepeat on a button or page-scroll.
	// We only bother scrolling if:
	//		1) Event is 'initialise' (so it scrolls immediately the button goes down)
	//		2) Event is 'deinitialise' (so we must ensure all buttons are popped up)
	//		3) Or if it is time for another autorepeat
	if (Event != DRAGEVENT_INITIALISE	&& 
		Event != DRAGEVENT_DEINITIALISE	&&
		!Timer.Elapsed(AUTOREPEATTIME))
	{
		// No need to update just now. Return, claiming this event
		return(TRUE);
	}

	const INT32 ClickScroll = 12000;		// Millipoint distance to scroll for a button click
	INT32 ScrollBy = 0;					// Set this to the amount by which to scroll

	Timer.Sample();						// Update the autorepeat timer

	switch (DragInfo->GetDragType())
	{
		case SGDRAG_SCROLLUP:
			if (Event == DRAGEVENT_DEINITIALISE)
			{
				// Drag completed: Unindent the scroll button
				Root->IndentedButton = IBUTTON_NONE;
				ParentGallery->ForceRedrawOfArea(&UpButton);
				ParentGallery->PaintListNow();
			}
			else if (UpButton.ContainsCoord(MousePos))
			{
				ScrollBy = -ClickScroll;

				// Indent the scroll button
				Root->IndentedButton = IBUTTON_UP;
				ParentGallery->ForceRedrawOfArea(&UpButton);
				ParentGallery->PaintListNow();
			}
			break;


		case SGDRAG_SCROLLDOWN:
			if (Event == DRAGEVENT_DEINITIALISE)
			{
				// Drag completed: Unindent the scroll button
				Root->IndentedButton = IBUTTON_NONE;
				ParentGallery->ForceRedrawOfArea(&DownButton);
				ParentGallery->PaintListNow();
			}
			else if (DownButton.ContainsCoord(MousePos))
			{
				ScrollBy = ClickScroll;

				// Indent the scroll button
				Root->IndentedButton = IBUTTON_DOWN;
				ParentGallery->ForceRedrawOfArea(&DownButton);
				ParentGallery->PaintListNow();
			}
			break;


		case SGDRAG_PAGEUP:
			if (PageUp.IsValid() && PageUp.ContainsCoord(MousePos))
			{
				ScrollBy = MiscInfo->WindowHeight - ClickScroll;
				if (ScrollBy < 0)
					ScrollBy = ClickScroll;

				ScrollBy = -ScrollBy;
			}
			break;


		case SGDRAG_PAGEDOWN:
			if (PageDown.IsValid() && PageDown.ContainsCoord(MousePos))
			{
				ScrollBy = MiscInfo->WindowHeight - ClickScroll;
				if (ScrollBy < 0)
					ScrollBy = ClickScroll;
			}
			break;

		default:
			break;
	}

	if (Event != DRAGEVENT_DEINITIALISE && ScrollBy != 0)
	{
		if (DragInfo->IsAdjustDrag())	// If adjust-drag, reverse scroll direction
			ScrollBy = -ScrollBy;

		Root->SetScrollOffset(Root->GetScrollOffset() + ScrollBy, MiscInfo);
	}

	// We always claim all events during this drag (it is not a drag to a particular place
	// to move an object - really we're using the drag system to "capture" the mouse)
	return(TRUE);
}



/********************************************************************************************

>	void SGScrollDragTarget::GetCursorID()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/2/95

	Purpose:	Base Method to set cursor over this target

********************************************************************************************/

UINT32 SGScrollDragTarget::GetCursorID()
{
	return(0);
}



/********************************************************************************************

>	virtual BOOL SGScrollDragTarget::GetStatusLineText(String_256 * TheText)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/2/95

	Returns:	FALSE - we never provide any status line help for scroll-drags

	Purpose:	Provide status line text for this target
   
********************************************************************************************/

BOOL SGScrollDragTarget::GetStatusLineText(String_256 * TheText)
{
	return FALSE;
}
















/********************************************************************************************

>	void SGScrollDragInfo::SGScrollDragInfo() 
	 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/2/95		  

	Purpose:	Default constructor. DO NOT CALL THIS CONSTRUCTOR

********************************************************************************************/
SGScrollDragInfo::SGScrollDragInfo()
{
	ERROR3("Default SGScrollDragInfo constructor called");	
}



/********************************************************************************************

>	SGScrollDragInfo::SGScrollDragInfo(SGDisplayRootScroll *ParentRootNode,
										SGDragType TheDragType,
										SGMiscInfo *MiscInfo,
										INT32 DragAnchorOffset,
										BOOL IsAdjust = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/2/95		  

	Inputs:		ParentRootNode - The SGDisplayRootScroll for the DisplayTree for which
				the drag is occurring

				TheDragType - The SGDragType indicating what is being dragged

				MiscInfo - The normal SGMiscInfo as recieved by display tree event handlers

				DragAnchorOffset - The offset, in millipoints, from the top of the scroll
				sausage to the point where the drag started (used only when dragging the
				sausage, to "lock" the sausage to the mouse pointer)

				IsAdjust - TRUE if it is an "adjust" drag [NOTE - Adjust drags are legal
				when dragging scrollbars, and are not turned into clicks like normal drags]				

	Purpose:	Constructor

********************************************************************************************/

SGScrollDragInfo::SGScrollDragInfo(SGDisplayRootScroll *ParentRootNode,
									SGDragType TheDragType,
									SGMiscInfo *MiscInfo,
									INT32 DragAnchorOffset,
									BOOL IsAdjust)
				 : DragInformation(FALSE)
{
	DragRootNode = ParentRootNode;
	DragType = TheDragType;
	DragAnchor = DragAnchorOffset;
	DragAdjust = IsAdjust;

	DragMiscInfo = *MiscInfo;		// Copy the MiscInfo structure for ourselves

	// Set up a few things about this drag - it does not do solid drag
	DoesSolidDrag = FALSE;
	SolidDragOffset.x = SolidDragOffset.y = 0;	// Set up defaults just in case
	SolidDragSize.Set(1,1);
}



/********************************************************************************************

>	virtual UINT32 SGScrollDragInfo::GetCursorID()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/2/95

	Returns:	a cursor ID to set during this drag

	Purpose:	To provide the resource ID of the cursor to use during this drag.
				For scrollbar drags, the cursor should remain the normal arrow shape.
   
********************************************************************************************/

UINT32 SGScrollDragInfo::GetCursorID()
{
	 return(0);
}



/********************************************************************************************

>	virtual BOOL SGScrollDragInfo::GetStatusLineText(String_256 * TheText)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/2/95

	Returns:	FALSE - The string has not been filled in
	Purpose:	Provide status line text for this drag - While dragging scrollbars no help
				is required/given, so this always returns FALSE
   
********************************************************************************************/

BOOL SGScrollDragInfo::GetStatusLineText(String_256 * TheText)
{
	return(FALSE);
}



/********************************************************************************************

>	void SGScrollDragInfo::OnClick(INT32 Flags,POINT Point) 
	 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/2/95		  
	Inputs:		Flags - Some flags
				Point - the point which was clicked

	Purpose:	This is called if a drag was attempted but never started because it was a 
				click all along

********************************************************************************************/

void SGScrollDragInfo::OnClick(INT32 Flags, POINT Point)
{
	// Do nothing. As the thing is scrolled once immediately on SGEVENT_INITIALISE, we
	// have already applied the correct click action.
}















/********************************************************************************************

>	SGListDragTarget::SGListDragTarget(DialogOp *TheDialog, CGadgetID TheGadget = NULL)
	 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95
	Inputs:		TheDialog - The kernel dialog in which the target exists
				TheGadget - The gadget within that dialogue which is the target

	Purpose:	Constructor

********************************************************************************************/

SGListDragTarget::SGListDragTarget(DialogOp *TheDialog, CGadgetID TheGadget)
					: KernelDragTarget(TheDialog, TheGadget)
{
	CurrentCursorID = _R(IDC_SGDRAGCURSOR);	

	ERROR3IF(!TheDialog->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)),
			"You can only use SGListDragTargets with SuperGallery dialogues!");
}



/********************************************************************************************

>	BOOL SGListDragTarget::OwnsEntireLine(SGDisplayNode *TheNode)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/95		  

	Inputs:		TheNode - the node to check

	Returns:	TRUE if the item fills the entire width of the display list

	Purpose:	Determines if the given display node has siblings to its left/right
				or if it fills an entire 'line' of the display list. This is used to
				determine whether to treat the top/bottom or left/right sides of the
				item as insert-before/after dropzones.

********************************************************************************************/

BOOL SGListDragTarget::OwnsEntireLine(SGDisplayNode *TheNode)
{
	DocRect TheRect;
	TheNode->GetFormatRect(&TheRect);

	SGDisplayNode *Ptr = TheNode->GetPrevious();
	if (Ptr != NULL)
	{
		DocRect TempRect;
		Ptr->GetFormatRect(&TempRect);

		// If overlap in the Y axis, then we are on the same line
		if (TempRect.lo.y <= TheRect.hi.y && TempRect.hi.y >= TheRect.lo.y)
			return(FALSE);
	}

	Ptr = TheNode->GetNext();
	if (Ptr != NULL)
	{
		DocRect TempRect;
		Ptr->GetFormatRect(&TempRect);

		// If overlap in the Y axis, then we are on the same line
		if (TempRect.lo.y <= TheRect.hi.y && TempRect.hi.y >= TheRect.lo.y)
			return(FALSE);
	}

	return(TRUE);
}



/********************************************************************************************

>	SGDragInsertType SGListDragTarget::GetDragInsertType(SuperGallery *ParentGallery,
												SGDisplayNode *DraggedNode,
												OilCoord *pMousePos,
												SGDisplayNode *DestNode)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95		  

	Inputs:		ParentGallery - The gallery in which the drag completed

				DraggedNode - The node being dragged

				MousePos - The end-drag position, as passed into DragTarget::ProcessEvent

				DestNode - the DisplayTree node in which the drag completed

	Returns:	An enumerated type indicating whether the node will be added to a group,
				or inserted above or below another node.

	Purpose:	Determines whether we will want to drop an item 'above' or 'below'
				the item under the current pointer position (used to work out which
				pointer shape to use to indicate the action which will be taken when a
				drag ends)

	SeeAlso:	SGListDragTarget::HandleDragCompleted

********************************************************************************************/

SGDragInsertType SGListDragTarget::GetDragInsertType(SuperGallery *ParentGallery,
														SGDisplayNode *DraggedNode,
														OilCoord *pMousePos,
														SGDisplayNode *DestNode)
{
	ERROR3IF(DraggedNode == NULL || pMousePos == NULL ||
				ParentGallery == NULL || DestNode == NULL,
				"Illegal NULL params");

	if (DestNode == DraggedNode || DestNode == DraggedNode->GetParent())
		return(SGDRAGINSERT_NONE);

	// If dragging a group over an item, then we will insert the group below the item's
	// parent group, so we always insert below.
	if (DraggedNode->IsKindOf(CC_RUNTIME_CLASS(SGDisplayGroup)) &&
		DestNode->IsKindOf(CC_RUNTIME_CLASS(SGDisplayItem)))
	{
		// Dragging a group over an item
		SGDisplayNode *DestParent = DestNode->GetParent();

		// It's an item in the group which is being dragged, so no point in dropping here
		if (DestParent == DraggedNode)
			return(SGDRAGINSERT_NONE);

		// It's an item in the previous group, so we would insert back where we came from
		if (DestParent == DraggedNode->GetPrevious())
			return(SGDRAGINSERT_NONE);

		// It's a different group, so we will insert after that group
		return(SGDRAGINSERT_AFTER);
	}


	// If dragging an item over a group, then we'll ADD to the group
	if ((DraggedNode->IsKindOf(CC_RUNTIME_CLASS(SGDisplayItem))) &&
		(DestNode->IsKindOf(CC_RUNTIME_CLASS(SGDisplayGroup))) )
	{
		// You can't drop an item into a read-only group (but we still allow moving of items
		// within a read-only group)
		if (DestNode != DraggedNode->GetParent() && DestNode->Flags.ReadOnly)
			return(SGDRAGINSERT_NONE);

		return(SGDRAGINSERT_ADD);
	}


	// If we're dragging an item, check that the destination is legal (not read-only)
	if (DraggedNode->IsKindOf(CC_RUNTIME_CLASS(SGDisplayItem)))
	{
		SGDisplayNode *DestParent = DestNode->GetParent();

		// You can't drop an item into a read-only group (but we still allow moving of items
		// within a read-only group)
		if (DestParent != NULL && DestParent != DraggedNode->GetParent() && DestParent->Flags.ReadOnly)
			return(SGDRAGINSERT_NONE);
	}


	SGMiscInfo MiscInfo;
	ParentGallery->FillInMiscInfo(&MiscInfo);

	// Determine where within the display item the drag was dropped
	DocCoord ListMouseCoord(pMousePos->x, pMousePos->y);
	ParentGallery->ConvertToVirtualCoords(&MiscInfo, &ListMouseCoord);

	DocRect DestRect;
	DestNode->GetFormatRect(&DestRect);

	BOOL IsInsertBefore = FALSE;

	if (OwnsEntireLine(DestNode))
	{
		// This item is full-width, so use top/bottom half as dropzones
		if (ListMouseCoord.y > (DestRect.lo.y + DestRect.hi.y) / 2)
			IsInsertBefore = TRUE;
	}
	else
	{
		// This item is not full width, so use left/right half as dropzones
		if (ListMouseCoord.x < (DestRect.lo.x + DestRect.hi.x) / 2)
			IsInsertBefore = TRUE;
	}

	// Finally, one last check to see if we're still trying to drop the item back
	// into the same place.
	if (IsInsertBefore)
	{
		if (DestNode->GetPrevious() == DraggedNode)
			return(SGDRAGINSERT_NONE);
	}
	else
	{
		if (DestNode->GetNext() == DraggedNode)
			return(SGDRAGINSERT_NONE);
	}

	return(IsInsertBefore ? SGDRAGINSERT_BEFORE : SGDRAGINSERT_AFTER);
}



/********************************************************************************************

>	void SGListDragTarget::HandleDragCompleted(SuperGallery *ParentGallery,
												SGDisplayNode *DraggedNode,
												OilCoord *pMousePos,
												BOOL DragThisItemOnly = FALSE)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95		  
	Inputs:		ParentGallery - The gallery in which the drag completed

				DraggedNode - The node being dragged

				MousePos - The end-drag position, as passed into DragTarget::ProcessEvent

				DragThisItemOnly - if TRUE, indicates that only the 'DragggedNode'
				should be dragged. If FALSE, then all items in the current gallery
				selection, plus DraggedNode, will be treated as if they were dragged.

	Purpose:	Moves/copies gallery items as appropriate when a drag completes

********************************************************************************************/

void SGListDragTarget::HandleDragCompleted(SuperGallery *ParentGallery,
											SGDisplayNode *DraggedNode,
											OilCoord *pMousePos,
											BOOL DragThisItemOnly)
{
	TRACEUSER("amb", _T("SGListDragTarget::HandleDragCompleted"));
	ERROR3IF(DraggedNode == NULL || pMousePos == NULL || ParentGallery == NULL,
				"Illegal NULL params");

	if (ParentGallery == NULL)
		return;

	// Convert the OilCoords into DocCoords
	DocCoord MousePos(pMousePos->x, pMousePos->y);
	SGDisplayNode *DestNode = ParentGallery->FindNodeUnderPointer(&MousePos);

	if (DestNode == NULL 				||		// No valid target
		DestNode == DraggedNode 		||		// Dropped back where we started
		(DragThisItemOnly && DestNode == DraggedNode->GetParent()))	// Dropped into own parent group
	{
		return;
	}


	BOOL InsertBefore = FALSE;			// TRUE if should insert-before the DestNode
	switch (GetDragInsertType(ParentGallery, DraggedNode, pMousePos, DestNode))
	{
		case SGDRAGINSERT_NONE:
			// The drag has no effect, so we exit now
			return;

		case SGDRAGINSERT_ADD:
		case SGDRAGINSERT_AFTER:
			// We need take no specific action here
			break;

		case SGDRAGINSERT_BEFORE:
			InsertBefore = TRUE;
			break;

		default:
			ERROR3("Illegal return value from GetDragInsertType");
			return;
	}

	// TRUE if dragged a group, FALSE if dragged an item
	BOOL GroupBeingDragged = DraggedNode->IsKindOf(CC_RUNTIME_CLASS(SGDisplayGroup));

	// Determine if we need to insert multiple nodes, and compile a list of
	// all the nodes to affect, if necessary. This list is then used to apply
	// the drag-finish to all selected items (done this way, as the movements
	// may well affect the selection in nasty ways, so we must cache it before
	// we move stuff around)
	INT32 ItemCount = 1;
	INT32 ItemIndex;
	if (!DragThisItemOnly)
	{
		if (GroupBeingDragged)
			ItemCount = ParentGallery->GetSelectedGroupCount();
		else
			ItemCount = ParentGallery->GetSelectedItemCount(NULL, NULL);

		if (ItemCount < 1)	// Just in case DraggedNode is not selected!
			ItemCount = 1;
	}

	SGDisplayNode **DraggedArray = NULL;
	SGDisplayRoot *DisplayTree = ParentGallery->GetDisplayTree();

	if (ItemCount > 1 && DisplayTree != NULL)
	{
		DraggedArray = (SGDisplayNode **) CCMalloc(ItemCount * sizeof(SGDisplayNode *));

		if (DraggedArray != NULL)
		{
			SGDisplayNode *Ptr = NULL;
			for (ItemIndex =0; ItemIndex < ItemCount; ItemIndex++)
			{
				if (GroupBeingDragged)
					Ptr = DisplayTree->FindNextSelectedGroup(Ptr);
				else
					Ptr = DisplayTree->FindNextSelectedItem(Ptr);
				DraggedArray[ItemIndex] = Ptr;

				ERROR3IF(Ptr == NULL, "Gallery selection ran out before selection count did");
			}
		}
	}

	if (DraggedArray == NULL)	// Just ensure we're safe
		ItemCount = 1;

	// OK, For each item we must move, apply the appropriate action.
	// If there is only one item, DraggedArray will be NULL, and ItemCount will be 1, in
	// which case, we'll only do the loop once, using DraggedNode as the dropped thing.

	// Determine if the DestNode is a group or an item, and determine the group affected (TargetGroup)
	BOOL TargetIsGroup	= DestNode->IsKindOf(CC_RUNTIME_CLASS(SGDisplayGroup));
	SGDisplayGroup *TargetGroup = (SGDisplayGroup *) ((TargetIsGroup) ? DestNode : DestNode->GetParent());
	ERROR3IF(TargetGroup == NULL, "Drag target display group is NULL!?");

	SGDisplayNode *NewItem = NULL;

	for (ItemIndex = 0; ItemIndex < ItemCount; ItemIndex++) 
	{
		if (DraggedArray != NULL)
			DraggedNode = DraggedArray[ItemIndex];

		NewItem = DraggedNode;

		if (DraggedNode->IsKindOf(CC_RUNTIME_CLASS(SGDisplayGroup)))
		{
			// Dragged node is a group, so move before/after the target group
			if (InsertBefore)
				TargetGroup->MoveBefore(DraggedNode);
			else
				TargetGroup->MoveAfter(DraggedNode);
		}
		else
		{
			// Dragged node is an item
			if (TargetIsGroup)
			{
				// Item was dropped on a group title, so copy into that group
				if (TargetGroup != DraggedNode->GetParent())
					NewItem = ParentGallery->CopyDisplayItem((SGDisplayItem *)DraggedNode, TargetGroup);
				// else it's being moved into its own group - do nothing
			}
			else
			{
				// Item was dropped on another item, so use that item as the target position
				if (TargetGroup != DraggedNode->GetParent())
				{
					// But it's in another group, so we must copy the item across
					if (InsertBefore)
					{
						NewItem = ParentGallery->CopyDisplayItem((SGDisplayItem *)DraggedNode, TargetGroup,
																	(SGDisplayItem *)DestNode);
					}
					else
					{
						// We want to insert *after* the DestNode, so we make the target node the NEXT node
						// after DestNode (as this routine alwyas does an InsertBefore operation)
						NewItem = ParentGallery->CopyDisplayItem((SGDisplayItem *)DraggedNode, TargetGroup,
																	(SGDisplayItem *) (DestNode->GetNext()));
					}
				}
				else
				{
					// We're just moving it within its own group
					if (InsertBefore)
						DestNode->MoveBefore(DraggedNode);
					else
					{
						// We have to be careful with MoveAfter to retain the correct order
						if (ItemIndex == 0 || DraggedArray[ItemIndex-1] == NULL)
							DestNode->MoveAfter(DraggedNode);
						else
							DraggedArray[ItemIndex-1]->MoveAfter(DraggedNode);
					}
				}
			}
		}

		// Update the selection state for copied items
		if (DraggedNode != NewItem)
		{
			if (DraggedNode != NULL)
				DraggedNode->SetSelected(FALSE);

			if (NewItem != NULL)
			{
				// Poke the flags directly to avoid an attempt to redraw, as at this point
				// the item's formatrect will be uninitialised (well, (0,0,0,0))
				NewItem->Flags.Selected = TRUE;
			}
		}

		// Update the last item in the array to point at the copy rather than the source
		if (ItemIndex > 0)
			DraggedArray[ItemIndex] = NewItem;
	}	// end for

	if (DraggedArray != NULL)						// Free up our temporary memory allocation
		CCFree(DraggedArray);

	ParentGallery->InvalidateCachedFormat();		// Redraw any changed areas of the list
	ParentGallery->ReformatAndRedrawIfNecessary();

	ParentGallery->AllItemsCopied(TargetGroup);		// Inform gallery this group has changed

	ParentGallery->SelectionHasChanged();			// And ensure it updates button shading state
}



/********************************************************************************************

>	BOOL SGListDragTarget::DetermineCursorShape(SuperGallery *ParentGallery,
												SGDisplayNode *DraggedNode,
												OilCoord *pMousePos)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/95

	Inputs:		ParentGallery - The gallery in which the drag completed
				DraggedNode - The node which is being dragged
				pMousePos - The end-drag position, as passed into DragTarget::ProcessEvent

	Returns:	TRUE if we should change the cursor shape (return this value
				from mouse events in ProcessEvent to have the drag manager use
				or not-use our pointer as appropriate)

	Purpose:	Determines what mouse pointer shape should be used during a drag
				while the mouse is over a gallery listbox target area. This sets
				the member variable CurrentCursorID, and if you then return the
				return value of this function from your ProcessEvent handler, this
				will result in the cursor shape beng updated correctly.

	Notes:		This is a subroutine to minimise the amount of work involved in
				deriving classes from SGListDragTarget for recieving drags of 
				specific things (e.g. colours, bitmaps, etc) in gallery listboxes

********************************************************************************************/

BOOL SGListDragTarget::DetermineCursorShape(SuperGallery *ParentGallery,
											SGDisplayNode *DraggedNode,
											OilCoord *pMousePos)
{
	// By default, we want a simple pointer cursor
	CurrentCursorID = _R(IDC_SGDRAGCURSOR);

	if (ParentGallery == NULL)
		return(FALSE);

	// Convert the OilCoords into DocCoords
	DocCoord MousePos(pMousePos->x, pMousePos->y);	

	// Make the gallery scroll the list if we're dragging near the top/bottom
	ParentGallery->AutoScrollForDrag(&MousePos);

	SGDisplayNode *DestNode = ParentGallery->FindNodeUnderPointer(&MousePos);
	if (DestNode == NULL)
		return(FALSE);		// No valid target area, so return now

	switch (GetDragInsertType(ParentGallery, DraggedNode, pMousePos, DestNode))
	{
		case SGDRAGINSERT_NONE:
			CurrentCursorID = _R(IDC_DRAGGING_COLOUR);		// No-can-drop cursor shape
			break;
		
		case SGDRAGINSERT_ADD:
			CurrentCursorID = _R(IDC_SGDRAGCURSOR);			// Can drop here
			break;

		case SGDRAGINSERT_BEFORE:
			CurrentCursorID = _R(IDC_SGABOVECURSOR);		// Will insert above
			break;

		case SGDRAGINSERT_AFTER:
			CurrentCursorID = _R(IDC_SGBELOWCURSOR);		// Will insert below
			break;
	}

	// We claim all mouse movements, so that the manager uses our drag pointer
	// while the mouse is over our target area
	return(TRUE);
}



/********************************************************************************************

>	virtual BOOL SGListDragTarget::ProcessEvent(DragEventType Event,
										DragInformation *pDragInfo,
										OilCoord *pMousePos, KeyPress* pKeyPress)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95		  
	Inputs:		Event - Indicates what has happened
				pDragInfo - points to drag information describing this drag. This
				should be an SGListDragInfo or something has gone seriously wrong.
				pMousePos - points to information on the current mouse position, in OIL coords
				pKeyPress - NULL, or if for a keypress event, keypress information

	Returns:	TRUE to claim the event, FALSE to let it through to other targets

	Purpose:	Event Handler for SuperGallery listitem drag events

********************************************************************************************/

BOOL SGListDragTarget::ProcessEvent(DragEventType Event, DragInformation *pDragInfo,
									OilCoord *pMousePos, KeyPress* pKeyPress)
{
	if (!pDragInfo->IsKindOf(CC_RUNTIME_CLASS(SGListDragInfo)))
		return(FALSE);

	SGListDragInfo *Info = (SGListDragInfo *)pDragInfo;

	switch(Event)
	{
		case DRAGEVENT_COMPLETED:
			HandleDragCompleted((SuperGallery *) TargetDialog,
								Info->GetDraggedNode(), pMousePos);
			return(TRUE);


		case DRAGEVENT_MOUSESTOPPED:
		case DRAGEVENT_MOUSEMOVED:
		case DRAGEVENT_MOUSEIDLE:
			// Call a subroutine to work out and set our current cursor shape
			return(DetermineCursorShape((SuperGallery *) TargetDialog,
										Info->GetDraggedNode(), pMousePos));
		default:
			break;
	}

	// Otherwise, we aren't interested in the event, so we don't claim it
	return(FALSE);
}



/********************************************************************************************

>	void SGListDragTarget::GetCursorID()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95

	Purpose:	Base Method to set cursor over this target

********************************************************************************************/

UINT32 SGListDragTarget::GetCursorID()
{
	return(CurrentCursorID);
}



/********************************************************************************************

>	virtual BOOL SGListDragTarget::GetStatusLineText(String_256 * TheText)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95

	Returns:	FALSE - we never provide any status line help for scroll-drags

	Purpose:	Provide status line text for this target

	Notes:		Bases the text on the current value of the member var CurrentCursorID.
				If this is an unknown value, defaults to _R(IDS_SGDRAG_LISTITEM)
   
********************************************************************************************/

BOOL SGListDragTarget::GetStatusLineText(String_256 * TheText)
{
	ERROR3IF(TheText == NULL, "Illegal NULL param");

	if (CurrentCursorID == _R(IDC_SGABOVECURSOR))
	{
		TheText->MakeMsg(_R(IDS_SGDRAG_INSABOVE));
	}
	else if (CurrentCursorID == _R(IDC_SGBELOWCURSOR))
	{
		TheText->MakeMsg(_R(IDS_SGDRAG_INSBELOW));
	}
	else
	{
		TheText->MakeMsg(_R(IDS_SGDRAG_LISTITEM));
	}

	return(TRUE);
}
















/********************************************************************************************

>	void SGListDragInfo::SGListDragInfo() 
	 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95		  

	Purpose:	Default constructor. DO NOT CALL THIS CONSTRUCTOR

********************************************************************************************/
SGListDragInfo::SGListDragInfo()
{
	ERROR3("Default SGListDragInfo constructor called");	
}



/********************************************************************************************

>	SGListDragInfo::SGListDragInfo(SuperGallery *ParentGal, SGDisplayNode *DraggedNode,
									SGMouseInfo *TheMouseInfo, BOOL IsAdjust = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95		  

	Inputs:		ParentGal - The gallery from which the drag originated

				DraggedNode - the SGDisplayNode that is being dragged

				TheMouseInfo -The MouseInfo which was passed into your HandleEvent
				method for the click which started this drag. (Passed back into the
				SGDisplayNode::DragWasReallyAClick handler if the drag becomes a click)

				IsAdjust - TRUE if it is an "adjust" drag [NOTE - Adjust drags are legal
				when dragging scrollbars, and are not turned into clicks like normal drags]				

	Purpose:	Constructor

********************************************************************************************/

SGListDragInfo::SGListDragInfo(SuperGallery *ParentGal, SGDisplayNode *DraggedNode,
								SGMouseInfo *TheMouseInfo, BOOL IsAdjust)
				 : DragInformation(IsAdjust)
{
	ParentGallery = ParentGal;
	DragNode = DraggedNode;
	MouseInfo = *TheMouseInfo;	// COPY the MouseInfo structure

	// Set up a few things about this drag - it does not do solid drag
	DoesSolidDrag = FALSE;
	SolidDragOffset.x = SolidDragOffset.y = 0;	// Set up defaults just in case
	SolidDragSize.Set(1,1);
}



/********************************************************************************************

>	virtual UINT32 SGListDragInfo::GetCursorID()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95

	Returns:	a cursor ID to set during this drag

	Purpose:	To provide the resource ID of the cursor to use during this drag.
				For scrollbar drags, the cursor should remain the normal arrow shape.
   
********************************************************************************************/

UINT32 SGListDragInfo::GetCursorID()
{
	 return(_R(IDC_DRAGGING_COLOUR));		// No-can-drop cursor shape
}



/********************************************************************************************

>	virtual BOOL SGListDragInfo::GetStatusLineText(String_256 * TheText)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95

	Returns:	FALSE - The string has not been filled in
	Purpose:	Provide status line text for this drag - While dragging scrollbars no help
				is required/given, so this always returns FALSE
   
********************************************************************************************/

BOOL SGListDragInfo::GetStatusLineText(String_256 * TheText)
{
	ERROR3IF(TheText == NULL, "Illegal NULL param");
	TheText->MakeMsg(_R(IDS_SGDRAG_LISTITEM));

	return(TRUE);
}



/********************************************************************************************

>	void SGListDragInfo::OnClick(INT32 Flags,POINT Point) 
	 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95		  
	Inputs:		Flags - Some flags
				Point - the point which was clicked

	Purpose:	This is called if a drag was attempted but never started because it was a 
				click all along. It passes the original Click information back to the
				SGDisplayNode which started the drag by calling its DragWasReallyAClick
				method

	SeeAlso:	SGDisplayNode::DragWasReallyAClick

********************************************************************************************/

void SGListDragInfo::OnClick(INT32 Flags, POINT Point)
{
	if (DragNode != NULL)
	{
		SGMiscInfo MiscInfo;
		ParentGallery->FillInMiscInfo(&MiscInfo);

		DragNode->DragWasReallyAClick(&MouseInfo, &MiscInfo);
	}
}


