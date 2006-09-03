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
// The FreeHand Tool
// Created by Rik on 2/9/93

/*
*/

#include "camtypes.h"
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "resource.h"	// _R(IDS_OUTOFMEMORY)
//#include "barsdlgs.h"
//#include "mario.h"
//#include "rik.h"
//#include "markn.h"
//#include "viewrc.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "mainfrm.h"
#include "infobar.h"
#include "csrstack.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "osrndrgn.h"
#include "oilfiles.h"
#include "blobs.h"
#include "freeinfo.h"
#include "opfree.h"
#include "freehand.h"

#include "fitcurve.h"
#include "opretro.h"
#include "module.h"
#include "ezmodule.h"
//#include "freehres.h"
#include "progress.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodepath.h"

#include "opdrbrsh.h"
#include "opbevel.h"
//#include "range.h" - in camtypes.h [AUTOMATICALLY REMOVED]


//#include "will2.h"
#include "nodeblnd.h"
#include "ndbldpth.h"
#include "nodetxts.h"
#include "ndtxtpth.h"
#include "attrmap.h"
#include "nodershp.h"
#include "brushref.h"
#include "brshcomp.h"
#include "ppbrush.h"
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "brshattr.h"
#include "sgline.h"
#include "progress.h"
#include "brshdata.h"
#include "brshgdgt.h"
#include "lineattr.h"
#include "brushmsg.h"
#include "coldlog.h"
#include "comattrmsg.h"
#include "pen.h"
#include "brshname.h"
#include "brshbeca.h"

//#include "grndbrsh.h"
#include "layer.h"

// Set things up so that the tool will be listed in the Dialog box
DECLARE_SOURCE("$Revision$");


// These are still TCHAR* while we wait for resource technology to be developed for modules
TCHAR* FreeHandTool::FamilyName	= _T("Drawing Tools");
TCHAR* FreeHandTool::ToolName 	= _T("Free Hand Tool");
TCHAR* FreeHandTool::Purpose 	= _T("To Draw arbitrary lines");
TCHAR* FreeHandTool::Author 	= _T("Rik");

CC_IMPLEMENT_MEMDUMP( FreeHandTool, Tool_v1 )

// Better memory tracking please
#define new CAM_DEBUG_NEW


/********************************************************************************************

	Preference:	FreehandPtrCrosshair
	Section:	Mouse
	Range:		FALSE, TRUE
	Purpose:	Determine whether freehand pointer has crosshairs or not

********************************************************************************************/
BOOL FreeHandTool::FreehandPtrCrosshair = FALSE;
                                  

/********************************************************************************************

>	FreeHandTool::FreeHandTool()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Dummp Constructor - It does nothing. All the real initialisation is done
				in FreeHandTool::Init which is called by the Tool Manager
	SeeAlso:	FreeHandTool::Init

********************************************************************************************/

FreeHandTool::FreeHandTool()
{
	// The Default smoothness
	Smoothness = 50;
	
	// No info bar or previous path by default
	pInfoBarOp = NULL;
	PreviousPath = NULL;

	// The previous path is not valid
	IsPreviousPathValid = FALSE;
	IsRetroPathValid = FALSE;
	AreWeRetroFitting = FALSE;

	// The actual paths
	TrackData = NULL;
	RetroPath = NULL;

	// Positions in the previous path to default values
	StartSlot = 0;
	NumSlots = 0;
	
	// Set the cursor pointers to null
	pNormalCursor = NULL;
	pActiveCursor = NULL;
	pJoinCursor = NULL;

	// Set the vars to deal with the Joining of paths
	JoinInfo.pJoinPath = NULL;
	JoinInfo.IsNearEndPoint = FALSE;
	JoinInfo.CloseSlot = 0;
	JoinInfo.Into = 0.0;
	JoinInfo.pAttrBrush = NULL;
	JoinInfo.FirstBrushSpacing = 25;

	// Make sure that the status line text is a valid string
	StatusMsg = String_256("");
	
	m_pBlendRef		= NULL;
	m_BrushSpacing	= 10000;
	m_NumInkNodes   = 0;
	m_pGRenderBrush = NULL;
	m_BrushHandle   = BrushHandle_NoBrush;
	
	m_LastBrushDocument = TEXT("No document");
}



/********************************************************************************************

>	FreeHandTool::~FreeHandTool()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Destructor (Virtual). Does nothing.

********************************************************************************************/

FreeHandTool::~FreeHandTool()
{
	//Clear out the old paths
	DeletePaths();
PORTNOTE("other", "Removed m_pGRenderBrush support");
#ifndef EXCLUDE_FROM_XARALX
	if (m_pGRenderBrush != NULL)
		delete m_pGRenderBrush;
#endif
}



/********************************************************************************************

>	BOOL FreeHandTool::Init( INT32 Pass )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Returns:	FALSE if it does not want to be created, TRUE otherwise
	Purpose:	Used to check if the Tool was properly constructed
	SeeAlso:	FreeHandTool::FreeHandTool

********************************************************************************************/

BOOL FreeHandTool::Init()
{
	// This should be set to NULL by default. It will be set properly below, if
	// everthing is working as it should
	pInfoBarOp = NULL;

	if (!OpRetroFit::Init())
		return FALSE;

	// Now we have to declare all our operations and if that works, try to find
	// the freehand tools info bar and create it
	if (OpFreeHand::Declare())
	{
		pInfoBarOp = new FreeHandInfoBarOp();
		if (pInfoBarOp)
			pInfoBarOp->pTool = this;
PORTNOTE("dialog", "Removed Bar reading")
#if 0
		// Resource File and Object that creates FreeHandInfoBarOp objects
		CCResTextFile 			ResFile;
		FreeHandInfoBarOpCreate BarCreate;

		if (ResFile.open(_R(IDM_FREEHAND_BAR), _R(IDT_INFO_BAR_RES)))
		{
			// Found the file and opened it
			if (DialogBarOp::ReadBarsFromFile(ResFile,BarCreate))
			{
				// read it in ok, so close it
				ResFile.close();

				// Info bar now exists.  Now get a pointer to it
				String_32 str = String_32(_R(IDS_FREEHAND_INFOBARNAME));
				DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(str);

				// Should have a dialog bar op by now
				if (pDialogBarOp != NULL)
				{
					// Make sure it is what we were expecting and set it
				 	if (pDialogBarOp->IsKindOf(CC_RUNTIME_CLASS(FreeHandInfoBarOp)))
					{
						pInfoBarOp = (FreeHandInfoBarOp*) pDialogBarOp;
						pInfoBarOp->pTool = this;
					}
				}
			}
		}
#endif
	}

	GetApplication()->DeclareSection(_T("Mouse"), 10);
	GetApplication()->DeclarePref(_T("Mouse"), _T("FreehandPtrCrosshair"), &FreehandPtrCrosshair, FALSE, TRUE);

	// See if it all worked and return depending on the result
	ENSURE(pInfoBarOp!=NULL, "Failed to create FreeHand Info Bar" );
	if (pInfoBarOp==NULL)
		return FALSE;
	else
		return TRUE;
}





/********************************************************************************************

>	void FreeHandTool::Describe(void *InfoPtr)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		InfoPtr - A pointer to a tool info block. It is passed cast to void* as
				the version of the tool is unknown at this point. Later versions of the
				Tool class may have more items in this block, that this tool will not use
	Outputs:	InfoPtr - The structure pointed to by InfoPtr will have had all the info
				that this version of the Tool knows about
	Purpose:	Allows the tool manager to extract information about the tool

********************************************************************************************/

void FreeHandTool::Describe(void *InfoPtr)
{
	// Cast structure into the latest one we understand.
	ToolInfo_v1 *Info = (ToolInfo_v1 *) InfoPtr;

	Info->InfoVersion = 1;
	Info->InterfaceVersion = GetToolInterfaceVersion();  // You should always have this line.
		
	// These are all arbitrary at present.
	Info->Version = 1;
	Info->ID      = GetID();
	Info->TextID  = _R(IDS_FREE_HAND_TOOL);
	Info->BubbleID = _R(IDBBL_FHND_TOOLBOX);

	Info->Family  = FamilyName;
	Info->Name    = ToolName;
	Info->Purpose = Purpose;
	Info->Author  = Author;

	Info->InfoBarDialog = _R(IDD_FREEHANDTOOL);
}


UINT32 FreeHandTool::GetID()
{
	return TOOLID_FREEHAND;
}



// The Free Hand Tools EventHandlers

/********************************************************************************************

>	virtual void FreeHandTool::SelectChange(BOOL isSelected)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/93
	Inputs:		IsSelected - TRUE if the tool is becoming selected, FALSE if it is loosing
				the selection
	Purpose:	Called when the tool is selected or deselected.  Creates and pushes the
				tool's cursor; pops and destroys it.
	Errors:		Sends warning to debugging terminal if creating the cursor fails.

********************************************************************************************/

