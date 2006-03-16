// $Id: groupops.cpp 662 2006-03-14 21:31:49Z alex $
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
*/

#include "camtypes.h" 
#include "camelot.h"
//#include "grptrans.h"
#include "groupops.h"
#include "group.h"
#include "undoop.h"
//#include "resource.h" 
//#include "simon.h"
#include "errors.h" 
#include "ensure.h"   
#include "docview.h"              
#include "opdesc.h"
//#include "mario.h"
#include "nodeattr.h"     
#include "ink.h"
#include "app.h"
#include "document.h"
#include "spread.h"
#include "layer.h"
#include "tool.h"
#include "bubbleid.h"
#include "fthrattr.h"
#include "opliveeffects.h"
#include "effects_stack.h"
//#include "phil.h"
#include "ophist.h"

DECLARE_SOURCE("$Revision: 662 $");  

CC_IMPLEMENT_DYNCREATE(OpGroup, SelOperation)  
CC_IMPLEMENT_DYNCREATE(OpUngroup, SelOperation)  
CC_IMPLEMENT_DYNCREATE(OpUngroupSpecial, OpUngroup)  

#define new CAM_DEBUG_NEW

/*********************************************************************************************

	Preference:	AskUngroupEffect
	Section:	Groups
	Range:		FALSE - TRUE
	Purpose:	Flag controlling whether user is asked about factoring out effects during ungroup
	SeeAlso:	-

**********************************************************************************************/ 
BOOL OpGroup::bAskBeforeFactoringEffects = FALSE;


/*********************************************************************************************

	Preference:	UngroupLocaliseEffects
	Section:	Groups
	Range:		FALSE - TRUE
	Purpose:	Flag controlling whether effects are localised by default or removed by default
	SeeAlso:	-

**********************************************************************************************/ 
BOOL OpGroup::bLocaliseEffects = FALSE;


// ------------------------------------------------------------------------------------------
// OpGroup methods
            
/********************************************************************************************

>	OpGroup::OpGroup() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/93
	Purpose:	OpGroup constructor

********************************************************************************************/
OpGroup::OpGroup(): SelOperation()								
{                              
}

 /********************************************************************************************

>	BOOL OpGroup::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpGroup initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.

********************************************************************************************/
BOOL OpGroup::Init()
{
	return (RegisterOpDescriptor(0,
 							_R(IDS_GROUPOP),
							CC_RUNTIME_CLASS(OpGroup),
 							OPTOKEN_GROUP,
 							OpGroup::GetState,
 							0,	/* help ID */
 							_R(IDBBL_GROUPOP),
 							0,	/* bitmap ID */
 							0,
							SYSTEMBAR_ILLEGAL,			// For now !
							TRUE, 						// Receive messages
							FALSE,
							FALSE,
							0,
							(GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION)

							)); 

	if (Camelot.DeclareSection(_T("Groups"), 2))
	{
		Camelot.DeclarePref(_T("Groups"), _T("AskUngroupEffects"), &bAskBeforeFactoringEffects, FALSE, TRUE);
		Camelot.DeclarePref(_T("Groups"), _T("UngroupLocaliseEffects"), &bLocaliseEffects, FALSE, TRUE);
	}
}               
    
/********************************************************************************************

>	OpState	OpGroup::GetState(String_256*, OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Returns:	The state of the OpGroup
	Purpose:	For finding the OpGroup's state. 

********************************************************************************************/
OpState	OpGroup::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
	String_256 DisableReason; 

	// Obtain the current selections 
	Range Sel(*(GetApplication()->FindSelection()));
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);
	  
	// This operation is disabled if there are no nodes selected, or if the only selected node
	// is a node group.                           
	Node* FirstSelected = Sel.FindFirst();  
	
	OpSt.Greyed = FALSE; 
	if (IS_A(FirstSelected,NodeGroup) 
		 &&	(Sel.FindNext(FirstSelected) == NULL) )      // Only a NodeGroup selected
	{  
		OpSt.Greyed = TRUE; 
		// Load reason why operation is disabled
		// Grouping only a single group is pointless
		DisableReason = String_256(_R(IDS_ILLEGAL_TO_GROUP_SINGLE_GROUP));
   		*UIDescription = DisableReason;	         
	}
		
	return(OpSt);   
}


