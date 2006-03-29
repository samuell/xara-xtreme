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
*/

#include "camtypes.h"
#include "app.h"
#include "node.h"
#include "ink.h"
#include "nodecomp.h"
#include "objchge.h"

// Resource headers
//#include "resource.h"

// operation headers
#include "transop.h"
#include "attrappl.h"

// attribute headers
#include "lineattr.h"
#include "txtattr.h"
#include "fillattr.h"
#include "attrbev.h"
#include "cutop.h"
#include "nodetxts.h"
#include "nodetext.h"
#include "opbevel.h"
#include "textops.h"
#include "blobs.h"
#include "docview.h"
#include "spread.h"
#include "nodebldr.h"
#include "blendatt.h"
//#include "mario.h"
#include "pathops.h"

DECLARE_SOURCE( "$Revision: 662 $" );

CC_IMPLEMENT_DYNCREATE(CompoundNodeTreeFactory, CCObject)
CC_IMPLEMENT_DYNCREATE(NodeCompound, NodeRenderableInk)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

// static variables that are used by the blending system when blending compound nodes ....

NodeCompound* NodeCompound::shadowThisNode = NULL;
NodeCompound* NodeCompound::shadowDeleteThis = NULL;

List* NodeCompound::blndConsBecomeAShadows = NULL;		// list of shadow controllers that were generated when blending (internally via PASSBACK)
List* NodeCompound::blndConsBecomeABevels = NULL;		// list of bevel controllers that were generated when blending (internally via PASSBACK)
List* NodeCompound::blndConsBecomeAContours = NULL;	// list of contour controllers that were generated when blending (internally via PASSBACK)

BOOL NodeCompound::isForPrinting = FALSE;

#ifdef _DEBUG

INT32 NodeCompound::shadowLastID = 0;
INT32 NodeCompound::bevelLastID = 0;
INT32 NodeCompound::contourLastID = 0;

INT32 NodeCompound::shadowLastBecomeAID = 0;
INT32 NodeCompound::bevelLastBecomeAID = 0;
INT32 NodeCompound::contourLastBecomeAID = 0;

#endif

/***********************************************************************************************

>	NodeCompound::NodeCompound(Node* 	ContextNode,
						AttachNodeDirection Direction,
						const DocRect& 		BoundingRect,
						BOOL				Locked = FALSE,
						BOOL 				Mangled = FALSE,
						BOOL 				Marked = FALSE,
						BOOL 				Selected = FALSE,
						BOOL 				Renderable = FALSE
						)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/99
	Inputs:		ContextNode: Pointer to a node which this node is to be attached to.
				MonoOn Direction: MonoOff
				Specifies the direction in which the node is to be attached to the
				ContextNode. The values this variable can take are as follows:
								  
				PREV      : Attach node as a previous sibling of the context node
				NEXT      : Attach node as a next sibling of the context node
				FIRSTCHILD: Attach node as the first child of the context node
				LASTCHILD : Attach node as a last child of the context node

				BoundingRect: Bounding rectangle

				The remaining inputs specify the status of the node:
			
				Locked:     Is node locked ?
				Mangled:    Is node mangled ?
				Marked:     Is node marked ?
				Selected:   Is node selected ?

	Purpose:	This constructor initialises the nodes flags and links it to ContextNode in the
				direction specified by Direction. All neccesary tree links are updated.
	Note:		SetUpShape() must be called before the NodeRegularShape is in a state in which
				it can be used.
    SeeAlso:	NodeRegularShape::SetUpShape
    Errors:		An ENSURE will occur if ContextNode is NULL

***********************************************************************************************/
NodeCompound::NodeCompound(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
					BOOL Marked, 
					BOOL Selected    
			  ) : NodeRenderableInk(ContextNode, Direction, Locked, Mangled, Marked, Selected )
{                         
	m_DPI = 96.0;
	m_bPrinting = FALSE;

	// blending attributes
	m_bBlendStartNode = FALSE;
	m_bBlendEndNode = FALSE;
	m_pBlendCreatedByNode = NULL;
}                        
 
/*********************************************************************************************

>	NodeCompound::NodeCompound() 

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/99
	Purpose:	
	Note:		
	SeeAlso:	

**********************************************************************************************/
NodeCompound::NodeCompound() : NodeRenderableInk()
{
	m_DPI = 96.0;
	m_bPrinting = FALSE;
	m_bBlendStartNode = FALSE;
	m_bBlendEndNode = FALSE;
	m_pBlendCreatedByNode = NULL;
}

/*********************************************************************************************

>	NodeCompound::~NodeCompound() 

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/99
	Purpose:	Destructor
	Note:		
	SeeAlso:	

**********************************************************************************************/
NodeCompound::~NodeCompound()
{

}

