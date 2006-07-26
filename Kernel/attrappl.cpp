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
// AttrAppl.cpp - Implementation of the OpApplyAttribToSelected class

#include "camtypes.h"

#include "attrappl.h"
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ink.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "lineattr.h"
#include "linwthop.h"
//#include "mario.h"
#include "nodedoc.h"
//#include "simon.h"
//#include "group.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "toollist.h"
//#include "will.h"		// for _R(IDS_APPLYTONODEOP)
#include "ndoptmz.h"	// for NodeAttributePtrItem
#include "objchge.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "progress.h"
#include "fillndge.h"
//#include "txtattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodetxtl.h"
//#include "jason.h"
#include "colormgr.h"
//#include "bubbleid.h"
#include "nodecont.h"
#include "lineattr.h"
#include "opdrbrsh.h"
#include "brshattr.h"
#include "effects_stack.h"
#include "ophist.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodetext.h"
#include "ppbrush.h"

CC_IMPLEMENT_DYNCREATE(OpApplyAttrib, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpApplyAttribToSelected, OpApplyAttrib)
CC_IMPLEMENT_DYNCREATE(OpApplyAttrInteractive, OpApplyAttrib)
CC_IMPLEMENT_DYNCREATE(OpApplyAttribsToSelected, OpApplyAttrib)
CC_IMPLEMENT_DYNCREATE(OpRepeatApplyAttribToSelected, Operation)
CC_IMPLEMENT_DYNCREATE(OpApplyAttribToNode, OpApplyAttrib)
CC_IMPLEMENT_DYNCREATE(OpReplaceAttributes, SelOperation)
CC_IMPLEMENT_DYNCREATE(ReplaceAttributesParam, OpParam)
CC_IMPLEMENT_MEMDUMP(ApplyAttribsToSelectedParam, OpParam)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

// Static Fill Blob selection state, used to determine whether or not to
// merge apply ops
FillBlobSelectionState OpRepeatApplyAttribToSelected::FillBlobState;


/********************************************************************************************

>	NodeRenderableInk * OpApplyAttrib::FindCompoundNodeWhichRequiresAttribute(
																NodeRenderableInk * pStartNode)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/4/2000
	Inputs:		pStartNode : the node to start from
	Outputs:	The compound node which requires the attribute

	Returns:	pStartNode when not finding any appropriate nodes
	Purpose:	When applying an attribute it's necessary to test its parents to see if
				the attribute application should be promoted
				Uses the PromoteAttributeApplicationToMe virtual function in Node to
				discover whether to promote or not
	

********************************************************************************************/
NodeRenderableInk * OpApplyAttrib::FindCompoundNodeWhichRequiresAttribute(
													NodeRenderableInk * pStartNode,
													CCRuntimeClass * pClass)
{
	NodeRenderableInk * pNodeToApply = pStartNode;

	if (!pStartNode->NeedsParent(NULL))
	{
		Node * pParent = pNodeToApply->FindParent();

		while (pParent)
		{
			if (pParent->IsCompoundClass())
			{
				if (((NodeCompound*)pParent)->PromoteAttributeApplicationToMe(pClass))
				{
					pNodeToApply = (NodeRenderableInk *)pParent;
				}
			}

			// we need to go all the way up the tree to find the top-most node
			pParent = pParent->FindParent();
		}
	}

	return pNodeToApply;
}

/********************************************************************************************

>	BOOL SelOperation::DoApply(Node* CurrentNode,
								NodeAttribute* Attrib,
								BOOL Mutate, 
								BOOL InvalidateRegion = TRUE
								BOOL KeepExistingCharacteristics  = TRUE,
								BOOL* AttribWasRequired = NULL,
								BOOL* pbCanDiscardUndo = NULL)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	5/4/95
	Inputs:		InvalidateRegion: When TRUE DoInvalidateNodeRegion is called before and
								  after the attribute has been applied. If the attribute
								  is being applied to a range of objects then it is 
								  inefficient to do this.
				KeepExistingCols: When TRUE the new attribute takes on the colour
								  characteristics of the attribute it is replacing.
	Outputs:	AttribWasRequired:This output is optional. It indicates if  the
								  Attrib was required by the object.
	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 
	Purpose:	
				Will not apply the attribute to CurrentNode if it does not require it
	Errors:		-
	SeeAlso:	NodeRenderableInk::GetObjectToApplyTo

********************************************************************************************/

BOOL SelOperation::DoApply(Node* CurrentNode, 
							NodeAttribute* Attrib, 
							BOOL Mutate, 
							BOOL InvalidateRegion,
							BOOL KeepExistingCols, /* = TRUE */
							BOOL* pbAttribWasRequired,    /* = NULL */
							BOOL* pbCanDiscardUndo /* = NULL */
							)
{
	return SelOperation::DoApply(this, CurrentNode, Attrib, Mutate, InvalidateRegion, KeepExistingCols, TRUE, pbAttribWasRequired, pbCanDiscardUndo);
}

BOOL SelOperation::DoApply( SelOperation* pOp,
						    Node* CurrentNode, 
							NodeAttribute* Attrib, 
							BOOL Mutate, 
							BOOL InvalidateRegion,
							BOOL KeepExistingCols, /* = TRUE */
							BOOL bOptimise,
							BOOL* pbAttribWasRequired,    /* = NULL */
							BOOL* pbCanDiscardUndo /* = NULL */
							)
{
	BOOL AttributeExists = FALSE;  // Until we find that the attribute does exist

	CCRuntimeClass* AttrType = Attrib->GetAttributeType();
	
	ERROR3IF(!CurrentNode->IsAnObject(), "Trying to apply an attribute to a non-NodeRenderableInk");
	
	// We don't allow attributes like 'Quality' to be applied to objects directly.
	ERROR3IF(!(Attrib->CanBeAppliedToObject()), "Trying to apply an illegal attribute to the object"); 
	
	if (!(Attrib->CanBeAppliedToObject()))
		return TRUE;  // Just pretend they ever tried.

	Node* OriginalCurrentNode = CurrentNode;

	if (CurrentNode->IsAnObject())
	{

		// Obtain the real object to apply the attribute to 
		CurrentNode = ((NodeRenderableInk*)CurrentNode)->GetObjectToApplyTo(Attrib->GetAttributeType()); 
		if (!CurrentNode)
		{
			ERROR3("OpApplyAttrib::DoApply, Unable to find object to apply to");
			return TRUE;
		}
	}

	// NOTE! Better to get the value of this flag here, now that GetObjectToApplyTo has been called
	BOOL bEffectRootOnly = (CurrentNode->IsAnObject() && ((NodeRenderableInk*)CurrentNode)->IsValidEffectAttr(Attrib));

	// If we are applying attributes to the Caret only the caller should not retain an undo record
	// for this operation...
	*pbCanDiscardUndo = *pbCanDiscardUndo && CurrentNode->DiscardsAttributeChildren();

	// BODGE! ------------------------------------------------------------
	// Don't apply stroke transparency as an effect attribute - nothing
	// needs it. Test has to be done here because other objects in the
	// selection might need it...
	if (bEffectRootOnly && AttrType == CC_RUNTIME_CLASS(AttrStrokeTransp))
		return TRUE;
	// END-BODGE! --------------------------------------------------------

	// Needed to support adding triggers, who can be multiply applied
	BOOL AddEvenIfMultiple = FALSE;

	// Do not apply the attribute if CurrentNode does not require it 
	if ((((NodeRenderableInk*)CurrentNode)->RequiresAttrib(Attrib)))
	{

		if (pbAttribWasRequired)
			*pbAttribWasRequired = TRUE;

		if (CurrentNode != OriginalCurrentNode)
		{
			// BODGE TEXT
			// We need to call AllowOp, for the TextStories benefit, 
			// so that lines know they have been affected 

			ObjChangeFlags cFlags;
			cFlags.Attribute = TRUE;
			ObjChangeParam ObjChange(OBJCHANGE_STARTING, cFlags, NULL, pOp);

			if (!CurrentNode->AllowOp(&ObjChange))
			{
				// It's already passed the test once.
				ERROR3("Doing op when no nodes will allow it");
			}
		}
			
		// -----------------------------------------------------------------------------
		// Determine if the current node already has an attribute which is the same 
		// runtime class as Attrib. 
		Node* n = CurrentNode->FindFirstChild(); 
		Node* pLastBoundedNode = NULL;
		if (bEffectRootOnly)
		{
			pLastBoundedNode = CurrentNode->FindLastChild(CC_RUNTIME_CLASS(NodeRenderableBounded));
			ERROR3IF(pLastBoundedNode==NULL, "Attempt to apply effect attr to node with no children");

			if (pLastBoundedNode) n = pLastBoundedNode->FindNext();
		}

		while (n  && (n->IsOrHidesAnAttribute()))
		{
			// Now lets see if we can find an attribute of the same type
			// as the one we are interested in.
			if (n->IsAnAttribute())
			{
				NodeAttribute* pNdAttr = (NodeAttribute*)n;
				 
				if( pNdAttr->GetAttributeType() == AttrType)
				{
					// Attributes are of the same type (whatever that might mean)

					// Look for another attribute of the same Type if we're allowed more than
					// one per object
					BOOL LookForMultiple = FALSE;

					if (Attrib->CanBeMultiplyApplied())
					{
						// If the classID's are the same then we want to replace the old one
						// (unless they're identical)
						if (Attrib->GetAttributeClassID() == pNdAttr->GetAttributeClassID())
						{
							LookForMultiple = FALSE;	// stop looking now
							AddEvenIfMultiple = FALSE;	// replace the old one
						}
						else
						{
							// they're not the same classID's then we'll add a new one
							// but keep looking for a duplicate
							LookForMultiple = TRUE;
							AddEvenIfMultiple = TRUE;
						}
					}
	
					// Do the attributes have the same value ?. If they do then there is no point in 
					// applying it a second time

					// to have the same value they must share the same runtime class
					if (IS_SAME_CLASS(pNdAttr, Attrib))
					{
						// Found an identical attribute, so ignore this apply
						if ((*pNdAttr)==(*Attrib))
							return TRUE;     // We are not failing, just doing nothing.

						if (Attrib->IsABrush())
							((AttrBrushType*)Attrib)->OnReplaceAttribute((AttrBrushType*)pNdAttr);
					}
					
					// if applying a brush attribute we must copy some data over
					if (!LookForMultiple)
					{
						AttributeExists = TRUE; 
						break; 
					}
				}
			}
			n = n->FindNext(); // Find next child of CurrentNode
		}	

		// >>>> Temporary BODGE to aid select-inside...
		// >>>> ALWAYS clear any attributes of the same type from the subtree (even if the
		// >>>>	attribute just applied replaced an existing one) so that dubious
		// >>>> attribute states (due to this routine not dealing with Select-inside
		// >>>> properly yet) can be cleared.
		// >>>> NOTE! the current att (n) is passed in to DoRemoveAttTypeFromSubtree so
		// >>>> that it won't be deleted along with atts of the same type - that would be
		// >>>> (has been) disastrous!
		// Triggers can be multiply applied to groups, so we need to check CanBeMultiplyApplied
		if (CurrentNode->IsCompound() && !Attrib->CanBeMultiplyApplied() && !bEffectRootOnly)
		{
			// Remove all instances of attributes of the same type from the subtree.
			// This is not neccessary if the AttributeExists flag is TRUE because
			// we know in this situation that the subtree cannot contain any other
			// instances of the attribute !.
			if (pOp && !pOp->DoRemoveAttrTypeFromSubtree(CurrentNode, Attrib->GetAttributeType(), n))
			{
				pOp->FailAndExecute(); // Just to make sure
				return FALSE;
			}  			
		}

		// At this point we have Either .....
		//      1. Found an attribute of the same type as the new one, and we will
		//		   now replace it.
		//      2. Found the specific attribute we were looking for and will replace
		//         that.
		//      3. Found an attribute to mutate.
		//      4. Found no attribute of the correct type, indicating that it has
		//         been factored out, and we need to put a new one in here.
		//		5. The attribute had CanBeMultiplyApplied() set, and no exact same
		//		   Attribute was found.
		//
		// Have we have got an Attribute to do something with ?
		NodeAttribute* AttribClone = NULL;
		if ( !AddEvenIfMultiple && n && n->IsAnAttribute())
		{
			// Yes !
			NodeAttribute* pAttr = (NodeAttribute*)n;

			if (Mutate)
			{
				// Mutate it into the new type.
				AttribClone = ((AttrFillGeometry*)n)->Mutate((AttrFillGeometry*)Attrib, bOptimise);
			}
			else
			{
				// We're gunna just replace the attribute with the new one.

				// First make a copy of the new attribute.
				if (pOp)
		          	ALLOC_WITH_FAIL(AttribClone ,((NodeAttribute*)Attrib->SimpleCopy()), pOp)
				else
					AttribClone = (NodeAttribute*)Attrib->SimpleCopy();
				if (AttribClone == NULL)
					return FALSE; 

				// Complication !!
				// If we are replacing a Fill Attribute and the attribute we are replacing
				// is already filled, then we need to extract the colour of the existing
				// fill and use them for the new fill.
				if ( KeepExistingCols && ((NodeAttribute*)n)->IsAFillAttr() )
				{
	            	AttrFillGeometry* NodeReplaced = (AttrFillGeometry*)n;

					// Copy the old fill characteristics into the new Fill
					if (!OpApplyAttrib::KeepExistingCharacteristics(NodeReplaced, (AttrFillGeometry*)AttribClone))
						return FALSE;
				}

				if ( Attrib->IsATranspFill() && Attrib->IsAFlatFill())
				{
					((AttrFillGeometry*)n)->RenderFillBlobs();
				}
			}

			// If Mutate returned NULL, which means the Mutate
			// did nothing, we should return TRUE, which will move
			// onto the next object.
			if (AttribClone == NULL)
				return TRUE; 

			// ----------------------------------------------------------
			// If we have just set an effect attribute then we can avoid releasing
			// cached info of the specified node and all its children...
			//
			// We RELY on invalidations associated with this function not
			// calling ReleaseCached themselves!
			//
			ReleaseCachedForAttrApply((NodeRenderableBounded*)CurrentNode, bEffectRootOnly);
			// ----------------------------------------------------------

			if (InvalidateRegion)
			{
				// Invalidate the Object before the attribute is applied
				if (!InvalidateNodeRegion(pOp, (NodeRenderableBounded*)CurrentNode, Attrib, Mutate))	// Doesn't invalidate cached info!
					return FALSE;
			}

			// Now we have done with the old attribute, so lets hide it, so
			// the changes can be undone
			//
			// Don't write any undo info if we don't have an op or the node
			// discards attributes by itself
			if (pOp && !CurrentNode->DiscardsAttributeChildren())
			{
				// Note that we wouldn't need this test and we wouldn't need
				// to use the HideNode/ShowNode actions here if solid fill
				// dragging used OpApplyAttrInteractive. Instead it uses an
				// older system which relies on the HideNodeAction recording
				// a pointer to an attribute at the start of the drag which
				// is subsequently updated during the drag.
				if (pOp->IsKindOf(CC_RUNTIME_CLASS(OpApplyAttrInteractive)))
				{
					ApplyAction* pUndoApply; 
		   			// Create an action to re-apply the attribute when we undo 
					if ( ApplyAction::Init(pOp, 
				 							pOp->GetUndoActionList(),
											CurrentNode,
											pAttr,
											TRUE,				// When the attribute gets hidden we
									  							// must include its size 
				 							(Action**)(&pUndoApply))
						 == AC_FAIL)
					{
						return FALSE;
					}
					pAttr->CascadeDelete();
					delete pAttr;
					pAttr = NULL;
				}
				else
				{
					if (!pOp->DoHideNode(n, TRUE))			// Include the subtree size 
						return FALSE;
				}
			}
			else
			{
				pAttr->CascadeDelete();
				delete pAttr;
				pAttr = NULL;
			}

		}
		else  // if (n != NULL)
		{
			// We've not found an attribute to replace, so we'll have to put
			// a new one in
			NodeAttribute* TempAttr = NULL;

			BOOL FoundAttr = ((NodeRenderableInk*)CurrentNode)->
								FindAppliedAttribute(Attrib->GetAttributeType(), &TempAttr);

			if (!FoundAttr || TempAttr == NULL)
				return FALSE;

			if (Mutate)
			{
				AttribClone = ((AttrFillGeometry*)TempAttr)->Mutate((AttrFillGeometry*)Attrib, bOptimise);
			}
			else
			{
				// We'll just put a copy of our attribute in the tree.
				if (pOp)
		        	ALLOC_WITH_FAIL(AttribClone ,((NodeAttribute*)Attrib->SimpleCopy()), pOp)
				else
					AttribClone = (NodeAttribute*)Attrib->SimpleCopy();

				if ( KeepExistingCols && (TempAttr->IsAFillAttr()) )
				{
	            	AttrFillGeometry* NodeReplaced = (AttrFillGeometry*)TempAttr;

					// Copy the old fill characteristics into the new Fill
					if (!OpApplyAttrib::KeepExistingCharacteristics(NodeReplaced, (AttrFillGeometry*)AttribClone))
						return FALSE;
				}

				if (Attrib->IsATranspFill() && Attrib->IsAFlatFill())
				{
					// If we are mutating into a flat fill, then we need
					// to make sure we remove any existing fill blobs.
					((AttrFillGeometry*)TempAttr)->RenderFillBlobs();
				}
			}

			if (AttribClone)
			{
				ReleaseCachedForAttrApply((NodeRenderableBounded*)CurrentNode, bEffectRootOnly);

				if (InvalidateRegion)
				{
					// Make sure the node is redrawn
					if (!InvalidateNodeRegion(pOp, (NodeRenderableBounded*)CurrentNode, Attrib, Mutate))	// Doesn't invalidate cached info!
						return FALSE;
				}
			}
		}

		// If the AttribClone has the same value as an applied attribute 
		// (A default attr if the attribs have been localised). Then we don't want to apply it !
		if (AttribClone && AttribClone->ShouldBeOptimized() && !bEffectRootOnly && bOptimise)
		{
			NodeAttribute* pAppliedAttr;
			if (((NodeRenderableInk*)CurrentNode)->FindAppliedAttribute(AttribClone->GetAttributeType(), 
												  &pAppliedAttr))
			{
				// Do the attributes have the same value ?
				if ((IS_SAME_CLASS(AttribClone, pAppliedAttr)))
				{
					if ((*AttribClone)==(*pAppliedAttr))
					{
						AttribClone->CascadeDelete();		// Delete the attribute

						// If attribute affect's the bounds of the object (eg. a LineWidth) then invalidate the
						// bounds of the object

						if (pAppliedAttr->EffectsParentBounds())
						{
							((NodeRenderableBounded*)CurrentNode)->InvalidateBoundingRect(TRUE);
						}

						delete AttribClone;
						AttribClone = NULL;
					}
				}
			}
		}

		// Effect attributes don't optimise so if the value of this is the
		// same as the default value we should not add it here
		//
		if (AttribClone && bEffectRootOnly && AttribClone->HasEquivalentDefaultValue() && bOptimise)
		{
			// Just allow the attribute to be hidden
			AttribClone->CascadeDelete();		// Delete the attribute
			delete AttribClone;
			AttribClone = NULL;
		}

		if (AttribClone)
		{
			// Finally !! We can add the new attribute node into the tree.
			if (bEffectRootOnly)
				AttribClone->AttachNode(pLastBoundedNode, NEXT);
			else
				AttribClone->AttachNode(CurrentNode, FIRSTCHILD);

			AttributeManager::pLastNodeAppliedTo = (NodeRenderableInk*)CurrentNode;

			// And now it's in the tree, we need to make sure that any fill control
			// points are valid.
			if (AttribClone->IsAFillAttr())
			{
		    	((AttrFillGeometry*)AttribClone)->AttributeChanged();
			}

			if (pOp && !CurrentNode->DiscardsAttributeChildren())
			{
				// Note that we wouldn't need this test and we wouldn't need
				// to use the HideNode/ShowNode actions here if solid fill
				// dragging used OpApplyAttrInteractive. Instead it uses an
				// older system which relies on the HideNodeAction recording
				// a pointer to an attribute at the start of the drag and the
				// contents of the attribute being updated during the drag.
				if (pOp->IsKindOf(CC_RUNTIME_CLASS(OpApplyAttrInteractive)))
				{
					UnApplyAction* pUndoApply; 
		   			// Create an action to hide the attribute when we undo 
					if ( UnApplyAction::Init(pOp, 
				 							  pOp->GetUndoActionList(),
											  CurrentNode,
											  AttribClone,
											  TRUE,				// When the attribute gets hidden we
									  							// must include its size 
				 							 (Action**)(&pUndoApply))
						 == AC_FAIL)
					{   
						AttribClone->CascadeDelete();       
						delete (AttribClone); 
						return FALSE;  
					}
				}
				else
				{
					HideNodeAction* UndoHideNodeAction; 
		   			// Create an action to hide the attribute when we undo 
					if ( HideNodeAction::Init(pOp,
				 							  pOp->GetUndoActionList(),
											  AttribClone,
											  TRUE,				// When the attribute gets hidden we
									  							// must include its size 
				 							 (Action**)(&UndoHideNodeAction))
						 == AC_FAIL)  
					{   
						AttribClone->CascadeDelete();       
						delete (AttribClone); 
						return FALSE;  
					}
				}
			}

		}

		ReleaseCachedForAttrApply((NodeRenderableBounded*)CurrentNode, bEffectRootOnly);

		if (InvalidateRegion)
		{
			// Invalidate the node rectangle, now the new attr has been applied
			if (!InvalidateNodeRegion(pOp, (NodeRenderableBounded*)CurrentNode, Attrib, Mutate))	// Doesn't invalidate cached info!
				return FALSE; 
		}
	}

	// we may have to apply this to a brushed node
	if (pOp)												// TODO: Make this work when no undo op specified?
	{
		if (!pOp->ApplyAttributeToBrush(CurrentNode, Attrib))
			return FALSE;
	}
	return TRUE; // success
}




