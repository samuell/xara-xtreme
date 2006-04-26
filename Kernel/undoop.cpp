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

#include "camtypes.h"
#include "undoop.h"

#include "ophist.h"

#include "range.h"                        
#include "document.h"  
#include "ensure.h"     
#include "selstate.h"
#include "tranform.h"
#include "docview.h"
//#include "mario.h"
#include "ink.h"
#include "spread.h"
#include "fixmem.h"
#include "nodepath.h"
#include "app.h"
#include "cpyact.h"
#include "clipint.h"
#include "pathedit.h"
#include "ndoptmz.h"
#include "sprdmsg.h"
#include "layer.h"
//#include "markn.h"
#include "progress.h"
#include "invalid.h"
#include "becomea.h"
#include "nodeattr.h"
#include "objchge.h"
#include "group.h"
#include "nodeblnd.h"
#include "nodetxtl.h"
#include "nodetxts.h"
#include "ndbldpth.h"
#include "textops.h"
#include "cutop.h"
#include "camdoc.h"
#include "effects_stack.h"	// for EffectsStack

#ifdef _DEBUG
#include "layermsg.h"	// layer messages for redraw layer message in debug builds
#endif

#include "ngcore.h"		// for NameGallery stretching implementation.
#include "chapter.h"

#include "opdupbar.h"
 
CC_IMPLEMENT_DYNCREATE( UndoableOperation, Operation )

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW    

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ObjectSet methods


/********************************************************************************************

>	BOOL ObjectSet::AddToSet(NodeRenderableInk* pObject)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/95
	Returns:	FALSE if we run out of memory
	Purpose:	Adds pObject to the set
	SeeAlso:	ObjectSet::InSet
	SeeAlso:	ObjectSet::CopySet


********************************************************************************************/

BOOL ObjectSet::AddToSet(NodeRenderableInk* pObject)
{
	ERROR3IF(pObject == NULL, "AddToSet: pObject is NULL"); 
	// Determine if the pObject is already in the set
	ObjectItem* pObjItem = (ObjectItem*) GetHead();
	while (pObjItem != NULL)
	{
		if (pObjItem->pObject == pObject)
		{
			return TRUE; // Already in set so return
		}
		pObjItem = (ObjectItem*)GetNext(pObjItem);
	}
	
	// The attribute type is not in the set so let's add it
	pObjItem = new  ObjectItem;
	if (!pObjItem)
		return FALSE; 	// out of memory (error has been set)

	pObjItem->pObject = pObject;

	AddHead(pObjItem); // Add attribute to the head of the list
	 
	return TRUE; 
}


/********************************************************************************************

>	BOOL ObjectSet::CopySet()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Inputs:		-
	Outputs:	-
	Returns:	A copy of the set, or NULL if we run out of memory
	Purpose:	Makes a copy of the object set
	Errors:		-
	SeeAlso:	ObjectSet::AddToSet
	SeeAlso:	ObjectSet::InSet

********************************************************************************************/

ObjectSet* ObjectSet::CopySet()
{
	ObjectSet* pSetCopy; 
	pSetCopy = new ObjectSet;
	if (!pSetCopy)
		return NULL; // Out of memory

	// Copy each item in turn
	ObjectItem* pItemCopy;
	ObjectItem* pCurrent = (ObjectItem*)GetHead(); 
	while (pCurrent)
	{
		pItemCopy = new ObjectItem;
		if (!pItemCopy)
		{
			// Tidyup
			pSetCopy->DeleteAll();
			delete pSetCopy;
			return NULL;  
		}
		pItemCopy->pObject = pCurrent->pObject;
		pSetCopy->AddTail(pItemCopy); 
		pCurrent = (ObjectItem*)GetNext(pCurrent); 	
	}
	return pSetCopy;
}

/********************************************************************************************

>	BOOL ObjectSet::InSet(NodeRenderableInk* pObject)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Inputs:		pObject: A pointer to an object
	Outputs:	-
	Returns:	TRUE if pObject is in the set
	Purpose:	To determine if pObject is in the set	
	Errors:		-
	SeeAlso:	ObjectSet::CopySet
	SeeAlso:	ObjectSet::AddToSet


********************************************************************************************/

BOOL ObjectSet::InSet(NodeRenderableInk* pObject)
{
	ObjectItem* pCurrent = (ObjectItem*)GetHead(); 
	while (pCurrent)
	{
		if (pCurrent->pObject == pObject)
			return TRUE; 
		pCurrent = (ObjectItem*)GetNext(pCurrent); 	
	}
	return FALSE; 
}  		  



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// UndoableOperation methods



BOOL UndoableOperation::MovingNode = FALSE;
                                                                
/********************************************************************************************

>	UndoableOperation::UndoableOperation()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
   	Purpose:	Constructs a new undoable operation object:
	Errors:		-
	SeeAlso:	Operation::Operation

********************************************************************************************/

UndoableOperation::UndoableOperation(): Operation()
{	
	// This var is a flag that ensures that this warning will only appear once for the lifetime of this op
	WarnInsertObjsOntoLockedLayers = TRUE;
}


/********************************************************************************************

>	virtual UndoableOperation::~UndoableOperation()


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	UndoableOperation destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UndoableOperation::~UndoableOperation()
{
}



// ---------------------------------------------------------------------------------------
// The Do functions 

/********************************************************************************************

>	BOOL UndoableOperation::DoInsertNewNode(NodeRenderableBounded* NewNode, 
											Spread *pSpread, 
											BOOL InvalidateRgn,
											BOOL ClearSelection = TRUE,
						 					BOOL SelectNewObject = TRUE,
						 					BOOL NormaliseAttributes=TRUE)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/93
	Inputs:		NewNode: 			The new node to be added 
				pSpread: 			The spread to add the new node to 
									*Set this to NULL if you want to insert
									the node onto the selected spread.
				InvalidateRgn: 		Invalidate the node's region (including blobs)
				ClearSelection: 	Clear all selected objects prior to the new object
									being inserted (Default TRUE)
				SelectNewObject:	Select the new object (Default TRUE)
				NormaliseAttributes:When TRUE redundant attributes are removed from the 
									node. If you know that the node being inserted has
									the correct set of attributes then you can set this to
									FALSE.   
 
				Note: Do not set the bounding rectangle of NewNode prior to calling 
					  this function, or the bounds will not be propagated. 
	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 
	Purpose:	Inserts the node as a last child of the active layer of the pSpread. 
				If the NewNode is a NodeRenderableInk then the node becomes selected and
				all other nodes are deselected. If pSpread is NULL then the object is inserted
				on the selected spread.
	SeeAlso:	UndoableOperation::DoInsertNewNode

********************************************************************************************/

BOOL UndoableOperation::DoInsertNewNode(NodeRenderableBounded* NewNode, 
								Spread *pSpread, 
								BOOL InvalidateRgn,
								BOOL ClearSelection,  		// = TRUE
			 					BOOL SelectNewObject, 		// = TRUE
			 					BOOL NormaliseAttributes) 	// = TRUE

{ 
	ENSURE( pOurDoc!=NULL, "No current document in DoInsertNode" );

	// Insert node as the active layer of the spread 
	pOurDoc->InsertNewNode(NewNode, pSpread);
	NewNode->SetSelected(FALSE); 

	// This is a brand new node, so we have to make sure that all its parents
	// are invalidated as well.
	// if this nodes bounding rect was already invalid when it was added the parent
	// will not be invalidated as well, so we first mark its bounds as valid.
	// This is very important - Ask Rik if you think it looks silly.
	NewNode->ValidateBoundingRect();
	NewNode->InvalidateBoundingRect();

	// If inserting a node onto a layer that is locked or invisible, warn the user before
	// proceeding, and ensure that the node is not selected.
	if (NewNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk)))
	{
		Layer* pLayer = (Layer*)NewNode->FindParent(CC_RUNTIME_CLASS(Layer));
		if (pLayer != NULL)
		{
			if (!pLayer->IsVisible() || pLayer->IsLocked())
			{
				if (WarnInsertObjsOntoLockedLayers)
				{
#if (_OLE_VER >= 0x200)
					// Only warn the user if the document is visible to him/her.
					if (pOurDoc && pOurDoc->GetOilDoc() && pOurDoc->GetOilDoc()->IsVisible())
#endif
					{
						InformWarning(_R(IDS_WARNING_INSERTLAYER));
					}
					// Set flag to FALSE so it won't be reported back again in this op
					WarnInsertObjsOntoLockedLayers = FALSE;
				}
				SelectNewObject = FALSE;
			}

			// Note that this layer has been edited
			pLayer->SetEdited(TRUE);
#ifdef _DEBUG
			// Tell the frame gallery to update its display of the frame
			BROADCAST_TO_ALL( LayerMsg( pLayer, LayerMsg::REDRAW_LAYER ) );
#endif
		}
	}


	/*
	if (InvalidateRgn)
	{
		if (!DoInvalidateNodeRegion((NodeRenderable*)NewNode, TRUE))
		{
			// Remove the NewNodeFrom the tree 
			NewNode->UnlinkNodeFromTree(); 
			return FALSE; 
		}
	}
	*/

	// Create a hide node action to hide the node when we undo 
    HideNodeAction* UndoHideNodeAction;     
	if (HideNodeAction::Init(this,                    
					  		 &UndoActions,
							 NewNode, 
							 TRUE, 		 // Include subtree size 
					  		 ( Action**)(&UndoHideNodeAction))      
					  		  == AC_FAIL)

	{  
		// Remove the NewNodeFrom the tree 
		NewNode->UnlinkNodeFromTree(); 
		return FALSE; 
	}


	// Make it the currently selected object if it is an Ink object
	if ( NewNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk)) )
	{
		if (ClearSelection)
		{
			NodeRenderableInk::DeselectAll();
			GetApplication()->UpdateSelection();
		}


		if (SelectNewObject)
		{
			// Select the new node, and don't redraw it yet 
			((NodeRenderableInk*)NewNode)->Select(FALSE);
		}

		if (NormaliseAttributes)
		{
			((NodeRenderableInk*)NewNode)->NormaliseAttributes(); // Removes redundant attributes
		}


	}

	// Update the selection
	if (NewNode->IsSelected())
		GetApplication()->UpdateSelection();

	if ( NewNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded)) )
	{
		if (InvalidateRgn)
		{
			if (!DoInvalidateNodeRegion((NodeRenderableBounded*)NewNode, TRUE))
			{
				return FALSE; 
			}
		}
	}

// Added by Jim: Broadcast the layerchanged message when inserting a new layer

	if (NewNode->GetRuntimeClass() == CC_RUNTIME_CLASS(Layer))
	{
		Spread* pSpread = (Spread*) NewNode->FindParent(CC_RUNTIME_CLASS(Spread)); 
		BROADCAST_TO_ALL(SpreadMsg(pSpread,SpreadMsg::LAYERCHANGES));
	}

	return (TRUE); 
}     




/********************************************************************************************

>	BOOL UndoableOperation::DoInsertNewNode(NodeRenderableBounded* NewNode, 
						 					Node* ContextNode,
						 					AttachNodeDirection Direction, 
						 					BOOL InvalidateRegion,
						 					BOOL ClearSelection = TRUE,
						 					BOOL SelectNewObject = TRUE,
						 					BOOL NormaliseAttributes=TRUE)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/93
	Inputs:		NewNode: 			The new node to be added 
				ContextNode:		The node to which NewNode is to be attached
				Direction:			The direction the new node should be attached to the
									context node. 
				InvalidateRgn: 		Invalidate the node's region (including blobs)
				ClearSelection: 	Clear all selected objects prior to the new object
									being inserted (Default TRUE)
				SelectNewObject:	Select the new object (Default TRUE) 
				NormaliseAttributes:When TRUE redundant attributes are removed from the 
									node. If you know that the node being inserted has
									the correct set of attributes then you can set this to
									FALSE. 
				Note: Do not set the bounding rectangle of NewNode prior to calling 
					  this function, or the bounds will not be propagated. 
	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 
	Purpose:	This high level do function attaches the node to ContextNode in the direction 
				specified. If the NewNode is a NodeRenderableInk then the node becomes 
				selected and all other nodes are deselected. 
	SeeAlso:	UndoableOperation::DoInsertNewNode

********************************************************************************************/

BOOL UndoableOperation::DoInsertNewNode(NodeRenderableBounded* NewNode, 
					 			Node* ContextNode,
					 			AttachNodeDirection Direction, 
					 			BOOL InvalidateRegion, 
					 			BOOL ClearSelection, 
					 			BOOL SelectNewObject,
								BOOL NormaliseAttributes)
{
	// Insert the NewNode into the tree
	NewNode->AttachNode(ContextNode, Direction); 
	NewNode->SetSelected(FALSE); 

	// This is a brand new node, so we have to make sure that all its parents
	// are invalidated as well.
	// if this nodes bounding rect was already invalid when it was added the parent
	// will not be invalidated as well, so we first mark its bounds as valid.
	// This is very important - Ask Rik if you think it looks silly.
	NewNode->ValidateBoundingRect();
	NewNode->InvalidateBoundingRect();

	// If inserting a node onto a layer that is locked or invisible, warn the user before
	// proceeding, and ensure that the node is not selected.
	if (NewNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk)))
	{
		Layer* pLayer = (Layer*)NewNode->FindParent(CC_RUNTIME_CLASS(Layer));
		if (pLayer != NULL)
		{
			if (!pLayer->IsVisible() || pLayer->IsLocked())
			{
				if (WarnInsertObjsOntoLockedLayers)
				{
#if (_OLE_VER >= 0x200)
					// Only warn the user if the document is visible to him/her.
					if (pOurDoc && pOurDoc->GetOilDoc() && pOurDoc->GetOilDoc()->IsVisible())
#endif
					{
						// ignore this warning on some ops - like if we have text syncing between layers
						// or other bar ops that manipulate invisible layers
						// or doing the right click from drag op since the msg upsets the drag
PORTNOTE("text","Removed OpTextFormat reference")
#ifndef EXCLUDE_FROM_XARALX
						if (!IS_A(this, OpTextFormat) && !IS_A(this, OpDuplicateBar) && !IS_A(this, OpCopyAndTransform))
							InformWarning(_R(IDS_WARNING_INSERTLAYER));
#endif
					}

					// Set flag to FALSE so it won't be reported back again in this op
					WarnInsertObjsOntoLockedLayers = FALSE;
				}
				SelectNewObject = FALSE;
			}

			// Note that this layer has been edited
			pLayer->SetEdited(TRUE);
#ifdef _DEBUG
			// Tell the frame gallery to update its display of the frame
			BROADCAST_TO_ALL(LayerMsg(pLayer, LayerMsg::/*LayerReason::*/REDRAW_LAYER));
#endif
		}
	}

	// Create a hide node action to hide the node when we undo 
    HideNodeAction* UndoHideNodeAction;     
	if (HideNodeAction::Init(this,                    
					  		 &UndoActions,
							 NewNode, 
							 TRUE, 		 // Include subtree size 
					  		 ( Action**)(&UndoHideNodeAction))      
					  		  == AC_FAIL)

	{   
		return FALSE; 
	}

	

	// Make it the currently selected object if it is an Ink object
	if ( NewNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk)) )
	{
		if (ClearSelection)
		{
			NodeRenderableInk::DeselectAll();
			GetApplication()->UpdateSelection();

		}

		if (SelectNewObject)
		{

			// Select the new node
			((NodeRenderableInk*)NewNode)->Select(FALSE);
		}


		if (NormaliseAttributes)
		{
			((NodeRenderableInk*)NewNode)->NormaliseAttributes(); // Removes redundant attributes
		}

	
	}

	// Update the selection
	if (NewNode->IsSelected())
		GetApplication()->UpdateSelection();


	if ( NewNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded)) )
	{
		if (InvalidateRegion)
		{
			NewNode->ReleaseCached(TRUE, FALSE, TRUE, TRUE);
			if (!DoInvalidateNodeRegion((NodeRenderableBounded*)NewNode, TRUE, FALSE, FALSE, FALSE))	// Don't recache automatically!
			{
				return FALSE; 
			}
		}
	}

//	#ifdef _DEBUG
//		// If the node being Inserted is a layer then people need to know about it
//		if (NewNode->GetRuntimeClass() == CC_RUNTIME_CLASS(Layer))
//	  	{
//			TRACE( _T("Layer inserted. Have you remembered to broadcast a SpreadMsg::LAYERCHANGES message?"));
//	    	//Spread* pSpread = (Spread*) NewNode->FindParent(CC_RUNTIME_CLASS(Spread)); 
//	   		//BROADCAST_TO_ALL(SpreadMsg(pSpread,SpreadMsg::LAYERCHANGES));
//	  	}
//	#endif

// Added by Jim: Broadcast the layerchanged message when inserting a new layer

	if (NewNode->GetRuntimeClass() == CC_RUNTIME_CLASS(Layer))
	{
		Spread* pSpread = (Spread*) NewNode->FindParent(CC_RUNTIME_CLASS(Spread)); 
		BROADCAST_TO_ALL(SpreadMsg(pSpread,SpreadMsg::LAYERCHANGES));
	}
		
	return (TRUE);
}  