/*********************************************************************************************

>	ChangeCode NodeCompound::OnChildChange(ObjChangeParam* pParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/99
	Purpose:	Processes child change messages
	Note:		
	SeeAlso:	

**********************************************************************************************/
ChangeCode NodeCompound::OnChildChange(ObjChangeParam* pParam)
{
//#pragma message( __LOCMSG__ "NodeCompound::OnChildChange - do nothing" )
//	TRACE( _T("Warning - NodeCompound::OnChildChange called\n") );
//	return CC_FAIL;
	if (IsNodeHidden())
		return CC_OK;
	
	ERROR2IF(pParam == NULL,CC_FAIL,"pParam == NULL");

	BOOL bRegen = TRUE;
	BOOL bCache = FALSE;

//	Node * pChild = NULL;

//	Node * pThis = this;

//	Node * pNextChild = NULL;

	UndoableOperation* pOp = NULL;
	if (pParam->GetOpPointer())
	{
		if (pParam->GetOpPointer()->IsKindOf(CC_RUNTIME_CLASS(UndoableOperation)))
		{
			pOp = pParam->GetOpPointer();
		}
	}

//	CCRuntimeClass * pClass = NULL;

//	ActionList * pActionList = NULL;

//	Action *pAction = NULL;

//	BOOL bDelete = FALSE;
//	INT32 NumChildrenThatNeedMe = 0;

	// the last code failled to give a spread for nodes recently divorced
	Spread * pSpread = Document::GetSelectedSpread(); //(Spread *)FindParent(CC_RUNTIME_CLASS(Spread));
	ERROR2IF(pSpread==NULL, CC_FAIL, "Can't find selected spread");

	List TextNodeList;
	NodeListItem * pItem = NULL;

	DocRect Bounds = BoundingRectangle;

	BlobManager* BlobMgr = GetApplication()->GetBlobManager();
//	INT32 BlobSize = 0;

	if (!pOp && pParam->GetChangeType() == OBJCHANGE_FINISHED &&
		pParam->GetDirection() == OBJCHANGE_CALLEDBYCHILD &&
		pParam->GetChangeFlags().RegenerateNode)
	{
		RegenerateNode(NULL, FALSE, FALSE);
		return CC_OK;
	}
		
	if (BlobMgr)
	{
		BlobMgr->GetBlobSize();
	}

	if (pParam->GetChangeType() == OBJCHANGE_STARTING)
	{
		
	}
	else if (pParam->GetChangeType() == OBJCHANGE_FINISHED)
	{
		if (pParam->GetChangeFlags().DeleteNode)
		{
			TRACEUSER( "DavidM", _T("Delete node\n"));
		}
		
		if (pOp != NULL)
		{ 
			// Ok! Tell all my children to regenerate (in some cases !)

			// check for ops which we do nothing with
			if (pOp->IsKindOf(CC_RUNTIME_CLASS(TransOperation)) &&
				!pOp->IS_KIND_OF(OpMovePathPoint))
			{
				// ok, we have a transform operation
				TRACEUSER( "DavidM", _T("Transform operation!"));

				if (((TransOperation *)pOp)->GetCurrentMatrix().IsTranslation())
				{
					// find out if the selected object is a child of myself
					Range Sel(*(GetApplication()->FindSelection()));

					Node * pNode = Sel.FindFirst(FALSE);

					bRegen = TRUE;

					while (pNode && bRegen)
					{
						if (pNode->FindParent())
						{
							if (pNode->FindParent()->ShouldITransformWithChildren())
							{
								bRegen = FALSE;
							}
						}

						pNode = Sel.FindNext(pNode, FALSE);
					}
				}
				else
				{
					// re-format all text nodes under me
					ReleaseCached();
					pOp->DoInvalidateRegion(pSpread, BoundingRectangle);

					BevelTools::GetAllNodesUnderNode(this, &TextNodeList, 
						CC_RUNTIME_CLASS(TextStory));

					pItem = (NodeListItem *)TextNodeList.GetHead();

					while (pItem)
					{
						if (pItem->pNode)
						{
PORTNOTE("text","Removed PrePostTextAction::DoFormatStory usage")
#ifndef EXCLUDE_FROM_XARALX
							PrePostTextAction::DoFormatStory(pOp, (TextStory *) pItem->pNode, TRUE);
#endif
						}

						pItem = (NodeListItem *)TextNodeList.GetNext(pItem);
					}

					TextNodeList.DeleteAll();
								
					bRegen = TRUE;
					bCache = FALSE;
				}
			}
			else if (pOp->IsKindOf(CC_RUNTIME_CLASS(OpApplyAttrib)))
			{
				// has there been a change which causes a geometry change ?
				// if so, we need to regen
				CCRuntimeClass * pClass = ((OpApplyAttrib *)pOp)->GetValueChangeType();

				if (pClass)
				{
					if (pClass->IsKindOf(CC_RUNTIME_CLASS(AttrTxtBase)) ||
						pClass->IsKindOf(CC_RUNTIME_CLASS(AttrBevelIndent)) ||
						pClass->IsKindOf(CC_RUNTIME_CLASS(AttrTranspFillGeometry)) ||
						pClass->IsKindOf(CC_RUNTIME_CLASS(AttrLineWidth)) ||
						pClass->IsKindOf(CC_RUNTIME_CLASS(AttrStrokeColour)) ||
						pClass->IsKindOf(CC_RUNTIME_CLASS(AttrStartCap)) ||
						pClass->IsKindOf(CC_RUNTIME_CLASS(AttrStartArrow)) ||
						pClass->IsKindOf(CC_RUNTIME_CLASS(AttrDashPattern)) ||
						pClass->IsKindOf(CC_RUNTIME_CLASS(AttrEndArrow)) ||
						pClass->IsKindOf(CC_RUNTIME_CLASS(AttrJoinType)) ||
						pClass->IsKindOf(CC_RUNTIME_CLASS(AttrStrokeColourChange)) ||
						pClass->IsKindOf(CC_RUNTIME_CLASS(AttrStrokeTranspChange)) ||
						pClass->IsKindOf(CC_RUNTIME_CLASS(AttrStrokeTranspTypeChange))
						)
					{
						bRegen = TRUE;
					}
					else
					{
						bRegen = FALSE;
					
					}
				}
			}
			// this operation is called for changes in fills (e.g. dragging a linear fill blob,
			// and then dropping it
			// we need to stop regeneration for normal fills, but regen for transparency fills
			else if (pOp->IsKindOf(CC_RUNTIME_CLASS(OpReplaceAttributes)))
			{
				// find out the op name
				String_256 OpName;

				// don't do colour fills, but do transparent fills
				NodeAttribute * pAttr = ((OpReplaceAttributes *)pOp)->GetAttribute();

				if (pAttr)
				{
					if (pAttr->IsAFillAttr())
					{
						bRegen = FALSE;
					}

					if (pAttr->IsATranspFill())
					{
						bRegen = TRUE;
					}
						
				}
			}
			else if (pOp->IsKindOf(CC_RUNTIME_CLASS(OpDelete)))
			{
				bRegen = FALSE;
			}
			else if (pOp->IsKindOf(CC_RUNTIME_CLASS(OpCopy)))
			{
				bRegen = FALSE;
			}
			else if (pOp->IsKindOf(CC_RUNTIME_CLASS(OpCut)))
			{
				bRegen = FALSE;
			}
			else if (pOp->IsKindOf(CC_RUNTIME_CLASS(OpPaste)))
			{
				bRegen = FALSE;
			}
PORTNOTE("text","Removed OpTextFormat usage")
#ifndef EXCLUDE_FROM_XARALX
			else if (pOp->IsKindOf(CC_RUNTIME_CLASS(OpTextFormat)))
			{
				// indicates a change in the text (e.g. the user has typed in)
				// find out if my children have all disappeared
				// invalidate and hide me
				Bounds.lo.x = BoundingRectangle.lo.x - BlobSize;
				Bounds.lo.y = BoundingRectangle.lo.y - BlobSize;
				Bounds.hi.x = BoundingRectangle.hi.x + BlobSize;
				Bounds.hi.y = BoundingRectangle.hi.y + BlobSize;
				
				ReleaseCached();
				pOp->DoInvalidateRegion(pSpread, Bounds);

				// regen if we've still got a valid interior
				if (GetInsideBoundingRect().IsEmpty())
				{
					bRegen = FALSE;
				}
				else
				{
					bRegen = TRUE;
				}

				bCache = TRUE;
			}
#endif
PORTNOTE("text","Removed OpDeleteTextStory usage")
#ifndef EXCLUDE_FROM_XARALX
			else if (pOp->IsKindOf(CC_RUNTIME_CLASS(OpDeleteTextStory)))
			{
				// indicates that a text story has been deleted - so if my inside
				// bounding rect is empty then hide me
				if (GetInsideBoundingRect().IsEmpty())
				{
					// hide me !
					pOp->DoHideNode(this, TRUE, NULL, TRUE);
					bRegen = FALSE;
				}
			}
#endif
			else
			{
				// regen for all other ops
				bRegen = TRUE;
			}

			if (bRegen)
			{
//				Document * pDoc = Document::GetCurrent();

				pOp->DoInvalidateRegion(pSpread, Bounds);	// Invalidate original bounds

				BOOL bDidRegen = RegenerateNode(pOp, bCache, FALSE);	// ForceRedraw is called internally by RegenerateNode...

				if (bDidRegen)
				{
					ReleaseCached();
					pOp->DoInvalidateRegion(pSpread, GetBoundingRect());
				}
			}
		}
		else
		{
			// were being called by with a pOp of NULL (i.e.  the change is occuring
			// in a none undoable way)

			// NOTE:  we need to regenerate here as well, because we will be called with
			// a pOp of NULL when being called through OpRepeatApplyAttribToSelected ....

//			Document * pDoc = Document::GetCurrent();

			RegenerateNode(NULL, bCache, FALSE);	// ForceRedraw is called internally by RegenerateNode...
			
			// we should also probably check whether it really is OpRepeatApplyAttribToSelected
			// that is causing this block of code to execute (BUT, there is no way of doing this!)
		}		
	}

	// exit (bearing in mind this has probably been called by my inherited classes)
	return NodeRenderableInk::OnChildChange(pParam);
}