/********************************************************************************************

>	static BOOL SelOperation::ReleaseCachedForAttrApply(NodeRenderableBounded* pNode, BOOL bEffectRootOnly) 

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/03/2005
	Inputs:		pCurrentNode	- the node that the attribute is being applied to 
				bEffectRootOnly	- TRUE if
	Returns:	TRUE if all ok.
	Purpose:	If we have just set an effect attribute then we can avoid releasing
				cached info of the specified node and all its children...
				We RELY on invalidations associated with this function not
				calling ReleaseCached themselves!
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL SelOperation::ReleaseCachedForAttrApply(NodeRenderableBounded* pNode, BOOL bEffectRootOnly)
{
	if (bEffectRootOnly)
		pNode->ReleaseCached(TRUE, FALSE, FALSE, TRUE);						// Parents and Derived data only
	else
		pNode->ReleaseCached();												// Self, Parents, Children and derived data

	return TRUE;
}




/********************************************************************************************

>		BOOL SelOperation::ApplyAttributeToBrush(Node* pCurrentNode, NodeAttribute* pAttr) 
						

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/2000
	Inputs:		pCurrentNode - the node that the attribute is being applied to 
				pAttr        - the attribute to apply
	Returns:	TRUE if all ok.
	Purpose:	Works out if pCurrentNode has an applied brush attribute, and if so then sets
				up an action to tell the brush to use the attribute being applied.
	Errors:		-
	SeeAlso:	-

	Notes:		This is only currently operational for fill attributes, others may be added
				in future.
				18/4/2000 - Line widths added
********************************************************************************************/

BOOL SelOperation::ApplyAttributeToBrush(Node* pCurrentNode, NodeAttribute* pAttr)
{
	ERROR2IF(pCurrentNode == NULL, FALSE, "Current node is NULL");
	ERROR2IF(pAttr == NULL, FALSE, "Attribute is NULL");
	
	// currently this only works for colour changes and transparent fills
	if (!pAttr->IsKindOf(CC_RUNTIME_CLASS(AttrStrokeColourChange)) && !pAttr->IsATranspFill()
		&& !pAttr->IsKindOf(CC_RUNTIME_CLASS(AttrLineWidth)) && !pAttr->IsABrush())
		return TRUE;
	
	// first get the existing brush attribute
	AttrBrushType* pAttrBrush = NULL;
	// for some reason we don't check the return value here??
	/*BOOL ok =*/ ((NodeRenderableInk*)pCurrentNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBrushType),
																		(NodeAttribute**)&pAttrBrush);

	// if we're applying a brush attribute then do something else
	if (pAttr->IsABrush())
	{
		AttrBrushType* pNewAttrBrush = (AttrBrushType*)pAttr;
		return ApplyBrushAttr(pCurrentNode, pNewAttrBrush);
	}


	// there isn't one - thats ok
	if (pAttrBrush == NULL || pAttrBrush->GetBrushHandle() == BrushHandle_NoBrush)
		return TRUE;

	// so now we have a brush attribute, so lets determine what to do:
	if (pAttr->IsKindOf(CC_RUNTIME_CLASS(AttrStrokeColourChange)))
	{
		// only do this is the new colour is not transparent
		AttrStrokeColourChange* pStrokeCol = static_cast<AttrStrokeColourChange*>(pAttr);
		DocColour* pCol = pStrokeCol->GetStartColour();
//PORTNOTE("other","Removed DocColour usage") - AMB Unremoved
		if (pCol->IsTransparent())
			return TRUE;

		return ApplyStrokeColourToBrush(pAttrBrush, (NodeRenderableInk*)pCurrentNode);
	}

// Is this a porting comment-out? AMB
//	if (pAttr->IsATranspFill())
//		return ApplyTransparencyToBrush(pAttrBrush, (NodeRenderableInk*)pCurrentNode);
	if (pAttr->IsKindOf(CC_RUNTIME_CLASS(AttrLineWidth)))
		return ApplyLineWidthToBrush(pAttrBrush, (NodeRenderableInk*)pCurrentNode, pAttr);

	// trash the cached bounding rect
	pAttrBrush->ClearCachedRect();

	// if it isn't one of the above then we dont have to do anything
	return TRUE;
}

/********************************************************************************************

>	BOOL SelOperation::ApplyTransparencyToBrush(AttrBrushType* pAttrBrush, NodeAttribute* pAttr) 
						

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/2000
	Inputs:		pAttrBrush   - the brush attribute that we wish to update
				pAttr        - the attribute to apply
	Returns:	TRUE if all ok.
	Purpose:	When a transparency is applied to a brushed object we need to set a flag
				in the brush to tell it to use this new transparency
********************************************************************************************/

BOOL SelOperation::ApplyTransparencyToBrush(AttrBrushType* pAttrBrush, NodeRenderableInk* pBrushedNode)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "Attribute pointer is NULL in OnApplyAttrib::ApplyTransparencyToBrush");
	ERROR2IF(pBrushedNode == NULL, FALSE,  "Attribute pointer is NULL in OnApplyAttrib::ApplyTransparencyToBrush");

	// make the OpParam
	ChangeBrushOpParam Param;

	Param.ChangeType = CHANGEBRUSH_USELOCALTRANSP;
	Param.m_bNewUseLocalTransp = TRUE;
	ChangeBrushAction* pAction = NULL;
	// initialise the action
	if (ChangeBrushAction::Init(this, &UndoActions, pBrushedNode, &Param, &pAction) == AC_FAIL)
		return FALSE;

	return TRUE;
}
/********************************************************************************************

>	BOOL SelOperation::ApplyStrokeColourToBrush(AttrBrushType* pAttrBrush,NodeRenderableInk* pBrushedNode) 
						

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/2000
	Inputs:		pAttrBrush   - the brush attribute that we wish to update
				pbrushed node	- the node that the brush applies to
	Returns:	TRUE if all ok.
	Purpose:	When a new stroke colour is applied to a brushed object we wish to set the
				flag that tells the brush to override the named colours in the brush with
				the colour applied

********************************************************************************************/

BOOL SelOperation::ApplyStrokeColourToBrush(AttrBrushType* pAttrBrush, NodeRenderableInk* pBrushedNode)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "Attribute pointer is NULL in OnApplyAttrib::ApplyStrokeColourToBrush");
	ERROR2IF(pBrushedNode == NULL, FALSE,  "Attribute pointer is NULL in OnApplyAttrib::ApplyStrokeColourToBrush");

	
	

	// if the brush already uses local colours then don't bother doing anything
	PathProcessorBrush *pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
		return TRUE;

	if (pPPB->GetUseLocalFillColour() || !pPPB->GetUseNamedColours())
		return TRUE;

	// make the OpParam
	ChangeBrushOpParam Param;

	Param.ChangeType = CHANGEBRUSH_USENAMEDCOL;
	Param.m_bNewUseNamed = FALSE;
	ChangeBrushAction* pAction = NULL;
	// initialise the action
	if (ChangeBrushAction::Init(this, &UndoActions, pBrushedNode, 
							&Param, &pAction) == AC_FAIL)
		return FALSE;

	return TRUE;
}



/********************************************************************************************

>	BOOL SelOperation::ApplyLineWidthToBrush(AttrBrushType* pAttrBrush, NodeRenderableInk* pBrushedNode,
								NodeAttribute* pAttr) 
						

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/2000
	Inputs:		pAttrBrush   - the brush attribute that we wish to update
				pBrushedNode   -the node that the brush applies to
				pAttr      - the line width that we are applying
	Returns:	TRUE if all ok.
	Purpose:	If we are applying a line width to a brushed node we need to tell the brush that
				it must scale to line width.
				We must also do an additional invalidate region so that if we are increasing in
				size then the brush will be rerendered properly
********************************************************************************************/

BOOL SelOperation::ApplyLineWidthToBrush(AttrBrushType* pAttrBrush, NodeRenderableInk* pBrushedNode,
										  NodeAttribute* pAttr)
{
	ERROR2IF(pAttrBrush == NULL, FALSE, "Attribute pointer is NULL in OnApplyAttrib::ApplyLineWidthToBrush");
	ERROR2IF(pBrushedNode == NULL, FALSE,  "Node pointer is NULL in OnApplyAttrib::ApplyLineWidthToBrush");
	ERROR2IF(pAttr == NULL, FALSE, "Attribute pointer is NULL in OnApplyAttrib::ApplyLineWidthToBrush");

	// if we are on a closed path then ask the PPB to even out the spacing
	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB != NULL)
	{
		// scale to the new line width
		AttrLineWidth* pLineWidth = (AttrLineWidth*)pAttr;
		BOOL UsesPressure = pAttrBrush->GetPressureCache() != NULL;
		pPPB->ScaleToValue(pLineWidth->Value.LineWidth, !UsesPressure);
		pPPB->AdjustSpacingForClosedPath(pBrushedNode);
	}

	return TRUE;
}



/********************************************************************************************

>BOOL SelOperation::ApplyBrushAttr(Node* pCurrentNode, AttrBrushType* pAttrBrush)
						

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/2000
	Inputs:		pCurrentNode - the node we are applying the brush to
				pAttrBrush   - the brush attribute we are applying
	Returns:	TRUE if all ok.
	Purpose:	When we apply a brush attribute to an object we want to check to see if the
				current line width attribute is the same as the default attribute.  If it is then
				we will apply the default line width suggested by the brush itself.
				This is because people keep on applying brush attributes to 500MP lines
				and ending up with thousands of brush objects, which makes everything really slow.
********************************************************************************************/

BOOL SelOperation::ApplyBrushAttr(Node* pCurrentNode, AttrBrushType* pAttrBrush)
{
	ERROR2IF(pCurrentNode == NULL || pAttrBrush == NULL, FALSE, "Null input pointers to OpApplyAttrib::ApplyBrushAttr");
	ERROR2IF(!pCurrentNode->IsAnObject(), FALSE, "Input node is not an ink node in OpApplyAttrib::ApplyBrushAttr");

	// its too complicated when you have compound nodes
	if (pCurrentNode->IsCompound())
		return TRUE;

	AttrLineWidth* pCurrentLineWidth = NULL;
	((NodeRenderableInk*)pCurrentNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrLineWidth), (NodeAttribute**)&pCurrentLineWidth);
	
	if (pCurrentLineWidth != NULL)
	{
		// Get the default line width
		AttrLineWidth* pDefault = (AttrLineWidth*)(AttributeManager::GetDefaultAttribute(ATTR_LINEWIDTH));
		if (pDefault == NULL)
		{
			ERROR3("Unable to get default line width, theres no way this can happen Jim!");
			// continue anyway
		}
		else
		{
			// check to see if default is the same as current, if not then just leave
			if (pCurrentLineWidth->Value.LineWidth != pDefault->Value.LineWidth-1)
			{
				delete pDefault;
				PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
				if (pPPB)
				{
					BOOL UsesPressure = pAttrBrush->GetPressureCache() != NULL;
					pPPB->ScaleToValue(pCurrentLineWidth->Value.LineWidth, !UsesPressure);
					pPPB->AdjustSpacingForClosedPath((NodeRenderableInk*)pCurrentNode);
				}

				return TRUE;
			}
			delete pDefault;
		}
	}


	// otherwise we want a new line width attribute
	MILLIPOINT LineWidth = pAttrBrush->GetDefaultLineWidth(TRUE);
	
	//DoApply makes a copy so we only need to make this local
	AttrLineWidth NewLineWidth;

	NewLineWidth.Value.LineWidth = LineWidth;

	// now apply it
	return DoApply(pCurrentNode, &NewLineWidth, FALSE, FALSE);
	
}

