// $Id: nodeblnd.cpp 662 2006-03-14 21:31:49Z alex $
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
#include "nodeblnd.h"
//#include "markn.h"
//#include "mario.h"
#include "becomea.h"
#include "undoop.h"
#include "document.h"
#include "ophist.h"
//#include "rndrgn.h"
#include "aw_eps.h"
//#include "cameleps.h"
#include "objchge.h"
#include "nodebldr.h"
#include "app.h"
#include "blobs.h"
//#include "contmenu.h"
#include "blndtool.h"
#include "tool.h"
#include "ndbldpth.h"
#include "attrmap.h"

#include "camfiltr.h"
#include "cxftags.h"
#include "cxfrec.h"
#include "rechblnd.h"
#include "nodershp.h"
//#include "opshadow.h"
//#include "opbevel.h"
#include "extender.h"
//#include "ngcore.h"		// NameGallery, for stretching functionality
#include "brshattr.h"
#include "slicehelper.h"

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNAMIC(NodeBlend,NodeGroup)
CC_IMPLEMENT_DYNCREATE(InitBlendAction,Action)
CC_IMPLEMENT_DYNAMIC(BlendRecordHandler, CamelotRecordHandler)

#define new CAM_DEBUG_NEW

#define NUM_DEFAULT_BLENDSTEPS 5

BOOL NodeBlend::s_DefaultNotAntialiased = FALSE;
NodeBlender* BlendRecordHandler::m_pLastInsertedNodeBlender = NULL;
NodeBlend* BlendRecordHandler::m_pLastInsertedNodeBlend = NULL;
NodeBlendPath* BlendRecordHandler::m_pLastNodeBlendPath = NULL;
CProfileBiasGain BlendRecordHandler::m_LastObjectProfile;
CProfileBiasGain BlendRecordHandler::m_LastAttrProfile;
CProfileBiasGain BlendRecordHandler::m_LastPositionProfile;
BOOL BlendRecordHandler::m_bLoadedProfiles = FALSE;

/*********************************************************************************************

>    NodeBlend::NodeBlend() 

     Author:	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:	6/10/94
     Inputs:	
     Outputs:	
     Returns:	
     Purpose: 	This constructor creates a NodeBlend linked to no other, with all status
		  	  	flags false and an uninitialised bounding rectangle.           
			
     Errors:    

**********************************************************************************************/
 
NodeBlend::NodeBlend(): NodeGroup()
{
	ResetVars();
}
	
/***********************************************************************************************

>   void NodeBlend::NodeBlend
	(
		Node* ContextNode,  
		AttachNodeDirection Direction,  
		BOOL Locked = FALSE, 
		BOOL Mangled = FALSE,  
		BOOL Marked = FALSE, 
		BOOL Selected = FALSE, 
	)

    Author:  Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:	6/10/94
    Inputs: ContextNode: Pointer to a node which this node is to be attached to.     
    
			Direction: 
			
				Specifies the direction in which this node is to be attached to the 
				ContextNode. The values this variable can take are as follows: 
								  
				PREV      : Attach node as a previous sibling of the context node
				NEXT      : Attach node as a next sibling of the context node
				FIRSTCHILD: Attach node as the first child of the context node
				LASTCHILD : Attach node as a last child of the context node                               
					      
			The remaining inputs specify the status of the node: 
			
			Locked:     Is node locked ?
			Mangled:    Is node mangled ?
			Marked:     Is node marked ?
			Selected:   Is node selected ?
			
    Outputs:   -
    Returns:   - 
    Purpose: This method initialises the node and links it to ContextNode in the
			 direction specified by Direction. All necessary tree links are
			 updated.     
			 
    Errors:  An assertion error will occur if ContextNode is NULL


***********************************************************************************************/

NodeBlend::NodeBlend(Node* ContextNode,  
					 AttachNodeDirection Direction,  
					 BOOL Locked, 
				 	 BOOL Mangled,  
					 BOOL Marked, 
					 BOOL Selected   
			   ):NodeGroup(ContextNode, Direction, Locked, Mangled, Marked, 
			 	Selected) 
{ 
	ResetVars();
} 

/*********************************************************************************************

>    void NodeBlend::ResetVars() 

     Author:	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:	6/10/94
     Inputs:	
     Outputs:	
     Returns:	
     Purpose: 	This sets all the member vars to the state they should be in on construction.
	 			All constructors should call this func.
     Errors:    

**********************************************************************************************/
 
void NodeBlend::ResetVars()
{
	m_NumBlendSteps			= NUM_DEFAULT_BLENDSTEPS;
	m_OneToOne				= FALSE;
	m_NotAntialiased		= s_DefaultNotAntialiased;
	m_ColBlendType			= COLOURBLEND_FADE;
	m_Tangential			= FALSE;
	m_BlendedOnCurve		= FALSE;
	m_Edit					= EDIT_STEPS;
	m_DistanceEntered		= -1;
	m_LastEdited			= NONE;
	m_NumNodeBlendPaths		= 0;
	objectProfileProcessing = FALSE;
	UpdateStepDistance();
}

/********************************************************************************************

>	virtual String NodeBlend::Describe(BOOL Plural, BOOL Verbose = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Inputs:		Plural: Flag indicating if the string description should be plural or
						singular. 
	Outputs:	-
	Retuns:		Description of the blend node 
	Purpose:	To return a description of the Blend object in either the singular or the 
				plural. This method is called by the DescribeRange method.
				
				The description will always begin with a lower case letter.   
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

String NodeBlend::Describe(BOOL Plural, BOOL Verbose) 
{     
	if (Plural)
		return(String(_R(IDS_BLEND_DESCRP)));  
	else
		return(String(_R(IDS_BLEND_DESCRS))); 
}; 


/***********************************************************************************************

> Node* NodeBlend::SimpleCopy()  

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:	6/10/94
	Inputs:     -  
    Outputs:    -
    Returns:    A copy of the node, or NULL if memory has run out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	            
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL.                                                                      
                                                                                 
**********************************************************************************************/

Node* NodeBlend::SimpleCopy()
{
	NodeBlend* pCopyOfNode = new NodeBlend();
	ERROR1IF(pCopyOfNode == NULL,NULL,_R(IDE_NOMORE_MEMORY)); 
	CopyNodeContents(pCopyOfNode);
	return (pCopyOfNode);
}   

/***********************************************************************************************

>	void NodeBlend::CopyNodeContents(Node* pCopyOfNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		pCopyOfNode - The node to copy data to
	Outputs:	-
	Returns:	-
	Purpose:	Copies the data from this node to pCopyOfNode by first calling the base class to get it to
				copy its stuff, and then copying its own stuff
	Scope:		protected
	SeeAlso:	NodeGroup::CopyNodeContents

***********************************************************************************************/

void NodeBlend::CopyNodeContents(NodeBlend* pCopyOfNode)
{
	NodeGroup::CopyNodeContents(pCopyOfNode);

	// Copy member vars here
	pCopyOfNode->m_NumBlendSteps	= m_NumBlendSteps;
	pCopyOfNode->m_OneToOne			= m_OneToOne;
	pCopyOfNode->m_NotAntialiased	= m_NotAntialiased;
	pCopyOfNode->m_ColBlendType		= m_ColBlendType;
	pCopyOfNode->m_Tangential		= m_Tangential;
	pCopyOfNode->m_BlendedOnCurve	= m_BlendedOnCurve;
	pCopyOfNode->m_Edit				= m_Edit;
	pCopyOfNode->m_DistanceEntered  = m_DistanceEntered;
	pCopyOfNode->m_NumNodeBlendPaths = m_NumNodeBlendPaths;
	pCopyOfNode->m_ObjectProfile     = m_ObjectProfile;
	pCopyOfNode->m_AttrProfile       = m_AttrProfile;
	pCopyOfNode->m_PositionProfile	 = m_PositionProfile;
}
   


/***********************************************************************************************
>   void NodeBlend::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeBlend::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeBlend), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeBlend))
		CopyNodeContents((NodeBlend*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 NodeBlend::GetNodeSize() const

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 NodeBlend::GetNodeSize() const 
{     
	return (sizeof(NodeBlend)); 
}  


/********************************************************************************************

>	BOOL NodeBlend::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						     Spread* pSpread )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/94
	Inputs:		PointerPos 	- The Location of the mouse pointer at the time of the click
				Click 		- The type of click received (single, double, drag etc)
				ClickMods 	- The modifiers to the click (eg shift, control etc )
	Returns:	TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Does nothing at the moment - Just returns FALSE.
		
********************************************************************************************/

BOOL NodeBlend::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )
{
	// we did not use the click, so let someone else try
	return FALSE;
}

/***********************************************************************************************

>	BOOL NodeBlend::HidingNode()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/94
	Inputs:		-
	Outputs:	-
	Purpose:	Called whenever the node gets hidden.
				It calls the Deinit() member function

***********************************************************************************************/

BOOL NodeBlend::HidingNode()
{
	// Call the base class first
	if (!NodeGroup::HidingNode())
		return FALSE;

	BOOL ok = TRUE;
	Node* pNode = FindFirstChild();
	while (pNode != NULL && ok)
	{
		ok = pNode->HidingNode();
		pNode = pNode->FindNext();
	}

	return ok;
}

/***********************************************************************************************

>	BOOL NodeBlend::ShowingNode()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/94
	Inputs:		-
	Outputs:	-
	Purpose:	Called whenever the node gets shown after it's been hidden.
				It calls the Reinit() member function

***********************************************************************************************/

BOOL NodeBlend::ShowingNode()
{
	// Call the base class first
	if (!NodeGroup::ShowingNode())
		return FALSE;

// This doesn't need to be done!!!
// The caller of ShowingNode recurses through the subtree by itself
//	BOOL ok = TRUE;
//	Node* pNode = FindFirstChild();
//	while (pNode != NULL && ok)
//	{
//		ok = pNode->ShowingNode();
//		pNode = pNode->FindNext();
//	}
//
//	return ok;
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL NodeBlend::DoBecomeA(BecomeA* pBecomeA)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/94
	Inputs:		pBecomeA = 	ptr to a class that contains all the info needed to become a new
							type of node.
	Outputs:	-
	Returns:	TRUE if the object has been transformed, FALSE if we run out of memory

	Purpose:	Transforms the object into another type of object. 
				This converts all its children, and replaces itself in the tree with a NodeGroup
	Errors:		-
	SeeAlso:	Node::CanBecomeA

********************************************************************************************/

BOOL NodeBlend::DoBecomeA(BecomeA* pBecomeA)
{
	if (!Reinit()) return FALSE;

	switch (pBecomeA->GetReason())
	{
		case BECOMEA_REPLACE:
		{
//			ERROR2IF(pBecomeA->GetUndoOp() == NULL,FALSE,"Trying to replace a NodeBlend, but pUndoOp == NULL");
			UndoableOperation* pUndoOp = pBecomeA->GetUndoOp();

			// Make all attrs children of the child nodes
			if (pUndoOp)
			{
				if (!pUndoOp->DoLocaliseCommonAttributes(this))
					return FALSE;
			}
			else
			{
				if (!LocaliseCommonAttributes())
					return FALSE;
			}

			// If being repaced in the tree, deselect the node
			BOOL IsSelected = this->IsSelected();
			SetSelected(FALSE);

			// When becoming a new type of node AND replacing ourselves in the tree, we must first ask all the 
			// blender child nodes, then the rest of the children, in that order.
			// This is because the blender nodes generate new nodes from other children in the tree, so
			// they have to do their stuff before the other nodes get replaced and hence disappear.

			// We need a list of all non-blender child nodes
			std::list<Node *>	ListOfNodes; //(30);

			// Scan the children of the blend
			Node* pNode = FindFirstChild();
			while (pNode != NULL)
			{
				Node* pThisNode = pNode;
				pNode = pNode->FindNext();

				// If this node is a blender, call its DoBecomeA() method
				// otherwise add the node to the list of non-blender nodes to use later

				if (IS_A(pThisNode,NodeBlender))
					pThisNode->DoBecomeA(pBecomeA);
				else
					ListOfNodes.push_back(pThisNode);
			}

			while( !ListOfNodes.empty() )
			{	
				// Here we must ask all the non-blender nodes to DoBecomeA()
				pNode = ListOfNodes.back(); ListOfNodes.pop_back();
				
				// CGS:  prevent NodeShadowControllers, NodeBevelControllers and NodeCountourControllers from
				// converting to paths when inside of a blend ....

				if (!pNode->IsController ())
				{
					pNode->DoBecomeA(pBecomeA);
				}
				else
				{
					// BUT if we need them converted to paths, then do it ....
					if (pBecomeA->GetInsertComplexBlendStepsAsPaths ())
					{
						pNode->DoBecomeA(pBecomeA);
					}
				}
			}

			// All the children have been delt with, so now's the time to deal with this blend node

			// When replacing the blend with shapes, hide the blend, create a group, and move all the children
			// so they become children of the new group node

			// Allocate a new NodeGroup node
			NodeGroup* pNodeGroup;
			ALLOC_WITH_FAIL(pNodeGroup, (new NodeGroup), pUndoOp); 
			if (pNodeGroup == NULL)
				return FALSE;

			// Insert the NodeGroup where the NodeBlend used to be
			if (pUndoOp)
			{
				if (!pUndoOp->DoInsertNewNode(pNodeGroup,this,NEXT,FALSE,FALSE,FALSE,FALSE))
					return FALSE;
			}
			else
			{
				pNodeGroup->AttachNode(this, NEXT);
			}

			// Select the group, but only if the caller wants us too (moulds don't, for example)
			if (IsSelected && pUndoOp)
			{
				if (!pUndoOp->DoSelectNode(pNodeGroup,FALSE))
					return FALSE;
			}

			// Karim 15/05/2000 - rewrote to handle non-optimising attrs.
			// Move our children into the new group node.
			//
			// NOTE: The IsEffectAttribute function won't work when called from ShouldBeOptimized below
			// because it looks along the tree for bounded nodes and this function will have removed them
			// all by the time that call is made.
			// So bInEffectArea duplicates that logic by being set as soon as any bounded object
			// is met as we traverse the child list. Not nice...
			//
			BOOL ok = TRUE;
			BOOL bInEffectArea = FALSE;
			Node* pNextKid	= NULL;
			Node* pThisKid	= FindFirstChild();
			while (ok && pThisKid != NULL)
			{
				pNextKid = pThisKid->FindNext();

				if (pThisKid->IsBounded())
					bInEffectArea = TRUE;

				// ink nodes just get moved.
				if (pThisKid->IsAnObject())
				{
					if (pUndoOp)
					{
						CALL_WITH_FAIL(pUndoOp->DoMoveNode(pThisKid, pNodeGroup, LASTCHILD), pUndoOp, ok)
					}
					else
					{
						pThisKid->MoveNode(pNodeGroup, LASTCHILD);
					}
				}
				// non-optimising attrs just get copied, as DoMoveNode() doesn't like them.
				else
					if (pThisKid->IsAnAttribute() &&
						(!((NodeAttribute*)pThisKid)->ShouldBeOptimized() ||
						bInEffectArea)
						)
					CALL_WITH_FAIL(pThisKid->CopyNode(pNodeGroup, LASTCHILD), pUndoOp, ok)

				pThisKid = pNextKid;
			}
			if (!ok)
				return FALSE;

			// Hide the blend node
			NodeHidden* pNodeHidden; 
			if (pUndoOp)
			{
				if(!pUndoOp->DoHideNode(this, FALSE, &pNodeHidden, FALSE))
					return FALSE;                
			}
			else
			{
				CascadeDelete();
				delete this;
			}

			// Factor out the attrs (needed after a call to DoLocaliseCommonAttributes
			if (pUndoOp)
			{
				if (!pUndoOp->DoFactorOutCommonChildAttributes(pNodeGroup))
					return FALSE;
			}
			else
			{
				if (!pNodeGroup->FactorOutCommonChildAttributes())
					return FALSE;
			}
		}
		break;

		case BECOMEA_PASSBACK:
		// the following case is executed when doing the blend of two grouped blends
		{
			// Sequentially ask the children of the blend to DoBecomeA
			// This is all that's required because the child objects are only passing back
			// the new node type, and NOT replacing themselves in the tree
			// This also ensures the receipient gets the list of paths in render order
			Node* pNode = FindFirstChild();
			while (pNode != NULL)
			{
				if (!pNode->DoBecomeA(pBecomeA))
					return FALSE;

				pNode = pNode->FindNext();
			}
		}
		break;

		default:
			ERROR3_PF(("Unknown BecomeA reason %d",pBecomeA->GetReason()));
			break;
	}

	return TRUE;
}

/********************************************************************************************

>	void NodeBlend::SetNumBlendSteps(UINT32 NumSteps)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/94
	Inputs:		NumSteps = num steps to set in this blend
	Outputs:	-
	Returns:	-
	Purpose:	Sets the number of blend steps in this blend
	SeeAlso:	-

********************************************************************************************/

void NodeBlend::SetNumBlendSteps(UINT32 NumSteps)
{
	m_NumBlendSteps = NumSteps;
}

/********************************************************************************************

>	UINT32 NodeBlend::GetNumBlendSteps()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/94
	Inputs:		-
	Outputs:	-
	Returns:	Num blend steps in this blend
	Purpose:	Gets the number of blend steps in this blend
	SeeAlso:	-

********************************************************************************************/

UINT32 NodeBlend::GetNumBlendSteps()
{
	return m_NumBlendSteps;
}

/********************************************************************************************

>	void NodeBlend::SetColourBlendType(ColourBlendType Type)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/95
	Inputs:		Type = the way colours are blended (i.e. fade, rainbow, or alt rainbow)
	Outputs:	-
	Returns:	-
	Purpose:	Sets up the way this blend blends colours
	SeeAlso:	-

********************************************************************************************/

void NodeBlend::SetColourBlendType(ColourBlendType Type)
{
	m_ColBlendType = Type;
}

/********************************************************************************************

>	ColourBlendType NodeBlend::GetColourBlendType()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/95
	Inputs:		-
	Outputs:	-
	Returns:	The way the blend blends colours
	Purpose:	How exactly will this blend blend colours? Use this func to find out.
	SeeAlso:	-

********************************************************************************************/

ColourBlendType NodeBlend::GetColourBlendType()
{
	return m_ColBlendType;
}

/********************************************************************************************

>	BOOL NodeBlend::IsOneToOne()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if blend should be done using one-to-one mapping
	Purpose:	Get the state of the OneToOne flag for this blend
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlend::IsOneToOne()
{
	return m_OneToOne;
}

/********************************************************************************************

>	void NodeBlend::SetOneToOne(BOOL state)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/94
	Inputs:		state = TRUE if blend should be done using one-to-one
	Outputs:	-
	Returns:	-
	Purpose:	Set the state of the OneToOne flag for this blend
	SeeAlso:	-

********************************************************************************************/

void NodeBlend::SetOneToOne(BOOL state)
{
	m_OneToOne = state;
}

/********************************************************************************************

>	BOOL NodeBlend::IsNotAntialiased()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the blend steps should NOT be rendered anti-aliased
	Purpose:	Get the state of the NotAntialiased flag for this blend
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlend::IsNotAntialiased()
{
	return m_NotAntialiased;
}

/********************************************************************************************

>	void NodeBlend::SetNotAntialiased(BOOL state)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/94
	Inputs:		state = TRUE if the blend steps should NOT be rendered anti-aliased
	Outputs:	-
	Returns:	-
	Purpose:	Set the state of the NotAntialiased flag for this blend
	SeeAlso:	-

********************************************************************************************/

void NodeBlend::SetNotAntialiased(BOOL state)
{
	m_NotAntialiased = state;
	s_DefaultNotAntialiased = state;
}

/********************************************************************************************

>	BOOL NodeBlend::IsTangential()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/99
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if it blends on a curve tangentially
	Purpose:	Get the state of the m_Tangential flag for this blend
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlend::IsTangential()
{
	return m_Tangential;
}

/********************************************************************************************

>	void NodeBlend::SetTangential(BOOL state)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/94
	Inputs:		state = TRUE if it blends on a curve tangentially
	Outputs:	-
	Returns:	-
	Purpose:	Set the state of the m_Tangential flag for this blend
	SeeAlso:	-

********************************************************************************************/

void NodeBlend::SetTangential(BOOL state)
{
	m_Tangential = state;
}


/********************************************************************************************

>	BOOL NodeBlend::IsOnACurve()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/9/99
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if it blends on a curve 
	Purpose:	Get the state of the m_blendedoncurve flag
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlend::IsOnACurve()
{
	return m_BlendedOnCurve;
}


/********************************************************************************************

>	void NodeBlend::SetBlendedOncurve(BOOL state)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/9/99
	Inputs:		state = TRUE if it blends on a curve 
	Outputs:	-
	Returns:	-
	Purpose:	Set the state of the m_BlendedOnCurve flag for this blend
	SeeAlso:	-

********************************************************************************************/

void NodeBlend::SetBlendedOnCurve(BOOL state)
{
	m_BlendedOnCurve = state;
}


/********************************************************************************************

>	EditState NodeBlend::GetEditState()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/99
	Inputs:		-
	Outputs:	-
	Returns:	The current state of the m_Edit flag 
	Purpose:	lets us know whether to edit the number of steps, 
				or the distance between steps
	SeeAlso:	-

********************************************************************************************/

EditState NodeBlend::GetEditState()
{
	return m_Edit;

}



/********************************************************************************************

>	void NodeBlend::SetEditType(EditType TypeToEdit)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/99
	Inputs:		the type to edit (can be EDIT_DISTANCE or EDIT_STEPS)
	Outputs:	-
	Returns:	- 
	Purpose:	sets the type to edit according to which button
				is selected on the infobar.  Also initialises the 
				m_DistanceEntered member if it has not already been initialised.
				This is in case the user selected distance edit but does not enter a 
				value. In this case, if the user has not selected a previous value I 
				assume that they wish to keep the value generated.
	SeeAlso:	-

********************************************************************************************/

void NodeBlend::SetEditState(EditState State)
{
	m_Edit = State;
	if (m_DistanceEntered == -1.0)
	{
		// first update in case something has changed
		UpdateStepDistance();
		m_DistanceEntered = GetStepDistance();
	}
}


/********************************************************************************************

>	BOOL NodeBlend::IsChildOfGroup()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/99
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this is a child of a nodegroup, FALSE otherwise
	Purpose:	to determine whether this nodeblend is part of a nodegroup
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlend::IsChildOfGroup()
{
	Node* pParent = ((Node*)this)->FindParent(CC_RUNTIME_CLASS(NodeGroup));
	if (pParent == NULL)
	{
	//	ERROR3("This blend has no parent");
		return FALSE;
	}
	return TRUE;
	/*
	if (pParent->IS_KIND_OF(NodeBevelController) ||
		pParent->IS_KIND_OF(NodeContourController) ||
		pParent->IS_KIND_OF(NodeShadowController))
		return FALSE;

	else if (pParent->IS_KIND_OF(NodeGroup))
		return TRUE

	return FALSE;*/
}