void FreeHandTool::SelectChange(BOOL isSelected)
{
	
	if (isSelected)
	{
		CCPen *pPen = Camelot.GetPressurePen();
		if (pPen != NULL)
		{
			PressureMode NewState = (pPen->IsRealPen()) ? PressureMode_Pen : PressureMode_None;
			pPen->SetPressureMode(NewState);
		}
	
		m_UpdateBrushState = UPDATE_ONIDLE;

		BOOL ok = BrushSelected(m_BrushHandle, FALSE);
		// if that didn't work then we won't use a brush
		if (!ok)
			m_BrushHandle = BrushHandle_NoBrush;
		
		// if we have a brush then default to no smoothness
	/*	if (m_BrushHandle != BrushHandle_NoBrush)
			Smoothness = 0;
		else
			Smoothness = 50;
*/
		m_LastBrushDocument = TEXT("No document");

		// Load the cursors
		if (!LoadCursors())
			InformError();
		
		// reset the join path pointer
		JoinInfo.pJoinPath = NULL;
		JoinInfo.FirstBrushSpacing = 25;
		// Find the blob manager ready for later
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();

		// Clear the sub-selection on all the paths in the current selection
		BOOL IsPathWithSubSelection = FALSE;
		if (Document::GetSelected() != NULL)
		{
			// Get the selection
			SelRange* Selected = GetApplication()->FindSelection();
			Node* pNode = Selected->FindFirst();
			while ((pNode!=NULL) && (!IsPathWithSubSelection))
			{
				// clear the sub-selection if this is a path
				if (pNode->IsNodePath())
				{
					// If this path has a sub-selection, then set the flag
					if (((NodePath*)pNode)->InkPath.IsSubSelection())
						IsPathWithSubSelection = TRUE;
				}

				// Now find the next selected node
				pNode = Selected->FindNext(pNode);
			}

			// if there was a sub-selection, then get rid of it
			if ((IsPathWithSubSelection) && (BlobMgr!=NULL))
			{
				// Get rid of the object blobs while we clear the paths sub-selection
				BlobStyle ClearBlobs;
				ClearBlobs.Object = FALSE;
				BlobMgr->ToolInterest(ClearBlobs);

				// There are now no object blobs on screen, so we can clear the subselection
				pNode = Selected->FindFirst();
				while (pNode!=NULL)
				{
					// clear the sub-selection if this is a path
					if (pNode->IsNodePath())
						((NodePath*)pNode)->InkPath.ClearSubSelection();

					// Now find the next selected node
					pNode = Selected->FindNext(pNode);
				}
			}
		}
	
	// Diccon 9/99 Code copied from Beztool::SelectChange() complete with original comments

	// If there is are TextStories or Blends with paths selected then select the paths
	// (PS. This is a rather disgusting way of doing things, and one you can't blame on me - MarkN 17-5-99)

	Range SelRng(*(GetApplication()->FindSelection()));
	Node* pNode = SelRng.FindFirst();
	
	while (pNode != NULL)
	{
		if (IS_A(pNode, TextStory))
		{
			TextStory* pStory = (TextStory*)pNode;
			if (pStory->GetTextPath() != NULL)
				pStory->GetTextPath()->Select(TRUE);
		}

		if (IS_A(pNode, NodeBlend))
		{
			NodeBlend* pNodeBlend = (NodeBlend*)pNode;
			UINT32 NBPCounter = 0;
			while (pNodeBlend->GetNodeBlendPath(NBPCounter) != NULL)
				pNodeBlend->GetNodeBlendPath(NBPCounter++)->Select(TRUE);
		}

		pNode = SelRng.FindNext(pNode);
	}
	
		// Create and display my info bar please
		if (pInfoBarOp != NULL)
		{
			pInfoBarOp->Create();
			pInfoBarOp->SetToolActiveState(TRUE);
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

		// Set the retro field on the info bar
		if (pInfoBarOp != NULL)
		{
			// Find out if we can retro fit and set the field appropriatly
			BOOL IsStillValid = IsRetroPathStillValid();
			pInfoBarOp->SetRetroState(IsStillValid);
		}

	}
	else
	{
		// Deselection of the tool
		// Get rid of all the tools cursors
		RemoveCursors();

PORTNOTE("other", "Removed m_pGRenderBrush support");
#ifndef EXCLUDE_FROM_XARALX
		if (m_pGRenderBrush != NULL)
		{
			delete m_pGRenderBrush;
			m_pGRenderBrush = NULL;
		}
#endif
		
		// Hide and destroy my info bar please
		if (pInfoBarOp != NULL)
		{
			pInfoBarOp->SetToolActiveState(FALSE);
			
PORTNOTE("other", "Removed brush gadget support");
#ifndef EXCLUDE_FROM_XARALX
			if (FreeHandInfoBarOp::GetBrushGadget()->IsDialogOpen ())
			{
				FreeHandInfoBarOp::GetBrushGadget()->CloseDialog ();
			}
#endif			
			pInfoBarOp->Delete();
		}
		// ensure that the colour picker is working
		SetColourEditorProcessing(TRUE);

		// ensure any tool object blobs are removed.
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		if (BlobMgr != NULL)
		{
			BlobStyle bsRemoves;
			bsRemoves.ToolObject = TRUE;
			BlobMgr->RemoveInterest(bsRemoves);
		}
	}
}



/********************************************************************************************

>	BOOL FreeHandTool::LoadCursors()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/94
	Returns:	TRUE if it worked, FALSE if it did not
	Purpose:	Loads all the cursors used by the freehand tool and pushs the normal
				one onto the top of the cursor stack
	Errors:		ERROR1 (_R(IDE_FHAND_BADCURSORS)) if it fails to load the cursors

********************************************************************************************/

BOOL FreeHandTool::LoadCursors()
{
	// This tool has just been selected, so it is not displaying a cursor
	pActiveCursor = NULL;

	// Try to create all our cursors - The normal cursor
	if (FreehandPtrCrosshair)
	{
		pNormalCursor = new Cursor(this, _R(IDC_FREEHANDTOOLCURSOR_X));
		pJoinCursor   = new Cursor(this, _R(IDC_FREEHANDJOINCURSOR_X));
		pModifyCursor = new Cursor(this, _R(IDC_FREEHANDMODIFY_X));
	}
	else
	{
		pNormalCursor = new Cursor(this, _R(IDC_FREEHANDTOOLCURSOR));
		pJoinCursor   = new Cursor(this, _R(IDC_FREEHANDJOINCURSOR));
		pModifyCursor = new Cursor(this, _R(IDC_FREEHANDMODIFY));
	}

	// did we get all the cursors ok
	if ((pNormalCursor==NULL) || (!pNormalCursor->IsValid()) || 
		(pJoinCursor==NULL)   || (!pJoinCursor->IsValid()) ||
		(pModifyCursor==NULL) || (!pModifyCursor->IsValid()))
	{
		// No, at least one of them was NULL, so delete them all
		// Deleting the null cursor will be ok
		delete pNormalCursor;
		delete pJoinCursor;
		delete pModifyCursor;

		// and ensure that the pointers are set to NULL
		pNormalCursor = NULL;
		pJoinCursor = NULL;
		pModifyCursor = NULL;

		// and set an error and return
		ERROR1(FALSE, _R(IDE_FHAND_BADCURSORS));
	}

	// All the cursors loaded ok if we got to here
	// So push the normal cursor onto the stack and mark it as active
	CurrentCursorID = CursorStack::GPush(pNormalCursor, FALSE);
	pActiveCursor = pNormalCursor;

	// return that it worked
	return TRUE;
}



/********************************************************************************************

>	void FreeHandTool::RemoveCursors()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/94
	Purpose:	Pops the freehand tools cursor off the top of the cursor stack, frees the
				memory used by all the cursors in the freehand tool and sets all the pointers
				to NULL.
	SeeAlso:	FreeHandTool::LoadCursors

********************************************************************************************/

void FreeHandTool::RemoveCursors()
{
	// If we ever had a cursor
	if (pActiveCursor!=NULL)
	{
		// pop it off the stack
		CursorStack::GPop(CurrentCursorID);

		// and free up the cursors we allocated
		delete pNormalCursor;
		delete pJoinCursor;
		delete pModifyCursor;

		// and set them all to NULL
		pNormalCursor = NULL;
		pJoinCursor = NULL;
		pActiveCursor = NULL;
		pModifyCursor = NULL;
		CurrentCursorID = 0;
	}
}




/********************************************************************************************

>	void FreeHandTool::ChangeCursor(Cursor* cursor)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/3/94
	Inputs:		ID of the cursor you want to flip to
	Purpose:	Changes to the specified cursor. Will only change the cursor if it isn't already
				this cursor, so it doesn't flicker.
	Errors:		can fail if the cursor cannot be created - the cursor code will fail.

********************************************************************************************/

void FreeHandTool::ChangeCursor(Cursor* pCursor)
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

>	void FreeHandTool::OnClick( DocCoord PointerPos, ClickType Click, 
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

void FreeHandTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )
{
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

			BROADCAST_TO_ALL(CommonAttrsChangedMsg);
		}
	}
	// Make sure this click is one that we want
	if (Click==CLICKTYPE_DRAG)
	{
		// Just what we wanted - Someone is dragging the mouse about
		// We need to make an operation to perform the drag with
	
		// quick safety check - if the current line width is less than 501MP then we want
		// to launch a freehand op even if we have a brush selected
		MILLIPOINT LineWidth = OpDrawBrush::GetCurrentLineWidthIfNotDefault();
		
		// Added a condition here - if our join info brush handle indicates no brush
		// then we are adding to a normal line, hence don't use the brush
		if (m_BrushHandle == BrushHandle_NoBrush || (LineWidth != -1 && LineWidth < 501)
			|| JoinInfo.m_BrushHandle == BrushHandle_NoBrush)
		{
			OpFreeHand* pOpFreeHand = new OpFreeHand;
			if ((pOpFreeHand!=NULL) && (GetNewPaths()))
			{	
				// Start the drag operation and pass in the Anchor Point to the push operation
				pOpFreeHand->DoDrag(StartPos, StartSpread, Smoothness, &JoinInfo, TrackData);
			}
			else
			{
				// Failed to get the memory to do the job
				ToolInformError(_R(IDS_OUT_OF_MEMORY), _R(IDS_OK));
			}
		}
		else
		{
		// launch a brush op
			Document* pDoc = Document::GetCurrent();
			if (pDoc == NULL)
			{
				ERROR3("No document in FreeHandTool::OnClick");
				return;
			}
		
			String_256 CurrentDoc = pDoc->GetDocName();
			if (CurrentDoc != m_LastBrushDocument)
			{
				m_LastBrushDocument = CurrentDoc;
				m_UpdateBrushState = UPDATE_NOW;
				BrushSelected(m_BrushHandle, FALSE);
			}
		
PORTNOTE("other", "Removed m_pGRenderBrush support");
#ifndef EXCLUDE_FROM_XARALX
			// This is a bit of a bodge to try to stop the internal error in 8bpp
			if (m_pGRenderBrush && m_pGRenderBrush->GetRenderDC()->GetSafeHdc() == NULL)
			{
				delete m_pGRenderBrush;
				m_pGRenderBrush = NULL;
			}
#endif

			if (m_pGRenderBrush == NULL || CurrentDoc != m_LastBrushDocument)
			{
				// or don't if we are not ready
				String_32 ProgString = _T("Preparing brush, please wait..");
				Progress Hourglass(&ProgString, -1, FALSE);
				m_pGRenderBrush = GetBigGRenderBrush(pSpread);
				
				if (m_pGRenderBrush == NULL)
				{
					ToolInformError(_R(IDS_OUT_OF_MEMORY), _R(IDS_OK));
					return;
				}
				m_LastBrushDocument = CurrentDoc;
				return;
			
			}
			// ensures that if we are over another brush we get the correct info
			OnMouseMove(PointerPos, pSpread, ClickMods);

			OpDrawBrush* pOpDrawBrush = new OpDrawBrush(this);
			//TRACEUSER( "Diccon", _T("About to start new stroke\n"));
			if ((pOpDrawBrush != NULL)&& (GetNewPaths()))
			{
				// turn off background rendering whilst we are in progress
				// note that we must turn off the selected docview or else we will 
				// screw up background rendering when we have multiple views (as I just found out)
				DocView* pDocView = DocView::GetCurrent(); //DocView::GetSelected(); //Current();
				if (pDocView != NULL)
					pDocView->SetPreventRenderView(TRUE);

				// turn off the colour picker, as it munches CPU
				SetColourEditorProcessing(FALSE);
PORTNOTE("other", "Removed m_pGRenderBrush support");
#ifndef EXCLUDE_FROM_XARALX
				m_pGRenderBrush->SetupMainBitmap();
#endif
				// find out if our brush definition wants time stamping, and tell the op
				BrushDefinition* pBrushDef = GetSelectedBrush();
				if (pBrushDef != NULL)
					pOpDrawBrush->SetTimeStamp(pBrushDef->GetTimeStampingPeriod());

				pOpDrawBrush->DoDrag(StartPos, StartSpread, Smoothness, m_BrushHandle, &JoinInfo, TrackData,
									m_pGRenderBrush, ClickMods);

				m_UpdateBrushState = UPDATE_NEVER; // we REALLY don't want to be updating whilst in the middle of drawing
			}
			else
			{
				// Failed to get the memory to do the job
				ToolInformError(_R(IDS_OUT_OF_MEMORY), _R(IDS_OK));
PORTNOTE("other", "Removed m_pGRenderBrush support");
#ifndef EXCLUDE_FROM_XARALX
				delete m_pGRenderBrush;
				m_pGRenderBrush = NULL;
#endif
			}
			
		}
	}


}




