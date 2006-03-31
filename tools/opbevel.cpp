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
// Implementation of the Bevel tool operations

/*
*/


#include "camtypes.h"
#include "opbevel.h"	
//#include "nbevcont.h"
#include "nodebmp.h"

#ifdef BUILDSHADOWS

// Code headers
#include "app.h"
#include "document.h"
#include "attrmgr.h"
#include "attrbev.h"
#include "nodebev.h"
#include "progress.h"
#include "objchge.h"
#include "becomea.h"
#include "attrmgr.h"
#include "nodeattr.h"
#include "fillattr.h"
#include "fillval.h"
#include "list.h"
#include "nodetxts.h"
#include "nodetxtl.h"
#include "nodeblnd.h"
#include "nodebldr.h"
#include "nodemold.h"
#include "mkshapes.h"
#include "groupops.h"
#include "range.h"
#include "lineattr.h"
#include "bevtool.h"
#include "view.h"
#include "docview.h"
#include "nodecont.h"
#include "nodeshad.h"
#include "opshadow.h"
#include "textops.h"
#include "spread.h"
#include "csrstack.h"
#include "nodetxts.h"
#include "nodeblnd.h"
#include "ncntrcnt.h" // ContourNodePathProcessor
#include "ndmldpth.h"
#include "slicehelper.h"
#include "opliveeffects.h"
#include "nodeliveeffect.h"
#include "ophist.h"

// Resource headers
//#include "bevres.h"


DECLARE_SOURCE( "$Revision$" );

CC_IMPLEMENT_DYNCREATE(OpCreateBevel, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpRemoveBevel, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpChangeBevelLightAngle, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpSelectBevel, Operation)
CC_IMPLEMENT_DYNCREATE(BevelInfo, OpParam)
CC_IMPLEMENT_DYNCREATE(RegenerateBevelAction, Action)
CC_IMPLEMENT_DYNCREATE(RegenerateBevelBitmapAction, Action)
CC_IMPLEMENT_DYNCREATE(ChangeLightAnglesAction, Action)
CC_IMPLEMENT_DYNCREATE(RemoveBevelAttributesAction, Action)

#define new CAM_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Bevel tools implementation

/********************************************************************************************

>	BOOL BevelTools::BuildListOfSelectedNodes(List *pList,
								const CCRuntimeClass * pClass)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/99
	Inputs:		
	Purpose:	Builds a list of all given attribute nodes in the selection
********************************************************************************************/

BOOL BevelTools::BuildListOfSelectedNodes(List *pList, const CCRuntimeClass * pClass,
										  BOOL bPromoteToParents)
{
	if (!pList || GetApplication()->FindSelection() == NULL)
		return FALSE;
	
	// run through the selection adding all nodes to the list
	Range Sel(*(GetApplication()->FindSelection()));
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = bPromoteToParents;
	Sel.Range::SetRangeControl(rg);

	if (GetApplication()->FindSelection()->IsEmpty())
		return FALSE;

	Node * pNode = Sel.FindFirst(FALSE);

//	NodeListItem * pItem = NULL;

	while (pNode)
	{
		GetAllNodesUnderNode(pNode, pList, pClass);

		pNode = Sel.FindNext(pNode, FALSE);
	}

	if (pList->IsEmpty())
		return FALSE;

	return TRUE;
}



	
/********************************************************************************************

>	BOOL BevelTools::GetAllNodesUnderNode(const Node * pNode, List * pList,
						const CCRuntimeClass * pClass, INT32 depth = 0)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/99
	Inputs:		The node to recurse down from, and the list to add found attributes to
	Purpose:	Recurses down the tree building a list of all light attribute nodes
********************************************************************************************/

