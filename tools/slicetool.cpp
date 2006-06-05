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
// Implementation of the Slice tool

// standard includes for a tool
#include "camtypes.h"
#include "oilfiles.h"
#include "csrstack.h"
//#include "viewrc.h"
#include "app.h"
#include "docview.h"
//#include "markn.h"

#include "layer.h"		// knowing about layers
#include "document.h"	// being able to get the spread
#include "slice.h"		// for the export slices op
#include "spread.h"
#include "ink.h"		// knowing about ink nodes
#include "sprdmsg.h"	// SpreadMsg
#include "layermsg.h"	// the layer messaging

#include "barcreationdlg.h" // for the create dlg

#include "slicetool.h"		// <******* Remember to change to include the tool's header file
//#include "sliceres.h"		// most of the resources that this tool uses
//#include "simon.h" // some string resources

// for the use of wix temple attribs
#include "cxfrech.h"
#include "attrval.h"
#include "userattr.h"
#include "tmpltatr.h"

// named set stuff
#include "ngcore.h"
#include "ngitem.h"
#include "ngsentry.h"

#include "selall.h" // for OPTOKEN_EDITSELECTNONE
#include "bitfilt.h" // for the get spread bounds

#include "slicehelper.h"
#include "opdupbar.h" // bar duplication
#include "blobs.h" // for the blob manager?

#include "selector.h"	// for OpSelectorDragBox
#include "guides.h"		// for NodeGuideline
#include "keypress.h"	// for KeyPress
#include "basebar.h"	// for BaseBar
#include "nodetxts.h"	// for TextStory
#include "opdragbx.h"	// for OpDragBox
#include "progress.h"	// for BeginSlowJob()

#include "vkextra.h"	// for CAMKEY(CC_)... key defs
//#include "justin.h"		// for _R(IDS_SLICE_MODE0) etc.
//#include "will2.h"		// for _R(IDS_SELOPTIONS)
//#include "resource.h"	// for _R(IDS_OUT_OF_MEMORY) and _R(IDS_OK)

//#include "clikdrag.h"	// for _R(IDS_TRANSLTRANS_)...

#include "opdupbar.h" // for the showlayer stuff

// enable slice-tool blobs.
#define SLICETOOL_BLOBS

void UpdateCurrentStateGadget(); // forward def

DECLARE_SOURCE( "$Revision$" );

#define MIN(a, b)	((a) < (b) ? (a) : (b))
#define MAX(a, b)	((a) < (b) ? (b) : (a))
#define CURSORID_UNSET 5000

CC_IMPLEMENT_MEMDUMP(SliceTool, DragTool)
CC_IMPLEMENT_DYNCREATE(SliceInfoBarOp, InformationBarOp)
CC_IMPLEMENT_DYNCREATE(OpSliceDragBox, OpSelectorDragBox)
CC_IMPLEMENT_DYNCREATE(OpSliceTranslate, TransOperation)

// Must come after the last CC_IMPLEMENT.. macro
#define new CAM_DEBUG_NEW     

// These are still char* while we wait for resource technology to be developed for modules
char* SliceTool::FamilyName	= "Slice Tools";
char* SliceTool::ToolName 	= "Slice Tool";
char* SliceTool::Purpose 	= "Slice manipulation";
char* SliceTool::Author 	= "Simon K";

// Init those other useful static vars
BOOL			SliceTool::CurrentTool 			= FALSE;
SliceInfoBarOp*	SliceTool::pSliceInfoBarOp		= NULL;

// global variable to remember what bar we were working on between switching tools
String_256	g_BarName;

/********************************************************************************************

>	SliceTool::SliceTool()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	Default Constructor.
				Other initialisation is done in SliceTool::Init which is called by the Tool Manager
	SeeAlso:	SliceTool::Init

********************************************************************************************/

SliceTool::SliceTool()
{
	// initialise all our member pointers to NULL.

	pcNormalSliceCursor		= NULL;
	pcAdjustCursor			= NULL;
	pcUnderCursor			= NULL;
	pcInsideCursor			= NULL;
	pcUnderAdjustCursor		= NULL;
	pcInsideAdjustCursor	= NULL;
	pcLeafCursor			= NULL;
	pcLeafAdjustCursor		= NULL;

	pcCurrentCursor		= NULL;
	pcALLCursor			= NULL;
	pcHorzGuideCursor	= NULL;
	pcVertGuideCursor	= NULL;

	StartSpread		= NULL;
	SelectionSpread	= NULL;
	SelectRange		= NULL;

	pClickSimpleNode	= NULL;
	pClickCompoundNode	= NULL;
	pLastClickNode		= NULL;
	pPreProcClickNode	= NULL;
}

/********************************************************************************************

>	SliceTool::~SliceTool()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	Destructor (Virtual). Does nothing.

********************************************************************************************/

SliceTool::~SliceTool()
{
}


/********************************************************************************************

>	BOOL SliceTool::Init( INT32 Pass )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Returns:	FALSE if it does not want to be created, TRUE otherwise
	Purpose:	Used to check if the Tool was properly constructed
	SeeAlso:	SliceTool::SliceTool

********************************************************************************************/

BOOL SliceTool::Init()
{
	// declare all ops used by SliceTool.
	BOOL	ok = OpSliceDragBox::Declare();
	if (ok) ok = OpSliceTranslate::Declare();
	ERROR2IF(!ok, FALSE, "Couldn't Declare all Ops in SliceTool::Init.");

	// This section reads in the infobar definition and creates an instance of
	// SliceInfoBarOp.  Also pSliceInfoBarOp, the ptr to the tool's infobar, is set up
	// after the infobar is successfully read and created.
	if (ok)
	{
		CCResTextFile 		file;				// Resource File
		SliceInfoBarOpCreate BarCreate;			// Object that creates SliceInfoBarOp objects

		 		ok = file.open(_R(IDM_SLICE_BAR), _R(IDT_INFO_BAR_RES));		// Open resource
		if (ok) ok = DialogBarOp::ReadBarsFromFile(file,BarCreate);	// Read and create info bar
		if (ok) file.close();									 	// Close resource

		ENSURE(ok,"Unable to load Slicebar.ini from resource\n"); 

		if (ok)
		{
			// Info bar now exists.  Now get a pointer to it
			String_32 str = String_32(_R(IDS_SLICETOOL_INFOBARNAME));
			DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(str);

					ok = (pDialogBarOp != NULL);
			if (ok) ok = pDialogBarOp->IsKindOf(CC_RUNTIME_CLASS(SliceInfoBarOp));
			if (ok) pSliceInfoBarOp = (SliceInfoBarOp*)pDialogBarOp;

			ENSURE(ok,"Error finding the Slice tool info bar");
		}
	}

	// Get a permanent pointer to the application's SelRange object.
	SelectRange = GetApplication()->FindSelection();

	// init the global var with the first bar name to create - ie Bar1
	g_BarName.MakeMsg(_R(IDS_BARNAME), 1);

	return (ok);
}


/********************************************************************************************

>	void SliceTool::Describe(void *InfoPtr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		InfoPtr - 	A pointer to a tool info block. It is passed cast to void* as
							the version of the tool is unknown at this point. Later versions 
							of the Tool class may have more items in this block, that this 
							tool will not use
	Outputs:	InfoPtr - 	The structure pointed to by InfoPtr will have had all the info
							that this version of the Tool knows about
	Purpose:	Allows the tool manager to extract information about the tool

********************************************************************************************/

void SliceTool::Describe(void *InfoPtr)
{
	// Cast structure into the latest one we understand.
	ToolInfo_v1 *Info = (ToolInfo_v1 *) InfoPtr;

	Info->InfoVersion = 1;
	
	Info->InterfaceVersion = GetToolInterfaceVersion();  // You should always have this line.
		
	// These are all arbitrary at present.
	Info->Version = 1;
	Info->ID      = GetID();
	Info->TextID  = _R(IDS_SLICE_TOOL);

	Info->Family  = FamilyName;
	Info->Name    = ToolName;
	Info->Purpose = Purpose;
	Info->Author  = Author;

	Info->BubbleID = _R(IDBBL_SLICE_TOOLBOX);
}

/********************************************************************************************

>	virtual void SliceTool::SelectChange(BOOL isSelected)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		isSelected	- TRUE  = tool has been selected
							- FALSE = tool has been deselected
	Outputs:	-
	Returns:	-
	Purpose:	Starts up and closes down the Slice tool
	Errors:		Debug warning if creating the cursor fails.
	SeeAlso:	-

********************************************************************************************/

void SliceTool::SelectChange(BOOL isSelected)
{
	if (isSelected)
	{
		// create the tool's cursors and push the default one onto the stack.
		if (!CreateCursors()) return;
		CurrentCursorID = CursorStack::GPush(pcNormalSliceCursor, FALSE);
		pcCurrentCursor = pcNormalSliceCursor;

		// This tool is now the current one.
		CurrentTool = TRUE;

		// Create and display the tool's info bar.
		if (pSliceInfoBarOp->Create())
			pSliceInfoBarOp->SetSliceTool(this);

		// register the Slice tool's blob preference with the app.
		BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
		if (pBlobMgr != NULL)
		{
			// we only use Tiny blobs.
			BlobStyle bsBlobs;
			bsBlobs.Tiny = TRUE;
			pBlobMgr->ToolInterest(bsBlobs);
		}

		// make a note that we mustn't ignore any selection-changed messages.
		m_fIgnoreSelChange = FALSE;

		// Update our info on the selection, and if necessary, render our tool blobs on.
		if (UpdateSelectionInfo())
			pBlobMgr->RenderToolBlobsOn(this, SelectionSpread, NULL);
	}
	else
	{
		// destroy the tool's cursors, if they exist.
		if (pcCurrentCursor != NULL)
		{
			CursorStack::GPop(CurrentCursorID);
			pcCurrentCursor = NULL;
			CurrentCursorID = 0;
		}
		DestroyCursors();

		// remove the info-bar's slice-tool pointer.
		pSliceInfoBarOp->SetSliceTool(NULL);

		// Remove the info bar from view by deleting the actual underlying window
		pSliceInfoBarOp->m_InfoBarCreated = FALSE;
		pSliceInfoBarOp->Delete();

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

		// Ensure our tool blobs have been rendered off the selected spread.
		BlobMgr->RenderToolBlobsOff(this, SelectionSpread, NULL);
	}
}



/********************************************************************************************

>	void SliceTool::SelectionHasChanged()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/10/1999
	Purpose:	A public function, for other people to call whenever they need
				to let the Slice tool know that the selection has changed.

				Allows the Slice tool to update its private information about the selection.
	Errors:		
	See also:	UpdateSelectionInfo();

********************************************************************************************/
void SliceTool::SelectionHasChanged()
{
	// we sometimes ignore selection-changed messages, so that we don't get our tool-blobs
	// in a tizzy.
	if (m_fIgnoreSelChange)
		return;

	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr==NULL) return;

	// ok, remove any of our previous tool blobs.
	pBlobMgr->RenderToolBlobsOff(this, SelectionSpread, NULL);

	// reset our set-sel flag, to indicate that we did not change the selection.
	// this flag needs to be cleared whenever the selection changes, in order to
	// keep a record of whether we changed the selection. whenever we update the
	// selection from within SliceTool, we must set this flag manually *after* we
	// tell everyone that the selection changed.
	m_bSliceToolSetSel = FALSE;

	// update the Slice tool's selection information,
	// and if necessary then render our tool blobs on.
	if (UpdateSelectionInfo())
		pBlobMgr->RenderToolBlobsOn(this, SelectionSpread, NULL);
}



/********************************************************************************************

>	void SliceTool::ViewChanged(const DocViewMsg& msg)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/07/2000
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	The SliceTool's view-changed message handler.
				This method is called directly from the message handler of SliceInfoBarOp,
				which lives further down in this file. Its sole purpose is to let us redraw
				our blobs correctly.
	Errors:		
	See also:	

********************************************************************************************/
void SliceTool::ViewChanged(const DocViewMsg& msg)
{
#ifdef SLICETOOL_BLOBS
	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr==NULL) return;

	switch (msg.State)
	{
		case DocViewMsg::DocViewState::SELABOUTTOCHANGE:
		{
			// Only bother rendering our tool blobs off if the new view is valid.
			if (msg.pNewDocView != NULL)
			{
				pBlobMgr->RenderToolBlobsOff(this, SelectionSpread, NULL);

				// so that we don't draw tool blobs onto any new view, we must ignore any
				// select-change message until the new view is installed.
				m_fIgnoreSelChange = TRUE;
			}

			break;
		}

		case DocViewMsg::DocViewState::SELCHANGED:
		{
			// if the new DocView is valid, then draw our blobs onto it.
			if (msg.pNewDocView != NULL)
				if (UpdateSelectionInfo())
					pBlobMgr->RenderToolBlobsOn(this, SelectionSpread, NULL);

			// ok, the new view is now here - we can stop ignoring sel-changed messages.
			m_fIgnoreSelChange = FALSE;
			break;
		}

		default:
			break;
	}
#endif
}



/********************************************************************************************

>	BOOL SliceTool::CreateCursors()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if all the Slice tool cursors have been successfully created
	Purpose:	Creates all the Slice tool cursors
	SeeAlso:	-

********************************************************************************************/

BOOL SliceTool::CreateCursors()
{
	// This tool has just been selected.  Create its cursors.
	pcNormalSliceCursor	= new Cursor(this, _R(IDC_POINTER_SLICE));
	pcAdjustCursor		= new Cursor(this, _R(IDC_POINTER_SLICE_ADJUST));
	pcUnderCursor		= new Cursor(this, _R(IDC_POINTER_SLICE_UNDER));
	pcInsideCursor		= new Cursor(this, _R(IDC_POINTER_SLICE_INSIDE));
	pcUnderAdjustCursor	= new Cursor(this, _R(IDC_POINTER_SLICE_UNDERADJUST));
	pcInsideAdjustCursor= new Cursor(this, _R(IDC_POINTER_SLICE_INSIDEADJUST));
	pcALLCursor			= new Cursor(this, _R(IDCSR_SEL_GRADPOINT));
	pcLeafCursor		= new Cursor(this, _R(IDC_POINTER_SLICE_LEAF));
	pcLeafAdjustCursor	= new Cursor(this, _R(IDC_POINTER_SLICE_LEAFADJUST));
	pcHorzGuideCursor	= new Cursor(this, _R(IDCSR_SEL_HGUIDE));
	pcVertGuideCursor	= new Cursor(this, _R(IDCSR_SEL_VGUIDE));

	// now check them...
	BOOL	ok = (pcNormalSliceCursor != NULL && pcNormalSliceCursor->IsValid());
	if (ok) ok = (pcAdjustCursor != NULL && pcAdjustCursor->IsValid());
	if (ok) ok = (pcUnderCursor != NULL && pcUnderCursor->IsValid());
	if (ok) ok = (pcInsideCursor != NULL && pcInsideCursor->IsValid());
	if (ok) ok = (pcUnderAdjustCursor != NULL && pcUnderAdjustCursor->IsValid());
	if (ok) ok = (pcInsideAdjustCursor != NULL && pcInsideAdjustCursor->IsValid());
	if (ok) ok = (pcALLCursor != NULL && pcALLCursor->IsValid());
	if (ok) ok = (pcLeafCursor != NULL && pcLeafCursor->IsValid());
	if (ok) ok = (pcLeafAdjustCursor != NULL && pcLeafAdjustCursor->IsValid());
	if (ok) ok = (pcHorzGuideCursor != NULL && pcHorzGuideCursor->IsValid());
	if (ok) ok = (pcVertGuideCursor != NULL && pcVertGuideCursor->IsValid());
	if (!ok)
		DestroyCursors();

	return ok;
}

