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

// SGTree.cpp - super gallery DisplayTree classes


#include "camtypes.h"

#include "document.h"	// For Document->GetTitle()
#include "dragmgr.h"	// Drag manager (DragManagerOp::StartDrag, RedrawStarting etc)
//#include "galres.h"		// Gallery bitmap resources
//#include "galstr.h"		// Gallery string resources
//#include "scroller.h"	// For scroll bar width
#include "sgallery.h"	// SuperGallery definitions
#include "sgdrag.h"		// Scroll bar drag target/info
#include "sgtree.h"		// This file's associated header
#include "sglib.h"		// For virtualising static switch
//#include "sglcart.h"

#include "ccdc.h"		// For render-into-dialogue support
//#include "sglfills.h"
#include "dlgcol.h"
#include "fillval.h"
//#include "grnddib.h"

//#include "richard3.h"	// For _R(IDS_GALLERY_PREPARE_FOR_UNFOLD)
#include "progress.h"
// Webster stuff
//#include "webster.h"
//#include "inetop.h"

// Implement the dynamic class bits...
CC_IMPLEMENT_DYNAMIC(SGDisplayNode,  CCObject)
	CC_IMPLEMENT_DYNAMIC(SGDisplayRoot,  SGDisplayNode)
		CC_IMPLEMENT_DYNAMIC(SGDisplayRootScroll,  SGDisplayRoot)
	CC_IMPLEMENT_DYNAMIC(SGDisplayGroup, SGDisplayNode)
	CC_IMPLEMENT_DYNAMIC(SGDisplayItem,  SGDisplayNode)


// This line mustn't go before any CC_IMPLEMENT_... macros
#define new CAM_DEBUG_NEW


static INT32 ScrollBarWidth		= 10;		// Width (PIXELS) of an SGDisplayRootScroll
											// provided gallery list scrollbar. MUST be even!


SGDisplayNode *SGDisplayNode::CurrentBGRenderNode = NULL;
BOOL SGDisplayNode::BGRenderClaimed= FALSE;
BOOL SGDisplayNode::BkgEraseMode = TRUE;


/***********************************************************************************************

>	SGDisplayNode::SGDisplayNode()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Purpose:	SGDisplayNode constructor
	SeeAlso:	SuperGallery; SGDisplayGroup; SGDisplayItem

***********************************************************************************************/

SGDisplayNode::SGDisplayNode()
{
	Parent = Next = Previous = NULL;

	Flags.Invisible = Flags.ReadOnly = Flags.Modified = FALSE;
	Flags.CanSelect = Flags.Selected = FALSE;
	Flags.Folded = Flags.RedrawPending = FALSE;
	Flags.Virtualised = FALSE;

	Flags.HandleEventCount = 0;

	Flags.Reserved = 0;

	FormatRect = DocRect(0,0,0,0);
}



/***********************************************************************************************

>	SGDisplayNode::~SGDisplayNode()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Purpose:	SGDisplayNode destructor
	SeeAlso:	SuperGallery; SGDisplayGroup; SGDisplayItem

***********************************************************************************************/

SGDisplayNode::~SGDisplayNode()
{
	ERROR3IF(Flags.HandleEventCount > 0, "AWOOGA! SGDisplayNode deleted while in its own HandleEvent method - Alert Jason!");
	ERROR3IF(Flags.HandleEventCount != 0, "Deleted SGDisplayNode had a corrupted HandleEventCount");

	if (Parent != NULL || GetChild() != NULL || Next != NULL || Previous != NULL)
	{
		ERROR3("Destructing SGDisplayNode which is still linked into a tree! I'll try to delink it first\n");
		RemoveFromTree();
	}
}



/***********************************************************************************************

>	virtual SGDisplayNode *SGDisplayNode::GetChild(void) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94 (Made virtual on 13/5/95)

	Returns:	A pointer to the first child of this SGDisplayNode object, or NULL

	Purpose:	Finds the child of this DisplayTree Node.
				Returns NULL if you have reached the boundary of the tree

	Notes:		This base-class method returns NULL - base nodes and items do not have
				children (to reduce memory usage). SGDisplayRoot and SGDisplayGroup override
				this method to provide child pointers.

	SeeAlso:	SGDisplayNode::SetChild; SuperGallery; SGDisplayNode::GetParent;
				SGDisplayNode::GetNext; SGDisplayNode::GetPrevious

***********************************************************************************************/

SGDisplayNode *SGDisplayNode::GetChild(void) const
{
	return NULL;
}



/***********************************************************************************************

>	void SGDisplayNode::SetChild(SGDisplayNode *NewChild)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94

	Inputs:		A pointer to the new first-child of this SGDisplayNode object

	Purpose:	Sets the child of this DisplayTree Node.

	Notes:		This base-class method gives an ERROR3 - base nodes and items do not have
				children (to reduce memory usage). SGDisplayRoot and SGDisplayGroup override
				this method to provide child pointers.

	SeeAlso:	SuperGallery; SGDisplayNode::GetParent;
				SGDisplayNode::GetNext; SGDisplayNode::GetPrevious

***********************************************************************************************/

void SGDisplayNode::SetChild(SGDisplayNode *NewChild)
{
	ERROR3("This is a childless SGDisplayNode - You cannot set it's child!");
}



/***********************************************************************************************

>	virtual void SGDisplayNode::InsertInternal(SGDisplayNode *NodeToInsert,
												SGDisplayNode *PrevNode, SGDisplayNode *NextNode)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94

	Inputs:		NodeToInsert - the node to insert
				PrevNode - NULL, or the node to insert after
				NextNode  - NULL, or the node to insert before

	Purpose:	Inserts the given node/subtree into this subtree, between PrevNode and
				NextNode. One of these two nodes may be NULL if you are trying to insert
				at the head/tail of a sibling list.
				
	Errors:		ERROR3s will be reported in debug builds for NULL NodeToInsert, or if
				NodeToInsert is linked into another tree (has non-null parent/next/previous
				pointers). It is perfectly legal for it to have a child subtree, though.

				Errors will also occur if BOTH Next/PrevNode are NULL, or if PrevNode is
				not currently the Previous Node of NextNode. (i.e. you must insert between
				two valid adjacent nodes, or at the head/tail of the sibling list)

	SeeAlso:	SuperGallery; SGDisplayNode::AddItem;
				SGDisplayNode::InsertAfter; SGDisplayNode::InsertBefore

***********************************************************************************************/

void SGDisplayNode::InsertInternal(SGDisplayNode *NodeToInsert,
									SGDisplayNode *PrevNode, SGDisplayNode *NextNode)
{
	if (NodeToInsert == NULL)
	{
		ERROR3("Attempt to insert a NULL node into a tree was ignored");
		return;
	}

	// The node cannot be inserted after/before *itself*!
	ERROR3IF(NodeToInsert == PrevNode || NodeToInsert == NextNode,
			"Illegal attempt to link a node before/after ITSELF!");

	// The node cannot have parent, next, prev, but can have children
	ERROR3IF(NodeToInsert->Parent != NULL ||
			 NodeToInsert->Next != NULL   || NodeToInsert->Previous != NULL,
			 "Illegal attempt to link an already-linked node into a tree");

	ERROR3IF(PrevNode == NULL && NextNode == NULL,
			 "SGDisplayNode::InsertInternal - can't insert between TWO NULL nodes!");

	ERROR3IF((PrevNode != NULL && PrevNode->Next != NextNode) ||
			 (NextNode != NULL && NextNode->Previous != PrevNode),
			 "SGDisplayNode::InsertInternal - Prev/Next nodes are not adjacent!");

	NodeToInsert->Previous = PrevNode;
	if (PrevNode != NULL)
	{
		// Linking in the middle of the sibling list
		PrevNode->Next = NodeToInsert;
		NodeToInsert->Parent = PrevNode->Parent;
	}
	else
	{
		// Must be trying to insert at the head of the sibling list, so becomes 1st child
		if (NextNode != NULL)
		{
			ERROR3IF(NextNode->Parent->GetChild() != NextNode,
						"Attempt to Insert node as first child has gone awry!");
		
			NextNode->Parent->SetChild(NodeToInsert);
		}
	}

	NodeToInsert->Next = NextNode;
	if (NextNode != NULL)
	{
		NextNode->Previous = NodeToInsert;
		NodeToInsert->Parent = NextNode->Parent;
	}

	// Because we have recreated part of the tree, we must inform the gallery that the
	// cached format is incorrect and needs to be recalculated.
	SuperGallery *ParentGallery = GetParentGallery();
	if (ParentGallery != NULL)
		ParentGallery->InvalidateCachedFormat();
}



/***********************************************************************************************

>	virtual void SGDisplayNode::AddItem(SGDisplayNode *NodeToInsert,
										SGSortKey *SortInfo = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94

	Inputs:		NodeToInsert - the node/subtree to be inserted
				SortInfo - NULL, or an array of MaxSGSortKeys sort key structures which
				describe how the item should be inserted. [NOTE that this parameter is 
				only used for insertion of SGDisplayItem and derived classes]

	Purpose:	Inserts the given node/subtree into this subtree. If SortInfo == NULL or
				NodeToInsert is not an SGDisplayItem, it is added as the last child of
				this node. Otherwise, it is inserted into the subtree of SGDisplayItems
				at the point 'specified' by SortInfo (By asking each DisplayItem in turn
				to compare itself to the one being added, until one is found which is
				considered "greater than" this one according to the sort mode)

	Errors:		ERROR3s will be reported in debug builds for NULL NodeToInsert, or if
				NodeToInsert is linked into another tree (has non-null parent/next/previous
				pointers). It is perfectly legal for it to have a child subtree, though.

	SeeAlso:	SuperGallery; SGDisplayNode::InsertAfter; SGDisplayNode::InsertBefore

***********************************************************************************************/

void SGDisplayNode::AddItem(SGDisplayNode *NodeToInsert, SGSortKey *SortInfo)
{
	if (NodeToInsert == NULL)
	{
		ERROR3("Attempt to add a NULL node to a tree was ignored");
		return;
	}

	// The node cannot have parent, next, prev, but can have children
	ERROR3IF(NodeToInsert->Parent != NULL ||
			 NodeToInsert->Next != NULL   || NodeToInsert->Previous != NULL,
			 "Illegal attempt to link an already-linked node into a tree");

	if (GetChild() == NULL)	// We have no children, so there is only one place for this node!
	{
		SetChild(NodeToInsert);						// Add it as our first child and return
		NodeToInsert->Parent = this;

		// Because we have recreated part of the tree, we must inform the gallery that the
		// cached format is incorrect and needs to be recalculated.
		SuperGallery *ParentGallery = GetParentGallery();
		if (ParentGallery != NULL)
			ParentGallery->InvalidateCachedFormat();
		return;
	}

	SGDisplayNode *Ptr  = GetChild();
	SGDisplayNode *Last = NULL;

	if (SortInfo == NULL || SortInfo[0].SortKey == 0 ||
			!NodeToInsert->IsKindOf(CC_RUNTIME_CLASS(SGDisplayItem)) ||
			!IsKindOf(CC_RUNTIME_CLASS(SGDisplayGroup)))
	{
		// There is no requested sort mode, or it is sort-by-none, or the node being inserted
		// is not a DisplayItem, or I am not a DisplayGroup, so I just add the item to the end
		// of my child list

		while (Ptr != NULL)		// Find the end of the sibling list
		{
			Last = Ptr;
			Ptr  = Ptr->Next;
		}

		// This ENSURE should never occur, as we checked for no-children above
		ERROR3IF(Last == NULL, "Something screwy has happened in SGDisplayNode:AddItem!");
		InsertInternal(NodeToInsert, Last, NULL);	// Insert it as the last child
	}
	else
	{
		// While searching givvus a parent! (bodge so libraries can sort whilst adding)
		NodeToInsert->Parent = this;

		// We can add with sorting, so add the item at an appropriate position based upon
		// the provided sort mode
		INT32 Result;
		while (Ptr != NULL)		// Search the sibling list for the first appropriate insertion point
		{
			Last = Ptr;

			// Compare using sort key 1
			Result = Ptr->CompareTo(NodeToInsert, SortInfo[0].SortKey);
			if (SortInfo[0].Reversed)
				Result = -Result;

			// If they are equal, and we have multi-key sort, use key 2
			if (Result == 0 && SortInfo[1].SortKey != 0)
			{
				Result = Ptr->CompareTo(NodeToInsert, SortInfo[1].SortKey);
				if (SortInfo[1].Reversed)
					Result = -Result;
			}
		
			// We have compared - if the current item is "greater" than the one being
			// inserted, then we can stop and insert it before that item
			if (Result > 0)
				break;

			Ptr = Ptr->Next;
		}

		// Delink the parent since it shouldn't really be connected yet
		NodeToInsert->Parent = NULL;
		
		if (Ptr == NULL)
		{
			// We must have run off the end of the list - insert at the end
			ERROR3IF(Last == NULL, "Something screwy has happened in SGDisplayNode:AddItem!");
			InsertInternal(NodeToInsert, Last, NULL);
		}
		else
		{
			// We found an item to insert before, so insert before it
			InsertInternal(NodeToInsert, Ptr->GetPrevious(), Ptr);
		}
	}
}



/***********************************************************************************************

>	virtual void SGDisplayNode::InsertAfter(SGDisplayNode *NodeToInsert)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94

	Inputs:		NodeToInsert - the node to, ... erm... insert.

	Purpose:	Inserts the given node into the DisplayTree as the next (right) sibling
				of this node.

	Notes:		The derived SGDisplayRoot node overrides this with a call to AddItem()

	Errors:		ERROR3 and quiet exit if NodeToInsert == NULL

	SeeAlso:	SuperGallery; SGDisplayNode::InsertBefore; SGDisplayNode::AddItem

***********************************************************************************************/

void SGDisplayNode::InsertAfter(SGDisplayNode *NodeToInsert)
{
	// Set the parent to modified to signify that one of it's children has been
	SGDisplayNode *Parent = GetParent();
	if(Parent != NULL)
		Parent->Flags.Modified = TRUE;

	InsertInternal(NodeToInsert, this, Next);
}



/***********************************************************************************************

>	virtual void SGDisplayNode::InsertBefore(SGDisplayNode *NodeToInsert)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94

	Inputs:		NodeToInsert - the node to, ... erm... insert.

	Purpose:	Inserts the given node into the DisplayTree as the previous (left) sibling
				of this node.

	Notes:		The derived SGDisplayRoot node overrides this with a call to AddItem()

	Errors:		ERROR3 and quiet exit if NodeToInsert == NULL

	SeeAlso:	SuperGallery; SGDisplayNode::InsertAfter; SGDisplayNode::AddItem

***********************************************************************************************/

void SGDisplayNode::InsertBefore(SGDisplayNode *NodeToInsert)
{
	// Set the parent to modified to signify that one of it's children has been
	SGDisplayNode *Parent = GetParent();
	if(Parent != NULL)
		Parent->Flags.Modified = TRUE;

	InsertInternal(NodeToInsert, Previous, this);
}



/***********************************************************************************************

>	virtual void SGDisplayNode::MoveAfter(SGDisplayNode *NodeToMove)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95

	Inputs:		NodeToMove - the node to move

	Purpose:	MOVES the given node (to a different position in the DisplayTree) as the
				previous (left) sibling of this node. If the node is not linked into
				a tree, it is effectively just inserted.

	Notes:		This base class method simply delinks the item and relinks it elsewhere
				in the display tree. However, derived classes will override this method
				so that moving display items can have a further effect of also rearranging
				the displayed "real" items. Before/After moving the real item, the
				derived class can then call this baseclass method to complete the action.
		
				Take care when moving items between groups (e.g. if an item is "moved"
				from one docuemnt to another, it could be a bad thing, so be very
				careful in derived classes to take appropriate action)

				Any attempt to move an item after *itself* is queitly ignored

	Errors:		ERROR3 and quiet exit if NodeToMove == NULL

	SeeAlso:	SuperGallery; SGDisplayNode::InsertAfter; SGDisplayNode::AddItem

***********************************************************************************************/

void SGDisplayNode::MoveAfter(SGDisplayNode *NodeToMove)
{
	ERROR3IF(NodeToMove == NULL, "Illegal NULL param");

	if (NodeToMove == this)
		return;

	NodeToMove->RemoveFromTree();
	InsertAfter(NodeToMove);
}



/***********************************************************************************************

>	virtual void SGDisplayNode::MoveBefore(SGDisplayNode *NodeToMove)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95

	Inputs:		NodeToMove - the node to move

	Purpose:	MOVES the given node (to a different position in the DisplayTree) as the
				previous (left) sibling of this node. If the node is not linked into
				a tree, it is effectively just inserted.

	Notes:		This base class method simply delinks the item and relinks it elsewhere
				in the display tree. However, derived classes will override this method
				so that moving display items can have a further effect of also rearranging
				the displayed "real" items. Before/After moving the real item, the
				derived class can then call this baseclass method to complete the action.
		
				Take care when moving items between groups (e.g. if an item is "moved"
				from one docuemnt to another, it could be a bad thing, so be very
				careful in derived classes to take appropriate action)

				Any attempt to move an item before *itself* is queitly ignored

	Errors:		ERROR3 and quiet exit if NodeToMove == NULL

	SeeAlso:	SuperGallery; SGDisplayNode::InsertBefore; SGDisplayNode::AddItem

***********************************************************************************************/

void SGDisplayNode::MoveBefore(SGDisplayNode *NodeToMove)
{
	ERROR3IF(NodeToMove == NULL, "Illegal NULL param");

	if (NodeToMove == this)
		return;

	NodeToMove->RemoveFromTree();
	InsertBefore(NodeToMove);
}



/***********************************************************************************************

>	virtual void SGDisplayNode::RemoveFromTree(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94

	Purpose:	De-links this node/subtree from the DisplayTree. This DOES NOT DELETE the
				node, just unlinks it in preparation for being deleted.

	Notes:		This does NOT delink children of this node from this node. To do that, you
				must call RemoveFromTree from each child node in turn. To delete a subtree
				you are better off calling DestroySubtree

	Errors:		ERROR3s will be reported in debug builds if certain corrupted tree
				structures are detected, indicating tree generation/maintenance code 
				has gone wrong

	SeeAlso:	SuperGallery; SGDisplayNode::InsertAfter;
				SGDisplayNode::InsertBefore; SGDisplayNode::AddItem;
				SGDisplayNode::DestroySubtree

***********************************************************************************************/

void SGDisplayNode::RemoveFromTree(void)
{
	// Because we have changed part of the tree, we must inform the gallery that the
	// cached format is incorrect and needs to be recalculated. We must also make sure
	// that this item is not pending a background redraw.
	SuperGallery *ParentGallery = GetParentGallery();
	if (ParentGallery != NULL)
	{
		// Ensure that the ParentGallery doesn't dereference this pointer on the next background
		// rendering pass (if we happen to be removed just after redrawing in the BG)
		if (this == ParentGallery->GetLastBackgroundNode())
			ParentGallery->SetLastBackgroundNode(NULL);

		if (Flags.RedrawPending)
			ParentGallery->DecrementPendingRedraws();

		ParentGallery->InvalidateCachedFormat();
	}

	Flags.RedrawPending = FALSE;		// We are NOT pending a redraw!

	if (Next == NULL)
	{
		// If the next sibling ptr is NULL, then we are at the end of a group or the list
		// so we 'touch' the previous node (or parent) FormatRect so that the next reformat
		// realises that a change has occurred around here and redraws where we were.
		SGDisplayNode *ToTouch = Previous;
		if (ToTouch == NULL)
			ToTouch = Parent;

		if (ToTouch != NULL)
			ToTouch->FormatRect.MakeEmpty();
	}

	// Delink our Parent from ourself
	if (Parent != NULL)
	{
		if (Parent->GetChild() == this)		// If we are first child, make Next child the first
		{
			// I must be the first child, so should not have a Previous node
			ERROR3IF(Previous != NULL, "Tree linking failure detected in SGDisplayNode::RemoveFromTree");
			Parent->SetChild(Next);
		}
		else
		{
			// I am not the parent's first child, so I must have a Previous node
			ERROR3IF(Previous == NULL, "Tree linking failure detected in SGDisplayNode::RemoveFromTree");
		}
	}


	// Delink our Previous/Next siblings (if any) from ourself
	if (Previous != NULL)
		Previous->Next = Next;

	if (Next != NULL)
		Next->Previous = Previous;


	// And finally, destroy our own backward links to parent, next, previous.
	// Done last to avoid 'losing' pointers before we have finished using them for delinking!
	Parent	 = NULL;
	Previous = NULL;
	Next	 = NULL;
}



/***********************************************************************************************

>	virtual void SGDisplayNode::DestroySubtree(BOOL IncludingThisNode = TRUE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/94

	Inputs:		IncludingThisNode - TRUE (The default) to delete this node (the root of
				the subtree) as well as all its children.
				FALSE to delete its children only (This leaves this node untouched, but
				vapes all child nodes)

	Purpose:	DESTROYS the subtree starting at (and including, if IncludingThisNode is
				TRUE) this node.
				This does a depth-first recursive scan of the subtree, delinking each item,
				and then CALLING EACH ITEMS DESTRUCTOR.

	Notes:		If you destroy at the root node, the entire tree is destroyed. The root node
				will be deleted, but note that the reference(s) to the root node (e.g. in
				the parent SuperGallery) will NOT be de-linked, so be *very* careful!

				However, if the root node is a derived SGDisplayRoot node, it will refuse
				to delete itself in this case, 

	Errors:		May be generated by the RemoveFromTree and destructor calls if the subtree
				is in some way corrupt - see these calls for details.

				An ERROR3 may be caused by the destructor if you are trying to delete
				a tree item from within that item's event handler!

	SeeAlso:	SuperGallery; SGDisplayNode::RemoveFromTree; SGDisplayNode::~SGDisplayNode

***********************************************************************************************/

void SGDisplayNode::DestroySubtree(BOOL IncludingThisNode)
{
	while (GetChild() != NULL)			// Recurse depth-first down the subtree, destroying it
		GetChild()->DestroySubtree();	// Destroy child. Child now points at the next child

	if (IncludingThisNode)			// If this node is included in the destruction...
	{
		RemoveFromTree();			// Delink ourself from the tree
		delete this;				// and invoke our own destructor
	}
}