/*********************************************************************************************

>	BOOL NodeCompound::RegenChildren()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/99
	Purpose:	Calls all the regenerate functions on all children (bottom to top)
	Note:		
	SeeAlso:	

**********************************************************************************************/
/*
BOOL NodeCompound::RegenChildren(UndoableOperation * pOp)
{
	// runs through all children doing a regeneration
	RegenerateNode(pOp);

	return TRUE;
}
*/

/*********************************************************************************************

>	BOOL NodeCompound::RegenerateNode(Node * pNode, UndoableOperation * pOp)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/99
	Purpose:	Recursive function to regenerate all nodes
	Note:		
	SeeAlso:	

**********************************************************************************************/
/*
BOOL NodeCompound::RegenerateNode(Node * pNode, UndoableOperation * pOp)
{
	Node * pChildNode = pNode->FindFirstChild();

	while (pChildNode)
	{
		if (!RegenerateNode(pChildNode, pOp))
			return FALSE;

		pChildNode = pChildNode->FindNext();
	}

	return pNode->RegenerateNode(pOp);
}
*/



/********************************************************************************************

>	void NodeCompound::SetSelected(BOOL Status)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/07/2000
	Inputs:		Status		whether we should become selected or deselected.

	Outputs:	If (Status) then all children are selected.

	Purpose:	Selects this node.
				Note that in most cases you'll want to call Select() or DeSelect(), which
				are virtual methods in NodeRenderable.

				This override of Node::SetSelected() contains special case code for the
				usual suspects - shadows, bevels and contours - which sometimes behave like
				groups, but try to fool the user into thinking they aren't. Any attempt to
				select NodeBevelController or its sisters of murphy will result in their
				*children* being selected instead.

				It's generally better to use Ranges with PromoteToParent to do rendering
				and similar with these nodes - eg to iterate over the selection, don't step
				through the tree, querying each node - use a SelRange instead. It should
				be quicker, and you can choose to have controller nodes automatically
				included.

********************************************************************************************/
void NodeCompound::SetSelected(BOOL Status)
{
	// normal nodes get a normal set-selected.
	if (PromoteHitTestOnChildrenToMe() || !Status)
		Node::SetSelected(Status);

	// contour, bevel & shadow controller nodes instead
	// select all of their ink-children (arbitrary).
	// NOTE - doesn't happen if Status is FALSE.
	else
	{
		Node* pKid = FindFirstChild();
		while (pKid != NULL)
		{
			if (pKid->IsAnObject())
				((NodeRenderableInk*)pKid)->SetSelected(Status);

			pKid = pKid->FindNext();
		}
	}
}

// Karim 17/07/2000 - I left these in for a while in case I needed to implement them within
// this class. It seems like I don't, so they should probably be removed at some point.
// This will cause a large rebuild due to their declarations in nodecomp.h .
// Simon 9/8/2000 - The controllers (bev/shadow/contour) never want to be drawn so dont try it matey
void NodeCompound::Select(BOOL ReDraw)
{
	NodeRenderableInk::Select(ReDraw && PromoteHitTestOnChildrenToMe());
}

void NodeCompound::DeSelect(BOOL ReDraw, BOOL bDeselectChildren)
{
	NodeRenderableInk::DeSelect(ReDraw && PromoteHitTestOnChildrenToMe(), bDeselectChildren);
}



/********************************************************************************************

>	virtual DocRect NodeCompound::GetInsideBoundingRect()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/99
	Inputs:		
	Purpose:	Gets the bounding rectangle of all nodes under me, except for the 'needs
				parent' nodes
	SeeAlso:	

********************************************************************************************/
DocRect NodeCompound::GetInsideBoundingRect()
{
	Node * pNode = FindFirstChild();

	DocRect dr;

	while (pNode)
	{
		if (pNode->IsAnObject())
		{
			if (!pNode->NeedsParent(NULL))
			{
				dr = dr.Union(((NodeRenderableInk *)pNode)->GetBoundingRect(FALSE, FALSE));
			}
		}

		pNode = pNode->FindNext();
	}

	return dr;
}

/********************************************************************************************

>	void NodeCompound::PreInformParentsOfRegenerate()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/99
	Inputs:		None
	Purpose:	Informs all parents of a regeneration of the node
	Notes:		Call after the RegenerateNode function in each compound node
	SeeAlso:	

********************************************************************************************/
void NodeCompound::PreInformParentsOfRegenerate()
{
	ObjChangeFlags flgs;
	flgs.RegenerateNode = TRUE;

	ObjChangeParam OP(OBJCHANGE_STARTING, flgs, this, NULL, OBJCHANGE_CALLEDBYCHILD);

	Node * pParent = FindParent();

	while (pParent)
	{
		pParent->OnChildChange(&OP);

		pParent = pParent->FindParent();
	}
	
	return;
}

/********************************************************************************************

>	virtual void NodeCompound::PostInformParentsOfRegenerate()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/99
	Inputs:		None
	Purpose:	Informs all parents of a regeneration of the node
	Notes:		Call after the RegenerateNode function in each compound node
	SeeAlso:	

********************************************************************************************/
void NodeCompound::PostInformParentsOfRegenerate()
{
	ObjChangeFlags flgs;
	flgs.RegenerateNode = TRUE;

	ObjChangeParam OP(OBJCHANGE_FINISHED, flgs, this, NULL, OBJCHANGE_CALLEDBYCHILD);

	Node * pParent = FindParent();

	while (pParent)
	{
		if (pParent->IsAnObject())
		{
			pParent->OnChildChange(&OP);
		}

		pParent = pParent->FindParent();
	}
	
	return;
}