/********************************************************************************************

>	void SliceTool::DestroyCursors()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroys all the Slice tool cursors
	SeeAlso:	-

********************************************************************************************/

void SliceTool::DestroyCursors()
{
	if (pcNormalSliceCursor != NULL) delete pcNormalSliceCursor;
	if (pcAdjustCursor != NULL) delete pcAdjustCursor;
	if (pcUnderCursor != NULL) delete pcUnderCursor;
	if (pcInsideCursor != NULL) delete pcInsideCursor;
	if (pcUnderAdjustCursor != NULL) delete pcUnderAdjustCursor;
	if (pcInsideAdjustCursor != NULL) delete pcInsideAdjustCursor;
	if (pcALLCursor != NULL) delete pcALLCursor;
	if (pcLeafCursor != NULL) delete pcLeafCursor;
	if (pcLeafAdjustCursor != NULL) delete pcLeafAdjustCursor;
	if (pcHorzGuideCursor != NULL) delete pcHorzGuideCursor;
	if (pcVertGuideCursor != NULL) delete pcVertGuideCursor;
}

/********************************************************************************************

>	void SliceTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		PointerPos 	- 	The DocCoord of the point where the mouse button was clicked
				Click 		-	Describes the type of click that was detected. 
				ClickMods 	-	Indicates which buttons caused the click and which modifers were
								pressed at the same time
				pSpread 	- 	The spread in which the click happened
	Returns:	-
	Purpose:	To handle a Mouse Click event for the Slice Tool.
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers

********************************************************************************************/

void SliceTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )
{
	// we ignore any clicks with the Menu mouse button.
	if (ClickMods.Menu)
		return;

	// remember the mouse position, in case we're starting a drag.
	if (Click == CLICKTYPE_SINGLE)
	{
		ClickStart = PointerPos;
		StartSpread = pSpread;
	}

	// remember the click modifiers for later use in the click-processing code.
	this->ClickMods = ClickMods;
	TypeOfClick = Click;

	// update our spread and bounds info about the selection.
	UpdateSelectionInfo();

	// refresh our ptr to the app's selection, for use in the click-processing code.
	SelectRange = GetApplication()->FindSelection();
	RangeControl rc = SelectRange->GetRangeControlFlags();
	rc.IgnoreInvisibleLayers = TRUE;
	SelectRange->Range::SetRangeControl(rc);

	// we process clicks in three stages, stopping as soon as
	// a stage successfully processes the click.

	// this function checks for and lets us drag guidelines.
	if (!PreProcessClick())
	{
		// this function is a carry-over from the Selector tool code.
		// it currently does nothing.
		if (!ProcessObjectClick())
		{
			// this function deals with normal mouse-click handling.
			PostProcessClick();
		}
	}
}



/*******************************************************************

>	virtual BOOL SliceTool::PreProcessClick()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/10/1999
	Inputs:		
	Outputs:	
	Returns:	TRUE if the mouse event was handled here,
				FALSE otherwise.
	Purpose:	Allows a click to be handled before going through the
				main click-handling code.
				This function checks for a clicked Node, and allows it
				to respond to the click.
	Errors:		
	See also:	SliceTool::OnClick

*******************************************************************/
BOOL SliceTool::PreProcessClick()
{
	// look for a Node which may be interested in this click, and allow it to respond.
	pPreProcClickNode = FindPreProcessClickNode(StartSpread, ClickStart);
	if (pPreProcClickNode != NULL)
		if (pPreProcClickNode->OnClick(ClickStart, TypeOfClick, ClickMods, StartSpread))
			return TRUE;

	// no preprocess node, or it didn't want the click.
	return FALSE;
}



/*******************************************************************

>	virtual NodeRenderableInk* SliceTool::FindPreProcessClickNode(Spread* pSpread,
																DocCoord ClickPos,
																BOOL bInterruptible = FALSE)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/10/1999
	Inputs:		pSpread			pointer to a Spread.
				dcPos			mouse click position in pSpread.
				bInterruptible	whether or not this function can be
								aborted by mouse movement.
	Outputs:	
	Returns:	Pointer to a node interested in processing the OnClick message.
	Purpose:	Scans the nodes in the spread, to see if any of them want to
				deal with this message before its processing continues.

				Currently, only looks for NodeGuidelines.
	Errors:		
	See also:	SliceTool::PreProcessClick()

*******************************************************************/
NodeRenderableInk* SliceTool::FindPreProcessClickNode(	Spread* pSpread,
														DocCoord ClickPos,
														BOOL bInterruptible )
{
	ERROR2IF(pSpread == NULL, NULL, "SliceTool::FindPreProcessClickNode- pSpread is NULL");

	// initialise vars.
	NodeRenderableInk* pFoundNode = NULL;
	BOOL bFound = FALSE;

	// look for a visible, editable guides layer.
	Layer* pLayer = pSpread->FindFirstLayer();
	while (pLayer != NULL && !bFound)
	{
		if (pLayer->IsGuide() && !pLayer->IsLocked() && pLayer->IsVisible())
		{
			// We have a layer that's also a guide layer
			// Now look for the guidelines in this layer
			Node* pNodeInLayer = pLayer->FindFirstChild(CC_RUNTIME_CLASS(NodeGuideline));
			while (pNodeInLayer != NULL && !bFound)
			{
				pFoundNode = (NodeGuideline*)pNodeInLayer;

				// found a node - now hit-test its bounds.
				DocRect Rect = pFoundNode->GetBoundingRect(FALSE,TRUE);
				bFound = (Rect.ContainsCoord(ClickPos));

				pNodeInLayer = pNodeInLayer->FindNext(CC_RUNTIME_CLASS(NodeGuideline));
			}
		}

		pLayer = pLayer->FindNextLayer();
	}

	if (bFound)
	{
		// OK, we have found a node interested in a preprocess click
		// We now have to ensure that it's not being obscured visually by another node
		NodeRenderableInk* pNode = NULL;
		if (bInterruptible)
		{
			// Allow the hit-test to be interrupted if the mouse moves!
			Node* pInterruptedNode = NULL;
			pNode = NodeRenderableInk::FindSimpleAtPoint(	pSpread,
															ClickPos,
															NULL,
															&pInterruptedNode);
			// If hit-test was interrupted then don't say anything about what's under the pointer!
			if (pInterruptedNode!=NULL)
				return NULL;
		}
		else
		{
			// Can't be interrupted by mouse movement so just go for it...
			pNode = NodeRenderableInk::FindSimpleAtPoint(pSpread,ClickPos);
		}

		if (pNode)
		{
			// Find out whether the hit node is in front of the guideline or not.
			// If it is, then clear the Found flag.
			Layer* pLowerLayer = (Layer*) pFoundNode->FindParent(CC_RUNTIME_CLASS(Layer));	// The guideline layer
			Layer* pNodeLayer = (Layer*) pNode->FindParent(CC_RUNTIME_CLASS(Layer));		// The layer containing the node
			// Make sure GuideLayer comes after NodeLayer
			do
			{
				pLowerLayer = pLowerLayer->FindNextLayer();		// Find the layer above the last one tested
			}
			while (pLowerLayer && pLowerLayer!=pNodeLayer);		// Keep going while there is a layer
																// and that layer isn't the one we're looking for
			// Get here when either we've run out of layers or we've found the layer we want
			if (pLowerLayer && pLowerLayer==pNodeLayer)			// If found layer above guide layer
				bFound=FALSE;									// Then flag that the guideline is obscured
		}
	}

	if (!bFound)
		pFoundNode = NULL;

	return pFoundNode;
}



/*******************************************************************

>	BOOL SliceTool::ProcessObjectClick()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/10/1999
	Inputs:		
	Outputs:	
	Returns:	FALSE always.
	Purpose:	In the selector tool, this function OnClick()'s each object
				in the tool's range if the Selector tool is using Object blobs.
				We don't, so we won't.
	Errors:		
	See also:	SelectorTool::ProcessObjectClick

*******************************************************************/
BOOL SliceTool::ProcessObjectClick()
{
	return FALSE;
}



/*******************************************************************

>	void SliceTool::PostProcessClick()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/10/1999
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	Provided a click wasn't eaten by a pre- or mid- processor,
				we deal with it here. This passes the event on to specific
				handler routines, depending on its type (TypeOfClick):

				CLICKTYPE_SINGLE	HandleSingleClick
				CLICKTYPE_DOUBLE	HandleDoubleClick
				CLICKTYPE_DRAG		HandleDragClick
				CLICKTYPE_UP		HandleButtonUp
	Errors:		
	See also:	

*******************************************************************/
void SliceTool::PostProcessClick()
{
	switch (TypeOfClick)
	{
	case CLICKTYPE_SINGLE:
		HandleSingleClick();
		break;

	case CLICKTYPE_DOUBLE:
		HandleDoubleClick();
		break;

	case CLICKTYPE_DRAG:
		HandleDragClick();
		break;

	case CLICKTYPE_UP:
		HandleButtonUp();
		break;

	default:
		// unrecognised click-type - do nothing.
		break;
	}
}



/*******************************************************************

>	virtual void SliceTool::HandleSingleClick()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/10/1999
	Purpose:	Single mouse-click event handler.
	Errors:		
	See also:	

*******************************************************************/
void SliceTool::HandleSingleClick()
{
	// work out what modifiers apply to this click.
	SetKeyDownCursor(ClickMods);

	// Find out which object, if any, was clicked on.  We hit-detect both the simple node
	// that was clicked and any top-level compound object it may be part of.
	pClickSimpleNode = NodeRenderableInk::FindSimpleAtPoint(StartSpread, ClickStart);
	pClickCompoundNode = NodeRenderableInk::FindCompoundFromSimple(pClickSimpleNode);
}



/*******************************************************************

>	void SliceTool::HandleDoubleClick()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/10/1999
	Purpose:	Double mouse-click event handler.
				Currently, passes the event to HandleSingleClick().
	Errors:		
	See also:	

*******************************************************************/
void SliceTool::HandleDoubleClick()
{
	HandleSingleClick();
}



/*******************************************************************

>	void SliceTool::HandleDragClick()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/10/1999
	Purpose:	Mouse-drag event handler.
	Errors:		
	See also:	

*******************************************************************/
void SliceTool::HandleDragClick()
{
	// check for a forced translate, and perform a translate
	// operation if appropriate.
	if (IsTranslateShortcut(ClickMods))
	{		
		if (SelectionSpread != NULL)
		{
			// There is a selection, so start a translate drag...
			DoTranslate();
		}
		return;
	}
	
	// Right, we clicked on either an object (possibly already selected) or
	// blank paper.  First, check if there is an object at the click position.
	if (pClickCompoundNode == NULL)
	{
		// There is only blank paper at the click point, so assume the user is
		// trying to start a marquee selection drag. Run the drag-box operation.
		DoDragBox();
		return;
	}

	// Is the clicked object already selected?  Note that the user may be trying to click on
	// a simple object selected inside a group that itself is not selected, so we must be
	// careful to check the state of the right nodes here.
	//
	// The logic here is:
	// If the clicked simple node or any of it's parents are selected
	// Then DON'T alter the selection to reflect the clicked object!
	//
	BOOL SimpleInSelected = FALSE;				// So far haven't encountered any selected nodes
	Node* pNode = pClickSimpleNode;				// Make a working pointer and initialise it
	do
	{
		if (pNode->IsSelected())				// If the working node is selected
		{
			SimpleInSelected = TRUE;			// Then the simple node or one of its parents are
			break;								// selected so we don't need to change the selection!
		}
		pNode = pNode->FindParent();			// Else check the working node's parent
	}
	while (pNode != pClickCompoundNode->FindParent());// until we've reached the compound node's parent
												// (Allows the compound node itself to be checked)

	if (!SimpleInSelected)
	{
		// No.  If the click was with the left button we must deselect all other objects.
		// If the clicked object isn't selected, we can't run a transform on it.  Hence we must
		// select it, but we prevent it being redrawn as selected (with its little blobs).
		if (!ClickMods.Adjust)
		{
			// Normal click, so deselect everything before we select the clicked node...
			NodeRenderableInk::DeselectAll(TRUE, FALSE);
		}
		else
		{
			// We clicked with the right button, but if the click was in a different spread to
			// the selection we have to deselect everything anyway.
			Node* pNode = SelectRange->FindFirst();
			if (pNode != NULL && pNode->FindParentSpread() != SelectionSpread)
			{
				// Clicked node in a different spread from previous selection, so clear selection
				NodeRenderableInk::DeselectAll(TRUE, FALSE);
			}
		}

		// Now that the selection state of all other objects is dealt with, make sure the
		// clicked object, as well as objects sharing its name, is selected.
		//
		// Currently, SelectAllSetsOfThisNode indirectly does a SelectRange->Update(TRUE).
		String_256 SetName;
		SetName.Empty();
		SliceHelper::SelectAllSetsOfThisNode(pClickCompoundNode, SetName, FALSE);
		if (SetName.IsEmpty())
		{
			pClickCompoundNode->Select(TRUE);

			// force a broadcast of the changed selection, so that our info-bar is informed.
			SelectRange->Update(TRUE);
		}
	}

	// Finally, run a transform on the selected object(s).  Of course, the user may not be
	// trying to drag the selection - if so the button will come up before a significant
	// drag has occurred and we can take it from there.
	MakeSelectionValidForDrag();

	// do the drag.
	DoTranslate();

	// remember that _we_ changed the selection.
	m_bSliceToolSetSel = TRUE;
}



/********************************************************************************************

>	void SliceTool::HandleButtonUp()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>, from JustinF's SelectorTool::HandleButtonUp code
	Created:	08/10/1999
	Purpose:	Mouse button-up event handler.
	Errors:		
	See also:	

********************************************************************************************/
void SliceTool::HandleButtonUp()
{
	// if we changed the selection last, or the SelRange cannot provide a usable
	// last-selection then use our own record. otherwise, ask the SelRange.
	Node* pLastSelNode = NULL;
	if (m_bSliceToolSetSel && pLastClickNode != NULL)
	{
		pLastSelNode = pLastClickNode;
	}
	else
	{
		pLastSelNode = SelectRange->GetLastSelectedNode();
		if ( pLastSelNode == NULL || !pLastSelNode->IsAnObject() )
			pLastSelNode = pLastClickNode;
	}

	// Find out what we should do with the click...
	ClickActionCode action = CLICKACTION_NONE;
	NodeRenderableInk* pActionNode = NULL;
	action = DetermineClickAction(	&pActionNode, (NodeRenderableInk*)pLastSelNode,
									pClickSimpleNode, pClickCompoundNode, StartSpread,
									ClickStart, ClickMods );

	// Act upon the information...
	switch (action)
	{
		//-------------------------------------------------//
		// No action required...
		case CLICKACTION_NONE:
			break;

		//-------------------------------------------------//
		// Anything that's selected must be deselected...
		case CLICKACTION_SELNONE:
		{
			// Don't clear selection if was an adjust click
			// (Change requested by Charles and Alan Burns)
			if (!ClickMods.Adjust)
			{
				// Markn 29/9/95: We ignore this if the click happened on a guideline.
				// If another type of node needs to behave like this, then a virt func in node
				// will be required instead of the hideous IS_A() clause in the 'if' statement
				if (pPreProcClickNode == NULL || !IS_A(pPreProcClickNode,NodeGuideline))
				{
					NodeRenderableInk::DeselectAll(TRUE, TRUE);
					pLastClickNode = NULL;
				}
			}
			break;
		}

		//-------------------------------------------------//
		// The action node must be selected or toggled...
		case CLICKACTION_SELNODE:
		case CLICKACTION_SELUNDER:
		case CLICKACTION_SELUNDERCYCLE:
		case CLICKACTION_SELUNDERFAIL:
		case CLICKACTION_SELUNDERFAIL2:
		case CLICKACTION_SELINSIDE:
		case CLICKACTION_SELINSIDECYCLE:
		case CLICKACTION_SELINSIDEFAIL:
		case CLICKACTION_SELINSIDEFAIL2:
		case CLICKACTION_SELLEAF:
		{
			ERROR3IF(pActionNode == NULL, "Action and ActionNode don't agree!");

			// if we aren't Adjust'ing, ensure no previous selection.
			if (!ClickMods.Adjust)
				NodeRenderableInk::DeselectAll(TRUE, FALSE);

			// try to select all nodes sharing the same name as the clicked node.
			String_256 SetName;
			SetName.Empty();
			SliceHelper::SelectAllSetsOfThisNode(pActionNode, SetName, ClickMods.Adjust);

			// if the clicked node has no name then the above code will do nothing,
			// so we need to do a normal select, accounting for Adjust.
			if (SetName.IsEmpty())
			{
				if (ClickMods.Adjust)
				{
					// If Adjust is applied, toggle the state of the action node.
					if (pActionNode->IsSelected())
						pActionNode->DeSelect(TRUE);
					else
						pActionNode->Select(TRUE);
				}
				else
				{
					pActionNode->Select(TRUE);
				}

				// force a broadcast of the changed selection state, so our info-bar updates.
				SelectRange->Update(TRUE);
			}

			// update our record of the last clicked node.
			pLastClickNode = pActionNode;

			// make a note that we changed the selection.
			m_bSliceToolSetSel = TRUE;

			break;
		}
		//-------------------------------------------------//
		default:
			ERROR3("Unknown Click action code!");
			break;
	}; // switch (action)

	// Make sure the cursor reflects which keys are down, now that the mouse button has
	// been released.
	SetKeyDownCursor(ClickModifiers::GetClickModifiers());
}