/********************************************************************************************

>	BOOL NodeBlend::ContainsBrushedNode()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/99
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this blend contains an object with a brush attribute
	Purpose:	as above
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlend::ContainsBrushedNode()
{
	Node* pNode = FindFirstChild();
	NodeAttribute* pAttr = NULL;
	while (pNode != NULL)
	{
		if (pNode->IsAnObject())
		{
			((NodeRenderableInk*)pNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBrushType), &pAttr);
			if (pAttr != NULL)
			{
				AttrBrushType* pAttrBrush = (AttrBrushType*)pAttr;
				if (pAttrBrush->GetBrushHandle() != BrushHandle_NoBrush)
					return TRUE;
			}
		}
		pNode = pNode->FindNext();
	}		
	return FALSE;
}

/********************************************************************************************

>	UINT32 NodeBlend::GetNumBlenders()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		-
	Outputs:	-
	Returns:	Num child NodeBlenders for this blend
	Purpose:	This counts the number of child nodes that are NodeBlenders
	SeeAlso:	-

********************************************************************************************/

UINT32 NodeBlend::GetNumBlenders()
{
	UINT32 Count = 0;
	Node* pNode = FindFirstChild();
	while (pNode != NULL)
	{
		if (IS_A(pNode,NodeBlender)) Count++;
		pNode = pNode->FindNext();
	}

	return Count;
}

/********************************************************************************************

>	BOOL NodeBlend::Reinit(BOOL ProgressBar)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Inputs:		ProgressBar = if TRUE, a progress bar will be displayed
	Outputs:	-
	Returns:	TRUE if all child blenders reinit OK
	Purpose:	This calls Reinit() on all child blenders.
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlend::Reinit(BOOL ProgressBar)
{
	Node* pNode = FindFirstChild();
	while (pNode != NULL)
	{
		if (IS_A(pNode,NodeBlender))
		{
			if (!((NodeBlender*)pNode)->Reinit(NULL,NULL,ProgressBar))
				return FALSE;
		}
		pNode = pNode->FindNext();
	}

	return TRUE;
}

/********************************************************************************************

>	void NodeBlend::Deinit(BOOL bNodesMayBeChanged = FALSE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This calls Deinit() on all child blenders.
	SeeAlso:	-

********************************************************************************************/

void NodeBlend::Deinit(BOOL bNodesMayBeChanged)
{
	Node* pNode = FindFirstChild();
	while (pNode != NULL)
	{
		if (IS_A(pNode,NodeBlender))
			((NodeBlender*)pNode)->Deinit(bNodesMayBeChanged);

		pNode = pNode->FindNext();
	}
}

/********************************************************************************************

>	BOOL NodeBlend::PostImport()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This is called after the entire doc has been imported
				It first deinits the blend (in case they are initialised currently), then
				reinits the blend, returning the result.
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlend::PostImport()
{
	Deinit();
	InvalidateBlendBoundingRect();
	return Reinit(FALSE);
}

/********************************************************************************************

>	void NodeBlend::InvalidateBlendBoundingRect()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This calls InvalidateBoundingRect() on all bounded children of this blend.
	SeeAlso:	-

********************************************************************************************/

void NodeBlend::InvalidateBlendBoundingRect()
{
	Node* pNode = FindFirstChild();
	while (pNode != NULL)
	{
		if (pNode->IsBounded())
			((NodeRenderableBounded*)pNode)->InvalidateBoundingRect();

		pNode = pNode->FindNext();
	}
}

/********************************************************************************************

>	DocRect NodeBlend::GetBlobBoundingRect()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/3/95
	Returns:	DocRect - The bounding rect of the node and its blobs
	Purpose:	This calls the base classes GetBlobBoundingRect(), and inflates the result by the width
				of a blob

********************************************************************************************/

DocRect NodeBlend::GetBlobBoundingRect()
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Find the base class blob bounding rectangle
	DocRect Rect = NodeGroup::GetBlobBoundingRect();

	// inflate it by the width of a blob (plus a bit)
	DocRect TempRect;
	GetApplication()->GetBlobManager()->GetBlobRect(Rect.lo,&TempRect);
	INT32 Delta = ((TempRect.hi.x - TempRect.lo.x)*1)/1;
	Rect.Inflate(Delta);

	return Rect;
#else
	return DocRect(0,0,0,0);
#endif
}

/********************************************************************************************

>	DocRect NodeBlender::GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		DontUseAttrs - TRUE if we should ignore the nodes attributes.
				Defaults to FALSE
				HitTest      - TRUE if being called during HitTest
	Returns:	The nodes bounding rect
	Purpose:	if the bounding rect is valid it is returned, if not, it is recalculated
				and then returned.
	SeeAlso:	NodeBlender::GetBlobBoundingRect

********************************************************************************************/

DocRect	NodeBlend::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	if (IsBoundingRectValid && !DontUseAttrs)
		return BoundingRectangle;

	// NOTE! NodeGroup:;GetBoundingRect doesn't set BoundingRectangle if DontUseAttrs is TRUE
	// It returns a computed value instead.
	// So this routine must use the return value from calling NodeGroup::GetBoundingRectangle
	// to return a correct value.
	return NodeGroup::GetBoundingRect(DontUseAttrs, HitTest);
}

/********************************************************************************************

>	void NodeBlend::RenderObjectBlobs(RenderRegion* pRender)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/3/95
	Inputs:		pRender - The region to draw the blobs in
	Purpose:	Renders the object blobs

********************************************************************************************/

void NodeBlend::RenderObjectBlobs(RenderRegion* pRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Find out about the groups bounding rect
	DocRect BoundingRect = GetBoundingRect();

	// Inflate the bounds by the width of a blob
	DocRect TempRect;
	GetApplication()->GetBlobManager()->GetBlobRect(BoundingRect.lo,&TempRect);
	INT32 Delta = ((TempRect.hi.x - TempRect.lo.x)*3)/4;
	BoundingRect.Inflate(Delta);

	// Find out where to draw the blobs
	DocCoord Low  = BoundingRect.LowCorner();
	DocCoord High = BoundingRect.HighCorner();

	// Set the colours of the blobs
	pRegion->SetFillColour(COLOUR_UNSELECTEDBLOB);
	pRegion->SetLineColour(COLOUR_NONE);

	// Draw all the blobs
	pRegion->DrawBlob(Low, BT_UNSELECTED);  
	pRegion->DrawBlob(High, BT_UNSELECTED); 
	pRegion->DrawBlob(DocCoord(Low.x, High.y), BT_UNSELECTED); 
	pRegion->DrawBlob(DocCoord(High.x, Low.y), BT_UNSELECTED); 

/*	NodeBlendPath* pNodeBlendPath = GetNodeBlendPath();
	if (pNodeBlendPath)
		pNodeBlendPath->RenderObjectBlobs(pRegion); */
#endif
}


/********************************************************************************************

>	virtual void NodeBlend::RenderBlendBlobs(pRender)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/94
	Inputs:		pRender = render region to render into
	Outputs:	-
	Returns:	-
	Purpose:	This renders all the blobs associated with this blend node
				This node doesn't do any rendering itself. All it does is ask its child blender nodes
				to render their blend blobs
	SeeAlso:	-

********************************************************************************************/

void NodeBlend::RenderBlendBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// The problem we have with this routine is that two adjacent blenders will reference the same
	// object, hence if we ask all blenders to render blobs for both its objects, blobs will get
	// rendered twice, EORing themselves off again.

	// So, we ask all but the last blender to just render the start object, then get the last one
	// to render both.

	Node* pLast  = NULL;

	// Find the last blender
	Node* pNode = FindFirstChild();
	while (pNode != NULL)
	{
		if (IS_A(pNode,NodeBlender)) pLast = pNode;
		pNode = pNode->FindNext();
	}

	// Render the blender blobs
	pNode = FindFirstChild();

	// altered by Diccon 3/9/99 to render the blobs of a blend on a path differently
	// so that it is apparent that the position of the end objects can be altered.
	// probably deserves a separate function but for now..
	if (m_BlendedOnCurve == FALSE)
	{
		while (pNode != NULL)
		{
			if (IS_A(pNode,NodeBlender))
			{
				BOOL RenderStart = TRUE;
				BOOL RenderEnd   = FALSE;

				if (pNode == pLast)	 RenderEnd = TRUE;
				
				((NodeBlender*)pNode)->RenderBlendBlobs(pRender,RenderStart,RenderEnd);
			}
		pNode = pNode->FindNext();
		}
	}
    else
		RenderBlendOnCurveBlobs(pRender);
#endif
}


/********************************************************************************************

>	virtual void NodeBlend::RenderBlendOnCurveBlobs(RenderRegion* pRender)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/99
	Inputs:		pRender = render region to render into
	Outputs:	-
	Returns:	-
	Purpose:	This renders all the blobs associated with this blend node if it is 
				on a curve.  It needs to render blobs on the editable nodes, i.e. if the whole
				thing is on a curve then the first and last nodes are editable.  if a blend on curve
				is blended to a straight blend then only the end on the curve is editable.
	SeeAlso:	-

********************************************************************************************/

