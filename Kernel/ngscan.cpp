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
	$Header: /Camelot/kernel/ngscan.cpp 12    16/12/03 17:23 Phil $
	The various scans made over documents and the selected objects by the Attribute gallery.
*/

#include "camtypes.h"

#include "ngcore.h"
#include "ngitem.h"
#include "ngprop.h"
#include "ngscan.h"
#include "ngsentry.h"

#include "extender.h"
#include <algorithm>

#include "app.h"
#include "undoop.h"
#include "objchge.h"
#include "ink.h"
#include "nodeattr.h"
#include "layer.h"
#include "keypress.h"
#include "nodebmp.h"		// for NodeBitmap
#include "nodershp.h"		// for NodeRegularShape
#include "ophist.h"

#include "cxfrech.h"		// TemplateAttribute
#include "attrval.h"
#include "userattr.h"
#include "tmpltatr.h"

//#include "mario.h"

#include "nodershp.h"		// for NodeRegularShape
#include "nodebmp.h"		// for NodeBitmap

// for text story stuff
#include "nodetxts.h"
#include "nodetxtl.h"
#include "nodetext.h"

//#include "sliceres.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

DECLARE_SOURCE("$Revision$");

#define new CAM_DEBUG_NEW

// This is shared between all instances of AllowOpScan so an operation can make
// multiple allow scans on the same OCP.
ObjChangeParam AllowOpScan::m_OCP;


/********************************************************************************************
>	BaseSelectScan::BaseSelectScan(BaseSelectScan::Change eChange, BOOL fQuiet = FALSE)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		eChange			---		how to change the selection: TOGGLE, DESELECT,
										SELECT, SELECT_EXCLUSIVE
				fQuiet			---		don't redraw blobs or broadcast messages
	Purpose:	Constructs an BaseSelectScan object.
	SeeAlso:	NodeScan
********************************************************************************************/

BaseSelectScan::BaseSelectScan(BaseSelectScan::Change eChange, BOOL fQuiet)
  :	NodeScan(&theSelectedDocument),
	m_eChange(eChange),
	m_fShout(!fQuiet),
	m_nAffected(0)
{  
	// Empty.
}



/********************************************************************************************
>	virtual BOOL BaseSelectScan::OnSelectable(NodeRenderable* pRender)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pRender	---		the object whose selection state is to be changed
	Returns:	The previous selection state of the object.
	Purpose:	Helper function for BaseSelectScan derivatives that change the selection
				state of renderable nodes.
********************************************************************************************/

BOOL BaseSelectScan::OnSelectable(NodeRenderable* pRender)
{
	// Check for the effects of the layer containing the object.
	Node* pNode = pRender;
	while (pNode != 0)
	{
		if (pNode->IsLayer())
		{
			// Quit now if the layer is invisible or locked.
			Layer* pLayer = (Layer*) pNode;
			if (/*!pLayer->IsVisible() ||*/ pLayer->IsLocked()) return FALSE;
			break;
		}

		pNode = pNode->FindParent();
	}

	// Perform the selection change.
	BOOL fPrevSel = pRender->IsSelected();
	switch (m_eChange)
	{
	case TOGGLE:
		if (fPrevSel)
			pRender->DeSelect(m_fShout);
		else
			pRender->Select(m_fShout);
		break;

	case DESELECT:
		pRender->DeSelect(m_fShout);
		break;

	default:
		pRender->Select(m_fShout);
		break;
	}

	// Keep count of how many we've changed.
	++m_nAffected;
	return fPrevSel;
}



/********************************************************************************************
>	virtual BOOL BaseSelectScan::Pre()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pNode		---		the current node in the scan
	Purpose:	Clears the selection first if the new state isn't "toggle" and the
				gallery ctrl-key isn't down.
	Returns:	TRUE, continue scanning.
	SeeAlso:	NodeScan
********************************************************************************************/

BOOL BaseSelectScan::Pre()
{
	// If selecting exclusively then clear the selection of all objects first and count
	// them towards a call to Update().
	if (m_eChange == SELECT_EXCLUSIVE)
	{
		m_nAffected += (INT32) GetApplication()->FindSelection()->Count();
		NodeRenderableInk::DeselectAll(m_fShout, FALSE);
	}

	return TRUE;
}



/********************************************************************************************
>	virtual BOOL BaseSelectScan::Do(Node* pNode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pNode		---		the current node in the scan
	Returns:	TRUE, continue scanning.
	Purpose:	Called by SelectScan when the 'select' action is executed.  Tests
				the given node for membership of each highlighted set and changes it's
				selection status accordingly.
	SeeAlso:	BaseSelectScan
********************************************************************************************/

BOOL BaseSelectScan::Do(Node* pNode)
{
	// Let OnSelectable() control the continuation of the scan.
	return pNode->IsNodeRenderableClass() ? OnSelectable((NodeRenderable*) pNode) : TRUE;
}



/********************************************************************************************
>	virtual BOOL BaseSelectScan::Post()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Purpose:	Called by BaseSelectScan after the selection is changed.  Updates the
				application's selection range object.
	SeeAlso:	NodeScan
********************************************************************************************/

