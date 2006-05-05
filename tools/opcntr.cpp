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

// Implementation of the Contour operations

#include "camtypes.h"
#include "selop.h"
#include "opcntr.h"
//#include "cntres.h"
#include "node.h"
#include "ink.h"
#include "nodecomp.h"
#include "group.h"
#include "nodecntr.h"
#include "ncntrcnt.h"
#include "progress.h"
#include "view.h"
#include "docview.h"
#include "document.h"
#include "lineattr.h"
#include "fillattr.h"
#include "fillval.h"
#include "attrval.h"
#include "nodepath.h"
#include "attrmap.h"
#include "gclips.h"
#include "trans2d.h"
#include "opbevel.h"
#include "nodecont.h"
#include "objchge.h"
#include "blndtool.h"
#include "app.h"
#include "nodebldr.h"
#include "ppbevel.h"	//	for bevel helpers
#include "fitcurve.h"
#include "rsmooth.h"
#include "nodeblnd.h"
#include "brshattr.h"
#include "strkattr.h"
#include "pbecomea.h"
#include "slicehelper.h"
//#include "simon.h"			// for "no objects selected" Description string
#include "ppstroke.h"
#include "brshbeca.h"
#include "ophist.h"

CC_IMPLEMENT_DYNCREATE(CreateContourParam, OpParam)
CC_IMPLEMENT_DYNCREATE(OpCreateContour, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpChangeContourWidth, UndoableOperation)
CC_IMPLEMENT_DYNCREATE(OpRemoveContour, SelOperation)
CC_IMPLEMENT_DYNCREATE(RegenerateContourAction, Action)
CC_IMPLEMENT_DYNCREATE(ChangeContourWidthAction, Action)
CC_IMPLEMENT_DYNCREATE(ChangeContourToOuterAction, Action)
CC_IMPLEMENT_DYNCREATE(ChangeContourToInnerAction, Action)
CC_IMPLEMENT_DYNCREATE(OpChangeContourToInner, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpChangeContourToOuter, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpChangeContourSteps, SelOperation)
CC_IMPLEMENT_DYNCREATE(ChangeContourStepsAction, Action)
CC_IMPLEMENT_DYNCREATE(OpChangeContourColourType, SelOperation)
CC_IMPLEMENT_DYNCREATE(ChangeContourColourTypeAction, Action)
CC_IMPLEMENT_DYNCREATE(OpChangeContourAttributeProfile, SelOperation)
CC_IMPLEMENT_DYNCREATE(ChangeContourAttributeProfileAction, Action)
CC_IMPLEMENT_DYNCREATE(OpChangeContourObjectProfile, SelOperation)
CC_IMPLEMENT_DYNCREATE(ChangeContourObjectProfileAction, Action)
CC_IMPLEMENT_DYNCREATE(OpChangeContourStepDistance, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpConvertPathToShapes, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpToggleContourInsetPath, SelOperation)
CC_IMPLEMENT_DYNCREATE(ToggleInsetPathAction, Action)

DECLARE_SOURCE( "$Revision$" );

#define new CAM_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Contour ops implementation

/********************************************************************************************
>	OpCreateContour::OpCreateContour()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Purpose:	Constructor.
********************************************************************************************/
OpCreateContour::OpCreateContour()
{
	//TRACEUSER( "MarkH", _T("Just Made a OpCreateContour Class!\n"));
}



/********************************************************************************************
>	OpCreateContour::~OpCreateContour()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Purpose:	Destructor.
********************************************************************************************/
OpCreateContour::~OpCreateContour()
{
	//TRACEUSER( "MarkH", _T("Just Killed OpCreateContour Class!!!\n"));
}



/********************************************************************************************
>	virtual void OpCreateContour::Do(OpDescriptor *pOpDesc)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		pOpDesc - OpDescriptor - unused
	Outputs:	-
	Returns:	-
	Purpose:	The do function.  Applys a Bevel to the selection in the current document
********************************************************************************************/
void OpCreateContour::Do(OpDescriptor *pOpDesc)
{
	End();
}


/********************************************************************************************
>	void OpCreateContour::DoWithParam(OpDescriptor* pOp, OpParam* pParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Applys a Bevel to the selected node after a mouse click
********************************************************************************************/

void OpCreateContour::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	DoStartSelOp(TRUE, TRUE);
	ObjChangeFlags cFlags;
	cFlags.RegenerateNode = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

	String_64 StatusString;
	StatusString.Load(_R(IDS_CONTOUR_CREATE_STATUS_STRING));

	BeginSlowJob(-1, TRUE, &StatusString);

	String_256 s(StatusString);
	GetApplication()->UpdateStatusBarText(&s, FALSE);

	CreateContourParam * pCCP = (CreateContourParam *)pParam;

	List * pNodeList = pCCP->m_pNodeList;

	// first, remove all contours from the list
	List ContourList;

	NodeListItem * pItem = (NodeListItem *)pNodeList->GetHead();
	NodeListItem * pContourItem = NULL;

	BOOL ok = TRUE;

	NodeHidden * pHidden = NULL;

	while (pItem)
	{
		BevelTools::GetAllNodesUnderNode(pItem->pNode, &ContourList, CC_RUNTIME_CLASS(NodeContourController));
		
		if (!ContourList.IsEmpty())
		{
			TRACEUSER( "DavidM", _T("Found non-empty contour list\n"));
			pContourItem = (NodeListItem *)ContourList.GetHead();
			
			while (pContourItem)
			{
				// remove the contour node, and promote all children
				// invalidate the node
				if (ok)
					ok = DoInvalidateNodeRegion((NodeRenderableInk *)pContourItem->pNode, TRUE);

				// factor out the child attributes
				if (ok)
					ok = DoLocaliseCommonAttributes((NodeRenderableInk *)pContourItem->pNode, TRUE);
				
				// now, move all child nodes upwards (except for the 'needs parent' nodes)
				Node * pChild = pContourItem->pNode->FindFirstChild();
				Node * pNextChild = NULL;
				
				while (pChild)
				{
					pNextChild = pChild->FindNext();
					if (ok && !pChild->NeedsParent(pContourItem->pNode) && !pChild->IsAnAttribute())
						ok = DoMoveNode(pChild, pContourItem->pNode, PREV);
					
					pChild = pNextChild;
				}
				
				
				// hide the original node
				if (ok)
					ok = DoHideNode(pContourItem->pNode, TRUE, &pHidden, FALSE);
				
				// move on
				pContourItem = (NodeListItem *)ContourList.GetNext(pContourItem);
			}
			
			ContourList.DeleteAll();

			if (ok)
				ok = DoFactorOutCommonChildAttributes((NodeRenderableInk *)pItem->pNode);
		}
		
		pItem = (NodeListItem *)pNodeList->GetNext(pItem);
	}

	pItem = (NodeListItem *)pNodeList->GetHead();

//	CCAttrMap * pAttrMap = NULL;
	AttrLineWidth * pLineWidthAttr = NULL;
	AttrStrokeColour * pStrokeColourAttr = NULL;
//	AttrStartArrow * pAttrStartArrow = NULL;
//	AttrEndArrow * pAttrEndArrow = NULL;
//	FlatFillAttribute * pFillAttr = NULL;
//	StrokeColourAttribute * pStrokeAttr = NULL;
//	NodeAttribute * pAttr = NULL;

	DocColour StrokeColour;
	MILLIPOINT LineWidth = 0;
//	INT32 Red = 0;
//	INT32 Blue = 0;
//	INT32 Green = 0;

	Path ArrowPath;
	ArrowPath.Initialise();

	while (pItem && ok)
	{
		if (pItem->pNode->IsNodePath() && pItem->pNode->AllowOp(&ObjChange))
		{
			// for all closed paths in the selection, copy their stroke attribute
			// colour as a fill attribute
			NodePath * pPathNode = (NodePath *)pItem->pNode;

			// find out if the path is closed
			if (!pPathNode->InkPath.IsSubPathClosed(0))
			{
				// first, localise all the common attributes
				if (ok)
					ok = DoLocaliseForAttrChange(pPathNode, (AttrTypeSet *)NULL, (ObjectSet *)NULL);
				
				// get the line width & the stroke colour out of the node
				pLineWidthAttr = (AttrLineWidth *)pPathNode->FindFirstChild(CC_RUNTIME_CLASS(AttrLineWidth));
				pStrokeColourAttr = (AttrStrokeColour *)pPathNode->FindFirstChild(CC_RUNTIME_CLASS(AttrStrokeColour));

				if (pLineWidthAttr)
					LineWidth = pLineWidthAttr->Value.LineWidth;

				if (pStrokeColourAttr)
					StrokeColour = pStrokeColourAttr->Value.Colour;

				// eliminate all fills from this node
				if (ok)
					ok = DoRemoveAttrTypeFromSubtree(pPathNode, CC_RUNTIME_CLASS(AttrFillGeometry));

				// create a new flat fill attribute & apply it to this node
				AttrFlatColourFill * pFlatFill = NULL;
				ALLOC_WITH_FAIL(pFlatFill, new AttrFlatColourFill, this);

				pFlatFill->SetStartColour(&StrokeColour);

				if (ok)
				{
					// [Phil, 17-09-2005]
					// NO!
					// Geez, DMc, you can't cast an AttrFlatColourFill* to a NodeRenderableBounded*!
					// That means that you can't call DoInsertNewNode, so we have to do the same thing
					// in essence directly...
					//					ok = DoInsertNewNode((NodeRenderableBounded *)pFlatFill, pPathNode,
					//						FIRSTCHILD,	FALSE, FALSE, FALSE, FALSE);
					pFlatFill->AttachNode(pPathNode, FIRSTCHILD);
					HideNodeAction* UndoHideNodeAction;
					if (HideNodeAction::Init(this, this->GetUndoActions(), pFlatFill, TRUE, (Action**)(&UndoHideNodeAction))
					  		== AC_FAIL)
					{
						pFlatFill->CascadeDelete();
						delete pFlatFill;
						pFlatFill = FALSE;
						FailAndExecute();
						End();
						return;
					}
				}

				// factor out all the attributes
				if (ok)
					ok = DoFactorOutAfterAttrChange(pPathNode, (AttrTypeSet *)NULL);
			}
		}

		pItem = (NodeListItem *)pNodeList->GetNext(pItem);
	}

	if (!ok)
	{
		FailAndExecute();
		End();
		return;
	}

	// have we any shadow nodes present ?
	// if so, build a new list
	pItem = (NodeListItem *)pNodeList->GetHead();
	NodeListItem * pNewItem = NULL;
	Node * pNode = NULL;
	Node * pShadowNode = NULL;

	List ListToContour;

	while (pItem)
	{
		// find out if any parents are shadow controllers
		pNode = pItem->pNode;
		pShadowNode = NULL;
		
		while (pNode)
		{
			if (pNode->IS_KIND_OF(NodeShadowController))
				pShadowNode = pNode; // we've found a shadow controller node

			pNode = pNode->FindParent();
		}

		// do we have a shadow node ? if so, make all non-needs parent children
		// be contoured
		if (pShadowNode)
		{
			// first, localise all attributes
			DoLocaliseCommonAttributes((NodeShadowController *)pShadowNode, TRUE);			

			// add children to the contour list
			pNode = pShadowNode->FindFirstChild();
			
			while (pNode)
			{
				if (!pNode->NeedsParent(pShadowNode))
				{
					if (pNode->IsAnObject())
					{
						ALLOC_WITH_FAIL(pNewItem, new NodeListItem(pNode), this);
						ListToContour.AddTail(pNewItem);
					}
				}

				pNode = pNode->FindNext();
			}
		}
		else
		{
			ALLOC_WITH_FAIL(pNewItem, new NodeListItem(pItem->pNode), this);
			ListToContour.AddTail(pNewItem);
		}

		pItem = (NodeListItem *)pNodeList->GetNext(pItem);
	}
	
	// create the tree factory
	ContourNodeTreeFactory * pFactory = NULL;
	ALLOC_WITH_FAIL(pFactory, new ContourNodeTreeFactory, this);

	pFactory->SetNumberOfSteps(pCCP->m_Steps);
	pFactory->SetWidth(pCCP->m_Width);
	pFactory->SetInsetPathFlag(pCCP->m_bInsetPath);

	List SingleNodeList;

	// handle each node individually
	pItem = (NodeListItem *)ListToContour.GetHead();

	NodeBlender * pBlender = NULL;

	while (pItem)
	{
		ALLOC_WITH_FAIL(pNewItem, new NodeListItem(pItem->pNode), this);

		SingleNodeList.AddTail(pNewItem);
		
		NodeContourController * pContourNode = (NodeContourController *)pFactory->CreateNode(&SingleNodeList, this);

		SingleNodeList.DeleteAll();
		
		// do we have a parent shadow node ? if so, regenerate 
		if (!pContourNode)
		{
			FailAndExecute();
			End();
			ListToContour.DeleteAll();
			
			return;
		}

		// check for a blend needing to be re-initialised
		pBlender = (NodeBlender *)pContourNode->FindNext(CC_RUNTIME_CLASS(NodeBlender));

		if (pBlender)
		{
			if (InitBlendAction::InitOnBlender(this, GetUndoActionList(), pBlender, TRUE, TRUE) != AC_OK)
				ERROR2RAW("Couldn't Initialise blend action");

			pBlender->Reinit(pContourNode, 	pBlender->GetNodeEnd(), FALSE);
		}
		else
		{
			pBlender = (NodeBlender *)pContourNode->FindPrevious(CC_RUNTIME_CLASS(NodeBlender));

			if (pBlender)
			{
				if (InitBlendAction::InitOnBlender(this, GetUndoActionList(), pBlender, TRUE, TRUE) != AC_OK)
					ERROR2RAW("Couldn't Initialise blend action");
					
				pBlender->Reinit(pBlender->GetNodeStart(), 	pContourNode, FALSE);
			}
		}

		if (pBlender)
		{
			NodeBlend* ptrBlend = (NodeBlend*) pBlender->FindParent ();

			ERROR3IF (!IS_A (ptrBlend, NodeBlend), "NodeBlend is not a NodeBlend!");

			BOOL done = FALSE;
			NodeBlender* ptrNode = ptrBlend->FindFirstBlender ();

			while (!done)
			{
				if (ptrNode != pBlender)
				{
					if (ptrNode->GetNodeStart () == pItem->pNode)
					{
						if (InitBlendAction::InitOnBlender(this, GetUndoActionList(), ptrNode, TRUE, TRUE) != AC_OK)
							ERROR2RAW("Couldn't Initialise blend action");

						if (pContourNode)
							ptrNode->Reinit(pContourNode, NULL, FALSE);
					}
					if (ptrNode->GetNodeEnd () == pItem->pNode)
					{
						if (InitBlendAction::InitOnBlender(this, GetUndoActionList(),  ptrNode, TRUE, TRUE) != AC_OK)
							ERROR2RAW("Couldn't Initialise blend action");

						if (pContourNode)
							ptrNode->Reinit(NULL, pContourNode, FALSE);
					}
				}

				ptrNode = ptrBlend->FindNextBlender (ptrNode);

				if (!ptrNode)
					done = TRUE;
			}
		}
		
		pNode = pContourNode->FindParent();
		
		while (pNode)
		{
			pNode->AllowOp(&ObjChange);
			pNode = pNode->FindParent();
		}

		// deselect all nodes under the contour node, and select all original nodes
		if (!pCCP->m_bInsetPath)
		{
			pNode = pContourNode->FindFirstChild();
			pContourNode->SetSelected(FALSE);
			
			while (pNode)
			{
				if (pNode->IsAnObject() && !pNode->NeedsParent(NULL))
					pNode->SetSelected(TRUE);
				
				pNode = pNode->FindNext();
			}
		}
		else
		{
			NodeContour* pContour = pContourNode->GetContourNode();
			ERROR3IF(!pContour,"Failed to get the contour node from the controller node!");
//////////
			NodeRenderableInk* pChild = pContourNode->FindFirstChildInk();
			CCAttrMap AttrMap;
			
			if(pChild)
				((NodeRenderableInk *)pChild)->FindAppliedAttributes(&AttrMap);

			// Create and initialize a new NodePath to hold the inset path!
			NodePath* pNewPath;
			ALLOC_WITH_FAIL(pNewPath, new NodePath, this);
			pNewPath->InkPath.Initialise();

			DoInvalidateNodeRegion(pContourNode,FALSE);

			// Create the DoBecomeA Function and call it on the controller
			BecomeA BecomeAReplace(BECOMEA_REPLACE, CC_RUNTIME_CLASS(NodePath), this, FALSE);
			pContour->DoBecomeA(&BecomeAReplace);

			// Now get the relavent path from the group of path nodes
			NodePath* pPathToCopy = NULL;

			if(pContourNode->IsContourAnOuterContour())
				pPathToCopy = (NodePath*)pContourNode->FindFirstChild(CC_RUNTIME_CLASS(NodePath));
			else
				pPathToCopy = (NodePath*)pContourNode->FindLastChild(CC_RUNTIME_CLASS(NodePath));

			pNewPath->InkPath.CloneFrom(pPathToCopy->InkPath);

			// now Initialize the flag array with the new Path and insert it after the group node!
			pNewPath->InkPath.InitialiseFlags(0, pNewPath->InkPath.GetNumCoords());
			DoInsertNewNode(pNewPath, pContourNode, NEXT, TRUE, FALSE, TRUE, TRUE);

			pNewPath->InkPath.IsFilled = TRUE;

			// Make sure we have the correct attributes applied to our new path and make it selected!
			AttrMap.ApplyAttributesToNode(pNewPath);
			pNewPath->NormaliseAttributes();
			
			pNewPath->SetSelected(TRUE);
			
			DoHideNode(pContourNode, TRUE, &pHidden, TRUE);
//////////
/*
			pContourNode->SetSelected(FALSE);
			pContour->SetSelected(TRUE);

			// do a do become a for the contour node
			BecomeA BecomeAReplace(BECOMEA_REPLACE, CC_RUNTIME_CLASS(NodePath), this, FALSE);
			pContourNode->DoBecomeA(&BecomeAReplace);
*/
		}

		SliceHelper::AddNamesToController(this,pContourNode);

		pItem = (NodeListItem *)ListToContour.GetNext(pItem);
	}
		
	ListToContour.DeleteAll();

	delete pFactory;
	pFactory = NULL;

	ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
	UpdateChangedNodes(&ObjChange, Document::GetSelectedSpread());

	GetApplication()->UpdateSelection();
	
	End();
}

