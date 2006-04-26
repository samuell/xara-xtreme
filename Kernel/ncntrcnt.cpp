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
//#include "node.h"
//#include "ink.h"
#include "nodecomp.h"
#include "saveeps.h"

// Save/load
#include "cxfrec.h"		// CXaraFileRecord

#include "cxftags.h"
//#include "cxfdefs.h"



#include "nodecntr.h"
#include "ncntrcnt.h"
//#include "undoop.h"
//#include "cntres.h"
//#include "bevres.h"
#include "nodecont.h"
//#include "rndrgn.h"
//#include "attrmap.h"
#include "opcntr.h"
//#include "app.h"
//#include "opbevel.h"
#include "objchge.h"
#include "transop.h"
#include "nodetext.h"
#include "cutop.h"
#include "textops.h"
#include "ophist.h"
#include "attrappl.h"
//#include "view.h"
//#include "docview.h"
//#include "becomea.h"
#include "lineattr.h"
//#include "fillattr.h"
//#include "fillval.h"
//#include "attrval.h"
//#include "arrows.h"
#include "gclips.h"
#include "blobs.h"
#include "nodeblnd.h"
#include "spread.h"
//#include "quality.h"
//#include "qualattr.h"
#include "gdraw.h"
#include "ppbevel.h"
//#include "cstatbar.h"
#include "extender.h"
#include "ngcore.h"		// NameGallery, for stretching functionality
//#include "blendatt.h"
#include "blndhelp.h"
//#include "mario.h"
#include "nodebldr.h"
#include "pathops.h"
#include "fthrattr.h"
#include "pathndge.h"
//#include "mrhbits.h"
#include "blndtool.h"
#include "cmxrendr.h"
//#include "cmxexdc.h"

CC_IMPLEMENT_DYNCREATE(NodeContourController, NodeGroup)
CC_IMPLEMENT_DYNCREATE(ContourNodeTreeFactory, CompoundNodeTreeFactory)
CC_IMPLEMENT_DYNCREATE(ContourNodePathProcessor, PathProcessor)
CC_IMPLEMENT_DYNCREATE(ContourRecordHandler, CamelotRecordHandler)
CC_IMPLEMENT_DYNAMIC(InsetPathPathProcessor, PathProcessor);

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

// the amount to increase the RGB values of the original object to obtain 
// the default colour of the contour
#define DEFAULTCOLOURINCREASE 180

/***********************************************************************************************

>	NodeContourController::NodeContourController()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Purpose:	Constructor

***********************************************************************************************/

NodeContourController::NodeContourController()
{
	m_Steps = 5;
	m_Width = 5000;
	m_BlendType = COLOURBLEND_FADE;
	m_bInsetPath = FALSE;	
	m_pPathProc = NULL;
	m_PerformedExtend = FALSE;

#ifdef _DEBUG	
	myContourID = -1;
	myContourBecomeAID = -1;
#endif
}

/***********************************************************************************************

>	NodeContourController::NodeContourController(Node* ContextNode,  
				AttachNodeDirection Direction,  
				BOOL Locked = FALSE, 
				BOOL Mangled = FALSE,  
				BOOL Marked = FALSE, 
				BOOL Selected = FALSE    
				) ;


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Purpose:	Constructor

***********************************************************************************************/
NodeContourController::NodeContourController(Node* ContextNode,  
				AttachNodeDirection Direction,  
				BOOL Locked, 
				BOOL Mangled,  
				BOOL Marked, 
				BOOL Selected    
				) 
{
	m_Steps = 5;
	m_Width = 5000;
	m_BlendType = COLOURBLEND_FADE;
	m_bInsetPath = FALSE;	
	m_pPathProc = NULL;

#ifdef _DEBUG	
	myContourID = -1;
	myContourBecomeAID = -1;
#endif
}

/***********************************************************************************************

>	NodeContourController::~NodeContourController()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Purpose:	Destructor

***********************************************************************************************/

NodeContourController::~NodeContourController()
{
#ifdef _DEBUG

	if (myContourID > -1)
	{
		TCHAR			strId[100];
		camSnprintf( strId, 100, _T("Popping NodeContourController ID:  %i\n"), myContourID );
			
		TRACEUSER( "ChrisS", strId );
	}

#endif
}

/***********************************************************************************************

>	DocRect NodeContourController::GetBoundingRect(BOOL DontUseAttrs = FALSE, BOOL HitTest = FALSE)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Get the bounding rect of this node

***********************************************************************************************/
DocRect NodeContourController::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	// run through all the children of me, summing their bounding rects
	if (!(!IsBoundingRectValid || DontUseAttrs))
	{	
		return BoundingRectangle;
	}	

	NodeContour * pContour = GetContourNode();

	DocRect dr;

	if (!pContour)
	{	
		dr = GetInsideBoundingRect();

		if (m_Width < 0)
		{
			dr.lo.x -= -m_Width;
			dr.lo.y -= -m_Width;
			dr.hi.x += -m_Width;
			dr.hi.y += -m_Width;
		}
	}
	else
	{
		dr = pContour->GetBoundingRect(DontUseAttrs, HitTest);
		dr = dr.Union(GetInsideBoundingRect());
	}
	
	IsBoundingRectValid = TRUE;
	BoundingRectangle = dr;

	return dr;

}

/***********************************************************************************************

>	DocRect NodeContourController::GetBlobBoundingRect()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Get the blob bounding rect of this node

***********************************************************************************************/
DocRect NodeContourController::GetBlobBoundingRect()
{
	DocRect br = GetBoundingRect(FALSE, FALSE);

	// get the blob size & do something with it
	BlobManager * pBlobMgr = GetApplication()->GetBlobManager();

	INT32 Width = 0;

	if (pBlobMgr)
	{
		Width = pBlobMgr->GetBlobSize();
	}

	br.lo.x -= Width;
	br.lo.y -= Width;
	br.hi.x += Width;
	br.hi.y += Width;

	return br;
}



/********************************************************************************************

>	virtual void NodeContourController::SelectInRect(const DocRect& Rect, SelStateAction st)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14 April 2000
	Inputs:		Rect	const reference to the bounding rect to use.
				st		the selection state, ie SET/TOGGLE/CLEAR.
	Outputs:	This node may have its selection status changed, depending on the inputs.

	Purpose:	Helper method for the static fn SelectAllInRect() used by marquee select.
				This method changes the selection state of this node according to the given
				bounding rectangle and sel-state action. Most nodes will want to use the
				default behaviour, which is to select themselves if their bounds lie within
				the given bounding rectangle. If you want to do something special with the
				marquee select, then override this method.

	Notes:		This method is meant to be called solely from SelectAllInRect() - if you wish
				to call it from elsewhere, it *may* work as you expect, but it is a good idea
				that you check your assumptions on what preconditions are necessary.

	Errors:		ERROR3 in DEBUG if st holds an invalid state.
	See also:	SelectAllInRect().

********************************************************************************************/
void NodeContourController::SelectInRect(const DocRect& Rect, SelStateAction st)
{
	// try to select each of our NodeRenderableInk children.
	NodeRenderableInk* pInkChild = NULL;
	Node* pChild = FindFirstChild();
	while (pChild != NULL)
	{
		if (pChild->IsAnObject())
		{
			pInkChild = (NodeRenderableInk*)pChild;
			if (Rect.ContainsRect(pInkChild->GetBoundingRect()))
			{
				switch (st)
				{
				case CLEAR:
					if (pInkChild->MarqueeSelectNode())
					{
						pInkChild->DeSelect(TRUE);
					}
					break;

				case SET:
					if (pInkChild->MarqueeSelectNode())
					{
						pInkChild->Select(TRUE);
					}
					break;

				case TOGGLE:
					if (pInkChild->MarqueeSelectNode())
					{
						if (pInkChild->IsSelected())
							pInkChild->DeSelect(TRUE);
						else
							pInkChild->Select(TRUE);
					}
					break;

				default:
					ERROR3("NodeContourController::SelectInRect; Unknown SelStateAction!\n");
					return;
				}
			}
		}
		pChild = pChild->FindNext();
	}
}



/***********************************************************************************************

>	void NodeContourController::Render(RenderRegion* pRender)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Renders this node

***********************************************************************************************/
void NodeContourController::Render(RenderRegion* pRender)
{
//#pragma message( __LOCMSG__ "NodeContourController::Render - do nothing" )
//	TRACE( _T("Warning - NodeContourController::Render called\n") );
	if (m_bInsetPath)
	{
		PathProcessor * pProc = pRender->GetFirstPathProcessor();

		if (pProc)
		{
			if (!pProc->IS_KIND_OF(InsetPathPathProcessor))
			{
				ERROR3("Top path processor isn't an inset path processor");
				m_pPathProc = NULL;
			}
			else
			{
				pRender->PopPathProcessor();
				m_pPathProc = NULL;
			}
		}
	}
}

/***********************************************************************************************

>	void NodeContourController::DisableInsetPathPathProcessor()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/2000
	Inputs:		See base class
	Purpose:	Disables the insert path path processor (which means things are drawn)

***********************************************************************************************/
void NodeContourController::DisableInsetPathPathProcessor()
{
	if (m_pPathProc)
		m_pPathProc->SetActive(FALSE);
}

/***********************************************************************************************

>	void NodeContourController::EnableInsetPathPathProcessor()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/2000
	Inputs:		See base class
	Purpose:	Enables the insert path path processor (which means things are drawn)

***********************************************************************************************/
void NodeContourController::EnableInsetPathPathProcessor()
{
	if (m_pPathProc)
		m_pPathProc->SetActive(TRUE);
}



/***********************************************************************************************

>	SubtreeRenderState NodeContourController::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/06/2004
	Inputs:		See base class
	Purpose:	Pre renders this node

***********************************************************************************************/
SubtreeRenderState NodeContourController::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)
{
//#pragma message( __LOCMSG__ "NodeContourController::RenderSubtree - do nothing" )
//	TRACE( _T("Warning - NodeContourController::RenderSubtree called\n") );
//	return SUBTREE_ROOTANDCHILDREN;
	if (m_bInsetPath && pRender!=NULL && !pRender->IsPrinting())
	{
		// create & push the path processor
		m_pPathProc = new InsetPathPathProcessor;

		ERROR2IF(m_pPathProc == NULL, SUBTREE_NORENDER, _R(IDE_NOMORE_MEMORY));
		pRender->PushPathProcessor(m_pPathProc);
	}
	
//	return SUBTREE_ROOTANDCHILDREN;
	return NodeGroup::RenderSubtree(pRender, ppNextNode, bClip);
}

