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
//
// Implementation of Live Effects operations.
//

#include "camtypes.h"			// pre-compiled header
#include "opliveeffects.h"		// our header file
//#include "phil.h"				// for _R(IDS_GREY_WHEN_SELECT_INSIDE)
#include "nodeliveeffect.h"
#include "mainfrm.h"
#include "dibutil.h"
#include "xpehost.h"
#include "progress.h"
#include "liveeffectstool.h"
//#include "richard.h"			// for _R(IDS_REMOVE)
//#include "simon.h"				// for _R(IDS_CONTINUE)
//#include "resource.h"			// for _R(IDS_CANCEL)
#include "nodecont.h"
#include "opshadow.h"
#include "nodeshad.h"
#include "grnddib.h"
#include "fthrattr.h"
#include "opfeathr.h"
#include "spread.h"
#include "contmenu.h"
#include "shadtool.h"
#include "objchge.h"
#include "effects_stack.h"

// dynamic class creation stuff.
CC_IMPLEMENT_DYNAMIC(OpLiveEffectParam, OpMenuParam);
CC_IMPLEMENT_DYNCREATE(OpLiveEffect, SelOperation);
CC_IMPLEMENT_DYNCREATE(OpApplyLiveEffect, OpLiveEffect);
CC_IMPLEMENT_DYNCREATE(OpEditLiveEffect, OpLiveEffect);
CC_IMPLEMENT_DYNCREATE(OpDeleteLiveEffect, OpLiveEffect);
CC_IMPLEMENT_DYNCREATE(OpDeleteAllLiveEffect, OpLiveEffect);
CC_IMPLEMENT_DYNCREATE(OpEffectRes, OpLiveEffect);
CC_IMPLEMENT_DYNCREATE(OpEffectLock, OpLiveEffect);
CC_IMPLEMENT_DYNCREATE(OpEffectLockAll, OpEffectLock);
CC_IMPLEMENT_DYNCREATE(OpEditLegacyEffect, Operation);
CC_IMPLEMENT_DYNCREATE(MarkEditListAction, Action);
CC_IMPLEMENT_DYNCREATE(ChangeEffectResAction, Action);
//CC_IMPLEMENT_DYNCREATE(ToggleLockAction, Action);

#ifdef FEATHER_EFFECT
CC_IMPLEMENT_DYNAMIC(OpFeatherEffectParam, OpLiveEffectParam);
CC_IMPLEMENT_DYNCREATE(OpApplyFeatherEffect, OpLiveEffect);
#endif

DECLARE_SOURCE("$Revision$");

#define new CAM_DEBUG_NEW




///////////////////////////////////////////////////////////////////////////////////////////////
// The OpLiveEffect class															    	 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	OpLiveEffect::OpLiveEffect()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
	Purpose:	Constructor.
	Errors:		
	See also:	

********************************************************************************************/
OpLiveEffect::OpLiveEffect()
{
	m_pLERange=NULL;
	m_bAppliedNewEffects = FALSE;
	m_iStackPos = 0;
	m_bPreviewQuality = FALSE;
}



/********************************************************************************************

>	OpLiveEffect::~OpLiveEffect()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
	Purpose:	Destructor.
	Errors:		
	See also:	

********************************************************************************************/
OpLiveEffect::~OpLiveEffect()
{
	if (m_pLERange)
	{
		delete m_pLERange;
		m_pLERange = NULL;
	}
}



/********************************************************************************************

>	static BOOL OpLiveEffect::Init()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
BOOL OpLiveEffect::Init()
{
	BOOL bOK = TRUE;
	// Use an ParamOpDescriptor to allow OpApplyLiveEffect to receive extra information
	// about the menu item that invoked it...
	ParamOpDescriptor* pOD = NULL;
	pOD = new ParamOpDescriptor(OPTOKEN_APPLY_LIVEEFFECT,
								CC_RUNTIME_CLASS(OpApplyLiveEffect),
								OpApplyLiveEffect::GetParamState);
	bOK = bOK && (pOD!=NULL);

	pOD = new ParamOpDescriptor(OPTOKEN_EDIT_LIVEEFFECT,
								CC_RUNTIME_CLASS(OpEditLiveEffect),
								OpEditLiveEffect::GetParamState);
	bOK = bOK && (pOD!=NULL);

	pOD = new ParamOpDescriptor(OPTOKEN_EDIT_LEGACYEFFECT,
								CC_RUNTIME_CLASS(OpEditLegacyEffect),
								OpEditLegacyEffect::GetParamState);
	bOK = bOK && (pOD!=NULL);

#ifdef FEATHER_EFFECT
	pOD = new ParamOpDescriptor(OPTOKEN_APPLY_FEATHEREFFECT,
								CC_RUNTIME_CLASS(OpApplyFeatherEffect),
								OpApplyFeatherEffect::GetParamState);
	bOK = bOK && (pOD!=NULL);
#endif

/*	bOK = bOK && RegisterOpDescriptor(0,								// Tool ID
 									_R(IDS_EDIT_LIVEEFFECT),				// String resource ID
									CC_RUNTIME_CLASS(OpEditLiveEffect),	// Runtime class
 									OPTOKEN_EDIT_LIVEEFFECT,			// Token string
 									OpEditLiveEffect::GetState,		// GetState function
									0,								// Help ID
									_R(IDBBL_EDIT_LIVEEFFECT),			// Bubble ID
									0,								// Resource ID
									0,								// Control ID
									SYSTEMBAR_ILLEGAL,				// Bar ID
									TRUE,							// Receive system messages
									FALSE,							// Smart duplicate operation
									FALSE,							// Clean operation
									0,								// No vertical counterpart
									GREY_WHEN_NO_CURRENT_DOC |
									DONT_GREY_WHEN_SELECT_INSIDE);	// automatic state checking.
*/
	bOK = bOK && RegisterOpDescriptor(0,								// Tool ID
 									_R(IDS_DELETE_LIVEEFFECT),				// String resource ID
									CC_RUNTIME_CLASS(OpDeleteLiveEffect),	// Runtime class
 									OPTOKEN_DELETE_LIVEEFFECT,			// Token string
 									OpDeleteLiveEffect::GetState,		// GetState function
									0,								// Help ID
									_R(IDBBL_DELETE_LIVEEFFECT),			// Bubble ID
									0,								// Resource ID
									0,								// Control ID
									SYSTEMBAR_ILLEGAL,				// Bar ID
									TRUE,							// Receive system messages
									FALSE,							// Smart duplicate operation
									FALSE,							// Clean operation
									0,								// No vertical counterpart
									GREY_WHEN_NO_CURRENT_DOC |
									DONT_GREY_WHEN_SELECT_INSIDE);	// automatic state checking.

	bOK = bOK && RegisterOpDescriptor(0,								// Tool ID
 									_R(IDS_DELETEALL_LIVEEFFECT),				// String resource ID
									CC_RUNTIME_CLASS(OpDeleteAllLiveEffect),	// Runtime class
 									OPTOKEN_DELETEALL_LIVEEFFECT,			// Token string
 									OpDeleteAllLiveEffect::GetState,		// GetState function
									0,								// Help ID
									_R(IDBBL_DELETEALL_LIVEEFFECT),			// Bubble ID
									0,								// Resource ID
									0,								// Control ID
									SYSTEMBAR_ILLEGAL,				// Bar ID
									TRUE,							// Receive system messages
									FALSE,							// Smart duplicate operation
									FALSE,							// Clean operation
									0,								// No vertical counterpart
									GREY_WHEN_NO_CURRENT_DOC |
									DONT_GREY_WHEN_SELECT_INSIDE);	// automatic state checking.

	bOK = bOK && RegisterOpDescriptor(0,								// Tool ID
 									_R(IDS_CHANGE_EFFECT_RES),				// String resource ID
									CC_RUNTIME_CLASS(OpEffectRes),	// Runtime class
 									OPTOKEN_CHANGE_EFFECT_RES,			// Token string
 									OpEffectRes::GetState,		// GetState function
									0,								// Help ID
									_R(IDBBL_CHANGE_EFFECT_RES),			// Bubble ID
									0,								// Resource ID
									0,								// Control ID
									SYSTEMBAR_ILLEGAL,				// Bar ID
									TRUE,							// Receive system messages
									FALSE,							// Smart duplicate operation
									FALSE,							// Clean operation
									0,								// No vertical counterpart
									GREY_WHEN_NO_CURRENT_DOC |
									DONT_GREY_WHEN_SELECT_INSIDE);	// automatic state checking.

	bOK = bOK && RegisterOpDescriptor(0,								// Tool ID
 									_R(IDS_CHANGE_EFFECT_LOCK),				// String resource ID
									CC_RUNTIME_CLASS(OpEffectLock),	// Runtime class
 									OPTOKEN_CHANGE_EFFECT_LOCK,			// Token string
 									OpEffectLock::GetState,		// GetState function
									0,								// Help ID
									_R(IDBBL_CHANGE_EFFECT_LOCK),			// Bubble ID
									0,								// Resource ID
									0,								// Control ID
									SYSTEMBAR_ILLEGAL,				// Bar ID
									TRUE,							// Receive system messages
									FALSE,							// Smart duplicate operation
									FALSE,							// Clean operation
									0,								// No vertical counterpart
									GREY_WHEN_NO_CURRENT_DOC |
									DONT_GREY_WHEN_SELECT_INSIDE);	// automatic state checking.

	bOK = bOK && RegisterOpDescriptor(0,								// Tool ID
 									_R(IDS_CHANGE_EFFECT_LOCKALL),				// String resource ID
									CC_RUNTIME_CLASS(OpEffectLockAll),	// Runtime class
 									OPTOKEN_CHANGE_EFFECT_LOCKALL,			// Token string
 									OpEffectLockAll::GetState,		// GetState function
									0,								// Help ID
									_R(IDBBL_CHANGE_EFFECT_LOCKALL),			// Bubble ID
									0,								// Resource ID
									0,								// Control ID
									SYSTEMBAR_ILLEGAL,				// Bar ID
									TRUE,							// Receive system messages
									FALSE,							// Smart duplicate operation
									FALSE,							// Clean operation
									0,								// No vertical counterpart
									GREY_WHEN_NO_CURRENT_DOC |
									DONT_GREY_WHEN_SELECT_INSIDE);	// automatic state checking.

	return bOK;
}



