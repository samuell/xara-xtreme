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
// The LiveEffects Tool
// Created by Phil on 27/09/2004

/*
*/

#include "camtypes.h"
#include "ensure.h"
//#include "resource.h"	// _R(IDS_OUTOFMEMORY)
//#include "barsdlgs.h"
//#include "mario.h"
//#include "phil.h"
//#include "markn.h"
//#include "viewrc.h"
#include "docview.h"
#include "document.h"
#include "mainfrm.h"
#include "infobar.h"
#include "csrstack.h"
#include "app.h"
#include "liveeffectsinfo.h"
#include "liveeffectstool.h"
//#include "liveeffectres.h"
#include "opliveeffects.h"
#include "range.h"
#include "module.h"
#include "ezmodule.h"
#include "oilfiles.h"
#include "nodetxts.h"
#include "nodeblnd.h"
#include "ndbldpth.h"
#include "blobs.h"
#include "comattrmsg.h"
#include "xpehost.h"
#include "nodeliveeffect.h"


// Set things up so that the tool will be listed in the Dialog box
DECLARE_SOURCE("$Revision$");


// These are still char* while we wait for resource technology to be developed for modules
char* LiveEffectsTool::FamilyName	= "PostProcessor Tools";
char* LiveEffectsTool::ToolName 	= "Live Effects Tool";
char* LiveEffectsTool::Purpose 		= "Apply live bitmap effects to drawn objects";
char* LiveEffectsTool::Author 		= "Phil";

CC_IMPLEMENT_MEMDUMP( LiveEffectsTool, Tool_v1 )

// Better memory tracking please
#define new CAM_DEBUG_NEW


/********************************************************************************************

>	LiveEffectsTool::LiveEffectsTool()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Dummp Constructor - It does nothing. All the real initialisation is done
				in LiveEffectsTool::Init which is called by the Tool Manager
	SeeAlso:	LiveEffectsTool::Init

********************************************************************************************/

LiveEffectsTool::LiveEffectsTool()
{
	// No info bar or previous path by default
	m_pInfoBarOp = NULL;

	// Set the cursor pointers to null
	pNormalCursor = NULL;
	pActiveCursor = NULL;

	// Make sure that the status line text is a valid string
	StatusMsg = String_256("");

	m_pPPStack = NULL;
	m_strCurrentEffectID = String_256("");
	m_iCurrentStackPos = 0;
}



/********************************************************************************************

>	LiveEffectsTool::~LiveEffectsTool()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Destructor (Virtual). Does nothing.

********************************************************************************************/

LiveEffectsTool::~LiveEffectsTool()
{
	ScreenChanged(FALSE);
}



/********************************************************************************************

>	BOOL LiveEffectsTool::Init( INT32 Pass )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Returns:	FALSE if it does not want to be created, TRUE otherwise
	Purpose:	Used to check if the Tool was properly constructed
	SeeAlso:	LiveEffectsTool::LiveEffectsTool

********************************************************************************************/

BOOL LiveEffectsTool::Init()
{
	// This should be set to NULL by default. It will be set properly below, if
	// everthing is working as it should
	m_pInfoBarOp = NULL;

	// Now we have to declare all our operations and if that works, try to find
	// the liveeffects tools info bar and create it
	if (OpLiveEffect::Init())
	{
		// Resource File and Object that creates FreeHandInfoBarOp objects
		CCResTextFile 			ResFile;
		LiveEffectsInfoBarOpCreate BarCreate;

		if (ResFile.open(_R(IDM_LIVEEFFECTS_BAR), _R(IDT_INFO_BAR_RES)))
		{
			// Found the file and opened it
			if (DialogBarOp::ReadBarsFromFile(ResFile,BarCreate))
			{
				// read it in ok, so close it
				ResFile.close();

				// Info bar now exists.  Now get a pointer to it
				String_32 str = String_32(_R(IDS_LIVEEFFECTS_INFOBARNAME));
				DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(str);

				// Should have a dialog bar op by now
				if (pDialogBarOp != NULL)
				{
					// Make sure it is what we were expecting and set it
				 	if (pDialogBarOp->IsKindOf(CC_RUNTIME_CLASS(LiveEffectsInfoBarOp)))
					{
						m_pInfoBarOp = (LiveEffectsInfoBarOp*) pDialogBarOp;
						m_pInfoBarOp->m_pTool = this;
					}
				}
			}
		}
	}

	// See if it all worked and return depending on the result
	ENSURE(m_pInfoBarOp!=NULL, "Failed to create LiveEffects Info Bar" );
	if (m_pInfoBarOp==NULL)
		return FALSE;
	else
		return TRUE;
}