BOOL BaseSelectScan::Post()
{
	if (m_nAffected != 0) GetApplication()->FindSelection()->Update(m_fShout);
	return TRUE;
}



/********************************************************************************************
>	SelectScan::SelectScan(SGNameItem* pItem, BaseSelectScan::Change eChange,
						   BOOL fQuiet = FALSE)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pItem			---		the set of objects to change the selection of
				eChange			---		how to change the selection: TOGGLE, DESELECT,
										SELECT, SELECT_EXCLUSIVE
	Purpose:	Constructs an SelectScan object.
	SeeAlso:	BaseSelectScan
********************************************************************************************/

SelectScan::SelectScan(SGNameItem* pItem, BaseSelectScan::Change eChange, BOOL fQuiet)
  :	BaseSelectScan(eChange, fQuiet),
	m_pItem(pItem)
{  
	// Empty.
}



/********************************************************************************************
>	virtual BOOL SelectScan::OnSelectable(NodeRenderable* pRender)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pRender		---		the current renderable node in the selection scan
	Returns:	TRUE, continue scanning.
	Purpose:	Called by SelectScan when the 'select' action is executed.  Tests
				the given node for membership of each highlighted set and changes it's
				selection status accordingly.
	SeeAlso:	BaseSelectScan
********************************************************************************************/

BOOL SelectScan::OnSelectable(NodeRenderable* pRender)
{
	// If the node is a member then change its selection state.
	if (m_pItem->IsMember(pRender))
	{
		// what it does is look at the text story if we are trying to select a text char
		// and we will be selecting the story
		// then just set the story to the end result
		// rather than selecting one char at a time (sjk 1/8/00)
		if (pRender->IsATextChar() && m_eChange != TOGGLE)
		{
			Node * pStory = pRender->FindParent(CC_RUNTIME_CLASS(TextStory));
			if (pStory && m_pItem->IsMember(pStory))
			{
				BOOL TurnOff = (m_eChange == DESELECT);

				if (pStory->IsSelected() != TurnOff)
				{
					if (TurnOff)
						pRender->DeSelect(m_fShout);
					else
						pRender->Select(m_fShout);
				}

				// keep the counts right
				m_nAffected++;

				return TRUE;
			}
		}

		BaseSelectScan::OnSelectable(pRender);
	}
	return TRUE;
}



/********************************************************************************************
>	SelectUnionScan::SelectUnionScan(BaseSelectScan::Change eChange)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		eChange			---		how to change the selection: TOGGLE, RESET,
										SET, SET_EXCLUSIVE
	Purpose:	Constructs an SelectUnionScan object.
	SeeAlso:	BaseSelectScan
********************************************************************************************/

SelectUnionScan::SelectUnionScan(BaseSelectScan::Change eChange)
  :	BaseSelectScan(eChange)
{  
	// Empty.
}



/********************************************************************************************
>	virtual BOOL SelectUnionScan::OnSelectable(NodeRenderable* pRender)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pRender		---		the current renderable node in the scan
	Purpose:	Called by SelectUnionScan when the 'select' action is executed.  Tests
				the given node for membership of the highlighted SGNameItems and changes
				its selection status if it is a member of any.
	SeeAlso:	BaseSelectScan
********************************************************************************************/

BOOL SelectUnionScan::OnSelectable(NodeRenderable* pRender)
{
	// If it's a member of any highlighted SGNameItem then change its selection.
	if (Test(pRender, Test::IS_MEMBER).ForEach() != 0)
		BaseSelectScan::OnSelectable(pRender);

	return TRUE;
}



/********************************************************************************************
>	SelectUnionScan::Test::Test(NodeRenderable* pRender, SelectUnionScan::Test::Question eQ)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pRender		---		the renderable node to test for membership of any
									highlighted SGNameItems.
				eQ			---		SelectUnionScan::Test::IS_MEMBER or IS_NON_MEMBER.
	Purpose:	Constructs an SelectUnionScan::Test object.
	SeeAlso:	DocScan; BaseSelectScan
********************************************************************************************/

SelectUnionScan::Test::Test(NodeRenderable* pRender, SelectUnionScan::Test::Question eQ)
  :	NameItemIter(NameItemIter::HIGHLIGHTED_ITEMS),
	m_pRender(pRender),
	m_nXor((BOOL) eQ)
{
	// Empty.
}



/********************************************************************************************
>	virtual BOOL SelectUnionScan::Test::Do(SGNameItem* pItem)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pItem			---		the current item in the iteration
	SeeAlso:	BaseSelectScan; SelectUnionScan::Test
********************************************************************************************/

BOOL SelectUnionScan::Test::Do(SGNameItem* pItem)
{
	return pItem->IsMember(m_pRender) ^ m_nXor;
}



/********************************************************************************************
>	SelectIntersectScan::SelectIntersectScan(BaseSelectScan::Change eChange)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		eChange			---		how to change the selection: TOGGLE, RESET,
										SET, SET_EXCLUSIVE
	Purpose:	Constructs an SelectIntersectScan object.
	SeeAlso:	DocScan; BaseSelectScan
********************************************************************************************/