void NodeBlend::RenderBlendOnCurveBlobs(RenderRegion* pRegion)
{
	NodeBlender* pNodeBlender = FindFirstBlender();

	while (pNodeBlender != NULL)
	{
		NodeBlender* pNextBlender = FindNextBlender(pNodeBlender);
		if (pNodeBlender->IsBlendedOnCurve())
		{
			Node* pStart = pNodeBlender->GetNodeStart();
			Node* pEnd = pNodeBlender->GetNodeEnd();

			if (pStart == NULL || pEnd == NULL)
			{
				ERROR3("This blender has no start and end");
				return;
			}
			else
			{
				BOOL bFinish = FALSE;
				Node* pNodeToRender = pStart;
				DocCoord FirstPoint;
				while (bFinish == FALSE)
				{

					if (pNodeToRender->IsABaseTextClass())
					{
						pRegion->SetLineColour(COLOUR_BEZIERBLOB);
						pRegion->SetFillColour(COLOUR_BEZIERBLOB);
					}
					else
					{

						pRegion->SetLineColour(COLOUR_UNSELECTEDBLOB);
						pRegion->SetFillColour(COLOUR_UNSELECTEDBLOB);
					}	
					
					// Draw a blob at the centre point
					DocRect BlobSize;
					BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
					if (pBlobMgr != NULL)
					{
						DocRect BoundingRect = ((NodeRenderableBounded*)pNodeToRender)->GetBoundingRect();
						DocCoord Point = BoundingRect.Centre();
						
						// we may be blended on a circle in which case the start and end nodes will be in the same
						// place.  In this case only do the first one
						if (pNodeToRender == pStart)
							FirstPoint = Point;
						else
						{
							// in fact as it turns out they may not be exactly in the same place, but very close
							double Distance = Point.Distance((Coord)FirstPoint);
							if (Distance < 10)
								break;
						}
						pBlobMgr->GetBlobRect(Point, &BlobSize);

						if (pNodeToRender->IsABaseTextClass())
						{
							pRegion->DrawBlob(Point, BT_UNSELECTED);
						}
							else
						{
							pRegion->DrawLine(DocCoord(BlobSize.hi.x, BlobSize.hi.y), DocCoord(BlobSize.lo.x, BlobSize.lo.y));
							pRegion->DrawLine(DocCoord(BlobSize.lo.x, BlobSize.hi.y), DocCoord(BlobSize.hi.x, BlobSize.lo.y));
							pRegion->DrawPixel(DocCoord(BlobSize.hi.x, BlobSize.lo.y));
							pRegion->DrawPixel(DocCoord(BlobSize.lo.x, BlobSize.lo.y));
						}
					}	
					if (pNodeToRender == pStart && pNextBlender == NULL)
						pNodeToRender = pEnd;
					else
						bFinish = TRUE;
				}
			} // end while
		}  // End if on curve
		pNodeBlender = pNextBlender;
	}
	return;
}
	


	/*
	NodeBlender* pFirstBlender = NULL;
	NodeBlender* pLastBlender = NULL;

	BOOL ok = FindFirstAndLastBlenders(&pFirstBlender, &pLastBlender);

	if (!ok)
	{
		ERROR3("Couldn't get first and last blenders");
		return;
	}

	Node* pFirstNode = NULL;
	Node* pLastNode = NULL;

	ok = GetStartAndEndNodes(&pFirstNode, &pLastNode);
	if (!ok)
		{
		ERROR3("Couldn't get first and last nodes");
		return;
	}

	// aah, the poetry of C++
	NodeBlender* pBlenderToRender = pFirstBlender;
	Node* pNodeToRender = pFirstNode;
	if (pBlenderToRender == NULL || pNodeToRender == NULL)
		return;
	BOOL bFinish = FALSE;
	while (bFinish == FALSE)
	{
		if (pBlenderToRender->IsBlendedOnCurve())
		{
			// if it is a regular shape then that class can do it for us
			if (IS_A(pNodeToRender, NodeRegularShape))
				((NodeRegularShape*)pNodeToRender)->RenderObjectBlobs(pRegion);
			
			// if its a group or other type we need to do it by hand.  If it is text then we want
			// a red rectangle instead of a black cross
			else if (pNodeToRender->IsCompound())
			{
				if (pNodeToRender->IsABaseTextClass())
				{
					pRegion->SetLineColour(COLOUR_BEZIERBLOB);
					pRegion->SetFillColour(COLOUR_BEZIERBLOB);
				}
				else
				{
					pRegion->SetLineColour(COLOUR_UNSELECTEDBLOB);
					pRegion->SetFillColour(COLOUR_UNSELECTEDBLOB);
				}
					
				// Draw a blob at the centre point
				DocRect BlobSize;
				BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
				if (pBlobMgr != NULL)
				{
					DocRect BoundingRect = ((NodeRenderableBounded*)pNodeToRender)->GetBoundingRect();
					DocCoord Point = BoundingRect.Centre();
					pBlobMgr->GetBlobRect(Point, &BlobSize);

					if (pNodeToRender->IsABaseTextClass())
					{
						pRegion->DrawBlob(Point, BT_UNSELECTED);
					}
					else
					{
						pRegion->DrawLine(DocCoord(BlobSize.hix, BlobSize.hiy), DocCoord(BlobSize.lox, BlobSize.loy));
						pRegion->DrawLine(DocCoord(BlobSize.lox, BlobSize.hiy), DocCoord(BlobSize.hix, BlobSize.loy));
						pRegion->DrawPixel(DocCoord(BlobSize.hix, BlobSize.loy));
						pRegion->DrawPixel(DocCoord(BlobSize.lox, BlobSize.loy));
					}
				}
			}
		}
		if (pNodeToRender == pFirstNode)
				pNodeToRender = pLastNode;
			else
				bFinish = TRUE;
			
	}

}*/


/********************************************************************************************

> BOOL NodeBlend::IsPointOverBlob(	DocCoord* pPointerPos,
									BlendPath** ppBlendPath,
									INT32* pIndex,
									BOOL* pAStart,
									UINT32* pRemapRef)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/11/94
	Inputs:		pPointerPos = point to check against
				ppBlendPath= ptr to place to store blend path ptr if a match is found
				pIndex	   = ptr to place to store index if match is found
				pAStart	   = ptr to place to store bool indicating start or end object flag
				pRemapRef  = ptr to place remap ref that's passed into NodeBlend::Remap()
	Outputs:	*ppBlendPath & *pIndex & *pAStart & *pRemapRef updated if TRUE returned, undefined otherwise
	Returns:	TRUE if match  found, FALSE otherwise
	Purpose:	This asks all its child blenders if the point given lies on a selected blob.
				If a match is found, the ptr to the blend path and index to blob element is returned.
				Also, if found,*pAStart = TRUE if blob belongs to start node, FALSE if end node.
				Also, if found,*pRemapRef contains a value that should be passed into NodeBlend::Remap()

				Markn 26-5-99:	Modified so that if the function returns TRUE, *pPointerPos is updated
								so that it contains the coords of the blob it is over
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlend::IsPointOverBlob(DocCoord* pPointerPos,
								BlendPath** ppBlendPath,
								INT32* pIndex,
								BOOL* pAStart,
								UINT32* pRemapRef)
{
	if (pPointerPos == NULL)
		return FALSE;

#if !defined(EXCLUDE_FROM_RALPH)
	BOOL Found = FALSE;
	Node* pNode = FindFirstChild();
	while (pNode != NULL && !Found)
	{
		if (IS_A(pNode,NodeBlender))
		{
			Found = ((NodeBlender*)pNode)->IsPointOverBlob(pPointerPos,ppBlendPath,pIndex,pAStart);
			if (Found) *pRemapRef = pNode->GetTag();
		}
		pNode = pNode->FindNext();
	}

	return Found;
#else
	return FALSE;
#endif
}

/********************************************************************************************

> BOOL NodeBlend::Remap(UINT32 RemapRef,DocCoord PosStart,DocCoord PosEnd,
						DocCoord * pInvPosStart,DocCoord * pInvPosEnd)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/11/94
	Inputs:		RemapRef   	= special remap ref value allowing blend to identify blender to remap
				PosStart 	= Coord of element to remap start node to
				PosEnd   	= Coord of element to remap end node to
				pInvPosStart= ptr to place to put the coord to use to invert the start mapping
				pInvPosEnd	= ptr to place to put the coord to use to invert the end mapping
	Outputs:	-
	Returns:	TRUE if remapped, FALSE otherwise
	Purpose:	This will ask the child blender referenced by RemapRef to remap its blend paths with the given
				params.
				If the child blender manages to remap a pair of blend paths, then this will return
				TRUE.
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlend::Remap(UINT32 RemapRef,DocCoord PosStart,DocCoord PosEnd,DocCoord* pInvPosStart,DocCoord* pInvPosEnd)
{
	BOOL Mapped = FALSE;
	Node* pNode = FindFirstChild();
	while (pNode != NULL && !Mapped)
	{
		if (pNode->GetTag() == RemapRef && IS_A(pNode,NodeBlender))
			Mapped = ((NodeBlender*)pNode)->Remap(PosStart,PosEnd,pInvPosStart,pInvPosEnd);
		pNode = pNode->FindNext();
	}

	return Mapped;
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
// Export code...

/*********************************************************************************************

>    void NodeBlend::PreExportRender(RenderRegion* pRegion)

     Author:	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:	28/11/94
     Inputs:	pRegion = ptr to the export render region to export to
     Outputs:	
     Returns:	
     Purpose: 	Called before this node or any of its children have been rendered to the export region.
				This outputs the "start blend" command.
				Supports ArtWorks EPS and Camelot EPS
     Errors:    

**********************************************************************************************/
 
void NodeBlend::PreExportRender(RenderRegion* pRegion)
{
#ifdef DO_EXPORT
	if (IS_A(pRegion, CamelotEPSRenderRegion))
		// We ust want the paths in EPS.
		return;

	if (pRegion->IS_KIND_OF(ArtWorksEPSRenderRegion))
	{
		EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();

		if (pRegion->IS_KIND_OF(CamelotEPSRenderRegion))
		{
			// Output the Camelot format for blends
			pDC->OutputValue(INT32(0));				// Reserved INT32 1
			pDC->OutputValue(INT32(0));				// Reserved INT32 2
			pDC->OutputValue(INT32(0));				// Reserved INT32 3
			pDC->OutputValue(INT32(m_ColBlendType));	// Colour blend type (fade, rainbow or alt rainbow)
			pDC->OutputValue(INT32(m_OneToOne));		// The "blend using a one to one mapping" flag
			pDC->OutputValue(INT32(m_NotAntialiased));	// The "don't antialias blend steps" flag
			pDC->OutputValue(INT32(m_NumBlendSteps));	// Num blend steps
			pDC->OutputValue(INT32(1));				// Version (Important that this is the last param!!!)
			pDC->OutputToken("csbd");				// Camelot "start blend" token
			pDC->OutputNewLine();
		}
		else 
		{
			// Use a cached flag set up by the next call to IsArtWorksEPSCompatible()
			// The same function is called by every child blender, so we don't want to do this (possibly)
			// time consuming task more than once per blend.
			m_AWEPSCompatibleCache = FALSE;

			if (IsArtWorksEPSCompatible())
			{
				// Output the AW format for blends, if this blend is ArtWorks-compatible
				pDC->OutputValue(INT32(1));				// Version
				pDC->OutputValue(INT32(0));				// Expanded flag
				pDC->OutputValue(GetNumInkObjects());	// Num objects we are blending
				pDC->OutputToken("asbd");				// ArtWorks "start blend" token
				pDC->OutputNewLine();
			}
		}
	}
#endif
}

/*********************************************************************************************

>    BOOL NodeBlend::ExportRender(RenderRegion* pRegion)

     Author:	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:	28/11/94
     Inputs:	pRegion = ptr to the export render region to export to
     Outputs:	
     Returns:	TRUE if ok, FALSE if something went wrong
     Purpose: 	Called after this node and all of its children have been rendered to the export region.
				This outputs the "end blend" command.
				Supports ArtWorks EPS and Camelot EPS
     Errors:    

**********************************************************************************************/
 
BOOL NodeBlend::ExportRender(RenderRegion* pRegion) 
{
#ifdef DO_EXPORT
	if (IS_A(pRegion, CamelotEPSRenderRegion))
		// We just want the paths in EPS.
		return FALSE;

	if (pRegion->IS_KIND_OF(ArtWorksEPSRenderRegion))
	{
		EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();

		if (pRegion->IS_KIND_OF(CamelotEPSRenderRegion))
		{
			pDC->OutputToken("cebd");				// Camelot "end blend" token
			pDC->OutputNewLine();
		}
		else if (IsArtWorksEPSCompatible())
		{
			pDC->OutputToken("aebd");				// ArtWorks "end blend" token
			pDC->OutputNewLine();
		}
		// Tell caller we rendered ourselves ok
		return TRUE;
	}
#endif
	// Render this node in the normal way
	return FALSE;
}


/*********************************************************************************************

>    BOOL NodeBlend::IsArtWorksEPSCompatible()

     Author:	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:	28/11/94
     Inputs:	
     Outputs:	
     Returns:	TRUE if this blend is AW compatible
     Purpose: 	This asks all the child node blenders if they are AW compatible, and
	 			returns TRUE if ALL of them are AW compatible.
				If the m_AWEPSCompatibleCache flag is TRUE, the current setting of m_AWEPSCompatible
				is returned, else it is recalculated (setting m_AWEPSCompatibleCache to TRUE in the process)

				A NodeBlend is only AWEPS compatible if the colour blend type is FADE (i.e. not rainbow
				or alt rainbow)

				See NodeBlender::IsArtWorksEPSCompatible() for the other things that must be true for this
				blend to be AWEPS compatible
     Errors:    
	 SeeAlso:	NodeBlender::IsArtWorksEPSCompatible()

**********************************************************************************************/
 
BOOL NodeBlend::IsArtWorksEPSCompatible()
{
	if (!m_AWEPSCompatibleCache)
	{
		m_AWEPSCompatible = (m_ColBlendType == COLOURBLEND_FADE);
		Node* pNode = FindFirstChild();

		while (pNode != NULL && m_AWEPSCompatible)
		{
			if (IS_A(pNode,NodeBlender))
				m_AWEPSCompatible = ((NodeBlender*)pNode)->IsArtWorksEPSCompatible();

			pNode = pNode->FindNext();
		}

		m_AWEPSCompatibleCache = TRUE;
	}

	return m_AWEPSCompatible;
}

/*********************************************************************************************

>    INT32 NodeBlend::GetNumInkObjects()

     Author:	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:	28/11/94
     Inputs:	
     Outputs:	
     Returns:	Returns the number of child ink objects that are NOT blender nodes
     Purpose: 	Used by the AW EPS blend export code to store the number of shapes this blend is blending.
     Errors:    

**********************************************************************************************/
 
INT32 NodeBlend::GetNumInkObjects()
{
	INT32  Count = 0;
	Node* pNode = FindFirstChild();

	while (pNode != NULL)
	{
		if (!IS_A(pNode,NodeBlender) && pNode->IS_KIND_OF(NodeRenderableInk))
			Count++;

		pNode = pNode->FindNext();
	}

	return Count;
}
	
//----------------------------------------------------------------------------

/********************************************************************************************

> 	virtual BOOL NodeBlend::AllowOp(ObjChangeParam* pParam, BOOL SetOpPermissionState = TRUE,
															BOOL DoPreTriggerEdit = TRUE)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>; Karim 20/01/2000
	Created:	6/5/95
	Inputs:		pParam				 = describes the way an op wants to change the node
				SetOpPermissionState = if TRUE the Op permission state of this node will be set according to
										the outcome of the call
				DoPreTriggerEdit	 = if TRUE then calls NameGallery::PreTriggerEdit.
									   *Must* be TRUE if the calling Op may make any nodes
									   change their bounds, eg move, line width, cut.
									   Use TRUE if unsure.
Outputs:	-
	Returns:	TRUE means the node and all its parents are happy with this op, FALSE means don't do it
	Purpose:	In this instance, the func gives the blend a chance to stop an op from happening to one
				of its children.

				Current implementation specs that blends will only allow you to change the appearence of a blend's
				child (e.g. change colour,translate, edit path).

				This must be called *before* the op is performed.
	SeeAlso:	Node::AllowOp(),GetOpPermission(),SetOpPermission();

********************************************************************************************/

BOOL NodeBlend::AllowOp(ObjChangeParam* pParam, BOOL SetOpPermissionState,
												BOOL DoPreTriggerEdit)
{
	ERROR2IF(pParam==NULL,FALSE,"NodeBlend::AllowOp() - pParam==NULL");

	// clean out the calling-child ptr, so it doesn't get passed around unintentionally.
	pParam->SetCallingChild(NULL);

	// decide if we allow it ... (and if we'll still exist after the op)
	BOOL allowed=TRUE;
	ObjChangeFlags Flags=pParam->GetChangeFlags();
	if (pParam->GetDirection()==OBJCHANGE_CALLEDBYCHILD)
	{
		if  (Flags.DeleteNode || Flags.MultiReplaceNode || Flags.MoveNode)
		{
			pParam->SetReasonForDenial(_R(IDS_BLEND_CANT_OP_ON_CHILDREN));
			allowed=FALSE;
		}
	}

	// if we allow it, (and our parents weren't calling us) see if our parents do ...
	if (allowed && pParam->GetDirection()!=OBJCHANGE_CALLEDBYPARENT && Parent!=NULL)
	{
		ObjChangeDirection OldDirection=pParam->GetDirection();
		pParam->SetCallingChild(this);
		pParam->SetDirection(OBJCHANGE_CALLEDBYCHILD);
		allowed=Parent->AllowOp(pParam,SetOpPermissionState,DoPreTriggerEdit);
		pParam->SetDirection(OldDirection);
	}

	if (allowed && pParam->GetDirection() == OBJCHANGE_CALLEDBYCHILD)
	{
		// Ilan 5/4/00
		// Inform attrs attached at compound level that children have changed
		UndoableOperation* pChangeOp = pParam->GetOpPointer();
		BOOL InformGeomLinkedAttrs = SetOpPermissionState && pChangeOp && pChangeOp->MayChangeNodeBounds();
		if(InformGeomLinkedAttrs)
		{
			NodeAttribute* pNA = FindFirstGeometryLinkedAttr();
			while(pNA)
			{
				pNA->LinkedNodeGeometryHasChanged(pChangeOp);
				pNA = pNA->FindNextGeometryLinkedAttr();
			}
		}
	}

	// if setting permissions ... (and possibly cause post-op code to be called)
	if (SetOpPermissionState)
	{
		if (allowed)
		{
			if (Parent!=NULL)
				Parent->SetOpPermission(PERMISSION_ALLOWED);

			// if post process required, ensure our OnChildChange is called (by setting permission on ourself),
			// insert an inverse action in the undo
			if (pParam->GetDirection()==OBJCHANGE_CALLEDBYCHILD || Flags.Attribute || Flags.RegenerateNode)
			{
				SetOpPermission(PERMISSION_ALLOWED);
				UndoableOperation* pOp=pParam->GetOpPointer();
			 	if (pOp!=NULL)
				{
					if (allowed) allowed=(InitBlendAction::Init(pOp,pOp->GetUndoActionList(),this)!=AC_FAIL);
					if (allowed) allowed=pOp->DoInvalidateNodeRegion(this, TRUE, FALSE, FALSE, !pParam->GetRetainCachedData());
				}
			}
		}
		else
			SetOpPermission(PERMISSION_DENIED,TRUE);
	}

	// if the op was allowed, inform our children by calling their AllowOp()s
	// this must be done after we've inserted our undo actions so that any child undo actions will be undone first!
	if (allowed && pParam->GetDirection()!=OBJCHANGE_CALLEDBYCHILD)
		AllowOp_AccountForCompound(pParam, SetOpPermissionState, DoPreTriggerEdit);

	if (allowed && Flags.Attribute && SetOpPermissionState)
	  	Deinit();

	// if we're ok so far and were asked to do a PreTriggerEdit, then
	// determine whether the Op may change the bounds of some nodes.
	// If it may, then call NameGallery::PreTriggerEdit.
	if (allowed && DoPreTriggerEdit)
	{
		// if the Op is non-NULL then query its MayChangeNodeBounds() method.
		UndoableOperation* pChangeOp = pParam->GetOpPointer();
		if (pChangeOp != NULL && pChangeOp->MayChangeNodeBounds())
		{
			PORTNOTETRACE("other","NodeBlend::AllowOp - removed call to PreTriggerEdit");
#ifndef EXCLUDE_FROM_XARALX
			allowed = NameGallery::Instance()->PreTriggerEdit(pChangeOp, pParam, this);
#endif
		}
	}

	// return result (directly, or indirectly via a child AllowOp()) to op
	return allowed;
}

