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
// FreeHand Tool Info bar operations

#include "camtypes.h"
#include "camelot.h"
#include "liveeffectstool.h"
#include "liveeffectsinfo.h"
//#include "range.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "resource.h"
//#include "barsdlgs.h"
//#include "justin2.h"
//#include "will.h" // for the 'many' string
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docvmsg.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "colmsg.h"
#include "brushmsg.h"
//#include "liveeffectres.h"
#include "xpehost.h"
#include "opliveeffects.h"
#include "nodeliveeffect.h"
#include "liveeffectsmenu.h"
#include "shadtool.h"
#include "opfeathr.h"
#include "appprefs.h"

DECLARE_SOURCE( "$Revision$" );

// Declare stuff for the memory tracking
CC_IMPLEMENT_DYNCREATE( LiveEffectsInfoBarOp, InformationBarOp )
#define new CAM_DEBUG_NEW




/********************************************************************************************

>	LiveEffectsInfoBarOp::LiveEffectsInfoBarOp()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/94
	Purpose:	Sets the info bars vars to NULL defaults

********************************************************************************************/

LiveEffectsInfoBarOp::LiveEffectsInfoBarOp()
{
	// Don't know about the tool and can not retro fit to start with
	m_pTool = NULL;

	// The Controls hasve not been created yet
	m_bControlsExist = FALSE;
	m_bLockSwitchState = FALSE;
	m_bLockAllSwitchState = FALSE;
}




/********************************************************************************************

>	LiveEffectsInfoBarOp::~LiveEffectsInfoBarOp()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/2000
	Purpose:	destructor

********************************************************************************************/

LiveEffectsInfoBarOp::~LiveEffectsInfoBarOp()
{
}


/********************************************************************************************

>	MsgResult LiveEffectsInfoBarOp::Message(Msg* Message) 

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/94
	Inputs:		Message - A dialog message 
	Returns:	The result of the message
	Purpose:	Takes a look at the message and if it is for me it responds to it.

********************************************************************************************/