/********************************************************************************************

>	static ListRange* OpLiveEffect::DoApplyLiveEffect(Range* pSel, String_256 strEffectID, String_64 strDisplayName, double dResolution, IXMLDOMDocumentPtr pEditsList = NULL, BOOL bMakeLocked = FALSE, BOOL bDetectDirectBitmaps = FALSE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/09/2004
	Inputs:		pSel - pointer to range of nodes to insert LiveEffects above
				strDispayName - The display name of the effect we're going to apply
	Outputs:	pNewSel - pointer to new range of LiveEffect nodes
						- NULL if no new live effects added
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
ListRange* OpLiveEffect::DoApplyLiveEffect(Range* pSel, String_256 strEffectID, String_64 strDisplayName, double dResolution, IXMLDOMDocumentPtr pEditsList, BOOL bMakeLocked, BOOL bDetectDirectBitmaps)
{
	ENSURE(pSel, "DoApplyLiveEffect not given a useful range");
	ENSURE(strEffectID!=String(""), "DoApplyLiveEffect given blank effect name string");

	ListRange* pLERange = new ListRange();
	ENSURE(pLERange, "DoApplyEffect can't create a ListRange");
	if (pLERange==NULL)
		return NULL;

	Node* pNode = pSel->FindFirst();
	while (pNode)
	{
		// create a new NodeLiveEffect, which we will shortly insert into the tree;
		// note that ALLOC_WITH_FAIL automatically calls FailAndExecute() if things go wrong.
		NodeBitmapEffect* pBmpFXNode = NULL;
		pBmpFXNode = DoApplyLiveEffect(this, pNode, strEffectID, strDisplayName, dResolution, pEditsList, bMakeLocked);

		if (pBmpFXNode)
		{
			// If the node we're applying to can supply DirectBitmaps and we can accept them
			// Then change res to be "automatic"
			if (bDetectDirectBitmaps &&
				pNode->IsAnObject() &&
				((NodeRenderableInk*)pNode)->CanSupplyDirectBitmap() &&
				dResolution != 0
				)
			{
				// Child node can supply a DirectBitmap so it would be better if we were in
				// "Automatic" mode.
				pBmpFXNode->SetPixelsPerInch(0);			// Set "Automatic"

				// We can only test EnableDirectCapture after we have set the res to automatic
				// Otherwise it may return FALSE because the stored res (dResolution) may not match that
				// of the DirectBitmap child.
				// If it fails, then reset resolution back to what was specified.
				if (!pBmpFXNode->EnableDirectCapture())
					pBmpFXNode->SetPixelsPerInch(dResolution);
			}

			// Deselect the node and select the live effect instead
			// TODO: Really?
			((NodeRenderable*)pNode)->DeSelect(FALSE);
			pBmpFXNode->Select(FALSE);

			pLERange->AddNode(pBmpFXNode);

			// Make sure the attributes are optimised correctly
			BOOL ok = DoFactorOutCommonChildAttributes(pBmpFXNode);
			if (!ok)
				pBmpFXNode = NULL;
		}

		if (!pBmpFXNode)
		{
			if (pLERange->FindFirst()==NULL)
			{
				// Don't let an empty list of LiveEffect nodes hang around
				delete pLERange;
				pLERange = NULL;
			}
			return pLERange;
		}

		pNode = pSel->FindNext(pNode);
	}

	if (pLERange->FindFirst()==NULL)
	{
		// Don't let an empty list of LiveEffect nodes hang around
		delete pLERange;
		pLERange = NULL;
	}

	return pLERange;
}




/********************************************************************************************

>	static NodeBitmapEffect* OpLiveEffect::DoApplyLiveEffect(UndoableOperation* pOp,
															 Node* pNode,
															 String_256 strEffectID,
															 String_64 strDisplayName,
															 double dResolution,
															 IXMLDOMDocumentPtr pEditsList = NULL,
															 BOOL bMakeLocked = FALSE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/02/2005
	Inputs:		pOp - Op to add undo info to
				pNode - pointer to node to insert LiveEffects above
				strEffectID - Unique identifier for Op
				strDispayName - The display name of the effect we're going to apply
				pEditsList - pointer to optional list of edit params for this effect
	Outputs:	-
	Returns:	TRUE if worked OK
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
NodeBitmapEffect* OpLiveEffect::DoApplyLiveEffect(UndoableOperation* pOp,
												  Node* pNode,
												  String_256 strEffectID,
												  String_64 strDisplayName,
												  double dResolution,
												  IXMLDOMDocumentPtr pEditsList,
												  BOOL bMakeLocked)
{
	ERROR2IF(pOp==NULL, NULL, "Missing Op pointer");
	ERROR2IF(pNode==NULL, NULL, "Missing Node pointer");
	ERROR2IF(strEffectID==String(""), NULL, "DoApplyLiveEffect given blank effect name string");
	ERROR2IF(strDisplayName==String(""), NULL, "DoApplyLiveEffect given blank display name string");

	BOOL bSelectInside = pNode->IsSelected() || pNode->IsParentOfSelected();

	// Possible solution to issue of Bitmap with Feather attr under DirectBitmap effects
	// Detect that state, delete the feather attr, apply a feather effect with the same values
	//pNode = ApplyDirectFeatherBodge(pNode);

	// create a new NodeLiveEffect, which we will shortly insert into the tree;
	// note that ALLOC_WITH_FAIL automatically calls FailAndExecute() if things go wrong.
	NodeBitmapEffect* pBmpFXNode = NULL;
	if (bMakeLocked)
	{
		ALLOC_WITH_FAIL(pBmpFXNode, new NodeLockedEffect, pOp);
	}
	else
	{
		ALLOC_WITH_FAIL(pBmpFXNode, new NodeLiveEffect, pOp);
	}
	if (pBmpFXNode==NULL)
		return NULL;

	// Get details from XPE so that displayname gets translated into
	// locale language if not already
	BOOL bDestructive = FALSE;
	XPEHost::GetEffectDetails(strEffectID, &strDisplayName, &bDestructive);
	ERROR3IF(bDestructive, "DoApplyLiveEffect asked to apply destructive effect!!!");
	if (bDestructive)
		return NULL;

	// Set effect params
	pBmpFXNode->SetPostProcessorID(strEffectID);
	pBmpFXNode->SetDisplayName(strDisplayName);
	pBmpFXNode->SetPixelsPerInch(dResolution);
	if (pEditsList)
	{
		pBmpFXNode->SetEditList(pEditsList);
	}
	else if (!pBmpFXNode->SetDefaultEditList())
	{
		return NULL;
	}

	if (pBmpFXNode->IsLockedEffect())
		((NodeLockedEffect*)pBmpFXNode)->SetLockPermanence(bDestructive);

	// put an action to hide the NodeLiveEffect onto the undo action-list,
	// so that if the user presses undo then it will be hidden.
	HideNodeAction* pUndoHideNodeAction = NULL;
	ActionCode ac = HideNodeAction::Init(pOp,
										pOp->GetUndoActionList(),
										pBmpFXNode,
										FALSE,		// don't include subtree size
										(Action**)&pUndoHideNodeAction,
										FALSE);		// don't tell subtree when undone
	if (ac == AC_FAIL)
		return NULL;

	// right! we've got our node, we've got our action - lets stick it in the tree
	// (at a position just next to the last node which will go in the group).
	pBmpFXNode->AttachNode(pNode, NEXT);

	// Move the node below the newly inserted LiveEffect
	BOOL ok = pOp->DoMoveNode(pNode, pBmpFXNode, FIRSTCHILD);
	if (!ok)
		return NULL;

// Set the OpPersmission flags in the newly inserted node
// Otherwise the tree scanner in UpdateChangedNodes won't go below this node
ObjChangeFlags cFlags(NULL, TRUE, NULL, NULL, NULL, NULL, NULL, TRUE);	// ReplaceNode & RegenerateNode
ObjChangeParam ObjChange(OBJCHANGE_STARTING, cFlags, NULL, pOp);
pBmpFXNode->AllowOp(&ObjChange);

	// Deselect the node and select the live effect instead
	// TODO: Really?
//	((NodeRenderable*)pNode)->DeSelect(FALSE);
//	pBmpFXNode->Select(FALSE);

	return pBmpFXNode;
}




#ifdef FEATHER_EFFECT
/********************************************************************************************

>	static ListRange* OpLiveEffect::DoApplyFeatherEffect(UndoableOperation* pOp,
														  Range* pSel,
														  String_256 strEffectID,
														  String_64 strDisplayName,
														  MILLIPOINT FeatherSize,
														  CProfileBiasGain Profile)
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/2004
	Inputs:		pSel - pointer to range of nodes to insert LiveEffects above
				strDispayName - The display name of the effect we're going to apply
	Outputs:	pNewSel - pointer to new range of LiveEffect nodes
						- NULL if no new live effects added
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
ListRange* OpLiveEffect::DoApplyFeatherEffect(UndoableOperation* pOp,
											  Range* pSel,
											  String_256 strEffectID,
											  String_64 strDisplayName,
											  MILLIPOINT FeatherSize,
											  CProfileBiasGain Profile)
{
	ENSURE(pSel, "DoApplyFeatherEffect not given a useful range");
	ENSURE(strEffectID!=String(""), "DoApplyFeatherEffect given blank effect name string");

	ListRange* pLERange = new ListRange();
	ENSURE(pLERange, "DoApplyEffect can't create a ListRange");
	if (pLERange==NULL)
		return NULL;

	Node* pNode = pSel->FindFirst();
	BOOL bOK = TRUE;
	while (pNode && bOK)
	{
		NodeFeatherEffect* pEffect = DoApplyFeatherEffectNode(pOp, pNode, strEffectID, strDisplayName, FeatherSize, Profile);
		if (pEffect)
		{
			pLERange->AddNode(pEffect);

			// Make sure the attributes are optimised correctly
			bOK = pOp->DoFactorOutCommonChildAttributes(pEffect);
		}
		else
			bOK = FALSE;

		pNode = pSel->FindNext(pNode);
	}

	if (pLERange->FindFirst()==NULL || bOK==FALSE)
	{
		// Don't let an empty list of LiveEffect nodes hang around
		delete pLERange;
		pLERange = NULL;
	}

	return pLERange;
}




/********************************************************************************************

>	static NodeFeatherEffect* OpLiveEffect::DoApplyFeatherEffectNode(UndoableOperation* pOp,
														  Node* pSel,
														  String_256 strEffectID,
														  String_64 strDisplayName,
														  MILLIPOINT FeatherSize,
														  CProfileBiasGain Profile)
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/03/2005
	Inputs:		pSel - pointer to node to insert Feather Effect above
				strDispayName - The display name of the effect we're going to apply
	Outputs:	-
	Returns:	Pointer to newly created NodeFeatherEffect or NULL
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
NodeFeatherEffect* OpLiveEffect::DoApplyFeatherEffectNode(UndoableOperation* pOp,
											  Node* pNode,
											  String_256 strEffectID,
											  String_64 strDisplayName,
											  MILLIPOINT FeatherSize,
											  CProfileBiasGain Profile)
{
	ENSURE(pNode, "DoApplyFeatherEffectNode not given a useful node");
	ENSURE(strEffectID!=String(""), "DoApplyFeatherEffectNode given blank effect name string");

	BOOL bSelectInside = pNode->IsSelected() || pNode->IsParentOfSelected();

	// create a new NodeLiveEffect, which we will shortly insert into the tree;
	// note that ALLOC_WITH_FAIL automatically calls FailAndExecute() if things go wrong.
	NodeFeatherEffect* pBmpFXNode = NULL;
	ALLOC_WITH_FAIL(pBmpFXNode, new NodeFeatherEffect, pOp);
	if (pBmpFXNode == NULL)
		return NULL;

	// put an action to hide the NodeClipViewController onto the undo action-list,
	// so that if the user presses undo then it will be hidden.
	pBmpFXNode->SetPostProcessorID(strEffectID);
	pBmpFXNode->SetDisplayName(strDisplayName);
	pBmpFXNode->m_FeatherSize = FeatherSize;
	pBmpFXNode->m_Profile = Profile;

	HideNodeAction* pUndoHideNodeAction = NULL;
	ActionCode ac = HideNodeAction::Init(pOp,
										pOp->GetUndoActions(),
										pBmpFXNode,
										FALSE,		// don't include subtree size
										(Action**)&pUndoHideNodeAction,
										FALSE);		// don't tell subtree when undone
	if (ac == AC_FAIL)
	{
		delete pBmpFXNode;
		return NULL;
	}

	// right! we've got our node, we've got our action - lets stick it in the tree
	// (at a position just next to the last node which will go in the group).
	pBmpFXNode->AttachNode(pNode, NEXT);

	// Move the node below the newly inserted LiveEffect
	BOOL bOK = pOp->DoMoveNode(pNode, pBmpFXNode, FIRSTCHILD);
	if (!bOK)
		return NULL;

	// Deselect the node and select the live effect instead
	((NodeRenderable*)pNode)->DeSelect(FALSE);
	pBmpFXNode->Select(FALSE);

// Set the OpPersmission flags in the newly inserted node
// Otherwise the tree scanner in UpdateChangedNodes won't go below this node
ObjChangeFlags cFlags(NULL, TRUE, NULL, NULL, NULL, NULL, NULL, TRUE);	// ReplaceNode & RegenerateNode
ObjChangeParam ObjChange(OBJCHANGE_STARTING, cFlags, NULL, pOp);
pBmpFXNode->AllowOp(&ObjChange);

	return pBmpFXNode;
}
#endif




/********************************************************************************************

>	virtual ListRange* OpLiveEffect::DoApplyLockedEffect(Range* pSel,
														  String_256 strEffectID,
														  String_64 strDisplayName,
														  double dResolution,
														  IXMLDOMDocumentPtr pEditsList = NULL,
														  BOOL bDetectDirectBitmaps = FALSE)


	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/09/2004
	Inputs:		pSel - pointer to range of nodes to insert LiveEffects above
				strDispayName - The display name of the effect we're going to apply
	Outputs:	pNewSel - pointer to new range of LiveEffect nodes
						- NULL if no new live effects added
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
ListRange* OpLiveEffect::DoApplyLockedEffect(Range* pSel,
											  String_256 strEffectID,
											  String_64 strDisplayName,
											  double dResolution,
											  IXMLDOMDocumentPtr pEditsList,
											  BOOL bDetectDirectBitmaps)
{
	ENSURE(pSel, "DoApplyLockedEffect not given a useful range");
	ENSURE(strEffectID!=String(""), "DoApplyLockedEffect given blank effect name string");

	ListRange* pLERange = new ListRange();
	ENSURE(pLERange, "DoApplyEffect can't create a ListRange");
	if (pLERange==NULL)
		return NULL;

	// we need to insert the controller node at the position of the highest
	// selected node in the z-order, ie last in the selection, so find it.
	Node* pFirstNode = pSel->FindFirst();
	Node* pNode = pFirstNode;
	Node* pLastNode = NULL;
	while (pNode != NULL)
	{
		pLastNode = pNode;
		pNode = pSel->FindNext(pLastNode);
	}	// loop terminates with pNode == NULL, pLastNode == last-node-in-sel.

	// create a new NodeClipViewController, which we will shortly insert into the tree;
	// note that ALLOC_WITH_FAIL automatically calls FailAndExecute() if things go wrong.
	NodeLockedEffect* pBmpFXNode = NULL;
	ALLOC_WITH_FAIL(pBmpFXNode, new NodeLockedEffect, this);
	BOOL ok = (pBmpFXNode != NULL);

	if (ok)
	{
		// Get details from XPE so that displayname gets translated into
		// locale language if not already and destructive state is updated
		// for the current machine (shouldn't really change, though)
		BOOL bDestructive = TRUE;
		XPEHost::GetEffectDetails(strEffectID, &strDisplayName, &bDestructive);

		pBmpFXNode->SetPostProcessorID(strEffectID);
		pBmpFXNode->SetDisplayName(strDisplayName);
		pBmpFXNode->SetPixelsPerInch(dResolution);
		if (pEditsList)
		{
			pBmpFXNode->SetEditList(pEditsList);
		}
		else if (!pBmpFXNode->SetDefaultEditList())
		{
			delete pBmpFXNode;

			// Don't let an empty list of LiveEffect nodes hang around
			delete pLERange;
			pLERange = NULL;
			return pLERange;
		}
		pBmpFXNode->SetLockPermanence(bDestructive);

		// put an action to hide the NodeClipViewController onto the undo action-list,
		// so that if the user presses undo then it will be hidden.
		HideNodeAction* pUndoHideNodeAction = NULL;
		ActionCode ac = HideNodeAction::Init(this,
											&UndoActions,
											pBmpFXNode,
											FALSE,		// don't include subtree size
											(Action**)&pUndoHideNodeAction,
											FALSE);		// don't tell subtree when undone
		if (ac == AC_FAIL)
		{
			delete pBmpFXNode;

			// Don't let an empty list of LiveEffect nodes hang around
			delete pLERange;
			pLERange = NULL;
			return pLERange;
		}
		else
		{
			// right! we've got our node, we've got our action - lets stick it in the tree
			// (at a position just next to the last node which will go in the group).
			pBmpFXNode->AttachNode(pLastNode, NEXT);
			pLERange->AddNode(pBmpFXNode);
		}
	}

	// move each item from the selection into our ClipView group,
	// remembering to deselect them as we go.
	// TODO:
	//	sneaky suspicion I should be putting this in a Do fn in UndoableOperation...
	if (ok)
	{
		pNode = pSel->FindNext(pFirstNode);				// the node we're moving now.

//		if (pFirstNode->IsAnObject())
//			ok = DoLocaliseCommonAttributes((NodeRenderableInk*)pFirstNode);

		ok = DoMoveNode(pFirstNode, pBmpFXNode, FIRSTCHILD);
		if (ok)
			((NodeRenderable*)pFirstNode)->DeSelect(FALSE, TRUE);
	}

	Node* pNextNode		= NULL;							// the next node to move.
	Node* pAnchorNode	= pFirstNode;					// the node we've just moved.
	while (ok && pNode != NULL)
	{
		// get the next node to move.
		pNextNode = pSel->FindNext(pNode);

		// now move the current node next to the anchor and deselect it.
//		if (pNode->IsAnObject())
//			ok = DoLocaliseCommonAttributes((NodeRenderableInk*)pNode);

		ok = DoMoveNode(pNode, pAnchorNode, NEXT);
		if (ok)
			((NodeRenderable*)pNode)->DeSelect(FALSE, TRUE);

		// get the new anchor node and the next node to move.
		pAnchorNode = pNode;
		pNode = pNextNode;
	}

	// If we're applied to a single node and that node can supply DirectBitmaps and we can accept them
	// Then change res to be same as child node
	NodeRenderableInk* pInkNode = pBmpFXNode->GetInkNodeFromController();
	if (bDetectDirectBitmaps &&
		pInkNode &&
		pInkNode->CanSupplyDirectBitmap()
		)
	{
		// Child node can supply a DirectBitmap so it would be better if we had same resolution
		// as the child node...
		double dDirectRes = 0;
		pInkNode->GetDirectBitmap(NULL, NULL, NULL, NULL, NULL, &dDirectRes);
		if (dDirectRes!=0)
		{
			pBmpFXNode->SetPixelsPerInch(dDirectRes);			// Set res

			// We can only test EnableDirectCapture after we have set the res correctly
			// Otherwise it may return FALSE because the stored res (dResolution) may not match that
			// of the DirectBitmap child.
			// If it fails, then reset resolution back to what was specified.
			if (!pBmpFXNode->EnableDirectCapture())
				pBmpFXNode->SetPixelsPerInch(dResolution);
		}
	}

	if (ok)
	{
		// select the new destructive effect, but don't draw any blobs yet.
		pBmpFXNode->Select(FALSE);

		// factor out any common attributes.
		DoFactorOutCommonChildAttributes(pBmpFXNode);

// Set the OpPersmission flags in the newly inserted node
// Otherwise the tree scanner in UpdateChangedNodes won't go below this node
ObjChangeFlags cFlags(NULL, TRUE, NULL, NULL, NULL, NULL, NULL, TRUE);	// ReplaceNode & RegenerateNode
ObjChangeParam ObjChange(OBJCHANGE_STARTING, cFlags, NULL, this);
pBmpFXNode->AllowOp(&ObjChange);
	}

	if (pLERange->FindFirst()==NULL)
	{
		// Don't let an empty list of LiveEffect nodes hang around
		delete pLERange;
		pLERange = NULL;
	}

	return pLERange;
}




/********************************************************************************************

>	BOOL OpLiveEffect::IsTopOfStack(EffectsStack* pPPStack, INT32 iStackPos)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/10/2004
	Inputs:		iStackPos - position in LiveEffect stack to find nodes
							0 means first stack pos above selection
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
BOOL OpLiveEffect::IsTopOfStack(EffectsStack* pPPStack, INT32 iStackPos)
{
	return (iStackPos>=(INT32)pPPStack->GetCount());
}




/********************************************************************************************

>	BOOL OpLiveEffect::EnsureLiveEffectOriginalBitmaps()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/09/2004
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	Ensure that all NodeBitmapEffects in the current selection
				have original bitmaps ready and wait for us to pick up
	Errors:		
	See also:	

********************************************************************************************/
BOOL OpLiveEffect::EnsureLiveEffectOriginalBitmaps()
{
	if (m_pLERange==NULL)
		return TRUE;

	BOOL bRenderingNeeded = FALSE;
	DocView* pDocView = DocView::GetSelected();

	bRenderingNeeded = FALSE;
	Node* pNode = m_pLERange->FindFirst();
	while (pNode)
	{
		ENSURE(pNode->IsBitmapEffect(), "LiveEffects list contains non-LiveEffect in EnsureLiveEffectOriginalBitmaps");
		NodeBitmapEffect* pLE = (NodeBitmapEffect*)pNode;
		if (!pLE->GetOriginalBitmap(NULL, NULL, NULL) && pLE->CanGenerateBitmap())
		{
			// We need to forcibly render this node to get its original
			// bitmap set up
			//
			// Version2 Bitmap Generation
			//
			// Grab bitmap without using on screen rendering
			// This is more efficient than rendering the whole tree and 
			// allows parameters to be controlled independently from screen view
			//
			// Ideally:
			// Start a render into a RenderRegion
			// Don't allow the RenderRegion to grab bitmap of its own - NOT DONE
			// Don't allow the RenderRegion to blit captures into it's own bitmap - NOT DONE
			// Ensure only the effect and it's children are rendered - DONE!
			// Just let the effect rendering functions create their capture bitmap - DONE!

			// Create a new GRenderDIB region
			// Most of the parameters here are immaterial because we are not really
			// interested in the results of the rendered region - just the capture
			pLE->ReleaseCached(TRUE, FALSE);							// We will change so tell parents to uncache

			// Bodge? Ensure that locked effects will render their children
			// because they don't respond to ReleaseCached...
			if (pLE->IsLockedEffect())
				pLE->SetProcessedBitmap(NULL, NULL, DocRect(), 0, 0, 0, 0);

			View* pView = View::GetCurrent();
			Spread* pSpread = Document::GetSelectedSpread();
			Matrix Mat = pView->ConstructScaledRenderingMatrix(pSpread, 1.0);
			FIXED16 Scale;
			Mat.Decompose(&Scale);
			double dpi = 0.0;	// use screen dpi (ie PIXELS_PER_INCH)
			GRenderDIB* pNewGRR = new GRenderDIB(pLE->GetBoundingRect(), Mat, Scale, 32, dpi);
			ERROR2IF(pNewGRR == NULL, FALSE, "Failed to create a GRenderDIB!");

			// State flags + pixel width calculations
			if (!pNewGRR->AttachDevice(pView, NULL, pSpread))		// view and spread from previous RR rather than Current - nb in create bitmap copy case
			{
				ERROR3("Cannot attach devices");
				
				delete pNewGRR;
				pNewGRR = NULL;
				
				return FALSE;
			}

			pNewGRR->m_DoCompression = FALSE;
			pNewGRR->InitDevice();
			pNewGRR->InitAttributes();
			pNewGRR->SetLineAttributes();
			pNewGRR->SetFillAttributes();
			pNewGRR->SetBackmostChangedNode(pSpread);		// Don't allow any cached 24BPP bitmaps to be used!

			// NB following call gets GRendRegion to create the bitmap into which GDraw will render
			BOOL Started = pNewGRR->StartRender();

			if (Started)
			{
				// Setup attribute state outside this subtree
				CCAttrMap* pAttrMap = new CCAttrMap;
				if (pAttrMap!=NULL)
				{
					pLE->FindAppliedAttributes(pAttrMap, 5000, NULL, FALSE, FALSE);
					pAttrMap->Render(pNewGRR);
					delete pAttrMap;
				}

				// This is the important bit - force top quality rendering
				// so that effects capture is done properly regardless of 
				// the current document/view quality
				pNewGRR->RRQuality.SetQuality(QUALITY_MAX);
				pNewGRR->SetQualityLevel();

				pNewGRR->SetImmediateRender(TRUE);

				pNewGRR->RenderTree(pLE, FALSE, FALSE);
				pNewGRR->StopRender();

			}

			// Sanity check
			// We really should have generated an original bitmap by now!
			ERROR2IF(!pLE->GetOriginalBitmap(NULL, NULL, NULL), FALSE, "EnsureLiveEffectOriginalBitmaps failed to generate an original bitmap");

			delete pNewGRR;
			pNewGRR = NULL;
		}

		pNode = m_pLERange->FindNext(pNode);
	}

	return TRUE;
}




