// $Id: opshadow.cpp 662 2006-03-14 21:31:49Z alex $
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
// Implementation of the Shadow tool operations

/*
*/


#include "camtypes.h"
#include "opshadow.h"	
#include "nodecont.h"
#include "nodebmp.h"
#include "nbevcont.h"

#ifdef BUILDSHADOWS

// Code headers
#include "app.h"
#include "document.h"
#include "attrmgr.h"
#include "nodeshad.h"
#include "progress.h"
#include "objchge.h"
#include "nodetxts.h"
#include "list.h"

// Resource headers
//#include "shadres.h"

// for the apply attribute alias class
//#include "simon.h"
//#include "mario.h"

#include "opbevel.h"
#include "docview.h"

#include "shadtool.h"
#include "shadinfo.h"
#include "spread.h"
#include "nodeblnd.h"
#include "nodebldr.h"
#include "blobs.h"

#include "slicehelper.h"
#include "opliveeffects.h"
#include "shadtool.h"

#include "colormgr.h"
//#include "jason.h"
//#include "gerry.h"
#include "ophist.h"

DECLARE_SOURCE( "$Revision: 662 $" );

CC_IMPLEMENT_DYNCREATE(OpApplyShadow, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpChangeShadowType, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpRemoveShadow, SelOperation)
CC_IMPLEMENT_DYNCREATE(ShadowPosition, OpParam)
CC_IMPLEMENT_DYNCREATE(OpShadowPosition, SelOperation)
CC_IMPLEMENT_DYNCREATE(ShadowPenumbra, OpParam)
CC_IMPLEMENT_DYNCREATE(OpShadowPenumbra, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpShadowAngle, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpShadowHeightAndAngle, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpShadowHeight, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpGlowWidth, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpShadowScale, SelOperation)
CC_IMPLEMENT_DYNCREATE(ChangePositionShadowAction, Action)
CC_IMPLEMENT_DYNCREATE(ChangePenumbraSizeAction, Action)
CC_IMPLEMENT_DYNCREATE(NodeShadowClick, OpParam)
CC_IMPLEMENT_DYNCREATE(OpApplyAttribForShadows, OpApplyAttribToSelected)
CC_IMPLEMENT_DYNCREATE(OpSaveShadowData, SelOperation)
CC_IMPLEMENT_DYNCREATE(ChangeFloorShadowAngleAction, Action)
CC_IMPLEMENT_DYNCREATE(ChangeFloorShadowHeightAction, Action)
CC_IMPLEMENT_DYNCREATE(ChangeShadowTypeAction, Action)
CC_IMPLEMENT_DYNCREATE(ChangeGlowWidthAction, Action)
CC_IMPLEMENT_DYNCREATE(ChangeShadowScaleAction, Action)
CC_IMPLEMENT_DYNCREATE(SaveShadowDataAction, Action)
CC_IMPLEMENT_DYNCREATE(ShadowProfileOpParam, OpParam)
CC_IMPLEMENT_DYNCREATE(OpChangeShadowProfile, SelOperation)
CC_IMPLEMENT_DYNCREATE(ChangeProfileAction, Action)
CC_IMPLEMENT_DYNCREATE(ShadowDarknessOpParam, OpParam)
CC_IMPLEMENT_DYNCREATE(OpChangeShadowDarkness, SelOperation)
CC_IMPLEMENT_DYNCREATE(ChangeShadowDarknessAction, Action)

#define new CAM_DEBUG_NEW


/********************************************************************************************

>	BOOL OpApplyShadow::Declare()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/2004
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/
BOOL OpApplyShadow::Declare()
{
	BOOL ok = TRUE;

	// Use an ParamOpDescriptor to allow OpApplyLiveEffect to receive extra information
	// about the menu item that invoked it...
	ParamOpDescriptor* pOD2 = new ParamOpDescriptor(OPTOKEN_APPLYSHADOW,
														CC_RUNTIME_CLASS(OpApplyShadow),
														OpApplyShadow::GetParamState);
	ok = ok && (pOD2!=NULL);

	return ok;
}



/********************************************************************************************
>	OpApplyShadow::OpApplyShadow()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/2004
	Purpose:	Constructor.
********************************************************************************************/
OpApplyShadow::OpApplyShadow()
{
}



/********************************************************************************************
>	OpApplyShadow::~OpApplyShadow()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/2004
	Purpose:	Destructor.
********************************************************************************************/
OpApplyShadow::~OpApplyShadow()
{
}


/********************************************************************************************
>	virtual void OpApplyShadow::Do(OpDescriptor *pOpDesc)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/2004
	Inputs:		pOpDesc - OpDescriptor - unused
	Outputs:	-
	Returns:	-
	Purpose:	The do function.  Applys a shadow to the selection in the current document
********************************************************************************************/
void OpApplyShadow::Do(OpDescriptor *pOpDesc)
{
	ERROR2RAW("Don't call this!");
	End();
}


/********************************************************************************************
>	void OpApplyShadow::DoWithParam(OpDescriptor* pOp, OpParam* pParam)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/2004
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Applys a shadow to the selected node after a mouse click
********************************************************************************************/

void OpApplyShadow::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
//	BOOL ok = FALSE;

	NodeShadowParam* pNSParam = (NodeShadowParam*)pParam;

	INT32 InsertPos = pNSParam->StackPosition - 1;	// We want to find the surface below the current pos
													// because we are going to insert above that surface

	// obtain the current selection.
	SelRange* pSelRange = GetApplication()->FindSelection();

	Range* pInitialRange = NULL;

	// render blobs off for tools which don't automatically redraw their blobs.
	Tool* pTool = Tool::GetCurrent();
	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread != NULL && pTool != NULL && !pTool->AreToolBlobsRenderedOnSelection())
		pTool->RenderToolBlobs(pSpread, NULL);

	// record the current selection state and if required, render off any selection blobs.
	if (!DoStartSelOp(FALSE, FALSE))
	{
		End();
		return;
	}

	// invalidate the region bounding the selection.
	if (!DoInvalidateNodesRegions(*pSelRange, TRUE, FALSE, FALSE, FALSE))
	{
		End();
		return;
	}

	// Force text subselection up to the whole parent text object
	pSelRange->MakePartialSelectionWhole(FALSE);

	// Get a list of the LiveEffects we will be working on
	if (InsertPos==-1)
	{
		// We are to apply the LiveEFfects to the selection
		pInitialRange = GetApplication()->FindSelection();
	}
	else
	{
		ENSURE(pNSParam->pPPStack, "Can't find EffectsStack");
		pInitialRange = pNSParam->pPPStack->GetLevelRange(&InsertPos);
	}

	// Check whether we can edit an existing effect or whether we have to apply a new one
	if (pInitialRange==NULL)
	{
		ERROR3("Failed to get surface range to apply shadow too");
		FailAndExecute();
		End();
		return;
	}

	// Need to add a new LiveEffects node to the selection
	// Decide whether we need to create a group or can apply to
	// each selected object individually
	ListRange* pPPRange = NULL;
	pPPRange = DoApplyShadow(this, pInitialRange, pNSParam);

	// We should now have a decent list of NodeShadowController nodes...
	if (pPPRange==NULL)
	{
		FailAndExecute();
		End();
		return;
	}

	// invalidate the (new) region bounding the selection.
	DoInvalidateNodesRegions(*pSelRange, TRUE, FALSE, FALSE);

	// render blobs back on if the current tool doesn't automatically redraw its blobs.
	if (pSpread != NULL && pTool != NULL && !pTool->AreToolBlobsRenderedOnSelection())
		pTool->RenderToolBlobs(pSpread, NULL);

	End();

	// Make sure the Shadow tool is current... (this func can be called from other tools)
	Tool* pShadowTool = Tool::FindTool(TOOLID_SOFTSHADOW);
	if (pShadowTool && !pShadowTool->IsCurrent())
	{
		((SoftShadowTool*)pShadowTool)->SetShadowEditRange(pPPRange, STACKPOS_TOP, TRUE);	// We know this range is consistent because we just created it!
		pShadowTool->SetCurrent();
	}

	// We don't actually need this range...!
	delete pPPRange;
	pPPRange = NULL;

	BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::EFFECTSTACKCHANGED)); 

}



/********************************************************************************************
>	static ListRange* OpApplyShadow::DoApplyShadow(UndoableOperation* pOp, Range* pRange, NodeShadowParam* pNSParam)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/2004
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Applys a shadow to the specified range (range probably comes from a
				PostProcessor stack)

********************************************************************************************/

ListRange* OpApplyShadow::DoApplyShadow(UndoableOperation* pOp, Range* pRange, NodeShadowParam* pNSParam)
{
	ERROR3IF(pOp==NULL, "DoApplyShadow not given an operation to work in");
	ERROR3IF(pRange==NULL, "DoApplyShadow not given a range to work on");

//	BOOL ok = TRUE;

	ListRange* pNewRange = new ListRange();
	ENSURE(pNewRange, "DoApplyEffect can't create a ListRange");
	if (pNewRange==NULL)
		return NULL;

	Node* pNode = pRange->FindFirst();

	// For each node in the range
	// Create a new NodeShadowController and insert it into the tree
	while (pNode)
	{
		NodeShadowController* pShadControl = DoApplyShadow(pOp, pNode, pNSParam);
		if (pShadControl)
		{
			pNewRange->AddNode(pShadControl);
			
			// Make sure the attributes are optimised correctly
			BOOL ok = pOp->DoFactorOutCommonChildAttributes(pShadControl);
			if (!ok)
			{
				delete pNewRange;
				return NULL;
			}
		}
		else
		{
			delete pNewRange;
			return NULL;
		}
			
		pNode = pRange->FindNext(pNode);
	}
	
	return pNewRange;
}




/********************************************************************************************
>	static NodeShadowController* OpApplyShadow::DoApplyShadow(UndoableOperation* pOp, Node* pNode, NodeShadowParam* pNSParam, BOOL bSetSelected = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/2004
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Applys a shadow to the specified range (range probably comes from a
				PostProcessor stack)

********************************************************************************************/

NodeShadowController* OpApplyShadow::DoApplyShadow(UndoableOperation* pOp, Node* pNode, NodeShadowParam* pNSParam, BOOL bSetSelected)
{
	ERROR3IF(pOp==NULL, "DoApplyShadow not given an operation to work in");
	ERROR3IF(pNode==NULL, "DoApplyShadow not given a node to work on");

	BOOL ok = TRUE;
	NodeShadowController* pShadControl = NULL;

	ObjChangeFlags flgs(FALSE, FALSE, FALSE, FALSE);
	ObjChangeParam ocp(OBJCHANGE_STARTING, flgs, NULL, pOp, OBJCHANGE_CALLEDBYOP);

	if (pNode->AllowOp(&ocp))
	{
		ALLOC_WITH_FAIL(pShadControl, (new NodeShadowController()), pOp);
		if (pShadControl==NULL) goto ApplyShadowError;
		
		// Create a new NodeShadow, apply default attributes and insert it into the tree
		NodeShadow* pShadow = NULL; 
		ALLOC_WITH_FAIL(pShadow, (new NodeShadow()), pOp);
		if (pShadow==NULL) goto ApplyShadowError;

		ok = pOp->DoInsertNewNode(pShadControl, pNode, NEXT, FALSE, FALSE, FALSE, FALSE);
		if (!ok) goto ApplyShadowError;

		ok = pOp->DoInvalidateNodeRegion(pShadControl, TRUE, FALSE, FALSE, FALSE);	// Don't force recache - we've already handled that
		if (!ok) goto ApplyShadowError;
		
		// Insert a flat fill attribute onto the shadow node
		AttrFlatColourFill * pAttr = NULL;
		ALLOC_WITH_FAIL(pAttr, new AttrFlatColourFill(pShadow, FIRSTCHILD), pOp);
		if (pAttr==NULL) goto ApplyShadowError;

		DocColour ShadowCol;

		if (pNSParam->m_Type == SHADOWTYPE_GLOW)
			AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
												_R(IDS_REDNAME), &ShadowCol);
		else
			AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
												_R(IDS_BLACKNAME), &ShadowCol);
		pAttr->SetStartColour(&ShadowCol);

		// Run through the selected nodes moving them to be children of the shadow controller
		ok = pOp->DoMoveNode(pNode, pShadControl, LASTCHILD);
		if (!ok) goto ApplyShadowError;

		pNode->SetSelected(FALSE);		// Deselect node (deselect whole subtree)
		
		// insert the shadow node now
		ok = pOp->DoInsertNewNode(pShadow, pShadControl, FIRSTCHILD, FALSE, FALSE, FALSE, FALSE);
		if (!ok) goto ApplyShadowError;

		// Now that shadow controller is in the tree in the correct place...
		// Release cached data for our parents only! We want to retain child cached data where possible
		pShadControl->ReleaseCached(TRUE, FALSE);
#ifdef NEW_SHADOW_RENDER
		// To make multiple stacked shadowing work properly
		pShadControl->ReleaseStackedShadows();

		// Tidy up effects of applying one shadow on top of another
		if (pNode->IsAShadowController())
		{
			// Ensure previously selected shadows are all deselected
			NodeShadowController* pPrevTopController = (NodeShadowController*) pNode;
//			NodeShadow* pPrevTopShadow = pPrevTopController->GetShadow();

			// Copy effect attrs up because the prev top controller will no longer be rendered
			NodeAttribute* pAttr = NodeAttribute::FindFirstAppliedAttr(pPrevTopController);
			while (pAttr && pAttr->IsEffectAttribute())
			{
				pOp->DoMoveNode(pAttr, pShadControl, LASTCHILD);

				pAttr = NodeAttribute::FindFirstAppliedAttr(pPrevTopController);	// First again, because we just moved one!
			}
		}