/********************************************************************************************

>	BOOL OpGroup::NeedToInvalidateGroup()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/7/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if we need to invalidate
	Purpose:	This function determines if the selected objects will need to be invalidated
				if they are to be made into a group. 

				

	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/

BOOL OpGroup::NeedToInvalidateGroup()
{
//	if (TRUE /*GetApplication()->IsBkRendering()*/)
//		return TRUE;		// Always invalidate if Background Redraw is occuring

	// If all selected objects are contiguous then there will be no need to invalidate
	// the group's bounds.

	// Obtain the current selection
	SelRange Sel(*(GetApplication()->FindSelection()));
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);
	
	Node* Current = Sel.FindFirst(); 
	
	ERROR2IF(Current == NULL, TRUE, "There are no selected objects"); 

	// Create a range of all objects starting with the first selected object
		
//	RangeControl rc = { TRUE, TRUE, TRUE }; // selected + unselected nodes, cross layer

	Range ObjectRange(Current, NULL, RangeControl(TRUE,TRUE,TRUE,TRUE));

	BOOL Contiguous = TRUE; // Until we know better
	DocRect SelBounds;

	Node* SelObject = Current; 
	Node* Object = Current;
	
	// Traverse both ranges simultaneously
	while (SelObject != NULL)
	{
		if (SelObject != Object)
		{
			// All selected objects are not contiguous
			if (Contiguous)
			{
				// We will need to calculate the bounds of the selection for what comes next
				SelBounds = Sel.GetBoundingRect();
				Contiguous = FALSE; // So we don't calculate the bounds again
			}
		
			// Check if the uncontiguous object intersects with the bounds of the selection
		
			if (Object->IsAnObject())
			{ 

				if ( ((NodeRenderableInk*)Object)->GetBoundingRect().IsIntersectedWith(SelBounds))
				{
					return TRUE; // An unselected object intersects with a selected object
				}
			}
			// Objects don't intersect so it doesn't matter
			Object = ObjectRange.FindNext(Object);
			ERROR3IF(Object == NULL, "Reached end of object list"); 
		}
		else
		{
			// Get next items in both ranges
			SelObject = Sel.FindNext(SelObject); 
			Object = ObjectRange.FindNext(Object);
		}
	}
	return FALSE; // No need to invalidate

}