/********************************************************************************************

>	void LiveEffectsTool::Describe(void *InfoPtr)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		InfoPtr - A pointer to a tool info block. It is passed cast to void* as
				the version of the tool is unknown at this point. Later versions of the
				Tool class may have more items in this block, that this tool will not use
	Outputs:	InfoPtr - The structure pointed to by InfoPtr will have had all the info
				that this version of the Tool knows about
	Purpose:	Allows the tool manager to extract information about the tool

********************************************************************************************/

void LiveEffectsTool::Describe(void *InfoPtr)
{
	// Cast structure into the latest one we understand.
	ToolInfo_v1 *Info = (ToolInfo_v1 *) InfoPtr;

	Info->InfoVersion = 1;
	Info->InterfaceVersion = GetToolInterfaceVersion();  // You should always have this line.
		
	// These are all arbitrary at present.
	Info->Version = 1;
	Info->ID      = GetID();
	Info->TextID  = _R(IDS_LIVEEFFECT_TOOL);
	Info->BubbleID = _R(IDBBL_LIVEEFFECT_TOOLBOX);

	Info->Family  = FamilyName;
	Info->Name    = ToolName;
	Info->Purpose = Purpose;
	Info->Author  = Author;

	Info->InfoBarDialog = _R(IDD_LIVEEFFECTSTOOL);
}


UINT32 LiveEffectsTool::GetID()
{
	return TOOLID_LIVEEFFECT;
}



// The LiveEffects Tools EventHandlers

/********************************************************************************************

>	virtual void LiveEffectsTool::SelectChange(BOOL isSelected)
	
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/93
	Inputs:		IsSelected - TRUE if the tool is becoming selected, FALSE if it is loosing
				the selection
	Purpose:	Called when the tool is selected or deselected.  Creates and pushes the
				tool's cursor; pops and destroys it.
	Errors:		Sends warning to debugging terminal if creating the cursor fails.

********************************************************************************************/

void LiveEffectsTool::SelectChange(BOOL isSelected)
{
// Stub out this function if the tool isn't wanted
#ifndef NO_ADVANCED_TOOLS		

	if (isSelected)
	{
//MessageBox(NULL, "SelectChange 1", "Debug", MB_OK);
		// Load the cursors
		if (!LoadCursors())
			InformError();

		// Update the list of plug-ins
//MessageBox(NULL, "SelectChange 2", "Debug", MB_OK);
		XPEHost::UpdateEditorsList();
//MessageBox(NULL, "SelectChange 3", "Debug", MB_OK);
SelRange* pSelRange = GetApplication()->FindSelection();
if (pSelRange) pSelRange->MakePartialSelectionWhole(TRUE, FALSE, TRUE);
		ScreenChanged(FALSE);
//MessageBox(NULL, "SelectChange 4", "Debug", MB_OK);
		
		// Find the blob manager ready for later
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();

		// Create and display my info bar please
		if (m_pInfoBarOp != NULL)
		{
			m_pInfoBarOp->Create();
			m_pInfoBarOp->SetToolActiveState(TRUE);
		}

		// Make sure that Objects blobs are on
		if (BlobMgr != NULL)
		{
			// Decide which blobs to display
			BlobStyle MyBlobs;
			MyBlobs.Object = TRUE;

			// tell the blob manager
			BlobMgr->ToolInterest(MyBlobs);
		}
//MessageBox(NULL, "SelectChange 5", "Debug", MB_OK);

	}
	else
	{
		// Deselection of the tool
		// Get rid of all the tools cursors
		RemoveCursors();

		// Close any open XPE editing
		XPEHost::EndEditLiveEffect();

		// Hide and destroy my info bar please
		if (m_pInfoBarOp != NULL)
		{
			m_pInfoBarOp->SetToolActiveState(FALSE);
			
			// Close any open dialogs related to the info bar here

			m_pInfoBarOp->Delete();
		}

		// ensure that the colour picker is working
//		SetColourEditorProcessing(TRUE);

		// ensure any tool object blobs are removed.
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		if (BlobMgr != NULL)
		{
			BlobStyle bsRemoves;
			bsRemoves.ToolObject = TRUE;
			BlobMgr->RemoveInterest(bsRemoves);
		}
	}
#endif	// NO_ADVANCED_TOOLS
}