MsgResult LiveEffectsInfoBarOp::Message(Msg* Message) 
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* DMsg = (DialogMsg*)Message;

		// Check if the message is a CANCEL
		if (DMsg->DlgMsg == DIM_CANCEL)
		{
			// Mark the controls as no longer existing
			m_bControlsExist = FALSE;
			
			// Close the dialog 
			Close();
		}

		else if (DMsg->DlgMsg == DIM_CREATE)
		{
			HandleCreate();
		}

		else
		{
			// Messages to all the controls, handled indivdually
			switch (DMsg->GadgetID)
			{
				case _R(IDC_LE_LABEL):
				{
					GadgetMessage_Label(DMsg);
					break;
				}
				case _R(IDC_COMBOBOX_LE_ORDER):
				{
					GadgetMessage_Order(DMsg);
					break;
				}
				case _R(IDC_CCBUTTON_LE_EDIT):
				case _R(IDC_BUTTON_LE_EDIT):
				{
					GadgetMessage_Edit(DMsg);
					break;
				}
				case _R(IDC_CCBUTTON_LE_ADD):
				case _R(IDC_BUTTON_LE_ADD):
				{
					GadgetMessage_Add(DMsg);
					break;
				}
				case _R(IDC_CCBUTTON_LE_INSERT):
				{
					GadgetMessage_Insert(DMsg);
					break;
				}
				case _R(IDC_COMBOBOX_LE_RES):
				{
					GadgetMessage_Resolution(DMsg);
					break;
				}
				case _R(IDC_CCBUTTON_LE_TOGGLELOCK):
				{
					GadgetMessage_ToggleLock(DMsg);
					break;
				}
				case _R(IDC_CCBUTTON_LE_REMOVE):
				{
					GadgetMessage_Remove(DMsg);
					break;
				}
				case _R(IDC_CCBUTTON_LE_REMOVEALL):
				{
					GadgetMessage_RemoveAll(DMsg);
					break;
				}
				case _R(IDC_CCBUTTON_LE_TOGGLELOCKALL):
				{
					GadgetMessage_ToggleLockAll(DMsg);
					break;
				}
				case _R(IDC_BUTTON_LE_SETUP):
				{
					GadgetMessage_Setup(DMsg);
					break;
				}
			}
		}
	}
	else
	{
		// Handle non dialog messages in here

		// likewise if it is a screen change message
// Commented out, Phil, 25/10/2004
// This upsets OpLiveEffect::UpdateCurrentEditor because it causes one of these messages to be sent while it's working
// and so the current PPStack that it's working on is deleted from under it by ScreenChanged
//		if (Message->IsKindOf(CC_RUNTIME_CLASS(ScreenChangeMsg)) || Message->IsKindOf(CC_RUNTIME_CLASS(CurrentAttrChangedMsg)))
//		{
//			if (m_pTool != NULL && HasWindow() 
//				&& (Document::GetCurrent() != NULL))
//			{
//				m_pTool->ScreenChanged(TRUE); //((ScreenChangeMsg*)Message)->m_bClear);
//				UpdateState();
//			}
//		}


		if (Message->IsKindOf(CC_RUNTIME_CLASS(DocViewMsg)))
		{
			DocViewMsg* pDVM = (DocViewMsg*)Message;
			if (pDVM->State == pDVM->DocViewState::SELCHANGED)
				m_pTool->ScreenChanged(TRUE);
		}
	}

	// do we have a selection change message? 
	if (MESSAGE_IS_A(Message, SelChangingMsg))
	{
		Document* pDoc  = Document::GetCurrent();
		if (pDoc != NULL)
		{
			if (m_bControlsExist && IsVisible())
			{
			}
		}
		// ask the tool to refresh its cache, as blobs will have changed
		// TODO: Could test for and exclude COLOURATTCHANGED and NONCOLOURATTCHANGED here?
		if (m_pTool != NULL && HasWindow()) 
			m_pTool->ScreenChanged(TRUE); 
	}

	if (MESSAGE_IS_A(Message, ColourChangingMsg))
	{
		ColourChangingMsg *TheMsg = (ColourChangingMsg *) Message;

		// handle the selected View's Colour Context is changing
		// e.g. if the show printer colours (plate) has changed
		if(TheMsg->State == ColourChangingMsg::ColourState::SELVIEWCONTEXTCHANGE)
		{
			// ask the tool to refresh its cache
			// (so that the cache does not have the old colour in it
			//  and the drawing will be agains the correct (current)
			//  background!)
			if (m_pTool != NULL && HasWindow()) 
				m_pTool->ScreenChanged(TRUE);
		}
	}

	if (MESSAGE_IS_A(Message, DocChangingMsg))
	{
		DocChangingMsg* pMsg = (DocChangingMsg*) Message;
		if (pMsg->State == DocChangingMsg::DocState::ABOUTTODIE && XPEHost::GetEditSessionDocument()==pMsg->pChangingDoc)
		{
			XPEHost::EndEditLiveEffect();
		}
	}

	else if (MESSAGE_IS_A(Message,OpMsg)) 	// Check for undo/redo
	{
		// update the gadgets
//		UpdateState();
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		ENSURE(BlobMgr, "Can't get BlobManager");
		
		if ( (((OpMsg*)Message)->MsgType == OpMsg::BEFORE_UNDO) ||
										(((OpMsg*)Message)->MsgType == OpMsg::BEFORE_REDO))
		{
			OpLiveEffect* pCurrentOp = XPEHost::GetCurrentLEOp();

			// If we haven't got a list of editors yet then get one
			if (pCurrentOp!=NULL)
			{
/*				// Attempt to suspend the modeless XPE edit while we undo
				m_strUndoEffectID = pCurrentOp->GetEffectID();
				m_iUndoStackPos = pCurrentOp->GetStackPos();
				// Stop the current Op and commit all of its changes
				// Disconnect it from the XPE system so that, just for the moment at least, other routines
				// think that there's no running edit session
				pCurrentOp->DoEndEdit(TRUE, TRUE);				// Sends ScreenChangeMsg - and thus releases pPPStack
				XPEHost::SetCurrentLEOp(NULL);					// Tells UpdateCurrentEditor not to try to update dying editor
*/
				// This is the only safe thing to do at the moment...
//				XPEHost::EndEditLiveEffect();
				pCurrentOp->DoEndEdit(TRUE, TRUE, TRUE, TRUE);
				XPEHost::SetCurrentLEOp(NULL);
				XPEHost::EndEditLiveEffect();
			}
		}
		else if ( (((OpMsg*)Message)->MsgType == OpMsg::AFTER_UNDO) ||
										(((OpMsg*)Message)->MsgType == OpMsg::AFTER_REDO))
		{
/*			// Attempt tp restore the modeless XPE edit after undo
			// Reset pPPStack in LiveEffectTool to reset the Tool after the ScreenChangeMsg sent above
			// (prevent idle handler looping forever calling this function)
			EffectsStack* pPPStack = EffectsStack::GetEffectsStackFromSelection();

			// Try to find the same effect in the new stack
			// If found then invoke a new Op to use the running editor on that effect
			BOOL bOK = pPPStack->FindBestProcessor(&m_strUndoEffectID, &m_iUndoStackPos);

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
				m_iUndoStackPos = STACKPOS_INSERT_TOP;
			}

			// Start a new Op but tell it to use the existing edit session instead of making a new one
			if (pOp)
			{
				OpLiveEffectParam Param;
				Param.strOpUnique = m_strUndoEffectID;
				Param.StackPosition = m_iUndoStackPos;
				Param.pPPStack = pPPStack;
				Param.bReUseEditor = TRUE;				// This is the important param!
				BOOL bValid = XPEHost::GetEffectDetails(m_strUndoEffectID, &Param.strMenuText, &Param.bIsDestructive);

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
						return (InformationBarOp::Message(Message));
					}
				}
			}

			// If we get here then we must have failed in some way so close down the running editor
			XPEHost::EndEditLiveEffect();
			delete pPPStack;
*/
		}
	}

	// Pass message on
	return (InformationBarOp::Message(Message));
}    




/********************************************************************************************

>	void LiveEffectsInfoBarOp::GadgetMessage_Order(DialogMsg* Message) 

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/10/2004
	Inputs:		Message - A dialog message 
	Returns:	The result of the message
	Purpose:	Takes a look at the message and if it is for me it responds to it.
	Note:		Strings in this gadget are NOT distinct - you cannot use string matching
				on effect names to identify which is selected!

********************************************************************************************/

void LiveEffectsInfoBarOp::GadgetMessage_Order(DialogMsg* Message) 
{
	switch (Message->DlgMsg)
	{
		case DIM_SELECTION_CHANGED:
		{
			WORD Index;
			GetValueIndex(Message->GadgetID, &Index);
			EffectsStack* pStack = m_pTool->GetEffectsStack();
			PPStackLevel* pStackLevel = (PPStackLevel*)pStack->FindItem(Index);

			if (pStackLevel)
			{
				String_256 strEffectID = pStackLevel->strPostProID;
				m_pTool->SetCurrentEffectID(strEffectID);
				m_pTool->SetCurrentStackPos(Index);
			}
			else
			{
				m_pTool->SetCurrentEffectID(String(""));
				m_pTool->SetCurrentStackPos(Index);
			}

			UpdateState();

#ifdef FEATHER_EFFECT
			String_256 strCurrentEffectID = m_pTool->GetCurrentEffectID();
			if (strCurrentEffectID==String_256(POSTPRO_ID_FEATHER))
			{
				INT32 iStackPos = m_pTool->GetCurrentStackPos();
				ListRange* pFeatherRange = m_pTool->GetEffectsStack()->GetLevelRange(&iStackPos);
				OpChangeFeatherSize::SetEditContext(iStackPos, pFeatherRange);
				return;
			}
#endif

		}
	}
}