/***********************************************************************************************

>	BOOL NodeContourController::DoBecomeA(BecomeA* pBecomeA)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Turns this node into another node of a particular type

***********************************************************************************************/
BOOL NodeContourController::DoBecomeA(BecomeA* pBecomeA)
{
	// Check for a NULL entry param							   
	ERROR2IF_PF(pBecomeA == NULL,FALSE,("pBecomeA is NULL"));

	if (!pBecomeA->BAPath())
	{
		return FALSE;
	}

	BOOL ValidReason = (pBecomeA->GetReason() == BECOMEA_REPLACE || pBecomeA->GetReason() == BECOMEA_PASSBACK);
	ERROR2IF_PF(!ValidReason,FALSE,("Unkown BecomeA reason %d",pBecomeA->GetReason()));

	UndoableOperation* pOp = pBecomeA->GetUndoOp();

//	NodeGroup			* pGroup		= NULL;
	Node				* pNode			= NULL;
//	Node				* pNextNode		= NULL;
	NodeHidden			* pHidden		= NULL;

//	NodeListItem * pItem = NULL;
	List MyList;
//	RegenerateContourAction * pAction = NULL;

	NodeContour * pContour = (NodeContour *)FindFirstChild(CC_RUNTIME_CLASS(NodeContour));

	BOOL ok = TRUE;

	switch (pBecomeA->GetReason())
	{
	case BECOMEA_REPLACE:
//		ERROR3IF(pOp == NULL, "No operation specified");

		if (!m_bInsetPath)
		{
			// if any of our children are selected, then we'll have to remember to select
			// the group created in our place.
			BOOL fSelectBecomeAGroup = IsParentOfSelected();

			if (!NodeGroup::DoBecomeA(pBecomeA))
				return FALSE;

			// now turn into a group.
			NodeGroup* pGroup = BecomeAGroup(pOp);

			// select the group but don't bother redrawing blobs an' suchlike.
			if (pGroup == NULL)
				ok = FALSE;
			else if (fSelectBecomeAGroup)
				pGroup->Select(FALSE);
		}
		else
		{
			// move the contour node to being next to me
			if (pContour)
			{
				// localise the attributes
				if (ok)
				{
					if (pOp)
						ok = pOp->DoLocaliseCommonAttributes(this, TRUE);
					else
						ok = LocaliseCommonAttributes(TRUE, FALSE, NULL);
				}
				
//				if (ok) ok = pOp->DoMoveNode(pContour, this, NEXT);

				// convert it to editable shapes
				if (ok) ok = pContour->DoBecomeA(pBecomeA);

				// hide me
				if (ok)
				{
					if (pOp)
						ok = pOp->DoHideNode(this, TRUE, &pHidden, TRUE);
					else
					{
						CascadeDelete();
						delete this;		// Scary!
					}
				}
			}
		}	
		break;
	case BECOMEA_PASSBACK :
		if (pBecomeA->IsBlendBecomeA())
		{
			CompoundNodeBlendBecomeA MyBecomeA(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath),
				NULL, FALSE, this, pBecomeA);
			
			// now that the become a is set up call all my sub nodes with this
			Node * pChild = FindFirstChild();
			
			while (pChild)
			{
				if (pChild->IsAnObject() && !pChild->NeedsParent(NULL))
				{
					MyBecomeA.ResetCount();
					
					if (pChild->CanBecomeA(&MyBecomeA))
					{
						// tell the become a that we're starting a block
						MyBecomeA.SetNumPathNodes(MyBecomeA.GetCount());
						MyBecomeA.ResetCount();
						
						pChild->DoBecomeA(&MyBecomeA);
					}
				}
				
				pChild = pChild->FindNext();
			}
		}
		else if (pBecomeA->IsCompoundBlendBecomeA ())			// test code ....
		{
			// CGS:  another compound node (of which we are a child) is requesting us to
			// BECOMEA_PASSBACK so get on and do it ....
			
			// this code is only called when blending contours that have been shadowed
			// and when converting to editable shapes (in the forementioned case)
			
			CompoundNodeBlendBecomeA MyBecomeA(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath),
				NULL, FALSE, this, pBecomeA);
			
			Node * pChild = FindFirstChild();
			
			while (pChild)
			{
				if (pChild->IsAnObject() && !pChild->NeedsParent(NULL))
				{
					MyBecomeA.ResetCount();
					
					if (pChild->CanBecomeA(&MyBecomeA))
					{
						// tell the become a that we're starting a block
						MyBecomeA.SetNumPathNodes(MyBecomeA.GetCount());
						MyBecomeA.ResetCount();
						
						pChild->DoBecomeA(&MyBecomeA);
					}
				}
				
				pChild = pChild->FindNext();
			}
		}
		else
		{
			pNode = FindFirstChild();
			while (pNode != NULL)
			{
				if (pNode->CanBecomeA(pBecomeA))
				{
					if (!pNode->DoBecomeA(pBecomeA))
						return FALSE;
				}
				
				pNode = pNode->FindNext();
			}
		}
			
		break;
		default: break;
	}

	return ok;
}

/********************************************************************************************

>	virtual NodeRenderableInk* NodeContourController::GetInkNodeFromController() const

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/2000
	Inputs:		-
	Outputs:	-
	Returns:	The ink node that is used by bevels, shadows and contours
	Purpose:	To return the node that we contour
				
	SeeAlso:	

********************************************************************************************/

NodeRenderableInk* NodeContourController::GetInkNodeFromController() const
{
	// Our contoured node is the only ink node child that is not a contour
	Node* pKid = FindFirstChild();

	while (pKid != NULL)
	{
		if (pKid->IsAnObject() && !pKid->IsAContour())
			break;

		pKid = pKid->FindNext();

	}

	// pKid is now either NULL, or is the node we're looking for.
	ERROR3IF(pKid == NULL, "NodeContourController::GetInkNodeFromController; Can't find contoured node!");
	return (NodeRenderableInk*)pKid;


}


/********************************************************************************************

>	BOOL NodeContourController::PromoteAttributeApplicationToMe(CCRuntimeClass *pAttrClass)
																		 const

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/2000
	Inputs:		The attribute's runtime class
	Outputs:	TRUE to promote, FALSE for not promote
	Purpose:	Indicates whether an attribute should be promoted to me
	
********************************************************************************************/

BOOL NodeContourController::PromoteAttributeApplicationToMe(CCRuntimeClass *pAttrClass) const
{
	if (pAttrClass == CC_RUNTIME_CLASS(AttrJoinType))
		return TRUE;

	return FALSE;
}

/********************************************************************************************

>	virtual NodeRenderableInk * 
			NodeContourController::CreateTreeFromNodeToBlend(NodeRenderableInk * pNode)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/2000
	Inputs:		The node to wrap up
	Outputs:	NULL for failure
	Returns:	The root of the new tree
	Purpose:	Creates a copy of my tree, wrapping up the given node
	
********************************************************************************************/
NodeRenderableInk * 
			NodeContourController::CreateTreeFromNodeToBlend(NodeRenderableInk * pNode, 
			CCAttrMap * pAttrMap)
{
	// remove all attributes from the node's subtree
	ERROR2IF(pNode->FindParent() != NULL, NULL, "Node shouldn't have parents");

	if (pNode->IsNodePath())
	{
		pNode->CascadeDelete();
	}
	
	// apply the attribute map to this node
	if (pNode->IsNodePath() && pAttrMap)
	{
		pNode->ApplyAttributes(pAttrMap, FALSE);
	}

	// now, lets make a copy of myself & the bevel node
	NodeContourController * pControl = NULL;

	pControl = (NodeContourController *)this->PublicCopy();
	ERRORIF(pControl == NULL, _R(IDE_NOMORE_MEMORY), NULL);

	NodeContour * pContour = (NodeContour *)FindFirstChild(CC_RUNTIME_CLASS(NodeContour));
	ERROR2IF(pContour == NULL, NULL, "Can't find bevel node");

	// apply the attribute map of the original bevel node to the bevel copy node
	NodeContour * pCopyContour = (NodeContour *)pContour->PublicCopy();
	ERRORIF(pCopyContour == NULL, _R(IDE_NOMORE_MEMORY), NULL);
	
	CCAttrMap * pContourMap = CCAttrMap::MakeAppliedAttrMap(pContour);
//	pContourMap->RemoveIndirectlyAppliedLayeredAttributes(pContour,FALSE);

	ERRORIF(pContourMap == NULL, _R(IDE_NOMORE_MEMORY), NULL);

	pCopyContour->ApplyAttributes(pContourMap, FALSE);

	delete pContourMap;
	
	pNode->AttachNode(pControl, FIRSTCHILD);

	if (m_Width < 0)
	{
		pCopyContour->AttachNode(pControl, FIRSTCHILD);
	}
	else
	{
		pCopyContour->AttachNode(pControl, LASTCHILD);
	}

	// Feathers and other GLA's get attached to the NodeCompoundController
/*	Node* pN = FindFirstChild();
	while(pN->IsAnAttribute())
	{
		NodeAttribute* pNA = (NodeAttribute*) pN->PublicCopy();
		pControl->AttachNode(pNA, FIRSTCHILD);
		pNA->LinkToGeometry(pControl);
		pN = pN->FindNext();
	}*/

	return pControl;
}


/***********************************************************************************************

>	BOOL NodeContourController::CanBecomeA(BecomeA* pBecomeA)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Can this node turn itself into a node of a particular type ?

***********************************************************************************************/
BOOL NodeContourController::CanBecomeA(BecomeA* pBecomeA)
{
	// Can only convert to paths
	if (!pBecomeA->BAPath())
		return FALSE;

	if (pBecomeA->IsCounting())
	{
		NodeRenderableInk* pNode = (NodeRenderableInk*)FindFirstChild(CC_RUNTIME_CLASS(NodeRenderableInk));
		while (pNode)
		{
			if (!pNode->NeedsParent(NULL))
			{
				// Update number of children that can become a path
				pNode->CanBecomeA(pBecomeA);
			}

			pNode = (NodeRenderableInk*)pNode->FindNext(CC_RUNTIME_CLASS(NodeRenderableInk));
		}
	}
	
	return TRUE;
}

/***********************************************************************************************

>	String NodeContourController::Describe(BOOL Plural, BOOL Verbose)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Gets the string descriptor for this object

***********************************************************************************************/
String NodeContourController::Describe(BOOL Plural, BOOL Verbose)
{
	String Name;
	Name.Load(_R(IDS_CONTOUR_NODE_NAME));

	if (Plural)
	{
		Name += _T("s");
	}

	return Name;
}