/********************************************************************************************

>	NodeCompound * NodeCompound::FindAssociatedBlendNode(BlendNodeParam * pParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/2000
	Inputs:		The blend node param (see blender & blendatt.h)
	Returns:	The node compound that has been found
	Purpose:	Gets the associated compound node I am to blend to, in the end blend path
				structure in the blend node param
				NULL if non found
	Notes:		Only applies to blends
				Use in conjunction with overrided Blend functions in derived classes
	SeeAlso:	

********************************************************************************************/
NodeCompound * NodeCompound::FindAssociatedBlendNode(BlendNodeParam * pParam)
{
	// count how many parents I have
	Node * pParent = FindParent();

	UINT32 NumParents = 0;
	UINT32 NumSameNodesAbove = 0;

	while (pParent)
	{
		if (pParent->GetRuntimeClass() == this->GetRuntimeClass())
		{
			NumSameNodesAbove++;
		}
		
		NumParents ++;
		pParent = pParent->FindParent();
	}

	UINT32 NumSameNodesBelow = 0;

	Node * pChild = FindFirstChild(CC_RUNTIME_CLASS(NodeCompound));

	while (pChild)
	{
		if (pChild->GetRuntimeClass() == this->GetRuntimeClass())
		{
			NumSameNodesBelow ++;
		}

		pChild = pChild->FindFirstChild(CC_RUNTIME_CLASS(NodeCompound));
	}

	// thus deduce how many similar nodes to myself are in the tree (including myself)
	UINT32 NumSameNodesInStartTree = NumSameNodesAbove + NumSameNodesBelow + 1;

	// ok, lets search the end node for similar nodes
	Node * pEndNode = pParam->GetEndBlendPath()->GetBlendNode();

	// if the end node isn't a compound node, then I can't find anything
	if (!pEndNode->IsCompoundClass())
		return NULL;

	// find the node with the closest number of parents to mine
	Node * pEndNodeStep = pEndNode;
	
	// build a list of all similar nodes in the end tree
	List EndNodeList;

	NodeListItem * pItem = NULL;

	while (pEndNodeStep)
	{
		if (pEndNodeStep->GetRuntimeClass() == this->GetRuntimeClass())
		{
			pItem = new NodeListItem(pEndNodeStep);
			ERRORIF(pItem == NULL, _R(IDE_NOMORE_MEMORY), NULL);

			EndNodeList.AddTail(pItem);
		}

		pEndNodeStep = pEndNodeStep->FindFirstChild(CC_RUNTIME_CLASS(NodeCompound));
	}

	// ok, now we have a list of nodes we have found in the end node list,
	// and we know how many exist above me in the start node list
	// therefore, pick a node in the list to match to
	if (EndNodeList.IsEmpty())
	{
		// can't match to any !
		return NULL;
	}

	NodeCompound * pRetnNode = NULL;

	if (NumSameNodesInStartTree == EndNodeList.GetCount())
	{
		// 1 to 1 relationship therefore easy !
		UINT32 Count = 0;

		pItem = (NodeListItem *)EndNodeList.GetHead();

		while (pItem)
		{
			if (Count == NumSameNodesAbove)
			{
				// get the node to return
				pRetnNode = (NodeCompound *)pItem->pNode;

				EndNodeList.DeleteAll();

				return pRetnNode;
			}
		}

		ERROR3("Number of nodes in list isn't the same as the number of nodes in start tree");
		EndNodeList.DeleteAll();
		return NULL;
	}

	EndNodeList.DeleteAll();

	// many to one and one to many relations are not implemented yet	
	return NULL;
}

/********************************************************************************************

>	virtual NodeRenderableInk * NodeCompound::GetNodeToBlend()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/99
	Inputs:		None
	Purpose:	Gets the blend node inside of this compound node
	Notes:		Only applies to blends
	SeeAlso:	

********************************************************************************************/
NodeRenderableInk * NodeCompound::GetNodeToBlend()
{
	Node * pChild = FindFirstChild();

	while (pChild)
	{
		if (pChild->IsAnObject())
		{
			if (!pChild->NeedsParent(NULL))
			{
				// we've found it
				if (pChild->IsCompoundClass())
				{
					return ((NodeCompound*)pChild)->GetNodeToBlend();
				}
				
				return (NodeRenderableInk*)pChild;
			}
		}

		pChild = pChild->FindNext();
	}

	// failure !
	return NULL;
}



/*********************************************************************************************

>    BOOL NodeCompound::AllocBlendConsList (ListType listType)

     Author:    Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
     Created:   22/6/2000
     Inputs:    The type of list to allocate
     Outputs:   
     Returns:   -
			  
     Purpose:	Allocates the relevant list
			
     Errors:    If the blendConstructedCompoundChildren list has already been allocated.

**********************************************************************************************/

BOOL NodeCompound::AllocBlendConsList (ListType listType)
{
#ifdef _DEBUG	
	List* accessList = InitListPtr (listType);
	if (accessList != NULL)
	{
		ERROR3IF (accessList->GetCount () > 0, "Trying to re-allocate a list that still has items in it!");
	}
#endif
	
	switch (listType)
	{
		case LT_BECOMEA_SHADOWSLIST:
			blndConsBecomeAShadows = new List;
		break;
		case LT_BECOMEA_BEVELSLIST:
			blndConsBecomeABevels = new List;
		break;
		case LT_BECOMEA_CONTOURSLIST:
			blndConsBecomeAContours = new List;
		break;
		default:
			ERROR3 ("NodeCompound::AllocBlendConsList - You wan't what??? !!!");
			return (TRUE);
	}
	
	return (TRUE);
}

/*********************************************************************************************

>    BOOL NodeCompound::AllocatedBlendConsList (ListType listType)

     Author:    Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
     Created:   22/6/2000
     Inputs:    The type of list to check
     Outputs:   
     Returns:   TRUE if list is not empty, FALSE otherwise
			  
     Purpose:	Determins whether blending has generated intermediate compound nodes
			
     Errors:    -

**********************************************************************************************/
	
BOOL NodeCompound::AllocatedBlendConsList (ListType listType)
{	
	List* accessList = InitListPtr (listType);

	if (accessList == NULL)
	{
		return (FALSE);
	}
	else
	{
		if (accessList->GetCount () > 0)
		{
			return (TRUE);
		}
	}
	
	return (FALSE);
}

/*********************************************************************************************

>    BOOL NodeCompound::KillBlendConsList (ListType listType, BOOL killAll)

     Author:    Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
     Created:   22/6/2000
     Inputs:    The type of list that we are killing
				killAll - do we kill all items in the list, or just the ones that are a part
				of the current document?
     Outputs:   
     Returns:   TRUE if success, FALSE otherwise
			  
     Purpose:	Clears out the list of nodes that were generated when blending compound nodes.
			
     Errors:    ERROR3IF the list == NULL

**********************************************************************************************/