/********************************************************************************************

>	void LiveEffectsInfoBarOp::GadgetMessage_Edit(DialogMsg* Message)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/10/2004
	Inputs:		Message - A dialog message 
	Returns:	The result of the message
	Purpose:	Takes a look at the message and if it is for me it responds to it.

********************************************************************************************/

void LiveEffectsInfoBarOp::GadgetMessage_Edit(DialogMsg* Message)
{
	switch (Message->DlgMsg)
	{
		case DIM_LFT_BN_CLICKED:
		{
			String_256 strCurrentEffectID = m_pTool->GetCurrentEffectID();

			// Special case for old legacy effects
			if (strCurrentEffectID==String_256(POSTPRO_ID_SHADOW))
			{
				// Make sure the Shadow tool is current... (this func can be called from other tools)
				Tool* pShadowTool = Tool::FindTool(TOOLID_SOFTSHADOW);
				if (pShadowTool && !pShadowTool->IsCurrent())
				{
					INT32 iStackPos = m_pTool->GetCurrentStackPos();
					ListRange* pShadowRange = m_pTool->GetEffectsStack()->GetLevelRange(&iStackPos);
					if (pShadowRange)
					{
						((SoftShadowTool*)pShadowTool)->SelectShadowRange(pShadowRange);
					}

					pShadowTool->SetCurrent();
////					((SoftShadowTool*)pShadowTool)->SetCurrentShadowLevelPos(m_pTool->GetCurrentStackPos());
//					INT32 iStackPos = m_pTool->GetCurrentStackPos();
//					ListRange* pShadowRange = m_pTool->GetEffectsStack()->GetLevelRange(&iStackPos);
//					((SoftShadowTool*)pShadowTool)->SetShadowEditRange(pShadowRange, m_pTool->GetCurrentStackPos(), TRUE);	// Must be consistent because it comes from the PPStack
					return;
				}
			}

#ifdef FEATHER_EFFECT
			if (strCurrentEffectID==String_256(POSTPRO_ID_FEATHER))
			{
				INT32 iStackPos = m_pTool->GetCurrentStackPos();
				ListRange* pFeatherRange = m_pTool->GetEffectsStack()->GetLevelRange(&iStackPos);
				OpChangeFeatherSize::SetEditContext(iStackPos, pFeatherRange);
				return;
			}
#endif

			if (strCurrentEffectID!=String(""))
			{
				OpDescriptor* pOp = NULL;
				pOp = OpDescriptor::FindOpDescriptor(OPTOKEN_EDIT_LIVEEFFECT);

				if (pOp)
				{
					// Commit changes of any modeless edit session already running
					// and close it down ready for this new one...
					if (XPEHost::IsEditSessionRunning())
					{
						XPEHost::EndEditLiveEffect();		// Could change PPStack!
						m_pTool->UpdatePPStack();
					}

					OpLiveEffectParam Param;
					Param.strOpUnique = strCurrentEffectID;
					Param.StackPosition = m_pTool->GetCurrentStackPos();
					Param.pPPStack = m_pTool->GetEffectsStack();
					BOOL bValid = XPEHost::GetEffectDetails(strCurrentEffectID, &Param.strMenuText, &Param.bIsDestructive);

					if (bValid)
					{
						pOp->Invoke(&Param);
						return;
					}
				}
			}
		}
	}
}




/********************************************************************************************

>	void LiveEffectsInfoBarOp::GadgetMessage_Add(DialogMsg* Message)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/10/2004
	Inputs:		Message - A dialog message 
	Returns:	The result of the message
	Purpose:	Takes a look at the message and if it is for me it responds to it.

********************************************************************************************/

void LiveEffectsInfoBarOp::GadgetMessage_Add(DialogMsg* Message)
{
	switch (Message->DlgMsg)
	{
		case DIM_LFT_BN_CLICKED:
		{
			// Commit changes of any modeless edit session already running
			// and close it down ready for this new one...
			if (XPEHost::IsEditSessionRunning())
			{
				XPEHost::EndEditLiveEffect();		// Could change PPStack!
				m_pTool->UpdatePPStack();
			}

			LiveEffectsMenu* pLEMenu = new LiveEffectsMenu();
			if (pLEMenu)
			{
				pLEMenu->SetStackPos(STACKPOS_INSERT_TOP);
				pLEMenu->SetEffectsStack(m_pTool->GetEffectsStack());
				pLEMenu->Show();
			}
		}
	}
}




/********************************************************************************************

>	void LiveEffectsInfoBarOp::GadgetMessage_Label(DialogMsg* Message)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/10/2004
	Inputs:		Message - A dialog message 
	Returns:	The result of the message
	Purpose:	Takes a look at the message and if it is for me it responds to it.

********************************************************************************************/

void LiveEffectsInfoBarOp::GadgetMessage_Label(DialogMsg* Message)
{
	switch (Message->DlgMsg)
	{
		case DIM_RGT_BN_CLICKED:
		{
			// Commit changes of any modeless edit session already running
			// and close it down ready for this new one...
			if (XPEHost::IsEditSessionRunning())
			{
				XPEHost::EndEditLiveEffect();		// Could change PPStack!
				m_pTool->UpdatePPStack();
			}

			LiveEffectsMenu* pLEMenu = new LiveEffectsMenu();
			if (pLEMenu)
			{
				pLEMenu->SetStackPos(STACKPOS_INSERT_TOP);
				pLEMenu->SetEffectsStack(m_pTool->GetEffectsStack());
				pLEMenu->Show();
			}
		}
	}
}