/********************************************************************************************

>	void OpGroup::Do(OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the Group operation. 
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
	
void OpGroup::Do(OpDescriptor*)
{   
	// Obtain the current selections 
	Range Sel(*(GetApplication()->FindSelection()));
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);

	// Find the first node which is selected 
	Node* FirstSelectedNode = Sel.FindFirst(); 
	
	ENSURE(FirstSelectedNode != NULL, "Called group operation with no nodes selected"); 
	
	HideNodeAction* UndoHideNodeAction; 

	// In the retail build it is best to do nothing if we find there are no selected nodes 
	if (FirstSelectedNode != NULL) // No nodes selected so End
	{   
		// Get the current tool
		Tool* pTool = Tool::GetCurrent();
		Spread* pSelSpread = Document::GetSelectedSpread();

		// Get the tool to remove all its blobs before we deselect the nodes.
		// Only do this if the current tool dosent update itself on sel changed messages
		if (pSelSpread!=NULL && pTool!=NULL && !pTool->AreToolBlobsRenderedOnSelection())
			pTool->RenderToolBlobs(pSelSpread,NULL);

		// Determine if it will be neccessary to invalidate the groups bounds
		BOOL InvalidateBounds = TRUE; 
		BOOL IfBgRedraw = !NeedToInvalidateGroup();

		// Draw blobs if we are not invalidating the object bounds
		if (!DoStartSelOp(IfBgRedraw,IfBgRedraw)) 
		{
			End();
			return;
		}
		
		if (InvalidateBounds)
		{
			// We need to invalidate the region
			if (!DoInvalidateNodesRegions(Sel, TRUE, FALSE, IfBgRedraw))
			{
				End();
				return;
			} 
		}

		// Go and find the spread that the selected objects are on
#ifdef _DEBUG
		Spread* pSpread = FirstSelectedNode->FindParentSpread();
		ENSURE(pSpread != NULL, "A selected nodes spread is NULL"); 
#endif

        // Find the selected node with the highest z-order position, as we will need to insert 
        // the group node here
        Node* FinalNode = NULL; 

		for(Node* n = Sel.FindFirst(); n != NULL; n = Sel.FindNext(n))
		{   
			FinalNode = n; 
		}

		ENSURE(FinalNode != NULL, "The final selected node is NULL ?"); 
                                        
		// Create a group node and attach it as a next sibling of the final selected node    
		NodeGroup* Group; 
		ALLOC_WITH_FAIL(Group, (new NodeGroup(FinalNode, NEXT)), this);
		// If the allocation fails the FailAndExecute will have been called 
		if (Group != NULL)				   
		{    
			// Create an action to hide the group when we undo 
			if ( HideNodeAction::Init(this, 
		 		 &UndoActions, 
				 Group, 
				 FALSE, // Don't include subtree size
				 (Action**)(&UndoHideNodeAction),
				 FALSE)	// Don't tell subtree when undone
				 == AC_FAIL)  
			{
				Group->UnlinkNodeFromTree();
				delete (Group); 

				End();
				return;
			};
		       
			// Take each node in the range and attach it to the NodeGroup
 			Node* n = Sel.FindFirst(); // Get first node to be added to group   
	
			// Loop until all nodes in range have been added to the group 	
 			Node* NxtInRange; 
 			
 			Node* LastObjectMoved = NULL; 	
 			  
 			DocRect GroupBounds; // bounding rectangle of the group  
 			while (n != NULL) // Stop when there are no more selected nodes. 
 			{                                                     
 				// Ensure that the selected node is a NodeRenderableInk
 				ENSURE(n->IsAnObject(), 
 					   "Selected node is not a NodeRenderableInk");    
 				NxtInRange = Sel.FindNext(n); // Neccessary because n is 
 											  // just about to be moved
 				// Attach n to NodeGroup 
				if (LastObjectMoved == NULL)
				{
	 				if (!DoMoveNode((NodeRenderableInk*)n, Group, LASTCHILD)) 
					{
						End();
						return;
					}
				}
				else
				{
					// We can do it quicker
	 				if (!DoMoveNode((NodeRenderableInk*)n, LastObjectMoved, NEXT)) 
					{
						End();
						return;
					}
				}

				// Deselect the object
				// Botch - until selstate restores child states
				//if (!DoDeselectNode(((NodeRenderableInk*)n)))
				//{
				//	goto EndOperation;
				//}
				((NodeRenderableInk*)n)->DeSelect(FALSE, TRUE); 

				GroupBounds = 
					GroupBounds.Union( ((NodeRenderableBounded*)n)->GetBoundingRect());
				
				
				LastObjectMoved = n; 

 				n = NxtInRange;  				// Get the next node in the range   
 			} 
			
                 
 			// Set the group's bounding rectangle 
			Group->InvalidateBoundingRect();

			// Select the group and draw its blobs   
			((NodeRenderable*)Group)->Select(FALSE);
			
			if (InvalidateBounds)
			{
				if (!DoInvalidateNodesRegions(*(GetApplication()->FindSelection()), TRUE, FALSE, IfBgRedraw))
				{
					// We need to invalidate the region
					End();
					return;
				}
			}

			// Now factor out all common attributes
			// bodge 
			if (!DoFactorOutCommonChildAttributes(Group))
			{
				End();
				return;
			}
 		}

		// Get the tool to remove all its blobs before we deselect the nodes.
		// Only do this if the current tool dosent update itself on sel changed messages
		if (pSelSpread!=NULL && pTool!=NULL && !pTool->AreToolBlobsRenderedOnSelection())
			pTool->RenderToolBlobs(pSelSpread,NULL);
	}                   
 	End(); 
}			


    
// ------------------------------------------------------------------------------------------
// OpUngroup methods
    
/********************************************************************************************

>	OpUngroup::OpUngroup() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpUngroup constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
            
OpUngroup::OpUngroup(): SelOperation()								
{                              
}

 /********************************************************************************************

>	BOOL OpUngroup::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpUngroup initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpUngroup::Init()
{
	return (RegisterOpDescriptor(0,
	 							_R(IDS_UNGROUPOP),
								CC_RUNTIME_CLASS(OpUngroup),
	 							OPTOKEN_UNGROUP,
	 							OpUngroup::GetState,
	 							0,	/* help ID */
	 							_R(IDBBL_UNGROUPOP),
	 							0,	/* bitmap ID */
	 							0,
								SYSTEMBAR_ILLEGAL,			// For now !
								TRUE, 						// Receive messages
								FALSE,
								FALSE,
								0,
								(GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION)

								 )); 
}               