BOOL NodeCompound::KillBlendConsList (ListType listType, BOOL killAll, BOOL isPrinting)
{
	List* accessList = InitListPtr (listType);

	ERROR3IF(accessList == NULL, "KillBlendConsList ():  Trying to delete an empty list! - fruitless function call!");

	NodeListItemWithDocPtr* pCurrent = (NodeListItemWithDocPtr*) accessList->GetHead ();

	if (!killAll)
	{
		if (!isPrinting)
		{
			// user has closed a document - clear out any data that was required for rendering

			Document* pCurrentDoc = Document::GetCurrent ();

			ERROR3IF (pCurrentDoc == NULL, "No current document!");
			
			while (pCurrent)
			{
				if (pCurrent->pOwner == pCurrentDoc)
				{
					NodeListItemWithDocPtr* pNext = (NodeListItemWithDocPtr*) accessList->GetNext (pCurrent);
					
					NodeListItemWithDocPtr* markedForDelete = (NodeListItemWithDocPtr*) accessList->RemoveItem (pCurrent);

					markedForDelete->pNode->CascadeDelete ();
					delete (markedForDelete->pNode);

					pCurrent = pNext;
				}
				else
				{
					// the node is NOT linked to the current document - so we can't delete it ....
					
					pCurrent = (NodeListItemWithDocPtr*) accessList->GetNext (pCurrent);
				}
			}
		}
		else
		{
			// were printing ....
			
			while (pCurrent)
			{
				if (pCurrent->isForPrinting)
				{
					NodeListItemWithDocPtr* pNext = (NodeListItemWithDocPtr*) accessList->GetNext (pCurrent);
					
					NodeListItemWithDocPtr* markedForDelete = (NodeListItemWithDocPtr*) accessList->RemoveItem (pCurrent);

					markedForDelete->pNode->CascadeDelete ();
					delete (markedForDelete->pNode);

					pCurrent = pNext;
				}
				else
				{
					// the node was not generated for printing - so we can't delete it ....
					
					pCurrent = (NodeListItemWithDocPtr*) accessList->GetNext (pCurrent);
				}
			}
		}
	}
	else
	{
		// camelot is dying - clear out all data ....
		
		while (pCurrent)
		{
			pCurrent->pNode->CascadeDelete ();
			delete (pCurrent->pNode);
			
			pCurrent = (NodeListItemWithDocPtr*) accessList->GetNext (pCurrent);
		}

		accessList->DeleteAll ();
		delete (accessList);

		accessList = NULL;

		switch (listType)
		{
			case LT_BECOMEA_SHADOWSLIST:
				blndConsBecomeAShadows = NULL;
			break;
			case LT_BECOMEA_BEVELSLIST:
				blndConsBecomeABevels = NULL;
			break;
			case LT_BECOMEA_CONTOURSLIST:
				blndConsBecomeAContours = NULL;
			break;
			default:
				ERROR3 ("NodeCompound::AllocBlendConsList - You wan't what??? !!!");
				return (FALSE);
		}
	}
	
	return (TRUE);
}

/*********************************************************************************************

>    BOOL NodeCompound::KillBlendConsListItem (ListType listType, NodeBlend* nodeBlend)

     Author:    Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
     Created:   22/6/2000
     Inputs:    The type of list that we are killing, the blend that the item is associated with
     Outputs:   
     Returns:   TRUE if success, FALSE otherwise
			  
     Purpose:	Clears out the list of nodes that were generated when blending compound nodes
				which are associated (i.e.  created) with the supplied NodeBlend

	 NOTE:		The associated blend node is set by calling NodeCompound::SetBlenderNode ()
			
     Errors:    ERROR3IF the list == NULL

**********************************************************************************************/

BOOL NodeCompound::KillBlendConsListItem (ListType listType, NodeBlend* nodeBlend)
{
	List* accessList = InitListPtr (listType);

	ERROR3IF(accessList == NULL, "KillBlendConsList ():  Trying to delete an empty list! - fruitless function call!");

	NodeListItemWithDocPtr* pCurrent = (NodeListItemWithDocPtr*) accessList->GetHead ();

	while (pCurrent)
	{
		if (pCurrent->pNode->IsCompoundClass())
		{
			NodeCompound* ptrGroup = (NodeCompound*) pCurrent->pNode;
			NodeBlend* ptrBlender = (NodeBlend*) ptrGroup->GetBlenderNode();

			if (ptrBlender == nodeBlend)
			{
				NodeListItemWithDocPtr* pNext = (NodeListItemWithDocPtr*) accessList->GetNext (pCurrent);
				
				NodeListItemWithDocPtr* markedForDelete = (NodeListItemWithDocPtr*) accessList->RemoveItem (pCurrent);

				markedForDelete->pNode->CascadeDelete ();
				delete (markedForDelete->pNode);

				pCurrent = pNext;
			}
			else
			{
				pCurrent = (NodeListItemWithDocPtr*) accessList->GetNext (pCurrent);
			}
		}
		else
		{
			pCurrent = (NodeListItemWithDocPtr*) accessList->GetNext (pCurrent);
		}
	}
	
	return (TRUE);
}

/*********************************************************************************************

>    BOOL NodeCompound::KillAllBlendBecomeAConsLists (BOOL killAll = FALSE)

     Author:    Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
     Created:   29/6/2000
     Inputs:    killAll - do we kill all items in the list, or just the ones that are a part
				of the current document?
     Outputs:   
     Returns:   TRUE if success, FALSE otherwise
			  
     Purpose:	Clears out the list of nodes that were generated when blending compound nodes
				(that were instructed to DOBECOMEA).
			
     Errors:    

**********************************************************************************************/

BOOL NodeCompound::KillAllBlendBecomeAConsLists (BOOL killAll /*= FALSE*/, BOOL isPrinting /*= FALSE*/)
{
	if (NodeCompound::AllocatedBlendConsList (LT_BECOMEA_BEVELSLIST))
	{
		NodeCompound::KillBlendConsList (LT_BECOMEA_BEVELSLIST, killAll, isPrinting);
	}

	if (NodeCompound::AllocatedBlendConsList (LT_BECOMEA_CONTOURSLIST))
	{
		NodeCompound::KillBlendConsList (LT_BECOMEA_CONTOURSLIST, killAll, isPrinting);
	}

	if (NodeCompound::AllocatedBlendConsList (LT_BECOMEA_SHADOWSLIST))
	{
		NodeCompound::KillBlendConsList (LT_BECOMEA_SHADOWSLIST, killAll, isPrinting);
	}

	return (TRUE);
}

/*********************************************************************************************

>    BOOL NodeCompound::KillAllBlendBecomeAConsListItem ()

     Author:    Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
     Created:   29/6/2000
     Inputs:    The blend that the item is associated with
     Outputs:   
     Returns:   TRUE if success, FALSE otherwise
			  
     Purpose:	Clears out the list of nodes (dobecomea) that were generated when blending
				compound nodes which are associated (i.e.  created) with the supplied NodeBlend

	 NOTE:		The associated blend node is set by calling NodeCompound::SetBlenderNode ()

**********************************************************************************************/

BOOL NodeCompound::KillAllBlendBecomeAConsListItem (NodeBlend* associatedWith)
{
	if (NodeCompound::AllocatedBlendConsList (LT_BECOMEA_BEVELSLIST))
	{
		TRACEUSER( "ChrisS", _T("Invoking delete all on CALLBACK BEVELS\n") );
		NodeCompound::KillBlendConsListItem (LT_BECOMEA_BEVELSLIST, associatedWith);
	}

	if (NodeCompound::AllocatedBlendConsList (LT_BECOMEA_CONTOURSLIST))
	{
		TRACEUSER ("ChrisS", _T("Invoking delete all on CALLBACK CONTOURS\n") );
		NodeCompound::KillBlendConsListItem (LT_BECOMEA_CONTOURSLIST, associatedWith);
	}

	// shadows MUST be deleted last (cause they could be shadowing a bevel, and if we
	// calls cascade delete on the shadow then this also deletes the bevel (leaving the
	// bevels list bevel vaped - but the list reference to it still valid).
	
	if (NodeCompound::AllocatedBlendConsList (LT_BECOMEA_SHADOWSLIST))
	{
		TRACEUSER ("ChrisS", _T("Invoking delete all on CALLBACK SHADOWS\n") );
		NodeCompound::KillBlendConsListItem (LT_BECOMEA_SHADOWSLIST, associatedWith);
	}

	return (TRUE);
}