BOOL BevelTools::GetAllNodesUnderNode(const Node * pNode, List * pList,
											   const CCRuntimeClass * pClass, INT32 depth)
{
	if (!pNode || !pList)
		return FALSE;

	// NOTHING exists under a locked (destructive) effect!
	if (pNode->IsBitmapEffect() && ((NodeBitmapEffect*)pNode)->IsLockedEffect())
		return TRUE;

	const Node * pStartNode = pNode;

	Node * pCurNode = pNode->FindFirstChild();

//	Node * pChild = NULL;
	NodeListItem * pItem = NULL;

	if (!pCurNode)
	{
		// starting conditions
		if (pStartNode->IsKindOf(pClass) && depth == 0 && !IsNodeInList(pList, pStartNode))
		{
			pItem = new NodeListItem;

			if (pItem)
			{
				pItem->pNode = (Node *)pStartNode;
				pList->AddTail(pItem);

				return TRUE;
			}
		}

		return FALSE;
	}
	
	// step through the node's siblings
	while (pCurNode)
	{	
		// check sub-tree first
		GetAllNodesUnderNode(pCurNode, pList, pClass, depth + 1);
		
		// check this node is of the correct class, and it also has a parent
		if (pCurNode->IsKindOf(pClass) && pCurNode->FindParent() &&
			!pCurNode->IsNodeHidden() && !IsNodeInList(pList, pCurNode))
		{
			pItem = new NodeListItem;

			if (!pItem)
			{
				ERROR3("Can't create NodeListItem");
				return FALSE;
			}

			pItem->pNode = pCurNode;
			pList->AddTail(pItem);
		}

		// move onto next sibling
		pCurNode = pCurNode->FindNext();
	}

	// check original node
	if (pStartNode->IsKindOf(pClass) && pStartNode->FindParent() &&
			!pStartNode->IsNodeHidden() && !IsNodeInList(pList, pStartNode))
	{
		pItem = new NodeListItem;

		if (!pItem)
		{
			ERROR3("Can't create NodeListItem");
			return FALSE;
		}

		pItem->pNode = (Node *)pStartNode;
		pList->AddTail(pItem);
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL BevelTools::IsNodeInList(List * pList, Node * pNode)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/99
	Inputs:		A list of NodeListItems and the node to search for
	Purpose:	Returns TRUE if the node is in the list
********************************************************************************************/
BOOL BevelTools::IsNodeInList(List * pList, const Node * pNode)
{
	NodeListItem * pItem = (NodeListItem * )pList->GetHead();

	while (pItem)
	{
		if (pItem->pNode == pNode)
			return TRUE;

		pItem = (NodeListItem * )pList->GetNext(pItem);
	}

	return FALSE;
}

/********************************************************************************************

>	BOOL BevelTools::HasSelectedChild(NodeRenderableInk * pNode)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/99
	Inputs:		
	Purpose:	returns TRUE if any of the children of this node are selected
********************************************************************************************/
BOOL BevelTools::HasSelectedChild(NodeRenderableInk * pNode)
{
	NodeRenderableInk * pStepNode = (NodeRenderableInk *)pNode->FindFirstDepthFirst();

	while (pStepNode)
	{
		if (pStepNode->IsSelected() && pStepNode != pNode)
		{
			return TRUE;
		}

		pStepNode = (NodeRenderableInk *)pStepNode->FindNextDepthFirst(pNode);
	}

	return FALSE;
}
	





/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Bevel ops implementation

/********************************************************************************************
>	OpCreateBevel::OpCreateBevel()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> & Olivier
	Created:	21/11/96
	Purpose:	Constructor.
********************************************************************************************/

OpCreateBevel::OpCreateBevel()
{
	//TRACEUSER( "MarkH", _T("Just Made a OpCreateBevel Class!\n"));
	m_NameID = 0;
}

/********************************************************************************************
>	void OpCreateBevel::GetOpName(String_256 * pString)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> & Olivier
	Created:	21/11/96
	Purpose:	Gets the name of this operation - dependent on what it did !
********************************************************************************************/

void OpCreateBevel::GetOpName(String_256 * pString)
{
	if (pString && m_NameID != 0)
		pString->Load(m_NameID);
}

/********************************************************************************************
>	OpCreateBevel::~OpCreateBevel()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> & Olivier
	Created:	21/11/96
	Purpose:	Destructor.
********************************************************************************************/

OpCreateBevel::~OpCreateBevel()
{
	//TRACEUSER( "MarkH", _T("Just Killed OpCreateBevel Class!!!\n"));
}

/********************************************************************************************
>	virtual void OpCreateBevel::Do(OpDescriptor *pOpDesc)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> & Olivier
	Created:	21/11/96
	Inputs:		pOpDesc - OpDescriptor - unused
	Outputs:	-
	Returns:	-
	Purpose:	The do function.  Applys a Bevel to the selection in the current document
********************************************************************************************/

void OpCreateBevel::Do(OpDescriptor *pOpDesc)
{
}

/********************************************************************************************
>	void OpCreateBevel::DoWithParam(OpDescriptor* pOp, OpParam* pParam)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/01/97
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Applys a Bevel to the selected node after a mouse click
********************************************************************************************/

void OpCreateBevel::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	if (!pParam || !pOp)
		return; // can't happen really

	// check for bevel already existing in the selection
	BevelInfo *pBevelInfo = (BevelInfo *)pParam;

	if (pBevelInfo->m_Indent <= 0)
	{
		// can't do anything !
		return ;
	}

	if (GetApplication()->FindSelection() == NULL)
		return;

//	NodeBevel* pBevel = NULL; 
	Node * pNextNode  = NULL;
	NodeBevelController * pBevelControl = NULL; 
//	NodeBevelController * pNewBevelControl = NULL; 
//	NodeBevelBegin * pBevelBegin = NULL;
	NodeHidden * pNodeHidden =  NULL;
	Node * pCurrentNode = NULL;
	
//	BOOL bOuterToInner = FALSE;
	DocRect LastRect;

//	Spread * pSpread = Document::GetSelectedSpread();

//	Node * xpNode = NULL;
//	Node * xpNode2 = NULL;

//	Node * pAttribNode = NULL;

	Range * pSel = GetApplication()->FindSelection();

	if (pSel->IsEmpty())
		return;

	// Start a slow job
	BeginSlowJob();
	DoStartSelOp(TRUE, TRUE);

	List BevelNodeList;

	// do all the existing bevel nodes in the selection first
	BevelTools::BuildListOfSelectedNodes(&BevelNodeList, CC_RUNTIME_CLASS(NodeBevelController));

	NodeListItem *pNodeListItem = (NodeListItem *)BevelNodeList.GetHead();

	AttrBevelIndent * pAttrIndent = NULL;
	AttrBevelType   * pAttrType	  = NULL;
	AttrBevelContrast * pAttrContrast = NULL;
	AttrBevelLightAngle * pAttrLightAngle = NULL;
	AttrBevelLightTilt * pAttrLightTilt = NULL;

	HideNodeAction *pAction = NULL;

	m_NameID = 0;

	while (pNodeListItem)
	{		
		// change the bevel
		pBevelControl = (NodeBevelController *)pNodeListItem->pNode;

		// no bevel control ? errr - error !
		if (!pBevelControl)
		{
			ERROR3("Can't find bevel control");
			BevelNodeList.DeleteAll();
			FailAndExecute();
			End();
			return;
		}

		// invalidate the region before
		pBevelControl->ReleaseCached();
		if (!DoInvalidateRegion(pBevelControl->FindParentSpread(), 
			pBevelControl->GetBoundingRect(TRUE, FALSE)))
		{
			FailAndExecute();
			End();
			BevelNodeList.DeleteAll();
			return;
		}

		// insert nodes if they are defaults exist !
		if (pBevelInfo->m_bBevelIndentChanged || pBevelInfo->m_bBevelDirectionChanged)
		{
			if (pBevelControl->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBevelIndent), 
				(NodeAttribute **)&pAttrIndent))
			{
				if (pAttrIndent)
				{
					if (pAttrIndent->IsADefaultAttr())
					{
						ALLOC_WITH_FAIL(pAttrIndent, 
							new AttrBevelIndent(pBevelControl, FIRSTCHILD), this);
						pAttrIndent->Value.m_Indent = pBevelControl->m_Indent;
						
						if (pBevelControl->m_bOuter)
						{
							pAttrIndent->Value.m_Indent = -pBevelControl->m_Indent;
						}
						
						if (HideNodeAction::Init(this, GetUndoActionList(), (Node *)pAttrIndent,
							FALSE, (Action **)&pAction, FALSE) != AC_OK)
						{
							FailAndExecute();
							End();
							BevelNodeList.DeleteAll();
							return;				
						}
					}
				}
			}
		}

		if (pBevelInfo->m_bBevelTypeChanged)
		{
			if (pBevelControl->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBevelType), 
				(NodeAttribute **)&pAttrType))
			{
				if (pAttrType)
				{
					if (pAttrType->IsADefaultAttr())
					{
						ALLOC_WITH_FAIL(pAttrType, 
							new AttrBevelType(pBevelControl, FIRSTCHILD), this);
						pAttrType->Value.m_Type = pBevelControl->m_BevelType;
						
						if (HideNodeAction::Init(this, GetUndoActionList(), (Node *)pAttrType,
							FALSE, (Action **)&pAction, FALSE) != AC_OK)
						{
							FailAndExecute();
							End();
							BevelNodeList.DeleteAll();
							return;				
						}			
					}
				}
			}					
		}

		if (pBevelInfo->m_bBevelLightAngleChanged)
		{
			if (pBevelControl->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBevelLightAngle), 
				(NodeAttribute **)&pAttrLightAngle))
			{
				if (pAttrLightAngle)
				{
					if (pAttrLightAngle->IsADefaultAttr())
					{
						ALLOC_WITH_FAIL(pAttrLightAngle, 
							new AttrBevelLightAngle(pBevelControl, FIRSTCHILD), this);
						pAttrLightAngle->Value.m_LightAngle = (INT32)pBevelControl->m_LightAngle;
						
						if (HideNodeAction::Init(this, GetUndoActionList(), (Node *)pAttrLightAngle,
							FALSE, (Action **)&pAction, FALSE) != AC_OK)
						{
							FailAndExecute();
							End();
							BevelNodeList.DeleteAll();
							return;				
						}			
					}
				}
			}
		}

		if (pBevelInfo->m_bBevelLightTiltChanged)
		{
			if (pBevelControl->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBevelLightTilt), 
				(NodeAttribute **)&pAttrLightTilt))
			{
				if (pAttrLightTilt)
				{
					if (pAttrLightTilt->IsADefaultAttr())
					{
						ALLOC_WITH_FAIL(pAttrLightTilt, 
							new AttrBevelLightTilt(pBevelControl, FIRSTCHILD), this);
						pAttrLightTilt->Value.m_LightTilt = (INT32)pBevelControl->m_Tilt;
						
						if (HideNodeAction::Init(this, GetUndoActionList(), (Node *)pAttrLightTilt,
							FALSE, (Action **)&pAction, FALSE) != AC_OK)
						{
							FailAndExecute();
							End();
							BevelNodeList.DeleteAll();
							return;				
						}			
					}
				}
			}
		}


		if (pBevelInfo->m_bBevelContrastChanged)
		{
			if (pBevelControl->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBevelContrast), 
				(NodeAttribute **)&pAttrContrast))
			{
				if (pAttrContrast)
				{
					if (pAttrContrast->IsADefaultAttr())
					{
						ALLOC_WITH_FAIL(pAttrContrast, 
							new AttrBevelContrast(pBevelControl, FIRSTCHILD), this);
						pAttrContrast->Value.m_Contrast = pBevelControl->m_Contrast;
						
						if (HideNodeAction::Init(this, GetUndoActionList(), (Node *)pAttrContrast,
							FALSE, (Action **)&pAction, FALSE) != AC_OK)
						{
							FailAndExecute();
							End();
							BevelNodeList.DeleteAll();
							return;				
						}			
					}
				}
			}
		}
		
		// localise out all the attributes
		if (!DoLocaliseCommonAttributes(pBevelControl, TRUE, TRUE))
		{
			FailAndExecute();
			End();
			BevelNodeList.DeleteAll();
			return;
		}

		BOOL bGenerateNode = FALSE;

		pCurrentNode = pBevelControl->FindFirstDepthFirst();

		while (pCurrentNode)
		{
			pNextNode = pCurrentNode->FindNextDepthFirst(pBevelControl);
			
			// hide the original attribute, and replace it with the new one,
			// for undo purposes
			if (pCurrentNode->IS_KIND_OF(AttrBevelIndent) && pBevelInfo->m_bBevelIndentChanged)
			{
				bGenerateNode = TRUE;
				ALLOC_WITH_FAIL(pAttrIndent, (new AttrBevelIndent()), this);

				// get the new width out of the parameter
				MILLIPOINT Width = abs(pBevelInfo->m_Indent);

				if (pBevelInfo->m_bBevelDirectionChanged)
				{
					// direction changed ! no need to check the direction of the original
					// attribute
					if (pBevelInfo->m_bOuter)
					{
						Width = -Width;
					}
				}
				else
				{
					// if the original attribute is an outer bevel, make the width negative
					if (((AttrBevelIndent *)pCurrentNode)->Value.m_Indent < 0)
					{
						Width = -Width;
					}
				}

				if (Width > 0)
				{
					// check maximum indent of outer bevels
					MILLIPOINT MaxValue = 
						ContourNodePathProcessor::GetMaxInnerContourWidth(pBevelControl);

					if (Width > MaxValue)
						Width = MaxValue;
				}

				// set the attribute value
				((AttrBevelIndent *)pAttrIndent)->Value.m_Indent = Width;
				
				// hide the original node
				if (!DoHideNode(pCurrentNode, TRUE, &pNodeHidden))
				{
					FailAndExecute();
					End();
					BevelNodeList.DeleteAll();
					return;
				}

				// attach the new node
				pAttrIndent->AttachNode(pNodeHidden, NEXT);

				// add an hide node action so that undo works ok
				if (HideNodeAction::Init(this, GetUndoActionList(), (Node *)pAttrIndent,
								FALSE, (Action **)&pAction, FALSE) != AC_OK)
				{
					FailAndExecute();
					End();
					BevelNodeList.DeleteAll();
					return;				
				}

				m_NameID = _R(IDS_CHANGEBEVELINDENTOPNAME);
			} 
			else if (pCurrentNode->IS_KIND_OF(AttrBevelType) && pBevelInfo->m_bBevelTypeChanged)
			{
				ALLOC_WITH_FAIL(pAttrType, (new AttrBevelType()), this);
				
				((AttrBevelType *)pAttrType)->Value.m_Type = pBevelInfo->m_BevelType;

				// hide the original node
				if (!DoHideNode(pCurrentNode, TRUE, &pNodeHidden))
				{
					FailAndExecute();
					End();
					BevelNodeList.DeleteAll();
					return;
				}

				// attach the new node
				pAttrType->AttachNode(pNodeHidden, NEXT);

				// add an hide node action so that undo works ok
				if (HideNodeAction::Init(this, GetUndoActionList(), (Node *)pAttrType,
								FALSE, (Action **)&pAction, FALSE) != AC_OK)
				{
					FailAndExecute();
					End();
					BevelNodeList.DeleteAll();
					return;
				}

				m_NameID = _R(IDS_CHANGEBEVELTYPEOPNAME);
			}
			else if (pCurrentNode->IS_KIND_OF(AttrBevelIndent) && pBevelInfo->m_bBevelDirectionChanged)
			{
				bGenerateNode = TRUE;
				ALLOC_WITH_FAIL(pAttrIndent, (new AttrBevelIndent()), this);
				
				if (pBevelInfo->m_bOuter)
				{
					((AttrBevelIndent *)pAttrIndent)->Value.m_Indent = 
						-abs(((AttrBevelIndent *)pCurrentNode)->Value.m_Indent);
				}
				else
				{
					((AttrBevelIndent *)pAttrIndent)->Value.m_Indent = 
						abs(((AttrBevelIndent *)pCurrentNode)->Value.m_Indent);

					// check the maximum inner contour width
					MILLIPOINT MaxValue = 
						ContourNodePathProcessor::GetMaxInnerContourWidth(pBevelControl);

					if (((AttrBevelIndent *)pAttrIndent)->Value.m_Indent > MaxValue)
						((AttrBevelIndent *)pAttrIndent)->Value.m_Indent = MaxValue;
				}
				
				// hide the original node
				if (!DoHideNode(pCurrentNode, TRUE, &pNodeHidden))
				{
					FailAndExecute();
					End();
					BevelNodeList.DeleteAll();
					return;
				}

				// attach the new node
				pAttrIndent->AttachNode(pNodeHidden, NEXT);

				// add an hide node action so that undo works ok
				if (HideNodeAction::Init(this, GetUndoActionList(), (Node *)pAttrIndent,
								FALSE, (Action **)&pAction, FALSE) != AC_OK)
				{
					FailAndExecute();
					End();
					BevelNodeList.DeleteAll();
					return;
				}

				if (pBevelInfo->m_bOuter)
				{
					m_NameID = _R(IDS_CHANGEBEVELTOOUTEROPNAME);
				}
				else
				{
					m_NameID = _R(IDS_CHANGEBEVELTOINNEROPNAME);
				}
			} 			
			else if (pCurrentNode->IS_KIND_OF(AttrBevelContrast) && pBevelInfo->m_bBevelContrastChanged)
			{
				ALLOC_WITH_FAIL(pAttrContrast, (new AttrBevelContrast()), this);
				
				pAttrContrast->Value.m_Contrast = pBevelInfo->m_Contrast;

				// hide the original node
				if (!DoHideNode(pCurrentNode, TRUE, &pNodeHidden))
				{
					FailAndExecute();
					End();
					BevelNodeList.DeleteAll();
					return;
				}

				// attach the new node
				pAttrContrast->AttachNode(pNodeHidden, NEXT);

				// add an hide node action so that undo works ok
				if (HideNodeAction::Init(this, GetUndoActionList(), (Node *)pAttrContrast,
								FALSE, (Action **)&pAction, FALSE) != AC_OK)
				{
					FailAndExecute();
					End();
					BevelNodeList.DeleteAll();
					return;
				}

				m_NameID = _R(IDS_CHANGEBEVELCONTRASTOPNAME);
			}
			else if (pCurrentNode->IS_KIND_OF(AttrBevelLightAngle) && pBevelInfo->m_bBevelLightAngleChanged)
			{
				ALLOC_WITH_FAIL(pAttrLightAngle, (new AttrBevelLightAngle()), this);
				
				pAttrLightAngle->Value.m_LightAngle = (INT32)pBevelInfo->m_LightAngle;

				// hide the original node
				if (!DoHideNode(pCurrentNode, TRUE, &pNodeHidden))
				{
					FailAndExecute();
					End();
					BevelNodeList.DeleteAll();
					return;
				}

				// insert the new attribute node
				pAttrLightAngle->AttachNode(pNodeHidden, NEXT);

				// add an hide node action so that undo works ok
				if (HideNodeAction::Init(this, GetUndoActionList(), (Node *)pAttrLightAngle,
								FALSE, (Action **)&pAction, FALSE) != AC_OK)
				{
					FailAndExecute();
					End();
					BevelNodeList.DeleteAll();
					return;
				}

				m_NameID = _R(IDS_CHANGEBEVELLIGHTANGLEOPNAME);
			}
			else if (pCurrentNode->IS_KIND_OF(AttrBevelLightTilt) && pBevelInfo->m_bBevelLightTiltChanged)
			{
				ALLOC_WITH_FAIL(pAttrLightTilt, (new AttrBevelLightTilt()), this);
				
				pAttrLightTilt->Value.m_LightTilt = (INT32)pBevelInfo->m_Tilt;

				// hide the original node
				if (!DoHideNode(pCurrentNode, TRUE, &pNodeHidden))
				{
					FailAndExecute();
					End();
					BevelNodeList.DeleteAll();
					return;
				}

				// insert the new attribute node
				pAttrLightTilt->AttachNode(pNodeHidden, NEXT);
				pBevelControl->m_Tilt = pBevelInfo->m_Tilt;

				// add an hide node action so that undo works ok
				if (HideNodeAction::Init(this, GetUndoActionList(), (Node *)pAttrLightTilt,
								FALSE, (Action **)&pAction, FALSE) != AC_OK)
				{
					FailAndExecute();
					End();
					BevelNodeList.DeleteAll();
					return;
				}

				m_NameID = _R(IDS_CHANGEBEVELLIGHTTILTOPNAME);
			}

//			else if (pCurrentNode->IS_KIND_OF(NodeBevel))
//			{
//				((NodeBevel*)pCurrentNode)->InvalidateBoundingRect();
//			}


			pCurrentNode = pNextNode;
		}

		if (!DoFactorOutCommonChildAttributes(pBevelControl))
		{
			FailAndExecute();
			End();
			
			BevelNodeList.DeleteAll();
			return;
		}
		
		// regenerate the node - if necessary !
		if (bGenerateNode)
		{
			pBevelControl->RegenerateNode(this, FALSE, TRUE);

			// Ilan 31/5/00
			// Inform geom linked attrs of the change. Nb outside the normal AllowOp mechanism
			NodeAttribute* pNA = pBevelControl->FindFirstGeometryLinkedAttr();
			while(pNA)
			{
				pNA->LinkedNodeGeometryHasChanged(this);
				pNA = pNA->FindNextGeometryLinkedAttr();
			}
		}
		else
		{
			// doesn't regen path so GLA's aren't affected
			pBevelControl->RegenerateBevelBitmap(this, TRUE);
		}

		pBevelControl->InvalidateBoundingRect(TRUE);
		pBevelControl->ReleaseCached();
		
		// invalidate the region after
		if (!DoInvalidateRegion(pBevelControl->FindParentSpread(), 
			pBevelControl->GetBoundingRect(TRUE, FALSE)))
		{
			FailAndExecute();
			End();
			BevelNodeList.DeleteAll();
			return;
		}
	
		pNodeListItem= (NodeListItem *)BevelNodeList.GetNext(pNodeListItem);
	}

	// kick the sel range first
	GetApplication()->UpdateSelection();

	// if there are any bevel nodes present, then exit - the above will have taken
	// care of it
	if (!BevelNodeList.IsEmpty() && !pBevelInfo->m_bCreateNewBevels && !pBevelInfo->m_bBevelDirectionChanged)
	{
		ObjChangeFlags flgs(FALSE, FALSE, FALSE, FALSE);
		ObjChangeParam OP(OBJCHANGE_FINISHED, flgs, NULL, this, OBJCHANGE_CALLEDBYOP);

		pBevelControl->AllowOp(&OP);
		BevelNodeList.DeleteAll();
		End();
		return;
	}

	EffectsStack* pStack = GetApplication()->FindSelection()->GetEffectsStack();
	ENSURE(pStack, "Argh!");
	Range* pSelList = pStack->GetBaseLevelRange();
	ENSURE(pSelList, "Argh!");

	TextStory *pThisParentStory = NULL;

	// for the allow op mechanism
	ObjChangeFlags flgs(FALSE, FALSE, FALSE, FALSE);
	ObjChangeParam OP(OBJCHANGE_STARTING, flgs, NULL, this, OBJCHANGE_CALLEDBYOP);

	// call allow op on the selection as we need to make sure it was called on something
	// it will just make the extending stuff be called correctly at the end and return quickly here
	pSel->AllowOp(&OP);

	BOOL bHasABevel = FALSE;

	// apply bevel to all renderable nodes which don't have a bevel on them
	Node* pNode = pSelList->FindFirst();
	while (pNode)
	{
		// are there any bevel nodes under this node ?
		BevelTools::GetAllNodesUnderNode(pNode, &BevelNodeList, CC_RUNTIME_CLASS(NodeBevel));

		bHasABevel = !BevelNodeList.IsEmpty();

		if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk)) &&
			!pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevelController)) &&
			pNode->FindParent(CC_RUNTIME_CLASS(NodeBevelController)) == NULL &&
			pNode->AllowOp(&OP, FALSE) &&
			!bHasABevel)
		{
			// we don't have a bevel controller node so apply a bevel to it
			
			// now, is this node a text node but not a text story node ? Oh dear ! Not a text story node,
			// which means that the text is selected, so we need to select the text story node and
			// bevel this !
			if (pNode->IsKindOf(CC_RUNTIME_CLASS(TextLine)) ||
				pNode->IsKindOf(CC_RUNTIME_CLASS(TextChar)))
			{
				pThisParentStory = (TextStory *)pNode->FindParent(CC_RUNTIME_CLASS(TextStory));

				if (pThisParentStory)
				{
					// does the parent story already have a bevel ?
					if (!(pThisParentStory->FindParent(CC_RUNTIME_CLASS(NodeBevelController))) &&
						!pThisParentStory->IsNodeHidden())
					{
						DoSelectNode(pThisParentStory);
						ApplyBevelToNode(pThisParentStory, pBevelInfo);

						m_NameID = _R(IDS_CREATEBEVELOPNAME);
					}
				}	
				else
				{
					ERROR3("No parent story found for text line/characters");
				}
			}
			else
			{				
				if (pNode->AllowOp(&OP))
				{
					ApplyBevelToNode(pNode, pBevelInfo);

					m_NameID = _R(IDS_CREATEBEVELOPNAME);
				}
			}
		}
		else if(bHasABevel)
		{
			// Ok if we have a controller node here and it already has a bevel then we need to move it
			// to it correct possition in the tree!
			NodeListItem* pItem = (NodeListItem *)BevelNodeList.GetHead();
			NodeBevelController* pControl = NULL;
			NodeBevel* pBevel = NULL;

			while(pItem)
			{
				pControl = (NodeBevelController*)pItem->pNode;
				pBevel = pControl->GetBevelNode();

				if(pControl && pBevel)
				{
					if(pBevelInfo->m_bOuter)
					{
						Node* pBeginBevel = pControl->FindFirstChild(CC_RUNTIME_CLASS(NodeBevelBegin));
						
						if(pBeginBevel)
						{
							DoMoveNode(pBevel,pBeginBevel,NEXT);
						}
						else
						{
							DoMoveNode(pBevel,pControl,FIRSTCHILD);
						}
					}
					else
					{
						DoMoveNode(pBevel,pControl,LASTCHILD);
					}
				}

				pItem = (NodeListItem *)BevelNodeList.GetNext(pItem);
			}
		}
		BevelNodeList.DeleteAll();

		pNode = pSelList->FindNext(pNode);
	}

	// rid ourselves of the selection list
	delete pSelList;

	GetApplication()->UpdateSelection();

	End();
}