/********************************************************************************************

>	BOOL UndoableOperation::DoHideNode(Node* NodeToHide, BOOL IncludeSubtreeSize, 
							   			NodeHidden** nodeHidden = NULL,
							   			BOOL TellSubtree = TRUE) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/93
	Inputs:		Node: 				The Node to be hidden 
	  
				IncludeSubtreeSize: Flag indicating if the subtree being hidden should 
									be considered as belonging to the Operation history
									or not. (See the note in ops.h)

	Outputs:	nodeHidden: A pointer to the NodeHidden hiding the NodeToHide. This 
				output is optional. 

	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 
	Purpose:	This low level Do function hides the subtree with root NodeToHide	
	Errors:		-
	SeeAlso:	UndoableOperation::DoHideNodes

********************************************************************************************/

BOOL UndoableOperation::DoHideNode(Node* NodeToHide, 
						   			BOOL IncludeSubtreeSize, 
						   			NodeHidden** nodeHidden,
						   			BOOL TellSubtree)  
{
	ERROR3IF(!OpFlags.SucceedAndDiscard && NodeToHide->IsAnAttribute() && NodeToHide->FindParent()->DiscardsAttributeChildren(), "DoHide called under Caret");

	if (NodeToHide->DiscardsAttributeChildren())	// If the node to hide doesn't want to produce undo (CaretNode)
		return TRUE;								// Then bomb out now before we do anything...

	// We shouldn't tell the node that it's being hidden if we are only
	// moving it.
	// Also if we are Hiding a Node Group, then there is no
	// need to tell anyone either.  As the children are unaffected.
	if (TellSubtree)
	{
		// Tell the node and it's subtree, that they are being hidden
		Node* pNode = NodeToHide->FindFirstDepthFirst();
		while (pNode!=NULL)
		{
			if (!pNode->HidingNode())
				return FALSE;

			// And find the next node
			pNode = pNode->FindNextDepthFirst(NodeToHide);
		}
		// Invalidate the region, we need to do this here because we are deleting the node
		if (NodeToHide->IsBounded())
			((NodeRenderableBounded*)NodeToHide)->InvalidateBoundingRect();

	}

// removed by Ed 23/5/95 'cos it caused text redraw problems
// and nobody could justify why just moving an object in the tree should
// cause the renderable bound associated with it to be invalidated
//	// Invalidate the region
//	if (NodeToHide->IsBounded())
//		((NodeRenderableBounded*)NodeToHide)->InvalidateBoundingRect();

	// Try to hide the Node    
	NodeHidden* Hidden; 
	ALLOC_WITH_FAIL(Hidden,(new NodeHidden(NodeToHide)),this);
	if (Hidden == NULL)
	{          
		return (FALSE); 
	}

	// Try to create a ShowNodeAction which will show the node that we have just hidden. 
	
	ShowNodeAction* UndoShowNodeAction; 

	if (ShowNodeAction::Init(this,  
							 &UndoActions, 
							 Hidden, 
							 IncludeSubtreeSize, 
							 (Action**)(&UndoShowNodeAction),
							 TellSubtree) == AC_FAIL)
	{   
		// We must unhide the NodeToHide manually, cos no action will do it for us 
		Hidden->ShowNode(); // Hidden is deleted don't worry 
		return FALSE; 
	}
	
	if (NodeToHide->IsSelected())
	{
		// Update the selection range 
		Camelot.UpdateSelection();
	}

	// If the user provided a nodeHidden parameter then return a pointer to the NodeHidden
	if (nodeHidden != NULL)
		*nodeHidden = Hidden;    

// Added by Jim: Broadcast the layerchanged message when inserting a new layer

	if (NodeToHide->GetRuntimeClass() == CC_RUNTIME_CLASS(Layer))
	{
		Spread* pSpread = (Spread*) Hidden->FindParent(CC_RUNTIME_CLASS(Spread)); 
		BROADCAST_TO_ALL(SpreadMsg(pSpread,SpreadMsg::LAYERCHANGES));
	}
	// Success	
	return (TRUE); 
}

/********************************************************************************************

>	BOOL UndoableOperation::DoHideNodes(Range NodeRange, BOOL IncludeSubtreeSize, 
										BOOL TellSubtree = TRUE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/03/94

	Inputs:		NodeRange: 			The Range of nodes to be hidden 
	  
				IncludeSubtreeSize: Flag indicating if the subtrees being hidden should 
									be considered as belonging to the Operation history
									or not. (See the note in ops.h)


	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 
	Purpose:	This low level Do function hides a range of the subtrees 
	Errors:		-
	SeeAlso:	UndoableOperation::DoHideNode

********************************************************************************************/

BOOL UndoableOperation::DoHideNodes(Range NodeRange, BOOL IncludeSubtreeSize, BOOL TellSubtree)
{
	Node* Current = NodeRange.FindFirst(); 
	while (Current != NULL)
	{
		Node* Next = NodeRange.FindNext(Current);
		if (!DoHideNode(Current, IncludeSubtreeSize, NULL, TellSubtree))
		{
			return FALSE; 	 // Failure 
		} 
		Current = Next; 
	}
	return TRUE; // Success
}				     



/********************************************************************************************

>	BOOL UndoableOperation::DoHideComplexRange(Range& RangeToHide)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95
	Inputs:		RangeToHide = the current range of nodes which need hiding
	Outputs:	RangeToHide = the updated range of nodes which need hiding
	Returns:	TRUE then all necessary objects were hidden correctly
				FALSE if failed to hide one of the objects

    Purpose:    This function is a pre process to DoHideNodes(). It calls each object in
				the range to hide themselves in a complex way. It may be that no objects
				in the selection require this facility, which means the RangeToHide object
				will be returned from this function unaffected. DoHideNodes() will then
				go through hidding all nodes in the range itself. If however a node responds
				to ComplexHide() and hides itself, the node will be removed from the range.
				This allows complex group nodes to control how they are hidden.
				Note, it is expected that ComplexHide() responders will hide nodes themselves
				This will have the automatic effect of removing them from the range. Hidden
				nodes do not appear in ranges.

********************************************************************************************/

BOOL UndoableOperation::DoHideComplexRange(Range& RangeToHide)
{
	BOOL Failed=FALSE;		
	Node *Next, *Current = RangeToHide.FindFirst();

	while (Current!=NULL)
	{
		// set the next node pointer.
		Next=RangeToHide.FindNext(Current);
		INT32 success = Current->ComplexHide(this, Next);
		// success=-1 then error unable to hide node
		// success= 0 then ignore
		// success= 1 then node has been hidden
		Failed=(success<0);
		if (Failed)
			break;

		Current=Next;
	}

	return (!Failed);
}



/********************************************************************************************
                          
> 	BOOL UndoableOperation::DoMoveNodes(Range NodeRange, 
							    		Node* Destination, 
							    		AttachNodeDirection Direction)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/93
	Inputs:		NodeRange:		The node range to be moved
				Destination:    The context node
				Direction:		The direction Node is to be attached to Destination    

	Outputs:	-
	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 
				
	Purpose:	This high level function moves the range of nodes from their current position
				in the tree, and attaches them to the Destination node in the direction 
				specified by Direction. 

				The nodes in the range maintain their relative z-order when they move.
				    
	Errors:		-
	SeeAlso:	UndoableOperation::DoMoveNode 

********************************************************************************************/

BOOL UndoableOperation::DoMoveNodes(Range NodeRange, 
									Node* Destination, 
				 					AttachNodeDirection Direction)   
{
	Node* Current = NodeRange.FindFirst(); // Get the first node to move
	ENSURE(Current != NULL, "Trying to move an empty range");
	
	Node* NextNode = NodeRange.FindNext(Current);

	// Attach the first node in the range to the Destination node in the direction specified
	if (!DoMoveNode(Current, Destination, Direction))
	{
		return FALSE; // Failed to move Current 
	}
	
	Node* PrevNode = Current; 

	// Get the next node in the range  
	Current = NextNode; 

	// Attach all other nodes in the range 
	while (Current != NULL)
	{
		NextNode = NodeRange.FindNext(Current);
		if (!DoMoveNode(Current, PrevNode, NEXT))
		{
			return FALSE; // Failed to move Current 
		}	
		PrevNode = Current; 
		Current = NextNode;  
	}
	return TRUE; 
}

/********************************************************************************************
                          
> 	BOOL UndoableOperation::DoMoveNode(Node* NodeToMove, 
							   		   Node* Destination, 
							   		   AttachNodeDirection Direction)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/93
	Inputs:		NodeToMove:		The node to be moved, cannot be an attribute
				Destination:    The context node
				Direction:		The direction Node is to be attached to Destination    

	Outputs:	-
	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 
				
	Purpose:	This high level function moves NodeToMove from its current position in the 
				tree, and attaches it to Destination in the direction specified by Direction.

				Note: It is illegal to move an attribute in the tree. This is because it may
					  be deleted as a result of attribute optimisation. It is unlikely that
					  you would want to move an attribute anyway.An ERROR2 will be generated 
					  if this occurs.
				Update, 22/03/2005:
						It is legal to move an effect attribute because they are not optimised
						in the same way as other attributes.
				    
	Errors:		-
	SeeAlso:	UndoableOperation::DoMoveNodes 

********************************************************************************************/

BOOL UndoableOperation::DoMoveNode(Node* NodeToMove, 
  						   		   Node* Destination, 
						  		   AttachNodeDirection Direction) 

{  
	ERROR2IF(NodeToMove->IsAnAttribute() && !((NodeAttribute*)NodeToMove)->IsEffectAttribute(), FALSE, "Trying to move an attribute, this is bad"); 

	// --------------------------------------------------------------------
	// We have to move attributes by copying...
	if (NodeToMove->IsAnAttribute())
	{
		// Copy the attribute to the destination location
		// Then hide the original

		// Copy
		NodeAttribute* pCopyAttr = NULL;
		BOOL bOK = FALSE;
		CALL_WITH_FAIL(((NodeAttribute*)NodeToMove)->NodeCopy((Node**)&pCopyAttr), this, bOK);
		if (!bOK) return FALSE; // No room to take a copy of the node

		// Create a hide node action to hide the node when we undo 
		pCopyAttr->AttachNode(Destination, Direction);
		HideNodeAction* UndoHideNodeAction;
		if (HideNodeAction::Init(this, this->GetUndoActions(), pCopyAttr, TRUE, (Action**)(&UndoHideNodeAction))
				== AC_FAIL)
		{
			pCopyAttr->CascadeDelete();
			delete pCopyAttr;
			return FALSE;
		}

		// Hide
		if (!DoHideNode(NodeToMove, TRUE, NULL, TRUE))
			return FALSE;

		return TRUE;
	}

	// --------------------------------------------------------------------
	// Objects are physically moved, leaving a NodeHidden in the original location
	// with a "hyperspace" reference to the object in its final location.
	//
	// We must flag that we are only moving the node, so we don't
	// try and called the 'Hiding Node' function, which should only
	// be called when we are 'Really' hiding the node.
	MovingNode = TRUE;

	// Find out if the node is selected
	BOOL IsSelected = NodeToMove->IsSelected();

	// Before we move the object determine what the source layer is
	Layer* pSrcLayer = NULL; 
	if (NodeToMove->IsAnObject())
	{
		// Only consider ink nodes
		pSrcLayer = (Layer*) (NodeToMove->FindParent(CC_RUNTIME_CLASS(Layer)));
	} 

	// Hide it
	HideNodeAction* UndoHideNodeAction;  
//	NodeRenderableBounded* pBounded = NULL;

	// hide the node
	if (!DoHideNode(NodeToMove, FALSE, NULL, FALSE)) 
		goto Failed; 
	
	// build the action
	if ( HideNodeAction::Init(this, 
			 				  &UndoActions, 
							  NodeToMove,
							  FALSE, 							// Don't include the 
							  									// subtree size 
							  (Action**)(&UndoHideNodeAction),
							  
							  FALSE)							// Don't tell subtree
							   == AC_FAIL)  
	{   
		goto Failed; 
	};  

	// Move the node to its new location
	NodeToMove->MoveNode(Destination, Direction);
	if (IsSelected)
		NodeToMove->SetSelected(IsSelected);

	MovingNode = FALSE;

	if (NodeToMove->IsAnObject())
	{
		// If the object has moved to a new layer then we need to invalidate both old and new 
		// layers
		Layer* pDestLayer = (Layer*) (NodeToMove->FindParent(CC_RUNTIME_CLASS(Layer)));
		if ((pSrcLayer != pDestLayer) && pSrcLayer && pDestLayer)
		{
			pSrcLayer->InvalidateBoundingRect();
			pDestLayer->InvalidateBoundingRect();
		}
		
		if (pSrcLayer)
		{
			// Note that this layer has been edited
			pSrcLayer->SetEdited(TRUE);
//#ifdef _DEBUG
//			// Tell the frame gallery to update its display of the frame
//			BROADCAST_TO_ALL(LayerMsg(pSrcLayer, LayerMsg::LayerReason::REDRAW_LAYER));
//#endif
		}

		if (pDestLayer && (pSrcLayer != pDestLayer))
		{
			// Note that this layer has been edited
			pDestLayer->SetEdited(TRUE);
//#ifdef _DEBUG
//			// Tell the frame gallery to update its display of the frame
//			BROADCAST_TO_ALL(LayerMsg(pDestLayer, LayerMsg::LayerReason::REDRAW_LAYER));
//#endif
		}

	}
	// finish happy
	return (TRUE);

Failed:
	MovingNode = FALSE;		// Ensure we clear this is an error occurred 
	return FALSE;
}


					

/********************************************************************************************

>	BOOL UndoableOperation::DoSaveCopyOfNodes(Range NodeRange)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/94
	Inputs:		NodeRange: The range of nodes to save 
	Outputs:	-
	Returns:	-
	Purpose:	This fn takes a copy of the nodes in the range. The copies are restored when 
				the operation is undone. 
	
	Errors:		-
	SeeAlso:	UndoableOperation::DoSaveCopyOfNode

********************************************************************************************/

BOOL UndoableOperation::DoSaveCopyOfNodes(Range NodeRange) 
{
	Node* Current = NodeRange.FindFirst();
	BOOL CopiedOK;
	Node* TheCopy;  
	HideNodeAction* UndoHideNodeAction; 
	while (Current != NULL)
	{	
		// Salary check
		if (!Current->IsBounded())
		{
			ERROR3("Current is not a NodeRenderable node");
			continue;
		}

		// Make a copy of Current 
		CALL_WITH_FAIL(Current->NodeCopy(&TheCopy), this, CopiedOK);  
		if (!CopiedOK)
		{
			return FALSE; // No room to take a copy of the node 
		}

		// We need to insert the new node into the tree, but we can't use DoInsertNewNode
		// as this will insert an unwanted NideNode action
		TheCopy->AttachNode(Current, PREV); 
		TheCopy->SetSelected(FALSE); 
		((NodeRenderableBounded*)TheCopy)->ValidateBoundingRect();
		((NodeRenderableBounded*)TheCopy)->InvalidateBoundingRect();

		// Hide the copy, creating a ShowNodeAction to show the copy when we undo 
		if (!DoHideNode(TheCopy, 
				   	    TRUE 		// The hidden node is in the operation history
				       )
		   )
		{
			return FALSE; 	
		} 

		// Create a Hide node action to hide the current node when we undo
		if ( HideNodeAction::Init(this, 
		 						  &UndoActions, 
								  Current, 
								  FALSE, 
			 					  (Action**)(&UndoHideNodeAction))
			== AC_FAIL)  
		{   
			return FALSE; 	   
		};  
    
		Current = NodeRange.FindNext(Current); 
	}
	return TRUE; 
} 

/********************************************************************************************

>	BOOL UndoableOperation::DoSaveCopyOfNode(NodeRenderable* Node)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/94
	Inputs:		Node: 
	Outputs:	-
	Returns:	-
	Purpose:	This fn saves a copy of the node in the operation history. When the operation
				is undone the copy is restored.  
	
	Errors:		-
	SeeAlso:	UndoableOperation::DoSaveCopyOfNodes

********************************************************************************************/

BOOL UndoableOperation::DoSaveCopyOfNode(NodeRenderable* Node)
{
	ENSURE((!OpFlags.Failed), "Calling a DO function after operation has failed");
	RangeControl		rc( TRUE, TRUE, TRUE ); // All nodes in range
	Range				range( Node, Node, rc );
	return DoSaveCopyOfNodes( range ); 
}

/********************************************************************************************

>	BOOL UndoableOperation::DoTransformNodes(Range NodeRange, 
									 TransformBase* Trans)   

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/93
	Inputs:		Range: The range of nodes to transform 
				Trans: The transform to apply to every node in the range 
	Outputs:	-
	Returns:	TRUE if successful 

				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 
				The Trans is deleted. 
				
	Purpose:	This high level Do function applies the transform Trans to each 
				object in the range. 
				
				Don't delete the Trans object, it's deleted in TransformNodeAction slaughter
				method. 
	           
	Errors:		-
	SeeAlso:	UndoableOperation::DoTransformNode


********************************************************************************************/