/********************************************************************************************

>	BOOL OpLiveEffect::GenerateBitmap(NodeBitmapEffect* pBitmapEffect,
									  double dResolution,
									  LPBITMAPINFO* ppInfo,
									  LPBYTE* ppBits,
									  DocRect* prectBounds)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/2005
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:
	Errors:		
	See also:	

********************************************************************************************/
BOOL OpLiveEffect::GenerateBitmap(NodeBitmapEffect* pBitmapEffect,
								  double dResolution,
								  LPBITMAPINFO* ppInfo,
								  LPBYTE* ppBits,
								  DocRect* prectBounds)
{
	// We need to forcibly render this node to get its original
	// bitmap set up
	//
	// Version2 Bitmap Generation
	//
	// Grab bitmap without using on screen rendering
	// This is more efficient than rendering the whole tree and 
	// allows parameters to be controlled independently from screen view
	//
	// Ideally:
	// Start a render into a RenderRegion
	// Don't allow the RenderRegion to grab bitmap of its own - NOT DONE
	// Don't allow the RenderRegion to blit captures into it's own bitmap - NOT DONE
	// Ensure only the effect and it's children are rendered - DONE!
	// Just let the effect rendering functions create their capture bitmap - DONE!

	// Create a new GRenderDIB region
	// Most of the parameters here are immaterial because we are not really
	// interested in the results of the rendered region - just the capture
	View* pView = View::GetCurrent();
	Spread* pSpread = Document::GetSelectedSpread();
	Matrix Mat = pView->ConstructScaledRenderingMatrix(pSpread, 1.0);
	FIXED16 Scale;
	Mat.Decompose(&Scale);
	GRenderDIB* pNewGRR = new GRenderDIB(pBitmapEffect->GetBoundingRect(), Mat, Scale, 32, dResolution);
	ERROR2IF(pNewGRR == NULL, FALSE, "Failed to create a GRenderDIB!");

	// State flags + pixel width calculations
	if (!pNewGRR->AttachDevice(pView, NULL, pSpread))		// view and spread from previous RR rather than Current - nb in create bitmap copy case
	{
		ERROR3("Cannot attach devices");
		
		delete pNewGRR;
		pNewGRR = NULL;
		
		*ppInfo = NULL;
		*ppBits = NULL;

		*prectBounds = DocRect();

		return FALSE;
	}

	pNewGRR->m_DoCompression = TRUE;			// We do want transparency
	pNewGRR->InitDevice();
	pNewGRR->InitAttributes();
	pNewGRR->SetLineAttributes();
	pNewGRR->SetFillAttributes();

	// NB following call gets GRendRegion to create the bitmap into which GDraw will render
	BOOL Started = pNewGRR->StartRender();

	if (Started)
	{
		// Setup attribute state outside this subtree
		CCAttrMap* pAttrMap = new CCAttrMap;
		if (pAttrMap!=NULL)
		{
			pBitmapEffect->FindAppliedAttributes(pAttrMap, 5000, NULL, FALSE, FALSE);
			pAttrMap->Render(pNewGRR);
			delete pAttrMap;
		}

		// This is the important bit - force top quality rendering
		// so that effects capture is done properly regardless of 
		// the current document/view quality
		pNewGRR->RRQuality.SetQuality(QUALITY_MAX);
		pNewGRR->SetQualityLevel();

		pNewGRR->RenderTree(pBitmapEffect, FALSE, FALSE);
		pNewGRR->StopRender();
	}

	*prectBounds = pNewGRR->GetClipRect();			// Get pixel-aligned rect
	pNewGRR->GetBitmapPointers(ppInfo, ppBits);		// Get the bitmap data out of the region, converting T channel
	pNewGRR->SetBitmapPointers(NULL, NULL);			// We own the bitmap now, it will not be deallocated by
													// the following renderegion deleteion
	delete pNewGRR;
	pNewGRR = NULL;

	return TRUE;
}




/********************************************************************************************

>	BOOL OpLiveEffect::SetChanged(BOOL bNewState)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/09/2004
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:
	Errors:		
	See also:	

********************************************************************************************/
BOOL OpLiveEffect::SetChanged(BOOL bNewState)
{
	if (m_pLERange==NULL)
		return TRUE;

	Node* pNode = m_pLERange->FindFirst();
	while (pNode)
	{
		ENSURE(pNode->IsBitmapEffect(), "LiveEffects list contains non-LiveEffect in EnsureLiveEffectOriginalBitmaps");
		NodeBitmapEffect* pLE = (NodeBitmapEffect*)pNode;

		pLE->SetChanged(bNewState);

		pNode = m_pLERange->FindNext(pNode);
	}

	return TRUE;
}




/********************************************************************************************

>	BOOL OpLiveEffect::HasChanged()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/09/2004
	Inputs:		
	Outputs:	
	Returns:	TRUE if any of the nodes in the liveeffects list have changed since
				this operation started
				FALSE otherwise
	Purpose:	Detect whether any of the LiveEffect nodes created/edited by this
				operation have actually been changed/do anything useful...
	Errors:		
	See also:	

********************************************************************************************/
BOOL OpLiveEffect::HasChanged()
{
	if (m_pLERange==NULL)
		return FALSE;

	Node* pNode = m_pLERange->FindFirst();
	while (pNode)
	{
		ENSURE(pNode->IsBitmapEffect(), "LiveEffects list contains non-LiveEffect in EnsureLiveEffectOriginalBitmaps");
		NodeBitmapEffect* pLE = (NodeBitmapEffect*)pNode;

		if (pLE->HasChanged())
			return TRUE;

		pNode = m_pLERange->FindNext(pNode);
	}

	return FALSE;
}




/********************************************************************************************
>	static BOOL OpLiveEffect::UpdateCurrentEditor()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/2004
	Purpose:	Reset the current Op to refer to the new PPStack and tell the running
				XPE edit that things are changing

********************************************************************************************/

BOOL OpLiveEffect::UpdateCurrentEditor()
{
	OpLiveEffect* pCurrentOp = XPEHost::GetCurrentLEOp();

	// If we haven't got a list of editors yet then get one
	if (pCurrentOp==NULL)
		return FALSE;

	// TODO: May need to force a redraw if the current op is in preview mode at the moment
	ENSURE(pCurrentOp->GetPreviewQuality()==FALSE, "We can't leave the bitmaps in the tree in preview mode!");

	// Read the current op's details...
	String_256 strEffectID = pCurrentOp->m_strEffectID;
	INT32 iStackPos = pCurrentOp->m_iStackPos;

	// Stop the current Op and commit all of its changes
	// Disconnect it from the XPE system so that, just for the moment at least, other routines
	// think that there's no running edit session
	pCurrentOp->DoEndEdit(TRUE, TRUE);				// Sends ScreenChangeMsg - and thus releases pPPStack
	XPEHost::SetCurrentLEOp(NULL);					// Tells UpdateCurrentEditor not to try to update dying editor

	// Reset pPPStack in LiveEffectTool to reset the Tool after the ScreenChangeMsg sent above
	// (prevent idle handler looping forever calling this function)
	EffectsStack* pPPStack = EffectsStack::GetEffectsStackFromSelection();

	// Try to find the same effect in the new stack
	// If found then invoke a new Op to use the running editor on that effect
	BOOL bOK = pPPStack->FindBestProcessor(&strEffectID, &iStackPos);

	// Decide which sort of Operation we need to use instead...
	OpDescriptor* pOp = NULL;
	if (bOK)
	{
		// We found the right sort of Effect in the stack so we'll just edit that
		pOp = OpDescriptor::FindOpDescriptor(OPTOKEN_EDIT_LIVEEFFECT);
	}
	else
	{
		// We didn't find the right sort of effect so we'll apply a brand new one!
		// At the top of the stack
		pOp = OpDescriptor::FindOpDescriptor(OPTOKEN_APPLY_LIVEEFFECT);
		iStackPos = STACKPOS_INSERT_TOP;
	}

	// Start a new Op but tell it to use the existing edit session instead of making a new one
	if (pOp)
	{
		OpLiveEffectParam Param;
		Param.strOpUnique = strEffectID;
		Param.StackPosition = iStackPos;
		Param.pPPStack = pPPStack;
		Param.bReUseEditor = TRUE;				// This is the important param!
		BOOL bValid = XPEHost::GetEffectDetails(strEffectID, &Param.strMenuText, &Param.bIsDestructive);

		String_256 strDummy;
		if (bValid && pOp->GetOpsState(&strDummy).Greyed==FALSE)
		{
			pOp->Invoke(&Param);

			// Reset the Tool's current stack pointer now so that it doesn't need
			// to be updated under idle events with the consequent call to this function
			// and infinite loop...
			Tool* pTool = Tool::GetCurrent();
			if (pTool->GetID()==TOOLID_LIVEEFFECT)
				((LiveEffectsTool*)pTool)->UpdatePPStack(FALSE);

			// If that successfully set up a new editor op then we're fine to carry on
			if (XPEHost::GetCurrentLEOp()!=NULL)
			{
				delete pPPStack;
				return TRUE;
			}
		}
	}

	// If we get here then we must have failed in some way so close down the running editor
	XPEHost::EndEditLiveEffect();
	delete pPPStack;
	return FALSE;
}




/********************************************************************************************

>	virtual void OpLiveEffect::DoEndEdit(BOOL bChangingSelection, BOOL bBroadcast, BOOL bCancelled)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/09/2004
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
void OpLiveEffect::DoEndEdit(BOOL bChangingSelection, BOOL bBroadcast, BOOL bCancelled, BOOL bEmpty)
{
	BOOL ok = FALSE;
	BOOL bRecache = FALSE;

	// If the are no changes in any of the LiveEffect nodes as a result of this edit
	// And we only just created new LiveEffects in this Op
	// Then remove them from the tree because they are redundant
	if (bCancelled || (m_bAppliedNewEffects && (!HasChanged() || bEmpty)))
	{
		if (bChangingSelection)
			FailAndExecuteIgnoreSelActions();
		else
			FailAndExecute();
		// Tell the selrange that things have changed!
		GetApplication()->FindSelection()->Update();
		End();
		return;
	}

	// If we were editing but we are told that the edit has resulted in a NULL effect
	// Then we can remove the effect to keep things neat and tidy
	if (bEmpty && !m_bAppliedNewEffects && m_pLERange!=NULL)
	{
		BOOL bOK = DoDeletePostProcessor(this, m_pLERange);
		if (!bOK)
		{
			FailAndExecute();
			// Tell the selrange that things have changed!
			GetApplication()->FindSelection()->Update();
			End();
			return;
		}
	}

	// We have finished with the range of edited nodes now so we should delete it
	// to reduce memory usage while this object sits on the undo/redo lists
	if (m_pLERange)
	{
		delete m_pLERange;
		m_pLERange = NULL;
	}

	// Check whether XPE's state machine ended up in the correct state
	if (GetPreviewQuality())
	{
		ERROR3("We would expect XPE to have ended with at least one non-preview bitmap!");
		SetPreviewQuality(FALSE);
		bRecache = TRUE;
	}

	Tool* pTool = Tool::GetCurrent();

	// Update the LiveEffect Tool's permanent status
	if (pTool->GetID()==TOOLID_LIVEEFFECT)
	{
		((LiveEffectsTool*)pTool)->SetCurrentEffectID(m_strEffectID);
		((LiveEffectsTool*)pTool)->SetCurrentStackPos(m_iStackPos);
	}

	// render blobs off for tools which don't automatically redraw their blobs.
	Spread* pSpread = Document::GetSelectedSpread();

	// invalidate the (new) region bounding the selection.
	if (!DoInvalidateNodesRegions(*(GetApplication()->FindSelection()), TRUE, FALSE, FALSE, bRecache))
	{
		End();
		return;
	}

	// render blobs back on if the current tool doesn't automatically redraw its blobs.
	if (pSpread != NULL && pTool != NULL && !pTool->AreToolBlobsRenderedOnSelection())
		pTool->RenderToolBlobs(pSpread, NULL);

ObjChangeFlags cFlags(NULL, TRUE, NULL, NULL, NULL, NULL, NULL, TRUE);
ObjChangeParam ObjChange(OBJCHANGE_FINISHED, cFlags, NULL, this);
ok = UpdateChangedNodes(&ObjChange);

	if (bBroadcast)
	{
		BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::EFFECTSTACKCHANGED));
	}

	// Tell the selrange that things have changed!
	GetApplication()->FindSelection()->Update();

	End();
}




/********************************************************************************************

>	static BOOL OpLiveEffect::DoCopyPPStackToSelection(UndoableOperation* pOp, EffectsStack* pPPStack, Document* pSrcDoc, Document* pDestDoc)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/2004
	Inputs:		pPPStack - pointer to EffectsStack to copy
	Outputs:	
	Returns:	
	Purpose:	Copy the specified LiveEffect stack onto the selection
				Used by Paste Attributes
	Errors:		
	See also:	

********************************************************************************************/

BOOL OpLiveEffect::DoCopyPPStackToSelection(UndoableOperation* pOp, EffectsStack* pPPStack, Document* pSrcDoc, Document* pDestDoc)
{
	ERROR2IF(pPPStack==NULL || pPPStack->IsEmpty(), FALSE, "No stack/empty stack given to DoCopyPPStackToSelection\n");

	SelRange* pRange = GetApplication()->FindSelection();
	ERROR2IF(pRange==NULL, FALSE, "Can't find SelRange\n");

	if (pPPStack==NULL)
		return TRUE;			// That was easy!

	// Convert the pPPStack into a simple ListRange for DoCopyEffectsStack
	ListRange EffectsStack;
	INT32 l = 0;
	PPStackLevel* pLevel = pPPStack->GetLevel(l);
	while (pLevel)
	{
		ERROR3IF(pLevel->pPPNode==NULL, "PPStack has a badly formed level in it");

		if (pLevel->pPPNode->CanBeUnlocked())
			EffectsStack.AddNode(pLevel->pPPNode);

		pLevel = pPPStack->GetLevel(++l);
	}

	pOp->DoInvalidateNodesRegions(*pRange, TRUE, FALSE);

	// Now loop through the selection
	if (!EffectsStack.IsEmpty())
	{
		Node* pNode = pRange->FindFirst();
		Node* pNextNode = NULL;
		while (pNode)
		{
			pNextNode = pRange->FindNext(pNode);

			DoCopyEffectsStack(pOp, pNode, &EffectsStack, pSrcDoc, pDestDoc);

			pNode = pNextNode;
		}
	}

	pOp->DoInvalidateNodesRegions(*pRange, TRUE, FALSE);

	return TRUE;
}




/********************************************************************************************

>	static BOOL OpLiveEffect::DoCopyEffectsStack(UndoableOperation* pOp, Node* pDestNode, ListRange* pEffectsStack, Document* pSrcDoc, Document* pDestDoc)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/02/2005
	Inputs:		pOp - pointer to Op
				pPPStack - pointer to EffectsStack to copy
	Outputs:	
	Returns:	
	Purpose:	Copy the specified LiveEffect stack onto the selection
				Used by Paste Attributes and OpCombineShapes
	Errors:		
	See also:	

********************************************************************************************/

BOOL OpLiveEffect::DoCopyEffectsStack(UndoableOperation* pOp, Node* pDestNode, ListRange* pEffectsStack, Document* pSrcDoc, Document* pDestDoc)
{
	ERROR2IF(pOp==NULL, FALSE, "No Op given to DoCopyEffectsStack\n");
	ERROR2IF(pEffectsStack==NULL || pEffectsStack->IsEmpty(), FALSE, "No stack/empty stack given to DoCopyEffectsStack\n");
	ERROR2IF(pDestNode==NULL, FALSE, "No destination node given to DoCopyEffectsStack\n");

	//-----------------------------------------------------------------------------
	// Remove all current LiveEffects on the selection
	//
	if (pDestNode->IsAnObject() && !pDestNode->NeedsParent(pDestNode->FindParent()))
		pDestNode = DoDeleteAllPostProcessors(pOp, (NodeRenderableInk*)pDestNode, FALSE, TRUE);	// TRUE means delete all effects in stack, even those below pDestNode

	//-----------------------------------------------------------------------------
	// Apply new LiveEffects from the specified stack
	//
	BOOL bWasSelected = pDestNode->IsSelected() || pDestNode->IsParentOfSelected();
	Node* pSelectNode = pDestNode;
	if (pDestNode->IsNodeRenderableClass())
		((NodeRenderable*)pDestNode)->DeSelect(FALSE, TRUE);

	Node* pTopNode = pDestNode;

	NodeEffect* pEffect = (NodeEffect*)pEffectsStack->FindFirst();
	while (pEffect)
	{
		// Test for legacy effect special cases first
		// Shadow -------------------------------------------------------------
		if (pEffect->IsAShadowController())
		{
			if (pTopNode->NeedsParent(pTopNode->FindParent()))
			{
				if (pTopNode->IsAShadow())
				{
					// TODO: Copy shadow properties here?
				}
			}
			else
			{
				NodeShadowController* pController = (NodeShadowController*)pEffect;

				NodeShadowParam param(pController->GetShadowType(),
										pController->GetFloorShadowAngle(),
										pController->GetFloorShadowHeight(),
										pController->GetOffsetX(),
										pController->GetOffsetY(),
										pController->GetPenumbraWidth(),
										pController->GetShadow()->GetDarkness(),
										pController->GetWallShadowScale(),
										pController->GetGlowWidth(),
										pController->GetShadow()->GetBiasGain(),
										TRUE);
				param.strOpUnique = pEffect->GetPostProcessorID();
				param.StackPosition = STACKPOS_INSERT_TOP;
				pTopNode = OpApplyShadow::DoApplyShadow(pOp, pTopNode, &param, FALSE);
	//			BOOL ok = pOp->DoFactorOutCommonChildAttributes(pShadControl);
				if (pTopNode == NULL)
				{
					pOp->FailAndExecute();
					return FALSE;
				}

				// Copy attributes applied to Shadow node
				NodeShadow* pShadow = pController->GetShadow();
				NodeShadow* pNewShadow = NULL;
				if (pTopNode->IsAShadowController())
					pNewShadow = ((NodeShadowController*)pTopNode)->GetShadow();
				ERROR3IF(pShadow==NULL || pNewShadow==NULL, "ShadowController doesn't seem to have a shadow");
				if (pShadow && pNewShadow)
				{
					DoCopyAttributes(pOp, pShadow, pNewShadow, FALSE, pSrcDoc, pDestDoc);
				}

				delete param.pPPStack;
			}
		}
		// Feather -----------------------------------------------------------------
		else if (pEffect->IsFeatherEffect())
		{
			NodeFeatherEffect* pFeather = (NodeFeatherEffect*)pEffect;
			String_64 strDisplayName;
			XPEHost::GetEffectDetails(pEffect->GetPostProcessorID(), &strDisplayName);
			pTopNode = DoApplyFeatherEffectNode(pOp, pTopNode, pEffect->GetPostProcessorID(), strDisplayName, pFeather->GetFeatherSize(), pFeather->GetProfile());
			if (pTopNode == NULL)
			{
				pOp->FailAndExecute();
				return FALSE;
			}
		}
		// LiveEffects -------------------------------------------------------------
		else if (pEffect->IsBitmapEffect())
		{
			if (pEffect->IsLockedEffect() && ((NodeLockedEffect*)pEffect)->CanBeUnlocked())
			{
				String_64 strDisplayName;
				XPEHost::GetEffectDetails(pEffect->GetPostProcessorID(), &strDisplayName);
				pTopNode = DoApplyLiveEffect(pOp, pTopNode, pEffect->GetPostProcessorID(), strDisplayName, ((NodeBitmapEffect*)pEffect)->GetPixelsPerInchValue(), ((NodeBitmapEffect*)pEffect)->GetEditList(), TRUE);	// MakeLocked
				if (pTopNode == NULL)
				{
					pOp->FailAndExecute();
					return FALSE;
				}
			}
			else if (!pEffect->IsLockedEffect())
			{
				String_64 strDisplayName;
				XPEHost::GetEffectDetails(pEffect->GetPostProcessorID(), &strDisplayName);
				pTopNode = DoApplyLiveEffect(pOp, pTopNode, pEffect->GetPostProcessorID(), strDisplayName, ((NodeBitmapEffect*)pEffect)->GetPixelsPerInchValue(), ((NodeBitmapEffect*)pEffect)->GetEditList(), FALSE);
				if (pTopNode == NULL)
				{
					pOp->FailAndExecute();
					return FALSE;
				}
			}
			if (pTopNode->IsEffect() && ((NodeEffect*)pTopNode)->IsLockedEffect())
			{
				// Don't allow anything to be selected inside a Locked effect!
				pSelectNode = pTopNode;
			}
		}

		// Copy effect attributes that might be applied to this level
		if (pTopNode)
		{
			DoCopyAttributes(pOp, pEffect, pTopNode, TRUE, pSrcDoc, pDestDoc);
		}

		// ----------------------------------------------------------------------
		pEffect = (NodeEffect*)pEffectsStack->FindNext(pEffect);
	}

	if (pSelectNode && bWasSelected && pSelectNode->IsNodeRenderableClass())
	{
		((NodeRenderable*)pSelectNode)->Select(FALSE);
	}

	return TRUE;
}