/********************************************************************************************

>	void LiveEffectsInfoBarOp::GadgetMessage_Resolution(DialogMsg* Message) 

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/06/2005
	Inputs:		Message - A dialog message 
	Returns:	The result of the message
	Purpose:	Takes a look at the message and if it is for me it responds to it.

********************************************************************************************/

void LiveEffectsInfoBarOp::GadgetMessage_Resolution(DialogMsg* Message) 
{
	switch (Message->DlgMsg)
	{
		case DIM_SELECTION_CHANGED:
		{
			// Commit changes of any modeless edit session already running
			// and close it down ready for this new one...
			if (XPEHost::IsEditSessionRunning())
			{
				XPEHost::EndEditLiveEffect();		// Could change PPStack!
				m_pTool->UpdatePPStack();
			}

			BOOL Valid = FALSE;
			INT32 lValue = 0;

			lValue = GetResDropListValue(_R(IDC_COMBOBOX_LE_RES), TRUE, &Valid);

			if (Valid)
			{
				// Update the resolution of the specified effects
				OpDescriptor* pOp = NULL;
				pOp = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGE_EFFECT_RES);

				if (pOp)
				{
					OpLiveEffectParam Param;
//					Param.strOpUnique = strCurrentEffectID;
					Param.StackPosition = m_pTool->GetCurrentStackPos();
					Param.pPPStack = m_pTool->GetEffectsStack();
					Param.dResolution = (double) lValue;
//					BOOL bValid = XPEHost::GetEffectDetails(strCurrentEffectID, &Param.strMenuText, &Param.bIsDestructive);

					pOp->Invoke(&Param);
					return;
				}
			}

			// Repopulate the dropdown so that if the user missed off the ppi
			// it is restored
			EffectsStack* pStack = m_pTool->GetEffectsStack();
			INT32 iStackPos = m_pTool->GetCurrentStackPos();
			ListRange* pLevelRange = NULL;
			if (pStack)
				pLevelRange = pStack->GetLevelRange(&iStackPos);

			PopulateResolutionDropdown(pStack, pLevelRange, FALSE);
		}
	}
}




/********************************************************************************************

>	void LiveEffectsInfoBarOp::GadgetMessage_ToggleLock(DialogMsg* Message)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/06/2005
	Inputs:		Message - A dialog message 
	Returns:	The result of the message
	Purpose:	Takes a look at the message and if it is for me it responds to it.

********************************************************************************************/

void LiveEffectsInfoBarOp::GadgetMessage_ToggleLock(DialogMsg* Message)
{
	switch (Message->DlgMsg)
	{
		case DIM_LFT_BN_CLICKED:
		{
			// Commit changes of any modeless edit session already running
			// and close it down ready for this new one...
			if (XPEHost::IsEditSessionRunning())
			{
				XPEHost::EndEditLiveEffect();		// Could change PPStack!
				m_pTool->UpdatePPStack();
			}

			// Toggle the state of the button
			m_bLockSwitchState = !m_bLockSwitchState;
			BOOL bIsSelected = GetBoolGadgetSelected(Message->GadgetID, m_bLockSwitchState);

			OpDescriptor* pOp = NULL;
			pOp = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGE_EFFECT_LOCK);

			if (pOp)
			{
				OpLiveEffectParam Param;
//				Param.strOpUnique = strCurrentEffectID;
				Param.StackPosition = m_pTool->GetCurrentStackPos();
				Param.pPPStack = m_pTool->GetEffectsStack();
//				BOOL bValid = XPEHost::GetEffectDetails(strCurrentEffectID, &Param.strMenuText, &Param.bIsDestructive);
				Param.bForceLock = m_bLockSwitchState;

				pOp->Invoke(&Param);
				return;
			}
		}
	}
}




/********************************************************************************************

>	void LiveEffectsInfoBarOp::GadgetMessage_ToggleLockAll(DialogMsg* Message)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/06/2005
	Inputs:		Message - A dialog message 
	Returns:	The result of the message
	Purpose:	Takes a look at the message and if it is for me it responds to it.

********************************************************************************************/

void LiveEffectsInfoBarOp::GadgetMessage_ToggleLockAll(DialogMsg* Message)
{
	switch (Message->DlgMsg)
	{
		case DIM_LFT_BN_CLICKED:
		{
			// Commit changes of any modeless edit session already running
			// and close it down ready for this new one...
			if (XPEHost::IsEditSessionRunning())
			{
				XPEHost::EndEditLiveEffect();		// Could change PPStack!
				m_pTool->UpdatePPStack();
			}

			// Toggle the state of the button
			m_bLockAllSwitchState = !m_bLockAllSwitchState;
			BOOL bIsSelected = GetBoolGadgetSelected(Message->GadgetID, m_bLockAllSwitchState);

			OpDescriptor* pOp = NULL;
			pOp = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGE_EFFECT_LOCKALL);

			if (pOp)
			{
				OpLiveEffectParam Param;
				Param.bForceLock = m_bLockAllSwitchState;

				pOp->Invoke(&Param);
				return;
			}
		}
	}
}




/********************************************************************************************

>	void LiveEffectsInfoBarOp::GadgetMessage_Insert(DialogMsg* Message)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/10/2004
	Inputs:		Message - A dialog message 
	Returns:	The result of the message
	Purpose:	Takes a look at the message and if it is for me it responds to it.

********************************************************************************************/