BOOL UndoableOperation::DoTransformNodes(Range NodeRange, 
								 		 TransformBase* Trans)
{
	// If the Trans is invertible then create a Transform node action to undo the transform
	// Note at this point Trans is not the inverse Transformation, but it will be soon !!
//	Node * pParent = NULL;
//	Node * pNodeToTransform = NULL;

	// DMc 17/5/99
	// include the parent nodes in the range if they wish to be transformed with
	// their children by changing the range control flags	
	RangeControl rg = NodeRange.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	NodeRange.Range::SetRangeControl(rg);

//	DoInvalidateNodesRegions(NodeRange, TRUE);
	DoInvalidateNodesRegions(NodeRange, TRUE, FALSE, FALSE, FALSE);	// Don't forcibly recache (see bHaveTransformedAllCached)

	if (Trans->IsInvertable()) 
	{
		TransformNodeAction* UndoTransformNodeAction; 

		if ( TransformNodeAction::Init(this, 
			 				  	 	   &UndoActions, 
								  	   NodeRange, 
								   	   Trans,
									   NULL,
							  	   	   (Action**)(&UndoTransformNodeAction))
							   		   == AC_FAIL) 
		 
		{   
			delete Trans; // for consistency 
			return FALSE; // Let's get out while we can 	   
		}; 
		
		// Scan the range and transform each node 
		Node* Current = NodeRange.FindFirst();
		Node* Next; 
//		Node* Parent = NULL;
//		BOOL bTransform = FALSE;
		while (Current != NULL)
		{
			Next = NodeRange.FindNext(Current); 

			if (Current->IsNodeRenderableClass())
			{
				// Prepare Control and Feedback flags
				Trans->bHaveTransformedCached = TRUE;
				Trans->bHaveTransformedChildren = TRUE;
				Trans->bTransformYourChildren = TRUE;

				((NodeRenderable *)Current)->Transform(*Trans);

				// Compute feedback flags for caller
				Trans->bHaveTransformedAllCached = Trans->bHaveTransformedAllCached && Trans->bHaveTransformedCached;
				Trans->bHaveTransformedAllChildren = Trans->bHaveTransformedAllChildren && Trans->bHaveTransformedChildren;

				// ----------------------------------------------
				// Now do smart testing to see whether we can retain any cached info the node was holding
				if (Current->IsBounded())
				{
					NodeRenderableBounded* pCurrentBound = (NodeRenderableBounded*) Current;

					// If we have transformed all cached data successfully
					// And 
					//		We are translating (in which case the cached data will remain usable)
					//		Or
					//		The node was Direct (in which case the cached data comes from outside the tree where it can't have been transformed)
					// Then
					//		Don't force the recacheing of the data for this node, just its parents
					if (Trans->bHaveTransformedCached && (Trans->IsTranslation() || pCurrentBound->HasCachedDirectBitmap()))
						// Yes, we can keep our cached info but we must tell our parents we have changed
						pCurrentBound->ReleaseCached(TRUE, FALSE, FALSE, TRUE);	// Parents and derived data only
					else
						// No we can't keep our cached info
						pCurrentBound->ReleaseCached(TRUE, TRUE);
				}
				// ----------------------------------------------
			}
			
			// JCF BODGE
		#ifdef _DEBUG
			else
			{
				TRACE( _T("Warning: UndoableOperation::DoTransformNodes: ")
					  _T("Current node %s is not a NodeRenderableInk\n"),
						(LPCTSTR) Current->Name());
			}
		#endif

			Progress::Update();
			Current = Next;  
		}

		// Invert the Trans which is in the TransformNodeAction
		((TransInvertable*)Trans)->Invert(); 
	}
	else // The Trans is not invertable
	{
		// We will have to go through all node in the range, making a copy and transforming it.
		// This used to use	DoSaveCopyOfNodes, but this dosen't work as it is not completly undoable
		Node* Current = NodeRange.FindFirst();
		Node* Next = NULL; 
		while (Current != NULL)
		{
			Next = NodeRange.FindNext(Current);
			if (Current->IsNodeRenderableClass())
			{
				// Make a copy of Current 
				BOOL CopiedOK = TRUE;
				Node* TheCopy = NULL;
				CALL_WITH_FAIL(Current->NodeCopy(&TheCopy), this, CopiedOK);  
				if (!CopiedOK)
					return FALSE;

				// transform the copy
				((NodeRenderable*)TheCopy)->Transform(*Trans);

				// put the copy into the tree
				if (!DoInsertNewNode((NodeRenderableBounded*)TheCopy, Current, PREV, TRUE, FALSE))
				{
					delete TheCopy;
					return FALSE;
				}

				// Hide the original
				if (!DoHideNode(Current, FALSE))
					return FALSE;
			}

			// JCF BODGE
		#ifdef _DEBUG
			else
			{
				TRACE( _T("Warning: UndoableOperation::DoTransformNodes: ")
					  _T("Current node %s is not a NodeRenderableInk\n"),
						(LPCTSTR) Current->Name());
			}
		#endif

			Progress::Update();
			Current = Next;  
		}
	}
	return (TRUE); // Success 
}   

/********************************************************************************************
                          
> 	BOOL UndoableOperation::DoTransformNode(NodeRenderableInk* NodeToTransform, 
											TransformBase* Trans);   

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/93
	Inputs:		NodeToTransform: The node to transform 
				Trans: The transform to apply to the node 
	Outputs:	-
	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 
				
	Purpose:	This high level Do function applies the transform Trans to the 
				NodeToTransform
	           
	Errors:		-
	SeeAlso:	UndoableOperation::DoTransformNodes

********************************************************************************************/

BOOL UndoableOperation::DoTransformNode(NodeRenderableInk* NodeToTransform, 
								TransformBase* Trans)
{
	ENSURE((!OpFlags.Failed), "Calling a DO function after operation has failed");
//	RangeControl rc = { TRUE, TRUE, TRUE }; // All nodes in range
	Range				range( NodeToTransform, NodeToTransform, RangeControl( TRUE, TRUE, TRUE ) );
	return( DoTransformNodes( range, Trans ) ); 
}   


/********************************************************************************************

>	BOOL UndoableOperation::DoSelectNode(NodeRenderableInk* NodeToSelect)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/93
	Inputs:		NodeToSelect: The node that is to be selected 
	Outputs:	-
	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 
	Purpose:	The function selects NodeToSelect. It also generates an action which will 
				deselect the node when executed.  
	Errors:		-
	SeeAlso:	UndoableOperation::DoDeselectNode

********************************************************************************************/

BOOL UndoableOperation::DoSelectNode(NodeRenderableInk* NodeToSelect, Spread *Parent)
{   
	SelectDeselectAction* UndoSelectDeselectAction; 
	// Create an action to deselect the NodeToSelect when we undo. 
	if ( SelectDeselectAction::Init(this, 
			 		 				&UndoActions, 
					 				NodeToSelect,
									Parent,
					 				(Action**)(&UndoSelectDeselectAction))
					 				!= AC_FAIL)  
	{   
		// We shall be able to deselect the object if we fail so select it.       
		NodeToSelect->Select(FALSE);  
		return (TRUE);      
	}
	else 
		return (FALSE); 
} 

/********************************************************************************************

>	BOOL UndoableOperation::DoInvalidateNodesRegions(Range NodeRange, BOOL IncludeBlobs, 
												 	 BOOL UndoBlobs = FALSE,
										   		     BOOL IfBgRedraw = FALSE,
													 BOOL bForceRecache = TRUE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/2/94
	Inputs:		NodeRange: 		The range of nodes  
				IncludeBlobs:	When TRUE we take the bounding rectangle of the objects 
								and their blobs. 
				UndoBlobs:		TRUE, Invert the 'IncludeBlobs' for undo/redo this op.
								FALSE, use the same 'IncludeBlobs' for undo/redo.
								
	Outputs:	-
	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 

	Purpose:	This low level Do function invalidates a region which is the union of
				the bounding rectangles of each node in the range. When the includeBlobs
				flag is TRUE the region which gets invalidated includes the objects blobs.  
				
				constraints: 

				All members of the range must be on the same spread
				No member of the range can be hidden 
				
	Errors:		An ENSURE will occur if the range of nodes is empty, in a retail build 
				the function will return TRUE but do nothing. 

	SeeAlso:	UndoableOperation::DoInvalidateNodeRegion

********************************************************************************************/

BOOL UndoableOperation::DoInvalidateNodesRegions(Range NodeRange, BOOL IncludeBlobs, 
												 BOOL UndoBlobs,
												 BOOL IfBgRedraw,
												 BOOL bForceRecache)
{
	ENSURE((!OpFlags.Failed), "Calling a DO function after operation has failed"); 

	if (UndoBlobs)
	{
		// Undo blobs = TRUE
		// We will use different 'IncludeBlobs' when we undo/redo this op
		// to the one we use initially.
		UndoBlobs = !IncludeBlobs;
	}
	else
	{
		// Undo blobs = FALSE
		// We will use the same 'IncludeBlobs' when we undo/redo this op
		// as the one we use initially.
		UndoBlobs = IncludeBlobs;
	}

	BOOL bOldValue = NodeRange.SetPromoteToParent(TRUE);
	NodeRenderableBounded* CurrentNode = (NodeRenderableBounded*)NodeRange.FindFirst(); 
	NodeRange.SetPromoteToParent(bOldValue);

	if (CurrentNode == NULL)
		return TRUE; // The range is empty so NOP

	// ENSURE the range of nodes is not empty
//	ENSURE (CurrentNode != NULL, "Calling DoInvalidateNodesRegions with an empty range"); 

//	ENSURE(CurrentNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderable)), 
//			"Trying to invalidate a non NodeRenderable node");

  	// Find the spread 
	Spread* pSpread = (Spread*)CurrentNode->FindParent(CC_RUNTIME_CLASS(Spread));



	// Matt 27/11/2000
	// OK, here's the beef... At present, if you group a shadowed object, the tree structure correctly
	// reflects this as a NodeShadowController with a NodeGroup parent... Now, when we've clicked on the
	// 'Set New Design' button, the NodeGroup is passed OK and is hidden after being copied (as was intended)
	// but the NodeShadowController beneath this is then tested: When asked what it's parent spread is, it
	// will promptly keel over, triggering the following ENSURE as its parent (the NodeGroup) no longer has
	// a pointer to its parent (ie it is NULL) - to cater for this, if we reach this point and we have no
	// parent spread, we can assume one of two things:
	// 1.	The node above us has been hidden, therefore we are already implicitly hidden and we shouldn't
	//		undertake any special work
	// 2.	Everything is stuffed so bad in our tree that it really doesn't matter what we do here - we're
	//		about to die anyhow...
	// Therefore, we can just return without failing with an ensure...
	//
	// ENSURE(pSpread != NULL, "Spread is NULL"); 



	if (pSpread)
	{
		if (IfBgRedraw)
		{
			// -------------------------------------------------------------------------------------
			// Try and create an InvalidateRegionAction 
			InvalidateRegionIfBgRedrawAction* UndoInvalidateRgnAction;                          
   
			// Create an InvalidateRegionAction  
			// Note that unfortunatly the action has to store the node range, rather than the 
			// bounding rectangle because GetBlobBoundingRect must be calculated on the fly. 

			if ( InvalidateRegionIfBgRedrawAction::Init(this,                    
											  &UndoActions,
											  NodeRange, 
											  pSpread,  	
											  UndoBlobs,
							 				  ( Action**)(&UndoInvalidateRgnAction)) == AC_FAIL)  
			{
				return FALSE;  // Failed to create action 
			}  
		
			if (!GetApplication()->IsBgRendering())
			{
				return TRUE;	// Ignore if no Bg Rendering in progress
			}

	//		GetApplication()->DeleteRenderRegions(DocView::GetSelected());
	//		GetApplication()->BgRendering = FALSE;
		}
		else
		{
			// -------------------------------------------------------------------------------------
			// Try and create an InvalidateRegionAction 
			InvalidateRegionAction* UndoInvalidateRgnAction;                          
   
			// Create an InvalidateRegionAction  
			// Note that unfortunatly the action has to store the node range, rather than the 
			// bounding rectangle because GetBlobBoundingRect must be calculated on the fly. 

			if ( InvalidateRegionAction::Init(this,                    
											  &UndoActions,
											  NodeRange, 
											  pSpread,  	
											  UndoBlobs,
							 				  ( Action**)(&UndoInvalidateRgnAction)) == AC_FAIL)  
			{
				return FALSE;  // Failed to create action 
			}  
		}

		ENSURE(pOurDoc != 0, "There was no document in DoInvalidateNodesRegions");

		// Invalidate the bounds of each node in the range

		if (pOurDoc != NULL)
		{
			DocRect TempRect; 

			BOOL bOldValue = NodeRange.SetPromoteToParent(TRUE);
			while (CurrentNode != NULL)
			{
				// Because we had a Range param, we could possibly be given any legal node in the tree,
				// e.g. the Insert node, not just renderable bounded objects, hence this 'if' statement
				if (CurrentNode->IsAnObject() || CurrentNode->IsPaper())
				{
					ENSURE(CurrentNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded)), 
						"Trying to invalidate a non NodeRenderable node");

					TempRect = (IncludeBlobs ? 
									(CurrentNode->GetUnionBlobBoundingRect()):
									(CurrentNode->GetBoundingRect())
								 );
TempRect = TempRect.Union(CurrentNode->GetEffectStackBounds());

					// Accumulate this invalid region in a pending list until the Op
					// finishes
					// Mark the region as requiring object to recache any cached
					// data they may be holding because the Op may have changed it
//					pOurDoc->ForceRedraw(pSpread, TempRect, TRUE, bForceRecache ? CurrentNode : NULL);
					pOurDoc->ForceRedraw(pSpread, TempRect, TRUE, CurrentNode, bForceRecache);
				}

				CurrentNode = (NodeRenderableBounded*) NodeRange.FindNext(CurrentNode); 
			}
			NodeRange.SetPromoteToParent(bOldValue);
		}
	}

	// Now invalidate the region
	return TRUE;  
}

/********************************************************************************************

>	BOOL UndoableOperation::DoInvalidateNodeRegion(NodeRenderableBounded* Node, 
										   		   BOOL IncludeBlobs,
										   		   BOOL UndoBlobs,
										   		   BOOL IfBgRedraw,
												   BOOL bForceRecache)
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/2/94
	Inputs:		Node: 			The node that needs to be invalidated    
				IncludeBlobs:	When TRUE we take the blob bounding rectangle of the node 
				UndoBlobs:		TRUE, Invert the 'IncludeBlobs' for undo/redo this op.
								FALSE, use the same 'IncludeBlobs' for undo/redo.
								
	Outputs:	-
	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 

	Purpose:	Invalidates the nodes bounding rectangle,  or blob bounding rectangle 
				
	Errors:		-
	SeeAlso:	UndoableOperation::DoInvalidateNodesRegions

********************************************************************************************/

BOOL UndoableOperation::DoInvalidateNodeRegion(NodeRenderableBounded* Node, 
									   		   BOOL IncludeBlobs,
									   		   BOOL UndoBlobs,
									   		   BOOL IfBgRedraw,
											   BOOL bForceRecache)
{	
	ENSURE((!OpFlags.Failed), "Calling a DO function after operation has failed");

	// Range = Sel nodes, Unsel nodes, cross layers, Locked layers, Ignore Non-renderables, Ignore invisible layers
	RangeControl		rc( TRUE, TRUE, TRUE, FALSE, TRUE, TRUE );
	Range				range( Node, Node, rc );

	return( DoInvalidateNodesRegions( range, IncludeBlobs, UndoBlobs, IfBgRedraw, bForceRecache ) ); 
}



/********************************************************************************************

>	BOOL UndoableOperation::DoInvalidateRegion(Spread* pSpread, DocRect& InvalidRegion)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/8/94
	Inputs:		pSpread - The spread that the region to invalidate is on
				InvalidRegion - The region that you want to invalidate
	Returns:	TRUE if it all worked, FALSE if not (TIDYUP THEN CALL End()!) 
	Purpose:	Invalidates the region specified and adds the appropriate action to the
				action list for the current operation. If you are invalidating a region
				associated with a node in the tree then you should be using
				DoInvalidateNodeRegion() or DoInvalidateNodesRegion().
	SeeAlso:	UndoableOperation::DoInvalidateNodeRegion

********************************************************************************************/

BOOL UndoableOperation::DoInvalidateRegion( Spread *pSpread, const DocRect &InvalidRegion )
{
	// make sure that things are worth doing
	ENSURE((!OpFlags.Failed), "Calling a DO function after operation has failed");
	ENSURE(pSpread!=NULL, "Attempt to invalidate a region on a null spread.");

	// If the region is empty we could save everyone a lot of trouble
	if (InvalidRegion.IsEmpty())
		return TRUE;

	// Try and create an InvalidateArbitaryRegionAction and fail if it does not work
	InvalidateArbitaryAction* pAction;
	ActionCode ActCode = InvalidateArbitaryAction::Init( this, &UndoActions, pSpread,
						 InvalidRegion, (Action**)(&pAction));
   	
   	// See if it worked
   	if (ActCode==AC_FAIL)
		return FALSE;

	// Now we can invalidate the region for the caller
	// First we need a document
	ENSURE(pOurDoc != NULL, "There was no document in DoInvalidateRegions");

	// Invalidate the bounds of each node in the range
	if (pOurDoc != NULL)
		// Accumulate this invalid region in a pending list until the Op
		// finishes
		// Mark the region as requiring object to recache any cached
		// data they may be holding because the Op may have changed it
		pOurDoc->ForceRedraw(pSpread, InvalidRegion, TRUE, pSpread);

	// Tell them that everything worked out just fine
	return TRUE;
}