#endif
		
		if (bSetSelected)
			pShadControl->FindBottomShadowController()->GetInkNodeFromController()->SetSelected(TRUE);

		ok = pOp->DoInvalidateNodeRegion(pShadow, TRUE, FALSE, FALSE, FALSE);	// Don't force recache - we've already handled that
		if (!ok) goto ApplyShadowError;
		
		
		// factor out the child attributes
		ok = pOp->DoFactorOutCommonChildAttributes(pShadControl);
		if (!ok) goto ApplyShadowError;
		
		// set all the attributes
		pShadControl->SetPenumbraWidth(pNSParam->m_Blur);
		pShadControl->SetOffsetX(pNSParam->m_OffsetX);
		pShadControl->SetOffsetY(pNSParam->m_OffsetY);
		pShadControl->SetShadowType(pNSParam->m_Type);
		pShadControl->SetFloorShadowAngle(pNSParam->m_FloorShadowAngle);
		pShadControl->SetFloorShadowHeight((float)pNSParam->m_FloorShadowHeight);
		pShadControl->SetWallShadowScale((float)pNSParam->m_Scale);
		pShadControl->SetGlowWidth(pNSParam->m_GlowWidth);
		pShadControl->SetFeatherWidth(pNSParam->m_FeatherWidth);
		pShadow->SetBiasGain(pNSParam->m_Profile);
		pShadow->SetDarkness(pNSParam->m_dDarkness);
		
		// set up the shadow
		((pShadControl)->GetShadow())->GenerateShadow();
		
		// TODO: Why?
		// reinitialise any blend which we may belong to.
		// this assumes that if we are directly blended, then our parent
		// must be a NodeBlend.
		if (pNode->IsAnObject())
		{
			NodeRenderableInk* pInk = (NodeRenderableInk*)pNode;
			Node* pTestBlend = pShadControl->FindParent();
			if (pTestBlend != NULL && pTestBlend->IsABlend())
				((NodeBlend*)pTestBlend)->ReinitialiseOnChildChanged(pOp, pInk, pShadControl);
		}

		// inform all parents
		// [Phil 26/08/2005: WHY??? Why not just let the normal UpdateChangedNodes call do this?]
		// [What a mess!]
		ObjChangeFlags flgs(FALSE, FALSE, FALSE, FALSE);
		flgs.RegenerateNode = TRUE;
		ObjChangeParam ocp2(OBJCHANGE_FINISHED, flgs, pShadControl, pOp, OBJCHANGE_CALLEDBYCHILD);
		
		Node *pParent = pShadControl->FindParent();
		
		while (pParent)
		{			
			pParent->OnChildChange(&ocp2);
			pParent = pParent->FindParent();
		}

		// TODO: Why?
		SliceHelper::AddNamesToController(pOp, pShadControl);
	}
			
	return pShadControl;

ApplyShadowError:
	delete pShadControl;
	return NULL;
}




/********************************************************************************************

>	OpState OpApplyShadow::GetState(String_256* Description, OpDescriptor*)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/2004
	Outputs:	-
	Returns:	Ungreyed, Unticked
	Purpose:	Find out the state of the new regular shape at the specific time

********************************************************************************************/
OpState OpApplyShadow::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;

	Blobby.Greyed = FALSE;

	return Blobby;
}




/********************************************************************************************

>	static OpState OpApplyShadow::GetParamState(String_256* pstrDescription, OpDescriptor* pOpDesc, OpParam* pOpParam)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	Return state of the specified LiveEffect (name in pstrDescription) being
				applied to the selection (taking into account the current position in the
				LiveEffect Stack.
	Errors:		
	See also:	

********************************************************************************************/
OpState OpApplyShadow::GetParamState(String_256* pstrDescription, OpDescriptor* pOpDesc, OpParam* pOpParam)
{
	// default is an unticked, *GREYED*, on-menu state.
	OpState OpSt;
	OpSt.Greyed = FALSE;

//	OpSt.Greyed = (XPEHost::IsEditSessionRunning());

	return OpSt;
}






/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//////////	OpChangeShadowType class
/////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************************************

>	BOOL OpChangeShadowType::Declare()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/2004
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/
BOOL OpChangeShadowType::Declare()
{
	BOOL ok = TRUE;

	// Use an ParamOpDescriptor to allow OpApplyLiveEffect to receive extra information
	// about the menu item that invoked it...
	ParamOpDescriptor* pOD2 = new ParamOpDescriptor(OPTOKEN_CHANGESHADOWTYPE,
														CC_RUNTIME_CLASS(OpChangeShadowType),
														OpChangeShadowType::GetParamState);
	ok = ok && (pOD2!=NULL);

	return ok;
}



/********************************************************************************************
>	OpChangeShadowType::OpChangeShadowType()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/2004
	Purpose:	Constructor.
********************************************************************************************/
OpChangeShadowType::OpChangeShadowType()
{
}



/********************************************************************************************
>	OpChangeShadowType::~OpChangeShadowType()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/2004
	Purpose:	Destructor.
********************************************************************************************/
OpChangeShadowType::~OpChangeShadowType()
{
}


/********************************************************************************************
>	virtual void OpChangeShadowType::Do(OpDescriptor *pOpDesc)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/2004
	Inputs:		pOpDesc - OpDescriptor - unused
	Outputs:	-
	Returns:	-
	Purpose:	The do function.  Applys a shadow to the selection in the current document
********************************************************************************************/
void OpChangeShadowType::Do(OpDescriptor *pOpDesc)
{
	ERROR2RAW("Don't call this!");
	End();
}


/********************************************************************************************
>	void OpChangeShadowType::DoWithParam(OpDescriptor* pOp, OpParam* pParam)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/2004
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Applys a shadow to the selected node after a mouse click
********************************************************************************************/

void OpChangeShadowType::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
//	BOOL ok = FALSE;

	NodeShadowParam* pNSParam = (NodeShadowParam*)pParam;

//	INT32 InsertPos = pNSParam->StackPosition - 1;	// We want to find the surface below the current pos
													// because we are going to insert above that surface

	// obtain the current selection.
	Range Sel(*(GetApplication()->FindSelection()));
	RangeControl rc = Sel.GetRangeControlFlags();
	rc.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rc);

//	Range* pInitialRange = NULL;

	// render blobs off for tools which don't automatically redraw their blobs.
	Tool* pTool = Tool::GetCurrent();
	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread != NULL && pTool != NULL && !pTool->AreToolBlobsRenderedOnSelection())
		pTool->RenderToolBlobs(pSpread, NULL);

	// record the current selection state and if required, render off any selection blobs.
	if (!DoStartSelOp(FALSE, FALSE))
	{
		End();
		return;
	}

	// invalidate the region bounding the selection.
	if (!DoInvalidateNodesRegions(Sel, TRUE, FALSE, FALSE, FALSE))
	{
		End();
		return;
	}

	// Force text subselection up to the whole parent text object
//	SelRange::MakeTextSelectionWhole(FALSE);

	// Need to add a new LiveEffects node to the selection
	// Decide whether we need to create a group or can apply to
	// each selected object individually
	BOOL bOK = DoChangeShadowType(this, pNSParam);

	// We should now have a decent list of LiveEffect nodes...
	if (!bOK)
	{
		FailAndExecute();
		End();
		return;
	}

	// invalidate the (new) region bounding the selection.
	DoInvalidateNodesRegions(*(GetApplication()->FindSelection()), TRUE, FALSE, FALSE, FALSE); // Assume DoChangeShadowType has released caches where needed

	// render blobs back on if the current tool doesn't automatically redraw its blobs.
	if (pSpread != NULL && pTool != NULL && !pTool->AreToolBlobsRenderedOnSelection())
		pTool->RenderToolBlobs(pSpread, NULL);

	End();
}



/********************************************************************************************
>	static BOOL OpChangeShadowType::DoChangeShadowType(UndoableOperation* pOp, Range* pRange, NodeShadowParam* pNSParam)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/2004
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Change the type of the specified range of shadows...

********************************************************************************************/

BOOL OpChangeShadowType::DoChangeShadowType(UndoableOperation* pOp, NodeShadowParam* pNSParam)
{
	ERROR3IF(pOp==NULL, "DoChangeShadowType not given an operation to work in");

//	BOOL ok = TRUE;

	ObjChangeFlags flgs(FALSE, FALSE, FALSE, FALSE);
	ObjChangeParam ocp(OBJCHANGE_STARTING, flgs, NULL, pOp, OBJCHANGE_CALLEDBYOP);

	ENSURE(pNSParam->pShadowTool, "Op not given pointer to shadow tool");
	Range* pShadowRange = pNSParam->pShadowTool->GetShadowEditRange();
	Node* pNode = pShadowRange->FindFirst();
	if (pNode==NULL)
	{
		ERROR3("Nothing was selected in OpChangeShadowType");
		return FALSE;
	}

	while (pNode)
	{
		ENSURE(pNode->IsAShadowController(), "Found unexpected node type");
		NodeShadowController* pController = (NodeShadowController*)pNode;

		if (pController->GetShadowType() != pNSParam->m_Type)
		{
//			Node *pParent = pController->FindParent();

			pController->InvalidateBoundingRect();
//			if (bRedraw)
			{
//				pOp->DoInvalidateNodeRegion(pController, TRUE);
				pController->ReleaseCached(TRUE, FALSE, TRUE, TRUE);
				pOp->DoInvalidateNodeRegion(pController, TRUE, FALSE, FALSE, FALSE);	// Don't force recache - we've already handled that
				pOp->DoInvalidateRegion(pController->FindParentSpread(), pController->GetBlobBoundingRect());
			}

			ChangeShadowTypeAction* Action = NULL;
			ChangeShadowTypeAction::Init(pOp, pOp->GetUndoActionList(), pController, pNSParam->m_Type, &Action);
//			m_OpName = _R(IDS_CHANGESHADOWTYPEOPNAME);

			pController->InvalidateBoundingRect();
//			if (bRedraw)
			{
//				pOp->DoInvalidateNodeRegion(pController, TRUE);
				pController->ReleaseCached(TRUE, FALSE, TRUE, TRUE);
				pOp->DoInvalidateNodeRegion(pController, TRUE, FALSE, FALSE, FALSE);	// Don't force recache - we've already handled that
				pOp->DoInvalidateRegion(pController->FindParentSpread(), pController->GetBlobBoundingRect());
			}
		}

		pNode = pShadowRange->FindNext(pNode);
	}

	return TRUE;
}

/********************************************************************************************

>	OpState OpChangeShadowType::GetState(String_256* Description, OpDescriptor*)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/2004
	Outputs:	-
	Returns:	Ungreyed, Unticked
	Purpose:	Find out the state of the new regular shape at the specific time

********************************************************************************************/
OpState OpChangeShadowType::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;

	Blobby.Greyed = FALSE;

	return Blobby;
}



/********************************************************************************************

>	static OpState OpChangeShadowType::GetParamState(String_256* pstrDescription, OpDescriptor* pOpDesc, OpParam* pOpParam)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	Return state of the specified LiveEffect (name in pstrDescription) being
				applied to the selection (taking into account the current position in the
				LiveEffect Stack.
	Errors:		
	See also:	

********************************************************************************************/
OpState OpChangeShadowType::GetParamState(String_256* pstrDescription, OpDescriptor* pOpDesc, OpParam* pOpParam)
{
	// default is an unticked, *GREYED*, on-menu state.
	OpState OpSt;
	OpSt.Greyed = FALSE;

//	OpSt.Greyed = (XPEHost::IsEditSessionRunning());

	return OpSt;
}



/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//////////	OpRemoveShadow class
/////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************************************
>	OpRemoveShadow::OpRemoveShadow()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/96
	Purpose:	Constructor.
********************************************************************************************/
OpRemoveShadow::OpRemoveShadow()
{
}



/********************************************************************************************
>	OpRemoveShadow::~OpRemoveShadow()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/96
	Purpose:	Destructor.
********************************************************************************************/
OpRemoveShadow::~OpRemoveShadow()
{
}