/********************************************************************************************

>	static BOOL OpApplyAttrib::KeepExistingCharacteristics(AttrFillGeometry* OldAttr, 
													AttrFillGeometry* NewAttr)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/95
	Inputs:		-
	Returns:	TRUE if all ok.
	Purpose:	Retains the previous fill charateristics when applying a new fill.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpApplyAttrib::KeepExistingCharacteristics(AttrFillGeometry* OldAttr, 
												AttrFillGeometry* NewAttr)
{
	NewAttr->SetStartTransp(OldAttr->GetStartTransp());
	NewAttr->SetEndTransp(OldAttr->GetEndTransp());

   	NewAttr->SetTranspType(OldAttr->GetTranspType());

	if (OldAttr->IsAFlatFill())
	{
		if (OldAttr->FindParent()->IsNodeDocument())
		{
			// If the applied attr is the default fill at the top of the tree,
			// then we need to 'bodge' it to be black.
			DocColour DefFillCol;
			AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
												_R(IDS_BLACKNAME), &DefFillCol);
			NewAttr->SetEndColour(&DefFillCol);
		}
		else
			NewAttr->SetEndColour(OldAttr->GetStartColour());
	}
	else
	{
    	NewAttr->SetStartColour(OldAttr->GetStartColour());
		NewAttr->SetEndColour(OldAttr->GetEndColour());
	}

	NewAttr->SetTesselation(OldAttr->GetTesselation());
	NewAttr->AttachBitmap(OldAttr->GetBitmap());

	if (OldAttr->IsAFractalFill())
	{
		NewAttr->SetSeed(OldAttr->GetSeed());
		NewAttr->SetGraininess(OldAttr->GetGraininess());
		NewAttr->SetGravity(OldAttr->GetGravity());
		NewAttr->SetSquash(OldAttr->GetSquash());
		NewAttr->SetTileable(OldAttr->GetTileable());
		NewAttr->SetFractalDPI(AttrFillGeometry::FractalDPI);
		NewAttr->RecalcFractal();
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL SelOperation::InvalidateNodeRegion(	SelOperation* pOp,
												NodeRenderableBounded* CurrentNode, 
												NodeAttribute* Attrib, 
												BOOL Mutate)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/95
	Inputs:		-
	Returns:	TRUE if region invalidated ok.
	Purpose:	Invalidates the Bounds of a Node.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL SelOperation::InvalidateNodeRegion(	SelOperation* pOp,
											NodeRenderableBounded* CurrentNode, 
											NodeAttribute* Attrib, 
											BOOL Mutate)
{
	// Include the blobs, if the attribute will change the parents bounds.
	BOOL IncludeBlobs = Attrib->EffectsParentBounds();

	if ( (Mutate && !Attrib->IsAValueChange()) ||
		 Attrib->GetRuntimeClass() == CC_RUNTIME_CLASS(AttrBitmapDpiChange) )
	{
		// Check for special cases of, either a mutate from one fill type to another,
		// or a dpi change.  Both of these can change the blob bounds.
		IncludeBlobs = TRUE;
	}

	// Invalidate the bounds of the node, including the blobs if necessary.
	if (pOp)
	{
		if (!pOp->DoInvalidateNodeRegion(CurrentNode, IncludeBlobs, FALSE, FALSE, FALSE))	// NOTE! We do not release cached info!
																						// Must be used in conjunction with ApplyToSelection
		{
			// Summit went wrong.
			return FALSE;
		}
	}
	else
	{
		BaseDocument* pBaseDoc = CurrentNode->FindOwnerDoc();
		if (pBaseDoc && pBaseDoc->IsKindOf(CC_RUNTIME_CLASS(Document)))
		{
			Document* pDoc = (Document*)pBaseDoc;
			Spread* pSpread = (Spread*)CurrentNode->FindParent(CC_RUNTIME_CLASS(Spread));
			DocRect TempRect; 
			TempRect = (IncludeBlobs ? 
							(CurrentNode->GetUnionBlobBoundingRect()):
							(CurrentNode->GetBoundingRect())
						 );
			TempRect = TempRect.Union(CurrentNode->GetEffectStackBounds());
			pDoc->ForceRedraw(pSpread, TempRect, TRUE, CurrentNode, FALSE);
		}
	}

	return TRUE;	// All ok
}

/********************************************************************************************

>	BOOL OpApplyAttrib::DoInvalidateRegions(Range* NodeRange, 
											NodeAttribute* Attrib, 
											BOOL Mutate, 
											NodeAttribute* OtherAttr,
											BOOL OtherMuate)
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/95

	Inputs:		NodeRange: The range of nodes to invalidate
				Attrib: The attribute to be applied
				Mutate: 

	Returns:	TRUE if region invalidated ok.
	Purpose:	Invalidates the regions of every node in the range
	Errors:		-
	SeeAlso:	OpApplyAttrib::InvalidateNodeRegion

********************************************************************************************/

BOOL OpApplyAttrib::DoInvalidateRegions(Range* NodeRange, 
										NodeAttribute* Attrib, 
										BOOL Mutate,
										NodeAttribute* OtherAttr,
										BOOL OtherMutate)
{
	// Include the blobs, if the attribute will change the parents bounds.
	BOOL IncludeBlobs = ((Attrib->EffectsParentBounds()) || (OtherAttr && (OtherAttr->EffectsParentBounds())));

	if (((Mutate && !Attrib->IsAValueChange()) ||
		 Attrib->GetRuntimeClass() == CC_RUNTIME_CLASS(AttrBitmapDpiChange)) ||
		 (OtherAttr && ((OtherMutate && !OtherAttr->IsAValueChange()) ||
		 (OtherAttr->GetRuntimeClass() == CC_RUNTIME_CLASS(AttrBitmapDpiChange))) )
		)
	{
		// Check for special cases of, either a mutate from one fill type to another,
		// or a dpi change.  Both of these can change the blob bounds.
		IncludeBlobs = TRUE;
	}

	// Invalidate the bounds of the node, including the blobs if necessary.
	if (!DoInvalidateNodesRegions(*NodeRange, IncludeBlobs, FALSE, FALSE, FALSE))	// NOTE! We do not release cached info!
																					// Must be used in conjunction with ApplyToSelection
	{
		// Summit went wrong.
		return FALSE;
	}

	return TRUE;	// All ok
}