/***********************************************************************************************

>	virtual SuperGallery *SGDisplayNode::GetParentGallery(void) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95

	Returns:	NULL if a cataclysmic event occurs
				Otherwise,a pointer to the SuperGallery that 'owns' the tree this node is in

	Purpose:	Recursively scans up the tree asking each parent node in turn for the parent
				gallery. It is expected that a node (eg SGDisplayRoot or SGDisplayGroup)
				will be found somewhere on this path which will know who our parent gallery
				is. (If not, there is a serious tree problem!)

				Nodes which directly know their parent gallery must override this method
				to ensure that they return the correct result instead of asking their parent!

	SeeAlso:	SGDisplayRoot::GetParentGallery; SGDisplayGroup::GetParentGallery

***********************************************************************************************/

SuperGallery *SGDisplayNode::GetParentGallery() const
{
	if (GetParent() != NULL) return GetParent()->GetParentGallery();

	return NULL;
}



/***********************************************************************************************

>	virtual BOOL SGDisplayNode::GiveEventToMyChildren(SGEventType EventType,
														void *EventInfo,
														SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94

	Inputs:		EventType - An enumerated value describing what type of event is to be processed

				EventInfo - A structure describing the event (may be NULL). The exact thing
							pointed at by this pointer depends upon the event type:

							MonoOn
							Event				Thing EventInfo points at
							SGEVENT_FORMAT		(SGFormatInfo *)
							SGEVENT_REDRAW		(SGRedrawInfo *)
							SGEVENT_MOUSECLICK	(SGMouseInfo *)
							MonoOff
							
							Use the SGDisplayNode::Get[Format]Info() inlines to retrieve
							this information for you (it does helpful ERROR3 checking for you)

				MiscInfo - A structure containing any other relevant information.
				This will always be non-NULL, and contain valid information.

	Outputs:	FormatInfo is updated as appropriate

	Returns:	TRUE if the event was handled successfully
				FALSE if it was not

	Purpose:	Causes the entire subtree below this node to check their formatting, and
				handle the given event. Once a node returns TRUE from its HandleEvent method,
				the event will NOT be passed on.

	Notes:		This is used by derived classes to save them the work of
				having to scan the tree in their own event code - each node just
				redraws itself, and then passes the event along by calling this
				function.

				If this node is folded, then it is treated as having no children (as they
				do not appear in the displayed list.

				The traversal loops in this code can handle any nodes being deleted around
				them, except for the "current node". However, in debug builds the "current
				node" should refuse to be deleted while it is handling an event.
				The traversal loops increment the "HandleEventCount" during calls to all the
				node HandleEvent methods in order to be able to detect this situation (It's
				done here to save doing it in all derived HandleEvent methods).

	SeeAlso:	SGDisplayNode::HandleEvent; SGDisplayRoot::HandleEvent;
				SGDisplayGroup::HandleEvent; SGDisplayItem::HandleEvent

***********************************************************************************************/

BOOL SGDisplayNode::GiveEventToMyChildren(SGEventType EventType, void *EventInfo,
											 SGMiscInfo *MiscInfo)
{
	BOOL Handled = FALSE;
	SGDisplayNode *Ptr = GetChild();
//	SGDisplayNode *NextPtr = NULL;

	if (EventType == SGEVENT_FORMAT)
	{
		// We're formatting, so update FormatInfo as we go
		SGFormatInfo *FormatInfo = GetFormatInfo(EventType, EventInfo);

		NewLine(FormatInfo, MiscInfo);		// When going down the tree, go to a new line

		if (Flags.Folded || Ptr == NULL)	// If folded, or no children, return
			return(FALSE);

		if (!Flags.Invisible)
			FormatInfo->IndentLevel++;		// Increment the indent level

		while (Ptr != NULL && !Handled)
		{
			// Call each child's handler in turn. We can cope with any child being deleted except
			// for the current one. To detect this, we increment HandleEventCOunt across the
			// call, which will trigger ERROR3's in the destructor if we attempt suicide!
			Ptr->Flags.HandleEventCount++;
			ERROR3IF(Ptr->Flags.HandleEventCount > 100,
						"Rampant recursion or node corruption in GiveEventToMyChildren");
			if (Ptr->HandleEvent(EventType, EventInfo, MiscInfo))
				Handled = TRUE;
			Ptr->Flags.HandleEventCount--;

			Ptr = Ptr->Next;
		}

		if (!Flags.Invisible)
			FormatInfo->IndentLevel--;		// Restore the indent level

		NewLine(FormatInfo, MiscInfo);		// When ascending back up the tree, go to a new line

		FormatInfo->LineHeight = UpTreeGap;
		NewLine(FormatInfo, MiscInfo);		// ... and add a small gap
	}
	else
	{
		if (Ptr == NULL)		// We have no children, so return
			return(FALSE);

		if (EventType != SGEVENT_BGFLUSH && Flags.Folded)
		{
			// If folded, return (but not if this is a FLUSH, which MUST go to ALL)
			return(FALSE);
		}

		while (Ptr != NULL && !Handled)
		{
			// Call each child's handler in turn. We can cope with any child being deleted except
			// for the current one. To detect this, we increment HandleEventCOunt across the
			// call, which will trigger ERROR3's in the destructor if we attempt suicide!
			Ptr->Flags.HandleEventCount++;
			ERROR3IF(Ptr->Flags.HandleEventCount > 100,
						"Rampant recursion or node corruption in GiveEventToMyChildren");
			if (Ptr->HandleEvent(EventType, EventInfo, MiscInfo))
				Handled = TRUE;
			Ptr->Flags.HandleEventCount--;

			Ptr = Ptr->Next;
		}
	}

	return(Handled);
}



/***********************************************************************************************

>	virtual void SGDisplayNode::NewLine(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/10/94

	Inputs:		FormatInfo - A structure containing all relevant information for items to
				calculate their formatted positions in the display list

	Outputs:	FormatInfo is updated as appropriate

	Purpose:	Resets the formatting info structure to default values for the start of
				the next 'line'.

	SeeAlso:	SGDisplayNode::HandleEvent; SGDisplayRoot::HandleEvent;
				SGDisplayGroup::HandleEvent; SGDisplayItem::HandleEvent

***********************************************************************************************/

void SGDisplayNode::NewLine(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo)
{
	if (FormatInfo->LineHeight > 0)
	{
		FormatInfo->LineHeight = GridLock(MiscInfo, FormatInfo->LineHeight) +
									GridLock(MiscInfo, InterLineGap);
		FormatInfo->LinePos -= FormatInfo->LineHeight;
	}
	
	FormatInfo->LineHeight = 0;
	FormatInfo->AvailableWidth = MiscInfo->MaxWidth - (FormatInfo->IndentLevel * IndentWidth);
	FormatInfo->AvailableWidth = GridLock(MiscInfo, FormatInfo->AvailableWidth);
}



/***********************************************************************************************

>	void SGDisplayNode::GridLockRect(SGMiscInfo *MiscInfo, DocRect *Rect)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/94

	Inputs:		FormatInfo - A structure containing all relevant information for items to
				calculate their formatted positions in the display list. NOTE this is also
				an output!

				Rect - A rectangle which needs to be locked to the pixel grid

	Outputs:	FormatInfo	- updated as appropriate
				Rect		- modified as necessary to lock its points to the pixel grid

	Purpose:	Given a rectangle and the normal FormatInfo, this ensures that all points
				of the rectangle are snapped onto a grid of the destination device pixels.
				This ensures that aliasing effects, due to rounding errors when mapping
				to the output pixel coordinates, do not occur.

	SeeAlso:	SGDisplayNode::GridLock; SGDisplayNode::DevicePixels

***********************************************************************************************/

void SGDisplayNode::GridLockRect(SGMiscInfo *MiscInfo, DocRect *Rect)
{
	Rect->lo.x = GridLock(MiscInfo, Rect->lo.x);
	Rect->lo.y = GridLock(MiscInfo, Rect->lo.y);
	Rect->hi.x = GridLock(MiscInfo, Rect->hi.x);
	Rect->hi.y = GridLock(MiscInfo, Rect->hi.y);
}



/***********************************************************************************************

>	virtual void SGDisplayNode::CalculateFormatRect(SGFormatInfo *FormatInfo,
													SGMiscInfo *MiscInfo,
													INT32 ItemWidth, INT32 ItemHeight)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/10/94

	Inputs:		FormatInfo - A structure containing all relevant information for items to
				calculate their formatted positions in the display list. NOTE this is also
				an output! (As passed into HandleEvent for SGEVENT_FORMAT events)

				MiscInfo - Miscellaneous information needed for formatting; as passed
				into all HandleEvent calls

				ItemWidth - The width of this item in millipoints, or 0 if this item
				is 'infinite' width (fills the entire line). If there is not enough space left
				on this line for the item, a new line is started.

				ItemHeight - The height of this item in millipoints. Sibling items are
				currently expected to have equal heights; although line formatting will cope
				with different heights, redraw may miss strips below items for the time
				being.

	Outputs:	FormatInfo	- updated as appropriate
				Member variable FormatRect now contains the format rectangle

	Returns:	TRUE if the resulting rectangle overlaps RedrawInfo->Bounds (i.e. if the
				node would need to redraw itself if handling a redraw request)
				else FALSE if the node need not be redrawn

	Purpose:	Given current formatting information, generates the rectangle within which
				this node and its subtree should be redrawn.

	Notes:		This relies upon the cached formatting information in this node being
				correct - this function can only be called once per node during a pass
				through the tree... the second call would give a different result.

				If you do not use this function to do all the formatting work for you, then
				you must remember to either updaet the 'FormatRect' member variable, or
				override the 'GetFormatRect' member function to supply this information
				to the caller properly.

	SeeAlso:	SGDisplayNode::HandleEvent; SGDisplayRoot::HandleEvent;
				SGDisplayGroup::HandleEvent; SGDisplayItem::HandleEvent

***********************************************************************************************/

void SGDisplayNode::CalculateFormatRect(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo,
										INT32 ItemWidth, INT32 ItemHeight)
{
	ERROR3IF(FormatInfo == NULL || MiscInfo == NULL,
			 "NULL parameter(s) passed to SGDisplayNode::CalculateFormatRect");

	if (Previous == NULL						|| 	// Is first child node, so go to a new line
		FormatInfo->AvailableWidth <= 0			|| 	// No room left at all on this line
		FormatInfo->AvailableWidth < ItemWidth	||	// Not enough room on the line for this item
		ItemWidth == 0)								// This item is infinitely wide
	{
			NewLine(FormatInfo, MiscInfo);
	}

	// If items do not fill the entire width, add a 2-pixel gap at the right edge, so that
	// adjacent items have a small gap between them. (see below)
	if (ItemWidth != 0)
		ItemWidth += MiscInfo->PixelSize * 2;

	ItemWidth  = GridLock(MiscInfo, ItemWidth);
	ItemHeight = GridLock(MiscInfo, ItemHeight);

	// We must now have enough room for this item across the current line, so plonk it in!
	// Update the height of the current line
	if (FormatInfo->LineHeight < ItemHeight)
	{
		// if (LineHeight != 0) AWOOGA! Line height has increased! Must go back and fill in
		// the background colour below the previous items on this line, to ensure the entire
		// line is fully redrawn.

		ERROR3IF(FormatInfo->LineHeight != 0,
			"Sibling Display Item heights are not equal! Jason must upgrade the redraw code");

		FormatInfo->LineHeight = ItemHeight;
	}

	// Generate the position rectangle. Note that this may be wider than MaxWidth - it is the
	// actual rectangle the item has (so scaling into the returned rect will always work,
	// even if part of the rectangle is clipped out of view). However, if the item was
	// requested as infinite width, it is returned as MaxWidth.
	INT32 ActualWidth = (ItemWidth == 0) ? FormatInfo->AvailableWidth : ItemWidth;

	// Ensure it is clipped within the available window space
	if (ActualWidth > FormatInfo->AvailableWidth)
		ActualWidth = FormatInfo->AvailableWidth;
	
	DocRect OldFormatRect(FormatRect);
	FormatRect.lo.x = MiscInfo->MaxWidth - FormatInfo->AvailableWidth;
	FormatRect.hi.x = FormatRect.lo.x + ActualWidth;
	FormatRect.lo.y = FormatInfo->LinePos - FormatInfo->LineHeight;
	FormatRect.hi.y = FormatInfo->LinePos;

	// If items do not fill the entire width, add a 2-pixel gap at the right edge, so that
	// adjacent items have a small gap between them. (see above)
	if (ActualWidth < FormatInfo->AvailableWidth)
		FormatRect.hi.x -= MiscInfo->PixelSize * 2;

	GridLockRect(MiscInfo, &FormatRect);	// And ensure it is locked onto the grid

	// If this node is no longer in the same position as it was last time we formatted,
	// then accumulate its Y bounds into the 'InvalidBounds' rect, to allow the gallery
	// to redraw only those regions of the list which are invalid
	if (FormatInfo->AccumulateBounds)
	{
		if (OldFormatRect != FormatRect)
		{
			FormatInfo->LastInvalidNode = this;		// We are the last node found to have invalid bounds

			if (FormatInfo->InvalidBounds.hi.y > 0)	// If we haven't found an invalid item before
				FormatInfo->InvalidBounds.hi.y = FormatRect.hi.y;

			if (FormatInfo->InvalidBounds.lo.y > FormatRect.lo.y)
				FormatInfo->InvalidBounds.lo.y = FormatRect.lo.y;
		}
		else
		{
			if (FormatInfo->LastInvalidNode != NULL)
			{
				// The immediately previous node had invalid bounds. If we lie below the current
				// invalid bounds, then we extend them to touch the top of us, to include any gap
				// between them and us.

				ERROR3IF(FormatInfo->InvalidBounds.hi.y > 0,
							"Gallery display formatting error - LastInvalidNode should be NULL "
							"if there haven't been any invalid nodes yet");

				if (FormatInfo->InvalidBounds.lo.y > FormatRect.hi.y)
					FormatInfo->InvalidBounds.lo.y = FormatRect.hi.y;

				FormatInfo->LastInvalidNode = NULL;		// Reset LastInvalid node so that the next node doesn't extend!
			}
		}
	}

//	if (Flags.Invisible)
//	{
//		FormatRect.hi.x = FormatRect.lo.x;
//		FormatRect.hi.y = FormatRect.lo.y;
//	}

	// Update the free width on the end of this line. Note that this may now be 0 or -ve,
	// but this will be sorted out on the next call to this method, in the clauses above.
//	if (!Flags.Invisible)
//	{
		FormatInfo->AvailableWidth -= ActualWidth;
		GridLock(MiscInfo, FormatInfo->AvailableWidth);
//	}
}



/***********************************************************************************************

>	virtual BOOL SGDisplayNode::SetFoldedState(BOOL NewState, BOOL ForceRedraw = TRUE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/95

	Inputs:		NewState - TRUE to fold, FALSE to unfold

	Returns:	TRUE if the new state is different from the old state (if anything has changed)

	Purpose:	Folds or unfolds a display node

	Notes:		This base-class implementation gives an ERROR3 - use folding only on groups

***********************************************************************************************/

BOOL SGDisplayNode::SetFoldedState(BOOL NewState, BOOL ForceRedraw)
{
	ERROR3("Folding can only be applied to groups!");
	return(FALSE);
}



/***********************************************************************************************

>	void SGDisplayNode::SetSelected(BOOL IsSelected = TRUE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/94

	Inputs:		IsSelected - TRUE to select, FALSE to deselect this item

	Purpose:	Sets the selection state of a SuperGallery Display Item tree node.
				Note that this does not cause a redraw of the gallery list box or anything.
				After setting the state(s) of item(s) you must therefore redraw them.

	Notes:		An ENSURE will be generated by any node which cannot be selected.
				(Derived classes wishing to allow selection state changes should set
				Flags.CanSelect)

	SeeAlso:	SGDisplayItem::SetSelected; SGDisplayNode::IsSelected

***********************************************************************************************/

void SGDisplayNode::SetSelected(BOOL IsSelected)
{
	ERROR3IF(!Flags.CanSelect, "Base class SGDisplayNode::SetSelected called! I'm not a selectable ITEM!");

	if (Flags.CanSelect && Flags.Selected != (UINT32)IsSelected)
	{
		Flags.Selected = (UINT32) IsSelected;
		ForceRedrawOfMyself();
	}
}



/***********************************************************************************************

>	virtual	BOOL SGDisplayNode::HandleEvent(SGEventType EventType, void *EventInfo,
							 				SGMiscInfo *MiscInfo);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/95	(was pure virtual before then)

	Inputs:		EventType - Indicates the event type to be handled (see SGEventType)

				EventInfo - NULL, or points at a structure containing useful info
				for processing this specific event. (See SGEventInfo)

				MiscInfo - Always supplied. Points at an SGMiscInfo struct which
				contains miscellanous useful information.

	Purpose:	Handles a generic display tree event. Events of interest trigger
				specific actions. 

				Generally overridden to provide redraw, mouse click, etc functionality.
				Overridden methos should call the base class for any event types
				which are unknown or which they do not specifically want to handle.

				For a description of how to use this, see the documentation, or
				take a look at other SGDisplay* types and galleries for example code.

				The base class doesn't do much, but does handle background redraw, and
				will pass all unhandled events on to its children (if any)

	Documentation:	docs\howtouse\sgallery.doc

***********************************************************************************************/

BOOL SGDisplayNode::HandleEvent(SGEventType EventType, void *EventInfo, SGMiscInfo *MiscInfo)
{
	switch(EventType)
	{
// Now handled by recursive DoBGRedrawPass method
//		case SGEVENT_BGREDRAW:
//			// Redraw in the background if necessary. This will return TRUE if it claims
//			// the event (the first one that successfully background renders will claim
//			// the event)
//			if (DoBGRedraw(MiscInfo))
//				return(TRUE);
//			break;

		case SGEVENT_BGFLUSH:
			DeregisterForBGRedraw();
			break;				// And drop through to flush all children
		default:
			break;
	}

	// And pass all events on to my children
	return(GiveEventToMyChildren(EventType, EventInfo, MiscInfo));
}



/***********************************************************************************************

>	virtual void SGDisplayNode::DragWasReallyAClick(SGMouseInfo *MouseInfo,
													SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/95

	Inputs:		MouseInfo - The mouse info passed to the original click handler
				MiscInfo - The misc info passed to the original click handler

	Purpose:	Handles a mouse click event. This is a callback function - drags of
				items from galleries will call this function back if the drag turns
				out to just be a click.

	Notes:		The base class method takes no action whatsoever. Derived classes
				should override this method to do something useful.

				For a description of how to use this, see the documentation, or
				take a look at other galleries for example code.

	Documentation:	docs\howtouse\sgallery.doc

	SeeAlso:	SGDisplayNode::HandleEvent; SGDisplayNode::DefaultDragHandler

***********************************************************************************************/

void SGDisplayNode::DragWasReallyAClick(SGMouseInfo *MouseInfo, SGMiscInfo *MiscInfo)
{
	// The base class does nothing
}



/***********************************************************************************************

>	virtual void SGDisplayNode::GetFormatRect(DocRect *FormatRect)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95

	Outputs:	FormatRect - Returned containing the format rectangle where this item last
				positioned itself - make sure a formatting event has gone around before
				calling this method, so that you get a meaningful result

	Purpose:	Determines where this item wants to redraw itself within the logical window
				DocCoord coordinates. If this is not a visible node type, or if someone
				neglected to cache the value in overridden methods, returns (0,0,0,0)

***********************************************************************************************/

void SGDisplayNode::GetFormatRect(DocRect *ResultFormatRect)
{
	if (ResultFormatRect != NULL)
		*ResultFormatRect = FormatRect;
}



/***********************************************************************************************

>	BOOL SGDisplayNode::IMustRedraw(SGRedrawInfo *RedrawInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/94

	Inputs:		RedrawInfo - The redraw information indicating the redraw Bounds

	Returns:	TRUE if this item should redraw itself. FALSE if it need not bother

	Purpose:	To determine if a given item needs to be redrawn. This is done by 
				determining if its bounding rectangle ('FormatRect') overlaps the
				bounding rectangle of the area to be redrawn as specified by
				RedrawInfo->Bounds.
	
	Notes:		Obviously, if FormatRect has not been correctly calculated/cached,
				this method will give spurious results!

	SeeAlso:	SGDisplayNode::CalculateFormatRect

***********************************************************************************************/

BOOL SGDisplayNode::IMustRedraw(SGRedrawInfo *RedrawInfo)
{
	// No redraw info?!
	if (RedrawInfo == NULL)
		return(TRUE);

	// Determine if the rect overlaps RedrawInfo->Bounds, returning TRUE if it does
	return (FormatRect.lo.y <= RedrawInfo->Bounds.hi.y &&
			FormatRect.hi.y >= RedrawInfo->Bounds.lo.y &&
			FormatRect.lo.x <= RedrawInfo->Bounds.hi.x &&
			FormatRect.hi.x >= RedrawInfo->Bounds.lo.x);
}



/***********************************************************************************************

>	virtual void SGDisplayNode::ForceRedrawOfMyself(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95

	Purpose:	Uses the cached FormatRect to force-redraw the appropriate part of the 
				SuperGallery display window to cause myself (only) to be redrawn.

	Notes:		Before redrawing, this scans up the tree to see if any parent is Folded.
				If this is the case, it returns without doing anything, as this item must
				therefore be hidden in the fold.
				By default, the background is erased. If bEraseBkg is FALSE, the backgound 
				will not be altered before redrawing. (Adrian 10/05/97) 


	SeeAlso:	SuperGallery::ForceRedrawOfArea

***********************************************************************************************/

void SGDisplayNode::ForceRedrawOfMyself(BOOL bEraseBkg)
{
//	if (!Flags.Invisible)
//	{
		SuperGallery *ParentGallery = GetParentGallery();
		if (ParentGallery != NULL)
		{
			// Before redrawing, search up the tree for any nodes which are folded - if there
			// are any, then I cannot be visible, so should not redraw!

			SGDisplayNode *Ptr = GetParent();
			while (Ptr != NULL)
			{
				if (Ptr->Flags.Folded)
					return;

				Ptr = Ptr->GetParent();
			}

			// Not hidden in a fold, so redraw my rectangle and set the redraw mode
			BkgEraseMode = bEraseBkg ? TRUE : FALSE;
			ParentGallery->ForceRedrawOfArea(&FormatRect);
		}
//	}
}



/***********************************************************************************************

>	virtual void SGDisplayNode::ForceRedrawOfMyselfAndChildren(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95

	Purpose:	Uses the cached FormatRect to force-redraw the appropriate part of the 
				SuperGallery display window to cause myself to be redrawn - derived
				classes may override this to also redraw their children (SGDisplayNode)

***********************************************************************************************/

void SGDisplayNode::ForceRedrawOfMyselfAndChildren(void)
{
//	if (!Flags.Invisible)
//	{
		ForceRedrawOfMyself();
//	}
}