/********************************************************************************************

>	void FreeHandTool::OnMouseMove(DocCoord* coord, Spread* pSpread, ClickModifiers mods)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		coordinate of mouse move, pointer to spread containing coord; click modifiers
	Purpose:	This routine is called whenever the mouse moves while we're in the freehand 
				tool. it sees what is under the pointer, and flips the cursor if clicking 
				will have a different effect. The rules are:

				Over space, no selection		- freehand cursor
				Over selected endpoint			- add freehand to line

********************************************************************************************/

void FreeHandTool::OnMouseMove(DocCoord Coord, Spread* pSpread, ClickModifiers mods)
{
	// Assume that we are not going to find anything of interest, so set the params to good values
	JoinInfo.pJoinPath = NULL;
	StartPos = Coord;
	StartSpread = pSpread;
	JoinInfo.FirstBrushSpacing = 25;
	// make sure the join info always reflects the brush that we should draw with
	JoinInfo.m_BrushHandle = m_BrushHandle;
	JoinInfo.pAttrBrush = NULL;
	JoinInfo.BrushDistance = 0;
	
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


	// loop through the selection
	while (pNode!=NULL)
	{
		// We are only interested in NodePaths
		// Diccon 9/99 now we are also interested in NodeBlends, as long
		// as they are blended on a path..
		if (pNode->IsNodePath())
		{
			// get the node as a NodePath
			NodePath* pPath = (NodePath*) pNode;

			// Go and see if we are near any of the endpoints of this path
			// (and stop looking if it is)
			if (IsCursorNearEndPoint(pPath, BlobRect))
				return;

			// We were not near the open ends of the selected path, so test
			// to see if we are anywhere near any point along the path
			if (IsCursorNearPath(pPath, Range, Coord))
				return;
		}
		// Diccon added 9/99
		else if (pNode->IS_KIND_OF(NodeBlend))
		{
			UINT32 NBPCounter = 0;
			NodeBlendPath* pNodeBlendPath = ((NodeBlend*)pNode)->GetNodeBlendPath(NBPCounter++);
			
			while (pNodeBlendPath != NULL) 
			{
				NodePath* pPath = (NodePath*)pNodeBlendPath;
				// Go and see if we are near any of the endpoints of this path
				// (and stop looking if it is)
				if (IsCursorNearEndPoint(pPath, BlobRect))
					return;

				// We were not near the open ends of the selected path, so test
				// to see if we are anywhere near any point along the path
				if (IsCursorNearPath(pPath, Range, Coord))
					return;

				pNodeBlendPath = ((NodeBlend*)pNode)->GetNodeBlendPath(NBPCounter++);
			}
		}
			 
		// Now find the next selected node
		pNode = Selected->FindNext(pNode);
	}

	// if we are in brush mode then check to see if the document we are over is 
	// the same as the last document we drew on. If it is different we will need to update
	if (m_BrushHandle != BrushHandle_NoBrush)
	{
		Document* pDoc = Document::GetCurrent();
		if (pDoc != NULL)
		{
			String_256 CurrentDoc = pDoc->GetDocName();
			if (CurrentDoc != m_LastBrushDocument)
			{
				m_LastBrushDocument = CurrentDoc;
				m_UpdateBrushState = UPDATE_NOW;
				BrushSelected(m_BrushHandle, FALSE);
			}
		}
	}


	// We did not find anything good, so set the cursor to the normal one
	ChangeCursor(pNormalCursor);

	// And set the status bar text
	StatusMsg.Load(_R(IDS_FREEHANDSTART), Tool::GetModuleID(GetID()));
	GetApplication()->UpdateStatusBarText(&StatusMsg);
}



/********************************************************************************************

>	BOOL FreeHandTool::GetStatusLineText(String_256* ptext, Spread*, DocCoord, ClickModifiers)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/1/95
	Inputs:		Others not used
	Outputs:	pText - pointer to a string that will hold the text
	Returns:	TRUE if it set the text, FALSE if not
	Purpose:	Sets the status line text on request from the app.

********************************************************************************************/

BOOL FreeHandTool::GetStatusLineText(String_256* ptext, Spread*, DocCoord, ClickModifiers)
{
	*ptext = StatusMsg;
	return TRUE;
}


/********************************************************************************************

>	BOOL FreeHandTool::IsCursorNearEndPoint(NodePath* pNodePath, const DocRect& BlobRect)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/94
	Inputs:		pNodePath - The Node Path that we want to test
				BlobRect - The rectangle that the endpoints have to be in to qualify
	Returns:	TRUE if it found an endpoint in the rect, FALSE if it did not
	Purpose:	Scans the path in the NodePath to see any of its endpoints lie inside the
				DocRect supplied. The DocRect should be the snapping rectangle around the
				cursor position. If this function finds that one of the endpoints is in the
				rect, it will remember the path, and the position of the endpoint so that
				when a new freehand line is started, this data can be passed in. This
				function also changes the cursor to the Join cursor and sets the status bar
				text if it finds a match.

********************************************************************************************/

BOOL FreeHandTool::IsCursorNearEndPoint(NodePath* pNodePath, const DocRect& BlobRect)
{
	// Find the path associated with this NodePath
	Path* pPath = &(pNodePath->InkPath);

	// Test to see if the cursor was anywhere near any of the paths endpoints
	INT32 CloseSlot;
	if (pPath->IsNearOpenEnd(BlobRect, &CloseSlot))
	{
		// Yep, so set the eventual starting position of any new freehand paths to the
		// endpoint in question
		pPath->SetPathPosition(CloseSlot);
		StartPos = pPath->GetCoord();
		
		// Also remember the path that we can Join with
		JoinInfo.pJoinPath = pNodePath;
		JoinInfo.IsNearEndPoint = TRUE;

		// And set the cursor
		ChangeCursor(pJoinCursor);

		// And the status bar text
		StatusMsg.Load(_R(IDS_FREEHANDSTARTJOIN), Tool::GetModuleID(GetID()));
		GetApplication()->UpdateStatusBarText(&StatusMsg);

		// now test to see if our nodepath is brushed
		NodeAttribute* pAttr = NULL;
		pNodePath->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBrushType), &pAttr);
		if (pAttr != NULL)
		{
			InitialiseJoinInfoForBrush((AttrBrushType*)pAttr, pNodePath, BlobRect.Centre());
		}
		
		// and tell our caller that we found a match
		return TRUE;
	}
	
	// No match was found
	return FALSE;
}




/********************************************************************************************

>	BOOL FreeHandTool::IsCursorNearPath(NodePath* pNodePath, INT32 Range, const DocCoord& PointerPos)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/10/94
	Inputs:		pNodePath - The Path that we want to see if we are near
				Range - The squared distance away from the path that we count as being
				close to the line
				PointerPos - The position of the mouse cursor
	Returns:	TRUE if we find that the path goes through the rectangle, FALSE if not
	Purpose:	This function tests to see if any point along the paths length is close to
				the cursor. If it finds that it is, then the cursor and the status
				bar help are changed and the information about the path is remembered in
				case a drag starts soon.

********************************************************************************************/