/*******************************************************************

>	virtual BOOL SliceTool::OnKeyPress(KeyPress *pKeyPress)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/10/1999
	Inputs:		pKeyPress	pointer to a key-press object.
	Outputs:	
	Returns:	
	Purpose:	key-press event handler. Catch CTRL/ALT/SHIFT modifiers
				and change the status-line/cursor appropriately.
	Errors:		
	See also:	

*******************************************************************/
BOOL SliceTool::OnKeyPress(KeyPress *pKeyPress)
{
	// Find the current state of the "click" keyboard modifiers...
	ClickMods = ClickModifiers::GetClickModifiers();

	switch (pKeyPress->GetVirtKey())
	{
	case CAMKEY(CC_MOD_ADJUST):						// bit 0 of fKeyStates (SHIFT)
	case CAMKEY(CC_MOD_ALTERNATIVE):					// bit 1 of fKeyStates (ALT)
	case CAMKEY(CC_MOD_CONSTRAIN):					// bit 2 of fKeyStates (CONTROL)
		// apparently, this is a bodge for speed.
		// see SelectorTool::OnKeyPress for details :-)
		break;

	case CAMKEY(TAB):								// moves selection to next rendered node
		if (pKeyPress->IsPress()) HandleTabKey(ClickMods.Adjust);
		break;

	case CAMKEY(HOME):								// select first object in render order
		if (pKeyPress->IsPress())
		{
			if (SelectionSpread != NULL)
				NodeRenderableInk::DeselectAll();
			HandleTabKey(FALSE);
		}
		break;

	case CAMKEY(END):								// select last object in render order
		if (pKeyPress->IsPress())
		{
			if (SelectionSpread != NULL)
				NodeRenderableInk::DeselectAll();
			HandleTabKey(TRUE);
		}
		break;

	default:									// not interested in processing this
		return FALSE;
	}	

	// If we processed a click modifier then update the cursor and return that we processed
	// the keystroke.
	SetKeyDownCursor(ClickMods);

	// Yes, we processed this key-event.
	return TRUE;
}



/*******************************************************************

>	void SliceTool::HandleTabKey(BOOL bIsShifted)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/10/1999
	Purpose:	Tab keypress event handler.
	Errors:		
	See also:	

*******************************************************************/
void SliceTool::HandleTabKey(BOOL bIsShifted)
{
}



/*******************************************************************

>	void SliceTool::SetKeyDownCursor(ClickModifiers cmMods)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/10/1999
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	Decodes the bit-field fKeyStates, indicating which combination
				of modifier keys are down, and sets the cursor appropriately.
	Errors:		
	See also:	

*******************************************************************/
void SliceTool::SetKeyDownCursor(ClickModifiers cmMods)
{
	// Get current position information for call to change pointer shape...
	Spread*  pSpread;
	DocCoord dcMousePos;
	if (DocView::GetCurrentMousePos(&pSpread, &dcMousePos) &&
		Tool::GetCurrentID() == TOOLID_SLICETOOL &&
		!BaseBar::IsDragging())
	{
		// Call nice central routine to figure out what pointer shape to show...
		// (Set the status bar text while we're at it.)
		String_256 Str;
		Cursor* pPtr;
		FigureUserFeedback(pSpread, dcMousePos, cmMods, TRUE, &Str, &pPtr);
		if (CurrentCursorID != CURSORID_UNSET)
			CursorStack::GSetTop(pPtr, CurrentCursorID);
		if (!(Str.IsEmpty()))
			SetStatusText( &Str );
	}
}



/********************************************************************************************

>	void SliceTool::OnMouseMove( DocCoord PointerPos,Spread* pSpread, ClickModifiers ClickMod )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		PointerPos 	- 	The DocCoord of the point where the mouse has moved to
				pSpread 	- 	The spread in which the move occurred
				ClickMods	- 	The state of the various modifiers at the time of the mouse move
	Returns:	TRUE if it handled the Click, FALSE otherwise
	Purpose:	To handle a Mouse Move event for the Slice Tool.
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers

********************************************************************************************/

void SliceTool::OnMouseMove(DocCoord PointerPos,Spread* pSpread,ClickModifiers ClickMods)
{
	// Display status bar text for the current position
//	DisplayStatusBarHelp(PointerPos, pSpread, ClickMods);

	// If there isn't any selection, or it's in a different spread, then do nothing.
	if (SelectionSpread == NULL || SelectionSpread != pSpread) return;
	
	String_256 str;
	Cursor* pcPointerShape;

	FigureUserFeedback(pSpread, PointerPos, ClickMods, FALSE, &str, &pcPointerShape);

	if (!(str.IsEmpty()))
		SetStatusText(&str);

	if (CurrentCursorID != CURSORID_UNSET)
		CursorStack::GSetTop(pcPointerShape, CurrentCursorID);
}



/********************************************************************************************
>	void SliceTool::MakeSelectionValidForDrag()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95
	Inputs:		-
	Outputs:	Can change the current selection, causing an immediate SelChanged message
	Returns:	-
	Purpose:	Runs through the selected nodes, making sure they are all happy with being 
				dragged.  Checks are:-
				1. Sub-Selected text characters are deselected, and their parent story (and
				all other nodes sharing its name) are selected instead.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void SliceTool::MakeSelectionValidForDrag()
{
	SelRange *pSelection = GetApplication()->FindSelection();
	RangeControl rg = pSelection->GetRangeControlFlags();
	rg.IgnoreInvisibleLayers = TRUE; // oddly setting this to TRUE actually means DO INCLUDE INIVISIBLE LAYERS in the range!!!
	RangeControl rcOld = rg;
	rg.PromoteToParent = TRUE;
	pSelection->Range::SetRangeControl(rg);

	Node* pNode = pSelection->FindFirst();
	BOOL ChangedSelection = FALSE;

	while (pNode != NULL)
	{
		// Push any text sub-selection up to the selected story, and make sure
		// that any other Nodes sharing the text-story's name are also selected.
 		if (pNode->IsAnAbstractTextChar())
		{
			((NodeRenderableInk*)pNode)->DeSelect(TRUE);
			TextStory* pStory = (TextStory*)pNode->FindParent(CC_RUNTIME_CLASS(TextStory));
			if (pStory != NULL)
			{
				String_256 SetName;
				SetName.Empty();
				SliceHelper::SelectAllSetsOfThisNode(pStory, SetName, FALSE);
				if (SetName.IsEmpty())
					pStory->Select(TRUE);
			 	ChangedSelection = TRUE;
			}
		}

		pNode = pSelection->FindNext(pNode);
	}

	// if we needed to change the selection, tell everyone.
	if (ChangedSelection)
	{
		// this call commented out, as I don't think it is required. pushing the selection
		// up to the letter's text-story in this way does not actually change the overall
		// state of the selection wrt named sets, as the story shares the same set name as
		// the character (always?). - Karim 25/10/1999
//		GetApplication()->FindSelection()->Update(TRUE);	// is this call actually 
		m_bSliceToolSetSel = TRUE;
	}

	pSelection->Range::SetRangeControl(rcOld);
}



/*******************************************************************

>	void SliceTool::DoTranslate()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/1999
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	Runs a translation tranformation drag on the selection.
	Errors:		
	See also:	

*******************************************************************/
void SliceTool::DoTranslate()
{
	// set appropriate transform parameters...
	TransformData tdParams;
	tdParams.CentreOfTrans	= ClickStart;
	tdParams.LeaveCopy		= FALSE;
	tdParams.LockAspect		= TRUE;
	tdParams.pRange			= NULL;
	tdParams.ScaleLines		= FALSE;
	tdParams.StartBlob		= 0;
	tdParams.TransFills		= TRUE;

	// set drag pointer shape.
	if (CurrentCursorID != CURSORID_UNSET)
		CursorStack::GSetTop(pcALLCursor, CurrentCursorID);

	// attempt to create a translate op.
	OpSliceTranslate* pSliceTransOp = new OpSliceTranslate();
	if (pSliceTransOp == NULL)
	{
		InformError(_R(IDS_OUT_OF_MEMORY), _R(IDS_OK));
		return;
	}

	// Get the current DocView (there must be one or we wouldn't be here).
	DocView* pDocView = DocView::GetCurrent();
	ERROR3IF(pDocView == NULL, "SliceTool::DoTranslate- Null current DocView");

	DocCoord dcOffset(0, 0);		// Default to offsets of 0

	// Fill a Transform Bounding Data structure up here
	BoundingData.x 		  = SelectionRect.lo.x;
	BoundingData.y 		  = SelectionRect.lo.y;
	BoundingData.Width    = SelectionRect.Width();
	BoundingData.Height   = SelectionRect.Height();
	BoundingData.XScale   = (FIXED16) 1;
	BoundingData.YScale   = (FIXED16) 1;
	BoundingData.Rotation = (ANGLE) 0;
	BoundingData.Shear 	  = (ANGLE) 0;

	// Run the transformation drag operation and return success code.
	pSliceTransOp->DragStarted(&tdParams, this, &BoundingData, ClickStart,
							  StartSpread, ClickMods, dcOffset, NULL, DRAGTYPE_AUTOSCROLL);
}



/*******************************************************************

>	void SliceTool::DoDragBox()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/1999
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	Runs a selector-tool drag-box operation.
	Errors:		
	See also:	

*******************************************************************/
void SliceTool::DoDragBox()
{
	OpSliceDragBox* pOpDragBox = new OpSliceDragBox();
	if (pOpDragBox == NULL)
	{
		InformError(_R(IDS_OUT_OF_MEMORY), _R(IDS_OK));
	}
	else
	{
		pOpDragBox->StartDragBox(StartSpread, ClickStart, ClickMods);
	}
}



/********************************************************************************************

>	void SliceTool::DisplayStatusBarHelp(DocCoord DocPos, Spread* pSpread, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		DocPos - the document coordinate of the point to display help on
				pSpread - pointer to the spread containing DocPos
				ClickMods - the current click modifiers
	Outputs:	-
	Returns:	-
	Purpose:	Displays status help string for the given position in the status bar.
	SeeAlso:	SliceTool::GetCurrentStatusText

********************************************************************************************/

void SliceTool::DisplayStatusBarHelp(DocCoord DocPos, Spread* pSpread, ClickModifiers ClickMods)
{
	String_256 StatusMsg("");

	// Get a string from the underlying help function and display it.
	GetCurrentStatusText(&StatusMsg, pSpread, DocPos, ClickMods);
	GetApplication()->UpdateStatusBarText(&StatusMsg);							 
}



/*******************************************************************

>	BOOL SliceTool::GetStatusLineText(String_256* ptext, Spread* pSpread,
									  DocCoord DocPos, ClickModifiers ClickMods)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>, from Phil's code for SelectorTool.
	Created:	11/10/1999
	Inputs:		ptext		Pointer to text string to fill in
				pSpread		Pointer to spread containing mouse position
				DocPos		Mouse position within spread
				ClickMods	Click modifiers
	Outputs:	-
	Returns:	TRUE always, indicating that the string was updated.
	Purpose:	Figure out what the status text for the SliceTool is at the given position
				on the given spread with the given click modifiers.
	Errors:		ERROR2 if pText is NULL.
	See also:	

*******************************************************************/
BOOL SliceTool::GetStatusLineText(String_256* ptext, Spread* pSpread,
								  DocCoord DocPos, ClickModifiers ClickMods)
{
	ERROR2IF(ptext==NULL,FALSE,"ptext is NULL!");

	Cursor* pcDummy;		// Dummy to hold unused pointer shape computed by FigureUserFeedback
	FigureUserFeedback(pSpread, DocPos, ClickMods, TRUE, ptext, &pcDummy);

	if (CurrentCursorID != CURSORID_UNSET)
		CursorStack::GSetTop(pcDummy,CurrentCursorID);

	return TRUE;
}



/********************************************************************************************

>	void SliceTool::GetCurrentStatusText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		pSpread points to a spread
				DocPos points to a point in a document
				ClickMods are the current click modifiers
	Outputs:	Updates the string in ptext
	Returns:	-
	Purpose:	Selects a suitable string for the status line based on the current location
				(as input via the parameters).
	SeeAlso:	SliceTool::GetStatusLineText, SliceTool::DisplayStatusBarHelp

********************************************************************************************/

void SliceTool::GetCurrentStatusText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)
{
	// You must use the Spread, DocCoord and ClickModifiers to select a suitable string to be 
	// displaied in the status bar.  This is usually done via some sort of switch statement.
	// Having selected a suitable string you should do a statement like	
	//	ptext->Load(<#Insert your string ID here #>);

	// Delete this line when you display useful strings!
	ptext->Empty(); 
}