/********************************************************************************************

>	BOOL LiveEffectsTool::LoadCursors()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/94
	Returns:	TRUE if it worked, FALSE if it did not
	Purpose:	Loads all the cursors used by the freehand tool and pushs the normal
				one onto the top of the cursor stack
	Errors:		ERROR1 (_R(IDE_FHAND_BADCURSORS)) if it fails to load the cursors

********************************************************************************************/

BOOL LiveEffectsTool::LoadCursors()
{
	// This tool has just been selected, so it is not displaying a cursor
	pActiveCursor = NULL;

	// Try to create all our cursors - The normal cursor
	pNormalCursor = new Cursor(this, _R(IDC_LIVEEFFECTSTOOLCURSOR));

	// did we get all the cursors ok
	if ((pNormalCursor==NULL) || (!pNormalCursor->IsValid()))
	{
		// No, at least one of them was NULL, so delete them all
		// Deleting the null cursor will be ok
		delete pNormalCursor;

		// and ensure that the pointers are set to NULL
		pNormalCursor = NULL;

		// and set an error and return
		ERROR1(FALSE, _R(IDE_LIVEEFFECT_BADCURSORS));
	}

	// All the cursors loaded ok if we got to here
	// So push the normal cursor onto the stack and mark it as active
	CurrentCursorID = CursorStack::GPush(pNormalCursor, FALSE);
	pActiveCursor = pNormalCursor;

	// return that it worked
	return TRUE;
}



/********************************************************************************************

>	void LiveEffectsTool::RemoveCursors()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/94
	Purpose:	Pops the freehand tools cursor off the top of the cursor stack, frees the
				memory used by all the cursors in the freehand tool and sets all the pointers
				to NULL.
	SeeAlso:	LiveEffectsTool::LoadCursors

********************************************************************************************/

void LiveEffectsTool::RemoveCursors()
{
	// If we ever had a cursor
	if (pActiveCursor!=NULL)
	{
		// pop it off the stack
		CursorStack::GPop(CurrentCursorID);

		// and free up the cursors we allocated
		delete pNormalCursor;

		// and set them all to NULL
		pNormalCursor = NULL;
		pActiveCursor = NULL;
		CurrentCursorID = 0;
	}
}




/********************************************************************************************

>	void LiveEffectsTool::ChangeCursor(Cursor* cursor)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/3/94
	Inputs:		ID of the cursor you want to flip to
	Purpose:	Changes to the specified cursor. Will only change the cursor if it isn't already
				this cursor, so it doesn't flicker.
	Errors:		can fail if the cursor cannot be created - the cursor code will fail.

********************************************************************************************/

void LiveEffectsTool::ChangeCursor(Cursor* pCursor)
{
	// only change if this cursor is different from the current cursor
	if ((pCursor!=pActiveCursor) && (pCursor!=NULL))
	{
		// set this cursor as the current cursor and immediately display it
		CursorStack::GSetTop(pCursor, CurrentCursorID);

		// remember this is our current cursor
		pActiveCursor = pCursor;
	}
}






/********************************************************************************************

>	void LiveEffectsTool::OnClick( DocCoord PointerPos, ClickType Click, 
								ClickModifiers ClickMods, Spread *pSpread )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		PointerPos - The Coords (in spread coords) of the point where the mouse 
				button was clicked
				Click -	Describes the type of click that was detected. 
				ClickMods -	Indicates which buttons caused the click and which modifers were
				pressed at the same time
				pSpread - the spread in which the click happened
	Returns:	TRUE if it handled the Click, FALSE otherwise
	Purpose:	To handle a Mouse Click event for the FreeHand Tool. It starts up a FreeHand 
				Operation.
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers

********************************************************************************************/

void LiveEffectsTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )
{
// Stub out this function if the tool isn't wanted
#ifndef NO_ADVANCED_TOOLS		

	if (ClickMods.Menu) return;							// Don't do anything if the user clicked the Menu button

	// See if there is already a drag going on
	if (Operation::GetCurrentDragOp()!=NULL)
		return;

	// If we get a click, treat it like a mouse move, allowing the tool to set up
	// any info that is passed to the operation in the drag
	// NEW 17/4/2000 Now we also want to click-select in the Freehand tool
	if (Click==CLICKTYPE_SINGLE)
	{
		OnMouseMove(PointerPos, pSpread, ClickMods);
		DragTool::OnClick (PointerPos, Click, ClickMods, pSpread);
	}

	if (Click == CLICKTYPE_UP)
	{
		// so long as we are not interfering with any drawing operations we will
		// try and change the selection
		//if (JoinInfo.pJoinPath == NULL)
		{
			DragTool::OnClick (PointerPos, Click, ClickMods, pSpread);
SelRange* pSelRange = GetApplication()->FindSelection();
if (pSelRange) pSelRange->MakePartialSelectionWhole(TRUE, FALSE, TRUE);

			BROADCAST_TO_ALL(CommonAttrsChangedMsg);
		}
	}
#endif	// NO_ADVANCED_TOOLS
}




/********************************************************************************************

>	void LiveEffectsTool::OnMouseMove(DocCoord* coord, Spread* pSpread, ClickModifiers mods)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		coordinate of mouse move, pointer to spread containing coord; click modifiers
	Purpose:	This routine is called whenever the mouse moves while we're in the freehand 
				tool. it sees what is under the pointer, and flips the cursor if clicking 
				will have a different effect. The rules are:

				Over space, no selection		- freehand cursor
				Over selected endpoint			- add freehand to line

********************************************************************************************/

void LiveEffectsTool::OnMouseMove(DocCoord Coord, Spread* pSpread, ClickModifiers mods)
{
// Stub out this function if the tool isn't wanted
#ifndef NO_ADVANCED_TOOLS		

	// We are interested in any selected paths that the cursor is over
	// we will also be needing the current docview
	SelRange* Selected = GetApplication()->FindSelection();
	Node* pNode = Selected->FindFirst();

	// Check to see if the selection is on the same spread as the mouse
	if (pNode!=NULL)
	{
		// Get the spread and return if it is different from the one with the cursor
		Spread* pNodeSpread = pNode->FindParentSpread();
		if (pNodeSpread!=pSpread)
			return;
	}

	// Find the Blob manager, so we can find out how big a rect to use
	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr==NULL)
		return;
	
	// Find the Rect round the mouse pos that counts as a hit
	DocRect BlobRect;
	pBlobMgr->GetBlobRect(Coord, &BlobRect);
	
	// Work out the square of the distance that we will count as 'close' to the line
	INT32 Range = BlobRect.Width() / 2;
	Range *= Range;

//	// loop through the selection
//	while (pNode!=NULL)
//	{
//		// Now find the next selected node
//		pNode = Selected->FindNext(pNode);
//	}

	// We did not find anything good, so set the cursor to the normal one
	ChangeCursor(pNormalCursor);

	// And set the status bar text
	StatusMsg.Load(_R(IDS_LIVEEFFECTSTART), Tool::GetModuleID(GetID()));
	GetApplication()->UpdateStatusBarText(&StatusMsg);

#endif	// NO_ADVANCED_TOOLS
}



/********************************************************************************************

>	BOOL LiveEffectsTool::GetStatusLineText(String_256* ptext, Spread*, DocCoord, ClickModifiers)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/1/95
	Inputs:		Others not used
	Outputs:	pText - pointer to a string that will hold the text
	Returns:	TRUE if it set the text, FALSE if not
	Purpose:	Sets the status line text on request from the app.

********************************************************************************************/

BOOL LiveEffectsTool::GetStatusLineText(String_256* ptext, Spread*, DocCoord, ClickModifiers)
{
	*ptext = StatusMsg;
	return TRUE;
}




/********************************************************************************************

>	void LiveEffectsTool::ScreenChanged()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/10/2004
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Essentially - clear your cached info because it may be out of date

********************************************************************************************/

void LiveEffectsTool::ScreenChanged(BOOL WipeRegion)
{
	// Get rid of our record of common LiveEffects in the stack above the selection
	if (m_pPPStack)
	{
		delete m_pPPStack;
		m_pPPStack = NULL;
	}

//	m_iCurrentStackPos = 0;
}