/***********************************************************************************************

>	ChangeCode NodeContourController::OnChildChange(ObjChangeParam* pParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Regenerates the node

***********************************************************************************************/
ChangeCode NodeContourController::OnChildChange(ObjChangeParam* pParam)
{
	BOOL bRegen = TRUE;
	BOOL bCache = FALSE;

	if (m_PerformedExtend)
	{
		// this means that we have done an extend
		// which added the regen action into the action list
		// so no need to regen the node
		// just reset the flag and leave (sjk 4/8/2000)
		m_PerformedExtend = FALSE;
		return CC_OK;
	}

//	Node * pChild = NULL;

//	Node * pNextChild = NULL;

	UndoableOperation* pOp = NULL;
	if (pParam->GetOpPointer())
	{
		if (pParam->GetOpPointer()->IsKindOf(CC_RUNTIME_CLASS(UndoableOperation)))
		{
			pOp = pParam->GetOpPointer();
		}
	}

	Document * pDoc = Document::GetCurrent();

	Spread * pSpread = (Spread *)FindParent(CC_RUNTIME_CLASS(Spread));

	if (!pOp)
	{
		// invalidate me & return
		if (pDoc)
		{
			if (pParam->GetChangeFlags ().RegenerateNode)
			{
				RegenerateNode(pOp, bCache, FALSE);		// CGS:  it is now legitimate for us to
														// do this
			}

			pDoc->ForceRedraw(pSpread, GetBoundingRect(FALSE, FALSE), FALSE, this);
		}

		return CC_OK;
	}

//	CCRuntimeClass * pClass = NULL;

//	ActionList * pActionList = NULL;

//	Action *pAction = NULL;

//	BOOL bDelete = FALSE;
//	INT32 NumChildrenThatNeedMe = 0;

	List TextNodeList;
//	NodeListItem * pItem = NULL;

	List ChildList;

//	Node * pParent = NULL;

	DocRect Bounds;

//	BlobManager* BlobMgr = GetApplication()->GetBlobManager();
//	INT32 BlobSize = 0;

	TRACEUSER( "DavidM", _T("Contour - onchildchange\n"));

	if (pParam->GetChangeType() == OBJCHANGE_FINISHED)
	{
		// always force a redraw
//		if (pDoc && pSpread)
//		{
//			// do the cached bounding rect first
//			pDoc->ForceRedraw(pSpread, BoundingRectangle, FALSE, this);
//			pDoc->ForceRedraw(pSpread, GetBoundingRect(), FALSE, this);
//		}
		
		if (pOp != NULL)
		{ 
			// Ok! Tell all my children to regenerate (in some cases !)

			// whatever happens, invalidate this region
//			ReleaseCached();
//			pOp->DoInvalidateRegion(pSpread, BoundingRectangle);
			
PORTNOTE("other","Removed OpPathNudge usage")
#ifndef EXCLUDE_FROM_XARALX
			if( pOp->IS_KIND_OF(OpPathNudge) )
			{
				bRegen = TRUE;
			}
			// check for ops which we do nothing with
			else
#endif
			if (pOp->IsKindOf(CC_RUNTIME_CLASS(TransOperation)) &&
				!pOp->IS_KIND_OF(OpMovePathPoint))
			{
				if (IsSelected())
					return CC_OK;

				return NodeGroup::OnChildChange(pParam);
			}
			else if (pOp->IsKindOf(CC_RUNTIME_CLASS(OpApplyAttrib)))
			{
				if (pSpread)
				{
					ReleaseCached();
					pOp->DoInvalidateRegion( pSpread, GetBoundingRect() );
				}
				
				CCRuntimeClass * pClass = ((OpApplyAttrib *)pOp)->GetValueChangeType();

				bRegen = FALSE;

				// if we don't have a class for the attribute being applied, try another
				// method
				if (pClass)
				{
					// only regen for text changes, or for arrow head / tail changes
					if (pClass->IsKindOf(CC_RUNTIME_CLASS(AttrTxtBase)) ||
						pClass->IsKindOf(CC_RUNTIME_CLASS(AttrStartArrow)) ||
						pClass->IsKindOf(CC_RUNTIME_CLASS(AttrEndArrow)))
					{
												
						bRegen = TRUE;
					}
					else if (pClass->IS_KIND_OF(AttrStrokeColour))
					{
						bRegen = FALSE;
					}
					else if (pClass->IsKindOf(CC_RUNTIME_CLASS(AttrLineWidth)))
					{
						// need to check the selection to see if there are any non-closed
						// paths in it - if so, we need to regenerate
						bRegen = FALSE;
						
						Range * pSel = GetApplication()->FindSelection();

						if (pSel)
						{
							Node * pStepNode = pSel->FindFirst();

							while (pStepNode)
							{
								if (pStepNode->IsNodePath())
								{
									if ( !((NodePath*)pStepNode)->InkPath.IsClosed() )
									{
										bRegen = TRUE;
										break;
									}
								}

								pStepNode = pSel->FindNext(pStepNode);
							}
						}
					}
					else if (pClass->IS_KIND_OF(AttrJoinType))
					{
						// join type attribute being applied ? then apply it
						// to me !
						if (DealWithJoinTypeChange(pOp))
							return CC_OK;

						return CC_FAIL;
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

				if (pSpread)
				{
					ReleaseCached();
					pOp->DoInvalidateRegion(pSpread, GetBoundingRect());
				}

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
				// MRH 1/7/00 - If this is kept false then we do not get the right results when deleting an object out of a group!
//				bRegen = FALSE;
				bRegen = TRUE;
			}
			else if (pOp->IsKindOf(CC_RUNTIME_CLASS(OpCopy)))
			{
				bRegen = FALSE; // copying doesn't change the appearance of the contour
			}
			else if (pOp->IsKindOf(CC_RUNTIME_CLASS(OpCut)))
			{
				bRegen = TRUE; //FALSE; // cutting can change the contours appearance as shapes/letters are removed
			}
			else if (pOp->IsKindOf(CC_RUNTIME_CLASS(OpPaste)))
			{
				bRegen = TRUE; //FALSE; // pasting can change the contours appearance as shapes/letters are added
			}
PORTNOTE("other","Removed OpDeleteTextStory & OpTextFormat usage")
#ifndef EXCLUDE_FROM_XARALX
			else if (pOp->IsKindOf(CC_RUNTIME_CLASS(OpTextFormat)))
			{
				return NodeGroup::OnChildChange(pParam);			
			} 
			else if (pOp->IsKindOf(CC_RUNTIME_CLASS(OpDeleteTextStory)))
			{
				// regen for all other ops
				return NodeGroup::OnChildChange(pParam);
			}
#endif
			else if (pOp->IsKindOf(CC_RUNTIME_CLASS(CarbonCopyOp)))
			{
				bRegen = FALSE; // copying doesn't change the appearance of the contour
			}

			if (bRegen)
			{
				// invalidate the old region
				if (pSpread)
				{
					ReleaseCached();
					pOp->DoInvalidateRegion(pSpread, BoundingRectangle);					
				}
				
				RegenerateNode(pOp, bCache, FALSE);
				return CC_OK;

			}
		}
		else
		{
			// do nothing
		}			
	}

	return NodeGroup::OnChildChange(pParam);
}

/***********************************************************************************************

>	BOOL NodeContourController::DealWithJoinTypeChange(UndoableOperation * pOp)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Promotes any join type change messages up to me

***********************************************************************************************/
BOOL NodeContourController::DealWithJoinTypeChange(UndoableOperation * pOp)
{
	// first, find the attribute which has been applied
	/*
	Node * pNode = FindFirstDepthFirst();
	Node * pAttr = NULL;

	while (pNode)
	{
		if (pNode->IsAnAttribute() && pNode->FindParent()->IsSelected())
		{
			if (pNode->IS_KIND_OF(AttrJoinType))
			{
				pAttr = pNode;
				break;
			}
		}

		pNode = pNode->FindNextDepthFirst(this);
	}

	// failed to find an therefore must be a default attribute or the attribute has
	// been promoted
	if (!pAttr)
	{
		// get the default attribute applied to me
		if (!FindAppliedAttribute(CC_RUNTIME_CLASS(AttrJoinType), (NodeAttribute **)&pAttr))
		{
			// still can't find an attribute ? Regen anyway
			return RegenerateNode(pOp, FALSE, TRUE);
		}

		if (!pAttr)
		{
			// still can't find an attribute ? Regen anyway
			return RegenerateNode(pOp, FALSE, TRUE);
		}
	}

	// now we've found the attribute which has been applied, then apply this to me !

	JoinTypeAttribute jt;
	jt.JoinType = ((AttrJoinType *)pAttr)->Value.JoinType;

	if (!NodeAttribute::ApplyAttributeToNode(this, CC_RUNTIME_CLASS(AttrJoinType),
		&jt, pOp))
	{
		return FALSE;
	}

	BOOL ok = RegenerateNode(pOp, FALSE, TRUE);
	*/

// Changed this because RegenerateNode rteurn value now means "done something" or "not done something"
// both of which are OK
//	BOOL ok = RegenerateNode(pOp, FALSE, TRUE);
//
//	return ok;
	RegenerateNode(pOp, FALSE, TRUE);
	return TRUE;
}




/***********************************************************************************************

>	BOOL NodeContourController::AllowOp(ObjChangeParam *pParam, BOOL SetOpPermissionState = TRUE,
																BOOL DoPreTriggerEdit = TRUE)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>; Karim 20/01/2000
	Created:	12/8/99
	Inputs:		pParam					describes the way an op wants to change the node
				SetOpPermissionState	if TRUE the OpPermission of nodes should be set
				DoPreTriggerEdit		if TRUE then NameGallery::PreTriggerEdit is called.
										*Must* be TRUE if the calling Op may make any nodes
										change their bounds, eg move, line width, cut.
										Use TRUE if unsure.
	Purpose:	Does the allow op mechanism

***********************************************************************************************/

BOOL NodeContourController::AllowOp(ObjChangeParam *pParam, BOOL SetOpPermissionState,
															BOOL DoPreTriggerEdit)
{
	ERROR2IF(pParam==NULL,FALSE,"NodeContourController::AllowOp() - pParam==NULL");

	// Set up a flag to see if any of the child objects get changed
	BOOL allowed=TRUE;

	UndoableOperation* pOp = pParam->GetOpPointer();

	if (pOp)
	{
PORTNOTE("other","NodeContourController::AllowOp - removed OpCreateBevel test")
#ifndef EXCLUDE_FROM_XARALX
		// can't bevel a contoured object
		if (pOp->IsKindOf(CC_RUNTIME_CLASS(OpCreateBevel)))
			allowed = FALSE;
		else
#endif
		if (pOp->IsKindOf(CC_RUNTIME_CLASS(OpRemoveBlend)))
		{
			allowed = FALSE;
			pParam->SetReasonForDenial(_R(IDS_CANT_REMOVE_BLEND_WHEN_CONTOURED));
		}
	}

	if (pParam->GetDirection() != OBJCHANGE_CALLEDBYCHILD ||
		pParam->GetCallingChild() != NULL)
	{
		BOOL AnyAllowed = AllowOp_AccountForCompound( pParam,
													  SetOpPermissionState,
													  DoPreTriggerEdit );

		// if called by a parent, just pass this result back
		if (pParam->GetDirection() == OBJCHANGE_CALLEDBYPARENT)
			return AnyAllowed;
	}

	Spread * pSpread = (Spread *)FindParent(CC_RUNTIME_CLASS(Spread));
	if (!pSpread)
		pSpread = Document::GetSelectedSpread();

	// decide if we allow it ... err we do!
	// if we allowed it, see if our parents do ...
	if (allowed && Parent!=NULL && pParam->GetDirection()!=OBJCHANGE_CALLEDBYPARENT)
	{
		ObjChangeDirection OldDirection=pParam->GetDirection();
		pParam->SetCallingChild(this);
		pParam->SetDirection(OBJCHANGE_CALLEDBYCHILD);
		allowed=Parent->AllowOp(pParam,SetOpPermissionState,DoPreTriggerEdit);
		pParam->SetDirection(OldDirection);
	}

	// if setting permisions ...
	if (SetOpPermissionState)
	{
		// if allowed, say it's ok to proceed
		if (allowed)
		{
			SetOpPermission(PERMISSION_ALLOWED, TRUE);
			
			if (pParam->GetDirection()==OBJCHANGE_CALLEDBYCHILD || pParam->GetChangeFlags().Attribute)
			{
				if (pOp!=NULL)
				{
					if (allowed) 
						allowed=pOp->DoInvalidateNodeRegion(this,TRUE);
					if (allowed) 
						allowed=pOp->DoInvalidateNodeRegion((NodeContour *)this->FindFirstChild(CC_RUNTIME_CLASS(NodeContour)),TRUE);
					if (allowed && pSpread) 
						allowed=pOp->DoInvalidateRegion(pSpread, GetBoundingRect());
				}
			}
		}
		else
			SetOpPermission(PERMISSION_DENIED,TRUE);
	}

	// if we're ok so far and were asked to do a PreTriggerEdit, then
	// determine whether the Op may change the bounds of some nodes.
	// If it may, then call NameGallery::PreTriggerEdit.
	if (allowed && DoPreTriggerEdit)
	{
		// if the Op is non-NULL then query its MayChangeNodeBounds() method.
		UndoableOperation* pChangeOp = pParam->GetOpPointer();
		if (pChangeOp != NULL && pChangeOp->MayChangeNodeBounds())
		{
			if (NameGallery::Instance())
				allowed = NameGallery::Instance()->PreTriggerEdit(pChangeOp, pParam, this);
		}
	}

	return allowed;
}

/***********************************************************************************************

>	BOOL NodeContourController::RegenerateNode(UndoableOperation * pOp = NULL,
		BOOL bCacheRender = FALSE);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Returns:	TRUE to indicate that the node was changed, FALSE if nothing changed
	Purpose:	Regenerates the node

***********************************************************************************************/
BOOL NodeContourController::RegenerateNode(UndoableOperation * pOp, BOOL bCacheRender,
										   BOOL bInformParents)
{
	// no need to regenerate if we're printing !
	if (IsPrinting())
		return FALSE;
	
	Document * pDoc = Document::GetCurrent();

	NodeContour * pContour = GetContourNode();

	// can't regenerate if we don't have a contour node
	if (!pContour)
		return FALSE;

	//Spread * pSpread = FindParentSpread(); // this can die if the item has been deleted as it is in no tree!
	Spread * pSpread = (Spread *)FindParent(CC_RUNTIME_CLASS(Spread));
	if (!pSpread)
		pSpread = Document::GetSelectedSpread();

	ERROR2IF(pSpread==NULL,FALSE,"Failed to get a spread pointer!"); // MRH 1/7/00

	if (!pOp)
	{	
		// inform parents a change is about to happen
		if (bInformParents)
			PreInformParentsOfRegenerate();

		if (pDoc && pSpread)
			pDoc->ForceRedraw(pSpread, BoundingRectangle, FALSE, this);
	
		pContour->GenerateContour();

		InvalidateBoundingRect(TRUE);
		
		if (pDoc)
			pDoc->ForceRedraw(pSpread, GetBoundingRect(TRUE, FALSE), FALSE, this);

		// inform parents that change has happened
		if (bInformParents)
			PostInformParentsOfRegenerate();
	}
	else
	{
		List ContList;
		NodeListItem * pItem = NULL;
//		RegenerateContourAction* pRegenAction = NULL;

		if (pOp)
		{
			ALLOC_WITH_FAIL(pItem, new NodeListItem, pOp);

			pItem->pNode = this;
			ContList.AddTail(pItem);
			
			// invalidate my region first
			ReleaseCached();
			if (!pOp->DoInvalidateRegion(pSpread, BoundingRectangle))
#ifndef EXCLUDE_FROM_XARALX
				if (!pOp->DoInvalidateNodeRegion(this))
#endif
					return FALSE;
			
	PORTNOTETRACE("other","NodeContourController::RegenerateNode - removed use of RegenerateContourAction");
			// set up the action to regenerate the bevel on this undo operation
#ifndef EXCLUDE_FROM_XARALX
			if (RegenerateContourAction::Init(pOp, pOp->GetUndoActionList(),&ContList,&pRegenAction,
				bCacheRender)  == AC_FAIL)
			{
				ERROR3("RegenerateContourAction::Init failed !\n");
				return FALSE;
			}
#endif			
			ContList.DeleteAll();

			// invalidate my region again
			ReleaseCached();
			if (!pOp->DoInvalidateRegion(pSpread, BoundingRectangle))
#ifndef EXCLUDE_FROM_XARALX
				if (!pOp->DoInvalidateNodeRegion(this))
#endif
					return FALSE;
		}
	}
		
	// make sure we kick the sel range
	GetApplication()->UpdateSelection();

	return TRUE;
}

/***********************************************************************************************

>	NodeContour * NodeContourController::GetContourNode()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Regenerates the node

***********************************************************************************************/
NodeContour * NodeContourController::GetContourNode()
{
	NodeContour * pNode = (NodeContour *)FindFirstChild(CC_RUNTIME_CLASS(NodeContour));

	// ERROR2IF(pNode == NULL, NULL, "Can't find contour node");

	return pNode;
}

/***********************************************************************************************

>	void NodeContourController::RenderEORDrag(RenderRegion * pRegion)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Regenerates the node

***********************************************************************************************/
void NodeContourController::RenderEorDrag(RenderRegion * pRender)
{
	BOOL bChildrenSelected = FALSE;

	Node * pNode = FindFirstChild();
	Node * pInsideNode = NULL;

	// check for a select inside drag
	while (pNode)
	{
		if (pNode->IsSelected() && pNode->IsAnObject())
		{
			bChildrenSelected = TRUE;
		}

		pNode = pNode->FindNext();
	}

	if (!bChildrenSelected)
	{
		Node * pSelParent = FindParent();
		
		while (pSelParent && pSelParent->IsAnObject())
		{
			if (pSelParent->IsSelected())
			{
				bChildrenSelected = TRUE;
				break;
			}

			pSelParent = pSelParent->FindParent();
		}
	}
	
	NodeContour * pContour = GetContourNode();

	if ((bChildrenSelected || IsSelected()))
	{
		if (GetWidth() < 0)
		{
			if (pContour)
			{	
				pContour->RenderEorDrag(pRender);
			}	
		}
		else
		{
			// render all selected nodes eor drags
			pInsideNode = FindFirstDepthFirst();
			
			while (pInsideNode && pInsideNode != this)
			{
				if (pInsideNode->IsAnObject())
				{
					((NodeRenderableInk *)pInsideNode)->RenderEorDrag(pRender);
				}
				
				pInsideNode = pInsideNode->FindNextDepthFirst(this);
			}
		}

	}
	else
	{
		// render all selected nodes eor drags
		pInsideNode = FindFirstDepthFirst();

		while (pInsideNode && pInsideNode != this)
		{
			if (pInsideNode->IsSelected() && pInsideNode->IsAnObject())
			{
				((NodeRenderableInk *)pInsideNode)->RenderEorDrag(pRender);
			}

			pInsideNode = pInsideNode->FindNextDepthFirst(this);
		}
	}
}

/***********************************************************************************************

>	INT32 NodeContourController::ComplexHide(UndoableOperation* pOp, Node* pNextInRange)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Hides this node

***********************************************************************************************/
INT32 NodeContourController::ComplexHide(UndoableOperation* pOp, Node* pNextInRange)
{
	NodeHidden * pHidden = NULL;
	BOOL ok = pOp->DoHideNode(this, TRUE, &pHidden, TRUE);

	if (ok)
		return 1;

	return -1;
}

/***********************************************************************************************

>	void NodeContourController::Transform(TransformBase &Trans)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Regenerates the node

***********************************************************************************************/
void NodeContourController::Transform(TransformBase &Trans)
{
	// See GroupCanTransformCached()
	NodeGroup::Transform(Trans);

	// regenerate for transforms which aren't scales or translations
	TransformBase *pTrans = &Trans;

	if (pTrans->IsKindOf(CC_RUNTIME_CLASS(Trans2DMatrix)))
	{
		Trans2DMatrix* pMat = (Trans2DMatrix*)pTrans;
		BOOL HasBeenFlipped = FALSE;

		if(pMat->GetWorkingQuadrant() == 2 || pMat->GetWorkingQuadrant() == 4)
		{
			HasBeenFlipped = TRUE;
		}

		// scale the width too
		if (pMat->GetAspect().MakeDouble() == 1.0 && pMat->GetSkew() == 0 && !HasBeenFlipped)
		{
			m_Width = (MILLIPOINT)(((double)m_Width) * pMat->GetScale().MakeDouble());
			BoundingRectangle = GetBoundingRect();
		}
		else
		{
			RegenerateNode(NULL, FALSE);
			BoundingRectangle = GetBoundingRect();
		}
	}
}



/***********************************************************************************************

>	BOOL NodeContourController::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Writes the node to a file

***********************************************************************************************/
BOOL NodeContourController::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	CXaraFileRecord Rec(TAG_CONTOURCONTROLLER, TAG_CONTOUR_CONTROLLER_SIZE);
	//TAG_CONTOUR_CONTROLLER_SIZE		= 4 + 4 + 1 + 8 + 8 + 8 + 8;
	BOOL ok = TRUE;

	if (ok) ok = Rec.Init();
	
	if (ok) ok = Rec.WriteINT32(m_Steps);
	if (ok) ok = Rec.WriteINT32(m_Width);

	BYTE Type = (BYTE)m_BlendType;

	if (m_bInsetPath)
		Type |= 128;

	if (ok) ok = Rec.WriteBYTE(Type);
	if (ok) ok = Rec.WriteDOUBLE((double)m_ObjBiasGain.GetBias());
	if (ok) ok = Rec.WriteDOUBLE((double)m_ObjBiasGain.GetGain());
	if (ok) ok = Rec.WriteDOUBLE((double)m_AttrBiasGain.GetBias());
	if (ok) ok = Rec.WriteDOUBLE((double)m_AttrBiasGain.GetGain());

	if (ok)	ok = pFilter->Write(&Rec);

	return ok;
}

/***********************************************************************************************

>	BOOL NodeContourController::WritePreChildrenNative(BaseCamelotFilter* pFilter)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Writes the node to a file

***********************************************************************************************/
BOOL NodeContourController::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
	return WritePreChildrenWeb(pFilter);
}