/********************************************************************************************

>	OpState	OpUngroup::GetState(String_256*, OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpUngroup
	Purpose:	For finding the OpUngroup's state. 
	Errors:		-
	SeeAlso:	-

	Notes:		Karim 06/10/2000
				Modified this method to check for shadowed, bevelled and contoured groups,
				as these cannot be ungrouped until the shadow, bevel or contour is removed.

********************************************************************************************/

OpState	OpUngroup::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
	
	String_256 DisableReason;

	// Obtain the current selections 
	Range Sel(*(GetApplication()->FindSelection()));

	// Determine if there are any group nodes selected,
	// and if so, whether they lie within controller nodes.
	Node* n = Sel.FindFirst();
//	Node* pParentController = NULL;
	BOOL SelectedGroup = FALSE;
//	BOOL InsideControllerNode = FALSE;
	while (n != NULL)
	{
		if (  n->IsSelected() && IS_A(n,NodeGroup) )
		{
/*			pParentController = n->FindParent();
			if (pParentController != NULL)
			{
				if (pParentController->IsABevelController() ||
//					pParentController->IsAShadowController() ||
					pParentController->IsAContourController())
				{
					InsideControllerNode = TRUE;
					break;
				}
			}
*/
			SelectedGroup = TRUE;
			break; 
		}
		n = Sel.FindNext(n);
	}
//	OpSt.Greyed = (InsideControllerNode || !SelectedGroup);
	OpSt.Greyed = !SelectedGroup;
	if (OpSt.Greyed)
	{
		// Load reason why operation is disabled.
/*
		// we're inside a controller node.
		if (InsideControllerNode)
		{
			String_256 DisableReason(_R(IDS_GROUPINSIDECONTROLLER));
			*UIDescription = DisableReason;
		}
*/
		// No group nodes are selected.
//		else if (!SelectedGroup)
		if(!SelectedGroup)
		{
			String_256 DisableReason(_R(IDS_NO_GROUPS_SEL));
			*UIDescription = DisableReason;
		}
	}
	return(OpSt);
}

/********************************************************************************************

>	void OpUngroup::Do(OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the Ungroup operation
	Errors:		An ENSURE failure will occur if this function is called and there are no 
				selected groups. 
	SeeAlso:	-

********************************************************************************************/

void OpUngroup::Do(OpDescriptor*)
{   
	if (DoStartSelOp(TRUE,TRUE))  // Try to record the selection state 
	{
		if (!UngroupSelectedGroups())
		{
			FailAndExecute(); 
		}    
	}
	End(); 
}  