BOOL FreeHandTool::IsCursorNearPath(NodePath* pNodePath, INT32 Range, const DocCoord& PointerPos)
{
	// Find the path associated with this NodePath
	Path* pPath = &(pNodePath->InkPath);

	// Test to see if the cursor was anywhere near any of the paths endpoints
	INT32 CloseSlot = 0;
	double mu;
	if (pPath->IsPointCloseTo(PointerPos, Range, &CloseSlot, &mu))
	{
		// Yep, so set the eventual starting position of any new freehand paths to the
		// endpoint in question
		StartPos = pPath->ClosestPointTo(mu, CloseSlot);

		// Also remember the path that we can Join with
		JoinInfo.pJoinPath = pNodePath;
		JoinInfo.IsNearEndPoint = FALSE;
		JoinInfo.CloseSlot = CloseSlot;
		JoinInfo.Into = mu;

		// And set the cursor
		ChangeCursor(pModifyCursor);

		// And the status bar text
		StatusMsg.Load(_R(IDS_FREEHANDMODSTART), Tool::GetModuleID(GetID()));
		GetApplication()->UpdateStatusBarText(&StatusMsg);

		// now test to see if our nodepath is brushed
		NodeAttribute* pAttr = NULL;
		pNodePath->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBrushType), &pAttr);
		if (pAttr != NULL)
		{
			AttrBrushType* pAttrBrush = (AttrBrushType*)pAttr;
			InitialiseJoinInfoForBrush(pAttrBrush, pNodePath, PointerPos);
		}

		// tell the caller that some point along the path is close to the pointer
		return TRUE;
	}

	// This path is not close to the cursor
	return FALSE;	
}



/********************************************************************************************

>	void FreeHandTool::RenderToolBlobs(Spread* pSpread, DocRect* pClipRect)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/94
	Inputs:		pSpread - The spread that the blob is to appear on
				pClipRect - Pointer to the rect that contains the blobs
	Purpose:	Renders the Tools Blobs. This will be used to draw paths as they are retro
				fitted live, as the accuracy bar is dragged!

********************************************************************************************/

void FreeHandTool::RenderToolBlobs(Spread* pSpread, DocRect* pClipRect)
{
	// Can only draw the path if there is a path to draw
	if (IsPreviousPathValid && IsRetroPathValid && RetroPath!=NULL)
	{
		RenderRegion* pRegion = DocView::RenderOnTop(pClipRect, pSpread, ClippedEOR);
		while (pRegion)
		{
			// Draw a Cross Hair
			pRegion->SetLineColour(COLOUR_XOREDIT);
			pRegion->SetLineWidth(0);
			pRegion->DrawPath(RetroPath);

			// Get the next region in the list
			pRegion = DocView::GetNextOnTop(pClipRect);
		}
	}	
}




/********************************************************************************************

>	void FreeHandTool::SetSmoothness(INT32 Smooth)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/94
	Inputs:		Smooth - The new value for the smoothness
	Purpose:	Sets the freehand tools accuracy setting

********************************************************************************************/

void FreeHandTool::SetSmoothness(INT32 Smooth)
{
	Smoothness = Smooth;
}




/********************************************************************************************

>	INT32  FreeHandTool::GetSmoothness()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/94
	Returns:	The freehand accuracy setting
	Purpose:	for finding out the freehand tools current smoothing accuracy value

********************************************************************************************/

INT32  FreeHandTool::GetSmoothness()
{
	return Smoothness;
}



/********************************************************************************************

>	void FreeHandTool::PreviousPathInvalid()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/94
	Purpose:	Marks the previous path as invalid. After calling this function, attempts to
				re-fit a path by changing the smoothing values will be ignored. This function
				should be called when something happens that would make it impossible to
				re-fit the path (eg the original is translated or rotated or undone)

********************************************************************************************/

void FreeHandTool::PreviousPathInvalid()
{
	// The previous path is no longer valid, so set all
	// the flags and vars associated with it
	IsPreviousPathValid = FALSE;
	IsRetroPathValid = FALSE;
	IsSelectBlobsOnScreen = FALSE;
	PreviousPath = NULL;
	StartSlot = 0;
	NumSlots = 0;
	PathCRC = 0;

	// Tell the info bar to show that retro fitting is no longer possible
	if (pInfoBarOp != NULL)
		pInfoBarOp->SetRetroState(FALSE);
}




/********************************************************************************************

>	void FreeHandTool::SetPreviousPath(NodePath* PrevPath, INT32 Start, INT32 Len)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/94
	Inputs:		PrevPath - The path that has just been added to the tree. ie the one most
				recently drawn with the freehand tool
				Start - The Start slot at which the new section of path begins.
				Len - The number of slots that the new path section contains.
	Purpose:	Marks the previously drawn path as valid so that it can be retro fitted.
				Retro fitting will be allowed until PreviousPathInvalid() is called.
	SeeAlso:	FreeHandTool::PreviousPathInvalid

********************************************************************************************/

void FreeHandTool::SetPreviousPath(NodePath* PrevPath, INT32 Start, INT32 Len)
{
	// Set all the params associated with the previous path
	PreviousPath = PrevPath;
	StartSlot = Start;
	NumSlots = Len;

	// Calculate the CRC for the path.
	PathCRC = BuildPathCRC(PreviousPath);

	// Set the flags so that we know what is valid and what is not
	IsPreviousPathValid = TRUE;
	IsRetroPathValid = FALSE;
	IsSelectBlobsOnScreen = TRUE;

	// Tell the info bar to flash up a little indicator that retro fitting is possible
	if (pInfoBarOp != NULL)
		pInfoBarOp->SetRetroState(TRUE);

	// Clear out the retro fit path
	RetroPath->ClearPath();
}



/********************************************************************************************

>	BOOL FreeHandTool::IsRetroPathStillValid()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Returns:	TRUE if we can Retro fit the currently selected object
	Purpose:	Finds out if we are able to do retro fitting and is used by the info bar
				as well as the tool when it becomes the selected tool and when a drag of the
				slider starts.

********************************************************************************************/

BOOL FreeHandTool::IsRetroPathStillValid()
{
	// Need to find out if we can retro fit the path or not
	BOOL IsStillValid = FALSE;
	SelRange* Selected = GetApplication()->FindSelection();
	DocView* pDocView = DocView::GetSelected();

	// If there is only one thing selected
	if ((pDocView!=NULL) && (Selected->Count()==1))
	{
		// Get the selected node
		Node* pNode = Selected->FindFirst();

		// see if it is our path
		if ((pNode==PreviousPath) && (pNode->IS_KIND_OF(NodePath)))
		{
			// Build the CRC for the path now and if it matches, flag that we can do retro fitting
			INT32 CurrentCRC = BuildPathCRC(PreviousPath);
			if (CurrentCRC==PathCRC)
				IsStillValid = TRUE;
		}
	}

	// and return the state of IsStillValid
	return IsStillValid;
}


/********************************************************************************************

>	INT32 FreeHandTool::BuildPathCRC(NodePath* PrevPath)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/94
	Inputs:		PrevPath - The path to build a CRC checksum for.
	Returns:	The CRC checksum for the path, relative to the first coord
	Purpose:	Goes though the path and adds all the coordinate offsets together to
				come up with a number that will hopefully identify the path uniquly,
				so that we can tell if this is in fact the path we want, only translated.

********************************************************************************************/

INT32 FreeHandTool::BuildPathCRC(NodePath* PrevPath)
{
	INT32 TotalCRC = 0;

	// Get at the path in the NodePath
	Path* CRCPath = &(PrevPath->InkPath);

	// Find out about the coords and how many there are
	DocCoord* Coords = CRCPath->GetCoordArray();
	INT32 NumCoords = StartSlot+NumSlots;

	// go though the coords
	for (INT32 i=StartSlot; i<NumCoords; i++)
	{
		// Get the coords relative from the first coord
		// making this process transparent to trnslation
		// Add it in to the CRC total
		TotalCRC += (Coords[i].x-Coords[StartSlot].x) + (Coords[i].y-Coords[StartSlot].y);
	}

	// return the CRC I have
	return TotalCRC;
}



/********************************************************************************************

>	void FreeHandTool::TranslateTrackData()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/94
	Purpose:	If the path in the tree has been translated since it was drawn, we have to
				translate the original retro fit data to match. This functions checks if
				a translation is required, and translates all the coords if it is needed.

********************************************************************************************/

void FreeHandTool::TranslateTrackData()
{
	// if we do not have any track data then stop now
	if (TrackData==NULL)
		return;

	// Get the two coord sections of the paths
	DocCoord* TrackCoords = TrackData->GetCoordArray();
	DocCoord* TreeCoords = PreviousPath->InkPath.GetCoordArray();

	// see if the first coord matches
	if ((TrackCoords[0].x!=TreeCoords[StartSlot].x) || (TrackCoords[0].y!=TreeCoords[StartSlot].y))
	{
		// Nop, so find the offset
		INT32 Dx = TreeCoords[StartSlot].x - TrackCoords[0].x;
		INT32 Dy = TreeCoords[StartSlot].y - TrackCoords[0].y;

		// Loop through all the coords and add on the offset
		INT32 NumCoords = TrackData->GetNumCoords();
		for (INT32 i=0; i<NumCoords; i++)
		{
			// Offset the track data
			TrackCoords[i].x += Dx;
			TrackCoords[i].y += Dy;
		}
	}
}


/********************************************************************************************

>	void FreeHandTool::RetroFitChanging()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/94
	Purpose:	Tries to re-fit the previous path with the current smoothness setting

********************************************************************************************/

void FreeHandTool::RetroFitChanging()
{
	BOOL IsStillValid = IsRetroPathStillValid();
	if (IsStillValid)
	{
		// Find the spread that the previous path was on
		Spread* pSpread = PreviousPath->FindParentSpread();

		// Find the docview that we are looking at
		DocView* pDocView = DocView::GetSelected();

		// if both of those are ok, carry on
		if (pSpread!=NULL && pDocView!=NULL)
		{
			BlobManager* BlobMgr = GetApplication()->GetBlobManager();
			ENSURE(BlobMgr!=NULL, "Blob Manager was not there");

			// Translate the original data if the path moved
			TranslateTrackData();

			// Get rid of the Selection blobs if they are still on screen
			if (IsSelectBlobsOnScreen)
			{
				BlobMgr->RenderMyBlobs(NULL, pSpread, PreviousPath);
				IsSelectBlobsOnScreen = FALSE;
			}

			// Erase the old EORed path (if it was there)
			BlobMgr->RenderToolBlobsOff(this, pSpread, NULL);

			// Refit the path - First work out the current error factor
			double ScaleFactor = (pDocView->GetViewScale()).MakeDouble();
			double ErrorLevel = (64 + (160*Smoothness)) / ScaleFactor;
			ErrorLevel = ErrorLevel * ErrorLevel;

			// and perform all the maths
			if ((RetroPath!=NULL) && (TrackData!=NULL))
			{
				CurveFitObject CurveFitter(RetroPath, ErrorLevel);
				if (CurveFitter.Initialise(TrackData, TrackData->GetNumCoords()))
					CurveFitter.FitCurve();
			}

			// Draw in the new path
			IsRetroPathValid = TRUE;
			BlobMgr->RenderToolBlobsOn(this, pSpread, NULL);
			AreWeRetroFitting = TRUE;
		}
		else
		{
			IsStillValid = FALSE;
		}
	}

	// If the retro fit is not valid, then invalidate the path properly
	if (!IsStillValid)
	{
		// mark the path as invalid
		PreviousPathInvalid();
	}
}