/********************************************************************************************

>	BOOL OpCreateContour::Declare()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94																		
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/
BOOL OpCreateContour::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_CREATECONTOUROP),
								CC_RUNTIME_CLASS(OpCreateContour), 
								OPTOKEN_CREATECONTOUR,
								OpCreateContour::GetState));

}



/********************************************************************************************

>	OpState OpCreateContour::GetState(String_256* Description, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Outputs:	-
	Returns:	Ungreyed, Unticked
	Purpose:	Find out the state of the new regular shape at the specific time

********************************************************************************************/
OpState OpCreateContour::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;

	if (Description)
		Description->Load(_R(IDS_CONTOUR_CREATE_STATUS_STRING));
	
	return Blobby;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//////////	OpRemoveContour class
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Bevel ops implementation

/********************************************************************************************
>	OpRemoveContour::OpRemoveContour()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Purpose:	Constructor.
********************************************************************************************/
OpRemoveContour::OpRemoveContour()
{
	//TRACEUSER( "MarkH", _T("Just Made a OpRemoveContour Class!\n"));
}



/********************************************************************************************
>	OpRemoveContour::~OpRemoveContour()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Purpose:	Destructor.
********************************************************************************************/
OpRemoveContour::~OpRemoveContour()
{
	//TRACEUSER( "MarkH", _T("Just Killed OpRemoveContour Class!!!\n"));
}



/********************************************************************************************
>	virtual void OpRemoveContour::Do(OpDescriptor *pOpDesc)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		pOpDesc - OpDescriptor - unused
	Outputs:	-
	Returns:	-
	Purpose:	The do function.  Applys a Bevel to the selection in the current document
********************************************************************************************/
void OpRemoveContour::Do(OpDescriptor *pOpDesc)
{
	// CGS:  (13/7/2000)  I have ammended this function to take into account multiple NodeBlends
	// since the contour may be part of more than one blend!
	
	// get all the contour nodes
	DoStartSelOp(TRUE, TRUE);
	ObjChangeFlags cFlags;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

	String_64 s(_T("Removing contour - please wait"));
	BeginSlowJob(-1, TRUE, &s);

	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(NodeContourController), TRUE);

	//List BlendList;
	//BevelTools::BuildListOfSelectedNodes(&BlendList, CC_RUNTIME_CLASS(NodeBlender), TRUE);

	// run through these nodes, eliminating them
	NodeListItem * pItem = (NodeListItem *)NodeList.GetHead();

	Node * pChildNode = NULL;
	Node * pNextNode = NULL;
	NodeHidden * pHidden = NULL;

	Node * pParent = NULL;

	BOOL ok = TRUE;

	NodeContourController * pControl = NULL;

	NodeBlender * pBlender = NULL;
	BOOL bBlendBefore = FALSE;
	BOOL bBlendAfter = FALSE;

	// for blends
	NodeRenderableInk *pRenderableNode = NULL;

	while (pItem)
	{
		pControl = (NodeContourController *)pItem->pNode;

		// try to find a blender node, as if there's one present we
		// need to reinitialise it
		pBlender = (NodeBlender *)pControl->FindNext(CC_RUNTIME_CLASS(NodeBlender));

		bBlendBefore = FALSE;
		bBlendAfter = FALSE;
		if (pBlender)
		{
			bBlendBefore = TRUE;
		}
		else
		{
			pBlender = (NodeBlender *)pControl->FindPrevious(CC_RUNTIME_CLASS(NodeBlender));

			if (pBlender)
			{
				bBlendAfter = TRUE;
			}
		}
		
		// get the parent for child change messaging
		pParent = pControl->FindParent();

		// invalidate the region
		if (ok)
			ok = DoInvalidateNodeRegion(pControl, TRUE);
		
		// factor out the common child attributes
		if (ok)
			ok = DoLocaliseCommonAttributes(pControl, TRUE);

		if (ok)
			SliceHelper::RemoveNamesFromController(this,pControl);

		pChildNode = pControl->FindFirstChild();

		// move all nodes to being siblings of the controller node, except for the
		// contour node itself
		while (pChildNode)
		{
			pNextNode = pChildNode->FindNext();

			if (pChildNode->IsAnObject() && !pChildNode->IS_KIND_OF(NodeContour))
			{
				if (ok)
				{
					ok = DoMoveNode(pChildNode, pControl, PREV);
				}

				if (ok)
					ok = DoSelectNode((NodeRenderableInk *)pChildNode);

				if (ok)
					ok = DoFactorOutAfterAttrChange((NodeRenderableInk *)pChildNode, 
													(AttrTypeSet *)NULL);
			}

			pChildNode = pNextNode;
		}

		pParent = pControl->FindParent();

		// agh why do i have to mess with a list just to add this action into the undo list?
		List ContList;
		NodeListItem * pActionItem = NULL;
		RegenerateContourAction* pRegenAction = NULL;
		ALLOC_WITH_FAIL(pActionItem, new NodeListItem, this);

		pActionItem->pNode = pControl;
		ContList.AddTail(pActionItem);

		RegenerateContourAction::Init(this, GetUndoActionList(),&ContList,&pRegenAction, TRUE);

		ContList.DeleteAll();

		// now, hide the controller node
		if (ok)
			ok = DoHideNode(pControl, TRUE, &pHidden, FALSE);

		// reinit the blends (if necessary)
		if (ok)
		{
			if (bBlendBefore)
			{
				if (InitBlendAction::InitOnBlender(this, GetUndoActionList(), pBlender, TRUE, TRUE) != AC_OK)
				{
					ERROR2RAW("Couldn't Initialise blend action");
				}

				// reinit the node
				pRenderableNode = (NodeRenderableInk *)pBlender->FindPrevious(CC_RUNTIME_CLASS(NodeRenderableInk)); 
					
				if (pRenderableNode)
				{
					pBlender->Reinit(pRenderableNode, 	pBlender->GetNodeEnd(), FALSE);
				}
			}
			else if (bBlendAfter)
			{
				if (InitBlendAction::InitOnBlender(this, GetUndoActionList(), pBlender, TRUE, TRUE) != AC_OK)
				{
					ERROR2RAW("Couldn't Initialise blend action");
				}

				// reinit the node
				pRenderableNode = (NodeRenderableInk *)pBlender->FindNext(CC_RUNTIME_CLASS(NodeRenderableInk)); 
					
				if (pRenderableNode)
				{
					pBlender->Reinit(pBlender->GetNodeStart(), pRenderableNode, FALSE);
				}
			}

			if (pBlender)
			{
				NodeBlend* ptrBlend = (NodeBlend*) pBlender->FindParent ();

				ERROR3IF (!IS_A (ptrBlend, NodeBlend), "NodeBlend is not a NodeBlend!");

				BOOL done = FALSE;
				NodeBlender* ptrNode = ptrBlend->FindFirstBlender ();

				while (!done)
				{
					if (ptrNode != pBlender)
					{
						if (ptrNode->GetNodeStart () == pControl)
						{
							if (InitBlendAction::InitOnBlender(this, GetUndoActionList(), ptrNode, TRUE, TRUE) != AC_OK)
							{
								ERROR2RAW("Couldn't Initialise blend action");
							}
							
							if (pRenderableNode)
							{
								ptrNode->Reinit(pRenderableNode, NULL, FALSE);
							}
						}
						if (ptrNode->GetNodeEnd () == pControl)
						{
							if (InitBlendAction::InitOnBlender(this, GetUndoActionList(), ptrNode, TRUE, TRUE) != AC_OK)
							{
								ERROR2RAW("Couldn't Initialise blend action");
							}

							if (pRenderableNode)
							{
								ptrNode->Reinit(NULL, pRenderableNode, FALSE);
							}
						}
					}

					ptrNode = ptrBlend->FindNextBlender (ptrNode);

					if (!ptrNode)
					{
						done = TRUE;
					}
				}
			}
		}
			
		// Karim 19/07/2000
		// the parent's bounds will have changed, so tell them so.
		if (pParent->IsBounded())		// paranoia code - this is always true.
			((NodeRenderableBounded*)pParent)->InvalidateBoundingRect();

		// update all the changed nodes on the parent of the contour
//		ObjChangeFlags flgs(FALSE, FALSE, FALSE, TRUE);
//		flgs.RegenerateNode = TRUE;
//		ObjChangeParam OP(OBJCHANGE_FINISHED, flgs, NULL, this, OBJCHANGE_CALLEDBYOP);

		while (pParent)
		{
			pParent->AllowOp(&ObjChange);
//			pParent->OnChildChange(&OP);

			pParent = pParent->FindParent();
		}

		// go onto the next item
		pItem = (NodeListItem *)NodeList.GetNext(pItem);
		
	}

	NodeList.DeleteAll();

	if (!ok)
	{
		FailAndExecute();
	}

	ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
	UpdateChangedNodes(&ObjChange, Document::GetSelectedSpread());

	GetApplication()->UpdateSelection();

	End();
}


/********************************************************************************************
>	void OpRemoveContour::DoWithParam(OpDescriptor* pOp, OpParam* pParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Applys a Bevel to the selected node after a mouse click
********************************************************************************************/

void OpRemoveContour::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	End();
}

/********************************************************************************************

>	BOOL OpRemoveContour::Declare()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94																		
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/
BOOL OpRemoveContour::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_REMOVECONTOUROP),
								CC_RUNTIME_CLASS(OpRemoveContour), 
								OPTOKEN_REMOVECONTOUR,
								OpRemoveContour::GetState,
								0,
								_R(IDBBL_REMOVECONTOUR),
								0));

}



/********************************************************************************************

>	OpState OpRemoveContour::GetState(String_256* Description, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Outputs:	-
	Returns:	Ungreyed, Unticked
	Purpose:	Find out the state of the new regular shape at the specific time

********************************************************************************************/
OpState OpRemoveContour::GetState(String_256* Description, OpDescriptor*)
{
	OpState State(FALSE,TRUE); // It's not ticked, but it is greyed by default
	
	// Get an ObjChangeParam ready so we can tell the selected blend's parents what we hope to do 
	// to the blend (i.e. replace it with other nodes).
	ObjChangeFlags cFlags;
	cFlags.MultiReplaceNode = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);

//	BOOL Denied = FALSE;
	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(NodeContourController), TRUE);

	if (NodeList.IsEmpty())
	{
		State.Greyed = TRUE;
	}
	else
	{
		State.Greyed = FALSE;
	}

	NodeList.DeleteAll();

	UINT32 IDS = 0;
	

	if (IDS == 0)
		IDS = _R(IDS_REMOVECONTOUR);

	*Description = String_256(IDS);

	return State;
}

////////////////////////////////////////////////////////////////////
// RegenerateContourAction class implementation
/********************************************************************************************

>	RegenerateContourAction::RegenerateContourAction()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

RegenerateContourAction::RegenerateContourAction()
{
	m_pNodes = NULL;
	m_bCache = FALSE;
}


/********************************************************************************************

>	ActionCode RegenerateContourAction::Init( 	Operation* 	pOp,
												ActionList* pActionList,
												List * pNodes
												ChangePositionShadowXAction** 	ppNewAction);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pNodes			= ptr to node list containing the NodeBevel nodes to be 
								  regenerated	
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
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



ActionCode RegenerateContourAction::Init(Operation* pOp,
										ActionList* pActionList,
										List * pNodes,
										RegenerateContourAction** ppNewAction,
										BOOL bCache)
{
	UINT32 ActSize = sizeof(RegenerateContourAction);
	
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(RegenerateContourAction),(Action**)ppNewAction);

	// make a copy of the node list
	List * pCopyList = NULL;
	ALLOC_WITH_FAIL(pCopyList, new List, pOp);
	NodeListItem * pItem = (NodeListItem *)pNodes->GetHead();
	NodeListItem * pCopyItem = NULL;

	while (pItem)
	{
		ALLOC_WITH_FAIL(pCopyItem, new NodeListItem, pOp);
		pCopyItem->pNode = pItem->pNode;
		
		pCopyList->AddTail(pCopyItem);

		pItem = (NodeListItem *)pNodes->GetNext(pItem);
	}

	(*ppNewAction)->m_pNodes = pCopyList;
	(*ppNewAction)->m_bCache = bCache;

	pItem = (NodeListItem *)pNodes->GetHead();

	DocView * pView = DocView::GetCurrent();

	Document * pDoc = Document::GetCurrent();

	DocRect dr;

	if (Ac != AC_FAIL)
	{
		while (pItem)
		{
			if (pItem->pNode && !pItem->pNode->IsNodeHidden() && pItem->pNode->FindParent()!=NULL)
			{
				dr = dr.Union(((NodeRenderableBounded *)pItem->pNode)->GetBoundingRect());
				
				if (pView && bCache)
				{
					GetApplication()->AddNodeToRegenList(pItem->pNode);
				}
				else
				{
					((NodeContourController *)(pItem->pNode))->RegenerateNode(NULL, FALSE);
				}

				dr = dr.Union(((NodeRenderableBounded *)pItem->pNode)->GetBoundingRect());
			}
			((NodeRenderableInk*)pItem->pNode)->ReleaseCached();
	
			pItem = (NodeListItem *)pNodes->GetNext(pItem);
		}

		if (pDoc)
		{
			if (Document::GetSelectedSpread())
			{
				pDoc->ForceRedraw(Document::GetSelectedSpread(), 
					dr);
			}
		}
	}

	(*ppNewAction)->m_OldRect = dr;

	// move my action to the head of the list
	// I have a problem, for certain ops we need to do the following, BUT for others
	// we don't (otherwise access violations occur).  um?
	pActionList->RemoveItem(*ppNewAction);
	pActionList->AddHead(*ppNewAction);

	return Ac;
}

/********************************************************************************************

>	ActionCode RegenerateContourAction::Execute();

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/99
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  Causes a regen of all bevels nodes in the action's list
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode RegenerateContourAction::Execute()
{
	ActionCode Act;
	RegenerateContourAction* pAction;
	
	Act = RegenerateContourAction::Init(pOperation, 
										pOppositeActLst,
										m_pNodes,
										&pAction,
										m_bCache);

	if (Act != AC_FAIL)
	{
	}

	return Act;
}

RegenerateContourAction::~RegenerateContourAction()
{
	if (m_pNodes)
	{
		m_pNodes->DeleteAll();
		delete m_pNodes;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Contour ops implementation

/********************************************************************************************
>	OpChangeContourWidth::OpChangeContourWidth()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Purpose:	Constructor.
********************************************************************************************/
OpChangeContourWidth::OpChangeContourWidth()
{
	//TRACEUSER( "MarkH", _T("Just Made a OpChangeContourWidth Class!\n"));
}



/********************************************************************************************
>	OpChangeContourWidth::~OpChangeContourWidth()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Purpose:	Destructor.
********************************************************************************************/
OpChangeContourWidth::~OpChangeContourWidth()
{
	//TRACEUSER( "MarkH", _T("Just Killed OpChangeContourWidth Class!!!\n"));
}



/********************************************************************************************
>	virtual void OpChangeContourWidth::Do(OpDescriptor *pOpDesc)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		pOpDesc - OpDescriptor - unused
	Outputs:	-
	Returns:	-
	Purpose:	The do function.  Applys a Bevel to the selection in the current document
********************************************************************************************/
void OpChangeContourWidth::Do(OpDescriptor *pOpDesc)
{
	End();
}


