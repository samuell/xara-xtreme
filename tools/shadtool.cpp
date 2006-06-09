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
// Implementation of the Shadow Tool

/*
*/

#include "camtypes.h"
#include "shadtool.h"			// Shadow Tool header
#include "nodecont.h"
//#include "oilkeys.h"
#include "keypress.h"
#include "vkextra.h"

#ifdef BUILDSHADOWS

// code headers
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "blobs.h"
#include "csrstack.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "oilfiles.h"
#include "opshadow.h"
#include "shadinfo.h"
#include "toollist.h"
#include "layer.h"
#include "layermsg.h"
#include "nodeshad.h"

// resource headers
//#include "markn.h"
//#include "shadbar.h"
//#include "shadres.h"
//#include "viewrc.h"
#include "opbevel.h"
//#include "biasres.h"			// bias profile header
#include "objchge.h"
#include "bubbleid.h"
//#include "mario.h"
#include "nodetxts.h"
#include "nodeblnd.h"
#include "nodebldr.h"
#include "slicehelper.h"
#include "opliveeffects.h"
#include "liveeffectstool.h"
#include "ophist.h"
#include "shadinfo.h"

DECLARE_SOURCE( "$Revision$" );

CC_IMPLEMENT_MEMDUMP(SoftShadowTool,Tool_v1)
CC_IMPLEMENT_DYNCREATE(OpDragCreateShadow2, SelOperation)
CC_IMPLEMENT_DYNCREATE(SaveShadowSettingsAction, Action)

// Must come after the last CC_IMPLEMENT.. macro
#define new CAM_DEBUG_NEW     

// default shadow colour is black.
#define DEFAULT_SHADOW_COLOUR	COLOUR_BLACK

// These are still char* while we wait for resource technology to be developed for modules
TCHAR* SoftShadowTool::FamilyName	= _T("Shadow Tools");
TCHAR* SoftShadowTool::ToolName 	= _T("Shadow Tool");
TCHAR* SoftShadowTool::Purpose 		= _T("Shadow manipulation");
TCHAR* SoftShadowTool::Author 		= _T("DavidMc");

// Init those other useful static vars
BOOL					SoftShadowTool::CurrentTool 			= FALSE;
SoftShadowInfoBarOp*	SoftShadowTool::pSoftShadowInfoBarOp	= NULL;
UINT32					SoftShadowTool::StatusID				= _R(IDS_SOFTSHADOWSTATUS_FINDSTART);



/********************************************************************************************

>	SoftShadowTool::SoftShadowTool()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Default Constructor.
				Other initialisation is done in SoftShadowTool::Init which is called by the Tool Manager
	SeeAlso:	SoftShadowTool::Init

********************************************************************************************/
SoftShadowTool::SoftShadowTool()
{
	pcCurrentCursor = NULL;
	m_bBlobRendering = TRUE;
	pLastBlobSpread = NULL;
	m_iEditStackPos = STACKPOS_TOP;
	m_pEditRange = NULL;
}



/********************************************************************************************

>	SoftShadowTool::~SoftShadowTool()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Destructor (Virtual). Does nothing.

********************************************************************************************/
SoftShadowTool::~SoftShadowTool()
{
	if (m_pEditRange)
	{
		delete m_pEditRange;
		m_pEditRange = NULL;
	}
}



/********************************************************************************************

>	BOOL SoftShadowTool::Init( INT32 Pass )

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	FALSE if it does not want to be created, TRUE otherwise
	Purpose:	Used to check if the Tool was properly constructed
	SeeAlso:	SoftShadowTool::SoftShadowTool

********************************************************************************************/
BOOL SoftShadowTool::Init()
{
	// Declare all your ops here and only succeed if all declarations succeed
	BOOL ok = TRUE;
	if (ok) 
		ok = (	OpApplyShadow::Declare() && 
				OpChangeShadowType::Declare() && 
				OpRemoveShadow::Declare() && 
				OpShadowPosition::Declare() && 
				OpShadowPenumbra::Declare() &&
				OpShadowAngle::Declare() &&
				OpShadowHeight::Declare() &&
				OpShadowScale::Declare() &&
				OpShadowHeightAndAngle::Declare() &&
				OpSaveShadowData::Declare() &&
				OpGlowWidth::Declare() &&
				OpChangeShadowProfile::Declare() &&
				OpChangeShadowDarkness::Declare() &&
				OpDragCreateShadow2::Declare());


	// This section reads in the infobar definition and creates an instance of
	// SoftShadowInfoBarOp.  Also pSoftShadowInfoBarOp, the ptr to the tool's infobar, is set up
	// after the infobar is successfully read and created.
	if (ok)
	{
		pSoftShadowInfoBarOp = new SoftShadowInfoBarOp;
		ERROR2IF(pSoftShadowInfoBarOp==NULL, FALSE, "Can't create Shadow tool Infobar");
		if (pSoftShadowInfoBarOp)
			pSoftShadowInfoBarOp->pSoftShadowTool = this;

/*		CCResTextFile 		file;				// Resource File
		SoftShadowInfoBarOpCreate BarCreate;			// Object that creates BlankInfoBarOp objects

		 		ok = file.open(_R(IDM_SOFTSHADOW_BAR), _R(IDT_INFO_BAR_RES));		// Open resource
		if (ok) ok = DialogBarOp::ReadBarsFromFile(file,BarCreate);	// Read and create info bar
		if (ok) file.close();									 	// Close resource

		ERROR3IF(!ok,"Unable to load shadbar.ini from resource\n"); 

		if (ok)
		{
			// Info bar now exists.  Now get a pointer to it
			String_32 str = String_32(_R(IDS_SHADOWTOOL_INFOBARNAME));
			DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(str);
			ok = (pDialogBarOp != NULL);

			if (ok)
				ok = pDialogBarOp->IsKindOf(CC_RUNTIME_CLASS(SoftShadowInfoBarOp));

			if (ok)
			{
				pSoftShadowInfoBarOp = (SoftShadowInfoBarOp*)pDialogBarOp;
				pSoftShadowInfoBarOp->pSoftShadowTool = this;
			}
		}
		
		ERROR3IF(!ok,"Error finding the shadow tool info bar");
*/	}

	return (ok);
}



/********************************************************************************************

>	void SoftShadowTool::Describe(void *InfoPtr)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		InfoPtr - 	A pointer to a tool info block. It is passed cast to void* as
							the version of the tool is unknown at this point. Later versions 
							of the Tool class may have more items in this block, that this 
							tool will not use
	Outputs:	InfoPtr - 	The structure pointed to by InfoPtr will have had all the info
							that this version of the Tool knows about
	Purpose:	Allows the tool manager to extract information about the tool

********************************************************************************************/
void SoftShadowTool::Describe(void *InfoPtr)
{
	// Cast structure into the latest one we understand.
	ToolInfo_v1 *Info = (ToolInfo_v1 *) InfoPtr;

	Info->InfoVersion = 1;
	
	Info->InterfaceVersion = GetToolInterfaceVersion();  // You should always have this line.
		
	// These are all arbitrary at present.
	Info->Version = 1;
	Info->ID      = GetID();
	Info->TextID  = _R(IDS_SOFTSHADOW_TOOL);

	Info->Family  = FamilyName;
	Info->Name    = ToolName;
	Info->Purpose = Purpose;
	Info->Author  = Author;

	Info->BubbleID = _R(IDBBL_SOFTSHADOW_TOOLBOX);
}



/********************************************************************************************

>	virtual void SoftShadowTool::SelectChange(BOOL isSelected)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		isSelected	- TRUE  = tool has been selected
							- FALSE = tool has been deselected
	Outputs:	-
	Returns:	-
	Purpose:	Starts up and closes down the blank tool
	Errors:		Debug warning if creating the cursor fails.
	SeeAlso:	-

********************************************************************************************/
void SoftShadowTool::SelectChange(BOOL isSelected)
{
	if (isSelected)
	{
		if (!CreateCursors()) return;
		CurrentCursorID = CursorStack::GPush(pcNormalSoftShadowCursor, FALSE);		// Push cursor but don't display now
		pcCurrentCursor = pcNormalSoftShadowCursor;

		// This tool is now the current one
		CurrentTool = TRUE;
		SetShadowEditRangeFromSelection();

		// Create and display the tool's info bar
		pSoftShadowInfoBarOp->Create();

		if (pSoftShadowInfoBarOp->pSoftShadowInfoBar)
		{
			// Update the InfoBar 
			pSoftShadowInfoBarOp->UpdateInfoBar();
		}

		// Say which blobs should be displayed
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		if (BlobMgr != NULL)
		{
			// Decide which blobs we will display and tell the blob manager
			BlobStyle MyBlobs;
			MyBlobs.Tiny = TRUE;
			BlobMgr->ToolInterest(MyBlobs);
		}

		RenderToolBlobs(Document::GetSelectedSpread(), NULL);
	}
	else
	{
		// Deselection - destroy the tool's cursors, if they exist.
		if (pcCurrentCursor != NULL)
		{
			CursorStack::GPop(CurrentCursorID);
			pcCurrentCursor = NULL;
			CurrentCursorID = 0;
		}
		DestroyCursors();

PORTNOTE("BiasGain", "Removed use of CBiasGainGadget in SoftShadowInfoBarOp")
#if !defined(EXCLUDE_FROM_XARALX)
		pSoftShadowInfoBarOp->CloseProfileDialog (pSoftShadowInfoBarOp->BiasGainGadget_m);
#endif

		// Remove the info bar from view by deleting the actual underlying window
		pSoftShadowInfoBarOp->Delete();

		// ensure any tool object blobs are removed.
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		if (BlobMgr != NULL)
		{
			BlobStyle bsRemoves;
			bsRemoves.ToolObject = TRUE;
			BlobMgr->RemoveInterest(bsRemoves);
		}

		// No longer the current tool
		CurrentTool = FALSE;

		RenderToolBlobs(Document::GetSelectedSpread(), NULL);
	}
}