/********************************************************************************************

>	void OpCreateBevel::ApplyBevelToNode(Node * pNode, OpParam* pParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/98															
	Returns:	-
	Purpose:	Creates/adjusts a bevel on the given node

********************************************************************************************/

void OpCreateBevel::ApplyBevelToNode(Node * pNode, BevelInfo* pBevelInfo)
{
	if (!pNode || !pBevelInfo)
		return;

	// can't double-bevel things !
	// or bevel things which require their parent
	if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevelController)) ||
		pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevel)) ||
		pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevelBegin)) ||
		pNode->NeedsParent(pNode->FindParent()))
	{
		return;
	}

	// deselect the node first
	if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk)))
		DoDeselectNode((NodeRenderableInk *)pNode);

	// Create a new NodeBevelController and insert it into the tree
	BOOL ok = TRUE;

	NodeBevelController * pBevelControl = NULL; 
//	NodeBevelController * pNewBevelControl = NULL; 
	NodeBevelBegin * pBevelBegin = NULL;
	NodeBevel	   * pBevel		 = NULL;
	Node * pThisNode = NULL;
	Node * pNextNode = NULL;

	ALLOC_WITH_FAIL(pBevelControl, (new NodeBevelController()), this);
	ok = (pBevelControl != NULL);

	// Create a new NodeBevel, apply default attributes and insert it into the tree
	if (ok)
	{
		ALLOC_WITH_FAIL(pBevel, (new NodeBevel()), this);
		ok = (pBevel != NULL);
	}

	if (ok)
	{
		ALLOC_WITH_FAIL(pBevelBegin, (new NodeBevelBegin()), this);
		ok = (pBevelBegin != NULL);
	}

	if(ok)
	{
		pBevel->m_Indent = pBevelInfo->m_Indent;
		pBevel->m_LightAngle = pBevelInfo->m_LightAngle;
		pBevel->m_BevelType = pBevelInfo->m_BevelType;
		pBevel->m_bOuter = pBevelInfo->m_bOuter;
		
		pBevelControl->m_Indent = pBevelInfo->m_Indent;
		pBevelControl->m_LightAngle = pBevelInfo->m_LightAngle;
		pBevelControl->m_BevelType = pBevelInfo->m_BevelType;
		pBevelControl->m_bOuter = pBevelInfo->m_bOuter;
	}

	// check for a shadow controller node
	if (!pNode->IsKindOf(CC_RUNTIME_CLASS(NodeShadowController)))
	{
		if (ok)	ok = DoInsertNewNode(pBevelControl, pNode, NEXT, TRUE, FALSE, FALSE, TRUE);

		if (ok)	ok = DoMoveNode(pNode, pBevelControl, FIRSTCHILD);

		if (ok)	ok = DoSelectNode((NodeRenderableInk *)pNode);
	}
	else
	{
		if (ok)
		{
			ok = DoInsertNewNode(pBevelControl, pNode, LASTCHILD, TRUE, FALSE, FALSE, TRUE);
		
			// now, move all the children of the shadow controller to be children of the bevel
			pThisNode = pNode->FindFirstChild();

			while (pThisNode)
			{
				pNextNode = pThisNode->FindNext();

				if (!pThisNode->IsKindOf(CC_RUNTIME_CLASS(NodeShadow)) && !pThisNode->IsNodeHidden() &&
					!pThisNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevelController)))
				{
					if (!pThisNode->IsKindOf(CC_RUNTIME_CLASS(NodeAttribute)))
					{
						if (ok) ok = DoMoveNode(pThisNode, pBevelControl, FIRSTCHILD);
						if (ok) ok = DoSelectNode((NodeRenderableInk *)pThisNode);
					}
				}

				pThisNode = pNextNode;
			}
		}
	}

	// insert the bevel begin node
	if (ok)
		ok = DoInsertNewNode(pBevelBegin, pBevelControl, FIRSTCHILD, TRUE, FALSE, FALSE, TRUE);

	// insert the node, and optimise attributes
	if (ok)
	{
//		if(pBevelInfo->m_bOuter)
			ok = DoInsertNewNode(pBevel, pBevelBegin, NEXT, TRUE, FALSE, FALSE, TRUE);
//		else
//			ok = DoInsertNewNode(pBevel, pBevelControl, LASTCHILD, TRUE, FALSE, FALSE, TRUE);
	}
	
	NodeAttribute * pFillAttr = NULL;
	NodeAttribute * pTranspFillAttr = NULL;
	NodeAttribute * pFillMapping = NULL;
	NodeAttribute * pFillEffect = NULL;
	NodeAttribute * pTranspFillMapping = NULL;
	NodeAttribute * pJoinType  = NULL;

	if(ok)
	{
		// get the first renderable node under the bevel controller node
		Node * pStepNode = pBevelControl->FindFirstDepthFirst();

		while (pStepNode && pStepNode != (Node *)pBevelControl)
		{
			if (pStepNode->IsAnObject() && !pStepNode->IsCompound() && !pStepNode->NeedsParent(NULL))
			{
				if (!pStepNode->IS_KIND_OF(NodeMouldPath))
				{
					if (pStepNode->IsNodePath())
					{
						// check for path being in a text story
						if (pStepNode->FindParent())
						{
							if (!pStepNode->FindParent()->IS_KIND_OF(TextStory))
							{
								break;
							}
						}
					}
					else if(pStepNode->IsABitmap())
					{
						// Ok we need to convert this bitmap into a NodePath and replace it in the tree!
						BecomeA BecomeAReplace(BECOMEA_REPLACE, CC_RUNTIME_CLASS(NodePath), this, FALSE);
						pStepNode->DoBecomeA(&BecomeAReplace);

						// Now reset the search so that it can find the new Converted bitmap path node!
						pStepNode = pBevelControl->FindFirstChildInk();
					}
					else
					{				
						break;
					}
				}
			}

			pStepNode = pStepNode->FindNextDepthFirst(pBevelControl);
		}

		if (pStepNode && pStepNode != (Node *)pBevelControl)
		{
			((NodeRenderableInk *)pStepNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillGeometry),&pFillAttr);
			((NodeRenderableInk *)pStepNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTranspFillGeometry),&pTranspFillAttr);
			((NodeRenderableInk *)pStepNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillMapping),&pFillMapping);
			((NodeRenderableInk *)pStepNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillEffect),&pFillEffect);
			((NodeRenderableInk *)pStepNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTranspFillMapping),&pTranspFillMapping);
			((NodeRenderableInk *)pStepNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrJoinType),&pJoinType);
		}

		if (pFillAttr)
			pFillAttr->CopyNode(pBevel, FIRSTCHILD);

		if (pTranspFillAttr)
			pTranspFillAttr->CopyNode(pBevel, FIRSTCHILD);

		if (pFillMapping)
			pFillMapping->CopyNode(pBevel, FIRSTCHILD);

		if (pFillEffect)
			pFillEffect->CopyNode(pBevel, FIRSTCHILD);

		if (pTranspFillMapping)
			pTranspFillMapping->CopyNode(pBevel, FIRSTCHILD);

		if (pJoinType)
			pJoinType->CopyNode(pBevel, FIRSTCHILD);
	}
	
	if(ok && !pBevelInfo->m_bOuter)
		pBevel->MoveNode(pBevelControl,LASTCHILD);

	// factor out the child attributes
	if (ok)	ok = DoFactorOutCommonChildAttributes(pBevelControl);

	if (!ok) FailAndExecute();

	// invalidate the region
	DocRect InvalidBevelRect = pBevelControl->GetBoundingRect(TRUE, FALSE);
	pBevelControl->ReleaseCached(TRUE, FALSE, TRUE, TRUE);
	
	if (pBevelInfo->m_bOuter)
	{
		InvalidBevelRect.lo.x -= pBevelInfo->m_Indent;
		InvalidBevelRect.lo.y -= pBevelInfo->m_Indent;
		InvalidBevelRect.hi.x += pBevelInfo->m_Indent;
		InvalidBevelRect.hi.y += pBevelInfo->m_Indent;
	}
	
	Spread * pSpread = Document::GetSelectedSpread();

	if (pSpread)
		DoInvalidateRegion(pSpread, InvalidBevelRect);	

	// sort out the attributes
	Document * pDoc = Document::GetCurrent();

	if (pDoc)
	{
		// apply the current bevel attributes
		AttrBevelType * pType = NULL;
		AttrBevelLightAngle * pLightAngle = NULL;
		AttrBevelContrast * pContrast = NULL;
		AttrBevelIndent * pIndent = NULL;
		AttrBevelLightTilt * pTilt = NULL;

		ALLOC_WITH_FAIL(pType, new AttrBevelType(pBevelControl, FIRSTCHILD), this);
		ALLOC_WITH_FAIL(pLightAngle, new AttrBevelLightAngle(pBevelControl, FIRSTCHILD), this);
		ALLOC_WITH_FAIL(pContrast, new AttrBevelContrast(pBevelControl, FIRSTCHILD), this);
		ALLOC_WITH_FAIL(pIndent, new AttrBevelIndent(pBevelControl, FIRSTCHILD), this);
		ALLOC_WITH_FAIL(pTilt, new AttrBevelLightTilt(pBevelControl, FIRSTCHILD), this);

		if (pType)
			pType->Value.m_Type = pBevelInfo->m_BevelType;

		if (pLightAngle)
			pLightAngle->Value.m_LightAngle = (INT32)pBevelInfo->m_LightAngle;

		if (pContrast)
			pContrast->Value.m_Contrast = pBevelInfo->m_Contrast;

		if (pIndent)
			pIndent->Value.m_Indent = pBevelInfo->m_Indent;

		if (pTilt)
			pTilt->Value.m_LightTilt = (INT32)pBevelInfo->m_Tilt;

		if (pBevelInfo->m_bOuter)
		{
			pIndent->Value.m_Indent = -pBevelInfo->m_Indent;
		}
		else
		{
			// check the width against the maximum allowed on inner bevels
			MILLIPOINT MaxWidth = ContourNodePathProcessor::GetMaxInnerContourWidth(pBevelControl);

			if (pBevelInfo->m_Indent < MaxWidth)
				pIndent->Value.m_Indent = pBevelInfo->m_Indent;
			else
				pIndent->Value.m_Indent = MaxWidth;
		}
	}

	// tell the shadow node to regen
	NodeShadow * pShadow = NULL;
	NodeShadowController * pShadowCont = NULL;

	// generate the bevel
	pBevelControl->CreateBevel();	

	SliceHelper::AddNamesToController(this,pBevelControl);

	pBevelControl->SetSelected(TRUE);
	GetApplication()->UpdateSelection();

	pBevel->InvalidateBoundingRect();

	// tell all nodes above the bevel controller node of the change
	ObjChangeFlags flgs(FALSE, FALSE, FALSE, FALSE);
	flgs.RegenerateNode = TRUE;
	ObjChangeParam OP(OBJCHANGE_FINISHED, flgs, pNode, this, OBJCHANGE_CALLEDBYOP);
				