/********************************************************************************************
>	virtual void OpRemoveShadow::DoWithParam(OpDescriptor *pOpDesc, OpParam* pParam)
	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/96
	Inputs:		pOpDesc - OpDescriptor - unused
	Outputs:	-
	Returns:	-
	Purpose:	The do function.  Applys a shadow to the selection in the current document
********************************************************************************************/
void OpRemoveShadow::DoWithParam(OpDescriptor *pOpDesc, OpParam* pParam)
{
	// Start a slow job
	BeginSlowJob();

	// Get the selection 
	NodeShadowParam* pNSParam = (NodeShadowParam*)pParam;

	BOOL ok = TRUE;
	// Start the SelectionOp
	if (ok) ok = DoStartSelOp(TRUE);

	NodeBlend * pBlend = NULL;
	NodeBlender * pBlender = NULL;
	NodeRenderableInk * pRenderableNode = NULL;

	BOOL bBlendBefore = FALSE;
	BOOL bBlendAfter  = FALSE;

	ENSURE(pNSParam->pShadowTool, "Op not given pointer to shadow tool");

	// We need to copy the range because we are about to fiddle with the document
	// tree and that could cause the EditRange to be uncached
	ListRange* pShadowRange = new ListRange(pNSParam->pShadowTool->GetShadowEditRange());
	Node* pSCNode = pShadowRange->FindFirst();
	while (pSCNode)
	{
		if (pSCNode->IS_KIND_OF(NodeShadowController) && pSCNode->FindParent()!=NULL)
		{
			Node* pNode = pSCNode;
			NodeShadowController* pControl = (NodeShadowController*) pSCNode;
			
			// are we part of a blend ? if so, make a record of the blender object
			// and whether we're before or after it
			pBlender = (NodeBlender *)pNode->FindNext(CC_RUNTIME_CLASS(NodeBlender));

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
				pBlender = (NodeBlender *)pNode->FindPrevious(CC_RUNTIME_CLASS(NodeBlender));

				if (pBlender)
				{
					bBlendAfter = TRUE;
					pBlend = (NodeBlend *)pBlender->FindParent();
				}
			}

			
			// We now have a selected NodeShadowController node. Here's what we do:
			//		Invalidate the region
			//		Hide the NodeShadowController
			//		Move all other nodes next to the place the NodeShadowContrller node used to be (select them too)

			// localise all attributes
			if (ok) ok = DoLocaliseCommonAttributes((NodeShadowController *)pNode, TRUE);

			NodeHidden* pNodeHidden;
			NodeShadowController* pNodeShadCont = (NodeShadowController*)pNode;

			// Will the shadowconller node allow us to do the op to it?
			//ok = pNodeShadCont->AllowOp(&ObjChange);
			if (ok)
				SliceHelper::RemoveNamesFromController(this, pNodeShadCont);

			// Invalidate the whole blend region
//			if (ok) ok = DoInvalidateNodeRegion(pNodeShadCont,TRUE,FALSE);
			if (ok)
			{
				pNodeShadCont->ReleaseCached(TRUE, FALSE, TRUE, TRUE);
				ok = DoInvalidateNodeRegion(pNodeShadCont, TRUE, FALSE, FALSE, FALSE);	// Don't force recache - we've already handled that
			}

			// move the children
			pNode = pNodeShadCont->FindFirstChild();

			while (pNode != NULL && ok)
			{
				Node* pNext = pNode->FindNext();

				if (pNode->IsAnObject() && !pNode->IS_KIND_OF(NodeShadow))
					ok = DoMoveNode(pNode,pNodeShadCont,PREV);			// Move next to hidden node (where shadowcont node used to be)
				// reselect the node
				if (ok) pNode->SetSelected(TRUE);
				pNode = pNext;
			}


			// Firstly, hide the nodeshadowcontroller, and deselect it
			if (ok) pNodeShadCont->SetSelected(FALSE);

			Node * pParent = pNodeShadCont->FindParent();
			
			if (ok) ok = DoHideNode(pNodeShadCont,TRUE,&pNodeHidden,FALSE);

			if (ok)
			{
				// do blends
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
							if (ptrNode->GetNodeStart() == pControl)
							{
								if (InitBlendAction::InitOnBlender(this, GetUndoActionList(), ptrNode, TRUE, TRUE) != AC_OK)
								{
									ERROR2RAW("Couldn't Initialise blend action");
								}

								// reinit the node
								//pRenderableNode = (NodeRenderableInk *)ptrNode->FindPrevious(CC_RUNTIME_CLASS(NodeRenderableInk)); 
								
								if (pRenderableNode)
								{
									ptrNode->Reinit(pRenderableNode, NULL, FALSE);
								}
							}
							if (ptrNode->GetNodeEnd() == pControl)
							{
								if (InitBlendAction::InitOnBlender(this, GetUndoActionList(),  ptrNode, TRUE, TRUE) != AC_OK)
								{
									ERROR2RAW("Couldn't Initialise blend action");
								}
								
								// reinit the node
								//pRenderableNode = (NodeRenderableInk *)ptrNode->FindNext(CC_RUNTIME_CLASS(NodeRenderableInk)); 

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

			// tell all parents
			if (ok)
			{
				ObjChangeFlags flgs(FALSE, FALSE, FALSE, FALSE);
				flgs.DeleteNode = TRUE;
				flgs.RegenerateNode = TRUE;
				ObjChangeParam OP(OBJCHANGE_FINISHED, flgs, pNodeShadCont, this, OBJCHANGE_CALLEDBYCHILD);

				while (pParent)
				{
					if (pParent->OnChildChange(&OP) != CC_OK)
						break;

					if (pParent->IS_KIND_OF(NodeBlend))
					{
						((NodeBlend *)pParent)->Deinit();
						((NodeBlend *)pParent)->Reinit(FALSE);
					}

					pParent = pParent->FindParent();
				}
			}
		}
		
		pSCNode = pShadowRange->FindNext(pSCNode);
	}

	if (pShadowRange)
	{
		delete pShadowRange;
		pShadowRange = NULL;
	}

	if (!ok)
		FailAndExecute();

	End();	
}


/********************************************************************************************

>	BOOL OpRemoveShadow::Declare()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/96
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/
BOOL OpRemoveShadow::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_REMOVESHADOWOP),
								CC_RUNTIME_CLASS(OpRemoveShadow), 
								OPTOKEN_REMOVESHADOW,
								OpRemoveShadow::GetState));

}


/********************************************************************************************

>	OpState OpRemoveShadow::GetState(String_256* Description, OpDescriptor*)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/96
	Outputs:	-
	Returns:	Ungreyed, Unticked
	Purpose:	Find out the state of the new regular shape at the specific time

********************************************************************************************/
OpState OpRemoveShadow::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//////////	OpShadowPosition code
/////////////////////////////////////////////////////////////////////////////////////////////

ShadowPosition::ShadowPosition()
{
	X = 0;
	Y = 0;
	m_bUpdateX = 0;
	m_bUpdateY = 0;
	m_pTool = NULL;
}

/********************************************************************************************

>	ShadowPosition::ShadowPosition(const INT32 x, const INT32 y)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/12/96

********************************************************************************************/

ShadowPosition::ShadowPosition(const INT32 x, const INT32 y,
							   BOOL UpdateX, BOOL UpdateY, SoftShadowTool* pTool)
{
	X = x;
	Y = y;
	m_bUpdateX = UpdateX;
	m_bUpdateY = UpdateY;
	m_pTool = pTool;
}

/********************************************************************************************

>	ShadowPosition::~ShadowPosition()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/12/96

********************************************************************************************/

ShadowPosition::~ShadowPosition()
{
}


/********************************************************************************************

>	OpShadowPosition::OpShadowPosition()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/96
	Outputs:	-
	Returns:	-
	Purpose:	constructor

********************************************************************************************/
OpShadowPosition::OpShadowPosition()
{
}

/********************************************************************************************

>	OpShadowPosition::~OpShadowPosition()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/96
	Outputs:	-
	Returns:	-
	Purpose:	destructor

********************************************************************************************/
OpShadowPosition::~OpShadowPosition()
{
}


/********************************************************************************************

>	BOOL OpShadowPosition::Declare()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/96
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpShadowPosition::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_POSITIONSHADOWOP),
								CC_RUNTIME_CLASS(OpShadowPosition), 
								OPTOKEN_SHADOWPOSITION,
								OpShadowPosition::GetState));
}

/********************************************************************************************

>	OpShadowPosition::DoWithParam(OpDescriptor* pOp, OpParam* pParam)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/96
	Outputs:	
	Returns:	
	Purpose:	

********************************************************************************************/
void OpShadowPosition::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	// Start a slow job
	BeginSlowJob();

	ShadowPosition *MyShadowPosition = (ShadowPosition *)pParam;
	ERROR3IF(MyShadowPosition == NULL, "Parameter block pointer was NULL");

	if (MyShadowPosition == NULL)
	{
		End();
		return;
	}

	ENSURE(MyShadowPosition->m_pTool, "Op not given pointer to shadow tool");
	Range* pShadowRange = MyShadowPosition->m_pTool->GetShadowEditRange();
	Node* pNode = pShadowRange->FindFirst();
	if (pNode==NULL)
	{
		ERROR3("Nothing was selected in OpShadowPosition");
		FailAndExecute();
		End();
		return;
	}
	while (pNode)
	{
		ENSURE(pNode->IsAShadowController(), "Found unexpected node type");
		NodeShadowController* pController = (NodeShadowController*)pNode;

		ChangePositionShadowAction* pAction = NULL;
			
//		DoInvalidateNodeRegion(pController->GetShadow(), TRUE);
		pController->ReleaseCached(TRUE, FALSE, TRUE, TRUE);
		DoInvalidateNodeRegion(pController->GetShadow(), TRUE, FALSE, FALSE, FALSE);	// Don't force recache - we've already handled that
		
		// set the new penumbra
		ActionCode ac = ChangePositionShadowAction::Init(this,
												  &UndoActions,pController,
												  MyShadowPosition->X,
												  MyShadowPosition->Y,
												  MyShadowPosition->m_bUpdateX,
												  MyShadowPosition->m_bUpdateY,
												  &pAction
												 );
		if (ac==AC_FAIL)
		{
			FailAndExecute();
			End();
			return;
		}
		
//		DoInvalidateNodeRegion(pController->GetShadow(), TRUE);
		pController->ReleaseCached(TRUE, FALSE, TRUE, TRUE);
		DoInvalidateNodeRegion(pController->GetShadow(), TRUE, FALSE, FALSE, FALSE);	// Don't force recache - we've already handled that

		// do the object change on the node
		ObjChangeFlags flgs(FALSE, FALSE, FALSE, TRUE);
		ObjChangeParam OP(OBJCHANGE_FINISHED, flgs, pController, this, OBJCHANGE_CALLEDBYOP);
		flgs.RegenerateNode = TRUE;
		Node * pNodeParent = pController->FindParent();

		while (pNodeParent)
		{
			pNodeParent->OnChildChange(&OP);

			pNodeParent = pNodeParent->FindParent();
		}

		pNode = pShadowRange->FindNext(pNode);
	}
	
	End();	
}


/********************************************************************************************

>	OpState OpShadowPosition::GetState(String_256* Description, OpDescriptor*)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/96
	Outputs:	-
	Returns:	Ungreyed, Unticked
	Purpose:	Find out the state of the new regular shape at the specific time

********************************************************************************************/
OpState OpShadowPosition::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//////////	OpShadowPenumbra code
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////


ShadowPenumbra::ShadowPenumbra()
{
	P = 0;
	m_pTool = NULL;
}

/********************************************************************************************

>	ShadowPenumbra::~ShadowPenumbra()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/96
	Outputs:	-
	Returns:	-
	Purpose:	ShadowPenumbra destructor

********************************************************************************************/

ShadowPenumbra::~ShadowPenumbra()
{
}

/********************************************************************************************

>	ShadowPenumbra::ShadowPenumbra(const INT32 p, SoftShadowTool* pTool)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/96
	Inputs:		p - new size of the penumbra
	Outputs:	-
	Returns:	-
	Purpose:	ShadowPenumbra constructor

********************************************************************************************/

ShadowPenumbra::ShadowPenumbra(const INT32 p, SoftShadowTool* pTool)
{
	P = p;
	m_pTool = pTool;
}


OpShadowPenumbra::OpShadowPenumbra()
{
}

OpShadowPenumbra::~OpShadowPenumbra()
{
}

/********************************************************************************************

>	BOOL OpShadowPenumbra::Declare()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/96
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpShadowPenumbra::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_PENUMBRASHADOWOP),
								CC_RUNTIME_CLASS(OpShadowPenumbra), 
								OPTOKEN_SHADOWPENUMBRA,
								OpShadowPenumbra::GetState));
}


/********************************************************************************************

>	OpState OpShadowEnumbra::GetState(String_256* Description, OpDescriptor*)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/96
	Outputs:	-
	Returns:	Ungreyed, Unticked
	Purpose:	Find out the state of the new regular shape at the specific time

********************************************************************************************/
OpState OpShadowPenumbra::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}


/********************************************************************************************
x
>	OpShadowPenumbra::DoWithParam(OpDescriptor* pOp, OpParam* pParam)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/96
	Outputs:	
	Returns:	
	Purpose:	

********************************************************************************************/
void OpShadowPenumbra::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	// Start a slow job
	BeginSlowJob();

	ShadowPenumbra *MyShadowPenumbra = (ShadowPenumbra *)pParam;
	ERROR3IF(MyShadowPenumbra == NULL, "Parameter block pointer was NULL");

	if (MyShadowPenumbra == NULL)
	{
		End();
		return;
	}

	// Get the selection
	ENSURE(MyShadowPenumbra->m_pTool, "Op not given pointer to shadow tool");
	Range* pShadowRange = MyShadowPenumbra->m_pTool->GetShadowEditRange();
	Node* pNode = pShadowRange->FindFirst();
	if (pNode==NULL)
	{
		ERROR3("Nothing was selected in OpShadowPenumbra");
		FailAndExecute();
		End();
		return;
	}
	while (pNode)
	{
		ENSURE(pNode->IsAShadowController(), "Found unexpected node type");
		NodeShadowController* pController = (NodeShadowController*)pNode;

		ChangePenumbraSizeAction* pAction = NULL;
		
		// Invalidate the region
//		DoInvalidateNodeRegion(pController, TRUE);
		pController->ReleaseCached(TRUE, FALSE, TRUE, TRUE);
		DoInvalidateNodeRegion(pController, TRUE, FALSE, FALSE, FALSE);	// Don't force recache - we've already handled that
		
		ActionCode ac = ChangePenumbraSizeAction::Init(this,
														&UndoActions,
														pController,
														MyShadowPenumbra->P,
														&pAction
														);
		if (ac==AC_FAIL)
		{
			FailAndExecute();
			End();
			return;
		}
		
//		DoInvalidateNodeRegion(pController, TRUE);
		pController->ReleaseCached(TRUE, FALSE, TRUE, TRUE);
		DoInvalidateNodeRegion(pController, TRUE, FALSE, FALSE, FALSE);	// Don't force recache - we've already handled that

		// do the object change on the node
		ObjChangeFlags flgs(FALSE, FALSE, FALSE, TRUE);
		ObjChangeParam OP(OBJCHANGE_FINISHED, flgs, pController, this, OBJCHANGE_CALLEDBYOP);

		Node* pNodeParent = pController->FindParent();

		while (pNodeParent)
		{
			pNodeParent->OnChildChange(&OP);

			pNodeParent = pNodeParent->FindParent();
		}

		pNode = pShadowRange->FindNext(pNode);
	}

	End();	
}



//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The ChangePositionShadowAction class