void LiveEffectsInfoBarOp::GadgetMessage_Insert(DialogMsg* Message)
{
	switch (Message->DlgMsg)
	{
		case DIM_LFT_BN_CLICKED:
		{
			// Commit changes of any modeless edit session already running
			// and close it down ready for this new one...
			if (XPEHost::IsEditSessionRunning())
			{
				XPEHost::EndEditLiveEffect();		// Could change PPStack!
				m_pTool->UpdatePPStack();
			}

			LiveEffectsMenu* pLEMenu = new LiveEffectsMenu();
			if (pLEMenu)
			{
				pLEMenu->SetStackPos(m_pTool->GetCurrentStackPos());
				pLEMenu->SetEffectsStack(m_pTool->GetEffectsStack());
				pLEMenu->Show();
			}
		}
	}
}




/********************************************************************************************

>	void LiveEffectsInfoBarOp::GadgetMessage_Remove(DialogMsg* Message)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/10/2004
	Inputs:		Message - A dialog message 
	Returns:	The result of the message
	Purpose:	Takes a look at the message and if it is for me it responds to it.

********************************************************************************************/

void LiveEffectsInfoBarOp::GadgetMessage_Remove(DialogMsg* Message)
{
	switch (Message->DlgMsg)
	{
		case DIM_LFT_BN_CLICKED:
		{
			String_256 strCurrentEffectID = m_pTool->GetCurrentEffectID();
			if (strCurrentEffectID!=String(""))
			{
				// Commit changes of any modeless edit session already running
				// and close it down ready for this new one...
				if (XPEHost::IsEditSessionRunning())
				{
					XPEHost::EndEditLiveEffect();		// Could change PPStack!
					m_pTool->UpdatePPStack();
				}

				OpDescriptor* pOp = NULL;
				pOp = OpDescriptor::FindOpDescriptor(OPTOKEN_DELETE_LIVEEFFECT);

				if (pOp)
				{
					OpLiveEffectParam Param;
					Param.strOpUnique = strCurrentEffectID;
					Param.StackPosition = m_pTool->GetCurrentStackPos();
					Param.pPPStack = m_pTool->GetEffectsStack();
					BOOL bValid = XPEHost::GetEffectDetails(strCurrentEffectID, &Param.strMenuText, &Param.bIsDestructive);

					// We want to delete the effect whether it was valid or not
					pOp->Invoke(&Param);
					return;
				}
			}
		}
	}
}




/********************************************************************************************

>	void LiveEffectsInfoBarOp::GadgetMessage_RemoveAll(DialogMsg* Message)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/10/2004
	Inputs:		Message - A dialog message 
	Returns:	The result of the message
	Purpose:	Takes a look at the message and if it is for me it responds to it.

********************************************************************************************/

void LiveEffectsInfoBarOp::GadgetMessage_RemoveAll(DialogMsg* Message)
{
	switch (Message->DlgMsg)
	{
		case DIM_LFT_BN_CLICKED:
		{
			// Commit changes of any modeless edit session already running
			// and close it down ready for this new one...
			if (XPEHost::IsEditSessionRunning())
			{
				XPEHost::EndEditLiveEffect();		// Could change PPStack!
				m_pTool->UpdatePPStack();
			}

			OpDescriptor* pOp = NULL;
			pOp = OpDescriptor::FindOpDescriptor(OPTOKEN_DELETEALL_LIVEEFFECT);

			if (pOp)
			{
				OpLiveEffectParam Param;
				pOp->Invoke(&Param);
				return;
			}
		}
	}
}




/********************************************************************************************

>	void LiveEffectsInfoBarOp::GadgetMessage_Setup(DialogMsg* Message)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/10/2004
	Inputs:		Message - A dialog message 
	Returns:	The result of the message
	Purpose:	Takes a look at the message and if it is for me it responds to it.

********************************************************************************************/

void LiveEffectsInfoBarOp::GadgetMessage_Setup(DialogMsg* Message)
{
	switch (Message->DlgMsg)
	{
		case DIM_LFT_BN_CLICKED:
		{
			// Commit changes of any modeless edit session already running
			// and close it down ready for this new one...
			if (XPEHost::IsEditSessionRunning())
			{
				XPEHost::EndEditLiveEffect();		// Could change PPStack!
				m_pTool->UpdatePPStack();
			}

			if (ClickModifiers::GetClickModifiers().Adjust)
				XPEEditOp::EditPluginSettings();
			else
			{
				OpDescriptor* pOp = NULL;
				pOp = OpDescriptor::FindOpDescriptor(OPTOKEN_PLUGSDLG);

				String_256 strDesc;
				if (pOp && !pOp->GetOpsState(&strDesc).Greyed)
				{
					pOp->Invoke();
					return;
				}
			}
		}
	}
}




/********************************************************************************************

>	void LiveEffectsInfoBarOp::HandleCreate()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		-
	Returns:	-
	Purpose:	Called when a DIM_CREATE message is received

********************************************************************************************/

void LiveEffectsInfoBarOp::HandleCreate()
{
	// Don't do anything if the Info bar isn't displayed
//	if (!LiveEffectsTool::IsCurrentTool() || !IsVisible()) return;

	if (WindowID!=NULL)
	{
		// Mark the controls as existing
		m_bControlsExist = TRUE;

		// Set the state of the pressure button according to the Op's state.
		// Why buttons on infobars can't dynamically auto-connect to their ops is beyond me
		UpdateState();
	}
}

/********************************************************************************************

>	void LiveEffectsInfoBarOp::SetToolActiveState(BOOL IsToolActive)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		IsToolActive - TRUE if the tool is becoming active, FALSE if it not
	Purpose:	Allows the info bar to know if it is valid or not

********************************************************************************************/

void LiveEffectsInfoBarOp::SetToolActiveState(BOOL IsToolActive)
{
	// Set the control state
	m_bControlsExist = IsToolActive;
}



/********************************************************************************************

>	virtual void LiveEffectsInfoBarOp::UpdateState()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/97

	Purpose:	Sets the state of button on the bar to keep them up to date

********************************************************************************************/