/********************************************************************************************

>	static BOOL OpLiveEffect::DoCopyAttributes(UndoableOperation* pOp, Node* pSrcNode, Node* pDestNode, BOOL bEffectsOnly, Document* pSrcDoc, Document* pDestDoc)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/03/2005
	Inputs:		pOp - POinter to Op to add undo records to
				pSrcNode - Node containing attrs to copy
				pDestNode - Node to copy attrs into
				bEffectsOnly - TRUE if should only copy Effect Attrs
				Doc ptrs to allow component data to be copied
	Outputs:	
	Returns:	
	Purpose:	Copy the specified LiveEffect stack onto the selection
				Used by Paste Attributes
				Assumes that only simple attributes will be copied in simple situations
				I.e. attrs that aren't optimsed so much
	Errors:		
	See also:	

********************************************************************************************/
BOOL OpLiveEffect::DoCopyAttributes(UndoableOperation* pOp, Node* pSrcNode, Node* pDestNode, BOOL bEffectsOnly, Document* pSrcDoc, Document* pDestDoc)
{
	if (!pDestNode->IsBounded() || !pSrcNode->IsBounded())
		return TRUE;

	((NodeRenderableBounded*)pDestNode)->InvalidateBoundingRect();
	DocRect NodeBounds = ((NodeRenderableBounded*)pDestNode)->GetBoundingRect(TRUE);
	Node* pNode = pSrcNode->FindFirstChild();
	while (pNode)
	{
		if (pNode->IsAnAttribute())
		{
			NodeAttribute* pAttr = (NodeAttribute*)pNode;
			if (!bEffectsOnly || pAttr->IsEffectAttribute())
			{
				if (pAttr->CanBeAppliedToObject())
				{
					NodeAttribute* pAttrCopy = NULL;
					pAttr->NodeCopy((Node **)(&pAttrCopy));

					if (pAttrCopy->IsAFillAttr())
					{
						// Set the bounds of the attribute. (Why is this wrong here?)
						((AttrFillGeometry*)pAttrCopy)->SetBoundingRect(((NodeRenderableBounded*)pSrcNode)->GetBoundingRect());

						// I'm not sure if this	is neccessary but Will does it when making an attribute
						// current.
						if (pAttrCopy->IsAFractalFill())
						{
							// The attr, should always use the default DPI for fractals.
							((AttrFillGeometry*)pAttrCopy)->SetFractalDPI(AttrFillGeometry::FractalDPI);
						}
					}

					pAttrCopy->TransformToNewBounds(NodeBounds); 
					pAttrCopy->AttachNode(pDestNode, LASTCHILD);
					pAttrCopy->LinkToGeometry(pDestNode);

					if (pAttrCopy->IsAFillAttr())
					{
						// Now the Attribute is in the tree, we need to tell the fill
						// attribute to check that it's control points are valid.
						// Unless the fill we transformed above, this will usually 
						// involve the fill 'centring' itself within the bounds of its
						// parent.
						((AttrFillGeometry*)pAttrCopy)->AttributeChanged();
					}

					// Inform all DocComponents in the destination doc that a copy is about to take place
					BOOL ok;
					CALL_WITH_FAIL((pDestDoc->StartComponentCopy()), pOp, ok)
	 				if (ok)
					{
						if (!pAttrCopy->CopyComponentData(pSrcDoc, pDestDoc))
						{
							pDestDoc->AbortComponentCopy(); // Cancel all data which has been copied
							ok = FALSE; // stop what were doing
						}

						if (ok)
							ok = pDestDoc->EndComponentCopy();
					}

					// Remove similar nodes in this level
					Node* pCurrent = pDestNode->FindFirstChild();
					Node* pNext;
					CCRuntimeClass* pAttrType = pAttrCopy->GetAttributeType();
					while (pCurrent)
					{
						pNext =  pCurrent->FindNext();
						// Determine if the Current node is to be hidden
						if (pCurrent!=pAttrCopy && pCurrent->IsKindOf(CC_RUNTIME_CLASS(NodeAttribute)))
						{
							{ 
								if (((NodeAttribute*)pCurrent)->GetAttributeType() == pAttrType)
								{
									pCurrent->CascadeDelete();
									delete pCurrent;
								}
							}
						}
						pCurrent = pNext;
					}
				}
			}
		}

		pNode = pNode->FindNext();
	}

	return TRUE;
}




/********************************************************************************************

>	static BOOL OpLiveEffect::DoLocaliseLiveEffects(UndoableOperation* pOp, Node* pNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/2004
	Inputs:		pPPStack - pointer to EffectsStack to copy
	Outputs:	
	Returns:	
	Purpose:	Copy the specified LiveEffect stack onto the selection
				Used by Paste Attributes
	Errors:		
	See also:	

********************************************************************************************/
BOOL OpLiveEffect::DoLocaliseLiveEffects(UndoableOperation* pOp, Node* pNode)
{
	ListRange* pPPStack = EffectsStack::GetEffectsStackFromNode(pNode, FALSE, TRUE, TRUE);
	if (pPPStack==NULL)
		return TRUE;

	NodeRenderableInk* pChild = pNode->FindFirstChildInk();
	while (pChild)
	{
		NodeRenderableInk* pNextChild = pChild->FindNextInk();

		// Don't localise into "controlled" nodes
		if (pChild->GetParentController()==NULL)
		{
			NodeRenderableInk* pInsertContext = pChild;
			NodeBitmapEffect* pLENode = (NodeBitmapEffect*)pPPStack->FindFirst();
			ERROR2IF(!pLENode->CanBeUnlocked(), FALSE, "Can't localise Destructive effects");
			while (pLENode)
			{
				// Make a copy of this LiveEffect
				NodeBitmapEffect* pCopy = NULL;
				BOOL bOK = FALSE;
				CALL_WITH_FAIL(pCopy = (NodeBitmapEffect*)pLENode->SimpleCopy(), pOp, bOK);
				if (!bOK) return FALSE; // No room to take a copy of the node

				// Can't insert directly as parent because of restrictions with HiddenNode
				// So insert as sibling and then move context node into sibling
				bOK = pOp->DoInsertNewNode(pCopy, pInsertContext, NEXT, TRUE, FALSE, FALSE, FALSE);
				if (!bOK) return FALSE;

				bOK = pOp->DoMoveNode(pInsertContext, pCopy, FIRSTCHILD);
				if (!bOK) return FALSE;

				pInsertContext = pCopy;

				// Copy any necessary children of the LiveEffect
				// This could be much more developed, possibly using a virtual function
				// on the node that's just been copied to copy it's necessary parts
				// or ComplexCopy
				// But this works well enough for now (NodeShadowControllers and NodeShadows)
				Node* pNode = pLENode->FindFirstChild();
				AttachNodeDirection dir = FIRSTCHILD;
				while (pNode)
				{
					if (pNode->NeedsParent(pNode->FindParent()) && pNode->IsBounded())
					{
						// Make a copy of this "Needed" node
						NodeRenderableBounded* pCopy = NULL;
						BOOL bOK = FALSE;
						CALL_WITH_FAIL(pNode->NodeCopy((Node**)&pCopy), pOp, bOK);		// Copy node and subtree
						if (!bOK) return FALSE; // No room to take a copy of the node

						bOK = pOp->DoInsertNewNode(pCopy, pInsertContext, dir, TRUE, FALSE, FALSE, FALSE);
						if (!bOK) return FALSE;
					}

					pNode = pNode->FindNext();
				}

				pLENode = (NodeBitmapEffect*)pPPStack->FindNext(pLENode);
			}
		}

		pChild = pNextChild;
	}

	delete pPPStack;

	//-----------------------------------------------------------------------------
	// Remove all current LiveEffects on the selection
	//
	if (pNode->IsAnObject())
		return (OpLiveEffect::DoDeleteAllPostProcessors(pOp, (NodeRenderableInk*)pNode)!=NULL);
	else
		return TRUE;
}




/********************************************************************************************

>	static BOOL OpLiveEffect::DoLocaliseEffectAttrs(UndoableOperation* pOp, Node* pNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/03/2005
	Inputs:		pOp - pointer to op
				pNode - 
	Outputs:	
	Returns:	TRUE if it worked
	Purpose:	Copy the Effect attributes from the specified node down to lower nodes,
				remaining effect attrs where possible but becoming normal attrs if that's
				not possible
				Used by ungroup
	Errors:		
	See also:	

********************************************************************************************/
BOOL OpLiveEffect::DoLocaliseEffectAttrs(UndoableOperation* pOp, Node* pNode)
{
	// From top of stack
	// find effect attr
	// Copy it down to children of specified node
	//   If node can cope with effect attr
	//   Then copy as effect attr
	//	 Else
	//     If object already has that type of attr
	//     Then do nothing
	//	   Else copy attribute into place and delete attrs in subtree

	ListRange* pPPStack = EffectsStack::GetEffectsStackFromNode(pNode, FALSE, TRUE, TRUE);
	if (pPPStack)
	{
		Node* pEffectNode = pPPStack->FindLast();
		while (pEffectNode)
		{
			NodeAttribute* pAttr = NodeAttribute::FindFirstAppliedAttr(pEffectNode);
			while (pAttr && pAttr->IsEffectAttribute() && pAttr->FindParent()==pEffectNode)
			{
				if (!pAttr->HasEquivalentDefaultValue(TRUE))			// Only copy attr if it doesn't look like the default
					DoCopyEffectAttr(pOp, pAttr, pNode);

				pAttr = NodeAttribute::FindPrevAppliedAttr(pAttr);
			}

			pEffectNode = pPPStack->FindPrev(pEffectNode);
		}
	}

	// Finally, do the same thing for the node itself...
	NodeAttribute* pAttr = NodeAttribute::FindFirstAppliedAttr(pNode);
	while (pAttr && pAttr->IsEffectAttribute() && pAttr->FindParent()==pNode)
	{
		if (!pAttr->HasEquivalentDefaultValue(TRUE))					// Only copy attr if it doesn't look like the default
			DoCopyEffectAttr(pOp, pAttr, pNode);

		pAttr = NodeAttribute::FindPrevAppliedAttr(pAttr);
	}

	delete pPPStack;

	return TRUE;
}




/********************************************************************************************

>	static BOOL OpLiveEffect::DoCopyEffectAttr(UndoableOperation* pOp, NodeAttribute* pAttr, Node* pNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/03/2005
	Inputs:		pOp - pointer to op
				pAttr - pointer to effect attribute to copy
				pNode - pointer to node whose children should get copies of the attribute
	Outputs:	
	Returns:	TRUE if it worked
	Purpose:	Copy the Effect attributes from the specified node down to lower nodes,
				remaining effect attrs where possible but becoming normal attrs if that's
				not possible
				Used by ungroup
	Errors:		
	See also:	

********************************************************************************************/
BOOL OpLiveEffect::DoCopyEffectAttr(UndoableOperation* pOp, NodeAttribute* pAttr, Node* pNode)
{
	Node* pChildNode = pNode->FindFirstChild();
	while (pChildNode)
	{
		if (pChildNode->IsAnObject())
		{
			NodeRenderableInk* pInkNode = (NodeRenderableInk*)pChildNode;
			if (pInkNode->IsValidEffectAttr(pAttr))
			{
				NodeAttribute* pExistingAttr = pInkNode->FindAppliedAttribute(pAttr->GetRuntimeClass());
				if (pExistingAttr==NULL || pExistingAttr->FindParent()!=pInkNode)
				{
					// We can copy our effect attr to pInkNode as an effect attr
					// Make a copy of this LiveEffect
					NodeAttribute* pCopyAttr = NULL;
					BOOL bOK = FALSE;
					CALL_WITH_FAIL(pCopyAttr = (NodeAttribute*)pAttr->SimpleCopy(), pOp, bOK);  
					if (!bOK) return FALSE; // No room to take a copy of the node

//					pOp->DoInsertNewNode(pCopyAttr, pInkNode, LASTCHILD, FALSE, FALSE, FALSE, FALSE);

					// Create a hide node action to hide the node when we undo 
					pCopyAttr->AttachNode(pInkNode, LASTCHILD);
					HideNodeAction* UndoHideNodeAction;
					if (HideNodeAction::Init(pOp, pOp->GetUndoActions(), pCopyAttr, TRUE, (Action**)(&UndoHideNodeAction))
					  		== AC_FAIL)
					{
						pCopyAttr->CascadeDelete();
						delete pCopyAttr;
						return FALSE;
					}
				}
			}
			else
			{
				NodeAttribute* pExistingAttr = pInkNode->FindAppliedAttribute(pAttr->GetRuntimeClass());
				if (pExistingAttr==NULL || pExistingAttr->FindParent()!=pInkNode)
				{
					// We can copy our effect attrs to pInkNode as a normal attr
					NodeAttribute* pCopyAttr = NULL;
					BOOL bOK = FALSE;
					CALL_WITH_FAIL(pCopyAttr = (NodeAttribute*)pAttr->SimpleCopy(), pOp, bOK);  
					if (!bOK) return FALSE; // No room to take a copy of the node

//					pOp->DoInsertNewNode(pCopyAttr, pInkNode, FIRSTCHILD, FALSE, FALSE, FALSE, FALSE);

					// Create a hide node action to hide the node when we undo 
					pCopyAttr->AttachNode(pInkNode, FIRSTCHILD);
					HideNodeAction* UndoHideNodeAction;
					if (HideNodeAction::Init(pOp, pOp->GetUndoActions(), pCopyAttr, TRUE, (Action**)(&UndoHideNodeAction))
					  		== AC_FAIL)
					{
						pCopyAttr->CascadeDelete();
						delete pCopyAttr;
						return FALSE;
					}

//					if (UndoHideNodeAction)
//						UndoHideNodeAction->DontPanic();

					// Remove any deeper attributes of the same type to make a consistent structure
					pOp->DoRemoveAttrTypeFromSubtree(pInkNode, pAttr->GetRuntimeClass(), pCopyAttr);
				}
			}
		}

		pChildNode = pChildNode->FindNext();
	}

	return TRUE;
}