SelectIntersectScan::SelectIntersectScan(BaseSelectScan::Change eChange)
  :	SelectUnionScan(eChange)
{
	// Empty.
}



/********************************************************************************************
>	virtual BOOL SelectIntersectScan::Do(NodeRenderable* pRender)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pNode		---		the current node in the scan
	Purpose:	Called by SelectIntersectScan when the 'intersect' action is executed.  Tests
				the given node for membership of all highlighted sets and changes it's
				selection status accordingly.
	SeeAlso:	BaseSelectScan
********************************************************************************************/

BOOL SelectIntersectScan::OnSelectable(NodeRenderable* pRender)
{
	// If it's a member of every highlighted SGNameItem then change its selection.
	if (Test(pRender, Test::IS_NON_MEMBER).ForEach() == 0)
		BaseSelectScan::OnSelectable(pRender);

	return TRUE;
}



/********************************************************************************************
>	AllowOpScan::AllowOpScan(UndoableOperation* pOp, NodeScan::Source* ps)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pOp		---		the UndoableOperation to scan for "allowance".
				ps	---			the source for nodes to scan over, by default the
								'selected' document's tree.
	Purpose:	Constructs an AllowOpScan object, which checks AllowOp for the given 
				operation on all nodes in the tree which are members of highlighted
				items in the 'Used Names' group of the Name Gallery.  Defines the 
				ObjChangeParam as STARTING.
	SeeAlso:	NodeScan; ObjChangeParam; UndoableOperation
********************************************************************************************/

AllowOpScan::AllowOpScan(UndoableOperation* pOp, NodeScan::Source* ps)
  :	UndoableNodeScan(pOp, ps)
{
	// Set up the change flags etc.
	m_OCP.Define(OBJCHANGE_STARTING, ObjChangeFlags(FALSE, FALSE, FALSE, TRUE), 0, m_pOp);
}



/********************************************************************************************
>	virtual BOOL AllowOpScan::Do(Node* pNode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pNode	---		the node to call AllowOp for if it is a member of
								a highlighted set in the Name Gallery.
	Purpose:	Tests if the given node is a member of a set that the user has
				highlighted in the Name Gallery and if so, calls AllowOp on it.
	SeeAlso:	AllowOpScan::IsLitUsedName; Node::AllowOp
********************************************************************************************/

BOOL AllowOpScan::Do(Node* pNode)
{
	// If the node is a highlighted member of a "used names" set then call it's AllowOp
	// function and stop the iteration if it refuses the op.  If it isn't a member then
	// ignore it.
	return IsLitUsedName(pNode).ForEach() == 0 || pNode->AllowOp(&m_OCP);
}



/********************************************************************************************
>	virtual BOOL AllowOpScan::Update()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pOp		---		the UndoableOperation to scan for "allowance".
				pOCP	---		the ObjChangeParam block to use
	Purpose:	Defines the ObjChangeParam as FINISHED and calls UpdateChangedNodes.
	Returns:	TRUE if successful.
	SeeAlso:	ObjChangeParam; UndoableOperation
********************************************************************************************/

BOOL AllowOpScan::Update()
{
	// Update all parents of changed nodes.
	m_OCP.Define(OBJCHANGE_FINISHED, ObjChangeFlags(FALSE, FALSE, FALSE, TRUE), 0, m_pOp);
	return m_pOp->UpdateChangedNodes(&m_OCP);

}



/********************************************************************************************
>	AllowOpScan::IsLitUsedName::IsLitUsedName(Node* pNode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Inputs:		pNode		---		the node to test for membership of a set within
									the 'Used Names' group of the Name Gallery.
	Purpose:	Tests if the given Node is a member of a set within the Attribute gallery.
	SeeAlso:	AllowOpScan; NameItemIter
********************************************************************************************/

AllowOpScan::IsLitUsedName::IsLitUsedName(Node* pNode)
  :	NameIterOp(pNode)
{
	// Empty.	
}



/********************************************************************************************
>	virtual BOOL AllowOpScan::IsLitUsedName::Do(SGNameItem* pItem)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Inputs:		pItem		---		the set item to test for membership
	Purpose:	Tests if the Node passed to the constructor is a member of the given
				set item.
	SeeAlso:	AllowOpScan::IsLitUsedName; SGNameItem::IsMember
********************************************************************************************/

BOOL AllowOpScan::IsLitUsedName::Do(SGNameItem* pItem)
{
	// Quit as soon as we find a set item the object is a member of.  ForEach will return
	// the address of the first set item the object is within.
	return !pItem->IsMember(m_pNode);
}



/********************************************************************************************
>	AllowOpSingleScan::AllowOpSingleScan(UndoableOperation* pOp, NodeScan::Source* ps,
										 const StringBase& strName)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pOp		---		the UndoableOperation to scan set strName for "allowance".
				ps		---		the source for candidate nodes to scan over, by default the
								'selected' document's tree.
				strName	---		name of the set whose members are to be scanned
	Purpose:	Constructs an AllowOpSingleScan object, which checks AllowOp for the given 
				operation on all nodes in the tree which are members of the specified
				set.
	SeeAlso:	NodeScan; AllowOpScan; OpDeleteNames; ObjChangeParam; UndoableOperation
********************************************************************************************/