/*********************************************************************************************

>    BOOL NodeCompound::BlendConsListInsert (ListType listType, Node* insertNode)

     Author:    Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
     Created:   22/6/2000
     Inputs:    The type of list to insert into, and the node to be inserted
     Outputs:   
     Returns:   TRUE
			  
     Purpose:	Inserts a blends intermediate node into our list.  This is necessary so that
				they may be deleted at the correct time.
			
     Errors:    ERROR3IF insertItem == NULL, ERROR3IF list == NULL

**********************************************************************************************/

BOOL NodeCompound::BlendConsListInsert (ListType listType, Node* insertNode)
{	
	ERROR3IF(insertNode == NULL, "BlendConsListInsert ():  Trying to insert a NULL item into a list!");
	
	List* accessList = InitListPtr (listType);

	ERROR3IF(accessList == NULL, "BlendConsListInsert ():  Trying to insert item into a list which is NULL!");
	
	NodeListItemWithDocPtr* pInsert = new NodeListItemWithDocPtr ();
					
	if (pInsert)	// and insert into the paths list ....
	{
		pInsert->pNode = insertNode;
		pInsert->pOwner = Document::GetCurrent ();
		pInsert->isForPrinting = isForPrinting;
		accessList->AddHead (pInsert);

		TRACEUSER ("ChrisS", _T("Blend constructed list just inserted a node\n") );
	}
	
	return (TRUE);
}




/*********************************************************************************************

>    List* NodeCompound::InitListPtr (ListType listType)

     Author:    Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
     Created:   22/6/2000
     Inputs:    The type of list to retrieve
     Outputs:   
     Returns:   Ptr to the relevant list
			  
     Purpose:	Returns a ptr to the relevant list of nodes (that when allocated when blending).
			
     Errors:    -

**********************************************************************************************/

List* NodeCompound::InitListPtr (ListType listType)
{
	List* list = NULL;
	
	switch (listType)
	{
		case LT_BECOMEA_SHADOWSLIST:
			list = blndConsBecomeAShadows;
		break;
		case LT_BECOMEA_BEVELSLIST:
			list = blndConsBecomeABevels;
		break;
		case LT_BECOMEA_CONTOURSLIST:
			list = blndConsBecomeAContours;
		break;
		default:
			list = NULL;
	}
	
	return (list);
}


	

/********************************************************************************************

>	virtual INT32 NodeCompound::EstimateNodeComplexity(OpParam* details)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/09/2000
	Inputs:		details		any data that should be used for the calculation
	Outputs:	-
	Returns:	an estimate of the nodes complexity
	Purpose:	This function estimates a complexity value for the node.  The complexity
				value is based upon the total length of all paths in the node.
	See Also:	OpBlendNodes::DeterminBlendObjectsProcessorHit ()

********************************************************************************************/

INT32 NodeCompound::EstimateNodeComplexity(OpParam* details)
{
	INT32 total = 0;
	
	// Call DoBecomeA on all the group's children
	Node* Current = FindFirstChild(); 
	Node* Next;

	while (Current != NULL)
	{
		Next = Current->FindNext();
		total += Current->EstimateNodeComplexity (details);
		Current = Next;
	}

	return (total);
}




/********************************************************************************************

>	virtual BOOL NodeCompound::HasVisibleChildren() const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/04/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this compound is empty and can thus be deleted or hidden without
				the user seeing any difference
	Purpose:	Determine whether this compound node is really doing anything useful

********************************************************************************************/

BOOL NodeCompound::HasVisibleChildren() const
{
	if (!this->IsRenderable())
		return FALSE;

	Node* pChild = FindFirstChild(); 
	while (pChild)
	{
		if (pChild->IsAnObject() && pChild->IsRenderable())
		{
			if (!pChild->NeedsParent((Node*)this))
				return TRUE;
		}

		pChild = pChild->FindNext();
	}

	return FALSE;
}




/********************************************************************************************

>	DocRect NodeCompound::GetExtendTargetBounds(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/06/2000
	Inputs:		ExtParams		const ref to an ExtendParams class describing the extension.
	Outputs:	
	Returns:	The DocRect to treat as the bounding rect of this node when extending.
	Purpose:	Return a DocRect which contains the bounds of this node as defined and
				required by the Extending mechanism.

				We treat x- and y- directions separately, and we always return our true
				bounds, unless we're extending in a direction, in which case we return the
				bounds of our extend control points, which currently means the union of the
				ExtendTargetBounds of our children.

	Notes:		Note that we're assuming that we always have kids. No kids => pear-shapey.

	See also:	SliceHelper::BoundingNodeSize().

********************************************************************************************/
DocRect NodeCompound::GetExtendTargetBounds(const ExtendParams& ExtParams)
{
	DocRect drBounds(0, 0, 0, 0);
	Node* pKid = FindFirstChild();
	while (pKid != NULL)
	{
		if (pKid->IsAnObject())
			drBounds = drBounds.Union(((NodeRenderableInk*)pKid)->GetExtendTargetBounds(ExtParams));
	}

	return drBounds;
}