/***********************************************************************************************

>	BOOL NodeContourController::PostImport()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Creates the contour after import

***********************************************************************************************/


/***********************************************************************************************

>	BOOL NodeContourController::PostImport()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Creates the contour after import

***********************************************************************************************/
BOOL NodeContourController::PostImport()
{
	RegenerateNode(NULL, FALSE);
	return TRUE;
}

/***********************************************************************************************

>	BOOL NodeContourController::EndBlendStep(BlendNodeParam * pParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Starts the blending of a step

***********************************************************************************************/
BOOL NodeContourController::EndBlendStep(BlendNodeParam * pParam)
{
	PORTNOTETRACE("other","NodeContourController::EndBlendStep - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// CGS:  firstly, are we just rendering, or are we making shapes?

	if (!pParam->GetHandleBecomeA ())
	{
		// were just rendering ....

		BOOL okToRender = TRUE;
		
		// get hold of the current path processor ....
		if (pParam->GetPathProcessor ())
		{
			ERROR2IF(!pParam->GetPathProcessor ()->IS_KIND_OF(SumAllPathsPathProcessor),
					 FALSE, "First path processor isn't a sum all paths path processor");

			// grab the path processor
			SumAllPathsPathProcessor * pProc = (SumAllPathsPathProcessor *) pParam->GetPathProcessor ();

			pProc->SetEnabled(FALSE);		// disable it

			// we can now get on and generate intermediate blend steps.  NOTE:  this node contour
			// controller should not be touched at all - since it is held within the tree
			// so we make a copy that we can play with ....
				
			Node* copy = NULL;
			this->NodeCopy (&copy);

			NodeContourController* ptrCopy = (NodeContourController*) copy;

			if (!ptrCopy) { return (FALSE); }		// out of memory

			// find the contour
			NodeContour * pContourCopy = (NodeContour *)ptrCopy->FindFirstChild(CC_RUNTIME_CLASS(NodeContour));
			ERROR2IF(pContourCopy == NULL, FALSE, "Can't find the contour node");

#ifdef _DEBUG		// special debug info
			INT32 id = ++NodeContourController::contourLastID;
			ptrCopy->myContourID = id;
			char strId [100];
			wsprintf (strId, "Pushing NodeCountourController ID:  %i\n", ptrCopy->myContourID);	
			TRACEUSER ("ChrisS", strId);
#endif

	//////////////////////////////////////////////////////////////////////////////////////////////////

			// now the fun begins, lest blend attributes for this particular blend step ....
			
			CCAttrMap * pMap = NULL;						// if pEndControl is a contour as well
			CCAttrMap * pPrimaryContourAttrMap = NULL;		// if pEndControl is not a contour

			// blend the contour's attributes
			NodeContourController * pEndControl = (NodeContourController *)FindAssociatedBlendNode(pParam);

			if (pEndControl)
			{
				// we are blending two contours YIPEE!		
				
				NodeContour * pEndContour = (NodeContour *)pEndControl->FindFirstChild(CC_RUNTIME_CLASS(NodeContour));
				NodeRenderableInk * pOrigEndNode = pEndControl->GetBlendCreatedByNode();

				// somethings a bit wrong if either of these fire ....
				ERROR2IF(pOrigEndNode == NULL, FALSE, "Cant find the original contour node which this is based on");
				ERROR2IF(!pOrigEndNode->IS_KIND_OF(NodeContourController), FALSE, "Original node isn't a NodeContourController");

				pOrigEndNode = (NodeContour *)pOrigEndNode->FindFirstChild(CC_RUNTIME_CLASS(NodeContour));

				BlendPath MyStartPath;
				MyStartPath.Initialise((NodeRenderableInk *)pContourCopy->PublicCopy(), -1, pContourCopy, 1, 0, NULL);
				
				BlendPath MyEndPath;
				MyEndPath.Initialise((NodeRenderableInk *)pEndContour->PublicCopy(), -1, pOrigEndNode, 1, 0, NULL);
				
				BlendNodeParam BNParam;
				BNParam.Init(pParam, &MyStartPath, &MyEndPath);
				
				pMap = new CCAttrMap(30);

				BlendHelpers * pHelp = new BlendHelpers;
				ERRORIF(pHelp == NULL, _R(IDE_NOMORE_MEMORY), FALSE);
				
				// now, perform the blend of the attributes
				if (!pHelp->BlendAttributes(&BNParam, pMap))
				{
					pParam->GetRenderRegion()->PopPathProcessor();
					okToRender = FALSE;
				}
				
				delete pHelp;

				// also blend my attributes & my node contour's attributes
				ptrCopy->BlendParameters ((NodeContourController*) m_pBlendCreatedByNode, (NodeContourController*) pOrigEndNode->FindParent(), pParam->GetAttrBlendRatio ());
			}
			else
			{
				// were blending a contoured node to a non-contoured node.
				// since we have not generated an attribute map, shadows that are generated
				// render as white ....

				// we need to make a attribute map for *this* NodeShadow, and apply it to
				// the copy ....

				NodeContour * pContour = (NodeContour *) FindFirstChild (CC_RUNTIME_CLASS (NodeContour));

				ERROR2IF (pContour == NULL, FALSE, "Can't find Contour node");

				// build an attribute map for the contour
				pPrimaryContourAttrMap = CCAttrMap::MakeAppliedAttrMap(pContour);

				// we don't have another node to blend to - but lets not let a little thing like that deter us!	
				ptrCopy->BlendParameters ((NodeContourController *) m_pBlendCreatedByNode, pParam->GetAttrBlendRatio ());
			}

			// scan for the path that original contour was generated with
			NodePath* currentPath = (NodePath *)ptrCopy->FindFirstChild(CC_RUNTIME_CLASS(NodePath));

			// if we find one, we know get on and merge all the paths so that the contour can
			// be generated correctly.

			// we do this by:

			// replacing currentPath by a group that contains all paths on the path processors list
			// (i.e.  we replace it by the blended paths - who have their blended attribute maps
			// applied to them)

			if (currentPath)
			{
				currentPath->CascadeDelete ();
				delete (currentPath);
				currentPath = NULL;

				NodeGroup* newGroup = new NodeGroup ();

				ListItem* pItem = pProc->GetList ()->GetHead ();
				
				Path* blendPath = NULL;
				CCAttrMap* blendMap = NULL;

				while (pItem)
				{
					blendPath = ((SumAllPathsElem *) pItem)->GetPath ();
					blendMap = ((SumAllPathsElem *) pItem)->GetAttrMap ();
					
					NodePath* newPath = new NodePath ();
					INT32 numCoords = blendPath->GetNumCoords ();
					newPath->InkPath.ClearPath ();
					newPath->InkPath.Initialise (numCoords);
					newPath->InkPath.CopyPathDataFrom (blendPath);
					blendMap->ApplyAttributesToNode (newPath);
					newPath->AttachNode(newGroup, LASTCHILD);
					
					pItem = pProc->GetList ()->GetNext (pItem);
					delete ((SumAllPathsElem *) pProc->GetList()->RemoveHead ());
				}

				if (ptrCopy->m_Width < 0)		// an outer contour
				{
					newGroup->AttachNode(pContourCopy, NEXT);
				}
				else if (ptrCopy->m_Width > 0)
				{
					newGroup->AttachNode(pContourCopy, PREV);
				}
			}

			// apply blended contour attributes ....

			if (pMap)
			{
				pMap->ApplyAttributesToNode (pContourCopy);
			}

			if (pPrimaryContourAttrMap)
			{
				pPrimaryContourAttrMap->ApplyAttributesToNode (pContourCopy);
			}

			// generate the contour (which will correctly be generated for merged paths if needed)
			
			if (!pContourCopy->GenerateContour())
			{
				okToRender = FALSE;
			}

			// now render the contour controller as a complete node (just like the document renders)
			
			if (okToRender)
			{
				// before we render this compound node, check to see if our parent is a shadow
				// if it is, then delay rendering - since the shadow should be responsible

				Node* parent = FindParent ();

				if (parent != NULL)
				{
					if (!IS_A (parent, NodeShadowController))
					{
						RenderRegion* pRegion = pParam->GetRenderRegion();

						// render the node
						if (pRegion)
							pRegion->RenderTreeNoCache(ptrCopy);
					}
					else
					{
						// the shadow MUST be made responsible
						SetShadowDeleteThisNode (ptrCopy);
					}
				}
				else
				{
					RenderRegion* pRegion = pParam->GetRenderRegion();

					// render the node
					if (pRegion)
						pRegion->RenderTreeNoCache(ptrCopy);
				}
			}

			if (pMap)
			{
				pMap->DeleteAttributes ();
				delete pMap;
			}

			if (pPrimaryContourAttrMap)
			{
				delete pPrimaryContourAttrMap;
			}

			// if the contour is NOT shadowed, then we can safely delete the copy of it

			Node* parent = FindParent ();

			if (parent != NULL)
			{
				if (!(IS_A (parent, NodeShadowController)))
				{
					ptrCopy->CascadeDelete ();
					delete (ptrCopy);
				}
			}
			else
			{
				ptrCopy->CascadeDelete ();
				delete (ptrCopy);
			}
		}
	}
	else	// were doing a make shapes (i.e.  become a .....)
	{	
		// get hold of the current path processor ....
		if (pParam->GetPathProcessor ())
		{
			ERROR2IF(!pParam->GetPathProcessor ()->IS_KIND_OF(SumAllPathsPathProcessor),
					 FALSE, "First path processor isn't a sum all paths path processor");

			// grab the path processor
			SumAllPathsPathProcessor * pProc = (SumAllPathsPathProcessor *)pParam->GetPathProcessor ();

			pProc->SetEnabled(FALSE);		// disable it

			// we can now get on and generate intermediate blend steps.  NOTE:  this node contour
			// controller should not be touched at all - since it is held within the tree
			// so we make a copy that we can play with ....
				
			Node* copy = NULL;
			this->NodeCopy (&copy);

			NodeContourController* ptrCopy = (NodeContourController*) copy;

			if (!ptrCopy) { return (FALSE); }		// out of memory

			// find the contour
			NodeContour * pContourCopy = (NodeContour *)ptrCopy->FindFirstChild(CC_RUNTIME_CLASS(NodeContour));
			ERROR2IF(pContourCopy == NULL, FALSE, "Can't find the contour node");

	//////////////////////////////////////////////////////////////////////////////////////////////////

			// now the fun begins, lest blend attributes for this particular blend step ....
			
			CCAttrMap * pMap = NULL;						// if pEndControl is a contour as well
			CCAttrMap * pPrimaryContourAttrMap = NULL;		// if pEndControl is not a contour

			// blend the contour's attributes
			NodeContourController * pEndControl = (NodeContourController *)FindAssociatedBlendNode(pParam);

			if (pEndControl)
			{
				// we are blending two contours YIPEE!		
				
				NodeContour * pEndContour = (NodeContour *)pEndControl->FindFirstChild(CC_RUNTIME_CLASS(NodeContour));
				NodeRenderableInk * pOrigEndNode = pEndControl->GetBlendCreatedByNode();

				// somethings a bit wrong if either of these fire ....
				ERROR2IF(pOrigEndNode == NULL, FALSE, "Cant find the original contour node which this is based on");
				ERROR2IF(!pOrigEndNode->IS_KIND_OF(NodeContourController), FALSE, "Original node isn't a NodeContourController");

				pOrigEndNode = (NodeContour *)pOrigEndNode->FindFirstChild(CC_RUNTIME_CLASS(NodeContour));

				BlendPath MyStartPath;
				MyStartPath.Initialise((NodeRenderableInk *)pContourCopy->PublicCopy(), -1, pContourCopy, 1, 0, NULL);
				
				BlendPath MyEndPath;
				MyEndPath.Initialise((NodeRenderableInk *)pEndContour->PublicCopy(), -1, pOrigEndNode, 1, 0, NULL);
				
				BlendNodeParam BNParam;
				BNParam.Init(pParam, &MyStartPath, &MyEndPath);
				
				pMap = new CCAttrMap(30);

				BlendHelpers * pHelp = new BlendHelpers;
				ERRORIF(pHelp == NULL, _R(IDE_NOMORE_MEMORY), FALSE);
				
				// now, perform the blend of the attributes
				if (!pHelp->BlendAttributes(&BNParam, pMap))
				{
					pParam->GetRenderRegion()->PopPathProcessor();
				}
				
				delete pHelp;

				// also blend my attributes & my node contour's attributes
				ptrCopy->BlendParameters ((NodeContourController*) m_pBlendCreatedByNode, (NodeContourController*) pOrigEndNode->FindParent(), pParam->GetAttrBlendRatio ());
			}
			else
			{
				// were blending a contoured node to a non-contoured node.
				// since we have not generated an attribute map, shadows that are generated
				// render as white ....

				// we need to make a attribute map for *this* NodeShadow, and apply it to
				// the copy ....

				NodeContour * pContour = (NodeContour *) FindFirstChild (CC_RUNTIME_CLASS (NodeContour));

				ERROR2IF (pContour == NULL, FALSE, "Can't find Contour node");

				// build an attribute map for the contour
				pPrimaryContourAttrMap = CCAttrMap::MakeAppliedAttrMap(pContour);

				// we don't have another node to blend to - but lets not let a little thing like that deter us!	
				ptrCopy->BlendParameters ((NodeContourController *) m_pBlendCreatedByNode, pParam->GetAttrBlendRatio ());
			}

			// scan for the path that original contour was generated with
			NodePath* currentPath = (NodePath *)ptrCopy->FindFirstChild(CC_RUNTIME_CLASS(NodePath));

			// if we find one, we know get on and merge all the paths so that the contour can
			// be generated correctly.

			// we do this by:

			// replacing currentPath by a group that contains all paths on the path processors list
			// (i.e.  we replace it by the blended paths - who have their blended attribute maps
			// applied to them)

			std::list<NodePath*> pathPtrs;				// a list of the new paths (so that we may call normaliseattributes)

			if (currentPath)
			{
				currentPath->CascadeDelete ();
				delete (currentPath);
				currentPath = NULL;

				NodeGroup* newGroup = new NodeGroup ();

				ListItem* pItem = pProc->GetList ()->GetHead ();
				
				Path* blendPath = NULL;
				CCAttrMap* blendMap = NULL;

				while (pItem)
				{
					blendPath = ((SumAllPathsElem *) pItem)->GetPath ();
					blendMap = ((SumAllPathsElem *) pItem)->GetAttrMap ();
					
					NodePath* newPath = new NodePath ();
					INT32 numCoords = blendPath->GetNumCoords ();
					newPath->InkPath.ClearPath ();
					newPath->InkPath.Initialise (numCoords);
					newPath->InkPath.CopyPathDataFrom (blendPath);
					blendMap->ApplyAttributesToNode (newPath);
					newPath->AttachNode(newGroup, LASTCHILD);

					pathPtrs.push_back (newPath);
					
					pItem = pProc->GetList ()->GetNext (pItem);
					delete ((SumAllPathsElem *) pProc->GetList()->RemoveHead ());
				}

				if (ptrCopy->m_Width < 0)		// an outer contour
				{
					newGroup->AttachNode(pContourCopy, NEXT);
				}
				else if (ptrCopy->m_Width > 0)
				{
					newGroup->AttachNode(pContourCopy, PREV);
				}

//				newGroup->NormaliseAttributes ();
			}

			// apply blended contour attributes ....

			if (pMap)
			{
				pMap->ApplyAttributesToNode (pContourCopy);
			}

			if (pPrimaryContourAttrMap)
			{
				pPrimaryContourAttrMap->ApplyAttributesToNode (pContourCopy);
			}

			// generate the contour (which will correctly be generated for merged paths if needed)
			
			pContourCopy->GenerateContour();

			// now insert into the tree or passback ....

			BecomeA* accessPtr = pParam->GetHandleBecomeA ()->GetBecomeA ();
			UndoableOperation* pUndoOp = accessPtr->GetUndoOp ();

			Node* parent = NULL;
			NodeRenderableInk* pCreator = GetBlendCreatedByNode ();
			ERROR2IF(pCreator == NULL, FALSE, "Cant find the original bevel node which this is based on");
			
			parent = pCreator->FindParent (CC_RUNTIME_CLASS (NodeShadowController));

			// we MUST also scan to see if the blend was shadowed (and not the bevel) ....
			
			Node* pBlenderParent = NULL;
			Node* pBlender = pParam->GetNodeBlend ();
			ERROR2IF(pBlender == NULL, FALSE, "Cant find the original node blend which this node is based on!");

			pBlenderParent = pBlender->FindParent (CC_RUNTIME_CLASS (NodeShadowController));

			BOOL parentIsNodeShadow = FALSE;

			if (parent)
			{
				if (!pBlenderParent)	// if the blend was NOT shadowed
				{
					parentIsNodeShadow = TRUE;
				}
				// else the blend was shadowed we need to insert into the tree
			}

			// are we wanting to be inserted into the tree?
			if (accessPtr->GetReason () == BECOMEA_REPLACE)
			{
				if (!parentIsNodeShadow)
				{
					if (!pUndoOp->DoInsertNewNode (ptrCopy,pParam->GetHandleBecomeA ()->GetContextNode (),PREV,TRUE,FALSE,FALSE,TRUE))
					{
						return FALSE;
					}

					// now normalise all attributes ....
					
					pContourCopy->NormaliseAttributes ();

					std::list<NodePath*>::iterator i;

					for (i = pathPtrs.begin (); i != pathPtrs.end (); i++)
					{
						(*i)->NormaliseAttributes ();
					}

					// if we are required to insert paths into the tree - THEN we MUST call DoBecomeA on
					// ptrCopy.  This call now also creates the bevel bitmap for us

					if (accessPtr->GetInsertComplexBlendStepsAsPaths ())
					{
						ptrCopy->DoBecomeA (accessPtr);
					}
				}
				else
				{
					SetShadowThisNode (ptrCopy);	// we need to set this so that NodeShadowController
													// can shadow ptrCopy
				}
			}
			// or do we just want our paths?
			else if (accessPtr->GetReason () == BECOMEA_PASSBACK)
			{
				if (parentIsNodeShadow)
				{
					SetShadowThisNode (ptrCopy);	// we need to set this so that NodeShadowController
													// can shadow ptrCopy
				}
				ptrCopy->DoBecomeA (accessPtr);
				ptrCopy->SetBlenderNode (pParam->GetNodeBlend ());

				// setup necessary stuff so that we can delete ptrCopy at the relevant time ....
					
				if (AllocatedBlendConsList (LT_BECOMEA_CONTOURSLIST))
				{	
					BlendConsListInsert (LT_BECOMEA_CONTOURSLIST, ptrCopy);
				}
				else
				{
					AllocBlendConsList (LT_BECOMEA_CONTOURSLIST);
					BlendConsListInsert (LT_BECOMEA_CONTOURSLIST, ptrCopy);
				}
			}

			if (pMap)
			{
				pMap->DeleteAttributes ();
				delete pMap;
			}

			if (pPrimaryContourAttrMap)
			{
				delete pPrimaryContourAttrMap;
			}
		}
	}
	
	return TRUE;
#else // EXCLUDE_FROM_XARALX
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL NodeContourController::BlendParameters (NodeContourController * pStart,
												 NodeContourController * pEnd, double Scale)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/2000
	Inputs:		pStart	-	the original start contour controller node (the one in the tree)
				pEnd	-	the original end contour controller node (the one in the tree)
				Scale	-	Scale factor to blend with
	Outputs:	Sets my & my shadow node child with the appropriate variables
	Returns:	
	Purpose:	Sets my & my shadow node child with the appropriate variables given the blend
				step
	
********************************************************************************************/

BOOL NodeContourController::BlendParameters (NodeContourController * pStart, NodeContourController * pEnd, double Scale)
{
	// First, lets get hold of the shadows that we need, and check.....
	
	NodeContour * pStartContour = (NodeContour *) pStart->GetContour ();
	ERROR2IF(!pStartContour, FALSE, "Can't find Contour child node for start");

	NodeContour * pEndContour = (NodeContour *) pEnd->GetContour ();
	ERROR2IF(!pEndContour, FALSE, "Can't find Contour child node for end");

	NodeContour * pThisContour = (NodeContour *) GetContour();
	ERROR2IF(!pThisContour, FALSE, "Can't find Contour child node for blend node");

	// lets blend the number of steps first ....

	double startSteps = pStart->m_Steps;
	double endSteps = pEnd->m_Steps;

	double newSteps = ((endSteps - startSteps) * Scale) + startSteps;
	// need to manually round newSteps - cause if we just cast straight to a UINT32, things look bad

	if (newSteps < 0) { newSteps = 1; }		// if negative, the cast screws up

	UINT32 temp = (UINT32) newSteps;
	double remainder = (double) (newSteps - temp);
	if (remainder <= 0.5)	{ m_Steps = temp; }
	else					{ m_Steps = temp + 1; }

	if (m_Steps <= 0) { m_Steps = 1; }		// cannot allow this to happen

	// now lets blend the contour width ....

	double startWidth = pStart->m_Width;
	double endWidth = pEnd->m_Width;

	double newWidth = ((endWidth - startWidth) * Scale) + startWidth;
	m_Width =  (INT32) newWidth;

	// now lets blend the profiles ....

	double StartBias = pStart->m_AttrBiasGain.GetBias();
	double EndBias   = pEnd->m_AttrBiasGain.GetBias();
	double NewBias   = ((EndBias - StartBias) * Scale) + StartBias;

	double StartGain = pStart->m_AttrBiasGain.GetGain();
	double EndGain   = pEnd->m_AttrBiasGain.GetGain();
	double NewGain   = ((EndGain - StartGain) * Scale) + StartGain;

	m_AttrBiasGain.SetBiasGain (NewBias, NewGain);

	StartBias = pStart->m_ObjBiasGain.GetBias();
	EndBias   = pEnd->m_ObjBiasGain.GetBias();
	NewBias   = ((EndBias - StartBias) * Scale) + StartBias;

	StartGain = pStart->m_ObjBiasGain.GetGain();
	EndGain   = pEnd->m_ObjBiasGain.GetGain();
	NewGain   = ((EndGain - StartGain) * Scale) + StartGain;

	m_ObjBiasGain.SetBiasGain (NewBias, NewGain);

	return (TRUE);
}

/********************************************************************************************

>	BOOL NodeContourController::BlendParameters(NodeContourController * pStart, double Scale)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/2000
	Inputs:		pStart	-	the original start contour controller node (the one in the tree)
				Scale	-	Scale factor to blend with
	Outputs:	Sets my & my shadow node child with the appropriate variables
	Returns:	
	Purpose:	Sets my & my shadow node child with the appropriate variables given the blend
				step
	
********************************************************************************************/

BOOL NodeContourController::BlendParameters(NodeContourController * pStart, double Scale)
{
	// First, lets get hold of the shadows that we need, and check ....
	
	NodeContour * pStartContour = (NodeContour *) pStart->GetContour ();
	ERROR2IF(!pStartContour, FALSE, "Can't find Contour child node for start");

	NodeContour * pThisContour = (NodeContour *) GetContour();
	ERROR2IF(!pThisContour, FALSE, "Can't find Contour child node for blend node");

	// lets blend the number of steps first ....

	double startSteps = pStart->m_Steps;
	double endSteps = 0;//pStart->m_Steps / 2;

	double startSteps2 = pStart->m_Steps;
	double endSteps2 = 0;//pStart->m_Steps / 2;

	//double newSteps = 
	
	double newSteps = -((endSteps - startSteps) * Scale);
	double newSteps2 = -((endSteps2 - startSteps2) * (1-Scale));
	// need to manually round newSteps - cause if we just cast straight to a UINT32, things look bad

	newSteps = newSteps2 - newSteps;

	if (newSteps < 0) { newSteps = 1; }		// if negative, the cast screws up

	UINT32 temp = (UINT32) newSteps;
	double remainder = (double) (newSteps - temp);
	if (remainder <= 0.5)	{ m_Steps = temp; }
	else					{ m_Steps = temp + 1; }

	if (m_Steps <= 0) { m_Steps = 1; }			// cannot allow this to happen

	// now lets blend the contour width ....

	double startWidth = pStart->m_Width;
	double endWidth = 0;

/*	double startWidth2 = pStart->m_Width;
	double endWidth2 = 0;

	double newWidth = -((endWidth - startWidth) * Scale);
	double newWidth2 = -((endWidth2 - startWidth2) * (1-Scale));

	newWidth = - (newWidth2 - newWidth);*/

	double newWidth = ((endWidth - startWidth) * Scale) + startWidth;
	m_Width =  (INT32) newWidth;

	// now lets blend the profiles ....

	CProfileBiasGain defaultBiasGain;
	
	double StartBias = pStart->m_AttrBiasGain.GetBias();
	double EndBias   = defaultBiasGain.GetBias();
	double NewBias   = ((EndBias - StartBias) * Scale) + StartBias;

	double StartGain = pStart->m_AttrBiasGain.GetGain();
	double EndGain   = defaultBiasGain.GetGain();
	double NewGain   = ((EndGain - StartGain) * Scale) + StartGain;

	m_AttrBiasGain.SetBiasGain (NewBias, NewGain);

	StartBias = pStart->m_ObjBiasGain.GetBias();
	EndBias   = defaultBiasGain.GetBias();
	NewBias   = ((EndBias - StartBias) * Scale) + StartBias;

	StartGain = pStart->m_ObjBiasGain.GetGain();
	EndGain   = defaultBiasGain.GetGain();
	NewGain   = ((EndGain - StartGain) * Scale) + StartGain;

	m_ObjBiasGain.SetBiasGain (NewBias, NewGain);

	return (TRUE);
}


/********************************************************************************************
>	NodeShadow* NodeContourController::GetShadow()

 	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/2000
	Inputs:		-
	Returns:	Pointer to child contour (NULL if not found!)
	Purpose:	Returns a type correct pointer to child contour
********************************************************************************************/

NodeContour* NodeContourController::GetContour ()
{
	NodeContour* pBob = (NodeContour*) FindFirstChild (CC_RUNTIME_CLASS (NodeContour));

	return pBob;
}


/***********************************************************************************************

>	Node* NodeContourController::SimpleCopy()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Creates a copy of this node

***********************************************************************************************/
Node* NodeContourController::SimpleCopy()
{
	NodeContourController* pNode = new NodeContourController;
	ERRORIF(pNode == NULL, _R(IDE_NOMORE_MEMORY), NULL);

	if (pNode)
	{
		CopyNodeContents(pNode);
	}

	return pNode;
}

/********************************************************************************************
>	void NodeContourController::CopyNodeContents(NodeContourController* pNewNode)

 	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/01/2004
	Inputs:		-
	Returns:	
	Purpose:	Sort of backwards copy constructor

********************************************************************************************/
void NodeContourController::CopyNodeContents(NodeContourController* pNewNode)
{
	pNewNode->m_Steps = m_Steps;
	pNewNode->m_Width = m_Width;
	pNewNode->m_BlendType = m_BlendType;
	pNewNode->m_AttrBiasGain = m_AttrBiasGain;
	pNewNode->m_ObjBiasGain = m_ObjBiasGain;
	pNewNode->m_bInsetPath = m_bInsetPath;
	pNewNode->m_PerformedExtend = m_PerformedExtend;

	if (pNewNode->m_pPathProc)
		delete pNewNode->m_pPathProc;
	pNewNode->m_pPathProc = NULL;

	// These shouldn't be here!
	pNewNode->m_bBlendStartNode = m_bBlendStartNode;
	pNewNode->m_bBlendEndNode = m_bBlendEndNode;
	pNewNode->m_pBlendCreatedByNode = m_pBlendCreatedByNode;

	NodeGroup::CopyNodeContents(pNewNode);
}




/***********************************************************************************************
>   void NodeContourController::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeContourController::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeContourController), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeContourController))
		CopyNodeContents((NodeContourController*)pNodeCopy);
}



   
/***********************************************************************************************

>	BOOL NodeContourController::AreOpenPathsInChildren()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		
	Purpose:	Returns TRUE if any of my children are open paths

***********************************************************************************************/
BOOL NodeContourController::AreOpenPathsInChildren()
{
	Node * pNode = FindFirstChild();

	while (pNode)
	{
		if (pNode->IsNodePath())
		{
			if (!((NodePath *)pNode)->InkPath.IsSubPathClosed(0))
			{
				return TRUE;
			}
		}

		pNode = pNode->FindNext();
	}

	return FALSE;
}



/********************************************************************************************

>	virtual DocRect NodeContourController::ValidateExtend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/1999
	Inputs:		ExtParams		description parameters for the extension.
	Outputs:	
	Returns:	TRUE if extending this Node and its children is a reversible operation,
				FALSE otherwise.
	Purpose:	Tests the reversibility of an Extend operation applied to this node.

				In the case of a NodeContourController, this function is identical to
				Node's implementation, except that this controller's own NodeContour
				child is ignored. This allows the NodeContour to tell its parent to
				extend, without fear of infinite recursion.
	Errors:		
	See also:	IsTypeExtendible(), Extend().

********************************************************************************************/
DocRect NodeContourController::ValidateExtend(const ExtendParams& ExtParams)
{
	Node* pBob = GetContourNode();
	DocRect drMinExtend(INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX), drThisMinExtend;
	for (	Node* pChildNode = FindFirstChild();
			pChildNode != NULL;
			pChildNode = pChildNode->FindNext()	)
	{
		if (pChildNode == pBob)
			continue;

		drThisMinExtend = pChildNode->ValidateExtend(ExtParams);
		if (drMinExtend.lo.x < drThisMinExtend.lo.x) drMinExtend.lo.x = drThisMinExtend.lo.x;
		if (drMinExtend.lo.y < drThisMinExtend.lo.y) drMinExtend.lo.y = drThisMinExtend.lo.y;
		if (drMinExtend.hi.x < drThisMinExtend.hi.x) drMinExtend.hi.x = drThisMinExtend.hi.x;
		if (drMinExtend.hi.y < drThisMinExtend.hi.y) drMinExtend.hi.y = drThisMinExtend.hi.y;
	}
	return drMinExtend;
}



/********************************************************************************************

>	virtual void NodeContourController::Extend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/1999
	Inputs:		ExtParams		description parameters for the extension.
	Outputs:	Some of this node's children may have their dimensions altered.
	Returns:	
	Purpose:	Perform an Extend operation on this Node, and its children if appropriate.

				In the case of a NodeContourController, this function is identical to
				Node's implementation, except that this controller's own NodeContour
				child is ignored. This allows the NodeContour to tell its parent to
				extend, without fear of infinite recursion.
	Errors:		
	See also:	

********************************************************************************************/
void NodeContourController::Extend(const ExtendParams& ExtParams)
{
	Node* pBob = GetContourNode();
	for (	Node* pChildNode = FindFirstChild();
			pChildNode != NULL;
			pChildNode = pChildNode->FindNext()	)
	{
		if (pChildNode == pBob)
			continue;

		pChildNode->Extend(ExtParams);
	}

	// Add an action to regen the Contour here
	// and instead of regening the Contour when the m_PerformedExtend occurs in the child change
	// cut out and let the action do the stuff
	m_PerformedExtend = TRUE; 
	RegenerateNode(ExtParams.pOp, FALSE, FALSE);
}



void NodeContourController::PreExportRender(RenderRegion* pRegion)
{
#ifdef DO_EXPORT
PORTNOTE("epsfilter", "Removed use of EPSFilter")
#ifndef EXCLUDE_FROM_XARALX
	if (pRegion->IsKindOf(CC_RUNTIME_CLASS(EPSRenderRegion)))
	{
		// Output "start group" token
		EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();
		pDC->OutputToken("u");
		pDC->OutputNewLine();
	}
	else if(pRegion->IsKindOf(CC_RUNTIME_CLASS(CMXRenderRegion)))
	{
		// mark start of a group...
		CMXExportDC *pDC = (CMXExportDC *) pRegion->GetRenderDC();
		DocRect BBox = GetBoundingRect();
		pDC->StartGroup(&BBox);
	}
#endif
#endif
}

BOOL NodeContourController::ExportRender(RenderRegion* pRegion) 
{
#ifdef DO_EXPORT
PORTNOTE("epsfilter", "Removed use of EPSFilter")
#ifndef EXCLUDE_FROM_XARALX
	if (pRegion->IsKindOf(CC_RUNTIME_CLASS(EPSRenderRegion)))
	{
		// Output "end group" token
		EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();
		pDC->OutputToken("U");
		pDC->OutputNewLine();
		
		// Tell caller we rendered ourselves ok
		return TRUE;
	}
	else if(pRegion->IsKindOf(CC_RUNTIME_CLASS(CMXRenderRegion)))
	{
		// mark start of a group...
		CMXExportDC *pDC = (CMXExportDC *) pRegion->GetRenderDC();
		pDC->EndGroup();

		return TRUE;
	}
#endif
#endif
	// Render thid node in the normal way
	return FALSE;
}

/********************************************************************************************

>	void NodeGroup::RenderTinyBlobs(RenderRegion* pRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Inputs:		pRender - The region to draw the blobs in
	Purpose:	Renders the tiny blobs for a group (A Single blob on the topmost child object
				in the group)

********************************************************************************************/

void NodeContourController::RenderTinyBlobs(RenderRegion* pRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// get the topmost object in this group and tell it to render its tiny blobs.
	Node* pNode = FindLastChild();
	while (pNode != NULL && !pNode->IsAnObject())
		pNode = pNode->FindPrevious();

	// empty groups are not allowed!
	if (pNode == NULL)
	{
		ERROR3("NodeGroup::RenderTinyBlobs; This group is empty! Shouldn't be!");
		return;
	}
	else
	{
		((NodeRenderableInk*)pNode)->RenderTinyBlobs(pRegion);
	}

#endif
}


//////////////////////////////////////////////////////////////////////////////////
// ContourNodeTreeFactory implementation
/***********************************************************************************************

>	ContourNodeTreeFactory::ContourNodeTreeFactory()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		
	Purpose:	Constructor

***********************************************************************************************/
ContourNodeTreeFactory::ContourNodeTreeFactory()
{
	m_Steps = 5;
	m_Width = 5000;
	m_pMap = NULL;
	m_bInsetPath = FALSE;
}

/***********************************************************************************************

>	ContourNodeTreeFactory::~ContourNodeTreeFactory()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		
	Purpose:	Destructor

***********************************************************************************************/
ContourNodeTreeFactory::~ContourNodeTreeFactory()
{
	if (m_pMap)
	{
		delete m_pMap;
		m_pMap = NULL;
	}
}

/***********************************************************************************************

>	NodeCompound * ContourNodeTreeFactory::CreateNode(List * pList, UndoableOperation * pOp = NULL);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		
	Purpose:	Creates the node tree given the parameters & the list of nodes to address

***********************************************************************************************/
NodeCompound * ContourNodeTreeFactory::CreateNode(List * pList, UndoableOperation * pOp)
{
	ERROR2IF(pList == NULL, NULL, "List is null");
	ERROR2IF(pList->IsEmpty(), NULL, "List is empty");

	String_256 StatusString;
	StatusString.Load(_R(IDS_CONTOUR_CREATE_STATUS_STRING));
	GetApplication()->UpdateStatusBarText(&StatusString, FALSE);
PORTNOTE("status","Removed UpdateStatusLineFont usage")
#ifndef EXCLUDE_FROM_XARALX
	GetApplication()->GetpCCStatusBar()->UpdateStatusLineFont();
#endif
	
	NodeContourController * pController = NULL;
	ALLOC_WITH_FAIL(pController, new NodeContourController, pOp);

	pController->SetNumberOfSteps(m_Steps);
	pController->SetWidth(m_Width);
	pController->SetInsetPathFlag(m_bInsetPath);

	NodeContour * pContour = NULL;

	NodeListItem * pItem = (NodeListItem *)pList->GetHead();

	CCAttrMap * pMap = m_pMap;

	if (!m_pMap)
	{
		if (!pItem->pNode->IS_KIND_OF(NodeBlend))
		{
			pMap = CCAttrMap::MakeAppliedAttrMap((NodeRenderableInk *)pItem->pNode);
		}
		else
		{
			pMap = CCAttrMap::MakeAppliedAttrMap((NodeRenderableInk *)
				(pItem->pNode->FindFirstChild(CC_RUNTIME_CLASS(NodeRenderableInk))));
		}

		if (!pMap)
		{
			ERROR3("can't create attribute map");
			return NULL;
		}
	}

	List AttrList;

	pMap->BuildListOfAttributes(&AttrList);
	
	// insert the controller into the tree before the first element in the list
	BOOL bOK = TRUE;

	// send a message indicating that the light angle has changed
	ObjChangeFlags flgs(FALSE, FALSE, FALSE, FALSE);
	flgs.RegenerateNode = TRUE;
	ObjChangeParam OP(OBJCHANGE_FINISHED, flgs, pItem->pNode, pOp, OBJCHANGE_CALLEDBYOP);

//	NodeAttribute * pAttrNode = NULL;

	Node * pNodeCopy = NULL;

	DocColour *pStartColour = NULL;

//	INT32 R = 0;
//	INT32 B = 0;
//	INT32 G = 0;

	INT32 H = 0;
	INT32 S = 0;
	INT32 V = 0;

	BOOL bFoundColourFill = FALSE;

//	BOOL bFirst = TRUE;
	if (pOp && pItem)
	{
		// can we do a contour to this node ?
		bFoundColourFill = FALSE;
		if (pItem->pNode->AllowOp(&OP, FALSE))
		{
			if (bOK)
			{
				bOK = pOp->DoInsertNewNode(pController, pItem->pNode, PREV, TRUE, FALSE, FALSE, TRUE);
			}

			while (pItem && bOK)
			{
				bOK = pOp->DoMoveNode(pItem->pNode, pController, LASTCHILD);

				if (pItem->pNode->IsAnObject() && bOK)
				{
					bOK = pOp->DoSelectNode((NodeRenderableInk *)pItem->pNode);
				}
				
				pItem = (NodeListItem *)pList->GetNext(pItem);
			}

			if (bOK)
			{
				ALLOC_WITH_FAIL(pContour, new NodeContour, pOp);
				
				if (!pContour)
				{
					ERROR3("cant' create contour node");
					return NULL;
				}
				
				// apply the attribute list
				pItem = (NodeListItem *)AttrList.GetHead();
				
				while (pItem && bOK)
				{
					// if (((NodeAttribute *)pItem->pNode)->CanBeAppliedToObject())
					// only apply these attributes
					NodeAttribute * pNodeAttr = (NodeAttribute *)pItem->pNode;
					if (pNodeAttr->IsAColourFill() || pNodeAttr->IsATranspFill() ||
						pNodeAttr->IsAStrokeColour() || pNodeAttr->IsAStrokeTransp() ||
						pNodeAttr->IS_KIND_OF(AttrLineWidth) ||
						pNodeAttr->IS_KIND_OF(AttrJoinType) ||
						pNodeAttr->IS_KIND_OF(AttrFillMapping) ||
						pNodeAttr->IS_KIND_OF(AttrTranspFillMapping))
					{
						pItem->pNode->NodeCopy(&pNodeCopy);

						if (pNodeCopy)
						{
							if (((NodeAttribute *)pNodeCopy)->IsAColourFill() && 
								!((NodeAttribute *)pNodeCopy)->IsATranspFill())
							{
								// invert the colour fill
								pStartColour = ((AttrFillGeometry *)pNodeCopy)->GetStartColour();

								if (pStartColour)
								{
									if (!pStartColour->IsTransparent())
									{
										bFoundColourFill = TRUE;

										pStartColour->GetHSVValue(&H, &S, &V);

										if (!m_bInsetPath)
										{
											V = (4 * (255 - V) / 5) + V;
											
											S = S / 5;
										}
										
										pStartColour->SetHSVValue(H,S,V);
									}
								}
							}
							
							pNodeCopy->AttachNode(pContour, FIRSTCHILD, FALSE, FALSE);
						}
						else
						{
							bOK = FALSE;
						}
					}
					
					
					pItem = (NodeListItem *)AttrList.GetNext(pItem);
				}
				
				AttrList.DeleteAll();
				
				// do the winding rule
				AttrWindingRule * pWinding = (AttrWindingRule *)pContour->FindFirstChild(
					CC_RUNTIME_CLASS(AttrWindingRule));
				
				if (pWinding)
				{
					pWinding->Value.WindingRule = NegativeWinding;
				}

				if (bOK)
				{
					// insert the node, factoring out all the common attributes
					// outer contours come before, inners after 
					if (pController->GetWidth() < 0)
					{
						Node * pInsertNode = pController->FindFirstChild(CC_RUNTIME_CLASS(NodeRenderableInk));

						if (pInsertNode)
							bOK = pOp->DoInsertNewNode(pContour, pInsertNode, PREV, TRUE, FALSE, FALSE, TRUE);
						else
							ERROR2(NULL, "No insertion node !");
					}
					else
					{
						bOK = pOp->DoInsertNewNode(pContour, pController, LASTCHILD, TRUE, FALSE, FALSE, TRUE);
					}

				}
			}

			if (bOK && !bFoundColourFill)
			{
				// does the controller node have a non-default fill ?
				// right, we must choose one then
				Node * pColourNode = pController->FindFirstDepthFirst();

				while (pColourNode && pColourNode != pController)
				{
					if (pColourNode->IsAnAttribute())
					{
						// found it !
						// clone it onto the contour node
						if (((NodeAttribute *)pColourNode)->IsAColourFill() && 
							!((NodeAttribute *)pColourNode)->IsATranspFill())
						{
							// invert the colour fill
							pColourNode->NodeCopy(&pNodeCopy);
							pStartColour = ((AttrFillGeometry *)pNodeCopy)->GetStartColour();
							
							if (pStartColour)
							{
								if (!pStartColour->IsTransparent())
								{
									bFoundColourFill = TRUE;
									pStartColour->GetHSVValue(&H, &S, &V);

									if (!m_bInsetPath)
									{									
										V = (4 * (255 - V) / 5) + V;
									
										S = S / 5;
									}
									
									pStartColour->SetHSVValue(H,S,V);
								}

								pNodeCopy->AttachNode(pContour, FIRSTCHILD, FALSE, FALSE);

								break;
							}
							else
							{
								delete pNodeCopy;
							}
						}
					}

					pColourNode = pColourNode->FindNextDepthFirst(pController);
				}
			}
			
			if (bOK)
			{
				// now, factor out all the children			
				bOK = pOp->DoFactorOutCommonChildAttributes(pController, TRUE);
			}

			if (bOK)
			{
				//	Karim 28/06/2000
				//	NodeContourController should never be selected.
//				pController->SetSelected(TRUE);

				// check the controller width on inner bevels so it can't be greater
				// than a certain amount
				if (pController->GetWidth() > 0)
				{
					MILLIPOINT MaxWidth = ContourNodePathProcessor::GetMaxInnerContourWidth(pController);
					if (pController->GetWidth() > MaxWidth)
					{
						pController->SetWidth(MaxWidth);
					}
				}

				pController->RegenerateNode(pOp, FALSE);
			}
			
			if (bOK)
			{
				bOK = pOp->DoInvalidateNodeRegion(pController, TRUE);
			}
		}
	}	

	AttrList.DeleteAll();

	if (m_bInsetPath)
	{
		// change the selection for inset path stuff
		pController->SetSelected(FALSE);
		pContour->SetSelected(TRUE);
	}

	GetApplication()->UpdateSelection();

  	if (pMap)
	{
		delete pMap;
		m_pMap = NULL;
	}

	if (bOK)
		return pController;

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////
// ContourNodePathProcessor implementation
/***********************************************************************************************

>	ContourNodePathProcessor ::ContourNodePathProcessor ()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/99
	Inputs:		
	Purpose:	Constructor

***********************************************************************************************/
ContourNodePathProcessor::ContourNodePathProcessor()
{
	TRACEUSER( "DavidM", _T("Created a contour node path processor\n"));	
	m_bActive = TRUE;
	m_bLineWidthTest = TRUE;
	m_bPrinting = FALSE;
	m_bRespectJoinType = FALSE;

}

/***********************************************************************************************

>	ContourNodePathProcessor ::~ContourNodePathProcessor ()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/99
	Inputs:		
	Purpose:	Destructor

***********************************************************************************************/
ContourNodePathProcessor::~ContourNodePathProcessor()
{
	TRACEUSER( "DavidM", _T("Killed a contour node path processor\n"));	
}

/***********************************************************************************************

>	void ContourNodePathProcessor::ProcessPath ( Path * pPath,
												 RenderRegion * pRegion,
												 PathShape ShapePath = PATHSHAPE_PATH )

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/99
	Inputs:		
	Purpose:	Sums all paths which are passed to it

***********************************************************************************************/

// Graeme ( 14/12/99 ) - Added the PathShape ShapePath field to the arguments. This is used by
// the Flash render region to determine whether or not the object is circular.
void ContourNodePathProcessor::ProcessPath ( Path * pPath,
											 RenderRegion * pRegion,
											 PathShape ShapePath )
{
//#pragma message( __LOCMSG__ "ContourNodeTreeFactory::ProcessPath - do nothing" )
//	TRACE( _T("Warning - ContourNodeTreeFactory::ProcessPath called\n") );
	UINT32 StrokeWidth = 0;
	
	if (m_bActive && pPath->IsClosed())
	{
		// get the pixel width
		// stroke the original path
		MILLIPOINT PixelWidth = pRegion->GetScaledPixelWidth();

		if (m_bPrinting)
		{
			PixelWidth = pRegion->GetPixelWidth();
		}

		StrokeWidth = PixelWidth * 2;

		BOOL bStrokePath = FALSE;
		
		// if we have a transparent line width then we need to stroke the path slightly
		// to kill off holes
		if (m_bLineWidthTest)
		{
			LineWidthAttribute * pLineWidth = (LineWidthAttribute *)pRegion->GetCurrentAttribute(ATTR_LINEWIDTH);

			if (pLineWidth)
			{
				if (pLineWidth->LineWidth < PixelWidth)
				{
					StrokeWidth = PixelWidth;
					bStrokePath = TRUE;
				}
			}
			
			StrokeColourAttribute * pLineColour = 
				(StrokeColourAttribute *)pRegion->GetCurrentAttribute(ATTR_STROKECOLOUR);
			
			if (pLineColour)
			{
				DocColour * pDocColourLine = pLineColour->GetStartColour();
				
				if (pDocColourLine)
				{
					if (pDocColourLine->IsTransparent())
					{
						bStrokePath = TRUE;
					}
				}
			}
		}
		else
		{
			bStrokePath = TRUE;
		}

		// stroke the original path
		if (bStrokePath)
		{		
			pRegion->SaveContext();
			
			Path StrokedPath;
			StrokedPath.Initialise();
			
			pPath->StrokePathToPath(StrokeWidth,
				LineCapRound,
				RoundJoin,
				NULL,
				&StrokedPath,
				200,
				FALSE);
			
			StrokedPath.TryToClose();
			StrokedPath.IsFilled = TRUE;
			StrokedPath.IsStroked = TRUE;

			pRegion->SetLineColour(COLOUR_NONE);

			pRegion->DrawPath(&StrokedPath, this, ShapePath);
			pRegion->RestoreContext();
		}
	}
	
	pRegion->DrawPath(pPath, this, ShapePath);
}

//////////////////////////////////////////////////////////////////////
// class to handle loading in of bevel records

/********************************************************************************************
>	ContourRecordHandler::ContourRecordHandler()

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Inputs:		-
	Returns:	
	Purpose:	Destructor
********************************************************************************************/
ContourRecordHandler::ContourRecordHandler()
{

}


/********************************************************************************************
>	ContourRecordHandler::~ContourRecordHandler()

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Inputs:		-
	Returns:	
	Purpose:	Destructor
********************************************************************************************/
ContourRecordHandler::~ContourRecordHandler()
{
}

/********************************************************************************************
>	UINT32*	ContourRecordHandler::GetTagList()

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Inputs:		-
	Returns:	
	Purpose:	Gets the tag list for this handler to handle 
********************************************************************************************/
UINT32*	ContourRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {	TAG_CONTOURCONTROLLER,
								TAG_CONTOUR,
								CXFRH_TAG_LIST_END};

	return (UINT32*)&TagList;
}

/********************************************************************************************
>	BOOL ContourRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Inputs:		-
	Returns:	
	Purpose:	Handles the bevel record - basically creates a BevelController node &
				creates all the attributes
********************************************************************************************/
BOOL ContourRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	INT32 Steps = 0;
	INT32 Width = 0;
	BYTE Type = 0;

	BOOL ok = TRUE;

	double ObjBias = 0;
	double ObjGain = 0;
	double AttrBias = 0;
	double AttrGain = 0;
	
	if (pCXaraFileRecord->GetTag() == TAG_CONTOURCONTROLLER)
	{
		ok = pCXaraFileRecord->ReadINT32(&Steps);

		if (ok) ok = pCXaraFileRecord->ReadINT32(&Width);
		if (ok)	ok = pCXaraFileRecord->ReadBYTE(&Type);
		if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&ObjBias);
		if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&ObjGain);
		if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&AttrBias);
		if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&AttrGain);

		NodeContourController * pNode = new NodeContourController;

		if (!pNode)
		{
			ERROR3("Can't create contour controller");
			return FALSE;
		}

		pNode->SetWidth(Width);
		pNode->SetNumberOfSteps(Steps);

		if ((Type & 128) != 0)
		{
			Type = Type - 128;
			pNode->SetInsetPathFlag(TRUE);
		}

		pNode->SetColourBlendType((ColourBlendType)Type);

		pNode->SetObjectProfile( CProfileBiasGain( (AFp)ObjBias, (AFp)ObjGain ) );
		pNode->SetAttrProfile( CProfileBiasGain( (AFp)AttrBias, (AFp)AttrGain ) );

		if (ok)
		{
			InsertNode(pNode);
		}
	}
	else if (pCXaraFileRecord->GetTag() == TAG_CONTOUR)
	{
		NodeContour * pNode = new NodeContour;

		if (!pNode)
		{
			ERROR3("Can't create NodeContour");
			return FALSE;
		}

		InsertNode(pNode);
	}
	else
	{
		ERROR3("Contour file record handler - unrecognised tag");
		ok = FALSE;
	}

	return ok;
}