/********************************************************************************************

>	BOOL SoftShadowTool::CreateCursors()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if all the shadow tool cursors have been successfully created
	Purpose:	Creates all the shadow tool cursors
	SeeAlso:	-

********************************************************************************************/
BOOL SoftShadowTool::CreateCursors()
{
	// This tool has just been selected.  Create the cursors.
	pcNormalSoftShadowCursor = new Cursor(this, _R(IDC_SOFTSHADOWTOOLCURSOR));
	pcBlobSoftShadowCursor = new Cursor(this, _R(IDC_SOFTSHADOWBLOBCURSOR));

	if ( pcNormalSoftShadowCursor==NULL || !pcNormalSoftShadowCursor->IsValid() || 
		pcBlobSoftShadowCursor==NULL || !pcBlobSoftShadowCursor->IsValid())
	{
		DestroyCursors();
		return FALSE;
	}
	else
		return TRUE;
}



/********************************************************************************************

>	void SoftShadowTool::DestroyCursors()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroys all the Shadow Tool cursors
	SeeAlso:	-

********************************************************************************************/
void SoftShadowTool::DestroyCursors()
{
	if (pcNormalSoftShadowCursor != NULL) delete pcNormalSoftShadowCursor;
	if (pcBlobSoftShadowCursor != NULL) delete pcBlobSoftShadowCursor ;
}



/********************************************************************************************

>	void SoftShadowTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/06/2000
	Inputs:		PointerPos		DocCoord of the point where the mouse button was clicked.
				Click			The type of click that was detected.
				ClickMods		Any modifiers on the click, eg CTRL/SHIFT held down.
				pSpread			Spread in which the click happened.
	Returns:	
	Purpose:	The Shadow tool's mouse-click handler.
	SeeAlso:	Tool::MouseClick;	ClickType;	ClickModifiers

********************************************************************************************/
void SoftShadowTool::OnClick(	DocCoord PointerPos,	ClickType cType,
								ClickModifiers cMods,	Spread* pSpread	)
{
	if (cMods.Menu) return;							// Don't do anything if the user clicked the Menu button

	switch (cType)
	{
		// if a drag is performed, then we have an Op especially for this contingency :o)
		case CLICKTYPE_DRAG:
		{
			OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_DRAGCREATESHADOW);
			DragCreateShadowParam DragParam(this, pSoftShadowInfoBarOp, PointerPos, cMods);
			if (pOpDesc != NULL)
				pOpDesc->Invoke(&DragParam);
		}
		break;
		default:
		{
			// call the base class ....

			DragTool::OnClick (PointerPos, cType, cMods, pSpread);
		}
		break;
	}
}



/********************************************************************************************

>	void SoftShadowTool::OnMouseMove( DocCoord PointerPos,Spread* pSpread, ClickModifiers ClickMod )

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		PointerPos 	- 	The DocCoord of the point where the mouse has moved to
				pSpread 	- 	The spread in which the move occurred
				ClickMods	- 	The state of the various modifiers at the time of the mouse move
	Returns:	TRUE if it handled the Click, FALSE otherwise
	Purpose:	To handle a Mouse Move event for the Blank Tool.

				This method lets the tool provide user feedback by changing status line text
				and the mouse cursor, depending on where the mouse is and what modifiers
				the user is pressing at the mo'.

	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers

********************************************************************************************/
void SoftShadowTool::OnMouseMove(DocCoord PointerPos, Spread* pSpread, ClickModifiers ClickMods)
{
	String_256 strFeedback;
	BOOL bShadowingOn = FigureUserFeedback(&strFeedback, pSpread, PointerPos, ClickMods);

	// we've found something we can drag a shadow from.
	if (bShadowingOn)
	{
		if (pcCurrentCursor != pcBlobSoftShadowCursor)
		{
			CursorStack::GSetTop(pcBlobSoftShadowCursor, CurrentCursorID);
			pcCurrentCursor = pcBlobSoftShadowCursor;
		}
	}

	// no dragging here, sonny.
	else
	{
		if (pcCurrentCursor != pcNormalSoftShadowCursor)
		{
			CursorStack::GSetTop(pcNormalSoftShadowCursor, CurrentCursorID);
			pcCurrentCursor = pcNormalSoftShadowCursor;
		}
	}

	// make sure the status line is up to date.
	GetApplication()->UpdateStatusBarText(&strFeedback);
}



/********************************************************************************************

>	virtual BOOL SoftShadowTool::GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		ptest - pointer to a string
				pSpread points to a spread
				DocPos points to a point in a document
				ClickMods are the current click modifiers
	Outputs:	Updates the string in ptext
	Returns:	TRUE if the string was updates (FALSE if not updated)
	Purpose:	Returns the current status line help string
	SeeAlso:	SoftShadowTool::GetCurrentStatusText, Tool_v1::GetStatusLineText

********************************************************************************************/
BOOL SoftShadowTool::GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)
{
	// We can call the underlying help function to get a string and return the result.
	String_256 strFeedback;
	FigureUserFeedback(&strFeedback, pSpread, DocPos, ClickMods);

	*ptext += strFeedback;
	return TRUE;
}



/********************************************************************************************

>	BOOL SoftShadowTool::FigureUserFeedback(String_256* pText,		Spread* pSpread,
											const DocCoord& dcPos,	ClickModifiers cMods)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31 August 2000

	Returns:	TRUE if the mouse is in a position where the shadow tool can be used,
				FALSE if the mouse is in a position where the tool cannot be used.

	Purpose:	Determines our status line feedback text, dependent on the mouse position,
				click modifiers etc.

********************************************************************************************/
BOOL SoftShadowTool::FigureUserFeedback(String_256* pText,		Spread* pSpread,
										const DocCoord& dcPos,	ClickModifiers cMods)
{
	NodeRenderableInk* pTargetInk = NodeRenderableInk::FindSimpleAtPoint(pSpread, dcPos);

	if (pTargetInk == NULL)
	{
		// There's nothing under the pointer!
		// "Move pointer over object to shadow"
		pText->Empty();
		pText->Load(_R(IDS_SHADOWSTATUS_MOVEPOINTER));
		return FALSE;
	}

	// Find out whether the clicked item relates to a shadow in the postprocessor stack
//	Range* pTargetRange = NULL;
//	INT32 iPos = STACKPOS_TOP;
//	BOOL bTopLevel = FALSE;

	// if we're over an existing shadow, tell the user what result dragging will have.
	// "Drag to position wall shadows"
	// "Drag to change angle and height of floor shadows"
	// "Drag left/right or up/down to change width of glow shadows"
	if (pTargetInk->IsAShadow())
	{
		NodeShadowController* pControl = (NodeShadowController*)((NodeShadow*)pTargetInk)->GetParentController();
		if (pControl != NULL)
		{
			ShadowType sType = pControl->GetShadowType();
			UINT32 StatusID = 0;
			StatusID =	(sType == SHADOWTYPE_WALL)	? _R(IDS_SHADOWSTATUS_DRAGWALLSHADOWS) :
						(sType == SHADOWTYPE_FLOOR)	? _R(IDS_SHADOWSTATUS_DRAGFLOORSHADOWS) :
						(sType == SHADOWTYPE_GLOW)	? _R(IDS_SHADOWSTATUS_DRAGGLOWSHADOWS) :
						0;
			pText->Empty();
			if (StatusID != 0)
				pText->Load(StatusID);
			else
				ERROR3("Unrecognised shadow type!");
		}
		else
		{
			ERROR3("NodeShadow found without a NodeShadowController!");
		}

		return TRUE;
	}

	pText->Empty();
	pText->Load(_R(IDS_SHADOWSTATUS_DRAGTOSHADOW));

	return TRUE;
}