/********************************************************************************************
>	void OpChangeContourWidth::DoWithParam(OpDescriptor* pOp, OpParam* pParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Applys a Bevel to the selected node after a mouse click
********************************************************************************************/

void OpChangeContourWidth::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	// first, begin the operation
	ObjChangeFlags cFlags;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

	String_64 StatusString;
	StatusString.Load(_R(IDS_CONTOUR_CREATE_STATUS_STRING));

	BeginSlowJob(-1, TRUE, &StatusString);

	ChangeContourWidthParam * pCCW = (ChangeContourWidthParam *)pParam;

	// fire off an action
	ChangeContourWidthAction * pAction = NULL;

	if (ChangeContourWidthAction::Init(this, this->GetUndoActionList(), pCCW->m_pNodes, pCCW->m_Width,
		pCCW->m_bKeepDirection,	&pAction) != AC_OK)
	{
		FailAndExecute();
		return;
	}

	// do the child change messages
	NodeListItem * pItem = (NodeListItem *)pCCW->m_pNodes->GetHead();

	Node * pNode = NULL;

	ObjChangeFlags flgs(FALSE, FALSE, FALSE, TRUE);
	Node * pContourNode = NULL;
	Node * pInsertNode  = NULL;

	BOOL ok = TRUE;

	while (pItem)
	{
		// move all contour nodes in the list appropriately
		if (!pCCW->m_bKeepDirection)
		{
			pContourNode = pItem->pNode->FindFirstChild(CC_RUNTIME_CLASS(NodeContour));

			if (pContourNode)
				pContourNode->AllowOp(&ObjChange);

			if (pCCW->m_Width < 0)
			{
				// outer contour nodes come before the nodes they are contouring
				
				pInsertNode = pItem->pNode->FindFirstChild(CC_RUNTIME_CLASS(NodeRenderableInk));
				
				if (pInsertNode != pContourNode)
				{
					if (ok)
						ok = DoMoveNode(pContourNode, pInsertNode, PREV);
				}
			}
			else
			{
				// inner contour nodes come after the nodes they are contouring
				
				pInsertNode = pItem->pNode->FindLastChild(CC_RUNTIME_CLASS(NodeRenderableInk));
				
				if (pInsertNode != pContourNode)
				{
					if (ok)
						ok = DoMoveNode(pContourNode, pInsertNode, NEXT);
				}
			}
		}
			
		pNode = pItem->pNode->FindParent();

		// Ilan 7/5/00
		// Inform geom linked attrs of the change. Nb outside the normal AllowOp mechanism
		NodeAttribute* pNA = pItem->pNode->FindFirstGeometryLinkedAttr();
		while(pNA)
		{
			pNA->LinkedNodeGeometryHasChanged(this);
			pNA = pNA->FindNextGeometryLinkedAttr();
		}

		while (pNode)
		{
			//ObjChangeParam OP(OBJCHANGE_FINISHED, flgs, pNode, this, OBJCHANGE_CALLEDBYOP);

			if (pNode->IsAnObject())
			{
				//pNode->OnChildChange(&OP);
				pNode->AllowOp(&ObjChange);
			}

			pNode = pNode->FindParent();
		}

		pItem = (NodeListItem *)pCCW->m_pNodes->GetNext(pItem);
	}

	ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
	UpdateChangedNodes(&ObjChange, Document::GetSelectedSpread());

	End();
}

/********************************************************************************************

>	BOOL OpChangeContourWidth::Declare()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94																		
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/
BOOL OpChangeContourWidth::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_CHANGECONTOURWIDTHOP),
								CC_RUNTIME_CLASS(OpChangeContourWidth), 
								OPTOKEN_CHANGECONTOURWIDTH,
								OpChangeContourWidth::GetState));

}



/********************************************************************************************

>	OpState OpChangeContourWidth::GetState(String_256* Description, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Outputs:	-
	Returns:	Ungreyed, Unticked
	Purpose:	Find out the state of the new regular shape at the specific time

********************************************************************************************/
OpState OpChangeContourWidth::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}

/********************************************************************************************

>	void OpChangeContourWidth::GetOpName(String_256 *pName)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/99
	Outputs:	-
	Returns:	The name of the op
	Purpose:	Returns the name of this op

********************************************************************************************/
void OpChangeContourWidth::GetOpName(String_256 *pName)
{
	if (pName)
		pName->Load(_R(IDS_CHANGECONTOURWIDTHOPNAME));
}


////////////////////////////////////////////////////////////////////
// ChangeContourWidthAction class implementation
/********************************************************************************************

>	ChangeContourWidthAction::ChangeContourWidthAction()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ChangeContourWidthAction::ChangeContourWidthAction()
{

}


/********************************************************************************************

>	ActionCode ChangeContourWidthAction::Init( 	Operation* 	pOp,
												ActionList* pActionList,
												List * pNodes
												ChangePositionShadowXAction** 	ppNewAction);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pNodes			= ptr to node list containing the NodeBevel nodes to be 
								  addressed
				NewWidth		= the new width to set all nodes in the list to
				bKeepDirection  = Whether to keep the direction (either inner or outer) when
								  setting the widths, or just to take the width as passed in
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
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



ActionCode ChangeContourWidthAction::Init(Operation* pOp,
										ActionList* pActionList,
										List * pNodes,
										MILLIPOINT NewWidth,
										BOOL bKeepDirection,
										ChangeContourWidthAction** ppNewAction)
{
	if (bKeepDirection && NewWidth < 0)
	{
		ERROR3("Can't keep the direction when the width is < 0");
		return AC_FAIL;
	}
	
	UINT32 ActSize = sizeof(ChangeContourWidthAction);
	
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(ChangeContourWidthAction),(Action**)ppNewAction);

	NodeListItem * pItem = NULL;
	NodeListItem * pNewItem = NULL;

	ContourWidthListItem * pNewWidthItem = NULL;

	Document * pDoc = Document::GetCurrent();

	NodeContourController * pContour = NULL;

	MILLIPOINT MaxWidth = 0;

	if (Ac == AC_OK)
	{
		// make a copy of the node list
		pItem = (NodeListItem *)pNodes->GetHead();

		while (pItem)
		{
			// store the node, and its old width value
			pContour = (NodeContourController *)pItem->pNode;

			ALLOC_WITH_FAIL(pNewItem, new NodeListItem(pContour), pOp);
			ALLOC_WITH_FAIL(pNewWidthItem, new ContourWidthListItem(pContour->GetWidth()), pOp);

			// invalidate the old rect
			if (pDoc)
			{
				pDoc->ForceRedraw(pContour->FindParentSpread(), pContour->GetBoundingRect(FALSE, FALSE),
					FALSE, pContour);
			}

			// regenerate the node
			if (NewWidth != 0)
			{
				// set the contour controller's new width
				if (!bKeepDirection)
				{
					if (NewWidth > 0)
					{
						MaxWidth = ContourNodePathProcessor::GetMaxInnerContourWidth(pContour);

						if (NewWidth > MaxWidth)
						{
							pContour->SetWidth(MaxWidth);
						}
						else
						{
							pContour->SetWidth(NewWidth);
						}
					}
					else
					{
						pContour->SetWidth(NewWidth);
					}					
				}
				else
				{
					if (pContour->GetWidth() < 0)
					{
						pContour->SetWidth(-NewWidth);
					}
					else
					{
						MaxWidth = ContourNodePathProcessor::GetMaxInnerContourWidth(pContour);

						if (NewWidth > MaxWidth)
						{
							pContour->SetWidth(MaxWidth);
						}
						else
						{
							pContour->SetWidth(NewWidth);
						}
					}
				}

				pContour->RegenerateNode(NULL, FALSE);
			}

			// invalidate the new rect
			if (pDoc)
			{
				pDoc->ForceRedraw(pContour->FindParentSpread(), pContour->GetBoundingRect(FALSE, FALSE),
					FALSE, pContour);
			}

			// add the items to the lists
			(*ppNewAction)->m_Nodes.AddTail(pNewItem);
			(*ppNewAction)->m_WidthList.AddTail(pNewWidthItem);

			pItem = (NodeListItem *)pNodes->GetNext(pItem);
		}
	}
	
	return Ac;
}

/********************************************************************************************

>	ActionCode ChangeContourWidthAction::Execute();

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/99
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  Causes a regen of all bevels nodes in the action's list
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeContourWidthAction::Execute()
{
	ChangeContourWidthAction * pNewAction = NULL;

	// first, initialise an action to restore all the current widths in my list
	ActionCode Ac = ChangeContourWidthAction::Init(pOperation,
												   pOppositeActLst,
												   &m_Nodes,
												   0, // tell the action not to regen - I'm 
												   FALSE,  // going to do that
												   &pNewAction);

	NodeListItem * pItem = NULL;
	NodeContourController * pContour = NULL;
	ContourWidthListItem * pWidthItem = NULL;

	Document * pDoc = Document::GetCurrent();

	if (Ac == AC_OK)
	{
		// run through my lists resetting the original widths back to what they were
		pItem = (NodeListItem *)m_Nodes.GetHead();
		pWidthItem = (ContourWidthListItem *)m_WidthList.GetHead();

		while (pItem && pWidthItem)
		{
			pContour = (NodeContourController *)pItem->pNode;

			// invalidate the old rect
			if (pDoc)
			{
				pDoc->ForceRedraw(pContour->FindParentSpread(), 
					pContour->GetBoundingRect(FALSE, FALSE),
					FALSE, pContour);
			}

			// restore the width of the node, and regenerate
			pContour->SetWidth(pWidthItem->m_Width);

			pContour->RegenerateNode(NULL, FALSE);

			// invalidate the new rect
			if (pDoc)
			{
				pDoc->ForceRedraw(pContour->FindParentSpread(), 
					pContour->GetBoundingRect(FALSE, FALSE),
					FALSE, pContour);
			}

			pItem = (NodeListItem *)m_Nodes.GetNext(pItem);
			pWidthItem = (ContourWidthListItem *)m_WidthList.GetNext(pWidthItem);
		}
	}
	
	return Ac;
}

ChangeContourWidthAction::~ChangeContourWidthAction()
{
	m_Nodes.DeleteAll();
	m_WidthList.DeleteAll();
}

////////////////////////////////////////////////////////////////////
// ChangeContourToOuterAction class implementation
/********************************************************************************************

>	ChangeContourToOuterAction::ChangeContourToOuterAction()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ChangeContourToOuterAction::ChangeContourToOuterAction()
{
	m_LastWidth = 0;

}


/********************************************************************************************

>	ActionCode ChangeContourToOuterAction::Init( 	Operation* 	pOp,
												ActionList* pActionList,
												List * pNodes
												ChangePositionShadowXAction** 	ppNewAction);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pNodes			= ptr to node list containing the NodeBevel nodes to be 
								  regenerated	
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
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



ActionCode ChangeContourToOuterAction::Init(Operation* pOp,
										ActionList* pActionList,
										NodeContourController * pController,
										ChangeContourToOuterAction** ppNewAction)
{
	UINT32 ActSize = sizeof(ChangeContourToOuterAction);
	
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(ChangeContourToOuterAction),(Action**)ppNewAction);

	if (Ac == AC_OK)
	{
		(*ppNewAction)->m_pContour = pController;
		(*ppNewAction)->m_LastWidth = pController->GetWidth();
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode ChangeContourToOuterAction::Execute();

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/99
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  Causes a regen of all bevels nodes in the action's list
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeContourToOuterAction::Execute()
{
	ChangeContourToOuterAction * pNewAction = NULL;

	// first, initialise an action to restore all the current widths in my list
	ActionCode Ac = ChangeContourToOuterAction::Init(pOperation,
												   pOppositeActLst,
												   m_pContour,
												   &pNewAction);

	Document * pDoc = Document::GetCurrent();

	if (Ac == AC_OK)
	{
		if (pDoc)
			pDoc->ForceRedraw(m_pContour->FindParentSpread(), m_pContour->GetBoundingRect(), FALSE, m_pContour);

		m_pContour->SetWidth(m_LastWidth);
		m_pContour->RegenerateNode(NULL, FALSE);

		if (pDoc)
			pDoc->ForceRedraw(m_pContour->FindParentSpread(), m_pContour->GetBoundingRect(), FALSE, m_pContour);
	}

	return Ac;
}

ChangeContourToOuterAction::~ChangeContourToOuterAction()
{
	
}

////////////////////////////////////////////////////////////////////
// ChangeContourToInnerAction class implementation
/********************************************************************************************

>	ChangeContourToInnerAction::ChangeContourToInnerAction()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ChangeContourToInnerAction::ChangeContourToInnerAction()
{
	m_LastWidth = 0;

}


/********************************************************************************************

>	ActionCode ChangeContourToInnerAction::Init( 	Operation* 	pOp,
												ActionList* pActionList,
												List * pNodes
												ChangePositionShadowXAction** 	ppNewAction);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pNodes			= ptr to node list containing the NodeBevel nodes to be 
								  regenerated	
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
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



ActionCode ChangeContourToInnerAction::Init(Operation* pOp,
										ActionList* pActionList,
										NodeContourController * pController,
										ChangeContourToInnerAction** ppNewAction)
{
	UINT32 ActSize = sizeof(ChangeContourToInnerAction);

	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(ChangeContourToInnerAction),(Action**)ppNewAction);

	if (Ac == AC_OK)
	{
		(*ppNewAction)->m_pContour = pController;
		(*ppNewAction)->m_LastWidth = pController->GetWidth();
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode ChangeContourToInnerAction::Execute();

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/99
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  Causes a regen of all bevels nodes in the action's list
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeContourToInnerAction::Execute()
{
	ChangeContourToInnerAction * pNewAction = NULL;

	// first, initialise an action to restore all the current widths in my list
	ActionCode Ac = ChangeContourToInnerAction::Init(pOperation,
												   pOppositeActLst,
												   m_pContour,
												   &pNewAction);

	Document * pDoc = Document::GetCurrent();

	if (Ac == AC_OK)
	{
		if (pDoc)
			pDoc->ForceRedraw(m_pContour->FindParentSpread(), m_pContour->GetBoundingRect(), FALSE, m_pContour);

		m_pContour->SetWidth(m_LastWidth);
		m_pContour->RegenerateNode(NULL, FALSE);

		if (pDoc)
			pDoc->ForceRedraw(m_pContour->FindParentSpread(), m_pContour->GetBoundingRect(), FALSE, m_pContour);
	}

	return Ac;
}

ChangeContourToInnerAction::~ChangeContourToInnerAction()
{
	
}

/////////////////////////////////////////////////////////////////////////////
// OpChangeContourToInner class

/********************************************************************************************
>	OpChangeContourWidth::OpChangeContourWidth()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Purpose:	Constructor.
********************************************************************************************/
OpChangeContourToInner::OpChangeContourToInner()
{
	//TRACEUSER( "MarkH", _T("Just Made a OpChangeContourWidth Class!\n"));
}



/********************************************************************************************
>	OpChangeContourToInner::~OpChangeContourToInner()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Purpose:	Destructor.
********************************************************************************************/
OpChangeContourToInner::~OpChangeContourToInner()
{
	//TRACEUSER( "MarkH", _T("Just Killed OpChangeContourToInner Class!!!\n"));
}