AllowOpSingleScan::AllowOpSingleScan(UndoableOperation* pOp, NodeScan::Source* ps,
									 const StringBase& strName)
  :	AllowOpScan(pOp, ps),
	m_strName(strName),
	m_pUsedNames(NameGallery::Instance()->GetUsedNames())
{
	ERROR3IF(m_pUsedNames == 0, "AllowOpSingleScan::AllowOpSingleScan: no used names");
}



/********************************************************************************************
>	virtual BOOL AllowOpSingleScan::Do(Node* pNode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pNode	---		the node to call AllowOp for if it is a member of
								a specified set.
	Purpose:	Tests if the given node is a member of a specified and if so, calls
				AllowOp on it.
	SeeAlso:	AllowOpScan::Do; SGUsedNames::IsMember; Node::AllowOp
********************************************************************************************/

BOOL AllowOpSingleScan::Do(Node* pNode)
{
	// Stop iterating if the node is a member which doesn't allow the op.
	return !m_pUsedNames->IsMember(pNode, m_strName) || pNode->AllowOp(&m_OCP);
}



/********************************************************************************************
>	static BOOL ApplyNameToNode(const StringBase& strName, Node* pNode,
								UndoableOperation* pOp, INT32 BarNo)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		strName		---		the node to apply a name to
				pNode		---		the name to apply to the node
				pOp			---		the Operation context in which to manipulate nodes
				BarNo		---		the bar number if it has one
	Returns:	TRUE if successful.
	Purpose:	Helper function to create a new Wix object name attribute and attach
				it to the given node in an undoable way.
	Errors:		Out of memory.
	SeeAlso:	ApplyScan
********************************************************************************************/

static BOOL ApplyNameToNode(const StringBase& strName, Node* pNode, UndoableOperation* pOp, INT32 BarNo)
{
	// Try to create a new object name attribute to apply to the target node.
	TemplateAttribute* pta;
	String_256 BarName = "";

	if (BarNo >= 0)
	{
		BarName.MakeMsg(_R(IDS_BARNAME), BarNo+1);
	}

	ALLOC_WITH_FAIL(pta, (new TemplateAttribute(TA_NAME, BarNo >= 0 ? BarName : _T("") , strName)), pOp);
	ERRORIF(pta == 0, _R(IDE_NOMORE_MEMORY), FALSE);

	// If the target node is an object, check that the attribute type is required by it,
	// and make a note for the attribute manager of the object being applied to.
	if (pNode->IsAnObject())
	{
		NodeRenderableInk* pObj = (NodeRenderableInk*) pNode;
		if (!pObj->RequiresAttrib(pta))
		{
			// (No objects reject Wix attributes, AFAIK)
			TRACEUSER( "JustinF", _T("ApplyNameToNode: %s failed RequireAttrib at 0x%p\n"),
											pObj->GetRuntimeClass()->m_lpszClassName, pObj);
			delete pta;
			return FALSE;
		}

		AttributeManager::pLastNodeAppliedTo = pObj;
	}

	// Attach the new attribute and make a note for the AttributeManager.
	pta->AttachNode(pNode, FIRSTCHILD);

/*				*** DUBIOUS TEXT BODGE ***
	// If we are applying the attribute to a node which discards its attribute children
	// (like the caret) then we should not generate any undo for the operation.
	if (m_pNode->DiscardsAttributeChildren()) SucceedAndDiscard();

	// JCF: this is a bodge for the text tool, copied from 
	// OpApplyAttribToSelected::ApplyToSelection:-
	//
	// "EOL nodes can now have attributes, but the last EOL cannot
	// be selected by the user, causing the text tool to show <multiple>
	// when it shouldn't really.  So if the last node in the range is
	// the node before the last EOL in a text story then we apply the
	// attribute to the EOL too."
	Node* pLastNode = selRange.FindLast();
	if (pLastNode != 0 && pLastNode->IsAVisibleTextNode())
	{
		// "Alternative approach to fix other EOLNode problems that have appeared, ie.
		// (Bugs 5371 & 5372). If the AbstractTextChar following the last selected node
		// is an EOLNode then apply to it too."
		AbstractTextChar* pNextChr =
			(AbstractTextChar*) pLastNode->FindNext(CC_RUNTIME_CLASS(AbstractTextChar));
		if (pNextChr != 0 && pNextChr->IsAnEOLNode() && !DoApply(pNextChr, pAttrib, pSets))
		{
			delete pAttrib;
			FailAndExecute();
			End();
			return;
		}
	}
*/
	// Create an action to hide the new attribute when we Undo.
	HideNodeAction* pHideAct;
	if (AC_FAIL == HideNodeAction::Init(pOp, pOp->GetUndoActions(),
										pta, TRUE, (Action**) &pHideAct))
	{
		pta->CascadeDelete();
		delete pta;
		return FALSE;
	}

	// Success.
	return TRUE;
}



/********************************************************************************************
>	ApplyScan::ApplyScan(UndoableOperation* pOp, NodeScan::Source* ps)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pOp			---		the UndoableOperation that is the context for this
									scanning
				ps			---		the source for nodes to scan over
	Purpose:	Constructs an ApplyScan object
	SeeAlso:	UndoableNodeScan; OpApplyNames; ApplyScan::ApplyItems
********************************************************************************************/