/***********************************************************************************************

>	virtual void SGDisplayNode::RegisterForBGRedraw(void);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/95

	Purpose:	Called by derived classes to register themselves for background redraw.

				Must be called every time you want a background redraw to occur. May be
				called multiple times before the redraw occurs (it ignores repeated
				calls)

				DO NOT call this method directly - see ShouldIDrawForeground()

	Notes:		DO NOT TOUCH the Flags.RedrawPending member variable directly!

	SeeAlso:	SGDisplayNode::DoBGRedraw; SGDisplayNode::ShouldIDrawForeground

***********************************************************************************************/

void SGDisplayNode::RegisterForBGRedraw(void)
{
	if (!Flags.RedrawPending)
	{
		SuperGallery *ParentGallery = GetParentGallery();

		if (ParentGallery != NULL)
		{
			Flags.RedrawPending = TRUE;
			ParentGallery->IncrementPendingRedraws();
		}
	}
}



/***********************************************************************************************

>	virtual BOOL SGDisplayNode::DoBGRedraw(SGMiscInfo *MiscInfo);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/95

	Inputs:		MiscInfo - the usual

	Returns:	TRUE to claim the redraw event, FALSE to not claim it

	Purpose:	Forces an immediate redraw of a given node, if it is pending for
				background redraw.

				This is used by the base-class system to render items waiting for
				BG redraw, but can also be used by derived classes to force a given
				item to be visible immediately. e.g. This might be done if your item is
				clicked while waiting to be redrawn.
	
				DO NOT call this method directly - see ShouldIDrawForeground()

	Notes:		DO NOT TOUCH the Flags.RedrawPending member variable directly!

	SeeAlso:	SGDisplayNode::RegisterForBGRedraw; SGDisplayNode::ShouldIDrawForeground

***********************************************************************************************/

BOOL SGDisplayNode::DoBGRedraw(SGMiscInfo *MiscInfo)
{
	BGRenderClaimed = FALSE;			

	if (Flags.RedrawPending /*&& !Flags.Invisible*/)
	{
		SuperGallery *ParentGallery = GetParentGallery();

		if (ParentGallery != NULL)
		{
			// Interlock the redraw with the drag manager to make sure the screen isn't
			// screwed up by us redrawing over a solid drag
			DocRect KernelRect(FormatRect);
			BOOL NeedInterlock = ParentGallery->ConvertFromVirtualCoords(MiscInfo, &KernelRect);

			if (NeedInterlock)
			{
				DragManagerOp::RedrawStarting(ParentGallery->WindowID,
											ParentGallery->GetListGadgetID(),
											&KernelRect);
			}

			CurrentBGRenderNode = this;
			ForceRedrawOfMyself();				// Invalidate myself
			ParentGallery->PaintListNow();		// And immediately redraw
			CurrentBGRenderNode = NULL;

			if (NeedInterlock)
				DragManagerOp::RedrawFinished();

		}
	}

	return(BGRenderClaimed);
}



/***********************************************************************************************

>	BOOL SGDisplayNode::ShouldIDrawForeground(BOOL ForceForeground)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/5/95

	Inputs:		ForceForeground - TRUE if you want to force foreground redrawing (e.g. if you
				know your thumbnail is cached you are better off just drawing it immediately),
				or FALSE to allow the operation to be backgrounded.

	Returns:	TRUE if you should draw in the foreground (i.e. draw the item to completion),
				FALSE if you should draw the background stuff (i.e. draw a grey box instead
				of a thumbnail) - if FALSE, you'll be called back to complete the redraw later.

	Purpose:	Call this method in derived class redraw methods. Your code should go like
				this:
				MonoOn
					if (ShouldIDrawForeground(ByGollyIveGotAThumbnailCachedAlready)
						DrawTheItemFully();
					else
						DrawAGreyBox();
				MonoOff

				This system is full automatic, and is all you have to do to get BG redraw
				to work. Note that foreground redraw will be expected when (a) ForceForeground
				is TRUE, (b) the item is selected, or (c) we are doing the second BG redraw pass.
				Background redraw will generally be used in all other circumstances.

	SeeAlso:	SGDisplayNode::RegisterForBGRedraw

***********************************************************************************************/

BOOL SGDisplayNode::ShouldIDrawForeground(BOOL ForceForeground)
{
	BOOL FG = FALSE;

	if (ForceForeground || Flags.Selected)
		FG = TRUE;
	else
	{
		if (CurrentBGRenderNode == this)
			FG = TRUE;
	}

	// And make sure our current idea of the state is up to date
	if (FG)
	{
		DeregisterForBGRedraw();
		BGRenderClaimed = TRUE;		// Flag the fact that someone has FG rendered
	}
	else
		RegisterForBGRedraw();

	return(FG);
}



/***********************************************************************************************

>	virtual void SGDisplayNode::DeregisterForBGRedraw(void);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/95

	Purpose:	Ensures that this node is not pending background redraw (resets it)
				This is called in response to SGEVENT_BGFLUSH, when flushing BG redraws
	
				DO NOT call this method directly - see ShouldIDrawForeground()

	Notes:		DO NOT TOUCH the Flags.RedrawPending member variable directly!

	SeeAlso:	SGDisplayNode::RegisterForBGRedraw; SGDisplayNode::ShouldIDrawForeground

***********************************************************************************************/

void SGDisplayNode::DeregisterForBGRedraw(void)
{
	if (Flags.RedrawPending /*&& !Flags.Invisible*/)
	{
		SuperGallery *ParentGallery = GetParentGallery();

		if (ParentGallery != NULL)
			ParentGallery->DecrementPendingRedraws();

		Flags.RedrawPending = FALSE;		// And finally, turn off pending flag
	}
}



/***********************************************************************************************

>	SGDisplayNode *SGDisplayNode::DoBGRedrawPass(SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/95

	Inputs:		MiscInfo - The usual gallery MiscInfo struct

	Returns:	NULL, or a pointer to the last rendered node.

	Purpose:	Applies a background rendering pass to the display tree. This will scan the
				tree from this node onwards looking for a node to background render.

				Once a node has been background rendered, this method will return the node
				which was rendered - SuperGalleries store this in LastBackgroundNode, and will
				call this method again, using that pointer as a point to start scanning from.

				This makes scanning for background rendering nodes generally very much more
				efficient than searching the entire tree each time, as usually bg render nodes
				occur in sequential runs in the tree.

	Notes:		The SGDisplayNode RemoveFromTree method ensures that the parent gallery does
				not use this pointer after the node is deleted, by calling
				SuperGallery::SetLastBackgroundNode(NULL) if necessary - if the system is
				changed, make sure that	it is updated to ensure that the pointer is not called
				upon after the item it references has been deleted.

				ONLY LEAF NODES are guaranteed to background render with this scheme

	SeeAlso:	SGDisplayNode::RegisterForBGRedraw; SGDisplayNode::ShouldIDrawForeground;
				SuperGallery::SetLastBackgroundNode; SGDisplayNode::RemoveFromTree

***********************************************************************************************/

SGDisplayNode *SGDisplayNode::DoBGRedrawPass(SGMiscInfo *MiscInfo)
{
	// If I'm not a leaf node, scan down the tree until the first leaf child is found
	if (GetChild() != NULL)
		return(GetChild()->DoBGRedrawPass(MiscInfo));

	// This is a bit nasty... The trouble is, if a group is virtualised, GetChild() will
	// return NULL and we won't go any further. In such situations we now execute this
	// bit of code here which will pass the rendering onto the next group...
	if (this->IsKindOf(CC_RUNTIME_CLASS(SGDisplayGroup)))
	{
		if(GetNext() != NULL)
			return(GetNext()->DoBGRedrawPass(MiscInfo));
	}

	// Search (including ourself in the search) for the next node which needs to render
	// If we find one, we render it and return immediately, returning the pointer to that
	// node as the place to start the next BG rendering pass.
	SGDisplayNode *Ptr = this;
	while (Ptr != NULL)
	{
		if (Ptr->DoBGRedraw(MiscInfo))
			return(Ptr);

		Ptr = Ptr->GetNext();
	}

	// We failed to find a sibling that needs to redraw, so go skip on to the next group
	// To save the stack, we search until we find a likely candidate (a group with kids)
	Ptr = Parent;
	while (Ptr != NULL)
	{
		// Go on to the next sibling of our parent
		Ptr = Ptr->GetNext();

		if (Ptr != NULL && (Ptr->Flags.RedrawPending || Ptr->GetChild() != NULL))
			return(Ptr->DoBGRedrawPass(MiscInfo));
	}

	// We didn't find anything to draw. On the next pass, we'll start from the root of
	// the tree again (we pass back NULL, and the SuperGallery starts from scratch)
	return(NULL);
}



/***********************************************************************************************

>	virtual SGDisplayNode *SGDisplayNode::FindSubtree(SuperGallery *ParentGal,
													Document *ParentDoc, Library *ParentLib);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/94

	Inputs:		ParentGal - The parent gallery of the node to find
				ParentDoc - The parent document of the node to find
				ParentLib - The parent library of the node to find

	Returns:	NULL, or the found node

	Purpose:	Searches this node and its subtree for any SGDisplayGroup nodes which
				have parent pointers which exactly match the input parameters. This is used
				to find the subtree for a given document or library.

	SeeAlso:	SGDisplayGroup::SGDisplayGroup

***********************************************************************************************/

SGDisplayGroup *SGDisplayNode::FindSubtree(SuperGallery *ParentGal,
											Document *ParentDoc, Library *ParentLib)
{
	SGDisplayNode  *Ptr = GetChild();
	SGDisplayGroup *Result;

	while (Ptr != NULL)
	{
		if (Ptr->IsKindOf(CC_RUNTIME_CLASS(SGDisplayGroup)))	// Search immediate children
		{
			Result = (SGDisplayGroup *) Ptr;
			if (Result->GetParentGallery()  == ParentGal &&
				Result->GetParentDocument() == ParentDoc &&
				Result->GetParentLibrary()  == ParentLib)
				return(Result);
		}

																// Recurse down subtrees
		Result = Ptr->FindSubtree(ParentGal, ParentDoc, ParentLib);
		if (Result != NULL)
			return(Result);

		Ptr = Ptr->GetNext();									// No luck - try the next child
	}

	return(NULL);
}



/***********************************************************************************************

>	virtual INT32 SGDisplayNode::CompareTo(SGDisplayNode *Other, INT32 SortKey)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/94

	Inputs:		Other - the node to compare this node to
				SortKey - An integer identifying how to compare the items
					0 = No sorting (always returns 0)
					1 = Sort-by-name
					Other values will return 0, unless the derived class overrides this
					method in order to provide other sort modes.

	Returns:	negative (I am lesser), 0 (we are equal), or positive (I am greater)

	Purpose:	Compares this node to the 'other' node, to determine their relative positions
				in the display tree. Returns a value which usually indicates that the other
				node should be inserted before (-1, or 0) or after (+1) this item.

	SeeAlso:	SGDisplayNode::AddItem

***********************************************************************************************/

INT32 SGDisplayNode::CompareTo(SGDisplayNode *Other, INT32 SortKey)
{
	ERROR3IF(Other == NULL, "Illegal NULL parameter");

	switch (SortKey)
	{
		case SGSORTKEY_BYNAME:
			{
				String_256 MyName;
				String_256 ItsName;

				GetNameText(&MyName);
				Other->GetNameText(&ItsName);

				return(MyName.CompareTo(ItsName));
			}
			break;
	}

	// No sorting (SGSORTKEY_NONE - 0), or 
	return(0);
}



/********************************************************************************************

>	virtual void SGDisplayNode::GetNameText(String_256 *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Outputs:	On exit, the string pointed at by Result will contain either a blank
				string, or the name text associated with this item (if any)

	Purpose:	To determine a name string for this node. Generally, this is used for
				a simple mechanism which searches for display items whose names match
				given search parameters in some way. It is also used in libraries to
				provide default redraw methods.

	Notes:		The base class returns a blank string.
				If you can provide a better name string, then override the base class
				method to do so.

	SeeAlso:	SGDisplayNode::GetFullInfoText

********************************************************************************************/

void SGDisplayNode::GetNameText(String_256 *Result)
{
	ERROR3IF(Result == NULL, "Illegal NULL param");
	if(Result == NULL) return;

	*Result = TEXT("");
}



/********************************************************************************************

>	virtual void SGDisplayNode::GetFullInfoText(String_256 *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Outputs:	On exit, the string pointed at by Result will contain either a blank
				string, or the full-information text associated with this item (if any)
				(NOTE that the FullInfo string does NOT include the name!)

	Purpose:	To determine a full-info string for this node. Generally, this is used for
				a simple mechanism which searches for display items whose info matches
				given search parameters in some way. It is also used in libraries to
				provide default redraw methods.

	Notes:		The base class returns a blank string

				If you can provide a better full-info string, then override the base class
				method to do so.

	SeeAlso:	SGDisplayNode::GetNameText

********************************************************************************************/

void SGDisplayNode::GetFullInfoText(String_256 *Result)
{
	ERROR3IF(Result == NULL, "Illegal NULL param");
	if(Result == NULL) return;

	*Result = TEXT("");
}


/********************************************************************************************

>	virtual void SGDisplayNode::GetKeyWords(String_256 *Result)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/95

	Outputs:	On exit, the string pointed at by Result will contain either a blank
				string, or a list of | seperated keywords associated with the item
				
	Purpose:	To determine the keywords for this node. Generally, this is used for
				a simple searching mechanism.
				
	Notes:		The base class returns a blank string.
				If you can provide a better name string, then override the base class
				method to do so.

	SeeAlso:	SGDisplayNode::GetFullInfoText

********************************************************************************************/

void SGDisplayNode::GetKeyWords(String_256 *Result)
{
	ERROR3IF(Result == NULL, "Illegal NULL param");
	if(Result == NULL) return;

	*Result = TEXT("");
}



/********************************************************************************************

>	virtual BOOL SGDisplayNode::GetBubbleHelp(DocCoord *MousePos, String_256 *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Inputs:		MousePos - The current mouse position. This will generally be expected
				to lie inside this item's FormatRect. With it, this item can provide
				help on specific areas of an item.

	Outputs:	On exit, if the return value is TRUE, the string pointed at by Result
				will contain a bubble help string for this item

	Returns:	TRUE if it filled in the string, FALSE if it did not
				
	Purpose:	Called by the parent gallery when bubble help is needed. The parent
				gallery will do a hit test to determine which node contains the pointer,
				and will then ask that node to supply bubble/status-line help.
				
	Notes:		The base class returns FALSE (i.e. provides no help)
				If you can provide help, then override the base class method to do so.

	SeeAlso:	SGDisplayNode::GetStatusLineHelp

********************************************************************************************/

BOOL SGDisplayNode::GetBubbleHelp(DocCoord *MousePos, String_256 *Result)
{
	ERROR3IF(MousePos == NULL || Result == NULL, "Invalid NULL params");
	return(FALSE);
}


	
/********************************************************************************************

>	virtual BOOL SGDisplayNode::GetStatusLineHelp(DocCoord *MousePos, String_256 *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Inputs:		MousePos - The current mouse position. This will generally be expected
				to lie inside this item's FormatRect. With it, this item can provide
				help on specific areas of an item.

	Outputs:	On exit, if the return value is TRUE, the string pointed at by Result
				will contain a status line help string for this item

	Returns:	TRUE if it filled in the string, FALSE if it did not
				
	Purpose:	Called by the parent gallery when status line help is needed. The parent
				gallery will do a hit test to determine which node contains the pointer,
				and will then ask that node to supply bubble/status-line help.
				
	Notes:		The base class returns FALSE (i.e. provides no help)
				If you can provide help, then override the base class method to do so.

	SeeAlso:	SGDisplayNode::GetBubbleHelp

********************************************************************************************/

BOOL SGDisplayNode::GetStatusLineHelp(DocCoord *MousePos, String_256 *Result)
{
	ERROR3IF(MousePos == NULL || Result == NULL, "Invalid NULL params");
	return(FALSE);
}



/********************************************************************************************

>	virtual void SGDisplayNode::SelectItems(BOOL SelectThem, BOOL Exclusive = FALSE,
						Document *ParentDocument = NULL, Library *ParentLibrary = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/95

	Inputs:		SelectThem - TRUE to select the given items, FALSE to deselect them

				Exclusive  - TRUE to apply this action to all items *outside* the given
				range, FALSE to apply it to all items *inside* the range.

				Document - NULL, or the document which defines the range of items to affect

				Library - NULL, or the library which defines the range of items to affect

	Purpose:	To select/deselect groups of display items in this Gallery display.
				All items whose state changes will force redraw themselves

				If selecting items, all groups will be deselected

				Do not call this method - use the SuperGallery version

	Notes:		To select all items in a range, and deselect all items outside the range,
				you need to use 2 calls to this method.

				To select/deselect all items in the display, pass FALSE, NULL, NULL to
				the last 3 parameters. (If Doc/Lib are both NULL, 'Exclusive' has no effect)

				This base-class method does a simple thing: If this node is selectable
				(not invisible) then it sets its own selection state according to the
				'SelectThem' flag, and then passes the request on to its children.
				Any node type which understands how to cope with the last 3 parameters MUST
				override this method to provide the appropriate handling.
				(This default handling suffices, however, for root and leaf nodes)

	SeeAlso:	SuperGallery::SelectItems; SGDisplayGroup::SelectItems

********************************************************************************************/

void SGDisplayNode::SelectItems(BOOL SelectThem, BOOL Exclusive,
								Document *ParentDocument, Library *ParentLibrary)
{
	// First, if I'm a selectable node, {de}select myself as appropriate to the request
	if (!Flags.Invisible && Flags.CanSelect)
		SetSelected(SelectThem);

	// Now, pass the selection request on to my children
	SGDisplayNode *Ptr = GetChild();

	while (Ptr != NULL)
	{
		Ptr->SelectItems(SelectThem, Exclusive, ParentDocument, ParentLibrary);
		Ptr = Ptr->GetNext();
	}
}



/********************************************************************************************

>	virtual void SGDisplayNode::SelectGroups(BOOL SelectThem, BOOL Exclusive = FALSE,
						Document *ParentDocument = NULL, Library *ParentLibrary = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/95

	Inputs:		SelectThem - TRUE to select the given groups, FALSE to deselect them

				Exclusive  - TRUE to apply this action to all groups *outside* the given
				range, FALSE to apply it to all groups *inside* the range.

				Document - NULL, or the document which defines the range of groups to affect

				Library - NULL, or the library which defines the range of groups to affect

	Purpose:	To select/deselect sets of display groups in this Gallery display.
				All groups whose state changes will force redraw themselves

				If selecting, all items in the tree will be deselected

				Do not call this method - use the SuperGallery version

	Notes:		To select all groups in a range, and deselect all groups outside the range,
				you need to use 2 calls to this method.

				To select/deselect all groups in the display, pass FALSE, NULL, NULL to
				the last 3 parameters. (If Doc/Lib are both NULL, 'Exclusive' has no effect)

	SeeAlso:	SuperGallery::SelectGroups; SGDisplayGroup::SelectGroups

********************************************************************************************/

void SGDisplayNode::SelectGroups(BOOL SelectThem, BOOL Exclusive,
								Document *ParentDocument, Library *ParentLibrary)
{
	// First, if selecting, then we need to ensure that all base-class nodes are deselected.
	// The group class overrides this behaviour to correctly select groups.
	if (SelectThem && !Flags.Invisible && Flags.CanSelect)
		SetSelected(FALSE);

	// Now, pass the selection request on to my children
	SGDisplayNode *Ptr = GetChild();
	while (Ptr != NULL)
	{
		Ptr->SelectGroups(SelectThem, Exclusive, ParentDocument, ParentLibrary);
		Ptr = Ptr->GetNext();
	}
}

/********************************************************************************************

>	virtual void SGDisplayNode::SelectRangeGroups(SGDisplayGroup *PrimeNode, SGDisplayGroup *AnchorNode);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/95

	Inputs:		PrimeNode - The group which MUST be selected. May NOT be NULL.

				AnchorNode - The other group, specifying a range of sibling groups to
				be selected. May be NULL, in which case only PrimeNode is selected

	Purpose:	Selects the PrimeNode, and if possible, all sibling items between it and
				the Anchor node. If Anchor == NULL or is not found, only PrimeNode is
				selected. Does not deselect any items - you should call SelectItems first
				to clear the seln.

********************************************************************************************/

void SGDisplayNode::SelectRangeGroups(SGDisplayGroup *PrimeNode, SGDisplayGroup *AnchorNode)
{
	ERROR3IF(PrimeNode == NULL, "SGDisplayNode::SelectRangeGroup - PrimeNode must be non-NULL");

	BOOL AnchorIsHere = FALSE;
	SGDisplayGroup *First = PrimeNode;

	SuperGallery *ParentGallery = GetParentGallery();
	ERROR3IF(ParentGallery == NULL, "NULL Parent gallery?!");

	if (AnchorNode != NULL)
	{
		SGDisplayNode *Ptr = PrimeNode->GetParent();
		ERROR3IF(Ptr == NULL, "SGDisplayNode::SelectRangeGroup - Tree linkage corrupt, or PrimeNode is the root!");		

		// Find which of the two nodes comes first	
		Ptr = Ptr->GetChild();
		while (Ptr != NULL && Ptr != PrimeNode)
		{
			if (Ptr == AnchorNode)
			{
				First = (SGDisplayGroup *)AnchorNode;		// Ooops - the Anchor node occurs first!
				AnchorIsHere = TRUE;	// And remember we've found the Anchor
				break;
			}
			
			Ptr = Ptr->GetNext();
		}

		// Continue scanning until we can be sure that the Anchor Node is in the sibling list
		while (Ptr != NULL && !AnchorIsHere)
		{
			AnchorIsHere = (Ptr == AnchorNode);
			Ptr = Ptr->GetNext();
		}
	}

	if (!AnchorIsHere)		// Only one item in the range!
	{
		PrimeNode->SetSelected(TRUE);
		PrimeNode->ForceRedrawOfMyself();

		// And inform the parent gallery of the selection change
		ParentGallery->SetLastSelectedNode(PrimeNode);
		ParentGallery->SelectionHasChanged();
		return;
	}

	// We have a valid range. Scan from First to Last, selecting everything in our path
	SGDisplayGroup *Last = (First == PrimeNode) ? AnchorNode : PrimeNode;
	while (First != NULL && First != Last)
	{
		First->SetSelected(TRUE);
		First->ForceRedrawOfMyself();
		
		First = (SGDisplayGroup *) First->GetNext();
	}

	// And set the last one
	Last->SetSelected(TRUE);
	Last->ForceRedrawOfMyself();

	// And inform the parent gallery of the selection change
	ParentGallery->SetLastSelectedNode(Last);
	ParentGallery->SelectionHasChanged();
}