void LiveEffectsInfoBarOp::UpdateState()
{
	// Don't try to set our controls when they're not there!
	if (!m_bControlsExist)
		return;

	if (CCamApp::DisableSys)			// Inside an error handler
		return;

	EffectsStack* pStack = m_pTool->GetEffectsStack();
	INT32 iStackPos = m_pTool->GetCurrentStackPos();

	IXMLDOMDocumentPtr pDoc = XPEHost::GetEditorsList();
	PopulateOrderDropdown(pStack, iStackPos);

	ListRange* pLevelRange = NULL;
	if (pStack)
		pLevelRange = pStack->GetLevelRange(&iStackPos);

	UpdateAddButton(pStack, pLevelRange);
	UpdateEditButton(pStack, pLevelRange);
	PopulateResolutionDropdown(pStack, pLevelRange);
	UpdateLockButton(pLevelRange);
	UpdateRemoveButton(pLevelRange);
	UpdateLockAllButton();
	UpdateRemoveAllButton();
	UpdateSetupButton();
}




/********************************************************************************************

>	void LiveEffectsInfoBarOp::PopulateOrderDropdown(ListRange* pPPStack, iSelectedStackPos)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/10/2004
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
********************************************************************************************/

void LiveEffectsInfoBarOp::PopulateOrderDropdown(EffectsStack* pPPStack, INT32 iSelectedStackPos)
{
	// Don't try to set our controls when they're not there!
	if (!m_bControlsExist)
		return;

	if (pPPStack==NULL)
		return;

	DeleteAllValues(_R(IDC_COMBOBOX_LE_ORDER));
	SetComboListLength(_R(IDC_COMBOBOX_LE_ORDER));
	SetGadgetWritable(_R(IDC_COMBOBOX_LE_ORDER), FALSE);

	if (!pPPStack->bConsistent)
	{
		// There is no consistent set of LiveEffects applied to the selection
		SetStringGadgetValue(_R(IDC_COMBOBOX_LE_ORDER), _R(IDS_LE_MANY));
		SetSelectedValueIndex(_R(IDC_COMBOBOX_LE_ORDER), 0);
	}
	else
	{
		if (pPPStack->IsEmpty())
		{
			// There is no consistent set of LiveEffects applied to the selection
			SetStringGadgetValue(_R(IDC_COMBOBOX_LE_ORDER), _R(IDS_LE_NONE));
			SetSelectedValueIndex(_R(IDC_COMBOBOX_LE_ORDER), 0);
		}
		else
		{
			// This is a consistent set of LiveEffects applied to the selection
			INT32 pos = 0;
			PPStackLevel* pLevel = (PPStackLevel*)pPPStack->GetHead();
			while (pLevel!=NULL)
			{
				String_64 strDisplayName;
				if (pLevel->pPPNode!=NULL && pLevel->pPPNode->IsBitmapEffect())
					strDisplayName = ((NodeBitmapEffect*)(pLevel->pPPNode))->GetDisplayName();
				BOOL bDestructive = FALSE;
				BOOL bFound = XPEHost::GetEffectDetails(pLevel->strPostProID, &strDisplayName, &bDestructive);

				TCHAR Str[64];
				String_64 pjm(_R(IDS_LE_ORDER_FORMAT));
				if (!bFound)
					pjm = String_64(_R(IDS_LE_ORDER_FORMAT_NOTFOUND));
				else if (bDestructive)
					pjm = String_64(_R(IDS_LE_ORDER_FORMAT_DESTRUCTIVE));
				camSprintf(Str, pjm, pos+1, (LPTSTR)strDisplayName);
				String_64 strOrderedName(Str);

				SetStringGadgetValue(_R(IDC_COMBOBOX_LE_ORDER), &strOrderedName, FALSE, pos);

				if (pos==iSelectedStackPos)
					SetSelectedValueIndex(_R(IDC_COMBOBOX_LE_ORDER), pos);

				pos++;
				pLevel = (PPStackLevel*)pPPStack->GetNext(pLevel);
			}
		}
	}
	SetComboListLength(_R(IDC_COMBOBOX_LE_ORDER));
}




/********************************************************************************************

>	void LiveEffectsInfoBarOp::PopulateResolutionDropdown(EffectsStack* pStack, ListRange* pLevelRange, BOOL bUpdateList = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/10/2004
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-

********************************************************************************************/