ApplyScan::ApplyScan(UndoableOperation* pOp, NodeScan::Source* ps)
  :	UndoableNodeScan(pOp, ps)
{
	// Empty.
}



/********************************************************************************************
>	virtual BOOL ApplyScan::Do(Node* pNode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pNode		---		the node to apply names to
	Purpose:	Applies names highlighted in the Attribute gallery to the given node.
	SeeAlso:	OpApplyNames; ApplyScan::ApplyItems
********************************************************************************************/

BOOL ApplyScan::Do(Node* pNode)
{
	// Apply all highlighted 'Used Names' to the node.
	return ApplyItems(pNode, m_pOp).ForEach() == 0;
}



/********************************************************************************************
>	ApplyScan::ApplyItems::ApplyItems(Node* pNode, UndoableOperation* pOp)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pNode		---		the node to apply highlighted items' names
				pOp			---		the Operation context in which to manipulate the attrs.
	SeeAlso:	OpApplyNames::Do
********************************************************************************************/

ApplyScan::ApplyItems::ApplyItems(Node* pNode, UndoableOperation* pOp)
  :	NameIterOp(pNode, pOp)
{
	// Empty.
}



/********************************************************************************************
>	BOOL ApplyScan::ApplyItems::Do(SGNameItem* pItem)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pItem		---		the highlighted set item whose name is to be applied
	Purpose:	Applies the Wix ObjectName attributes that apply to objects.
	SeeAlso:	OpApplyNames
********************************************************************************************/

BOOL ApplyScan::ApplyItems::Do(SGNameItem* pItem)
{
	// If the object is already a member of the item's set then ignore it.
	if (pItem->IsMember(m_pNode)) return TRUE;

	// Extract the name from the item and apply it.
	String_256 strName;
	pItem->GetNameText(&strName);
	return ApplyNameToNode(strName, m_pNode, m_pOp, pItem->m_BarNumber);
}



/********************************************************************************************
>	ApplySingleScan::ApplySingleScan(UndoableOperation* pOp, NodeScan::Source* ps,
									 const StringBase& strName)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pOp			---		the UndoableOperation that is the context for this
									scanning
				ps			---		the source for nodes to scan over
				strName		---		the name to apply to the selected objects
	Purpose:	Constructs an ApplySingleScan object
	SeeAlso:	ApplyScan; UndoableNodeScan; OpApplyNames::DoWithParam
********************************************************************************************/

ApplySingleScan::ApplySingleScan(UndoableOperation* pOp, NodeScan::Source* ps,
								 const StringBase& strName)
  :	ApplyScan(pOp, ps),
	m_strName(strName)
{
	// Empty.
}



/********************************************************************************************
>	virtual BOOL ApplySingleScan::Do(Node* pNode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pNode		---		the node to apply the name to
	Purpose:	Applies the name passed to the constructor to all the selected objects.
	SeeAlso:	ApplyScan; UndoableNodeScan; OpApplyNames::DoWithParam
********************************************************************************************/

BOOL ApplySingleScan::Do(Node* pNode)
{
	return ApplyNameToNode(m_strName, pNode, m_pOp, -1);
}



/********************************************************************************************
>	static NodeSetProperty* ApplyPropToNode(const StringBase& strName, Node* pNode,
											NodeSetProperty* pCopyPropNode,
											UndoableOperation* pOp)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		strName		---		the node to 'apply' a property to
				pNode		---		the property to 'apply' to the node
				pOp			---		the Operation context in which to manipulate nodes
				pCopyPropNode	---		the NodeSetProperty whose properties are to be
										copied into the 'applied' property, or null 
										for no copy and default properties.
	Returns:	Pointer to the newly 'applied' NodeSetProperty, or null for failure.
	Purpose:	Helper function to create a new NodeSetProperty, with default properties,
				and attach it to the given node in an undoable way.
	Errors:		Out of memory.
	SeeAlso:	ApplyPropScan
********************************************************************************************/

static NodeSetProperty* ApplyPropToNode(const StringBase& strName, Node* pNode,
										NodeSetProperty* pCopyPropNode,
										UndoableOperation* pOp)
{
	// If the set sentinel already has a NodeSetProperty for the given name then
	// do nothing.
	NodeSetProperty* psp =
			Document::GetSelected()->GetSetSentinel()->FindPropertyNode(strName);
	if (psp != 0) return psp;

	// Try to create a new property node to 'apply' to the sentinel.
	ALLOC_WITH_FAIL(psp, (new NodeSetProperty(strName)), pOp);
	ERRORIF(psp == 0, _R(IDE_NOMORE_MEMORY), 0);

	// Try to make a bunch of default or copied properties for it.
	BOOL fOK;
	ALLOC_WITH_FAIL(fOK, 
					(pCopyPropNode != 0 ? psp->CopyProperties(pCopyPropNode)
										: psp->CreateDefaults()),
					pOp);
	if (!fOK)
	{
		delete psp;
		ERROR1RAW(_R(IDE_NOMORE_MEMORY));
		return 0;
	}

	// Attach the new property and create an action to hide it when we Undo.
	HideNodeAction* pHideAct;
	psp->AttachNode(pNode, LASTCHILD);
	if (AC_FAIL == HideNodeAction::Init(pOp, pOp->GetUndoActions(),
										psp, TRUE, (Action**) &pHideAct))
	{
		psp->CascadeDelete();
		delete psp;
		return 0;
	}

	// Success.
	return psp;
}