/********************************************************************************************

>	OpApplyAttribToSelected::OpApplyAttribToSelected() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpApplyAttribToSelected constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
            
OpApplyAttribToSelected::OpApplyAttribToSelected(): OpApplyAttrib()								
{                              
	ValueChangeType = NULL;
	MergeRepeats = FALSE;
	m_pAttr = NULL;
}


/********************************************************************************************

>	void OpApplyAttribToSelected::DoWithParam(OpDescriptor* OpDesc, OpParam* pOpParam)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		OpDescriptor 
	Outputs:	pOpParam: Param1 contains a pointer to the NodeAttribute to apply to the 
						  currently selected nodes
	Returns:	-
	Purpose:	Performs the OpApplyAttribToSelected operation. This function applies the
				NodeAttribute to all selected objects.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
		
void OpApplyAttribToSelected::DoWithParam(OpDescriptor* OpDesc, OpParam* pOpParam)		   
{   
#ifndef STANDALONE

	AttrTypeSet AttrTypes; 

	Range SelRng(*(GetApplication()->FindSelection()));

 
	ERROR3IF(pOpParam == NULL, "The OpApplyAttribToSelected operation requires an attribute parameter"); 

	BeginSlowJob();
	AttributeManager::pLastNodeAppliedTo = NULL;

	// Obtain a pointer to the attribute which we will need to apply to all selected nodes 
	NodeAttribute* Attrib = (NodeAttribute*)(void *)pOpParam->Param1;
	m_pAttr = Attrib;

	// DMc - make sure that the change type is set
	if (Attrib)
	{
		ValueChangeType = Attrib->GetRuntimeClass();
	}

	NodeAttribute* pAppliedAttrib; 

	BOOL Mutate = FALSE;
	BOOL RedrawBlobs = FALSE;
	NodeAttribute* OldAttr = NULL;

	if (Attrib == NULL)
	{
		// If the first param is NULL then we are doing a mutation
		Attrib = (NodeAttribute*)(void *)pOpParam->Param2;
		m_pAttr = Attrib;

		// DMc - make sure that the change type is set
		if (Attrib)
		{
			ValueChangeType = Attrib->GetRuntimeClass();
		}
	
		Mutate = TRUE;

		ValueChangeType = Attrib->GetRuntimeClass();

		if( ValueChangeType == CC_RUNTIME_CLASS(AttrTranspChange)
			|| ValueChangeType == CC_RUNTIME_CLASS(FillGeometryNudger)
			)
		{
			MergeRepeats = TRUE;
		}
	}	
	else
   	{
		// Someone has specified a Particular Attribute that we should replace
		// and ignore all others.
   		OldAttr = (NodeAttribute*)(void *)pOpParam->Param2;
		RedrawBlobs = Attrib->EffectsParentBounds();
	}

	SelRange* Sel;
	ObjChangeFlags cFlags;
	cFlags.Attribute = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

	Sel = GetApplication()->FindSelection(); 

	if (!Sel->AllowOp(&ObjChange))
	{
		FailAndExecute();
		goto EndOperation;
	}

	// Get a description of the attribute being applied so that we can use it to create the 
	// undo string. 
	UndoAttribStrID = Attrib->GetAttrNameID();

	if (!DoStartSelOp(RedrawBlobs, RedrawBlobs, TRUE,TRUE))
	{
		goto EndOperation;  
	}

	NodeAttribute* OtherAttrib;

	// Some Attributes require a second attribute to be changed as well,
	// which has to be done within this op.

	BOOL IsMutate;
	OtherAttrib = AttributeManager::GetOtherAttrToApply(Attrib, &IsMutate);
			
	// Before we apply the attribute to the selection we must localise all attributes
	// with the same type that we are going to apply. If we don't do this then the
	// tree will be left in an invalid state. 

	AttrTypes.AddToSet((Attrib->GetAttributeType())); 
	if (OtherAttrib != NULL)
	{
		AttrTypes.AddToSet((OtherAttrib->GetAttributeType())); 
	}
	
	// Invalidate before
	if (!DoInvalidateRegions(&SelRng, 
							 Attrib, 
							 Mutate,
							 OtherAttrib,
							 IsMutate))  //Mutators have to include bounds
	{
		goto EndOperation;
	}

		
	if (!DoLocaliseForAttrChange(&SelRng, &AttrTypes))
	{
		goto EndOperation;
	}

	if (!ApplyToSelection(Attrib, Mutate))
	{
		goto EndOperation;
	}
	
	pAppliedAttrib = Attrib;	
	Attrib = OtherAttrib;

	// Karim 26/06/2000
	// Some attributes (only AttrLineWidth at the mo') need to *selectively*
	// apply the second attribute - hence it must *not* be applied here.
	if (Attrib != NULL && !pAppliedAttrib->OtherAttrIsAppliedSelectively())
	{
		if (!ApplyToSelection(Attrib, IsMutate))
		{
			delete Attrib;
			goto EndOperation;
		}
	}
	
	// Invalidate after
	if (!DoInvalidateRegions(&SelRng, 
							 pAppliedAttrib, 
							 Mutate,
							 OtherAttrib,
							 IsMutate))  //Mutators have to include bounds
	{
		delete Attrib;
		goto EndOperation; 
	}

	delete Attrib;
	
	// Having applied the attributes, we  must now try and factor out the newly 
	// applied attributes
	if (!DoFactorOutAfterAttrChange(&SelRng, &AttrTypes))
	{
		goto EndOperation;
	}

	AttrFillGeometry::LastRenderedMesh = NULL;

	if (Document::GetSelected())
		Document::GetSelected()->SetModified(TRUE);

	ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
	if (!UpdateChangedNodes(&ObjChange))
		FailAndExecute();

	AttributeManager::LastAppliedBounds = Sel->GetBoundingRect();

	EndOperation:			
	EndSlowJob();

	AttrTypes.DeleteAll();

#endif
	End(); // End of operation
} 

/********************************************************************************************

>	BOOL OpApplyAttribToSelected::ApplyToSelection(NodeAttribute* Attrib, BOOL Mutate)		   

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 


	Purpose:	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpApplyAttribToSelected::ApplyToSelection(NodeAttribute* Attrib, BOOL Mutate)		   
{
	return DoApplyToSelection(this, Attrib, Mutate, TRUE);				// Do undo and Do optimisations
}

BOOL OpApplyAttrib::DoApplyToSelection(SelOperation* pOp, NodeAttribute* Attrib, BOOL bMutate, BOOL bOptimise)
{
	SelRange* Sel = GetApplication()->FindSelection();
	ERROR3IF(Sel==NULL,"Can't find SelRange!");
	if (Sel==NULL) return FALSE;
	Node* FirstSelNode = Sel->FindFirst();
	BOOL bAttrWasRequired = FALSE;

	// Find out which spread the selection is on
	if (FirstSelNode != NULL) 	  // Can't do anything if nothing is selected
	{ 	
		ERROR2IF(!FirstSelNode->IsAnObject(), FALSE, "ApplyToSelection SelRange doesn't have an Ink node in it");
//		NodeRenderableInk* FirstSelectedNode = (NodeRenderableInk*)FirstSelNode;
//		Spread *pSpread = FirstSelectedNode->FindParentSpread();

		// Set up a range of all selected nodes 
		EffectsStack* pStack = Sel->GetEffectsStack(FALSE, FALSE);	// Get cached stack, don't escape derived objects (allow attrs to be applie to derived objects)
		ERROR3IF(pStack==NULL, "Failed to get PPStack in ApplyToSelection");
		INT32 stackpos = STACKPOS_TOP;
		Range* pLevel = pStack->GetLevelRange(&stackpos, FALSE);			// Don't escape old controllers, apply attr to base nodes
		Node* CurrentNode = pLevel->FindFirst();
//		Node* FirstNode = CurrentNode;

		BOOL bCanDiscardUndo = TRUE;

		// While there are still nodes to apply the attribute to
		while (CurrentNode != NULL)
		{
			// move to the next one but remember the current one
			Node* OldNode = CurrentNode;
			CurrentNode = pLevel->FindNext(CurrentNode);

			if (OldNode->IsAnObject())
			{
				// Scan up through the stack to find the appropriate level to apply this attribute
				NodeRenderableInk * pNodeToApply = OpApplyAttrib::FindCompoundNodeWhichRequiresAttribute((NodeRenderableInk *)OldNode,
																				Attrib->GetRuntimeClass());

				if (pNodeToApply != OldNode)
				{
					// apply the attribute to all ink-children of the given node.
					Node * pChildNode = pNodeToApply->FindFirstChild();
					
					while (pChildNode != NULL)
					{
						Node * pNextChildNode = pChildNode->FindNext();

						if (pChildNode->IsAnObject() && ((NodeRenderableInk*)pChildNode)->RequiresAttrib(Attrib))
						{
							if (!DoApply(pOp, pChildNode, Attrib, bMutate, FALSE, TRUE, bOptimise, &bAttrWasRequired, &bCanDiscardUndo))	// Will only apply Attrib if the node requires it.
								return FALSE; 
						}

						pChildNode = pNextChildNode;
					}
				}
				else
				{
					if (!DoApply(pOp, OldNode, Attrib, bMutate, FALSE, TRUE, bOptimise, &bAttrWasRequired, &bCanDiscardUndo))	 // Will not apply Attrib if the node does
					// not require it.
					{
						return FALSE; 
					}
				}


				// now, if the node to apply is still a parent of the other nodes, skip past them
				if (pNodeToApply != OldNode && pNodeToApply->IsCompoundClass())
				{
					if (((NodeCompound*)pNodeToApply)->PromoteAttributeApplicationToMe(Attrib->GetRuntimeClass()))
					{
						while (CurrentNode != NULL && CurrentNode->FindParent(pNodeToApply->GetRuntimeClass()) == pNodeToApply)
						{
							CurrentNode = pLevel->FindNext(CurrentNode);
						}
					}
				}
			}
			else
			{
				if (!DoApply(pOp, OldNode, Attrib, bMutate, FALSE, TRUE, bOptimise, &bAttrWasRequired, &bCanDiscardUndo))	 // Will not apply Attrib if the node does not require it.
				{
					return FALSE; 
				}
			}

			// a bodge to handle the case when a line width is applied to a transparent line.
			// what we want in this case is to change the line colour (as well as the line width)
			// so the line becomes visible. Black colour is set on those lines.

			if ((IS_A(Attrib, AttrLineWidth)) && OldNode->IsAnObject())
			{
				NodeRenderableInk *pRendNode = ((NodeRenderableInk*)OldNode)->GetObjectToApplyTo(Attrib->GetAttributeType());

				// find the line colour attribute applied to that node
				NodeAttribute *pAppliedAttr = NULL;
				pRendNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeColour), &pAppliedAttr);
				if (!pAppliedAttr) // should always find one 
					continue; 

				// get the colour attribute from it
				ColourFillAttribute *pColAttr = (ColourFillAttribute *)pAppliedAttr->GetAttributeValue();
				if (!pColAttr)
					continue;

				// get the actual colour
				DocColour *Col = pColAttr->GetStartColour();
				if (!Col)
					continue;
				
				// check if the line colour for that node is set to 'no colour'
				if (Col->IsTransparent())
				{
					// we have to replace the applied attribute with black colour

					// create a new stroke colour attribute, 
					AttrStrokeColour *pAttr = new AttrStrokeColour();
					if (pAttr == NULL)
						continue;

					// and set it with black colour
					DocColour ColourToApply(COLOUR_BLACK);
					((AttrStrokeColour *)pAttr)->SetStartColour(&ColourToApply);

					// apply the new line colour to the node
					if (!DoApply(pOp, OldNode, pAttr, bMutate, FALSE, FALSE, bOptimise, &bAttrWasRequired, &bCanDiscardUndo))
						continue; 

					// remove the attribute
					delete pAttr;
					pAttr = NULL;
				}
			}

			ContinueSlowJob();
		}

		// Nasty textism.  EOL nodes can now have attributes, but the last EOL cannot be selected by
		// the user, causing the text tool to show <multiple> when it shouldn't really.
		// So if the last node in the range is the node before the last EOL in a text story then we
		// apply the attribute to the EOL too.
		Node* pLastSelected = pLevel->FindLast();
		if (pLastSelected!=NULL && pLastSelected->IsABaseTextClass())
		{
			// Alternative approach to fix other EOLNode problems that have appeared. (Bugs 5371 & 5372)
			// If the AbstractTextChar following the last selected node is an EOLNode then apply to it too.
			if (pLastSelected->IsAVisibleTextNode())
			{
				AbstractTextChar* pNextATChar = (AbstractTextChar*)pLastSelected->FindNext(CC_RUNTIME_CLASS(AbstractTextChar));
				if (pNextATChar!=NULL && pNextATChar->IsAnEOLNode())
				{
					if (!DoApply(pOp, pNextATChar, Attrib, bMutate, FALSE, TRUE, bOptimise, &bAttrWasRequired, &bCanDiscardUndo))
						return FALSE; 
				}
			}

//			TextStory* pStory = ((BaseTextClass*)pLastSelected)->FindParentStory();
//			if (pStory!=NULL)
//			{
//				// Get a pointer to the last EOL in the story
//				VisibleTextNode* pLastEOL = pStory->FindLastVTN();
//				ERROR3IF(!pLastEOL->IsAnEOLNode(), "Last node in story was not EOL");
//
//				// is the EOL next to the last node in the selected?
//				if (pLastEOL!=NULL && pLastEOL==pLastSelected->FindNext(CC_RUNTIME_CLASS(AbstractTextChar)))
//				{
//					if (!DoApply(pLastEOL, Attrib, Mutate, FALSE))
//						return FALSE; 
//				}
//			}
		}

		// If all nodes report DiscardsAttributeChildren() = TRUE
		// Then we can abandon the entire Op once it's finished
		if (bCanDiscardUndo)
			pOp->SucceedAndDiscard();

	}
	return TRUE;
}


/********************************************************************************************

>	BOOL OpApplyAttribToSelected::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpApplyAttribToSelected initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpApplyAttribToSelected::Init()
{
	// Register the opdescriptors for the OpApplyAttribToSelected operation
	OpDescriptor* OpDesc = new OpDescriptor(
								 			0,
								 			_R(IDS_APPLYATTRIBOP), 				
											CC_RUNTIME_CLASS(OpApplyAttribToSelected),
								 			OPTOKEN_APPLYATTRIB,
								 			OpApplyAttribToSelected::GetState);	
	ERRORIF(!OpDesc, _R(IDE_NOMORE_MEMORY), FALSE);

	OpDesc = new OpDescriptor(
								 			0,
								 			_R(IDS_APPLYATTR_INTERACTIVE),
											CC_RUNTIME_CLASS(OpApplyAttrInteractive),
								 			OPTOKEN_APPLYATTRINTERACTIVE,
								 			OpApplyAttrInteractive::GetState);
	ERRORIF(!OpDesc, _R(IDE_NOMORE_MEMORY), FALSE);


		// Bodge this needs to use faby Macro technology so that the op is attached to
		// a system bar.
	OpDesc = new OpChangeLineWidthOpDesc(0,
										 _R(IDS_CHANGELINEWIDTH),
										 CC_RUNTIME_CLASS(OpApplyAttribToSelected),
										 OPTOKEN_CHANGELINEWIDTH,
										 OpChangeLineWidthOpDesc::GetState,
										 0,
										 _R(IDBBL_CHANGELINEWIDTH),
										 _R(IDCB_LINEWIDTH_COMBO_BOX),
										 _R(IDCB_LINEWIDTH_COMBO_BOX));
	ERRORIF(!OpDesc, _R(IDE_NOMORE_MEMORY), FALSE);

	if (!OpRepeatApplyAttribToSelected::Init())
		return FALSE;	
	
	return TRUE;
}               

/********************************************************************************************

>	OpState	OpApplyAttribToSelected::GetState(String_256*, OpDescriptor* OpDesc)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpApplyAttribToSelected operation
	Purpose:	For finding OpApplyAttribToSelected state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpApplyAttribToSelected::GetState(String_256*, OpDescriptor* OpDesc)
{
	OpState OpSt;

	SelRange* Sel = GetApplication()->FindSelection();
	ERROR2IF(Sel==NULL,OpSt,"Can't find SelRange!");

	// The Operation is greyed if there are no nodes selected 
	OpSt.Greyed = ( (Sel->FindFirst() == NULL) );  
												
	return(OpSt);   
}


/********************************************************************************************

>	virtual void OpApplyAttribToSelected::GetOpName(String_256* OpName) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/94
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
					
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpApplyAttribToSelected::GetOpName(String_256* OpName) 
{ 
	
	*OpName = String_256(UndoAttribStrID); 
	*OpName += String_256(_R(IDS_CHANGE)); 
}  








// -----------------------------------------------------------------------------------------
// OpApplyAttrInteractive methods

/********************************************************************************************

>	OpApplyAttrInteractive::OpApplyAttrInteractive() 

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpApplyAttrInteractive constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpApplyAttrInteractive::OpApplyAttrInteractive(): OpApplyAttrib()								
{                              
}




/********************************************************************************************

>	OpState	OpApplyAttrInteractive::GetState(String_256*, OpDescriptor* OpDesc)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/2005
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpApplyAttrInteractive operation
	Purpose:	For finding OpApplyAttrInteractive state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpApplyAttrInteractive::GetState(String_256*, OpDescriptor* OpDesc)
{
	OpState OpSt;

	SelRange* Sel = GetApplication()->FindSelection();
	ERROR2IF(Sel==NULL,OpSt,"Can't find SelRange!");

	// The Operation is greyed if there are no nodes selected 
	OpSt.Greyed = ( (Sel->FindFirst() == NULL) );  
												
	return OpSt;
}




/********************************************************************************************

>	virtual void OpApplyAttrInteractive::GetOpName(String_256* OpName) 

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/2005
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpApplyAttrInteractive::GetOpName(String_256* OpName) 
{ 
	*OpName = String_256(m_UndoAttribStrID); 
	*OpName += String_256(_R(IDS_CHANGE)); 
}  




/********************************************************************************************

>	virtual BOOL OpApplyAttrInteractive::DoDragAttr(AttrValueChange* pNewValue)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpApplyAttrInteractive::DoDragAttr(AttrValueChange* pNewValue)
{
	// ApplyAttribute without optimisations, without undo, get list of attributes affected and their initial values
	// Assume the value supplied is a "Mutator"
	AttrFillGeometry* pMutator = NULL;
	//BOOL MakeAttrCurrent = FALSE; // until we know better
	BOOL ApplyAttribute = FALSE;

	ERROR2IF(pNewValue==NULL, FALSE, "DoDragattr not given a decent attribute pointer");
	ERROR3IF(!pNewValue->IsAFillAttr(), "A Mutator must be a fill attribute");
    pMutator = (AttrFillGeometry*)pNewValue;

	SelRange* Sel = GetApplication()->FindSelection();
	ERROR3IF(Sel==NULL, "Can't find SelRange!");
	if (Sel==NULL) return FALSE;

	ApplyAttribute = AttributeManager::CanBeAppliedToSelection(pMutator, &m_AttrGroupList);

	if (ApplyAttribute)
	{
		// ------------------------------------------------------------------
		// Apply attributes without preamble and without optimisation or
		// localisation so that dragging starts up quickly.
		// Noe that the initial application is done undoably so that the intial
		// states of the attributes are recorded (see DragAttrFinished).
		//
		// Some Attributes require a second attribute to be changed as well,
		// which has to be done within this op.
		BOOL bMutateOther;
		NodeAttribute* pOtherAttrib = AttributeManager::GetOtherAttrToApply(pNewValue, &bMutateOther);

		// Apply attributes with undo - The undo info is used to record the initial states
		// of the attributes so that the drag can be abandoned correctly and finalised correctly
		DoApplyToSelection(this, pNewValue, TRUE, FALSE);		// Don't allow optimisations yet

		if (pOtherAttrib && !pNewValue->OtherAttrIsAppliedSelectively())
			DoApplyToSelection(this, pOtherAttrib, bMutateOther, FALSE);

		if (pOtherAttrib)
		{
			delete pOtherAttrib;
			pOtherAttrib = NULL;
		}

//		StartDrag(DRAGTYPE_NOSCROLL);
		Operation::SetQuickRender(TRUE, this);

		// Quick redraw
		Sel->ForceRedrawView(GetWorkingDocView(), TRUE, TRUE, TRUE);
		GetWorkingDocView()->FlushRedraw();
		GetApplication()->ServiceRendering();
	}

	return ApplyAttribute;
}




/********************************************************************************************

>	virtual BOOL OpApplyAttrInteractive::DragAttrChanged(AttrValueChange* pNewValue)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpApplyAttrInteractive::DragAttrChanged(AttrValueChange* pNewValue)
{
	// Change attrs in the list and update without optimisation, without undo
	SelRange* Sel = GetApplication()->FindSelection();
	ERROR3IF(Sel==NULL, "Can't find SelRange!");
	if (Sel==NULL) return FALSE;

	BOOL bMutateOther;
	NodeAttribute* pOtherAttrib = AttributeManager::GetOtherAttrToApply(pNewValue, &bMutateOther);

	DoApplyToSelection(NULL, pNewValue, TRUE, FALSE);		// Don't allow optimisations yet

	if (pOtherAttrib && !pNewValue->OtherAttrIsAppliedSelectively())
		DoApplyToSelection(NULL, pOtherAttrib, bMutateOther, FALSE);

	if (pOtherAttrib)
	{
		delete pOtherAttrib;
		pOtherAttrib = NULL;
	}

	// Quick redraw
	Sel->ForceRedrawView(GetWorkingDocView(), TRUE, TRUE, TRUE);
	GetWorkingDocView()->FlushRedraw();
	GetApplication()->ServiceRendering();

	return TRUE;
}




/********************************************************************************************

>	virtual BOOL OpApplyAttrInteractive::DragAttrFinished(AttrValueChange* pNewValue, BOOL bSuccess)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpApplyAttrInteractive::DragAttrFinished(AttrValueChange* pNewValue, BOOL bSuccess)
{
	SelRange* Sel = GetApplication()->FindSelection();
	ERROR3IF(Sel==NULL, "Can't find SelRange!");
	if (Sel==NULL)
	{
		FailAndExecute();
		End();
		return FALSE;
	}

//	bSuccess = EndDrag();
	Operation::SetQuickRender(FALSE, this);

	if (!bSuccess)
	{
		// Just restore back to how things were when the drag started
		BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::NONCOLOURATTCHANGED)); 
		FailAndExecute();
		End();
		return FALSE;
	}

	BeginSlowJob();

	// ---------------------------------------------------------------------
	// Before we record the real undo info
	// restore the original attributes by executing the undo info
	// recorded when the drag started.
	//
	// This is a bit low level but it leverages the existing undo system to
	// record the initial states of the attributes rather than writing
	// something new to do the same job.
	//
	FailAndExecute();
	KeepOnEnd();			// This ensures that this Op is not self-deleted inside End()!
	End();

	// -----------------------------------------------------------------
	m_AttrTypes.DeleteAll();
	AttributeManager::pLastNodeAppliedTo = NULL;
	ValueChangeType = pNewValue->GetRuntimeClass();
	BOOL RedrawBlobs = FALSE;
	NodeAttribute* pOtherAttrib = NULL;

	ObjChangeFlags cFlags;
	cFlags.Attribute = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING, cFlags, NULL, this);

	if (!Sel->AllowOp(&ObjChange))
	{
		FailAndExecute();
		goto EndOperation;
	}

	// Get a description of the attribute being applied so that we can use it to create the 
	// undo string. 
	m_UndoAttribStrID = pNewValue->GetAttrNameID();

	if (!DoStartSelOp(RedrawBlobs, RedrawBlobs, TRUE, TRUE))
	{
		FailAndExecute();
		goto EndOperation;
	}

	// Some Attributes require a second attribute to be changed as well,
	// which has to be done within this op.
	BOOL bMutateOther;
	pOtherAttrib = AttributeManager::GetOtherAttrToApply(pNewValue, &bMutateOther);
			
	// Before we apply the attribute to the selection we must localise all attributes
	// with the same type that we are going to apply. If we don't do this then the
	// tree will be left in an invalid state. 
	m_AttrTypes.AddToSet((pNewValue->GetAttributeType())); 
	if (pOtherAttrib)
		m_AttrTypes.AddToSet((pOtherAttrib->GetAttributeType())); 
	
	// Invalidate before
	if (!DoInvalidateRegions(Sel, 
							 pNewValue, 
							 TRUE,				// Always mutate new value attribute
							 pOtherAttrib,
							 bMutateOther))		// Mutators have to include bounds
	{
		FailAndExecute();
		goto EndOperation;
	}

	if (!DoLocaliseForAttrChange(Sel, &m_AttrTypes))
	{
		FailAndExecute();
		goto EndOperation;
	}

	// -----------------------------------------------------------------
	DoApplyToSelection(this, pNewValue, TRUE, TRUE);		// Now we can undo, Now we can optimise!

	if (pOtherAttrib && !pNewValue->OtherAttrIsAppliedSelectively())
		DoApplyToSelection(this, pOtherAttrib, bMutateOther, TRUE);

	if (pOtherAttrib)
	{
		delete pOtherAttrib;
		pOtherAttrib = NULL;
	}

	// -----------------------------------------------------------------
	// Invalidate after
	if (!DoInvalidateRegions(Sel,
							 pNewValue, 
							 TRUE,
							 pOtherAttrib,
							 bMutateOther))  //Mutators have to include bounds
	{
		FailAndExecute();
		goto EndOperation;
	}

	// Having applied the attributes, we  must now try and factor out the newly 
	// applied attributes
	if (!DoFactorOutAfterAttrChange(Sel, &m_AttrTypes))
	{
		FailAndExecute();
		goto EndOperation;
	}

	AttrFillGeometry::LastRenderedMesh = NULL;

	if (Document::GetSelected())
		Document::GetSelected()->SetModified(TRUE);

	cFlags.Attribute = TRUE;
	ObjChange.Define(OBJCHANGE_FINISHED, cFlags, NULL, this);
	if (!UpdateChangedNodes(&ObjChange))
		FailAndExecute();

	AttributeManager::LastAppliedBounds = Sel->GetBoundingRect();

	// -----------------------------------------------------------------
	AttributeManager::UpdateCurrentAppliedAttr(pNewValue, &m_AttrGroupList, TRUE, TRUE);
	AttributeManager::UpdateAfterAttrApplied(pNewValue);

// ------------------------------------------
EndOperation:			
	EndSlowJob();

	m_AttrTypes.DeleteAll();
	m_AttrGroupList.DeleteAll();

	End(); // End of operation

	Sel->BroadcastAnyPendingMessages();

	return TRUE;
}




/********************************************************************************************

>	virtual void OpApplyAttrInteractive::PerformMergeProcessing() 

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/10/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This virtual function gets called to allow for the merging of nudge operations
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpApplyAttrInteractive::PerformMergeProcessing()
{	 
	// Obtain a pointer to the operation history for the current document
	OperationHistory* pOpHist = &pOurDoc->GetOpHistory();

	// Ensure that we are the last operation added to the operation history
	// Note cannot be an ERROR2 cos this function cannot fail. 
	ERROR3IF(pOpHist->FindLastOp() != this, "Last Op should be this op"); 
	if (pOpHist->FindLastOp() != this)
		return;
	
	// OK lets see if the operation performed before this was an OpNudge operation
	Operation* pPrevOp = pOpHist->FindPrevToLastOp();
	if (pPrevOp == NULL)   // Check if there was a previous op
		return;
	
	if (!IS_A(pPrevOp, OpApplyAttrInteractive))
		return;

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

	if (! ((*ThisOpsSelection) == (*LastOpsSelection)))
		return;

	ActionList* pPrevList = pPrevOp->GetUndoActionList();
	ActionList* pThisList = this->GetUndoActionList();
	if (pPrevList->GetCount()!=pThisList->GetCount())
		return;

	// Check that all actions are the same before we go any further...
	Action* pPrevAct = (Action*)pPrevList->GetHead();
	Action* pThisAct = (Action*)pThisList->GetHead();
	while (pPrevAct && pThisAct)
	{
		if (pPrevAct->GetRuntimeClass() != pThisAct->GetRuntimeClass())
			return;

		if (pPrevAct->IsKindOf(CC_RUNTIME_CLASS(UnApplyAction)))
		{
			UnApplyAction* pPrevApplyAct = (UnApplyAction*)pPrevAct;
			UnApplyAction* pThisApplyAct = (UnApplyAction*)pThisAct;
			if (pPrevApplyAct->m_pApplyNode != pThisApplyAct->m_pApplyNode)
				return;
		}

		pPrevAct = (Action*)pPrevList->GetNext(pPrevAct);
		pThisAct = (Action*)pThisList->GetNext(pThisAct);
	}

	// Now we know all actions are the same, we can go ahead and transfer
	// The attribute information from this action list to the previous
	// action list...
	pPrevAct = (Action*)pPrevList->GetHead();
	pThisAct = (Action*)pThisList->GetHead();
	while (pPrevAct && pThisAct)
	{
		ERROR3IF(pPrevAct->GetRuntimeClass() != pThisAct->GetRuntimeClass(), "This can't happen!");
		if (pPrevAct->GetRuntimeClass() != pThisAct->GetRuntimeClass())
			return;

		if (pPrevAct->IsKindOf(CC_RUNTIME_CLASS(UnApplyAction)))
		{
			UnApplyAction* pPrevApplyAct = (UnApplyAction*)pPrevAct;
			UnApplyAction* pThisApplyAct = (UnApplyAction*)pThisAct;
			ERROR3IF(pPrevApplyAct->m_pApplyNode != pThisApplyAct->m_pApplyNode, "This can't happen!");
			if (pPrevApplyAct->m_pApplyNode != pThisApplyAct->m_pApplyNode)
				return;

			// Update the previous UnApply action to hold the latest
			// attribute value from this Operation
			if (pPrevApplyAct->m_pAttribute)
				delete pPrevApplyAct->m_pAttribute;

			pPrevApplyAct->m_pAttribute = pThisApplyAct->m_pAttribute;

			pThisApplyAct->m_pAttribute = NULL;
		}

		pPrevAct = (Action*)pPrevList->GetNext(pPrevAct);
		pThisAct = (Action*)pThisList->GetNext(pThisAct);
	}

	// This op is no longer required, so let's vape it
	pOpHist->DeleteLastOp();
}




/********************************************************************************************

>	void OpApplyAttrInteractive::DragFinished(DocCoord PointerPos,
											  ClickModifiers ClickMods,
											  Spread* pSpread,
											  BOOL Worked,
											  BOOL bSolidDrag)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/08/2005
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickMods - the keyboard modifiers in place
				pSpread - the spread that was under the mouse
				Worked - TRUE if the drag was completed successfully
	Purpose:	Marks the end of the drag. It is at this point that all the transformations
				should be applied to the selection if everything worked.
				OpApplyAttrInteractive implements this just to pick up Escape handling
				through the Success flag

********************************************************************************************/