/********************************************************************************************

>	ListRange* SoftShadowTool::GetShadowEditRange(NodeShadowController** ppMasterShadow = NULL)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/05/2005
	Inputs:		-
	Outputs:	-
	Returns:	Pointer to a range of shadow nodes to operate on (owned by this function, no need to delete)
				NULL if no current range is editable
	Purpose:	Find the editable shadow nodes in the current selection
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ListRange* SoftShadowTool::GetShadowEditRange(NodeShadowController** ppMasterShadow)
{
	// See whether we have a specific list of nodes...
//	if (m_pEditRange && m_pEditRange->MatchesSelectionEffectLevel(m_iEditStackPos))
	if (m_pEditRange==NULL)
		SetShadowEditRangeFromSelection();

	if (m_pEditRange)
	{
		if (ppMasterShadow)
		{
			if (!m_pEditRange->IsEmpty())
			{
				ERROR3IF(!m_pEditRange->FindFirst()->IsAShadowController(), "Stored ShadowEditRange contains non-shadow");
				*ppMasterShadow = (NodeShadowController*)m_pEditRange->FindFirst();
			}
			else
				*ppMasterShadow = NULL;
		}

		return m_pEditRange;
	}

/*	// See whether the user has selected shadow nodes himself...
	if (SetShadowEditRangeFromSelection())
	{
		// We found that all nodes in the sel range are shadows
		if (ppMasterShadow)
		{
			ERROR3IF(!m_pEditRange->FindFirst()->IsAShadowController(), "Stored ShadowEditRange contains non-shadow");
			*ppMasterShadow = (NodeShadowController*)m_pEditRange->FindFirst();
		}

		return m_pEditRange;
	}
*/
	ERROR3("I don't think we should ever get here!");
	// Stored edit context doesn't match the current selection
	// so we must find the top surface of shadows in the effects stack above the selection
	SelRange* pSelRange = GetApplication()->FindSelection();
	return pSelRange->GetTopClassRange(CC_RUNTIME_CLASS(NodeShadowController), TRUE, FALSE, (Node**)ppMasterShadow, NULL);
}




/********************************************************************************************

>	void SoftShadowTool::SetShadowEditRange(ListRange* pNewRange, INT32 iNewPos, BOOL bConsistent = FALSE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/05/2005
	Inputs:		pNewRange - pointer to new range to edit
				iStackPos - value of stack pos
	Outputs:	-
	Returns:	-
	Purpose:	Find the editable shadow nodes in the current selection
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SoftShadowTool::SetShadowEditRange(ListRange* pNewRange, INT32 iNewPos, BOOL bConsistent)
{
	if (m_pEditRange && m_pEditRange != pNewRange)
	{
		delete m_pEditRange;
		m_pEditRange = NULL;
	}

	m_iEditStackPos = iNewPos;
	if (pNewRange)
		m_pEditRange = new ListRange(pNewRange);
	else
		m_pEditRange = NULL;

	// Since the user usually indicates a shadow range by selecting shadow nodes
	// we must do the same thing for consistency
/*	if (m_pEditRange)
	{
		NodeRenderableInk::DeselectAll();

		Node* pNode = m_pEditRange->FindFirst();
		while (pNode)
		{
			if (pNode->IsAShadowController())
			{
				NodeShadowController* pController = (NodeShadowController*) pNode;
				pController->GetShadow()->Select(FALSE);
			}

			pNode = m_pEditRange->FindNext(pNode);
		}
	}

	// Make sure InfoBar is updated to reflect the new stored edit range
	BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::EFFECTSTACKCHANGED));
*/
}




/********************************************************************************************

>	void SoftShadowTool::SelectShadowRange(ListRange* pNewRange)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/2005
	Inputs:		pNewRange - pointer to new range to edit
	Outputs:	-
	Returns:	-
	Purpose:	Find the editable shadow nodes in the current selection
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SoftShadowTool::SelectShadowRange(ListRange* pNewRange)
{
	ERROR3IF(pNewRange==NULL, "SelectShadowRange not given range pointer");

	// Make a local copy because the passed-in list may automatically die
	// when the selection changes
	ListRange myRange(pNewRange);

	NodeRenderableInk::DeselectAll(TRUE, TRUE);
	Node* pNode = myRange.FindFirst();
	while (pNode)
	{
		if (pNode->IsAShadowController())
		{
			NodeShadowController* pController = (NodeShadowController*)pNode;
			if (pController->GetShadow())
				pController->GetShadow()->Select(TRUE);
		}

		pNode = myRange.FindNext(pNode);
	}

	// Make sure InfoBar is updated to reflect the new stored edit range
	BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::EFFECTSTACKCHANGED));
}




/********************************************************************************************

>	BOOL SoftShadowTool::SetShadowEditRangeFromSelection(BOOL bBroadcast = FALSE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/05/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Scan the selection to see whether it contains just shadow nodes
				and whether they map to a level in the effects stack
				If so, set the edit range from that info
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL SoftShadowTool::SetShadowEditRangeFromSelection(BOOL bBroadcast)
{
	SelRange mySelRange = *GetApplication()->FindSelection();
	mySelRange.SetPromoteToParent(FALSE);

	LiveEffectsTool* pLETool = NULL;
	INT32 iPos = 0;
	EffectsStack* pStack = mySelRange.GetEffectsStack();
	Node* pNode = mySelRange.FindFirst();
//	if (pNode==NULL)
//		return FALSE;

	ListRange* pShadowRange = new ListRange;

	while (pNode)
	{
		if (pNode->IsAShadowController() && !pShadowRange->Contains(pNode))
			pShadowRange->AddNode(pNode);

		else if (pNode->IsAShadow())
		{
			NodeShadow* pShadow = (NodeShadow*)pNode;
			ERROR3IF(pShadow->GetParentController()==NULL, "Shadow has no controller!");
			if (!pShadowRange->Contains(pShadow->GetParentController()))
				pShadowRange->AddNode(pShadow->GetParentController());
		}

		else
		{
			Node* pParent = pNode->FindParent();
			NodeShadowController* pTopController = NULL;
			while (pParent && (pParent->IsEffect() || pParent->IsController()))
			{
				if (pParent->IsAShadowController())
					pTopController = (NodeShadowController*)pParent;

				pParent = pParent->FindParent();
			}
			if (pTopController && !pShadowRange->Contains(pTopController))
				pShadowRange->AddNode(pTopController);
		}

		pNode = mySelRange.FindNext(pNode);
	}

	// Let's set up the range we've just made for editing!
	if (m_pEditRange)
	{
		delete m_pEditRange;
		m_pEditRange = NULL;
	}

	// Here we have a listrange containing only shadow controller nodes
	while (iPos < (INT32)pStack->GetCount())
	{
		if (pShadowRange->MatchesSelectionEffectLevel(iPos))
			break;

		iPos++;
	}
	if (iPos >= (INT32)pStack->GetCount())
		iPos = STACKPOS_INVALID;

	// Tell the effects tool about this consistent range of effects
	m_iEditStackPos = iPos;
	m_pEditRange = pShadowRange;					// Assign ownership of this list to the class
	pLETool = (LiveEffectsTool*)Tool::FindTool(TOOLID_LIVEEFFECT);
	if (pLETool)
	{
		pLETool->SetCurrentEffectID(POSTPRO_ID_SHADOW);
		pLETool->SetCurrentStackPos(iPos);
	}

	if (bBroadcast)
	{
		// Make sure InfoBar is updated to reflect the new stored edit range
		BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::EFFECTSTACKCHANGED));
	}
	return TRUE;

//NoSet:
//	if (pShadowRange)
//		delete pShadowRange;
//
//	return FALSE;
}




/********************************************************************************************

>	NodeShadowController* SoftShadowTool::GetShadowEditField(ShadowMaskBits flags, String_256* pStrInfo = NULL)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/05/2005
	Inputs:		flags - bitfield describing which fields to search for
	Outputs:	-
	Returns:	Pointer to a sample node
				NULL if no common nodes could be found
	Purpose:	Find the editable shadow nodes with common values in the current selection
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

NodeShadowController* SoftShadowTool::GetShadowEditField(ShadowMaskBits flags, String_256* pStrInfo)
{
	BOOL bNone = TRUE;
	BOOL bConsistent = FALSE;
	BOOL bMany = FALSE;
	NodeShadowController* pMaster = NULL;

	pMaster = GetShadowEditField(flags, &bNone, &bConsistent, &bMany);

	// There are no fields of the required type
	if (bNone && pStrInfo!=NULL)
		pStrInfo->Load(_R(IDS_SHADOWSTRING_NONE));

	// There are many fields of the required type until we prove otherwise
	if (bMany && pStrInfo!=NULL)
		pStrInfo->Load(_R(IDS_SHADOWSTRING_MANY));

	// Don't confuse the caller by returning None or Many when there is a consistent value
	if (bConsistent && pStrInfo!=NULL)
		pStrInfo->Empty();

	return pMaster;
}




/********************************************************************************************

>	NodeShadowController* SoftShadowTool::GetShadowEditField(ShadowMaskBits flags, BOOL* pbNone, BOOL* pbConsistent, BOOL* pbMany)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/05/2005
	Inputs:		flags - bitfield describing which fields to search for
	Outputs:	-
	Returns:	Pointer to a sample node
				NULL if no common nodes could be found
	Purpose:	Find the editable shadow nodes with common values in the current selection
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

NodeShadowController* SoftShadowTool::GetShadowEditField(ShadowMaskBits flags, BOOL* pbNone, BOOL* pbConsistent, BOOL* pbMany)
{
	ERROR3IF(pbNone==NULL, "!1");
	ERROR3IF(pbConsistent==NULL, "!2");
	ERROR3IF(pbMany==NULL, "!3");

	*pbNone = TRUE;
	*pbConsistent = FALSE;
	*pbMany = FALSE;

	Range* pRange = GetShadowEditRange();
	if (pRange==NULL || pRange->IsEmpty())
		return NULL;

	*pbNone = FALSE;

	NodeShadowController* pMaster = NULL;
	Node* pNode = pRange->FindFirst();
	ShadowFieldMask mask(flags);
	while (pNode)
	{
		ENSURE(pNode->IsAShadowController(), "Expected a NodeShadowController in GetShadowField");

		if (pMaster==NULL)
			pMaster = (NodeShadowController*)pNode;
		else
		{
			*pbMany = TRUE;
			if (!pMaster->CompareState((NodeShadowController*)pNode, mask))
				return NULL;
		}

		pNode = pRange->FindNext(pNode);
	}

	*pbConsistent = TRUE;
	return pMaster;
}




/********************************************************************************************

>	virtual BOOL SelectorTool::OnKeyPress(KeyPress* pKey)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/2004
	Inputs:		pKey			pointer to a key-press object representing a keystroke.
	Outputs:	-
	Returns:	TRUE if the key-event is handled, FALSE if not.
	Purpose:	Called when a key is pressed of released.  If the key is a "click modifier"
				such as the ALT or CTRL key then the cursor is changed to indicate whatever
				the tool can do with that modifier.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL SoftShadowTool::OnKeyPress(KeyPress* pKey)
{
	// Find the current state of the "click" keyboard modifiers...
	ClickModifiers ClickMods = ClickModifiers::GetClickModifiers();

	switch (pKey->GetVirtKey())
	{
	case CAMKEY(1):									// toggle tool blobs
		if (pKey->IsPress() && !pKey->IsModified())
		{
			pSoftShadowInfoBarOp->HandleShadowTypeChange(SHADOWTYPE_NONE);
			return TRUE;
		}
		break;

	case CAMKEY(2):									// toggle edit blobs
		if (pKey->IsPress() && !pKey->IsModified())
		{
			pSoftShadowInfoBarOp->HandleShadowTypeChange(SHADOWTYPE_WALL);
			return TRUE;
		}
		break;

	case CAMKEY(3):									// toggle fill blobs
		if (pKey->IsPress() && !pKey->IsModified())
		{
			pSoftShadowInfoBarOp->HandleShadowTypeChange(SHADOWTYPE_FLOOR);
			return TRUE;
		}
		break;

	case CAMKEY(4):									// toggle bounds/rotate blobs
		if (pKey->IsPress() && !pKey->IsModified())
		{
			pSoftShadowInfoBarOp->HandleShadowTypeChange(SHADOWTYPE_GLOW);
			return TRUE;
		}
		break;

	case CAMKEY(5):									// toggle bounds/rotate blobs
		if (pKey->IsPress() && !pKey->IsModified())
		{
			pSoftShadowInfoBarOp->HandleShadowTypeChange(SHADOWTYPE_FEATHER);
			return TRUE;
		}
		break;

	case CAMKEY(A):									// select all shadows only
		if (pKey->IsPress() && !pKey->IsModified())
		{
			pSoftShadowInfoBarOp->SelectShadowsAlone();
			return TRUE;
		}
		break;
	}	

	return FALSE;
}



//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------



/********************************************************************************************

>	OpDragCreateShadow2::OpDragCreateShadow2()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/06/2000
	Purpose:	Constructor.

********************************************************************************************/
OpDragCreateShadow2::OpDragCreateShadow2()
{
	m_pInfoBar	= NULL;
	m_pTool		= NULL;
	m_plInitialShadowSettings	= NULL;
	m_pOwnedRange = NULL;
}