/********************************************************************************************
>	ApplyPropScan::ApplyPropScan(UndoableOperation* pOp, NodeScan::Source* ps,
								 const StringBase& strName,
								 NodeSetProperty* pCopyPropNode = 0)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pOp				---		the UndoableOperation that is the context for this
										scanning
				strName			---		the name to apply to the selected objects
				pCopyPropNode	---		the NodeSetProperty whose properties are to be
										copied into the 'applied' property, or null 
										for no copy and default properties.
	Purpose:	Constructs an ApplyPropScan object
	SeeAlso:	ApplySingleScan; ApplyScan; UndoableNodeScan; OpApplyNames::DoWithParam
********************************************************************************************/

ApplyPropScan::ApplyPropScan(UndoableOperation* pOp, const StringBase& strName,
							 NodeSetProperty* pCopyPropNode)
  :	ApplySingleScan(pOp, &theSetSentinel, strName),
	m_pCopyPropNode(pCopyPropNode)
{
	// Empty.
}



/********************************************************************************************
>	virtual BOOL ApplyPropScan::Do(Node* pNode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pNode		---		the node to apply the name to
	Purpose:	Applies a default property to the given node (ie. the set sentinel).
	SeeAlso:	ApplySingleScan; ApplyScan; UndoableNodeScan; OpApplyNames::DoWithParam
********************************************************************************************/

BOOL ApplyPropScan::Do(Node* pNode)
{
	return ApplyPropToNode(m_strName, pNode, m_pCopyPropNode, m_pOp) != 0;
}



/********************************************************************************************
>	HideScan::HideScan(UndoableOperation* pOp, NodeScan::Source* ps)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pOp		---		the UndoableOperation that is the context for this
								scanning
	Purpose:	Constructs a HideScan object, which scans through the tree
				for objects which are members of highlighted 'Used Names' sets in the
				Name Gallery and excludes them from their sets by hiding the Wix
				ObjectName attributes which apply to them.
	SeeAlso:	UndoableNodeScan; OpDeleteNames; HideScan::HideItems
********************************************************************************************/

HideScan::HideScan(UndoableOperation* pOp, NodeScan::Source* ps)
  :	UndoableNodeScan(pOp, ps)
{
	// Empty.
}



/********************************************************************************************
>	virtual BOOL HideScan::Do(Node* pNode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pNode		---		the node to test for membership of a highlighted set
									and if necessary exclude it from the set
	Purpose:	Excludes the given node from any highlighted sets in the Name Gallery.
	SeeAlso:	OpDeleteNames; HideScan::HideItems
********************************************************************************************/

BOOL HideScan::Do(Node* pNode)
{
	// If the object is a highlighted member of a "used names" set then hide its
	// Wix ObjectName attribute.
	return HideItems(pNode, m_pOp).ForEach() == 0;
}



/********************************************************************************************
>	HideScan::HideItems::HideItems(Node* pNode, UndoableOperation* pOp)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pNode		---		the node to test for membership of a highlighted set
									and if necessary exclude it from the set
				pOp			--		the Operation context in which to manipulate the attrs.
	Purpose:	Excludes the given node from any highlighted sets in the Name Gallery.
	SeeAlso:	OpDeleteNames
********************************************************************************************/

HideScan::HideItems::HideItems(Node* pNode, UndoableOperation* pOp)
  :	NameIterOp(pNode, pOp)
{
	// Empty.
}



/********************************************************************************************
>	BOOL HideScan::HideItems::Do(SGNameItem* pItem)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pItem		---		the set item to test membership of
	Purpose:	Hides Wix ObjectName attributes that are applied to nodes within the
				given set item.
	SeeAlso:	OpDeleteNames
********************************************************************************************/

BOOL HideScan::HideItems::Do(SGNameItem* pItem)
{
	// If the node is a member of the item's set then exclude it from the set.
	if (pItem->IsMember(m_pNode))
	{
		// Find the exact Wix attribute that makes the node a member of pItem's set.
		TemplateAttribute* pAttr;
		for (pAttr = (TemplateAttribute*) m_pNode->FindFirstAttr( &Node::IsAnObjectName );
			 pAttr != 0;
			 pAttr = (TemplateAttribute*) pAttr->FindNextAttr( &Node::IsAnObjectName ))
				if (pItem->IsEqual(pAttr->GetParam())) break;

		// Try to hide the attribute.
		ERROR3IF(pAttr == 0, "HideScan::HideItems::Do: can't find TemplateAttribute");
		if (!m_pOp->DoHideNode(pAttr, TRUE, 0, TRUE)) return FALSE;
	}

	// Continue scanning items.
	return TRUE;
}