/********************************************************************************************

>	BOOL OpUngroup::ParentAllowsUngroup(NodeGroup* pChild)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/10/2000

	Inputs:		pChild	the group node to check.

	Returns:	TRUE if our parent's type is good for ungrouping,
				FALSE otherwise.

	Purpose:	Nasty BODGE code!

				Checks for the user trying to ungroup a shadowed (maybe bevelled
				or contoured too in future) group, disallows the operation and
				explains to them what the problem is/how to solve it.

	Errors:		ERROR3 if the parent is an unexpected type - all such cases should
				never get this far into the ungroup code, they should have been caught
				by the GetState() checks.

********************************************************************************************/
BOOL OpUngroup::ParentAllowsUngroup(NodeGroup* pChild)
{
	Node* pParent = pChild->FindParent();
	UINT32 MessageID = 0;

	if (pParent != NULL)
	{
		// if the parent is a NodeShadowController,
		// then post a message informing the user.
		if (pParent->IsABevelController())
			MessageID = _R(IDS_GROUPINSIDEBEVEL);
//		else if (pParent->IsAShadowController())	// Shadows are now PostProcessors and allow ungrouping
//			MessageID = _R(IDS_GROUPINSIDESHADOW);
		else if (pParent->IsAContourController())
			MessageID = _R(IDS_GROUPINSIDECONTOUR);

		if(MessageID != 0)
		{
			::InformWarning(MessageID);
			return FALSE;
		}

		// we must catch any group-derived nodes which make it past any ungroup checks.
		// the GetState() code should catch all such instances, so if execution gets here,
		// then something somewhere else is broken!
		if (pParent->IsAGroup())
		{
			ERROR3("OpUngroup::UngroupSelectedGroups; Group-derived parent node got past OpUngroup checks!");
			return FALSE;
		}
	}

	return TRUE;
}


     
/********************************************************************************************

>	BOOL OpUngroup::UngroupSelectedGroups()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful
				FALSE if the operation was aborted, or if there are no selected group nodes. 

	Purpose:	The function scans the tree and ungroups selected group nodes. It implements 
				the main body of the OpUngroup Do function. 
				
				The reason the Do function was split was so that the UngroupSpecial operation 
				could re-use the code. 
								
	Errors:		-
	Scope:		protected
	SeeAlso:	OpUngroup::Do

********************************************************************************************/
     