/********************************************************************************************

>	virtual Node* OpLiveEffect::DoDeleteAllPostProcessors(UndoableOperation* pOp, NodeRenderableInk* pNode, BOOL bDeleteOldControllers = FALSE, BOOL bDeleteBelowLocked = FALSE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/11/2004
	Inputs:		pOp - Operation to add undo actions to
				pNode - Selected node (note, not parent of selected)
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
Node* OpLiveEffect::DoDeleteAllPostProcessors(UndoableOperation* pOp, NodeRenderableInk* pNode, BOOL bDeleteOldControllers, BOOL bDeleteBelowLocked)
{
	BOOL bOK = TRUE;

	// First, find pointer to child node to be moved into place as the LiveEffects stack is deleted
	Node* pNewBaseNode = pNode;
	if (pNode->IsEffect())
	{
		ENSURE(((NodeEffect*)pNode)->IsLockedEffect(), "Only destructive LiveEffects should be selectable");
		// Find the first ink node that's a child of this LiveEffect
		if (bDeleteBelowLocked)
		{
			// Locked effects can have several children - GetInkNodeFC only returns non-NULL
			// when there is a single node below the locked effect
			if (((NodeEffect*)pNewBaseNode)->GetInkNodeFromController()==NULL)
				pNewBaseNode = pNewBaseNode->FindFirstChild();
			else
			{
				Node* pChild = pNewBaseNode;
				while (pChild && pChild->IsEffect())
				{
					pChild = ((NodeEffect*)pChild)->GetInkNodeFromController();

					if (pChild)
						pNewBaseNode = pChild;
				}
			}
		}
	}

	Node* pHighestLE = NULL;
	Node* pEffectScanBase = NULL;

/*	// [Phil, 22/08/2005] There must be a better way to arrange the following logic!
	//
	// Push common attributes down into the nodes we're going to retain
	Node* pAncestor = pChild->FindParent();
	if (pAncestor && !pAncestor->IsAnObject())
		pAncestor = NULL;
	if (pAncestor)
	{
		bOK = pOp->DoLocaliseCommonAttributes((NodeRenderableInk*)pAncestor, FALSE, TRUE);
		if (!bOK) return FALSE;
	}

	// If there are old style controller nodes applied to the object before any effects
	// Then get rid of them...
	if (bDeleteOldControllers)
	{
		while (pAncestor && pAncestor->IsController() && !pAncestor->IsEffect())
		{
			pHighestLE = pAncestor;
			pAncestor = pAncestor->FindParent();
		}
	}
	else
	{
		// Move the BaseNode pointer above the old controllers that we are not deleting
		while (pAncestor && pAncestor->IsController() && !pAncestor->IsEffect())
		{
			pNewBaseNode = pAncestor;
			pAncestor = pAncestor->FindParent();
		}
	}

	// Delete all LiveEffects from pChild upwards
	while (pAncestor && pAncestor->IsEffect())
	{
		pHighestLE = pAncestor;
		pAncestor = pAncestor->FindParent();
	}
*/
	// ------------------------------------------------------------------
	// Deal with old style controllers
	{
		Node* pAncestor = pNewBaseNode->FindParent();
		Node* pHighestOldController = NULL;
		pEffectScanBase = pNewBaseNode;
		while (pAncestor && pAncestor->IsController() && !pAncestor->IsEffect())
		{
			pHighestOldController = pAncestor;
			pAncestor = pAncestor->FindParent();
		}

		if (pHighestOldController)
		{
			if (bDeleteOldControllers)
				pEffectScanBase = pHighestOldController;	// Leave base node pointer alone and tell scanner to start above the old controllers
			else
			{
				pEffectScanBase = pHighestOldController;	// Tell scanner to start above the old controllers
				pNewBaseNode = pHighestOldController;		// but Reset base node pointer to refer to it so we leave it in the tree
			}
		}
	}

	// ------------------------------------------------------------------
	// Find highest Effect above the base node
	{
		Node* pAncestor = pEffectScanBase->FindParent();
		while (pAncestor && pAncestor->IsEffect())
		{
			pHighestLE = pAncestor;
			pAncestor = pAncestor->FindParent();
		}
	}

	// ------------------------------------------------------------------
	// Push common attributes down into the nodes we're going to retain (the base node)
	{
		Node* pAncestor = pNewBaseNode->FindParent();
		if (pAncestor && pAncestor->IsAnObject())
		{
			bOK = pOp->DoLocaliseCommonAttributes((NodeRenderableInk*)pAncestor, FALSE, TRUE);
			if (!bOK) return NULL;
		}
	}

	// ------------------------------------------------------------------
	// If we have found Effects above the selected node (then we can
	// get rid of them)
	if (pHighestLE)
	{
		// Loop around all the children of the LiveEffect moving them up
		// (but note that only Destructive effects can have multiple children...)
		// (Er, but note again that effect attributes can be left in the tree...)
		Node* pInsertNode = pHighestLE;
		Node* pNextChild = NULL;
		Node* pChild = pNewBaseNode;
		while (pChild)
		{
			pNextChild = pChild->FindNext();								// Find next child before we move it!

			if (pChild->IsAnObject())
			{
				// Only move pChild if it's not linked to the PostProcessor above it!
				// (Otherwise leave it in place so it gets hidden along with the PostPros)
				if (!((NodeRenderableInk*)pChild)->NeedsParent(pChild->FindParent()))	// Controlled?
				{
					bOK = pOp->DoMoveNode(pChild, pInsertNode, NEXT);
					if (!bOK) return NULL;

					((NodeRenderableInk*)pChild)->Select(FALSE);

					if (pChild->IsAnObject())
						pOp->DoFactorOutCommonAttributes((NodeRenderableInk*)pChild);

					pInsertNode = pChild;
				}
			}

			pChild = (NodeRenderableInk*)pNextChild;
		}

		// Now that all the children are moved outside all the LE nodes
		// we can delete all the LE nodes (by hiding the root node and thus
		// hiding the entire stack at one go!)
		if (pHighestLE->IsRenderable())
			((NodeRenderable*)pHighestLE)->DeSelect(FALSE);
		bOK = pOp->DoHideNode(pHighestLE, FALSE, NULL, FALSE);
	}

	return pNewBaseNode;
}




/********************************************************************************************

>	BOOL OpLiveEffect::DoDeletePostProcessor(UndoableOperation* pOp, Range* pRange)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/11/2004
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
BOOL OpLiveEffect::DoDeletePostProcessor(UndoableOperation* pOp, Range* pRange)
{
	BOOL ok = FALSE;

	// We should have a decent list of LiveEffect nodes...
	ERROR2IF(pRange==NULL, FALSE, "DoDeletePostProcessor given an empty range");

	// Go through all the LiveEffects, move their children up to be siblings and then
	// "delete" the LiveEffect nodes by hiding them.
	//
	Node* pNode = pRange->FindFirst();
	while (pNode)
	{
		if (pNode->IsEffect())
		{
			NodeEffect* pLE = (NodeEffect*)pNode;
			DoDeletePostProcessor(pOp, pLE);
		}

		pNode = pRange->FindNext(pNode);
	}

	return TRUE;
}




/********************************************************************************************

>	BOOL OpLiveEffect::DoDeletePostProcessor(UndoableOperation* pOp, NodeEffect* pLE, BOOL bMoveEffectAttrs = FALSE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/2005
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
BOOL OpLiveEffect::DoDeletePostProcessor(UndoableOperation* pOp, NodeEffect* pLE, BOOL bMoveEffectAttrs)
{
	BOOL ok = FALSE;

	// We should have a decent list of LiveEffect nodes...
	ERROR2IF(pLE==NULL, FALSE, "DoDeletePostProcessor given NULL node");

	// Go through all the LiveEffects, move their children up to be siblings and then
	// "delete" the LiveEffect nodes by hiding them.
	//
	BOOL bSelected = pLE->IsSelected();

	pOp->DoLocaliseCommonAttributes(pLE);

	// Loop around all the children of the LiveEffect moving them up
	// (but note that only Destructive effects can have multiple children...)
	// (Er, but note again that effect attributes can be left in the tree...)
	Node* pInsertNode = pLE;
	Node* pNextChild = NULL;
	Node* pParent = pLE->FindParent();
	NodeEffect* pParentEffect = NULL;
	if (pParent && pParent->IsEffect())
		pParentEffect = (NodeEffect*)pLE->FindParent();
	NodeRenderableInk* pChild = (NodeRenderableInk*)pLE->FindFirstChild(CC_RUNTIME_CLASS(NodeRenderableInk));

	while (pChild)
	{
		pNextChild = pChild->FindNext();							// Find next child before we move it!

		if (pChild->IsAnObject())
		{
			// Only move pChild if it's not linked to the PostProcessor above it!
			// (Otherwise leave it in place so it gets hidden along with the PostPros)
			if (!((NodeRenderableInk*)pChild)->NeedsParent(pChild->FindParent()))	// Controlled?
			{
				pOp->DoMoveNode(pChild, pInsertNode, NEXT);
				if (bSelected)
					pChild->Select(FALSE);
				if (pChild->IsAnObject())
					pOp->DoFactorOutCommonAttributes((NodeRenderableInk*)pChild);

				pInsertNode = pChild;
			}
		}

		if (bMoveEffectAttrs && pParentEffect && pChild->IsAnAttribute() && ((NodeAttribute*)pChild)->IsEffectAttribute())
		{
			if (pParentEffect->IsValidEffectAttr((NodeAttribute*)pChild))
			{
				pOp->DoMoveNode(pChild, pInsertNode, NEXT);
				pInsertNode = pChild;
			}
		}

		pChild = (NodeRenderableInk*)pNextChild;
	}

	pLE->ReleaseCached(TRUE, FALSE, FALSE, FALSE);
	if (bSelected)
		pLE->DeSelect(FALSE);
	pOp->DoHideNode(pLE, FALSE, NULL, FALSE);

	return TRUE;
}




/********************************************************************************************

>	BOOL OpLiveEffect::DoEffectRangeOp(UINT32 idsProgress, Range* pRange, OpLiveEffectParam* pParam)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/2005
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
BOOL OpLiveEffect::DoEffectRangeOp(UINT32 idsProgress, Range* pRange, OpLiveEffectParam* pParam)
{
	BOOL ok = FALSE;
	Progress progress(idsProgress, pRange->Count(), FALSE);

	// We should have a decent list of LiveEffect nodes...
	ERROR2IF(pRange==NULL, FALSE, "DoEffectRangeOp given an empty range");

	// Go through all the LiveEffects, move their children up to be siblings and then
	// "delete" the LiveEffect nodes by hiding them.
	//
	INT32 iCount = 0;
	INT32 iChanges = 0;
	Node* pNode = pRange->FindFirst();
	while (pNode && progress.Update(iCount, TRUE) && !OpHasFailed())
	{
		if (pNode->IsEffect())
		{
			// --------------------------------------------
			// This is the core of this function,
			// Call overridden specialist function to perform
			// the Op on this node alone
			NodeEffect* pLE = (NodeEffect*)pNode;
			NodeBitmapEffect* pNewEffect = NULL;
			BOOL bChanged = DoEffectNodeOp(pLE, pParam, &pNewEffect);
			// --------------------------------------------

			if (bChanged)
			{
				iChanges++;

				// Invalidate the region we just changed
				// (Rely on DoEffectNodeop to have released the cache if it thinks it needs to
				if (!DoInvalidateNodeRegion(pNewEffect, TRUE, FALSE, FALSE, FALSE))
				{
					FailAndExecute();
					End();
					return FALSE;
				}

				// Update the view inside our progress indication so the user gets
				// better idea of where we are...
				DocView* pDocView = GetWorkingDocView();
				if (pDocView)
				{
					// Update just the working view immediately
					pDocView->FlushRedraw();
					GetApplication()->ServiceRendering();
				}
			}
		}

		iCount++;
		pNode = pRange->FindNext(pNode);
	}

	// If we aborted with nodes remaining to be done
	// Then abort the whole Operation!
	if (pNode)
	{
		FailAndExecute();
		End();
		return FALSE;
	}

ObjChangeFlags cFlags(NULL, TRUE, NULL, NULL, NULL, NULL, NULL, TRUE);
ObjChangeParam ObjChange(OBJCHANGE_FINISHED, cFlags, NULL, this);
ok = UpdateChangedNodes(&ObjChange);

	// If we didn't change anything then don't add this op to the undo history
	if (iChanges==0)
	{
		SucceedAndDiscard();
		End();
		return FALSE;
	}

	End();
	return FALSE;			// We don't need any of the normal tidy up to occur
}




/********************************************************************************************

>	virtual void OpLiveEffect::Do(OpDescriptor* pOpDesc)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/2005
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
void OpLiveEffect::Do(OpDescriptor* pOpDesc)
{
//	ERROR3("Why are you doing this? You need params, boy, params!");
	DoWithParam(pOpDesc, NULL);
}




/********************************************************************************************

>	virtual void OpLiveEffect::DoWithParam(OpDescriptor* pOpDesc, OpParam* pOpParam)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/2005
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
void OpLiveEffect::DoWithParam(OpDescriptor* pOpDesc, OpParam* pOpParam)
{
	BOOL ok = FALSE;

	OpLiveEffectParam* pLEOpParam = NULL;
	if (pOpParam && pOpParam->IsKindOf(CC_RUNTIME_CLASS(OpLiveEffectParam)))
		pLEOpParam = (OpLiveEffectParam*) pOpParam;

	if (!CheckEffectOpParam(pLEOpParam))
	{
		End();
		return;
	}

	// Remember some details for later on when the Op ends successfully (?)
	if (pLEOpParam)
	{
		m_strEffectID = pLEOpParam->strOpUnique;
		m_iStackPos = pLEOpParam->StackPosition;
	}

	// obtain the current selection.
	Range Sel(*(GetApplication()->FindSelection()));
	RangeControl rc = Sel.GetRangeControlFlags();
	rc.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rc);

	if (!AllowEmptySelection() && Sel.FindFirst()==NULL)
	{
		End();
		return;
	}

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

// Ask the range whether it's OK to do the transform...
// Get an ObjChangeParam ready, so we can ask op permission from nodes.
ObjChangeFlags cFlags(NULL, TRUE, NULL, NULL, NULL, NULL, NULL, TRUE);	// ReplaceNode & RegenerateNode
ObjChangeParam ObjChange(OBJCHANGE_STARTING, cFlags, NULL, this);
BOOL bOK = Sel.AllowOp(&ObjChange);
if (!bOK)
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

	// Get a list of the LiveEffects we will be working on
	ListRange* pLERange = NULL;
	if (pLEOpParam && pLEOpParam->pPPStack)
	{
		pLERange = pLEOpParam->pPPStack->GetLevelRange(&pLEOpParam->StackPosition);	// We don't own the returned range
		// Make a local copy in case the original dies during selection processing
		if (pLERange)
			pLERange = new ListRange(pLERange);
	}
	m_bAppliedNewEffects = FALSE;

	// -------------------------------------------------------------------
	// Call virtual function of derived Ops to do the specialised function
	BOOL bEndNormally = DoEffectOp(pLERange, pLEOpParam);
	// -------------------------------------------------------------------

	if (pLERange)
	{
		delete pLERange;
		pLERange = NULL;
	}

	if (bEndNormally)
	{
		// invalidate the (new) region bounding the selection.
		if (!DoInvalidateNodesRegions(*(GetApplication()->FindSelection()), TRUE, FALSE, FALSE, FALSE))
		{
			End();
			return;
		}

		// render blobs back on if the current tool doesn't automatically redraw its blobs.
		if (pSpread != NULL && pTool != NULL && !pTool->AreToolBlobsRenderedOnSelection())
			pTool->RenderToolBlobs(pSpread, NULL);

ObjChangeFlags cFlags(NULL, TRUE, NULL, NULL, NULL, NULL, NULL, TRUE);
ObjChangeParam ObjChange(OBJCHANGE_FINISHED, cFlags, NULL, this);
BOOL ok = UpdateChangedNodes(&ObjChange);

		BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::EFFECTSTACKCHANGED)); 

		// Tell the selrange that things have changed!
		GetApplication()->FindSelection()->Update();

		End();
	}
}




/********************************************************************************************

>	virtual BOOL OpLiveEffect::CheckEffectOpParam(OpLiveEffectParam* pLEOpParam)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/2005
	Inputs:		pLEopParam - pointer to parameter object to check
	Outputs:	-
	Returns:	TRUE if Op can continue
				FALSE otherwise
	Purpose:	

********************************************************************************************/
BOOL OpLiveEffect::CheckEffectOpParam(OpLiveEffectParam* pLEOpParam)
{
	if (pLEOpParam->StackPosition==-1)
	{
		ERROR3("OpLiveEffect must be given a valid position in the LiveEffects Stack");
		return FALSE;
	}

//	ERROR3IF(pLEOpParam->pPPStack==NULL, "Can't find EffectsStack");
	if (pLEOpParam->pPPStack==NULL)
		return FALSE;

	return TRUE;
}

	
	
	
///////////////////////////////////////////////////////////////////////////////////////////////
// The OpApplyLiveEffect class														    	 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	static OpState OpApplyLiveEffect::GetParamState(String_256* pstrDescription, OpDescriptor* pOpDesc, OpParam* pOpParam)

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
OpState OpApplyLiveEffect::GetParamState(String_256* pstrDescription, OpDescriptor* pOpDesc, OpParam* pOpParam)
{
	// default is an unticked, *GREYED*, on-menu state.
	OpState OpSt;
	OpSt.Greyed = FALSE;

	if (XPEHost::IsEditSessionRunning())
	{
		OpSt.Greyed = TRUE;
		*pstrDescription = String_256(_R(IDS_LE_EDIT_RUNNING));
	}
	else if (GetApplication()->FindSelection()->FindFirst()==NULL)
	{
		OpSt.Greyed = TRUE;
		*pstrDescription = String_256(_R(IDS_LE_NOTHING_SELECTED));
	}

	return OpSt;
}