//	Node * pNodeParent = pNode->FindParent();

	// also check for blends needing to be re-initialised
	NodeBlender * pBlender = NULL;
	NodeBlend * pBlend = NULL;
	NodeShadowController* pShadController = NULL;

	if (!IS_A (pBevelControl->FindParent (), NodeShadowController))
	{
		pBlender = (NodeBlender *)pBevelControl->FindNext(CC_RUNTIME_CLASS(NodeBlender));
	}
	else
	{
		pShadController = (NodeShadowController*) pBevelControl->FindParent ();
		pBlender = (NodeBlender *)pBevelControl->FindParent ()->FindNext(CC_RUNTIME_CLASS(NodeBlender));
	}

	if (pBlender)
	{
		pBlend = (NodeBlend *)pBlender->FindParent();

		if (InitBlendAction::InitOnBlender(this, GetUndoActionList(), pBlender, TRUE, TRUE) != AC_OK)
			ERROR2RAW("Couldn't Initialise blend action");

		// re-insert this action at the head of the list
		if (!pShadController)
			pBlender->Reinit(pBevelControl, pBlender->GetNodeEnd(), FALSE);
		else
			pBlender->Reinit(pShadController, pBlender->GetNodeEnd(), FALSE);
	}
	else 
	{
		// can't have both ends changed !
		pBlender = (NodeBlender *)pBevelControl->FindPrevious(CC_RUNTIME_CLASS(NodeBlender));

		if (pBlender)
		{
			pBlend = (NodeBlend *)pBlender->FindParent();

			if (InitBlendAction::InitOnBlender(this, GetUndoActionList(), pBlender, TRUE, TRUE) != AC_OK)
				ERROR2RAW("Couldn't Initialise blend action");
		
			if (!pShadController)
				pBlender->Reinit(pBlender->GetNodeStart(), pBevelControl, FALSE);
			else
				pBlender->Reinit(pBlender->GetNodeStart(), pShadController, FALSE);
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
				if (ptrNode->GetNodeStart () == pNode)
				{
					if (InitBlendAction::InitOnBlender(this, GetUndoActionList(), ptrNode, TRUE, TRUE) != AC_OK)
						ERROR2RAW("Couldn't Initialise blend action");

					if (pBevelControl)
					{
						if (!pShadController)
							ptrNode->Reinit(pBevelControl, NULL, FALSE);
						else
							ptrNode->Reinit(pShadController, NULL, FALSE);
					}
				}
				if (ptrNode->GetNodeEnd () == pNode)
				{
					if (InitBlendAction::InitOnBlender(this, GetUndoActionList(),  ptrNode, TRUE, TRUE) != AC_OK)
						ERROR2RAW("Couldn't Initialise blend action");

					if (pBevelControl)
					{
						if (!pShadController)
							ptrNode->Reinit(NULL, pBevelControl, FALSE);
						else
							ptrNode->Reinit(NULL, pShadController, FALSE);
					}
				}
			}

			ptrNode = ptrBlend->FindNextBlender (ptrNode);

			if (!ptrNode)
				done = TRUE;
		}
	}

	if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeShadowController)))
	{
		pShadowCont = (NodeShadowController *)pNode;

		pShadow = pShadowCont->GetShadow();

		pShadow->InvalidateBoundingRect();
		pShadow->GetParentController()->InvalidateBoundingRect();
		DoInvalidateNodeRegion(pShadow, TRUE);
		DoInvalidateNodeRegion(pShadow->GetParentController(), TRUE);

		ChangePenumbraSizeAction * pAction = NULL;

		ChangePenumbraSizeAction::Init(this, 
										 GetUndoActionList(),
										 pShadowCont,
										 pShadowCont->GetPenumbraWidth(),
										 &pAction);										
	}
}