/*******************************************************************

>	virtual void SliceTool::FigureUserFeedback(	Spread* pSpread,
												DocCoord dcPos,
												ClickModifiers cmods,
												BOOL DoSlowTests,
												String_256* pStr,
												Cursor** ppPointerShape	)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/10/1999
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	This routine computes the status line help and the pointer shape for a
				given mouse position in the Slice tool. It does NOT set either of these
				values but returns them both to the caller. It's up to the caller to decide
				whether to use these values or not.
				The tests done to figure out which pointer shapes, status help (and maybe 
				other stuff one day) is quite complex so it's most efficient to compute them
				all at once and then let the caller decide which ones to actually use.
				The precedence of pointer shapes, status messages and other user feedback is
				determined by the order of the checks within this function.
	Errors:		
	See also:	

*******************************************************************/
void SliceTool::FigureUserFeedback(	Spread* pSpread,
									DocCoord dcPos,
									ClickModifiers cmods,
									BOOL DoSlowTests,
									String_256* pStr,
									Cursor** ppPointerShape )
{
	// Initialise the status text string and pointer shape to be "null".
	pStr->Empty();
	*ppPointerShape = pcNormalSliceCursor;

	if (Tool::GetCurrentID() != TOOLID_SLICETOOL)
		return;

	BOOL DescribeDrag = TRUE;

	//-------------------------------------------------------------------------
	// Set the pointer shape, according to the key modifiers and/or the current state.
	if (!cmods.Adjust && !cmods.Constrain && !cmods.Alternative1)
	{
		*ppPointerShape = pcNormalSliceCursor;
		Append(pStr,_R(IDS_SLICE_MODE0));	// "Normal select mode:"
	}

	else if (cmods.Adjust && !cmods.Constrain && !cmods.Alternative1)
	{
		*ppPointerShape = pcAdjustCursor;
		Append(pStr,_R(IDS_SLICE_MODE1));	//	"Adjust select:"
	}

	else if (!cmods.Adjust && IsSelectUnderClick(cmods))
	{
		*ppPointerShape = pcUnderCursor;
		Append(pStr,_R(IDS_SLICE_MODE2));	//	"Select under:"
	}

	else if (cmods.Adjust && IsSelectUnderClick(cmods))
	{
		*ppPointerShape = pcUnderAdjustCursor;
		Append(pStr,_R(IDS_SLICE_MODE3));	//	"Adjust select under:"
	}

	else if (!cmods.Adjust && IsSelectMemberClick(cmods))
	{
		*ppPointerShape = pcInsideCursor;
		Append(pStr,_R(IDS_SLICE_MODE4));	//	"Force drag/Select member:"
	}

	else if (cmods.Adjust && IsSelectMemberClick(cmods))
	{
		*ppPointerShape = pcInsideAdjustCursor;
		Append(pStr,_R(IDS_SLICE_MODE5));	//	"Force drag/Adjust select member:"
	}

	else if (!cmods.Adjust && IsSelectLeafClick(cmods))
	{
		*ppPointerShape = pcLeafCursor;
		Append(pStr,_R(IDS_SLICE_MODE6));	//	"Select inside:"
	}

	else if (cmods.Adjust && IsSelectLeafClick(cmods))
	{
		*ppPointerShape = pcLeafAdjustCursor;
		Append(pStr,_R(IDS_SLICE_MODE7));	//	"Adjust select inside:"
	}

	else
	{
		ERROR3("SliceTool::FigureUserFeedback- out of range ClickModifiers");
		return;
	}

	// Check for direct drag mode and say something about it...
	if (IsTranslateShortcut(cmods))
	{
		if (SelectRange->FindFirst())
		{
			*ppPointerShape = pcALLCursor;
			Append(pStr,_R(IDS_SLICEINSIDE4));	//	"Drag to move the selected objects"
		}
		DescribeDrag = FALSE;
	}

	// See if there's a non-selectable object (guideline) under the mouse
	NodeRenderableInk* pPreProNode = FindPreProcessClickNode(pSpread,dcPos,TRUE);
	if (pPreProNode)
	{
		if (IS_A(pPreProNode,NodeGuideline))
		{
			NodeGuideline* pGuideline = (NodeGuideline*)pPreProNode;
			if (pGuideline->GetType()==GUIDELINE_HORZ)
			{
				*ppPointerShape = pcHorzGuideCursor;
				//	"Drag up or down to move guideline; Drag onto top ruler to delete it"
				Append(pStr,_R(IDS_SLICEHORZGUIDE));
				return;
			}

			if (pGuideline->GetType()==GUIDELINE_VERT)
			{
				*ppPointerShape = pcVertGuideCursor;
				//	"Drag left or right to move guideline; Drag onto left ruler to delete it"
				Append(pStr,_R(IDS_SLICEVERTGUIDE));
				return;
			}
		}
	}

	//-------------------------------------------------------------------------
	// If we've got time to do the slow tests then go ahead and do them...
	if (DoSlowTests)
	{
		// Perform a hit-test at the mouse position and set the status bar text
		// accordingly.
		// Allow the hit-test to be interrupted if the mouse moves!
		Node* pInterruptedNode = NULL;
		NodeRenderableInk* pSimple = NodeRenderableInk::FindSimpleAtPoint(pSpread, 
																		  dcPos, 
																		  NULL, 
																		  &pInterruptedNode);
		// If hit-test was interrupted then don't say anything about what's under the pointer!
		if (pInterruptedNode!=NULL)
			return;

		NodeRenderableInk* pCompound = NodeRenderableInk::FindCompoundFromSimple(pSimple);

		// if we changed the selection last, or the SelRange cannot provide a usable
		// last-selection then use our own record. otherwise, ask the SelRange.
		Node* pLastSelNode = NULL;
		if (m_bSliceToolSetSel && pLastClickNode != NULL)
		{
			pLastSelNode = pLastClickNode;
		}
		else
		{
			pLastSelNode = SelectRange->GetLastSelectedNode();
			if ( pLastSelNode==NULL || !(pLastSelNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk))) )
				pLastSelNode = pLastClickNode;
		}

		// Find out what we should do with the click...
		ClickActionCode action = CLICKACTION_NONE;
		NodeRenderableInk* pActionNode = NULL;
		action = DetermineClickAction(&pActionNode,(NodeRenderableInk*)pLastSelNode,
										pSimple,pCompound,pSpread,dcPos,cmods);

		// Act upon the information...
		switch (action)
		{
			//-------------------------------------------------//
			// No action required...
			case CLICKACTION_NONE:
				break;

			//-------------------------------------------------//
			// Anything that's selected must be deselected...
			case CLICKACTION_SELNONE:
				if (!cmods.Adjust)
				{
					// If there are selected objects append message about clearing.
					if (SelectRange && SelectRange->FindFirst())
						Append(pStr,_R(IDS_SLICENONE1));	//	"Click to clear the selection"
					// Message about marquee drag
					if (DescribeDrag)
						Append(pStr,_R(IDS_SLICENONE4));	//	"Drag to marquee select objects"
					Append(pStr,_R(IDS_SLICENONE2));		//	"Move pointer over object to select"
				}
				else
				{
					// Adjust is held down so describe marquee add.
					if (DescribeDrag)
						//	"Drag to marquee select objects to add to selection"
						Append(pStr,_R(IDS_SLICENONE5));
					//	"Move pointer over object to add/remove from selection"
					Append(pStr,_R(IDS_SLICENONE3));
				}
				break;

			//-------------------------------------------------//
			// The action node must be selected...
			case CLICKACTION_SELNODE:
				Append(pStr,cmods,
							_R(IDS_SLICENODE3), // "Click to select this #1%S alone; Drag to move it"
							_R(IDS_SLICENODE4), // "Click to select this #1%S"
							_R(IDS_SLICENODE5), // "Click to deselect this #1%S"
							pActionNode);
				break;
			case CLICKACTION_SELUNDER:
				Append(pStr,cmods,
							_R(IDS_SLICEUNDER1), // "Click to select the #1%S under the last selected object alone"
							_R(IDS_SLICEUNDER2), // "Click to select the #1%S under the last selected object"
							_R(IDS_SLICEUNDER3), // "Click to deselect the #1%S under the last selected object"
							pActionNode);
				break;
			case CLICKACTION_SELUNDERCYCLE:
				Append(pStr,cmods,
							_R(IDS_SLICEUNDERCYCLE1), // "Click to select the top #1%S alone; (Reached the bottom)"
							_R(IDS_SLICEUNDERCYCLE2), // "Click to select the top #1%S; (Reached the bottom)"
							_R(IDS_SLICEUNDERCYCLE3), // "Click to deselect the top #1%S; (Reached the bottom)"
							pActionNode);
				break;
			case CLICKACTION_SELUNDERFAIL:
				Append(pStr,cmods,
							_R(IDS_SLICEUNDERFAIL1), // "Click to select the #1%S alone; (Nothing under the last selected object)"
							_R(IDS_SLICEUNDERFAIL2), // "Click to select the #1%S; (Nothing under the last selected object)"
							_R(IDS_SLICEUNDERFAIL3), // "Click to deselect the #1%S; (Nothing under the last selected object)"
							pActionNode);
				break;
			case CLICKACTION_SELUNDERFAIL2:
				Append(pStr,cmods,
							_R(IDS_SLICEUNDERFAIL21), // "Click to select the #1%S alone; (The last selected object is not under the pointer)"
							_R(IDS_SLICEUNDERFAIL22), // "Click to select the #1%S; (The last selected object is not under the pointer)"
							_R(IDS_SLICEUNDERFAIL23), // "Click to deselect the #1%S; (The last selected object is not under the pointer)"
							pActionNode);
				break;
			case CLICKACTION_SELINSIDE:
				Append(pStr,cmods,
							_R(IDS_SLICEINSIDE1), // "Click to select the #1%S member of the last selected object alone"
							_R(IDS_SLICEINSIDE2), // "Click to select the #1%S member of the last selected object"
							_R(IDS_SLICEINSIDE3), // "Click to deselect the #1%S member of the last selected object"
							pActionNode);
				break;
			case CLICKACTION_SELINSIDECYCLE:
				Append(pStr,cmods,
							_R(IDS_SLICEINSIDECYCLE1), // "Click to select the top #1%S alone; (Reached the simplest object)"
							_R(IDS_SLICEINSIDECYCLE2), // "Click to select the top #1%S; (Reached the simplest object)"
							_R(IDS_SLICEINSIDECYCLE3), // "Click to deselect the top #1%S; (Reached the simplest object)"
							pActionNode);
				break;
			case CLICKACTION_SELINSIDEFAIL:
				Append(pStr, _R(IDS_SLICEINSIDEFAIL1));	// "Nothing inside this object"
				break;
			case CLICKACTION_SELINSIDEFAIL2:
				Append(pStr,cmods,
							_R(IDS_SLICEINSIDEFAIL21), // "Click to select the #1%S alone; (The pointer is not over a member of the last selected object)"
							_R(IDS_SLICEINSIDEFAIL22), // "Click to select the #1%S; (The pointer is not over a member of the last selected object)"
							_R(IDS_SLICEINSIDEFAIL23), // "Click to deselect the #1%S; (The pointer is not over a member of the last selected object)"
							pActionNode);
				break;
			case CLICKACTION_SELLEAF:
				Append(pStr,cmods,
							_R(IDS_SLICELEAF1), // "Click to select this #1%S alone"
							_R(IDS_SLICELEAF2), // "Click to select this #1%S"
							_R(IDS_SLICELEAF3), // "Click to deselect this #1%S"
							pActionNode);
				break;
			//-------------------------------------------------//
			default:
				ERROR3("Unknown Click action code!");
				break;
		};

		// If we're in normal click mode (no modifiers down) then remind the user
		// that they can use the modifer keys...
		if (!cmods.Adjust && !cmods.Constrain && !cmods.Alternative1)
			Append(pStr,_R(IDS_SELOPTIONS));
	}
}




/********************************************************************************************

>	ClickActionCode SliceTool::DetermineClickAction(NodeRenderableInk** ppActionNode,
													NodeRenderableInk* pLastClickNode,
													NodeRenderableInk* pClickSimpleNode,
													NodeRenderableInk* pClickCompoundNode,
													Spread* pStartSpread,
													DocCoord ClickStart,
													ClickModifiers ClickMods )

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>, from Phil's SelectorTool code
	Created:	11/10/1999
	Inputs:		-
	Outputs:	pNodeToSelect	Pointer to pointer to node to select or NULL
	Returns:	ActionCode describing what the click should do
	Purpose:	Determine what action needs to be taken in response to a click.
	Errors:		-
	SeeAlso:	SliceTool::HandleButtonUp

********************************************************************************************/

SliceTool::ClickActionCode SliceTool::DetermineClickAction(	NodeRenderableInk** ppActionNode,
															NodeRenderableInk* pLastClickNode,
															NodeRenderableInk* pClickSimpleNode,
															NodeRenderableInk* pClickCompoundNode,
															Spread* pStartSpread,
															DocCoord ClickStart,
															ClickModifiers ClickMods)
{
	*ppActionNode = NULL;

	//--------------------------------------
	// Test "leaf" modifier...
	if (IsSelectLeafClick(ClickMods))
	{
		// Go directly to leaf nodes!
		if (pClickSimpleNode != pClickCompoundNode)
		{
			*ppActionNode = pClickSimpleNode;
			
			// <<<<< Inclusion by Mike 11/01/96
			// this stuff is to check whether any parent is responding to
			// AllowSelectInside() and returning FALSE. Selections will not go down
			// into these objects if so.

			Node* pParentNode = pClickSimpleNode->FindParent();
			while (pParentNode)
			{
				if (pParentNode->IsKindOf(CC_RUNTIME_CLASS(Layer)))
					break;
				if (pParentNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk)))
				{
					if ( (!pParentNode->AllowSelectInside()) && 
						 ((NodeRenderableInk*)pParentNode)->CanSelectAsCompoundParent()
					   )
					{
						*ppActionNode = (NodeRenderableInk*)(pParentNode);
					}
				}
				if (pParentNode==pClickCompoundNode)
					break;
				pParentNode = pParentNode->FindParent();
			}

			// <<<<< End of inclusion
		}

		// If we still haven't found what we're looking for
		// Cycle round to the top again...
		return CycleClickAction(ppActionNode,pClickCompoundNode,CLICKACTION_SELLEAF,CLICKACTION_SELNODE);
	}

	//--------------------------------------
	// Test "under" modifier...
	if (IsSelectUnderClick(ClickMods))
	{
		// Try to perform a select under
		// First check that the context node is still under the pointer
		// If not then all we can do is a normal click operation...
		if (!ValidateLastClickUnder(pLastClickNode,pStartSpread,ClickStart))
			return CycleClickAction(ppActionNode,pClickCompoundNode,CLICKACTION_SELUNDERFAIL2,CLICKACTION_SELUNDERFAIL2);

		// Find the leaf node at the click position, but only search nodes
		// before the last clicked node.
		*ppActionNode = NodeRenderableInk::FindSimpleAtPoint(pStartSpread,ClickStart,pLastClickNode);
		// Then find a compound node containing the leaf, preferably a sibling
		// of the last clicked node.
		*ppActionNode = NodeRenderableInk::FindCompoundFromSimple(*ppActionNode,pLastClickNode);

		// If the "under" node turns out to be the node we started from
		// return a failure code but go ahead and re-select it...
		// (If we failed to find anything under the last node, and the last node is the top node)
		if (*ppActionNode==NULL && pLastClickNode == pClickCompoundNode)
			return CycleClickAction(ppActionNode,pClickCompoundNode,CLICKACTION_SELUNDERFAIL,CLICKACTION_SELUNDERFAIL);

		// If we still haven't found what we're looking for
		// Cycle round to the top again...
		return CycleClickAction(ppActionNode,pClickCompoundNode,CLICKACTION_SELUNDER,CLICKACTION_SELUNDERCYCLE);
	}

	//--------------------------------------
	// Test "member" modifier...
	if (IsSelectMemberClick(ClickMods))
	{
		// See if the clicked simple node is a descendent of the last clicked node
		if (!ValidateLastClickInside(pLastClickNode,pClickSimpleNode))
			return CycleClickAction(ppActionNode,pClickCompoundNode,CLICKACTION_SELINSIDEFAIL2,CLICKACTION_SELINSIDEFAIL2);

		// If the node we're going to look inside is not compound and it's the top node
		// return a failure code but go ahead and re-select it...
		if (pLastClickNode && !pLastClickNode->IsCompound() && pLastClickNode == pClickCompoundNode)
			return CycleClickAction(ppActionNode,pClickCompoundNode,CLICKACTION_SELINSIDEFAIL,CLICKACTION_SELINSIDEFAIL);

		// Try to perform a select inside
		*ppActionNode = NodeRenderableInk::FindInnerCompound(pClickSimpleNode,pLastClickNode);

		// If we still haven't found what we're looking for
		// Cycle round to the top again...
		return CycleClickAction(ppActionNode,pClickCompoundNode,CLICKACTION_SELINSIDE,CLICKACTION_SELINSIDECYCLE);
	}

	//--------------------------------------
	// OK, so no modifiers are currently down
	// Just try to do a normal click action...
	return CycleClickAction(ppActionNode,pClickCompoundNode,CLICKACTION_NONE,CLICKACTION_SELNODE);
}