/********************************************************************************************

>	virtual void OpApplyLiveEffect::GetOpName(String_256* pstrOpName)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
void OpApplyLiveEffect::GetOpName(String_256* pstrOpName)
{
	*pstrOpName = String_256(_R(IDS_LE_OPAPPLY));
}



/********************************************************************************************

>	virtual void OpApplyLiveEffect::DoWithParam(OpDescriptor* pOpDesc, OpParam* pOpParam)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/09/2004
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	Insert a new LiveEffect attribute in the stack above the selected object(s)
	Errors:		
	See also:	

********************************************************************************************/
void OpApplyLiveEffect::DoWithParam(OpDescriptor* pOpDesc, OpParam* pOpParam)
{
	BOOL ok = FALSE;

	OpLiveEffectParam* pLEOpParam = NULL;
	if (pOpParam->IsKindOf(CC_RUNTIME_CLASS(OpLiveEffectParam)))
		pLEOpParam = (OpLiveEffectParam*) pOpParam;

	INT32 InsertPos = pLEOpParam->StackPosition;		// We want to find the surface below the current pos
	if (pLEOpParam->StackPosition<STACKPOS_TOP)
		InsertPos = InsertPos - 1;					// We want to find the surface below the current pos
													// because we are going to insert above that surface

	// Remember some details for later on when the Op ends successfully
	m_strEffectID = pLEOpParam->strOpUnique;
	m_iStackPos = InsertPos;
	if (m_strEffectID.IsEmpty())
	{
		End();
		return;
	}

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

	// We need to allow modeless editors to open even when there is no selection so that
	// they will remain open while the user changes the selection
	// So we must avoid calling AllowOp in that case because it returns bOK = FALSE for empty selections
	if (!(pLEOpParam->bReUseEditor && pSelRange->IsEmpty()))
	{
		// Ask the range whether it's OK to do the transform...
		// Get an ObjChangeParam ready, so we can ask op permission from nodes.
		ObjChangeFlags cFlags(NULL, TRUE, NULL, NULL, NULL, NULL, NULL, TRUE);	// ReplaceNode & RegenerateNode
		ObjChangeParam ObjChange(OBJCHANGE_STARTING, cFlags, NULL, this);
		BOOL bOK = pSelRange->AllowOp(&ObjChange);
		if (!bOK)
		{
			End();
			return;
		}
	}

	// invalidate the region bounding the selection.
	if (!DoInvalidateNodesRegions(*pSelRange, TRUE, FALSE, FALSE, FALSE))
	{
		End();
		return;
	}

	// Force text subselection up to the whole parent text object
	pSelRange->MakePartialSelectionWhole(FALSE, FALSE, TRUE);

	// Get a list of the LiveEffects we will be working on
	ENSURE(m_pLERange==NULL, "!");
	if (InsertPos==-1)
	{
		// We are to apply the LiveEFfects to the selection
		pInitialRange = GetApplication()->FindSelection();
		m_iStackPos = 0;
	}
	else
	{
		ENSURE(pLEOpParam->pPPStack, "Can't find EffectsStack");
		pInitialRange = pLEOpParam->pPPStack->GetLevelRange(&InsertPos);
		m_iStackPos = InsertPos+1;			// Remember some details for later on when the Op ends successfully
											// (New stack pos refers to item to be shown in UI after successful insertion)
	}

	// Check whether we can edit an existing effect or whether we have to apply a new one
	if (pInitialRange==NULL)
	{
		ERROR3("If this happens, the UI logic disagrees with the Op about what the Op can do");
		FailAndExecute();
		End();
		return;
	}

	// Need to add a new LiveEffects node to the selection
	// Decide whether we need to create a group or can apply to
	// each selected object individually
	if (pLEOpParam && !pLEOpParam->bIsDestructive)
	{
		if (NodeBitmapEffect::DefaultLocked)
			m_pLERange = DoApplyLiveEffect(pInitialRange, pLEOpParam->strOpUnique, pLEOpParam->strMenuText, NodeBitmapEffect::DefaultLockedPixelsPerInch, pLEOpParam->pEditsList, TRUE, TRUE);
		else
			m_pLERange = DoApplyLiveEffect(pInitialRange, pLEOpParam->strOpUnique, pLEOpParam->strMenuText, NodeBitmapEffect::DefaultLivePixelsPerInch, pLEOpParam->pEditsList, FALSE, TRUE);
	}
	else
	{
		// Destructive effects can only be applied on the top of the stack
		// because of the grouping that they do
		if (!IsTopOfStack(pLEOpParam->pPPStack, pLEOpParam->StackPosition))						// InsertPos?
		{
			InformWarning(_R(IDS_LE_DESTRUCTIVE_APPLY_TOP), _R(IDS_CONTINUE));
			FailAndExecute();
			End();
			return;
		}
		else
		{
			m_pLERange = DoApplyLockedEffect(pInitialRange, pLEOpParam->strOpUnique, pLEOpParam->strMenuText, NodeBitmapEffect::DefaultLockedPixelsPerInch, NULL, TRUE);
		}
	}
	m_bAppliedNewEffects = (m_pLERange!=NULL);

	// We should now have a decent list of LiveEffect nodes...
	if (pLEOpParam->pEditsList==NULL)
	{
		// Mark all LiveEffects as untouched at this stage for change detection later on...
		SetChanged(FALSE);

		// Make sure we've got all the bitmaps we need to proceed
		ok = EnsureLiveEffectOriginalBitmaps();
		if (!ok)
		{
			FailAndExecute();
			End();
			return;
		}
		
		// OK, LiveEffects nodes are in the tree and have the original bitmaps that we need
		//
		// Now decide which bitmap to pass to XPE
		NodeBitmapEffect* pLE = NULL;
		if (m_pLERange)
		{
			Node* pNode = m_pLERange->FindLast();
			while (pNode && !(pNode->IsBitmapEffect() && ((NodeBitmapEffect*)pNode)->CanGenerateBitmap()))
				pNode = m_pLERange->FindPrev(pNode);

			if (pNode==NULL)
			{
				FailAndExecute();
				End();
				return;
			}

			pLE = (NodeBitmapEffect*)pNode;
		}

		// ----------------------------------------------
		// This is the important bit!
		//
		// Pass the sample bitmap to the XPE for editing
		// REMEMBER! This is only starting an edit - the results won't be known until later
		//
		ok = XPEHost::EditLiveEffect(this, pLE, pLEOpParam->strOpUnique, pLEOpParam->bReUseEditor);
		if (!ok)
		{
			FailAndExecute();
			End();
			return;
		}

		GetApplication()->FindSelection()->Update();

		// DON'T end the operation!
		// It is INT32-running (wait for someone to call DoEndEdit...)
	}
	else
	{
		// invalidate the (new) region bounding the selection.
		DoInvalidateNodesRegions(*pSelRange, TRUE, FALSE, FALSE);

		// render blobs back on if the current tool doesn't automatically redraw its blobs.
		if (pSpread != NULL && pTool != NULL && !pTool->AreToolBlobsRenderedOnSelection())
			pTool->RenderToolBlobs(pSpread, NULL);

ObjChangeFlags cFlags(NULL, TRUE, NULL, NULL, NULL, NULL, NULL, TRUE);
ObjChangeParam ObjChange(OBJCHANGE_FINISHED, cFlags, NULL, this);
BOOL ok = UpdateChangedNodes(&ObjChange);

		BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::EFFECTSTACKCHANGED)); 

		// Tell the selrange that things have changed!
		GetApplication()->FindSelection()->Update();

		End();
	}
}



///////////////////////////////////////////////////////////////////////////////////////////////
// The OpEditLiveEffect class														    	 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	static OpState OpEditLiveEffect::GetParamState(String_256* pstrDescription, OpDescriptor* pOpDesc, OpParam* pOpParam)

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
OpState OpEditLiveEffect::GetParamState(String_256* pstrDescription, OpDescriptor* pOpDesc, OpParam* pOpParam)
{
	// default is an unticked, *GREYED*, on-menu state.
	OpState OpSt;
	OpSt.Greyed = FALSE;

/*	if (XPEHost::IsEditSessionRunning())
	{
		OpSt.Greyed = TRUE;
		*pstrDescription = String_256(_R(IDS_LE_EDIT_RUNNING));
	}
	else*/ if (GetApplication()->FindSelection()->FindFirst()==NULL)
	{
		OpSt.Greyed = TRUE;
		*pstrDescription = String_256(_R(IDS_LE_NOTHING_SELECTED));
	}

	return OpSt;
}



/********************************************************************************************

>	static OpState OpEditLiveEffect::GetState(String_256* pstrDescription, OpDescriptor* pOpDesc)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/09/2004
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
/*OpState OpEditLiveEffect::GetState(String_256* pstrDescription, OpDescriptor* pOpDesc)
{
	// default is an unticked, *GREYED*, on-menu state.
	OpState OpSt;
	OpSt.Greyed = FALSE;
	*pstrDescription = String_256(_R(IDS_DESCRIBE_LIVEEFFECT));

	if (XPEHost::IsEditSessionRunning())
	{
		OpSt.Greyed = TRUE;
		*pstrDescription = String_256(_R(IDS_LE_EDIT_RUNNING));
	}

	return OpSt;
}*/



/********************************************************************************************

>	virtual void OpEditLiveEffect::GetOpName(String_256* pstrOpName)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/09/2004
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
void OpEditLiveEffect::GetOpName(String_256* pstrOpName)
{
	*pstrOpName = String_256(_R(IDS_LE_OPEDIT));
}




/********************************************************************************************

>	virtual BOOL OpEditLiveEffect::DoEffectOp(ListRange* pLevelRange, OpLiveEffectParam* pLEOpParam)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/09/2004
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
BOOL OpEditLiveEffect::DoEffectOp(ListRange* pLevelRange, OpLiveEffectParam* pLEOpParam)
{
	m_pLERange = new ListRange(pLevelRange);	// Grab ownership of returned list by copying it
	BOOL ok = FALSE;

	// Find out whether the user is trying to edit a destructive effect...
	{
		Node* pNode = m_pLERange->FindFirst();
		INT32 count=0;
		BOOL bDestructive = FALSE;
		BOOL bLocked = FALSE;
		while (pNode)
		{
			if (pNode->IsBitmapEffect() && ((NodeBitmapEffect*)pNode)->IsLockedEffect())
			{
				bLocked = TRUE;
				if (!((NodeLockedEffect*)pNode)->CanBeUnlocked())
					bDestructive = TRUE;
			}

			count++;
			pNode = m_pLERange->FindNext(pNode);
		}
		if (bDestructive)
		{
			ENSURE(count==1, "Shouldn't allow user to edit more than one destructive effects at a time - greying should prevent this");

			INT32 butt = AskQuestion(_R(IDS_LE_DESTRUCTIVE_EDIT), _R(IDS_LE_RECREATEEFFECT), _R(IDS_CANCEL));
			if (butt==2)
			{
				FailAndExecute();
				End();
				return FALSE;
			}

			// We need to abandon this edit, delete the destructive effects and go re-apply new ones
			// by calling OpApplyLiveEffect
			//
			// Record list of nodes under this effect list before we delete it
			ListRange* pRangeUnderDeletedEffect = new ListRange();
			if (pRangeUnderDeletedEffect==NULL)
			{
				FailAndExecute();
				End();
				return FALSE;
			}
			Node* pNode = m_pLERange->FindFirst();
			while (pNode)
			{
				if (pNode->IsBitmapEffect())
				{
					Node* pChild = ((NodeBitmapEffect*)pNode)->GetInkNodeFromController();
					if (pChild) pRangeUnderDeletedEffect->AddNode(pChild);
				}
				else
					ERROR3("Something's gone horribly wrong - non bitmap effect foudn in list of locked effects to be deleted\n");

				pNode = m_pLERange->FindNext(pNode);
			}
			if (pRangeUnderDeletedEffect->IsEmpty())
			{
				ERROR3("List of nodes under locked effect unexpectedly empty\n");
				FailAndExecute();
				End();
				return FALSE;
			}

			// Get rid of this edit list before calling XPE
			ok = OpLiveEffect::DoDeletePostProcessor(this, m_pLERange);
			if (ok)
			{
				// OK, now insret new effects in place fo the ones we just deleted
				// (Leave m_iStackPos alone because it must, by definition, remain the same)
				delete m_pLERange;
				m_pLERange = DoApplyLockedEffect(pRangeUnderDeletedEffect, pLEOpParam->strOpUnique, pLEOpParam->strMenuText, NodeBitmapEffect::DefaultLockedPixelsPerInch);
				m_bAppliedNewEffects = (m_pLERange!=NULL);
			}
			else
			{
				FailAndExecute();
				End();
				return FALSE;
			}
			if (pRangeUnderDeletedEffect)
			{
				delete pRangeUnderDeletedEffect;
				pRangeUnderDeletedEffect = NULL;
			}
		}
		else
		{
			// Record the current state of this edit list before calling XPE
			// Mark this point in the LiveEffect's life ("on my mark") so that it can be undone
			// back to this point
			ActionCode ac = MarkEditListAction::DoMarkEditList(this, &UndoActions, pLEOpParam->pPPStack, pLEOpParam->StackPosition);
		}
	}

	// Mark all LiveEffects as untouched at this stage for change detection later on...
	SetChanged(FALSE);

	// Make sure we've got all the bitmaps we need to proceed
	ok = EnsureLiveEffectOriginalBitmaps();
	if (!ok)
	{
		FailAndExecute();
		End();
		return FALSE;
	}
	
	// OK, LiveEffects nodes are in the tree and have the original bitmaps that we need
	//
	// Now decide which bitmap to pass to XPE
	NodeBitmapEffect* pLE = NULL;
	if (m_pLERange)
	{
		Node* pNode = m_pLERange->FindLast();
		while (pNode && !(pNode->IsBitmapEffect() && ((NodeBitmapEffect*)pNode)->CanGenerateBitmap()))
			pNode = m_pLERange->FindPrev(pNode);

		if (pNode==NULL)
		{
			FailAndExecute();
			End();
			return FALSE;
		}

		pLE = (NodeBitmapEffect*)pNode;
	}

	// ----------------------------------------------
	// This is the important bit!
	//
	// Pass the sample bitmap to the XPE for editing
	// REMEMBER! This is only starting an edit - the results won't be known until later
	//
	ok = XPEHost::EditLiveEffect(this, pLE, pLEOpParam->strOpUnique, pLEOpParam->bReUseEditor);
	if (!ok)
	{
		FailAndExecute();
		End();
		return FALSE;
	}

	GetApplication()->FindSelection()->Update();

	// DON'T end the operation!
	// Someone will call DoEndEdit
	return FALSE;
}




///////////////////////////////////////////////////////////////////////////////////////////////
// The OpEditLegacyEffect class														    	 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	static OpState OpEditLegacyEffect::GetParamState(String_256* pstrDescription, OpDescriptor* pOpDesc, OpParam* pOpParam)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/08/2005
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	-
	Errors:		
	See also:	

********************************************************************************************/
OpState OpEditLegacyEffect::GetParamState(String_256* pstrDescription, OpDescriptor* pOpDesc, OpParam* pOpParam)
{
	// default is an unticked, *GREYED*, on-menu state.
	OpState OpSt;
	OpSt.Greyed = FALSE;

	if (XPEHost::IsEditSessionRunning())
	{
		OpSt.Greyed = TRUE;
		*pstrDescription = String_256(_R(IDS_LE_EDIT_RUNNING));
	}
	else if (GetApplication()->FindSelection()->FindFirst()==NULL)
	{
		OpSt.Greyed = TRUE;
		*pstrDescription = String_256(_R(IDS_LE_NOTHING_SELECTED));
	}

	return OpSt;
}