/********************************************************************************************

>	void FreeHandTool::RetroFitFinished()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/94
	Purpose:	Should be called when any changes to the accuracy slider have been completed

********************************************************************************************/

void FreeHandTool::RetroFitFinished()
{
	// Start an operation here that will hide the original path and put a new one in
	// Make sure that we have a path that we are able to change
	if ((IsPreviousPathValid && AreWeRetroFitting) && (RetroPath!=NULL))
	{
		// Find the spread that the previous path was on
		Spread* pSpread = PreviousPath->FindParentSpread();
		if (pSpread!=NULL)
		{
			BlobManager* BlobMgr = GetApplication()->GetBlobManager();
			ENSURE(BlobMgr!=NULL, "Blob Manager was not there");

			// Draw the selection blobs back on again
			if (!IsSelectBlobsOnScreen)
			{
				BlobMgr->RenderMyBlobs(NULL, pSpread, PreviousPath);
				IsSelectBlobsOnScreen = TRUE;
			}

			// Erase the old EORed path (if it was there)
			BlobMgr->RenderToolBlobsOff(this, pSpread, NULL);
			
			// Build the undo info to change the existing path to the new one
			if (IsRetroPathValid)
			{
				// Make a copy of the node we are joining with
				Node* pNode;
				if (PreviousPath->NodeCopy(&pNode))
				{
					// Start a slow job
					BeginSlowJob();

					// Clear out the old data in the path
					NodePath* pNewPath = (NodePath*) pNode;

					// Go and replace the section that we have changed
					BOOL IsOk;
					if (StartSlot==0)
						IsOk = pNewPath->InkPath.RetroReplaceSection(StartSlot, NumSlots, RetroPath, TRUE);
					else
						IsOk = pNewPath->InkPath.RetroReplaceSection(StartSlot, NumSlots, RetroPath, FALSE);
					
					// If it worked, then build a bit of undo
					if (IsOk)
					{
						// Create a new Retro Fit Operation
						OpRetroFit* pRetroOp = new OpRetroFit;
						if (pRetroOp!=NULL)
						{
							// Make a note of the start slot and length of the section we are
							// changing as the operation causes this information to be invalidated
							INT32 OldStartSlot = StartSlot;
							INT32 OldNumSlots = RetroPath->GetNumCoords();
							if (pRetroOp->BuildUndo(PreviousPath, (NodePath*)pNode))
								SetPreviousPath(pNewPath, OldStartSlot, OldNumSlots);
						}
						else
						{
							// Did not get the memory for the operation
							IsOk = FALSE;
						}
					}

					// End the slow job we started
					EndSlowJob();

					// If things generally did not work, tell the happy user
					if (!IsOk)
						ToolInformError(_R(IDS_RETROOUTOFMEM));
				}
			}

			// Clear out the data in the fitted version of the path, as this is no longer needed
			// We do keep the Original track data though.
			RetroPath->ClearPath();
			IsRetroPathValid = FALSE;
		}
	}

	// We are no longer retro fitting the path, so reset the flag to indicate this
	AreWeRetroFitting = FALSE;
}



/********************************************************************************************

>	BOOL FreeHandTool::GetNewPaths()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/95
	Returns:	TRUE if everything worked like a dream, FALSE if it screwed up big time
	Purpose:	Gets rid of the old path data and creates new paths. One of the useful
				side effects of doing this is that it helps to keep the path data near the
				top of the heap, make the freehand tool seem more responsive.

********************************************************************************************/

BOOL FreeHandTool::GetNewPaths()
{
	// Delete the paths that we have
	DeletePaths();

	// Create some new paths
	if ((RetroPath==NULL) && (TrackData==NULL))
	{
		// Create a new path for the track data
		TrackData = new Path();
		if (TrackData==NULL)
			return FALSE;
	
		// Create a new path for the smoothed data
		RetroPath = new Path();
		if (RetroPath==NULL)
		{
			delete TrackData;
			TrackData = NULL;
			return FALSE;
		}
	
		// init the track buffer and fail if it does not work
		if (!TrackData->Initialise(48,48))
		{
			DeletePaths();
			return FALSE;
		}
	
		// Same with the retro fitting path
		if (!RetroPath->Initialise(12, 12))
		{
			DeletePaths();
			return FALSE;
		}

	}

	// All Worked
	return TRUE;
}



/********************************************************************************************

>	BOOL FreeHandTool::DeletePaths()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/95
	Returns:	TRUE - always
	Purpose:	Deletes the paths that hold the track data and the smoothed path and sets
				thier params to NULL.

********************************************************************************************/

BOOL FreeHandTool::DeletePaths()
{
	// Retro fitting is no longer an option
	PreviousPathInvalid();

	// Clear out the old Track Data
	if (TrackData!=NULL)
	{
		delete TrackData;
		TrackData = NULL;
	}
	// Clear out the smoothed version of the path
	if (RetroPath!=NULL)
	{
		delete RetroPath;
		RetroPath = NULL;
	}
	// always return TRUE at present
	return TRUE;
}


/********************************************************************************************

>	void FreeHandTool::DrawWithBrush()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/99
	Returns:	-
	Purpose:	Sets the brush handle member to that of the current default brush attribute.

********************************************************************************************/

void FreeHandTool::SetCurrentBrushHandle()
{
	Document* pDoc = Document::GetCurrent();
	if (pDoc == NULL)
	{
		ERROR3("No document");
		return;
	}

	AttributeManager* pAttrMgr = &(pDoc->GetAttributeMgr());
	if (pAttrMgr == NULL)
	{
		ERROR3("No attribute manager");
		return;
	}

	AttrBrushType* pAttr = (AttrBrushType*)(pAttrMgr->GetCurrentAttribute(CC_RUNTIME_CLASS(NodeRenderableInk), CC_RUNTIME_CLASS(AttrBrushType)));

	if (pAttr == NULL)
	{
		ERROR3("No current brush attribute");
		return;
	}
	m_BrushHandle = pAttr->GetBrushHandle();
	
}

/********************************************************************************************

>	void FreeHandTool::CreateBrush()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/99
	Returns:	-
	Purpose:	Called from the infobar, creates a brush definition from the ink objects in 
				the selection and passes it over to the BrushComponent.  Also creates a 
				GRenderBrush render region which is used for rendering the brush into when 
				the user begins to draw.
				
********************************************************************************************/