BOOL OpUngroup::UngroupSelectedGroups()
{ 
	// Obtain the current selections
	 
	Range Sel(*(GetApplication()->FindSelection()));

	BOOL Ungrouped = FALSE; // Set to true when we ungroup a group      
	BOOL bSelectGroupsHaveEffectAttrs = FALSE;
	Node* FirstSelectedNode = Sel.FindFirst(); 
	if (FirstSelectedNode != NULL) // No nodes selected so End
	{
		BOOL bLocalisePostPros = FALSE;
		BOOL bDeletePostPros = FALSE;
		EffectsStack* pStack = GetApplication()->FindSelection()->GetEffectsStack();
		bSelectGroupsHaveEffectAttrs = FindSelectedEffectAttrs();
		if ((pStack && !pStack->IsEmpty()) || bSelectGroupsHaveEffectAttrs)
		{
			bLocalisePostPros = TRUE;
			// If the preference is set of the user forces it by holding down Shift
			// We will ask the user what he wanst to do with effects
			ClickModifiers clickmods = ClickModifiers::GetClickModifiers();
			if (OpGroup::bAskBeforeFactoringEffects || clickmods.Adjust)
			{
				OpGroup::bAskBeforeFactoringEffects = TRUE;

				// We need to ask the user about removing or localising PostProcessors
				INT32 ret = AskQuestion(_R(IDS_LE_ASK_UNGROUPLOCALISE), _R(IDS_LE_COPY), _R(IDS_LE_ALWAYSCOPY), _R(IDS_LE_REMOVE), _R(IDS_CANCEL));
				switch (ret)
				{
				case 1:	bLocalisePostPros = TRUE;
						break;
				case 2:	bLocalisePostPros = TRUE;
						OpGroup::bAskBeforeFactoringEffects = FALSE;
						OpGroup::bLocaliseEffects = TRUE;
						break;
				case 3:	bDeletePostPros = TRUE;
						OpGroup::bLocaliseEffects = FALSE;	// ?
						break;
				case 4: return FALSE;
						break;
				}
			}
			else
			{
				bLocalisePostPros = OpGroup::bLocaliseEffects;
				bDeletePostPros = !OpGroup::bLocaliseEffects;
			}
		}

		// Get the current tool
		Tool* pTool = Tool::GetCurrent();
		Spread* pSelSpread = Document::GetSelectedSpread();

		// Get the tool to remove all its blobs before we deselect the nodes.
		// Only do this if the current tool dosent update itself on sel changed messages
		if (pSelSpread!=NULL && pTool!=NULL && !pTool->AreToolBlobsRenderedOnSelection())
			pTool->RenderToolBlobs(pSelSpread,NULL);

		// Find out which spread the node to be ungrouped is in
//		Spread* pSpread = FirstSelectedNode->FindParentSpread();

	    Node* CurrentNode = FirstSelectedNode;
	    Node* NextCurrent;
	    Node* GrpNode;
	    Node* NxtGrpNode;
	    Node* Anchor;

	    // Ungroup all selected groups   
		while (CurrentNode != NULL)
		{
			NextCurrent = Sel.FindNext(CurrentNode);

			if (CurrentNode->IsSelected() && IS_A(CurrentNode, NodeGroup))
			{
				// Karim 09/10/2000
				// Shadows BODGE - if our parent is a NodeShadowController, then we cannot ungroup.
				// Phil 2005, Shadows are different now but other controllers still disallow 
				// ungrouping!
				if (ParentAllowsUngroup((NodeGroup*)CurrentNode))
				{
					Ungrouped = TRUE;

					// Karim 09/10/2000
					// Invalidate region ONLY if in the middle of a background redraw UNLESS we were feathered.
					// Invalidate if we are changing effects so they the changes get redrawn
					BOOL RedrawNow = (CurrentNode->FindFirstChild(CC_RUNTIME_CLASS(AttrFeather))!=NULL ||
										bDeletePostPros || bLocalisePostPros ||
										bSelectGroupsHaveEffectAttrs
										);
					DoInvalidateNodeRegion( ((NodeRenderableBounded*)CurrentNode), TRUE, FALSE, !RedrawNow);

					// Localise any containing PostProcessors
					// (This will push down common attributes into the group)
/*					// Find topmost effect applied to the current node and localise attributes
					// down from there...
					Node* pTopNode = EffectsStack::EscapeOldControllers(CurrentNode);
					Node* pHighestLE = CurrentNode;
					do
					{
						if (pTopNode->IsPostProcessor())
							pHighestLE = pTopNode;

						pTopNode = pTopNode->FindParent();
					}
					while (pTopNode && pTopNode->IsPostProcessor());
*/
					if (bLocalisePostPros)
					{
PORTNOTETRACE("other", "Ungroup can't localise effects yet");
#if !defined(EXCLUDE_FROM_XARALX)
						BOOL bOK = TRUE;
						bOK = bOK && OpLiveEffect::DoLocaliseEffectAttrs(this, CurrentNode);
						bOK = bOK && OpLiveEffect::DoLocaliseLiveEffects(this, CurrentNode);
						if (!bOK) return FALSE;
#endif
					}
					else if (bDeletePostPros)
					{
PORTNOTETRACE("other", "Ungroup can't localise effects yet");
#if !defined(EXCLUDE_FROM_XARALX)
						if (CurrentNode->IsAnObject())
						{
							BOOL bOK = (OpLiveEffect::DoDeleteAllPostProcessors(this, (NodeRenderableInk*)CurrentNode, FALSE, TRUE)!=NULL);
							// Effect attrs will be deleted when the group and/or postpros are deleted
							// because of this we need to force the Op to redraw (see If BgRedraw above)
							if (!bOK) return FALSE;
						}
#endif
					}

					// Localise the attributes 
					if (!DoLocaliseCommonAttributes(((NodeGroup*)CurrentNode)))
						return FALSE;

					// Deselect the group node, cos we have got to remove its blobs
					((NodeGroup*)CurrentNode)->DeSelect(FALSE);

					ENSURE(!(CurrentNode->IsSelected()), "Deselect failed to deselect current node");

					// Hide the group node
					NodeHidden* Hidden; 
					if(!DoHideNode(CurrentNode, FALSE, &Hidden, FALSE))
						return FALSE;

					Anchor = Hidden;
					ENSURE(Anchor->IsNodeHidden(),
						   "The parent of a hidden group node is not a NodeHidden");

					// Move and select each node in turn. 
					GrpNode = CurrentNode->FindFirstChild();  		 // Group's first child node

					// Make sure that the group has at least one child
					#ifdef _DEBUG
					if (GrpNode == NULL)
					{
						TRACE( _T("Whilst ungrouping a group node without any children was found - It must be a Corel file"));
					}
					#endif

					//ENSURE(GrpNode != NULL, "Trying to ungroup a node which has no children");
					while (GrpNode != NULL)
					{

						// Find the next node to ungroup before we move the group node
						NxtGrpNode = GrpNode->FindNext(); 

						// If the node is not a NodeHidden then 	
						// Move the node to its new location in the tree. There is no need
						// to render the node.
						if (!GrpNode->IsAnAttribute() && !GrpNode->IsNodeHidden())
						{
							if (!DoMoveNode(GrpNode, Anchor, NEXT))
								return FALSE;

							if (GrpNode->IsAnObject())
								((NodeRenderableInk*)GrpNode)->Select(FALSE); 

							Anchor = GrpNode; // Attach the next node to ungroup to the last one
											  // so maintaining z ordering.
						}
						GrpNode = NxtGrpNode;
					}
				}
			}
			CurrentNode = NextCurrent; 
		}

		// Get the tool to remove all its blobs before we deselect the nodes.
		// Only do this if the current tool dosent update itself on sel changed messages
		if (pSelSpread!=NULL && pTool!=NULL && !pTool->AreToolBlobsRenderedOnSelection())
			pTool->RenderToolBlobs(pSelSpread,NULL);
	}
	else
		return (FALSE); // No nodes selected

	return (Ungrouped); // Ungrouped will be TRUE if we have ungrouped any groups
}