/********************************************************************************************

>	virtual void SGDisplayNode::SelectRangeItems(SGDisplayItem *PrimeNode, SGDisplayItem *AnchorNode);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/95

	Inputs:		PrimeNode - The item which MUST be selected. May NOT be NULL.

				AnchorNode - The other item, specifying a range of sibling items to
				be selected. May be NULL, in which case only PrimeNode is selected

	Purpose:	Selects the PrimeNode, and if possible, all sibling items between it and
				the Anchor node. If Anchor == NULL or is not found, only PrimeNode is
				selected. Does not deselect any items - you should call SelectItems first
				to clear the seln.

********************************************************************************************/

void SGDisplayNode::SelectRangeItems(SGDisplayItem *PrimeNode, SGDisplayItem *AnchorNode)
{
	ERROR3IF(PrimeNode == NULL, "SGDisplayItem::SelectRangeItem - PrimeNode must be non-NULL");

	BOOL AnchorIsHere = FALSE;
	SGDisplayItem *First = PrimeNode;

	SuperGallery *ParentGallery = GetParentGallery();
	ERROR3IF(ParentGallery == NULL, "NULL Parent gallery?!");

	if (AnchorNode != NULL)
	{
		SGDisplayNode *Ptr = PrimeNode->GetParent();
		ERROR3IF(Ptr == NULL, "SGDisplayItem::SelectRangeItem - Tree linkage corrupt, or PrimeNode is the root!");		

		// Find which of the two nodes comes first	
		Ptr = Ptr->GetChild();
		while (Ptr != NULL && Ptr != PrimeNode)
		{
			if (Ptr == AnchorNode)
			{
				First = (SGDisplayItem *)AnchorNode;		// Ooops - the Anchor node occurs first!
				AnchorIsHere = TRUE;	// And remember we've found the Anchor
				break;
			}
			
			Ptr = Ptr->GetNext();
		}

		// Continue scanning until we can be sure that the Anchor Node is in the sibling list
		while (Ptr != NULL && !AnchorIsHere)
		{
			AnchorIsHere = (Ptr == AnchorNode);
			Ptr = Ptr->GetNext();
		}
	}

	if (!AnchorIsHere)		// Only one item in the range!
	{
		PrimeNode->SetSelected(TRUE);
		PrimeNode->ForceRedrawOfMyself();

		// And inform the parent gallery of the selection change
		ParentGallery->SetLastSelectedNode(PrimeNode);
		ParentGallery->SelectionHasChanged();
		return;
	}

	// We have a valid range. Scan from First to Last, selecting everything in our path
	SGDisplayItem *Last = (First == PrimeNode) ? AnchorNode : PrimeNode;
	while (First != NULL && First != Last)
	{
		First->SetSelected(TRUE);
		First->ForceRedrawOfMyself();
		
		First = (SGDisplayItem *) First->GetNext();
	}

	// And set the last one
	Last->SetSelected(TRUE);
	Last->ForceRedrawOfMyself();

	// And inform the parent gallery of the selection change
	ParentGallery->SetLastSelectedNode(Last);
	ParentGallery->SelectionHasChanged();
}


/***********************************************************************************************

>	void SGDisplayNode::DrawPlinth(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo,
									DialogColourInfo *RedrawColours,
									DocRect *ButtonRect,
									BOOL Indented = FALSE,
									UINT32 GlyphResourceID = 0)
									
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/95

	Inputs:		RedrawInfo - The redraw info, as normal with all this redraw stuff
				MiscInfo - The MiscInfo, as normal for a SG Event
				RedrawColours - The object what knows about them thar plotting colours
				ButtonRect - The rectangle inside which to draw the plinth
				Indented - TRUE for indented button, FALSE for raised button
				GlyphResourceID - 0 (for no glyph, in which case the plinth 'face' is flat-
				filled with the appropriate colour), else the resource ID of a bitmap
				to be drawn in the center of the plinth/button.

	Purpose:	Draws a plinth (2 white lines and 2 dark grey lines) around the inside edge
				of the given rectangle, in order to give a Windows 95 like button plinth.
				It can include a button glyph bitmap. It's static so anybody can
				call it if they so desire.
	
	Notes:		It would be advantageous if the provided button rectangle is aligned
				to the device pixel grid if you want its appearance to be correct

				The area within the plinth is filled with colour, so all pixels within
				the given rectangle are guaranteed to be painted - i.e. you do not need
				to clear the background region before calling this routine (indeed you
				should not, if you wish to avoid flicker)

***********************************************************************************************/

void SGDisplayNode::DrawPlinth(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo,
									 DialogColourInfo *RedrawColours,
									 DocRect *ButtonRect, BOOL Indented,
									 UINT32 GlyphResourceID)
{
	ERROR3IF(RedrawInfo == NULL || RedrawColours == NULL || ButtonRect == NULL,
			"SGDisplayNode::DrawPlinth - NULL parameters aer illegal");
//	if (!Flags.Invisible)
//	{
		RenderRegion *pRender = RedrawInfo->Renderer;
		pRender->SaveContext();
	
		pRender->SetLineWidth(0);
	
		// Fill inside the plinth with Button Face grey
		DocRect InsideRect(*ButtonRect);
		InsideRect.Inflate(-MiscInfo->PixelSize);

		// Fill inside the plinth with button-face (grey), and optionally with the glyph bitmap
		// (We always fill behind the bitmap first in case the bitmap isn't big enough to
		// fill the entire area)
		DocColour trans(COLOUR_TRANS);
		pRender->SetLineColour(trans);
		pRender->SetFillColour(RedrawColours->ButtonFace());
		pRender->DrawRect(&InsideRect);

		if (GlyphResourceID != 0)
		{
			// Quick v1.5 release bodge to center the up & down scroll arrow glyphs in the plinth
			if (GlyphResourceID == _R(IDB_GALLERY_SCROLLUP) || GlyphResourceID == _R(IDB_GALLERY_SCROLLDOWN))
			{
				DocRect CenterRect(InsideRect);
				CenterRect.Inflate(((7*MiscInfo->PixelSize) - CenterRect.Width())/2);	// Shrink to 7 pixels, centered
				DrawBitmap(pRender, &CenterRect, GlyphResourceID);
			}
			else
				DrawBitmap(pRender, &InsideRect, GlyphResourceID);
		}

		// Draw the plinth border
		DocColour LineCol;

		if (Indented)
			LineCol = RedrawColours->ButtonShadow();
		else
			LineCol = RedrawColours->ButtonHighlight();

		pRender->SetFillColour(LineCol);

		DocRect TempRect(*ButtonRect);					// Left
		TempRect.hi.x = TempRect.lo.x + MiscInfo->PixelSize;
		pRender->DrawRect(&TempRect);

		TempRect = *ButtonRect;							// Top
		TempRect.lo.y = TempRect.hi.y - MiscInfo->PixelSize;
		pRender->DrawRect(&TempRect);

		if (Indented)
			LineCol = RedrawColours->ButtonHighlight();
		else
			LineCol = RedrawColours->ButtonShadow();
	
		pRender->SetFillColour(LineCol);
		TempRect = *ButtonRect;							// Right
		TempRect.lo.x = TempRect.hi.x - MiscInfo->PixelSize;
		pRender->DrawRect(&TempRect);
	
		TempRect = *ButtonRect;							// Bottom
		TempRect.hi.y = TempRect.lo.y + MiscInfo->PixelSize;
		pRender->DrawRect(&TempRect);
	
		pRender->RestoreContext();
//	}
}



/***********************************************************************************************

>	void SGDisplayNode::DrawSelectionOutline(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo,
											DocRect *BoundsRect, INT32 Width = 0)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Inputs:		RedrawInfo - The info passed in to HandleEvent for redraws
				MiscInfo   - Standard miscinfo struct
				BoundsRect - The rectangle to draw the border into
				Width - 0 for a default (2-pixel-wide) border, else the width of the
				lines, in pixels. The outside of the frame will always touch the edge
				of the given rectangle, so essentially, this is used as a 'defalate'
				value to find the inside edge of the lines.

	Purpose:	Draws a black 2-pixel-thick frame-rectangle just inside the given Rect.
				This is the normal selection-rectangle outline which should go outside
				an icon or thumbnail when an item is selected.

	Notes:		If you wish to draw inside the rectangle, use
					TheRect.Inflate(DevicePixels(MiscInfo, 2));

				This draws 4 filled rectangles around the border of the rectangle. The
				area inside the rectangle is left unpainted - this will reduce flicker
				on large items as compared to blatting a single rectangle behind the
				icon/thumbnail.

***********************************************************************************************/

void SGDisplayNode::DrawSelectionOutline(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo,
											DocRect *BoundsRect, INT32 Width)
{
	ERROR3IF(RedrawInfo == NULL || MiscInfo == NULL || BoundsRect == NULL,
			"SGDisplayNode::DrawSelectionOutline - NULL parameters aer illegal");

//	if (!Flags.Invisible)
//	{
		RenderRegion *pRender = RedrawInfo->Renderer;	

		if (Width == 0)			// Default value of zero means a 2-pixel border
			Width = DevicePixels(MiscInfo, 2);

		pRender->SaveContext();
	
		pRender->SetLineWidth(0);
		DocColour trans(COLOUR_TRANS);
		pRender->SetLineColour(trans);
		DocColour black(COLOUR_BLACK);
		pRender->SetFillColour(black);

		DocRect TempRect(*BoundsRect);					// Left
		TempRect.hi.x = TempRect.lo.x + Width;
		pRender->DrawRect(&TempRect);

		TempRect = *BoundsRect;							// Top
		TempRect.lo.y = TempRect.hi.y - Width;
		pRender->DrawRect(&TempRect);

		TempRect = *BoundsRect;							// Right
		TempRect.lo.x = TempRect.hi.x - Width;
		pRender->DrawRect(&TempRect);

		TempRect = *BoundsRect;							// Bottom
		TempRect.hi.y = TempRect.lo.y + Width;
		pRender->DrawRect(&TempRect);

		pRender->RestoreContext();
//	}
}



/***********************************************************************************************

>	void SGDisplayNode::DrawBitmap(RenderRegion *Renderer, DocRect *BoundsRect, UINT32 ResID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/95

	Inputs:		Renderer - The render region to render with
				BoundsRect - The rectangle to draw the bitmap into
				ResID - A non-zero bitmap-resource identifier (the bitmap to draw)

	Purpose:	Draws the given bitmap to screen in the given DocRect of the supergallery
				display list. (Used to be necessary before RR:DrawBitmap came along)

	SeeAlso:	RenderRegion::DrawBitmap

***********************************************************************************************/

void SGDisplayNode::DrawBitmap(RenderRegion *Renderer, DocRect *BoundsRect, UINT32 ResID)
{
	ERROR3IF(Renderer == NULL || BoundsRect == NULL || ResID == 0,
			"SGDisplayNode::DrawBitmap - NULL Parameter(s) are illegal");

//	if (!Flags.Invisible)
//	{
		Renderer->DrawBitmap(BoundsRect->lo, ResID);
//	}
}



/***********************************************************************************************

>	virtual void SGDisplayNode::StartRendering(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95

	Inputs:		RedrawInfo - The RedrawInfo passed to your HandleEvent function
							 for SGEVENT_REDRAWs
				MiscInfo -	 The MiscInfo passed in to your HandleEvent function

	Purpose:	MUST be called by all derived node types when they are about to start
				rendering. This allows us to do things like using GRenderRegions to small
				bitmaps (a region for each item rather than one region for the whole window)
				and other fabby things.

	SeeAlso:	SGDisplayNode::StopRendering

***********************************************************************************************/

void SGDisplayNode::StartRendering(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo)
{
	// Does nothing, for now
}



/***********************************************************************************************

>	virtual void SGDisplayNode::StopRendering(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95

	Inputs:		RedrawInfo - The RedrawInfo passed to your HandleEvent function
							 for SGEVENT_REDRAWs
				MiscInfo -	 The MiscInfo passed in to your HandleEvent function

	Purpose:	MUST be called by all derived node types when they have finished
				rendering. This allows us to do things like using GRenderRegions to small
				bitmaps (a region for each item rather than one region for the whole window)
				and other fabby things.

	SeeAlso:	SGDisplayNode::StopRendering

***********************************************************************************************/

void SGDisplayNode::StopRendering(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo)
{
	// Does nothing, for now
}


/***********************************************************************************************

>	virtual BOOL SGDisplayNode::DefaultPreDragHandler(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Inputs:		Mouse - Information on the mouse state for this click
				MiscInfo - the normal info as passed to event handlers

	Returns:	TRUE if the click caused any action to be taken (selection state to change)
				FALSE if the click was ignored for whatever reason

	Purpose:	Provides part 1 of the default selection model for clicks on gallery display
				nodes. Should be called by all derived gallery DisplayItems to handle clicks
				upon them, when multiple-selection support is desired.

				You should call this method immediately prior to starting a drag as a result
				of a click event. Note that it is paired with DefaultClickHandler (which
				should be called when the drag you start turns out to be a click, if you
				want multiple-selection capability).

				See the SGDisplayColour (kernel\sgcolour.cpp) for an example of use

	SeeAlso:	SGDisplayItem::DefaultClickHandler; SGDisplayColour::HandleEvent

***********************************************************************************************/

BOOL SGDisplayNode::DefaultPreDragHandler(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo)
{
	if (Mouse->Adjust)		// Drags cannot be started with adjust
		return(FALSE);

	SuperGallery *ParentGallery = GetParentGallery();

	if (Mouse->Extend)
	{
		BOOL Handled = TRUE;

		// This was a click to extend the selection to the clicked item
#if FALSE
/* I was led to believe that this was how shift-select worked, but it isn't (thank God)
   so I have removed this case again - Jason

		if (IsSelected())
		{
			// If this item is selected, then shift-clicking it acts to leave it
			// selected, and deselect every other item.
			
			ParentGallery->SelectItems(FALSE);	// Deselect everything else
			ParentGallery->SelectGroups(FALSE);	// Deselect all groups

			// Repaint the list box now. This is because if there is a large
			// distance between the old selection and the new one, we get a huge
			// redraw cliprect, so get a (slow) complete redraw, instead of two
			// small redraws. It is thus better to break the redraw into 2 steps
			// so that we are more likely to get 2 fast redraws than one slow one.
			ParentGallery->PaintListNow();
		
			// And select myself, with immediate redraw
			SetSelected(TRUE);
			ParentGallery->PaintListNow();

			// Update the ParentGallery to know that we are the new multi-selection anchor
			ParentGallery->SetLastSelectedNode(this);

			// And inform the parent gallery that the selection may have changed
			ParentGallery->SelectionHasChanged();

			return(TRUE);
		}
		else
*/
#endif
		{
			if (ParentGallery->GetSelectedItemCount() == 0 && ParentGallery->GetSelectedGroupCount() == 0)
				Handled = FALSE;	// No selection - treat extend-click as a normal click
			else
			{
				ParentGallery->SelectItems(FALSE);	// Deselect all items
				ParentGallery->SelectGroups(FALSE);	// Deselect all groups

				ParentGallery->SelectRange(this, ParentGallery->GetLastSelectedNode());
			}
		}

		if (Handled)
			return(TRUE);
	}
	
	
	if (!Flags.Selected)
	{
		if (!Mouse->Adjust)
		{
			// If it's not an adjust-click, deselect all other items and groups
			ParentGallery->SelectItems(FALSE);
			ParentGallery->SelectGroups(FALSE);

			// Repaint the list box now. This is because if there is a large
			// distance between the old selection and the new one, we get a huge
			// redraw cliprect, so get a (slow) complete redraw, instead of two
			// small redraws. It is thus better to break the redraw into 2 steps
			// so that we are more likely to get 2 fast redraws than one slow one.
			ParentGallery->PaintListNow();
		}
		else
		{
			// Mutual exclusion stuff... If we're a group, deselect all items, etc...
			if(this->IS_KIND_OF(SGDisplayGroup))
				ParentGallery->SelectItems(FALSE);
			else if(this->IS_KIND_OF(SGDisplayItem))
				ParentGallery->SelectGroups(FALSE);
			else
				ERROR3("What are we if we're not a group or an item ?");				
		}
	
		// And select myself, and do an immediate redraw
		SetSelected(TRUE);
		ParentGallery->PaintListNow();

		// Update the ParentGallery to know that we are the new multi-selection anchor
		ParentGallery->SetLastSelectedNode(this);

		// And inform the parent gallery that the selection may have changed
		ParentGallery->SelectionHasChanged();

		return(TRUE);
	}

	return(FALSE);
}



/***********************************************************************************************

>	virtual BOOL SGDisplayNode::DefaultClickHandler(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo,
													BOOL AfterDrag = FALSE,
													BOOL AdjustDoubleClick = TRUE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/94

	Inputs:		Mouse - Information on the mouse state for this click
				MiscInfo - the normal info as passed to event handlers

				AfterDrag - TRUE if this is being called when a drag turns into a click, and
				you called DefaultPreDragHandler before the drag started

				AdjustDoubleClick - TRUE to do normal adjust-double-click handling (which
				closes the gallery after applying the item)
				FALSE to not close the gallery (used by the colour gallery to stop the
				gallery closing when applying with adjust as it applies line colour, so
				overrides the default behaviour)

	Returns:	TRUE if the click caused any action to be taken (selection state to change)
				FALSE if the click was ignored for whatever reason

	Purpose:	Provides the default selection model for clicks on gallery display nodes.
				Should be called by all derived gallery DisplayItems to handle clicks
				upon them, when multiple-selection support is desired.

	SeeAlso:	SuperGallery; SGDisplayNode::InsertAfter; SGDisplayNode::InsertBefore

***********************************************************************************************/

BOOL SGDisplayNode::DefaultClickHandler(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo,
										BOOL AfterDrag, BOOL AdjustDoubleClick)
{
	TRACEUSER( "Matt", _T("SGDisplyaNode::DefaultClickHandler called\n"));
	SuperGallery *ParentGallery = GetParentGallery();

	if (!AfterDrag)
	{
		// This was not called after having previously called DefaultPreDragHandler, 
		// so we have to do both halves of the processing. If PreDrag deals with it,
		// we don't need to do anything further.
		if (DefaultPreDragHandler(Mouse, MiscInfo))
			return(TRUE);
	}

	if (Mouse->DoubleClick && !Flags.Invisible)
	{
		// On double-click, this item becomes the only selected item, and
		// it is applied (if that action is supported by the parent gallery)

		ParentGallery->SelectItems(FALSE);	// Deselect everything else
		ParentGallery->SelectGroups(FALSE);	// Deselect all groups

		// Repaint the list box now. This is because if there is a large
		// distance between the old selection and the new one, we get a huge
		// redraw cliprect, so get a (slow) complete redraw, instead of two
		// small redraws. It is thus better to break the redraw into 2 steps
		// so that we are more likely to get 2 fast redraws than one slow one.
		ParentGallery->PaintListNow();
	
		// And select myself, and do an immediate redraw
		SetSelected(TRUE);
		ParentGallery->PaintListNow();

		// Update the ParentGallery to know that we are the new multi-selection anchor
		ParentGallery->SetLastSelectedNode(this);

		// And inform the parent gallery that the selection may have changed
		ParentGallery->SelectionHasChanged();
	
		if (Mouse->Adjust)
		{
			BOOL ActionApplied = ParentGallery->ApplyAction(SGACTION_APPLYADJUST);
			if (!ActionApplied)
				ActionApplied = ParentGallery->ApplyAction(SGACTION_APPLY);

			if (ActionApplied && AdjustDoubleClick)
			{
				// Adjust/Ctrl double click of an item. This applies the item and then
				// auto closes the gallery (just like RISC OS and Win95)
				ParentGallery->SetVisibility(FALSE);

				DialogBarOp::SetSystemStateChanged();	// Ensure toolbar button pops out again
			}
		}
PORTNOTE("galleries", "Disabled clipart gallery")
#ifndef EXCLUDE_FROM_XARALX
		else if (ParentGallery->IsKindOf(CC_RUNTIME_CLASS(LibFillsSGallery)))
						 LibClipartSGallery::ImportClipart(TRUE, (LibraryGallery*) ParentGallery);
#endif
		else
			ParentGallery->ApplyAction(SGACTION_APPLY);

		return(TRUE);
	}

	// If this was an adjust click, it should toggle the selection state
	if (Mouse->Adjust)
	{
		BOOL AreWeSelected = IsSelected();

		// Mutual exclusion stuff... If we're a group, deselect all items, etc...
		if(this->IS_KIND_OF(SGDisplayGroup))
			ParentGallery->SelectItems(FALSE);
		else if(this->IS_KIND_OF(SGDisplayItem))
			ParentGallery->SelectGroups(FALSE);
		else
			ERROR3("What are we if we're not a group or an item ?");				

		// Invert my selection state, and do an immediate redraw
		SetSelected(!AreWeSelected);
		ParentGallery->PaintListNow();

		// Update the ParentGallery to know that we are the new multi-selection anchor
		ParentGallery->SetLastSelectedNode(this);

		// And inform the parent gallery that the selection may have changed
		ParentGallery->SelectionHasChanged();

		return(TRUE);
	}
	return(FALSE);
}


/***********************************************************************************************

>	CWindowID SGDisplayNode::GetListWindow(void)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/05/2006

	Purpose:	Returns the window id of the list box window of the parent gallery

	SeeAlso:	-

***********************************************************************************************/

CWindowID SGDisplayNode::GetListWindow(void)
{
	return(DialogManager::GetGadget(GetParentGallery()->GetReadWriteWindowID(), GetParentGallery()->GetListGadgetID()));
}



/***********************************************************************************************

>	virtual void SGDisplayNode::DumpSubtree(INT32 TreeLevel = 1)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/94
	Purpose:	Dumps TRACE output to the debugger showing the layout of the tree
				THIS FUNCTION IS ONLY PRESENT IN DEBUG BUILDS (#ifdef _DEBUG)
				It will also only produce debug output if your UserName is "Jason"

***********************************************************************************************/

#ifdef _DEBUG
void SGDisplayNode::DumpSubtree(INT32 TreeLevel)
{
	PORTNOTETRACE("galleries", "Disabled debug dump");
#ifndef EXCLUDE_FROM_XARALX
	if (IsUserName("Matt"/*Jason*/))
	{
		// First, dump out myself...
		TCHAR Temp[200];
		TCHAR Msg[200];
		INT32 i;
		for (i = 0; i < (TreeLevel * 2) && i < 50; i++)
		   Msg[i] = ' ';

		Msg[i] = '\0';

		wsprintf(Temp, "%s (%x)\n", GetRuntimeClass()->m_lpszClassName, (INT32)this);
		camStrcat(Msg, Temp);
		OutputDebugString(Msg);


		// Now recursively dump all my children...
		SGDisplayNode *Ptr = GetChild();
		while (Ptr != NULL)
		{
			Ptr->DumpSubtree(TreeLevel+1);
			Ptr = Ptr->Next;
		}
	}
#endif
}
#endif