/********************************************************************************************

>	BOOL UndoableOperation::DoDeselectNode(NodeRenderableInk* NodeToDeselect)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/93
	Inputs:		NodeToDeselect: The node that is to be de-selected 
	Outputs:	-
	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 
	Purpose:	The function de-selects NodeToDeselect. It also generates an action which will 
				select the node when executed.  
	Errors:		-
	SeeAlso:	UndoableOperation::DoDeselectNode

********************************************************************************************/

  
BOOL UndoableOperation::DoDeselectNode(NodeRenderableInk* NodeToDeselect, Spread *Parent)
{   
	SelectDeselectAction* UndoSelectDeselectAction; 
	// Create an action to deselect the NodeToSelect when we undo. 
	if ( SelectDeselectAction::Init(this, 
			 		 				&UndoActions, 
					 				NodeToDeselect,
									Parent,
					 				(Action**)(&UndoSelectDeselectAction))
					 				!= AC_FAIL)  
	{                   
		// We shall be able to select the object if we fail so deselect it. 
		NodeToDeselect->DeSelect(FALSE);  
		return (TRUE);      
	}               
	else
		return (FALSE); 
} 

/********************************************************************************************

>	BOOL UndoableOperation::DoMakeShapes(Range& NodeRange)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/05/94

	Inputs:		NodeRange: The Range of nodes to make into shapes 
	  
	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 
	Purpose:	This low level Do function calls the DoBecomeA virtual function on all 
				SelectedNodes. It is the node's DoBecomeA function which is responsible
				for generating actions. 
	Errors:		-
	SeeAlso:	Node::DoBecomeA

********************************************************************************************/

BOOL UndoableOperation::DoMakeShapes(Range NodeRange)
{
	Node* CurrentNode = NodeRange.FindFirst();
	Node* Next; 

	while (CurrentNode != NULL)
	{
// BODGE - since the group is selected and won't be replaced by anything else, no need to reselect it
// this fixes a bug where make shapes on grouped text stories/molds/blends don't leave the parent group
// selected but selects the objects inside!
		BOOL reselect = !IS_A(CurrentNode,NodeGroup) && !IS_A(CurrentNode,NodeBlend);

		BecomeA BecomeAPath(BECOMEA_REPLACE,CC_RUNTIME_CLASS(NodePath), this, reselect);
		Next = NodeRange.FindNext(CurrentNode);
		if (!(CurrentNode->DoBecomeA(&BecomeAPath)))
		{
			// Operation failed
			return FALSE; 
		} 
		CurrentNode = Next; 
	}
	return TRUE; // Success
}

/********************************************************************************************

>	BOOL UndoableOperation::DoCopyNodesToClipboard(Range NodeRange)

	
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/05/94

	Inputs:		NodeRange: The Range of nodes to copy
	  
	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 

	Purpose:	This Do function copies all nodes in the range to the internal clipboard.
				The objects are made attribute complete prior to being moved.

				Before the copy begins StartComponentCopy is called on each DocComponent in
				the clipboard.
				
				The nodes are then copied to the clipboard
				
				CopyComponentData is then called on each node which has been copied to allow
				component data to be copied to the clipboard.

				In the operations End method: If the operation has not failed then 
				EndComponentCopy is called on each doc component do give them a chance to 
				commit changes. If the operation has failed then AbortComponentCopy is called
				instead.

	Errors:		-
	SeeAlso:	NodeRenderableInk::MakeAttributeComplete


********************************************************************************************/

BOOL UndoableOperation::DoCopyNodesToClipboard(Range NodeRange)
{
	BOOL CopiedOK;
	BOOL ok;

	InternalClipboard* pInternalClip = InternalClipboard::Instance();

	// After the operation ends we will need to inform all DocComponents in the clipboard
	// of the outcome.
	InformDocComponentsOfOperationsOutcome(pInternalClip); 


	// Inform all DocComponents in the clipboard that a copy is about to take place
	CALL_WITH_FAIL(pInternalClip->StartComponentCopy(), this, ok)
	if (!ok)
	{
		// Start Component copy has failed so abort operation
		// Note that AbortComponentCopy will get called in the ops end method
		return FALSE;
	} 

	// Try to copy the selection to the clipboard
	CALL_WITH_FAIL(InternalClipboard::CopyObjects(NodeRange, this), this, CopiedOK)

	if (!CopiedOK)
	{
		return FALSE; 
	}

	// Now try and copy accross the component data

	CALL_WITH_FAIL(InternalClipboard::CopyComponentData(pOurDoc), this, CopiedOK)

	if (!CopiedOK)
	{
		return FALSE; 
	}


	// Create action, this action will do nothing for undo but it's twin redo action will
	// copy the selection to the clipboard.
	CopyObjectsToClipboardAction* UndoCopyObjectsToClipboardAction;                          

	if ( CopyObjectsToClipboardAction::Init(this, 
						   			   &UndoActions, 
						   			   NodeRange,  
						  			  ( Action**)(&UndoCopyObjectsToClipboardAction)) == AC_FAIL)  
	{
		return FALSE;  // Failed to create action 
	} 
 	return TRUE; // Success
}

/********************************************************************************************

>	BOOL UndoableOperation::DoFlattenRange(Range NodeRange) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Not implemented yet
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL UndoableOperation::DoFlattenRange(Range NodeRange) 
{
	return FALSE;
}

/********************************************************************************************

	BOOL UndoableOperation::DoRemoveAttrTypeFromSubtree(Node* Subtree, RuntimeClass* AttrType, Node* pExceptThis = NULL);

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/94
	Inputs:		Subtree:		Subtree to remove nodes from
				NodeClass:		The type of attribute to remove from the subtree
				pExceptThis:	The node that shouldn't be removed!

	Outputs:	-
	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 

	Purpose:	Searches the subtree and every attribute which has type AttrType
				is hidden.
	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/

BOOL UndoableOperation::DoRemoveAttrTypeFromSubtree(Node* Subtree, CCRuntimeClass* AttrType, Node* pExceptThis)
{
	// Traverse the subtree depth first
	Node* Current = Subtree->FindFirstDepthFirst();
	Node* Next;
	while (Current != NULL)
	{
		Next =  Current->FindNextDepthFirst(Subtree); 
		// Determine if the Current node is to be hidden
		if (Current!=pExceptThis && Current->IsKindOf(CC_RUNTIME_CLASS(NodeAttribute)))
		{
			if (((NodeAttribute*)Current)->GetAttributeType() == AttrType)
			{
				if (Current->FindParent()->DiscardsAttributeChildren())
				{
					Current->CascadeDelete();
					delete Current;
				}
				else
				{ 
					if(!DoHideNode(Current, 
								   TRUE			// Include the subtree size 
								   ))
					{
						return FALSE;			// Operation failed
					} 
				}
			}
		}
		Current = Next;
	}
	return TRUE; 
}


/********************************************************************************************

>	BOOL UndoableOperation::DoChangeSelection(NodePath* ThisNode, INT32 Index, BOOL NewState)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		ThisNode points at the NodePath whose state we want to change
				Index is the element we want to change
				NewState is the new state of selection we want to set
	Outputs:	-
	Returns:	TRUE if succeeded, FALSE otherwise
	Purpose:	This 'Do' function changes the selected state of a particular element in a path
				it creates all the necessary undo information so that the selection can be undone
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL UndoableOperation::DoChangeSelection(NodePath* ThisNode, INT32 Index, BOOL NewState)
{
#ifndef STANDALONE

	ModifyFlagsAction* UnAction;
	PathFlags* Flags = ThisNode->InkPath.GetFlagArray();
	if (Flags[Index].IsSelected != NewState)
	{
		ActionCode Act = ModifyFlagsAction::Init(this, 
													&UndoActions,
													Flags[Index],
													Index,
													ThisNode,
													(Action**)&UnAction);
		if (Act == AC_FAIL)
			return FALSE;
		
		Flags[Index].IsSelected = NewState;
		return TRUE;
	}

#endif

	return TRUE;
}

/********************************************************************************************

>	BOOL UndoableOperation::DoDeletePathSection(NodePath* ThisPath, INT32 Index, INT32 NumElements, BOOL RedrawPath = TRUE)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		ThisPath points at the NodePath whose state we want to change
				Index is the element we want to change
				NumElements is the number of elements to be deleted
				RedrawPath - TRUE if the path ares should be redrawn, FALSE if the caller will do it
	Outputs:	-
	Returns:	TRUE if succeeded, FALSE otherwise
	Purpose:	This 'Do' function changes the selected state of a particular element in a path
				it creates all the necessary undo information so that the selection can be undone
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL UndoableOperation::DoDeletePathSection(NodePath* ThisPath, INT32 Index, INT32 NumElements, BOOL RedrawPath)
{
#ifndef STANDALONE

	InsertPathElementAction* UnAction = NULL;
	ActionCode Act;
	Act = InsertPathElementAction::Init(this, &UndoActions, NumElements, Index,
											(Action**)(&UnAction));
	if (Act == AC_FAIL)
		return FALSE;

	Document* pDocument = GetWorkingDoc();
	ERROR2IF(pDocument == NULL, FALSE, "There was no Document when deleteing path elements");
	Spread* pSpread = ThisPath->FindParentSpread();
	ERROR2IF(pSpread == NULL, FALSE, "Path had no parent spread");

	// Force a re-draw of the place where the path used to be
	if (RedrawPath)
	{
		DocRect Invalid = ThisPath->GetUnionBlobBoundingRect();
		// Mark the region as requiring object to recache any cached
		// data they may be holding because the Op may have changed it
		pDocument->ForceRedraw(pSpread, Invalid, FALSE, ThisPath);
	}

	if ((Act != AC_NORECORD) && (UnAction != NULL))
	{
		// I have to claim some memory to store the elements I'm deleting...
		PathVerb* ChangedVerbs = NULL;
		DocCoord* ChangedCoords = NULL;
		PathFlags* ChangedFlags = NULL;
		ALLOC_WITH_FAIL(ChangedVerbs,(PathVerb*) CCMalloc(NumElements * sizeof(PathVerb)),this);
		ALLOC_WITH_FAIL(ChangedCoords,(DocCoord*) CCMalloc(NumElements * sizeof(DocCoord)),this);
		ALLOC_WITH_FAIL(ChangedFlags,(PathFlags*) CCMalloc(NumElements * sizeof(PathFlags)),this);

		if (!ChangedVerbs || !ChangedCoords || !ChangedFlags)
		{
			if (ChangedVerbs) CCFree(ChangedVerbs);
			if (ChangedCoords) CCFree(ChangedCoords);
			if (ChangedFlags) CCFree(ChangedFlags);
			return FALSE;
		}

		// Get pointers to all the arrays of the path
		PathVerb* Verbs = NULL;
		DocCoord* Coords = NULL;
		PathFlags* Flags = NULL;
		ThisPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);

		// Now copy the data from the path into the arrays
		for (INT32 i=0;i<NumElements;i++)
		{
			ChangedVerbs[i] = Verbs[Index+i];
			ChangedCoords[i] = Coords[Index+i];
			ChangedFlags[i] = Flags[Index+i];
		}

		// Now pass these arrays to the Insert action
		UnAction->RecordPath(ChangedVerbs, ChangedFlags, ChangedCoords, ThisPath);
	}

	// Now we've recorded the data we're about to delete, let's delete it
	ThisPath->InkPath.DeleteSection(Index, NumElements);

	if (RedrawPath)
	{
		DocRect Invalid = ThisPath->GetUnionBlobBoundingRect();
		// Accumulate this invalid region in a pending list until the Op
		// finishes
		// Mark the region as requiring object to recache any cached
		// data they may be holding because the Op may have changed it
		pDocument->ForceRedraw(pSpread, Invalid, FALSE, ThisPath);
	}

#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL UndoableOperation::DoAlterPathElement(	NodePath* ThisPath, 
												INT32 Index, 
												DocCoord NewCoord,
												PathFlags NewFlags,
												PathVerb NewVerb);


	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/94
	Inputs:		ThisPath points at the NodePath whose state we want to change
				Index is the element we want to change
				NewCoord is the new coordinate value
				NewFlags is the new flags value
				NewVerb is the new verb value
				RedrawPath - TRUE if the screen area of the path should be invalidated, FALSE if not
	Outputs:	-
	Returns:	TRUE if succeeded, FALSE otherwise
	Purpose:	This 'Do' function changes the selected element, recording undo information
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL UndoableOperation::DoAlterPathElement(	NodePath* ThisPath, 
											INT32 Index, 
											DocCoord NewCoord,
											PathFlags NewFlags,
											PathVerb NewVerb,
											BOOL RedrawPath)
{
#ifndef STANDALONE

	// Here we're changing an element, so we have to change the element in the path,
	// recording undo information at the same time.
	ModifyElementAction* ModAction = NULL;
	
	// Get pointers to all the arrays of the path
	PathVerb* Verbs = NULL;
	DocCoord* Coords = NULL;
	PathFlags* Flags = NULL;
	ThisPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);

	// Create an undo action for this action, which is a ModifyElementAction
	ActionCode Act;
	Act = ModifyElementAction::Init(this, 
									&UndoActions, 
									Verbs[Index],
									Flags[Index],
									Coords[Index],
									Index,
									ThisPath,
									(Action**)(&ModAction));
	if (Act == AC_FAIL)
		return FALSE;

	Document* pDocument = GetWorkingDoc();
	ERROR2IF(pDocument == NULL, FALSE, "There was no Document when deleteing path elements");
	Spread* pSpread = ThisPath->FindParentSpread();
	ERROR2IF(pSpread == NULL, FALSE, "Path had no parent spread");

	// Force a re-draw of the place where the path used to be
	if (RedrawPath)
	{
		DocRect Invalid = ThisPath->GetUnionBlobBoundingRect();
		// Mark the region as requiring object to recache any cached
		// data they may be holding because the Op may have changed it
		pDocument->ForceRedraw( pSpread, Invalid, FALSE, ThisPath );
	}

	// Update the coords
	Verbs[Index] = NewVerb;
	Flags[Index] = NewFlags;
	Coords[Index] = NewCoord;

	// The bounding rect may have changed
	ThisPath->InvalidateBoundingRect();

	// redraw the new area
	if (RedrawPath)
	{
		// redraw the new area
		DocRect Invalid = ThisPath->GetUnionBlobBoundingRect();
		// Mark the region as requiring object to recache any cached
		// data they may be holding because the Op may have changed it
		pDocument->ForceRedraw( pSpread, Invalid, FALSE, ThisPath );
	}

#endif
	return TRUE;
}


/********************************************************************************************

>	BOOL UndoableOperation::DoSmoothNodePath(NodePath* pThisNode, double smoothacc)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		pThisNode - a pointer to a node to smooth.
				smoothacc - the smooth accuracy, usually in the range (256^2, 8000^2)
	Outputs:	a new tree node will be created	as a result
	Returns:	TRUE if the operation completed
				FALSE if the operation failed due to lack of memory. The environment matches
				that on entry to the function.
	Purpose:	This function will smooth a node path given an accuracy. It will create a
				new node in the tree after the node specified and hide node passed if all 
				is succesfull.
				
********************************************************************************************/

BOOL UndoableOperation::DoSmoothNodePath(NodePath* pThisNode, double smoothacc)
{
#ifndef STANDALONE

	// Copy the nodepath and all its children, without placing the copy in the tree
	Node* pnode;
	if (!pThisNode->NodeCopy(&pnode))
		return FALSE;
	NodePath* pSmoothNode = (NodePath*)pnode;

	// ok, smooth the nodepaths path data
	if (!pSmoothNode->InkPath.SmoothRegions(smoothacc, FALSE, TRUE))
		goto smoothfailed;

	// update the smoothed paths bounding rectangle
	pSmoothNode->InvalidateBoundingRect();

	// Now stick the new path into the tree
	if (!DoInsertNewNode(pSmoothNode, pThisNode, NEXT, TRUE))
		goto smoothfailed;
	
	// Now we've formed a smoothed path, let's hide the original
	if (!DoHideNode(pThisNode,TRUE))
		goto smoothfailed;

#endif

	// return all is well
	return TRUE;

#ifndef STANDALONE

// failed to smooth the node so time to tidy up.
smoothfailed:
	pSmoothNode->CascadeDelete();
	delete pSmoothNode;
	return FALSE;

#endif
}


/********************************************************************************************

>	BOOL UndoableOperation::DoMakeNodeFromPath(	NodePath* pContextNode,
												AttachNodeDirection Direction,
												Path* pParentPath
												BOOL CopyAttributes )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		pContextNode	= pointer to a context node in the tree to attach our
								  path to
				direction		= direction on which to attach the new NodePath object
								  created by this call.
				pParentPath		= Path to place in the new NodePath object created
				CopyAttributes	= if TRUE then take the attributes applied to the context
								  node and apply them to the new node.		 
	Outputs:	-
	Returns:	TRUE if the node was created successfully
				FALSE if the node could not be created (out of memory)
	Purpose:	This functio will create a new nodepath and attach it in the specified
				direction to the context node specified. It will also copy attributes
				applied as children of the ConextNode to the new node if required.

********************************************************************************************/