/********************************************************************************************

>	BOOL SliceTool::ValidateLastClickUnder(NodeRenderableInk* pLastClickNode, DocCoord ClickStart)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>, from Phil's SelectorTool code
	Created:	11/10/1999
	Inputs:		-
	Outputs:	pLastClickNode	Pointer to last node selected or NULL
	Returns:	TRUE if last click node is still under the pointer (somewhere)
				FALSE otherwise
	Purpose:	Validate that the last click node is still under the pointer
				Note! This routine can be slow depending on how deep it has to look in the
				tree for the last selected object.
	Errors:		-
	SeeAlso:	SliceTool::HandleButtonUp

********************************************************************************************/

BOOL SliceTool::ValidateLastClickUnder(NodeRenderableInk* pLastClickNode, Spread* pStartSpread, DocCoord ClickStart)
{
	NodeRenderableInk* pSearchNode = NULL;
	do
	{
		pSearchNode = NodeRenderableInk::FindSimpleAtPoint(pStartSpread,ClickStart,pSearchNode);
	}
	while (pSearchNode && pLastClickNode!=NodeRenderableInk::FindCompoundFromSimple(pSearchNode,pLastClickNode));

	return (pSearchNode!=NULL);
}




/********************************************************************************************

>	BOOL SliceTool::ValidateLastClickInside(NodeRenderableInk* pLastClickNode,NodeRenderableInk* pClickSimpleNode)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>, from Phil's SelectorTool code
	Created:	11/10/1999
	Inputs:		-
	Outputs:	pLastClickNode	Pointer to last node selected or NULL
	Returns:	TRUE if the simple clicked node is inside the last clicked node somewhere
				FALSE otherwise
	Purpose:	Validate that the simple node is inside the last clicked node
	Errors:		-
	SeeAlso:	SliceTool::HandleButtonUp

********************************************************************************************/

BOOL SliceTool::ValidateLastClickInside(NodeRenderableInk* pLastClickNode,NodeRenderableInk* pClickSimpleNode)
{
	Node* pSearchNode = pClickSimpleNode;
	while (pSearchNode && pSearchNode!=pLastClickNode)
		pSearchNode = pSearchNode->FindParent();
	return (pSearchNode!=NULL);
}



/********************************************************************************************

>	ClickActionCode SliceTool::CycleClickAction(NodeRenderableInk** ppActionNode,
												NodeRenderableInk* pClickCompoundNode,
												ClickActionCode foundAction,
												ClickActionCode cycleAction)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>, from Phil's SelectorTool code
	Created:	11/10/1999
	Inputs:		-
	Outputs:	pNodeToSelect	Pointer to pointer to node to select or NULL
	Returns:	ActionCode describing what the click should do
	Purpose:	Determine what action needs to be taken in response to a click.
	Errors:		-
	SeeAlso:	SliceTool::DetermineClickAction

********************************************************************************************/

SliceTool::ClickActionCode SliceTool::CycleClickAction(	NodeRenderableInk** ppActionNode,
														NodeRenderableInk* pClickCompoundNode,
														ClickActionCode foundAction,
														ClickActionCode cycleAction )
{
	// If we have found a node then return the specified action code...
	if (*ppActionNode)
	{
		return foundAction;
	}
	// Else no suitable node so see whether the click occurred over a compound node
	else
	{
		// If click occurred over a compound node then we can return that
		// along with the alternative action code...
		if (pClickCompoundNode)
		{
			*ppActionNode = pClickCompoundNode;
			return cycleAction;
		}
		else
		// Else if there wasn't even a compound node we must return the information that
		// the click occurred over white space...
		{
			*ppActionNode = NULL;
			return CLICKACTION_SELNONE;
		}
	}
}



/********************************************************************************************

>	static void SliceTool::SetStatusText(String_256* pStr)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Inputs:		pStr			pointer to the string to display
	Outputs:	-
	Returns:	-
	Purpose:	Sets the status bar text to the given string.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void SliceTool::SetStatusText(String_256* pStr)
{
	GetApplication()->UpdateStatusBarText(pStr);
}



/********************************************************************************************

>	static void SliceTool::SetStatusText(UINT32 nStringID)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		nStringID			the numeric identifier of the string resource
	Outputs:	-
	Returns:	-
	Purpose:	Sets the status bar text to the given string.  Alternative to loading the
				string yourself and calling the other SetStatusText function.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void SliceTool::SetStatusText(UINT32 nStringID)
{
	String_256 str(nStringID);
	SetStatusText(&str);
}



/********************************************************************************************

>	BOOL SliceTool::IsTranslateShortcut(ClickModifiers cmods) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the translate shortcut keys are detected.
	Purpose:	Detects whether the current mouse click modifiers denote the translate
				drag operation shortcut.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL SliceTool::IsTranslateShortcut(ClickModifiers cmods) const
{
	return cmods.Constrain && cmods.Alternative1;
}




/********************************************************************************************

>	BOOL SliceTool::IsClickModified(ClickModifiers cmods) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the current click is "modified".
	Purpose:	Tests whether any of the modifiers, eg. Constrain, Adjust etc, apply to
				the current mouse click (as received by the OnClick function).
	Errors:		-
	SeeAlso:	SliceTool::HandleSingleClick

********************************************************************************************/

BOOL SliceTool::IsClickModified(ClickModifiers cmods) const
{
	return cmods.Adjust || cmods.Constrain || cmods.Alternative1 || cmods.Menu;
}




/********************************************************************************************

>	BOOL SliceTool::IsSelectUnderClick(ClickModifiers cmods) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the current click signifies the object below the clicked object
				should be selected, FALSE otherwise.
	Purpose:	Decides whether the current click is modified to be an "under" click or not
	Errors:		-
	SeeAlso:	SliceTool::HandleSingleClick

********************************************************************************************/

BOOL SliceTool::IsSelectUnderClick(ClickModifiers cmods) const
{
	return (!cmods.Constrain && cmods.Alternative1);
}




/********************************************************************************************

>	BOOL SliceTool::IsSelectMemberClick(ClickModifiers cmods) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the last click indicated "select-inside".
	Purpose:	Reports whether the current mouse click meant the user wanted to "select-
				inside" or not.
	Errors:		-
	SeeAlso:	SliceTool::HandleButtonUp

********************************************************************************************/

BOOL SliceTool::IsSelectMemberClick(ClickModifiers cmods) const
{
	return (cmods.Constrain && cmods.Alternative1);
}



/********************************************************************************************

>	BOOL SliceTool::IsSelectLeafClick(ClickModifiers cmods) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the last click indicated "select-leaf".
	Purpose:	Reports whether the current mouse click meant the user wanted to "select-
				leaf" or not.
	Errors:		-
	SeeAlso:	SliceTool::HandleButtonUp

********************************************************************************************/

BOOL SliceTool::IsSelectLeafClick(ClickModifiers cmods) const
{
	return (cmods.Constrain && !cmods.Alternative1);
}



/********************************************************************************************

>	NodeRenderableInk* SliceTool::FindFrom(NodeRenderableInk* pSimpleNode) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/1/95
	Inputs:		pSimpleNode			the simple node to begin searching from
	Outputs:	-
	Returns:	The compound object the simple node is part of, if any, or pSimpleNode
				if it isn't.
	Purpose:	Front-end short-hand for NodeRenderableInk::FindCompoundFromSimple
	Errors:		-
	SeeAlso:	NodeRenderableInk::FindCompoundFromSimple

********************************************************************************************/

NodeRenderableInk* SliceTool::FindFrom(NodeRenderableInk* pSimpleNode) const
{
	return NodeRenderableInk::FindCompoundFromSimple(pSimpleNode);
}



/********************************************************************************************

>	BOOL SliceTool::Append(String_256* pStr, ClickModifiers cmods, UINT32 resID, NodeRenderableInk* pActionNode = NULL)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-

********************************************************************************************/

BOOL SliceTool::Append(String_256* pStr, ClickModifiers cmods,
											UINT32 SelectID,
											UINT32 AddID,
											UINT32 RemoveID,
											NodeRenderableInk* pActionNode)
{
	ERROR2IF(SelectID==0, FALSE, "Asked to append a string resource with a null ID");

	// Append a message to the string, preceding it with a separator if there was something
	// already in the string...
	if (!pStr->IsEmpty())
		*pStr += String_256(_R(IDS_SLICE_SEPARATOR));

	String_256 Message;
	UINT32 TemplateID = 0;
	Message.Empty();

	if (!cmods.Adjust || pActionNode==NULL)
		TemplateID = SelectID;
	else
	{
		if (!pActionNode->IsSelected())
			TemplateID = AddID;
		else
			TemplateID = RemoveID;
	}

	if (TemplateID==0)
		TemplateID = SelectID;

	if (pActionNode==NULL)
		*pStr += String_256(TemplateID);
	else
	{
		Message._MakeMsg( (TCHAR*) String_256(TemplateID), &pActionNode->Describe(FALSE) );
		*pStr += Message;
	}

	return TRUE;
}




/********************************************************************************************

>	BOOL SliceTool::Append(String_256* pStr, UINT32 resID)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-

********************************************************************************************/

BOOL SliceTool::Append(String_256* pStr, UINT32 StringID)
{
	// Append a message to the string, preceding it with a separator if there was something
	// already in the string...
	if (!pStr->IsEmpty())
		*pStr += String_256(_R(IDS_SLICE_SEPARATOR));
	*pStr += String_256(StringID);

	return TRUE;
}




/********************************************************************************************

>	BOOL SliceTool::Append(String_256* pStr, String_256 String)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-

********************************************************************************************/

BOOL SliceTool::Append(String_256* pStr, String_256 String)
{
	// Append a message to the string, preceding it with a separator if there was something
	// already in the string...
	if (!pStr->IsEmpty())
		*pStr += String_256(_R(IDS_SLICE_SEPARATOR));
	*pStr += String;

	return TRUE;
}




/********************************************************************************************

>	BOOL SliceTool::Append(String_256* pStr,
							  String_256 SelectTemplate,
							  String_256 AddTemplate,
							  String_256 RemoveTemplate,
							  NodeRenderableInk* pActionNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/06/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-

********************************************************************************************/

BOOL SliceTool::Append(String_256* pStr,
						  ClickModifiers cmods,
						  String_256 SelectTemplate,
						  String_256 AddTemplate,
						  String_256 RemoveTemplate,
						  NodeRenderableInk* pActionNode)
{
	ERROR2IF(SelectTemplate.IsEmpty(), FALSE, "Asked to Append an empty message");

	// Append a message to the string, preceding it with a separator if there was something
	// already in the string...
	if (!pStr->IsEmpty())
		*pStr += String_256(_R(IDS_SLICE_SEPARATOR));

	String_256 Message;
	String_256* pTemplate;
	Message.Empty();

	if (!cmods.Adjust || pActionNode==NULL)
		pTemplate = &SelectTemplate;
	else
	{
		if (!pActionNode->IsSelected())
			pTemplate = &AddTemplate;
		else
			pTemplate = &RemoveTemplate;
	}

	if (pTemplate->IsEmpty())
		pTemplate = &SelectTemplate;

	if (pActionNode==NULL)
		*pStr += *pTemplate;
	else
	{
		Message._MakeMsg( (TCHAR*) *pTemplate, &pActionNode->Describe(FALSE) );
		*pStr += Message;
	}

	return TRUE;
}



/********************************************************************************************

>	virtual void SliceTool::RenderToolBlobs(Spread* pSpread, DocRect* pClipRect)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/07/2000
	Inputs:		pSpread		the spread to render blobs for - must == this->SelectionSpread.
				pClipRect	the rect to clip to - used to prevent unnecessary drawing.

	Purpose:	Render the SliceTool's tool blob.
				This consists of a bounding rect identical to that used when dragging the
				selection (dotted, red line), around whatever is currently selected.

	See also:	SLICETOOL_BLOBS #define at the top of this file to turn this off.

********************************************************************************************/
void SliceTool::RenderToolBlobs(Spread* pSpread, DocRect* pClipRect)
{
#ifdef SLICETOOL_BLOBS
	// don't bother if there's no selection spread or if we're asked to render for
	// a spread which isn't the selection spread.
	if (	SelectionSpread == NULL ||
			pSpread != SelectionSpread )
	{
		return;
	}

	// get the selected DocView - if we can't get it, then leave now.
	DocView* pView = DocView::GetSelected();
	if (pView == NULL)
		return;

	// sorry, this isn't the neatest programming.
	// The size of the SliceTool blob bitmap is about 10 pixels square.
	// The desired distance between blob and selection is 2 pixels.
	INT32 BlobGap	= (INT32)(2  * pView->GetScaledPixelWidth().MakeDouble());
	INT32 BlobRadius	= (INT32)(10 * pView->GetScaledPixelWidth().MakeDouble());
		 BlobRadius /= 2;

	// the absolute position of the blob - just left and below the top of the selection rect.
	DocCoord dcBlobPos(	m_drBlobRect.hix + (BlobGap + BlobRadius),
						m_drBlobRect.hiy - (BlobGap + BlobRadius) );

	// render the blobs for all appropriate render regions.
	RenderRegion* pRegion = DocView::RenderOnTop(pClipRect, pSpread, ClippedEOR);
	while (pRegion != NULL)
	{
		pRegion->SaveContext();

		// draw a bounding box around the selection.
		pRegion->SetLineColour(COLOUR_XORSELECT);
		pRegion->DrawDragBounds(&m_drBlobRect);

		// draw a little named-set bitmap to denote what sets are selected.
		switch (m_SetSelectionState)
		{
		case FullSetsSelected:
			pRegion->DrawBitmapBlob(dcBlobPos, _R(IDBMP_SLICE_SELALL));
			break;

		case HalfSetsSelected:
			pRegion->DrawBitmapBlob(dcBlobPos, _R(IDBMP_SLICE_SELSOME));
			break;

		case NoSetsSelected:
			pRegion->DrawBitmapBlob(dcBlobPos, _R(IDBMP_SLICE_SELNONE));
			break;

		default:
			ERROR3("SliceTool::RenderToolBlobs; unrecognised set-selection state!");
			break;
		}

		pRegion->RestoreContext();

		// Go on to the next render region, if any.
		pRegion = DocView::GetNextOnTop(pClipRect);
	}
#endif
}



/********************************************************************************************
>	BOOL SliceTool::UpdateSelectionInfo()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>, from JustinF's SelectorTool::UpdateSelectionInfo() code.
	Created:	14/10/1999
	Inputs:		
	Outputs:	
	Returns:	TRUE if there is a current selection,
				FALSE if no object is selected.
	Purpose:	Updates the slice tool's record of the spread and bounding rectangle
				of the current selection.
	Errors:		
	SeeAlso:	SliceTool::SelectChange
********************************************************************************************/
BOOL SliceTool::UpdateSelectionInfo()
{
	// Reset all our infomation about the selection to "no selection".
	SelectionSpread = NULL;
	SelectionRect.MakeEmpty();
	m_drBlobRect.MakeEmpty();

	// we want to take controller nodes into account in this selection.
	RangeControl rc = SelectRange->GetRangeControlFlags();
	RangeControl rcOld = rc;
	rc.PromoteToParent = TRUE;
	SelectRange->Range::SetRangeControl(rc);

	// Go find the first node in the selection, so that we can find out about its spread
	Node* pFirstNode = SelectRange->FindFirst();
	if (pFirstNode != NULL)
	{
		// Find the spread that the selection lives on, if any, and its bounds.
		SelectionSpread = pFirstNode->FindParentSpread();
		if (SelectionSpread != NULL)
		{
			// Update the bounding rectangle of the selection.
			SelectionRect	= SelectRange->GetBoundingRect();

			// update our tool-blob rendering info.
			m_drBlobRect	= SelectRange->GetBoundingRectForEorDragging();

			// call the name-gallery to update named set info.
			NameGallery* pNameGallery = NameGallery::Instance();
			if (pNameGallery)
				pNameGallery->FastUpdateNamedSetSizes();

			// decide what set-selection info we'll be displaying.
			INT32 nSetSelection = SliceHelper::DoesSelectionOnlyContainCompleteSets();
			m_SetSelectionState =	(nSetSelection == 1) ?	FullSetsSelected :
									(nSetSelection == 0) ?	HalfSetsSelected :
															NoSetsSelected;

			if (SelectionRect.IsEmpty())
				SelectionRect.Inflate(1);
		}
	}

	SelectRange->Range::SetRangeControl(rcOld);

	// Return TRUE if there is a selection.
	return SelectionSpread != NULL;
}