/********************************************************************************************
>	HidePropScan::HidePropScan(UndoableOperation* pOp)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pOp		---		the UndoableOperation that is the context for this
								scanning
	Purpose:	Constructs a HidePropScan object, which hides set properties for names
				highlighted in the Attribute gallery.
	SeeAlso:	UndoableNodeScan; OpDeleteNames; HidePropScan::HideProp
********************************************************************************************/

HidePropScan::HidePropScan(UndoableOperation* pOp)
  :	HideScan(pOp, &theSetSentinel)
{
	// Empty.
}



/********************************************************************************************
>	virtual BOOL HidePropScan::Do(Node* pNode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pNode		---		the document's NodeSetSentinel.
	Purpose:	Excludes the given node from any highlighted sets in the Name Gallery.
	SeeAlso:	OpDeleteNames; HidePropScan::HideProp
********************************************************************************************/

BOOL HidePropScan::Do(Node* pNode)
{
	// If the object is a highlighted member of a "used names" set then hide its
	// properties.
	return HideProp(pNode, m_pOp).ForEach() == 0;
}



/********************************************************************************************
>	HidePropScan::HideProp::HideProp(Node* pNode, UndoableOperation* pOp)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pNode		---		the document's NodeSetSentinel.
				pOp			--		the Operation context in which to manipulate the attrs.
	Purpose:	Excludes the given node from any highlighted sets in the Name Gallery.
	SeeAlso:	OpDeleteNames
********************************************************************************************/

HidePropScan::HideProp::HideProp(Node* pNode, UndoableOperation* pOp)
  :	HideScan::HideItems(pNode, pOp)
{
	ERROR3IF(pNode == 0 || !pNode->IS_KIND_OF(NodeSetSentinel),
				"HidePropScan::HideProp::HideProp: not a NodeSetSentinel");
}



/********************************************************************************************
>	BOOL HidePropScan::HideProp::Do(SGNameItem* pItem)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pItem		---		the set item to test membership of
	Purpose:	Hides set properties for all given item in the Attribute gallery.
	SeeAlso:	OpDeleteNames
********************************************************************************************/

BOOL HidePropScan::HideProp::Do(SGNameItem* pItem)
{
	// If the node is a member of the item's set then hide its associated property node.
	if (pItem->IsMember(m_pNode))
	{
		// Find the property.
		String_256 strName;
		pItem->GetNameText(&strName);
		NodeSetProperty* pPropNode =
				Document::GetSelected()->GetSetSentinel()->FindPropertyNode(strName);

		// Try to hide the attribute.
		//ERROR3IF(pPropNode == 0, "HidePropScan::HideProp::Do: can't find NodeSetProperty");
		if (!pPropNode) return TRUE; // trying to delete something that isn't there! Surely we have already succeeded!
		if (!m_pOp->DoHideNode(pPropNode, TRUE, 0, TRUE)) return FALSE;
	}

	// Continue scanning items.
	return TRUE;
}



/********************************************************************************************
>	HideSingleScan::HideSingleScan(UndoableOperation* pOp, NodeScan::Source* ps,
								   const StringBase& strName)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pOp			---		the UndoableOperation that is the context for this
									scanning
				ps			---		the source for candidate nodes to scan over
				strName		---		the object name to hide
	Purpose:	Constructs a HideSingleScan object, which hides set properties for names
				highlighted in the Attribute gallery.
	SeeAlso:	UndoableNodeScan; HideSingleScan::Do; OpChangeSetProperties::DoWithParam
********************************************************************************************/

HideSingleScan::HideSingleScan(UndoableOperation* pOp, NodeScan::Source* ps,
							   const StringBase& strName)
  :	HideScan(pOp, ps),
	m_strName(strName),
	m_pUsedNames(NameGallery::Instance()->GetUsedNames())
{
	ERROR3IF(m_pUsedNames == 0, "HideSingleScan::HideSingleScan: no used names");
}



/********************************************************************************************
>	virtual BOOL HideSingleScan::Do(Node* pNode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pNode		---		the node to consider hiding.
	Purpose:	Excludes the given node from a specified set if it's a member.
	SeeAlso:	HideSingleScan::HideSingleScan; SGUsedNames::IsMember
********************************************************************************************/

BOOL HideSingleScan::Do(Node* pNode)
{
	// If the node is a member of the set then hide it's attribute.
	if (m_pUsedNames->IsMember(pNode, m_strName))
	{
		// Find the exact Wix attribute that makes the node a member of the set.
		TemplateAttribute* pAttr;
		for (pAttr = (TemplateAttribute*) pNode->FindFirstAttr( &Node::IsAnObjectName );
			 pAttr != 0;
			 pAttr = (TemplateAttribute*) pAttr->FindNextAttr( &Node::IsAnObjectName ))
				if (pAttr->GetParam() == m_strName) break;

		// Try to hide the attribute.
		ERROR3IF(pAttr == 0, "HideSingleScan::Do: can't find TemplateAttribute");
		if (!m_pOp->DoHideNode(pAttr, TRUE, 0, TRUE)) return FALSE;
	}

	// Continue scanning items.
	return TRUE;
}