BOOL UndoableOperation::DoMakeNodeFromPath( NodePath* pParentNode,
									 		Path* pParentPath,
									 		AttachNodeDirection Direction,
									 		BOOL CopyAttributes)
{
	// Create the node path object we will stick in the tree.
	NodePath* pChildNode;

	ALLOC_WITH_FAIL(pChildNode, new NodePath(), this);

	if (!pChildNode)
		return FALSE;

	BOOL ok;
	// make room for the new path in the node path.
	CALL_WITH_FAIL
	(
		pChildNode->SetUpPath(pParentPath->GetNumCoords(),12),
		this,ok
	);

	if (!ok)
	{
		delete pChildNode;
		return FALSE;
	}

	// now copy the path data in there.
	pChildNode->InkPath.CopyPathDataFrom(pParentPath);

	if (CopyAttributes)
	{
		CCAttrMap * AttribMap = NULL;
		AttribMap = CCAttrMap::MakeAppliedAttrMap(pParentNode);

		if (AttribMap)
		{
			AttribMap->ApplyAttributesToNode (pChildNode);
		}
	}

	// Now stick the new path into the tree
	CALL_WITH_FAIL
	(
		DoInsertNewNode(pChildNode, pParentNode, Direction, TRUE, FALSE),
		this,ok
	);

	// if we've failed tidy up
	if (!ok)
	{
		pChildNode->CascadeDelete();
		delete pChildNode;
		return FALSE;
	}
	return TRUE;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Do functions to localise/factor out attributes for attribute application

// Range functions first

/********************************************************************************************
>	BOOL UndoableOperation::DoLocaliseForAttrChange(Range* pRange, 
													AttrTypeSet* pAffectedAttrTypes, 
													BOOL ExcludeTextObjects = FALSE)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/95

	Inputs:		pRange:	The range of objects which need their attributes localising
	
				pAffectedAttrTypes: A set of the types of the attributes which are to be 
									applied to the Range. This function will localise 
									all attributes which have a type in this set. 
									
									All attriute types in this set must be localised on the 
									same compound node (See note below).

				ExcludeTextObjects: When TRUE atttributes will not be localised on Text
									compounds. This is useful for selection deletions, where
									TextStory localisation will be left to the Story itself.
									This is a bit unpleasant.

	Outputs:	-
	Returns:	TRUE if successful, 
				FALSE if we run out of memory. Tidyup then call End()

	Purpose:	This Do function must be called on a range prior to a set of attributes 
				being applied to it. It globally localises those attributes which have
				a type in the pAffectedAttrTypes set.
				
				After the attribute has been applied you should call DoFactorOutAfterAttrChange
												 				
				If an object in the range Discards its attribute children then its attributes
				are not localised.

				Note: When applying attributes, we are not always going to apply the attributes
					  to the object itself, sometimes we need to apply them to the object's parent. 
					  Eg. when we apply a line based attribute to a text character the attributes
					  will get applied to the parent TextLine.

					  In this situation we do not localise the objects attributes, but its parents
					  attributes instead. 
					  
					  Warning
					  ~~~~~~~

					  All attribute types in the AttrTypeSet must be localised
					  on the same compound node. This means for example that the AttrType set cannot
					  contain a Bold and a Line space attribute. If we need to do this in future
					  then the routine will need to be changed.

	Errors:		-
	SeeAlso:	AttrTypeSet
	SeeAlso:	NodeAttribute::GetAttributeType
	SeeAlso:	UndoableOperation::DoFactorOutAfterAttrChange
	SeeAlso:	UndoableOperation::DoLocaliseForAttrChange
	SeeAlso:	Range::DoFactorOutAfterAttrChange

********************************************************************************************/


BOOL UndoableOperation::DoLocaliseForAttrChange(Range* pRange, 
												AttrTypeSet* pAffectedAttrTypes, 
												BOOL ExcludeTextObjects /* = FALSE */)
{
	ERROR3IF(pRange->Count() == 0, "Range::DoLocaliseForAttrChange called on an empty range");  

	// Iterate over the top of any liveeffects applied to the selection
	// Its important that this iteration matches those used in DoApplyAttribute functions
	// so that attributes are put in the places where mutation will find them
	ListRange* pLevel = EffectsStack::GetNewLevelRange(pRange, FALSE);	// We DO own this range
	Node* pCurrent = pLevel->FindFirst();

	// There is no need to localise a compound more than once so we remember the last localised 
	NodeRenderableInk* pLocalisedCompound = NULL;

	CCRuntimeClass* AttrType;
	Node* pParent;
	Node* pObject;
				  		
	while (pCurrent)
	{
		pObject = pCurrent;
		// Determine if the attribute will get applied to this object, or an alternative object
		// (probably its parent)
		if (pAffectedAttrTypes && (!pAffectedAttrTypes->IsEmpty()))
		{
			AttrType = ((AttrTypeItem*)pAffectedAttrTypes->GetHead())->AttributeType;
			ERROR3IF(!AttrType, "AttrType set contains NULL attribute type");  
			pObject = ((NodeRenderableInk*)pObject)->GetObjectToApplyTo(AttrType);
		}

		if (pObject != NULL)
		{

			// We only need to localise those nodes which have a compound parent
			pParent = pObject->FindParent(); 
			ERROR3IF(pParent == NULL, "Range::DoLocaliseForAttrChange, node found without a parent"); 
		
			// Only localise the parent if it is a compound node which has not already been localised
			if ((pParent->IsCompound()) && (pParent != pLocalisedCompound))
			{
				if (!(ExcludeTextObjects && ((IS_A(pParent, TextLine)) || (IS_A(pParent, TextStory)))) )
				{
					// Attempt to localise the compound
					if (!DoLocaliseForAttrChange((NodeRenderableInk*)pObject, pAffectedAttrTypes, NULL))
					{
						delete pLevel;
						return FALSE;
					}
				 
					pLocalisedCompound = (NodeRenderableInk*)pParent;  // Remember that it's been localised
				}
			}
		}
		
		pCurrent = pLevel->FindNext(pCurrent);
	}

	delete pLevel;
	return TRUE;
}

/********************************************************************************************

>	BOOL UndoableOperation::DoLocaliseForAttrChange(Range* pRange, 
													CCRuntimeClass* pAffectedAttrType)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/95

	Inputs:		pRange:	The range of objects which need their attributes localising

	
				pAffectedAttrType: The type of the attribute that you are about to apply
								   to the range

	Outputs:	-
	Returns:	TRUE if successful, 
				FALSE if we run out of memory. Tidyup then call End()

	Purpose:	This Do function must be called on a range prior to an attribute 
				being applied to it. If you are going to apply multiple attributes to 
				the range then it is more efficient to call the other version of this 
				function which takes a set of attribute types. 
				
				The function globally localises those attributes which have	a type in the 
				pAffectedAttrTypes set.

				If an object in the range Discards its attribute children then its attributes
				are not localised.
				   				
				Note: When applying an attribute, we are not always going to apply it
			    to the object itself, sometimes we need to apply it to the object's parent. 
			    Eg. when we apply a line based attribute to a text character the attributes
			    will get applied to the parent TextLine.

			    In this situation we do not localise the objects attributes, but its parents
			    attributes instead. 
			  

	Errors:		-
	SeeAlso:	AttrTypeSet
	SeeAlso:	NodeAttribute::GetAttributeType
	SeeAlso:	UndoableOperation::DoFactorOutAfterAttrChange
	SeeAlso:	UndoableOperation::DoLocaliseForAttrChange
	SeeAlso:	Range::DoFactorOutAfterAttrChange

********************************************************************************************/


							 
BOOL UndoableOperation::DoLocaliseForAttrChange(Range* pRange, 
							 					CCRuntimeClass* pAffectedAttrType)
{
	ERROR3IF(pRange->Count() == 0, "Range::DoLocaliseForAttrChange called on an empty range");  

	// Iterate over the top of any liveeffects applied to the selection
	// Its important that this iteration matches those used in DoApplyAttribute functions
	// so that attributes are put in the places where mutation will find them
	ListRange* pLevel = EffectsStack::GetNewLevelRange(pRange, FALSE);	// We DO own this range
	Node* pCurrent = pLevel->FindFirst();

	// There is no need to localise a compound more than once so we remember the last localised 
	NodeRenderableInk* pLocalisedCompound = NULL;

	Node* pParent;
	Node* pObject;
				  		
	while (pCurrent)
	{
		pObject = pCurrent;
		// Determine if the attribute will get applied to this object, or an alternative object
		// (probably its parent)
		ERROR3IF(!pAffectedAttrType, "AttrType is NULL");  
		pObject = ((NodeRenderableInk*)pObject)->GetObjectToApplyTo(pAffectedAttrType);

		{

			// We only need to localise those nodes which have a compound parent
			pParent = pObject->FindParent(); 
			ERROR3IF(pParent == NULL, "Range::DoLocaliseForAttrChange, node found without a parent"); 
		
			// Only localise the parent if it is a compound node which has not already been localised
			if ((pParent->IsCompound()) && (pParent != pLocalisedCompound))
			{
				// Attempt to localise the compound
				if (!DoLocaliseForAttrChange((NodeRenderableInk*)pObject, pAffectedAttrType, NULL))
				{
					delete pLevel;
					return FALSE;
				}
				 
				pLocalisedCompound = (NodeRenderableInk*)pParent;  // Remember that it's been localised
			}
		}
		pCurrent = pLevel->FindNext(pCurrent);
	}

	delete pLevel;
	return TRUE;
}

/********************************************************************************************

>	BOOL UndoableOperation::DoFactorOutAfterAttrChange(Range* pRange, 
													   AttrTypeSet* pAffectedAttrTypes)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95

	Inputs:		pRange: The objects that need their attributes factoring out
	
				pAffectedAttrTypes: The types of the attributes that have just been
									applied to the range.

	Outputs:	-
	Returns:	TRUE if successful, 
				FALSE if we run out of memory. Tidyup then call End()

	Purpose:	This Do function must be called on the range after attributes have
				being applied to it. 
				
				The function globally factors out those attributes which have a type in the 
				pAffectedAttrTypes set.
				
				Before calling the function you should have localised attributes by calling
				DoLocaliseForAttrChange.
				
				If an object in the range Discards its attribute children then its attributes
				are ignored when factoring (eg. the caret)

				Note: When applying attributes, we are not always going to apply the attributes
					  to the object itself, sometimes we need to apply them to the object's parent. 
					  Eg. when we apply a line based attribute to a text character the attributes
					  will get applied to the parent TextLine.

					  In this situation we do not factor out the objects attributes, but its parents
					  attributes instead. 
					  
					  Warning
					  ~~~~~~~

					  All attribute types in the AttrTypeSet must have been localised 
					  (and so need factoring out) on the same compound node. This means for example 
					  that the AttrType set cannot contain a Bold and a Line space attribute. If 
					  we need to do this in future then the routine will need to be changed.

 

	SeeAlso:	AttrTypeSet
	SeeAlso:	NodeAttribute::GetAttributeType
	SeeAlso:	UndoableOperation::DoFactorOutAfterAttrChange
	SeeAlso:	Range::DoLocaliseForAttrChange


********************************************************************************************/


BOOL UndoableOperation::DoFactorOutAfterAttrChange(Range* pRange, 
												   AttrTypeSet* pAffectedAttrTypes)
{
	ERROR3IF(pRange->Count() == 0, "Range::DoFactorOutAfterAttrChange called on an empty range");  

	// Iterate over the top of any liveeffects applied to the selection
	// Its important that this iteration matches those used in DoApplyAttribute functions
	// so that attributes are put in the places where mutation will find them
	ListRange* pLevel = EffectsStack::GetNewLevelRange(pRange, FALSE);	// We DO own this range
	Node* pCurrent = pLevel->FindFirst();

	// There is no need to factor out the attributes on a compound more than once, so we 
	// remember the last compound node which has had its attributes factored out 
	NodeRenderableInk* pFactoredOutCompound = NULL;

	Node* pParent;
	Node* pObject;
				  		
	while (pCurrent)
	{
		// If the object can discard its attribute children then we should not try to factor
		// out its attributes
		CCRuntimeClass* AttrType;
		
		pObject = pCurrent;
		if (pAffectedAttrTypes && (!pAffectedAttrTypes->IsEmpty()))
		{
			AttrType = ((AttrTypeItem*)pAffectedAttrTypes->GetHead())->AttributeType;
			ERROR3IF(!AttrType, "AttrType set contains NULL attribute type");  
			pObject = ((NodeRenderableInk*)pObject)->GetObjectToApplyTo(AttrType);
		}

		// DY added test for NULL, which can occur with bevels
		if ((pObject != NULL))
		{
			// We only need to factor out attributes on nodes which have a compound parent
			pParent = pObject->FindParent(); 
			ERROR3IF(pParent == NULL, "Range::DoFactorOutAfterAttrChange, node found without a parent"); 
		
			// Only factor out attribs if  the parent has not already had its attribs factored out
			if ((pParent->IsCompound()) && (pParent != pFactoredOutCompound))
			{
				// Attempt to localise the compound
				if (!DoFactorOutAfterAttrChange((NodeRenderableInk*)pObject, pAffectedAttrTypes))
				{
					delete pLevel;
					return FALSE;
				}
				 
				pFactoredOutCompound = (NodeRenderableInk*)pParent;  // Remember that it's been localised
			}
		}
		
		pCurrent = pLevel->FindNext(pCurrent);
	}

	delete pLevel;
	return TRUE;
}

/********************************************************************************************

>	BOOL UndoableOperation::DoFactorOutAfterAttrChange(Range* pRange, 
													   CCRuntimeClass* pAffectedAttrType)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95

	Inputs:		pRange: The objects that need their attributes factoring out

				pAffectedAttrType: The type of the attribute that have just been
									applied to the range.

	Outputs:	-
	Returns:	TRUE if successful, 
				FALSE if we run out of memory. Tidyup then call End()

	Purpose:	This Do function must be called on the range after an attribute 
				being applied to it. If you have applied multiple attributes to 
				the range then it is more efficient to call the other version of this 
				function which takes a set of attribute types. 
				
				The function globally factors out the attribute which has type pAffectedAttrType

				Before calling the function you should have localised the attribute by calling
				DoLocaliseForAttrChange.

				If an object in the range Discards its attribute children then its attributes
				are ignored when factoring (eg. the caret)

				Note: When applying an attribute, we are not always going to apply it
			    to the object itself, sometimes we need to apply it to the object's parent. 
			    Eg. when we apply a line based attribute to a text character the attributes
			    will get applied to the parent TextLine.

			    In this situation we do not factor out the objects attributes, but its parents
			    attributes instead.
			    
			    Note: When applying attributes, we are not always going to apply the attributes
					  to the object itself, sometimes we need to apply them to the object's parent. 
					  Eg. when we apply a line based attribute to a text character the attributes
					  will get applied to the parent TextLine.

					  In this situation we do not factor out the object's attributes, but its parents
					  attributes instead. 
						   				
	SeeAlso:	AttrTypeSet
	SeeAlso:	NodeAttribute::GetAttributeType
	SeeAlso:	UndoableOperation::DoLocaliseForAttrChange
	SeeAlso:	Range::DoLocaliseForAttrChange
	SeeAlso:	Range::DoFactorOutAfterAttrChange


********************************************************************************************/

BOOL UndoableOperation::DoFactorOutAfterAttrChange(Range* pRange, 
												   CCRuntimeClass* pAffectedAttrType)
{
	ERROR3IF(pRange->Count() == 0, "Range::DoFactorOutAfterAttrChange called on an empty range");  
	// Scan the range

	// Iterate over the top of any liveeffects applied to the selection
	// Its important that this iteration matches those used in DoApplyAttribute functions
	// so that attributes are put in the places where mutation will find them
	ListRange* pLevel = EffectsStack::GetNewLevelRange(pRange, FALSE);	// We DO own this range
	Node* pCurrent = pLevel->FindFirst();

	// There is no need to factor out the attributes on a compound more than once, so we 
	// remember the last compound node which has had its attributes factored out 
	NodeRenderableInk* pFactoredOutCompound = NULL;

	Node* pParent;
	Node* pObject;			  		
	while (pCurrent)
	{
		pObject = pCurrent;
		// Get the object that the attribute has been applied to
		ERROR3IF(!pAffectedAttrType, "AttrType is NULL");  
		pObject = ((NodeRenderableInk*)pObject)->GetObjectToApplyTo(pAffectedAttrType);

		// If the object can discard its attribute children then we should not try to factor
		// out its attributes

		{

			// We only need to factor out attributes on nodes which have a compound parent
			pParent = pObject->FindParent(); 
			ERROR3IF(pParent == NULL, "Range::DoFactorOutAfterAttrChange, node found without a parent"); 
		
			// Only factor out attribs if  the parent has not already had its attribs factored out
			if ((pParent->IsCompound()) && (pParent != pFactoredOutCompound))
			{
				// Attempt to localise the compound
				if (!DoFactorOutAfterAttrChange((NodeRenderableInk*)pObject, pAffectedAttrType))
				{
					delete pLevel;
					return FALSE;
				}
				 
				pFactoredOutCompound = (NodeRenderableInk*)pParent;  // Remember that it's been localised
			}
		}
		pCurrent = pLevel->FindNext(pCurrent);
	}

	delete pLevel;
	return TRUE;
}



/********************************************************************************************

>	BOOL UndoableOperation::DoLocaliseForAttrChange(NodeRenderableInk* Object, 
							 						AttrTypeSet* pAffectedAttrTypes, 
							 						ObjectSet* pLocalisedCompounds)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/95
	Inputs:		Object: The object that you are about to apply attributes to
				
				pAffectedAttrTypes: A set of the types of the attributes which are to be 
									applied to the Object. This function will localise 
									all attributes which have a type in this set.

				pLocalisedCompounds: An optional set of compounds which have already had
									 all attributes in the pAffectedAttrTypes localised. 
									 If Object has a parent compound which 
									 is in this set then its attributes will not need to
									 be localised again. 
	Outputs:	-
	Returns:	TRUE if successful, 
				FALSE if we run out of memory. Tidyup then call End()

	Purpose:	This Do function must be called on an object prior to a set of attributes 
				being applied to it. It globally localises those attributes which have
				a type in the pAffectedAttrTypes set.
				
				If the function is called on a  object which does not have a compound parent 
				then there will be no attributes to localise so the function will simply
				return TRUE without having done anything.

				After a compound object has localised its children it will be added to the 
				pLocalisedCompounds set if this has been provided. 

				After the attribute has been applied you should call DoFactorOutAfterAttrChange

				If the object discards its attribute children then the routine does nothing 
				(eg. the caret)

				Note: When applying attributes, we are not always going to apply the attributes
				to the object itself, sometimes we need to apply them to the object's parent. 
				Eg. when we apply a line based attribute to a text character the attributes
				will get applied to the parent TextLine.

				In this situation we do not localise the objects attributes, but its parents
				attributes instead. 
			  
			    Warning
			    ~~~~~~~

			    All attribute types in the AttrTypeSet must be localised
			    on the same compound node. This means for example that the AttrType set cannot
			    contain a Bold and a Line space attribute. If we need to do this in future
			    then the routine will need to be changed.



	Errors:		-
	SeeAlso:	AttrTypeSet
	SeeAlso:	NodeAttribute::GetAttributeType
	SeeAlso:	UndoableOperation::DoFactorOutAfterAttrChange
	SeeAlso:	Range::DoLocaliseForAttrChange
	SeeAlso:	Range::DoFactorOutAfterAttrChange

********************************************************************************************/

// This function must get called on a compound node before a new attribute is attached to it
BOOL UndoableOperation::DoLocaliseForAttrChange(NodeRenderableInk* Object, 
							 					AttrTypeSet* pAffectedAttrTypes, 
							 					ObjectSet* pLocalisedCompounds)
{
	CCRuntimeClass* AttrType;
	// Determine if the attribute will get applied to this object, or an alternative object
	// (probably its parent)
	if (pAffectedAttrTypes && (!pAffectedAttrTypes->IsEmpty()))
	{
		AttrType = ((AttrTypeItem*)pAffectedAttrTypes->GetHead())->AttributeType;
		ERROR3IF(!AttrType, "AttrType set contains NULL attribute type");  
		Object = Object->GetObjectToApplyTo(AttrType);
	}

	{	


		// If the object has not got a compound parent then there will be no attributes 
		// to localise, so we need not do anything
	
		Node* pParent;
		pParent = Object->FindParent(); 
		ERROR3IF(!pParent, "UndoableOperation::DoLocaliseForAttrChange called on an object which has no parent"); 

		if (pParent->IsCompound())
		{
			// Before we attempt to localise the compound's attributes let's check that they are not 
			// already localised.
			
			if (pLocalisedCompounds)
			{
				if (pLocalisedCompounds->InSet((NodeRenderableInk*)pParent))
				{
					return TRUE; // Attributes are already localised so there is no need to localise again !
				}
				else
				{
					pLocalisedCompounds->AddToSet((NodeRenderableInk*)pParent); // Record that the compound has been localised
				}
			}

			return (DoLocaliseCommonAttributes((NodeRenderableInk*)pParent,  // We are localising the compound's attrs
											   FALSE,	 // No need to check for duplicates
											   TRUE,  	 // Localise globally
											   pAffectedAttrTypes)); // Only attributes of these types
		}
		
	}
	return TRUE; 
}

/********************************************************************************************

>	BOOL  UndoableOperation::DoLocaliseForAttrChange(NodeRenderableInk* Object, 
							 						 CCRuntimeClass* pAffectedAttrType, 
							 						 ObjectSet* pLocalisedCompounds)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95
	Inputs:		Object: The object that you are about to apply the attribute to
				
				pAffectedAttrType: The type of the attribute that you are about to apply
								   to the object
																
				pLocalisedCompounds: An optional set of compounds which have already had
									 their  pAffectedAttrType localised. 
									 If Object has a parent compound which 
									 is in this set then its attributes will not need to
									 be localised again. 


	Outputs:	-
	Returns:	TRUE if successful, 
				FALSE if we run out of memory. Tidyup then call End()

	Purpose:	This Do function must be called on an object prior to an attribute 
				being applied to it. If you are going to apply multiple attributes to 
				the Object then it is more efficient to call the other version of this 
				function which takes a set of attribute types. 
				
				The function globally localises those attributes which have	a type in the 
				pAffectedAttrTypes set.
				
				If the function is called on a  object which does not have a compound parent 
				then there will be no attributes to localise so the function will simply
				return TRUE without having done anything.

				If the object discards its attribute children then the routine does nothing 
				(eg. the caret)

				Note: When applying attributes, we are not always going to apply the attributes
			    to the object itself, sometimes we need to apply them to the object's parent. 
			    Eg. when we apply a line based attribute to a text character the attributes
			    will get applied to the parent TextLine.

			    In this situation we do not localise the objects attributes, but its parents
			    attributes instead. 


	SeeAlso:	AttrTypeSet
	SeeAlso:	NodeAttribute::GetAttributeType
	SeeAlso:	UndoableOperation::DoFactorOutAfterAttrChange
	SeeAlso:	Range::DoLocaliseForAttrChange
	SeeAlso:	Range::DoFactorOutAfterAttrChange


********************************************************************************************/
							 
BOOL  UndoableOperation::DoLocaliseForAttrChange(NodeRenderableInk* Object, 
							 					 CCRuntimeClass* pAffectedAttrType,
							 					 ObjectSet* pLocalisedCompounds)
{
	BOOL ok = TRUE;
	// Determine if the attribute will get applied to this object, or an alternative object
	// (probably its parent)
	ERROR3IF(!pAffectedAttrType, "AttrType is NULL");  
	Object = Object->GetObjectToApplyTo(pAffectedAttrType);

	{	

		Node* pParent;
		pParent = Object->FindParent(); 
		ERROR3IF(!pParent, "UndoableOperation::DoLocaliseForAttrChange called on an object which has no parent"); 

		if (pParent->IsCompound())
		{
	
			// We an attribute type set
			AttrTypeSet Set; 
		
			// Add the attributes type to the set
			if (!Set.AddToSet(pAffectedAttrType))
			{
				ok = FALSE; 
			}

			if (ok)
			{
				// Before we attempt to localise the compound's attributes let's check that they are not 
				// already localised.
			
				if (pLocalisedCompounds)
				{
					if (pLocalisedCompounds->InSet((NodeRenderableInk*)pParent))
					{
						return TRUE; // Attributes are already localised so there is no need to localise again !
					}
					else
					{
						pLocalisedCompounds->AddToSet((NodeRenderableInk*)pParent); // Record that the compound has been localised
					}
				}


				ok = DoLocaliseCommonAttributes((NodeRenderableInk*)pParent,// We are localising the compound's attrs
												   FALSE,	 		   		// No need to check for duplicates
												   TRUE,  	 		   		// Localise globally
												   &Set); 					// Only attributes of these types
			}
	
			// The attribute type set is no longer required
			Set.DeleteAll(); 
		}
	}
	return ok; 
}


/********************************************************************************************

>	BOOL UndoableOperation::DoFactorOutAfterAttrChange(NodeRenderableInk* Object,
  								 					   AttrTypeSet* pAffectedAttrTypes)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95
	Inputs:		Object: The object that you have just applied attributes to
				
				pAffectedAttrTypes: The types of the attributes that have just been
									applied to the object.

	Outputs:	-
	Returns:	TRUE if successful, 
				FALSE if we run out of memory. Tidyup then call End()

	Purpose:	This Do function must be called on an object after attributes have
				being applied to it. 
				
				The function globally factors out those attributes which have a type in the 
				pAffectedAttrTypes set.
				
				If the function is called on a  object which does not have a compound parent 
				then there will be no attributes to factor out so the function will simply
				return TRUE without having done anything.

				Before calling the function you should have localised attributes by calling
				DoLocaliseForAttrChange. 

				If the object discards its attribute children then the routine does nothing 
				(eg. the caret)

				Note: When applying attributes, we are not always going to apply the attributes
					  to the object itself, sometimes we need to apply them to the object's parent. 
					  Eg. when we apply a line based attribute to a text character the attributes
					  will get applied to the parent TextLine.

					  In this situation we do not factor out the objects attributes, but its parents
					  attributes instead. 
					  
					  Warning
					  ~~~~~~~

					  All attribute types in the AttrTypeSet must have been localised 
					  (and so need factoring out) on the same compound node. This means for example 
					  that the AttrType set cannot contain a Bold and a Line space attribute. If 
					  we need to do this in future then the routine will need to be changed.


	SeeAlso:	AttrTypeSet
	SeeAlso:	NodeAttribute::GetAttributeType
	SeeAlso:	UndoableOperation::DoLocaliseForAttrChange
	SeeAlso:	Range::DoLocaliseForAttrChange
	SeeAlso:	Range::DoFactorOutAfterAttrChange


********************************************************************************************/

BOOL UndoableOperation::DoFactorOutAfterAttrChange(NodeRenderableInk* Object,
  								 				   AttrTypeSet* pAffectedAttrTypes)
{
	CCRuntimeClass* AttrType;

	// Get object which attrib was applied to
	if (pAffectedAttrTypes && (!pAffectedAttrTypes->IsEmpty()))
	{
		AttrType = ((AttrTypeItem*)pAffectedAttrTypes->GetHead())->AttributeType;
		ERROR3IF(!AttrType, "AttrType set contains NULL attribute type");  
		Object = Object->GetObjectToApplyTo(AttrType);
	}

	{	
		Node* pParent;
		pParent = Object->FindParent(); 
		ERROR3IF(!pParent, "UndoableOperation::DoFactorOutAfterAttrChange called on an object which has no parent"); 

		if (pParent->IsCompound())
		{
			return (DoFactorOutCommonChildAttributes((NodeRenderableInk*)pParent, 
													 TRUE,	   // Global
													 pAffectedAttrTypes));
		}
	}
	return TRUE;
}


/********************************************************************************************

>	BOOL UndoableOperation::DoFactorOutAfterAttrChange(NodeRenderableInk* Object,
  								 					   CCRuntimeClass* pAffectedAttrType)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95
	Inputs:		Object: The object that you have just applied attributes to
				
				pAffectedAttrType: The types of the attribute that have just been
									applied to the object.

	Outputs:	-
	Returns:	TRUE if successful, 
				FALSE if we run out of memory. Tidyup then call End()

	Purpose:	This Do function must be called on an object after an attribute 
				being applied to it. If you have applied multiple attributes to 
				the Object then it is more efficient to call the other version of this 
				function which takes a set of attribute types. 
				
				The function globally factors out the attribute with type pAffectedAttrType
				
				If the function is called on a  object which does not have a compound parent 
				then there will be no attributes to factor out so the function will simply
				return TRUE without having done anything.

				Before calling the function you should have localised attributes by calling
				DoLocaliseForAttrChange. 

				If the object discards its attribute children then the routine does nothing 
				(eg. the caret)

				Note: When applying attributes, we are not always going to apply the attributes
					  to the object itself, sometimes we need to apply them to the object's parent. 
					  Eg. when we apply a line based attribute to a text character the attributes
					  will get applied to the parent TextLine.

					  In this situation we do not factor out the objects attributes, but its parents
					  attributes instead. 

	SeeAlso:	AttrTypeSet
	SeeAlso:	NodeAttribute::GetAttributeType
	SeeAlso:	UndoableOperation::DoLocaliseForAttrChange
	SeeAlso:	Range::DoLocaliseForAttrChange
	SeeAlso:	Range::DoFactorOutAfterAttrChange


********************************************************************************************/

BOOL UndoableOperation::DoFactorOutAfterAttrChange(NodeRenderableInk* Object,
												   CCRuntimeClass* pAffectedAttrType)
{
	BOOL ok = TRUE;

	// Get object attr was applied to
	ERROR3IF(!pAffectedAttrType, "AttrType is NULL");  
	Object = Object->GetObjectToApplyTo(pAffectedAttrType);

	{	

		Node* pParent;
		pParent = Object->FindParent(); 
		ERROR3IF(!pParent, "UndoableOperation::DoFactorOutAfterAttrChange called on an object which has no parent"); 

		if (pParent->IsCompound())
		{
	
			// We need an attribute type set
			AttrTypeSet Set; 

			// Add the attributes type to the set
			if (!(Set.AddToSet(pAffectedAttrType)))
			{
				ok = FALSE; 
			}

			if (ok)
			{

				return (DoFactorOutCommonChildAttributes((NodeRenderableInk*)pParent, 
														 TRUE,	   // Global
													 	 &Set));
			}
	
			Set.DeleteAll(); 
		}
	}
	return ok; 
}

/********************************************************************************************

>	BOOL UndoableOperation::DoFactorOutAfterAttrChange(ObjectSet* pLocalisedCompounds,
													   AttrTypeSet* pAffectedAttrTypes)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95

	Inputs:		pLocalisedCompounds: A set of compound nodes which have localised their
									 pAffectedAttrTypes attributes

				pAffectedAttrTypes: The types of the attributes that have been
									applied to the range.

	Outputs:	-
	Returns:	TRUE if successful, 
				FALSE if we run out of memory. Tidyup then call End()

	Purpose:	This Do function is designed to be called after all attributes in the 
				pAffectedAttrTypes set  have been localised on all compound nodes in 
				the pLocalisedCompounds set.	
				
				The function globally factors out the attributes which have a type which is
				in the pAffectedAttrTypes set.

				Before calling the function you should have localised the attribute by calling
				DoLocaliseForAttrChange.



	SeeAlso:	AttrTypeSet
	SeeAlso:	NodeAttribute::GetAttributeType
	SeeAlso:	UndoableOperation::DoLocaliseForAttrChange


********************************************************************************************/

BOOL UndoableOperation::DoFactorOutAfterAttrChange(ObjectSet* pLocalisedCompounds, 
												   AttrTypeSet* pAffectedAttrTypes)
{
	ERROR3IF(!pLocalisedCompounds, "DoFactorOutAfterAttrChange called with a NULL compound set");  
	//ERROR3IF(!pAffectedAttrTypes, "DoFactorOutAfterAttrChange called with a NULL attr type set"); 

	// Scan the range
	ObjectItem* pObjItem = (ObjectItem*)(pLocalisedCompounds->GetHead());
	NodeRenderableInk* pCurrent;

	while (pObjItem)
	{
	 	pCurrent = pObjItem->pObject;
		ERROR3IF(!(pCurrent->IsCompound()), "Set should only contain compound objects");
		if (pCurrent->IsCompound())
		{	
			if (!DoFactorOutCommonChildAttributes((NodeRenderableInk*)pCurrent, 
													 TRUE,	   // Global
												 	 pAffectedAttrTypes))
			{
				return FALSE; 
			}
		}
		pObjItem = (ObjectItem*)pLocalisedCompounds->GetNext(pObjItem);
	}

	return TRUE; 

}


/********************************************************************************************

>	BOOL UndoableOperation::DoFactorOutAfterAttrChange(ObjectSet* pLocalisedCompounds,
													   CCRuntimeClass* pAffectedAttrType)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/95

	Inputs:		pLocalisedCompounds: A set of compound nodes which have localised their
									 pAffectedAttrType attributes.

				pAffectedAttrType: The type of the attribute that have just been
									applied to the range.

	Outputs:	-
	Returns:	TRUE if successful, 
				FALSE if we run out of memory. Tidyup then call End()

	Purpose:	This Do function is designed to be called after pAffectedAttrType has been 
				localised on all compound nodes in the pLocalisedCompounds set.	
				
				The function globally factors out the attribute which has type pAffectedAttrType

				Before calling the function you should have localised the attribute by calling
				DoLocaliseForAttrChange.
					  


	SeeAlso:	AttrTypeSet
	SeeAlso:	NodeAttribute::GetAttributeType
	SeeAlso:	UndoableOperation::DoLocaliseForAttrChange


********************************************************************************************/



BOOL UndoableOperation::DoFactorOutAfterAttrChange(ObjectSet* pLocalisedCompounds,
												  CCRuntimeClass* pAffectedAttrType)
{
	ERROR3IF(!pLocalisedCompounds, "DoFactorOutAfterAttrChange called with a NULL compound set");  
	ERROR3IF(!pAffectedAttrType, "DoFactorOutAfterAttrChange called with a NULL attr type"); 


	// Scan the range
	ObjectItem* pObjItem = (ObjectItem*)(pLocalisedCompounds->GetHead());
	NodeRenderableInk* pCurrent;

	// We need an attribute type set
	AttrTypeSet Set; 

	// Add the attributes type to the set
	if (!(Set.AddToSet(pAffectedAttrType)))
	{
		return FALSE; 
	}

	while (pObjItem)
	{
	 	pCurrent = pObjItem->pObject;
		ERROR3IF(!(pCurrent->IsCompound()), "Set should only contain compound objects");
		if (pCurrent->IsCompound())
		{	
			if (!DoFactorOutCommonChildAttributes((NodeRenderableInk*)pCurrent, 
													 TRUE,	   // Global
												 	 &Set))
			{
				Set.DeleteAll();
				return FALSE; 
			}
		}
		pObjItem = (ObjectItem*)pLocalisedCompounds->GetNext(pObjItem);
	}

	Set.DeleteAll(); 
	return TRUE; 
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// These functions are for attribute optimisation when deleting objects in a range

// They are now defunct. They were a nice idea but they could not cope with attribute children
// of empty groups !


/********************************************************************************************

>	BOOL UndoableOperation::GetPreDelFactorOutInfo(Range* pRange, List* pPreDelFactorOutInfoList)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/95

	Inputs:		pRange: The range of objects that will be deleted
	
	Outputs:	pPreDelFactorOutInfoList: Information to be used by the DoFactorOutAfterDeletion
										  function.

	Returns:	TRUE if succesful
				FALSE if we ran out of memory. In this situation all items on the 
				pPreDelFactorOutInfoList are deleted. 

	Purpose:	This function is an information gatherer for the DoFactorOutAfterDeletion
				function. See this function for a full description of its purpose. 
	Errors:		-
	SeeAlso:	UndoableOperation::DoFactorOutAfterDeletion

********************************************************************************************/
/*
BOOL UndoableOperation::GetPreDelFactorOutInfo(Range* pRange, List* pPreDelFactorOutInfoList)
{
	// On entry to this function the PreDelFactorOutInfoList should be empty
	ERROR3IF(!(pPreDelFactorOutInfoList->IsEmpty()), "PreDelFactorOutInfoList is not empty"); 
	// Lets make sure
	pPreDelFactorOutInfoList->DeleteAll();
	
	NodeRenderableInk* pCurrentCompound; // A pointer to the compound parent of the objects we are
								  		 // currently processing 
	
	pCurrentCompound = NULL;
	PreDelFactorOutInfoItem* pCurrentCompoundInfo = NULL;
	 
	// Scan the range
	Node* pCurrent = pRange->FindFirst();
	Node* pCurrentsParent = NULL;
	while (pCurrent)
	{
		pCurrentsParent = pCurrent->FindParent();
		ERROR3IF(pCurrentsParent == NULL, "Current has a NULL parent"); 
		if (pCurrentsParent->IsCompound())
		{
			if (pCurrentsParent != pCurrentCompound)
			{
				pCurrentCompound = (NodeRenderableInk*)pCurrentsParent; 

				// We are processing a new Compound node. Create a new PreDelFactorOutInfoItem
				// so that we can record those attributes that must be factored out in it. 

				// Check if the last compound node we processed needs any attrs factoring out. 
				// if it doesn't then we can safely delete its entry (every little helps)
				if (pCurrentCompoundInfo && pCurrentCompoundInfo->AttrTypesToFactorOut.IsEmpty())
				{
					// bye-bye
					pPreDelFactorOutInfoList->RemoveItem(pCurrentCompoundInfo);
					delete pCurrentCompoundInfo; 
				}

				pCurrentCompoundInfo = new PreDelFactorOutInfoItem(); 
				if (!pCurrentCompoundInfo)
					goto OutOfMemory;

				pCurrentCompoundInfo->pCompound = pCurrentCompound;

				// Add the item to the list
				pPreDelFactorOutInfoList->AddHead(pCurrentCompoundInfo);
			}
			((NodeRenderableInk*)pCurrent)->AddChildAttrTypesToSet(&(pCurrentCompoundInfo->AttrTypesToFactorOut)); 
		}

		// Get next object to process
		pCurrent = pRange->FindNext(pCurrent); 
	}
	
	return TRUE; // success

	OutOfMemory:
	// Delete all items on the pPreDelFactorOutInfoList
	pCurrentCompoundInfo = (PreDelFactorOutInfoItem*)(pPreDelFactorOutInfoList->GetHead());
	while (pCurrentCompoundInfo)
	{
		pCurrentCompoundInfo->AttrTypesToFactorOut.DeleteAll();
		pCurrentCompoundInfo = (PreDelFactorOutInfoItem*)
			(pPreDelFactorOutInfoList->GetNext(pCurrentCompoundInfo));
	}
	pPreDelFactorOutInfoList->DeleteAll(); 
	return FALSE; 
		
} 
*/
/********************************************************************************************

>	BOOL UndoableOperation::DoFactorOutAfterDeletion(List* pPreDelFactorOutInfoList)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/95
	Inputs:		pPreDelFactorOutInfoList: given to you by the GetPreDelFactorOutInfo function

										  All items on this list are helpfully deleted after
										  this function returns.
	Outputs:	-
	Returns:	TRUE if successful, 
				FALSE if we run out of memory. Tidyup then call End()

	Purpose:	Whenever we delete a range of objects, if any objects in the range have compound
				parents then we must adjust the common attributes on these compounds. 

				For example if a compound has three red objects, and two yellow objects. If
				we delete the yellow objects, then red should become a common attribute of the
				compound.

				We could acheive this feat by firstly localising all attributes on the range, hiding
				the range, and then factoring out after. However this will result in unecessary
				attributes dangling off hidden nodes, so we try to be a bit cleverer than this.

				Prior to deleting the range we call GetPreDelFactorOutInfo. This function returns
				us a list containing information about which attributes need to be factored out 
				on which compounds. 

				After deleting the range this function should be called to  factor 
				out the neccessary attributes.

	Errors:		-
	SeeAlso:	UndoableOperation::GetPreDelFactorOutInfo

********************************************************************************************/
			
// This function should get called after a range of objects have been deleted ('Hidden')
/*
BOOL UndoableOperation::DoFactorOutAfterDeletion(List* pPreDelFactorOutInfoList)
{
	PreDelFactorOutInfoItem* pCurrentCompoundInfo = 
		(PreDelFactorOutInfoItem*)(pPreDelFactorOutInfoList->GetHead());

	NodeRenderableInk* pCompound;

	BOOL ok = TRUE;

	// Process each record
	while (ok && pCurrentCompoundInfo)
	{	
		// Are there any attributes to factor out ??
		if (!(pCurrentCompoundInfo->AttrTypesToFactorOut.IsEmpty()))
		{
			pCompound = pCurrentCompoundInfo->pCompound; 
			ERROR3IF(!(pCompound->IsCompound()), "List should only contain compound objects");
			if (pCompound->IsCompound())
			{	
				if (!DoFactorOutCommonChildAttributes((NodeRenderableInk*)pCompound, 
														 TRUE,	   // Global
													 	 &(pCurrentCompoundInfo->AttrTypesToFactorOut)))
				{
					ok = FALSE; 
				}
			}
		}
		pCurrentCompoundInfo = (PreDelFactorOutInfoItem*)
			(pPreDelFactorOutInfoList->GetNext(pCurrentCompoundInfo));
	}

	// Delete all items on the pPreDelFactorOutInfoList
	pCurrentCompoundInfo = (PreDelFactorOutInfoItem*)(pPreDelFactorOutInfoList->GetHead());
	while (pCurrentCompoundInfo)
	{
		pCurrentCompoundInfo->AttrTypesToFactorOut.DeleteAll();
		pCurrentCompoundInfo = (PreDelFactorOutInfoItem*)
			(pPreDelFactorOutInfoList->GetNext(pCurrentCompoundInfo));
	}
	pPreDelFactorOutInfoList->DeleteAll(); 

	if (!ok)
		FailAndExecute();
	return ok; 

} 
*/
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/********************************************************************************************

>	BOOL UndoableOperation::DoFactorOutCommonChildAttributes(NodeRenderableInk* CompoundObject, 
															 BOOL Global = FALSE,
															 List* pAffectedAttrTypes = NULL)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/8/94

	Inputs:		CompoundObject:	The object to factor out
	
				Global:	When TRUE we recursively factor out attributes in all parent compounds 
						of the CompoundObject (Bottom up)
	
				pAffectedAttrTypes: An optional set of attribute types. If this is specified
									then we only consider factoring out those attributes
									which have a type which is in this set. 

	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 

	Purpose:	This function factors out all attributes which are common to all children
				of the compound object. All common attributes become first children of the
				compound object.

				If the compound contains a node which can discard its attributes (like the caret)
				then its attributes are not considered when factoring out.

				As with all Do functions actions are generated to undo/redo 

	Errors:		-
	SeeAlso:	UndoableOperation::DoLocaliseCommonAttributes
	SeeAlso:	NodeRenderableInk::FactorOutCommonChildAttributes

********************************************************************************************/

BOOL UndoableOperation::DoFactorOutCommonChildAttributes(NodeRenderableInk* CompoundObject, 
														 BOOL Global, 
														 AttrTypeSet* pAffectedAttrTypes /* = NULL */)
{
	LocaliseCommonAttrAct* UndoAct;
				 
	if (!CompoundObject->DiscardsAttributeChildren())
	{
		// Make a copy of the attribute type set to store in the Action
		AttrTypeSet* pSet = NULL;
		if (pAffectedAttrTypes)
		{
			pSet = pAffectedAttrTypes->CopySet(); 
			if (!pSet)
				return FALSE; // Failed to create set
		}

		// Create an action to localise attributes which have been factored out
		if ( LocaliseCommonAttrAct::Init(this,
			 		 					 &UndoActions,
					 					 CompoundObject,
										 Global,
										 pSet,
					 					 (Action**)(&UndoAct))
					 					  == AC_FAIL)
			return FALSE;
	}

	if (!CompoundObject->FactorOutCommonChildAttributes(Global, pAffectedAttrTypes))
	{
		FailAndExecuteAllButLast();
		return FALSE; 
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL UndoableOperation::DoFactorOutCommonAttributes(NodeRenderableInk* CompoundObject, 
														 BOOL Global = FALSE,
														 List* pAffectedAttrTypes = NULL)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/2004

	Inputs:		CompoundObject:	The object to factor out
				Global:	When TRUE we recursively factor out attributes in all parent compounds 
						of the CompoundObject (Bottom up)
				pAffectedAttrTypes: An optional set of attribute types. If this is specified
									then we only consider factoring out those attributes
									which have a type which is in this set. 
	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 
	Purpose:	Just a wrapper to get function name consistency and do the
				common IsCompound test to prevent DFOCCA from barfing...

	Errors:		-
	SeeAlso:	UndoableOperation::DoLocaliseCommonAttributes
	SeeAlso:	NodeRenderableInk::FactorOutCommonChildAttributes

********************************************************************************************/

BOOL UndoableOperation::DoFactorOutCommonAttributes(NodeRenderableInk* pRootNode, 
													 BOOL bGlobal, 
													 AttrTypeSet* pAffectedAttrTypes /* = NULL */)
{
	if (pRootNode->IsCompound())
		DoFactorOutCommonChildAttributes(pRootNode, bGlobal, pAffectedAttrTypes);
		
	return TRUE;
}

/********************************************************************************************

>	BOOL  UndoableOperation::DoLocaliseCommonAttributes(NodeRenderableInk* CompoundObject, 
														BOOL CheckForDuplicates = FALSE,
														BOOL Global = FALSE,  
														AttrTypeSet* pAffectedAttrTypes = NULL)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/8/94
	
	Inputs:		CompoundObject:	The compound object to localise
	
				CheckForDuplicates: Checks for duplicate attributes being attached to 
									children of the compound. This should never be 
									the case as it would indicate that common attributes
									are incorrect.

				Global: TRUE indicates that we should localise the attributes on all
						parent compound nodes (top down), before localising the attributes for
						this compound.

				pAffectedAttrTypes: An optional set of attribute types. If this is specified
									then we only consider localising  those attributes
									which have a type which is in this set. 

	Outputs:	-
	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 

	Purpose:	This function is the opposite of DoFactorOutCommonChildAttributes
				it copies all attributes common to the compound object to each child object 
				within the group which requires each attribute. The groups common attributes 
				are deleted.

				As with all Do functions actions are generated to undo/redo 

	Errors:		-
	SeeAlso:	UndoableOperation::DoFactorOutCommonChildAttributes
	SeeAlso:	NodeRenderableInk::LocaliseCommonAttributes

********************************************************************************************/


BOOL UndoableOperation::DoLocaliseCommonAttributes(NodeRenderableInk* CompoundObject, 
												   BOOL CheckForDuplicates /* FALSE */, 
												   BOOL Global /*= FALSE*/,  
												   AttrTypeSet* pAffectedAttrTypes /*= NULL*/)
{	

	FactorOutCommonChildAttrAct* UndoAct;

	
	// Create an action to localise attributes which have been factored out
	if (!CompoundObject->DiscardsAttributeChildren())
	{
		// Make a copy of the attribute type set to store in the Action
		AttrTypeSet* pSet = NULL;
		if (pAffectedAttrTypes)
		{
			pSet = pAffectedAttrTypes->CopySet(); 
			if (!pSet)
				return FALSE; // Failed to create set
		}

		if ( FactorOutCommonChildAttrAct::Init(this, 
			 		 				 		   &UndoActions, 
					 				 		   CompoundObject,
											   Global, 
											   pSet,
					 				 		   (Action**)(&UndoAct))
					 				  		   == AC_FAIL)
			return FALSE;
	}

	if (!CompoundObject->LocaliseCommonAttributes(CheckForDuplicates, 
													  Global, 
													  pAffectedAttrTypes))
	{
		FailAndExecuteAllButLast(); 
		return FALSE; 
	}

	return TRUE;
}






/********************************************************************************************

static BOOL UndoableOperation::RegisterOpDescriptor(
			 					     UINT32 toolID,       
			 					  	 UINT32 txID, 
			 					  	 CCRuntimeClass* RuntimeClass,        
				 					 TCHAR* tok,	
				 					 pfnGetState gs,	 
				 					 UINT32 helpId, 
									 UINT32 bubbleID ,
									 UINT32 resourceID,  
				 					 UINT32 controlID,
				 					 SystemBarType GroupBarID,	 
				 					 BOOL ReceiveMessages, 
				 					 BOOL Smart,
				 					 BOOL Clean,
				 					 UINT32 OneOpenInstID,		
									 UINT32 AutoStateFlags)	

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/94
	Inputs:	
			 	toolID 			: Module Identifier
			    txID  			: String Resource ID
				RuntimeClass	: The runtime class of the Opdescriptor's operation
			    tok,			: pointer to the token string
			    gs,	 			: pointer to the GetState function
			    helpId 			: help identifier 
			    bubbleID   		: string resource for bubble help
			    bmpID = 0,		: bitmap ("icon") resource ID
			    ReceiveMessages : Does the OpDescriptor need to respond to system messages
			    Smart 			; A smart duplicate operation ?
			    Clean 			: Does the operation change the document
				OneOpenInstID	: When not 0 only one live instance
				 				  of the operation is permitted, and
								  OneOpenInstID is the string resource
								  ID describing why.This will be useful
				 				  for 1 open instance dialogs.
				AutoStateFlags  : Flags which indicate conditions when
				 				  the operation should automatically be
								  greyed/ticked etc..(This cuts down the 
								  number of tests that need to be made in the 
								  GetState function). See Opdesc.h for a description
	
								  
			      
	Outputs:	-
	Returns:	TRUE if successful, else ERROR is called and FALSE returned. 

	Purpose:	This function should be called from the operations Init method. It 
				creates an OpDescriptor for the operation

	SeeAlso:	UndoableOpDescriptor::UndoableOpDescriptor

********************************************************************************************/


BOOL UndoableOperation::RegisterOpDescriptor(
			 					     UINT32 toolID,       
			 					  	 UINT32 txID, 
			 					  	 CCRuntimeClass* RuntimeClass,        
				 					 TCHAR* tok,	
				 					 pfnGetState gs,	 
				 					 UINT32 helpId, 
									 UINT32 bubbleID ,
									 UINT32 resourceID,  
				 					 UINT32 controlID,
				 					 SystemBarType GroupBarID,	 
				 					 BOOL ReceiveMessages, 
				 					 BOOL Smart,
				 					 BOOL Clean,
				 					 UINT32 OneOpenInstID,		
									 UINT32 AutoStateFlags)
{
	// tok is going into a string which turns out to be a string 32, so do a sanity check (Neville 26/6/97)
	size_t len = camStrclen(tok);
	ERROR2IF(len > 32,FALSE,"UndoableOperation::RegisterOpDescriptor token buffer overflow!");

	// Try to create the OpDescriptor
	UndoableOpDescriptor* OpDesc = new UndoableOpDescriptor(
								 							toolID,
								 							txID,
															RuntimeClass, 
															tok,
								 							gs,
								 							helpId,
								 							bubbleID,
															resourceID,
															controlID,
								 							ReceiveMessages, 
															Smart,
								 							Clean,
								 							OneOpenInstID,
								 							AutoStateFlags	
				 						   				   );

	ERRORIF(!OpDesc, _R(IDE_NOMORE_MEMORY), FALSE);
	return TRUE; 
}  



/********************************************************************************************

>	BOOL UndoableOperation::DoInsertPathElement(NodePath* ThisPath, 
												INT32 Index, 
												DocCoord NewCoord,
												PathFlags NewFlags,
												PathVerb NewVerb,
												BOOL RedrawPath = TRUE);


	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Inputs:		ThisPath points at the NodePath into which we want to insert an element
				Index - the the element will be inserted after this element
				NewCoord is the new element's coordinate value
				NewFlags is the new element's flags value
				NewVerb is the new element's verb value
				RedrawPath - TRUE if the screen area of the path should be redrawn, FALSE
				if the caller is responsible
	Outputs:	-
	Returns:	TRUE if succeeded, FALSE otherwise
	Purpose:	This 'Do' function adds an element into a path, recording undo information
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL UndoableOperation::DoInsertPathElement(NodePath* ThisPath, 
											INT32 Index, 
											DocCoord NewCoord,
											PathFlags NewFlags,
											PathVerb NewVerb,
											BOOL RedrawPath)
{
#ifndef STANDALONE

	// Create an undo action for this action, which is a RemoveElementAction
	RemovePathElementAction* pAction = NULL;
	ActionCode Act;
	Act = RemovePathElementAction::Init(this, 
									&UndoActions, 
									1,
									Index+1,
									(Action**)(&pAction));
	if (Act == AC_FAIL)
		return FALSE;

	// Tell the operation where the path is
	if (pAction != NULL)
		pAction->RecordPath(ThisPath);

	Document* pDocument = GetWorkingDoc();
	ERROR2IF(pDocument == NULL, FALSE, "There was no Document when deleteing path elements");
	Spread* pSpread = ThisPath->FindParentSpread();
	ERROR2IF(pSpread == NULL, FALSE, "Path had no parent spread");

	// Force a re-draw of the place where the path used to be
	if (RedrawPath)
	{
		DocRect Invalid = ThisPath->GetUnionBlobBoundingRect();
		// Mark the region as requiring object to recache any cached
		// data they may be holding because the Op may have changed it
		pDocument->ForceRedraw( pSpread, Invalid, FALSE, ThisPath );
	}

	// Insert the new element
	ThisPath->InkPath.InsertSection(Index,1);

	// Get pointers to all the arrays of the path
	PathVerb* Verbs = NULL;
	DocCoord* Coords = NULL;
	PathFlags* Flags = NULL;
	ThisPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);

	// Update the coords
	Verbs[Index+1] = NewVerb;
	Flags[Index+1] = NewFlags;
	Coords[Index+1] = NewCoord;

	// The bounding rect may have changed
	ThisPath->InvalidateBoundingRect();

	// Redraw the new area
	if (RedrawPath)
	{
		DocRect Invalid = ThisPath->GetUnionBlobBoundingRect();
		// Mark the region as requiring object to recache any cached
		// data they may be holding because the Op may have changed it
		pDocument->ForceRedraw( pSpread, Invalid, FALSE, ThisPath );
	}

#endif
	return TRUE;
}



/********************************************************************************************

>	BOOL UndoableOperation::DoReversePath (NodePath* ThisPath, BOOL RedrawPath = TRUE)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/2000
	Inputs:		ThisPath points at the NodePath that we want to reverse
				RedrawPath - TRUE if the screen area of the path should be redrawn, FALSE
				if the caller is responsible
	Outputs:	The reversed path in ThisPath->InkPath
	Returns:	TRUE if succeeded, FALSE otherwise
	Purpose:	This 'Do' function reverse a path, recording undo information
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL UndoableOperation::DoReversePath (NodePath* ThisPath, BOOL RedrawPath /*= TRUE*/)
{
	PathVerb* Verbs = NULL;
	PathFlags* Flags = NULL;
	DocCoord* Coords = NULL;
	ThisPath->InkPath.GetPathArrays(&Verbs, &Coords, &Flags);
	INT32 NumCoords = ThisPath->InkPath.GetNumCoords();
	//BOOL PrevSelected = FALSE;
	//INT32 PrevPos = 0;
	
	Document* pDocument = GetWorkingDoc();
	Spread* pSpread = ThisPath->FindParentSpread();
	
	if (RedrawPath)
	{
		// Mark the region as requiring object to recache any cached
		// data they may be holding because the Op may have changed it
		DocRect Invalid = ThisPath->GetUnionBlobBoundingRect();
		pDocument->ForceRedraw( pSpread, Invalid, FALSE, ThisPath );
	}

	Path* NewEditPath = new Path ();

	if (!NewEditPath) { return (FALSE); }

	if (!NewEditPath->Initialise(NumCoords, 24))
	{
		return FALSE;
	}

	// for the new reversed path ....

	PathVerb* Verbs2 = NULL;
	PathFlags* Flags2 = NULL;
	DocCoord* Coords2 = NULL;
	NewEditPath->GetPathArrays(&Verbs2, &Coords2, &Flags2);

	NewEditPath->CopyPathDataFrom (&(ThisPath->InkPath));

	// the following loop as been optimised since this is the main part of the alg
	// (which just reverses the supplied path) ....

	NewEditPath->Reverse ();
	
	// now lets build an undo record ....
	
	ModifyPathAction* ModAction;				// since thats what were doing
	
	ActionCode Act;
	Act = ModifyPathAction::Init(this, &UndoActions, NumCoords, (Action**)(&ModAction));
	if (Act == AC_FAIL)
	{
		FailAndExecute();
		End();
		return FALSE;
	}

	// for use with the action ....
	
	PathVerb* ChangedVerbs = NULL;
	DocCoord* ChangedCoords = NULL;
	PathFlags* ChangedFlags = NULL;
	INT32* ChangedIndices = NULL;

	// If the function returned AC_NO_RECORD we shouldn't record any undo information in the action
	// NOTE - during unwind all actions return AC_OK with a NULL ModAction pointer!!
	if ((Act!=AC_NORECORD) && (ModAction!=NULL))
	{
		// This next bit is a bit hellish. Any one of these four allocations can fail, in which case 
		// we have to tidy up afterwards. Cue a lot of nested ifs and elses.

		ALLOC_WITH_FAIL(ChangedVerbs,(PathVerb*) CCMalloc(NumCoords * sizeof(PathVerb)),this);
		if (ChangedVerbs)
		{
			ALLOC_WITH_FAIL(ChangedCoords,(DocCoord*) CCMalloc(NumCoords * sizeof(DocCoord)),this);
			if (ChangedCoords)
			{
				ALLOC_WITH_FAIL(ChangedFlags,(PathFlags*) CCMalloc(NumCoords * sizeof(PathFlags)),this);
				if (ChangedFlags)
				{
					ALLOC_WITH_FAIL(ChangedIndices,(INT32*) CCMalloc(NumCoords * sizeof(INT32)),this);
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
		for (INT32 loop = 0; loop < NumCoords; loop++)
		{
			ChangedVerbs [loop] = Verbs [loop];
			ChangedFlags [loop] = Flags [loop];
			ChangedCoords [loop] = Coords [loop];
			ChangedIndices [loop] = loop;
		}
	
		// Now we've allocated the arrays, let's tell the action about 'em
		ModAction->StoreArrays (ChangedVerbs, ChangedFlags, ChangedCoords, ChangedIndices, ThisPath);//pOrigPath);

	}

/////////////////////////////////////////////////////////////////////////////////////////////////
			
	// now we update the supplied path with the new path ....

	ThisPath->InkPath.CopyPathDataFrom (NewEditPath);
	
	delete (NewEditPath);		// and clean up

	if (ThisPath->IsANodeBlendPath ())
	{
		((NodeBlendPath*) ThisPath)->DestroyCachedInformation ();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////

	ThisPath->InvalidateBoundingRect();

	if (RedrawPath)
	{		
		DocRect Invalid = ThisPath->GetUnionBlobBoundingRect();
		// Mark the region as requiring object to recache any cached
		// data they may be holding because the Op may have changed it
		pDocument->ForceRedraw( pSpread, Invalid, FALSE, ThisPath );
	}
	return (TRUE);
}



/********************************************************************************************

>	virtual void UndoableOperation::PerformMergeProcessing()  


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/11/94
	Inputs:		
	Outputs:	-
	Returns:	- 
	Purpose:	This function gets called from the Operation::EndOp method after the operation
				has ended successfuly and been added to the operation history. If the operation
				could potentially merge itself with the previous operation then perform this 
				merging here.

				Note that this base class function does nothing.
				
				Here are some rules:
				
				1. Note that this function cannot fail - doing so would really be quite scarry.
				   If however you find yourself running out of memory in this function then 
				   simply tidyup (leaving things how they were) and then return. 	
				   
				2. After this function has been invoked the operation history size must not be
				   greater than it was prior to the function getting called. It would be quite
				   unimpressive if this was the case anyway.

				3. In many cases you will want to delete 'this' operation from this function. so
				   just be careful huh!

				The way that you merge the operation is basically up to you, however here are a few 
				pointers. 

				* The first thing you should do on entering this function is to check that the 
				last Operation added to the operation history is this operation. Call 
				OperationHistory::FindLastOp to verify this within an ERROR2 Macro.

				* Next you will probably want to check if the previous operation performed can be
				merged with this op. To obtain the previous op call OperationHistory::FindPrevToLastOp.
				Testing the runtime class of this op is probably enough for you to determine if
				a merge can take place or not.

				* Usually you will want to augment the previous operation in some way so that it
				includes the function which was performed by this operation. This could involve
				changing an action in the previous operation, adding new actions etc. 



	Errors:		-
	SeeAlso:	OperationHistory::FindLastOp 
				OperationHistory::FindPrevToLastOp
				OperationHistory::DeleteLastOp
				ActionList:: 


********************************************************************************************/
void UndoableOperation::PerformMergeProcessing()
{
	return; // The base class function does nowt
} 



/********************************************************************************************
>	void UndoableOperation::MergeWithPrevious()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/96
	Inputs:		-
	Outputs:	-
	Returns:	- 
	Purpose:	Moves all the actions in this operation to the end of the previous action, then
				deletes this action
	SeeAlso:	UndoableOperation::PerformMergeProcessing()
********************************************************************************************/
void UndoableOperation::MergeWithPrevious()
{
	// Obtain a pointer to the operation history for the operation's document
	OperationHistory* pOpHist = &GetWorkingDoc()->GetOpHistory();
	if (pOpHist == NULL)
	{
		ERROR3("There's no OpHistory!?");
		return;
	}

	// Ensure that we are the last operation added to the operation history
	if (pOpHist->FindLastOp() != this)
	{
		ERROR3("Last Op should be this op");
		return;
	}
	
	Operation* pPrevOp = pOpHist->FindPrevToLastOp();
	if (pPrevOp != NULL)
	{	
		// Copy all the actions from us to the end of the action list in the previous op.
		ActionList* pPrevActions = pPrevOp->GetUndoActionList();
		ActionList* pPrevOtherActions = pPrevOp->GetRedoActionList();
		if ((pPrevActions == NULL) || (pPrevOtherActions == NULL))
		{
			ERROR3("Previous actions list pointer was NULL");
			return;
		}

		// Copy actions across
		Action* pCurrentAction = (Action*)UndoActions.RemoveHead();
		while (pCurrentAction != NULL)
		{
			pCurrentAction->TransferToOtherOp(pPrevOp, pPrevActions, pPrevOtherActions);
			pCurrentAction = (Action*)UndoActions.RemoveHead();
		}
	}

	// And remove ourself
	pOpHist->DeleteLastOp();
} 

/********************************************************************************************
>	void UndoableOperation::IncludeParentTransformNodesInRange(Range * Rng)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/99
	Inputs:		-
	Outputs:	A new range (to be deleted outside of this function)
	Returns:	- 
	Purpose:	Transforms the node, checking if any parents need to transform too
	SeeAlso:	
********************************************************************************************/
Range * UndoableOperation::IncludeParentTransformNodesInRange(Range * Rng)
{
	// do this by changing the selection, and then restoring it
	// first, save the current selection
	List * pSelList = GetApplication()->FindSelection()->MakeListOfNodes(TRUE);
	List * pRngList = Rng->MakeListOfNodes(FALSE);

	NodeRenderableInk::DeselectAll();

	NodeListItem * pItem = (NodeListItem *)pRngList->GetHead();
	Node * pParent = NULL;

	while (pItem)
	{
		if (pItem->pNode)
		{
			if (pItem->pNode->IsRenderable())
			{
				pItem->pNode->SetSelected(TRUE);
				
				pParent = pItem->pNode->FindParent();
				
				while (pParent)
				{
					if (pParent->ShouldITransformWithChildren())
					{
						pParent->SetSelected(TRUE);
					}
					
					pParent = pParent->FindParent();
				}
			}				
		}

		pItem = (NodeListItem *)pRngList->GetNext(pItem);
	}

	pRngList->DeleteAll();
	delete pRngList;

	// update the selection range and make a copy of it
	GetApplication()->UpdateSelection();

	Range * RetnRng = new Range(*(GetApplication()->FindSelection()));

	// restore the selection
	NodeRenderableInk::DeselectAll();

	pItem = (NodeListItem *)pSelList->GetHead();

	while (pItem)
	{
		if (pItem->pNode)
		{
			pItem->pNode->SetSelected(TRUE);
		}

		pItem = (NodeListItem *)pSelList->GetNext(pItem);
	}

	// update the selection range and make a copy of it
	GetApplication()->UpdateSelection();
	
	pSelList->DeleteAll();
	delete pSelList;

	// set the range control flags
	RangeControl rg = RetnRng->GetRangeControlFlags();
	rg.PromoteToParent = TRUE;

	RetnRng->SetRangeControl(rg);
	
	return RetnRng;
}


/********************************************************************************************

> 	virtual BOOL UndoableOperation::UpdateChangedNodes(	ObjChangeParam* pParam,
														Spread* pSpread = NULL )

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/2000
	Inputs:		pParam		describes the way an op has changes the node(s)
				pSpread		pointer to the spread containing the nodes affected by the Op
							NULL => use the currently selected spread.
	Returns:	TRUE if all affected nodes were able to cope with the Op,
				FALSE otherwise.

	Purpose:	This function overrides its namesake in Operation, calling the base function
				first and then adding a call to NameGallery::PostTriggerEdit if this Op
				admits it may change the bounds of nodes within the tree.

	Errors:		ERROR2 if pParam is NULL.
	SeeAlso:	Operation::UpdateChangedNodes(ObjChangeParam*, Spread*)

********************************************************************************************/

BOOL UndoableOperation::UpdateChangedNodes(ObjChangeParam* pParam,Spread* pSpread)
{
	ERROR2IF(pParam == NULL, FALSE, "pParam == NULL");
	BOOL	ok = Operation::UpdateChangedNodes(pParam, pSpread);
	
	/////////////////////////////////////////////////////////////////////////////////////////
	// Karim MacDonald 19/01/2000.
	// If the call to the base class was successful, then we can try to call
	// NameGallery::PostTriggerEdit, to allow any nodes marked for stretching to
	// change in response to any bounds changes in the tree.
	if (ok)
	{
		// When querying the Op responsible for the change, I've decided to use 'this'.
		// Could've asked pParam for the Op's pointer, but 'this' is probably more reliable.
		UndoableOperation* pChangeOp = this;// pParam->GetOpPointer();
		if (pChangeOp != NULL && pChangeOp->MayChangeNodeBounds() && NameGallery::Instance())
		{
			NameGallery::Instance()->PostTriggerEdit(pChangeOp, pParam);
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////

	return ok;
}
				


/********************************************************************************************

>	BOOL UndoableOperation::NoStretchUpdateChangedNodes(ObjChangeParam* pParam,
														Document* pDoc)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/2000
	Inputs:		pParam	description of the change.
				pDoc	pointer to the document.
	Outputs:	Causes any nodes marked for updating to update themselves.
	Returns:	TRUE if successful,
				FALSE otherwise.

	Purpose:	This is *not* an override of Operation::UpdateChangedNodes(..., Document*),
				although it performs *almost* exactly the same job. It is provided solely
				for NameGallery::PostTriggerEdit to call in order to get all affected nodes
				to update themselves correctly after a stretch. PostTriggerEdit is itself
				called in response to normal UpdateChangedNodes calls, so this avoids
				recursion problems.

	Errors:		We ERROR2 if either of the parameters is NULL.
	See also:	Operation::UpdateChangedNodes(ObjChangeParam*, Document*).

********************************************************************************************/
BOOL UndoableOperation::NoStretchUpdateChangedNodes(ObjChangeParam* pParam, Document* pDoc)
{
	ERROR2IF(pParam == NULL, FALSE, "pParam == NULL");
	ERROR2IF(pDoc	== NULL, FALSE, "pDoc == NULL");

	BOOL ok = TRUE;
	Chapter* pChapter = Node::FindFirstChapter(pDoc);
	while (pChapter != NULL && ok)
	{
		// Now find the first child of the Chapter, because this is the level at which Spreads hang out
		Node* pNode = pChapter->FindFirstChild();

		// Update changed nodes in each of the chapter's spreads. We *don't* call
		// Operation::UpdateChangedNodes(..., Spread*), as that would lead to recursion...
		while (pNode != NULL && ok)
		{
			if (pNode->IsSpread())
			{
				Spread* pSpread = (Spread*)pNode;
				ok = UpdateChangedNode(pParam, pSpread->FindFirstChild());
			}

			pNode = pNode->FindNext();
		}

		pChapter = pChapter->FindNextChapter(); 
	}

	return ok;
}