void FreeHandTool::CreateBrush()
{
	//TRACEUSER( "Diccon", _T("\nCreating Brush"));


		SelRange* pSel = GetApplication()->FindSelection();

		if (pSel == NULL)
		{
			return ;
		}

		m_NumInkNodes = 0;

		Node* pNode = pSel->FindFirst();
		
		if (pNode == NULL)
		{
			InformWarning(_R(IDS_BRUSHCREATENOSEL));
			return;
		}

		// first count the number of objects
		while (pNode != NULL)
		{
			if (pNode->IsAnObject())
			{
				if (BrushDefinition::ObjectCanCreateBrush((NodeRenderableInk*)pNode))
					m_NumInkNodes++;

				if (m_NumInkNodes >= MAX_BRUSH_OBJECTS)
				{
					InformWarning(_R(IDS_BRUSHCREATE_TOOMANY));
					break;
				}
			}
			pNode = pSel->FindNext(pNode);
		}

		if (m_NumInkNodes == 0)
		{
			InformWarning(_R(IDS_BRUSHCREATE_INVALID));
			return;
		}

		InitialiseBrushInkNodeArray((UINT32)m_NumInkNodes);
		
		DocRect BBox; // 

		// iterate through again and assign the selection pointers to the new array
		UINT32 Counter = 0;
		pNode = pSel->FindFirst();

		while (pNode != NULL && Counter < m_NumInkNodes)
		{
			if (pNode->IsAnObject())
			{				
				// check to see that its not too big
				BBox = ((NodeRenderableInk*)pNode)->GetBoundingRect();
				if (BBox.Height() > MAX_BRUSH_SPACING || BBox.Width() > MAX_BRUSH_SPACING)
				{
					InformWarning(_R(IDS_BRUSH_TOOBIG));
					return;
				}
				if (BrushDefinition::ObjectCanCreateBrush((NodeRenderableInk*)pNode))
				{
					m_BrushInkNodeArray[Counter++] = (NodeRenderableInk*)pNode;
				}
			}
			pNode = pSel->FindNext(pNode);
		}
	
		Spread *pBrushSpread = new Spread;
		if (pBrushSpread == NULL)
		{
			ERROR3("Couldn't create new brush spread");
			return;
		}

		Layer *pBrushLayer = new Layer(pBrushSpread, FIRSTCHILD, String_256(TEXT("Diccon did this")));
		if (pBrushLayer == NULL)
		{
			ERROR3("Couldn't create new brush (5)");
			delete pBrushSpread;
			return;
		}

		BOOL LineWidth = FALSE;
		BOOL Compound = FALSE;
		// And attach the clipart tree to our new layer
		Node* pLastNode = NULL;
//		AttrBrushType* pAttrBrush = NULL;

		for (UINT32 i = 0; i < m_NumInkNodes; i++)
		{
			NodeRenderableInk* pInk = m_BrushInkNodeArray[i];
			
			// do a quick line width check - we need this info later
			if (NodeHasLineWidth(pInk))
				LineWidth = TRUE;
			if (pInk->IsCompound())
				Compound = TRUE;
			pInk->MakeAttributeComplete();

			Node* pInsertNode = NULL;
			// OK if we're trying to make a brush out of an object that is already a brush then 
			// convert the object to normal shapes first.
		/*	pInk->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBrushType), 
													(NodeAttribute**)&pAttrBrush);

			if (pAttrBrush && pAttrBrush->GetBrushHandle() != BrushHandle_NoBrush)
			{
				//Turn the brush into a group with lots of nodepaths
				BrushBecomeAGroup BecomeA(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath), NULL);
				pAttrBrush->DoBecomeA(&BecomeA, pInk);
		
				// the brush will create a group out of itself and we want to retrieve that
				NodeGroup* pBrushGroup = BecomeA.GetNodeGroup();
				if (pBrushGroup != NULL)
					pInsertNode = pBrushGroup;
				else
				{
					ERROR3("Unable to make group from brush");
					return;
				}

				pAttrBrush = NULL;
			}
			else*/
			{   
				// the normal case
				pInsertNode = pInk->PublicCopy();
				
				if (pInsertNode == NULL)
				{
					ERROR3("Error copying node");
					return;
				}

				pInk->CopyChildrenTo(pInsertNode);
			
			}
			pInk->NormaliseAttributes();
			if (i==0)
				pInsertNode->AttachNode(pBrushLayer, FIRSTCHILD, FALSE, TRUE);
			else
				pInsertNode->AttachNode(pLastNode, NEXT, FALSE, TRUE);

			pLastNode = pInsertNode;
		}

		// convert any indexed colours in our subtree
		LineDefinition::ConvertIndexedColours(pBrushSpread);

		// now create the brush definition
		BrushDefinition* pBrushDef = new BrushDefinition(pBrushSpread);

		if (pBrushDef == NULL)
		{
			ERROR3("Couldn't allocate brush def");
			delete pBrushSpread;
			delete pBrushLayer;
			return;
		}

		// just double check
		if (!pBrushDef->IsActivated())
		{
			ERROR3("Brush failed to initialise correctly, aborting Create Brush");
			delete pBrushDef;
			delete pBrushSpread;
			delete pBrushLayer;
			return;
		}

		// set the default spacing
		DocRect BRect = pBrushDef->GetLargestBoundingBox();
		MILLIPOINT Width = BRect.Width();
		if (pBrushDef->GetNumBrushObjects() > 1 || LineWidth || Compound)
		{
			MILLIPOINT Spacing = (MILLIPOINT)(Width * 1.1);
			if (Spacing > MAX_BRUSH_SPACING)
				Spacing = MAX_BRUSH_SPACING;

			pBrushDef->SetSpacing(Spacing);
		}
		else
			pBrushDef->SetSpacing(Width / 5);

		// get the brush component of the document
		Document* pDoc = Document::GetCurrent();
		if (pDoc == NULL)
		{
			ERROR3("No document");
			delete pBrushSpread;
			delete pBrushLayer;
			return;
		}
		BrushComponent *pBrushComp = (BrushComponent*)(pDoc->GetDocComponent(CC_RUNTIME_CLASS(BrushComponent)));
		if (pBrushComp == NULL)
		{
			ERROR3("Couldn't get brush component");
			delete pBrushSpread;
			delete pBrushLayer;
			return;
		}
		BrushHandle NewHandle = pBrushComp->AddNewItem(pBrushDef, TRUE);
		
		if (NewHandle != BrushHandle_NoBrush)
		{
			if (pInfoBarOp != NULL)
				pInfoBarOp->AddBrush();

			// we want this brush selected
			BrushSelected(NewHandle, FALSE);

			// and we want to clear the document selection
			NodeRenderableInk::DeselectAll();
		}
		else
			delete pBrushDef;
		
}



/***********************************************************************************************

>	void FreeHandTool::LaunchBrushDefinitionDialog(BrushHandle Handle)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	6/10/99
	Inputs:		Handle of the brush definition to edit
	Returns:	-
	Purpose:	launches the brush edit dialog for the brush handle supplied.  A good idea to call
				this just after creating a brush so the user can edit it before using it.

***********************************************************************************************/

void FreeHandTool::LaunchBrushDefinitionDialog(BrushHandle Handle)
{
PORTNOTE("other", "Removed m_pGRenderBrush support");
#ifndef EXCLUDE_FROM_XARALX
	// first get the brush definition from the component
	// get the brush component from the document
	Document* pDoc = Document::GetCurrent();
	if (!pDoc) return;
	BrushComponent* pBrushComp = (BrushComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(BrushComponent));
	if (!pBrushComp) return;

	BrushDefinition* pBrushDef = pBrushComp->FindBrushDefinition(Handle);
	if (!pBrushDef) return;

	// now launch the edit dialog
	BrushData* pData = pBrushDef->GetBrushData();
	
	if (pData != NULL)
	{
		// get the line gallery, as we want to edit the definition version
		LineGallery* pLineGallery = LineGallery::GetInstance();
		if (pLineGallery != NULL)
		{
			// get a pointer to the gadget
			CBrushGadget* pGadget = pLineGallery->GetBrushGadget();
			if (pGadget != NULL)
			{
				// brush definitions don't know their own handle so we need to tell the data
				pData->m_BrushHandle = Handle;
			
				pGadget->EnableEditingBrushDefinition ();
				pGadget->HandleBrushButtonClick(pData->m_BrushHandle);
			}
			// we need to delete the data
			delete pData;
		}
	}
#endif
}

/***********************************************************************************************

>	void FreeHandTool::InitialiseBrushInkNodeArray(UINT32 NumObjects)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	6/10/99
	Inputs:		Number of objects to size the array at
	Returns:	-
	Purpose:	Clears out the m_BrushRefPtrArray if it is not empty, and sets the size 

***********************************************************************************************/

void FreeHandTool::InitialiseBrushInkNodeArray(UINT32 NumObjects)
{
	UINT32 i = 0;
	// we don't wish to delete any pointers as they point to objects
	// that are actually in the document.
	m_BrushInkNodeArray.clear();

	m_BrushInkNodeArray.resize(NumObjects);

	// fill the array with NULL objects so that we can check later
	// to see if our allocations have worked
	i = 0;
	while (i < m_BrushInkNodeArray.size())
	{
		m_BrushInkNodeArray[i++] = NULL;
	}

}


/********************************************************************************************

>	GRenderBrush* FreeHandTool::GetBigGRenderBrush()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/99
	Returns:	-
	Purpose:	Gets a GRenderBrush the size of the current view, then renders into it the 
				contents of the current view

********************************************************************************************/

GRenderBrush* FreeHandTool::GetBigGRenderBrush(Spread* pSpread)
{
PORTNOTE("other", "Removed m_pGRenderBrush support");
#ifndef EXCLUDE_FROM_XARALX
	m_bBrushReady = FALSE;
	Document* pDoc = Document::GetSelected();
	if (!pDoc)
		return NULL;

	DocView * pDocView = DocView::GetSelected();
	
	// get the docview rect and convert to spread coords 
	DocRect ViewRect = pDocView->GetDocViewRect(pSpread);
	pSpread->DocCoordToSpreadCoord(&ViewRect);
	
	// find out about our device 
	//View*	pView = View::GetSelectedView();
	CCamView* pCCamView = pDocView->GetConnectionToOilView(); 
	CDC* pDevContext = ((ScreenCamView*)pCCamView)->GetRenderDC();
	HDC DeviceHdc = pDevContext->GetSafeHdc();
	INT32 DeviceBPP = GetDeviceCaps(DeviceHdc, BITSPIXEL);
	
	// a couple more variables we need to get the GRenderBrush
	Matrix	RenderMat = pDocView->ConstructRenderingMatrix(pSpread);	
	FIXED16 Scale = 1;
//	INT32 i = 0;
	GRenderBrush* pGRender = NULL;	
	
	INT32 DeviceRes = GetDeviceCaps( DeviceHdc, LOGPIXELSX );
	// create a new GRenderBrush at 32BPP and 96 dpi
	pGRender = new GRenderBrush(ViewRect, RenderMat, Scale, 32, DeviceRes); 

	// Check to see if the Render Region is valid!
	if (pGRender == NULL)
	{
		ERROR3("Failed to allocate DIB");
		return NULL;
	}

	BOOL ok = pGRender->AttachDevice(pDocView, pDevContext, pSpread);
	if (!ok)
	{	
		delete pGRender;
		pGRender = NULL;	
		return NULL;
	}
 	
	if (DeviceBPP < 24)
		pGRender->SetDeepDIB(FALSE);
	else
		pGRender->SetDeepDIB(TRUE);

	
	pGRender->InitDevice();
	pGRender->m_DoCompression = TRUE;
	
	pGRender->InitAttributes();
	pGRender->RRQuality.SetQuality(QUALITY_MAX);
	pGRender->SetQualityLevel();
	pGRender->SetLineAttributes();
	pGRender->SetFillAttributes();


	// lleeeets geeet ready to render! 	
	if (!pGRender->StartRender())
	{
		ERROR3("Unable to setup bitmap");
		delete pGRender;
		return NULL;
	}
	
	// if we're less than 24 bit then we need to allocate smaller brush bitmaps as well
	if (DeviceBPP < 24)
	{
		if (!InitialiseBrushBitmaps(pGRender))
		{
			InformWarning(_R(IDS_BRUSHBITMAP_INVALID));
			delete pGRender;
			return NULL;
		}
	}

	// or blit, in this case, as it is quicker than rendering the view
	if (!pGRender->CaptureView(pSpread))
	{
		ERROR3("Unable to capture view");
		delete pGRender;
		return NULL;
	}


	m_bBrushReady = TRUE;
	return pGRender;
#else
	return NULL;
#endif
}


/********************************************************************************************

>	MILLIPOINT  FreeHandTool::GetBrushSpacing()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/99
	Returns:	The current brush spacing
	Purpose:	for finding out the freehand tools current brush spacing

********************************************************************************************/