/********************************************************************************************

>	ChangePositionShadowAction::ChangePositionShadowAction()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ChangePositionShadowAction::ChangePositionShadowAction()
{
	m_pNodeShadowCont  = NULL;
	m_OldPositionX = 0;
	m_OldPositionY = 0;
	m_bUpdateX = 0;
	m_bUpdateY = 0;
}


/********************************************************************************************

>	ActionCode ChangePositionShadowAction::Init( 	Operation* 	pOp,
												ActionList* pActionList,
												NodeShadowCont* 	pThisNodeShadow,
												ChangePositionShadowXAction** 	ppNewAction);

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/96
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pThisNodeShadow	= ptr to NodeShadow to change 
				PosShadowX	 	= New position X to apply to pThisNodeShadow
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



ActionCode ChangePositionShadowAction::Init(Operation* pOp,
										ActionList* pActionList,
										NodeShadowController* pThisNodeShadow,
										MILLIPOINT NewOffsetX,
										MILLIPOINT NewOffsetY,
										BOOL UpdateX,
										BOOL UpdateY,
										ChangePositionShadowAction** ppNewAction,
										BOOL Reverse)
{
	UINT32 ActSize = sizeof(ChangePositionShadowAction);

	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(ChangePositionShadowAction),(Action**)ppNewAction);

	MILLIPOINT OldOffsetX = pThisNodeShadow->GetOffsetX();
	MILLIPOINT OldOffsetY = pThisNodeShadow->GetOffsetY();

	if (Ac != AC_FAIL)
	{
		(*ppNewAction)->m_pNodeShadowCont  = pThisNodeShadow;

		// first thing we need to do is move my action to the start of the undo list
		// this is because we want to regen after all other actions have occurred
		// DMc
		/*
		if (Reverse)
		{
			pActionList->RemoveItem(*ppNewAction);
			pActionList->AddHead(*ppNewAction);
		}
		*/
		
		if (UpdateX)
			pThisNodeShadow->SetOffsetX(NewOffsetX);
		else
			NewOffsetX = pThisNodeShadow->GetOffsetX();

		if (UpdateY)
			pThisNodeShadow->SetOffsetY(NewOffsetY);
		else
			NewOffsetY = pThisNodeShadow->GetOffsetY();

		// and set it in the history
		(*ppNewAction)->m_OldPositionX = OldOffsetX;
		(*ppNewAction)->m_OldPositionY = OldOffsetY;
		(*ppNewAction)->m_bUpdateX = UpdateX;
		(*ppNewAction)->m_bUpdateY = UpdateY;

		TRACEUSER( "DavidM", _T("Moving shadow by %d %d, %d %d\n"),
			NewOffsetX, NewOffsetY, OldOffsetX, OldOffsetY);

		pThisNodeShadow->SetWallShadowOffset(DocCoord(NewOffsetX, NewOffsetY), TRUE);
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode ChangePositionShadowAction::Execute();

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

ActionCode ChangePositionShadowAction::Execute()
{
	ActionCode Act;
	ChangePositionShadowAction* pAction;

	Act = ChangePositionShadowAction::Init(	pOperation, 
										pOppositeActLst,
										m_pNodeShadowCont,
										m_OldPositionX, m_OldPositionY,
										m_bUpdateX, m_bUpdateY,
										&pAction,
										TRUE);

	// and set the matrix
	if (Act != AC_FAIL)
	{
		(m_pNodeShadowCont)->SetOffsetX(m_OldPositionX);
		(m_pNodeShadowCont)->SetOffsetY(m_OldPositionY);
	}

	return Act;
}

ChangePositionShadowAction::~ChangePositionShadowAction()
{
}


//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The ChangePenumbraSizeAction class

/********************************************************************************************

>	ChangePenumbraSizeAction::ChangePenumbraSizeAction()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ChangePenumbraSizeAction::ChangePenumbraSizeAction()
{
	m_pNodeShadowCont  = NULL;
	m_OldPenumbraSize = 0;
}


/********************************************************************************************

>	ActionCode ChangePenumbraSizeAction::Init( 	Operation* 	pOp,
												ActionList* pActionList,
												NodeShadow* 	pThisNodeShadow,
												MILLIPOINT	PenumbraSize,
												ChangePositionShadowXAction** 	ppNewAction);

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/96
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pThisNodeShadow	= ptr to NodeShadow to change 
				PenumbraSize	= New position X to apply to pThisNodeShadow
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



ActionCode ChangePenumbraSizeAction::Init(Operation* pOp,
										ActionList* pActionList,
										NodeShadowController* pThisNodeShadow,
										MILLIPOINT NewPenumbraSize,
										ChangePenumbraSizeAction** ppNewAction,
										BOOL bReverse,
										BOOL bCache)
{
	UINT32 ActSize = sizeof(ChangePenumbraSizeAction);

	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(ChangePenumbraSizeAction),(Action**)ppNewAction);

	Document * pDoc = Document::GetCurrent();

	DocView * pView = DocView::GetCurrent();

	Spread * pSpread = NULL;

	if (Ac != AC_FAIL)
	{
		// reverse the action
		// DMc
		if (bReverse)
		{
			pActionList->RemoveItem(*ppNewAction);
			pActionList->AddHead(*ppNewAction);
			(*ppNewAction)->m_bCache = bCache;
		}

		pSpread = (Spread *)pThisNodeShadow->FindParent(CC_RUNTIME_CLASS(Spread));

		if (!pSpread)
			pSpread = Document::GetSelectedSpread();

		if (pDoc)
		{
			if (pSpread)
			{
				pThisNodeShadow->ReleaseCached(TRUE, FALSE, TRUE, TRUE);		// Fix for 11304
				pDoc->ForceRedraw(pSpread, pThisNodeShadow->GetBoundingRect(FALSE, FALSE), FALSE, pThisNodeShadow, FALSE);
			}
		}
		
		(*ppNewAction)->m_pNodeShadowCont  = pThisNodeShadow;
		(*ppNewAction)->m_OldPenumbraSize = pThisNodeShadow->GetPenumbraWidth();

		pThisNodeShadow->SetPenumbraWidth(NewPenumbraSize);
		// ((pThisNodeShadow)->GetShadow())->SetCurrentBlur(NewPenumbraSize);

		if (pView && bCache)
		{
			GetApplication()->AddNodeToRegenList(pThisNodeShadow);
		}
		else
		{			
			((pThisNodeShadow)->GetShadow())->GenerateShadow();
		}

		if (pDoc)
		{
			if (pSpread)
			{
				pThisNodeShadow->ReleaseCached(TRUE, FALSE, TRUE, TRUE);		// Fix for 11304
				pDoc->ForceRedraw(pSpread, pThisNodeShadow->GetBoundingRect(FALSE, FALSE), FALSE, pThisNodeShadow, FALSE);
				pDoc->ForceRedraw(pSpread, pThisNodeShadow->GetBoundingRect(TRUE, FALSE), FALSE, pThisNodeShadow, FALSE);
			}
		}
		
		(*ppNewAction)->m_LastRect = (*ppNewAction)->m_pNodeShadowCont->GetBoundingRect();
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode ChangePenumbraSizeAction::Execute();

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/96
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the num blend steps in pThisNodeBlend to OldNumSteps,
				after creating another action to record the current num steps of pThisNodeBlend
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangePenumbraSizeAction::Execute()
{
	ActionCode Act;
	ChangePenumbraSizeAction* pAction;

	// force a redraw of the region
	DocRect dr = m_LastRect;

	Act = ChangePenumbraSizeAction::Init(	pOperation, 
										pOppositeActLst,
										m_pNodeShadowCont,
										m_OldPenumbraSize,
										&pAction, TRUE, m_bCache
										);

	Spread * pSpread = NULL;

	if (Act != AC_FAIL)
	{
		Document * pDoc = Document::GetCurrent();

		pSpread = (Spread *)m_pNodeShadowCont->FindParent(CC_RUNTIME_CLASS(Spread));

		if (!pSpread)
			pSpread = Document::GetSelectedSpread();
		
		if (pDoc)
		{
			m_LastRect = m_pNodeShadowCont->GetBoundingRect();

			if (pSpread)
			{
				pDoc->ForceRedraw(pSpread, 
					m_LastRect, FALSE, m_pNodeShadowCont);
			}
		}
	}
	
	return Act;
}

ChangePenumbraSizeAction::~ChangePenumbraSizeAction()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//////////	NodeShadowClick class
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************************************
>	NodeShadowClick::NodeShadowClick(Node *p)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/01/97
	Purpose:	When a user click, used by SoftShadowTool::DoWithParam()
********************************************************************************************/

NodeShadowClick::NodeShadowClick()
{
	m_pNode = NULL;
	m_pTool = NULL;
}

NodeShadowClick::NodeShadowClick(Node *p, SoftShadowTool* pTool)
{
	m_pNode = p;
	m_pTool = pTool;
}

NodeShadowClick::~NodeShadowClick()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//////////	OpApplyAttribForShadows class
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************************************

>	BOOL OpApplyAttribForShadows::DoApplyFillAttributeToShadow(Node * CurrentNode, 
		NodeAttribute * Attrib, BOOL * AttribWasRequired = NULL)


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/95
	Inputs:		-
	Returns:	TRUE if all ok.
	Purpose:	Same as DoApply only for shadow nodes
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL OpApplyAttribForShadows::DoApply(Node* CurrentNode, 
							NodeAttribute* Attrib, 
							BOOL Mutate, 
							BOOL InvalidateRegion,
							BOOL KeepExistingCols, /* = TRUE */
							BOOL* AttribWasRequired    /* = NULL */
							)
{
	if (!CurrentNode->IsKindOf(CC_RUNTIME_CLASS(NodeShadowController)) ||
		(!Attrib->IsKindOf(CC_RUNTIME_CLASS(AttrFillGeometry)) &&
		!Attrib->IsKindOf(CC_RUNTIME_CLASS(AttrValueChange))))
	{
		return OpApplyAttrib::DoApply(CurrentNode, Attrib, Mutate, InvalidateRegion,
			KeepExistingCols, AttribWasRequired);
	}
	
	// do the apply for all nodes under me
	Node * pNode = CurrentNode->FindFirstChild();

	BOOL bOk = TRUE;

	while (pNode && bOk)
	{
		if (pNode->IsRenderable() && !pNode->IsAnAttribute())
		{
			if (!pNode->IsKindOf(CC_RUNTIME_CLASS(NodeShadow)))
			{
				bOk = OpApplyAttrib::DoApply(pNode, Attrib, Mutate, InvalidateRegion,
					KeepExistingCols, AttribWasRequired);
			}
		}

		pNode = pNode->FindNext();
	}
	
	return bOk;
}