void LiveEffectsInfoBarOp::PopulateResolutionDropdown(EffectsStack* pStack, ListRange* pLevelRange, BOOL bUpdateList)
{
	// Don't try to set our controls when they're not there!
	if (!m_bControlsExist)
		return;

	if (bUpdateList)
	{
		BuildResDropList(_R(IDC_COMBOBOX_LE_RES), TRUE, FALSE, -1);
		SetGadgetWritable(_R(IDC_COMBOBOX_LE_RES), TRUE);
	}

	if (pLevelRange==NULL)
		return;

	double dRes = -1;								// -1 means "None"
	Node* pNode = pLevelRange->FindFirst();
	BOOL bCanChangeRes = FALSE;
	while (pNode)
	{
		if (pNode->IsBitmapEffect())
		{
			NodeBitmapEffect* pEffect = (NodeBitmapEffect*)pNode;
			bCanChangeRes = bCanChangeRes || pEffect->CanBeUnlocked();
			if (dRes==-1)
				dRes = pEffect->GetPixelsPerInchValue();
			else
			{
				if (dRes != pEffect->GetPixelsPerInchValue())
				{
					dRes = -2;						// -2 means "Many"
					break;
				}
			}
		}

		pNode = pLevelRange->FindNext(pNode);
	}

	switch ((INT32)dRes)
	{
	case -2:	// Many
		// There is no consistent set of LiveEffects applied to the selection
		SetSelectedValueIndex(_R(IDC_COMBOBOX_LE_RES), -1);
		SetStringGadgetValue(_R(IDC_COMBOBOX_LE_RES), _R(IDS_LE_MANY), FALSE, -1);
		EnableGadget(_R(IDC_COMBOBOX_LE_RES), FALSE);
		break;

	case -1:	// None
		// There are no LiveEffects applied to the selection
		SetSelectedValueIndex(_R(IDC_COMBOBOX_LE_RES), -1);
		SetStringGadgetValue(_R(IDC_COMBOBOX_LE_RES), _R(IDS_LE_NONE), FALSE, -1);
		EnableGadget(_R(IDC_COMBOBOX_LE_RES), FALSE);
		break;

	case 0:		// Automatic
		// There is a consistent LiveEffect resolution of 0/automatic applied to the selection
		SetStringGadgetValue(_R(IDC_COMBOBOX_LE_RES), _R(IDS_LE_AUTO), FALSE, -1);
		SetSelectedValueIndex(_R(IDC_COMBOBOX_LE_RES), 0);
		EnableGadget(_R(IDC_COMBOBOX_LE_RES), bCanChangeRes);
		break;

	default:	// Consistent value
		SetResDropListValue(_R(IDC_COMBOBOX_LE_RES), TRUE, (INT32)dRes);
		EnableGadget(_R(IDC_COMBOBOX_LE_RES), bCanChangeRes);
		break;
	}
}




/********************************************************************************************

>	void LiveEffectsInfoBarOp::UpdateLockButton(ListRange* pLevelRange)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/10/2004
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
********************************************************************************************/

void LiveEffectsInfoBarOp::UpdateLockButton(ListRange* pLevelRange)
{
// While the Lock button is hidden from the UI
// we have to avoid trying to use it (I can't find
// a way to test programmatically whether it exists)
return;

	// Don't try to set our controls when they're not there!
	if (!m_bControlsExist)
		return;

	if (pLevelRange==NULL)
		return;

	// Try to find a consistent lock state...
	BOOL bSomeOrAllAreLocked = FALSE;
	BOOL bEnableButton = FALSE;
	Node* pNode = pLevelRange->FindFirst();
	while (pNode)
	{
		if (pNode->IsBitmapEffect())
		{
			NodeBitmapEffect* pEffect = (NodeBitmapEffect*)pNode;
			if (!pEffect->IsFeatherEffect())
			{
				if (pEffect->IsLockedEffect())
				{
					bEnableButton = (bEnableButton || pEffect->CanBeUnlocked());
					bSomeOrAllAreLocked = (bSomeOrAllAreLocked || pEffect->CanBeUnlocked());
					if (bSomeOrAllAreLocked && bEnableButton)
						break;
				}
				else
					bEnableButton = TRUE;
			}
		}

		pNode = pLevelRange->FindNext(pNode);
	}

	m_bLockSwitchState = bSomeOrAllAreLocked;
	SetBoolGadgetSelected(_R(IDC_CCBUTTON_LE_TOGGLELOCK), m_bLockSwitchState);
	EnableGadget(_R(IDC_CCBUTTON_LE_TOGGLELOCK), bEnableButton);
}




/********************************************************************************************

>	void LiveEffectsInfoBarOp::UpdateRemoveButton(ListRange* pLevelRange)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/06/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
********************************************************************************************/

void LiveEffectsInfoBarOp::UpdateRemoveButton(ListRange* pLevelRange)
{
	// Don't try to set our controls when they're not there!
	if (!m_bControlsExist)
		return;

	if (pLevelRange==NULL)
		return;

	BOOL bEnableButton = FALSE;
	Node* pNode = pLevelRange->FindFirst();
	while (pNode)
	{
		if (pNode->IsEffect())
		{
			bEnableButton = TRUE;
			break;
		}

		pNode = pLevelRange->FindNext(pNode);
	}

	EnableGadget(_R(IDC_CCBUTTON_LE_REMOVE), bEnableButton);
}




/********************************************************************************************

>	void LiveEffectsInfoBarOp::UpdateAddButton(EffectsStack* pStack, ListRange* pLevelRange)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/09/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
********************************************************************************************/

void LiveEffectsInfoBarOp::UpdateAddButton(EffectsStack* pStack, ListRange* pLevelRange)
{
	// Don't try to set our controls when they're not there!
	if (!m_bControlsExist)
		return;

	if (pLevelRange==NULL)
		return;

//	BOOL bEnableButton = !XPEHost::IsEditSessionRunning();
	BOOL bEnableButton = (pLevelRange->FindFirst()!=NULL);

	EnableGadget(_R(IDC_CCBUTTON_LE_ADD), bEnableButton);
	EnableGadget(_R(IDC_BUTTON_LE_ADD), bEnableButton);
}




/********************************************************************************************

>	void LiveEffectsInfoBarOp::UpdateAddButton(EffectsStack* pStack, ListRange* pLevelRange)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/09/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
********************************************************************************************/

void LiveEffectsInfoBarOp::UpdateAddButton(EffectsStack* pStack, ListRange* pLevelRange)
{
	// Don't try to set our controls when they're not there!
	if (!m_bControlsExist)
		return;

	if (pLevelRange==NULL)
		return;

//	BOOL bEnableButton = !XPEHost::IsEditSessionRunning();
	BOOL bEnableButton = (pLevelRange->FindFirst()!=NULL);

	EnableGadget(_R(IDC_CCBUTTON_LE_ADD), bEnableButton);
	EnableGadget(_R(IDC_BUTTON_LE_ADD), bEnableButton);
}




/********************************************************************************************

>	void LiveEffectsInfoBarOp::UpdateEditButton(EffectsStack* pStack, ListRange* pLevelRange)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/06/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
********************************************************************************************/