MILLIPOINT FreeHandTool::GetBrushSpacing()
{
	return m_BrushSpacing;
}


/********************************************************************************************

>	void  FreeHandTool::SetBrushSpacing(MILLIPOINT Spacing)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/99
	inputs:		the spacing value to set
	Returns:	-
	Purpose:	for setting the freehand tools current brush spacing

********************************************************************************************/

void FreeHandTool::SetBrushSpacing(MILLIPOINT Spacing)
{
	m_BrushSpacing = Spacing;
}



/********************************************************************************************

>	BOOL FreeHandTool::InitialiseBlendRef(NodeRenderableInk* pInk)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/99
	Inputs:		pInk - inknode to generate the blendref from
	Outputs:	-
	Returns:	TRUE if successful, false otherwise
	Purpose:	To initialise a BlendRef object

********************************************************************************************/

BOOL FreeHandTool::InitialiseBlendRef(NodeRenderableInk* pInk)
{
	ERROR2IF(pInk == NULL,FALSE,"InkNode is NULL");
	
	if (m_pBlendRef != NULL)
	{	
		delete m_pBlendRef;
		m_pBlendRef = NULL;
	}
	m_pBlendRef = new BlendRef;
	
	BOOL ok = (m_pBlendRef != NULL);
	// initialise the blender
	Progress* pProgress = NULL;
	if (ok)  ok = m_pBlendRef->Initialise(pInk, -1, NULL, pProgress, FALSE, NULL);	

	return ok;
}



/********************************************************************************************

>	DocRect FreeHandTool::GetLargestInkBoundingRect()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/99
	Inputs:		-
	Outputs:	-
	Returns:	the bounding rect of the largest brush ink object, or an empty rect
	Purpose:	as above, note that the m_brushinknodes pointer must be initialised

********************************************************************************************/

DocRect FreeHandTool::GetLargestInkBoundingRect()
{
	DocRect LargestBRect;  //create an empty rect

	

	INT32 BiggestArea = 0;
	for (UINT32 i = 0; i < m_NumInkNodes; i++)
	{
		DocRect TempRect = m_BrushInkNodeArray[i]->GetBoundingRect();
		INT32 TempArea = TempRect.Height() * TempRect.Width();
		if (TempArea > BiggestArea)
		{
			LargestBRect = TempRect;
			BiggestArea  = TempArea;
		}
	}
	return LargestBRect;
}


/********************************************************************************************

>	BOOL FreeHandTool::NodeHasLineWidth(NodeRenderableInk* pInk)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/99
	Inputs:		pNode - inknode to test
	Outputs:	-
	Returns:	TRUE if this node has an applied linewidth > 0, FALSE if not
	Purpose:	To determine if this node has a linewidth applied to it that is greater than zero
				It turns out that the best way to find this out is to find out if it has an
				applied stroke colour, rather than look for line width.
********************************************************************************************/

BOOL FreeHandTool::NodeHasLineWidth(NodeRenderableInk* pNode)
{
	ERROR2IF(pNode == NULL, FALSE, "pNode is NULL in FreeHandTool::NodeHasLineWidth");
	NodeAttribute* pAttr = NULL;
	pNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeColour), &pAttr);
	
	if (pAttr == NULL)
		return FALSE;

	// ok so we've got an attribute, lets see if it has the NoColour flag set
	StrokeColourAttribute* pVal = (StrokeColourAttribute*)pAttr->GetAttributeValue();
	if (pVal != NULL)
	{
		DocColour Col = pVal->Colour;
		if (Col.IsTransparent())
			return FALSE;
	}

	return TRUE;
}


/********************************************************************************************

>	BrushDefinition FreeHandTool::GetSelectedBrush()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/99
	Inputs:		-
	Outputs:	-
	Returns:	pointer to the brush definition of the current default brush attribute 
				this can be NULL.
	Purpose:	In order to determine which brush to draw with when performing a real time
				draw brush operation.  

********************************************************************************************/

BrushDefinition* FreeHandTool::GetSelectedBrush()
{
	Document* pDoc = Document::GetCurrent();
	ERROR2IF(pDoc == NULL, NULL, "No document");
	
	BrushComponent* pBrushComp = (BrushComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(BrushComponent));
	if (pBrushComp == NULL)
	{
		ERROR3("No brush component");
		return NULL;
	}

	return pBrushComp->FindBrushDefinition(m_BrushHandle);
	
}



/********************************************************************************************

>	BOOL FreeHandTool::SetBrushUpdateState(UINT32 Update)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/99
	Inputs:		Update - when we want the brush to update next upon entering brush selected
						 UPDATE_NOW: updates immediately
						 UPDATE_ONIDLE: updates on the next idle
						 UPDATE_NEVER: will not update until the flag is changed
				
	Outputs:	-
	Returns:	TRUE if successful, FALSE if Update is invalid
	Purpose:	Tells us when we should next update the offscreen buffer used by the brush.
				Often it is wise ot wait until the next idle to update because that gives time
				for the screen to redraw after the brush combo has closed.

********************************************************************************************/

BOOL FreeHandTool::SetBrushUpdateState(UPDATE_STATE Update)
{
	if (Update < UPDATE_NOW || Update > UPDATE_NEVER)
	{
		ERROR3("Invalid update flag");
		return FALSE;
	}
	m_UpdateBrushState = Update;
	return TRUE;
}

/********************************************************************************************

>	void FreeHandTool::BrushSelected(BrushHandle Handle, BOOL ApplyToSelection)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/99
	Inputs:		Handle to the selected brush
				ApplyToSelection - whether or not we wish to apply this brush to the selection
	Outputs:	-
	Returns:	TRUE if successful, FALSE if something went wrong
	Purpose:	sets the brushhandle member of the tool to Handle. Designed to be called by the
				infobar when it receives a brushmessage.  Also initiates a number of steps that
				must be taken to ensure that real time drawing with this brush can proceed smoothly

********************************************************************************************/

BOOL FreeHandTool::BrushSelected(BrushHandle Handle, BOOL ApplyToSelection)
{
PORTNOTE("other", "Removed m_pGRenderBrush support");
#ifndef EXCLUDE_FROM_XARALX
	if (m_UpdateBrushState == UPDATE_NEVER)
		return TRUE;

	// first apply what we've got to the selection
	if (ApplyToSelection)
	{
		// if this fails then quit
		if (!ApplyBrushToSelection(Handle))
			return FALSE;

		m_UpdateBrushState = UPDATE_ONIDLE; // So that we update on the next idle rather than immediately
	}

	//TRACEUSER( "Diccon", _T("Start Brush selected\n"));
	m_BrushHandle = Handle;
	
	// note that we do not do the update straight unless the flag is set.  If it is not set then
	// we set the flag and then do the update on the next idle event.  The reason for this is that
	// if we do not wait then the brush combo will still be on the screen whilst we do our screen grab

	if (m_UpdateBrushState == UPDATE_NOW)
		m_UpdateBrushState = UPDATE_NEVER;
	else
	{	
		m_UpdateBrushState = UPDATE_ONIDLE;
		return TRUE;
	}

	// if its the 'null' brush then just kill the render region and leave
	if (m_BrushHandle == BrushHandle_NoBrush)
	{
		if (m_pGRenderBrush != NULL)
		{
			delete m_pGRenderBrush;
			m_pGRenderBrush = NULL;
		}
		return TRUE;
	}

	// if we are <24BPP display then make the whole thing again from scratch
	// as we will need different sized brush bitmaps and everything
	if (m_pGRenderBrush != NULL && m_pGRenderBrush->GetScreenDepth() < 24)
	{
		delete m_pGRenderBrush;
		m_pGRenderBrush = NULL;
	}

	String_32 ProgString = _T("Preparing brush, please wait..");
	Progress Hourglass(&ProgString, -1, FALSE);
	
	// set smoothness to zero
//	SetSmoothness(0);

	Spread* pCurrentSpread = Document::GetSelectedSpread();
	// make a new GRenderBrush for the new brush
	if (pCurrentSpread == NULL)
	{
		ERROR3("No Spread in FreeHandTool::BrushSelected");
		return FALSE;
	}
		
	// inform the join info of what we're going to draw
	JoinInfo.m_BrushHandle = m_BrushHandle;
	
	if (m_pGRenderBrush == NULL)
	{
		m_pGRenderBrush = GetBigGRenderBrush(pCurrentSpread);
	
		if (m_pGRenderBrush != NULL)
		{
			if (!InitialiseBrushBitmaps(m_pGRenderBrush))
			{
				InformWarning(_R(IDS_BRUSHBITMAP_INVALID));
				delete m_pGRenderBrush;
				m_pGRenderBrush = NULL;

				return FALSE;
			}

			m_bBrushReady = TRUE;

		}
		else
		{
			ToolInformError(_R(IDS_OUT_OF_MEMORY));
		
			return FALSE;
		}
	}
	else
	{
		// grab whats on the screen
		BOOL ok = m_pGRenderBrush->CaptureView(pCurrentSpread);

		// tell GDraw to use this bitmap
		if (ok) m_pGRenderBrush->SetupMainBitmap();
		return ok; //m_pGRenderBrush->CaptureView(pCurrentSpread);
	}
#endif

	return TRUE;
}


/********************************************************************************************

>	BOOL FreeHandTool::InitialiseBrushBitmaps(GRenderBrush* pGRender)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/99
	Inputs:		pGRender - the render region for which we want brush bitmaps for
	Outputs:	-
	Returns:	TRUE if successful, FALSE if something went wrong
	Purpose:	To work out how large our brush bitmaps must be and allocate them to the
				render region supplied

********************************************************************************************/