/********************************************************************************************
>	CreateDisplayScan::CreateDisplayScan()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Purpose:	Constructs a CreateDisplayScan object.
	SeeAlso:	NodeScan
********************************************************************************************/

CreateDisplayScan::CreateDisplayScan()
  :	NodeScan(&theSelectedDocument)
{
	// Empty.
}



/********************************************************************************************
>	virtual BOOL CreateDisplayScan::Pre()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Purpose:	Called by CreateDisplayScan when the gallery display is (re)created.  Resets
				the running counters of total and selected objects in each display item.
	SeeAlso:	NodeScan
********************************************************************************************/

BOOL CreateDisplayScan::Pre()
{
	return ClearCounters().ForEach() == 0;
}



/********************************************************************************************
>	virtual BOOL CreateDisplayScan::Do(Node* pNode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pNode			---		the current node in the scan
	Purpose:	Called by CreateDisplayScan when the gallery display is (re)created.  Tests
				the given node for membership of each set and if it is a member, creates
				a new SGNameItem for its name ('attribute value') if one doesn't already
				exist, and includes it in the running counts of total and selected objects.
	SeeAlso:	NodeScan
********************************************************************************************/

BOOL CreateDisplayScan::Do(Node* pNode)
{
	// Create/register with the sets that contain pInkNode as a member.
	return CreateItems(pNode).ForEach() == 0;
}



/********************************************************************************************
>	virtual BOOL CreateDisplayScan::Post()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Purpose:	Called by CreateDisplayScan when the gallery display is (re)created.  Deletes
				any SGNameItems which don't have any members and folds any groups that
				don't have items.
	SeeAlso:	NodeScan
********************************************************************************************/

BOOL CreateDisplayScan::Post()
{
	return DelItemIfUnref().ForEach() == 0 && FoldGroupIfEmpty().ForEach() == 0;
}



/********************************************************************************************
>	virtual BOOL CreateDisplayScan::ClearCounters::Do(SGNameItem* pItem)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pItem		---		current item in the iteration
	Purpose:	Resets (zeroes) the total and selected counters in all items.
	SeeAlso:	CreateDisplayScan; NameItemIter
********************************************************************************************/

BOOL CreateDisplayScan::ClearCounters::Do(SGNameItem* pItem)
{
	pItem->Reset();
	return TRUE;
}



/********************************************************************************************
>	CreateDisplayScan::CreateItems::CreateItems(Node* pNode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pNode	---		the node to pass to the Do function
	Purpose:	Constructs a CreateDisplayScan::CreateItems object.
	SeeAlso:	CreateDisplayScan; NameGroupIter
********************************************************************************************/

CreateDisplayScan::CreateItems::CreateItems(Node* pNode)
  :	m_pNode(pNode)
{
	// Empty.
}



/********************************************************************************************
>	virtual BOOL CreateDisplayScan::CreateItems::Do(SGNameGroup* pGroup)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pGroup		---		current group in the iteration
	Purpose:	Creates items for all the sets in pGroup.
	SeeAlso:	CreateDisplayScan; NameItemIter
********************************************************************************************/

BOOL CreateDisplayScan::CreateItems::Do(SGNameGroup* pGroup)
{
	return pGroup->CreateItems(m_pNode);
}



/********************************************************************************************
>	virtual BOOL CreateDisplayScan::DelItemIfNoRef::Do(SGNameItem* pItem)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pItem		---		current item in the iteration
	Purpose:	Deletes the names of items which no longer refer to any objects.
********************************************************************************************/

BOOL CreateDisplayScan::DelItemIfUnref::Do(SGNameItem* pItem)
{
	// Basic integrity checks for the number of objects & nodes encountered.
	ERROR3IF(pItem->GetNodeCount() < pItem->GetObjectCount(),
				"CreateDisplayScan::DelItemIfNoRef::Do: nodes < objects");
	ERROR3IF(pItem->GetNodeCount() > pItem->GetObjectCount() + 1,
				"CreateDisplayScan::DelItemIfNoRef::Do: nodes > objects + 1");
	ERROR3IF(pItem->GetSelectedCount() > pItem->GetObjectCount(),
				"CreateDisplayScan::DelItemIfNoRef::Do: selected > objects");

	// Does the item's set not contain any objects?
	if (pItem->GetNodeCount() == 0)
	{
		pItem->RemoveFromTree();
		delete pItem;
	}

	// 'Decimate the contentless 'til the bitter end', sayeth Tiberius.
	return TRUE;
}



/********************************************************************************************
>	virtual BOOL CreateDisplayScan::FoldGroupIfEmpty::Do(SGNameGroup* pGroup)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pGroup		---		current group in the iteration
	Purpose:	Folds groups which have no items.
********************************************************************************************/

BOOL CreateDisplayScan::FoldGroupIfEmpty::Do(SGNameGroup* pGroup)
{
	// Is the group item-less and unfolded?
	if (pGroup->GetChild() == 0 && !pGroup->Flags.Folded)
	{
		// Remember to unfold this group as soon as it has items again.  But if the user
		// has folded the group deliberately then we don't interfere with it.
		pGroup->Flags.Reserved |= 1;
		pGroup->Flags.Folded = TRUE;
	}

	return TRUE;
}