/***********************************************************************************************

>	SGDisplayRoot::SGDisplayRoot()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/94

	Purpose:	SGDisplayRoot constructor (makes root nodes invisible by default)

	SeeAlso:	SGDisplayNode::SGDisplayNode

***********************************************************************************************/

SGDisplayRoot::SGDisplayRoot()
{
	Child = NULL;

	Flags.Invisible = TRUE;			// The root node is always invisible
	ParentGallery = NULL;
	ScrollExtent = 0;

	ERROR3("SGDisplayRoot constructed in an illegal manner");
}



/***********************************************************************************************

>	SGDisplayRoot::SGDisplayRoot(SuperGallery *ParentGal)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/94

	Inputs:		ParentGal - A pointer to the parent gallery of this node/tree

	Purpose:	SGDisplayRoot constructor (makes root nodes invisible by default)

	SeeAlso:	SGDisplayNode::SGDisplayNode

***********************************************************************************************/

SGDisplayRoot::SGDisplayRoot(SuperGallery *ParentGal)
{
	Child = NULL;

	Flags.Invisible = TRUE;			// The root node is always invisible
	ParentGallery = ParentGal;
	ScrollExtent = 0;
}



/***********************************************************************************************

>	virtual SGDisplayNode *SGDisplayRoot::GetChild(void) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94 (Made virtual 13/5/95)
	Returns:	A pointer to the first child of this SGDisplayNode object, or NULL

	Purpose:	Finds the child of this DisplayTree Node.
				Returns NULL if you have reached the boundary of the tree

	SeeAlso:	SuperGallery; SGDisplayNode::GetParent;
				SGDisplayNode::GetNext; SGDisplayNode::GetPrevious

***********************************************************************************************/

SGDisplayNode *SGDisplayRoot::GetChild(void) const
{
	return(Child);
}



/***********************************************************************************************

>	void SGDisplayRoot::SetChild(SGDisplayNode *NewChild)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94

	Inputs:		A pointer to the new first-child of this SGDisplayNode object

	Purpose:	Sets the child of this DisplayTree Node. Overrides to base class method
				to allow the child to be set.

	SeeAlso:	SuperGallery; SGDisplayNode::GetParent;
				SGDisplayNode::GetNext; SGDisplayNode::GetPrevious

***********************************************************************************************/

void SGDisplayRoot::SetChild(SGDisplayNode *NewChild)
{
	Child = NewChild;
}



/***********************************************************************************************

>	virtual SuperGallery *SGDisplayRoot::GetParentGallery(void) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/94

	Returns:	The parent SuperGallery, or NULL

	Purpose:	Returns the SuperGallery which 'owns' this node (and its subtree)

	SeeAlso:	SGDisplayNode::GetParentGallery

***********************************************************************************************/

SuperGallery *SGDisplayRoot::GetParentGallery(void) const
{
	ERROR3IF(ParentGallery == NULL, "I have no parent gallery! This is not right!");

	return(ParentGallery);
}



/***********************************************************************************************

>	virtual void SGDisplayRoot::InsertAfter(SGDisplayNode *NodeToInsert)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/94

	Inputs:		NodeToInsert - the new node to insert into the tree

	Purpose:	OVERRIDES SGDisplayNode::InsertAfter

				Insertions at the root node are always converted to AddItem() calls,
				to insert the item as the last child of the root node. The root cannot
				have siblings!

	SeeAlso:	SGDisplayNode::AddItem

***********************************************************************************************/

void SGDisplayRoot::InsertAfter(SGDisplayNode *NodeToInsert)
{
	AddItem(NodeToInsert);
}



/***********************************************************************************************

>	virtual void SGDisplayRoot::InsertBefore(SGDisplayNode *NodeToInsert)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/94

	Inputs:		NodeToInsert - the new node to insert into the tree

	Purpose:	OVERRIDES SGDisplayNode::InsertBefore

				Insertions at the root node are always converted to AddItem() calls,
				to insert the item as the last child of the root node. The root cannot
				have siblings!

	SeeAlso:	SGDisplayNode::AddItem

***********************************************************************************************/

void SGDisplayRoot::InsertBefore(SGDisplayNode *NodeToInsert)
{
	AddItem(NodeToInsert);
}



/***********************************************************************************************

>	virtual void SGDisplayRoot::RemoveFromTree(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/94

	Purpose:	OVERRIDES SGDisplayNode::RemoveFromTree
				The root node cannot be removed from the tree - it returns quietly, taking
				no action (this allows the generic treatment of DestroyTree to work without
				having to take special action for delinking the root node)

	SeeAlso:	SGDisplayNode::RemoveFromTree; SGDisplayNode::DestroySubtree

***********************************************************************************************/

void SGDisplayRoot::RemoveFromTree(void)
{
	// do nothing
}



/***********************************************************************************************

>	virtual void SGDisplayRoot::DestroySubtree(BOOL IncludingThisNode = TRUE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/94

	Inputs:		IncludingThisNode - TRUE (The default) to delete this node (the root of
				the subtree) as well as all its children.
				FALSE to delete its children only (This leaves this node untouched, but
				vapes all child nodes)
				NOTE - This parameter is ignored, and always treated as FALSE

	Purpose:	DESTROYS the subtree starting at this root node.
				This does a depth-first recursive scan of the subtree, delinking each item,
				and then CALLING EACH ITEMS DESTRUCTOR.

				IT DOES NOT destruct the root node, however (i.e. it destroys the tree down
				to the point where only the root is left alive).

	Errors:		May be generated by the RemoveFromTree and destructor calls if the subtree
				is in some way corrupt - see these calls for details.

	SeeAlso:	SuperGallery; SGDisplayNode::RemoveFromTree; SGDisplayNode::DestroySubtree

***********************************************************************************************/

void SGDisplayRoot::DestroySubtree(BOOL IncludingThisNode)
{
	// Destroy all our children, but DO NOT delete 'this' node!
	SGDisplayNode::DestroySubtree(FALSE);
}



/***********************************************************************************************

>	virtual void SGDisplayRoot::InitFormatInfo(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/10/94

	Inputs:		FormatInfo	- Points at the format information to be initialised
				MiscInfo	- Points at a structure containing standard 'MiscInfo'

	Outputs:	FormatInfo is initialised as appropriate
				MiscInfo->MaxWidth is gridlocked to the pixel grid

	Purpose:	Resets the formatting info structure to default values for the start of a
				formatting pass. Must be called before SGDisplayRoot::Redraw

	SeeAlso:	SGDisplayRoot::Redraw

***********************************************************************************************/

void SGDisplayRoot::InitFormatInfo(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo)
{
	MiscInfo->MaxWidth = GridLock(MiscInfo, MiscInfo->MaxWidth); // Maximum width of formatting lines

	FormatInfo->LinePos = 0;								// Start at the top of the list
	FormatInfo->LineHeight = 0;								// The current line is 0 height so far
	FormatInfo->AvailableWidth = MiscInfo->MaxWidth;		// The full line is left to allocate
	FormatInfo->IndentLevel = 0;							// Initially we are not indented

	FormatInfo->AccumulateBounds = FALSE;					// By default, don't accumulate bounds
	FormatInfo->LastInvalidNode  = NULL;					// For the group dragging redraw fix
	FormatInfo->InvalidBounds = DocRect(0, 1, MiscInfo->MaxWidth, 2);
}



/***********************************************************************************************

>	virtual BOOL SGDisplayRoot::HandleEvent(SGEventType EventType, void *EventInfo,
											SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94

	Inputs:		See SGDisplayNode::HandleEvent

	Returns:	TRUE if the event was handled successfully
				FALSE if it was not

	Purpose:	Handles a SuperGallery DisplayTree event

	Notes:		This overrides the pure virtual SGDisplayNode::HandleEvent method

				A node need not handle a specific event - if it does not handle it, it
				should return FALSE.

				Redraw and Formatting handlers should never return TRUE, as this will
				prevent the event from continuing through the tree.

				Non-leaf-nodes must call SGDisplayNode::GiveEventToMyChildren in order
				to pass the event down the tree.

	SeeAlso:	SGDisplayNode::HandleEvent; SGDisplayGroup::HandleEvent;
				SGDisplayItem::HandleEvent;	SGDisplayRoot::InitFormatInfo;
				SGDisplayRoot::CalculateListExtent;
				SGDisplayNode::GiveEventToMyChildren

***********************************************************************************************/

BOOL SGDisplayRoot::HandleEvent(SGEventType EventType, void *EventInfo,
								SGMiscInfo *MiscInfo)
{
	switch (EventType)
	{
		case SGEVENT_FORMAT:
			// Do nothing, but ensure we don't use the 'default:' case.
			break;

		case SGEVENT_REDRAW:
			{
				SGRedrawInfo *RedrawInfo = GetRedrawInfo(EventType, EventInfo);

				// This line is to stop the white flicker when backgrounding the items
				// Hmmm... There are a few problems which need sorting out though !
				if (BkgEraseMode == TRUE)
				{
					StartRendering(RedrawInfo, MiscInfo);

					RenderRegion *pRender = RedrawInfo->Renderer;

					// Set default background colour
					pRender->SetFillColour(RedrawInfo->Background);
					pRender->SetLineColour(COLOUR_TRANS);	//RedrawInfo->Background);

					// Work out height first to avoid ENSURE in DocRect ctor
					INT32 height=ScrollExtent;
					// Extend the rect if necessary to ensure entire window bg is filled
					if (ScrollExtent < MiscInfo->WindowHeight)
						height = MiscInfo->WindowHeight;

					if ((height>=0) && (MiscInfo->MaxWidth>=0))
					{
						// Fill entire display window background with bg colour
						DocRect WindowBG(0, -height, MiscInfo->MaxWidth, 0);
						pRender->DrawRect(&WindowBG);
					}

					StopRendering(RedrawInfo, MiscInfo);
				}
				else
					BkgEraseMode = TRUE;

				if (GetChild() == NULL)	// We have no children (empty tree)
				{
					// Redraw ourself as 'No Items' indicator
					// ERROR3("SGDisplayRoot::Redraw - I have no children to draw");
					return(TRUE);	// Might as well return TRUE - there is nobody else!
				}
			}
			break;


		default:
			return(SGDisplayNode::HandleEvent(EventType, EventInfo, MiscInfo));
	}

	// We have children, so recursively call them all with this event
	BOOL result = GiveEventToMyChildren(EventType, EventInfo, MiscInfo);

	// If we've just reformatted the tree, remember the new list extent
	if (EventType == SGEVENT_FORMAT)
	{
		SGFormatInfo *FormatInfo = GetFormatInfo(EventType, EventInfo);

		ScrollExtent = (ABS(FormatInfo->LinePos));
		SetScrollOffset(GetScrollOffset(), MiscInfo);	// Ensure not scrolled off end!

		if (FormatInfo->LastInvalidNode != NULL)
		{
			// The immediately previous (final) node had invalid bounds.
			// We'd better include the blank part of the list below it in the
			// invalid bounds, to ensure we don't leave little bits of stuff past the end
			// of the list if it has got smaller.

			ERROR3IF(FormatInfo->InvalidBounds.hi.y > 0,
						"Gallery display formatting error - LastInvalidNode should be NULL "
						"if there haven't been any invalid nodes yet");

			FormatInfo->InvalidBounds.lo.y = FormatInfo->LinePos - MiscInfo->WindowHeight;
		}
	}

	return(result);
}



/***********************************************************************************************

>	virtual INT32 SGDisplayRoot::CalculateListExtent(SGFormatInfo *FormatInfo,
														SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/94

	Inputs:		FormatInfo - should have been initialised with InitFormatInfo
				MiscInfo - Miscellanous info needed for formatting

	Outputs:	FormatInfo is updated as appropriate

	Returns:	The needed (window scroll) extent to contain the entire list if it were to
				be redrawn right now. NOTE that this is a positive value; if you wish to
				use this to find the bottom of the virtual space coordinates, you must
				negate it (as Virtual space runs from 0 to -Extent)

	Purpose:	Causes the entire subtree from this node to format themselves, and
				returns the 'height' of the entire list display, in millipoints

	SeeAlso:	SGDisplayRoot::InitFormatInfo; SGDisplayRoot::GetCachedListExtent

***********************************************************************************************/

INT32 SGDisplayRoot::CalculateListExtent(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo)
{
	if (GetChild() == NULL)	
		return(0);

	// Ask ourself to handle a FORMAT event. This recursively formats the tree, and returns
	// with FormatInfo indicating the state at the end of formatting (FormatInfo->LinePos
	// is at the end position of the list, and hence gives us the extent)
	HandleEvent(SGEVENT_FORMAT, FormatInfo, MiscInfo);
	return(ScrollExtent);
}



/***********************************************************************************************

>	virtual INT32 SGDisplayRoot::GetCachedListExtent(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/3/95

	Returns:	The currently cached value for the scroll extent

	Purpose:	Returns the 'height' of the entire list display, in millipoints.
				This return value is only valid after the tree has been formatted (including
				a previous call to CalculateListExtent), so long as the cached tree format
				is still valid. Use with care.

	SeeAlso:	SGDisplayRoot::CalculateListExtent

***********************************************************************************************/

INT32 SGDisplayRoot::GetCachedListExtent(void)
{
	return(ScrollExtent);
}



/***********************************************************************************************

>	virtual void SGDisplayRoot::SetScrollOffset(INT32 NewOffset, SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/95

	Inputs:		NewOffset - The new scroll offset.
				This is a number between 0 and LengthOfList, in MILLIPOINTs

				MiscInfo - the usual

	Purpose:	Changes the scroll offset, and if necessary, redraws the scroll bar

				In the base class, no scrollbar is supplied; this method does nothing

	SeeAlso:	SGDisplayRoot::GetScrollOffset; SGDisplayRootScroll::SetScrollOffset

***********************************************************************************************/

void SGDisplayRoot::SetScrollOffset(INT32 NewOffset, SGMiscInfo *MiscInfo)
{
	// Base class does nothing
}



/***********************************************************************************************

>	virtual INT32 SGDisplayRoot::GetScrollOffset(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/95

	Returns:	In the base class, always returns 0

	Returns:	The current Scroll offset. This is a positive MLLIPOINT value. Note that
				virtual coordinates go from 0 to -Extent, so the point of VirtualSpace which
				maps to the top left displayed pixel on screen is at (-ScrollOffset).

				In the base class, no scrollbar is supplied; this method does nothing

	SeeAlso:	SGDisplayRoot::SetScrollOffset; SGDisplayRootScroll::GetScrollOffset

***********************************************************************************************/

INT32 SGDisplayRoot::GetScrollOffset(void)
{
	// Base class does nothing - There is no scroll offset
	return(0);
}



/***********************************************************************************************

>	void SGDisplayRoot::RedrawScrollBar(SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/95

	Inputs:		MiscInfo - The normal MiscInfo for SGDisplayNode events

	Purpose:	Causes the scroll bar, if any, to be redrawn.
				In the base class, this method does nothing

	SeeAlso:	SGDisplayRootScroll::RedrawScrollBar

***********************************************************************************************/

void SGDisplayRoot::RedrawScrollBar(SGMiscInfo *MiscInfo)
{
	// The base class has no scrollbar, so does nothing
}



/***********************************************************************************************

>	virtual SGDisplayItem *SGDisplayRoot::FindNextSelectedItem(
												SGDisplayNode *CurrentItem = NULL,
												BOOL *SkipGroup = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/95

	Inputs:		CurrentItem - NULL to find the first selected item, else
				An SGDisplayItem (node) indicating the 'position' to search from for the next
				selected item
	
	Outputs:	if Non-NULL, SkipGroup will be filled in with TRUE if the scan has just
				skipped onto a new document/library group within the tree, or FALSE if the
				scan remains in the same group as the last selected node was in. If CurrentItem
				was NULL on entry, this returned value will be TRUE.

	Returns:	NULL (if no selection), or
				A pointer to the first selected leaf of this tree which occurs after
				the 'CurrentItem'. If CurrentItem is NULL, then the first selected
				leaf node is returned.

	Purpose:	Scanning the selection within a Display Tree - this will scan over
				multiple documents/libraries without blinking an eyelid.

				Use to scan through multiple-group selections, or to find the
				first selected item without having to know which document/library group
				it belongs to.

	Notes:		This method relies heavily on the flat (root/group/item) structure of
				current display trees - it will fail horribly if this simple layout is
				changed.

				Warning: It may be possible for groups AND items to be selected at one
				time. This essentially filters Groups out of the selection scan.

	SeeAlso:	SGDisplayNode::FindSubtree; SGDisplayGroup::FindNextSelectedItem;
				SGDisplayRoot::GetSelectedItemCount; SGDisplayGroup::GetSelectedItemCount;
				SGDisplayRoot::FindNextSelectedGroup

***********************************************************************************************/

SGDisplayItem *SGDisplayRoot::FindNextSelectedItem(SGDisplayNode *CurrentItem,
													BOOL *SkipGroup)
{
	BOOL ReturnSkip = (CurrentItem == NULL);
	SGDisplayItem *ReturnItem = NULL;

	// Find the current group node - my first child, or the the current item's parent.
	SGDisplayGroup *CurrentGroup;
	if (CurrentItem != NULL)
		CurrentGroup = (SGDisplayGroup *) CurrentItem->GetParent();
	else
		CurrentGroup = (SGDisplayGroup *) GetChild();

	ERROR3IF(CurrentGroup != NULL && !CurrentGroup->IsKindOf(CC_RUNTIME_CLASS(SGDisplayGroup)),
				"The gallery displaytree seems to have mutated- "
				"where have my lovely groups gone?");

	// Search each sibling group in turn until we find another selected leaf item
	while (CurrentGroup != NULL && ReturnItem == NULL)
	{
		ReturnItem = CurrentGroup->FindNextSelectedItem((SGDisplayItem *)CurrentItem);
		if (ReturnItem == NULL)
		{
			// No more selected items in this group - skip onto the next group, and flag
			// the fact that we had to skip to a different group
			ReturnSkip   = TRUE;
			CurrentGroup = (SGDisplayGroup *) CurrentGroup->GetNext();
			CurrentItem  = NULL;
		}
	}

	// Return the SkipGroup output
	if (SkipGroup != NULL)
		*SkipGroup = ReturnSkip;

	
	// And return NULL or the next selected node
	return(ReturnItem);
}



/***********************************************************************************************

>	virtual INT32 SGDisplayRoot::GetSelectedItemCount(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/95
	
	Returns:	The number of items in this display tree which are selected (may return 0)

	Purpose:	To determine how many eaf items of this tree are selected

	Notes:		This scans the selection (is not cached), so calling it multiple times
				should be avoided where possible

				This method relies heavily on the flat (root/group/item) structure of
				current display trees - it will fail horribly if this simple layout is
				changed.

	SeeAlso:	SGDisplayRoot::FindNextSelectedItem; SGDisplayGroup::GetSelectedItemCount;
				SGDisplayRoot::GetSelectedGroupCount

***********************************************************************************************/

INT32 SGDisplayRoot::GetSelectedItemCount(void)
{
	INT32 Count = 0;

	SGDisplayGroup *Ptr = (SGDisplayGroup *) GetChild();
	while (Ptr != NULL)
	{
		Count += Ptr->GetSelectedItemCount();
		Ptr = (SGDisplayGroup *) Ptr->GetNext();
	}

	return(Count);
}


/********************************************************************************************

>	virtual void SGDisplayRoot::SelectRange(SGDisplayNode *PrimeNode, SGDisplayNode *AnchorNode);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/95

	Inputs:		PrimeNode - The node which MUST be selected. May NOT be NULL.

				AnchorNode - The other node, specifying a range of sibling nodes to
				be selected. May be NULL, in which case only PrimeNode is selected

	Purpose:	Selects the PrimeNode, and if possible, all sibling nodes between it and
				the Anchor node. If Anchor == NULL or is not found, only PrimeNode is
				selected. Does not deselect any nodes - you should call SelectItems first
				to clear the seln.

	Note:		The code to do the selection is now sub-contracted out to SelectRangeGroups
				and SelectRangeItems, depending on the type of the two given nodes.

				If prime node is an item and the other a group, nothing will be selected...
				If prime node is an item and the null, the item will be selected...

********************************************************************************************/

void SGDisplayRoot::SelectRange(SGDisplayNode *PrimeNode, SGDisplayNode *AnchorNode)
{
	ERROR3IF(PrimeNode == NULL, "SGDisplayRoot::SelectRange - PrimeNode must be non-NULL");

	if (AnchorNode == NULL || PrimeNode->GetParent() != AnchorNode->GetParent())
	{
		// There is no anchor, or the prime/anchor nodes are not siblings, so
		// we cannot select a range - select PrimeNode, deselect all others
		ParentGallery->SelectItems(FALSE);
		ParentGallery->SelectGroups(FALSE);

		// Paint the list now to reduce the chance of large redraws if the
		// old/new selections are far apart.
		ParentGallery->PaintListNow();

		PrimeNode->SetSelected(TRUE);
		ParentGallery->PaintListNow();
	}
	else if (PrimeNode->IS_KIND_OF(SGDisplayItem) && (AnchorNode == NULL || AnchorNode->IS_KIND_OF(SGDisplayItem)))
	{
		// We are selecting a range of items so deselect all groups first
		ParentGallery->SelectGroups(FALSE);	// Deselect all groups
		SelectRangeItems((SGDisplayItem *)PrimeNode, (SGDisplayItem *)AnchorNode);
	}
	else if (PrimeNode->IS_KIND_OF(SGDisplayGroup) && (AnchorNode == NULL || AnchorNode->IS_KIND_OF(SGDisplayGroup)))
	{
		// We are selecting a range of groups so deselect all items first
		ParentGallery->SelectItems(FALSE);	// Deselect all items
		SelectRangeGroups((SGDisplayGroup *)PrimeNode, (SGDisplayGroup *)AnchorNode);
	}

	// And ensure the gallery updates itself appropriately
	ParentGallery->SelectionHasChanged();
}