/********************************************************************************************

>	virtual void OpEditLegacyEffect::GetOpName(String_256* pstrOpName)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/08/2005
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
void OpEditLegacyEffect::GetOpName(String_256* pstrOpName)
{
	*pstrOpName = String_256(_R(IDS_LE_OPEDIT));
}




/********************************************************************************************

>	virtual void OpEditLegacyEffect::Do(OpDescriptor* pOpDesc)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/08/2005
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
void OpEditLegacyEffect::Do(OpDescriptor* pOpDesc)
{
//	ERROR3("Why are you doing this? You need params, boy, params!");
	DoWithParam(pOpDesc, NULL);
}




/********************************************************************************************

>	virtual void OpEditLegacyEffect::DoWithParam(OpDescriptor* pOpDesc, OpParam* pOpParam)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/08/2005
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
void OpEditLegacyEffect::DoWithParam(OpDescriptor* pOpDesc, OpParam* pOpParam)
{
	BOOL ok = FALSE;

	OpLiveEffectParam* pLEOpParam = NULL;
	if (pOpParam && pOpParam->IsKindOf(CC_RUNTIME_CLASS(OpLiveEffectParam)))
		pLEOpParam = (OpLiveEffectParam*) pOpParam;

	// Remember some details for later on when the Op ends successfully (?)
	if (pLEOpParam)
	{
		if (pLEOpParam->strOpUnique==String_256(POSTPRO_ID_SHADOW))
		{
			// Make sure the Shadow tool is current... (this func can be called from other tools)
			Tool* pShadowTool = Tool::FindTool(TOOLID_SOFTSHADOW);
			if (pShadowTool)
			{
				INT32 iStackPos = pLEOpParam->StackPosition;
				ListRange* pShadowRange = pLEOpParam->pPPStack->GetLevelRange(&iStackPos);
				if (pShadowRange)
				{
					((SoftShadowTool*)pShadowTool)->SelectShadowRange(pShadowRange);
				}

				pShadowTool->SetCurrent();
				End();
				return;
			}
		}

		if (pLEOpParam->strOpUnique==String_256(POSTPRO_ID_FEATHER))
		{
			INT32 iStackPos = pLEOpParam->StackPosition;
			ListRange* pFeatherRange = pLEOpParam->pPPStack->GetLevelRange(&iStackPos);
			OpChangeFeatherSize::SetEditContext(iStackPos, pFeatherRange);
			End();
			return;
		}
	}

	End();
}




///////////////////////////////////////////////////////////////////////////////////////////////
// The OpDeleteLiveEffect class														    	 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	static OpState OpDeleteLiveEffect::GetState(String_256* pstrDescription, OpDescriptor* pOpDesc)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/09/2004
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
OpState OpDeleteLiveEffect::GetState(String_256* pstrDescription, OpDescriptor* pOpDesc)
{
	// default is an unticked, *GREYED*, on-menu state.
	OpState OpSt;
	OpSt.Greyed = FALSE;
	*pstrDescription = String_256(_R(IDS_DESCRIBE_LIVEEFFECT));

	if (XPEHost::IsEditSessionRunning())
	{
		OpSt.Greyed = TRUE;
		*pstrDescription = String_256(_R(IDS_LE_EDIT_RUNNING));
	}

	return OpSt;
}



/********************************************************************************************

>	virtual void OpDeleteLiveEffect::GetOpName(String_256* pstrOpName)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/09/2004
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
void OpDeleteLiveEffect::GetOpName(String_256* pstrOpName)
{
	*pstrOpName = String_256(_R(IDS_LE_OPDELETE));
}




/********************************************************************************************

>	virtual BOOL OpDeleteLiveEffect::DoEffectOp(ListRange* pLevelRange, OpLiveEffectParam* pLEOpParam)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/09/2004
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
BOOL OpDeleteLiveEffect::DoEffectOp(ListRange* pLevelRange, OpLiveEffectParam* pLEOpParam)
{
	OpLiveEffect::DoDeletePostProcessor(this, pLevelRange);

	return TRUE;
}




///////////////////////////////////////////////////////////////////////////////////////////////
// The OpDeleteAllLiveEffect class														    	 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	static OpState OpDeleteAllLiveEffect::GetState(String_256* pstrDescription, OpDescriptor* pOpDesc)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/09/2004
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
OpState OpDeleteAllLiveEffect::GetState(String_256* pstrDescription, OpDescriptor* pOpDesc)
{
	// default is an unticked, *GREYED*, on-menu state.
	OpState OpSt;
	OpSt.Greyed = FALSE;
//	*pstrDescription = String_256(_R(IDS_DESCRIBE_LIVEEFFECT));

	if (XPEHost::IsEditSessionRunning())
	{
		OpSt.Greyed = TRUE;
		*pstrDescription = String_256(_R(IDS_LE_EDIT_RUNNING));
	}

	return OpSt;
}



/********************************************************************************************

>	virtual void OpDeleteAllLiveEffect::GetOpName(String_256* pstrOpName)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/09/2004
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
void OpDeleteAllLiveEffect::GetOpName(String_256* pstrOpName)
{
	*pstrOpName = String_256(_R(IDS_LE_OPDELETE));
}




/********************************************************************************************

>	virtual BOOL OpDeleteAllLiveEffect::DoEffectOp(ListRange* pLevelRange, OpLiveEffectParam* pLEOpParam)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/09/2004
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
BOOL OpDeleteAllLiveEffect::DoEffectOp(ListRange* pLevelRange, OpLiveEffectParam* pLEOpParam)
{
	Range* pSelRange = GetApplication()->FindSelection();
	ENSURE(pSelRange, "OpDeleteAllLiveEffect can't find selection\n");

	// ----------------------------------------------
	// This is the important bit!
	//
	// Go through all the LiveEffects, move their children up to be siblings and then
	// "delete" the LiveEffect nodes by hiding them.
	//
	Node* pNode = pSelRange->FindFirst();
	while (pNode)
	{
		// First, find pointer to child list to be moved into place as the LiveEffects stack is deleted
		Node* pNextSel = pSelRange->FindNext(pNode);

		if (pNode->IsAnObject())
			DoDeleteAllPostProcessors(this, (NodeRenderableInk*)pNode, FALSE, TRUE);

		pNode = pNextSel;
	}

	return TRUE;
}




BOOL OpDeleteAllLiveEffect::CheckEffectOpParam(OpLiveEffectParam* pLEOpParam)
{
//	if (!pLEOpParam->bSilent)
//	{
//		INT32 button = InformWarning(_R(IDS_LE_DELETEALL_WARNING), _R(IDS_REMOVE), _R(IDS_CANCEL));
//		if (button==2)	// Cancel
//			return FALSE;
//	}
//
	// Don't check for valid effects stack - this op doesn't need it.

	return TRUE;
}




///////////////////////////////////////////////////////////////////////////////////////////////
// The OpEffectRes class															    	 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	static OpState OpEffectRes::GetState(String_256* pstrDescription, OpDescriptor* pOpDesc)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/2005
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
OpState OpEffectRes::GetState(String_256* pstrDescription, OpDescriptor* pOpDesc)
{
	// default is an unticked, *GREYED*, on-menu state.
	OpState OpSt;
	OpSt.Greyed = FALSE;
	*pstrDescription = String_256(_R(IDS_DESCRIBE_LIVEEFFECT));

	if (XPEHost::IsEditSessionRunning())
	{
		OpSt.Greyed = TRUE;
		*pstrDescription = String_256(_R(IDS_LE_EDIT_RUNNING));
	}

	return OpSt;
}



/********************************************************************************************

>	virtual void OpEffectRes::GetOpName(String_256* pstrOpName)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/2005
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
void OpEffectRes::GetOpName(String_256* pstrOpName)
{
	*pstrOpName = String_256(_R(IDS_LE_OPEFFECTRES));
}




/********************************************************************************************

>	virtual BOOL OpEffectRes::DoEffectOp(ListRange* pLevelRange, OpLiveEffectParam* pLEOpParam)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/2005
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
BOOL OpEffectRes::DoEffectOp(ListRange* pLevelRange, OpLiveEffectParam* pLEOpParam)
{
	BOOL bOK = FALSE;

	bOK = DoEffectRangeOp(_R(IDS_LE_CHANGE_RES_PROGRESS), pLevelRange, pLEOpParam);

	return bOK;
}




/********************************************************************************************

>	BOOL OpEffectRes::DoEffectNodeOp(NodeEffect* pLE, OpLiveEffectParam* pParam, NodeBitmapEffect** ppNewEffect)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/2005
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
BOOL OpEffectRes::DoEffectNodeOp(NodeEffect* pLE, OpLiveEffectParam* pParam, NodeBitmapEffect** ppNewEffect)
{
	// We should have a decent list of LiveEffect nodes...
	ERROR2IF(pLE==NULL, FALSE, "OpEffectRes::DoEffectNodeOp given NULL node");
	ERROR2IF(pParam==NULL, FALSE, "OpEffectRes::DoEffectNodeOp given NULL param");

	if (pLE->IsLockedEffect())
	{
		// Only change the res of scriptable effects
		if (((NodeLockedEffect*)pLE)->CanBeUnlocked())
		{
			if (ChangeEffectResAction::CreateChangeEffectResAction(this, (NodeLockedEffect*)pLE, pParam->dResolution))
			{
				((NodeLockedEffect*)pLE)->SetPixelsPerInch(pParam->dResolution);

				// Ensure that locked effects will render their children
				// because they don't respond to ReleaseCached...
				((NodeLockedEffect*)pLE)->SetProcessedBitmap(NULL, NULL, DocRect(), 0, 0, 0, 0);
				pLE->ReleaseCached(TRUE, FALSE);

				*ppNewEffect = (NodeLockedEffect*)pLE;

				return TRUE;
			}
		}
	}
	else
		if (pLE->IsBitmapEffect())
		{
			if (ChangeEffectResAction::CreateChangeEffectResAction(this, (NodeBitmapEffect*)pLE, pParam->dResolution))
			{
				((NodeBitmapEffect*)pLE)->SetPixelsPerInch(pParam->dResolution);
				pLE->ReleaseCached();
				*ppNewEffect = (NodeBitmapEffect*)pLE;

				return TRUE;
			}
		}
		else
			ERROR3("Unknown effect type in OpEffectRes::DoEffectNodeOp");

	return FALSE;
}




///////////////////////////////////////////////////////////////////////////////////////////////
// The OpEffectLock class															    	 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	static OpState OpEffectLock::GetState(String_256* pstrDescription, OpDescriptor* pOpDesc)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/2005
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
OpState OpEffectLock::GetState(String_256* pstrDescription, OpDescriptor* pOpDesc)
{
	// default is an unticked, *GREYED*, on-menu state.
	OpState OpSt;
	OpSt.Greyed = FALSE;
	*pstrDescription = String_256(_R(IDS_DESCRIBE_LIVEEFFECT));

	return OpSt;
}



/********************************************************************************************

>	virtual void OpEffectLock::GetOpName(String_256* pstrOpName)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/2005
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
void OpEffectLock::GetOpName(String_256* pstrOpName)
{
	*pstrOpName = String_256(_R(IDS_LE_OPEFFECTLOCK));
}




/********************************************************************************************

>	virtual BOOL OpEffectLock::DoEffectOp(ListRange* pLevelRange, OpLiveEffectParam* pParam)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/2005
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
BOOL OpEffectLock::DoEffectOp(ListRange* pLevelRange, OpLiveEffectParam* pParam)
{
	BOOL bOK = FALSE;

	UINT32 idsProgress = _R(IDS_LE_UNLOCK_PROGRESS);
	if (pParam->bForceLock) idsProgress = _R(IDS_LE_LOCK_PROGRESS);
	bOK = DoEffectRangeOp(idsProgress, pLevelRange, pParam);

	return bOK;
}




/********************************************************************************************

>	BOOL OpEffectLock::DoEffectNodeOp(NodeEffect* pLE, OpLiveEffectParam* pParam, NodeBitmapEffect** ppNewEffect)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/2005
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
BOOL OpEffectLock::DoEffectNodeOp(NodeEffect* pLE, OpLiveEffectParam* pParam, NodeBitmapEffect** ppNewEffect)
{
	// We should have decent inputs...
	ERROR2IF(pLE==NULL, FALSE, "OpEffectLock::DoEffectNodeOp given NULL node");
	ERROR2IF(pParam==NULL, FALSE, "OpEffectRes::DoEffectNodeOp given NULL param");

	double dViewPPI = 96.0;
	View* pView = View::GetCurrent();
	if (pView) dViewPPI = 72000.0 / pView->GetPixelWidth().MakeDouble();
	BOOL bForceLock = pParam->bForceLock;
	NodeBitmapEffect* pNewEffect = NULL;

	if (pLE->IsBitmapEffect() && !pLE->IsFeatherEffect())
	{
		NodeBitmapEffect* pBitmapEffect = (NodeBitmapEffect*)pLE;
		if (pBitmapEffect->IsLockedEffect())
		{
			if (((NodeLockedEffect*)pBitmapEffect)->CanBeUnlocked() && bForceLock==FALSE)
			{
				// -----------------------------------------------------------------
				// Unlock
				BOOL bWasSelected = pBitmapEffect->IsSelected();

				// Get resolution for live effect from locked effect but set to
				// automatic if its the same as the current screen resolution
				double dNewRes = pBitmapEffect->GetPixelsPerInch();
				if (dNewRes==dViewPPI)
					dNewRes = 0;			// "Automatic"

				pNewEffect = DoApplyLiveEffect(this,
								  pBitmapEffect,
								  pBitmapEffect->GetPostProcessorID(),
								  pBitmapEffect->GetDisplayName(),
								  dNewRes,	// NodeBitmapEffect::DefaultLivePixelsPerInch, //pBitmapEffect->GetPixelsPerInch(),
								  pBitmapEffect->GetEditList()
								  );
				CopyEffectAttrs(pBitmapEffect, pNewEffect);
				DoFactorOutCommonChildAttributes(pNewEffect);
				DoDeletePostProcessor(this, pBitmapEffect, TRUE);

				if (bWasSelected)
				{
					pNewEffect->Select(FALSE);		// Should skip down to underlying node automatically
				}

				*ppNewEffect = pNewEffect;
				return TRUE;
			}
		}
		else
		{
			if (bForceLock==TRUE)	// Prevent unwanted extra work
			{
				// -----------------------------------------------------------------
				// Lock
				ERROR3IF(pBitmapEffect->IsLockedEffect(), "Unexpected type of BitmapEffect in OpEffectLock");
				NodeLiveEffect* pLiveEffect = (NodeLiveEffect*)pBitmapEffect;
				BOOL bWasSelected = pLiveEffect->IsParentOfSelected();
//				double dReqdResolution = NodeBitmapEffect::DefaultLockedPixelsPerInch;
				double dReqdResolution = pLiveEffect->GetPixelsPerInch();
				LPBITMAPINFO lpInfo = NULL;
				LPBYTE lpBits = NULL;
				DocRect rectBounds;
				double dRes = 0;
				Matrix matTransform;	// Defaults to identity

				if (bWasSelected)
				{
					// Deselect before modifying the tree so that flags get sorted out correctly
					pLiveEffect->DeSelect(FALSE, TRUE);
				}

				if (pLiveEffect->GetPixelsPerInch()==dReqdResolution)
				{
					// We should already have a bitmap of the required resolution
					// (unless its been chucked out of the cache)
					// If cached bitmap was result of a DirectBitmap process
					// Then get bounding rect, transformation and resolution from child
					BOOL bDirect = pLiveEffect->GetChildDirectBitmap(NULL, NULL, NULL, NULL, &matTransform, &dRes);
					if (dRes==0)
						dRes = pLiveEffect->GetPixelsPerInch();

					// We only want to get the bitmap bounds in direct space if the matrix indicates
					// a valid transform from direct space into document space.
					// If not then we want bounds from document space.
					// See also HasCachedDirectBitmap
					bDirect = bDirect && !matTransform.IsIdentity();
					pLiveEffect->GetProcessedBitmap(bDirect, &lpInfo, &lpBits, &rectBounds);

					pLiveEffect->RemoveBitmapFromCache();			// So that bitmap is no longer subject to auto-deletion
				}

				// NOTE: We don't need to regenerate here because the node will grab a bitmap automatically
				// during rendering

				pNewEffect = DoApplyLiveEffect(this,
												pLiveEffect,
												pLiveEffect->GetPostProcessorID(),
												pLiveEffect->GetDisplayName(),
												dReqdResolution,					//pLiveEffect->GetPixelsPerInch(),
												pLiveEffect->GetEditList(),
												TRUE								// bMakeLocked
												);
				CopyEffectAttrs(pLiveEffect, pNewEffect);
				DoFactorOutCommonChildAttributes(pNewEffect);

				// If we have an initialisation bitmap set it now
				if (pNewEffect && lpInfo && lpBits && rectBounds.IsValid())
				{
					pNewEffect->SetProcessedBitmap(lpInfo, lpBits, rectBounds, 0, 0, 0, 0, 72000.0/dRes, &matTransform);
				}

				DoDeletePostProcessor(this, pLiveEffect, TRUE);

				if (bWasSelected)
				{
					pNewEffect->Select(FALSE);
				}

				*ppNewEffect = pNewEffect;
				return TRUE;
			}
		}
	}
	else
		ERROR3("Unknown effect type in OpEffectLock::DoEffectNodeOp");

	return FALSE;
}




/********************************************************************************************

>	BOOL OpEffectLock::CopyEffectAttrs(Node* pSrcNode, Node* pDestNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/2005
	Inputs:		pOp - pointer to op
				pNode - 
	Outputs:	
	Returns:	TRUE if it worked
	Purpose:	Copy the Effect attributes from the specified node to another specified node
	Errors:		
	See also:	

********************************************************************************************/
BOOL OpEffectLock::CopyEffectAttrs(NodeBitmapEffect* pSrcNode, NodeBitmapEffect* pDestNode)
{
	NodeAttribute* pAttr = NodeAttribute::FindFirstAppliedAttr(pSrcNode);
	while (pAttr && pAttr->IsEffectAttribute() && pAttr->FindParent()==pSrcNode)
	{
		if (!pAttr->HasEquivalentDefaultValue(TRUE))			// Only copy attr if it doesn't look like the default
		{
			if (pDestNode->IsValidEffectAttr(pAttr))
			{
				NodeAttribute* pExistingAttr = pDestNode->FindAppliedAttribute(pAttr->GetRuntimeClass());
				if (pExistingAttr==NULL || pExistingAttr->FindParent()!=pDestNode)
				{
					// We can copy our effect attr to pInkNode as an effect attr
					// Make a copy of this LiveEffect
					NodeAttribute* pCopyAttr = NULL;
					BOOL bOK = FALSE;
					CALL_WITH_FAIL(pCopyAttr = (NodeAttribute*)pAttr->SimpleCopy(), this, bOK);
					if (!bOK) return FALSE; // No room to take a copy of the node

					pCopyAttr->AttachNode(pDestNode, LASTCHILD);
				}
			}
		}

		pAttr = NodeAttribute::FindPrevAppliedAttr(pAttr);
	}

	return TRUE;
}




///////////////////////////////////////////////////////////////////////////////////////////////
// The OpEffectLockAll class														    	 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	static OpState OpEffectLockAll::GetState(String_256* pstrDescription, OpDescriptor* pOpDesc)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/2005
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
OpState OpEffectLockAll::GetState(String_256* pstrDescription, OpDescriptor* pOpDesc)
{
	// default is an unticked, *GREYED*, on-menu state.
	OpState OpSt;
	OpSt.Greyed = FALSE;
	*pstrDescription = String_256(_R(IDS_DESCRIBE_LIVEEFFECT));

	if (XPEHost::IsEditSessionRunning())
	{
		OpSt.Greyed = TRUE;
		*pstrDescription = String_256(_R(IDS_LE_EDIT_RUNNING));
	}

	return OpSt;
}