void LiveEffectsInfoBarOp::UpdateEditButton(EffectsStack* pStack, ListRange* pLevelRange)
{
	// Don't try to set our controls when they're not there!
	if (!m_bControlsExist)
		return;

	if (pLevelRange==NULL)
		return;

	BOOL bEnableButton = FALSE;
	if (pStack->bConsistent /*&& !XPEHost::IsEditSessionRunning()*/)
	{
		Node* pNode = pLevelRange->FindFirst();
		while (pNode)
		{
			if (pNode->IsEffect())
			{
				bEnableButton = TRUE;
				break;
			}

			pNode = pLevelRange->FindNext(pNode);
		}
	}

	EnableGadget(_R(IDC_CCBUTTON_LE_EDIT), bEnableButton);
	EnableGadget(_R(IDC_BUTTON_LE_EDIT), bEnableButton);
}




/********************************************************************************************

>	void LiveEffectsInfoBarOp::UpdateLockAllButton()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/10/2004
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
********************************************************************************************/

void LiveEffectsInfoBarOp::UpdateLockAllButton()
{
	// Don't try to set our controls when they're not there!
	if (!m_bControlsExist)
		return;

	SelRange Sel(*(GetApplication()->FindSelection()));
	Sel.SetPromoteToParent(TRUE);
	Node* pNode = Sel.FindFirst();
	Node* pNextNode = NULL;
	BOOL bSomeOrAllAreLocked = FALSE;
	BOOL bEnableButton = FALSE;
	while (pNode && !bSomeOrAllAreLocked && !bEnableButton)
	{
		pNextNode = Sel.FindNext(pNode);

		// We must find all effects in our subtree
		Node* pScanNode = pNode->FindFirstPreorder();
		while (pScanNode)
		{
			if (pScanNode->IsBitmapEffect() && !((NodeBitmapEffect*)pScanNode)->IsFeatherEffect())
			{
				NodeBitmapEffect* pEffect = (NodeBitmapEffect*)pScanNode;
				if (pEffect->IsLockedEffect())
				{
					bEnableButton = (bEnableButton || pEffect->CanBeUnlocked());
					bSomeOrAllAreLocked = (bSomeOrAllAreLocked || pEffect->CanBeUnlocked());
					if (bEnableButton && bSomeOrAllAreLocked)
						break;
				}
				else
					bEnableButton = TRUE;

				// We don't want to lock any effects under this one
				// So find the next node in preorder order outside the scan node
				pScanNode = pScanNode->FindNextPreorder(pNode, TRUE);
			}
			else
				pScanNode = pScanNode->FindNextPreorder(pNode);
		}

		pNode = pNextNode;
	}

	m_bLockAllSwitchState = bSomeOrAllAreLocked;
	SetBoolGadgetSelected(_R(IDC_CCBUTTON_LE_TOGGLELOCKALL), m_bLockAllSwitchState);
	EnableGadget(_R(IDC_CCBUTTON_LE_TOGGLELOCKALL), bEnableButton);
}




/********************************************************************************************

>	void LiveEffectsInfoBarOp::UpdateRemoveAllButton()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/06/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
********************************************************************************************/

void LiveEffectsInfoBarOp::UpdateRemoveAllButton()
{
	// Don't try to set our controls when they're not there!
	if (!m_bControlsExist)
		return;

	SelRange Sel(*(GetApplication()->FindSelection()));
	Node* pNode = Sel.FindFirst();
	BOOL bEnableButton = FALSE;
	ListRange* pStack = NULL;
	while (pNode)
	{
		pStack = EffectsStack::GetEffectsStackFromNode(pNode, TRUE, TRUE);
		if (pStack)
		{
			bEnableButton = TRUE;
			break;
		}

		pNode = Sel.FindNext(pNode);
	}

	if (pStack)
	{
		delete pStack;
		pStack = NULL;
	}

	EnableGadget(_R(IDC_CCBUTTON_LE_REMOVEALL), bEnableButton);
}




/********************************************************************************************

>	void LiveEffectsInfoBarOp::UpdateSetupButton()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/07/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
********************************************************************************************/

void LiveEffectsInfoBarOp::UpdateSetupButton()
{
	// Don't try to set our controls when they're not there!
	if (!m_bControlsExist)
		return;

	OpDescriptor* pOp = NULL;
	pOp = OpDescriptor::FindOpDescriptor(OPTOKEN_PLUGSDLG);

	String_256 strDesc;
	EnableGadget(_R(IDC_BUTTON_LE_SETUP), pOp && !pOp->GetOpsState(&strDesc).Greyed);
}




/********************************************************************************************

>	void LiveEffectsInfoBarOp::SetGadgetWritable(INT32 id, BOOL enable)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Inputs:		id, the 'IDC_?' of the control.
				enable, TRUE to allow the control to be typed into. FALSE to make it
						read only.
	Purpose:	Sets the state of the 'Read Only' flag of an edit field or combo box.
	NOTE!!!		This should be in DialogOp/DialogManager!

********************************************************************************************/

void LiveEffectsInfoBarOp::SetGadgetWritable(INT32 id, BOOL enable)
{
	// Get the window handle of the gadget, from the gadget ID
	HWND gadget = ::GetDlgItem(WindowID, id);

	// See if it's got a child window (it may be a Combo Box)
	HWND hEdit = ::ChildWindowFromPoint(gadget, CPoint(1,1));

	if (hEdit)				// Was there a child window ?
		gadget = hEdit;		// Yes, so send the message to it

 	if (enable)
	{
		::SendMessage(gadget, EM_SETREADONLY, FALSE, 0);	// Clear the Read Only Flag
	}
	else
	{
		::SendMessage(gadget, EM_SETREADONLY, TRUE, 0);		// Set the Read Only Flag
	}
}