/********************************************************************************************

>	virtual String_32& NodeCompound::GetName()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/6/94
	Inputs:		-
	Outputs:	-
	Returns:	The name of the group
	Purpose:	This routine is used to obtain the name of a group
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

String_32& NodeCompound::GetName()
{
	return CompoundName; 
} 

/********************************************************************************************

>	virtual void SetName(String_32& Name)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/6/94
	Inputs:		Name: The new group name
	Outputs:	-
	Returns:	-
	Purpose:	For setting the group's name
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void NodeCompound::SetName(String_32& Name) 
{
	CompoundName = Name; 
}


   
/********************************************************************************************

>	virtual BOOL NodeCompound::AllowOp_AccountForCompound(ObjChangeParam* pParam,
															BOOL SetOpPermissionState,
															BOOL DoPreTriggerEdit)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/10/2000
	Inputs:		pParam					information about the requested change, eg which Op.
				SetOpPermissionState	whether flags should be set ready for OnChildChange.
				DoPreTriggerEdit		if TRUE then NameGallery::PreTriggerEdit is called.
										*Must* be TRUE if the calling Op may make any nodes
										change their bounds, eg move, line width, cut.
										Use TRUE if unsure.

	Returns:	TRUE if ANY of the objects in the compound node have allowed the op.

	Purpose:	Overrides Node::AllowOp_AccountForCompound - see definition of that function
				for a general explanation.

				Controller nodes need to inform all of their other children whenever one
				child is queried via AllowOp. This is because unlike a normal group, editing
				one child may well affect its siblings, who must be informed of the edit.

	See Also:	Node::AllowOp, Node::AllowOp_AccountForCompound

	Notes:		Karim 20/12/2000
				Added the OpTextFormat bodge for feathers on bevelled text - sorry!

********************************************************************************************/
BOOL NodeCompound::AllowOp_AccountForCompound(ObjChangeParam* pParam,
												BOOL SetOpPermissionState,
												BOOL DoPreTriggerEdit)
{
	BOOL AnyAllowed = FALSE;

	Node* pCallingChild = (pParam->GetDirection() == OBJCHANGE_CALLEDBYCHILD) ?
													pParam->GetCallingChild() : NULL;
	pParam->SetCallingChild(NULL);

	ObjChangeFlags Flags = pParam->GetChangeFlags();
	UndoableOperation* pChangeOp = pParam->GetOpPointer();

	if( Flags.Attribute || 
		Flags.TransformNode || 
		Flags.RegenerateNode ||
		( pChangeOp != NULL 
PORTNOTE("text","Removed OpTextUndoable usage")
#ifndef EXCLUDE_FROM_XARALX
		  && pChangeOp->IS_KIND_OF( OpTextUndoable )
#endif
		 ) )
	{

		ObjChangeDirection OldDirection = pParam->GetDirection();
		pParam->SetDirection(OBJCHANGE_CALLEDBYPARENT);
		Node* pNode = FindFirstChild();

		BOOL InformGeomLinkedAttrs =	SetOpPermissionState &&
										pChangeOp != NULL &&
										pChangeOp->MayChangeNodeBounds();

		while (pNode != NULL)
		{
			if (pNode != pCallingChild)
			{
				if (pNode->IsAnObject())
					AnyAllowed |= pNode->AllowOp(pParam, SetOpPermissionState, DoPreTriggerEdit);
				else
				{
					if (pNode->IsAnAttribute() && ((NodeAttribute*)pNode)->IsLinkedToNodeGeometry())
						if (InformGeomLinkedAttrs)
							((NodeAttribute*)pNode)->LinkedNodeGeometryHasChanged(pChangeOp);
				}
			}

			pNode = pNode->FindNext();
		}

		pParam->SetDirection(OldDirection);
	}

	// if setting flags and any child allowed it, set this permission allowed
	if (SetOpPermissionState && AnyAllowed)
		SetOpPermission(PERMISSION_ALLOWED);

	return AnyAllowed;
}




/********************************************************************************************

>	virtual BOOL NodeCompound::CanBecomeA(BecomeA* pBecomeA)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/94
	Inputs:		InkClass: The class	of object
				pNumObjects = ptr to place number of objects of type pClass that will be created (Note: can be NULL).
							  *pNumObects in undefined on entry
	Outputs:	-
	Returns:	TRUE if the node, or any of its children can transmogrify themselves to become 
				an InkClass object

	Purpose:	This function is used by the convert to shapes operation. It determines if 
				the node or any of its children can convert themselves into an InkClass object. 

				The number you put into pNumObjects (if it's not NULL) should exactly equal the total number
				of pClass objects you create.  It should NOT contain any additional objects you may produce
				such as group objects for containing the pClass object, or attributes.

				Also, the entry value of *pNumObjects cannot be assumed to be 0.

	Errors:		-

********************************************************************************************/

BOOL NodeCompound::CanBecomeA(BecomeA* pBecomeA)
{
	BOOL bOK=FALSE;

	Node* Current = FindFirstChild(); 
	while (Current != NULL)
	{
		if (Current->CanBecomeA(pBecomeA))				// Increments count
		{
			bOK = TRUE;
			if (!pBecomeA->IsCounting())
				return TRUE;							// Don't want total, so return now
		}

		Current = Current->FindNext();
	}

	return bOK;
}


/********************************************************************************************

>	virtual BOOL NodeCompound::DoBecomeA(BecomeA* pBecomeA)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> (and Markn)
	Created:	29/4/94
	Inputs:		pBecomeA = 	ptr to a class that contains all the info needed to become a new
							type of node.
	Outputs:	-
	Returns:	TRUE if the object has been transformed, FALSE if we run out of memory

	Purpose:	Transforms the object into another type of object. 
				Note: changed 7/10/94 by MarkN to take the pBecomeA param, so that more data could be passed
				to these functions in the future without causing massive rebuilds due to the editing of
				node.h

				14/6/95 (Markn): This now localises attributes before calling it's children, then factors them out
				at the end.  This is so that common attrs trickle down to ALL leaf nodes, no matter how deep 
				they may be.  This is important when a child node is a blend, because the action of DoBecomeA()
				in the blend's case creates new child attrs for the path it creates, hence making any common
				attrs illegal.
	Errors:		-
	SeeAlso:	Node::CanBecomeA

********************************************************************************************/

BOOL NodeCompound::DoBecomeA(BecomeA* pBecomeA)
{
	ERROR2IF(pBecomeA == NULL,FALSE,"NULL BecomeA ptr");

	UndoableOperation* pOp = pBecomeA->GetUndoOp();
	BOOL ok;

	if (pBecomeA->GetReason() == BECOMEA_REPLACE)
	{
//		ERROR2IF(pBecomeA->GetUndoOp() == NULL,FALSE,"Trying to replace a NodeGroup, but pUndoOp == NULL");

		// Make all attrs children of the child nodes
		if (pOp)
			ok = pOp->DoLocaliseCommonAttributes(this);
		else
			ok = LocaliseCommonAttributes();
		if (!ok)
			return FALSE;
	}

	// Call DoBecomeA on all the group's children
	Node* Current = FindFirstChild(); 
	Node* Next;

	while (Current != NULL)
	{
		Next = Current->FindNext();
		if(!(Current->DoBecomeA(pBecomeA)))
		{	
			return FALSE; 		// Out of memory
		} 
		Current = Next; 
	}

	if (pBecomeA->GetReason() == BECOMEA_REPLACE)
	{
//		ERROR2IF(pBecomeA->GetUndoOp() == NULL,FALSE,"Trying to replace a NodeGroup, but pUndoOp == NULL");

		// Factor out the attrs (needed after a call to DoLocaliseCommonAttributes
		if (pOp)
			ok = pOp->DoFactorOutCommonChildAttributes(this);
		else
			ok = FactorOutCommonChildAttributes();
		if (!ok)
			return FALSE;
	}

	return TRUE; 
}