BOOL OpUngroup::FindSelectedEffectAttrs() const
{
	Range* pRange = GetApplication()->FindSelection();
	Node* pNode = pRange->FindFirst();
	while (pNode)
	{
		if (pNode->IsAGroup())
		{
			NodeAttribute* pAttr = NodeAttribute::FindFirstAppliedAttr(pNode);
			while (pAttr && pAttr->FindParent() == pNode)
			{
				if (pAttr->IsEffectAttribute() && !pAttr->HasEquivalentDefaultValue(TRUE))
					return TRUE;

				pAttr = NodeAttribute::FindPrevAppliedAttr(pAttr);
			}
		}

		pNode = pRange->FindNext(pNode);
	}

	return FALSE;
}



// ------------------------------------------------------------------------------------------
// OpUngroupSpecial methods
    
/********************************************************************************************

>	OpUngroupSpecial::OpUngroupSpecial() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpUngroupSpecial constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
            
OpUngroupSpecial::OpUngroupSpecial(): OpUngroup()								
{                              
}

 /********************************************************************************************

>	BOOL OpUngroupSpecial::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpUngroupSpecial initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpUngroupSpecial::Init()
{
	return (RegisterOpDescriptor(0,
								_R(IDS_UNGROUPSPECIALOP),
								CC_RUNTIME_CLASS(OpUngroupSpecial),
								OPTOKEN_UNGROUPSPECIAL,
								OpUngroupSpecial::GetState,
								0,	/* help ID */
								_R(IDBBL_UNGROUPSPECIALOP),
								0,
								0,
								SYSTEMBAR_ILLEGAL,			// For now !
								TRUE, 						// Receive messages
								FALSE,
								FALSE,
								0,
								(GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION)

 								)); 

}               

/********************************************************************************************

>	OpState	OpUngroupSpecial::GetState(String_256*, OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpUngroupSpecial
	Purpose:	For finding the OpUngroupSpecial's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpUngroupSpecial::GetState(String_256* s, OpDescriptor* o)
{
	return(OpUngroup::GetState(s,o));  
}

/********************************************************************************************

>	void OpUngroupSpecial::Do(OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the Ungroup Special operation
	Errors:		An ENSURE failure will occur if this function is called and there are no 
				selected groups. 
	SeeAlso:	-

********************************************************************************************/
		
void OpUngroupSpecial::Do(OpDescriptor*)		   
{    
	// Call UngroupSelectedGroups until there are no more groups to ungroup or until 
	// the operation fails.
	if (DoStartSelOp(TRUE,TRUE))  // Try to record the selection state 
	{
		while (OpUngroup::UngroupSelectedGroups()); 
	}
	End(); 
}                                                                               