/********************************************************************************************

>	OpDragCreateShadow2::~OpDragCreateShadow2()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/06/2000
	Purpose:	Destructor.
				Free any resources used during this drag.

********************************************************************************************/
OpDragCreateShadow2::~OpDragCreateShadow2()
{
	m_pInfoBar	= NULL;
	m_pTool		= NULL;
	if (m_plInitialShadowSettings != NULL)
	{
		m_plInitialShadowSettings->DeleteAll();
		delete m_plInitialShadowSettings;
		m_plInitialShadowSettings = NULL;
	}

	if (m_pOwnedRange)
	{
		delete m_pOwnedRange;
		m_pOwnedRange = NULL;
	}
}



/********************************************************************************************

>	virtual void OpDragCreateShadow2::DoWithParam(OpDescriptor* pOpDesc, OpParam* pOpParam)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/06/2000
	Inputs:		pOpDesc		ptr to the OpDescriptor which invoked this operation.
				pOpParam	parameter structure required by the operation to perform.
	Purpose:	Called to start this drag operation.
				Description of behaviour for the various dragging scenarios:

				Selected	Shadowed	Dragged		Result
				--------	--------	-------		------
				No			No			Shape		Select shape, then apply & drag shadow.
							Yes			Shape		Select shape, then drag shadow.
										Shadow		Select shadow, then drag it.
				Yes			No			Shape		Apply and	} drag shadow, and copy its
							Yes			Shape					} settings to all similar 
										Shadow					} shadows in the selection.

	Notes:		Apart from initially biding by the selection rules for groups, it doesn't
				matter whether the target object is grouped or not - we act on the selection.

				The node on which the drag started determines the behaviour of the drag.
				If it has no shadow, then a default shadow will be applied to it.
				Selected nodes without shadows will have the control shadow applied to them.
				Selected nodes with shadows will only mirror the control shadow if they have
				the same type of shadow.

********************************************************************************************/
void OpDragCreateShadow2::DoWithParam(OpDescriptor* pOpDesc, OpParam* pOpParam)
{
	// sometimes we work on the selection, and sometimes we change the selection and
	// _then_ work on it; either way, we gotta be a selop...
	// ps don't know what the BOOLs do - they're one of the few things left from Dave's code.
	DoStartSelOp(FALSE, FALSE, FALSE, FALSE);

	// initialise our internal state, partly from the passed-in parameter.
	DragCreateShadowParam* pParam = (DragCreateShadowParam*)pOpParam;

	m_pInfoBar			= pParam->m_pInfoBar;
	m_pTool				= pParam->m_pTool;
	m_InitialPointerPos	= pParam->m_PointerPos;

	m_pMasterShadow		= NULL;
	if (m_plInitialShadowSettings != NULL)
	{
		m_plInitialShadowSettings->DeleteAll();
		delete m_plInitialShadowSettings;
		m_plInitialShadowSettings = NULL;
	}

	// -----------------------------------------------------
	// get the node under the pointer, and use it to determine our target node for the drag.
	// if there is none, then exit.
	NodeRenderableInk* pTargetInk = NodeRenderableInk::FindSimpleAtPoint(Document::GetSelectedSpread(),
																		 m_InitialPointerPos);
	if (pTargetInk == NULL)
	{
		NodeRenderableInk::DeselectAll(TRUE, TRUE);
		End();
		return;
	}
	NodeRenderableInk* pTargetCompound = NodeRenderableInk::FindCompoundFromSimple(pTargetInk);	// Normal click logic
	NodeCompound* pController = pTargetInk->GetParentController();
	BOOL bControllerOfSelected = FALSE;
	if (pController)
		bControllerOfSelected = pController->IsParentOfSelected();

	// By default assume that the selected shadows, as decribed by m_pEditRange will be dragged
	// If the clicked node is selected
	// Or is the shadow of a selected node
	// Or is the inknode of a selected shadow
	// Then just use the selection as is
	// Otherwise, call Select the clicked object and call SetEditRangeFromSelection

//	INT32 iPos = STACKPOS_INVALID;
//	BOOL bTopLevel = FALSE;
	ListRange* pShadowRange = NULL;
//	ListRange* pTempRange = NULL;

	// Get the master shadow from the current edit range
	m_pTool->GetShadowEditRange(&m_pMasterShadow);

	// If the clicked node was a shadow but is not in the selection
	if (pTargetInk->IsAShadow())
	{
		Node* pShadowedInk = NULL;
		if (pController && pController->IsAShadowController())
		{
			pShadowedInk = ((NodeShadowController*)pController)->GetInkNodeFromController();
		}

		if (pTargetInk->IsSelected() || pShadowedInk->IsSelected())
		{
			// Shadow is in selection
		}
		else
		{
			// Shadow is not in selection so alter the selection to refer to the
			// clicked shadow
			NodeRenderableInk::DeselectAll(TRUE, TRUE);
			pTargetInk->Select(TRUE);
			m_pTool->SetShadowEditRangeFromSelection();
			m_pTool->GetShadowEditRange(&m_pMasterShadow);
		}
	}
	else
	{
		if (pTargetCompound->IsSelected())
		{
			// Clicked node is in selection
		}
		else
		{
			// Clicked node is not in selection
			NodeRenderableInk::DeselectAll(TRUE, TRUE);
			pTargetCompound->Select(TRUE);
			m_pTool->SetShadowEditRangeFromSelection();
			m_pTool->GetShadowEditRange(&m_pMasterShadow);
		}
	}

	// Make sure you call GetShadowEditRange from this point onwards to get the stored COPY of pShadowRange!

	// after all the malarky above, we'll see if we can get the target's shadow controller.
	// if we can't, or if the Shift key was pressed, then we'll make a new one.
	if (m_pMasterShadow == NULL || pParam->m_ClickMods.Adjust)
	{
		// Doesn't yet have a shadow so we can create one for the whole selection
		NodeShadowParam NSParam(NodeShadowController::DefaultType);
		Range TempRange(*GetApplication()->FindSelection());
		TempRange.SetPromoteToParent(TRUE);
		pShadowRange = OpApplyShadow::DoApplyShadow(this, &TempRange, &NSParam);
		m_pOwnedRange = pShadowRange;
		m_pTool->SetShadowEditRange(pShadowRange, STACKPOS_TOP, TRUE);

		// Make sure you call GetShadowEditRange from this point onwards to get the stored COPY of pShadowRange!

		// quit gracefully if we can't create it.
		if (pShadowRange == NULL)
		{
			FailAndExecute();
			End();
			return;
		}

		m_pMasterShadow = (NodeShadowController*)pShadowRange->FindFirst();
		if (m_pMasterShadow == NULL)
		{
			FailAndExecute();
			End();
			return;
		}

		SliceHelper::AddNamesToController(this, m_pMasterShadow);
	}
	else
	{
		// Find out whether we have a consistent level or not
		// (And preserve current shadow settings while we're looking)
		pShadowRange = m_pTool->GetShadowEditRange();
		Node* pNode = pShadowRange->FindFirst();
//		NodeShadowController* pNewController = NULL;
		BOOL bOK;
		while (pNode)
		{
			if (pNode->IsAShadowController())
			{
				bOK = m_pMasterShadow->CompareState((NodeShadowController*)pNode) && SaveSettings((NodeShadowController*)pNode);
				if (!bOK)
				{
					FailAndExecute();
					End();
					return;
				}
			}
			else
			{
				FailAndExecute();
				End();
				return;
			}

			pNode = pShadowRange->FindNext(pNode);
		}
	}

	// at this point in the proceedings, we should definitely have a master shadow controller.
	// if we don't, then something somewhere has gone fruity :o)
	if (m_pMasterShadow == NULL)
	{
		ERROR3("OpDragCreateShadow2::DoWithParam; Selection problem");
		FailAndExecute();
		End();
		return;
	}

	// Conditionally issue redraws if there were other effects above us in the stack
	pShadowRange = m_pTool->GetShadowEditRange();
	Node* pNode = pShadowRange->FindFirst();
//	NodeShadowController* pNewController = NULL;
	BOOL bOK;
	bOK = DoInvalidateNodesRegions(*pShadowRange, TRUE, FALSE, FALSE, FALSE);	// Don't recache
	if (!bOK)
	{
		FailAndExecute();
		End();
		return;
	}

	while (pNode)
	{
		if (pNode->IsAShadowController())
		{
			NodeShadowController* pController = (NodeShadowController*)pNode;
//			pController->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
			pController->ReleaseCached(TRUE, FALSE, FALSE, FALSE);
		}

		pNode = pShadowRange->FindNext(pNode);
	}

	// record initial information from the master shadow.
	m_InitialGlowWidth				= m_pMasterShadow->GetGlowWidth();
	m_dcInitialWallOffset			= m_pMasterShadow->GetWallShadowOffset();
	m_dcInitialFloorShadowVector	= m_pMasterShadow->GetFloorShadowVector();
	m_InitialFeatherWidth			= m_pMasterShadow->GetFeatherWidth();

	// tell the info-bar to update itself for whatever objects are now selected.
	if (m_pInfoBar != NULL)
		m_pInfoBar->UpdateAllGadgets();

	// Prevent re-cacheing during this drag
	Operation::SetQuickRender(TRUE);

	// ok, stop faffing around and get on with the drag.
	StartDrag(DRAGTYPE_AUTOSCROLL);
}