/********************************************************************************************
>	virtual void OpChangeContourToInner::Do(OpDescriptor *pOpDesc)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		pOpDesc - OpDescriptor - unused
	Outputs:	-
	Returns:	-
	Purpose:	The do function.  Applys a Bevel to the selection in the current document
********************************************************************************************/
void OpChangeContourToInner::Do(OpDescriptor *pOpDesc)
{
	TRACEUSER( "DavidM", _T("OpChangeContourToInner - Do !\n"));
	BeginSlowJob(-1, TRUE);
	DoStartSelOp(TRUE, TRUE);

	// get all the contour nodes in the selection
	List ContourList;

	BevelTools::BuildListOfSelectedNodes(&ContourList, CC_RUNTIME_CLASS(NodeContourController),
		TRUE);

	NodeListItem * pItem = (NodeListItem *)ContourList.GetHead();

	ChangeContourToInnerAction * pAction = NULL;

	NodeContourController * pControl = NULL;

	BOOL ok = TRUE;

	Document * pDoc = Document::GetCurrent();

	// set up the object changed param
	ObjChangeFlags flgs(FALSE, FALSE, FALSE, TRUE);
	flgs.RegenerateNode = TRUE;
	ObjChangeParam MyObjChangeParam(OBJCHANGE_FINISHED, flgs, NULL, this, OBJCHANGE_CALLEDBYOP);

	Node * pParent = NULL;

	// run through the list, changing all widths to be positive
	while (pItem && ok)
	{
		if (pItem->pNode)
		{
			pControl = (NodeContourController *)pItem->pNode;

			if (pControl->GetWidth() < 0)
			{
				// redraw the node
				if (pDoc)
					pDoc->ForceRedraw(pControl->FindParentSpread(), pControl->GetBoundingRect(), FALSE, pControl);

				// and invalidate all parents' rectangles
				pParent = pControl->FindParent();
				pControl->ReleaseCached();		// Associated with DoInvalidateRegion, does parents itself

				while (pParent)
				{
					if (pParent->IsAnObject())
					{
						DoInvalidateRegion(pParent->FindParentSpread(), ((NodeRenderableInk *)pParent)->GetBoundingRect());
					}

					pParent = pParent->FindParent();
				}				

				// check for maximum width
				MILLIPOINT MaxWidth = 
					ContourNodePathProcessor::GetMaxInnerContourWidth(pControl);

				if (ChangeContourToInnerAction::Init(this, this->GetUndoActionList(),
					pControl, &pAction) != AC_OK)
				{
					ok = FALSE;
				}

				// set the width to positive
				pControl->SetWidth(-pControl->GetWidth());

				if (pControl->GetWidth() > MaxWidth)
					pControl->SetWidth(MaxWidth);

				// move the node
				if (ok)
				{
					Node * pContourNode = pControl->FindFirstChild(CC_RUNTIME_CLASS(NodeContour));

					if (pContourNode)
					{
						Node * pInsertNode = pControl->FindLastChild(CC_RUNTIME_CLASS(NodeRenderableInk));

						if (pInsertNode != pContourNode)
							ok = DoMoveNode(pContourNode, pInsertNode, NEXT);
					}
				}

				// regenerate the node
				pControl->RegenerateNode(NULL, FALSE);

				// Ilan 7/5/00
				// Inform geom linked attrs of the change. Nb outside the normal AllowOp mechanism
				NodeAttribute* pNA = pControl->FindFirstGeometryLinkedAttr();
				while(pNA)
				{
					pNA->LinkedNodeGeometryHasChanged(this);
					pNA = pNA->FindNextGeometryLinkedAttr();
				}

				// redraw the node
				if (pDoc)
					pDoc->ForceRedraw(pControl->FindParentSpread(), pControl->GetBoundingRect(), FALSE, pControl);
			}
		}

		pItem = (NodeListItem *)ContourList.GetNext(pItem);
	}

	ContourList.DeleteAll();

	if (!ok)
	{
		FailAndExecute();
		End();
		return;
	}
	
	End();
}


/********************************************************************************************
>	void OpChangeContourToInner::DoWithParam(OpDescriptor* pOp, OpParam* pParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Changes all contours in the selection to inner contours
********************************************************************************************/

void OpChangeContourToInner::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	// first, begin the operation
	BeginSlowJob(-1, TRUE);
	DoStartSelOp(TRUE, TRUE);


	
	End();
}

/********************************************************************************************

>	BOOL OpChangeContourToInner::Declare()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94																		
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/
BOOL OpChangeContourToInner::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_OP_CONTOURINNER),
								CC_RUNTIME_CLASS(OpChangeContourToInner), 
								OPTOKEN_CHANGECONTOUR_INNER,
								OpChangeContourToInner::GetState,
								0,
								_R(IDBBL_CONTOURINNER)
								));

}



/********************************************************************************************

>	OpState OpChangeContourToInner::GetState(String_256* Description, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Outputs:	-
	Returns:	Ungreyed, Unticked
	Purpose:	Find out the state of the new regular shape at the specific time

********************************************************************************************/
OpState OpChangeContourToInner::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;

	TRACEUSER( "DavidM", _T("OpChangeContourToInner - get state !\n"));

	if (Description)
	{
		Description->Load(_R(IDS_CONTOURINNEROPNAME));
	}

	return Blobby;
}

/////////////////////////////////////////////////////////////////////////////
// OpChangeContourToOuter class

/********************************************************************************************
>	OpChangeContourWidth::OpChangeContourWidth()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Purpose:	Constructor.
********************************************************************************************/
OpChangeContourToOuter::OpChangeContourToOuter()
{
	//TRACEUSER( "MarkH", _T("Just Made a OpChangeContourWidth Class!\n"));
}



/********************************************************************************************
>	OpChangeContourToOuter::~OpChangeContourToOuter()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Purpose:	Destructor.
********************************************************************************************/
OpChangeContourToOuter::~OpChangeContourToOuter()
{
	//TRACEUSER( "MarkH", _T("Just Killed OpChangeContourToOuter Class!!!\n"));
}



/********************************************************************************************
>	virtual void OpChangeContourToOuter::Do(OpDescriptor *pOpDesc)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		pOpDesc - OpDescriptor - unused
	Outputs:	-
	Returns:	-
	Purpose:	Changes all contours in the selection to outer contours
********************************************************************************************/
void OpChangeContourToOuter::Do(OpDescriptor *pOpDesc)
{
	TRACEUSER( "DavidM", _T("OpChangeContourToOuter - Do !\n"));
	BeginSlowJob(-1, TRUE);
	DoStartSelOp(TRUE, TRUE);

	// get all the contour nodes in the selection
	List ContourList;

	BevelTools::BuildListOfSelectedNodes(&ContourList, CC_RUNTIME_CLASS(NodeContourController),
		TRUE);

	NodeListItem * pItem = (NodeListItem *)ContourList.GetHead();

	ChangeContourToOuterAction * pAction = NULL;

	NodeContourController * pControl = NULL;

	BOOL ok = TRUE;

	// set up the object changed param
	ObjChangeFlags flgs(FALSE, FALSE, FALSE, TRUE);
	flgs.RegenerateNode = TRUE;
	ObjChangeParam MyObjChangeParam(OBJCHANGE_FINISHED, flgs, NULL, this, OBJCHANGE_CALLEDBYOP);

	Node * pParent = NULL;

	Document * pDoc = Document::GetCurrent();

	// run through the list, changing all widths to be positive
	while (pItem && ok)
	{
		if (pItem->pNode)
		{
			pControl = (NodeContourController *)pItem->pNode;

			if (pControl->GetWidth() > 0)
			{
				// redraw the node
				if (pDoc)
					pDoc->ForceRedraw(pControl->FindParentSpread(), pControl->GetBoundingRect(), FALSE, pControl);

				// invalidate the parents
				pParent = pControl->FindParent();
				pControl->ReleaseCached();		// Associated with DoInvalidateRegion, does parents itself

				while (pParent)
				{
					if (pParent->IsAnObject())
					{
						DoInvalidateRegion(pParent->FindParentSpread(), ((NodeRenderableInk *)pParent)->GetBoundingRect());
					}

					pParent = pParent->FindParent();
				}			

				if (ChangeContourToOuterAction::Init(this, this->GetUndoActionList(),
					pControl, &pAction) != AC_OK)
				{
					ok = FALSE;
				}

				// set the width to negative
				pControl->SetWidth(-pControl->GetWidth());

				// move the node
				if (ok)
				{
					Node * pContourNode = pControl->FindFirstChild(CC_RUNTIME_CLASS(NodeContour));

					if (pContourNode)
					{
						Node * pInsertNode = pControl->FindFirstChild(CC_RUNTIME_CLASS(NodeRenderableInk));

						if (pInsertNode != pContourNode)
							ok = DoMoveNode(pContourNode, pInsertNode, PREV);
					}
				}


				// regenerate the node
				pControl->RegenerateNode(NULL, FALSE);

				// Ilan 7/5/00
				// Inform geom linked attrs of the change. Nb outside the normal AllowOp mechanism
				NodeAttribute* pNA = pControl->FindFirstGeometryLinkedAttr();
				while(pNA)
				{
					pNA->LinkedNodeGeometryHasChanged(this);
					pNA = pNA->FindNextGeometryLinkedAttr();
				}

				// redraw the node
				if (pDoc)
					pDoc->ForceRedraw(pControl->FindParentSpread(), pControl->GetBoundingRect(), FALSE, pControl);
			}
		}

		pItem = (NodeListItem *)ContourList.GetNext(pItem);
	}

	ContourList.DeleteAll();

	if (!ok)
	{
		FailAndExecute();
		End();
		return;
	}
	
	End();
}


/********************************************************************************************
>	void OpChangeContourToOuter::DoWithParam(OpDescriptor* pOp, OpParam* pParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Applys a Bevel to the selected node after a mouse click
********************************************************************************************/

void OpChangeContourToOuter::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	// first, begin the operation
	BeginSlowJob(-1, TRUE);
	
	End();
}

/********************************************************************************************

>	BOOL OpChangeContourToOuter::Declare()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94																		
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/
BOOL OpChangeContourToOuter::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_OP_CONTOUROUTER),
								CC_RUNTIME_CLASS(OpChangeContourToOuter), 
								OPTOKEN_CHANGECONTOUR_OUTER,
								OpChangeContourToOuter::GetState,
								0,
								_R(IDBBL_CONTOUROUTER)
								));

}



/********************************************************************************************

>	OpState OpChangeContourToOuter::GetState(String_256* Description, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Outputs:	-
	Returns:	Ungreyed, Unticked
	Purpose:	Find out the state of the new regular shape at the specific time

********************************************************************************************/
OpState OpChangeContourToOuter::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;

	if (Description)
	{
		Description->Load(_R(IDS_CONTOUROUTEROPNAME));
	}
	
	return Blobby;
}

///////////////////////////////////////////////////////////////////////////////////////////
//  OpChangeContourSteps
//
// Changes the number of steps of all the selected blends


/********************************************************************************************

>	void OpChangeContourSteps::DoWithParam(OpDescriptor*,OpParam* pOpParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Returns:	-
	Purpose:	This changes all the selected blend objects to have pOpParam->Param1 number of steps

********************************************************************************************/

void OpChangeContourSteps::DoWithParam(OpDescriptor*,OpParam* pOpParam)
{
	ERROR3IF(pOpParam == NULL,"NULL OpParam ptr");
	if (pOpParam == NULL) return;

	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(NodeRenderableInk));

	NodeListItem *pItem = NULL;

	BOOL ok = !NodeList.IsEmpty();

	if (ok) ok = DoStartSelOp(FALSE,FALSE);

	if (ok)
	{
		// The new number of steps is in pOpParam->Param1 of the 
		UINT32 NewNumSteps = UINT32(pOpParam->Param1);
		pItem = (NodeListItem *)NodeList.GetHead();

		Node* pSelNode = NULL;

		if (pItem)
		{
			pSelNode = pItem->pNode;
		}

		while (pSelNode != NULL && ok)
		{
			Node* pNode = pSelNode;

			pItem = (NodeListItem *)NodeList.GetNext(pItem);

			if (pItem)
			{
				pSelNode = pItem->pNode;
			}
			else
			{
				pSelNode = NULL;
			}

			if (pNode->IS_KIND_OF(NodeContourController))
			{
				// We now have a selected blend node so:
				//	Invalidate the node's region
				//	Store the current number of blend steps in an undo actiom
				// 	Change the number of steps to NewNumSteps
			
				NodeRenderableInk * pInk = (NodeRenderableInk *)pNode;

				UINT32 NumSteps = 0;
				double DistanceEntered = 0.0;
				NumSteps = ((NodeContourController *)pNode)->GetNumberOfSteps();
				DistanceEntered = 0.0;
				
				ChangeContourStepsAction* pAction;

				// Ask the node if it's ok to do the op
				ObjChangeFlags cFlags;
				ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);
				ok = pInk->AllowOp(&ObjChange);

				if (ok) ok = DoInvalidateNodeRegion(pInk,TRUE,FALSE);
				if (ok)	ok = (InvalidateBoundsAction::Init(this,&UndoActions,pInk,TRUE) != AC_FAIL);
				if (ok) ok = ChangeContourStepsAction::Init(this,&UndoActions,pInk,NumSteps, DistanceEntered, &pAction) != AC_FAIL;
				
				if (ok)
				{
					((NodeContourController *)pNode)->SetNumberOfSteps(NewNumSteps);
					pNode->RegenerateNode(NULL, FALSE);
				}
				
				if (ok) ok = DoInvalidateNodeRegion(pInk,TRUE,FALSE);
				if (ok)	ok = (InvalidateBoundsAction::Init(this,&UndoActions,pInk,TRUE) != AC_FAIL);
			}
		}
	}

	NodeList.DeleteAll();

	if (ok) 
	{
		
	}
	else
		FailAndExecute();

	End();
}

/********************************************************************************************

>	BOOL OpChangeContourSteps::Declare()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpChangeContourSteps::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								0,
								CC_RUNTIME_CLASS(OpChangeContourSteps), 
								OPTOKEN_CHANGECONTOURSTEPS,
								OpChangeContourSteps::GetState,
								0,  /* help ID */
								0,	/* bubble ID */
								0	/* bitmap ID */
								));
}


/********************************************************************************************

>	static OpState OpChangeContourSteps::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the operation 
	Returns:	The state of the operation, so that menu items (ticks and greying) can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpChangeContourSteps::GetState(String_256* Description, OpDescriptor*)
{
	OpState State(FALSE,TRUE); // It's not ticked, but it is greyed by default
	
	// DMc - to test for bevels & contours
	// are there any contour nodes in the selection
	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(NodeContourController));

	if (!NodeList.IsEmpty())
	{
		State.Greyed = FALSE;
	}

	NodeList.DeleteAll();

	if (State.Greyed)
		*Description = String_256(_R(IDS_CONTOURSTEPS));
	else
		*Description = String_256(_R(IDS_CONTOURSTEPS));

	return State;
}

/********************************************************************************************

>	virtual void OpChangeContourSteps::GetOpName(String_256* OpName) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpChangeContourSteps::GetOpName(String_256* OpName) 
{ 
	*OpName = String_256(_R(IDS_CONTOURSTEPS_UNDO));
}  



//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The ChangeContourStepsAction class

/********************************************************************************************

>	ChangeContourStepsAction::ChangeContourStepsAction()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ChangeContourStepsAction::ChangeContourStepsAction()
{
	pNodeBlend  = NULL;
	OldNumSteps = 0;
}


/********************************************************************************************

>	ActionCode ChangeContourStepsAction::Init( 	Operation* 	pOp,
												ActionList* pActionList,
												NodeBlend* 	pThisNodeBlend,
												UINT32 		NumSteps,
												ChangeContourStepsAction** 	ppNewAction);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pThisNodeBlend 	= ptr to NodeBlend to change 
				NumSteps	 	= Num steps to applied to pThisNodeBlend
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
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



ActionCode ChangeContourStepsAction::Init(Operation* pOp,
										ActionList* pActionList,
										Node* pThisNodeBlend,
										UINT32 NumSteps,
										double DistanceEntered,
										ChangeContourStepsAction** ppNewAction)
{
	UINT32 ActSize = sizeof(ChangeContourStepsAction);

	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(ChangeContourStepsAction),(Action**)ppNewAction);

	if (Ac != AC_FAIL)
	{
		(*ppNewAction)->pNodeBlend  = pThisNodeBlend;
		(*ppNewAction)->OldNumSteps = NumSteps;
		(*ppNewAction)->OldDistanceEntered = DistanceEntered;
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode ChangeContourStepsAction::Execute();

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the num blend steps in pThisNodeBlend to OldNumSteps,
				after creating another action to record the current num steps of pThisNodeBlend
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeContourStepsAction::Execute()
{
	ActionCode Act;
	ChangeContourStepsAction* pAction;

	UINT32 NumSteps = 0;
	double DistanceEntered = 0.0;

	NumSteps = ((NodeContourController *)pNodeBlend)->GetNumberOfSteps();

	Act = ChangeContourStepsAction::Init(	pOperation, 
										pOppositeActLst,
										pNodeBlend,
										NumSteps,
										DistanceEntered,
										&pAction);
	if (Act != AC_FAIL)
	{
		((NodeContourController *)pNodeBlend)->SetNumberOfSteps(OldNumSteps);
		pNodeBlend->RegenerateNode(NULL, FALSE);
	}

	return Act;
}

ChangeContourStepsAction::~ChangeContourStepsAction()
{
}

///////////////////////////////////////////////////////////////////////////////////////////
//  OpChangeContourColourType
//
// Changes the number of steps of all the selected blends


/********************************************************************************************

>	void OpChangeContourColourType::DoWithParam(OpDescriptor*,OpParam* pOpParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Returns:	-
	Purpose:	This changes all the selected blend objects to have pOpParam->Param1 number of steps

********************************************************************************************/

void OpChangeContourColourType::DoWithParam(OpDescriptor*,OpParam* pOpParam)
{
	ERROR3IF(pOpParam == NULL,"NULL OpParam ptr");
	if (pOpParam == NULL) return;

	DoStartSelOp(TRUE, TRUE);
	BeginSlowJob(-1, TRUE);

	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(NodeContourController));

	NodeListItem * pItem = (NodeListItem *)NodeList.GetHead();

	NodeContourController * pController = NULL;

	BOOL ok = TRUE;

	Node * pParent = NULL;

	while (pItem && ok)
	{
		pController = (NodeContourController *)pItem->pNode;

		ChangeContourColourTypeAction *pAction = NULL;

		if (ChangeContourColourTypeAction::Init(this, GetUndoActionList(), pController, (ColourBlendType)(INT32)(pOpParam->Param1),
			&pAction) != AC_OK)
		{
			FailAndExecute();
			End();
			return;
		}

		if (ok)
			ok = DoInvalidateNodeRegion(pController, TRUE);

		if (ok)
		{
			pParent = pController->FindParent();

			while (pParent)
			{
				ObjChangeFlags flgs;
				ObjChangeParam OP(OBJCHANGE_FINISHED, flgs, NULL, this, OBJCHANGE_CALLEDBYOP);

				pParent->OnChildChange(&OP);

				pParent = pParent->FindParent();
			}
		}		

		pItem = (NodeListItem *)NodeList.GetNext(pItem);
	}

	NodeList.DeleteAll();

	// Inform the effected parents of the change
	if (ok)
	{
		End();
	}
	else
	{
		FailAndExecute();
		End();
	}
}