/********************************************************************************************

>	BOOL OpApplyAttribForShadows::DoApplyFillAttributeToShadow(Node * CurrentNode, 
		NodeAttribute * Attrib, BOOL * AttribWasRequired = NULL)


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/95
	Inputs:		-
	Returns:	TRUE if all ok.
	Purpose:	Alias init function
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL OpApplyAttribForShadows::Init()
{
	// Register the opdescriptors for the OpApplyAttribToSelected operation
	OpDescriptor* OpDesc = new OpDescriptor(
								 			0,
								 			_R(IDS_APPLYATTRIBOP), 				
											CC_RUNTIME_CLASS(OpApplyAttribForShadows),
								 			OPTOKEN_SHADOWATTRIB,
								 			OpApplyAttribToSelected::GetState);	

	ERRORIF(!OpDesc, _R(IDE_NOMORE_MEMORY), FALSE);
	return(OpDesc != NULL);
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The ChangeFloorShadowHeightAction class

/********************************************************************************************

>	ChangeFloorShadowHeightAction::ChangeFloorShadowHeightAction()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ChangeFloorShadowHeightAction::ChangeFloorShadowHeightAction()
{
	m_pNodeShadowCont  = NULL;
	m_LastHeight = 0;
}


/********************************************************************************************

>	ActionCode ChangeFloorShadowHeightAction::Init( 	Operation* 	pOp,
												ActionList* pActionList,
												NodeShadow* 	pThisNodeShadow,
												MILLIPOINT	PenumbraSize,
												ChangePositionShadowXAction** 	ppNewAction);

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/96
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pThisNodeShadow	= ptr to NodeShadow to change 
				PenumbraSize	= New position X to apply to pThisNodeShadow
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



ActionCode ChangeFloorShadowHeightAction::Init(Operation* pOp,
										ActionList* pActionList,
										NodeShadowController* pThisNodeShadow,
										double NewHeight,
										ChangeFloorShadowHeightAction** ppNewAction,
										BOOL bReverse,
										BOOL bCache)
{
	UINT32 ActSize = sizeof(ChangeFloorShadowHeightAction);

	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,
		CC_RUNTIME_CLASS(ChangeFloorShadowHeightAction),
		(Action**)ppNewAction);

	Document * pDoc = Document::GetCurrent();

	if (Ac != AC_FAIL)
	{
		// reverse the action
		// DMc
		if (bReverse)
		{
			pActionList->RemoveItem(*ppNewAction);
			pActionList->AddHead(*ppNewAction);
		}
		
		(*ppNewAction)->m_pNodeShadowCont  = pThisNodeShadow;
		(*ppNewAction)->m_LastHeight = pThisNodeShadow->GetFloorShadowHeight();
		(*ppNewAction)->m_bCache = bCache;

		pThisNodeShadow->SetFloorShadowHeight((float)NewHeight);

		if (pDoc)
		{
			pDoc->ForceRedraw(pThisNodeShadow->FindParentSpread(), pThisNodeShadow->GetBlobBoundingRect(), FALSE, pThisNodeShadow);
		}

		pThisNodeShadow->RegenerateNode(NULL, FALSE);

		if (pDoc)
		{
			pDoc->ForceRedraw(pThisNodeShadow->FindParentSpread(), pThisNodeShadow->GetBlobBoundingRect(), FALSE, pThisNodeShadow);
		}
		
		(*ppNewAction)->m_LastRect = (*ppNewAction)->m_pNodeShadowCont->GetBoundingRect();	
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode ChangeFloorShadowHeightAction::Execute();

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/96
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the num blend steps in pThisNodeBlend to OldNumSteps,
				after creating another action to record the current num steps of pThisNodeBlend
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeFloorShadowHeightAction::Execute()
{
	ActionCode Act;
	ChangeFloorShadowHeightAction* pAction;

	// force a redraw of the region
	DocRect dr = m_LastRect;

	Act = ChangeFloorShadowHeightAction::Init(	pOperation, 
										pOppositeActLst,
										m_pNodeShadowCont,
										m_LastHeight,
										&pAction,
										TRUE,
										m_bCache);

	if (Act != AC_FAIL)
	{
		Document * pDoc = Document::GetCurrent();
		
		if (pDoc)
		{
			m_LastRect = m_pNodeShadowCont->GetShadow()->GetBoundingRect();
			pDoc->ForceRedraw(m_pNodeShadowCont->GetShadow()->FindParentSpread(), 
				m_LastRect, FALSE, m_pNodeShadowCont);
		}
	}
	
	return Act;
}

ChangeFloorShadowHeightAction::~ChangeFloorShadowHeightAction()
{
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The ChangeFloorShadowAngleAction class

/********************************************************************************************

>	ChangeFloorShadowAngleAction::ChangeFloorShadowAngleAction()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ChangeFloorShadowAngleAction::ChangeFloorShadowAngleAction()
{
	m_pNodeShadowCont  = NULL;
	m_LastAngle = 0;
}


/********************************************************************************************

>	ActionCode ChangeFloorShadowAngleAction::Init( 	Operation* 	pOp,
												ActionList* pActionList,
												NodeShadow* 	pThisNodeShadow,
												MILLIPOINT	PenumbraSize,
												ChangePositionShadowXAction** 	ppNewAction);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pThisNodeShadow	= ptr to NodeShadow to change 
				PenumbraSize	= New position X to apply to pThisNodeShadow
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



ActionCode ChangeFloorShadowAngleAction::Init(Operation* pOp,
										ActionList* pActionList,
										NodeShadowController* pThisNodeShadow,
										INT32 NewAngle,
										ChangeFloorShadowAngleAction** ppNewAction,
										BOOL bReverse, BOOL bCache)
{
	UINT32 ActSize = sizeof(ChangeFloorShadowAngleAction);

	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,
		CC_RUNTIME_CLASS(ChangeFloorShadowAngleAction),
		(Action**)ppNewAction);


	Document * pDoc = Document::GetCurrent();	

	DocRect dr;

	if (Ac != AC_FAIL)
	{
		// reverse the action
		// DMc
		if (bReverse)
		{
			pActionList->RemoveItem(*ppNewAction);
			pActionList->AddHead(*ppNewAction);
		}
		
		(*ppNewAction)->m_pNodeShadowCont  = pThisNodeShadow;
		(*ppNewAction)->m_LastAngle = (INT32)pThisNodeShadow->GetFloorShadowAngle();
		(*ppNewAction)->m_bCache = bCache;

		// set the values, invalidate and regenerate
		pThisNodeShadow->InvalidateBoundingRect();

		dr = pThisNodeShadow->GetBoundingRect();
		TRACEUSER( "DavidM", _T("Rect before %d %d %d %d\n"),
			dr.lo.x, dr.lo.y, dr.hi.x, dr.hi.y);

		pDoc->ForceRedraw(pThisNodeShadow->GetShadow()->FindParentSpread(), 
				pThisNodeShadow->GetBoundingRect(), FALSE, pThisNodeShadow);

		pThisNodeShadow->SetFloorShadowAngle(NewAngle);

		pThisNodeShadow->RegenerateNode(NULL, bCache);

		pThisNodeShadow->InvalidateBoundingRect();

		dr= pThisNodeShadow->GetBoundingRect();
		TRACEUSER( "DavidM", _T("Rect after %d %d %d %d\n"),
			dr.lo.x, dr.lo.y, dr.hi.x, dr.hi.y);

		pDoc->ForceRedraw(pThisNodeShadow->GetShadow()->FindParentSpread(), 
				pThisNodeShadow->GetBoundingRect(), FALSE, pThisNodeShadow);
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode ChangeFloorShadowAngleAction::Execute();

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/96
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the num blend steps in pThisNodeBlend to OldNumSteps,
				after creating another action to record the current num steps of pThisNodeBlend
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeFloorShadowAngleAction::Execute()
{
	ActionCode Act;
	ChangeFloorShadowAngleAction* pAction;

	// force a redraw of the region
	DocRect dr = m_LastRect;

	Act = ChangeFloorShadowAngleAction::Init(	pOperation, 
										pOppositeActLst,
										m_pNodeShadowCont,
										m_LastAngle,
										&pAction,
										TRUE,
										m_bCache);

	if (Act != AC_FAIL)
	{
		Document * pDoc = Document::GetCurrent();
		
		if (pDoc)
		{
			m_LastRect = m_pNodeShadowCont->GetBoundingRect();
			pDoc->ForceRedraw(m_pNodeShadowCont->GetShadow()->FindParentSpread(), 
				m_LastRect, FALSE, m_pNodeShadowCont);
		}
	}
	
	return Act;
}

ChangeFloorShadowAngleAction::~ChangeFloorShadowAngleAction()
{
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The ChangeShadowTypeAction class

/********************************************************************************************

>	ChangeShadowTypeAction::ChangeShadowTypeAction()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ChangeShadowTypeAction::ChangeShadowTypeAction()
{
	m_pNodeShadowCont  = NULL;
	m_LastType = SHADOWTYPE_NONE;
}


/********************************************************************************************

>	ActionCode ChangeShadowTypeAction::Init( 	Operation* 	pOp,
												ActionList* pActionList,
												NodeShadow* 	pThisNodeShadow,
												MILLIPOINT	PenumbraSize,
												ChangePositionShadowXAction** 	ppNewAction);

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/96
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pThisNodeShadow	= ptr to NodeShadow to change 
				PenumbraSize	= New position X to apply to pThisNodeShadow
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



ActionCode ChangeShadowTypeAction::Init(Operation* pOp,
										ActionList* pActionList,
										NodeShadowController* pThisNodeShadow,
										ShadowType NewShadowType,
										ChangeShadowTypeAction** ppNewAction,
										BOOL bReverse)
{
	UINT32 ActSize = sizeof(ChangeShadowTypeAction);

	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,
		CC_RUNTIME_CLASS(ChangeShadowTypeAction),
		(Action**)ppNewAction);

	Document * pDoc = Document::GetCurrent();

	if (Ac != AC_FAIL)
	{
		// reverse the action
		// DMc
		if (bReverse)
		{
			pActionList->RemoveItem(*ppNewAction);
			pActionList->AddHead(*ppNewAction);
		}

		if (pDoc)
		{
			pThisNodeShadow->ReleaseCached(TRUE, FALSE, TRUE, TRUE);
			pDoc->ForceRedraw(pThisNodeShadow->FindParentSpread(),
				pThisNodeShadow->GetBlobBoundingRect(), FALSE, pThisNodeShadow, FALSE);
		}
		
		// set the values, invalidate and regenerate
		(*ppNewAction)->m_pNodeShadowCont  = pThisNodeShadow;
		(*ppNewAction)->m_LastType = pThisNodeShadow->GetShadowType();

		pThisNodeShadow->SetShadowType(NewShadowType);

		pThisNodeShadow->RegenerateNode(NULL);
		
		(*ppNewAction)->m_LastRect = (*ppNewAction)->m_pNodeShadowCont->GetBoundingRect();

		if (pDoc)
		{
			pThisNodeShadow->ReleaseCached(TRUE, FALSE, TRUE, TRUE);
			pDoc->ForceRedraw(pThisNodeShadow->FindParentSpread(),
				pThisNodeShadow->GetBlobBoundingRect(), FALSE, pThisNodeShadow, FALSE);
		}
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode ChangeShadowTypeAction::Execute();

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/96
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the num blend steps in pThisNodeBlend to OldNumSteps,
				after creating another action to record the current num steps of pThisNodeBlend
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeShadowTypeAction::Execute()
{
	ActionCode Act;
	ChangeShadowTypeAction* pAction;

	// force a redraw of the region
	DocRect dr = m_LastRect;

	Act = ChangeShadowTypeAction::Init(	pOperation, 
										pOppositeActLst,
										m_pNodeShadowCont,
										m_LastType,
										&pAction);

	if (Act != AC_FAIL)
	{
		Document * pDoc = Document::GetCurrent();
		
		if (pDoc)
		{
			m_LastRect = m_pNodeShadowCont->GetBoundingRect();
			pDoc->ForceRedraw(m_pNodeShadowCont->GetShadow()->FindParentSpread(), 
				m_LastRect, FALSE, m_pNodeShadowCont);
		}
	}
	
	return Act;
}

ChangeShadowTypeAction::~ChangeShadowTypeAction()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//////////	OpShadowAngle code
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	OpShadowAngle::OpShadowAngle()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	Constructor

********************************************************************************************/
OpShadowAngle::OpShadowAngle()
{
}

/********************************************************************************************

>	OpShadowAngle::~OpShadowAngle()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	Destructor
********************************************************************************************/
OpShadowAngle::~OpShadowAngle()
{
}


/********************************************************************************************

>	BOOL OpShadowAngle::Declare()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	

********************************************************************************************/

BOOL OpShadowAngle::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_ANGLESHADOWOP),
								CC_RUNTIME_CLASS(OpShadowAngle), 
								OPTOKEN_SHADOWANGLE,
								OpShadowAngle::GetState));
}

/********************************************************************************************

>	OpShadowAngle::DoWithParam(OpDescriptor* pOp, OpParam* pParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	

********************************************************************************************/
void OpShadowAngle::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	BeginSlowJob();
	DoStartSelOp(TRUE);

	NodeShadowParam* pNSParam = (NodeShadowParam *)pParam;

	ENSURE(pNSParam->pShadowTool, "Op not given pointer to shadow tool");
	Range* pShadowRange = pNSParam->pShadowTool->GetShadowEditRange();
	Node* pNode = pShadowRange->FindFirst();
	if (pNode==NULL)
	{
		ERROR3("Nothing was selected in OpShadowAngle");
		FailAndExecute();
		End();
		return;
	}

	while (pNode)
	{
		ENSURE(pNode->IsAShadowController(), "Found unexpected node type");
		NodeShadowController* pController = (NodeShadowController*)pNode;

		pController->InvalidateBoundingRect();
//		DoInvalidateNodeRegion(pController, TRUE);
		pController->ReleaseCached(TRUE, FALSE, TRUE, TRUE);
		DoInvalidateNodeRegion(pController, TRUE, FALSE, FALSE, FALSE);	// Don't force recache - we've already handled that

		ChangeFloorShadowAngleAction * pAction = NULL;

		if (!ChangeFloorShadowAngleAction::Init(this, GetUndoActionList(), pController,
			(INT32)pNSParam->m_FloorShadowAngle, &pAction))
		{
			ERROR3("Change floor shadow angle action failed\n");
		}

		pController->InvalidateBoundingRect();
//		DoInvalidateNodeRegion(pController, TRUE);
		pController->ReleaseCached(TRUE, FALSE, TRUE, TRUE);
		DoInvalidateNodeRegion(pController, TRUE, FALSE, FALSE, FALSE);	// Don't force recache - we've already handled that

		// inform all parents
		ObjChangeFlags flgs(FALSE, FALSE, FALSE, FALSE);
		flgs.RegenerateNode = TRUE;
		ObjChangeParam OP(OBJCHANGE_FINISHED, flgs, pController, this, OBJCHANGE_CALLEDBYOP);
		Node * pParent = pController->FindParent();
	
		while (pParent)
		{			
			pParent->OnChildChange(&OP);
			pParent = pParent->FindParent();
		}			

		pNode = pShadowRange->FindNext(pNode);
	}

	End();
}


/********************************************************************************************

>	OpState OpShadowAngle::GetState(String_256* Description, OpDescriptor*)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/96
	Outputs:	-
	Returns:	Ungreyed, Unticked
	Purpose:	Find out the state of the new regular shape at the specific time

********************************************************************************************/
OpState OpShadowAngle::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//////////	OpShadowHeight code
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	OpShadowHeight::OpShadowHeight()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	Constructor

********************************************************************************************/
OpShadowHeight::OpShadowHeight()
{
}

/********************************************************************************************

>	OpShadowHeight::~OpShadowHeight()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	Destructor
********************************************************************************************/
OpShadowHeight::~OpShadowHeight()
{
}


/********************************************************************************************

>	BOOL OpShadowHeight::Declare()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	

********************************************************************************************/

BOOL OpShadowHeight::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_HEIGHTSHADOWOP),
								CC_RUNTIME_CLASS(OpShadowHeight), 
								OPTOKEN_SHADOWHEIGHT,
								OpShadowHeight::GetState));
}

/********************************************************************************************

>	OpShadowHeight::DoWithParam(OpDescriptor* pOp, OpParam* pParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	

********************************************************************************************/
void OpShadowHeight::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	// Start a slow job
	BeginSlowJob();
	DoStartSelOp(TRUE);

	NodeShadowParam * pNSParam = (NodeShadowParam *)pParam;

	ENSURE(pNSParam->pShadowTool, "Op not given pointer to shadow tool");
	Range* pShadowRange = pNSParam->pShadowTool->GetShadowEditRange();
	Node* pNode = pShadowRange->FindFirst();
	if (pNode==NULL)
	{
		ERROR3("Nothing was selected in OpShadowHeight");
		FailAndExecute();
		End();
		return;
	}
	while (pNode)
	{
		ENSURE(pNode->IsAShadowController(), "Found unexpected node type");
		NodeShadowController* pController = (NodeShadowController*)pNode;

		pController->InvalidateBoundingRect();
		// DoInvalidateNodeRegion(pControl, TRUE);

		ChangeFloorShadowHeightAction * pAction = NULL;

		ChangeFloorShadowHeightAction::Init(this, GetUndoActionList(), pController,
			pNSParam->m_FloorShadowHeight, &pAction);

		pController->InvalidateBoundingRect();
		// DoInvalidateNodeRegion(pControl, TRUE);

		// inform all parents
		ObjChangeFlags flgs(FALSE, FALSE, FALSE, FALSE);
		flgs.RegenerateNode = TRUE;
		ObjChangeParam OP(OBJCHANGE_FINISHED, flgs, pController, this, OBJCHANGE_CALLEDBYOP);
		Node * pParent = pController->FindParent();
	
		while (pParent)
		{			
			pParent->OnChildChange(&OP);
			pParent = pParent->FindParent();
		}

		pNode = pShadowRange->FindNext(pNode);
	}

	End();
}