/********************************************************************************************

>	static NodeRenderableInk* OpDragCreateShadow2::GetSelectedShadowFromNode(NodeRenderableInk* pClickedNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/2004
	Inputs:		pClickedInk	- ptr to a simple node which the user has clicked on
	Outputs:	ppLevelRange - output ptr to range of NodeShadowControllers
	Returns:	NodeRenderableInk* - ptr to a user-selectable node in the postprocessor stack
				above the selection, if present
				NULL otherwise
	Purpose:	Determine whether the clicked item is at an editable position in the
				postprocessor stack

********************************************************************************************/
NodeRenderableInk* OpDragCreateShadow2::GetSelectedShadowFromNode(NodeRenderableInk* pClickedNode)
{
	Node* pUpNode = pClickedNode->FindParent();
	while (pUpNode && (pUpNode->IsAShadowController() || pUpNode->IsNodeHidden()))
	{
		if (pUpNode->IsAShadowController())
		{
			NodeShadowController* pController = (NodeShadowController*)pUpNode;
			NodeShadow* pShadow = pController->GetShadow();
			if (pShadow && pShadow->IsSelected())
			{
				return pShadow;
			}
		}

		pUpNode = pUpNode->FindParent();
	}

	return NULL;
}




/********************************************************************************************

>	virtual void OpDragCreateShadow2::DragPointerMove(	DocCoord PointerPos,
														ClickModifiers ClickMods,
														Spread* pSpread, BOOL bSolidDrag)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/06/2000
	Inputs:		PointerPos		where it's at, man ;)
				ClickMods		what's happenin', man ;)
				pSpread			the spread where the action is, dude ;)

	Purpose:	Called by Camelot's drag code whenever the pointer moves as a shadow is
				being dragged. Updates the master shadow according to the position of the
				mouse pointer, and mirrors the master shadow's settings in all other shadows
				of the same type, which were selected when the drag was started.
	Errors:		
	See also:	

********************************************************************************************/
void OpDragCreateShadow2::DragPointerMove(	DocCoord PointerPos,
											ClickModifiers ClickMods,
											Spread* pSpread, BOOL bSolidDrag)
{
	// ok, decide how the drag is gonna change our master shadow controller,
	// then apply its settings to all other shadows in our shadow-list.

	// the shadow settings are calculated relative to this rectangle.
	DocRect drBaseRect = m_pMasterShadow->GetInsideBoundingRect();

	// two useful offset vectors which we use to determine the shadow settings.
	DocCoord dcTotalOffset	= PointerPos - m_InitialPointerPos;

	// we have different behaviours, depending on the type of shadow we are dealing with.
	switch (m_pMasterShadow->GetShadowType())
	{
	// wall shadows are simply offset by the pointer.
	case SHADOWTYPE_WALL:
		{
			DocCoord dcControlOffset = m_dcInitialWallOffset + dcTotalOffset;

			// set the same shadow settings in all wall-shadow members of the shadow list.
			ChangeAllShadows(&dcControlOffset, NULL, 0, SHADOWTYPE_WALL);

			// update the info-bar with these settings as well:
			//	1. to feedback the wall-shadow position in the UI, and
			//	2. to keep a record of the most recent wall-shadow settings.
			m_pInfoBar->UpdateGadgetPositionShadowX();
			m_pInfoBar->UpdateGadgetPositionShadowY();
		}
		break;

	// floor shadows are positioned so as to maintain two constant values:
	//	1. the ratio of the shadow's height to the pointer's height above the shadow's base (alpha).
	//	2. the horizontal distance from the pointer to the centre of the shadow (offx / alpha)
	//
	// in this model, the floor shadow is a parallelogram with a fixed base.
	// it has a defining line from the centre of its base to the centre of its top,
	// and the control pointer is glued to one of the sides.
	case SHADOWTYPE_FLOOR:
		{
			DocCoord BaseCentre((drBaseRect.hi.x + drBaseRect.lo.x) / 2, drBaseRect.lo.y);
			DocCoord InitVector = m_InitialPointerPos - BaseCentre;

			// we don't like a zero initial pointer height.
			if (InitVector.y > 0)
			{
				DocCoord NewVector	= PointerPos - BaseCentre;

				const double alpha	= (double)m_dcInitialFloorShadowVector.y / (double)InitVector.y;
				const double offx	= (double)m_dcInitialFloorShadowVector.x - ((double)InitVector.x * alpha);

				DocCoord	dcControlVector((MILLIPOINT)(0.5 + (double)NewVector.x * alpha + offx),
											(MILLIPOINT)(0.5 + (double)NewVector.y * alpha)			);

				// set the same shadow settings in all floor-shadow members of the shadow list.
				ChangeAllShadows(NULL, &dcControlVector, 0, SHADOWTYPE_FLOOR);
			}
		}
		break;

	// it's very hard to accurately feed back glow-width changes, as the outline of the
	// shadowed node would need to be taken into account. therefore, we use a clever & quick
	// approximation from David's old code - add up the x- and y- offsets from where the
	// drag started, taking into account our position relative to the centre of the shadowed
	// node. the upshot of this is that vertical or horizontal mouse movements directly
	// relate to how wide the glow is.
	case SHADOWTYPE_GLOW:
		{
			if (m_InitialPointerPos.x < drBaseRect.Centre().x)
				dcTotalOffset.x = -dcTotalOffset.x;

			if (m_InitialPointerPos.y < drBaseRect.Centre().y)
				dcTotalOffset.y = -dcTotalOffset.y;

			MILLIPOINT GlowWidth = m_InitialGlowWidth + (dcTotalOffset.x + dcTotalOffset.y);

			if (GlowWidth < 0)
				GlowWidth = 0;

			// set the same shadow settings in all glow-shadow members of the shadow list.
			ChangeAllShadows(NULL, NULL, GlowWidth, SHADOWTYPE_GLOW);
		}
		break;

	case SHADOWTYPE_FEATHER:
		{
			if (m_InitialPointerPos.x < drBaseRect.Centre().x)
				dcTotalOffset.x = -dcTotalOffset.x;

			if (m_InitialPointerPos.y < drBaseRect.Centre().y)
				dcTotalOffset.y = -dcTotalOffset.y;

			MILLIPOINT FeatherWidth = m_InitialFeatherWidth + (dcTotalOffset.x + dcTotalOffset.y);

			if (FeatherWidth < 0)
				FeatherWidth = 0;

			// set the same shadow settings in all glow-shadow members of the shadow list.
			ChangeAllShadows(NULL, NULL, FeatherWidth, SHADOWTYPE_FEATHER);
		}
		break;

	default:
		ERROR3("OpDragCreateShadow2::DragPointerMove; found an unrecognised shadow type!");
		return;
	}
}