/********************************************************************************************

>	virtual BOOL SliceTool::DragFinished(DragEndType det)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>, from JustinF's SelectorTool::DragFinished
	Created:	14/10/1999
	Inputs:		det		a code indicating how the drag was ended,
						eg. did the user hit ESCAPE and cancel it?
	Outputs:	
	Returns:	TRUE if the drag operation should be allowed to complete,
				FALSE to signal that the drag should be cancelled.
	Purpose:	Called by TransOperation when a drag is finished.
				Allows the SliceTool to reset its cursors and update
				its current selection information.
	Errors:		
	SeeAlso:	

********************************************************************************************/
BOOL SliceTool::DragFinished(DragEndType det)
{
	// update the current cursor.
	SetKeyDownCursor(ClickModifiers::GetClickModifiers());

	// we always leave cancelling the drag to a capricious user.
	return TRUE;
}



//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------



/********************************************************************************************

>	MsgResult SliceInfoBarOp::Message(Msg* Message) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		Message = The message to handle
	Outputs:	-
	Returns:	-
	Purpose:	Slice info bar dialog message handler
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult SliceInfoBarOp::Message(Msg* Message) 
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		switch(Msg->DlgMsg)
		{
		case DIM_CANCEL:
			m_InfoBarCreated = FALSE;
			Close(); // close the dlg
			break;

		case DIM_CREATE:
			m_InfoBarCreated = TRUE;
			// Initialise the infobar controls here
			// This is sent when you create the infobar in your tool startup code
			SetStringGadgetValue(_R(IDC_ST_STATE), _R(IDS_ROLLOVER_DEFAULT), FALSE, 0);
			SetStringGadgetValue(_R(IDC_ST_STATE), _R(IDS_ROLLOVER_MOUSE), FALSE, 1);
			SetStringGadgetValue(_R(IDC_ST_STATE), _R(IDS_ROLLOVER_CLICKED), FALSE, 2);
			SetStringGadgetValue(_R(IDC_ST_STATE), _R(IDS_ROLLOVER_SELECTED), FALSE, 3);
			SetStringGadgetValue(_R(IDC_ST_STATE), _R(IDS_ROLLOVER_ALL), FALSE, 4);
			SetStringGadgetValue(_R(IDC_ST_STATE), _R(IDS_ROLLOVER_NONE), TRUE, 5);
			UpdateCurrentStateGadget();

			// set up the bar name bit and grey the buttons that are selection dependant
			// check again for the new bar name
			m_TopVisibleState = -1;

			// set the bar name
			//g_BarName.MakeMsg(_R(IDS_BARNAME), 1);
			OnSelectionChanged();
			break;

			//NB: This means someone has altered the combo box. It doesn't mean
			//the selection has changed!
		case DIM_SELECTION_CHANGED: 
			if(Msg->GadgetID == _R(IDC_ST_STATE))
			{
				OpParam Param(GetSelectedValueIndex(_R(IDC_ST_STATE)), 0);
				OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_SHOWSTATE); 
				if (pOpDesc != NULL)
					pOpDesc->Invoke((OpParam*)&Param);
				else
				{
					ERROR3("Couldn't find OPTOKEN_SHOWSTATE op descriptor");
				}
				UpdateCurrentStateGadget();
			}
			break;

			case DIM_LFT_BN_CLICKED:
				switch(Msg->GadgetID)
				{
				case _R(IDC_ST_EXPORT):
					// run the Op that does the image slicing / rollover creation
					// from the back end
					{
						// matt-24/08/2000
						// Should check first that buttons exist in this document - same code as
						// used in Edit section below...

						BOOL ok = TRUE;
						// nothing selected then test that the bar we think we are editing exists
						// warn if it doesn't
						if (!m_EditingBar)
						{
							String_256 defaultLayerName;
							defaultLayerName.Load(_R(IDS_ROLLOVER_DEFAULT));
							Layer * pDef = SliceHelper::FindLayerCalled(defaultLayerName);

							if (pDef == NULL || !SliceHelper::BarNameExists(pDef, g_BarName))
							{
								ok = FALSE;
								InformWarning(_R(IDS_BAR_DOESNT_EXIST));
							}
						}

						if (ok)
						{
							OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_IMAGESLICE); 
							if (pOpDesc != NULL)
								pOpDesc->Invoke();//(OpParam*)&Param);
							else
							{
								ERROR3("Couldn't find OPTOKEN_IMAGESLICE op descriptor");
							}
						}
					}
					break;

				case _R(IDC_NEW_BAR):
					// Brings up the edit/create bar dlg
					{
						// find a new bar name and put it in g_BarName
						String_256 DefLayerName;
						DefLayerName.Load(_R(IDS_ROLLOVER_DEFAULT));
						Layer * pDef = SliceHelper::FindLayerCalled(DefLayerName);
						INT32 UnusedBarID = 0;
						String_256 NewBarName = "";

						do
						{
							UnusedBarID++;
							NewBarName.MakeMsg(_R(IDS_BARNAME), UnusedBarID);
						}
						while (pDef && SliceHelper::BarNameExists(pDef, NewBarName));

						// run the op to create a new bar
						OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_BARCREATIONDLG); 
						if (pOpDesc != NULL)
						{
							OpParamBarCreationDlg Param(FALSE /*Creating*/, NewBarName, SliceHelper::CountButtonsInBar(NewBarName));
							pOpDesc->Invoke((OpParam*)&Param);
						}
						else
						{
							ERROR3("Couldn't find OPTOKEN_BARCREATIONDLG op descriptor");
						}
					}
					break;
				
				case _R(IDC_EDIT_BAR):
					// Brings up the edit/create bar dlg
					{
						OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_BARCREATIONDLG); 
						if (pOpDesc != NULL)
						{
							BOOL ok = TRUE;
							// nothing selected then test that the bar we think we are editing exists
							// warn if it doesn't
							if (!m_EditingBar)
							{
								String_256 DefLayerName;
								DefLayerName.Load(_R(IDS_ROLLOVER_DEFAULT));
								Layer * pDef = SliceHelper::FindLayerCalled(DefLayerName);

								if (pDef == NULL || !SliceHelper::BarNameExists(pDef, g_BarName))
								{
									ok = FALSE;
									InformWarning(_R(IDS_BAR_DOESNT_EXIST));
								}
							}

							if (ok)
							{
								OpParamBarCreationDlg Param(TRUE /*editing*/, g_BarName, SliceHelper::CountButtonsInBar(g_BarName));
								pOpDesc->Invoke((OpParam*)&Param);
							}
						}
						else
						{
							ERROR3("Couldn't find OPTOKEN_BARCREATIONDLG op descriptor");
						}
					}
					break;
				
				case _R(IDC_STATES):
					// Brings up the edit/create bar dlg
					{
						OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_BARSTATESDLG); 
						if (pOpDesc != NULL)
						{
							BOOL ok = TRUE;
							// nothing selected then test that the bar we think we are editing exists
							// warn if it doesn't
							if (!m_EditingBar)
							{
								String_256 DefLayerName;
								DefLayerName.Load(_R(IDS_ROLLOVER_DEFAULT));
								Layer * pDef = SliceHelper::FindLayerCalled(DefLayerName);

								if (pDef == NULL || !SliceHelper::BarNameExists(pDef, g_BarName))
								{
									ok = FALSE;
									InformWarning(_R(IDS_BAR_DOESNT_EXIST));
								}
							}

							if (ok)
							{
								OpParamBarStatesDlg ParamBarStatesDlg(g_BarName);
								pOpDesc->Invoke(&ParamBarStatesDlg);
							}
						}
					}
					break;
				
				case _R(IDC_REDEFINE_STATE):
					// Brings up the edit/create bar dlg
					{
						OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_BARREDEFINESTATEDLG); 
						if (pOpDesc != NULL)
						{
							FindCurrentState();
							BOOL ok = TRUE;
							// nothing selected then test that the bar we think we are editing exists
							// warn if it doesn't
							if (!m_EditingBar)
							{
								String_256 DefLayerName;
								DefLayerName.Load(_R(IDS_ROLLOVER_DEFAULT));
								Layer * pDef = SliceHelper::FindLayerCalled(DefLayerName);

								if (pDef == NULL || !SliceHelper::BarNameExists(pDef, g_BarName))
								{
									ok = FALSE;
									InformWarning(_R(IDS_BAR_DOESNT_EXIST));
								}
							}

							if (ok)
							{
								OpParamBarRedefineStateDlg ParamBarRedefineStateDlg(g_BarName, m_TopVisibleState);
								pOpDesc->Invoke(&ParamBarRedefineStateDlg);
							}
						}
					}
					break;

				case _R(IDC_LIVE): // is the stretching live or not???
					{
						NodeBarProperty* pNodeBarProperty = (NodeBarProperty*) Document::GetCurrent()->GetSetSentinel()->FindBarProperty();
						INT32 BarNumber = SliceHelper::GetBarNumberFromBarName(g_BarName);

						if (pNodeBarProperty  && BarNumber < pNodeBarProperty->HowMany())
						{
							BarDataType NewBarData = pNodeBarProperty->Bar(BarNumber);
							NewBarData.IsLive = !NewBarData.IsLive;

							NameGallery * pNameGallery = NameGallery::Instance();
							if (pNameGallery)
								pNameGallery->FastUpdateNamedSetSizes();

							pNodeBarProperty->MakeChange(BarNumber, NewBarData);
						}
					}

					break;
				
				}
			break;
		}

	}
	else if (m_InfoBarCreated && MESSAGE_IS_A(Message, SpreadMsg))
	{
		SpreadMsg*	pSpreadMsg = (SpreadMsg*) Message;

		switch (pSpreadMsg->Reason)
		{
			case SpreadMsg::LAYERCHANGES:
				UpdateCurrentStateGadget();
				break;
		}
	}
	else if (m_InfoBarCreated && MESSAGE_IS_A(Message, LayerMsg))
	{
		LayerMsg *TheMsg = (LayerMsg *) Message;

		if (TheMsg->Reason == LayerMsg::LAYER_VISIBILITY_CHANGED)
			{
				UpdateCurrentStateGadget();
			}
	}

	// Karim MacDonald - 20/10/1999
	// for whenever someone announces that they have changed the selection.
	// FYI, this SelChangingMsg is also sent when you do (SelRange*)->Update(TRUE).
	else if (m_InfoBarCreated && MESSAGE_IS_A(Message, SelChangingMsg))
	{
		SelChangingMsg* pSelChange = (SelChangingMsg*)Message;

		if (pSelChange->State == SelChangingMsg::SelectionState::SELECTIONCHANGED ||
			pSelChange->State == SelChangingMsg::SelectionState::NONCOLOURATTCHANGED)
		{
			OnSelectionChanged();
		}
	}

	// Karim 06/07/2000 - the slice tool needs to know if the view changes.
	else if (MESSAGE_IS_A(Message, DocViewMsg))
	{
		if (pSliceTool != NULL)
			pSliceTool->ViewChanged( *((DocViewMsg*)Message) );
	}

	// If we've changed to a different document then get rid of the dlg.
	else if (MESSAGE_IS_A(Message, DocChangingMsg))
	{
		DocChangingMsg* TheMsg = (DocChangingMsg*) Message;
		if (TheMsg->State == DocChangingMsg::BORN || TheMsg->State == DocChangingMsg::SELCHANGED)
		{
			// this will be a new set of named sets!
			NameGallery * pNameGallery = NameGallery::Instance();
			if (pNameGallery)
				pNameGallery->FastUpdateNamedSetSizes();
		}
	}

	// Pass the message on to the immediate Slice class
	return (InformationBarOp::Message(Message));
}    


/********************************************************************************************

>	static INT32 SliceInfoBarOp::FindCurrentState()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/99
	Returns:	An ID as to what layers exist and are visible
				from the specailly named rollover states.

				0 = default
				1 = mouse
				2 = clicked
				3 = selected
				4 = all the above
				5 = doesn't exactly match any of the above

				NB. matches the order of the combo box options

	Purpose:	Scans the layers looking at which named states are visible

********************************************************************************************/

INT32 SliceInfoBarOp::FindCurrentState()
{
	// find a spread?
	Spread* pSelSpread = Document::GetSelectedSpread();
	if (pSelSpread == NULL)
		return 6;

	// init the layer pointers
	Layer * pLayer = pSelSpread->FindFirstLayer();

	BOOL ActiveState[4];
	String_32 StateName[4];

	StateName[DEFAULT].Load(_R(IDS_ROLLOVER_DEFAULT)); // = "Default";
	StateName[MOUSE].Load(_R(IDS_ROLLOVER_MOUSE)); // = "Mouse";
	StateName[CLICKED].Load(_R(IDS_ROLLOVER_CLICKED)); // = "Clicked";
	StateName[SELECTED].Load(_R(IDS_ROLLOVER_SELECTED)); // = "Selected";

	INT32 i = 0;

	// init the bools
	for (i = 0; i < 4 ; i++)
		ActiveState[i] = FALSE;

	m_TopVisibleState = -1;

	while (pLayer)
	{
		for (i = 0; i < 4 ; i++)
		{
			if (pLayer->GetLayerID().CompareTo(StateName[i]) == 0)
			{
				ActiveState[i] = pLayer->IsVisible();
				if (ActiveState[i])
				{
					if (m_TopVisibleState == -1 || m_TopVisibleState > i)
						m_TopVisibleState = i;
				}
			}
		}

		pLayer = pLayer->FindNextLayer();
	}


	if (ActiveState[0] && ActiveState[1] && (ActiveState[2] || !INCLUDE_CLICKED_STATE ) && ActiveState[3])
		return 4; // all states

	if (ActiveState[0] && !ActiveState[1] && !ActiveState[2] && !ActiveState[3])
		return 0; // default state

	if (!ActiveState[0] && ActiveState[1] && !ActiveState[2] && !ActiveState[3])
		return 1; // mouse state

	if (!ActiveState[0] && !ActiveState[1] && ActiveState[2] && !ActiveState[3])
		return 2; // clicked state

	if (!ActiveState[0] && !ActiveState[1] && !ActiveState[2] && ActiveState[3])
		return 3; // selected state

	if (!ActiveState[0] && !ActiveState[1] && !ActiveState[2] && !ActiveState[3])
		return 5; // no states

	return 6; // another state
}