/********************************************************************************************

>	OpState OpShadowHeight::GetState(String_256* Description, OpDescriptor*)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/96
	Outputs:	-
	Returns:	Ungreyed, Unticked
	Purpose:	Find out the state of the new regular shape at the specific time

********************************************************************************************/
OpState OpShadowHeight::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//////////	OpShadowScale code
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	OpShadowScale::OpShadowScale()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	Constructor

********************************************************************************************/
OpShadowScale::OpShadowScale()
{
}

/********************************************************************************************

>	OpShadowScale::~OpShadowScale()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	Destructor
********************************************************************************************/
OpShadowScale::~OpShadowScale()
{
}


/********************************************************************************************

>	BOOL OpShadowScale::Declare()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	

********************************************************************************************/

BOOL OpShadowScale::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_SCALESHADOWOP),
								CC_RUNTIME_CLASS(OpShadowScale), 
								OPTOKEN_SHADOWSCALE,
								OpShadowScale::GetState));
}

/********************************************************************************************

>	OpShadowScale::DoWithParam(OpDescriptor* pOp, OpParam* pParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	

********************************************************************************************/
void OpShadowScale::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	// Start a slow job
	BeginSlowJob();
	DoStartSelOp(TRUE);

	NodeShadowParam * pNSParam = (NodeShadowParam *)pParam;

	ENSURE(pNSParam->pShadowTool, "Op not given pointer to shadow tool");
	Range* pShadowRange = pNSParam->pShadowTool->GetShadowEditRange();
	Node* pNode = pShadowRange->FindFirst();
	if (pNode==NULL)
	{
		ERROR3("Nothing was selected in OpShadowScale");
		FailAndExecute();
		End();
		return;
	}
	while (pNode)
	{
		ENSURE(pNode->IsAShadowController(), "Found unexpected node type");
		NodeShadowController* pController = (NodeShadowController*)pNode;

		pController->InvalidateBoundingRect();
		// DoInvalidateNodeRegion(pControl, TRUE);

		ChangeShadowScaleAction * pAction = NULL;

		ChangeShadowScaleAction::Init(this, GetUndoActionList(), pController,
			pNSParam->m_Scale, &pAction);

		pController->InvalidateBoundingRect();
		// DoInvalidateNodeRegion(pControl, TRUE);

		// inform all parents
		ObjChangeFlags flgs(FALSE, FALSE, FALSE, FALSE);
		flgs.RegenerateNode = TRUE;
		ObjChangeParam OP(OBJCHANGE_FINISHED, flgs, pController, this, OBJCHANGE_CALLEDBYOP);
		Node * pParent = pController->FindParent();
	
		while (pParent)
		{			
			pParent->OnChildChange(&OP);
			pParent = pParent->FindParent();
		}

		pNode = pShadowRange->FindNext(pNode);
	}

	End();
}


/********************************************************************************************

>	OpState OpShadowScale::GetState(String_256* Description, OpDescriptor*)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/96
	Outputs:	-
	Returns:	Ungreyed, Unticked
	Purpose:	Find out the state of the new regular shape at the specific time

********************************************************************************************/
OpState OpShadowScale::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The ChangeShadowScaleAction class

/********************************************************************************************

>	ChangeShadowScaleAction::ChangeShadowScaleAction()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ChangeShadowScaleAction::ChangeShadowScaleAction()
{
	m_pNodeShadowCont  = NULL;
	m_LastScale = 0;
}


/********************************************************************************************

>	ActionCode ChangeShadowScaleAction::Init( 	Operation* 	pOp,
												ActionList* pActionList,
												NodeShadow* 	pThisNodeShadow,
												MILLIPOINT	PenumbraSize,
												ChangePositionShadowXAction** 	ppNewAction);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pThisNodeShadow	= ptr to NodeShadow to change 
				PenumbraSize	= New position X to apply to pThisNodeShadow
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



ActionCode ChangeShadowScaleAction::Init(Operation* pOp,
										ActionList* pActionList,
										NodeShadowController* pThisNodeShadow,
										double NewScale,
										ChangeShadowScaleAction** ppNewAction,
										BOOL bReverse)
{
	UINT32 ActSize = sizeof(ChangeShadowScaleAction);

	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,
		CC_RUNTIME_CLASS(ChangeShadowScaleAction),
		(Action**)ppNewAction);


	Document * pDoc = Document::GetCurrent();	

	DocRect dr;

	if (Ac != AC_FAIL)
	{
		// reverse the action
		// DMc
		if (bReverse)
		{
			pActionList->RemoveItem(*ppNewAction);
			pActionList->AddHead(*ppNewAction);
		}
		
		(*ppNewAction)->m_pNodeShadowCont  = pThisNodeShadow;
		(*ppNewAction)->m_LastScale = pThisNodeShadow->GetWallShadowScale();

		// set the values, invalidate and regenerate
		pThisNodeShadow->InvalidateBoundingRect();

		dr = pThisNodeShadow->GetBoundingRect();
		TRACEUSER( "DavidM", _T("Rect before %d %d %d %d\n"),
			dr.lo.x, dr.lo.y, dr.hi.x, dr.hi.y);

		pDoc->ForceRedraw(pThisNodeShadow->GetShadow()->FindParentSpread(), 
				pThisNodeShadow->GetShadow()->GetBoundingRect(), FALSE, pThisNodeShadow);

		pThisNodeShadow->SetWallShadowScale((float)NewScale);

		pThisNodeShadow->RegenerateNode(NULL);

		pThisNodeShadow->InvalidateBoundingRect();

		dr= pThisNodeShadow->GetBoundingRect();
		TRACEUSER( "DavidM", _T("Rect after %d %d %d %d\n"),
			dr.lo.x, dr.lo.y, dr.hi.x, dr.hi.y);

		pDoc->ForceRedraw(pThisNodeShadow->GetShadow()->FindParentSpread(), 
				pThisNodeShadow->GetShadow()->GetBoundingRect(), FALSE, pThisNodeShadow);
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode ChangeShadowScaleAction::Execute();

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/96
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the num blend steps in pThisNodeBlend to OldNumSteps,
				after creating another action to record the current num steps of pThisNodeBlend
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeShadowScaleAction::Execute()
{
	ActionCode Act;
	ChangeShadowScaleAction* pAction;

	// force a redraw of the region
	DocRect dr = m_LastRect;

	Act = ChangeShadowScaleAction::Init(	pOperation, 
										pOppositeActLst,
										m_pNodeShadowCont,
										m_LastScale,
										&pAction);

	if (Act != AC_FAIL)
	{
		Document * pDoc = Document::GetCurrent();
		
		if (pDoc)
		{
			m_LastRect = m_pNodeShadowCont->GetBoundingRect();
			pDoc->ForceRedraw(m_pNodeShadowCont->GetShadow()->FindParentSpread(), 
				m_LastRect, FALSE, m_pNodeShadowCont);
		}
	}
	
	return Act;
}

ChangeShadowScaleAction::~ChangeShadowScaleAction()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//////////	OpShadowHeightAndAngle code
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	OpShadowHeightAndAngle::OpShadowHeightAndAngle()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	Constructor

********************************************************************************************/
OpShadowHeightAndAngle::OpShadowHeightAndAngle()
{
}

/********************************************************************************************

>	OpShadowHeightAndAngle::~OpShadowHeightAndAngle()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	Destructor
********************************************************************************************/
OpShadowHeightAndAngle::~OpShadowHeightAndAngle()
{
}


/********************************************************************************************

>	BOOL OpShadowHeightAndAngle::Declare()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	

********************************************************************************************/

BOOL OpShadowHeightAndAngle::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_HEIGHTANDANGLESHADOWOP),
								CC_RUNTIME_CLASS(OpShadowHeightAndAngle), 
								OPTOKEN_SHADOWHEIGHTANDANGLE,
								OpShadowHeightAndAngle::GetState));
}

/********************************************************************************************

>	OpShadowHeightAndAngle::DoWithParam(OpDescriptor* pOp, OpParam* pParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	

********************************************************************************************/
void OpShadowHeightAndAngle::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	BeginSlowJob();
	DoStartSelOp(TRUE);

	NodeShadowParam * pNSParam = (NodeShadowParam *)pParam;

	ENSURE(pNSParam->pShadowTool, "Op not given pointer to shadow tool");
	Range* pShadowRange = pNSParam->pShadowTool->GetShadowEditRange();
	Node* pNode = pShadowRange->FindFirst();
	if (pNode==NULL)
	{
		ERROR3("Nothing was selected in OpShadowHeightAndAndle");
		FailAndExecute();
		End();
		return;
	}
	while (pNode)
	{
		ENSURE(pNode->IsAShadowController(), "Found unexpected node type");
		NodeShadowController* pController = (NodeShadowController*)pNode;

		pController->InvalidateBoundingRect();
		// DoInvalidateNodeRegion(pControl, TRUE);

		ChangeFloorShadowAngleAction * pAction = NULL;
		ChangeFloorShadowHeightAction * pAction2 = NULL;


		if (!ChangeFloorShadowAngleAction::Init(this, GetUndoActionList(), pController,
			(INT32)pNSParam->m_FloorShadowAngle, &pAction, TRUE, TRUE))
		{
			ERROR3("Change floor shadow angle action failed\n");
		}

		if (!ChangeFloorShadowHeightAction::Init(this, GetUndoActionList(), pController,
			pNSParam->m_FloorShadowHeight, &pAction2, TRUE, TRUE))
		{
			ERROR3("Change floor shadow angle action failed\n");
		}

		pController->InvalidateBoundingRect();
		// DoInvalidateNodeRegion(pControl, TRUE);

		// do the object change on the node
		// inform all parents
		ObjChangeFlags flgs(FALSE, FALSE, FALSE, FALSE);
		flgs.RegenerateNode = TRUE;
		ObjChangeParam OP(OBJCHANGE_FINISHED, flgs, pController, this, OBJCHANGE_CALLEDBYOP);
		Node * pParent = pController->FindParent();
	
		while (pParent)
		{			
			pParent->OnChildChange(&OP);
			pParent = pParent->FindParent();
		}

		pNode = pShadowRange->FindNext(pNode);
	}

	End();

}


/********************************************************************************************

>	OpState OpShadowHeightAndAngle::GetState(String_256* Description, OpDescriptor*)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/96
	Outputs:	-
	Returns:	Ungreyed, Unticked
	Purpose:	Find out the state of the new regular shape at the specific time

********************************************************************************************/
OpState OpShadowHeightAndAngle::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The SaveShadowDataAction class

/********************************************************************************************

>	SaveShadowDataAction::SaveShadowDataAction()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

SaveShadowDataAction::SaveShadowDataAction()
{
	m_pSelRange = NULL;
}


/********************************************************************************************

>	ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							Range &SelRange,
							SaveShadowDataAction** NewAction,
							BOOL bReverse = TRUE);
	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pThisNodeShadow	= ptr to NodeShadow to change 
				PenumbraSize	= New position X to apply to pThisNodeShadow
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



ActionCode SaveShadowDataAction::Init( Operation* pOp,
							ActionList* pActionList,
							Range &MyRange,
							SaveShadowDataAction** ppNewAction,
							BOOL bReverse)
{
	UINT32 ActSize = sizeof(SaveShadowDataAction);

	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,
		CC_RUNTIME_CLASS(SaveShadowDataAction),
		(Action**)ppNewAction);

	ShadowDragData * pItem = NULL;

	if (Ac != AC_FAIL)
	{
		// reverse the action
		// DMc
		if (bReverse)
		{
			pActionList->RemoveItem(*ppNewAction);
			pActionList->AddHead(*ppNewAction);
		}

		(*ppNewAction)->m_LastRect = MyRange.GetBoundingRect();	
		
		// save all the data in the range
		ALLOC_WITH_FAIL((*ppNewAction)->m_pSelRange, new Range(MyRange), pOp);

		Node * pNode = (*ppNewAction)->m_pSelRange->FindFirst();

		Spread *pSpread = NULL;

		while (pNode)
		{
			pSpread = pNode->FindParentSpread();
			
			if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeShadowController)))
			{
				ALLOC_WITH_FAIL(pItem, new ShadowDragData, pOp);

				if (!pItem)
				{
					return AC_FAIL;
				}

				pItem->PenumbraSize = ((NodeShadowController *)pNode)->GetPenumbraWidth();
				pItem->PositionShadowX = ((NodeShadowController *)pNode)->GetOffsetX();
				pItem->PositionShadowY = ((NodeShadowController *)pNode)->GetOffsetY();
				pItem->ShadowAngle = ((NodeShadowController *)pNode)->GetFloorShadowAngle();
				pItem->ShadowHeight = ((NodeShadowController *)pNode)->GetFloorShadowHeight();
				pItem->ShadowScale = ((NodeShadowController *)pNode)->GetWallShadowScale();
				pItem->GlowWidth   = ((NodeShadowController *)pNode)->GetGlowWidth();
				pItem->FeatherWidth = ((NodeShadowController*)pNode)->GetFeatherWidth();

				(*ppNewAction)->m_DataList.AddTail(pItem);
			}

			pNode = (*ppNewAction)->m_pSelRange->FindNext(pNode);
		}
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode SaveShadowDataAction::Execute();

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/96
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the num blend steps in pThisNodeBlend to OldNumSteps,
				after creating another action to record the current num steps of pThisNodeBlend
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode SaveShadowDataAction::Execute()
{
	ActionCode Act;
	SaveShadowDataAction* pAction;

	// force a redraw of the region
	DocRect dr = m_LastRect;

	Act = SaveShadowDataAction::Init(	pOperation, 
										pOppositeActLst,
										*m_pSelRange,
										&pAction);

	Document * pDoc = Document::GetCurrent();

	if (Act != AC_FAIL)
	{
		Node * pNode = m_pSelRange->FindFirst();
		ShadowDragData * pItem = (ShadowDragData *)m_DataList.GetHead();

		while (pNode)
		{
			if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeShadowController)) && pItem)
			{
				// invalidate the last region
				if (pDoc)
				{
					pDoc->ForceRedraw(pNode->FindParentSpread(), ((NodeShadowController *)pNode)->GetShadow()->GetBoundingRect(), TRUE, pNode);
				}				
		
				((NodeShadowController *)pNode)->SetPenumbraWidth(pItem->PenumbraSize);
				((NodeShadowController *)pNode)->SetOffsetX(pItem->PositionShadowX);
				((NodeShadowController *)pNode)->SetOffsetY(pItem->PositionShadowY);
				((NodeShadowController *)pNode)->SetFloorShadowAngle(pItem->ShadowAngle);
				((NodeShadowController *)pNode)->SetFloorShadowHeight(pItem->ShadowHeight);
				((NodeShadowController *)pNode)->SetWallShadowScale(pItem->ShadowScale);
				((NodeShadowController *)pNode)->SetGlowWidth(pItem->GlowWidth);
				((NodeShadowController*)pNode)->SetFeatherWidth(pItem->FeatherWidth);

				TRACEUSER( "DavidM", _T("Setting floor shadow angle %d\n"), pItem->ShadowAngle);
				
				pItem = (ShadowDragData *)m_DataList.GetNext(pItem);

				// regenerate the node
				pNode->RegenerateNode(NULL, FALSE);

				// invalidate the last region
				if (pDoc)
				{
					pDoc->ForceRedraw(pNode->FindParentSpread(), ((NodeShadowController *)pNode)->GetShadow()->GetBoundingRect(), TRUE, pNode);
				}				

				GetApplication()->ServiceRendering();
			}

			pNode = m_pSelRange->FindNext(pNode);
		}
	}

	return Act;
}

SaveShadowDataAction::~SaveShadowDataAction()
{
	m_DataList.DeleteAll();

	if (m_pSelRange)
		delete m_pSelRange;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//////////	OpSaveShadowData code
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	OpSaveShadowData::OpSaveShadowData()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	Constructor

********************************************************************************************/
OpSaveShadowData::OpSaveShadowData()
{
	m_OpName = 0;
}