/********************************************************************************************

>	NodeAttribute * OpCreateBevel::DoDepthSearchForFill(Node * pNode)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	18/12/98																		
	Returns:	The fill attribute found, NULL if non
	Purpose:	Searches the subtree of pNode for the first fill attribute

********************************************************************************************/

NodeAttribute * OpCreateBevel::DoDepthSearchForFill(Node * pNode)
{
	if (!pNode)
		return NULL;

	while (pNode)
	{
		if (pNode->IsKindOf(CC_RUNTIME_CLASS(AttrFillGeometry)) &&
			!pNode->IsKindOf(CC_RUNTIME_CLASS(AttrStrokeColour)) &&
			!pNode->IsKindOf(CC_RUNTIME_CLASS(AttrStrokeTransp)) &&
			!pNode->IsKindOf(CC_RUNTIME_CLASS(AttrTranspFillGeometry)) &&
			!pNode->IsKindOf(CC_RUNTIME_CLASS(AttrValueChange)) &&
			!pNode->IsNodeHidden())
		{
			return (NodeAttribute *)pNode;	
		}
	
		// do the child of this node first
		Node * pChild = pNode->FindFirstChild();

		NodeAttribute * pAttr = DoDepthSearchForFill(pChild);

		if (pAttr)
			return pAttr;

		pNode = pNode->FindNext();
	}

	return NULL;
}

/********************************************************************************************

>	void OpCreateBevel::ConvertToPaths(Node * pNode)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	18/12/98																		
	Returns:	
	Purpose:	Converts all quick shape nodes (recursively) to paths

********************************************************************************************/

void OpCreateBevel::ConvertToPaths(Node * pNode)
{
	/*
	BecomeA BAGroup(BECOMEA_REPLACE, CC_RUNTIME_CLASS(NodeGroup), this, TRUE);

	Node * pChildNode = NULL;
	Node * pNextNode  = NULL;

	// convert all nodes under the main node into being regular shapes
	while (pNode)
	{
		pChildNode = pNode->FindFirstChild();

		if (pChildNode)
		{
			ConvertToPaths(pChildNode);
		}

		pNextNode = pNode->FindNext();

		if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk)))
		{
			pNode->DoBecomeA(&BAPath);
			TRACEUSER( "DavidM", _T("Found regular shape !\n"));
		}
	
		pNode = pNextNode;
	}
	*/
}

/********************************************************************************************

>	BOOL OpCreateBevel::Declare()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94																		
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpCreateBevel::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_CREATEBEVELOP),
								CC_RUNTIME_CLASS(OpCreateBevel), 
								OPTOKEN_CREATEBEVEL,
								OpCreateBevel::GetState));

}



/********************************************************************************************

>	OpState OpCreateBevel::GetState(String_256* Description, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Outputs:	-
	Returns:	Ungreyed, Unticked
	Purpose:	Find out the state of the new regular shape at the specific time

********************************************************************************************/

OpState OpCreateBevel::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;

	// not ALLOCed with fail otherwise this'll fall over all the time !
	OpCreateBevel * pBevelOp = new OpCreateBevel;

	if (!pBevelOp)
	{
		ERROR3("Cannot create shadow op");
		Blobby.Greyed = TRUE;
		return Blobby;
	}

	ObjChangeFlags flgs(FALSE, FALSE, FALSE, FALSE);
	ObjChangeParam OP(OBJCHANGE_STARTING, flgs, NULL, pBevelOp, OBJCHANGE_CALLEDBYOP);

	if (GetApplication()->FindSelection()->AllowOp(&OP, FALSE))
	{				
		Blobby.Greyed = FALSE;
	}
	else
	{
		Blobby.Greyed = TRUE;
		delete pBevelOp;
		return Blobby;
	}

	delete pBevelOp;

	return Blobby;
}


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//////////	OpRemoveBevel class
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************
>	OpRemoveBevel::OpRemoveBevel()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/96
	Purpose:	Constructor.
********************************************************************************************/

OpRemoveBevel::OpRemoveBevel()
{
}



/********************************************************************************************
>	OpRemoveBevel::~OpRemoveBevel()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/96
	Purpose:	Destructor.
********************************************************************************************/

OpRemoveBevel::~OpRemoveBevel()
{
}

/********************************************************************************************
>	void OpRemoveBevel::GetOpName(String_256 * pString)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> & Olivier
	Created:	21/11/96
	Purpose:	Gets the name of this operation - dependent on what it did !
********************************************************************************************/