/********************************************************************************************

>	virtual ChangeCode NodeBlend::OnChildChange(ObjChangeParam* pParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/02/95
	Inputs:		pParam	= pointer to a object change parameter class
	Returns:	CC_OK		if we have successfully processed the change.
				CC_FAIL		if we cannot handle this particular change and must prevent the
							child from continuing
	Purpose:	This function should be overridden in derived object classes.
				Composite objects can use this function to respond to one of their children
				undergoing a change. They should return CC_FAIL whenever they are unable to
				cope with the change.

				Blends deinitialise, then reinitialise themselves.  Hopefully that should do the trick.

	SeeAlso:	WarnParentOfChange(),AllowOp();

********************************************************************************************/

ChangeCode NodeBlend::OnChildChange(ObjChangeParam* pParam)
{
	ERROR2IF(pParam == NULL,CC_FAIL,"pParam == NULL");

	ObjChangeType cType  = pParam->GetChangeType();
	ObjChangeFlags cFlags = pParam->GetChangeFlags();

	if (cType == OBJCHANGE_FINISHED)
	{
		UndoableOperation* pUndoOp = pParam->GetOpPointer();

//		Node* pNodeChanging = pParam->GetChangeObj();
		UINT32 NBPCounter = 0;
		NodeBlendPath* pNodeBlendPath = GetNodeBlendPath(NBPCounter++);
//		BOOL NodeBlendPathEdited = (pNodeChanging != NULL && pNodeChanging == pNodeBlendPath);


		// if the op has finished, and we're either the parent of a selected node OR an attribute 
		// was applied OR our node blend path has been edited, deinit the blend in an undoable way

		while (pNodeBlendPath != NULL)
		{
			pNodeBlendPath->OnEdited(pParam);
			if (!TransformBlendEndObjects(pUndoOp))
				return CC_FAIL;
			pNodeBlendPath = GetNodeBlendPath(NBPCounter++);	
		}	

		if (pUndoOp != NULL)
		{
			if (InitBlendAction::Init(pUndoOp,pUndoOp->GetUndoActionList(),this, FALSE, FALSE, cFlags.RegenerateNode) == AC_FAIL)
				return CC_FAIL;

			if (!pUndoOp->DoInvalidateNodeRegion(this, TRUE, FALSE, FALSE, !pParam->GetRetainCachedData()))
				return CC_FAIL;
		}
		else
		{
			// If no undo op ptr, deinit the blend in a non-undoable way
			InvalidateBlendBoundingRect();
			Deinit(cFlags.RegenerateNode);

			DocRect rect = GetUnionBlobBoundingRect();
			Document* pDoc = (Document*)FindOwnerDoc();
			Spread*	pSpread = FindParentSpread();
			if (pDoc != NULL && pSpread != NULL && pDoc->IS_KIND_OF(Document))
				pDoc->ForceRedraw(pSpread, rect, FALSE, this);
		}
	

	}

	return CC_OK;
}


/********************************************************************************************

>	virtual BOOL NodeBlend::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/95
	Inputs:		pSpread		The spread in which things are happening
				PointerPos	The Location of the mouse pointer at the time of the click
				pMenu		The menu to which items should be added
	Returns:	BOOL - TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Allows the Blend to respond to pop up menu clicks on itself.

********************************************************************************************/

BOOL NodeBlend::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)
{
//WEBSTER-ranbirr-01/12/96	
#ifndef WEBSTER

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	BOOL ok = TRUE;
	
	ok = ok && pMenu->BuildCommand(TOOL_OPTOKEN_BLEND, 		TRUE);
//	ok = ok && pMenu->BuildCommand(OPTOKEN_ADDBLENDPATH, 	TRUE);
//	ok = ok && pMenu->BuildCommand(OPTOKEN_REMOVEBLEND, 	FALSE);
//	ok = ok && pMenu->BuildCommand(OPTOKEN_BLENDONETOONE,  	FALSE);
//	ok = ok && pMenu->BuildCommand(OPTOKEN_BLENDANTIALIAS, 	TRUE);

	return ok;
#else
	return FALSE;
#endif

#else	//webster
	return FALSE;

#endif //webster
}

/********************************************************************************************

>	NodeBlendPath*	NodeBlend::GetNodeBlendPath(UINT32 Index)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/99, changed DY 23/9/99
	Inputs:		Index - number of the NodeBlendPath that you want
	Returns:	Ptr to the NodeBlendPath associated with this blend, or NULL if it aint got one
	Purpose:	Finds the path the blend with try and follow, if it's got one

********************************************************************************************/

NodeBlendPath*	NodeBlend::GetNodeBlendPath(UINT32 Index)
{
	Node* pNode = FindFirstChild();
	UINT32 NBPCounter = 0;
	while (pNode)
	{
		if (IS_A(pNode,NodeBlendPath))
		{	
			//if (((NodeBlendPath*)pNode)->GetPathIndex() == Index)
			if (NBPCounter == Index)
				return (NodeBlendPath*)pNode;

			NBPCounter++;
		}

		pNode = pNode->FindNext();
	}

	return NULL;
}

/********************************************************************************************

>	BOOL NodeBlend::BlendAlongThisPath(NodePath* pPath,CCAttrMap* pAttrMap,UndoableOperation* pUndoOp);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/99
	Inputs:		pPath	= ptr to the definition of the path to blend alone
				pAttrMap = ptr to attrs to apply to given path (can be NULL)
				pUndoOp = ptr to undo op performing th operation.
	Returns:	TRUE if all went to plan
				FALSE otherwise
	Purpose:	Makes this blend object blend along the given path

				It creates a NodeBlendPath from the given NodePath, and inserts it a
				the last child of the blend

********************************************************************************************/

BOOL NodeBlend::BlendAlongThisPath(NodePath* pNodePath,CCAttrMap* pAttrMap,UndoableOperation* pUndoOp, BOOL BlendPathOnTop)
{
	ERROR2IF(pUndoOp   == NULL,FALSE,"I need an undoable op to perform this task");
	ERROR2IF(pNodePath == NULL,FALSE,"Er, where is the path then?");

	BOOL ok = TRUE;
	// DY at this point there should only be one nodeblendpath, hence the zero index
	NodeBlendPath* pNodeBlendPath = GetNodeBlendPath(0);
	if (pNodeBlendPath != NULL)
		ok = pUndoOp->DoHideNode(pNodeBlendPath,TRUE);

	if (ok) pNodeBlendPath = new NodeBlendPath();
	if (ok)	ok = (pNodeBlendPath != NULL);
	if (ok) ok = pNodeBlendPath->CopyNodePath(pNodePath);
	if (ok) ok = pUndoOp->DoLocaliseCommonAttributes(this);
	
	if (ok)  //DY 16/9/99, previously blendpath was always inserted as last child
	{
		if (BlendPathOnTop)
			ok = pUndoOp->DoInsertNewNode(pNodeBlendPath,this,LASTCHILD,FALSE,FALSE,FALSE,FALSE);
		else
			ok = pUndoOp->DoInsertNewNode(pNodeBlendPath,this,FIRSTCHILD,FALSE,FALSE,FALSE,FALSE);
	}

	if (ok) ok = pNodeBlendPath->ApplyAttributes(pAttrMap);
	if (ok) ok = pUndoOp->DoFactorOutCommonChildAttributes(this);

	if (ok) ok = InitBlenderPathProportions(pUndoOp);
	if (ok) ok = TransformBlendEndObjects(pUndoOp);

//  Dont' need to do these two steps as the call to AllowOp() will take care of these things.
//	Deinit();
//	InvalidateBlendBoundingRect();


	return ok;
}

BOOL NodeBlend::InitBlenderPathProportions(UndoableOperation* pUndoOp)
{
	BOOL ok = TRUE;

	UINT32 NumBlenders = GetNumBlenders();
	UINT32 CurBlender  = 0;
	double CurProp = 0.0;
	double PropIncrement = 1.0 / NumBlenders;
	NodeBlender* pNodeBlender = FindFirstBlender();
	while (ok && pNodeBlender != NULL)
	{
		CurBlender++;
		pNodeBlender->SetBlendedOnCurve(TRUE);
		pNodeBlender->SetProportionOfPathDistStart(CurProp);
		CurProp += PropIncrement;
		pNodeBlender->SetProportionOfPathDistEnd(CurProp);
		pNodeBlender = FindNextBlender(pNodeBlender);
	
	}

	return ok;
}

BOOL NodeBlend::RotateBlendEndObjectsBack(UndoableOperation* pUndoOp)
{
	BOOL ok = TRUE;

	Node* pLast = NULL;
	NodeBlender* pNodeBlender = FindFirstBlender();
	while (ok && pNodeBlender != NULL)
	{
		NodeRenderableInk* pStart = pNodeBlender->GetNodeStart();
		NodeRenderableInk* pEnd   = pNodeBlender->GetNodeEnd();

		if (pStart && pStart != pLast)
		{
			double AngleChange = -pNodeBlender->GetAngleStart();
			ok = TransformNodeToPoint(pStart,NULL,pUndoOp,AngleChange);
			if (ok && pUndoOp && AngleChange != 0.0)
			{
				ChangeBlenderOpParam ChangeParam(CHANGEBLENDER_ANGLESTART);
				ChangeParam.m_NewAngleStart = 0.0;
				ActionCode Ac = ChangeBlenderAction::Init(	pUndoOp,pUndoOp->GetUndoActionList(),
															pNodeBlender,ChangeParam);
				ok = (Ac !=AC_FAIL);
			}
		}

		if (pEnd && ok)
		{
			double AngleChange = -pNodeBlender->GetAngleEnd();
			ok = TransformNodeToPoint(pEnd,NULL,pUndoOp,AngleChange);
			if (ok && pUndoOp && AngleChange != 0.0)
			{
				ChangeBlenderOpParam ChangeParam(CHANGEBLENDER_ANGLEEND);
				ChangeParam.m_NewAngleEnd = 0.0;
				ActionCode Ac = ChangeBlenderAction::Init(	pUndoOp,pUndoOp->GetUndoActionList(),
															pNodeBlender,ChangeParam);
				ok = (Ac !=AC_FAIL);
			}
			pLast = pEnd;
		}

		pNodeBlender = FindNextBlender(pNodeBlender);
	}

	return ok;
}

BOOL NodeBlend::TransformBlendEndObjects(UndoableOperation* pUndoOp)
{
	BOOL ok = TRUE;

	Node* pLast = NULL;
	NodeBlender* pNodeBlender = FindFirstBlender();
	while (ok && pNodeBlender != NULL)
	{
		NodeRenderableInk* pStart = pNodeBlender->GetNodeStart();
		NodeRenderableInk* pEnd   = pNodeBlender->GetNodeEnd();
		ERROR3IF(pStart   == NULL,"No start node");
		ERROR3IF(pEnd   == NULL,"No end node");

		if ((pStart == NULL) || (pEnd == NULL)) { return (FALSE); }

	
		DocCoord Point;
		double Angle = 0.0;
		if (pStart && pStart != pLast)
		{
			if (pNodeBlender->GetPointOnNodeBlendPath(0.0,&Point,&Angle))
			{
				double AngleStart = pNodeBlender->GetAngleStart();
				double AngleChange = Angle - AngleStart;
				ok = TransformNodeToPoint(pStart,&Point,pUndoOp,AngleChange);
				if (ok && pUndoOp && AngleChange != 0.0)
				{
					ChangeBlenderOpParam ChangeParam(CHANGEBLENDER_ANGLESTART);
					ChangeParam.m_NewAngleStart = AngleStart + AngleChange;
					ActionCode Ac = ChangeBlenderAction::Init(	pUndoOp,pUndoOp->GetUndoActionList(),
																pNodeBlender,ChangeParam);
					ok = (Ac !=AC_FAIL);
				}
			}
		}

		if (pEnd && ok)
		{
			if (pNodeBlender->GetPointOnNodeBlendPath(1.0,&Point,&Angle))
			{
				double AngleEnd = pNodeBlender->GetAngleEnd();
				double AngleChange = Angle - AngleEnd;
				ok = TransformNodeToPoint(pEnd,&Point,pUndoOp,AngleChange);

				ERROR3IF (!ok, "one does not find this amusing!");

				if (ok && pUndoOp && AngleChange != 0.0)
				{
					ChangeBlenderOpParam ChangeParam(CHANGEBLENDER_ANGLEEND);
					ChangeParam.m_NewAngleEnd = AngleEnd + AngleChange;
					ActionCode Ac = ChangeBlenderAction::Init(	pUndoOp,pUndoOp->GetUndoActionList(),
																pNodeBlender,ChangeParam);
					ok = (Ac !=AC_FAIL);
				}
			}
			pLast = pEnd;
		}

		pNodeBlender = FindNextBlender(pNodeBlender);
	}

	return ok;
}

BOOL NodeBlend::TransformNodeToPoint(NodeRenderableInk* pNode,DocCoord* pPoint,UndoableOperation* pUndoOp,double Angle)
{
	ERROR2IF(pNode   == NULL,FALSE,"Null node ptr");

	BOOL ok = TRUE;


	// Get the node's bounds
	pNode->InvalidateBoundingRect(TRUE);
	DocRect Rect = pNode->GetBoundingRect();

	// Work out the translation that will move the centre of the bounds onto the given point 
	DocCoord centre = Rect.Centre(); //(Rect.lox + (Rect.Width()/2),Rect.loy + (Rect.Height()/2));

	INT32 dx = 0;
	INT32 dy = 0;
	if (pPoint)
	{
		dx = pPoint->x - centre.x;
		dy = pPoint->y - centre.y;
	}

	if (pUndoOp)
	{
		// DoTransformNode() needs an instance of the matrix.  It gets deleted when the action is destroyed
		if (Angle != 0.0)
		{
			Trans2DMatrix* pRotate = new Trans2DMatrix(centre,Angle);
			if (ok)
			{
				RangeControl rc(TRUE,TRUE);
				rc.SiblingsOnly = TRUE;			// Don't look at children - stops children getting 
												// transformed twice when select-inside is present
				Range r(pNode,pNode, rc);
				ok = pUndoOp->DoTransformNodes(r,pRotate);
			}
		}

		if (dx != 0 || dy != 0)
		{
			Trans2DMatrix* pTrans = new Trans2DMatrix(dx,dy);
			if (ok)
			{
				RangeControl rc(TRUE,TRUE);
				rc.SiblingsOnly = TRUE;			// Don't look at children - stops children getting 
												// transformed twice when select-inside is present
				Range r(pNode,pNode, rc);
				ok = pUndoOp->DoTransformNodes(r,pTrans);
			}
		
		}
	}
	else
	{
		// Non-undoable form of the transform
		if (Angle != 0.0)
		{
			Trans2DMatrix Rotate(centre,Angle);
			pNode->Transform(Rotate);
		}

		if (dx != 0 || dy != 0)
		{
			Trans2DMatrix Trans(dx,dy);
			pNode->Transform(Trans);
		}
	}

	return ok;
}

/*********************************************************************************************

>	BOOL NodeBlend::DetachNodeBlendPath(Node* pContextNode, AttachNodeDirection AttDir,UndoableOperation* pUndoOp)

     Author:    Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:   29/4/99 //changed Diccon 9/99
     Inputs:    -
     Outputs:   -
     Returns:   -
     Purpose:   Removes all nodeblendpaths

**********************************************************************************************/