/********************************************************************************************

>	OpSaveShadowData::~OpSaveShadowData()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	Destructor
********************************************************************************************/
OpSaveShadowData::~OpSaveShadowData()
{
}

/********************************************************************************************

>	void OpSaveShadowData::GetOpName(String_256 * pString)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	Gets the op name
********************************************************************************************/
void OpSaveShadowData::GetOpName(String_256 * pString)
{
	if (pString && m_OpName != 0)
	{
		pString->Load(m_OpName);
	}
}



/********************************************************************************************

>	BOOL OpSaveShadowData::Declare()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	

********************************************************************************************/

BOOL OpSaveShadowData::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_SAVESHADOWDATAOP),
								CC_RUNTIME_CLASS(OpSaveShadowData), 
								OPTOKEN_SAVESHADOWDATA,
								OpSaveShadowData::GetState));
}

/********************************************************************************************

>	BOOL OpSaveShadowData::SaveShadowData(UndoableOperation * pOp)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/1/2000
	Inputs:		The undoable operation to use
	Outputs:	
	Returns:	TRUE for success, FALSE for failure
	Purpose:	Saves the shadow in the selection at this point, so it can be restored
				on an undo

********************************************************************************************/
BOOL OpSaveShadowData::SaveShadowData(UndoableOperation * pOp)
{
	Range MyRange(*(GetApplication()->FindSelection()));

	RangeControl rg = MyRange.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	MyRange.SetRangeControl(rg);

	Spread * pSpread = Document::GetSelectedSpread();

	SaveShadowDataAction * pAction = NULL;

	if (SaveShadowDataAction::Init(pOp, pOp->GetUndoActionList(), MyRange, &pAction) != AC_OK)
		return FALSE;

	// now, invalidate the whole range's blob bounds
	Node * pNode = MyRange.FindFirst();
	DocRect Bounds;

	while (pNode)
	{
		if (pNode->IsAnObject())
		{
// Nothing changed - so why recache eveything?
//			((NodeRenderableInk *)pNode)->ReleaseCached();
			Bounds = Bounds.Union(((NodeRenderableInk *)pNode)->GetBlobBoundingRect());
		}

		pNode = MyRange.FindNext(pNode);
	}

	BlobManager * pBlobMgr = GetApplication()->GetBlobManager();

	if (pBlobMgr)
	{
		Bounds.Inflate(pBlobMgr->GetBlobSize());
	}

	if (pSpread)
	{
		if (!pOp->DoInvalidateRegion(pSpread, Bounds))
		{
			return FALSE;
		}
	}

	return TRUE;
}

/********************************************************************************************

>	void OpSaveShadowData::DoWithParam(OpDescriptor* pOp, OpParam * pParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	Saves the shadow in the selection at this point, so it can be restored
				on an undo

********************************************************************************************/
void OpSaveShadowData::DoWithParam(OpDescriptor* pOp, OpParam * pParam)
{
	// Start a slow job
	BeginSlowJob();
	DoStartSelOp(TRUE);

	m_OpName = ((SaveShadowDataInfo *)pParam)->ID;

	if (!SaveShadowData(this))
	{
		ERROR2RAW("Can't save shadow data");
	}
	
	End();
}


/********************************************************************************************

>	OpState OpSaveShadowData::GetState(String_256* Description, OpDescriptor*)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/96
	Outputs:	-
	Returns:	Ungreyed, Unticked
	Purpose:	Find out the state of the new regular shape at the specific time

********************************************************************************************/
OpState OpSaveShadowData::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}


//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The ChangeGlowWidthAction class

/********************************************************************************************

>	ChangeGlowWidthAction::ChangeGlowWidthAction()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ChangeGlowWidthAction::ChangeGlowWidthAction()
{
	m_pNodeShadowCont  = NULL;
	m_LastWidth = 0;
}


/********************************************************************************************

>	ActionCode ChangeGlowWidthAction::Init( 	Operation* 	pOp,
												ActionList* pActionList,
												NodeShadow* 	pThisNodeShadow,
												MILLIPOINT	PenumbraSize,
												ChangePositionShadowXAction** 	ppNewAction);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pThisNodeShadow	= ptr to NodeShadow to change 
				PenumbraSize	= New position X to apply to pThisNodeShadow
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



ActionCode ChangeGlowWidthAction::Init(Operation* pOp,
										ActionList* pActionList,
										NodeShadowController* pThisNodeShadow,
										INT32 NewWidth,
										ChangeGlowWidthAction** ppNewAction,
										BOOL bReverse, BOOL bCache)
{
	UINT32 ActSize = sizeof(ChangeGlowWidthAction);

	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,
		CC_RUNTIME_CLASS(ChangeGlowWidthAction),
		(Action**)ppNewAction);


	Document * pDoc = Document::GetCurrent();	

	DocRect dr;

	if (Ac != AC_FAIL)
	{
		// reverse the action
		// DMc
		if (bReverse)
		{
			pActionList->RemoveItem(*ppNewAction);
			pActionList->AddHead(*ppNewAction);
		}
		
		(*ppNewAction)->m_pNodeShadowCont  = pThisNodeShadow;
		(*ppNewAction)->m_LastWidth = pThisNodeShadow->GetGlowWidth();
		(*ppNewAction)->m_bCache = bCache;

		// set the values, invalidate and regenerate
		pThisNodeShadow->InvalidateBoundingRect();

		dr = pThisNodeShadow->GetBoundingRect();
		TRACEUSER( "DavidM", _T("Rect before %d %d %d %d\n"),
			dr.lo.x, dr.lo.y, dr.hi.x, dr.hi.y);

		pDoc->ForceRedraw(pThisNodeShadow->GetShadow()->FindParentSpread(), 
				pThisNodeShadow->GetShadow()->GetBoundingRect(), FALSE, pThisNodeShadow);

		pThisNodeShadow->SetGlowWidth(NewWidth);

		pThisNodeShadow->RegenerateNode(NULL, bCache);

		pThisNodeShadow->InvalidateBoundingRect();

		dr= pThisNodeShadow->GetBoundingRect();
		TRACEUSER( "DavidM", _T("Rect after %d %d %d %d\n"),
			dr.lo.x, dr.lo.y, dr.hi.x, dr.hi.y);

		pDoc->ForceRedraw(pThisNodeShadow->GetShadow()->FindParentSpread(), 
				pThisNodeShadow->GetShadow()->GetBoundingRect(), FALSE, pThisNodeShadow);
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode ChangeGlowWidthAction::Execute();

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/96
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the num blend steps in pThisNodeBlend to OldNumSteps,
				after creating another action to record the current num steps of pThisNodeBlend
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeGlowWidthAction::Execute()
{
	ActionCode Act;
	ChangeGlowWidthAction* pAction;

	// force a redraw of the region
	DocRect dr = m_LastRect;

	Act = ChangeGlowWidthAction::Init(	pOperation, 
										pOppositeActLst,
										m_pNodeShadowCont,
										m_LastWidth,
										&pAction,
										TRUE,
										m_bCache);

	if (Act != AC_FAIL)
	{
		Document * pDoc = Document::GetCurrent();
		
		if (pDoc)
		{
			m_LastRect = m_pNodeShadowCont->GetBoundingRect();

			pDoc->ForceRedraw(m_pNodeShadowCont->FindParentSpread(), 
				m_LastRect, FALSE, m_pNodeShadowCont);
		}
	}
	
	return Act;
}

ChangeGlowWidthAction::~ChangeGlowWidthAction()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//////////	OpGlowWidth code
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	OpGlowWidth::OpGlowWidth()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	Constructor

********************************************************************************************/
OpGlowWidth::OpGlowWidth()
{
}

/********************************************************************************************

>	OpGlowWidth::~OpGlowWidth()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	Destructor
********************************************************************************************/
OpGlowWidth::~OpGlowWidth()
{
}


/********************************************************************************************

>	BOOL OpGlowWidth::Declare()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	

********************************************************************************************/

BOOL OpGlowWidth::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_GLOWWIDTHOP),
								CC_RUNTIME_CLASS(OpGlowWidth), 
								OPTOKEN_GLOWWIDTH,
								OpGlowWidth::GetState));
}

/********************************************************************************************

>	OpGlowWidth::DoWithParam(OpDescriptor* pOp, OpParam* pParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	

********************************************************************************************/
void OpGlowWidth::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	// Start a slow job
	BeginSlowJob();
	DoStartSelOp(TRUE);

	NodeShadowParam * pNSParam = (NodeShadowParam *)pParam;

	ENSURE(pNSParam->pShadowTool, "Op not given pointer to shadow tool");
	Range* pShadowRange = pNSParam->pShadowTool->GetShadowEditRange();
	Node* pNode = pShadowRange->FindFirst();
	if (pNode==NULL)
	{
		ERROR3("Nothing was selected in OpShadowPenumbra");
		FailAndExecute();
		End();
		return;
	}
	while (pNode)
	{
		ENSURE(pNode->IsAShadowController(), "Found unexpected node type");
		NodeShadowController* pController = (NodeShadowController*)pNode;

//		pController->ReleaseCached();
//		DoInvalidateRegion(pController->FindParentSpread(), pController->GetBlobBoundingRect());
		pController->ReleaseCached(TRUE, FALSE, TRUE, TRUE);
		DoInvalidateNodeRegion(pController, TRUE, FALSE, FALSE, FALSE);
		
		ChangeGlowWidthAction * pAction = NULL;

		ChangeGlowWidthAction::Init(this, GetUndoActionList(), pController,
			pNSParam->m_GlowWidth, &pAction);

		// do the object change on the node
		ObjChangeFlags flgs(FALSE, FALSE, FALSE, TRUE);
		ObjChangeParam OP(OBJCHANGE_FINISHED, flgs, pController, this, OBJCHANGE_CALLEDBYOP);
		Node * pNodeParent = pController->FindParent();

		while (pNodeParent)
		{
			pNodeParent->OnChildChange(&OP);

			pNodeParent = pNodeParent->FindParent();
		}

//		pController->ReleaseCached();
//		DoInvalidateRegion(pController->FindParentSpread(), pController->GetBlobBoundingRect());
		pController->ReleaseCached(TRUE, FALSE, TRUE, TRUE);
		DoInvalidateNodeRegion(pController, TRUE, FALSE, FALSE, FALSE);

		pNode = pShadowRange->FindNext(pNode);
	}

	End();
}


/********************************************************************************************

>	OpState OpGlowWidth::GetState(String_256* Description, OpDescriptor*)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/96
	Outputs:	-
	Returns:	Ungreyed, Unticked
	Purpose:	Find out the state of the new regular shape at the specific time

********************************************************************************************/
OpState OpGlowWidth::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//////////	OpChangeShadowProfile code
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	OpChangeShadowProfile::OpChangeShadowProfile()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	Constructor

********************************************************************************************/
OpChangeShadowProfile::OpChangeShadowProfile()
{
}

/********************************************************************************************

>	OpChangeShadowProfile::~OpChangeShadowProfile()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	Destructor
********************************************************************************************/
OpChangeShadowProfile::~OpChangeShadowProfile()
{
}


/********************************************************************************************

>	BOOL OpChangeShadowProfile::Declare()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	

********************************************************************************************/

BOOL OpChangeShadowProfile::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_SHADOWPROFILE),
								CC_RUNTIME_CLASS(OpChangeShadowProfile), 
								OPTOKEN_SHADOWPROFILE,
								OpChangeShadowProfile::GetState));
}