BOOL FreeHandTool::InitialiseBrushBitmaps(GRenderBrush* pGRender)
{
	ERROR2IF(pGRender == NULL, FALSE, "GRenderBrush is NULL in FreeHandTool::InitialiseBrushBitmaps");

	// first work out the largest possible size of our brush		
	BrushDefinition* pBrushDef = GetSelectedBrush();
	if (pBrushDef == NULL)
	{
		ERROR3("No brush definition");
		return FALSE;
	}
	// find out how big the biggest shape is
	DocRect LargestRect = pBrushDef->GetLargestPossibleRect(TRUE);
	if (LargestRect.IsEmpty())
	{
		ERROR3("Empty bounding rect");	
		return FALSE;
	}

	// Oh dear it still isn't big enough, guess I'll just have to resort to 
	// this nasty hack (please look away)
/*	INT32 Height = LargestRect.Height();
	INT32 Width = LargestRect.Width();

	LargestRect.Inflate((INT32)(Height * 0.1), (INT32)(Width *0.1));
*/
PORTNOTE("other", "Removed m_pGRenderBrush support");
#ifndef EXCLUDE_FROM_XARALX
	if (!pGRender->InitialiseBrushBitmaps(LargestRect))
	{
		return FALSE;
	}
#endif
	return TRUE;
}





/********************************************************************************************

>	void FreeHandTool::BrushDeleted(BrushHandle Handle)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/99
	Inputs:		Handle to the brush that has been deleted
	Outputs:	-
	Returns:	-
	Purpose:	Lets the tool know that a brush has been deactivated (we call it deleted in the UI
				but its actually still there).  If this brush is the current brush then we need
				to set the current brush to default.
				Also informs the infobar

********************************************************************************************/

void FreeHandTool::BrushDeleted(BrushHandle Handle)
{
	if (Handle == m_BrushHandle)
		BrushSelected(BrushHandle_NoBrush, FALSE);

	if (pInfoBarOp != NULL)
		pInfoBarOp->RemoveBrush(Handle);
}

/********************************************************************************************

>	void FreeHandTool::BrushFinished()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called by OpDrawBrush to tell the tool that the previous draw brush op has 
				finished.  Therefore the tool must prepare for the next one (if any)
********************************************************************************************/

void FreeHandTool::BrushFinished()
{
	// turn the colour picker back on
	SetColourEditorProcessing(TRUE);
	
	// turn background rendering back on 
	Document* pDoc = Document::GetSelected();
	DocView* pDocView = pDoc->GetFirstDocView();
	
	while (pDocView)
	{
		pDocView->SetPreventRenderView(FALSE);
		pDocView = pDoc->GetNextDocView(pDocView);
	}

	/*
	DocView* pDocView = DocView::GetSelected(); //gotta be getselected in case we have multiple views
	if (pDocView != NULL)
		pDocView->SetPreventRenderView(FALSE);
	*/
	m_bBrushReady = FALSE;
	m_UpdateBrushState = UPDATE_ONIDLE;
	///TRACEUSER( "Diccon", _T("Brush Finished\n"));
	BrushSelected(m_BrushHandle, FALSE);


}

/********************************************************************************************

>	void FreeHandTool::ScreenChanged()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called by the infobar to let the tool know that the screen has changed.  
				Essentially this means that if we are in brush drawing mode then we need to 
				blit the contents of the screen into our big render region
********************************************************************************************/

void FreeHandTool::ScreenChanged(BOOL WipeRegion)
{
	//TRACEUSER( "Diccon", _T("FreehandTool::ScreenChanged\n"));

	if (WipeRegion == TRUE)
	{
PORTNOTE("other", "Removed m_pGRenderBrush support");
#ifndef EXCLUDE_FROM_XARALX
		if (m_pGRenderBrush != NULL)
		{
			delete m_pGRenderBrush;
			m_pGRenderBrush = NULL;
		}
#endif
	}

	m_bBrushReady = FALSE;
	
	m_UpdateBrushState = UPDATE_ONIDLE;
}


/********************************************************************************************

>	BOOL FreeHandTool::ApplyBrushToSelection(BrushHandle Handle)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/99
	Inputs:		Handle to the selected brush
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Applies the chosen brush to all items in the selection via the attribute manager.
				However we do not want to make this a default attribute so if the selection is
				empty then nothing happens.

********************************************************************************************/

BOOL FreeHandTool::ApplyBrushToSelection(BrushHandle Handle)
{
	// first check to see if we have a selection
	SelRange* pSel = GetApplication()->FindSelection();
	if (pSel == NULL)
	{
		ERROR3("No selection in FreeHandTool::ApplyBrushToSelection");
		return FALSE;
	}

	// we have decided that if the selection is not empty then we will not try to 
	// go ahead and apply the attribute, as this gets annoying when you only want to 
	// draw with a different brush
	if (pSel->IsEmpty())
		return TRUE;

	// ok so we have a selection, now get an attribute node from the brush component
	NodeAttribute* pNewAttr = BrushComponent::CreateNode(Handle);
	if (pNewAttr != NULL)
		AttributeManager::AttributeSelected(pNewAttr);

	// we may have a scenario where we wish to apply the default brush
	if (Handle == BrushHandle_NoBrush)
	{
		// in this case we just need to make an attribute node
		NodeAttribute* pAttr = new AttrBrushType;
		if (pAttr != NULL)
			AttributeManager::AttributeSelected(pAttr);
	}
	return TRUE;
	
}
	

/********************************************************************************************

>	virtual BOOL FreeHandTool::OnIdle()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/99
	Inputs:		-
	Outputs:	-
	Returns:	FALSE always as we need more idles
	Purpose:
********************************************************************************************/

BOOL FreeHandTool::OnIdle()
{
	// just check that we have a document
	if (Document::GetCurrent() == NULL)
	{
		m_UpdateBrushState = UPDATE_NEVER;
		return FALSE;
	}

	if (m_UpdateBrushState == UPDATE_ONIDLE)
	{
#ifdef NEWFASTBRUSHES
		static Application* pApp = GetApplication ();

		BOOL doCaptureNow = TRUE;

		if (pApp)
		{
			if (!pApp->IsRenderingComplete ())
			{
				// TEST
				// there are still regions that are rendering,
				// we probably should NOT grab the screen yet!
				doCaptureNow = FALSE;
				TRACEUSER( "ChrisS", _T("Brush System:  Not finished rendering - just ignored screen capture request ...."));
			}
		}

		if (doCaptureNow)
		{
#endif
			// bodge so that we do not end up with the freehand cursor
PORTNOTE("other", "Removed cursor bodge");
#ifndef EXCLUDE_FROM_XARALX
			HCURSOR Cursor = GetCursor();
#endif
			m_UpdateBrushState = UPDATE_NOW;
			BrushSelected(m_BrushHandle, FALSE);
PORTNOTE("other", "Removed cursor bodge");
#ifndef EXCLUDE_FROM_XARALX
			SetCursor(Cursor);
#endif
#ifdef NEWFASTBRUSHES
		}
#endif

	}
	return FALSE;
}


/********************************************************************************************

>	void FreeHandTool::SetColourEditorProcessing(BOOL Value)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Tells the colour editor whether or not to do timer processing, we need to turn
				it off in order to draw a brush stroke.
********************************************************************************************/

void FreeHandTool::SetColourEditorProcessing(BOOL Value)
{
	ColourEditDlg* pColourEditor = ColourEditDlg::GetColourEditDlg();
	if (pColourEditor != NULL)
		pColourEditor->SetDoTimerProcessing(Value);
}


/********************************************************************************************

>	void FreeHandTool::InitialiseJoinInfoForBrush(AttrBrushType* pAttrBrush, Path* pPath, 
													DocCoord JoinPoint)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/2000
	Inputs:		pAttrBrush - the brush that we are going to join to
				NodepPath	   - the path that we are going to join to 
				JoinPoint  - the point where we will join
	Returns:	-
	Purpose:	Initialises the JoinInfo struct so that we correctly join the existing brush.

********************************************************************************************/

void FreeHandTool::InitialiseJoinInfoForBrush(AttrBrushType* pAttrBrush, NodePath* pPath, DocCoord JoinPoint)
{
	if (pAttrBrush == NULL || pPath == NULL)
	{
		ERROR3("NULL entry pointers in FreeHandTool::InitialiseJoinInfoForBrush");
		return;
	}

	// initialise joininfo with the data
	JoinInfo.m_BrushHandle = pAttrBrush->GetBrushHandle();

	// Now we need to find out the spacing of the first brush object.

	//find out how far along the path we are
	MILLIPOINT Distance = 0;
	if (!pPath->InkPath.GetDistanceToPoint(JoinPoint, &Distance))
	{
		ERROR3("Unable to find distance to point in FreeHandTool::InitialiseJoinInfoForBrush");
		return;
	}

	// didn't bother to write an access fn. for this, ask the path processor to work
	// out the spacing.
	BrushAttrValue* pVal = (BrushAttrValue*)pAttrBrush->GetAttributeValue();
	if (pVal != NULL)
	{
		PathProcessorBrush* pPPB = pVal->GetPathProcessor();
		if (pPPB != NULL)
		{
			// we want to get the scaling and spacing at this distance along the path
			double Scaling = 1.0;
			MILLIPOINT Spacing = pPPB->GetSpacing();
			pPPB->GetSpacingAndScalingAtDistance(Distance, &Spacing, &Scaling);
			JoinInfo.FirstBrushSpacing = (MILLIPOINT)((double)Spacing * Scaling);
			JoinInfo.BrushDistance = Distance;

			pPPB->CopyBrushData(&JoinInfo.m_BrushData);

			// get the stroke colour
			JoinInfo.UseLocalColour = pPPB->GetUseLocalFillColour();
			JoinInfo.UseNamedColour = pPPB->GetUseNamedColours();
			if ( JoinInfo.UseLocalColour == TRUE || JoinInfo.UseNamedColour == FALSE)
			{
				AttrStrokeColour *pStrokeCol = NULL;
				pPath->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeColour), (NodeAttribute**)&pStrokeCol);
				if (pStrokeCol)
				{
					JoinInfo.StrokeColour = pStrokeCol->Value.Colour;
				}
			}

		}

		JoinInfo.pAttrBrush = pAttrBrush;
		
	}



}


IMPLEMENT_SIMPLE_MODULE( FreeHandModule, MODULEID_FREEHAND, FreeHandTool, 
							"Freehand Tool", "To draw things", "Rik" );