/***********************************************************************************************

>	virtual SGDisplayGroup *SGDisplayRoot::FindNextSelectedGroup(
											SGDisplayNode *CurrentGroup = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/95

	Inputs:		CurrentGroup - NULL to find the first selected Group, else
				An SGDisplayGroup indicating the 'position' to search from for the next
				selected Group

	Returns:	NULL (if no selection), or
				A pointer to the first selected leaf of this tree which occurs after
				the 'CurrentGroup'. If CurrentGroup is NULL, then the first selected
				leaf node is returned.

	Purpose:	Scanning the group-selection within a Display Tree.

	Notes:		This method relies heavily on the flat (root/group/Group) structure of
				current display trees - it will fail horribly if this simple layout is
				changed.

				Warning: It may be possible for groups AND items to be selected at one
				time. This essentially filters Items out of the selection scan.

	SeeAlso:	SGDisplayRoot::FindNextSelectedItem; SGDisplayRoot::GetSelectedGroupCount

***********************************************************************************************/

SGDisplayGroup *SGDisplayRoot::FindNextSelectedGroup(SGDisplayNode *CurrentGroup)
{
	// Find the current group node - my first child, or the the current Group's parent.
	if (CurrentGroup != NULL)
		CurrentGroup = ((SGDisplayGroup *) CurrentGroup)->GetNext();
	else
		CurrentGroup = (SGDisplayGroup *) GetChild();

	ERROR3IF(CurrentGroup != NULL && !CurrentGroup->IsKindOf(CC_RUNTIME_CLASS(SGDisplayGroup)),
				"The gallery displaytree seems to have mutated- "
				"where have my lovely groups gone?");

	// Search each sibling group in turn until we find another selected leaf Group
	while (CurrentGroup != NULL)
	{
		if (CurrentGroup->IsSelected())		// Found a selected group - return it
			return((SGDisplayGroup *)CurrentGroup);

		CurrentGroup = ((SGDisplayGroup *) CurrentGroup)->GetNext();
	}

	// No more selected groups
	return(NULL);
}



/***********************************************************************************************

>	virtual INT32 SGDisplayRoot::GetSelectedGroupCount(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/95
	
	Returns:	The number of Groups in this display tree which are selected (may return 0)

	Purpose:	To determine how many eaf Groups of this tree are selected

	Notes:		This scans the selection (is not cached), so calling it multiple times
				should be avoided where possible

				This method relies heavily on the flat (root/group/Group) structure of
				current display trees - it will fail horribly if this simple layout is
				changed.

	SeeAlso:	SGDisplayRoot::FindNextSelectedGroup; SGDisplayGroup::GetSelectedGroupCount

***********************************************************************************************/

INT32 SGDisplayRoot::GetSelectedGroupCount(void)
{
	INT32 Count = 0;

	SGDisplayGroup *Ptr = (SGDisplayGroup *) GetChild();
	while (Ptr != NULL)
	{
		if (Ptr->IsSelected())
			Count++;

		Ptr = (SGDisplayGroup *) Ptr->GetNext();
	}

	return(Count);
}





/***********************************************************************************************

>	static SGDisplayNode *SGDisplayRoot::FindNextItemInTree(SGDisplayNode *StartItem)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/04/96
	
	Inputs:		StartItem	- Item from which to start looking for next item
	
	Returns:	The next item, or NULL if no further items found

	Purpose:	Given an item in the tree, find the next one. 

	Notes:		This function will skip virtualised groups, so if you need to do something
				globally, don't call this...

	SeeAlso:	SGDisplayRoot::GetSelectedItemCount;

***********************************************************************************************/

SGDisplayNode *SGDisplayRoot::FindNextItemInTree(SGDisplayNode *StartItem)
{
	ERROR3IF(StartItem == NULL, "SGDisplayRoot::FindNextItemInTree given a NULL StartItem - bad");
	if(StartItem == NULL) return NULL;

	SGDisplayNode *Item = StartItem;

	// If this is the last item in the group, skip to the next group
	if (Item->GetNext() == NULL)
	{
		ERROR3IF(Item->GetParent() == NULL, "Tree linkage corruption");

		// Check if we're the last group
		if (Item->GetParent()->GetNext() != NULL)
		{
			SGDisplayNode *TmpItem = Item->GetParent()->GetNext();

			// Jump over any virtualised groups
			while(TmpItem != NULL && TmpItem->GetChild() == NULL)
				TmpItem = TmpItem->GetNext();

			// Check if gone over last group
			if(TmpItem == NULL)
				Item = NULL;
			else
				Item = TmpItem->GetChild();
		}
		else
			Item = NULL;
	}
	else
		Item = Item->GetNext();		// Get next sibling item

	return Item;
}











/***********************************************************************************************

>	SGDisplayRootScroll::SGDisplayRootScroll(SuperGallery *ParentGal)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/94

	Inputs:		ParentGal - A pointer to the parent gallery of this node/tree

	Purpose:	SGDisplayRootScroll constructor (makes root nodes invisible by default)

	SeeAlso:	SGDisplayNode::SGDisplayNode

***********************************************************************************************/

SGDisplayRootScroll::SGDisplayRootScroll(SuperGallery *ParentGal) : SGDisplayRoot(ParentGal)
{
	ScrollOffset = 0;
	IndentedButton = IBUTTON_NONE;

	// Read the scroll bar width - use half of the normal scroll bar width (and is an even number)
PORTNOTE("galleries", "hard wired scrollbar width");
	ScrollBarWidth = 12;
//	ScrollBarWidth = (CScroller::GetScrollerSize() / 2) & 0xFE;
	
	if (ScrollBarWidth < 10)	// But make sure it lies within a sensible range
		ScrollBarWidth = 10;
	if (ScrollBarWidth > 100)
		ScrollBarWidth = 100;
}



/***********************************************************************************************

>	virtual void SGDisplayRootScroll::SetScrollOffset(INT32 NewOffset, SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/95

	Inputs:		NewOffset - The new scroll offset.
				This is a number between 0 and LengthOfList, in MILLIPOINTs

				MiscInfo - the usual

	Purpose:	Changes the scroll offset, and if necessary, redraws the scroll bar

	Notes:		To ensure that the current scroll offset is within bounds, call
				SetScrollOffset(GetScrollOffset(), MiscInfo);

	SeeAlso:	SGDisplayRootScroll::GetScrollOffset; SGDisplayRoot::SetScrollOffset

***********************************************************************************************/

void SGDisplayRootScroll::SetScrollOffset(INT32 NewOffset, SGMiscInfo *MiscInfo)
{
	// Ensure we don't scroll out of bounds
	if (NewOffset > ScrollExtent - MiscInfo->WindowHeight)
		NewOffset = ScrollExtent - MiscInfo->WindowHeight;

	if (NewOffset < 0)
		NewOffset = 0;

	// Make sure the scroll offset is aligned tidily with the output pixel grid
	GridLock(MiscInfo, NewOffset);

	// And if it has changed, set the new scroll offset, and scroll the window
	if (NewOffset != ScrollOffset)
	{
		DocCoord ScrollBy(0, NewOffset - ScrollOffset);

		if (!GetParentGallery()->AreYouRedrawingNow())
		{
			// A normal scroll - we are not inside the redraw code, so we can safely
			// assume that the screen display is up to date (and therefore can be scrolled
			// with 'ScrollArea') and that it is safe to invoke redraw.

			DocRect ScrollRect(0, 0, MiscInfo->MaxWidth - DevicePixels(MiscInfo, ScrollBarWidth),
								MiscInfo->WindowHeight);

			// Scroll the window. During the scroll, we make sure that the drag manager
			// has removed any eor or solid drag blobs from screen, so we don't copy
			// them as part of the scroll!
			DragManagerOp::RedrawStarting(GetParentGallery()->WindowID,
											GetParentGallery()->GetListGadgetID());

				// And scroll
				GetParentGallery()->ScrollArea(&ScrollRect, &ScrollBy);

				// Set the new scroll offset, using the gridlocked value returned from ScrollArea
				ScrollOffset += ScrollBy.y;

				// Redraw immediately. If we scroll only a little bit, and do not redraw
				// immediately, we get a small horizontal strip, plus the vertical scroll bar
				// region, and so the redraw bounds is the entire window, so we redraw heaps
				// more than actually changes, thus making scrolling dead slow.
				GetParentGallery()->PaintListNow();

				// And redraw the proportional scrollbar
				DocRect ScrollBarRect(MiscInfo->MaxWidth - DevicePixels(MiscInfo, ScrollBarWidth),
										-ScrollExtent,
										MiscInfo->MaxWidth, 0);
				GetParentGallery()->ForceRedrawOfArea(&ScrollBarRect);

				// And finally, force another immediate repaint of the list gadget
				GetParentGallery()->PaintListNow();

			DragManagerOp::RedrawFinished();
		}
		else
		{
			// Awooga! We have been forced to fix the scroll offset during a redraw (probably
			// because the window size has changed). We thus change the scroll offset, but
			// do not try to redraw or anything - hopefully the pending redraw will handle
			// everything.

			// Set the new scroll offset, using the gridlocked value returned from ScrollArea
			ScrollOffset += ScrollBy.y;
		}
	}
}



/***********************************************************************************************

>	INT32 SGDisplayRootScroll::GetScrollOffset(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/95

	Returns:	The current Scroll offset. This is a positive MLLIPOINT value. Note that
				virtual coordinates go from 0 to -Extent, so the point of VirtualSpace which
				maps to the top left displayed pixel on screen is at (-ScrollOffset).

	Purpose:	Returns the current scroll offset for the gallery display list represented in
				the tree.

	SeeAlso:	SGDisplayRootScroll::SetScrollOffset; SGDisplayRoot::GetScrollOffset

***********************************************************************************************/

INT32 SGDisplayRootScroll::GetScrollOffset(void)
{
	return(ScrollOffset);
}



/***********************************************************************************************

>	virtual BOOL SGDisplayRootScroll::CalculateScrollRects(SGMiscInfo *MiscInfo,
										BOOL Translate,
										DocRect *UpButton, DocRect *DownButton,
										DocRect *Sausage, DocRect *PageUp = NULL,
										DocRect *PageDown = NULL, DocRect *ScrollRect = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/2/95

	Inputs:		MiscInfo - The misc info, as passed to all event handlers
				Translate - TRUE to translate the returned rectangles to give Display-list
				coordinates, FALSE to give the coordinates as offsets within the visible
				display area on screen.

	Outputs:	The rects - Rectangles in millipoint offsets within the list gadget
				of the various bits of the scrollbar: up/down buttons, plus the
				sausage, and the page-up/down areas above/below the sausage. ScrollRect
				is the all-inclusive rectangle containing all the others.

				PageUp, PageDown, and ScrollRect may be NULL. All others may not be NULL

	Returns:	TRUE if the scroll rects are valid, FALSE if there is not enough room
				for a scrollbar. If FALSE is returned, none of the outputs are valid, though
				some of them may have been changed/corrupted.

	Purpose:	Determines where in the gadget the scroll bar rectangles are. Used by
				the SGScrollDragTarget in order to determine if the mouse pointer still
				lies over a given rectangle.

***********************************************************************************************/

BOOL SGDisplayRootScroll::CalculateScrollRects(SGMiscInfo *MiscInfo,
												BOOL Translate,
												DocRect *UpButton, DocRect *DownButton,
												DocRect *Sausage, DocRect *PageUp,
												DocRect *PageDown, DocRect *pScrollRect)
{
	ERROR3IF(MiscInfo == NULL || UpButton == NULL || DownButton == NULL || Sausage == NULL,
				"SGDispayRootScroll::CalculateScrollRects - NULL params are illegal");

	// Calculate the strip into which the scrollbar must fit...
	INT32 ScrollWidth = DevicePixels(MiscInfo, ScrollBarWidth);

#ifdef _DEBUG
	if (((MiscInfo->MaxWidth - ScrollWidth) > MiscInfo->MaxWidth) || (0 > MiscInfo->WindowHeight))
	{
		TRACE( _T("Bad MiscInfo in SGDispayRootScroll::CalculateScrollRects\n"));
		TRACE( _T("    MaxWidth     = %d\n"), MiscInfo->MaxWidth);
		TRACE( _T("    WindowHeight = %d\n"), MiscInfo->WindowHeight);
		TRACE( _T("    DisplayMode  = %d\n"), MiscInfo->DisplayMode);
		TRACE( _T("    PixelSize    = %d\n"), MiscInfo->PixelSize);
		TRACE( _T("ScrollBarWidth   = %d\n"), ScrollBarWidth);
		TRACE( _T("ScrollWidth      = %d\n"), ScrollWidth);
	}
#endif

	DocRect ScrollRect(MiscInfo->MaxWidth - ScrollWidth, 0,
						MiscInfo->MaxWidth, MiscInfo->WindowHeight);

	// Convert, if necessary, the position, into display-list offsets
	if (Translate)
		ScrollRect.Translate(0, -(ScrollOffset + MiscInfo->WindowHeight));
	
	GridLockRect(MiscInfo, &ScrollRect);

	if (!ScrollRect.IsValid())
		return(FALSE);

	// Calculate the squares for the up/down scroll buttons
	*UpButton =ScrollRect;
	UpButton->lo.x += DevicePixels(MiscInfo, 1);
	UpButton->lo.y = (UpButton->hi.y - ScrollWidth) + DevicePixels(MiscInfo, 1);

	*DownButton = *UpButton;
	DownButton->Translate(0, -(MiscInfo->WindowHeight -
								(ScrollWidth - DevicePixels(MiscInfo, 1)) ));

	// And make the scroll rect include only the central strip
	ScrollRect.Inflate(0, -(ScrollWidth - DevicePixels(MiscInfo, 1)));

	BOOL Result = CalculateSausageRect(MiscInfo, &ScrollRect, Sausage);

	if (PageUp != NULL)
	{
		*PageUp = ScrollRect;
		PageUp->lo.y = Sausage->hi.y;
	}

	if (PageDown != NULL)
	{
		*PageDown = ScrollRect;
		PageDown->hi.y = Sausage->lo.y;
	}

	if (pScrollRect != NULL)
		*pScrollRect = ScrollRect;

	return(Result);
}



/***********************************************************************************************

>	BOOL SGDisplayRootScroll::CalculateSausageRect(SGMiscInfo *MiscInfo,
												DocRect *ScrollBarRect, DocRect *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/95

	Inputs:		MiscInfo - The misc info, as passed to all event handlers
				ScrollBarRect - The bounds in which the scroll sausage can slide

	Outputs:	Result - Either unchanged (return == FALSE) or the rectangle to use
				for the scroll sausage (return == TRUE)

	Returns:	TRUE if it succeeds; FALSE if it did nothing
				(FALSE is returned when there is no room for a scroll sausage etc)

	Purpose:	Determines where within the given scroller the scroll 'sausage' (thumb)
				should be positioned to give the correct display

	SeeAlso:	SGDisplayRootScroll::SetScrollOffset; SGDisplayRoot::GetScrollOffset

***********************************************************************************************/

BOOL SGDisplayRootScroll::CalculateSausageRect(SGMiscInfo *MiscInfo,
												DocRect *ScrollBarRect, DocRect *Result)
{
	INT32 PixelSize = MiscInfo->PixelSize;
	
	// If the scroll bar hasn't got enough room to exist, it does not appear
	if (ScrollBarRect->hi.y - ScrollBarRect->lo.y < 5*PixelSize)
		return(FALSE);

	*Result = *ScrollBarRect;
	Result->Inflate(-PixelSize);	// Scroll sausage sits just inside the scroll rect
	Result->hi.x += PixelSize;		// But the window border already does the 1-pixel black
									// line on the right, so we actually touch the scroll
									// rect on this edge.
	GridLockRect(MiscInfo, Result);

	// If the sausage does not entirely fill the available space (we can scroll), then 
	// scale and translate the sausage rect appropriately
	if (ScrollExtent > MiscInfo->WindowHeight)
	{
		// It is necessary to calculate the center of the sausage and grow it outwards
		// from that, so that we can ensure it never gets shorter that being a small square.

		// Calculate the size of the proportional scroll sausage
		INT32 SausageHeight = (INT32) (((double)Result->Height()) *
								(((double)MiscInfo->WindowHeight) / (double)ScrollExtent));

		// Don't allow sausage to become smaller than a square
		if (SausageHeight < (ScrollBarWidth-2) * PixelSize)
			SausageHeight = (ScrollBarWidth-2) * PixelSize;

		// BarTravel is the range in which the *center* of the scroll sausage can move -
		// this is limited by the overall bar height and the size of the scroll sausage
		INT32 BarTravel = Result->Height() - SausageHeight;

		// Range in which the ScrollOffset is limited
		INT32 ScrollTravel = ScrollExtent - MiscInfo->WindowHeight;

		// From which we calculate how far down the bar the sausage center appears
		double MidOffset =  ((double)BarTravel *
							(((double)ScrollOffset) / (double)ScrollTravel));

		// Convert this into a position within the scroll rectangle (subtract from the top,
		// taking into account the gap at the top of the travel to contain half the sausage)
		MidOffset = (double) Result->hi.y - (((double)SausageHeight)/2 + MidOffset);

		// Finally, shift and resize the scroll rectangle to generate the sausage rect
		Result->hi.y = (INT32) (-0.5 + MidOffset + SausageHeight/2);
		Result->lo.y = (INT32) (-0.5 + MidOffset - SausageHeight/2);
		GridLockRect(MiscInfo, Result);
	}

	return(TRUE);
}



/***********************************************************************************************

>	virtual BOOL SGDisplayRootScroll::HandleEvent(SGEventType EventType, void *EventInfo,
											SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94

	Inputs:		See SGDisplayNode::HandleEvent

	Returns:	TRUE if the event was handled successfully
				FALSE if it was not

	Purpose:	Handles a SuperGallery DisplayTree event
				For this Scrollbar-providing version of the DisplayRoot node, adds a
				scrollbar at the right side of the window, and updates the event info
				appropriately to make the rest of the tree format to the left of that
				scroll bar. The scrollbar is 100% automated - any attempt to change the
				tree will result in the scrollbar updating as appropriate.

	Notes:		This overrides the pure virtual SGDisplayNode::HandleEvent method

				A node need not handle a specific event - if it does not handle it, it
				should return FALSE.

				Redraw and Formatting handlers should never return TRUE, as this will
				prevent the event from continuing through the tree.

				Non-leaf-nodes must call SGDisplayNode::GiveEventToMyChildren in order
				to pass the event down the tree.

	SeeAlso:	SGDisplayNode::HandleEvent; SGDisplayGroup::HandleEvent;
				SGDisplayItem::HandleEvent;	SGDisplayRoot::InitFormatInfo;
				SGDisplayRoot::CalculateListExtent;
				SGDisplayNode::GiveEventToMyChildren

***********************************************************************************************/