/********************************************************************************************

>	EffectsStack* LiveEffectsTool::GetEffectsStack()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/10/2004
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Allow info bar to get at LE stack whenever it needs to

********************************************************************************************/

EffectsStack* LiveEffectsTool::GetEffectsStack() const
{
	return m_pPPStack;
}




/********************************************************************************************

>	BOOL LiveEffectsTool::UpdatePPStack()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/10/2004
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Get a list of LE nodes that are common to all selected nodes
				Only returns a list of ALL the selected nodes have identical stacks
				above them...

********************************************************************************************/

BOOL LiveEffectsTool::UpdatePPStack(BOOL bUCE)
{
	if (m_pPPStack!=NULL)
		return FALSE;

	m_pPPStack = EffectsStack::GetEffectsStackFromSelection();

	BOOL bOK;

	// If we know what type of effect we were last editing, try to find that type again
	if (!m_strCurrentEffectID.IsEmpty())
	{
		bOK = m_pPPStack->FindBestProcessor(&m_strCurrentEffectID, &m_iCurrentStackPos);
		if (!bOK)
		{
			m_strCurrentEffectID.Empty();
			m_iCurrentStackPos = STACKPOS_TOP;
		}
	}

	// If we don't have a current effect set for the current stack then try to
	// find the topmost effect
	if (m_strCurrentEffectID.IsEmpty())
	{
		m_iCurrentStackPos = STACKPOS_TOP;
		bOK = m_pPPStack->GetLevelInfo(&m_strCurrentEffectID, &m_iCurrentStackPos);
		if (!bOK)
		{
			m_strCurrentEffectID.Empty();
			m_iCurrentStackPos = STACKPOS_TOP;
		}
	}

	// Tell any running XPE editor that things have changed
	if (bUCE)
		OpLiveEffect::UpdateCurrentEditor();

	// Safety net: UpdateCurrentEditor should not leave m_pPPStack NULL!
	ERROR3IF(m_pPPStack==NULL, "Effect tool's effect stack is NULL in UpdatePPStack!");
	if (m_pPPStack==NULL)
		m_pPPStack = EffectsStack::GetEffectsStackFromSelection();

	// Safety net: UpdateCurrentEditor should not leave m_pPPStack NULL!
	ERROR3IF(m_pPPStack==NULL, "Effect tool's effect stack is NULL in UpdatePPStack!");
	if (m_pPPStack==NULL)
		m_pPPStack = EffectsStack::GetEffectsStackFromSelection();

	return TRUE;
}




/********************************************************************************************

>	BOOL LiveEffectsTool::OnIdle()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/10/2004
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if we don't need any more idles
				FALSE if we expect idle calls to continue
	Purpose:	Ensure UI is up to date

********************************************************************************************/

BOOL LiveEffectsTool::OnIdle()
{
	// just check that we have a document
	if (Document::GetCurrent() == NULL)
		return FALSE;

	// If any of our cached information about the selection is out of date
	// update it and reflect those changes in the Infobar
	BOOL bChanged = UpdatePPStack();
	if (bChanged && m_pInfoBarOp!=NULL)
	{
//		m_pInfoBarOp->PopulateOrderDropdown(m_pPPStack, m_iCurrentStackPos);

//		INT32 iPos = m_iCurrentStackPos;
//		ListRange* pLevelRange = NULL;
//		if (m_pPPStack)
//			pLevelRange = m_pPPStack->GetLevelRange(&iPos);
//		m_pInfoBarOp->PopulateResolutionDropdown(pLevelRange);
//		m_pInfoBarOp->UpdateLockButton(pLevelRange);
//		m_pInfoBarOp->UpdateLockAllButton();
		m_pInfoBarOp->UpdateState();

//		IXMLDOMDocumentPtr pDoc = XPEHost::GetEditorsList();
//		m_pInfoBarOp->PopulateTypeDropdown(pDoc, m_strCurrentEffectName); // overkill - we only want to update the selected item
	}

	return FALSE;
}




IMPLEMENT_SIMPLE_MODULE( LiveEffectModule, MODULEID_LIVEFFECT, LiveEffectsTool, 
							"Live Effect Tool", "To do bitmap things to drawn objects", "Phil" );