BOOL NodeBlend::DetachNodeBlendPath(Node* pContextNode, AttachNodeDirection AttDir,UndoableOperation* pUndoOp)
{
	ERROR2IF(pContextNode == NULL,FALSE,"NULL pContextNode");
	ERROR2IF(pUndoOp      == NULL,FALSE,"NULL pUndoOp");

	BOOL ok = TRUE;
	UINT32 NBPCounter = 0;
	NodeBlendPath* pNodeBlendPath = GetNodeBlendPath(NBPCounter++);
	while (pNodeBlendPath != NULL)
	{
		NodePath* pNodePath = pNodeBlendPath->CreateNodePathCopy();
		if (pNodePath != NULL)
		{
			CCAttrMap* pAttrMap = new CCAttrMap(30);
					ok = (pAttrMap != NULL);
			if (ok)	ok = pNodeBlendPath->FindAppliedAttributes(pAttrMap);
			if (ok) ok = pUndoOp->DoInsertNewNode(pNodePath,pContextNode,AttDir,FALSE,FALSE,FALSE,FALSE);
			if (ok)	ok = pUndoOp->DoHideNode(pNodeBlendPath,TRUE);
			if (ok) ok = pUndoOp->DoSelectNode(pNodePath);				// Select it, because it's nicer that way
			if (ok)	pNodePath->ApplyAttributes(pAttrMap,TRUE);

			if (pAttrMap != NULL)
			{
				delete pAttrMap;
				pAttrMap = NULL;
			}
		}
		// go through all the blenders and set their index flags to -1
		// NOTE - need to make this undoable
		NodeBlender* pBlender = FindFirstBlender();
		while (pBlender != NULL)
		{	
			pBlender->SetNodeBlendPathIndex(-1);
			pBlender = FindNextBlender(pBlender);
		
		}
		pNodeBlendPath = GetNodeBlendPath(NBPCounter++);
	}
	return ok;
}

/*********************************************************************************************

>	NodeBlender* NodeBlend::FindFirstBlender()

     Author:    Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:   29/4/99
     Inputs:    -
     Outputs:   -
     Returns:   Pointer to the first NodeBlender belonging to this blend object
				or NULL if there aren't any
     Purpose:   Convenience function.  A drive-thru NodeBlender takeaway service
	 SeeAlso:	FindNextBlender()

**********************************************************************************************/

NodeBlender* NodeBlend::FindFirstBlender()
{
	Node* pNode = FindFirstChild();
	while (pNode != NULL)
	{
		if (IS_A(pNode,NodeBlender))
			return ((NodeBlender*)pNode);

		pNode = pNode->FindNext();
	}

	return NULL;
}


/*********************************************************************************************

>	NodeBlender* NodeBlend::FindNextBlender(NodeBlender* pNodeBlender)

     Author:    Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:   29/4/99
     Inputs:    pNodeBlender = ptr to previous NodeBlender found
     Outputs:   -
     Returns:   Pointer to the next NodeBlender belonging to this blend object, in rendering order
				or NULL if there aren't any
     Purpose:   Convenience function.  A drive-thru NodeBlender takeaway service
	 SeeAlso:	FindFirstBlender()

**********************************************************************************************/

NodeBlender* NodeBlend::FindNextBlender(NodeBlender* pNodeBlender)
{
	if (pNodeBlender)
	{
		Node* pNode = pNodeBlender->FindNext();

		while (pNode != NULL)
		{
			if (IS_A(pNode,NodeBlender))
				return ((NodeBlender*)pNode);

			pNode = pNode->FindNext();
		}
	}

	return NULL;
}


/*********************************************************************************************

>	NodeBlender* NodeBlend::FindLastBlender()

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
     Created:   16/9/99
     Inputs:    -
     Outputs:   -
     Returns:   Pointer to the last NodeBlender belonging to this blend object, in rendering order
				or NULL if there aren't any
     Purpose:   Convenience function.  A drive-thru NodeBlender takeaway service
	 SeeAlso:	FindFirstBlender(), FindNextBlender()

**********************************************************************************************/

NodeBlender* NodeBlend::FindLastBlender()
{
	Node* pNode = FindLastChild();
	while (pNode != NULL)
	{
		if (pNode->IS_KIND_OF(NodeBlender))
		{
			return (NodeBlender*)pNode;
		}
		pNode = pNode->FindPrevious();
	}
	return NULL;

}


/*********************************************************************************************

>	BOOL NodeBlend::FindFirstAndLastBlender(NodeBlender** pFirstBlender, NodeBlender** ppLastBlender)

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
     Created:   16/9/99
     Inputs:    -
     Outputs:   pFirstBlender, pLastBlender, as the names suggest
     Returns:   TRUE if successful, FALSE otherwise
     Purpose:   Convenience function.  A drive-thru NodeBlender takeaway service
	 SeeAlso:	FindFirstBlender(), FindNextBlender()

**********************************************************************************************/

BOOL NodeBlend::FindFirstAndLastBlenders(NodeBlender** ppFirstBlender, NodeBlender** ppLastBlender)
{
	NodeBlender* pTempBlender = FindFirstBlender();
	if (pTempBlender == NULL)
		return FALSE;
	else
		*ppFirstBlender = pTempBlender;

	pTempBlender = NULL;
	pTempBlender = FindLastBlender();
	if (pTempBlender == NULL)
		return FALSE;
	
	*ppLastBlender = pTempBlender;

	return TRUE;
}



/*********************************************************************************************

>	Node* NodeBlend::HasEditableChild(CCRuntimeClass* ChildClass, Node* pPreviousChild)

     Author:    Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:   29/4/99
     Inputs:    ChildClass		= the runtime class of the editable object
	 			pPreviousChild	= a pointer to the previous editable child 
	 							  returned by 'this' node, NULL if this is the first
								  call to this node.
     Outputs:   -
     Returns:   A node pointer, to an object which forms part of the editable surface of
	 			its parent (this node).
     Purpose:   This function returns our edit node (the NodeBlendPath)

**********************************************************************************************/

Node* NodeBlend::HasEditableChild(CCRuntimeClass* ChildClass, Node* pPreviousChild)
{
	if (ChildClass != CC_RUNTIME_CLASS(NodePath))
		return NULL;

	NodeBlendPath* pNodeBlendPath = GetNodeBlendPath(0);
	// check to see if this has already been asked for once
	if (((Node*)pNodeBlendPath) == pPreviousChild)
		return NULL;

	return pNodeBlendPath;
}

/*********************************************************************************************

>	 BOOL NodeBlend::GetBlendDistance(BOOL FullDistance, double* Distance)

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   26/9/99
     Inputs:    boolean indicating whether to return the full length of the blendpath
				or just the length that is currently occupied by the blend
     Outputs:   length of the blend
     Returns:   TRUE if successful, FALSE otherwise
     Purpose:   To find out how long the blend is.

**********************************************************************************************/

BOOL NodeBlend::GetBlendDistance( BOOL FullDistance, double* Distance)
{
	NodeBlender* pBlender = FindFirstBlender();
	double TempDistance = 0.0;
	
	while (pBlender != NULL)
	{
		if (pBlender->IsBlendedOnCurve()) 
		{
			NodeBlendPath* pPath = pBlender->GetNodeBlendPath();
			if (pPath == NULL) 
				return FALSE;

			double PathDistance = pPath->GetPathLength();
			double StartProportion = 0.0;
			double EndProportion = 1.0;

			if (FullDistance ==FALSE)
			{
				// the blend may only go part way along the curve, calculate how
				// much distance the actual blend takes up 
				
				StartProportion = pBlender->GetProportionOfPathDistStart();
				EndProportion = pBlender->GetProportionOfPathDistEnd();
			}

			TempDistance += (PathDistance * (EndProportion - StartProportion));

		}
		else
		{
			TempDistance += pBlender->GetLinearDistance();
		}
		pBlender = FindNextBlender(pBlender);
	}
	
	ERROR2IF((TempDistance < 0), FALSE, "Negative path length");

	*Distance = TempDistance;
	return TRUE;
}


/*********************************************************************************************

>	 BOOL NodeBlend::GetNumStepsFromDistance(double StepDistance, UINT32* NumSteps)

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   26/9/99
     Inputs:    the desired distance between steps
     Outputs:   number of steps required to achieve the distance closest to the specified value
     Returns:   TRUE if successful, FALSE otherwise
     Purpose:   To determine the number of steps that will achieve a distance between steps close
				to the value passed.

**********************************************************************************************/

BOOL NodeBlend::GetNumStepsFromDistance(double Distance, UINT32* NumSteps)
{
	ASSERT(Distance);
	double BlendLength = 0.0;
	BOOL Valid = GetBlendDistance(TRUE, &BlendLength);

	if (!Valid)
		return FALSE;

	UINT32 TempNumSteps = (UINT32)(BlendLength / Distance);

	ERROR3IF((TempNumSteps < 0), "Negative number of blend steps");
	*NumSteps = TempNumSteps;
	return TRUE;
}



/*********************************************************************************************

>	 double NodeBlend::GetStepDistance()

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   26/9/99
     Inputs:    -
     Outputs:   -
     Returns:   the distance between steps of the blend
     Purpose:   works out the distance between each step in the blend
**********************************************************************************************/

double NodeBlend::GetStepDistance()
{
	double BlendLength = 0.0;
	GetBlendDistance(FALSE, &BlendLength);
	
	double StepDistance = BlendLength/m_NumBlendSteps;

	ERROR2IF((StepDistance < 0), 0, "Negative step distance");
	return StepDistance;
}

/*********************************************************************************************

>	 void NodeBlend::UpdateStepDistance()

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   26/9/99
     Inputs:    -
     Outputs:   -
     Returns:   -
     Purpose:   works out the distance between each step in the blend 
				and updates the member variable
**********************************************************************************************/

void NodeBlend::UpdateStepDistance()
{
	double BlendLength = 0.0;
	GetBlendDistance(FALSE, &BlendLength);
	
	m_StepDistance = BlendLength/m_NumBlendSteps;
}



/*********************************************************************************************

>	 double NodeBlend::GetDistanceEntered()

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   7/9/99
     Inputs:    -
     Outputs:   -
     Returns:   the last distance that the user entered in the edit box,
				this will be initialised to -1 so if the user has not 
				entered any value then -1 will be returned
     Purpose:   to return the last distance between blend steps that user 
				entered
**********************************************************************************************/

double NodeBlend::GetDistanceEntered()
{
	return m_DistanceEntered;
}

/*********************************************************************************************

>	 void NodeBlend::SetDistanceEntered(double Distance)

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   7/9/99
     Inputs:    Distance between steps to set
     Outputs:   -
     Returns:   -
     Purpose:   to set the distance between blend steps that user 
				entered
**********************************************************************************************/

void NodeBlend::SetDistanceEntered(double Distance)
{
	 m_DistanceEntered = Distance;
}


/*********************************************************************************************

>	 EndObject NodeBlend::GetLastEdited()

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   15/9/99
     Inputs:    -
     Outputs:   -
     Returns:   the last object edited, either FIRST, LAST or NONE.
     Purpose:   Access function, lets you know which end object of a blend on a path 
				was last edited. 
**********************************************************************************************/

EndObject NodeBlend::GetLastEdited()
{
	return m_LastEdited;
}


/*********************************************************************************************

>	 void NodeBlend::SetLastEdited(EndObject ObjectEdited)

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   15/9/99
     Inputs:    Object last edited
     Outputs:   -
     Returns:   -
     Purpose:   Access function 
**********************************************************************************************/

void NodeBlend::SetLastEdited(EndObject LastEdited)
{
	m_LastEdited = LastEdited;
}



/*********************************************************************************************

>	 UINT32 NodeBlend::GetNumNodeBlendPaths()

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   23/9/99
     Inputs:    -
     Outputs:   -
     Returns:   the number of nodeblend paths this blend is using
     Purpose:   Access function
**********************************************************************************************/

UINT32 NodeBlend::GetNumNodeBlendPaths()
{
	UINT32 NBPCounter = 0;
	Node* pNode = FindFirstChild();
	while (pNode != NULL)
	{
		if (pNode->IS_KIND_OF(NodeBlendPath))
			NBPCounter++;
		pNode = pNode->FindNext();
	}

	return NBPCounter;
}


/*********************************************************************************************

>	 void NodeBlend::SetNumNodeBlendPathsInc(BOOL Increase)

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   23/9/99
     Inputs:    Increase - TRUE if you wish to add, FALSE if you want to take away
     Outputs:   -
     Returns:   -
     Purpose:   Record a change in the number of nodeblendpaths
	 Errors:    If m_NumNodeBlendPaths = 0 and Increment = FALSE

**********************************************************************************************/

void NodeBlend::SetNumNodeBlendPathsInc(BOOL Increase)
{
	if (Increase)
		m_NumNodeBlendPaths++;
	else
	{
		if (m_NumNodeBlendPaths == 0)
		{ 
			ERROR3("Blend has no nodeblend paths");
			return;
		}
		m_NumNodeBlendPaths--;
	}
}



/*********************************************************************************************

>	 void NodeBlend::SetNumNodeBlendPaths(INT32 Index)

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   23/9/99
     Inputs:    Index - number that you wish to set
     Outputs:   -
     Returns:   -
     Purpose:   Record a change in the number of nodeblendpaths
	 Errors:    If m_NumNodeBlendPaths = 0 and Increment = FALSE
**********************************************************************************************/

void NodeBlend::SetNumNodeBlendPaths(INT32 Index)
{
	if (Index < 0)
	{
		ERROR3("Index is less than zero");
		return;
	}
	else
		m_NumNodeBlendPaths = Index;
}




/*********************************************************************************************

>	 BOOL NodeBlend::GetStartAndEndProportions(double* StartProp, double* EndProp)

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   15/9/99
     Inputs:    -
     Outputs:   StartProp - the proportion along the nodeblendpath where the first blend object is
				EndProp - the proportion along the nodeblendpath where the last blend object is
     Returns:   TRUE if successful, FALSE if not, or if the blend is not on a path
     Purpose:   To find the proportions along the path where the first and last objects are located
**********************************************************************************************/

BOOL NodeBlend::GetStartAndEndProportions(double *StartProp, double* EndProp)
{
	if (m_BlendedOnCurve == FALSE)
	{
		ERROR3("Blend is not on a curve");
		return FALSE;
	}
	NodeBlender* pNodeBlender = FindFirstBlender();

	if (pNodeBlender == NULL)
	{
		ERROR3("Can't find first blender");
		return FALSE;
	}

	double TempStartProp = pNodeBlender->GetProportionOfPathDistStart();
	
	if (TempStartProp < 0.0 || TempStartProp > 1.0)
	{
		ERROR3("Invalid value for start proportion");
		return FALSE;
	}

	double TempEndProp = 1.0;

	while (pNodeBlender != NULL)
	{
		TempEndProp = pNodeBlender->GetProportionOfPathDistEnd();
		pNodeBlender = FindNextBlender(pNodeBlender);
	}

	if (TempEndProp < 0.0 || TempEndProp > 1.0)
	{
		ERROR3("Invalid value for end proportion");
		return FALSE;
	}

	*StartProp = TempStartProp;
	*EndProp = TempEndProp;
	return TRUE;
}


/*********************************************************************************************

>	 BOOL NodeBlend::GetStartAndEndNodes(Node** ppStart, Node** ppEnd)

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   15/9/99
     Inputs:    -
     Outputs:   pStart - the first blend object 
				pEnd   - the last blend object 
     Returns:   TRUE if successful, FALSE if not
	 Purpose:   To find the first and last objects in a blend 
**********************************************************************************************/

BOOL NodeBlend::GetStartAndEndNodes(Node** ppStart, Node** ppEnd)
{

	NodeBlender* pFirstBlender = FindFirstBlender();
	if (pFirstBlender == NULL)
	{
		ERROR3("No first blender");
		return FALSE;
	}

	*ppStart = pFirstBlender->GetNodeStart();
	if (*ppStart == NULL)
	{
		ERROR3("No start node");
		return FALSE;
	}


	NodeBlender* pLastBlender = FindLastBlender();
	if (pLastBlender == NULL)
	{
		ERROR3("No last blender");
		return FALSE;
	}

	*ppEnd = pLastBlender->GetNodeEnd();
	if (*ppEnd == NULL)
	{
		ERROR3("No last node");
		return FALSE;
	}
	
	return TRUE;
}


/*********************************************************************************************

>	 BOOL NodeBlend::NonLinearObjectProfile()

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   4/10/99
     Inputs:    -
     Outputs:   - 
     Returns:   TRUE if the blend currently has a non-linear object profile, otherwise false
	 Purpose:   as above
**********************************************************************************************/

BOOL NodeBlend::NonLinearObjectProfile()
{
	if (m_ObjectProfile.GetBias() != 0.0)
		return TRUE;
	if (m_ObjectProfile.GetGain() != 0.0)
		return TRUE;
	return FALSE;
}


/*********************************************************************************************

>	 BOOL NodeBlend::HitOnEndDragBlob(DocCoord Point, Node** ppHitNode)

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   20/9/99
     Inputs:    Point  - the point to test
     Outputs:   ppHitNode - the node that was hit, if there was one
   	Returns:    TRUE if Point is within the central drag blob of an end node, otherwise false
 			    also FALSE if blend is not on a curve
     Purpose:   To determine whether or not Point is over the central blob of one of the end 
				objects of a blend on a curve.
**********************************************************************************************/