void OpRemoveBevel::GetOpName(String_256 * pString)
{
	if (pString)
		pString->Load(_R(IDS_REMOVEBEVELOPNAME));
}




/********************************************************************************************
>	virtual void OpRemoveBevel::Do(OpDescriptor *pOpDesc)
	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/96
	Inputs:		pOpDesc - OpDescriptor - unused
	Outputs:	-
	Returns:	-
	Purpose:	The do function.  Applys a Bevel to the selection in the current document
********************************************************************************************/

void OpRemoveBevel::Do(OpDescriptor *pOpDesc)
{
	// have we a bevel controller node in the selection ?
	if (!NodeBevelController::SelectionHasBevelNode())
		return;
	
	// Prepare an ObjChangeParam so we can mark which nodes will allow this op to happen to them
	ObjChangeFlags cFlags;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

	// Get the selection 
	Range Sel(*(GetApplication()->FindSelection()));
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.SetRangeControl(rg);

	BOOL ok = TRUE;

	// Start the SelectionOp
	DoStartSelOp(TRUE, TRUE, TRUE, TRUE);

	// invalidate the whole range
	ok = DoInvalidateNodesRegions(Sel,
							 TRUE);

	Node * pNode = NULL;
	Node * pNextNode = NULL;
//	Node * pAttribNode = NULL;

	NodeHidden * pNodeHidden = NULL;

	List AttribList;
	NodeListItem * pItem = NULL;
	NodeListItem * pNext = NULL;

	DocRect dr;

	Spread * pSpread = Document::GetSelectedSpread();

//	INT32 count = 0;

	List BevelControllerList;

	// build a list of the selected bevel nodes
	BevelTools::BuildListOfSelectedNodes(&BevelControllerList, 
		CC_RUNTIME_CLASS(NodeBevelController));

	// go through this list, hiding them
	pItem = (NodeListItem *)BevelControllerList.GetHead();

	NodeShadowController * pShadControl = NULL;

	NodeBlend * pBlend = NULL;
	NodeBlender * pBlender = NULL;

	BOOL bBlendBefore = FALSE;
	BOOL bBlendAfter  = FALSE;

	// for blends
	NodeRenderableInk *pRenderableNode = NULL;

	while (pItem)
	{
		pNext = (NodeListItem *)BevelControllerList.GetNext(pItem);

		NodeBevelController* pControl = (NodeBevelController*) pItem->pNode;

		if (pControl)
		{
			SliceHelper::RemoveNamesFromController(this,pControl);

			// are we part of a blend ? if so, make a record of the blender object
			// and whether we're before or after it
			pBlender = (NodeBlender *)pItem->pNode->FindNext(CC_RUNTIME_CLASS(NodeBlender));

			bBlendBefore = FALSE;
			bBlendAfter = FALSE;

			if (pBlender)
			{
				// we're before it !
				bBlendBefore = TRUE;

				pBlend = (NodeBlend *)pBlender->FindParent();
			}
			else
			{
				// try the blend after
				pBlender = (NodeBlender *)pItem->pNode->FindPrevious(CC_RUNTIME_CLASS(NodeBlender));

				if (pBlender)
				{
					bBlendAfter = TRUE;
					pBlend = (NodeBlend *)pBlender->FindParent();
				}
			}
			
			// remove the attributes from the node
			if (ok) ok = DoRemoveAttrTypeFromSubtree(pItem->pNode, CC_RUNTIME_CLASS(AttrBevelIndent));
			if (ok) ok = DoRemoveAttrTypeFromSubtree(pItem->pNode, CC_RUNTIME_CLASS(AttrBevelLightAngle));
			if (ok) ok = DoRemoveAttrTypeFromSubtree(pItem->pNode, CC_RUNTIME_CLASS(AttrBevelType));
			if (ok) ok = DoRemoveAttrTypeFromSubtree(pItem->pNode, CC_RUNTIME_CLASS(AttrBevelContrast));
			
			// invalidate the rect
			Node * pParent = pItem->pNode->FindParent();
	
			if (!pItem->pNode->IsNodeHidden() && pParent)
			{
				if (ok) ok = DoInvalidateNodeRegion((NodeRenderableInk *)pItem->pNode, TRUE);			
				
				// localise the attributes
				if (!DoLocaliseCommonAttributes((NodeRenderableInk *)pItem->pNode, TRUE))
				{
					FailAndExecute();
					End();
					BevelControllerList.DeleteAll();
					
					return;
				}
				
				// now, move non-bevel nodes to top level
				// go from back to front to maintain the rendering order
				pNode = pItem->pNode->FindLastChild();
				
				while (pNode)
				{
					pNextNode = pNode->FindPrevious();
					
					if (!pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevel)) &&
						!pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevelBegin)) &&
						!pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevelController)) &&
						!pNode->IsNodeHidden())
					{
						if (pNode->IsAnObject() && !pNode->IsAnAttribute())
						{
							if (ok)
								ok = DoMoveNode(pNode, pItem->pNode, NEXT);
							
							if (ok)
								ok = DoSelectNode((NodeRenderableInk *)pNode);
						}
						else if (pNode->IsAnAttribute())
						{
							// hide attributes
							// DoHideNode(pNode, FALSE, &pNodeHidden);

							if (pNode->IsAnObjectName())
								DoHideNode(pNode, FALSE);
						}
					}
					
					pNode = pNextNode;
				}
				
				// finally, deselect & hide the bevel controller node
				if (ok)
					ok = DoDeselectNode((NodeRenderableInk *)pItem->pNode);
				
				// need to tell any shadows to regen
				pShadControl = 
					(NodeShadowController *)pItem->pNode->FindParent(CC_RUNTIME_CLASS(NodeShadowController));

				if (ok) 
				{
					pItem->pNode->AllowOp(&ObjChange);
					ok = DoHideNode((NodeBevelController *)pItem->pNode, TRUE, &pNodeHidden);
				}

				if (pParent)
				{
					if (pParent->IsAnObject())
					{
						if (ok) 
							ok = DoFactorOutCommonChildAttributes((NodeRenderableInk *)pParent, TRUE);
					}
				}

				// do the blend inform - i.e. inform them that their objects have changed
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
/*				
				// inform all parents of the change
				while (pParent)
				{
					// set up the object change parameters
					ObjChangeFlags flgs(FALSE, FALSE, FALSE, TRUE);
					flgs.RegenerateNode = TRUE;
					ObjChangeParam OP(OBJCHANGE_FINISHED, flgs, NULL, this, OBJCHANGE_CALLEDBYOP);

					pParent->OnChildChange(&OP);

					pParent = pParent->FindParent();
				}	
*/			}
		}

		// move onto the next one
		pItem = pNext;
	}

	BevelControllerList.DeleteAll();

	if (!ok)
	{
		FailAndExecute();
	}

	ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
	UpdateChangedNodes(&ObjChange, pSpread);

	GetApplication()->UpdateSelection();
	End();	
}


/********************************************************************************************

>	BOOL OpRemoveBevel::Declare()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/96
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpRemoveBevel::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_REMOVEBEVELOP),
								CC_RUNTIME_CLASS(OpRemoveBevel), 
								OPTOKEN_REMOVEBEVEL,
								OpRemoveBevel::GetState));

}


/********************************************************************************************

>	OpState OpRemoveBevel::GetState(String_256* Description, OpDescriptor*)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/96
	Outputs:	-
	Returns:	Ungreyed, Unticked
	Purpose:	Find out the state of the new regular shape at the specific time

********************************************************************************************/

OpState OpRemoveBevel::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}