/********************************************************************************************

>	BOOL OpChangeContourColourType::Declare()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpChangeContourColourType::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								0,
								CC_RUNTIME_CLASS(OpChangeContourColourType), 
								OPTOKEN_CHANGECONTOURCOLOURTYPE,
								OpChangeContourColourType::GetState,
								0,  /* help ID */
								0,	/* bubble ID */
								0	/* bitmap ID */
								));
}


/********************************************************************************************

>	static OpState OpChangeContourColourType::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the operation 
	Returns:	The state of the operation, so that menu items (ticks and greying) can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpChangeContourColourType::GetState(String_256* Description, OpDescriptor*)
{
	OpState State(FALSE,TRUE); // It's not ticked, but it is greyed by default
	
	// DMc - to test for bevels & contours
	// are there any contour nodes in the selection
	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(NodeContourController));

	if (!NodeList.IsEmpty())
	{
		State.Greyed = FALSE;
	}

	NodeList.DeleteAll();

	if (State.Greyed)
		*Description = String_256(_R(IDS_CONTOURCOLOURTYPE));
	else
		*Description = String_256(_R(IDS_CONTOURCOLOURTYPE));

	return State;
}

/********************************************************************************************

>	virtual void OpChangeContourColourType::GetOpName(String_256* OpName) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpChangeContourColourType::GetOpName(String_256* OpName) 
{ 
	*OpName = String_256(_R(IDS_CONTOURCOLOURTYPE));
}  



//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The ChangeContourColourTypeAction class

/********************************************************************************************

>	ChangeContourColourTypeAction::ChangeContourColourTypeAction()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ChangeContourColourTypeAction::ChangeContourColourTypeAction()
{
	m_pNode = NULL;
}


/********************************************************************************************

>	ActionCode ChangeContourColourTypeAction::Init( 	Operation* 	pOp,
												ActionList* pActionList,
												NodeBlend* 	pThisNodeBlend,
												UINT32 		NumSteps,
												ChangeContourColourTypeAction** 	ppNewAction);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pThisNodeBlend 	= ptr to NodeBlend to change 
				NumSteps	 	= Num steps to applied to pThisNodeBlend
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
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



ActionCode ChangeContourColourTypeAction::Init( Operation* pOp,
							ActionList* pActionList,
							NodeContourController* pThisNodeContour,
							ColourBlendType type,
							ChangeContourColourTypeAction** NewAction)
{
	UINT32 ActSize = sizeof(ChangeContourColourTypeAction);

	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,
		CC_RUNTIME_CLASS(ChangeContourColourTypeAction),(Action**)NewAction);

	Document * pDoc = Document::GetCurrent();

	if (Ac != AC_FAIL)
	{
		(*NewAction)->m_pNode  = pThisNodeContour;
		(*NewAction)->m_OldType = pThisNodeContour->GetColourBlendType();
		pThisNodeContour->SetColourBlendType(type);		

		// redraw the contour
		if (pDoc)
		{
			pDoc->ForceRedraw(pThisNodeContour->FindParentSpread(), 
				pThisNodeContour->GetBoundingRect(), FALSE, pThisNodeContour);
		}
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode ChangeContourColourTypeAction::Execute();

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the num blend steps in pThisNodeBlend to OldNumSteps,
				after creating another action to record the current num steps of pThisNodeBlend
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeContourColourTypeAction::Execute()
{
	ChangeContourColourTypeAction * pAction = NULL;
	
	ActionCode Act = ChangeContourColourTypeAction::Init(	pOperation, 
										pOppositeActLst,
										m_pNode,
										m_OldType,
										&pAction);
	if (Act != AC_FAIL)
	{
		
	}

	return Act;
}

ChangeContourColourTypeAction::~ChangeContourColourTypeAction()
{
}


//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The ChangeContourAttributeProfileAction class

/********************************************************************************************

>	ChangeContourAttributeProfileAction::ChangeContourAttributeProfileAction()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ChangeContourAttributeProfileAction::ChangeContourAttributeProfileAction()
{
	m_pNode = NULL;
}


/********************************************************************************************

>	ActionCode ChangeContourAttributeProfileAction::Init( 	Operation* 	pOp,
												ActionList* pActionList,
												NodeBlend* 	pThisNodeBlend,
												UINT32 		NumSteps,
												ChangeContourAttributeProfileAction** 	ppNewAction);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pThisNodeBlend 	= ptr to NodeBlend to change 
				NumSteps	 	= Num steps to applied to pThisNodeBlend
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
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



ActionCode ChangeContourAttributeProfileAction::Init( Operation* pOp,
							ActionList* pActionList,
							NodeContourController* pThisNodeContour,
							CProfileBiasGain Profile,
							ChangeContourAttributeProfileAction** NewAction)
{
	UINT32 ActSize = sizeof(ChangeContourAttributeProfileAction);

	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,
		CC_RUNTIME_CLASS(ChangeContourAttributeProfileAction),(Action**)NewAction);

	Document * pDoc = Document::GetCurrent();

	if (Ac != AC_FAIL)
	{
		(*NewAction)->m_pNode  = pThisNodeContour;
		(*NewAction)->m_Profile = pThisNodeContour->GetAttrProfile();

		// change the profile & regen
		pThisNodeContour->SetAttrProfile(Profile);
		pThisNodeContour->RegenerateNode(NULL, FALSE);
	
		// redraw the contour
		if (pDoc)
		{
			pDoc->ForceRedraw(pThisNodeContour->FindParentSpread(), 
				pThisNodeContour->GetBoundingRect(), FALSE, pThisNodeContour);
		}
	}

	return Ac;
}



/********************************************************************************************

>	void ChangeContourAttributeProfileAction::ChangeAttributeProfileWithNoUndo (CProfileBiasGain &Profile)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/2000
	Inputs:		Profile - the profile that is to be applied directly (i.e.  applied with no undo)
	Purpose:	When applying contour (attribute) profiles, we only want to generate one bit of undo information.
				This function allows us to meet this requirement (the one bit of undo information
				is generated via OpChangeContourAttributeProfile::DoWithParam ()).  This function is ONLY
				called from within ContourInfoBarOp::ChangeProfile () - after
				OpChangeContourAttributeProfile::DoWithParam () has been called.
	Errors:		-
	SeeAlso:	SoftShadowInfoBarOp::ChangeProfile (), OpChangeContourAttributeProfile::DoWithParam ().

********************************************************************************************/

void ChangeContourAttributeProfileAction::ChangeAttributeProfileWithNoUndo (CProfileBiasGain &Profile)
{
	BeginSlowJob(-1, TRUE);
	
	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(NodeContourController));

	NodeListItem * pItem = (NodeListItem *)NodeList.GetHead();

	NodeContourController * pController = NULL;

//	Node * pParent = NULL;

//	ChangeContourObjectProfileAction * pAction= NULL;

	Document * pDoc = Document::GetCurrent();

	while (pItem)
	{
		pController = (NodeContourController *)pItem->pNode;

		// change the profile & regen
		pController->SetAttrProfile(Profile);
		pController->RegenerateNode(NULL, FALSE);
	
		// redraw the contour
		if (pDoc)
		{
			pDoc->ForceRedraw(pController->FindParentSpread(), 
				pController->GetBoundingRect(), FALSE, pController);
		}
	
		pItem = (NodeListItem *)NodeList.GetNext(pItem);
	}

	NodeList.DeleteAll();

	EndSlowJob ();

	if (pDoc->GetOpHistory ().CanRedo ())
	{
		// then we need to clear out the redo information - since we are now 'before' it ....
		pDoc->GetOpHistory ().DeleteRedoableOps ();

		// and update the state of things ....
		DialogBarOp::SetSystemStateChanged();
		DialogBarOp::UpdateStateOfAllBars();
	}
}

/********************************************************************************************

>	ActionCode ChangeContourAttributeProfileAction::Execute();

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the num blend steps in pThisNodeBlend to OldNumSteps,
				after creating another action to record the current num steps of pThisNodeBlend
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeContourAttributeProfileAction::Execute()
{
	ChangeContourAttributeProfileAction * pAction = NULL;
	
	ActionCode Act = ChangeContourAttributeProfileAction::Init(	pOperation, 
										pOppositeActLst,
										m_pNode,
										m_Profile,
										&pAction);
	if (Act != AC_FAIL)
	{
		
	}

	return Act;
}

ChangeContourAttributeProfileAction::~ChangeContourAttributeProfileAction()
{
}

///////////////////////////////////////////////////////////////////////////////////////////
//  OpChangeContourColourType
//
// Changes the number of steps of all the selected blends


/********************************************************************************************

>	void OpChangeContourAttributeProfile::DoWithParam(OpDescriptor*,OpParam* pOpParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Returns:	-
	Purpose:	This changes all the selected blend objects to have pOpParam->Param1 number of steps

********************************************************************************************/

void OpChangeContourAttributeProfile::DoWithParam(OpDescriptor*,OpParam* pOpParam)
{
	ERROR3IF(pOpParam == NULL,"NULL OpParam ptr");
	if (pOpParam == NULL) return;

	BOOL ok = TRUE;

	ChangeContourProfileParam * pCParam = (ChangeContourProfileParam *)pOpParam;

	DoStartSelOp(TRUE, TRUE);
	BeginSlowJob(-1, TRUE);
	
	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(NodeContourController));

	NodeListItem * pItem = (NodeListItem *)NodeList.GetHead();

	NodeContourController * pController = NULL;

//	Node * pParent = NULL;

	ChangeContourAttributeProfileAction * pAction= NULL;

	while (pItem && ok)
	{
		pController = (NodeContourController *)pItem->pNode;

		ChangeContourAttributeProfileAction::Init(this, this->GetUndoActionList(), 
			pController, pCParam->m_Profile, &pAction);
	
		pItem = (NodeListItem *)NodeList.GetNext(pItem);
	}

	NodeList.DeleteAll();

	// Inform the effected parents of the change
	if (ok)
	{
		End();
	}
	else
	{
		FailAndExecute();
		End();
	}
}

/********************************************************************************************

>	BOOL OpChangeContourAttributeProfile::Declare()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpChangeContourAttributeProfile::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_CHANGECONTOURATTRIBUTEPROFILE),
								CC_RUNTIME_CLASS(OpChangeContourAttributeProfile), 
								OPTOKEN_CHANGECONTOURATTRPROFILE,
								OpChangeContourAttributeProfile::GetState,
								0,  /* help ID */
								0,	/* bubble ID */
								0	/* bitmap ID */
								));
}


/********************************************************************************************

>	static OpState OpChangeContourAttributeProfile::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the operation 
	Returns:	The state of the operation, so that menu items (ticks and greying) can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpChangeContourAttributeProfile::GetState(String_256* Description, OpDescriptor*)
{
	OpState State(FALSE,TRUE); // It's not ticked, but it is greyed by default
	
	// DMc - to test for bevels & contours
	// are there any contour nodes in the selection
	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(NodeContourController));

	if (!NodeList.IsEmpty())
	{
		State.Greyed = FALSE;
	}

	NodeList.DeleteAll();

	return State;
}

/********************************************************************************************

>	virtual void OpChangeContourAttributeProfile::GetOpName(String_256* OpName) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpChangeContourAttributeProfile::GetOpName(String_256* OpName) 
{ 
	*OpName = String_256(_R(IDS_CONTOURCOLOURTYPE));
}  

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The ChangeContourObjectProfileAction class

/********************************************************************************************

>	ChangeContourObjectProfileAction::ChangeContourObjectProfileAction()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ChangeContourObjectProfileAction::ChangeContourObjectProfileAction()
{
	m_pNode = NULL;
}


/********************************************************************************************

>	ActionCode ChangeContourObjectProfileAction::Init( 	Operation* 	pOp,
												ActionList* pActionList,
												NodeBlend* 	pThisNodeBlend,
												UINT32 		NumSteps,
												ChangeContourObjectProfileAction** 	ppNewAction);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pThisNodeBlend 	= ptr to NodeBlend to change 
				NumSteps	 	= Num steps to applied to pThisNodeBlend
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
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



ActionCode ChangeContourObjectProfileAction::Init( Operation* pOp,
							ActionList* pActionList,
							NodeContourController* pThisNodeContour,
							CProfileBiasGain Profile,
							ChangeContourObjectProfileAction** NewAction)
{
	UINT32 ActSize = sizeof(ChangeContourObjectProfileAction);

	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,
		CC_RUNTIME_CLASS(ChangeContourObjectProfileAction),(Action**)NewAction);

	Document * pDoc = Document::GetCurrent();

	if (Ac != AC_FAIL)
	{
		(*NewAction)->m_pNode  = pThisNodeContour;
		(*NewAction)->m_Profile = pThisNodeContour->GetObjectProfile();

		// change the profile & regen
		pThisNodeContour->SetObjectProfile(Profile);
		pThisNodeContour->RegenerateNode(NULL, FALSE);
	
		// redraw the contour
		if (pDoc)
		{
			pDoc->ForceRedraw(pThisNodeContour->FindParentSpread(), 
				pThisNodeContour->GetBoundingRect(), FALSE, pThisNodeContour);
		}
	}

	return Ac;
}



/********************************************************************************************

>	void ChangeContourObjectProfileAction::ChangeObjectProfileWithNoUndo (CProfileBiasGain &Profile)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/2000
	Inputs:		Profile - the profile that is to be applied directly (i.e.  applied with no undo)
	Purpose:	When applying contour (object) profiles, we only want to generate one bit of undo information.
				This function allows us to meet this requirement (the one bit of undo information
				is generated via OpChangeContourObjectProfile::DoWithParam ()).  This function is ONLY
				called from within ContourInfoBarOp::ChangeProfile () - after
				OpChangeContourObjectProfile::DoWithParam () has been called.
	Errors:		-
	SeeAlso:	SoftShadowInfoBarOp::ChangeProfile (), OpChangeContourObjectProfile::DoWithParam ().

********************************************************************************************/

void ChangeContourObjectProfileAction::ChangeObjectProfileWithNoUndo (CProfileBiasGain &Profile)
{
	BeginSlowJob(-1, TRUE);
	
	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(NodeContourController));

	NodeListItem * pItem = (NodeListItem *)NodeList.GetHead();

	NodeContourController * pController = NULL;

//	Node * pParent = NULL;

//	ChangeContourObjectProfileAction * pAction= NULL;

	Document * pDoc = Document::GetCurrent();

	while (pItem)
	{
		pController = (NodeContourController *)pItem->pNode;

		// change the profile & regen
		pController->SetObjectProfile(Profile);
		pController->RegenerateNode(NULL, FALSE);
	
		// redraw the contour
		if (pDoc)
		{
			pDoc->ForceRedraw(pController->FindParentSpread(), 
				pController->GetBoundingRect(), FALSE, pController);
		}
	
		pItem = (NodeListItem *)NodeList.GetNext(pItem);
	}

	NodeList.DeleteAll();

	EndSlowJob ();

	if (pDoc->GetOpHistory ().CanRedo ())
	{
		// then we need to clear out the redo information - since we are now 'before' it ....
		pDoc->GetOpHistory ().DeleteRedoableOps ();

		// and update the state of things ....
		DialogBarOp::SetSystemStateChanged();
		DialogBarOp::UpdateStateOfAllBars();
	}
}

/********************************************************************************************

>	ActionCode ChangeContourObjectProfileAction::Execute();

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the num blend steps in pThisNodeBlend to OldNumSteps,
				after creating another action to record the current num steps of pThisNodeBlend
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeContourObjectProfileAction::Execute()
{
	ChangeContourObjectProfileAction * pAction = NULL;
	
	ActionCode Act = ChangeContourObjectProfileAction::Init(	pOperation, 
										pOppositeActLst,
										m_pNode,
										m_Profile,
										&pAction);
	if (Act != AC_FAIL)
	{
		
	}

	return Act;
}

ChangeContourObjectProfileAction::~ChangeContourObjectProfileAction()
{
}

/********************************************************************************************

>	void OpChangeContourObjectProfile::DoWithParam(OpDescriptor*,OpParam* pOpParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Returns:	-
	Purpose:	This changes all the selected blend objects to have pOpParam->Param1 number of steps

********************************************************************************************/