/********************************************************************************************

>	virtual void OpDragCreateShadow2::DragFinished(	DocCoord PointerPos,
													ClickModifiers ClickMods,
													Spread* pSpread, BOOL bSuccess, BOOL bSolidDrag)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/06/2000
	Inputs:		PointerPos		where the pointer is when the drag ends.
				ClickMods		modifiers (eg CTRL/SHIFT) in use when the drag ends.
				pSpread			the spread where the drag is taking place.
				bSuccess		whether the drag finished successfully, or aborted
								half way through, eg if the user pressed Escape.

	Purpose:	Lets us tidy ourselves up, refresh the view etc. after the drag has finished.
				Used by the shadowing code to save or restore the old shadow settings from
				before the drag.
	Errors:		
	See also:	

********************************************************************************************/
void OpDragCreateShadow2::DragFinished(	DocCoord PointerPos, ClickModifiers ClickMods,
										Spread* pSpread, BOOL bSuccess, BOOL bSolidDrag)
{
	// tell the drag framework that we're finished.
	EndDrag();

	// Allow re-cacheing after this drag
	Operation::SetQuickRender(FALSE);

	// if we've succeeded so far, then attempt to create a new action in which the original
	// shadow settings are saved. if there are no saved shadow settings, then we obviously
	// don't need to worry about this.

	// also, note that the process of saving settings means that we relinquish control of our
	// list of initial settings to the action. to make sure we don't try to access this
	// list again, we set our pointer to it to NULL.
	if (bSuccess)
	{
		if (m_plInitialShadowSettings != NULL && !m_plInitialShadowSettings->IsEmpty() )
		{
			ActionCode ac = SaveShadowSettingsAction::Init(	this, GetUndoActionList(),
															m_plInitialShadowSettings );
			if (ac == AC_FAIL)
				bSuccess = FALSE;
			else
				m_plInitialShadowSettings = NULL;
		}
	}

	if (m_pOwnedRange)
	{
		delete m_pOwnedRange;
		m_pOwnedRange = NULL;
	}

	// if something went wrong, eg the user cancelled the drag, then fail,
	// and reset all shadows to their initial settings before the drag.
	if (!bSuccess)
	{
		// Force Redraw all the affected regions
		ListRange* pShadowRange = m_pTool->GetShadowEditRange();
		Node* pNode = pShadowRange->FindFirst();
		DocRect drInvalidRegion;
		while (pNode)
		{
			ENSURE(pNode->IsAShadowController(), "Unexpected node type in ChangeAllShadows");
			NodeShadowController* pShadowController	= (NodeShadowController*)pNode;
			NodeShadow* pNodeShadow					= pShadowController->GetShadow();
			drInvalidRegion = drInvalidRegion.Union(pShadowController->GetEffectStackBounds());
			pShadowController->ReleaseCached(TRUE, FALSE, FALSE, TRUE);	// Parents and derived data only

			// record the current bounds covered by the shadows.
			drInvalidRegion = drInvalidRegion.Union(pNodeShadow->GetBlobBoundingRect());

			pNode = pShadowRange->FindNext(pNode);
		}

		// redraw the invalidated screen areas.
		Document* pDoc	= Document::GetCurrent();
		Spread* pSpread	= pDoc->GetSelectedSpread();
		if (pDoc != NULL && pSpread != NULL)
		{
			pDoc->ForceRedraw(pSpread, drInvalidRegion, FALSE, NULL, FALSE);	// Don't automatically flush out subtree caches
		}

		RestoreSavedSettings();
		FailAndExecute();
	}
	else
	{
		// Conditionally issue redraws if there were other effects above us in the stack
		EffectsStack* pStack = GetApplication()->FindSelection()->GetEffectsStack();	// From cache
		ENSURE(pStack, "Something's gone seriously wrong");
		if (!pStack->NodeInTopLevel(m_pMasterShadow))
		{
			ListRange* pShadowRange = m_pTool->GetShadowEditRange();
			DoInvalidateNodesRegions(*pShadowRange, TRUE);		// Argh! this function wants an actual Range!
		}
	}

	// tell the Op system that we're finished.
	End();
}



/********************************************************************************************

>	static BOOL OpDragCreateShadow2::Declare()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/06/2000
	Returns:	TRUE if all went ok,
				FALSE otherwise.
	Purpose:	Adds ourself to the list of all known operations.

	See also:	SoftShadowTool::Init(), where this function is called from.

********************************************************************************************/
BOOL OpDragCreateShadow2::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_DRAGCREATESHADOWOP),
								CC_RUNTIME_CLASS(OpDragCreateShadow2), 
								OPTOKEN_DRAGCREATESHADOW,
								OpDragCreateShadow2::GetState));
}



/********************************************************************************************

>	static OpState OpDragCreateShadow2::GetState(	String_256* pstrDescription,
													OpDescriptor* pOpDesc	)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/06/2000
	Returns:	The state of the operation.
	Purpose:	Get the state of this operation, ie whether or not it can be invoked.

				We currently always give an all-clear - this function is really a lot more
				use for Ops which live in menus.

********************************************************************************************/
OpState OpDragCreateShadow2::GetState(String_256* pstrDescription, OpDescriptor* pOpDesc)
{
	OpState DefaultOpSt;
	return DefaultOpSt;
}