void OpApplyAttrInteractive::DragFinished(DocCoord PointerPos,
										  ClickModifiers ClickMods,
										  Spread* pSpread,
										  BOOL Success,
										  BOOL bSolidDrag)
{
	DragAttrFinished(NULL, Success);
}




// ---




// -----------------------------------------------------------------------------------------
// OpApplyAttribsToSelected methods

/********************************************************************************************

>	OpApplyAttribsToSelected::OpApplyAttribsToSelected() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/93
	Purpose:	OpApplyAttribsToSelected constructor

********************************************************************************************/
            
            
OpApplyAttribsToSelected::OpApplyAttribsToSelected(): OpApplyAttrib()								
{
	ValueChangeType = NULL;	 // ??
	MergeRepeats = FALSE;	 // ??
	m_pAttribList = NULL;
   
}

/********************************************************************************************

>	void OpApplyAttribsToSelected::DoWithParam(OpDescriptor* OpDesc, 
											   OpParam* pOpParam)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		OpDescriptor: A pointer to the OpDescriptor invoking the operation
				OpParam:	  This must be a ApplyAttribsToSelectedParam type

				OpParam->AttribsToApply: This is the list of attributes that is to be applied
										 to the selection. Each item on this list must be
										 a NodeAttributePtrItem.

				OpParam->undoAttribStrID: Specifies the undo string to display (eg. Paste Atrributes)
	
	Outputs:	OpParam->AttrGroupList:	 This list must be empty on entry to this function
										 If (Success) then this list will contain one ListListItem
										 per attribute on the AttribsToApplyList. Each of these
										 items will point to a Set of  AttributeGroupItems.

										 Attribute -> { Set of current attribute groups }
										 
										 Whenever an attribute is applied to an object, the Current
										 AttributeGroup associated with the object is added to
										 the attributes set of current attribute groups. 
										 Therefore if an attribute's set is empty then we know
										 that the attribute has not been applied.

											
				OpParam->Success: 		 FALSE if the operation	failed. i.e. FailAndExecute()
										 was called.

				anyAttrsApplied:		 FALSE if the operation has not applied any attributes
	
	Purpose:	Performs the OpApplyAttribsToSelected operation. This tries to apply the list
				of attributes in the OpParam->AttribsToApplyList to the selection. If no attribute
				can be applied then the operation does not generate any actions, and so will
				be a NOP and be destroyed during End().
				
				This Operation currently does not support mutation 

********************************************************************************************/
		
void OpApplyAttribsToSelected::DoWithParam(OpDescriptor* OpDesc, 
											OpParam* pOpParam 
										  )		   
{   
#ifndef STANDALONE
	BeginSlowJob();

	UndoAttribStrID = ((ApplyAttribsToSelectedParam*)pOpParam)->UndoAttribStrID;	// TODO: Fix this?
	
	/*BOOL bOK = */DoApplyAttribsToSelection(pOpParam, TRUE);

	EndSlowJob();

#else
	End();
#endif	// STAND_ALONE

} 


/********************************************************************************************

>	BOOL SelOperation::DoApplyAttribsToSelection(OpParam* pOpParam, BOOL bClearIfNoneApplied = FALSE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com> (from Simon)
	Created:	16/02/2005 (from 16/8/93)
	Inputs:		OpParam:	  This must be a ApplyAttribsToSelectedParam type

				OpParam->AttribsToApply: This is the list of attributes that is to be applied
										 to the selection. Each item on this list must be
										 a NodeAttributePtrItem.

				OpParam->undoAttribStrID: Specifies the undo string to display (eg. Paste Atrributes)
	
	Outputs:	OpParam->AttrGroupList:	 This list must be empty on entry to this function
										 If (Success) then this list will contain one ListListItem
										 per attribute on the AttribsToApplyList. Each of these
										 items will point to a Set of  AttributeGroupItems.

										 Attribute -> { Set of current attribute groups }
										 
										 Whenever an attribute is applied to an object, the Current
										 AttributeGroup associated with the object is added to
										 the attributes set of current attribute groups. 
										 Therefore if an attribute's set is empty then we know
										 that the attribute has not been applied.

											
				OpParam->Success: 		 FALSE if the operation	failed. i.e. FailAndExecute()
										 was called.

				anyAttrsApplied:		 FALSE if the operation has not applied any attributes
	
	Purpose:	Performs the OpApplyAttribsToSelected operation. This tries to apply the list
				of attributes in the OpParam->AttribsToApplyList to the selection. If no attribute
				can be applied then the operation does not generate any actions, and so will
				be a NOP and be destroyed during End().
				
				This Operation currently does not support mutation 

********************************************************************************************/

BOOL SelOperation::DoApplyAttribsToSelection(OpParam* pOpParam, BOOL bClearIfNoneApplied)
{   
	// Ensure that the OpParam is valid
	ERROR3IF(!pOpParam, "No parameters passed to OpApplyAttribsToSelected");
	ApplyAttribsToSelectedParam* pParam = (ApplyAttribsToSelectedParam*)pOpParam;
	// get inputs		 
	List* pAttrsToApply = pParam->AttribsToApply;
//	m_pAttribList = pAttrsToApply;

//	UndoAttribStrID = pParam->UndoAttribStrID;	// TODO: Fix this?

	// and outputs
	List* AttrGroupList = pParam->AttrGroupList;
	ERROR3IF(!(AttrGroupList->IsEmpty()), 
		"AttributeGroupList not empty on entry to OpApplyAttribsToSelected"); 
	BOOL* Success = pParam->Success;
	*Success = FALSE; 					// Let's be pessimistic
	BOOL* AppliedAttrs = pParam->AnyAttrsApplied;
	*AppliedAttrs = FALSE;
	BOOL bAnyApplied = FALSE;
	BOOL bWeStartedThisOp = FALSE;
 
	// Create a stable range of objects to apply to
	Range SelRng(*(GetApplication()->FindSelection()));
	
	// We need two sets of attribute types
	// one for Line level attributes, and one for non Line level attributes. This is because the localise
	// and factor out routines require each attribute in the set to be all applicable to the same 
	// object. This would not be true if our set contained a BOLD, and a LINE_SPACING attribute for example.
		
	// Make our attribute type sets
	AttrTypeSet LineLevelAttrTypes;
	AttrTypeSet OtherAttrTypes;
	NodeAttributePtrItem* pAttrItem = (NodeAttributePtrItem*)(pAttrsToApply->GetHead());
	NodeAttribute* pAttr;
	AttrTypeSet* SetToAddTo;
	while (pAttrItem)
	{
		pAttr = pAttrItem->NodeAttribPtr;
		ERROR3IF(!pAttr, "Should be an attribute");
		if (pAttr->IsALineLevelAttrib())
			SetToAddTo = &LineLevelAttrTypes;
		else
			SetToAddTo = &OtherAttrTypes;

		if (!SetToAddTo->AddToSet(pAttr->GetAttributeType()))
		{
			FailAndExecute();
			goto EndOperation;
		}
		pAttrItem = (NodeAttributePtrItem*)(pAttrsToApply->GetNext(pAttrItem));
	}

	// -----------------------------------------------------------
	if (SelRng.FindFirst()!=NULL)
	{
		SelRange* Sel;
		ObjChangeFlags cFlags;
		cFlags.Attribute = TRUE;
		ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

		Sel = GetApplication()->FindSelection(); 

		if (!Sel->AllowOp(&ObjChange))
		{
			FailAndExecute();
			goto EndOperation;
		}
	}

	if (!GetStarted())
	{
		if (!DoStartSelOp(FALSE,FALSE, TRUE,TRUE))   // Try to record the selection state , don't
									 				 // render the blobs though 
			goto EndOperation;

		bWeStartedThisOp = TRUE;
	}

	// Make a stable range
//	Range SelRng(*(GetApplication()->FindSelection()));

	// Invalidate objects regions (include blobs because this may be neccessary, 
	// 							   there is little overhead anyway).
	if (!DoInvalidateNodesRegions(SelRng, TRUE, FALSE))
		goto EndOperation;

	// Before we apply the attribute to the selection we must localise all attributes
	// with the same type that we are going to apply. If we don't do this then the
	// tree will be left in an invalid state. 
	
	if (!LineLevelAttrTypes.IsEmpty() && SelRng.FindFirst()!=NULL)
	{
		if (!DoLocaliseForAttrChange(&SelRng, &LineLevelAttrTypes))
			goto EndOperation;
	}

	if (!OtherAttrTypes.IsEmpty() && SelRng.FindFirst()!=NULL)
	{
		if (!DoLocaliseForAttrChange(&SelRng, &OtherAttrTypes))
			goto EndOperation;
	}

	// -----------------------------------------------------------
	if (!DoApplyToSelection(pAttrsToApply, AttrGroupList, &bAnyApplied))
	{
		goto EndOperation;
	}

	// -----------------------------------------------------------
	// If attributes were applied we must factor them out
	if (bAnyApplied)
	{
		// Having applied the attributes, we  must now try and factor out the newly 
		// applied attributes
		if (!LineLevelAttrTypes.IsEmpty())
		{
			if (!DoFactorOutAfterAttrChange(&SelRng, 
										&LineLevelAttrTypes))
			{
				goto EndOperation;
			}
		}

		if (!OtherAttrTypes.IsEmpty())
		{
			if (!DoFactorOutAfterAttrChange(&SelRng, 
										&OtherAttrTypes))
			{
				goto EndOperation;
			}
		}

		{	
			ObjChangeFlags cFlags;
			cFlags.Attribute = TRUE;

			ObjChangeParam ObjChange(OBJCHANGE_FINISHED,cFlags,NULL,this);
			if (!UpdateChangedNodes(&ObjChange))
			{
				FailAndExecute();
				goto EndOperation;
			}
		}

		// Invalidate after
		if (!DoInvalidateNodesRegions(SelRng, TRUE, FALSE))
		{
			goto EndOperation;
		}
	}
	else if (bClearIfNoneApplied)
	{
		// We applied no attributes so we can just chuck this undo record
		// and all its actions away
		SucceedAndDiscard();
	}

	// Output values
	*Success = TRUE;				// If we got here we must have succeeded
	*AppliedAttrs = bAnyApplied;	// Tell caller if any attributes were applied
  
EndOperation:
	
	// Delete our lists of attribute types
	LineLevelAttrTypes.DeleteAll();
	OtherAttrTypes.DeleteAll();

	if (bWeStartedThisOp)
		End();

	return *Success;
} 


/********************************************************************************************

>	BOOL SelOperation::ApplyToSelection(List* Attribs, List* AttrGroupList)		   

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/95

	Inputs:		A list of attributes to apply to the selection. Each item in the list is
				a NodeAttributePtrItem.

	
	Outputs:	AttrGroupList:	 This list must be empty on entry to this function
								 If (Success) then this list will contain one ListListItem
								 per attribute on the Attribs List. Each of these
								 items will point to a Set of  AttributeGroupItems.

								 Attribute -> { Set of current attribute groups }
								 
								 Whenever an attribute is applied to an object, the Current
								 AttributeGroup associated with the object is added to
								 the attributes set of current attribute groups. 
								 Therefore if an attribute's set is empty then we know
								 that the attribute has not been applied.

	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!)
				
	SeeAlso:	OpApplyAttribsToSelected::DoWithParam 

********************************************************************************************/