void OpChangeContourObjectProfile::DoWithParam(OpDescriptor*,OpParam* pOpParam)
{
	ERROR3IF(pOpParam == NULL,"NULL OpParam ptr");
	if (pOpParam == NULL) return;

	BOOL ok = TRUE;

	ChangeContourProfileParam * pCParam = (ChangeContourProfileParam *)pOpParam;

	DoStartSelOp(TRUE, TRUE);
	BeginSlowJob(-1, TRUE);
	
	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(NodeContourController));

	NodeListItem * pItem = (NodeListItem *)NodeList.GetHead();

	NodeContourController * pController = NULL;

//	Node * pParent = NULL;

	ChangeContourObjectProfileAction * pAction= NULL;

	while (pItem && ok)
	{
		pController = (NodeContourController *)pItem->pNode;

		ChangeContourObjectProfileAction::Init(this, this->GetUndoActionList(), 
			pController, pCParam->m_Profile, &pAction);
	
		pItem = (NodeListItem *)NodeList.GetNext(pItem);
	}

	NodeList.DeleteAll();

	// Inform the effected parents of the change
	if (ok)
	{
		End();
	}
	else
	{
		FailAndExecute();
		End();
	}
}

/********************************************************************************************

>	BOOL OpChangeContourObjectProfile::Declare()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpChangeContourObjectProfile::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_CHANGECONTOUROBJECTPROFILE),
								CC_RUNTIME_CLASS(OpChangeContourObjectProfile), 
								OPTOKEN_CHANGECONTOUROBJPROFILE,
								OpChangeContourObjectProfile::GetState,
								0,  /* help ID */
								0,	/* bubble ID */
								0	/* bitmap ID */
								));
}


/********************************************************************************************

>	static OpState OpChangeContourObjectProfile::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the operation 
	Returns:	The state of the operation, so that menu items (ticks and greying) can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpChangeContourObjectProfile::GetState(String_256* Description, OpDescriptor*)
{
	OpState State(FALSE,TRUE); // It's not ticked, but it is greyed by default
	
	// DMc - to test for bevels & contours
	// are there any contour nodes in the selection
	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(NodeContourController));

	if (!NodeList.IsEmpty())
	{
		State.Greyed = FALSE;
	}

	NodeList.DeleteAll();

	return State;
}

/********************************************************************************************

>	virtual void OpChangeContourObjectProfile::GetOpName(String_256* OpName) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpChangeContourObjectProfile::GetOpName(String_256* OpName) 
{ 
	*OpName = String_256(_R(IDS_CONTOURCOLOURTYPE));
}  

///////////////////////////////////////////////////////////////////////////////////////////
//  OpChangeContourStepDistance
//
// Changes the number of steps of all the selected blends


/********************************************************************************************

>	void OpChangeContourStepDistance::DoWithParam(OpDescriptor*,OpParam* pOpParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Returns:	-
	Purpose:	This changes all the selected blend objects to have pOpParam->Param1 number of steps

********************************************************************************************/

void OpChangeContourStepDistance::DoWithParam(OpDescriptor*,OpParam* pOpParam)
{
	ERROR3IF(pOpParam == NULL,"NULL OpParam ptr");
	if (pOpParam == NULL) return;

	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(NodeRenderableInk));

	NodeListItem *pItem = NULL;

	BOOL ok = !NodeList.IsEmpty();

	if (ok) ok = DoStartSelOp(FALSE,FALSE);

	if (ok)
	{
		// The new number of steps is in pOpParam->Param1 of the 
		INT32 NewStepDistance = INT32(pOpParam->Param1);
		pItem = (NodeListItem *)NodeList.GetHead();

		Node* pSelNode = NULL;

		if (pItem)
		{
			pSelNode = pItem->pNode;
		}

		while (pSelNode != NULL && ok)
		{
			Node* pNode = pSelNode;

			pItem = (NodeListItem *)NodeList.GetNext(pItem);

			if (pItem)
			{
				pSelNode = pItem->pNode;
			}
			else
			{
				pSelNode = NULL;
			}

			if (pNode->IS_KIND_OF(NodeContourController))
			{
				// We now have a selected blend node so:
				//	Invalidate the node's region
				//	Store the current number of blend steps in an undo actiom
				// 	Change the number of steps to NewNumSteps
			
				NodeRenderableInk * pInk = (NodeRenderableInk *)pNode;

				UINT32 NumSteps = 0;
				double DistanceEntered = 0.0;
				NumSteps = ((NodeContourController *)pNode)->GetNumberOfSteps();
				DistanceEntered = 0.0;
				
				ChangeContourStepsAction* pAction;

				// Ask the node if it's ok to do the op
				ObjChangeFlags cFlags;
				ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);
				ok = pInk->AllowOp(&ObjChange);

				if (ok) ok = DoInvalidateNodeRegion(pInk,TRUE,FALSE);
				if (ok)	ok = (InvalidateBoundsAction::Init(this,&UndoActions,pInk,TRUE) != AC_FAIL);
				if (ok) ok = ChangeContourStepsAction::Init(this,&UndoActions,pInk,NumSteps, DistanceEntered, &pAction) != AC_FAIL;
				
				if (ok)
				{
					if (abs(((NodeContourController *)pNode)->GetWidth()) < NewStepDistance * 2)
					{
						((NodeContourController *)pNode)->SetNumberOfSteps(0);
					}
					else
					{
						((NodeContourController *)pNode)->SetNumberOfSteps(
							(abs(((NodeContourController *)pNode)->GetWidth()) / NewStepDistance) - 1);
					}
					pNode->RegenerateNode(NULL, FALSE);
				}
				
				if (ok) ok = DoInvalidateNodeRegion(pInk,TRUE,FALSE);
				if (ok)	ok = (InvalidateBoundsAction::Init(this,&UndoActions,pInk,TRUE) != AC_FAIL);
			}
		}
	}

	NodeList.DeleteAll();

	if (ok) 
	{
		// Inform the effected parents of the change
		ObjChangeFlags cFlags;
		ObjChangeParam ObjChange(OBJCHANGE_FINISHED,cFlags,NULL,this);
		UpdateChangedNodes(&ObjChange);
	}
	else
		FailAndExecute();

	End();
}

/********************************************************************************************

>	BOOL OpChangeContourStepDistance::Declare()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpChangeContourStepDistance::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								0,
								CC_RUNTIME_CLASS(OpChangeContourStepDistance), 
								OPTOKEN_CHANGECONTOURSTEPDISTANCE,
								OpChangeContourStepDistance::GetState,
								0,  /* help ID */
								0,	/* bubble ID */
								0	/* bitmap ID */
								));
}


/********************************************************************************************

>	static OpState OpChangeContourStepDistance::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the operation 
	Returns:	The state of the operation, so that menu items (ticks and greying) can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpChangeContourStepDistance::GetState(String_256* Description, OpDescriptor*)
{
	OpState State(FALSE,TRUE); // It's not ticked, but it is greyed by default
	
	// DMc - to test for bevels & contours
	// are there any contour nodes in the selection
	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(NodeContourController));

	if (!NodeList.IsEmpty())
	{
		State.Greyed = FALSE;
	}

	NodeList.DeleteAll();

	if (State.Greyed)
		*Description = String_256(_R(IDS_CONTOURSTEPS));
	else
		*Description = String_256(_R(IDS_CONTOURSTEPS));

	return State;
}

/********************************************************************************************

>	virtual void OpChangeContourStepDistance::GetOpName(String_256* OpName) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpChangeContourStepDistance::GetOpName(String_256* OpName) 
{ 
	*OpName = String_256(_R(IDS_CONTOURSTEPS_UNDO));
}  

/////////////////////////////////////////////////////////////////
// OpConvertPathToShapes class

/********************************************************************************************

>	BOOL OpConvertPathToShapes::Declare()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/2000
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/
BOOL OpConvertPathToShapes::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_CONVERTPATHTOSHAPES),
								CC_RUNTIME_CLASS(OpConvertPathToShapes), 
								OPTOKEN_CONVERTPATHTOSHAPES,
								OpConvertPathToShapes::GetState));

}

/********************************************************************************************

>	OpState OpConvertPathToShapes::GetState(String_256* Description, OpDescriptor*);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/2000
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Gets the state of the operation

********************************************************************************************/
OpState OpConvertPathToShapes::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;

	// do greyed for no selection
	Range * pSel = GetApplication()->FindSelection();

	Blobby.Greyed = FALSE;

	if (!pSel)
	{
		Blobby.Greyed = TRUE;
		return Blobby;
	}

	// If there is nothing selected, grey the item and give a reason.
	if (pSel->IsEmpty())
	{
		Blobby.Greyed = TRUE;
		*Description = String_256 (_R(IDS_NO_OBJECTS_SELECTED));
		return Blobby;
	}
	
	// run through the range finding out if any nodes have promote hit test on
	// children to me set in their parents, or are needs parent nodes
	Node * pNode = pSel->FindFirst();
	Node * pSubNode = NULL;
	Node * pParent = NULL;
	
	while (pNode && Blobby.Greyed == FALSE)
	{
		if (pNode->NeedsParent(NULL))
		{
			Blobby.Greyed = TRUE;
			break;
		}
		
		pParent = pNode->FindParent();
		
		while (pParent)
		{
			if (pParent->ShouldITransformWithChildren())
			{
				Blobby.Greyed = TRUE;
				break;
			}
			
			pParent = pParent->FindParent();
		}

		// check all nodes under this node too
		pSubNode = pNode->FindFirstDepthFirst();

		while (pSubNode && pSubNode != pNode)
		{
			if (pSubNode->NeedsParent(NULL) ||
				pSubNode->ShouldITransformWithChildren())
			{
				Blobby.Greyed = TRUE;
				break;
			}

			pSubNode = pSubNode->FindNextDepthFirst(pNode);
		}
		
		pNode = pSel->FindNext(pNode);
	}

	return Blobby;
}

/********************************************************************************************

>	void OpConvertPathToShapes::Do(OpDescriptor * pDesc)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/2000
	Returns:	
	Purpose:	Does the operation

********************************************************************************************/
void OpConvertPathToShapes::Do(OpDescriptor * pDesc)
{
	// run through the sel range, dealing with every node
	DoStartSelOp(TRUE, TRUE, TRUE, TRUE);

	Range * pSel = GetApplication()->FindSelection();

	if (!pSel)
	{
		FailAndExecute();
		End();
		return;
	}

	Node * pNode = pSel->FindFirst();
	Node * pNextNode = NULL;

	while (pNode)
	{
		// must get the next node first since the geometry will change
		pNextNode = pSel->FindNext(pNode);

		if (pNode->IsAnObject())
		{		
			if (!ConvertPathToShapes(this, (NodeRenderableInk *)pNode))
			{
				FailAndExecute();
				End();
				return;
			}
		}

		pNode = pNextNode;		
	}

	End();
}

/********************************************************************************************

>	void OpConvertPathToShapes::GetOpName(String_256* OpName)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/2000
	Returns:	
	Purpose:	Gets the op's name

********************************************************************************************/
void OpConvertPathToShapes::GetOpName(String_256* OpName)
{
	if (OpName)
	{
		OpName->Load(_R(IDS_CONVERTPATHTOSHAPES));
	}
}

/********************************************************************************************

>	static BOOL OpConvertPathToShapes::ConvertPathToShapes(OpConvertPathToShapes* pOp, NodeRenderableInk* pInk)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/2000
	Inputs:		pOp  - pointer to the op doing this (can be NULL for external use)
				pInk - the node to convert 
	Returns:	TRUE for success, FALSE for failure
	Purpose:	Does the convert

				DY 28/11/2000 Some serious changes have taken place now, in order to deal 
				with groups of objects containing brushes or strokes.  Basically the following can 
				occur:

				- pInk is a single ink object with a brush or stroke attribute, in which case it
				is dealt with indiviually in ConvertBrush/Stroke.
				- pInk is an ink object with no brush or stroke attribute - basically the old case, 
				here we simply sum the paths of pInk and make a new shape out of them.
				- pInk is a group containing any combination of normal shapes, brushed or stroked shapes.
				This is the difficult case, basically the ConvertPathToShapesBecomeA passback function
				will store converted brushes and strokes under a group context node (though there is one
				exception to this, where there are no brushes), and stores the summed paths of 
				any normal shapes.  We then extract these and put them all under one new group.

********************************************************************************************/

BOOL OpConvertPathToShapes::ConvertPathToShapes(OpConvertPathToShapes* pOp, NodeRenderableInk* pInk)
{
//	UINT32 NumObjs = 0;

	// first, find out whether we have a non-transparent line attribute

	// if we do, then lets not do anything.

	// BUT, if we do; then lets get on a do some funky stuff ....
	// i.e. make the line colour into the fill colour for this node

	AttrStrokeColour * pColour = NULL;
	
	pInk->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeColour), 
		(NodeAttribute **)(&pColour));

	ConvertPathToShapesBecomeA MyBecomeA(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath), pOp, FALSE);

	if (pColour)
	{
		StrokeColourAttribute * pAttrVal = (StrokeColourAttribute *)pColour->GetAttributeValue();
		
		if (pAttrVal->Colour.IsTransparent ())
		{
			return TRUE;
		}
		if (!pInk->CanBecomeA(&MyBecomeA))
		{
			ERROR3("Ink node cannot become nodepath in OpConvertPathToShapes::ConvertPathToShapes");
			return TRUE;
		}
	}
	

	/* find out if we have an applied brush node, if so then do our own thing
	Note that this block must always be before the Stroke attribute block because
	the stroke attribute, when used in conjunction with the brush attribute, acts
	as a pressure provider to the brush
	*/