/********************************************************************************************

>	OpChangeShadowProfile::DoWithParam(OpDescriptor* pOp, OpParam* pParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	

********************************************************************************************/
void OpChangeShadowProfile::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	// Start a slow job
	BeginSlowJob();
	DoStartSelOp(TRUE);

	// run through the selection changing all profiles
	CProfileBiasGain Profile;

	// recast the op param
	ShadowProfileOpParam * pProfileParam = (ShadowProfileOpParam *)pParam;

	CProfileBiasGain OldGain;

	ENSURE(pProfileParam->m_pTool, "Op not given pointer to shadow tool");
	Range* pShadowRange = pProfileParam->m_pTool->GetShadowEditRange();
	Node* pNode = pShadowRange->FindFirst();

	// If the range is empty just stop without doing anything
	// This allows Profile dialog to remain onscreen while selection changes
	// (Should really be greyed out)
	if (pNode==NULL)
	{
		FailAndExecute();
		End();
		return;
	}

	while (pNode)
	{
		ENSURE(pNode->IsAShadowController(), "Found unexpected node type");
		NodeShadowController* pController = (NodeShadowController*)pNode;
		NodeShadow* pShadow = pController->GetShadow();

		// initialise the action
		ChangeProfileAction * pAction;
		ChangeProfileAction::Init(this, GetUndoActionList(), pShadow, pProfileParam->Profile, &pAction);

		pNode = pShadowRange->FindNext(pNode);
	}

	End();
}


/********************************************************************************************

>	OpState OpChangeShadowProfile::GetState(String_256* Description, OpDescriptor*)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/96
	Outputs:	-
	Returns:	Ungreyed, Unticked
	Purpose:	Find out the state of the new regular shape at the specific time

********************************************************************************************/
OpState OpChangeShadowProfile::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The ChangeProfileAction class

/********************************************************************************************

>	ChangeProfileAction::ChangeProfileAction()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ChangeProfileAction::ChangeProfileAction()
{
	m_pNodeShadow  = NULL;
}


/********************************************************************************************

>	ActionCode ChangeProfileAction::Init( 	Operation* 	pOp,
												ActionList* pActionList,
												NodeShadow* 	pThisNodeShadow,
												MILLIPOINT	PenumbraSize,
												ChangePositionShadowXAction** 	ppNewAction);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pThisNodeShadow	= ptr to NodeShadow to change 
				PenumbraSize	= New position X to apply to pThisNodeShadow
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



ActionCode ChangeProfileAction::Init( Operation* pOp,
							ActionList* pActionList,
							NodeShadow* pThisNodeShadow,
							CProfileBiasGain &Profile,
							ChangeProfileAction** ppNewAction,
							BOOL bReverse,
							BOOL bCache)
{
	UINT32 ActSize = sizeof(ChangeProfileAction);

	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,
		CC_RUNTIME_CLASS(ChangeProfileAction),
		(Action**)ppNewAction);

	Document * pDoc = Document::GetCurrent();	

	DocRect dr;

	if (Ac != AC_FAIL)
	{
		// reverse the action
		// DMc
		if (bReverse)
		{
			pActionList->RemoveItem(*ppNewAction);
			pActionList->AddHead(*ppNewAction);
		}
		
		(*ppNewAction)->m_pNodeShadow  = pThisNodeShadow;
		(*ppNewAction)->m_LastProfile  = pThisNodeShadow->GetBiasGain();

		// set the values, invalidate and regenerate
		pThisNodeShadow->InvalidateBoundingRect();

		pDoc->ForceRedraw(pThisNodeShadow->FindParentSpread(), 
				pThisNodeShadow->GetBoundingRect(), FALSE, pThisNodeShadow);

		pThisNodeShadow->SetBiasGain(Profile);

		((NodeShadowController*)pThisNodeShadow->GetParentController())->RegenerateNode(NULL, bCache);

		pThisNodeShadow->InvalidateBoundingRect();

		dr= ((NodeShadowController*)pThisNodeShadow->GetParentController())->GetBoundingRect();
		TRACEUSER( "DavidM", _T("Rect after %d %d %d %d\n"),
			dr.lo.x, dr.lo.y, dr.hi.x, dr.hi.y);

		pDoc->ForceRedraw(pThisNodeShadow->FindParentSpread(), 
				pThisNodeShadow->GetBoundingRect(), FALSE, pThisNodeShadow);
	}

	return Ac;
}



/********************************************************************************************

>	void ChangeProfileAction::ChangeShadowProfileWithNoUndo(CProfileBiasGain &Profile, SoftShadowTool* pShadowTool)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/2000
	Inputs:		Profile - the profile that is to be applied directly (i.e.  applied with no undo)
	Purpose:	When applying shadow profiles, we only want to generate one bit of undo information.
				This function allows us to meet this requirement (the one bit of undo information
				is generated via OpChangeShadowProfile::DoWithParam ()).  This function is ONLY
				called from within SoftShadowInfoBarOp::ChangeProfile () - after
				OpChangeShadowProfile::DoWithParam () has been called.
	Errors:		-
	SeeAlso:	SoftShadowInfoBarOp::ChangeProfile (), OpChangeShadowProfile::DoWithParam ().

********************************************************************************************/

void ChangeProfileAction::ChangeShadowProfileWithNoUndo (CProfileBiasGain &Profile, SoftShadowTool* pShadowTool)
{
	// Start a slow job
	BeginSlowJob();

	// run through the selection changing all profiles
	CProfileBiasGain OldGain;

	Document * pDoc = Document::GetCurrent();

	ENSURE(pShadowTool, "Op not given pointer to shadow tool");
	Range* pShadowRange = pShadowTool->GetShadowEditRange();
	Node* pNode = pShadowRange->FindFirst();
	while (pNode)
	{
		ENSURE(pNode->IsAShadowController(), "Found unexpected node type");
		NodeShadowController* pController = (NodeShadowController*)pNode;
		NodeShadow* pThisNodeShadow = pController->GetShadow();

		pThisNodeShadow->SetBiasGain(Profile);

		pController->RegenerateNode(NULL, FALSE);										//bCache);

		pThisNodeShadow->InvalidateBoundingRect();

		pDoc->ForceRedraw(pThisNodeShadow->FindParentSpread(), 
						  pThisNodeShadow->GetBoundingRect(), FALSE, pThisNodeShadow);

		pNode = pShadowRange->FindNext(pNode);
	}

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

>	ActionCode ChangeProfileAction::Execute();

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/96
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the num blend steps in pThisNodeBlend to OldNumSteps,
				after creating another action to record the current num steps of pThisNodeBlend
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeProfileAction::Execute()
{
	ActionCode Act;
	ChangeProfileAction* pAction;

	Act = ChangeProfileAction::Init(	pOperation, 
										pOppositeActLst,
										m_pNodeShadow,
										m_LastProfile,
										&pAction,
										TRUE,
										FALSE);

	NodeCompound* pController = m_pNodeShadow->GetParentController();

	if (Act != AC_FAIL)
	{
		Document * pDoc = Document::GetCurrent();
		
		if (pDoc)
		{
			DocRect dr = pController->GetBoundingRect();
			
			pDoc->ForceRedraw(pController->FindParentSpread(), 
				dr, FALSE, pController);
		}
	}
	
	return Act;
}

ChangeProfileAction::~ChangeProfileAction()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
//////////	OpChangeShadowDarkness code
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	OpChangeShadowDarkness::OpChangeShadowDarkness()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	Constructor

********************************************************************************************/
OpChangeShadowDarkness::OpChangeShadowDarkness()
{
}

/********************************************************************************************

>	OpChangeShadowDarkness::~OpChangeShadowDarkness()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	Destructor
********************************************************************************************/
OpChangeShadowDarkness::~OpChangeShadowDarkness()
{
}


/********************************************************************************************

>	BOOL OpChangeShadowDarkness::Declare()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	

********************************************************************************************/

BOOL OpChangeShadowDarkness::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_SHADOWDARKNESS),
								CC_RUNTIME_CLASS(OpChangeShadowDarkness), 
								OPTOKEN_SHADOWDARKNESS,
								OpChangeShadowDarkness::GetState));
}

/********************************************************************************************

>	OpChangeShadowDarkness::DoWithParam(OpDescriptor* pOp, OpParam* pParam)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:	
	Outputs:	
	Returns:	
	Purpose:	

********************************************************************************************/
void OpChangeShadowDarkness::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	// Start a slow job
	BeginSlowJob();
	DoStartSelOp(TRUE);

	// recast the op param
	ShadowDarknessOpParam * pDarknessParam = (ShadowDarknessOpParam *)pParam;

	ENSURE(pDarknessParam->m_pTool, "Op not given pointer to shadow tool");
	Range* pShadowRange = pDarknessParam->m_pTool->GetShadowEditRange();
	Node* pNode = pShadowRange->FindFirst();
	if (pNode==NULL)
	{
		ERROR3("Nothing was selected in OpChangeShadowDarkness");
		FailAndExecute();
		End();
		return;
	}
	while (pNode)
	{
		ENSURE(pNode->IsAShadowController(), "Found unexpected node type");
		NodeShadowController* pController = (NodeShadowController*)pNode;
		NodeShadow* pShadow = pController->GetShadow();
			
		// invoke the action
		ChangeShadowDarknessAction * pAction = NULL;

		if (ChangeShadowDarknessAction::Init(this, this->GetUndoActionList(), pShadow,
			pDarknessParam->m_dDarkness, &pAction, FALSE, FALSE) != AC_OK)
		{
			FailAndExecute();
			End();
			return;
		}

		pController->ReleaseCached(TRUE, FALSE, TRUE, TRUE);
//		if (!DoInvalidateRegion(pController->FindParentSpread(), pController->GetBlobBoundingRect()))
		if (!DoInvalidateNodeRegion(pController, TRUE, FALSE, FALSE, FALSE))
		{
			FailAndExecute();
			End();
			return;
		}

		// tell all parents of the controller node
		// inform all parents
		ObjChangeFlags flgs(FALSE, FALSE, FALSE, FALSE);
		flgs.RegenerateNode = TRUE;
		ObjChangeParam OP(OBJCHANGE_FINISHED, flgs, pController, this, OBJCHANGE_CALLEDBYCHILD);

		Node *pParent = pController->FindParent();
		while (pParent)
		{			
			pParent->OnChildChange(&OP);
			pParent = pParent->FindParent();
		}
				
				
		pNode = pShadowRange->FindNext(pNode);
	}

	End();
}


/********************************************************************************************

>	OpState OpChangeShadowDarkness::GetState(String_256* Description, OpDescriptor*)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/96
	Outputs:	-
	Returns:	Ungreyed, Unticked
	Purpose:	Find out the state of the new regular shape at the specific time

********************************************************************************************/
OpState OpChangeShadowDarkness::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	
	return Blobby;
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The ChangeShadowDarknessAction class

/********************************************************************************************

>	ChangeShadowDarknessAction::ChangeShadowDarknessAction()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ChangeShadowDarknessAction::ChangeShadowDarknessAction()
{
	m_pNodeShadow  = NULL;
}


/********************************************************************************************

>	ActionCode ChangeShadowDarknessAction::Init(( Operation* pOp,
							ActionList* pActionList,
							NodeShadow* pThisNodeShadow,
							double dDarkness;
							ChangeShadowDarknessAction** NewAction,
							BOOL bReverse = TRUE,
							BOOL bCache = FALSE);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pThisNodeShadow	= ptr to NodeShadow to change 
				PenumbraSize	= New position X to apply to pThisNodeShadow
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



ActionCode ChangeShadowDarknessAction::Init( Operation* pOp,
							ActionList* pActionList,
							NodeShadow* pThisNodeShadow,
							double dDarkness,
							ChangeShadowDarknessAction** NewAction,
							BOOL bReverse,
							BOOL bCache)
{
	UINT32 ActSize = sizeof(ChangeShadowDarknessAction);

	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,
		CC_RUNTIME_CLASS(ChangeShadowDarknessAction),
		(Action**)NewAction);

	Document * pDoc = Document::GetCurrent();	

	DocRect dr;

	if (Ac != AC_FAIL)
	{
		(*NewAction)->m_pNodeShadow  = pThisNodeShadow;

		double dOldDarkness = pThisNodeShadow->GetDarkness();

		(*NewAction)->m_dLastDarkness = dOldDarkness;

		// set the values, invalidate and regenerate
		pThisNodeShadow->InvalidateBoundingRect();

		pDoc->ForceRedraw(pThisNodeShadow->FindParentSpread(), 
				pThisNodeShadow->GetBoundingRect(), FALSE, pThisNodeShadow);

		pThisNodeShadow->SetDarkness(dDarkness);

		pThisNodeShadow->InvalidateBoundingRect();

		dr = pThisNodeShadow->GetParentController()->GetBoundingRect();

		pDoc->ForceRedraw(pThisNodeShadow->FindParentSpread(), 
			pThisNodeShadow->GetBoundingRect(), FALSE, pThisNodeShadow);
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode ChangeShadowDarknessAction::Execute();

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/01/96
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the num blend steps in pThisNodeBlend to OldNumSteps,
				after creating another action to record the current num steps of pThisNodeBlend
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeShadowDarknessAction::Execute()
{
	ActionCode Act;
	ChangeShadowDarknessAction* pAction;

	Act = ChangeShadowDarknessAction::Init(	pOperation, 
										pOppositeActLst,
										m_pNodeShadow,
										m_dLastDarkness,
										&pAction,
										TRUE,
										FALSE);

	NodeCompound* pController = m_pNodeShadow->GetParentController();

	if (Act != AC_FAIL)
	{
		Document * pDoc = Document::GetCurrent();
		
		if (pDoc)
		{
			DocRect dr = pController->GetBoundingRect();
			
			pDoc->ForceRedraw(pController->FindParentSpread(), 
				dr, FALSE, pController);
		}
	}
	
	return Act;
}

ChangeShadowDarknessAction::~ChangeShadowDarknessAction()
{
}










#endif	// BUILDSHADOWS