BOOL SelOperation::DoApplyToSelection(List* Attribs, List* AttrGroupList, BOOL* pAttribWasRequired)
{
	ERROR3IF(!(AttrGroupList->IsEmpty()), "Invalid output param");

	// Set up a range of all selected nodes 
	SelRange* pSel = GetApplication()->FindSelection();
	EffectsStack* pStack = pSel->GetEffectsStack(FALSE, FALSE);	// Get cached stack, don't escape derived objects (allow attrs to be applie to derived objects)
	ERROR3IF(pStack==NULL, "Failed to get PPStack in ApplyToSelection");
	INT32 stackpos = STACKPOS_TOP;
	Range* pLevel = pStack->GetLevelRange(&stackpos, FALSE);			// Don't escape old controllers, apply attr to base nodes

	BOOL bCanDiscardUndo = TRUE;

	NodeAttributePtrItem* pAttrItem = (NodeAttributePtrItem*)(Attribs->GetHead());
	while (pAttrItem)
	{
		NodeAttribute* pAttr = pAttrItem->NodeAttribPtr;

		// Create the pAttr's set of current attribute groups { currently empty }
		ListListItem* pAttrsSetItem = new ListListItem;
		if (!pAttrsSetItem)
			goto Failed;
		List* pAttrsSet = &(pAttrsSetItem->list);
	 	AttrGroupList->AddTail(pAttrsSetItem); 
		
		// While there are still nodes to apply the attribute to
		Node* CurrentNode = pLevel->FindFirst();
//		Node* FirstNode = CurrentNode;
		while (CurrentNode)   
		{
			NodeAttribute* AttrToApply = pAttr;
			BOOL AppliedAttribIsCopy = FALSE;
			BOOL ThisAttrWasRequired = FALSE;
			if (pAttr->IsAFillAttr())
			{
				// Transform the attribute to fit the node's bounds
				DocRect NodeBounds = ((NodeRenderableInk*)CurrentNode)->GetBoundingRect(TRUE);
				// We will transform a copy of the attribute.
				AttrToApply = (NodeAttribute*)(pAttr->SimpleCopy());
				AppliedAttribIsCopy = TRUE; // cos it is
				if (!AttrToApply)
					goto Failed;
				AttrToApply->TransformToNewBounds(NodeBounds);
			}

			if (!DoApply(CurrentNode, 
						 AttrToApply, 
						 FALSE,  // Don't mutate
						 FALSE,	 // Don't invalidate region
						 FALSE,	 // Don't keep existing characteristics
						 &ThisAttrWasRequired,
						 &bCanDiscardUndo))
			{
				goto Failed;
			}

			if (ThisAttrWasRequired)
			{
				*pAttribWasRequired = TRUE;

				// This next bit is a bit yucky, we are in desperate need of templates to implement
				// proper sets.
				// The attribute was required by the object
				// Add the current attribute group associated with the object to the attributes set
				// Search the pAttrsSet set to see if it's already there

				CCRuntimeClass* NewAttrGroup = ((NodeRenderableInk*)CurrentNode)->GetCurrentAttribGroup();
				ERROR3IF(NewAttrGroup == NULL, "Object has a NULL attribute group"); 
				BOOL InSet = FALSE;
				AttributeGroupItem* pAttrGrpItem = (AttributeGroupItem*)pAttrsSet->GetHead();
				while(pAttrGrpItem != NULL)
				{
					CCRuntimeClass* InSetAttrGroup = pAttrGrpItem->AttrGroup;
					ERROR3IF(InSetAttrGroup == NULL, "NULL attribute group found");
					if (NewAttrGroup == InSetAttrGroup)
					{
						InSet = TRUE; 
						break; // It's already in the set don't add it
					}
					pAttrGrpItem = (AttributeGroupItem*)pAttrsSet->GetNext(pAttrGrpItem);
				}
			
				if (!InSet)	// The AttrGroup needs adding to the pAttrsSet 
				{
					// Create a new AttrGroupItem
					pAttrGrpItem = new AttributeGroupItem();
					if (!pAttrGrpItem)
						goto Failed;

					pAttrGrpItem->AttrGroup = NewAttrGroup;

					// And add it to our set
					pAttrsSet->AddHead(pAttrGrpItem); 
				}  
			}

			if (CurrentNode->IsAnObject())
				((NodeRenderableInk*)CurrentNode)->InvalidateBoundingRect();

			CurrentNode = pLevel->FindNext(CurrentNode);
			if (AppliedAttribIsCopy)
			{
				delete AttrToApply; // not required
				AttrToApply = NULL; 
			}
		}
		ContinueSlowJob();
		pAttrItem = (NodeAttributePtrItem*)(Attribs->GetNext(pAttrItem));
	}

	// If all nodes report DiscardsAttributeChildren() = TRUE
	// Then we can abandon the entire Op once it's finished
	if (bCanDiscardUndo)
		SucceedAndDiscard();

	return TRUE; 

	Failed:
	// Tidyup, and return FALSE
	AttrGroupList->DeleteAll(); 
	return FALSE;
}

/********************************************************************************************

>	BOOL OpApplyAttribsToSelected::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpApplyAttribsToSelected initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.

********************************************************************************************/

BOOL OpApplyAttribsToSelected::Init()
{
	// Register the opdescriptor. Don't need to do it the posh way cos this op will
	// not be connected directly to the interface.
	OpDescriptor* OpDesc = new OpDescriptor(0,
								 			_R(IDS_APPLYATTRIBOP), // Never used				
											CC_RUNTIME_CLASS(OpApplyAttribsToSelected),
								 			OPTOKEN_APPLYATTRIBS,
											OpApplyAttribsToSelected::GetState);
	ERRORIF(!OpDesc, _R(IDE_NOMORE_MEMORY), FALSE);
	
	return(TRUE);
}               

/********************************************************************************************

>	OpState	OpApplyAttribsToSelected::GetState(String_256*, OpDescriptor* OpDesc)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93

	Returns:	The state of the OpApplyAttribsToSelected operation. This fn will 
				probably never get called !, because the operation is not attached to the UI.

	Purpose:	For finding OpApplyAttribsToSelected state. 

********************************************************************************************/

OpState	OpApplyAttribsToSelected::GetState(String_256*, OpDescriptor* OpDesc)
{
	OpState OpSt;
	return(OpSt);   
}


/********************************************************************************************

>	virtual void OpApplyAttribsToSelected::GetOpName(String_256* OpName) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/94
	Outputs:	The undo string for the operation
	Purpose:	The GetOpName fn is overridden so that we can return a string
				suitable for our purposes.

********************************************************************************************/

void OpApplyAttribsToSelected::GetOpName(String_256* OpName) 
{ 

	*OpName = String_256(UndoAttribStrID); 
}  


/********************************************************************************************
********************************************************************************************/


/********************************************************************************************

>	OpRepeatApplyAttribToSelected::OpRepeatApplyAttribToSelected() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpRepeatApplyAttribToSelected constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
            
OpRepeatApplyAttribToSelected::OpRepeatApplyAttribToSelected(): Operation()								
{                              
}


/********************************************************************************************

>	void OpRepeatApplyAttribToSelected::DoWithParam(OpDescriptor* OpDesc, OpParam* pOpParam)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/95
	Inputs:		OpDescriptor 
	Outputs:	pOpParam: Param1 contains a pointer to the NodeAttribute to apply to the 
						  currently selected nodes
	Returns:	-
	Purpose:	Performs the OpApplyAttribToSelected operation. This function applies the
				NodeAttribute to all selected objects.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
		
void OpRepeatApplyAttribToSelected::DoWithParam(OpDescriptor* OpDesc, OpParam* pOpParam)		   
{    
	ERROR3IF(pOpParam == NULL, "The OpApplyAttribToSelected operation requires an attribute parameter"); 

	AttrTypeSet AttrTypes; 
	NodeRenderableInk* pLocalisedCompound = NULL;
	NodeRenderableInk* pFactoredOutCompound = NULL;
	Node* pCurrent;

	Range SelRng(*(GetApplication()->FindSelection()));

	BeginSlowJob();
	AttributeManager::pLastNodeAppliedTo = NULL;

	// Obtain a pointer to the attribute which we will need to apply to all selected nodes 
	NodeAttribute* Attrib = (NodeAttribute*)(void *)pOpParam->Param1;

	BOOL Mutate = FALSE;
	NodeAttribute* OldAttr = NULL;

	if (Attrib == NULL)
	{
		// If the first param is NULL then we are doing a mutation
		Attrib = (NodeAttribute*)(void *)pOpParam->Param2;
		Mutate = TRUE;
	}	
	else
   	{
		// Someone has specified a Particular Attribute that we should replace
		// and ignore all others.
   		OldAttr = (NodeAttribute*)(void *)pOpParam->Param2;
	}

	SelRange* Sel;
	ObjChangeFlags cFlags;
	cFlags.Attribute = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);

	Sel = GetApplication()->FindSelection();
	if (!Sel->AllowOp(&ObjChange))
	{
		FailAndExecute();
		goto EndOperation;
	}

	// Some Attributes require a second attribute to be changed as well,
	// which has to be done within this op.
	NodeAttribute* OtherAttrib;
	BOOL IsMutate;
	OtherAttrib = AttributeManager::GetOtherAttrToApply(Attrib, &IsMutate);

	// Before we apply the attribute to the selection we must localise all attributes
	// with the same type that we are going to apply. If we don't do this then the
	// tree will be left in an invalid state. 

	AttrTypes.AddToSet((Attrib->GetAttributeType())); 
	if (OtherAttrib != NULL)
	{
		AttrTypes.AddToSet((OtherAttrib->GetAttributeType())); 
	}
		
//	if (!DoLocaliseForAttrChange(&SelRng, &AttrTypes))
//	{
//		goto EndOperation;
//	}

	pCurrent = SelRng.FindFirst();

	// There is no need to localise a compound more than once so we remember the last localised 

	CCRuntimeClass* AttrType;
	Node* pParent;
	Node* pObject;
				  		
	while (pCurrent)
	{
		pObject = pCurrent;
		// Determine if the attribute will get applied to this object, or an alternative object
		// (probably its parent)
		if (!AttrTypes.IsEmpty())
		{
			AttrType = ((AttrTypeItem*)AttrTypes.GetHead())->AttributeType;
			ERROR3IF(!AttrType, "AttrType set contains NULL attribute type");  
			pObject = ((NodeRenderableInk*)pObject)->GetObjectToApplyTo(AttrType);
		}

		{
			// We only need to localise those nodes which have a compound parent
			pParent = pObject->FindParent(); 
			ERROR3IF(pParent == NULL, "Range::DoLocaliseForAttrChange, node found without a parent"); 
		
			// Only localise the parent if it is a compound node which has not already been localised
			if ((pParent->IsCompound()) && (pParent != pLocalisedCompound))
			{
				((NodeRenderableInk*)pParent)->LocaliseCommonAttributes(FALSE,	 	// No need to check for duplicates
																		TRUE,  	 	// Localise globally
												  						&AttrTypes);// Only attributes of these types

				pLocalisedCompound = (NodeRenderableInk*)pParent;  // Remember that it's been localised
			}
		}
		
		pCurrent = SelRng.FindNext(pCurrent); 
	}

	ApplyToSelection(Attrib, Mutate);

	if (OtherAttrib != NULL)
	{
		ApplyToSelection(OtherAttrib, IsMutate);
		delete OtherAttrib;
	}	

	// Having applied the attributes, we  must now try and factor out the newly 
	// applied attributes
//	if (!DoFactorOutAfterAttrChange(&SelRng, 
//									&AttrTypes))
//	{
//		goto EndOperation;
//	}

	// Scan the range
	pCurrent = SelRng.FindFirst();

	// There is no need to factor out the attributes on a compound more than once, so we 
	// remember the last compound node which has had its attributes factored out 

	while (pCurrent)
	{
		// If the object can discard its attribute children then we should not try to factor
		// out its attributes
		CCRuntimeClass* AttrType;
		
		pObject = pCurrent;
		if (!AttrTypes.IsEmpty())
		{
			AttrType = ((AttrTypeItem*)AttrTypes.GetHead())->AttributeType;
			ERROR3IF(!AttrType, "AttrType set contains NULL attribute type");  
			pObject = ((NodeRenderableInk*)pObject)->GetObjectToApplyTo(AttrType);
		}

		{
			// We only need to factor out attributes on nodes which have a compound parent
			pParent = pObject->FindParent(); 
			ERROR3IF(pParent == NULL, "Range::DoFactorOutAfterAttrChange, node found without a parent"); 
		
			// Only factor out attribs if  the parent has not already had its attribs factored out
			if ((pParent->IsCompound()) && (pParent != pFactoredOutCompound))
			{
				((NodeRenderableInk*)pParent)->FactorOutCommonChildAttributes(TRUE, &AttrTypes);
				 
				pFactoredOutCompound = (NodeRenderableInk*)pParent;  // Remember that it's been localised
			}
		}
		
		pCurrent = SelRng.FindNext(pCurrent); 
	}

	AttrFillGeometry::LastRenderedMesh = NULL;

	if (Document::GetSelected())
		Document::GetSelected()->SetModified(TRUE);

	ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,NULL);
	if (!UpdateChangedNodes(&ObjChange))
		FailAndExecute();

	AttributeManager::LastAppliedBounds = Sel->GetBoundingRect();

EndOperation:			
	AttrTypes.DeleteAll();

	EndSlowJob();
	End(); 				// End of operation
} 

/********************************************************************************************

>	BOOL OpRepeatApplyAttribToSelected::ApplyToSelection(NodeAttribute* Attrib, BOOL Mutate)		   

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 


	Purpose:	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/



BOOL OpRepeatApplyAttribToSelected::ApplyToSelection(NodeAttribute* Attrib, BOOL Mutate)		   
{
	SelRange* Sel = GetApplication()->FindSelection();
	ERROR3IF(Sel==NULL,"Can't find SelRange!");
	if (Sel==NULL) return FALSE;
	Node* FirstSelectedNode = Sel->FindFirst();

	// Find out which spread the selection is on
//	Spread* pSpread = FirstSelectedNode->FindParentSpread();

	if (FirstSelectedNode != NULL) 	  // Can't do anything if nothing is selected
	{ 	
		EffectsStack* pStack = Sel->GetEffectsStack(FALSE, FALSE);	// Get cached stack, don't escape derived objects (allow attrs to be applie to derived objects)
		ERROR3IF(pStack==NULL, "Failed to get PPStack in ApplyToSelection");
		INT32 stackpos = STACKPOS_TOP;
		Range* pLevel = pStack->GetLevelRange(&stackpos, FALSE);		// Don't escape old controllers, apply attr to base nodes
		Node* pCurrentNode = pLevel->FindFirst();

		// While there are still nodes to apply the attribute to
		while (pCurrentNode != NULL)   
		{
			BOOL bEffect = (pCurrentNode->IsAnObject() && ((NodeRenderableInk*)pCurrentNode)->IsValidEffectAttr(Attrib));

			Node* pApplyNode = pCurrentNode;
			if (pApplyNode->IsAnObject())
				pApplyNode = ((NodeRenderableInk*)pApplyNode)->GetObjectToApplyTo(Attrib->GetRuntimeClass());

			if (!DoApply(pApplyNode, Attrib, Mutate, bEffect))
				return FALSE;

			ContinueSlowJob();

			pCurrentNode = pLevel->FindNext(pCurrentNode);
		} 
	}

	return TRUE;
}




/********************************************************************************************

>	BOOL OpRepeatApplyAttribToSelected::DoApply(Node* CurrentNode, NodeAttribute* Attrib, BOOL Mutate, BOOL bEffectRootOnly)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		-
	Outputs:	-
	Returns:	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 

	Purpose:	
	Errors:		-
	SeeAlso:	-
	Note:		This routine may no longer be needed now that OpApplyAttrib::DoApply is static
				and can be told not to write undo info.

********************************************************************************************/