//	NodeAttribute* pAttr = NULL;
	AttrBrushType* pBrush = NULL;
	pInk->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBrushType), (NodeAttribute**)&pBrush);
	
	// note that this only deals with a single ink node case, it cannot deal with groups
	if (pBrush != NULL && pBrush->GetBrushHandle() != BrushHandle_NoBrush && !pInk->IsCompound())
	{
		if (ConvertBrush(pOp, pBrush, pInk))
			return TRUE;
	}


	// We have another special case if there is a stroke attribute applied
	AttrStrokeType* pStroke = NULL;
	pInk->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeType), (NodeAttribute**)&pStroke);
	AttrVariableWidth* pVarWidth = NULL;
	pInk->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrVariableWidth), (NodeAttribute**)&pVarWidth);

	/* This next block exists as the simplest stroke case, and allows us to avoid making any groups,
	however we can only enter the next block if we are a single ink node with an an acitve stroke applied
	the entry conditions to this block: 
	- Do you have an active PathProcessorStroke?
	- Do you have an active VariableWidthFunction?
	- Are you not compound?
	*/
	if (pStroke != NULL && pStroke->HasPathProcessor() && pVarWidth != NULL && 	
		pVarWidth->HasActiveValueFunction() &&!pInk->IsCompound())
	{
		BOOL Success = ConvertStroke(pOp, pStroke, pInk);
		if (Success)
		{
			// invalidate the node so we get rid of any fill there might have been
			DocRect BRect = pInk->GetBoundingRect();
			pInk->ReleaseCached();
			Spread* pSpread = Document::GetSelectedSpread();
			if (pOp != NULL && pSpread != NULL)
				pOp->DoInvalidateRegion(pSpread, BRect);	

			pInk->SetSelected(FALSE);

			NodeHidden * pHidden = NULL;
			
			if (pOp)
				return pOp->DoHideNode(pInk, TRUE, &pHidden, FALSE);
			else
			{
				pInk->CascadeDelete();
				delete pInk;
				return(TRUE);
			}
		}

	}
	
	/* If we have reached this point then we are one of the following:
	- A group, potentially containing various combinations of brushes or strokes 
	- A single normal ink node, with no brush or stroke applied */

	// create the new path node
	NodePath * pNewNode = NULL;
	ALLOC_WITH_FAIL(pNewNode, new NodePath, pOp);

	pNewNode->InkPath.Initialise();	
	if (pOp)
		pOp->DoLocaliseForAttrChange(pInk, (AttrTypeSet *)NULL, (ObjectSet *)NULL);
	else
	{
		Node* pParent = pInk->FindParent(); 
 		ERROR3IF(!pParent, "UndoableOperation::DoLocaliseForAttrChange called on an object which has no parent"); 
		if (pParent->IsCompound())
		{
			((NodeRenderableInk*)pParent)->LocaliseCommonAttributes(FALSE, TRUE, NULL);
		}
	}


	// we need a group to store any groups of strokes we might have
	NodeRenderableInk* pGroup = NULL;

	// Do the conversion
	pInk->DoBecomeA(&MyBecomeA);

	// the summed path consists of the outlines of all the normal (i.e. non-brush or stroke) objects
	MyBecomeA.GetSummedPath(&(pNewNode->InkPath));

	// the context node is a group under which all strokes and brushes are placed
	pGroup = MyBecomeA.GetContextNode();
	

	// Right, some changes here to do with the case where we have strokes and brushes in a group
	// basically the BecomeA mechanism will have dealt with them separately, and generated a nodegroup
	// out of any brushes we may have had, and summed the paths of all other nodes.

	// first deal with any summed paths we may have generated
	if (pNewNode->InkPath.GetNumCoords() > 0)
	{
		AdjustStrokeAndFillAttrs(pOp, pNewNode, pInk);
	}
	else
	{
		delete pNewNode;
		pNewNode = NULL;
	}

	BOOL ok = TRUE;
	// ok, if we have a) a group and a nodepath or b) a group with a sibling, 
	// then we'll make an extra nodegroup to store them under

	if ((pNewNode && pGroup) || (pGroup && pGroup->FindNext()))
	{
		NodeGroup* pNewGroup = NULL;
		ALLOC_WITH_FAIL(pNewGroup, new NodeGroup, pOp);
		if (pNewGroup)
		{
			pGroup->InsertChainSimple(pNewGroup, LASTCHILD);
			if (pNewNode)
				pNewNode->AttachNode(pNewGroup, LASTCHILD);
		}
		if (pOp)
		{
			ok = pOp->DoInsertNewNode(pNewGroup, pInk, NEXT, TRUE, FALSE, TRUE, TRUE);
			if (ok) ok = pOp->DoFactorOutAfterAttrChange(pNewGroup, (AttrTypeSet *)NULL);
		}
		else
		{
			pNewGroup->AttachNode(pInk, NEXT);
			Node* pParent;
			pParent = pNewGroup->FindParent();
			ERROR3IF(!pParent, "Trying to FactorOutCommonChildAttributes on an object which has no parent");
			if (pParent->IsCompound())
			{
				((NodeRenderableInk*)pParent)->FactorOutCommonChildAttributes(TRUE, (AttrTypeSet *)NULL);
			}
		}
	}
	else if (pNewNode != NULL)
	{
		// otherwise we've only got one new node, made from non-strokes and brushes
		if (pOp)
		{
			ok = pOp->DoInsertNewNode(pNewNode, pInk, NEXT, TRUE, FALSE, TRUE, TRUE);
//			if (ok) ok = DoFactorOutAfterAttrChange(pNewNode, (AttrTypeSet *)NULL);
		}
		else
		{
			pNewNode->AttachNode(pInk, NEXT);
		}
	}
	else if (pGroup != NULL)
	{
		if (pOp)
		{
			ok = pOp->DoInsertNewNode(pGroup, pInk, NEXT, TRUE, FALSE, TRUE, TRUE);
			if (ok) ok = pOp->DoFactorOutAfterAttrChange(pGroup, (AttrTypeSet *)NULL);
		}
		else
		{
			pGroup->AttachNode(pInk, NEXT);
			Node* pParent;
			pParent = pGroup->FindParent();
			ERROR3IF(!pParent, "Trying to FactorOutCommonChildAttributes on an object which has no parent");
			if (pParent->IsCompound())
			{
				((NodeRenderableInk*)pParent)->FactorOutCommonChildAttributes(TRUE, (AttrTypeSet *)NULL);
			}
		}
	}

	if (pOp)
	{
		pOp->DoFactorOutAfterAttrChange(pInk, (AttrTypeSet *)NULL);
	}
	else
	{
		Node* pParent;
		pParent = pInk->FindParent();
		ERROR3IF(!pParent, "Trying to FactorOutCommonChildAttributes on an object which has no parent");
		if (pParent->IsCompound())
		{
			((NodeRenderableInk*)pParent)->FactorOutCommonChildAttributes(TRUE, (AttrTypeSet *)NULL);
		}
	}

	pInk->SetSelected(FALSE);

	// Hide the original ink node
	if (pOp)
	{
		NodeHidden * pHidden = NULL;
		if (ok)
			ok = pOp->DoHideNode(pInk, TRUE, &pHidden, FALSE);
	}
	else
	{
		pInk->CascadeDelete();
		delete pInk;
	}
		
	return ok;

}


/********************************************************************************************

>	static BOOL OpConvertPathToShapes::ConvertBrush(OpConvertPathToShapes* pOp, AttrBrushType* pAttrBrush, NodeRenderableInk* pInk)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/2000
	Inputs:		pAttrBrush - the brush attribute that is applied to the node we wish to convert
				pInk	   - the node we wish to convert
			

	Returns:	TRUE for success, FALSE for failure
	Purpose:	To convert nodes with a brush attribute applied to them.  Note that this ONLY
				works for single nodes, compound nodes are dealt with in the main Convert function
				as there are many different possible permutations.

********************************************************************************************/

BOOL OpConvertPathToShapes::ConvertBrush(OpConvertPathToShapes* pOp, AttrBrushType* pAttrBrush, NodeRenderableInk* pInk)
{
	ERROR2IF(pAttrBrush == NULL || pInk == NULL, FALSE, "Inputs are NULL to OpConvertPathToShapes::ConvertBrush");
	
	if (pInk->IsCompound())
	{
		ERROR3("Attempting to convert brush on a compound node when we really shouldn't be");
		return FALSE;
	}

	PathProcessorBrush* pPPB = pAttrBrush->GetPathProcessor();
	if (pPPB == NULL)
	{
		ERROR3("Brush has no path processor in OpConvertPathToShapes::ConvertBrush");
		return FALSE;
	}

	// Fairly simple, as we let the attributes becomea function do the work
	pInk->ReleaseCached();						// This call is associated with the DoInvalidateRegion call below
												// but must be called before pInk is removed from the tree
	pInk->SetSelected(FALSE);
	BecomeA BecomeAPath(BECOMEA_REPLACE,CC_RUNTIME_CLASS(NodePath), pOp, FALSE);
	BOOL Retval = pAttrBrush->DoBecomeA(&BecomeAPath, pInk);

	// If DoBecomeA worked, pInk is no longer in the tree...

	// invalidate the node so we get rid of any fill there might have been
	if (pOp)
	{
		DocRect BRect = pAttrBrush->GetAttrBoundingRect(pInk);
		Spread* pSpread = Document::GetSelectedSpread();
		if (pSpread != NULL)
			pOp->DoInvalidateRegion(pSpread, BRect);
	}

	return Retval;
}


/********************************************************************************************

>	static BOOL OpConvertPathToShapes::ConvertStroke(OpConvertPathToShapes* pOp, AttrStrokeType* pStroke, NodeRenderableInk* pInk)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/2000
	Inputs:		pStroke	   - the stroke attribute that is applied to the node we wish to convert
				pInk	   - the node we wish to convert
			

	Returns:	TRUE for success, FALSE for failure
	Purpose:	To convert nodes with a stroke attribute applied to them.  Note that this ONLY
				works for single nodes, compound nodes are dealt with in the main Convert function
				as there are many different possible permutations.
				Note that this function assumes that there is also an AttrVariableWidth applied
				to pInk, if there is not then your stroke attr just won't do anything.

********************************************************************************************/

BOOL OpConvertPathToShapes::ConvertStroke(OpConvertPathToShapes* pOp, AttrStrokeType* pStroke, NodeRenderableInk* pInk)
{
	ERROR2IF(pStroke == NULL || pInk == NULL, FALSE, "Inputs are NULL to OpConvertPathToShapes::ConvertBrush");
	
	if (pInk->IsCompound())
	{
		ERROR3("Attempting to convert brush on a compound node when we really shouldn't be");
		return FALSE;
	}

	// we need the path processor to do the work
	PathProcessorStroke* pPPS = pStroke->GetPathProcessor();
	if (pPPS == NULL)
	{
		ERROR3("Can't get PathProcessorStroke in OpConvertPathToShapes::ConvertStroke");
		return FALSE;
	}

	// Use a SimpleBecomeA to get the path from the parent object
	SimpleBecomeA BecomeA(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath), NULL);
	
	BOOL Retval = FALSE;
	
	if (pInk->DoBecomeA(&BecomeA))
	{
		NodePath* pNodePath = BecomeA.GetNodePath();
		if (pNodePath)
		{
			NodePath* pStrokePath = pPPS->GetSmoothProcessedPath(&(pNodePath->InkPath), pInk);
			if (pStrokePath)
			{
				// Karim 05/12/2000
				// process the returned stroke, to change its working
				// winding rule from non-zero to even-odd.
				Path* pInkStrokePath = &(pStrokePath->InkPath);
				double Flatness = pInkStrokePath->CalculateFlatnessValueFromPath(750.0, 2.0, 375.0);
				pInkStrokePath->ClipPathToPath(*pInkStrokePath, pInkStrokePath, 3 | 0x10, 20, Flatness, Flatness);

				// We've got the path we wish to use
				if (AdjustStrokeAndFillAttrs(pOp, pStrokePath, pInk))
				{
					if (pOp)
						Retval = pOp->DoInsertNewNode(pStrokePath, pInk, NEXT, TRUE, FALSE, TRUE, TRUE);
					else
					{
						pStrokePath->AttachNode(pInk, NEXT);
						Retval = TRUE;
					}
				}
				else
				{
					delete pStrokePath;
				}
			}
			delete pNodePath;
			pNodePath = NULL;
		}
		else
		{
			ERROR3("Failed to get nodepath from ink object in OpConvertPathToShapes::ConvertStroke");
		}
	}
	return Retval;
}

/********************************************************************************************

>	static BOOL OpConvertPathToShapes::AdjustStrokeAndFillAttrs(OpConvertPathToShapes* pOp, 
													NodePath* pNewNode, 
													NodeRenderableInk* pCreatedByNode = NULL,
													CCAttrMap* pAttrMap = NULL)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/2000
	Inputs:		pNewNode - the newpath node that we have inserted into the tree
				pCreatedByNode - the original node that created pNewNode, can be null
				pAttrMap - if pCreatedByNode is not supplied then an attrmap of the creator node 
							must be supplied

	Returns:	TRUE for success, FALSE for failure
	Purpose:	This is only of use for ink nodes that had a variable width stroke attribute
				applied to them.  In this case we wish to take the old stroke colour and make
				it into a new fill colour for pNewNode.  We also wish to apply a transparent
				stroke colour.

********************************************************************************************/

BOOL OpConvertPathToShapes::AdjustStrokeAndFillAttrs(OpConvertPathToShapes* pOp, 
														NodePath* pNewNode, 
														NodeRenderableInk* pCreatedByNode,
														CCAttrMap* pAttrMap)
{
	ERROR2IF(pNewNode == NULL, FALSE, "New node is NULL in OpConvertPathToShapes::AdjustStrokeAndFillAttrs");
	ERROR2IF(pCreatedByNode == NULL && pAttrMap == NULL, FALSE, "Both parent node and attrmap are NULL in OpConvertPathToShapes::AdjustStrokeAndFillAttrs");

	pNewNode->InkPath.IsFilled = TRUE;
	pNewNode->InkPath.IsStroked = TRUE;
	pNewNode->InkPath.InitialiseFlags();	

	// we need to get the stroke colour of our original object
	AttrStrokeColour * pColour = NULL;

	// look up this attribute
	if (pCreatedByNode)
	{
		pCreatedByNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeColour), 
			(NodeAttribute **)(&pColour));
	}
	else
	{
		pAttrMap->Lookup(CC_RUNTIME_CLASS(AttrStrokeColour), (void*&)pColour);
	}

	// make a new flat fill attribute and apply this to the node
	if (pColour)
	{
		StrokeColourAttribute * pAttrVal = (StrokeColourAttribute *)pColour->GetAttributeValue();

		if (pAttrVal)
		{
			AttrFlatColourFill *pFill = NULL;
			ALLOC_WITH_FAIL(pFill, new AttrFlatColourFill(pNewNode, FIRSTCHILD), pOp);
			
			if (pFill)
				pFill->SetStartColour(&(pAttrVal->Colour));
		}
	}
	else
		ERROR3("Unable to find stroke colour in OpConvertPathToShapes::AdjustStrokeAndFillAttrs");

	// If we've got a current fill transparency then leave it alone
	AttrFillGeometry* pTranspFill = NULL;
	if (pCreatedByNode)
	{
		pCreatedByNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTranspFillGeometry), (NodeAttribute**)&pTranspFill);
	}
	else
	{
		pAttrMap->Lookup(CC_RUNTIME_CLASS(AttrTranspFillGeometry), (void*&)pTranspFill);
	}
	if (pTranspFill && !pTranspFill->IsAFlatFill())
	{
		Node* pNewTranspFill = NULL;
		ALLOC_WITH_FAIL(pNewTranspFill, pTranspFill->PublicCopy(), pOp);
		if (pNewTranspFill)
		{
			pNewTranspFill->AttachNode(pNewNode, FIRSTCHILD);
		}
	}
	else
	{
		// if we've got a current stroke transparency then lets convert this to a fill transparency
		AttrStrokeTransp* pTransp = NULL;
		if (pCreatedByNode)
		{
			pCreatedByNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeTransp), (NodeAttribute**)&pTransp);
		}
		else
		{
			pAttrMap->Lookup(CC_RUNTIME_CLASS(AttrStrokeTransp), (void*&)pTransp);
		}
		if (pTransp)
		{
			// make a flatfill transp and apply it
			AttrFlatTranspFill* pTranspFill = NULL;
			ALLOC_WITH_FAIL(pTranspFill, new AttrFlatTranspFill(pNewNode, FIRSTCHILD), pOp);

			if (pTranspFill)
			{
				pTranspFill->SetStartTransp(&pTransp->Value.Transp);
				pTranspFill->SetTranspType(pTransp->Value.TranspType);
			}
		}
	}
	
	// apply a transparent line colour
	AttrStrokeColour * pTranspAttr = NULL;
	ALLOC_WITH_FAIL(pTranspAttr, new AttrStrokeColour(pNewNode, FIRSTCHILD), pOp);
	if (pTranspAttr)
	{
		StrokeColourAttribute * pTranspVal = (StrokeColourAttribute *)pTranspAttr->GetAttributeValue();
		pTranspVal->Colour = DocColour(COLOUR_NONE);
	}

	// apply a line width
	AttrLineWidth* pLineWidth = NULL;
	ALLOC_WITH_FAIL(pLineWidth, new AttrLineWidth(pNewNode, FIRSTCHILD), pOp);
	if (pLineWidth)
		pLineWidth->Value.LineWidth = 500;

	AttrWindingRule * pWindingRule = NULL;
	ALLOC_WITH_FAIL(pWindingRule, new AttrWindingRule(pNewNode, LASTCHILD), pOp);

	if (pWindingRule)
		pWindingRule->Value.WindingRule = NegativeWinding;

	return TRUE;
}



///////////////////////////////////////////////////////////////
// The becomeA structure for the above
/********************************************************************************************

>	ConvertPathToShapesBecomeA::ConvertPathToShapesBecomeA(	BecomeAReason Reason,
					CCRuntimeClass* pClass, 
					UndoableOperation* pOp, 
					BOOL Sel) : BecomeA(Reason, pClass, pOp, Sel)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/2000
	Inputs:		See base class
	Purpose:	Constructor (initialises the path)

********************************************************************************************/
ConvertPathToShapesBecomeA::ConvertPathToShapesBecomeA(	BecomeAReason Reason,
					CCRuntimeClass* pClass, 
					UndoableOperation* pOp, 
					BOOL Sel) : BecomeA(Reason, pClass, pOp, Sel)
{
	m_SummedPath.Initialise();
	m_pContextNode = NULL;
	
}