BOOL SGDisplayRootScroll::HandleEvent(SGEventType EventType, void *EventInfo,
										SGMiscInfo *MiscInfo)
{
	// THUMBMSG and BGFLUSH can be broadcast with MiscInfo == NULL. We ignore these msgs
	// so we just pass them on to the base class immediately.
	if (MiscInfo == NULL)
		return(SGDisplayRoot::HandleEvent(EventType, EventInfo, MiscInfo));

	SGMiscInfo NewMiscInfo(*MiscInfo);	// Ensure we don't corrupt the one passed in

	// Calculate the strip into which the scrollbar must fit...
	INT32 ScrollWidth = DevicePixels(MiscInfo, ScrollBarWidth);

	DocRect ScrollRect;
	DocRect UpButton;
	DocRect DownButton;
	DocRect PageUp;
	DocRect PageDown;
	DocRect SausageRect;
	BOOL HasScrollBar = CalculateScrollRects(MiscInfo, TRUE,
											 &UpButton, &DownButton, &SausageRect,
											 &PageUp, &PageDown, &ScrollRect);

	// Convince the DisplayTree the scroll area is outside the region they can use
	// We add 1 pixel to put a 1-pixel white gap between the scrollbar and the right end of all items
	NewMiscInfo.MaxWidth -= ScrollWidth;

	// Remember what the scroll extent used to be. If formatting causes a chnage to the extent,
	// we'll redraw ourself to make sure that our display is up to date.
	INT32 OldScrollExtent = ScrollExtent;

	switch (EventType)
	{
		case SGEVENT_FORMAT:
			if (HasScrollBar)
			{
				SGFormatInfo *FormatInfo = GetFormatInfo(EventType, EventInfo);

				// Reset AvailableWidth so the first line doesn't overwrite the scrollbar!
				FormatInfo->AvailableWidth = NewMiscInfo.MaxWidth;
			}
			break;


		case SGEVENT_REDRAW:
			// Does nothing here - redraw is now delayed until after we have called all our
			// children, so that rampant clipping problems in the font gallery do not cause
			// the scrollbar to be left in an overwritten state.
			break;


		case SGEVENT_MOUSECLICK:
			if (HasScrollBar && ScrollExtent > 0)
			{
				SGMouseInfo *MouseInfo = GetMouseInfo(EventType, EventInfo);
				SGDragType DragType = SGDRAG_NONE;

				if (ScrollRect.ContainsCoord(MouseInfo->Position))
				{
					if (SausageRect.ContainsCoord(MouseInfo->Position))
						DragType = SGDRAG_SAUSAGE;
					else if (SausageRect.hi.y < MouseInfo->Position.y)
						DragType = SGDRAG_PAGEUP;
					else if (SausageRect.lo.y > MouseInfo->Position.y)
						DragType = SGDRAG_PAGEDOWN;
					else
						return(TRUE);
				}

				if (DragType == SGDRAG_NONE && UpButton.ContainsCoord(MouseInfo->Position))
					DragType = SGDRAG_SCROLLUP;

				if (DragType == SGDRAG_NONE && DownButton.ContainsCoord(MouseInfo->Position))
					DragType = SGDRAG_SCROLLDOWN;

				if (DragType != SGDRAG_NONE)
				{
					SGScrollDragInfo *DragInfo =
						new SGScrollDragInfo(this, DragType, MiscInfo,
											SausageRect.hi.y - MouseInfo->Position.y,
											MouseInfo->MenuClick);

					if (DragInfo != NULL)
						DragManagerOp::StartDrag(DragInfo, GetListWindow());
				}
			}
			break;


		case SGEVENT_DRAGSTARTED:
			{
				DragMessage *Msg = GetDragInfo(EventType, EventInfo);

				// Is it a Drag Started message?
				if (Msg->State == DragMessage::DRAGSTARTED)
				{
					// Is it a SuperGallery scroll drag?
					// And is it a scroll drag for THIS display tree?
					if (Msg->pInfo->IsKindOf(CC_RUNTIME_CLASS(SGScrollDragInfo)) &&
						((SGScrollDragInfo *)(Msg->pInfo))->GetDragRootNode() == this)
					{
						SuperGallery *ParentGallery = GetParentGallery();						

						// AMB comment out assignment to variable as variable is unused. Can this be right?
						/* SGScrollDragTarget *DragTarget = */
								new SGScrollDragTarget(ParentGallery,
														ParentGallery->GetListGadgetID());
					}
				}
			}
			break;

			default:
				break;
	}

	// And pass the event on to the tree via our base class handler
	BOOL Result = SGDisplayRoot::HandleEvent(EventType, EventInfo,
									(HasScrollBar) ? &NewMiscInfo : MiscInfo);

	if (EventType == SGEVENT_FORMAT && ScrollExtent != OldScrollExtent && HasScrollBar)
	{
		// The scroll extent has changed as a result of this reformat.
		// We'd better invalidate the scrollbar rectangle to make sure it's up to date
		SuperGallery *ParentGal = GetParentGallery();


		// We modify the scrollbar rectangle before invalidating it - if the extent has changed,
		// the scrollbar may have been scrolled off the visible area!!
		// The easiest way to redraw it is to just redraw everything down the right side of the extent
		ScrollRect.hi.y = 0;
		ScrollRect.lo.y = -(ScrollExtent + MiscInfo->WindowHeight);
		ParentGal->ForceRedrawOfArea(&ScrollRect);

		// If the extent has got smaller, ensure the invalid redraw region includes
		// everything to the very bottom of the list
		SGFormatInfo *FormatInfo = GetFormatInfo(EventType, EventInfo);
		if (FormatInfo->AccumulateBounds)
			FormatInfo->InvalidBounds.lo.y = min(-OldScrollExtent, -ScrollExtent);
	}

	if (EventType == SGEVENT_REDRAW && HasScrollBar)		// If room for a scrollbar, draw it
	{
		// Redraw is now left until after all children have been redrawn, as the font gallery
		// has a clipping problem where it was overwriting the scrollbar area. By drawing last
		// we at least end up in the correct screen display state.
		INT32 PixelSize = DevicePixels(MiscInfo, 1);

		SGRedrawInfo *RedrawInfo = GetRedrawInfo(EventType, EventInfo);
		RenderRegion *Renderer = RedrawInfo->Renderer;

		StartRendering(RedrawInfo, MiscInfo);

		// Note that we call the base class, so we can rely on it to clear the
		// background of the area outside the scrollbar

		// If we don't need to redraw anything, then we, er... don't redraw anything
		if (ScrollRect.IsIntersectedWith(RedrawInfo->Bounds) ||
			UpButton.IsIntersectedWith(RedrawInfo->Bounds)	 ||
			DownButton.IsIntersectedWith(RedrawInfo->Bounds))
		{
			DialogColourInfo RedrawColours;

			// Fill the page-up and down rectangles with background colour.
			// We move the left and top/bottom edges by 1 pixel to accomodate
			// the black border lines that we draw around these areas.
			Renderer->SetLineWidth(0);
			DocColour trans(COLOUR_TRANS);
			Renderer->SetLineColour(trans);
			Renderer->SetFillColour(RedrawColours.ButtonFace());

			PageUp.lo.x += PixelSize;
			PageUp.hi.y -= PixelSize;
			if (PageUp.IsValid())
				Renderer->DrawRect(&PageUp);

			PageDown.lo.x += PixelSize;
			PageDown.lo.y += PixelSize;
			if (PageDown.IsValid())
				Renderer->DrawRect(&PageDown);

			// Draw a 1-pixel black border around the region
			// (down left edge and between scroll bar and arrows)
			// We draw these lines as filled rectangles to avoid the fact that lines
			// draw in different blimming places in different render regions
			DocColour black(COLOUR_BLACK);
			Renderer->SetFillColour(black);

			Renderer->DrawPixelLine(DocCoord(ScrollRect.lo.x, DownButton.lo.y),
								DocCoord(ScrollRect.lo.x, UpButton.hi.y));
			Renderer->DrawPixelLine(DocCoord(ScrollRect.lo.x, ScrollRect.hi.y - PixelSize),
								DocCoord(ScrollRect.hi.x, ScrollRect.hi.y - PixelSize));
			Renderer->DrawPixelLine(DocCoord(ScrollRect.lo.x, ScrollRect.lo.y),
								DocCoord(ScrollRect.hi.x, ScrollRect.lo.y));

			// Draw the scroll sausage
			DrawPlinth(RedrawInfo, MiscInfo, &RedrawColours, &SausageRect, FALSE);

			Renderer->DrawPixelLine(DocCoord(ScrollRect.lo.x, SausageRect.hi.y),
								DocCoord(ScrollRect.hi.x, SausageRect.hi.y));
			Renderer->DrawPixelLine(DocCoord(ScrollRect.lo.x, SausageRect.lo.y - PixelSize),
								DocCoord(ScrollRect.hi.x, SausageRect.lo.y - PixelSize));

			// Draw the scroll-up and scroll-down buttons
			DrawPlinth(RedrawInfo, MiscInfo, &RedrawColours,
							&UpButton, (IndentedButton == IBUTTON_UP),
							_R(IDB_GALLERY_SCROLLUP));

			DrawPlinth(RedrawInfo, MiscInfo, &RedrawColours,
							&DownButton, (IndentedButton == IBUTTON_DOWN),
							_R(IDB_GALLERY_SCROLLDOWN));
		}

		StopRendering(RedrawInfo, MiscInfo);
	}

	return(Result);
}



/***********************************************************************************************

>	void SGDisplayRootScroll::RedrawScrollBar(SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/95

	Inputs:		MiscInfo - The normal MiscInfo for SGDisplayNode events

	Purpose:	Causes the scroll bar, if any, to be redrawn, ensuring that it shows the
				correct information for the current format of the display list

	SeeAlso:	SGDisplayRootScroll::RedrawScrollBar

***********************************************************************************************/

void SGDisplayRootScroll::RedrawScrollBar(SGMiscInfo *MiscInfo)
{
	// Calculate the strip into which the scrollbar must fit...
	INT32 ScrollWidth = DevicePixels(MiscInfo, ScrollBarWidth);

	DocRect ScrollRect(MiscInfo->MaxWidth - ScrollWidth, -MiscInfo->WindowHeight,
						MiscInfo->MaxWidth, 0);
	ScrollRect.Translate(0, -ScrollOffset);
	GridLockRect(MiscInfo, &ScrollRect);

	GetParentGallery()->ForceRedrawOfArea(&ScrollRect);
}














/***********************************************************************************************

>	SGDisplayGroup::SGDisplayGroup()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/94

	Purpose:	SGDisplayGroup constructor. NOTE that to initialise the parent gallery,
				document, and library pointers, you should use the other constructor.

***********************************************************************************************/

SGDisplayGroup::SGDisplayGroup()
{
	Child = NULL;

	ParentGallery  = NULL;
	ParentDocument = NULL;
	ParentLibrary  = NULL;

	ChildArea.MakeEmpty();

	TRACE( _T("Warning: Using default constructor for SGDisplayGroup is silly\n"));
}



/***********************************************************************************************

>	SGDisplayGroup::SGDisplayGroup(SuperGallery *ParentGal,
									Document *ParentDoc = NULL, Library *ParentLib = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/94

	Inputs:		ParentGal - points to the SuperGallery object which 'owns' this node
				ParentDoc - NULL, or a pointer to the document this group references
				ParentLib - NULL, or a pointer to the library this group references

	Purpose:	SGDisplayGroup constructor. Initialises the Group's parent pointers to
				point at its parent(s). Note that generally speaking, one of ParentDoc,
				ParentLib will be NULL, and the other will be non-NULL. (This is not the
				case, however, for the Font gallery!)

***********************************************************************************************/

SGDisplayGroup::SGDisplayGroup(SuperGallery *ParentGal,
								Document *ParentDoc, Library *ParentLib)
{
	// Sanity checks
	ERROR3IF(ParentGal == NULL, "SGDisplayGroup needs a parent gallery!");
	ERROR3IF(ParentDoc != NULL && ParentLib != NULL,
				"SGDisplayGroup cannot have BOTH a doc and lib for parents!");

	Child = NULL;

	ParentGallery  = ParentGal;
	ParentDocument = ParentDoc;
	ParentLibrary  = ParentLib;

	if (ParentLibrary != NULL)		// Library groups (which come off disc) default to folded
		Flags.Folded = TRUE;

	ChildArea.MakeEmpty();

	ReadGroupTitle();
}



/***********************************************************************************************

>	virtual SGDisplayNode *SGDisplayGroup::GetChild(void) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94 (Made virtual 13/5/95)
	Returns:	A pointer to the first child of this SGDisplayNode object, or NULL

	Purpose:	Finds the child of this DisplayTree Node.
				Returns NULL if you have reached the boundary of the tree

	SeeAlso:	SuperGallery; SGDisplayNode::GetParent;
				SGDisplayNode::GetNext; SGDisplayNode::GetPrevious

***********************************************************************************************/

SGDisplayNode *SGDisplayGroup::GetChild(void) const
{
	return(Child);
}



/***********************************************************************************************

>	void SGDisplayGroup::SetChild(SGDisplayNode *NewChild)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/5/95

	Inputs:		A pointer to the new first-child of this SGDisplayNode object

	Purpose:	Sets the child of this DisplayTree Node. Overrides to base class method
				to allow the child to be set.

	SeeAlso:	SuperGallery; SGDisplayNode::GetParent;
				SGDisplayNode::GetNext; SGDisplayNode::GetPrevious

***********************************************************************************************/

void SGDisplayGroup::SetChild(SGDisplayNode *NewChild)
{
	Child = NewChild;
}



/***********************************************************************************************

>	virtual BOOL SGDisplayGroup::SetFoldedState(BOOL NewState, BOOL ForceRedraw = TRUE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/95

	Inputs:		NewState - TRUE to fold, FALSE to unfold

				ForceRedraw - TRUE to force a reformat-and-redraw of the group, or FALSE
				to mark the tree format invalid but not bother actually reformatting and
				redrawing just yet (usually only set to FALSE when constructing the
				DisplayTree before the gallery window is opened, for AutoFolding)

	Returns:	TRUE if the new state is different from the old state (if anything has changed)

	Purpose:	Folds or unfolds a display group

	Notes:		If changed, forces an immediate reformat and redraw of the Display list

***********************************************************************************************/

BOOL SGDisplayGroup::SetFoldedState(BOOL NewState, BOOL ForceRedraw)
{
	if ((BOOL)Flags.Folded != NewState)
	{
		SuperGallery *ParentGal = GetParentGallery();

		// Devirtualise the group. If there were problems, keep it folded...
		if (!NewState && IsVirtualised())
		{
			// For library galleries we need to set the Quiet button status ready to un-supress errors
			ParentGal->SetQuietStatus(FALSE);

			// On older machines this can take a couple of seconds, so we need some feedback...
			Progress ProgMsg(_R(IDS_GALLERY_PREPARE_FOR_UNFOLD), -1, FALSE);

			if(!DeVirtualise())	
				return FALSE;
		}

		Flags.Folded = NewState;
		ChildArea.MakeEmpty();

		ParentGal->InvalidateCachedFormat();

		if (ForceRedraw)
			ParentGal->ReformatAndRedrawIfNecessary();

		return(TRUE);
	}

	return(FALSE);
}



/***********************************************************************************************

>	virtual void SGDisplayGroup::ForceRedrawOfMyselfAndChildren(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95

	Purpose:	Uses the cached FormatRect to force-redraw the appropriate part of the 
				SuperGallery display window to cause myself to be redrawn. Also redraws
				the entire area that my children will occupy, thus redrawing this entire
				category.

	SeeAlso:	SuperGallery::RedrawArea

***********************************************************************************************/

void SGDisplayGroup::ForceRedrawOfMyselfAndChildren(void)
{
	SuperGallery *ParentGallery = GetParentGallery();
	if (ParentGallery != NULL)
	{
		DocRect WholeRect(FormatRect);				// My own rectangle

		if (!Flags.Folded && GetChild() != NULL && !ChildArea.IsEmpty())
		{
			// Union with child bounds rect, if any
			WholeRect.Union(ChildArea);
		}

		ParentGallery->ForceRedrawOfArea(&WholeRect);
	}
}



/***********************************************************************************************

>	virtual SuperGallery *SGDisplayGroup::GetParentGallery(void) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/94

	Returns:	The parent SuperGallery, or NULL

	Purpose:	Returns the SuperGallery which 'owns' this node (and its subtree)

	SeeAlso:	SGDisplayGroup::GetParentLibrary; SGDisplayGroup::GetParentDocument;
				SGDisplayNode::GetParentGallery

***********************************************************************************************/

SuperGallery *SGDisplayGroup::GetParentGallery(void) const
{
	return(ParentGallery);
}



/***********************************************************************************************

>	virtual SGDisplayItem *SGDisplayGroup::FindNextSelectedItem(
												SGDisplayItem *CurrentItem = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/95

	Inputs:		CurrentItem - NULL to find the first selected item, else
				An item indicating the 'position' to search from for the next selected item
	
	Returns:	NULL (if no selection), or
				A pointer to the first selected child of this node which occurs after
				the 'CurrentItem'. If CurrentItem is NULL, then the first selected
				child node is returned.

	Purpose:	Scanning the selection within a Group (document/library).
				Use after finding the subtree for a given doc/lib with FindSubtree, to
				scan through items in the selection.

	SeeAlso:	SGDisplayNode::FindSubtree; SGDisplayGroup::GetSelectedItemCount()

***********************************************************************************************/

SGDisplayItem *SGDisplayGroup::FindNextSelectedItem(SGDisplayItem *CurrentItem)
{
	if (CurrentItem == NULL)
		CurrentItem = (SGDisplayItem *) GetChild();
	else
	{
		if (CurrentItem->GetParent() != this)	// Sanity check
		{
			ERROR3("Asking for next selected when previous was not in this group! Starting from first child");
			CurrentItem = (SGDisplayItem *) GetChild();
		}
		else
			CurrentItem = (SGDisplayItem *) CurrentItem->GetNext();
	}

	while (CurrentItem != NULL && !CurrentItem->IsSelected())
		CurrentItem = (SGDisplayItem *) CurrentItem->GetNext();

	return(CurrentItem);
}



/***********************************************************************************************

>	virtual INT32 SGDisplayGroup::GetSelectedItemCount(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/95

	
	Returns:	The number of items in this group which are selected (may return 0)

	Purpose:	To determine how many child items of this group are selected

	Notes:		This scans the selection (is not cached), so calling it multiple times
				should be avoided where possible

	SeeAlso:	SGDisplayNode::FindSubtree; SGDisplayGroup::FindNextSelectedItem

***********************************************************************************************/

INT32 SGDisplayGroup::GetSelectedItemCount(void)
{
	INT32 Count = 0;
	SGDisplayItem *Item = FindNextSelectedItem(NULL);

	while (Item != NULL)
	{
		if (Item->IsSelected())
			Count++;

		Item = FindNextSelectedItem(Item);
	}

	return(Count);
}



/********************************************************************************************

>	virtual void SGDisplayGroup::SelectItems(BOOL SelectThem, BOOL Exclusive = FALSE,
						Document *ParentDocument = NULL, Library *ParentLibrary = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/95

	Inputs:		SelectThem - TRUE to select the given items, FALSE to deselect them

				Exclusive  - TRUE to apply this action to all items *outside* the given
				range, FALSE to apply it to all items *inside* the range.

				Document - NULL, or the document which defines the range of items to affect

				Library - NULL, or the library which defines the range of items to affect

	Purpose:	To select/deselect groups of display items in this Gallery display.
				Do not call this method - use the SuperGallery version

	Notes:		To select all items in a range, and deselect all items outside the range,
				you need to use 2 calls to this method.

				To select/deselect all items in the display, pass FALSE, NULL, NULL to
				the last 3 parameters. (If Doc/Lib are both NULL, 'Exclusive' has no effect)

				OVERRIDES the base SGDisplayNode method to handle the last 3 paramters
				properly. Firstly, groups won't select themselves; secondly, the call is
				only passed on to my children if they are in the requested range.

	SeeAlso:	SuperGallery::SelectItems; SGDisplayNode::SelectItems

********************************************************************************************/

void SGDisplayGroup::SelectItems(BOOL SelectThem, BOOL Exclusive,
								Document *ParentDoc, Library *ParentLib)
{
	BOOL InRange = TRUE;
	
	// If we are selecting items, make sure we (a group) are deselected
	if (SelectThem && Flags.CanSelect)
		SetSelected(FALSE);

	// If anythign other than entire-display-tree was requested, then check if it includes
	// our children...
	if (ParentDoc != NULL || ParentLib != NULL)
	{
		// First, do we contain display stuff for the requested Doc/Lib?
		InRange = (ParentDocument == ParentDoc) || (ParentLibrary == ParentLib);

		// Next, was the range inclusive (everything in doc/lib) or
		// exclusive (everything outside doc/lib)?
		if (Exclusive)
			InRange = !InRange;
	}

	if (!InRange)		// My children are not included in the range specified - return
		return;

	// Now, pass the selection request on to my children
	SGDisplayNode *Ptr = GetChild();
	while (Ptr != NULL)
	{
		Ptr->SelectItems(SelectThem, Exclusive, ParentDocument, ParentLibrary);
		Ptr = Ptr->GetNext();
	}
}



/********************************************************************************************

>	virtual void SGDisplayGroup::SelectGroups(BOOL SelectThem, BOOL Exclusive = FALSE,
						Document *ParentDocument = NULL, Library *ParentLibrary = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/95

	Inputs:		SelectThem - TRUE to select the given groups, FALSE to deselect them

				Exclusive  - TRUE to apply this action to all groups *outside* the given
				range, FALSE to apply it to all groups *inside* the range.

				Document - NULL, or the document which defines the range of groups to affect

				Library - NULL, or the library which defines the range of groups to affect

	Purpose:	To select/deselect sets of display groups in this Gallery display.
				All groups whose state changes will force redraw themselves

				If selecting, all non-group nodes in the tree will be deselected

				Do not call this method - use the SuperGallery version

	Notes:		To select all groups in a range, and deselect all groups outside the range,
				you need to use 2 calls to this method.

				To select/deselect all groups in the display, pass FALSE, NULL, NULL to
				the last 3 parameters. (If Doc/Lib are both NULL, 'Exclusive' has no effect)

	SeeAlso:	SuperGallery::SelectGroups; SGDisplayGroup::SelectGroups

********************************************************************************************/

void SGDisplayGroup::SelectGroups(BOOL SelectThem, BOOL Exclusive,
								Document *ParentDoc, Library *ParentLib)
{
	BOOL InRange = TRUE;
	
	// If anything other than entire-display-tree was requested, then check if it includes us
	if (ParentDoc != NULL || ParentLib != NULL)
	{
		// First, do we contain display stuff for the requested Doc/Lib?
		InRange = (ParentDocument == ParentDoc) || (ParentLibrary == ParentLib);

		// Next, was the range inclusive (everything in doc/lib) or
		// exclusive (everything outside doc/lib)?
		if (Exclusive)
			InRange = !InRange;
	}

	// Select myself if I am withint the specified range
	if (InRange && Flags.CanSelect /*&& !Flags.Invisible*/)
		SetSelected(SelectThem);

	// Now, pass the selection request on to my children, so the items will be
	// deselected if necessary
	SGDisplayNode *Ptr = GetChild();
	while (Ptr != NULL)
	{
		Ptr->SelectGroups(SelectThem, Exclusive, ParentDocument, ParentLibrary);
		Ptr = Ptr->GetNext();
	}
}


/***********************************************************************************************

>	void SGDisplayGroup::ReadGroupTitle(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/94

	Purpose:	Reads the title text for this Group from its parent document/library
				This information is cached, but at present it is re-read on each
				redraw request to ensure it is kept up to date

***********************************************************************************************/

void SGDisplayGroup::ReadGroupTitle(void)
{
	// Generate the text to be displayed in this group heading
	if (ParentDocument != NULL)
	{
		String_256 NewTitle;
		const String_256 DocTitle = ParentDocument->GetTitle();
		NewTitle.MakeMsg(_R(IDS_GALGROUPDOCUMENT), (const TCHAR *) DocTitle);

		// Truncate the name into our 64-char buffer
		NewTitle.Left(&TitleText, 63);
	}
PORTNOTE("galleries", "disabled folder updated stuff")
#ifndef EXCLUDE_FROM_XARALX
	else if (ParentLibrary != NULL)
	{
		ParentLibrary->GetLibraryTitle(&TitleText);
		//>> webster (Adrian 02/01/97)
		UINT32 nModified = ParentLibrary->GetModified();
		if (nModified == FOLDER_UPDATED)
		{
			String_256 strUpdated(_R(IDS_FOLDERUPDATED));
			TitleText += strUpdated;
		}
		if (nModified == FOLDER_NEW)
		{
			String_256 strNew(_R(IDS_FOLDERNEW));
			TitleText += _T(" ");
			TitleText += strNew;
		}
		//<< webster
	}
#endif
}



/***********************************************************************************************

>	virtual void SGDisplayNode::DragWasReallyAClick(SGMouseInfo *MouseInfo,
													SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/95

	Inputs:		MouseInfo - The mouse info passed to the original click handler
				MiscInfo - The misc info passed to the original click handler

	Purpose:	Handles a mouse click event. This is a callback function - drags of
				items from galleries will call this function back if the drag turns
				out to just be a click.

	Notes:		SELECTABLE groups override this method to call DefaultClickHandler when
				drags on them turn into clicks

	Documentation:	docs\howtouse\sgallery.doc

	SeeAlso:	SGDisplayGroup::HandleEvent; SGDisplayNode::DefaultClickHandler

***********************************************************************************************/

void SGDisplayGroup::DragWasReallyAClick(SGMouseInfo *MouseInfo, SGMiscInfo *MiscInfo)
{
	// Just get default selection action to be applied for this click.
	// The TRUE indicates that this is a drag-click, and we previously called 
	// the DefaultPreDragHandler - we don't want it to do those same actions twice!

	// But we only do anything at all if we're a selectable group
	if (Flags.CanSelect)
		DefaultClickHandler(MouseInfo, MiscInfo, TRUE);
}



/***********************************************************************************************

>	virtual BOOL SGDisplayGroup::HandleEvent(SGEventType EventType, void *EventInfo,
											 SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94

	Inputs:		See SGDisplayNode::HandleEvent

	Returns:	TRUE if the event was handled successfully
				FALSE if it was not

	Purpose:	Handles a SuperGallery DisplayTree event

	Notes:		This overrides the pure virtual SGDisplayNode::HandleEvent method

				A node need not handle a specific event - if it does not handle it, it
				should return FALSE.

				Redraw and Formatting handlers should never return TRUE, as this will
				prevent the event from continuing through the tree.

				Non-leaf-nodes must call SGDisplayNode::GiveEventToMyChildren in order
				to pass the event dow the tree.

	SeeAlso:	SGDisplayNode::HandleEvent; SGDisplayNode::GiveEventToMyChildren

***********************************************************************************************/