BOOL NodeBlend::HitOnEndDragBlob(DocCoord Point, Node** ppHitNode)
{

	NodeBlender* pBlender = FindFirstBlender();
	BOOL ok = FALSE;
	while (pBlender != NULL)
	{
		/*NodeBlender* pNextBlender =*/ FindNextBlender(pBlender);
		// only the end objects of blends on curves are editable
		if (pBlender->IsBlendedOnCurve())
		{
			Node* pFirstNode = pBlender->GetNodeStart();
			Node* pLastNode = pBlender->GetNodeEnd();
			if (pFirstNode == NULL || pLastNode == NULL)
			{
				return FALSE;
			}
			ok = HitOnDragBlob((NodeRenderableInk*)pFirstNode, Point);
			if (ok)
			{
				*ppHitNode = pFirstNode;
				return TRUE;
			}
			ok = HitOnDragBlob((NodeRenderableInk*)pLastNode, Point);
			if (ok)
			{
				*ppHitNode = pLastNode;
				return TRUE;
			}
		}
		pBlender = FindNextBlender(pBlender);
	}


	return ok;
}


/*********************************************************************************************

>	 BOOL NodeBlend::HitOnDragBlob(NodeRenderableInk* pInk, DocCoord PointerPos)

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   15/9/99
     Inputs:    pInk - the node to test
				PointerPos - the point to test
     Outputs:	- 
     Returns:   TRUE if PointerPos is within the central blob of pink, else FALSE
	 Purpose:   To find out if the pointer position is within the bounds of the blob
				in the middle of this node.
**********************************************************************************************/

BOOL NodeBlend::HitOnDragBlob(NodeRenderableInk* pInk, DocCoord PointerPos)
{
	if (pInk == NULL)
	{
		ERROR3("Ink node is NULL");
		return FALSE;
	}
	BOOL ok = FALSE;
	// find the bounding rect of the centre blob
	DocRect BlobSize;
	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr != NULL)
	{
		DocRect BoundingRect = pInk->GetBoundingRect();
		DocCoord CentrePoint = BoundingRect.Centre();
		pBlobMgr->GetBlobRect(CentrePoint, &BlobSize);

		ok = BlobSize.ContainsCoord(PointerPos);
	}

	return ok;
}



/********************************************************************************************

>	virtual INT32 NodeBlend::EstimateNodeComplexity (OpParam* details)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/09/2000

	Inputs:		details		any data that should be used for the calculation

	Outputs:	-

	Returns:	an estimate of the nodes complexity

	Purpose:	This function estimates a complexity value for the node.  The complexity
				value is based upon the total length of all paths in the node.

	See Also:	OpBlendNodes::DeterminBlendObjectsProcessorHit ()

********************************************************************************************/

INT32 NodeBlend::EstimateNodeComplexity (OpParam* details)
{
	NodeBlender* pBlender = FindFirstBlender();
	INT32 total = 0;

	while (pBlender)
	{
		total += pBlender->EstimateNodeComplexity (details);
		pBlender = FindNextBlender(pBlender);
	}

	return (total);
}

/*********************************************************************************************

>	 NodeBlender* NodeBlend::NodeIsPartOfBlender( Node* pNode)

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   15/9/99
     Inputs:    
				pNode - the node to look for
     Outputs:	- 
     Returns:   the nodeblender which uses pNode at the specified position
	 Purpose:   To find out if pNode is used by a blender in this nodeblend at the position
				specified. Useful if you are changing the node through a different blender and
				wish to update related blends
**********************************************************************************************/

NodeBlender* NodeBlend::NodeIsPartOfBlender(Node* pNode, BOOL FirstNode)
{
	if (pNode == NULL)
	{
		ERROR3("Node is NULL");
		return NULL;
	}

	NodeBlender* pNodeBlender = FindFirstBlender();

	if (pNodeBlender == NULL)
	{
		ERROR3("No nodeblender");
		return NULL;
	}

	while (pNodeBlender != NULL)
	{
		if (FirstNode)
		{
			Node* pCheckFirstNode = pNodeBlender->GetNodeStart();
			if (pCheckFirstNode == pNode)
				return pNodeBlender;
		}
		else
		{
			Node* pCheckLastNode = pNodeBlender->GetNodeEnd();
			if (pCheckLastNode == pNode)
				return pNodeBlender;
		}
		
		pNodeBlender = FindNextBlender(pNodeBlender);
	}
	return NULL;
}


/*********************************************************************************************

>	 NodeBlender* NodeBlend::NodeIsPartOfBlender( Node* pNode, Nodeblender* pBlender, BOOL* First)

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   15/9/99
     Inputs:    pNode - the node to look for
				pBlender - the blender that we already know pNode belongs to
     Outputs:	First, whether the node is first on last in the newly found blender
     Returns:   the nodeblender which uses pNode and is not pBlender
	 Purpose:   Overridden version of above, now that we have multi stage blends which can go
				to and from arbitrary end points we can't always rely on knowing the end position
**********************************************************************************************/

NodeBlender* NodeBlend::NodeIsPartOfBlender(Node* pNode, NodeBlender* pBlender, BOOL* First)
{
	if (pNode == NULL)
	{
		ERROR3("Node is NULL");
		return NULL;
	}

	NodeBlender* pNodeBlender = FindFirstBlender();

	if (pNodeBlender == NULL)
	{
		ERROR3("No nodeblender");
		return NULL;
	}


	while (pNodeBlender != NULL)
	{
		if (pNodeBlender != pBlender)
		{
			Node* pStart = pNodeBlender->GetNodeStart();
			Node* pEnd = pNodeBlender->GetNodeEnd();

			if (pStart == NULL || pEnd == NULL)
			{
				ERROR3("Start or end node is NULL");
				return NULL;
			}

			if (pStart == pNode)
			{
				*First = TRUE;
				return pNodeBlender;
			}
			else if (pEnd == pNode)
			{
				*First = FALSE;
				return pNodeBlender;
			}
		}
		pNodeBlender = FindNextBlender(pNodeBlender);
	}
	return NULL;
}


/*********************************************************************************************

>	 virtual NodeCompound* NodeBlend::GetParentController() const

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   15/9/99
     Inputs:    -
     Outputs:	- 
     Returns:   Controller node if there is one, or NULL
	 Purpose:   If the blend is part of a bevel, shadow, or contour then
				this returns the parent controller
**********************************************************************************************/

NodeCompound* NodeBlend::GetParentController() const
{
	Node* pParent = (Node*)this->FindParent();

	if (pParent == NULL)
	{
		ERROR3("This blend has no parents");
		return NULL;
	}

	while ((pParent != NULL) && (!pParent->IsLayer()))
	{
		if (pParent->IsCompoundClass() && pParent->IsController())
			return static_cast<NodeCompound*>(pParent);

		pParent = pParent->FindParent();
	}
	return NULL;

}



//-----------------------------------------------

/********************************************************************************************

>	InitBlendAction::InitBlendAction()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

InitBlendAction::InitBlendAction()
{
	m_pNodeBlend = NULL;
	m_pNodeBlender = NULL;
	m_pStartNode = NULL;
	m_pEndNode = NULL;
	m_bReverse = FALSE;
	m_bStoreStartAndEndNodes = FALSE;
	m_enInitType = IBAT_INVALID;
}


/********************************************************************************************

>	ActionCode InitBlendAction::Init(UndoableOperation* pOp,ActionList* pActionList,NodeBlend* pThisNodeBlend,
										BOOL StoreStartAndEndNodes, BOOL bReverse)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/95
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pThisNodeBlend  = ptr to blend to init.
				pActionList 	= ptr to action list to which this action should be added
				StoreStartAndEndNodes = Whether to store the start & end nodes or not
				bReverse		=	Whether to insert this action into the head or the tail
									of the action list (if FALSE it's treated normally)
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.

				Records an action that, when executed, will Deinit() then Reinit() the blend, then 
				invalidate its bounds.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/



ActionCode InitBlendAction::Init(UndoableOperation* pOp,ActionList* pActionList,NodeBlend* pThisNodeBlend,
								 BOOL StoreStartAndEndNodes, BOOL bReverse, BOOL bNodesMayBeChanged)
{
	UINT32 ActSize = sizeof(InitBlendAction);

	InitBlendAction* pNewAction;
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(InitBlendAction),(Action**)&pNewAction);

	NodeBlender * pBlender = (NodeBlender *)pThisNodeBlend->FindFirstChild(CC_RUNTIME_CLASS(NodeBlender));

	if (pNewAction)
		pNewAction->m_enInitType = IBAT_INIT;

	if (Ac != AC_FAIL && pNewAction != NULL)
	{
		pNewAction->m_pNodeBlend = pThisNodeBlend;

		if (StoreStartAndEndNodes && pBlender)
		{
			pNewAction->m_pStartNode = pBlender->GetNodeStart();
			pNewAction->m_pEndNode   = pBlender->GetNodeEnd();
		}

		pNewAction->m_bReverse = bReverse;
		pNewAction->m_bStoreStartAndEndNodes = StoreStartAndEndNodes;
		pNewAction->m_bNodesMayBeChanged = bNodesMayBeChanged;

		if (bReverse)
		{
			// reverse this action in the action list
			pActionList->RemoveItem(pNewAction);
			pActionList->AddHead(pNewAction);
		}

		pThisNodeBlend->InvalidateBlendBoundingRect();
		pThisNodeBlend->Deinit(bNodesMayBeChanged);
//		if (!pOp->DoInvalidateNodeRegion(pThisNodeBlend,TRUE))
//			Ac = AC_FAIL;
	}

	return Ac;
}

ActionCode InitBlendAction::InitOnBlender(UndoableOperation* pOp,ActionList* pActionList,
										  NodeBlender* pThisNodeBlender, BOOL StoreStartAndEndNodes, BOOL bReverse)
{
	UINT32 ActSize = sizeof(InitBlendAction);

	InitBlendAction* pNewAction;
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(InitBlendAction),(Action**)&pNewAction);
	
	NodeBlend* pThisNodeBlend = (NodeBlend *)pThisNodeBlender->FindParent(CC_RUNTIME_CLASS(NodeBlend));

	pNewAction->m_enInitType = IBAT_INITONBLENDER;
	pNewAction->m_pNodeBlender = pThisNodeBlender;

	if (Ac != AC_FAIL && pNewAction != NULL)
	{
		pNewAction->m_pNodeBlend = pThisNodeBlend;

		if (StoreStartAndEndNodes && pThisNodeBlender)
		{
			pNewAction->m_pStartNode = pThisNodeBlender->GetNodeStart();
			pNewAction->m_pEndNode   = pThisNodeBlender->GetNodeEnd();
		}

		pNewAction->m_bReverse = bReverse;
		pNewAction->m_bStoreStartAndEndNodes = StoreStartAndEndNodes;
		pNewAction->m_bNodesMayBeChanged = FALSE;

		if (bReverse)
		{
			// reverse this action in the action list
			pActionList->RemoveItem(pNewAction);
			pActionList->AddHead(pNewAction);
		}

		pThisNodeBlend->InvalidateBlendBoundingRect();
		pThisNodeBlend->Deinit();
//		if (!pOp->DoInvalidateNodeRegion(pThisNodeBlend,TRUE))
//			Ac = AC_FAIL;
	}

	return Ac;
}


/********************************************************************************************

>	ActionCode InitBlendAction::Execute();

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/95
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  
				It calls Deinit() then Reinit() on the blend, then invalidate its bounds.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode InitBlendAction::Execute()
{
	if (pOperation->IS_KIND_OF(UndoableOperation))
	{
		switch (m_enInitType)
		{
			case IBAT_INIT:
			{
				if (InitBlendAction::Init((UndoableOperation*)pOperation,pOppositeActLst,m_pNodeBlend,
					m_bStoreStartAndEndNodes, m_bReverse, m_bNodesMayBeChanged) != AC_OK)
					return AC_FAIL;
			}
			break;
			case IBAT_INITONBLENDER:
			{
				if (InitBlendAction::InitOnBlender((UndoableOperation*)pOperation,pOppositeActLst,m_pNodeBlender,
					m_bStoreStartAndEndNodes, m_bReverse) != AC_OK)
					return AC_FAIL;
			}
			break;
			default:
			{
				ERROR3 ("Er - I don't know what to do!");
				return (AC_FAIL);
			}
			break;
		}

		// DMc
		// ok, if we've got either the start or the end nodes then re-init
		if (m_pStartNode != NULL && m_pEndNode != NULL)
		{
			NodeBlender * pBlender = NULL;

			switch (m_enInitType)
			{
				case IBAT_INIT:
				{
					pBlender = (NodeBlender *)m_pNodeBlend->FindFirstChild(CC_RUNTIME_CLASS(NodeBlender));
				}
				break;
				case IBAT_INITONBLENDER:
				{
					pBlender = m_pNodeBlender;//(NodeBlender *)m_pNodeBlend->FindFirstChild(CC_RUNTIME_CLASS(NodeBlender));
				}
				break;
				default:
				{
					ERROR3 ("Er - I don't know what to do!");
					return (AC_FAIL);
				}
				break;
			}

			if (pBlender)
			{
				pBlender->Reinit((NodeRenderableInk *)m_pStartNode, (NodeRenderableInk *)m_pEndNode, FALSE);
			}
		}

		return AC_OK;
	}
	else
	{
		ERROR3("InitBlendAction::Execute() called with op that's not an undoable op");
		return AC_FAIL;
	}
}

InitBlendAction::~InitBlendAction()
{
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

/********************************************************************************************

  >	virtual BOOL NodeBlend::WritePreChildren(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the blend record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlend::WritePreChildren(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	BOOL ok = TRUE;

		// Diccon 9/99 added profile record 
	if (ok)
	{
		CXaraFileRecord ProfileRec(TAG_BLENDPROFILES, TAG_BLENDPROFILES_SIZE);
				ok = ProfileRec.Init();
		if (ok) ok = ProfileRec.WriteDOUBLE(m_ObjectProfile.GetBias());
		if (ok) ok = ProfileRec.WriteDOUBLE(m_ObjectProfile.GetGain());
		if (ok) ok = ProfileRec.WriteDOUBLE(m_AttrProfile.GetBias());
		if (ok) ok = ProfileRec.WriteDOUBLE(m_AttrProfile.GetGain());
		if (ok) ok = ProfileRec.WriteDOUBLE(/*m_PositionProfile.GetBias()*/0.0);
		if (ok) ok = ProfileRec.WriteDOUBLE(/*m_PositionProfile.GetGain()*/0.0);
		if (ok) ok = pFilter->Write(&ProfileRec);

	}

	CXaraFileRecord Rec(TAG_BLEND,TAG_BLEND_SIZE);

	UINT16 NumSteps = UINT16(m_NumBlendSteps);
	BYTE Flags =( m_OneToOne << 0)				| 
				((m_NotAntialiased == 0) << 1)	| 
				( m_Tangential << 2);

	
	ERROR3IF(BYTE(m_ColBlendType) > (1 << TAG_BLEND_COLEFFECT_SHIFT),"Col blend type will not fit into the flags byte");
	BYTE ColEffect = (BYTE(m_ColBlendType) << TAG_BLEND_COLEFFECT_SHIFT) & TAG_BLEND_COLEFFECT_MASK;

	Flags = Flags | ColEffect;

	if (ok) ok = Rec.Init();
	if (ok) ok = Rec.WriteUINT16(NumSteps);
	if (ok) ok = Rec.WriteBYTE(Flags);
	if (ok) ok = pFilter->Write(&Rec);


	

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

  >	virtual BOOL NodeBlend::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the blend record to the filter

  				WEBSTER - markn 29/1/97
				Changed so that it reacts to the "Convert blends to outlines" switch
				
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlend::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	if (pFilter->GetConvertBlendsToOutlines())
		return pFilter->WriteNodeAsOutlines(this);

	return WritePreChildren(pFilter);
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See NodeBlend::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL NodeBlend::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	if (pFilter->IsCompactNativeFilter())
	{
		if (pFilter->GetConvertBlendsToOutlines())
			return pFilter->WriteNodeAsOutlines(this);
	}

	return WritePreChildren(pFilter);
#else
	return FALSE;
#endif
}

/********************************************************************************************

  >	virtual BOOL NodeBlend::CanWriteChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/1/97
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Determines whether the blend's children are to be saved or not.

  				WEBSTER - markn 29/1/97
				
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlend::CanWriteChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT

	if (pFilter->GetConvertBlendsToOutlines())
		return FALSE;

	return TRUE;
#else
	return FALSE;
#endif
}

/********************************************************************************************

  >	virtual BOOL NodeBlend::CanWriteChildrenNative(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/1/97
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Determines whether the blend's children are to be saved or not.

  				WEBSTER - markn 29/1/97
				
	SeeAlso:	-

********************************************************************************************/

BOOL NodeBlend::CanWriteChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT

	if (pFilter->IsCompactNativeFilter())
	{
		if (pFilter->GetConvertBlendsToOutlines())
			return FALSE;
	}

	return TRUE;
#else
	return FALSE;
#endif
}