/********************************************************************************************

>	BOOL ConvertPathToShapesBecomeA::PassBack(NodeRenderableInk* pNewNode,
										  NodeRenderableInk* pCreatedByNode,
										  CCAttrMap* pAttrMap=NULL)
	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Inputs:		See base class
	Outputs:	TRUE for success, FALSE for failure
	Created:	10/2/2000
	Purpose:	The passback function for the convert path to shapes operation
				Just stores the path passed in so that the operation can use it

				DY 28/11/2000 This has been changed to deal with what gets passed back
				by brush and stroke attributes.  So basically there are three cases:

				1) No brush or stroke attributes applied:  Easy, we just add the paths to
				the summed path that we are storing.
				2) Stroke attribute applied: We ask the stroke for its stroked path and store it 
				as a sibling to our context node.
				3) Brush applied: Somewhat more complex - the BrushBecomeAGroup does pretty much
				what its name suggests, and we store the returned group under the context node.
				Recall that if both a brush and a stroke are applied then the stroke acts as a
				pressure provider to the brush.

********************************************************************************************/
BOOL ConvertPathToShapesBecomeA::PassBack(NodeRenderableInk* pNewNode,
										  NodeRenderableInk* pCreatedByNode,
										  CCAttrMap* pAttrMap)
{
	// continuity checks
	if (!pNewNode || !pCreatedByNode)
	{
		ERROR3("The new node or the created by node is NULL");
		return FALSE;
	}

	if (!pNewNode->IsNodePath())
	{
		ERROR3("New node isn't a node path");
		return FALSE;
	}

//	if (!GetUndoOp())
//	{
//		ERROR3("No operation defined");
//		return FALSE;
//	}

	// first, if we've not been given an attribute map, then create one
	if (!pAttrMap)
	{
		CCAttrMap * pXMap = CCAttrMap::MakeAppliedAttrMap(pCreatedByNode);
		
		pAttrMap = pXMap->Copy();

		delete pXMap;

		// exit if we don't have an attribute map
		if (!pAttrMap)
		{
			pNewNode->DeleteChildren(pNewNode->FindFirstChild());
			delete pNewNode;
			ERROR3("Couldn't create attribute map");
			return FALSE;
		}
	}
	NodePath * pPathNode = NULL;
	// Pre release bodge - DY 15/11 Brush attributes should really be catered for in MakeNodePathFromAttributes
	// but that has too many potential knock-on effects for me to test now
	AttrBrushType* pAttrBrush = NULL;
	pAttrMap->Lookup(CC_RUNTIME_CLASS(AttrBrushType), (void*&)pAttrBrush);
	if (pAttrBrush && pAttrBrush->GetBrushHandle() != BrushHandle_NoBrush)
	{
		//Turn the brush into a group with lots of nodepaths
		BrushBecomeAGroup BecomeA(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath), GetUndoOp());
		pAttrBrush->DoBecomeA(&BecomeA, pCreatedByNode);
		
		// the brush will create a group out of itself and we want to retrieve that
		NodeGroup* pBrushGroup = BecomeA.GetNodeGroup();
		if (pBrushGroup != NULL)
		{
			if (m_pContextNode == NULL)
			{
				m_pContextNode = pBrushGroup;
			}
			else
			{
				// insert as the last sibling
				Node* pNode = m_pContextNode;
				Node* pNext = NULL;
				while (pNode)
				{
					pNext = pNode->FindNext();
					if (pNext == NULL)
						pBrushGroup->AttachNode(pNode, NEXT);
					pNode = pNext;
				}
			}
		}
		else
			ERROR3("Failed to create node group!");

		pAttrMap->DeleteAttributes();
		delete pAttrMap;
		delete pNewNode;
		return TRUE;
	}

	// likewise if we have an active stroke attribute
	AttrStrokeType* pStroke = NULL;
	pAttrMap->Lookup(CC_RUNTIME_CLASS(AttrStrokeType), (void*&)pStroke);
	AttrVariableWidth* pVarWidth = NULL;
	pAttrMap->Lookup(CC_RUNTIME_CLASS(AttrVariableWidth), (void*&)pVarWidth);
	
	if (pStroke && pStroke->HasPathProcessor() && pVarWidth && pVarWidth->HasActiveValueFunction())
	{
		PathProcessorStroke* pPPS = pStroke->GetPathProcessor();
		Path* pPath = &((NodePath*)pNewNode)->InkPath;
		NodePath* pStrokedPath = pPPS->GetSmoothProcessedPath(pPath, pCreatedByNode);

		if (pStrokedPath)
		{
			// if we don't have a context node then make one
			if (m_pContextNode == NULL)
			{
				ALLOC_WITH_FAIL(m_pContextNode, new NodeGroup, GetUndoOp());
				if (m_pContextNode == NULL)
				{
					delete pStrokedPath;
					return FALSE;
				}
			}

			// Karim 05/12/2000
			// process the returned stroke, to change its working
			// winding rule from non-zero to even-odd.
			Path* pInkStrokePath = &(pStrokedPath->InkPath);
			double Flatness = pInkStrokePath->CalculateFlatnessValueFromPath(750.0, 2.0, 375.0);
			pInkStrokePath->ClipPathToPath(*pInkStrokePath, pInkStrokePath, 3 | 0x10, 20, Flatness, Flatness);

			// bit of a hack here, but it avoids some code duplication.  Basically OpConvertPathToShapes
			// has a really handy function doing all the things you need to do when converting a stroked shape
			// to a path, so we're going to assume that our undo op is definitely an OpConvertPathToShapes.
			UndoableOperation* pOp = GetUndoOp();
			if (pOp == NULL || pOp->IsKindOf(CC_RUNTIME_CLASS(OpConvertPathToShapes)))
			{
				OpConvertPathToShapes* pConvert = (OpConvertPathToShapes*)pOp;
		
				OpConvertPathToShapes::AdjustStrokeAndFillAttrs(pConvert, pStrokedPath, NULL, pAttrMap);
			}
			// insert as the last sibling
			Node* pNode = m_pContextNode;
			Node* pNext = NULL;
			while (pNode)
			{
				pNext = pNode->FindNext();
				if (pNext == NULL)
					pStrokedPath->AttachNode(pNode, NEXT);
				pNode = pNext;
			}
			pAttrMap->DeleteAttributes();
			delete pAttrMap;
			delete pNewNode;
			return TRUE;
		}
	}



	// This deals with the case if we have neither a brush or stroke attribute, 
	// simply sum the paths.

	double Flatness = ((NodePath *)pNewNode)->InkPath.CalculateFlatnessValueFromPath(750.0, 1.0, 375.0);
	pPathNode = ((NodePath *)pNewNode)->MakeNodePathFromAttributes(Flatness, pAttrMap, TRUE);

		// ensure that the path is correct by clipping it
	Path BlankPath;
	BlankPath.Initialise();
	
	Path ClipPath;
	ClipPath.Initialise();

	// delete the new node
	pNewNode->DeleteChildren(pNewNode->FindFirstChild());
	delete pNewNode;

	pAttrMap->DeleteAttributes();
	delete pAttrMap;

	if (!pPathNode)
	{
		return FALSE;
	}
/*
	// smooth the new node's path
	double Error = SMOOTH_MIN + (SMOOTH_MAX - SMOOTH_MIN)*pow((35/100), 3.0);

	FitCurveNoChangeGeometry::SmoothPath(&(pPathNode->InkPath), Error);
*/	
	DocRect dr1(0,0,0,0);
	DocRect dr2(0,0,0,0);

	m_SummedPath.GetTrueBoundingRect(&dr1);
	pPathNode->InkPath.GetTrueBoundingRect(&dr2);

	// now, let's add this node to the become A path
//	if (m_SummedPath.GetBoundingRect().IsIntersectedWith(pPathNode->InkPath.GetBoundingRect())) MRH 19/5/00
	if (dr1.IsIntersectedWith(dr2))
	{
		Path OrigPath;
		OrigPath.Initialise();
		OrigPath.CloneFrom(m_SummedPath);
		
		m_SummedPath.ClearPath();
		
		OrigPath.ClipPathToPath(pPathNode->InkPath, &m_SummedPath, 7 | CLIPPING_CLIP_WINDING,
			50, 50, 50);
	}
	else
	{
		m_SummedPath.MergeTwoPaths(pPathNode->InkPath);
	}

	delete pPathNode;

	return TRUE;
}


/********************************************************************************************

>	NodeRenderableInk* ConvertPathToShapesBecomeA::GetContextNode()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Inputs:		-
	Outputs:	The context node that was generated to hold brushes or strokes converted to 
				shapes, or NULL if there isn't one
	Created:	10/2/2000
	Purpose:	Having potentially converted brushes or strokes to shapes we need to retrieve 
				them.  This can take a couple of forms, depending on what was converted:
				1 brush will return one group containing nodepaths of the brush objects
				>1 brush will return a group, with group children containing the brushes
				1 or more brushes + 1 or more strokes will return a group
				Strokes ONLY - will return a nodepath, which will contain any other strokes
				as its siblings.
				

********************************************************************************************/

NodeRenderableInk* ConvertPathToShapesBecomeA::GetContextNode()
{
	if (m_pContextNode == NULL)
		return NULL;

	NodeRenderableInk* pRetNode = m_pContextNode;

	// if the context node has no children then we have no brushes, only strokes, so return the first sibling
	if (pRetNode->FindFirstChild() == NULL)
	{
		// we will have inserted any strokes as siblings to the context node
		pRetNode = static_cast<NodeRenderableInk*>(pRetNode->FindNext());
		if (pRetNode == NULL)
		{
			ERROR3("Urk, we don't seem to have anything to return in ConvertPathToShapesBecomeA::GetContextNode");
		}
		// unhook the context node and delete it
		m_pContextNode->CascadeDelete();
		delete m_pContextNode;
	}
	m_pContextNode = NULL;
	return pRetNode;
}

/////////////////////////////////////////////////////////////////////
// OpToggleContourInsetPath class

/********************************************************************************************

>	BOOL OpToggleContourInsetPath::Declare()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/2000
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/
BOOL OpToggleContourInsetPath::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_INSETPATHOP),
								CC_RUNTIME_CLASS(OpToggleContourInsetPath), 
								OPTOKEN_TOGGLEINSETPATH,
								OpToggleContourInsetPath::GetState));

}

/********************************************************************************************

>	OpState OpToggleContourInsetPath::GetState(String_256* Description, OpDescriptor*);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/2000
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Gets the state of the operation

********************************************************************************************/
OpState OpToggleContourInsetPath::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;

	// are there any contours in the selection ?
	List ContourList;

	BevelTools::BuildListOfSelectedNodes(&ContourList, CC_RUNTIME_CLASS(NodeContour));

	if (ContourList.IsEmpty())
	{
		Blobby.Greyed = TRUE;
	}
	else
	{
		ContourList.DeleteAll();
	}
	
	return Blobby;
}

/********************************************************************************************

>	void OpToggleContourInsetPath::Do(OpDescriptor * pDesc, OpParam * pParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/2000
	Returns:	
	Purpose:	Does the operation

********************************************************************************************/
void OpToggleContourInsetPath::DoWithParam(OpDescriptor * pDesc, OpParam * pParam)
{
	DoStartSelOp(TRUE, TRUE);

	if (!GetApplication()->FindSelection())
	{
		End();
		return;
	}

	BOOL bFlag = FALSE;

	if (pParam->Param1)
		bFlag = TRUE;

//	BOOL ok = TRUE;

	if (bFlag)
		m_OpName = _R(IDS_INSETPATH_OP_UNDO_NAME);
	else
		m_OpName = _R(IDS_INSETPATH_OP_REDO_NAME);

	List ContourList;
	BevelTools::BuildListOfSelectedNodes(&ContourList, CC_RUNTIME_CLASS(NodeContourController));

	NodeListItem * pItem = (NodeListItem *)ContourList.GetHead();

//	ToggleInsetPathAction * pAction = NULL;

	NodeContour * pContour = NULL;
	NodeContourController * pController = NULL;
	NodeHidden * pHidden = NULL;
	Node * pChild = NULL;
//	Node * pNext = NULL;
//	Node * pNodeGroup = NULL;
	CCAttrMap AttrMap;
//	NodeContour * pContourCopy = NULL;
	BOOL IsAnOuter = TRUE;

	while (pItem)
	{
		pController = (NodeContourController *)pItem->pNode;
		pContour = (NodeContour *)pController->FindFirstChild(CC_RUNTIME_CLASS(NodeContour));
		
		if (!pController || !pContour)
			goto FailExit;

		IsAnOuter = pController->IsContourAnOuterContour();

		if (bFlag)
		{
			// only swap attributes when you're going from a contour to an inset path node
			// now, try to find the first appropriate node to take an attribute map from to
			// copy over to the contour node
			pChild = pController->FindFirstDepthFirst();
			
			while (pChild)
			{
				if (pChild->IsAnObject() && !pChild->IS_KIND_OF(NodeContour) && !pChild->IsCompound())
				{
					if (((NodeRenderableInk *)pChild)->FindAppliedAttributes(&AttrMap))
					{
/*						// create a copy of the contour node, and apply this attribute map to it
						pContourCopy = (NodeContour *)pContour->PublicCopy();
						AttrMap.ApplyAttributesToNode(pContourCopy);
						
						// insert this node into the tree after the contour node
						if (!DoInsertNewNode(pContourCopy, pContour, NEXT, TRUE, FALSE, FALSE, TRUE))
						{
							goto FailExit;
						}

						if (!DoHideNode(pContour, TRUE, &pHidden, TRUE))
							goto FailExit;
*/
						// Create and initialize a new NodePath to hold the inset path!
						NodePath* pNewPath;
						ALLOC_WITH_FAIL(pNewPath, new NodePath, this);
						pNewPath->InkPath.Initialise();

						DoInvalidateNodeRegion(pController,FALSE);

						// Create the DoBecomeA Function and call it on the controller
						BecomeA BecomeAReplace(BECOMEA_REPLACE, CC_RUNTIME_CLASS(NodePath), this, FALSE);
						pContour->DoBecomeA(&BecomeAReplace);

						// Now get the relavent path from the group of path nodes
						NodePath* pPathToCopy = NULL;

						if(IsAnOuter)
							pPathToCopy = (NodePath*)pController->FindFirstChild(CC_RUNTIME_CLASS(NodePath));
						else
							pPathToCopy = (NodePath*)pController->FindLastChild(CC_RUNTIME_CLASS(NodePath));

						pNewPath->InkPath.CloneFrom(pPathToCopy->InkPath);

						// now Initialize the flag array with the new Path and insert it after the group node!
						pNewPath->InkPath.InitialiseFlags(0, pNewPath->InkPath.GetNumCoords());
						DoInsertNewNode(pNewPath, pController, NEXT, TRUE, FALSE, TRUE, TRUE);

						pNewPath->InkPath.IsFilled = TRUE;

						// Make sure we have the correct attributes applied to our new path and make it selected!
						AttrMap.ApplyAttributesToNode(pNewPath);
						pNewPath->NormaliseAttributes();
						
						pNewPath->SetSelected(TRUE);
						
						DoHideNode(pController, TRUE, &pHidden, TRUE);

						// ok we're out
						break;
						
					}
				}
				
				pChild = pChild->FindNextDepthFirst(pController);
			}
		}
/*
		if (ToggleInsetPathAction::Init(this, GetUndoActionList(), pController, bFlag,
			&pAction) != AC_OK)
		{
			goto FailExit;
		}

		// make sure that the controller node is deselected and the contour node is
		pController->SetSelected(FALSE);

		if (pContourCopy)
			pContourCopy->SetSelected(TRUE);

		// do a do become a on the controller
		BecomeA BecomeAReplace(BECOMEA_REPLACE, CC_RUNTIME_CLASS(NodePath), this, FALSE);
		pController->DoBecomeA(&BecomeAReplace);
*/		pItem = (NodeListItem *)ContourList.GetNext(pItem);
	}

	ContourList.DeleteAll();

	if (GetApplication()->FindSelection())
	{
		GetApplication()->FindSelection()->Update(TRUE);
	}
	
	End();
	return;

FailExit:
	FailAndExecute();
	End();
}

/********************************************************************************************

>	void OpToggleContourInsetPath::GetOpName(String_256* OpName)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/2000
	Returns:	
	Purpose:	Gets the op's name

********************************************************************************************/
void OpToggleContourInsetPath::GetOpName(String_256* OpName)
{
	if (OpName)
	{
		OpName->Load(m_OpName);
	}
}

//////////////////////////////////////////////////////////////////
// The ToggleInsetPathAction class
/********************************************************************************************

>	ToggleInsetPathAction::ToggleInsetPathAction()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ToggleInsetPathAction::ToggleInsetPathAction()
{
	m_pNode = NULL;
}


/********************************************************************************************

>	ActionCode ToggleInsetPathAction::Init( 	Operation* 	pOp,
												ActionList* pActionList,
												NodeContourController * pNode
												ChangePositionShadowXAction** 	ppNewAction);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pNode			= ptr to the node to address
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
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



ActionCode ToggleInsetPathAction::Init(Operation* pOp,
										ActionList* pActionList,
										NodeContourController * pNode,
										BOOL bFlag, 
										ToggleInsetPathAction** ppNewAction)
{
	UINT32 ActSize = sizeof(ToggleInsetPathAction);
	
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(ToggleInsetPathAction),(Action**)ppNewAction);

	if (Ac == AC_OK)
	{
		(*ppNewAction)->m_pNode = pNode;
		(*ppNewAction)->m_OldSetting = pNode->GetInsetPathFlag();

		pNode->SetInsetPathFlag(bFlag);

		pNode->RegenerateNode(NULL, FALSE, TRUE);
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode ToggleInsetPathAction::Execute();

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/99
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  Causes a regen of all bevels nodes in the action's list
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ToggleInsetPathAction::Execute()
{
	ActionCode Act;
	ToggleInsetPathAction* pAction;

	// re-invalidate the old rect
	Document * pDoc = Document::GetCurrent();

	if (pDoc)
	{
		pDoc->ForceRedraw(m_pNode->FindParentSpread(), m_pNode->GetBoundingRect(), FALSE, m_pNode);
	}
	
	Act = ToggleInsetPathAction::Init(	pOperation, 
										pOppositeActLst,
										m_pNode,
										m_OldSetting,
										&pAction);

	if (Act != AC_FAIL)
	{
	}

	return Act;
}

ToggleInsetPathAction::~ToggleInsetPathAction()
{

}



