BOOL SGDisplayGroup::HandleEvent(SGEventType EventType, void *EventInfo,
								 SGMiscInfo *MiscInfo)
{
	TRACEUSER( "Matt", _T("SGDisplayGroup::HandleEvent called\n"));
	// And handle the specific event
	switch (EventType)
	{
		case SGEVENT_FORMAT:
			{
				SGFormatInfo *FormatInfo = GetFormatInfo(EventType, EventInfo);

				// If we've recently folded or unfolded (ChildArea is Empty) then change the
				// format rectangle to ensure everything below this point is redrawn
				if (ChildArea.IsEmpty())
					FormatRect = DocRect(0,0,0,0);

				if (!Flags.Invisible)
				{
					CalculateFormatRect(FormatInfo, MiscInfo, DefaultGroupWidth, DefaultGroupHeight);
				}
				else
				{
					CalculateFormatRect(FormatInfo, MiscInfo, DefaultGroupWidth, 0);
				}
			}
			break;

		case SGEVENT_REDRAW:
			{
				DocRect MyRect = FormatRect;	// Rely on cached format info being correct

				SGRedrawInfo *RedrawInfo = GetRedrawInfo(EventType, EventInfo);

				if (IMustRedraw(RedrawInfo) && !Flags.Invisible && Flags.CanSelect)
				{
					StartRendering(RedrawInfo, MiscInfo);

					DocColour Col(192, 192, 192);	//128, 128, 128);
					Col.SetSeparable(FALSE);		// Do not colour correct or separate this colour!

					RedrawInfo->Renderer->SetLineWidth(0);
					RedrawInfo->Renderer->SetLineColour(RedrawInfo->Transparent);
					RedrawInfo->Renderer->SetFillColour(Col);
					RedrawInfo->Renderer->DrawRect(&MyRect);

					ReadGroupTitle();		// (Re)Cache the title text
	
					// Plot the folder glyph (an 18x14 bitmap)
					DocRect GlyphRect(MyRect);
					GlyphRect.lo.x += DevicePixels(MiscInfo, 2);
					GlyphRect.hi.x = GlyphRect.lo.x + DevicePixels(MiscInfo, 18);

					// Center the bitmap's 14 pixels of height in our strip
					INT32 Excess = GlyphRect.Height() - DevicePixels(MiscInfo, 14);
					Excess = GridLock(MiscInfo, Excess / 2);
					GlyphRect.lo.y += Excess;
					GlyphRect.hi.y = GlyphRect.lo.y + DevicePixels(MiscInfo, 14);
					MyRect.lo.x = GlyphRect.hi.x;

					#ifdef _DEBUG
						if(IsVirtualised())
						{
							// in debug builds we get a green rectangle to denote virtualised-ness...
							DrawBitmap(RedrawInfo->Renderer, &GlyphRect,
								(Flags.Folded) ? _R(IDB_GALLERY_FOLD2) : _R(IDB_GALLERY_FOLD0));
						} else {
							DrawBitmap(RedrawInfo->Renderer, &GlyphRect,
										(Flags.Folded) ? _R(IDB_GALLERY_FOLD1) : _R(IDB_GALLERY_FOLD0));
						}
					#else
						DrawBitmap(RedrawInfo->Renderer, &GlyphRect,
									(Flags.Folded) ? _R(IDB_GALLERY_FOLD1) : _R(IDB_GALLERY_FOLD0));
					#endif

					// Mode selection rectangle one pixel to the right so it looks a bit better
					MyRect.lo.x += DevicePixels(MiscInfo, 1);

					if (MyRect.lo.x < MyRect.hi.x)	// If still room left, draw title
					{
						// Space between text and glyph
						MyRect.lo.x += DevicePixels(MiscInfo, 3);

						if (Flags.Selected)
						{
							// Fill the entire text background with the 'selected' colour
							RedrawInfo->Renderer->SetFillColour(RedrawInfo->SelBackground);
							RedrawInfo->Renderer->DrawRect(&MyRect);

							RedrawInfo->Renderer->SetFixedSystemTextColours(&RedrawInfo->SelForeground, &RedrawInfo->SelBackground);
						}
						else
							RedrawInfo->Renderer->SetFixedSystemTextColours(&RedrawInfo->Foreground, &Col);

						// Space between text and glyph
						MyRect.lo.x += DevicePixels(MiscInfo, 3);
	
						if (MyRect.lo.x < MyRect.hi.x)	// If still room left, draw text
							RedrawInfo->Renderer->DrawFixedSystemText(&TitleText, MyRect);
					}

					StopRendering(RedrawInfo, MiscInfo);
				}

				// Check if the cliprect overlaps any of our children - if not, then we
				// can return now, passing the event on quickly without bothering to give
				// it to our children.

				if (Flags.Folded || GetChild() == NULL ||
					!ChildArea.IsIntersectedWith(RedrawInfo->Bounds))
				{
					return(FALSE);
				}
			}
			break;


		case SGEVENT_MOUSECLICK:
			{
				if (!Flags.Invisible)
				{
					SGMouseInfo *Mouse = GetMouseInfo(EventType, EventInfo);

					// If the click hit us, we will always claim it to save the event going
					// on through the tree unnecessarily
					if (FormatRect.ContainsCoord(Mouse->Position))
					{
						DocRect FolderRect(FormatRect);
						FolderRect.hi.x = FolderRect.lo.x + DevicePixels(MiscInfo, 24);

						// Single clicks on the folder icon will fold/unfold the category
						if (FolderRect.ContainsCoord(Mouse->Position))
						{
							if (!Mouse->DoubleClick)	// We ignore double-clicks, though
							{
								// Toggle the folded state of this group and force a redraw
								SetFoldedState((Flags.Folded) ? FALSE : TRUE);
							}
						}
						else
						{
							if (Mouse->DoubleClick)	// It was a double-click, so {un}fold instead
							{
								// Toggle the folded state of this group and force a redraw
								SetFoldedState((Flags.Folded) ? FALSE : TRUE);
							}
							else
							{
								// Not a click on the folder or a general double-click, so assume it's a
								// drag of the group
								// This starts the drag. We will be called back, either with:
								// a) SGDisplayNode::DragWasReallyAClick(), or
								// b) Some variant of MoveBefore, MoveAfter, or AddItem
								//	  (possibly via SuperGallery::CopyItem) to rearrange this
								//	  item in the tree
	
								if (Flags.CanSelect)
									DefaultPreDragHandler(Mouse, MiscInfo);
	
								SGListDragInfo *DragGroup;
								DragGroup = new SGListDragInfo(GetParentGallery(), this,
																Mouse, Mouse->MenuClick);
								if (DragGroup != NULL)
									DragManagerOp::StartDrag(DragGroup, GetListWindow());
								// The DragWasReallyAClick handler will take care of clicks
							}
						}

						// Claim this event - nobody else can own this click
						return(TRUE);
					}

					// Next, if the click cannot hit any of our children, pass the event on
					// without passing it to any of the children, to save time
					if (Flags.Folded || GetChild() == NULL ||
						!ChildArea.ContainsCoord(Mouse->Position))
					{
						return(FALSE);
					}
				}
			}
			break;		


		case SGEVENT_CLAIMPOINT:
			{
				if (!Flags.Invisible)
				{
					SGClaimPointInfo *PointInfo = GetClaimPointInfo(EventType, EventInfo);

					// If the point hit us, we must claim the event so that the caller
					// knows which tree item contains the point.
					if (FormatRect.ContainsCoord(PointInfo->Position))
					{
						PointInfo->ClosestSoFar = 0;
						PointInfo->Claimant = this;		// Let 'em know it was me!
						return(TRUE);
					}

					// OK, we don't OWN the point, but are we closer to it than anyone else
					// checked so far? If so, we update ClosestSoFar and Claimant, but we
					// do not claim the event (so others can check if they are closer)
				
					// Find the distance to the 2 closest edges of the rectangle. Then
					// the approx. distance to the rectangle is the larger of the two.
					INT32 XDist = 0;
					if (FormatRect.lo.x > PointInfo->Position.x)
						XDist = FormatRect.lo.x - PointInfo->Position.x;
					else if (FormatRect.hi.x < PointInfo->Position.x)
						XDist = PointInfo->Position.x - FormatRect.lo.x;
	
					INT32 YDist = 0;
					if (FormatRect.lo.y > PointInfo->Position.y)
						YDist = FormatRect.lo.y - PointInfo->Position.y;
					else if (FormatRect.hi.y < PointInfo->Position.y)
						YDist = PointInfo->Position.y - FormatRect.lo.y;
	
					XDist = max(XDist, YDist);	// XDist is now approx the dist to the point
	
					if (XDist < PointInfo->ClosestSoFar)
					{
						PointInfo->Claimant = this;
						PointInfo->ClosestSoFar = XDist;
						// drop through to pass the event on...
					}

					// Next, if the point cannot hit any of our children, pass the event on
					// without passing it to any of the children, to save time
					if (Flags.Folded || GetChild() == NULL ||
						!ChildArea.ContainsCoord(PointInfo->Position))
					{
						return(FALSE);
					}
				}
			}
			break;

		default:
			return(SGDisplayNode::HandleEvent(EventType, EventInfo, MiscInfo));
	}

	// Pass the event on to my children, as appropriate (but not if it's a thumbmsg)
	BOOL Result = GiveEventToMyChildren(EventType, EventInfo, MiscInfo);

	if (EventType == SGEVENT_FORMAT)
	{
		SGFormatInfo *FormatInfo = GetFormatInfo(EventType, EventInfo);

		// After they have formatted themselves, calculate the area they fill, so we can
		// quickly determine if events (clicks/redraws) fall over any of our children.
		ChildArea = FormatRect;		// We are 'infinite' width, so copy our rect to get width
									// and the top of the rectangle.

		// And then move the bottom down to include my children, and not myself
		ChildArea.hi.y = ChildArea.lo.y;
		ChildArea.lo.y = FormatInfo->LinePos;
	}

	// And return...
	return(Result);
}



/***********************************************************************************************

>	void SGDisplayGroup::GetChildArea(DocRect *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/5/95

	Outputs:	Result - Filled in with a rectangle (may be 'Empty')

	Purpose:	Retrieves the display list rectangle of this group's children. This is a
				bounding rectangle which entirely contains the children of this group.

	Notes:		This rectangle may be Empty (if the group is folded, or if it has not
				yet been reformatted after being unfolded)

				The rectangle does NOT include the group's ("titlebar") FormatRect - to
				include the group title and all children, you must union with FormatRect

***********************************************************************************************/

void SGDisplayGroup::GetChildArea(DocRect *Result)
{
	ERROR3IF(Result == NULL, "Illegal NULL param");

	*Result = ChildArea;
}


/***********************************************************************************************

>	virtual BOOL SGDisplayGroup::DefaultPreDragHandler(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Inputs:		Mouse - Information on the mouse state for this click
				MiscInfo - the normal info as passed to event handlers

	Returns:	TRUE if the click caused any action to be taken (selection state to change)
				FALSE if the click was ignored for whatever reason

	Purpose:	Provides part 1 of the default selection model for clicks on gallery display
				items. Should be called by all derived gallery DisplayItems to handle clicks
				upon them, when multiple-selection support is desired.

				You should call this method immediately prior to starting a drag as a result
				of a click event. Note that it is paired with DefaultClickHandler (which
				should be called when the drag you start turns out to be a click, if you
				want multiple-selection capability).

				See the SGDisplayColour (kernel\sgcolour.cpp) for an example of use

	Notes:		The code for this has now been moved into the SGDisplayNode base class since
				group selection is now also required.

	SeeAlso:	SGDisplayItem::DefaultClickHandler; SGDisplayColour::HandleEvent

***********************************************************************************************/

BOOL SGDisplayGroup::DefaultPreDragHandler(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo)
{
	return(SGDisplayNode::DefaultPreDragHandler(Mouse, MiscInfo));
}



/***********************************************************************************************

>	virtual BOOL SGDisplayGroup::DefaultClickHandler(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo,
													BOOL AfterDrag = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/94

	Inputs:		Mouse - Information on the mouse state for this click
				MiscInfo - the normal info as passed to event handlers
				AfterDrag - TRUE if this is being called when a drag turns into a click, and
				you called DefaultPreDragHandler before the drag started

	Returns:	TRUE if the click caused any action to be taken (selection state to change)
				FALSE if the click was ignored for whatever reason

	Purpose:	Provides the default selection model for clicks on gallery display items.
				Should be called by all derived gallery DisplayItems to handle clicks
				upon them, when multiple-selection support is desired.

	Notes:		The code for this has now been moved into the SGDisplayNode base class since
				group selection is now also required.

	SeeAlso:	SuperGallery; SGDisplayNode::InsertAfter; SGDisplayNode::InsertBefore

***********************************************************************************************/

BOOL SGDisplayGroup::DefaultClickHandler(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo,
										BOOL AfterDrag)
{
	return(SGDisplayNode::DefaultClickHandler(Mouse, MiscInfo, AfterDrag));
}


/********************************************************************************************

>	virtual BOOL SGDisplayGroup::CanVirtualise(void);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/95

	Returns:	TRUE if it can
				
	Purpose:	Most groups shouldn't need to be virtualised out. Really it's just library
				groups - SGLibGroup - that should be.

********************************************************************************************/

BOOL SGDisplayGroup::CanVirtualise(void)
{
	return FALSE;
}

/********************************************************************************************

>	virtual BOOL SGDisplayGroup::Virtualise(void);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/95

	Returns:	TRUE if it virtualised out OK (or already was)
				
	Purpose:	Virtualise a group out of memory, or rather, vape the items and entire tree
				associcated with the group, leaving a memory-minimal shell.

********************************************************************************************/

BOOL SGDisplayGroup::Virtualise(void)
{
	// Group is already virtualised
	if(IsVirtualised())
		return TRUE;

PORTNOTE("galleries", "Disabled virtualising switch")
#ifndef EXCLUDE_FROM_XARALX
	// Virtualisation disabled...
	if(!SGLibGroup::LibraryVirtualisingEnabled)
		return FALSE;
#endif

	DestroySubtree(FALSE);	// Delete all items in the group (but not this one obviously)
	SetVirtualisedState(TRUE);

#ifdef _DEBUG
	ForceRedrawOfMyselfAndChildren();
#endif

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL SGDisplayGroup::DeVirtualise(void);

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/95

	Returns:	TRUE if it devirtualised back in OK (or already had)
				
	Purpose:	Virtualise a group back into memory, or rather, add the items associated with
				the group to the group so we can display them, etc.

********************************************************************************************/

BOOL SGDisplayGroup::DeVirtualise(void)
{
	if(!IsVirtualised())
		return TRUE;

#ifdef _DEBUG
	ForceRedrawOfMyselfAndChildren();
#endif

	// Might need some help with this one - override and implement

	return FALSE;
}









/***********************************************************************************************

>	SGDisplayItem::SGDisplayItem()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/5/95

	Purpose:	DisplayItem constructor

***********************************************************************************************/

SGDisplayItem::SGDisplayItem()
{
	// Items can be selected
	Flags.CanSelect = TRUE;
}



/***********************************************************************************************

>	virtual BOOL SGDisplayItem::HandleEvent(SGEventType EventType, void *EventInfo,
											 SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94

	Inputs:		See SGDisplayNode::HandleEvent

	Returns:	TRUE if the event was handled successfully
				FALSE if it was not

	Purpose:	Handles a SuperGallery DisplayTree event

	Notes:		This overrides the pure virtual SGDisplayNode::HandleEvent method

				A node need not handle a specific event - if it does not handle it, it
				should return FALSE.

				Redraw and Formatting handlers should never return TRUE, as this will
				prevent the event from continuing through the tree.

				ClaimPoint handlers should always return FALSE, unless they contain
				the given point, in which case they should return TRUE.

				Non-leaf-nodes must call SGDisplayNode::GiveEventToMyChildren in order
				to pass the event dow the tree. THIS node is a leaf-node, so it doesn't.

				Derived DisplayItem classes should call this base class method if they
				wish to handle CLAIMPOINT broadcasts to provide drag-target detection.

	SeeAlso:	SGDisplayNode::HandleEvent; SGDisplayRoot::HandleEvent;
				SGDisplayGroup::HandleEvent

***********************************************************************************************/

BOOL SGDisplayItem::HandleEvent(SGEventType EventType, void *EventInfo,
								 SGMiscInfo *MiscInfo)
{
	switch(EventType)
	{
		case SGEVENT_CLAIMPOINT:
			{
				SGClaimPointInfo *PointInfo = GetClaimPointInfo(EventType, EventInfo);

				// If the point hit us, we must claim the event so that the caller
				// knows which tree item contains the point.
				if (FormatRect.ContainsCoord(PointInfo->Position))
				{
					PointInfo->ClosestSoFar = 0;
					PointInfo->Claimant = this;		// Let 'em know it was me!
					return(TRUE);
				}

				// OK, we don't OWN the point, but are we closer to it than anyone else
				// checked so far? If so, we update ClosestSoFar and Claimant, but we
				// do not claim the event (so others can check if they are closer)
				
				// Find the distance to the 2 closest edges of the rectangle. Then
				// the approx. distance to the rectangle is the larger of the two.
				INT32 XDist = 0;
				if (FormatRect.lo.x > PointInfo->Position.x)
					XDist = FormatRect.lo.x - PointInfo->Position.x;
				else if (FormatRect.hi.x < PointInfo->Position.x)
					XDist = PointInfo->Position.x - FormatRect.lo.x;

				INT32 YDist = 0;
				if (FormatRect.lo.y > PointInfo->Position.y)
					YDist = FormatRect.lo.y - PointInfo->Position.y;
				else if (FormatRect.hi.y < PointInfo->Position.y)
					YDist = PointInfo->Position.y - FormatRect.lo.y;

				XDist = max(XDist, YDist);	// XDist is now approx the dist to the point

				if (XDist < PointInfo->ClosestSoFar)
				{
					PointInfo->Claimant = this;
					PointInfo->ClosestSoFar = XDist;
					// drop through to pass the event on...
				}
			}
			break;


		default:
			return(SGDisplayNode::HandleEvent(EventType, EventInfo, MiscInfo));
	}

	return(FALSE);
}



/***********************************************************************************************

>	virtual void SGDisplayItem::AddItem(SGDisplayNode *NodeToInsert,
										SGSortKey *SortInfo = NULL)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94

	Inputs:		NodeToInsert - the node/subtree to be inserted
				SortInfo - NULL, or an array of MaxSGSortKeys sort key structures, which
				describes the sort mode to be used for determining the position of the
				insertion.

	Purpose:	OVERRIDES the DisplayNode action, and gives an ERROR3 -
				DisplayItems are only allowed to be leaf-nodes, and hence you cannot
				insert items as children of them.

	SeeAlso:	SuperGallery; SGDisplayNode::InsertAfter; SGDisplayNode::InsertBefore

***********************************************************************************************/

void SGDisplayItem::AddItem(SGDisplayNode *NodeToInsert, SGSortKey *SortInfo)
{
	ERROR3("You can't SGDisplayItem::AddItem() - DisplayItems are supposed to be leaf nodes");
}



/***********************************************************************************************

>	virtual void SGDisplayItem::RemoveFromTree(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/95

	Purpose:	De-links this node/subtree from the DisplayTree. This DOES NOT DELETE the node,
				just unlinks it in preparation for being deleted.

	Notes:		This method calls SGDisplayNode::RemoveFromTree to do all the work.
				It is overridden simply so that display items can deselect themselves and
				inform the parent gallery if they were the selected node, so that pointers
				to removed nodes are not kept lying around.

	SeeAlso:	SGDisplayNode::RemoveFromTree

***********************************************************************************************/

void SGDisplayItem::RemoveFromTree(void)
{
//	SetSelected(FALSE);
	SGDisplayNode::RemoveFromTree();
}



/***********************************************************************************************

>	virtual BOOL SGDisplayItem::DefaultPreDragHandler(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Inputs:		Mouse - Information on the mouse state for this click
				MiscInfo - the normal info as passed to event handlers

	Returns:	TRUE if the click caused any action to be taken (selection state to change)
				FALSE if the click was ignored for whatever reason

	Purpose:	Provides part 1 of the default selection model for clicks on gallery display
				items. Should be called by all derived gallery DisplayItems to handle clicks
				upon them, when multiple-selection support is desired.

				You should call this method immediately prior to starting a drag as a result
				of a click event. Note that it is paired with DefaultClickHandler (which
				should be called when the drag you start turns out to be a click, if you
				want multiple-selection capability).

				See the SGDisplayColour (kernel\sgcolour.cpp) for an example of use

	Notes:		The code for this has now been moved into the SGDisplayNode base class since
				group selection is now also required.

	SeeAlso:	SGDisplayItem::DefaultClickHandler; SGDisplayColour::HandleEvent

***********************************************************************************************/

BOOL SGDisplayItem::DefaultPreDragHandler(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo)
{
	return(SGDisplayNode::DefaultPreDragHandler(Mouse, MiscInfo));
}



/***********************************************************************************************

>	virtual BOOL SGDisplayItem::DefaultClickHandler(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo,
													BOOL AfterDrag = FALSE,
													BOOL AdjustDoubleClick = TRUE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/94

	Inputs:		Mouse - Information on the mouse state for this click
				MiscInfo - the normal info as passed to event handlers
				AfterDrag - TRUE if this is being called when a drag turns into a click, and
				you called DefaultPreDragHandler before the drag started

				AdjustDoubleClick - Override for derived classes to enable/disable the
				default adjust-double-click action (which closes the gallery after apply).
				The colour gallery has a special meaning for adjust-double-click, so overrides
				this variable to pass in FALSE and disable default action.

	Returns:	TRUE if the click caused any action to be taken (selection state to change)
				FALSE if the click was ignored for whatever reason

	Purpose:	Provides the default selection model for clicks on gallery display items.
				Should be called by all derived gallery DisplayItems to handle clicks
				upon them, when multiple-selection support is desired.

	Notes:		The code for this has now been moved into the SGDisplayNode base class since
				group selection is now also required.

	SeeAlso:	SuperGallery; SGDisplayNode::InsertAfter; SGDisplayNode::InsertBefore

***********************************************************************************************/

BOOL SGDisplayItem::DefaultClickHandler(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo,
										BOOL AfterDrag, BOOL AdjustDoubleClick)
{
	return(SGDisplayNode::DefaultClickHandler(Mouse, MiscInfo, AfterDrag, AdjustDoubleClick));
}