/********************************************************************************************

>	BOOL NodeBlend::ReinitialiseOnChildChanged(	UndoableOperation* pOp,
												NodeRenderableInk* pOldChild,
												NodeRenderableInk* pNewChild )
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08 September 2000
	Inputs:		pOp			we need an undoable operation to do this undoably.
				pOldChild	the node which the blend used to (currently) point to.
				pNewChild	the node which the blend should now point to.

	Returns:	TRUE if success, FALSE otherwise.

	Purpose:	Used by the shadowing code to reinitialise the blend when one of the blended
				nodes has had a shadow applied to it. In this case, the tree structure has
				changed, and the blend framework needs to know this.

	Errors:		ERROR2 if any parameters are NULL.

********************************************************************************************/
BOOL NodeBlend::ReinitialiseOnChildChanged(	UndoableOperation* pOp,
											NodeRenderableInk* pOldChild,
											NodeRenderableInk* pNewChild )
{
	// validate input params.
	ERROR2IF(pOp == NULL || pOldChild == NULL || pNewChild == NULL, FALSE,
			"NodeBlend::ReinitialiseOnChildChanged; NULL input parameters!");

	// iterate over our NodeBlenders, looking for a ptr to the old child.
	NodeBlender* pBlender = FindFirstBlender();
	while (pBlender != NULL)
	{
		// reinitialise the first blended node.
		if (pBlender->GetNodeStart() == pOldChild)
		{
			if (InitBlendAction::InitOnBlender(pOp, pOp->GetUndoActionList(), pBlender, TRUE, TRUE) == AC_FAIL)
			{
				ERROR3("NodeBlend::ReinitialiseOnChildChanged; couldn't init blend action");
			}

			pBlender->Reinit(pNewChild, NULL, FALSE);
		}

		// reinitialise the second blended node.
		// (should be 'else if' here, but for robustness, we'll just use 'if').
		if (pBlender->GetNodeEnd() == pOldChild)
		{
			if (InitBlendAction::InitOnBlender(pOp, pOp->GetUndoActionList(), pBlender, TRUE, TRUE) == AC_FAIL)
			{
				ERROR3("NodeBlend::ReinitialiseOnChildChanged; couldn't init blend action");
			}

			pBlender->Reinit(NULL, pNewChild, FALSE);
		}

		pBlender = FindNextBlender(pBlender);
	}

	return TRUE;
}



/********************************************************************************************

>	virtual DocRect NodeBlend::ValidateExtend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/11/1999
	Inputs:		ExtParams		extend description parameters.
	Outputs:	
	Returns:	TRUE if this node and its children cannot be validly extended,
				FALSE otherwise.
	Purpose:	Checks whether or not performing an extend operation on this node will leave
				it in a state from which it would be impossible to return.

				In the case of a NodeBlend, we consider its children, which will be the ones
				to move under a blend. Under extension, the choice of which child nodes to
				extend is fairly complicated - see NodeBlend::FindExtendingChildren for more
																				information.
	Errors:		
	See also:	NodeBlend::Extend, NodeBlend::FindExtendingChildren.

********************************************************************************************/
DocRect NodeBlend::ValidateExtend(const ExtendParams& ExtParams)
{
	// the routine for finding extending nodes may take a while, so check first
	// whether we're actually doing any extending, and if not, leave now.
	if (!(ExtParams.fExtendFlags & X_EXTEND) && !(ExtParams.fExtendFlags & Y_EXTEND))
		return DocRect(INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX);

	// okay, compile lists of child nodes to validate for extension.
	std::list<NodeRenderableInk*> lpInks;
	std::list<NodeBlendPath*> lpBlendPaths;
	FindExtendingChildren(lpInks, lpBlendPaths);

	// under non-stretch extension, a blend's child nodes behave in two ways.
	// if the child node is a blend-path, it is extended, and requires validation, as a path.
	// we test blend-paths first, as these are more likely to invalidate the extension.
	DocRect drMinExtend(INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX), drThisMinExtend;
	for (	std::list<NodeBlendPath*>::iterator iterPath = lpBlendPaths.begin();
			iterPath != lpBlendPaths.end();
			iterPath ++	)
	{
		drThisMinExtend = ((NodeBlendPath*)(*iterPath))->ValidateExtend(ExtParams);
		if (drMinExtend.lo.x > drThisMinExtend.lo.x) drMinExtend.lo.x = drThisMinExtend.lo.x;
		if (drMinExtend.lo.y > drThisMinExtend.lo.y) drMinExtend.lo.y = drThisMinExtend.lo.y;
		if (drMinExtend.hi.x > drThisMinExtend.hi.x) drMinExtend.hi.x = drThisMinExtend.hi.x;
		if (drMinExtend.hi.y > drThisMinExtend.hi.y) drMinExtend.hi.y = drThisMinExtend.hi.y;
	}

	// Karim 17/04/2000
	// We're changing things so that blended objects actually extend, rather than translate.
	// This _may_ cause some havoc with blend-on-a-path...

/*	// if it is any other ink node, it is translated as a whole, so its centre needs testing.
	INT32 i = 0;
	INT32 numInks = lpInks.size();
	DocCoord* doccArray = new DocCoord[numInks];
	for (	std::list<NodeRenderableInk*>::iterator iterInk = lpInks.begin();
			iterInk != lpInks.end();
			iterInk ++ )
	{
		doccArray[i ++] = ((NodeRenderableInk*)(*iterInk))->FindExtendCentre();
	}

	drThisMinExtend = Extender::ValidateControlPoints(numInks, doccArray, ExtParams);
	if (drMinExtend.lox > drThisMinExtend.lox) drMinExtend.lox = drThisMinExtend.lox;
	if (drMinExtend.loy > drThisMinExtend.loy) drMinExtend.loy = drThisMinExtend.loy;
	if (drMinExtend.hix > drThisMinExtend.hix) drMinExtend.hix = drThisMinExtend.hix;
	if (drMinExtend.hiy > drThisMinExtend.hiy) drMinExtend.hiy = drThisMinExtend.hiy;
	delete doccArray;
*/
	// if it is any other ink node, it is extended as if it is not even in a blend,
	// so we validate it as normal.
	/*size_t				numInks = */lpInks.size();
	for (	std::list<NodeRenderableInk*>::iterator iterInk = lpInks.begin();
			iterInk != lpInks.end();
			iterInk ++ )
	{
		drThisMinExtend = ((NodeRenderableInk*)(*iterInk))->ValidateExtend(ExtParams);
		if (drMinExtend.lo.x > drThisMinExtend.lo.x) drMinExtend.lo.x = drThisMinExtend.lo.x;
		if (drMinExtend.lo.y > drThisMinExtend.lo.y) drMinExtend.lo.y = drThisMinExtend.lo.y;
		if (drMinExtend.hi.x > drThisMinExtend.hi.x) drMinExtend.hi.x = drThisMinExtend.hi.x;
		if (drMinExtend.hi.y > drThisMinExtend.hi.y) drMinExtend.hi.y = drThisMinExtend.hi.y;
	}

	return drMinExtend;
}



/********************************************************************************************

>	virtual void NodeBlend::Extend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/11/1999
	Inputs:		ExtParams		extend description parameters.
	Outputs:	This node and its children may undergo change in the process of extension.
	Returns:	
	Purpose:	Applies an Extend operation to this node.
				If we're just stretching, then we simply do a stretch transform on ourself.
				If the extend operation involves any actual extension, we must determine
				which of the nodes lying below us we must extend. The child nodes which are
				extended will fall into two categories:
					* NodeBlendPaths will take care of themselves, via their Extend() fn.
					* Nodes which are blended between will be translated appropriately.
					(Karim 17/04/2000 - behaviour has changed so that Nodes instead have
										their Extend() methods called)
				Note however that the choice of which nodes to translate is a little complex;
													look at FindExtendingChildren() for info.
	Errors:		
	See also:	NodeBlend::FindExtendingChildren()

********************************************************************************************/
void NodeBlend::Extend(const ExtendParams& ExtParams)
{
	// try to perform a stretch first of all.
	TransformStretchObject(ExtParams);

	// figuring out which children to do what to is complicated and may take time,
	// so only continue if we _have_ to extend along either axis, otherwise quit now.
	if ( !(ExtParams.fExtendFlags & (X_EXTEND | Y_EXTEND)) )
		return;

	// okay, compile lists of child nodes which will be extended.
	std::list<NodeRenderableInk*> lpInks;
	std::list<NodeBlendPath*> lpBlendPaths;
	FindExtendingChildren(lpInks, lpBlendPaths);
	
	// extend the blend-paths.
	for (	std::list<NodeBlendPath*>::iterator iterPath = lpBlendPaths.begin();
			iterPath != lpBlendPaths.end();
			iterPath ++	)
	{
		((NodeBlendPath*)(*iterPath))->Extend(ExtParams);
	}

	// Karim 17/04/2000
	// We're changing things so that blended objects actually extend, rather than translate.
	// This _may_ cause some havoc with blend-on-a-path...

	// translate the objects.
	for (	std::list<NodeRenderableInk*>::iterator iterInk = lpInks.begin();
			iterInk != lpInks.end();
			iterInk ++ )
	{
//		((NodeRenderableInk*)(*iterInk))->TransformTranslateObject(ExtParams);
		((NodeRenderableInk*)(*iterInk))->Extend(ExtParams);
	}
}



/********************************************************************************************

>	void NodeBlend::FindExtendingChildren(	std::list<NodeRenderableInk*>& lpInks,
											std::list<NodeBlendPath*>& lpBlendPaths )

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/11/1999
	Inputs:		lpInks			a NodeRenderableInk list, to fill with translating objects.
				lpBlendPaths	a NodeBlendPath list, to fill with extending blend-paths.
	Outputs:	The two lists passed will be filled with pointers to Nodes in the tree.
	Returns:	
	Purpose:	Performs a search of the children below this blend node, returning a list of
				object nodes which should be translated in an extension, and another list of
				blend-path nodes, which should be extended appropriately.
				
				We travel down the tree, building a list of objects which belong to normal
				blender nodes but do not also belong to blender-on-a-path nodes, and a list
				of blender-on-a-path nodes also. This is a recursive algorithm, as blends can
				lie within other blends. This recursion is carried out in a loop within this
				function.
	Errors:		
	See also:	

********************************************************************************************/
void NodeBlend::FindExtendingChildren(	std::list<NodeRenderableInk*>& lpInks,
										std::list<NodeBlendPath*>& lpBlendPaths )
{
	std::list<NodeBlend*> lpBlends;				// a stack of blends, used for recursion.
	std::list<NodeGroup*> lpGroups;				// a stack of groups, used for recursion.

	// push ourself onto the blend stack, to start all the fun!
	lpBlends.push_back(this);

	NodeBlend* pNodeBlend = NULL;			//
	NodeGroup* pNodeGroup = NULL;			//
	NodeRenderableInk* pInk = NULL;			//
	NodeBlendPath* pNodeBlendPath = NULL;	//

	// loop until both stacks are empty.
	while (!lpBlends.empty() || !lpGroups.empty())
	{
		// deal with the blends first, as these are the complicated beasties.
		if (!lpBlends.empty())
		{
			// pull the next blend off our blend stack, for processing.
			pNodeBlend = lpBlends.front();
			lpBlends.pop_front();

			// process each of the blend's blender objects.
			// in our first pass, we add each blender's paths and objects to separate
			// lists. we need to discard any objects belonging to both a path-blender
			// and a normal blender, hence the two object-lists below.
			std::list<NodeRenderableInk*> lpBlendedObjects;
			std::list<NodeRenderableInk*> lpBlendPathObjects;
			NodeBlender* pBlender = pNodeBlend->FindFirstBlender();
			while (pBlender != NULL)
			{
				// does this blender follow a path?
				pNodeBlendPath = pBlender->GetNodeBlendPath();
				if (pNodeBlendPath != NULL)
				{
					// yes - stick its path on the list of paths to extend.
					lpBlendPaths.push_back(pNodeBlendPath);

					// note down its nodes, so they don't land on the extending list later.
					pInk = pBlender->GetNodeStart();
					if (pInk != NULL)			// should never happen, but test anyway.
						lpBlendPathObjects.push_back(pInk);
					pInk = pBlender->GetNodeEnd();
					if (pInk != NULL)			// should never happen, but test anyway.
						lpBlendPathObjects.push_back(pInk);
				}

				// no - ok, remember its nodes in our short-list of blended objects.
				else
				{
					pInk = pBlender->GetNodeStart();
					if (pInk != NULL)			// should never happen, but test anyway.
						lpBlendedObjects.push_back(pInk);
					pInk = pBlender->GetNodeEnd();
					if (pInk != NULL)			// should never happen, but test anyway.
						lpBlendedObjects.push_back(pInk);
				}

				pBlender = pNodeBlend->FindNextBlender(pBlender);
			}

			// ok, we now have a list of blended objects, some of which we must discard
			// if they are also blended along a path.
			lpBlendPathObjects.sort();
			lpBlendedObjects.sort();
			std::list<NodeRenderableInk*>::iterator iterNode = lpBlendedObjects.begin();
			while (!lpBlendPathObjects.empty() && !lpBlendPathObjects.empty())
			{
				// pop the next object to discard from the front of its list.
				pInk = lpBlendPathObjects.front();
				lpBlendPathObjects.pop_front();

				// move through the object-list until we find a pointer whose value
				// is not less than our discard-pointer.
				while (iterNode != lpBlendedObjects.end() && (*iterNode) < pInk)
					iterNode ++;

				// discard any elements from the object list whose pointer value
				// matches our discard pointer.
				while (iterNode != lpBlendedObjects.end() && (*iterNode) == pInk)
					iterNode = lpBlendedObjects.erase(iterNode);
			}

			// we move the remaining objects onto our group, blend and object lists.
			// if there were two or more blenders, then some nodes may occur twice
			// in our lists, so we test for these and only move them across once.
			pInk = NULL;
			while (!lpBlendedObjects.empty())
			{
				if (pInk == lpBlendedObjects.front())
				{
					lpBlendedObjects.pop_front();
					continue;
				}

				pInk = lpBlendedObjects.front();
				lpBlendedObjects.pop_front();

				if (IS_A(pInk, NodeGroup))			// another group on the groups stack.
					lpGroups.push_front((NodeGroup*)pInk);

				else if (IS_A(pInk, NodeBlend))				// don't think a blender can hold
					lpBlends.push_front((NodeBlend*)pInk);	// a blend directly, but only 99%
															// certain, so we check for it.

				else if (pInk->IsAnObject())		// wahey! got another object to extend.
					lpInks.push_back(pInk);
			}
		}

		// now groups - we add any renderable-ink children to the objects list,
		// and any group or blend children to the appropriate recursion list.
		if (!lpGroups.empty())
		{
			// we needn't worry about avoiding hidden nodes in this loop,
			// as we explicitly check the type of all nodes iterated over.
			pNodeGroup = lpGroups.front();
			lpGroups.pop_front();
			Node* pChild = pNodeGroup->FindFirstChild();
			while (pChild != NULL)
			{
				if (IS_A(pChild, NodeGroup))		// another group on the groups stack.
					lpGroups.push_front((NodeGroup*)pChild);

				else if (IS_A(pChild, NodeBlend))	// another blend on the blends stack.
					lpBlends.push_front((NodeBlend*)pChild);

				else if (pChild->IsAnObject())		// wahey! got another object to extend.
					lpInks.push_back((NodeRenderableInk*)pChild);

				pChild = pChild->FindNext();
			}
		}
	}
}



//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/********************************************************************************************

>	virtual UINT32* BlendRecordHandler::GetTagList()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/6/96
	Inputs:		-
	Returns:	Ptr to a list of tag values, terminated by CXFRH_TAG_LIST_END
	Purpose:	Provides the record handler system with a list of records handled by this
				handler
	SeeAlso:	-

********************************************************************************************/
UINT32* BlendRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {	TAG_BLEND,
								TAG_BLENDER,
								TAG_BLENDER_CURVEPROP,
								TAG_BLENDER_CURVEANGLES,
								TAG_BLEND_PATH,
								TAG_BLENDPROFILES,
								TAG_BLENDERADDITIONAL,
								TAG_NODEBLENDPATH_FILLED,
								CXFRH_TAG_LIST_END};

	return (UINT32*)&TagList;
}



/********************************************************************************************

>	virtual BOOL BlendRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
	SeeAlso:	-

********************************************************************************************/
BOOL BlendRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");

	BOOL ok = TRUE;

	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_BLEND:
			ok = HandleBlendRecord(pCXaraFileRecord);
			break;

		case TAG_BLENDER:
			ok = HandleBlenderRecord(pCXaraFileRecord);
			break;

		case TAG_BLENDER_CURVEPROP:
			ok = HandleBlenderCurvePropRecord(pCXaraFileRecord);
			break;
		
		case TAG_BLENDER_CURVEANGLES:
			ok = HandleBlenderCurveAnglesRecord(pCXaraFileRecord);
			break;
		
		case TAG_BLEND_PATH:
			ok = HandleBlendPathRecord(pCXaraFileRecord);
			break;
		case TAG_NODEBLENDPATH_FILLED:
			ok = HandleBlendPathFlags(pCXaraFileRecord);
			break;
		case TAG_BLENDPROFILES:
			ok = HandleBlendProfileRecord(pCXaraFileRecord);
			break;
		case TAG_BLENDERADDITIONAL:
			ok = HandleBlenderExtraRecord(pCXaraFileRecord);
			break;
		default:
			ok = FALSE;
			ERROR3_PF(("I don't handle records with the tag (%d)\n",pCXaraFileRecord->GetTag()));
			break;
	}

	return ok;
}