/***********************************************************************************************

>	MILLIPOINT ContourNodePathProcessor::GetMaxInnerContourWidth(NodeCompound * pCompound);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/99
	Inputs:		See base class
	Purpose:	Calculates the maximum inner bevel allowed, given the node

***********************************************************************************************/
MILLIPOINT ContourNodePathProcessor::GetMaxInnerContourWidth(NodeCompound * pCompound)
{
	Node * pNode = pCompound->FindFirstDepthFirst();

	DocRect dr;

	MILLIPOINT MaxInnerBevel = -1;
	MILLIPOINT Len = 0;

	while (pNode && pNode != pCompound)
	{
		// ignore all compound nodes, non-object nodes and needs parent nodes
		if (pNode->IsAnObject() && !pNode->NeedsParent(NULL) && !pNode->IsCompound())
		{
			dr = ((NodeRenderableInk *)pNode)->GetBoundingRect(FALSE, FALSE);

			// get the maximum out of these two
			if (dr.Width() > dr.Height())
			{
				Len = dr.Width();
			}
			else
			{
				Len = dr.Height();
			}

			Len /= 2;

			if (MaxInnerBevel < 0)
			{
				MaxInnerBevel = Len;
			}
			else if (MaxInnerBevel < Len)
			{
				MaxInnerBevel = Len;
			}
		}

		pNode = pNode->FindNextDepthFirst(pCompound);
	}

	return MaxInnerBevel;
}