/********************************************************************************************

>	void OpDragCreateShadow2::ChangeAllShadows(	DocCoord* pdcWallOffset,
												DocCoord* pdcFloorVector,
												MILLIPOINT GlowWidth,
												ShadowType ChangeType	)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/06/2000
	Inputs:		pdcWallOffset	the offset to reset wall shadows with.
				pdcFloorVector	the vector to reset floor shadows with.
				GlowWidth		the width to reset glow shadows with.
				ChangeType		which 

	Outputs:	Shadows of type ChangeType in m_ShadowList have their parameters reset
				accordingly.

	Purpose:	Change all shadows in our list, of type ChangeType, in a way specific to the
				type:
				SHADOWTYPE_WALL		Wall shadow is offset.
				SHADOWTYPE_FLOOR	Floor shadow vector is reset.
				SHADOWTYPE_GLOW		Glow shadow width is reset.

				Each shadow is regenerated, and the document is refreshed accordingly.

	Errors:		ERROR3 in DEBUG	if an invalid ShadowType, or if the specific input
								parameter relating to the ShadowType is invalid.

				NB Does *NOT* check for errors in release!

********************************************************************************************/
void OpDragCreateShadow2::ChangeAllShadows(	DocCoord* pdcWallOffset,
											DocCoord* pdcFloorVector,
											MILLIPOINT GlowWidth, ShadowType ChangeType	)
{
	// input validation, only in _DEBUG.
	// Declare this outside the DEBUG bit so it can be used below...
	MILLIPOINT FeatherWidth = GlowWidth;
	BOOL fInvalidParams = FALSE;
	double dFloorShadowAngle = 0;
	double dFloorShadowHeight = 0;
	switch (ChangeType)
	{
	case SHADOWTYPE_WALL:
		// wall shadows - must have a valid wall offset.
		fInvalidParams = (pdcWallOffset == NULL);
		break;
	case SHADOWTYPE_FLOOR:
		// floor shadows - must have a valid floor control vector.
		fInvalidParams = (pdcFloorVector == NULL);
		if (pdcFloorVector)
		{
			// Compute common angle and height for all selected floor shadows
			INT32 BaseHeight = m_pMasterShadow->GetInsideBoundingRect().Height();
			if (BaseHeight > 0)
				dFloorShadowHeight = (double)pdcFloorVector->y / (double)BaseHeight;
			else
				dFloorShadowHeight = 1.0;

			if (dFloorShadowHeight > MAXSHADOWHEIGHT)
				dFloorShadowHeight = MAXSHADOWHEIGHT;
			else if (dFloorShadowHeight < 0.01)
				dFloorShadowHeight = 0.01;

			// the angle of the floor shadow is the clockwise rotation of the control vector
			// from the vertical, in degrees.
			// Phil says: ARGH! Should be anti-clockwise from due east! What's wrong with the mathematical convention?
			dFloorShadowAngle = pdcFloorVector->AngleFromVertical();
			if (dFloorShadowAngle < -MAXSHADOWANGLE)
				dFloorShadowAngle = -MAXSHADOWANGLE;
			else if (dFloorShadowAngle > MAXSHADOWANGLE)
				dFloorShadowAngle = MAXSHADOWANGLE;
		}
		break;
	case SHADOWTYPE_GLOW:
		// glow shadows - must have a non-negative glow-width.
		fInvalidParams = (GlowWidth < 0);
		break;
	case SHADOWTYPE_FEATHER:
		// glow shadows - must have a non-negative glow-width.
		fInvalidParams = (FeatherWidth < 0);
		break;
	default:
		// unrecognised ShadowType.
		fInvalidParams = TRUE;
		break;
	}
	ERROR3IF(fInvalidParams, "OpDragCreateShadow2::ChangeAllShadows; Invalid input params!");

	// the region to redraw onscreen.
	DocRect drInvalidRegion;

	// initialisation - get the bounds of the first shadow on our list.
	NodeShadowController* pShadowController = NULL;
	NodeShadow* pNodeShadow = NULL;
	ENSURE(!drInvalidRegion.IsValid() || drInvalidRegion.IsEmpty(), "bounding rect should be invalid before loop!");

	// apply the new settings to every shadow we have which matches ChangeType.
	ListRange* pShadowRange = m_pTool->GetShadowEditRange();
	Node* pNode = pShadowRange->FindFirst();
	while (pNode)
	{
		ENSURE(pNode->IsAShadowController(), "Unexpected node type in ChangeAllShadows");
		pShadowController	= (NodeShadowController*)pNode;
		pNodeShadow			= pShadowController->GetShadow();
		drInvalidRegion		= drInvalidRegion.Union(pShadowController->GetEffectStackBounds());
		pShadowController->ReleaseCached(TRUE, FALSE, FALSE, TRUE);	// Parents and derived data only

		if (pShadowController->GetShadowType() == ChangeType)
		{
			// record the bounds covered by the shadows before the change.
			drInvalidRegion = drInvalidRegion.Union(pNodeShadow->GetBlobBoundingRect());

			// make the change.
			switch (ChangeType)
			{
			case SHADOWTYPE_WALL:
				{
					pShadowController->SetWallShadowOffset(*pdcWallOffset, TRUE);
				}
				break;

			case SHADOWTYPE_FLOOR:
				{
//					pShadowController->SetFloorShadowVector(*pdcFloorVector);
					pShadowController->SetFloorShadowAngle(dFloorShadowAngle);
					pShadowController->SetFloorShadowHeight(dFloorShadowHeight);
					pShadowController->RegenerateNode(NULL);
				}
				break;

			case SHADOWTYPE_GLOW:
				{
					pShadowController->SetGlowWidth(GlowWidth);
					pShadowController->RegenerateNode(NULL);
				}
				break;

			case SHADOWTYPE_FEATHER:
				{
					pShadowController->SetFeatherWidth(FeatherWidth);
					pShadowController->RegenerateNode(NULL);
				}
				break;

			default:
				// Do nothing
				break;
			}

			// record the current bounds covered by the shadows.
			drInvalidRegion = drInvalidRegion.Union(pNodeShadow->GetBlobBoundingRect());
		}

		// Karim 16/11/2000
		// Feathers BODGE - search up the tree for any GeometryLinkedAttrs attached to
		// ourself or our parents, and tell them that their linked node geometry has changed.
		{
			for (Node*	pParent =  pShadowController;
						pParent != NULL && pParent->IsAnObject();
						pParent =  pParent->FindParent() )
			{
				for (NodeAttribute*	pGLA = pParent->FindFirstGeometryLinkedAttr();
									pGLA != NULL;
									pGLA = pGLA->FindNextGeometryLinkedAttr() )
				{
					pGLA->LinkedNodeGeometryHasChanged(this);
				}
			}
		}

		pNode = pShadowRange->FindNext(pNode);
	}

	// redraw the invalidated screen areas.
	Document* pDoc	= Document::GetCurrent();
	Spread* pSpread	= pDoc->GetSelectedSpread();
	if (pDoc != NULL && pSpread != NULL)
	{
		pDoc->ForceRedraw(pSpread, drInvalidRegion, FALSE, NULL, FALSE);	// Don't automatically flush out subtree caches
		GetApplication()->ServiceRendering();
	}
}



/********************************************************************************************

>	BOOL OpDragCreateShadow2::SaveSettings(Range* pShadowRange)

	Author:		Phli
	Created:	25/11/2004
	Inputs:		pShadow		the shadow whose settings we will save.

	Outputs:	m_plInitialShadowSettings is created if necessary,
				and updated with the saved settings.

	Returns:	TRUE if successful,
				FALSE otherwise.

	Purpose:	Record the given shadow's settings.

	Errors:		ERROR3 if pShadow is NULL.
	See also:	RestoreSavedSettings().

********************************************************************************************/
BOOL OpDragCreateShadow2::SaveSettings(Range* pShadowRange)
{
	ERROR3IF(pShadowRange == NULL, "OpDragCreateShadow2::SaveSettings; NULL input params!");

	NodeShadowController* pController = (NodeShadowController*)pShadowRange->FindFirst();
	while (pController)
	{
		BOOL bOK = SaveSettings(pController);
		if (!bOK)
			return FALSE;

		pController = (NodeShadowController*)pShadowRange->FindNext(pController);
	}

	return TRUE;
}



/********************************************************************************************

>	BOOL OpDragCreateShadow2::SaveSettings(NodeShadowController* pShadow)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/2000
	Inputs:		pShadow		the shadow whose settings we will save.

	Outputs:	m_plInitialShadowSettings is created if necessary,
				and updated with the saved settings.

	Returns:	TRUE if successful,
				FALSE otherwise.

	Purpose:	Record the given shadow's settings.

	Errors:		ERROR3 if pShadow is NULL.
	See also:	RestoreSavedSettings().

********************************************************************************************/
BOOL OpDragCreateShadow2::SaveSettings(NodeShadowController* pShadow)
{
	ERROR3IF(pShadow == NULL, "OpDragCreateShadow2::SaveSettings; NULL input params!");

	if (m_plInitialShadowSettings == NULL)
	{
		ALLOC_WITH_FAIL(m_plInitialShadowSettings, new List, this);
		if (m_plInitialShadowSettings == NULL)
			return FALSE;
	}

	ShadowSettingsItem* pShadowSettings = NULL;
	ALLOC_WITH_FAIL(pShadowSettings, new ShadowSettingsItem(pShadow), this);
	if (pShadowSettings == NULL)
		return FALSE;

	switch (pShadow->GetShadowType())
	{
	case SHADOWTYPE_WALL:
		pShadowSettings->dcWallOffset	= pShadow->GetWallShadowOffset();
		break;
	case SHADOWTYPE_FLOOR:
		pShadowSettings->dcFloorVector	= pShadow->GetFloorShadowVector();
		break;
	case SHADOWTYPE_GLOW:
		pShadowSettings->GlowWidth		= pShadow->GetGlowWidth();
		break;
	case SHADOWTYPE_FEATHER:
		pShadowSettings->FeatherWidth	= pShadow->GetFeatherWidth();
		break;
	default:
		ERROR3("OpDragCreateShadow2::SaveSettings; unrecognised shadow type!");
		break;
	}

	m_plInitialShadowSettings->AddTail(pShadowSettings);

	return TRUE;
}