/********************************************************************************************

BOOL BlendRecordHandler::HandleBlendRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a blend record
	SeeAlso:	-

********************************************************************************************/
BOOL BlendRecordHandler::HandleBlendRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_BLEND,FALSE,"I don't handle this tag type");

	UINT16 NumSteps = 0;
	BYTE Flags = 0;

	BOOL	ok = pCXaraFileRecord->ReadUINT16(&NumSteps);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&Flags);

	if (ok)
	{
		NodeBlend* pBlend = new NodeBlend;
		if (pBlend != NULL)
		{
			BOOL OneToOne  =  Flags & TAG_BLEND_FLAG_ONETOONE;
			BOOL Antialias =  Flags & TAG_BLEND_FLAG_ANTIALIAS;
			BOOL Tangential=  Flags & TAG_BLEND_FLAG_TANGENTIAL;
			BYTE ColEffect = (Flags & TAG_BLEND_COLEFFECT_MASK) >> TAG_BLEND_COLEFFECT_SHIFT;

			pBlend->SetOneToOne(OneToOne != 0);
			pBlend->SetNotAntialiased(Antialias == 0);
			pBlend->SetTangential(Tangential != 0);
			pBlend->SetNumBlendSteps(NumSteps);
			pBlend->SetColourBlendType(ColourBlendType(ColEffect));

			ok = InsertNode(pBlend);
			if (ok) 
				SetLastInsertedNodeBlend(pBlend);

			// load the profiles, if we have some
			if (m_bLoadedProfiles && ok)
			{
				CProfileBiasGain* pObjProfile = pBlend->GetObjectProfile();
				if (pObjProfile == NULL)
					ok = FALSE;
				else
				{
					pObjProfile->SetBiasGain(m_LastObjectProfile.GetBias(), m_LastObjectProfile.GetGain());
					pBlend->RequestObjectProfileProcessing (TRUE);
				}
				if (ok)
				{
					CProfileBiasGain* pAttrProfile = pBlend->GetAttrProfile();
					if (pAttrProfile == NULL)
						ok = FALSE;
					else
					{
						pAttrProfile->SetBiasGain(m_LastAttrProfile.GetBias(), m_LastAttrProfile.GetGain());
						pBlend->RequestObjectProfileProcessing (TRUE);
					}
				}
				if (ok)
				{
					CProfileBiasGain* pPosProfile = pBlend->GetPositionProfile();
					if (pPosProfile == NULL)
						ok = FALSE;
					else
						pPosProfile->SetBiasGain(m_LastPositionProfile.GetBias(), m_LastPositionProfile.GetGain());
				}
			}


		}
		else
			ok = FALSE;
	}
	
	m_bLoadedProfiles = FALSE;
	return ok;
}



/********************************************************************************************

BOOL BlendRecordHandler::HandleBlenderRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a blender record
	SeeAlso:	-

********************************************************************************************/
BOOL BlendRecordHandler::HandleBlenderRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_BLENDER,FALSE,"I don't handle this tag type");

	INT32 PathIndexStart= -1;
	INT32 PathIndexEnd  = -1;

	BOOL	ok = pCXaraFileRecord->ReadINT32(&PathIndexStart);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&PathIndexEnd);

	if (ok)
	{
		NodeBlender* pBlender = new NodeBlender;
		if (pBlender != NULL)
		{
			pBlender->SetPathIndexStart(PathIndexStart);
			pBlender->SetPathIndexEnd(PathIndexEnd);

			ok = InsertNode(pBlender);
			if (ok) SetLastInsertedNodeBlender(pBlender);
		
		}
		else
			ok = FALSE;
	}

	return ok;
}



/********************************************************************************************

>	BOOL BlendRecordHandler::HandleBlenderCurvePropRecord(CXaraFileRecord* pCXaraFileRecord);

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/99
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a blender curve proportion record
	SeeAlso:	-

********************************************************************************************/
BOOL BlendRecordHandler::HandleBlenderCurvePropRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_BLENDER_CURVEPROP,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;
	NodeBlender* pNodeBlender = GetLastInsertedNodeBlender();

	if (pNodeBlender != NULL)
	{
		double ProportionOfPathDistStart = -1.0;
		double ProportionOfPathDistEnd = -1.0;

				ok = pCXaraFileRecord->ReadDOUBLE(&ProportionOfPathDistStart);
		if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&ProportionOfPathDistEnd);

		if (ok)
		{
			pNodeBlender->SetProportionOfPathDistStart(ProportionOfPathDistStart);
			pNodeBlender->SetProportionOfPathDistEnd(ProportionOfPathDistEnd);
		}
	}

	return ok;
}



/********************************************************************************************

>	BOOL BlendRecordHandler::HandleBlenderCurveAnglesRecord(CXaraFileRecord* pCXaraFileRecord);

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/99
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a blender curve angles record
	SeeAlso:	-

********************************************************************************************/
BOOL BlendRecordHandler::HandleBlenderCurveAnglesRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_BLENDER_CURVEANGLES,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;
	NodeBlender* pNodeBlender = GetLastInsertedNodeBlender();

	if (pNodeBlender != NULL)
	{
		double AngleStart = 0.0;
		double AngleEnd   = 0.0;

				ok = pCXaraFileRecord->ReadDOUBLE(&AngleStart);
		if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&AngleEnd);

		if (ok)
		{
			pNodeBlender->SetAngleStart(AngleStart);
			pNodeBlender->SetAngleEnd(AngleEnd);
		}
	}

	return ok;
}



/********************************************************************************************

>	BOOL BlendRecordHandler::HandleBlendPathRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/99
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a blend path record
	SeeAlso:	-

********************************************************************************************/
BOOL BlendRecordHandler::HandleBlendPathRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_BLEND_PATH,FALSE,"I don't handle this tag type");

	BOOL ok = FALSE;

	NodeBlendPath* pNodeBlendPath = new NodeBlendPath;

	if (pNodeBlendPath != NULL && pNodeBlendPath->SetUpPath())
	{
		ok = pCXaraFileRecord->ReadPath(&pNodeBlendPath->InkPath); 
		if (ok) pNodeBlendPath->InkPath.InitialiseFlags();	// Init the path flags array to something sensible
	 	if (ok)	ok = InsertNode(pNodeBlendPath);
		if (ok) 
		{
			SetLastNodePathInserted(pNodeBlendPath);
			SetLastInsertedNodeBlendPath(pNodeBlendPath);
		}
		if (ok)
		{
			Node* pParent = pNodeBlendPath->FindParent();
			if ((pParent != NULL) && (pParent->IS_KIND_OF(NodeBlend)))
			{
				NodeBlend* pNodeBlend = (NodeBlend*)pParent;
				pNodeBlend->SetBlendedOnCurve(TRUE);
				pNodeBlend->SetNumNodeBlendPathsInc(TRUE);
				
				// this deals with the case of older files that contain a blend on a curve.
				// these blends will contain only one NBP and all blenders will refer to that 
				// curve.  
				// We can tell the old files because their Index values will not have been changed
				// from the default value of -2.
				NodeBlender* pBlender = pNodeBlend->FindFirstBlender();
				while (pBlender != NULL)
				{
					if (pBlender->GetNodeBlendPathIndex() == -2)
						pBlender->SetNodeBlendPathIndex(0);

					pBlender = pNodeBlend->FindNextBlender(pBlender);
				}
			}
		}
	}

	return ok;
}



/********************************************************************************************

>	BOOL BlendRecordHandler::HandleNodeBlendPathFlags(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/99
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a blend path filled record
	SeeAlso:	-

********************************************************************************************/
BOOL BlendRecordHandler::HandleBlendPathFlags(CXaraFileRecord* pXFRecord)
{
	ERROR2IF(pXFRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pXFRecord->GetTag() != TAG_NODEBLENDPATH_FILLED,FALSE,"I don't handle this tag type");

	INT32 Flag;
	BOOL ok		= pXFRecord->ReadINT32(&Flag);
	
	if (ok)
	{
		NodeBlendPath* pNodeBlendPath = GetLastInsertedNodeBlendPath();
		ERROR2IF(pNodeBlendPath == NULL, FALSE, "No NodeBlendPath");

		pNodeBlendPath->InkPath.IsFilled = Flag;
	}

	return ok;
}



/********************************************************************************************

>	BOOL BlendRecordHandler::HandleBlenderExtraRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/99
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a a blender extra info record
				(exact contents yet to be finalised).
	SeeAlso:	-

	Notes:		Karim 05/02/2001
				Added BitField parameter to extra blend record.
				This stores whether or not a blend-on-path is reversed.
				Unfortunately, the extra information needs to go at the end of
				an existing record, so must use the dangerous ReadBYTEnoError() fn.

********************************************************************************************/
BOOL BlendRecordHandler::HandleBlenderExtraRecord(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pRecord->GetTag() != TAG_BLENDERADDITIONAL,FALSE,"I don't handle this tag type");

	INT32 Flag;
	INT32 Index;
	INT32 ObjIndexStart;
	INT32 ObjIndexEnd;
	BYTE BitField;

	BOOL ok		= pRecord->ReadINT32(&Flag);
	if (ok)  ok = pRecord->ReadINT32(&Index);
	if (ok)  ok = pRecord->ReadINT32(&ObjIndexStart);
	if (ok)  ok = pRecord->ReadINT32(&ObjIndexEnd);
	if (ok)
	{
		// if this fn fails, it means we read in an old file without BitField
		// info, so we arbitrarily set it to 0.
		if (!pRecord->ReadBYTEnoError(&BitField))
			BitField = 0;
	}

	if (ok)
	{
		NodeBlender* pBlender = GetLastInsertedNodeBlender();
		ERROR2IF(pBlender == NULL, FALSE, "No NodeBlender");

		pBlender->SetBlendedOnCurve(Flag);
		pBlender->SetNodeBlendPathIndex(Index);
		pBlender->SetObjIndexStart(ObjIndexStart);
		pBlender->SetObjIndexEnd(ObjIndexEnd);
		pBlender->SetReversed((BitField & 1) == 1);

		NodeBlend* pNodeBlend = pBlender->GetNodeBlend();
		ERROR2IF(pNodeBlend == NULL, FALSE, "Parent is not NodeBlend");
		
		INT32 CurrentNodeBlendPaths = (INT32)pNodeBlend->GetNumNodeBlendPaths();
		if (Index >  CurrentNodeBlendPaths)
			pNodeBlend->SetNumNodeBlendPaths(Index+1);
		
	}
	return ok;
}



/********************************************************************************************

>	BOOL BlendRecordHandler::HandleBlendProfileRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/99
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a blend profile record
	SeeAlso:	-

	NOTE:      12/7/2000 in order to fix a bug loading v.3 blends into the browser plugin I had
			   to reverse the order in which blend data was saved.  Thus in order to load it in
			   again we have to cache the profile data and wait for our next blend to be loaded.
			   The cause of the plug-in problem is still unresolved...
********************************************************************************************/
BOOL BlendRecordHandler::HandleBlendProfileRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_BLENDPROFILES,FALSE,"I don't handle this tag type");

	double ObjBias = 0.0;
	double ObjGain = 0.0;
	double AttrBias = 0.0;
	double AttrGain = 0.0;
	double PosBias = 0.0;
	double PosGain = 0.0;

	BOOL	ok = pCXaraFileRecord->ReadDOUBLE(&ObjBias);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&ObjGain);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&AttrBias);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&AttrGain);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&PosBias);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&PosGain);

	m_LastObjectProfile.SetBiasGain(ObjBias, ObjGain);
	m_LastAttrProfile.SetBiasGain(AttrBias, AttrGain);
	m_LastPositionProfile.SetBiasGain(PosBias, PosGain);

	m_bLoadedProfiles = TRUE;

	return ok;
}



/********************************************************************************************

>	virtual void BlendRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/6/96
	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update
	Returns:	-
	Purpose:	This provides descriptions for the blend records.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#if XAR_TREE_DIALOG
void BlendRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	char s[256];

	//	 Call base class first
	CamelotRecordHandler::GetRecordDescriptionText(pRecord,pStr);

	switch (pRecord->GetTag())
	{
		case TAG_BLEND:
		{
			UINT16 NumSteps = 0;
			BYTE Flags = 0;

			pRecord->ReadUINT16(&NumSteps);
			pRecord->ReadBYTE(&Flags);

			_stprintf(s,"NumSteps\t= %hu\r\n",NumSteps);
			(*pStr) += s;
			_stprintf(s,"Flags\t\t= %d\r\n",INT32(Flags));
			(*pStr) += s;
			(*pStr) += "\r\n";

			_stprintf(s,"One to One\t= %d\r\n",Flags & TAG_BLEND_FLAG_ONETOONE);
			(*pStr) += s;
			_stprintf(s,"Antialiased\t= %d\r\n",Flags & TAG_BLEND_FLAG_ANTIALIAS);
			(*pStr) += s;
			_stprintf(s,"Tangential\t= %d\r\n",Flags & TAG_BLEND_FLAG_TANGENTIAL);
			(*pStr) += s;

			BYTE ColEffect = (Flags & TAG_BLEND_COLEFFECT_MASK) >> TAG_BLEND_COLEFFECT_SHIFT;
			_stprintf(s,"Colour Effect\t= %d\r\n",INT32(ColEffect));
			(*pStr) += s;
		}
		break;

		case TAG_BLENDER:
		{
			INT32 PathIndexStart = -1;
			INT32 PathIndexEnd = -1;

			pRecord->ReadINT32(&PathIndexStart);
			pRecord->ReadINT32(&PathIndexEnd);

			_stprintf(s,"PathIndexStart\t= %d\r\n",PathIndexStart);
			(*pStr) += s;
			_stprintf(s,"PathIndexEnd\t= %d\r\n",PathIndexEnd);
			(*pStr) += s;

		}
		break;

		case TAG_BLENDER_CURVEPROP:
		{
			double PropStart = -1.0;
			double PropEnd   = -1.0;
			pRecord->ReadDOUBLE(&PropStart);
			pRecord->ReadDOUBLE(&PropEnd);
			_stprintf(s,"Proportion of path distance start\t= %f\r\n",PropStart);
			(*pStr) += s;
			_stprintf(s,"Proportion of path distance end\t= %f\r\n",PropEnd);
			(*pStr) += s;
		}
		break;

		case TAG_BLENDER_CURVEANGLES:
		{
			double AngleStart = -1.0;
			double AngleEnd   = -1.0;
			pRecord->ReadDOUBLE(&AngleStart);
			pRecord->ReadDOUBLE(&AngleEnd);
			_stprintf(s,"Angle start\t= %f\r\n",AngleStart);
			(*pStr) += s;
			_stprintf(s,"Angle end\t= %f\r\n",AngleEnd);
			(*pStr) += s;
		}
		break;

		case TAG_BLEND_PATH:
			// Use standard base class func for describing the path textually
			DescribePath(pRecord,pStr);
			break;
		case TAG_BLENDERADDITIONAL:
		{
			INT32 BlendedOnCurve = FALSE;
			pRecord->ReadINT32(&BlendedOnCurve);
			_stprintf(s,"BlendedOnCurve\t= %f\r\n",BlendedOnCurve);
			(*pStr) += s;
		}
		break;
		case TAG_NODEBLENDPATH_FILLED:
		{
			INT32 Filled = FALSE;
			pRecord->ReadINT32(&Filled);
			_stprintf(s,"Filled \t= %f\r\n",Filled);
			(*pStr) += s;
		}
		break;
		case TAG_BLENDPROFILES:
			double ObjBias = 0.0;
			double ObjGain = 0.0;
			double AttrBias = 0.0;
			double AttrGain = 0.0;
			double PosBias = 0.0;
			double PosGain = 0.0;
			BOOL ok    = pRecord->ReadDOUBLE(&ObjBias);
			if (ok) 
			{
				_stprintf(s,"Object Bias\t= %f\r\n",ObjBias);
				(*pStr) += s;
				ok = pRecord->ReadDOUBLE(&ObjGain);
			}
			if (ok) 
			{
				_stprintf(s,"Object Gain\t= %f\r\n",ObjGain);
				(*pStr) += s;	
				ok = pRecord->ReadDOUBLE(&AttrBias);
			}
			if (ok) 
			{
				_stprintf(s,"Attribute Bias\t= %f\r\n",AttrBias);
				(*pStr) += s;
				ok = pRecord->ReadDOUBLE(&AttrGain);
			}
			if (ok) 
			{
				_stprintf(s,"Attribute Gain\t= %f\r\n",AttrGain);
				(*pStr) += s;
				ok = pRecord->ReadDOUBLE(&PosBias);
			}
			if (ok) 
			{
				_stprintf(s,"Position Bias\t= %f\r\n",PosBias);
				(*pStr) += s;
				ok = pRecord->ReadDOUBLE(&PosGain);
			}
			if (ok)
			{
				_stprintf(s,"Position Gain\t= %f\r\n",PosGain);
				(*pStr) += s;
			}
			break;
	}
}
#endif