/********************************************************************************************

>	void SliceInfoBarOp::OnSelectionChanged()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/99
	Returns:	-
	Purpose:	Should be called everytime something changes the selection,
				so that the correct buttons are highlit and the correct
				text is placed in the barname edit field.
********************************************************************************************/
void SliceInfoBarOp::OnSelectionChanged()
{
	// find out what the bar id should be - the next one to be used or the selected one
	Spread* pSpread = Document::GetSelectedSpread();

	// is it part of a animation?
	// test the current frame if it is a frame or a layer
	if (pSpread != NULL)
	{
		Layer * ActiveLayer = pSpread->FindActiveLayer();
		if (ActiveLayer && ActiveLayer->IsFrame())
		{
			// can't do lots of these things if we are in an animation
			EnableGadget(_R(IDC_NEW_BAR), FALSE);
			EnableGadget(_R(IDC_EDIT_BAR), FALSE);
			EnableGadget(_R(IDC_STATES), FALSE);
			EnableGadget(_R(IDC_LIVE), FALSE);
			EnableGadget(_R(IDC_ST_EXPORT), FALSE);
			EnableGadget(_R(IDC_ST_STATE), FALSE);
		}
		else
		{
			// get the selection
			Range Sel(*(GetApplication()->FindSelection()));

			// set the range flags so it includes shadow and bevel manager nodes
			RangeControl rg = Sel.GetRangeControlFlags();
			rg.PromoteToParent = TRUE;
			rg.IgnoreInvisibleLayers = TRUE;
			Sel.Range::SetRangeControl(rg);

			m_EditingBar = FALSE;
			g_BarName = GetNextUnusedBarName(&Sel, &m_EditingBar);
			INT32 BarNo = SliceHelper::GetBarNumberFromBarName(g_BarName);
			NodeBarProperty* pNodeBarProperty = (NodeBarProperty*) Document::GetCurrent()->GetSetSentinel()->FindBarProperty();
			BOOL IsBar = pNodeBarProperty && BarNo < pNodeBarProperty->HowMany();

			SetStringGadgetValue(_R(IDC_ST_BARNAME), &g_BarName);

			EnableGadget(_R(IDC_NEW_BAR), !m_EditingBar && Sel.Count() >= 1);
			EnableGadget(_R(IDC_EDIT_BAR), TRUE /*m_EditingBar || BarNo > 0*/);
			EnableGadget(_R(IDC_STATES), TRUE /*m_EditingBar || BarNo > 0*/);
			EnableGadget(_R(IDC_LIVE), IsBar);
			EnableGadget(_R(IDC_ST_EXPORT), TRUE);
			EnableGadget(_R(IDC_ST_STATE), TRUE);

			// set the live tick
			SetLongGadgetValue(_R(IDC_LIVE), IsBar && pNodeBarProperty->Bar(BarNo).IsLive);
		}
	}

	// Karim - 19/10/1999
	// inform the SliceTool that the selection has changed.
	if (pSliceTool != NULL)
		pSliceTool->SelectionHasChanged();
}


/********************************************************************************************

>	String_256 SliceInfoBarOp::GetNextUnusedBarName(Range * pSel, BOOL * pFromSel)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/99
	Returns:	The text that is the bar name to put in the bar name field
	Params:		pSel	-	A pointer to the selection range
				pFromSel-	Fills in this bool that says if the name
							was taken from the selection or not.
	Purpose:	It checks the selection to see if it is part of a bar.
				If it was it returns that name.
				If not it returns the next good name to use for a bar,
				that isn't currently in use.
********************************************************************************************/
String_256 SliceInfoBarOp::GetNextUnusedBarName(Range * pSel, BOOL * pFromSel)
{
	String_256 NewBarName = "";

	*pFromSel = FALSE;

	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread == NULL)
		return g_BarName;

	if (pSel)
	{
		Node * pCurrent = pSel->FindFirst();
		// for each node in the selection
		while (pCurrent)
		{
			// does the selection contain a named bar ?
			// if it does we will edit this and return it here
			if (ScanFromNodeForBarMembers(pCurrent, NewBarName))
			{
				*pFromSel = TRUE;
				return NewBarName;
			}

			pCurrent = pSel->FindNext(pCurrent);
		}

		// no actual bar in selection so does the selection cross a bar?
		SliceHelper::BarNameInRect(pSel->GetBoundingRect(), &g_BarName);
	}

	return g_BarName; // keep what we had before
}


/********************************************************************************************

>	BOOL SliceInfoBarOp::ScanFromNodeForBarMembers(Node * pNode, String_256 &BarName, String_256 * pButtonName)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/99
	Returns:	The text that is the bar name to put in the bar name field
	Params:		pNode		-	The node to check from
				BarName		-	Fills in this string with the name of the bar found.
				pButtonName	-	If provided fills in the button name found in a bar.
	Purpose:	Checks from a given node down looking for the first template attrib (set name)
				Once found it returns TRUE and fills in the bar name string from the question
				of the template attrib and if a ptr to a string was passed for the button name
				it fills this in too.
				NB. This function is RECURSIVE
********************************************************************************************/
BOOL SliceInfoBarOp::ScanFromNodeForBarMembers(Node * pNode, String_256 &BarName, String_256 * pButtonName)
{
	if (!BarName.IsEmpty())
		return TRUE;

	if (pNode->IsAnAttribute())
	{
		if (pNode->IsKindOf(CC_RUNTIME_CLASS(TemplateAttribute)))
			{
				BarName = SliceHelper::GetBarName((TemplateAttribute *)pNode);

				// found a bar name? then set the button name too
				if (!BarName.IsEmpty() && pButtonName)
					*pButtonName = ((TemplateAttribute *)pNode)->GetParam();
			}
	}
	else // find anything else interesting?
	{
		Node * pChildNode = pNode->FindFirstChild();

		while (pChildNode)
		{
			// ***recursive call***
			if (ScanFromNodeForBarMembers(pChildNode, BarName, pButtonName))
				return TRUE;
			pChildNode = pChildNode->FindNext();
		}
	}

	return FALSE;
}



/********************************************************************************************

>	void SliceInfoBarOp::DealWithSingleClick(DocCoord PointerPos, 
					ClickModifiers Mods, Spread * pSpread);

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com> from DavidMc
	Created:	4/10/99	&   16/6/99
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Deals with a single click situation (i.e. no drag occurred)
	
********************************************************************************************/
/*
 *	Commented out by Karim MacDonald, 14/10/1999.
 *	This function has been replaced by HandleClick() and the other
 *	Handle...() functions.

void SliceInfoBarOp::DealWithSingleClick(DocCoord PointerPos, ClickModifiers Mods, 
											Spread * pSpread)
{
	// first, find out if the click is over an object
	NodeRenderableInk * pInk = NodeRenderableInk::FindSimpleAtPoint(pSpread, PointerPos);
	NodeRenderableInk * pCompound = NodeRenderableInk::FindCompoundFromSimple(pInk);

	if (pCompound)
	{
		pInk = pCompound;
	}

	// has the shift key been pressed ?
	BOOL bShift = Mods.Adjust;
	BOOL bRedraw = FALSE;
	SelRange Sel(*(GetApplication()->FindSelection()));

	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);

	DocRect RedrawRect = Sel.GetBlobBoundingRect();

	Document * pDoc = Document::GetCurrent();

	BlobManager* BlobMgr = GetApplication()->GetBlobManager();

	BlobStyle bs1(TRUE);
	BlobStyle bs2(FALSE);
	
	if (pDoc && BlobMgr)
	{
		bs1 = BlobMgr->GetCurrentInterest();
		BlobMgr->ToolInterest(bs2);
		pDoc->ForceRedraw(Document::GetSelectedSpread(), RedrawRect);		
	}

	if (pInk)
	{
		String_256 SetName;
		SetName.Empty();

		BOOL WasSelected = pInk->IsSelected();

		// change the selection
		if (!bShift)
		{
			NodeRenderableInk::DeselectAll(TRUE, FALSE);
		}

		// set name shows the last set found
		// find the top shape on this layer
		Node *pParent = pInk->FindParent();
		Node *pNode = pInk;
		while (!pParent->IsLayer())
		{
			pNode = pParent;
			pParent = pNode->FindParent();
		}
		SliceHelper::SelectAllSetsOfThisNode(pNode, SetName, bShift);
		
		// the clicked chappie isn't in a set just select him
		if (SetName.IsEmpty())
		{
			if (bShift)
				pInk->SetSelected(!WasSelected);
			else
				pInk->SetSelected(TRUE);
		}

		GetApplication()->UpdateSelection();
		
		OnSelectionChanged();
	}
	else
	{
		// deselect everything
		NodeRenderableInk::DeselectAll(TRUE, FALSE);
		GetApplication()->UpdateSelection();

		OnSelectionChanged();
	}

	// force a redraw on the area
	if (pDoc && BlobMgr)
	{
		BlobMgr->ToolInterest(bs1);
		pDoc->ForceRedraw(Document::GetSelectedSpread(), RedrawRect);		
	}
}*/



//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------



/********************************************************************************************

>	void OpSliceDragBox::StartDragBox(	Spread* pSpread,
										DocCoord Anchor,
										ClickModifiers ClickMods)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>, from Rik's OpSelectorDragBox code.
	Created:	11/10/1999
	Inputs:		Anchor - The position of the mouse at the start of the Drag
	Purpose:	This is called when a Drag operation has been detected

********************************************************************************************/
void OpSliceDragBox::StartDragBox(Spread* pSpread, DocCoord Anchor, ClickModifiers ClickMods)
{
	// We had better take a note of the starting point of the drag
	StartSpread = pSpread;
	StartPoint = Anchor;
	LastMousePosition = Anchor;

	// Put some helpful text in the status bar.
	SliceTool::SetStatusText(_R(IDS_SLICE_DRAGBOXTEXT));

	// And tell the Dragging system that we need drags to happen
	StartDrag(DRAGTYPE_AUTOSCROLL);
}



/********************************************************************************************

>	void OpSliceDragBox::DragFinished(	DocCoord PointerPos,
										ClickModifiers ClickMods,
										BOOL Success, BOOL bSolidDrag)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>, from Rik's OpSelectorDragBox code.
	Created:	11/10/1999
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickMods - the key modifiers being pressed
				Success - TRUE if the drag was terminated properly, FALSE if it
				was ended with the escape key being pressed
	Purpose:	This is called when a drag operation finishes. This removes the EORed drag
				rect from the screen and then selects all the objects that were in it.
				It then goes through these objects, selecting all other objects sharing a name
				with any of them.
	SeeAlso:	ClickModifiers
********************************************************************************************/
void OpSliceDragBox::DragFinished(	DocCoord PointerPos,
									ClickModifiers ClickMods,
									Spread* pSpread, BOOL Success, BOOL bSolidDrag)
{
	// Build the rectangle of the drag box at the end of the drag
	DocRect BoundingRect(MIN(StartPoint.x, LastMousePosition.x),
						 MIN(StartPoint.y, LastMousePosition.y),
						 MAX(StartPoint.x, LastMousePosition.x),
						 MAX(StartPoint.y, LastMousePosition.y));

	// First Rub out the old box
	RenderDragBlobs(BoundingRect, StartSpread, bSolidDrag);

	// Put the hourglass up
	BeginSlowJob();

	// we need this info in scope here, although it is used just below.
	SelRange* pSel = GetApplication()->FindSelection();
	RangeControl rcOld = pSel->GetRangeControlFlags();

	// Go and try and select a few things
	if (Success)
	{
		// If we didn't drag with the right button then deselect everything prior to selecting
		// the "lasso-ed" objects.
		if (!ClickMods.Adjust)
			NodeRenderableInk::DeselectAll(TRUE, FALSE);

		// Select the objects in the BoundingRect
		RangeControl rg = rcOld;
		rg.IgnoreInvisibleLayers = TRUE;
		rcOld = rg;
		rg.PromoteToParent = TRUE;
		pSel->Range::SetRangeControl(rg);
		SliceHelper::SelectAllSetsInRect(BoundingRect, pSpread,
										   SliceHelper::SelStateAction::SET);

		// End the Drag
		if (!EndDrag())
			FailAndExecute();
	}
	else
	{
		// Set up the flags that say it all went wrong.
		EndDrag();
		FailAndExecute();
	}

	// return the app's selection range flags to their original state.
	pSel->Range::SetRangeControl(rcOld);

	// Final end of the operation.
	End();
}



/********************************************************************************************
>	BOOL OpSliceDragBox::Declare()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>, from Rik's OpSelectorDragBox code.
	Created:	11/10/1999
	Returns:	TRUE if all went OK, False otherwise.
	Purpose:	Adds the operation to the list of all known operations.
********************************************************************************************/
BOOL OpSliceDragBox::Declare()
{	
	return RegisterOpDescriptor(0, _R(IDS_SLICE_BOX), CC_RUNTIME_CLASS(OpSliceDragBox), 
								OPTOKEN_SLICE_DRAGBOX, OpSliceDragBox::GetState);
}



/*******************************************************************

>	OpState OpSliceDragBox::GetState(String_256*, OpDescriptor*)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>, from Rik's OpSelectorDragBox code.
	Created:	11/10/1999
	Inputs:		
	Outputs:	Description		GetState fills this string with an approriate description
								of the current state of the SliceTool.
	Returns:	The state of the operation, so that menu items (ticks and greying) can be
				done properly.
	Purpose:	Find out the state of the operation at the specific time.
	Errors:		
	See also:	

*******************************************************************/
OpState OpSliceDragBox::GetState(String_256*, OpDescriptor*)
{
	OpState os;

	return os;
}



//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------



RectListItem::RectListItem(DocRect source) : m_Rect(source)
{
	// empty.
}



DocRect RectListItem::GetRect()
{
	return m_Rect;
}



//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------



/********************************************************************************************

>	OpSliceTranslate::OpSliceTranslate()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>, from Rik's OpTranslateTrans code.
	Created:	13/10/1999
	Purpose:	Constructor.

********************************************************************************************/

OpSliceTranslate::OpSliceTranslate() : TransOperation()
{
	//Set status line help
	StatusHelpID = _R(IDS_TRANSLTRANS_STATUS1);
	StatusHelpID2 = _R(IDS_TRANSLTRANS_STATUS2);
	CanScaleLines = FALSE;
}




/********************************************************************************************

>	void OpSliceTranslate::InitTransformImmediate(OpParam* pOpParam)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>, from Rik's OpTranslateTrans code.
	Created:	13/10/1999
	Inputs:		pOpParam - The parameters that were passed into the operation
	Purpose:	Sets up the transform ready for an immediate translation. This is called from
				DoWithParam()
	SeeAlso:	TransOperation::DoWithParam()

********************************************************************************************/

void OpSliceTranslate::InitTransformImmediate(OpParam* pOpParam)
{
	// Set the initial position
	StartPos = DocCoord(0,0);
	RawStartPos = StartPos;
	MagStartPos = StartPos;

	// and copy the offset to translate by from Param2
	DocCoord* Offset = (DocCoord*)(pOpParam->Param2);
	LastPos.x = Offset->x;
	LastPos.y = Offset->y;

	OriginalGridOffset.x=0;
	OriginalGridOffset.y=0;
}


/********************************************************************************************

>	virtual void OpSliceTranslate::InitTransformOnDrag(DocCoord PointerPos, ClickModifiers ClickMods)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>, from Rik's OpTranslateTrans code.
	Created:	13/10/1999
	Inputs:		PointerPos - The position of the mouse at the start of the drag
				ClickMods - the keyboard modifiers that were active at the start of the drag
	Purpose:	Sets up the parameters needed to build the transform matrix at the start
				of the drag. The base class version of this function does nothing.

********************************************************************************************/

void OpSliceTranslate::InitTransformOnDrag(DocCoord PointerPos, ClickModifiers ClickMods)
{
	// make a note of the current mouse position
	LastPos = PointerPos;

	// Record the offset from the mouse pos to the grid
	OriginalGridOffset = GetStartPos();
	DocView::ForceSnapToGrid(StartSpread, &OriginalGridOffset);
	OriginalGridOffset = GetStartPos() - OriginalGridOffset;
}




/********************************************************************************************

>	virtual void OpSliceTranslate::UpdateTransformOnDrag(DocCoord PointerPos, Spread* pSpread,
									ClickModifiers& ClickMods)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>, from Rik's OpTranslateTrans code.
	Created:	13/10/1999
	Inputs:		PointerPos - The latest position of the mouse
	Purpose:	Updates the op's state variables (eg selection bounding rect) and
				its blobs whenever the mouse moves.

********************************************************************************************/