/********************************************************************************************

>	void OpDragCreateShadow2::RestoreSavedSettings()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/2000

	Outputs:	Nothing happens to m_plInitialShadowSettings.

	Purpose:	For all shadows in our saved-settings list, restore their settings to those
				stored with them in the list.

				Note that this routine also causes the shadows to regenerate, and causes
				redraws for the invalidated areas of the screen.
	Errors:		
	See also:	

********************************************************************************************/
void OpDragCreateShadow2::RestoreSavedSettings()
{
	// first of all, check whether we actually have any shadow settings to restore.
	if (m_plInitialShadowSettings == NULL || m_plInitialShadowSettings->IsEmpty())
		return;

	// the region to redraw onscreen.
	DocRect drInvalidRegion;

	// initialisation - get the bounds of the first shadow on our list.
	ShadowSettingsItem* pShadowSettings = (ShadowSettingsItem*)m_plInitialShadowSettings->GetHead();
	NodeShadowController* pShadowController = pShadowSettings->pShadow;
	NodeShadow* pNodeShadow = pShadowController->GetShadow();
	drInvalidRegion = pNodeShadow->GetBlobBoundingRect();

	// skip through our list and reset all shadows on it.
	while (pShadowSettings != NULL)
	{
		pShadowController	= pShadowSettings->pShadow;
		pNodeShadow			= pShadowController->GetShadow();
		drInvalidRegion		= drInvalidRegion.Union(pShadowController->GetEffectStackBounds());
		pShadowController->ReleaseCached();
		pNodeShadow->ReleaseCached();

		// record the bounds covered by the shadows before the change.
		drInvalidRegion = drInvalidRegion.Union(pNodeShadow->GetBlobBoundingRect());

		// change the shadows appropriately.
		switch (pShadowController->GetShadowType())
		{
		case SHADOWTYPE_WALL:
			pShadowController->SetWallShadowOffset(pShadowSettings->dcWallOffset, TRUE);
			break;
		case SHADOWTYPE_FLOOR:
			pShadowController->SetFloorShadowVector(pShadowSettings->dcFloorVector);
			pShadowController->RegenerateNode(this);
			break;
		case SHADOWTYPE_GLOW:
			pShadowController->SetGlowWidth(pShadowSettings->GlowWidth);
			pShadowController->RegenerateNode(this);
			break;
		case SHADOWTYPE_FEATHER:
			pShadowController->SetFeatherWidth(pShadowSettings->FeatherWidth);
			pShadowController->RegenerateNode(this);
			break;
		default:
			ERROR3("OpDragCreateShadow2::RestoreSavedSettings; unrecognised shadow type!");
			break;
		}

		// record the new bounds of the shadows.
		drInvalidRegion = drInvalidRegion.Union(pNodeShadow->GetBlobBoundingRect());

		pShadowSettings = (ShadowSettingsItem*)m_plInitialShadowSettings->GetNext(pShadowSettings);
	}

	// now redraw the invalid screen-region.
	Document*	pDoc	= Document::GetCurrent();
	Spread*		pSpread	= pDoc->GetSelectedSpread();
	if (pDoc != NULL && pSpread != NULL)
	{
		pDoc->ForceRedraw(pSpread, drInvalidRegion);
		GetApplication()->ServiceRendering();
	}
}



//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------



/********************************************************************************************

>	SaveShadowSettingsAction::SaveShadowSettingsAction()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/06/2000
	Purpose:	Constructor.

********************************************************************************************/
SaveShadowSettingsAction::SaveShadowSettingsAction()
{
	m_plShadowSettings = NULL;
}



/********************************************************************************************

>	SaveShadowSettingsAction::~SaveShadowSettingsAction()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/06/2000
	Purpose:	Destructor - free used resources.

********************************************************************************************/
SaveShadowSettingsAction::~SaveShadowSettingsAction()
{
	if (m_plShadowSettings != NULL)
	{
		m_plShadowSettings->DeleteAll();
		delete m_plShadowSettings;
		m_plShadowSettings = NULL;
	}
}



/********************************************************************************************

>	ActionCode SaveShadowSettingsAction::Init(	UndoableOperation* pUndoOp,

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/06/2000
	Inputs:		pUndoOp				ptr to the Op which invokes this action.
				pActionList			ptr to the action list which this action should live on.
				plShadowSettings	ptr to a list of saved shadows & their settings.

	Returns:	AC_OK		if we're successful.
				AC_NORECORD	if things went wrong but we can continue, just without undo.	
				AC_FAIL		if things went so wrong that the op will have to rewind itself.

	Purpose:	Doh! Just realised that this action's name is a complete misnomer!
				It actually restores saved shadow settings, given a pointer to a list of
				ShadowSettingsItem's, saving the settings before the restore into a new
				a new action on the opposite undo/redo list.

********************************************************************************************/
ActionCode SaveShadowSettingsAction::Init(	UndoableOperation* pUndoOp,
											ActionList* pActionList,
											List* plShadowSettings )
{
	// call the base class to create and put the action onto the action list.
	ActionCode ac = AC_FAIL;
	UINT32 ActSize = sizeof(SaveShadowSettingsAction);
	SaveShadowSettingsAction* pNewAction = NULL;
	ac = Action::Init(	pUndoOp,
						pActionList,
						ActSize,
						CC_RUNTIME_CLASS(SaveShadowSettingsAction),
						(Action**)&pNewAction  );

	// ok, the action now exists.
	// initialise it with the passed-in list of saved shadow settings.
	if (ac != AC_FAIL && pNewAction != NULL)
	{
		pNewAction->SetShadowList(plShadowSettings);
	}

	return ac;
}



/********************************************************************************************

>	ActionCode SaveShadowSettingsAction::Execute()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/06/2000
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	The nitty gritty bit, which is called on undo/redo.
	Errors:		
	See also:	

********************************************************************************************/
ActionCode SaveShadowSettingsAction::Execute()
{
	List* plCurrentSettings = NULL;
	ALLOC_WITH_FAIL(plCurrentSettings, new List, pOperation);
	if (plCurrentSettings == NULL)
		return AC_FAIL;

	// first loop - save the current settings for each shadow we'll be changing.
	//
	ShadowSettingsItem* pOldSettings = NULL;
	ShadowSettingsItem* pNewSettings = (ShadowSettingsItem*)m_plShadowSettings->GetHead();
	while (pNewSettings != NULL)
	{
		NodeShadowController* pShadow = pNewSettings->pShadow;

		pOldSettings = NULL;
		ALLOC_WITH_FAIL(pOldSettings, new ShadowSettingsItem(pShadow), pOperation);
		if (pOldSettings == NULL)
		{
			plCurrentSettings->DeleteAll();
			return AC_FAIL;
		}

		switch (pShadow->GetShadowType())
		{
		case SHADOWTYPE_WALL:
			pOldSettings->dcWallOffset	= pShadow->GetWallShadowOffset();
			break;
		case SHADOWTYPE_FLOOR:
			pOldSettings->dcFloorVector	= pShadow->GetFloorShadowVector();
			break;
		case SHADOWTYPE_GLOW:
			pOldSettings->GlowWidth		= pShadow->GetGlowWidth();
			break;
		case SHADOWTYPE_FEATHER:
			pOldSettings->FeatherWidth	= pShadow->GetFeatherWidth();
			break;
		default:
			ERROR3("SaveShadowSettingsAction::Execute; unrecognised shadow type!");
			break;
		}

		plCurrentSettings->AddTail(pOldSettings);

		pNewSettings = (ShadowSettingsItem*)m_plShadowSettings->GetNext(pNewSettings);
	}

	// second loop - reset each shadow with the settings stored for it in the list.
	//
	DocRect drInvalidRegion;

	pNewSettings = (ShadowSettingsItem*)m_plShadowSettings->GetHead();
	NodeShadowController* pShadow = pNewSettings->pShadow;
	NodeShadow* pNodeShadow = pShadow->GetShadow();
	drInvalidRegion = pNodeShadow->GetBlobBoundingRect();

	while (pNewSettings != NULL)
	{
		pShadow		= pNewSettings->pShadow;
		pNodeShadow	= pShadow->GetShadow();
		drInvalidRegion = drInvalidRegion.Union(pShadow->GetEffectStackBounds());
		pShadow->ReleaseCached();
		pNodeShadow->ReleaseCached();

		drInvalidRegion = drInvalidRegion.Union(pNodeShadow->GetBlobBoundingRect());

		switch (pShadow->GetShadowType())
		{
		case SHADOWTYPE_WALL:
			pShadow->SetWallShadowOffset(pNewSettings->dcWallOffset, TRUE);
			break;
		case SHADOWTYPE_FLOOR:
			pShadow->SetFloorShadowVector(pNewSettings->dcFloorVector);
			pShadow->RegenerateNode((UndoableOperation*)pOperation);
			break;
		case SHADOWTYPE_GLOW:
			pShadow->SetGlowWidth(pNewSettings->GlowWidth);
			pShadow->RegenerateNode((UndoableOperation*)pOperation);
			break;
		case SHADOWTYPE_FEATHER:
			pShadow->SetFeatherWidth(pNewSettings->FeatherWidth);
			pShadow->RegenerateNode((UndoableOperation*)pOperation);
			break;
		default:
			ERROR3("SaveShadowSettingsAction::Execute; unrecognised shadow type!");
			break;
		}

		drInvalidRegion = drInvalidRegion.Union(pNodeShadow->GetBlobBoundingRect());


		pNewSettings = (ShadowSettingsItem*)m_plShadowSettings->GetNext(pNewSettings);
	}

	// update the document.
	Document*	pDoc	= Document::GetCurrent();
	Spread*		pSpread	= pDoc->GetSelectedSpread();
	if (pDoc != NULL && pSpread != NULL)
	{
		pDoc->ForceRedraw(pSpread, drInvalidRegion);
		GetApplication()->ServiceRendering();
	}

	// empty our current list of settings and use our new one to create a new action.
	m_plShadowSettings->DeleteAll();
	delete m_plShadowSettings;
	m_plShadowSettings = NULL;
	return SaveShadowSettingsAction::Init(	(UndoableOperation*)pOperation,
											pOppositeActLst,
											plCurrentSettings	);
}



#endif	// BUILDSHADOWS