/********************************************************************************************

>	virtual void OpEffectLockAll::GetOpName(String_256* pstrOpName)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/2005
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
void OpEffectLockAll::GetOpName(String_256* pstrOpName)
{
	*pstrOpName = String_256(_R(IDS_LE_OPEFFECTLOCKALL));
}




/********************************************************************************************

>	virtual BOOL OpEffectLockAll::DoEffectOp(ListRange* pLevelRange, OpLiveEffectParam* pLEOpParam)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/2005
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
BOOL OpEffectLockAll::DoEffectOp(ListRange* pLevelRange, OpLiveEffectParam* pLEOpParam)
{
	SelRange Sel(*(GetApplication()->FindSelection()));

	Sel.SetPromoteToParent(TRUE);
	Node* pNode = Sel.FindFirst();
	Node* pNextNode = NULL;
	NodeBitmapEffect* pNewEffect = NULL;
	BOOL bChangedSomething = FALSE;
	BOOL bChanged;
	while (pNode)
	{
		pNextNode = Sel.FindNext(pNode);

		// We must find all the uppermost effects in our subtree
		// Use a preorder traversal to do this, pruning off subtrees as soon as an effect is found
		Node* pScanNode = pNode->FindFirstPreorder();
		while (pScanNode)
		{
			if (pScanNode->IsBitmapEffect() && !((NodeBitmapEffect*)pScanNode)->IsFeatherEffect())
			{
				pNewEffect = NULL;
				bChanged = OpEffectLock::DoEffectNodeOp((NodeBitmapEffect*)pScanNode, pLEOpParam, &pNewEffect);
				if (pNewEffect && bChanged)
				{
					if (pScanNode == pNode)		// Update root node so that FindNextPreorder isn't confused
						pNode = pNewEffect;
					pScanNode = pNewEffect;
				}
				bChangedSomething = bChangedSomething || bChanged;

				// We don't want to lock any effects under this one
				// So prune this subtree from the traversal -
				// find the next node in preorder order outside the scan node
				pScanNode = pScanNode->FindNextPreorder(pNode, TRUE);
			}
			else
				pScanNode = pScanNode->FindNextPreorder(pNode);
		}

		pNode = pNextNode;
	}

	if (!bChangedSomething)
		SucceedAndDiscard();

	return TRUE;
}




BOOL OpEffectLockAll::CheckEffectOpParam(OpLiveEffectParam* pLEOpParam)
{
	return TRUE;
}




#ifdef FEATHER_EFFECT
///////////////////////////////////////////////////////////////////////////////////////////////
// The OpApplyFeatherEffect class													    	 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	static OpState OpApplyFeatherEffect::GetParamState(String_256* pstrDescription, OpDescriptor* pOpDesc, OpParam* pOpParam)

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
OpState OpApplyFeatherEffect::GetParamState(String_256* pstrDescription, OpDescriptor* pOpDesc, OpParam* pOpParam)
{
	// default is an unticked, *GREYED*, on-menu state.
	OpState OpSt;
	OpSt.Greyed = (GetApplication()->FindSelection()->FindFirst()==NULL);

	return OpSt;
}



/********************************************************************************************

>	virtual void OpApplyFeatherEffect::GetOpName(String_256* pstrOpName)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/2004
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
void OpApplyFeatherEffect::GetOpName(String_256* pstrOpName)
{
	*pstrOpName = String_256(_R(IDS_LE_OPAPPLYFEATHER));
}



/********************************************************************************************

>	virtual void OpApplyFeatherEffect::DoWithParam(OpDescriptor* pOpDesc, OpParam* pOpParam)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/09/2004
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	Insert a new FeatherEffect attribute in the stack above the selected object(s)
	Errors:		
	See also:	

********************************************************************************************/
void OpApplyFeatherEffect::DoWithParam(OpDescriptor* pOpDesc, OpParam* pOpParam)
{
	BOOL ok = FALSE;

	OpFeatherEffectParam* pFeatherOpParam = NULL;
	if (pOpParam->IsKindOf(CC_RUNTIME_CLASS(OpFeatherEffectParam)))
		pFeatherOpParam = (OpFeatherEffectParam*) pOpParam;

	INT32 InsertPos = pFeatherOpParam->StackPosition - 1;	// We want to find the surface below the current pos
													// because we are going to insert above that surface

	// Remember some details for later on when the Op ends successfully
	m_strEffectID = pFeatherOpParam->strOpUnique;
	m_iStackPos = InsertPos;

	// obtain the current selection.
	SelRange* pSelRange = GetApplication()->FindSelection();

	if (pSelRange->FindFirst()==NULL)
	{
		End();
		return;
	}

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

// Ask the range whether it's OK to do the transform...
// Get an ObjChangeParam ready, so we can ask op permission from nodes.
ObjChangeFlags cFlags(NULL, TRUE, NULL, NULL, NULL, NULL, NULL, TRUE);	// ReplaceNode & RegenerateNode
ObjChangeParam ObjChange(OBJCHANGE_STARTING, cFlags, NULL, this);
BOOL bOK = pSelRange->AllowOp(&ObjChange);
if (!bOK)
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
	pSelRange->MakePartialSelectionWhole(FALSE, FALSE, TRUE);

	// Get a list of the LiveEffects we will be working on
	ENSURE(m_pLERange==NULL, "!");
	if (InsertPos==-1)
	{
		// We are to apply the LiveEFfects to the selection
		pInitialRange = GetApplication()->FindSelection();
		m_iStackPos = 0;
	}
	else
	{
		ENSURE(pFeatherOpParam->pPPStack, "Can't find EffectsStack");
		pInitialRange = pFeatherOpParam->pPPStack->GetLevelRange(&InsertPos);
		m_iStackPos = InsertPos+1;			// Remember some details for later on when the Op ends successfully
											// (New stack pos refers to item to be shown in UI after successful insertion)
	}

	// Check whether we can edit an existing effect or whether we have to apply a new one
	if (pInitialRange==NULL)
	{
		ERROR3("If this happens, the UI logic disagrees with the Op about what the Op can do");
		FailAndExecute();
		End();
		return;
	}

	m_pLERange = DoApplyFeatherEffect(this, pInitialRange, pFeatherOpParam->strOpUnique, pFeatherOpParam->strMenuText, pFeatherOpParam->FeatherSize, pFeatherOpParam->Profile);
	m_bAppliedNewEffects = TRUE;

	// We should now have a decent list of LiveEffect nodes...
	if (m_pLERange==NULL)
	{
		FailAndExecute();
		End();
		return;
	}

	// Inform the modeless Feather UI of the change
	// and give it its own copy of the range of feather effect nodes
	OpChangeFeatherSize::SetEditContext(m_iStackPos, m_pLERange);

	// Update the LiveEffect Tool's permanent status
	if (pTool->GetID()==TOOLID_LIVEEFFECT)
	{
		((LiveEffectsTool*)pTool)->SetCurrentEffectID(m_strEffectID);
		((LiveEffectsTool*)pTool)->SetCurrentStackPos(m_iStackPos);
	}

	// invalidate the (new) region bounding the selection.
	DoInvalidateNodesRegions(*pSelRange, TRUE, FALSE, FALSE);

	// render blobs back on if the current tool doesn't automatically redraw its blobs.
	if (pSpread != NULL && pTool != NULL && !pTool->AreToolBlobsRenderedOnSelection())
		pTool->RenderToolBlobs(pSpread, NULL);

ObjChange.Define(OBJCHANGE_FINISHED, cFlags, NULL, this);
ok = UpdateChangedNodes(&ObjChange);

	BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::EFFECTSTACKCHANGED)); 

	// Tell the selrange that things have changed!
	GetApplication()->FindSelection()->Update();

	End();
}
#endif




///////////////////////////////////////////////////////////////////////////////////////////////
// The ChangeEffectResAction class													    	 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	ChangeEffectResAction::ChangeEffectResAction()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
ChangeEffectResAction::ChangeEffectResAction()
{
	m_dResolution = 0;
	m_pEffectNode = NULL;
}


/********************************************************************************************

>	ActionCode ChangeEffectResAction::Init(	Operation* pOp,
											ActionList* pActionList,
											Action** NewAction)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/2005
	Inputs:		pOp is the pointer to the operation to which this action belongs
				pTool is 
				pActionList is the action list to which this action should be added
	Outputs:	NewAction is a pointer to a pointer to an action, allowing the function to
				return a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no
				room in the undo buffer (which is determined by the base class Init function
				called within) the function will either return AC_NO_RECORD which means the
				operation can continue, but no undo information needs to be stored, or AC_OK
				which means the operation should continue AND record undo information. If
				the function returns AC_FAIL, there was not enough memory to record the undo
				information, and the user has decided not to continue with the operation.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/
ActionCode ChangeEffectResAction::Init(	Operation* pOp,
										ActionList* pActionList,
										Action** NewAction)
{
	UINT32 ActSize = sizeof(ChangeEffectResAction);

	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(ChangeEffectResAction), NewAction);

	return Ac;
}



/********************************************************************************************

>	BOOL ChangeEffectResAction::CreateChangeEffectResAction(Operation* pOp,
											  NodeBitmapEffect* pEffectNode,
											  double dRes
											 )

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/2005
	Inputs:		pOp is the currently running operation
				pActionList is a pointer ot the action list to which the action should be appended
	Outputs:	-
	Returns:	Action code which indicates success or failure to create the action
	Purpose:	This static function makes it a little easier to use this action. It creates an instance
				of this action and appends it to the action list.
	Errors:		-

********************************************************************************************/

BOOL ChangeEffectResAction::CreateChangeEffectResAction(Operation* pOp,
											  NodeBitmapEffect* pEffectNode,
											  double dRes
											 )
{
	ERROR3IF(pOp==NULL, "CreateChangeEffectResAction given NULL op pointer");
	ERROR3IF(pEffectNode==NULL, "CreateChangeEffectResAction given NULL node pointer");

	if (dRes == pEffectNode->GetPixelsPerInchValue())	// Prevent unwanted extra work
		return FALSE;

	ChangeEffectResAction* RecAction = NULL;
	ActionCode Act = ChangeEffectResAction::Init(pOp, pOp->GetUndoActionList(), (Action**)&RecAction);
	if ( (Act == AC_OK) && (RecAction != NULL) )
	{
		RecAction->m_dResolution = pEffectNode->GetPixelsPerInchValue();
		RecAction->m_pEffectNode = pEffectNode;
	}
	else if (Act==AC_FAIL)
	{
		pOp->FailAndExecute();
		pOp->End();
	}

	return (Act != AC_FAIL);
}



/********************************************************************************************

>	ActionCode ChangeEffectResAction::Execute()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/2005
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, either AC_OK, AC_NORECORD or AC_FAIL
	Purpose:	Inserts the floating endpoint into the Line Tool, creating an undo action to
				remove the endpoint.
	Errors:		-
	SeeAlso:	RemoveFloaterAction::Execute

********************************************************************************************/
ActionCode ChangeEffectResAction::Execute()
{
	ChangeEffectResAction* ReAction = NULL;
	ActionCode Act = AC_FAIL;
	
	// Create a redo action for this action, which is another ChangeEffectResAction
	Act = ChangeEffectResAction::Init(pOperation, 
									pOppositeActLst, 
									(Action**)(&ReAction));
	if (Act == AC_FAIL)
		return AC_FAIL;

	// ---------------------------------------------------
	// Record the current values in the Action
	ReAction->m_pEffectNode = m_pEffectNode;
	ReAction->m_dResolution = m_pEffectNode->GetPixelsPerInchValue();;

	// ---------------------------------------------------
	// Now do the actual action
	if (m_pEffectNode->IsLockedEffect())
	{
		((NodeLockedEffect*)m_pEffectNode)->SetPixelsPerInch(m_dResolution);
	}
	else
		if (m_pEffectNode->IsBitmapEffect())
		{
			((NodeBitmapEffect*)m_pEffectNode)->SetPixelsPerInch(m_dResolution);
			m_pEffectNode->ReleaseCached();
		}

	return Act;
}

ChangeEffectResAction::~ChangeEffectResAction()
{
	m_pEffectNode = NULL;
}




///////////////////////////////////////////////////////////////////////////////////////////////
// The ToggleLockAction class														    	 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	ToggleLockAction::ToggleLockAction()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
/*ToggleLockAction::ToggleLockAction()
{
	m_bLockedState = FALSE;
	m_pEffectNode = NULL;
}*/


/********************************************************************************************

>	ActionCode ToggleLockAction::Init(	Operation* pOp,
											ActionList* pActionList,
											Action** NewAction)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/2005
	Inputs:		pOp is the pointer to the operation to which this action belongs
				pTool is 
				pActionList is the action list to which this action should be added
	Outputs:	NewAction is a pointer to a pointer to an action, allowing the function to
				return a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no
				room in the undo buffer (which is determined by the base class Init function
				called within) the function will either return AC_NO_RECORD which means the
				operation can continue, but no undo information needs to be stored, or AC_OK
				which means the operation should continue AND record undo information. If
				the function returns AC_FAIL, there was not enough memory to record the undo
				information, and the user has decided not to continue with the operation.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/
/*ActionCode ToggleLockAction::Init(	Operation* pOp,
										ActionList* pActionList,
										Action** NewAction)
{
	UINT32 ActSize = sizeof(ToggleLockAction);

	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(ToggleLockAction), NewAction);

	return Ac;
}*/



/********************************************************************************************

>	BOOL ToggleLockAction::CreateToggleLockAction(Operation* pOp,
											  NodeBitmapEffect* pEffectNode,
											  BOOL bForceLock
											 )

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/2005
	Inputs:		pOp is the currently running operation
				pActionList is a pointer ot the action list to which the action should be appended
	Outputs:	-
	Returns:	Action code which indicates success or failure to create the action
	Purpose:	This static function makes it a little easier to use this action. It creates an instance
				of this action and appends it to the action list.
	Errors:		-

********************************************************************************************/

/*BOOL ToggleLockAction::CreateToggleLockAction(Operation* pOp,
											  NodeBitmapEffect* pEffectNode,
											  BOOL bForceLock
											 )
{
	ERROR3IF(pOp==NULL, "CreateToggleLockAction given NULL op pointer");
	ERROR3IF(pEffectNode==NULL, "CreateToggleLockAction given NULL node pointer");

	if (pEffectNode->IsLockedEffect() == bForceLock)	// Prevent unwanted extra work
		return FALSE;

	ToggleLockAction* RecAction = NULL;
	ActionCode Act = ToggleLockAction::Init(pOp, pOp->GetUndoActionList(), (Action**)&RecAction);
	if ( (Act == AC_OK) && (RecAction != NULL) )
	{
		RecAction->m_bLockedState = pEffectNode->IsLockedEffect();
		RecAction->m_pEffectNode = pEffectNode;
	}
	else if (Act==AC_FAIL)
	{
		pOp->FailAndExecute();
		pOp->End();
	}

	return (Act != AC_FAIL);
}*/



/********************************************************************************************

>	ActionCode ToggleLockAction::Execute()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/2005
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, either AC_OK, AC_NORECORD or AC_FAIL
	Purpose:	Inserts the floating endpoint into the Line Tool, creating an undo action to
				remove the endpoint.
	Errors:		-
	SeeAlso:	RemoveFloaterAction::Execute

********************************************************************************************/
/*ActionCode ToggleLockAction::Execute()
{
	ToggleLockAction* ReAction = NULL;
	ActionCode Act = AC_FAIL;
	
	// Create a redo action for this action, which is another ToggleLockAction
	Act = ToggleLockAction::Init(pOperation, 
									pOppositeActLst, 
									(Action**)(&ReAction));
	if (Act == AC_FAIL)
		return AC_FAIL;

	// ---------------------------------------------------
	// Record the current values in the Action
	ReAction->m_pEffectNode = m_pEffectNode;
ERROR3("Unimplemented!");
//	ReAction->m_dResolution = m_pEffectNode->GetPixelsPerInchValue();;

	// ---------------------------------------------------
	// Now do the actual action

	return Act;
}

ToggleLockAction::~ToggleLockAction()
{
	m_pEffectNode = NULL;
}*/




///////////////////////////////////////////////////////////////////////////////////////////////
// The MarkEditListAction class														    	 //
///////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	MarkEditListAction::MarkEditListAction()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/09/2004
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
MarkEditListAction::MarkEditListAction()
{
	pEditList = NULL;
	iStackPos = -1;
}


/********************************************************************************************

>	ActionCode MarkEditListAction::Init(	Operation* pOp,
											ActionList* pActionList,
											Action** NewAction)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/09/2004
	Inputs:		pOp is the pointer to the operation to which this action belongs
				pTool is 
				pActionList is the action list to which this action should be added
	Outputs:	NewAction is a pointer to a pointer to an action, allowing the function to
				return a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no
				room in the undo buffer (which is determined by the base class Init function
				called within) the function will either return AC_NO_RECORD which means the
				operation can continue, but no undo information needs to be stored, or AC_OK
				which means the operation should continue AND record undo information. If
				the function returns AC_FAIL, there was not enough memory to record the undo
				information, and the user has decided not to continue with the operation.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/
ActionCode MarkEditListAction::Init(	Operation* pOp,
										ActionList* pActionList,
										Action** NewAction)
{
	UINT32 ActSize = sizeof(MarkEditListAction);

	ActionCode Ac = Action::Init( pOp, pActionList, ActSize, CC_RUNTIME_CLASS(MarkEditListAction), NewAction);

	return Ac;
}



/********************************************************************************************

>	ActionCode MarkEditListAction::DoMarkEditList(Operation* pOp,
											ActionList* pActionList
											)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/09/2004
	Inputs:		pOp is the currently running operation
				pActionList is a pointer ot the action list to which the action should be appended
				pTool is a pointer to the Line tool.
	Outputs:	-
	Returns:	Action code which indicates success or failure to create the action
	Purpose:	This static function makes it a little easier to use this action. It creates an instance
				of this action and appends it to the action list.
	Errors:		-
	SeeAlso:	RemoveFloaterAction::DoRemove

********************************************************************************************/

ActionCode MarkEditListAction::DoMarkEditList(Operation* pOp,
											  ActionList* pActionList,
											  EffectsStack* pPPStack,
											  INT32 iStackPos
											 )
{
	ERROR2IF(iStackPos<0, AC_FAIL, "Bad stack pos in DoMarkEditList");

	MarkEditListAction* RecAction = NULL;
	ActionCode Act = MarkEditListAction::Init(pOp, pActionList, (Action**)&RecAction);
	if ( (Act == AC_OK) && (RecAction != NULL) )
	{
		PPStackLevel* pLevel = (PPStackLevel*)pPPStack->FindItem(iStackPos);
		if (pLevel && pLevel->pPPNode && pLevel->pPPNode->IsBitmapEffect())
		{
			RecAction->pEditList = CXMLUtils::NewDocument(((NodeBitmapEffect*)pLevel->pPPNode)->GetEditList());
			RecAction->iStackPos = iStackPos;
		}
		else
			return AC_FAIL;
	}
	return Act;
}



/********************************************************************************************

>	ActionCode MarkEditListAction::Execute()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/09/2004
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, either AC_OK, AC_NORECORD or AC_FAIL
	Purpose:	Inserts the floating endpoint into the Line Tool, creating an undo action to
				remove the endpoint.
	Errors:		-
	SeeAlso:	RemoveFloaterAction::Execute

********************************************************************************************/
ActionCode MarkEditListAction::Execute()
{
	ERROR2IF(this->iStackPos<0, AC_FAIL, "Bad stack pos in MarkEditListAction::Execute");

	MarkEditListAction* ReAction = NULL;
	ActionCode Act = AC_FAIL;
	
	// Create a redo action for this action, which is another MarkEditListAction
	Act = MarkEditListAction::Init(pOperation, 
									pOppositeActLst, 
									(Action**)(&ReAction));
	if (Act == AC_FAIL)
		return AC_FAIL;

	// ---------------------------------------------------
	// Record the current edit list in the Action?
	EffectsStack* pPPStack = EffectsStack::GetEffectsStackFromSelection();
	if (pPPStack==NULL)
		return AC_FAIL;

	PPStackLevel* pLevel = (PPStackLevel*)pPPStack->FindItem(this->iStackPos);
	if (pLevel && pLevel->pPPNode && pLevel->pPPNode->IsBitmapEffect())
	{
		ReAction->pEditList = CXMLUtils::NewDocument(((NodeBitmapEffect*)pLevel->pPPNode)->GetEditList());
		ReAction->iStackPos = this->iStackPos;
	}
	else
	{
		ReAction->pEditList = CXMLUtils::NewDocument(this->pEditList);
		ReAction->iStackPos = this->iStackPos;
	}

	// ---------------------------------------------------
	// Now do the actual action
	// Run through the selection, setting edit lists on LiveEffects
	Node* pNode = NULL;
	if (pLevel) pNode = pLevel->listSelNodes.FindFirst();
	while (pNode)
	{
		if (pNode->IsBitmapEffect())
		{
			NodeBitmapEffect* pLE = (NodeBitmapEffect*)pNode;
			pLE->SetProcessedBitmap(NULL, NULL, DocRect(), 0, 0, 0, 0);			// Remove cached bitmap to be sure
			pLE->SetEditList(CXMLUtils::NewDocument(this->pEditList));
		}
		else
			return AC_FAIL;
		
		pNode = pLevel->listSelNodes.FindNext(pNode);
	}

	delete pPPStack;
	pPPStack = NULL;

	return Act;
}

MarkEditListAction::~MarkEditListAction()
{
	pEditList = NULL;
}