void OpSliceTranslate::UpdateTransformOnDrag(	DocCoord PointerPos, Spread* pSpread,
												ClickModifiers& ClickMods	)
{
	// get the bounding box of the dragged selection and update it's position.
	DocCoord Offset = PointerPos - LastPos;
	DocRect Bounds(	BoundingData.x, BoundingData.y,
					BoundingData.x+BoundingData.Width,
					BoundingData.y+BoundingData.Height );
	Bounds.lo = Bounds.lo + Offset;
	Bounds.hi = Bounds.hi + Offset;
	DocRect SnappedBounds = Bounds;

	// if this drag is not being constrained, then see if we can snap to anything.
	if (!ClickMods.Constrain)
	{
		// Apply snapping to the pointer pos
		// First apply magnetic snapping alone
		if (MagneticGripPoint && DocView::SnapSelected(pSpread, &PointerPos, TRUE, FALSE))
		{
			// Magnetic snapping worked!
		}
		else
		{
			// Magnetic snapping failed!

			// If magnetic snapping failed then try grid snapping
			// on the adjusted coordinate
			if (ClickMods.Alternative1)
			{
				PointerPos = PointerPos - OriginalGridOffset;
				DocView::SnapSelected(pSpread, &PointerPos, FALSE, TRUE);
				PointerPos = PointerPos + OriginalGridOffset;
			}

			// ok, try snapping the selection's bounds to anything in the document,
			// eg guidelines, other objects etc.
			else
			{
				if (DocView::SnapSelected(pSpread,&SnappedBounds,LastRawPos,RawPos))
					PointerPos = PointerPos + (SnappedBounds.lo - Bounds.lo);

				// right, nothing else has worked - how about trying to snap our bounding
				// rect to one of the bounding rects in our list?
				else
				{
					if (SnapRectToBoxList(pSpread, &SnappedBounds, LastRawPos, RawPos))
						PointerPos = PointerPos + (SnappedBounds.lo - Bounds.lo);
				}
			}
		}
	}

	// Update BoundingData's offset information and pass it to Bounds.
	INT32 dx = PointerPos.x - LastPos.x;
	INT32 dy = PointerPos.y - LastPos.y;
	BoundingData.x += dx;
	BoundingData.y += dy;
	BoundingData.XYChanged = TRUE;
	Bounds.lox = BoundingData.x;
	Bounds.loy = BoundingData.y;
	Bounds.hix = BoundingData.x + BoundingData.Width;
	Bounds.hiy = BoundingData.y + BoundingData.Height;

	// do a collision-detection between the selection bounding rect and a list
	// of named-set bounding rect's we built at the start of the drag.
	// highlight our selection bounding rect if we collide.
	if (TestBoundingBoxCollision(&Bounds, pSpread))
	{
		// new collision? if so, draw fresh blobs.
		if (!m_bDrawingBlobs)
		{
			m_DragRect = Bounds;
			m_bDrawingBlobs = TRUE;
			RenderDragBlobs(m_DragRect, StartSpread, FALSE);
		}

		// otherwise, update the current blobs.
		else
		{
			// rub out old collision blobs.
			RenderDragBlobs(m_DragRect, StartSpread, FALSE);

			// update the record of the selection bounds.
			m_DragRect = Bounds;

			// draw new collision blobs.
			RenderDragBlobs(m_DragRect, StartSpread, FALSE);
		}
	}

	// we're not presently colliding, so if we have to then
	// rub out any old blobs and invalidate the drag-rect.
	else
	{
		if (m_bDrawingBlobs)
		{
			// don't render the blobs from here anymore, and render them off.
			m_bDrawingBlobs = FALSE;
			RenderDragBlobs(m_DragRect, StartSpread, FALSE);

			// invalidate the drag-rect, so that it won't be rendered from RenderDragBlobs().
			m_DragRect.hix = m_DragRect.lox - 1;
		}
	}

	// Make a mental note of the current position
	LastPos = PointerPos;

	// Update the current spread (must do this if CanChangeSpread returns TRUE)
	CurrentSpread = pSpread;
}



/********************************************************************************************

>	virtual BOOL OpSliceTranslate::CanChangeSpread()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/June/2006
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this transform allows the drag to be transferred to another spread
	Purpose:	Tell the baseclass functions whether to draw drag feedback only on the start
				spread or to allow drag rendering to be done on other spreads too.

********************************************************************************************/

BOOL OpSliceTranslate::CanChangeSpread()
{
	return TRUE;
}


/********************************************************************************************

>	BOOL OpSliceTranslate::SnapRectToBoxList(	Spread* pSpread, DocRect* pDocRect,
												const DocCoord& PrevCoord,
												const DocCoord& CurCoord )
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/1999
	Inputs:		pSpread		a pointer to the relevant spread node.
				pDocRect	a pointer to the rectangle to snap.
				PrevCoord	a reference to the ???
				CurCoord	a reference to ???
	Outputs:	pDocRect may be moved so that it snaps to the side of one of this op's
				list of collision-rectangles.
	Returns:	TRUE if pDocRect was snapped,
				FALSE otherwise.
	Purpose:	Performs a snap check between the given rect and this op's own collection of
				collision bounding rects. The given rect is tested successively against each
				member of our list, and if it lies within snapping distance of any rectangle
				it will be shifted to snap to that rectangle.

				This function does not live with all the other snap functions because it's
				use is restricted solely to the context of this operation.
	Errors:		
	See also:	All Snap member functions of all classes derived from NodeRenderableBounded.

********************************************************************************************/
BOOL OpSliceTranslate::SnapRectToBoxList(	Spread* pSpread, DocRect* pDocRect,
											const DocCoord& PrevCoord,
											const DocCoord& CurCoord )
{
	// iterate over our list of rectangles and check pDocRect against each one.
	BOOL bSnapped = FALSE;
	RectListItem* pCurListItem = (RectListItem*)m_lBoxes.GetHead();
	while (!bSnapped && pCurListItem != NULL)
	{
		bSnapped = CSnap::SnapRectToRect(pDocRect, pCurListItem->GetRect());
		pCurListItem = (RectListItem*)m_lBoxes.GetNext(pCurListItem);
	}

	return bSnapped;
}



/********************************************************************************************

>	void OpSliceTranslate::BuildMatrix()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>, from Rik's OpTranslateTrans code.
	Created:	13/10/1999
	Purpose:	Builds the transform matrix required to rotate the selection about the
				point CentreOfRot by the required number of degrees

********************************************************************************************/

void OpSliceTranslate::BuildMatrix()
{
	// Build a translation matrix by takeing the offset from the last mouse position
	// to the start mouse position
		Transform = Matrix(LastPos.x-GetStartPos().x, LastPos.y-GetStartPos().y);
}



/********************************************************************************************

>	virtual BOOL OpSliceTranslate::ShouldPointerBeOffset()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>, from Rik's OpTranslateTrans code.
	Created:	13/10/1999
	Returns:	FALSE
	Purpose:	Tells the base class of the operation that we want our mouse coords to be
				left alone

********************************************************************************************/

BOOL OpSliceTranslate::ShouldPointerBeOffset()
{
	return FALSE;
}




/********************************************************************************************

>	virtual void OpSliceTranslate::ConstrainDrag(DocCoord* PointerPos)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>, from Rik's OpTranslateTrans code.
	Created:	13/10/1999
	Inputs:		PointerPos - The current position of the mouse that needs to be constrained
	Outputs:	PointerPos - The position of the mouse after it has been constrained
	Purpose:	Will constrain the mouse position to lie along rays from the centre of
				rotation at the constrain angle apart (45 degrees by default)

********************************************************************************************/

void OpSliceTranslate::ConstrainDrag(DocCoord* PointerPos)
{
	// Lock the mouse to move along the axis or diagonally
	DocCoord Blobby = GetStartPos();
	DocView::ConstrainToAngle(Blobby, PointerPos);
}



/********************************************************************************************

>	BOOL OpSliceTranslate::Declare()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>, from Rik's OpTranslateTrans code.
	Created:	13/10/1999
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpSliceTranslate::Declare()
{
	return (RegisterOpDescriptor(0, _R(IDS_SLICE_MOVE), CC_RUNTIME_CLASS(OpSliceTranslate),
								OPTOKEN_SLICE_TRANSLATE, TransOperation::GetState)); 
}



/********************************************************************************************

>	void OpSliceTranslate::CompileCollisionBoxList()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/1999
	Inputs:		
	Outputs:	Fills OpSliceTranslate::m_lBoxes with the required
				list of collision-test bounding boxes.
	Returns:	
	Purpose:	Make a list of the overall bounding rects of all named sets
				which have no members selected. These are used to collision
				detect against during the drag.
	Errors:		
	See also:	

********************************************************************************************/
void OpSliceTranslate::CompileCollisionBoxList()
{
	// iterate through the current list of named sets. if a named set
	// has no objects selected, then add its collective bounding rect
	// to the list of bounding rects, for collision detection.
	NameGallery * pNameGallery = NameGallery::Instance();
	//pNameGallery->ForceUpdate();
	SGUsedNames* pNames = pNameGallery ? pNameGallery->GetUsedNames() : NULL;
	SGNameItem* pItem = pNames ? (SGNameItem*) pNames->GetChild() : NULL;

	m_lBoxes.DeleteAll();
	while (pItem != NULL)
	{
		if (pItem->IsNoneSelected())
			m_lBoxes.AddTail(new RectListItem(pItem->GetSetBounds()));

		// get next name.
		pItem = (SGNameItem*)pItem->GetNext();
	}
}



/********************************************************************************************

>	BOOL OpSliceTranslate::TestBoundingBoxCollision(Rect* pTestRect)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/1999
	Inputs:		pTestRect	the rectangle to test against each member of m_lBoxes.
				pSpread		a pointer to the current spread, so that we can render
							the collided rectangle(s?) if necessary.
	Outputs:	
	Returns:	TRUE if a collision is detected,
				FALSE otherwise.
	Purpose:	Test the given bounding rect against the list of bounding rects,
				m_lBoxes, and return TRUE if it overlaps any of them.

				TODO: we may want to change this to a void function.

	Additional:	Karim MacDonald 07/02/2000
				This function now always returns FALSE, to prevent visual artifacts when
				dragging with the slice-tool, a result of the re-introduction of a dashed 
				line around the selection during a drag.

	Errors:		ERROR2 if any parameter is NULL.
	See also:	

********************************************************************************************/
BOOL OpSliceTranslate::TestBoundingBoxCollision(DocRect* pTestRect, Spread* pSpread)
{
	ERROR2IF(pTestRect == NULL || pSpread == NULL, FALSE,
		"OpSliceTranslate::TestBoundingBoxCollision- called with a NULL parameter");

	// iterate over the list, checking each rect against pTestRect.
	RectListItem* pListItem = (RectListItem*)m_lBoxes.GetHead();
	BOOL bCollision = FALSE;
	while (!bCollision && pListItem != NULL)
	{
		bCollision = pTestRect->IsIntersectedWith(pListItem->GetRect());

		// get the next object in the list.
		if (!bCollision)
			pListItem = (RectListItem*)m_lBoxes.GetNext(pListItem);
	}

//	return bCollision;
	return FALSE;
}



/********************************************************************************************

>	void OpSliceTranslate::DragStarted(	TransformData* TransData,
										DragTool* pTool,
										TransformBoundingData* pBounds,
										DocCoord ClickPos,
										Spread* pSpread,
										ClickModifiers ClickMods,
										DocCoord ClickOffset, 
										Node* NodeToTransform,
										DragType dragtype )

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/1999
	Inputs:		see TransOperation::DragStarted
	Purpose:	When we start a drag, we want to compile a list of bounding boxes which
				belong to non-selected named sets, eg if there is a group of objects called
				'Button One' and it is not selected, we want to add its overall bounding box
				to a list, so we can collision detect against it later.

				Once that's done we call the base class, to start the drag.
	Errors:		
	See also:	

********************************************************************************************/
void OpSliceTranslate::DragStarted(	TransformData* TransData,
									DragTool* pTool,
									TransformBoundingData* pBounds,
									DocCoord ClickPos,
									Spread* pSpread,
									ClickModifiers ClickMods,
									DocCoord ClickOffset, 
									Node* NodeToTransform,
									DragType dragtype )
{
	// compile a list of any names which are *not* used by the current selection,
	// and use this to compile a list of bounding boxes which we can collision-test
	// against during the drag, to announce if we cross any other named sets.
	CompileCollisionBoxList();
	m_bDrawingBlobs = FALSE;

	// call the base-class drag-start routine.
	TransOperation::DragStarted(TransData, pTool, pBounds, ClickPos, pSpread, ClickMods,
								ClickOffset, NodeToTransform, dragtype);
}



/********************************************************************************************

>	void OpSliceTranslate::DragFinished(DocCoord dcPos, ClickModifiers ClickMods,
										Spread* pSpread, BOOL bSuccess, BOOL bSolidDrag)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/1999
	Inputs:		dcPos		
				ClickMods	
				pSpread		
				bSuccess	
	Outputs:	
	Returns:	
	Purpose:	Called when the selection-drag finishes, either normally or having been
				cancelled by the user. Performs tidy-up when the drag is over.

				This function calls the base-class for its main functionality.
	Errors:		
	See also:	

********************************************************************************************/
void OpSliceTranslate::DragFinished(DocCoord dcPos, ClickModifiers ClickMods,
									Spread* pSpread, BOOL bSuccess, BOOL bSolidDrag)
{
	// have we finished drawing blobs, or do we need to erase them?
	if (m_bDrawingBlobs)
	{
		RenderDragBlobs(m_DragRect, StartSpread, bSolidDrag);		// erase them.
		TransOperation::RenderDragBlobs(m_DragRect, StartSpread, bSolidDrag);
	}

	m_lBoxes.DeleteAll();
	TransOperation::DragFinished(dcPos, ClickMods, pSpread, bSuccess, bSolidDrag);
}



/********************************************************************************************

>	void SliceInfoBarOp::UpdateCurrentStateGadget()

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/00
	Returns:	
	Purpose:	Quickly scans the tree to find the layers shown then sets the drop down control
				which displays the current button state.
				Includes the #define code for including the "clicked" state or not.

********************************************************************************************/
void SliceInfoBarOp::UpdateCurrentStateGadget()
{
	INT32 state = FindCurrentState();

/*	// set the text in the redefine state button
	String_256 RedefineText = "";
	String_256 Temp(_R(IDS_ROLLOVER_DEFAULT));
	RedefineText += " \"";
	if (m_TopVisibleState == 1)
		Temp.Load(_R(IDS_ROLLOVER_MOUSE));
	else if (m_TopVisibleState == 2)
		Temp.Load(_R(IDS_ROLLOVER_CLICKED));
	else if (m_TopVisibleState == 3)
		Temp.Load(_R(IDS_ROLLOVER_SELECTED));
	else if (m_TopVisibleState == 4)
		Temp.Load(_R(IDS_BACK_BAR));

	RedefineText.MakeMsg(_R(IDS_DESIGN_TEXT), (LPCSTR) Temp);
	SetStringGadgetValue(_R(IDC_REDEFINE_STATE), &RedefineText);
*/
	EnableGadget(_R(IDC_REDEFINE_STATE), m_TopVisibleState != -1);

	// set the text in the dropdown
	if (state >=0 && state <= 4 + INCLUDE_CLICKED_STATE)
		SetSelectedValueIndex(_R(IDC_ST_STATE), state);
	else
		SetStringGadgetValue(_R(IDC_ST_STATE), _R(IDS_MANY), FALSE, -1);
}