////////////////////////////////////////////////////////////////////
// RegenerateBevelAction class implementation
/********************************************************************************************

>	RegenerateBevelAction::RegenerateBevelAction()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

RegenerateBevelAction::RegenerateBevelAction()
{
	m_pNodes = NULL;
	m_bCache = FALSE;
}


/********************************************************************************************

>	ActionCode RegenerateBevelAction::Init( 	Operation* 	pOp,
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

ActionCode RegenerateBevelAction::Init(Operation* pOp,
										ActionList* pActionList,
										List * pNodes,
										RegenerateBevelAction** ppNewAction,
										BOOL bCache)
{

	UINT32 ActSize = sizeof(RegenerateBevelAction);

	ERROR2IF(!pOp->IS_KIND_OF(UndoableOperation), AC_FAIL, "Operation isn't undoable");
	
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(RegenerateBevelAction),(Action**)ppNewAction);

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

	if (Ac != AC_FAIL)
	{
		while (pItem)
		{
			if (pItem->pNode)
			{
				if (pItem->pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevel)))
				{
					NodeBevel* pBevel = (NodeBevel*)pItem->pNode;
					if (pView && bCache)
					{
						ASSERT(pBevel->FindParent());
						GetApplication()->AddNodeToRegenList(pBevel->FindParent());
						if (pBevel->FindParent()->IsBounded())
							((NodeRenderableBounded*)pBevel->FindParent())->InvalidateBoundingRect(TRUE);
					}
					else
					{
						if (pBevel->FindParent(CC_RUNTIME_CLASS(Spread)))
						{
							((NodeBevelController*)pBevel->GetParentController())->RegenerateNode(NULL, FALSE);
						}
					}
				}
			}

			pItem = (NodeListItem *)pNodes->GetNext(pItem);
		}
	}

	// move my action to the head of the list
	pActionList->RemoveItem(*ppNewAction);
	pActionList->AddHead(*ppNewAction);

	return Ac;
}

/********************************************************************************************

>	ActionCode RegenerateBevelAction::Execute();

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/99
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  Causes a regen of all bevels nodes in the action's list
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode RegenerateBevelAction::Execute()
{
	ActionCode Act;
	RegenerateBevelAction* pAction;

	Act = RegenerateBevelAction::Init(	pOperation, 
										pOppositeActLst,
										m_pNodes,
										&pAction,
										m_bCache);

	if (Act != AC_FAIL)
	{
	}

	return Act;
}

RegenerateBevelAction::~RegenerateBevelAction()
{
	if (m_pNodes)
	{
		m_pNodes->DeleteAll();
		delete m_pNodes;
	}
}

////////////////////////////////////////////////////////////////////
// RegenerateBevelBitmapAction class implementation
/********************************************************************************************

>	RegenerateBevelBitmapAction::RegenerateBevelBitmapAction()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

RegenerateBevelBitmapAction::RegenerateBevelBitmapAction()
{
	m_pNodes = NULL;
}


/********************************************************************************************

>	ActionCode RegenerateBevelBitmapAction::Init( 	Operation* 	pOp,
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



ActionCode RegenerateBevelBitmapAction::Init(Operation* pOp,
										ActionList* pActionList,
										List * pNodes,
										RegenerateBevelBitmapAction** ppNewAction)
{
	UINT32 ActSize = sizeof(RegenerateBevelBitmapAction);
	
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(RegenerateBevelBitmapAction),
		(Action**)ppNewAction);

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

	pItem = (NodeListItem *)pNodes->GetHead();

//	DocView * pView = DocView::GetCurrent();

	if (Ac != AC_FAIL)
	{
		while (pItem)
		{
			if (pItem->pNode)
			{
				if (pItem->pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevel)))
				{
					NodeBevel* pBevel = (NodeBevel*)pItem->pNode;
					NodeBevelController* pControl = (NodeBevelController*)pBevel->GetParentController();
	
					if (pControl)
					{
						pControl->RegenerateBevelBitmap(NULL, TRUE);
					}
				}
			}

			pItem = (NodeListItem *)pNodes->GetNext(pItem);
		}
	}

	// move my action to the head of the list
	pActionList->RemoveItem(*ppNewAction);
	pActionList->AddHead(*ppNewAction);

	return Ac;
}

/********************************************************************************************

>	ActionCode RegenerateBevelBitmapAction::Execute();

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/99
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  Causes a regen of all bevels nodes in the action's list
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode RegenerateBevelBitmapAction::Execute()
{
	ActionCode Act;
	RegenerateBevelBitmapAction* pAction;

	Act = RegenerateBevelBitmapAction::Init(	pOperation, 
										pOppositeActLst,
										m_pNodes,
										&pAction);

	if (Act != AC_FAIL)
	{
	}

	return Act;
}

RegenerateBevelBitmapAction::~RegenerateBevelBitmapAction()
{
	if (m_pNodes)
	{
		m_pNodes->DeleteAll();
		delete m_pNodes;
	}
}


////////////////////////////////////////////////////////////////////
// ChangeLightAnglesAction class implementation
/********************************************************************************************

>	ChangeLightAnglesAction::ChangeLightAnglesAction()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ChangeLightAnglesAction::ChangeLightAnglesAction()
{
}


/********************************************************************************************

>	ActionCode ChangeLightAnglesAction::( Operation* pOp,
										  ActionList* pActionList,
										  List * pNodes,
										  INT32 Angle,
										  INT32 OldAngle,
										  ChangeLightAnglesAction** NewAction);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pNodes			= ptr to node list containing the bevel controller nodes to be 
								  addressed
				Angle			= the new light angle
				OldAngle		= the old light angle
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



ActionCode ChangeLightAnglesAction::Init( Operation* pOp,
										  ActionList* pActionList,
										  List * pNodes,
										  INT32 NewAngle, 
										  INT32 OldAngle,
										  ChangeLightAnglesAction** NewAction)
{
	if (!pOp)
		return AC_FAIL;

	if (!pOp->IsKindOf(CC_RUNTIME_CLASS(UndoableOperation)))
		return AC_FAIL;	
	
	UINT32 ActSize = sizeof(ChangeLightAnglesAction);
	
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(ChangeLightAnglesAction),(Action**)NewAction);

	if (Ac != AC_OK)
		return Ac;

//	UndoableOperation * pUndoOp = (UndoableOperation *)pOp;

	// run through the node list, making copies of all the affected nodes
	NodeListItem * pItem = NULL;
	NodeListItem * pNewItem = NULL;

	pItem = (NodeListItem *)pNodes->GetHead();

	AttrBevelLightAngle *pAttr = NULL;
	
	// store the old angle setting
	(*NewAction)->m_OldAngle = (INT32)OldAngle;
	(*NewAction)->m_NewAngle = (INT32)NewAngle;

	List AttrList;

//	NodeListItem * pAttrItem = NULL;

//	Node * pLastNode = NULL;
	NodeBevelController * pBevelNode = NULL;
	AttrBevelLightAngle * pAttrNode = NULL;

	DocView * pView = DocView::GetCurrent();
	
	while (pItem)
	{
		if (pItem->pNode)
		{
			if (pItem->pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevelController)))
			{
				// first, add this to the action's list of nodes
				pBevelNode = (NodeBevelController *)pItem->pNode;

				ALLOC_WITH_FAIL(pNewItem, new NodeListItem, pOp);
				pNewItem->pNode = pItem->pNode;
				(*NewAction)->m_NodeList.AddTail(pNewItem);

				// find out if a light angle attribute exists under this controller
				// node
				if (pBevelNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBevelLightAngle),
					(NodeAttribute **)(&pAttrNode)))
				{
					if (!pAttrNode->IsADefaultAttr())
					{
						// if it exists then
						// just set its value
						pAttrNode->Value.m_LightAngle = NewAngle;
					}
					else
					{
						// well, we have a non-factored out one so add one in based on the 
						// node's settings
						ALLOC_WITH_FAIL(pAttr, new AttrBevelLightAngle(pBevelNode, FIRSTCHILD), pOp);
						pAttr->Value.m_LightAngle = NewAngle;
					}
				}
				
				// change the bevel node's angle
				pBevelNode->m_LightAngle = NewAngle;

				// and invalidate the bevel node
				if (pBevelNode->GetBevelNode())
				{
					if (pView)
					{
						GetApplication()->AddNodeToRegenList(pBevelNode);
					}
					else
					{
						pBevelNode->GetBevelNode()->InvalidateMe();
					}
				}
							
				// and now factor out
				// ((NodeRenderableInk *)pLastNode)->FactorOutCommonChildAttributes();
			}
		}

		pItem = (NodeListItem *)pNodes->GetNext(pItem);
	}
	
	return Ac;
}

/********************************************************************************************

>	ActionCode ChangeLightAnglesAction::Execute();

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/99
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  Causes a regen of all bevels nodes in the action's list
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeLightAnglesAction::Execute()
{
	ActionCode Act = AC_OK;
	ChangeLightAnglesAction* pAction;

	Act = ChangeLightAnglesAction::Init(pOperation, 
										pOppositeActLst,
										&m_NodeList,
										m_OldAngle,
										m_NewAngle,
										&pAction);

	if (Act != AC_FAIL)
	{
		INT32 angle = m_NewAngle;
		m_NewAngle = m_OldAngle;
		m_OldAngle = angle;
	}

	return Act;
}

ChangeLightAnglesAction::~ChangeLightAnglesAction()
{
	m_NodeList.DeleteAll();
}

////////////////////////////////////////////////////////////////////
// RemoveBevelAttributesAction class implementation
/********************************************************************************************

>	RemoveBevelAttributesAction::RemoveBevelAttributesAction()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

RemoveBevelAttributesAction::RemoveBevelAttributesAction()
{
	m_bShouldDo = FALSE;
}


/********************************************************************************************

>	ActionCode RemoveBevelAttributesAction::Init( Operation* pOp,
										  ActionList* pActionList,
										  NodeRenderableInk * pNode, 
										  BOOL m_bShouldDo,
										  RemoveBevelAttributesAction** NewAction);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pNode			=	ptr to the node to use
				m_bShouldDo		= whether to perform the action now, or wait until the undo
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



ActionCode RemoveBevelAttributesAction::Init( Operation* pOp,
										  ActionList* pActionList,
										  NodeRenderableInk * pNode, 
										  BOOL bShouldDo,
										  RemoveBevelAttributesAction** NewAction)
{
	if (!pOp)
		return AC_FAIL;

	if (!pOp->IsKindOf(CC_RUNTIME_CLASS(UndoableOperation)))
		return AC_FAIL;	
	
	UINT32 ActSize = sizeof(RemoveBevelAttributesAction);
	
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(RemoveBevelAttributesAction),(Action**)NewAction);

	if (Ac != AC_OK)
		return Ac;

//	UndoableOperation * pUndoOp = (UndoableOperation *)pOp;

	(*NewAction)->m_bShouldDo = bShouldDo;
	(*NewAction)->m_pNode	  = pNode;

//	HideNodeAction * pHideAction = NULL;

//	BOOL ok = TRUE;

//	NodeListItem * pItem = NULL;

	// put this at the head of the list
	pActionList->RemoveItem(*NewAction);
	pActionList->AddHead(*NewAction);

	return Ac;
}

/********************************************************************************************

>	ActionCode ChangeLightAnglesAction::Execute();

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/99
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  Causes a regen of all bevels nodes in the action's list
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode RemoveBevelAttributesAction::Execute()
{
	ActionCode Act = AC_OK;
	RemoveBevelAttributesAction* pAction;

	if (m_bShouldDo)
	{
		Act = RemoveBevelAttributesAction::Init(pOperation, 
										pOppositeActLst,
										m_pNode,
										FALSE,
										&pAction);
	}
	else
	{
		Act = RemoveBevelAttributesAction::Init(pOperation, 
										pOppositeActLst,
										m_pNode,
										TRUE,
										&pAction);
	}

	NodeListItem * pItem = NULL;

	// the business end
	if (!m_bShouldDo && Act == AC_OK)
	{
		// get rid of the existing list
		pAction->m_AttrList.DeleteAll();
		
		// remove all bevel attributes from the node's tree
		Node * pChild = pAction->m_pNode->FindFirstDepthFirst();
		Node * pNext = NULL;

		while (pChild && pChild != pAction->m_pNode)
		{
			pNext = pChild->FindNextDepthFirst(pAction->m_pNode);

			if (pChild->IsAnAttribute())
			{
				if (pChild->IS_KIND_OF(AttrBevel))
				{
					// delete the node
//					pChild->UnlinkNodeFromTree();

					pItem = new NodeListItem(pChild);

					pAction->m_AttrList.AddTail(pItem);					
				}
			}

			pChild = pNext;
		}
	}
	else
	{
		pItem = (NodeListItem *)m_AttrList.GetHead();

		while (pItem)
		{
			pItem->pNode->AttachNode(m_pNode, FIRSTCHILD);

			pItem = (NodeListItem *)m_AttrList.GetNext(pItem);
		}

		m_AttrList.DeleteAll();

		NodeBevelController * pController = 
			(NodeBevelController *)m_pNode->FindParent(CC_RUNTIME_CLASS(NodeBevelController));

		if (pController)
			pController->OptimiseAttributes();

		// m_pNode->NormaliseAttributes();
	}

	return Act;
}

RemoveBevelAttributesAction::~RemoveBevelAttributesAction()
{
	m_AttrList.DeleteAll();	
}



/********************************************************************************************
>	OpChangeBevelLightAngle::OpChangeBevelLightAngle()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> & Olivier
	Created:	21/11/96
	Purpose:	Constructor.
********************************************************************************************/