/********************************************************************************************

>	virtual NodeGroup* NodeCompound::BecomeAGroup(UndoableOperation* pUndoOp)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/07/2000

	Inputs:		pUndoOp		ptr to an Op to use for all the undoable bits.

	Outputs:	This node is converted into a group.

	Returns:	A pointer to the new group node if successful,
				NULL otherwise.

	Purpose:	This method is for the convenience of any compound node descended from
				NodeGroup. It 'changes' the node into a group by following these steps:
					*	Attributes are undoably localised on this node.
					*	A group node is created.
					*	All children of this node are undoably moved under the group node.
					*	The group is undoably inserted into the tree as the next sibling.
					*	Attributes are undoably factored out on the group node.
					*	This node is undoably hidden.

	Notes:		The group's selection state is maintained - if this node is selected now,
				then the new group will be selected - although this may not be enough
				for some nodes, eg NodeShadowController et al.

				Although calling this method on a normal NodeGroup should have no visible
				effect, it *is* wasteful of time and memory. So DON'T call this method
				on a plain NodeGroup!

	Errors:		We require a valid Op ptr, so will ERROR3 and return FALSE
				if we don't get one (maybe this should be an ERROR2).

********************************************************************************************/
NodeGroup* NodeCompound::BecomeAGroup(UndoableOperation* pUndoOp)
{
	PORTNOTETRACE("other","NodeCompound::BecomeAGroup - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// we can't do a thing without an Op...
//	if (pUndoOp == NULL)
//	{
//		ERROR3("NodeGroup::BecomeAGroup; NULL Op pointer!");
//		return NULL;
//	}

	// localise any attributes into our children, to ensure that none of our direct children
	// are attributes. we do this because we'll be moving all our children, and directly
	// moving attrs is forbidden (see DoMoveNode() for more info).
	BOOL	ok;
	
	if (pUndoOp)
		ok = pUndoOp->DoLocaliseCommonAttributes(this);
	else
		ok = LocaliseCommonAttributes(FALSE, FALSE, NULL);

	// create the new group node for our children.
	NodeGroup* pGroup = NULL;
	if (ok)
	{
		ALLOC_WITH_FAIL(pGroup, new NodeGroup, pUndoOp);
		ok = (pGroup != NULL);
	}

	// insert the new group into the tree as our right-sibling.
	if (ok)
	{
		if (pUndoOp)
			ok = pUndoOp->DoInsertNewNode(pGroup, this, NEXT, TRUE, FALSE, IsSelected(), FALSE);
		else
			pGroup->AttachNode(this, NEXT, FALSE, FALSE);
	}

	// move all our children into the new group.
	// some of our children may be non-optimisable attributes, which DoMoveNode() won't move.
	// In these cases, we non-undoably put copies of them under the new group node (which is
	// undoably inserted anyway, so the non-undoable stuff should be ok).
	if (ok)
	{
		Node* pNextKid	= NULL;
		Node* pThisKid	= FindFirstChild();
		while (ok && pThisKid != NULL)
		{
			pNextKid = pThisKid->FindNext();

			if (pThisKid->IsAnObject())
			{
				if (pUndoOp)
				{
					CALL_WITH_FAIL(pUndoOp->DoMoveNode(pThisKid, pGroup, LASTCHILD), pUndoOp, ok)
				}
				else
				{
					pThisKid->MoveNode(pGroup, LASTCHILD);
				}
				if (ok && pThisKid->IsSelected())
					pGroup->SetParentOfSelected(TRUE);
			}
			else if (	pThisKid->IsAnAttribute() &&
						!((NodeAttribute*)pThisKid)->ShouldBeOptimized() )
				CALL_WITH_FAIL(pThisKid->CopyNode(pGroup, LASTCHILD), pUndoOp, ok)

			pThisKid = pNextKid;
		}
	}

	// factor out attributes on the new group - this matches the localise call up top.
	if (ok)
	{
		if (pUndoOp)
			ok = pUndoOp->DoFactorOutCommonChildAttributes(pGroup);
		else
			ok = pGroup->FactorOutCommonChildAttributes();
	}

	// invalidate the group's bounding box.
	if (ok) pGroup->InvalidateBoundingRect();

	// finally, hide ourself.
	if (ok)
	{
		if (pUndoOp)
			ok = pUndoOp->DoHideNode(this, TRUE);
		else
		{
			CascadeDelete();
			delete this;		// Scary!
		}
	}

	return ok ? pGroup : NULL;
#else
	return NULL;
#endif
}




/********************************************************************************************

>	virtual NodeRenderableInk* NodeCompound::GetInkNodeFromController() const

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/2000
	Inputs:		-
	Outputs:	-
	Returns:	The ink node that is used by bevels, shadows and contours
	Purpose:	If we had a Controller node inherited class from NodeGroup then this 
				function really belongs there.  But we don't, so this function only 
				returns anything if your node also returns TRUE to IsAController().
				If so it will return the node that is shadowed, bevelled or contoured.
				
	SeeAlso:	

********************************************************************************************/

NodeRenderableInk* NodeCompound::GetInkNodeFromController() const
{
	return NULL;
}




/********************************************************************************************

>	DocRect NodeCompound::GetChildBoundingRect(BOOL bIncludeAttrs = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/09/2004
	Returns:	The bounding box of the children of the live effect
	Purpose:	Get the bounding rect of the children without updating BOundingRectangle
				or IsBOundingRectValid for this node...

********************************************************************************************/

DocRect NodeCompound::GetChildBoundingRect(BOOL bIncludeAttrs)
{
	// just set it to be an empty rectangle
	DocRect BoundRect(0,0,0,0);
	
	Node* pNode = FindFirstChild();
	while (pNode!=NULL)
	{
		// Add in the bounding rect of this node with all the others
		if (pNode->IsBounded())
			BoundRect = BoundRect.Union(((NodeRenderableBounded*)pNode)->GetBoundingRect(!bIncludeAttrs));

		// And find the next node
		pNode = pNode->FindNext();
	}

	return BoundRect;
}




/********************************************************************************************

>	virtual BOOL NodeCompound::HasEffectAttrs() const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/01/2005
	Returns:	TRUE if the node has effect attributes
				FALSE otherwise
	Purpose:	Detect Effect Attributes

********************************************************************************************/

BOOL NodeCompound::HasEffectAttrs() const
{
	Node* pn = FindLastChild();
	while (pn && !pn->IsBounded())
	{
		if (pn->IsAnAttribute() && ((NodeAttribute*)pn)->IsEffectAttribute())
			return TRUE;

		pn = pn->FindPrevious();
	}
	return FALSE;
}

	
	
	
/********************************************************************************************

>	virtual BOOL NodeCompound::ContainsEffects() const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/08/2005
	Returns:	TRUE if the node contains effects nodes
				FALSE otherwise
	Purpose:	Detect Effects

********************************************************************************************/

BOOL NodeCompound::ContainsEffects() const
{
	Node* pn = FindFirstChild();
	while (pn)
	{
		if (pn->IsEffect())
			return TRUE;

		if (pn->IsCompoundClass() && ((NodeCompound*)pn)->ContainsEffects())
			return TRUE;

		pn = pn->FindNext();
	}

	return FALSE;
}

	
	
	
/********************************************************************************************

>	void NodeCompound::TransformEffectAttrs(TransformBase& Trans)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/2005
	Inputs:		Trans - The transform that should be applied to all the children
	Purpose:	Transforms the Child nodes of this node. This allows fills etc to move
				with the objects.

********************************************************************************************/

void NodeCompound::TransformEffectAttrs(TransformBase& Trans)
{
	// Find all Effect Attributes (in subtree after last bounded node
	// and transform them.
	Node* pn = FindLastChild();
	while (pn && !pn->IsBounded())
	{
		if (pn->IsAnAttribute() && ((NodeAttribute*)pn)->IsEffectAttribute())
			((NodeAttribute*)pn)->Transform( Trans );

		pn = pn->FindPrevious();
	}
}