BOOL OpRepeatApplyAttribToSelected::DoApply(Node* CurrentNode, NodeAttribute* Attrib, BOOL Mutate, BOOL bEffectRootOnly)
{
	ERROR3IF(!CurrentNode->IsBounded(), "DoApply assumes CurrentNode is bounded");

	// have we been allowed to do this ???
	Spread* pSpread = (Spread*)CurrentNode->FindParent(CC_RUNTIME_CLASS(Spread)); 
	DocRect TempRect = ((NodeRenderableBounded*)CurrentNode)->GetBoundingRect();

	BOOL AttributeExists = FALSE;  // Until we find that the attribute does exist

	CCRuntimeClass* AttrType = Attrib->GetAttributeType();

	// -----------------------------------------------------------------------------
	// Determine if the current node already has an attribute which is the same 
	// runtime class as Attrib. 
	Node* n = CurrentNode->FindFirstChild(); 
	Node* pLastBoundedNode = NULL;
	if (bEffectRootOnly)
	{
		// BODGE! ------------------------------------------------------------
		// Don't apply stroke transparency as an effect attribute - nothing
		// needs it. Test has to be done here because other objects in the
		// selection might need it...
		if (AttrType==CC_RUNTIME_CLASS(AttrStrokeTransp))
			return TRUE;
		// END-BODGE! --------------------------------------------------------

		pLastBoundedNode = CurrentNode->FindLastChild(CC_RUNTIME_CLASS(NodeRenderableBounded));
		ERROR3IF(pLastBoundedNode==NULL, "Attempt to apply effect attr to node with no children");

		if (pLastBoundedNode) n = pLastBoundedNode->FindNext();
	}

	while (n != NULL)
	{
		// Now lets see if we can find an attribute of the same type
		// as the one we are interested in.
		if (n->IsAnAttribute())
		{
			if( ((NodeAttribute*)n)->GetAttributeType() == AttrType)
			{
				AttributeExists = TRUE; 
				break; 
			}
		}
		n = n->FindNext(); // Find next child of CurrentNode
	}	

	// At this point we have Either .....
	//      1. Found an attribute of the same type as the new one, and we will
	//		   now replace it.
	//      2. Found the specific attribute we were looking for and will replace
	//         that.
	//      3. Found an attribute to mutate.
	//      4. Found no attribute of the correct type, indicating that it has
	//         been factored out, and we need to put a new one in here.

	// Have we have got an Attribute to do something with ?
	NodeAttribute* AttribClone = NULL; 
	NodeAttribute* pAttr = (NodeAttribute*)n;
	if (pAttr)
	{
		// Yes !
		if (Mutate)
		{
			// Mutate it into the new type.
			AttribClone = ((AttrFillGeometry*)pAttr)->Mutate((AttrFillGeometry*)Attrib);
		}

		if (AttribClone != NULL)
		{
			// Set the attribute directly
			*((AttrFillGeometry*)pAttr) = *((AttrFillGeometry*)AttribClone);

			AttributeManager::pLastNodeAppliedTo = (NodeRenderableInk*)CurrentNode;

			delete AttribClone;
			AttribClone = NULL;
		}
	}
	else
	{
		// We've not found an attribute to replace, so we'll have to put
		// a new one in
		NodeAttribute* TempAttr = NULL;

		BOOL FoundAttr = ((NodeRenderableInk*)CurrentNode)->
							FindAppliedAttribute(Attrib->GetAttributeType(), &TempAttr);

		if (!FoundAttr || TempAttr == NULL)
			return FALSE;

		if (Mutate)
		{
			AttribClone = ((AttrFillGeometry*)TempAttr)->Mutate((AttrFillGeometry*)Attrib);
		}
		else
		{
			// We'll just put a copy of our attribute in the tree.
	        ALLOC_WITH_FAIL(AttribClone ,((NodeAttribute*)Attrib->SimpleCopy()), this)

			if (TempAttr->IsAFillAttr())
			{
	            AttrFillGeometry* NodeReplaced = (AttrFillGeometry*)TempAttr;

				// Copy the old fill characteristics into the new Fill
				if (!OpApplyAttrib::KeepExistingCharacteristics(NodeReplaced, (AttrFillGeometry*)AttribClone))
					return FALSE;
			}

			if (Attrib->IsATranspFill() && Attrib->IsAFlatFill())
			{
				// If we are mutating into a flat fill, then we need
				// to make sure we remove any existing fill blobs.
				((AttrFillGeometry*)TempAttr)->RenderFillBlobs();
			}
		}

		// If the AttribClone has the same value as an applied attribute 
		// (A default attr if the attribs have been localised). Then we don't want to apply it !
		if (AttribClone && AttribClone->ShouldBeOptimized() && !bEffectRootOnly)
		{
			NodeAttribute* pAppliedAttr;
			if (((NodeRenderableInk*)CurrentNode)->FindAppliedAttribute(AttribClone->GetAttributeType(), 
												  &pAppliedAttr))
			{
				// Do the attributes have the same value ?
				if ((IS_SAME_CLASS(AttribClone, pAppliedAttr)))
				{
					if ((*AttribClone)==(*pAppliedAttr))
					{
						AttribClone->CascadeDelete();		// Delete the attribute

						// If attribute affect's the bounds of the object (eg. a LineWidth) then invalidate the
						// bounds of the object

						if (pAppliedAttr->EffectsParentBounds())
						{
							((NodeRenderableBounded*)CurrentNode)->InvalidateBoundingRect(TRUE);
						}

						delete AttribClone;
						AttribClone = NULL;
					}
				}
			}
		}

		// Effect attributes don't optimise so if the value of this is the
		// same as the default value we should not add it here
		//
		if (AttribClone && bEffectRootOnly && AttribClone->HasEquivalentDefaultValue())
		{
			// Just allow the attribute to be hidden
			AttribClone->CascadeDelete();		// Delete the attribute
			delete AttribClone;
			AttribClone = NULL;
		}

		if (AttribClone)
		{
			// Finally !! We can add the new attribute node into the tree.
			if (bEffectRootOnly)
				AttribClone->AttachNode(pLastBoundedNode, NEXT);
			else
				AttribClone->AttachNode(CurrentNode, FIRSTCHILD);

			AttributeManager::pLastNodeAppliedTo = (NodeRenderableInk*)CurrentNode;

			// And now it's in the tree, we need to make sure that any fill control
			// points are valid.
			if (AttribClone->IsAFillAttr())
			{
		    	((AttrFillGeometry*)AttribClone)->AttributeChanged();
			}
		}
	}

	// Now that the attr value has been set (or left unchanged) test to see if we can remove effect attrs
	// Only try to "optimise" effect attributes here if this is going to be a permanent tree change
	// Don't bother in the middle of slider dragging
	if (pAttr && pAttr->IsEffectAttribute() && bEffectRootOnly)
	{
		// Effect attributes don't optimise so if the value of this is the
		// same as the default value  we should remove it here
		//
		if (pAttr->HasEquivalentDefaultValue())
		{
			// "Hide" the node
			pAttr->CascadeDelete();
			delete pAttr;
		}
	}

	// Now force a redraw on the attribute
	// We release cached data manually, because we need to be clever about effect attributes
	// only releasing parent cached data...
//	ReleaseCachedForAttrApply((NodeRenderableBounded*)CurrentNode, bEffectRootOnly); // Function not available in this class!
	if (bEffectRootOnly)
		((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);		// Parents and Derived data only
	else
		((NodeRenderableBounded*)CurrentNode)->ReleaseCached();													// Self, Parents and Children

	// (GetSelected... URGH.)
	Document::GetSelected()->ForceRedraw(pSpread, TempRect, TRUE, CurrentNode, FALSE);	// No automatic call to ReleaseCached
	return TRUE;
}


/********************************************************************************************

>	BOOL OpRepeatApplyAttribToSelected::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpApplyAttribToSelected initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpRepeatApplyAttribToSelected::Init()
{
	// Register the opdescriptors for the OpApplyAttribToSelected operation
	OpDescriptor* OpDesc = new OpDescriptor(
								 			0,
								 			_R(IDS_APPLYATTRIBOP), 				
											CC_RUNTIME_CLASS(OpRepeatApplyAttribToSelected),
								 			OPTOKEN_REPEATAPPLYATTRIB,
								 			OpRepeatApplyAttribToSelected::GetState);	

	ERRORIF(!OpDesc, _R(IDE_NOMORE_MEMORY), FALSE);
	return(OpDesc != NULL);
}               

/********************************************************************************************

>	OpState	OpRepeatApplyAttribToSelected::GetState(String_256*, OpDescriptor* OpDesc)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/95
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpRepeatApplyAttribToSelected operation
	Purpose:	For finding OpRepeatApplyAttribToSelected state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpRepeatApplyAttribToSelected::GetState(String_256*, OpDescriptor* OpDesc)
{
	OpState OpSt;

	SelRange* Sel = GetApplication()->FindSelection();
	ERROR2IF(Sel==NULL,OpSt,"Can't find SelRange!");

	// The Operation is greyed if there are no nodes selected 
	OpSt.Greyed = ( (Sel->FindFirst() == NULL) );  
												
	return(OpSt);   
}


/********************************************************************************************

>	virtual void OpRepeatApplyAttribToSelected::GetOpName(String_256* OpName) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/95
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
					
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpRepeatApplyAttribToSelected::GetOpName(String_256* OpName) 
{ 
	*OpName = "";
}  

CCRuntimeClass* OpApplyAttrib::GetValueChangeType()
{
	return ValueChangeType;
}

BOOL OpApplyAttrib::IsMergeableApplyOp()
{
	return MergeRepeats;
}


/********************************************************************************************

								OpApplyAttribToNode

********************************************************************************************/



/********************************************************************************************

>	OpApplyAttribToNode::OpApplyAttribToNode() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpApplyAttribToNode constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
            
OpApplyAttribToNode::OpApplyAttribToNode(): OpApplyAttrib()								
{
	m_pAttr = NULL;
}


/********************************************************************************************

>	void OpApplyAttribToNode::DoWithParam(OpDescriptor* OpDesc, OpParam* pOpParam)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>/Simon
	Created:	29/1/95
	Inputs:		OpDescriptor 
	Outputs:	pOpParam: Param1 contains a pointer to the NodeAttribute to apply to the 
						  currently selected nodes
	Returns:	-
	Purpose:	Performs the OpApplyAttribToNode operation. This function applies the
				NodeAttribute to all selected objects.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
		
void OpApplyAttribToNode::DoWithParam(OpDescriptor* OpDesc, OpParam* pOpParam)		   
{    
	ERROR3IF(pOpParam == NULL, "The OpApplyAttribToNode operation requires an attribute parameter"); 

	// Obtain a pointer to the attribute which we will need to apply to all selected nodes 
	NodeRenderableInk* InkNode = (NodeRenderableInk*)(void *)pOpParam->Param1;
	NodeAttribute* NewAttr = (NodeAttribute*)(void *)pOpParam->Param2;
	m_pAttr = NewAttr;

	ValueChangeType = NewAttr->GetRuntimeClass();

	if (NewAttr)
		UndoAttribStrID = NewAttr->GetAttrNameID();

	ObjectSet LocalisedCompoundSet; // Every time we localise a compound's attributes we will add the
						   			// compound to this set.

   	AttrTypeSet AttrTypes;			// The attribute types to localise and factor out 
	
	BOOL NotInterested; 
	NodeAttribute* OtherAttrib = AttributeManager::GetOtherAttrToApply(NewAttr, &NotInterested);

	// Prepare an ObjChangeParam for use when asking the ink node if we can do this op to it.
	ObjChangeFlags cFlags;
	cFlags.Attribute = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

	// If this attribute affects the bounds if its parents then we must take the blobs off
	// at the start.  We can rely on the redraw at the end to put them back on
	BOOL AttrAffectsBounds = NewAttr->EffectsParentBounds();
	if (!DoStartSelOp(AttrAffectsBounds, FALSE, TRUE, TRUE)) 
	{
		goto End;
	}
	
	// Find out the types of attribute about to be applied, and add them to the AttrTypes set
	ERROR3IF(NewAttr==NULL,"NewAttr == NULL");
	AttrTypes.AddToSet((NewAttr->GetAttributeType()));

	if (OtherAttrib != NULL)
	{
		AttrTypes.AddToSet((OtherAttrib->GetAttributeType())); 
	}
	

	// If we are applying an attribute to a VTN which is selected then apply the attribute
	// to this node and all other selected nodes.
	if (InkNode->IsAnAbstractTextChar() && InkNode->IsSelected())
	{
		AbstractTextChar* Scan = (AbstractTextChar*)InkNode;
		AbstractTextChar* Last;
			
		// Find the first selected AbstractTextChar
		do
		{
			InkNode = Scan;
			Scan = Scan->FindPrevAbstractTextCharInStory();
		} while ((Scan != NULL) && (Scan->IsSelected()));

		// Find the last
		Scan = (AbstractTextChar*)InkNode;
		do
		{
			Last = Scan;
			Scan = Scan->FindNextAbstractTextCharInStory();
		} while ((Scan != NULL) && (Scan->IsSelected()));
		   
		Range SubSelRange(InkNode, Last, RangeControl(TRUE));

		// Will the ink node (and all its parents) allow this op to happen?
		if (!SubSelRange.AllowOp(&ObjChange))
		{
			FailAndExecute();
			goto End;	
		}


		// Invalidate the region before applying the attribute/s
		BOOL IsMutate;
		NodeAttribute* OtherAttr = AttributeManager::GetOtherAttrToApply(NewAttr, &IsMutate);

		if (!DoInvalidateRegions(&SubSelRange, 
								 NewAttr, 
								 NewAttr->IsAFillAttr(),
								 OtherAttr,
								 IsMutate))  //Mutators have to include bounds
		{
			goto End;
		}


		// Apply the attr to all selected AbstractTextChars
		do
		{
			if (!DoApplyAttrib(InkNode, NewAttr, &AttrTypes, &LocalisedCompoundSet))
			{
				// error will have been set
				FailAndExecute(); // Just make sure
				goto End;
			}
			InkNode = ((AbstractTextChar*)InkNode)->FindNextAbstractTextCharInStory();
		} while ((InkNode != NULL) && (InkNode->IsSelected()));


		// Invalidate the region after applying the attribute/s
		if (!DoInvalidateRegions(&SubSelRange, 
								 NewAttr, 
								 NewAttr->IsAFillAttr(),
								 OtherAttr,
								 IsMutate))  //Mutators have to include bounds
		{
			goto End;		
		}


	}
	else
	{
		// Will the ink node (and all its parents) allow this op to happen?
		if (!InkNode->AllowOp(&ObjChange))
		{
			FailAndExecute(); 
			goto End;	
		}

		if (!DoApplyAttrib(InkNode, NewAttr, &AttrTypes, &LocalisedCompoundSet, TRUE))
		{
			goto End;
		}

	}
	
	// Finally factor out the neccessary attributes
	if (!DoFactorOutAfterAttrChange(&LocalisedCompoundSet,
									&AttrTypes))
	{
		goto End;
	}

	// Update all effected parent nodes

	ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
	if (!UpdateChangedNodes(&ObjChange))
	{
		FailAndExecute();
	}
	  
	End:
	// First tidy up
	AttrTypes.DeleteAll();
	LocalisedCompoundSet.DeleteAll();
		
	// then end
	End(); // End of operation
}

/********************************************************************************************

>	BOOL OpApplyAttribToNode::DoApplyAttrib(NodeRenderableInk* InkNode, 
											NodeAttribute* NewAttr, 
										    AttrTypeSet* pAttrTypesToBeApplied, 
										    ObjectSet* pLocalisedCompounds, 
										    BOOL InvalidateBounds = FALSE)		   
)		   

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>/Simon
	Created:	14/4/95
	Inputs:		InkNode: 			   The node to apply the attribute to
				NewAttr: 			   The attribute to apply

				pAttrTypesToBeApplied: A set of the types of the attributes which 
									   are to be applied. Before we apply an attribute 
									   we localise all attrs in this set.

				InvalidateBounds:	   When TRUE bounds will be invalidated.
				
	Outputs:	pLocalisedCompounds:   This is also an input. We maintain a set of the
									   compound objects which have been localised, so that
									   we don't localise more than once on each.

	Returns:	TRUE if successful, otherwise FALSE (Tidyup and call end)
	Purpose:	Applies NewAttr to InkNode
	Errors:		-
	SeeAlso:	-

********************************************************************************************/



BOOL OpApplyAttribToNode::DoApplyAttrib(NodeRenderableInk* InkNode, NodeAttribute* NewAttr, 
										AttrTypeSet* pAttrTypesToBeApplied, 
										ObjectSet* pLocalisedCompounds, 
										BOOL InvalidateBounds)		   
{
	NodeAttribute* OtherAttr;

	AttributeManager::pLastNodeAppliedTo = NULL;

	ERROR3IF(InkNode==NULL,"InkNode == NULL");
	ERROR3IF(NewAttr==NULL,"NewAttr == NULL");
	if (InkNode == NULL) return FALSE;


	// Get a description of the attribute being applied so that we can use it to create the 
	// undo string. 
	UndoAttribStrID = NewAttr->GetAttrNameID();

	if (!AttrFillGeometry::HitList.IsEmpty())
	{
		// Bodge Alert !!
		// This is not nice, but it cure the problem !!
		// When a colour drop occurs on a blob which is part of a multi-select
		// fill mesh, then the colour drop will have to change several fills
		// at once.
		// To make this happen, I have made the colour drop checking routine
		// build a list of all the hit fills, in the 'HitList'.

		// The 'HitList' has some items in it, so lets apply the mutator to all
		// of the fills parents.

		ListItem* pAttrPtr = AttrFillGeometry::HitList.GetHead();

		while (pAttrPtr != NULL)
		{
			// It's a list of Attribute Pointer Items,
			// so lets extract the pointer
			NodeAttribute* pAttr = ((NodeAttributePtrItem*)pAttrPtr)->NodeAttribPtr;
			NodeRenderableInk* pParent = (NodeRenderableInk*)pAttr->FindParent();

			// Before we apply the attributes, we localise all attrs of the same type
			if (!DoLocaliseForAttrChange(pParent, 
							 			 pAttrTypesToBeApplied,
							 			 pLocalisedCompounds))
			{
				return FALSE; 
			}

			if (!DoApply(pParent, NewAttr, NewAttr->IsAFillAttr(), InvalidateBounds))
			{
				return FALSE; 
			}


			// Some Attributes require a second attribute to be changed as well,
			// which has to be done within this op.

			BOOL IsMutate;
			OtherAttr = AttributeManager::GetOtherAttrToApply(NewAttr, &IsMutate);
	
			if (OtherAttr != NULL)
			{
				if (!DoApply(pParent, OtherAttr, IsMutate, InvalidateBounds))
				{
					return FALSE; 
				}
				delete OtherAttr;
			}	

			pAttrPtr = AttrFillGeometry::HitList.GetNext(pAttrPtr);
		}
		
		// Now make sure the HitList is cleared out for next time
		AttrFillGeometry::HitList.DeleteAll();
	}
	else
	{
		// Before we apply the attributes, we localise all attrs of the same type
		if (!DoLocaliseForAttrChange(InkNode, 
						 			 pAttrTypesToBeApplied,
						 			 pLocalisedCompounds))
		{
			return FALSE; 
		}

		if (!DoApply(InkNode, NewAttr, NewAttr->IsAFillAttr(), InvalidateBounds))	// The attrib is always a Mutator
			return FALSE; 

		// Some Attributes require a second attribute to be changed as well,
		// which has to be done within this op.

		BOOL IsMutate;
		OtherAttr = AttributeManager::GetOtherAttrToApply(NewAttr, &IsMutate);
	
		if (OtherAttr != NULL)
		{
			if (!DoApply(InkNode, OtherAttr, IsMutate, InvalidateBounds))
			{
				return FALSE; 
			}
			delete OtherAttr;
		}	
	}

	if (Document::GetSelected())
		Document::GetSelected()->SetModified(TRUE);


	AttributeManager::LastAppliedBounds = InkNode->GetBoundingRect();

	return TRUE;
} 
 

/********************************************************************************************

>	BOOL OpApplyAttribToNode::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpApplyAttribToNode initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpApplyAttribToNode::Init()
{
	
	// Register the opdescriptors for the OpApplyAttribToNode operation
	OpDescriptor* OpDesc = new OpDescriptor(
								 			0,
								 			_R(IDS_APPLYTONODEOP), 				
											CC_RUNTIME_CLASS(OpApplyAttribToNode),
								 			OPTOKEN_APPLYTONODE,
								 			OpApplyAttribToNode::GetState);	

	ERRORIF(!OpDesc, _R(IDE_NOMORE_MEMORY), FALSE);
	return(OpDesc != NULL);
}               

/********************************************************************************************

>	OpState	OpApplyAttribToNode::GetState(String_256*, OpDescriptor* OpDesc)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/1/95
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpApplyAttribToNode operation
	Purpose:	For finding OpApplyAttribToNode state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpApplyAttribToNode::GetState(String_256*, OpDescriptor* OpDesc)
{
	OpState OpSt;

	OpSt.Greyed = ( FALSE );  
												
	return(OpSt);   
}


/********************************************************************************************

>	virtual void OpApplyAttribToNode::GetOpName(String_256* OpName) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/1/95
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
					
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpApplyAttribToNode::GetOpName(String_256* OpName) 
{ 
	
	*OpName = String_256(UndoAttribStrID); 
	*OpName += String_256(_R(IDS_CHANGE)); 
}  


/********************************************************************************************

								OpReplaceAttributes

********************************************************************************************/



/********************************************************************************************

>	OpReplaceAttributes::OpReplaceAttributes() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpReplaceAttributes constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
            
OpReplaceAttributes::OpReplaceAttributes(): SelOperation()								
{                
	m_pAttr = NULL;
}


/********************************************************************************************

>	void OpReplaceAttributes::DoWithParam(OpDescriptor* OpDesc, OpParam* pOpParam)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/1/95
	Inputs:		OpDescriptor 
	Outputs:	pOpParam: Param1 contains a pointer to the NodeAttribute to apply to the 
						  currently selected nodes
	Returns:	-
	Purpose:	Performs the OpReplaceAttributes operation. This function applies the
				NodeAttribute to all selected objects.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
		
void OpReplaceAttributes::DoWithParam(OpDescriptor* OpDesc, OpParam* pOpParam)		   
{    
	ERROR3IF(pOpParam == NULL, "The OpReplaceAttributes operation requires an attribute parameter");
	ERROR3IF(!IS_A(pOpParam, ReplaceAttributesParam), "Wrong kind of param in OpReplaceAttributes");

	AttributeManager::pLastNodeAppliedTo = NULL;

	// Objects used to mark changed nodes, so that parents will update after attr replacement
	ObjChangeFlags cFlags;
	cFlags.Attribute = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);
	BOOL ok;

	// Obtain a pointer to the attribute which we will need to apply to all selected nodes 
	NodeAttribute* NewAttr = ((ReplaceAttributesParam*)pOpParam)->m_pAttr;

	m_pAttr = NewAttr;

	List* OldAttrs = ((ReplaceAttributesParam*)pOpParam)->m_pOldAttrs;

	ListItem* pAttr;

	// Get a description of the attribute being applied so that we can use it to create the 
	// undo string. 
	UndoAttribStrID = NewAttr->GetAttrNameID();

	if (!DoStartSelOp(FALSE,FALSE, TRUE,TRUE))  // Try to record the selection state , don't
									 // render the blobs though 
	{
		goto EndOperation;  
	}

	// Ask all parents of all attrs in the list if it's ok to do the op
	ok = TRUE;
	pAttr = OldAttrs->GetHead();
	while (pAttr != NULL && ok)
	{
		Node* pParent = ((NodeAttributePtrItem*)pAttr)->NodeAttribPtr->FindParent();
ObjChange.SetRetainCachedData(((NodeAttributePtrItem*)pAttr)->NodeAttribPtr->IsEffectAttribute());
		ok = (pParent != NULL) && pParent->AllowOp(&ObjChange);
		pAttr = OldAttrs->GetNext(pAttr);
	}
	if (!ok) 
	{
		FailAndExecute();
		goto EndOperation;
	}

	pAttr = OldAttrs->GetHead();
	while (pAttr != NULL)
	{
		DoReplace(((NodeAttributePtrItem*)pAttr)->NodeAttribPtr, NewAttr);

		pAttr = OldAttrs->GetNext(pAttr);
	}

	// If the user has specified that the Last Attribute applied should
	// become current, then we need to update the Current Attribute now.
	if (AttributeManager::LastAttrAppliedBecomesCurrent)
	{
		NodeAttribute* NewCurrent;
		NodeAttribute* LastAttr;
	
		if (AttributeManager::pLastNodeAppliedTo != NULL)
		{
			LastAttr = AttributeManager::pLastNodeAppliedTo->FindAppliedAttribute(NewAttr->GetAttributeType());
		
			if (LastAttr != NULL)
			{
				ALLOC_WITH_FAIL(NewCurrent ,((NodeAttribute*)LastAttr->SimpleCopy()), this)

				if (NewCurrent->IsAFillAttr())
				{
					DocRect Bounds = AttributeManager::pLastNodeAppliedTo->GetBoundingRect();
					((AttrFillGeometry*)NewCurrent)->SetBoundingRect(Bounds);
				}

				if (LastAttr->IsAFractalFill())
				{
					// The current attr, should always use the default DPI for fractals.
				 	((AttrFillGeometry*)NewCurrent)->SetFractalDPI(AttrFillGeometry::FractalDPI);
				}

				// Find the Attr Groups that need updating
				List AttrGroups;
				if (AttributeManager::CanBeAppliedToNode(AttributeManager::pLastNodeAppliedTo,
													 		LastAttr, &AttrGroups))
 				{
					AttributeManager::UpdateCurrentAttr(NewCurrent, FALSE, &AttrGroups);
				}

				AttrGroups.DeleteAll();
				delete NewCurrent;
			}
		}
	}

	AttrFillGeometry::LastRenderedMesh = NULL;

	if (Document::GetSelected())
		Document::GetSelected()->SetModified(TRUE);

	// Update all parents of this
	ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
ObjChange.SetRetainCachedData(TRUE);	// Tell TextStory not to invalidate everything in OnChildChange
	if (!UpdateChangedNodes(&ObjChange))
		FailAndExecute();

	EndOperation:			
	End(); // End of operation
} 

/********************************************************************************************

>	BOOL OpReplaceAttributes::DoReplace(NodeAttribute* OldAttr, NodeAttribute* NewAttr)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 


	Purpose:	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


BOOL OpReplaceAttributes::DoReplace(NodeAttribute* OldAttr, NodeAttribute* NewAttr)
{
	Node* CurrentNode = OldAttr->FindParent();
	ERROR3IF(CurrentNode == NULL, "Can't find parent node in Replace Attr");

	BOOL bOldWasEffect = OldAttr->IsEffectAttribute();

	if (!DoInvalidateNodeRegion((NodeRenderableBounded*)CurrentNode, FALSE, TRUE, FALSE, FALSE))	// Do not recache
		return FALSE; 

// >>>> Temporary BODGE to aid select-inside...
// >>>> ALWAYS clear any attributes of the same type from the subtree (even if the
// >>>>	attribute just applied replaced an existing one) so that dubious
// >>>> attribute states (due to this routine not dealing with Select-inside
// >>>> properly yet) can be cleared.
// >>>> NOTE! the current att (n) is passed in to DoRemoveAttTypeFromSubtree so
// >>>> that it won't be deleted along with atts of the same type - that would be
// >>>> (has been) disastrous!
	if (CurrentNode->IsCompoundClass() && !((NodeCompound*)CurrentNode)->IsValidEffectAttr(NewAttr))
//			if ((!AttributeExists) && (CurrentNode->IsCompound()))
	{
		// Remove all instances of attributes of the same type from the subtree.
		// This is not neccessary if the AttributeExists flag is TRUE because
		// we know in this situation that the subtree cannot contain any other
		// instances of the attribute !.
		if (!DoRemoveAttrTypeFromSubtree(CurrentNode, NewAttr->GetAttributeType(), OldAttr))
		{
			return FALSE; 
		}  			
	}

	NodeAttribute* AttribClone = NULL;

	// Have we have got an Attribute to do something with ?
	if (OldAttr != NULL)
	{
		// We're gunna just replace the attribute with the new one.

		// First make a copy of the new attribute.
      	ALLOC_WITH_FAIL(AttribClone ,((NodeAttribute*)NewAttr->SimpleCopy()), this)

		if (AttribClone == NULL)
		{
			// We have failed to create the clone 
			return FALSE; 
		}

		// Complication !!
		// If we are replacing a Fill Attribute and the attribute we are replacing
		// is already filled, then we need to extract the colour of the existing
		// fill and use them for the new fill.
		if ( OldAttr->IsAFillAttr() )
		{
        	AttrFillGeometry* NodeReplaced = (AttrFillGeometry*)OldAttr;

			// Are these both Graduated Fills ?
			if ( (NodeReplaced->GetAttributeType() == CC_RUNTIME_CLASS(AttrFillGeometry)) ||
				 (NodeReplaced->GetAttributeType() == CC_RUNTIME_CLASS(AttrTranspFillGeometry)) )
			{
				// Copy the old colours into the new Fill
			
				// NOTE:
				// This should really be done with a virtual function

				if (NodeReplaced->IsAFlatFill())
				{
					((AttrFillGeometry*)AttribClone)->SetEndColour(NodeReplaced->GetStartColour());
			    	((AttrFillGeometry*)AttribClone)->SetEndTransp(NodeReplaced->GetStartTransp());
				}
				else
				{
			    	((AttrFillGeometry*)AttribClone)->SetStartColour(NodeReplaced->GetStartColour());
					((AttrFillGeometry*)AttribClone)->SetEndColour(NodeReplaced->GetEndColour());
			    	((AttrFillGeometry*)AttribClone)->SetStartTransp(NodeReplaced->GetStartTransp());
					((AttrFillGeometry*)AttribClone)->SetEndTransp(NodeReplaced->GetEndTransp());
				}
				
		    	((AttrFillGeometry*)AttribClone)->SetTranspType(NodeReplaced->GetTranspType());
				((AttrFillGeometry*)AttribClone)->SetTesselation(NodeReplaced->GetTesselation());
				((AttrFillGeometry*)AttribClone)->SetSeed(NodeReplaced->GetSeed());
				((AttrFillGeometry*)AttribClone)->SetGraininess(NodeReplaced->GetGraininess());
				((AttrFillGeometry*)AttribClone)->SetGravity(NodeReplaced->GetGravity());
				((AttrFillGeometry*)AttribClone)->SetSquash(NodeReplaced->GetSquash());
				((AttrFillGeometry*)AttribClone)->SetTileable(NodeReplaced->GetTileable());
				((AttrFillGeometry*)AttribClone)->AttachBitmap(NodeReplaced->GetBitmap());
				((AttrFillGeometry*)AttribClone)->RecalcFractal();

			}

			if ( NewAttr->IsATranspFill() && NewAttr->IsAFlatFill())
			{
				((AttrFillGeometry*)OldAttr)->RenderFillBlobs();
			}
		}

		// Now we have done with the old attribute, so lets hide it, so
		// the changes can be undone
		if(!DoHideNode(OldAttr, 
					   TRUE			// Include the subtree size 
					   ))
			{
				return FALSE; 
			} 
	}
		
	// Finally !! We can add the new attribute node into the tree.
	if (bOldWasEffect)
		AttribClone->AttachNode(CurrentNode, LASTCHILD);
	else
		AttribClone->AttachNode(CurrentNode, FIRSTCHILD);

	AttributeManager::pLastNodeAppliedTo = (NodeRenderableInk*)CurrentNode;

	// And now it's in the tree, we need to make sure that any fill control
	// points are valid.
	if (AttribClone->IsAFillAttr())
	{
    	((AttrFillGeometry*)AttribClone)->AttributeChanged();
	}

	HideNodeAction* UndoHideNodeAction; 

   	// Create an action to hide the attribute when we undo 
	if ( HideNodeAction::Init(this, 
		 					  &UndoActions, 
							  AttribClone, 
							  TRUE,				// When the attribute gets hidden we
							  					// must include its size 
		 					 (Action**)(&UndoHideNodeAction))
		 == AC_FAIL)  
	{   
		AttribClone->CascadeDelete();       
		delete (AttribClone); 
		return FALSE; 
	}

	ReleaseCachedForAttrApply((NodeRenderableBounded*)CurrentNode, bOldWasEffect);

	if (!DoInvalidateNodeRegion((NodeRenderableBounded*)CurrentNode, FALSE, TRUE, FALSE, FALSE))	// Do not recache
		return FALSE;

	return TRUE;
}

/********************************************************************************************

>	BOOL OpReplaceAttributes::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpReplaceAttributes initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpReplaceAttributes::Init()
{
	
	// Register the opdescriptors for the OpReplaceAttributes operation
	OpDescriptor* OpDesc = new OpDescriptor(
								 			0,
								 			_R(IDS_REPLACEATTRSOP), 				
											CC_RUNTIME_CLASS(OpReplaceAttributes),
								 			OPTOKEN_REPLACEATTRS,
								 			OpReplaceAttributes::GetState);	


	if (OpDesc != NULL)
	{
		// Bodge this needs to use faby Macro technology so that the op is attached to
		// a system bar.
		OpDesc = new OpChangeLineWidthOpDesc(0,
											 _R(IDS_CHANGELINEWIDTH),
											 CC_RUNTIME_CLASS(OpReplaceAttributes),
											 OPTOKEN_CHANGELINEWIDTH,
											 OpChangeLineWidthOpDesc::GetState,
											 0,
											 _R(IDBBL_CHANGELINEWIDTH));
		 
	}



	
	ERRORIF(!OpDesc, _R(IDE_NOMORE_MEMORY), FALSE);
	return(OpDesc != NULL);
}               

/********************************************************************************************

>	OpState	OpReplaceAttributes::GetState(String_256*, OpDescriptor* OpDesc)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/1/95
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpReplaceAttributes operation
	Purpose:	For finding OpReplaceAttributes state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpReplaceAttributes::GetState(String_256*, OpDescriptor* OpDesc)
{
	OpState OpSt;

	OpSt.Greyed = ( FALSE );  
												
	return(OpSt);   
}


/********************************************************************************************

>	virtual void OpReplaceAttributes::GetOpName(String_256* OpName) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/1/95
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
					
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpReplaceAttributes::GetOpName(String_256* OpName) 
{ 
	
	*OpName = String_256(UndoAttribStrID); 
	*OpName += String_256(_R(IDS_CHANGE)); 
}  