OpChangeBevelLightAngle::OpChangeBevelLightAngle()
{
	//TRACEUSER( "MarkH", _T("Just Made a OpCreateBevel Class!\n"));
}



/********************************************************************************************
>	OpCreateBevel::~OpCreateBevel()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> & Olivier
	Created:	21/11/96
	Purpose:	Destructor.
********************************************************************************************/

OpChangeBevelLightAngle::~OpChangeBevelLightAngle()
{
	//TRACEUSER( "MarkH", _T("Just Killed OpCreateBevel Class!!!\n"));
}

/********************************************************************************************

>	BOOL OpChangeBevelLightAngle::Declare()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/99
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpChangeBevelLightAngle::Declare()
{
	BOOL retn = (RegisterOpDescriptor(
								0, 
								_R(IDS_CHANGEATTRIBUTE),
								CC_RUNTIME_CLASS(OpChangeBevelLightAngle), 
								OPTOKEN_CHANGEATTRIBUTE,
								OpChangeBevelLightAngle::GetState));
	return retn;

}

/********************************************************************************************
>	void OpChangeBevelLightAngle::DoWithParam(OpDescriptor* pOp, OpParam* pParam)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/01/97
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Changes all the bevel light angles in the list to the given angle
********************************************************************************************/

void OpChangeBevelLightAngle::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	if (!pOp || !pParam)
	{
		End();
		return;
	}

	OpChangeLightAnglesParam * pAttrParam = (OpChangeLightAnglesParam *)pParam;

	if (!pAttrParam->pNodes)
	{
		End();
		return;
	}

	if (pAttrParam->pNodes->IsEmpty())
	{
		End();
		return;
	}

	// Start a slow job
	BeginSlowJob(-1, TRUE);

	DoStartSelOp(TRUE);

	BOOL ok = TRUE;

	// first, localise all the attributes
	List * pNodes = pAttrParam->pNodes;
	NodeListItem * pItem = (NodeListItem *)pNodes->GetHead();

	AttrBevelLightAngle * pLightAttr = NULL;
	AttrBevelLightAngle * pNewLightAttr = NULL;
//	Node * pParent = NULL;
	NodeBevelController * pControl = NULL;
	NodeHidden		* pHidden = NULL;
	Node * pThisNode = NULL;
	Node * pNextNode = NULL;
	HideNodeAction *pAction = NULL;							
	List AttrList;

	// invalidate all the nodes first and localise attributes
	while (pItem)
	{
		if (pItem->pNode)
		{
			if (pItem->pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevelController)))
			{
				// find the applied attribute
				pControl = (NodeBevelController *)pItem->pNode;

				if (ok)
					ok = DoLocaliseForAttrChange(pControl, CC_RUNTIME_CLASS(AttrBevelLightAngle),
					NULL);

				BevelTools::GetAllNodesUnderNode(pControl, &AttrList, CC_RUNTIME_CLASS(AttrBevelLightAngle));

				if (AttrList.IsEmpty())
				{
					ALLOC_WITH_FAIL(pLightAttr, new AttrBevelLightAngle(pControl, FIRSTCHILD), this);
					pLightAttr->Value.m_LightAngle = (INT32)pControl->m_LightAngle;
					
					if (HideNodeAction::Init(this, GetUndoActionList(), (Node *)pLightAttr,
						FALSE, (Action **)&pAction, FALSE) != AC_OK)
					{
						FailAndExecute();
						End();
						return;				
					}
				}

				AttrList.DeleteAll();

				pThisNode = pControl->FindFirstDepthFirst();

				while (pThisNode && ok)
				{
					pNextNode = pThisNode->FindNextDepthFirst(pControl);
					
					if (pThisNode->IsAnAttribute())
					{
						if (pThisNode->IS_KIND_OF(AttrBevelLightAngle))
						{
							// make a new attribute node to replace this one
							ALLOC_WITH_FAIL(pNewLightAttr, new AttrBevelLightAngle, this);
							pNewLightAttr->Value.m_LightAngle = pAttrParam->NewLightAngle;

							// hide the original node
							if (ok)
								ok = DoHideNode(pThisNode, FALSE, &pHidden);

							// insert the new node
							pNewLightAttr->AttachNode(pHidden, NEXT);

							if (HideNodeAction::Init(this, GetUndoActionList(), (Node *)pNewLightAttr,
								FALSE, (Action **)&pAction, FALSE) != AC_OK)
							{
								ok = FALSE;
							}
						}
					}

					pThisNode = pNextNode;
				}

				if (ok)
					ok = DoFactorOutAfterAttrChange(pControl, CC_RUNTIME_CLASS(AttrBevelLightAngle));

				pControl->RegenerateBevelBitmap(this, TRUE);
			}
		}

		pItem = (NodeListItem *)pNodes->GetNext(pItem);
	}					

	if (!ok)
	{
		FailAndExecute();
		End();
		return;
	}
				
	End();
}




/********************************************************************************************

>	OpState OpChangeBevelLightAngle::GetState(String_256* Description, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Outputs:	-
	Returns:	Ungreyed, Unticked
	Purpose:	Find out the state of the new regular shape at the specific time

********************************************************************************************/

OpState OpChangeBevelLightAngle::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}

/********************************************************************************************

>	void OpChangeBevelLightAngle::GetOpName(String_256 * pString)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Outputs:	-
	Returns:	
	Purpose:	Gets the name of the op

********************************************************************************************/

void OpChangeBevelLightAngle::GetOpName(String_256 * pString)
{
	if (pString)
		pString->Load(_R(IDS_CHANGEBEVELLIGHTANGLEOPNAME));
}

/********************************************************************************************
>	OpSelectBevel::OpSelectBevel()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> & Olivier
	Created:	21/11/96
	Purpose:	Constructor.
********************************************************************************************/
OpSelectBevel::OpSelectBevel()
{
	//TRACEUSER( "MarkH", _T("Just Made a OpCreateBevel Class!\n"));
}



/********************************************************************************************
>	OpCreateBevel::~OpCreateBevel()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> & Olivier
	Created:	21/11/96
	Purpose:	Destructor.
********************************************************************************************/
OpSelectBevel::~OpSelectBevel()
{
	//TRACEUSER( "MarkH", _T("Just Killed OpCreateBevel Class!!!\n"));
}

/********************************************************************************************

>	BOOL OpSelectBevel::Declare()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/99
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/
BOOL OpSelectBevel::Declare()
{
	BOOL retn = (RegisterOpDescriptor(
								0, 
								_R(IDS_SELECTBEVEL),
								CC_RUNTIME_CLASS(OpSelectBevel), 
								OPTOKEN_SELECTBEVEL,
								OpSelectBevel::GetState));
	return retn;

}

/********************************************************************************************
>	void OpSelectBevel::Do(OpDescriptor* pOp)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/01/97
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Changes all the bevel light angles in the list to the given angle
********************************************************************************************/

void OpSelectBevel::Do(OpDescriptor* pOp)
{

	if (!pOp)
	{
		End();
		return;
	}

	BeginSlowJob(-1, TRUE);

	List ControllerList;

	// build the list of all selected bevel controller nodes
	BevelTools::BuildListOfSelectedNodes(&ControllerList, CC_RUNTIME_CLASS(NodeBevelController), TRUE);

	if (ControllerList.IsEmpty())
	{
		End();
		return;
	}

	// deselect everything
	NodeRenderableInk::DeselectAll();

	// now, run through the list selecting all bevel inking nodes
	NodeListItem * pItem  = (NodeListItem *)ControllerList.GetHead();
	NodeBevel	 * pBevel = NULL;
	
	while (pItem)
	{
		if (pItem->pNode)
		{
			pBevel = ((NodeBevelController *)pItem->pNode)->GetBevelNode();

			if (pBevel)
			{
				pBevel->SetSelected(TRUE);
			}
		}

		pItem = (NodeListItem *)ControllerList.GetNext(pItem);
	}

	// clean up
	ControllerList.DeleteAll();

	GetApplication()->UpdateSelection();

	End();

	// get the current tool (if it's the bevel tool) and get it
	// to regen its blobs
	if (Tool::GetCurrentID() == TOOLID_BEVELTOOL)
	{
		BevelTool * pTool = (BevelTool *)Tool::GetCurrent();
		pTool->InvalidateToolBlobs();
	}
}




/********************************************************************************************

>	OpState OpSelectBevel::GetState(String_256* Description, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Outputs:	-
	Returns:	Ungreyed, Unticked
	Purpose:	Find out the state of the new regular shape at the specific time

********************************************************************************************/
OpState OpSelectBevel::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}




#endif