/////////////////////////////////////////////////////////////////////////
// The InsetPathPathProcessor

/***********************************************************************************************

>	InsetPathPathProcessor::InsetPathPathProcessor()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/2000
	Inputs:		See base class
	Purpose:	Constructor

***********************************************************************************************/
InsetPathPathProcessor::InsetPathPathProcessor()
{
	m_bActive = TRUE;
}

/***********************************************************************************************

>	InsetPathPathProcessor::~InsetPathPathProcessor()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/2000
	Inputs:		See base class
	Purpose:	Destructor

***********************************************************************************************/
InsetPathPathProcessor::~InsetPathPathProcessor()
{
	
}

/***********************************************************************************************

>	void InsetPathPathProcessor::ProcessPath(Path *pPath,
							 RenderRegion *pRender,
							 PathShape ShapePath)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/2000
	Inputs:		See base class
	Purpose:	Destructor

***********************************************************************************************/
void InsetPathPathProcessor::ProcessPath(Path *pPath,
							 RenderRegion *pRender,
							 PathShape ShapePath)
{
//#pragma message( __LOCMSG__ "ContourNodeTreeFactory::ProcessPath - do nothing" )
//	TRACE( _T("Warning - ContourNodeTreeFactory::ProcessPath called\n") );
	if (m_bActive)
		return;

	pRender->DrawPath( pPath, this );
}
