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
// Created by DMc on 2/9/93

/*
*/

#include "camtypes.h"
//#include "ensure.h"
//#include "resource.h"	// _R(IDS_OUTOFMEMORY)
//#include "barsdlgs.h"
//#include "mario.h"
//#include "rik.h"
//#include "markn.h"
//#include "viewrc.h"
//#include "docview.h"
//#include "document.h"
//#include "mainfrm.h"
#include "infobar.h"
#include "csrstack.h"
//#include "app.h"
#include "osrndrgn.h"
#include "oilfiles.h"
#include "blobs.h"
//#include "viewrc.h"

#include "bevinfo.h"
#include "bevtool.h"
#include "module.h"
#include "ezmodule.h"
//#include "bevres.h"
#include "progress.h"
#include "opbevel.h"

//#include "will2.h"

#include "nodetxts.h"
#include "attrbev.h"
//#include "attrmgr.h"
//#include "nodebev.h"
#include "spread.h"
#include "cntrtool.h"
#include "gclips.h"
#include "bubbleid.h"
#include "objchge.h"
#include "nodemold.h"
#include "ndclpcnt.h"
#include "ncntrcnt.h"
//#include "simon.h"
//#include "xshelpid.h"
//#include "helppath.h"
#include "helpuser.h"
#include "dragmgr.h"
#include "lineattr.h"
//#include "cntres.h"
#include "strkattr.h"

#include "ophist.h"

#ifdef BUILDSHADOWS

// Set things up so that the tool will be listed in the Dialog box
DECLARE_SOURCE("$Revision: 671 $");


// These are still char* while we wait for resource technology to be developed for modules
TCHAR* BevelTool::FamilyName	= _T("Bevelling Tool");
TCHAR* BevelTool::ToolName 		= _T("Bevelling Tool");
TCHAR* BevelTool::Purpose 		= _T("To Bevel things");
TCHAR* BevelTool::Author 		= _T("DMc");

BevelInfoBarOp * BevelTool::pBevelInfoBarOp = NULL;
BOOL BevelTool::m_bActive = FALSE;
UINT32 BevelTool::StatusID   = _R(IDS_BEVELSTATUSHELP);
OpDragBevel *  BevelTool::m_pDragBevelOp = NULL;

CC_IMPLEMENT_MEMDUMP( BevelTool, Tool_v1 )
CC_IMPLEMENT_MEMDUMP( BevelBlob, ListItem )
CC_IMPLEMENT_DYNCREATE(OpDragBevel,SelOperation)

/////////////////////////////////////////////////////////
// standard flatness
const MILLIPOINT BevelToolFlatness=200;


// Better memory tracking please
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	BevelTool::BevelTool()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Dummp Constructor - It does nothing. All the real initialisation is done
				in BevelTool::Init which is called by the Tool Manager
	SeeAlso:	BevelTool::Init

********************************************************************************************/

BevelTool::BevelTool()
{
	// Set the cursor pointers to null
	pNormalCursor = NULL;
	pActiveCursor = NULL;
	pBevelInfoBarOp = NULL;
	pMouseOverCursor = NULL;
	m_bRegenBlobsOnNextRender = FALSE;
	m_LastZoom = FIXED16(0);
	m_bDisableBlobRendering = FALSE;
	m_pDragBevelOp = NULL;
}





/********************************************************************************************

>	BevelTool::~BevelTool()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Destructor (Virtual). Does nothing.

********************************************************************************************/

BevelTool::~BevelTool()
{
	// delete the attribute list
	m_BevelBlobList.DeleteAll();
	
	RemoveCursors();

	if (m_pDragBevelOp)
	{
		delete m_pDragBevelOp;
		m_pDragBevelOp = NULL;
	}
}



/********************************************************************************************

>	BOOL BevelTool::Init( INT32 Pass )

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Returns:	FALSE if it does not want to be created, TRUE otherwise
	Purpose:	Used to check if the Tool was properly constructed
	SeeAlso:	BevelTool::BevelTool

********************************************************************************************/

BOOL BevelTool::Init()
{
	BOOL ok = OpDragBevel::Declare();

	if (!m_pDragBevelOp)
		m_pDragBevelOp = new OpDragBevel;

	// This section reads in the infobar definition and creates an instance of
	// BevelInfoBarOp.  Also pBevelInfoBarOp, the ptr to the tool's infobar, is set up
	// after the infobar is successfully read and created.
	if (ok)
	{
#if 0
		CCResTextFile 		file;				// Resource File
		BevelInfoBarOpCreate BarCreate;			// Object that creates BlankInfoBarOp objects

	 	ok = file.open(_R(IDM_BEVEL_BAR), _R(IDT_INFO_BAR_RES));		// Open resource
		if (ok) ok = DialogBarOp::ReadBarsFromFile(file,BarCreate);	// Read and create info bar
		if (ok) file.close();									 	// Close resource

		ERROR3IF(!ok,"Unable to load bevbar.ini from resource\n"); 

		if (ok)
		{
			// Info bar now exists.  Now get a pointer to it
			String_32 str = String_32(_R(IDS_BEVELTOOL_INFOBARNAME));
			DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(str);
			ok = (pDialogBarOp != NULL);

			if (ok)
				ok = pDialogBarOp->IsKindOf(CC_RUNTIME_CLASS(BevelInfoBarOp));

			if (ok)
			{
				pBevelInfoBarOp = (BevelInfoBarOp*)pDialogBarOp;
				pBevelInfoBarOp->pTool = this;
			}
		}
#endif
		pBevelInfoBarOp = new BevelInfoBarOp();
		ok = (pBevelInfoBarOp != NULL);
		if (ok) pBevelInfoBarOp->pTool = this;
		
		ERROR3IF(!ok,"Error finding the bevel tool info bar");
	}

	return (ok);
}

/********************************************************************************************

>	void BevelTool::Describe(void *InfoPtr)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		InfoPtr - A pointer to a tool info block. It is passed cast to void* as
				the version of the tool is unknown at this point. Later versions of the
				Tool class may have more items in this block, that this tool will not use
	Outputs:	InfoPtr - The structure pointed to by InfoPtr will have had all the info
				that this version of the Tool knows about
	Purpose:	Allows the tool manager to extract information about the tool

********************************************************************************************/

void BevelTool::Describe(void *InfoPtr)
{
	// Cast structure into the latest one we understand.
	ToolInfo_v1 *Info = (ToolInfo_v1 *) InfoPtr;

	Info->InfoVersion = 1;
	Info->InterfaceVersion = GetToolInterfaceVersion();  // You should always have this line.
		
	// These are all arbitrary at present.
	Info->Version = 1;
	Info->ID      = GetID();
	Info->TextID  = _R(IDS_BEVEL_TOOL);
	Info->BubbleID = _R(IDBBL_BEVEL_TOOLBOX);

	Info->Family  = FamilyName;
	Info->Name    = ToolName;
	Info->Purpose = Purpose;
	Info->Author  = Author;

	Info->InfoBarDialog = _R(IDD_BEVELINFOBAR);
}


UINT32 BevelTool::GetID()
{
	return TOOLID_BEVELTOOL;
}



// The Free Hand Tools EventHandlers

/********************************************************************************************

>	virtual void BevelTool::SelectChange(BOOL isSelected)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/93
	Inputs:		IsSelected - TRUE if the tool is becoming selected, FALSE if it is loosing
				the selection
	Purpose:	Called when the tool is selected or deselected.  Creates and pushes the
				tool's cursor; pops and destroys it.
	Errors:		Sends warning to debugging terminal if creating the cursor fails.

********************************************************************************************/

void BevelTool::SelectChange(BOOL isSelected)
{
	// get the blob manager
	BlobManager* BlobMgr = GetApplication()->GetBlobManager();
	Spread * pSpread = Document::GetSelectedSpread();
//	DocView * pView = DocView::GetCurrent();
	// register my interest in blob rendering
	BlobStyle BevelBlobs;

	if (isSelected)
	{
		// Load the cursors
		m_bDisableBlobRendering = TRUE;
		if (!LoadCursors())
			InformError();

		// register the tool interest
		BevelBlobs.Tiny = TRUE;
		BlobMgr->ToolInterest(BevelBlobs);		

		m_bActive = TRUE;

		// pBevelInfoBarOp->MakeCurrent();

		// update the blob list
		SetupBlobList();

		// create & show the dialog
		pBevelInfoBarOp->Create();
		
		if (pBevelInfoBarOp->pCurrentBar)
		{
			pBevelInfoBarOp->Open();
		}

		if (pBevelInfoBarOp)
		{
			pBevelInfoBarOp->SetToolActiveState(TRUE);
		}

		m_bDisableBlobRendering = FALSE;

		if (BlobMgr && pSpread)
		{
			// render the tool blobs on
			BlobMgr->RenderToolBlobsOn(this, pSpread, NULL);
		}
	}
	else
	{
		// Deselection of the tool
		// we need to do this otherwise the blobs are rendered incorrectly
		// when switching to another tool

		if (BlobMgr && pSpread)
		{
			// SetupBlobList();
			// render the tool blobs off
			BlobMgr->RenderToolBlobsOff(this, pSpread, NULL);
		}

		m_bDisableBlobRendering = TRUE;
		
		RemoveCursors();

		BevelBlobs.Tiny = FALSE;
		BlobMgr->ToolInterest(BevelBlobs);		

		m_bActive = FALSE;
		
		if (pBevelInfoBarOp)
		{
			pBevelInfoBarOp->SetToolActiveState(FALSE);
			pBevelInfoBarOp->Delete();
		}
		
		// ensure any tool object blobs are removed.
		BlobStyle bsRemoves;
		bsRemoves.ToolObject = TRUE;
		BlobMgr->RemoveInterest(bsRemoves);

		m_BevelBlobList.DeleteAll();

		// RecalculateSelectionBoundingBoxes(FALSE);

		m_bDisableBlobRendering = FALSE;
	}
}

/********************************************************************************************

>	void BevelTool::RecalculateSelectionBoundingBoxes(BOOL InvalidateDoc);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/99
	Inputs:		Whether the invalidate the document region
	Returns:	
	Purpose:	Forces the bounding rects of all bevel nodes (with blobs) to
				be recalculated, and if InvalidateDoc is TRUE then it invalidates the
				document's rect
	Errors:		

********************************************************************************************/

void BevelTool::RecalculateSelectionBoundingBoxes(BOOL InvalidateDoc)
{
	Spread * pSpread = Document::GetSelectedSpread();
	
	// can't do anything
	if (!pSpread)
		return;

	Document * pDoc = Document::GetCurrent();

	if (!pDoc)
		return;
	
	DocRect ClipRect;
	Node * pNode = NULL;

	Range Sel(*(GetApplication()->FindSelection()));

	if (Sel.IsEmpty())
		return;

	pNode = Sel.FindFirst(TRUE);

	while (pNode)
	{
		if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded)) &&
			!pNode->IsNodeHidden())
		{
			ClipRect = ClipRect.Union(((NodeRenderableBounded *)pNode)->GetBlobBoundingRect());
		}

		pNode = Sel.FindNext(pNode, TRUE);
	}
	
	if (InvalidateDoc)
	{
		pDoc->ForceRedraw(pSpread, ClipRect, TRUE);
	}
}

/********************************************************************************************

>	BOOL BevelTool::LoadCursors()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/3/99
	Returns:	TRUE if it worked, FALSE if it did not
	Purpose:	Loads all the cursors used by the freehand tool and pushs the normal
				one onto the top of the cursor stack
	Errors:		ERROR1 (_R(IDE_FHAND_BADCURSORS)) if it fails to load the cursors

********************************************************************************************/

BOOL BevelTool::LoadCursors()
{
	// This tool has just been selected, so it is not displaying a cursor
	pActiveCursor = NULL;

	// Try to create all our cursors - The normal cursor
		pNormalCursor = new Cursor(this, _R(IDC_POINTER_BEVEL));
	pMouseOverCursor = new Cursor(this, _R(IDC_ELIPTOOLOVERBLOB));
	pSizeCursor = new Cursor(this, _R(IDC_SIZE_BEVEL));
	
	// did we get all the cursors ok
	if (pNormalCursor==NULL || pMouseOverCursor==NULL || pSizeCursor==NULL)
	{
		// No, at least one of them was NULL, so delete them all
		// Deleting the null cursor will be ok
		delete pNormalCursor;

		// and ensure that the pointers are set to NULL
		pNormalCursor = NULL;

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

>	void BevelTool::RemoveCursors()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/3/99
	Purpose:	Pops the freehand tools cursor off the top of the cursor stack, frees the
				memory used by all the cursors in the freehand tool and sets all the pointers
				to NULL.
	SeeAlso:	BevelTool::LoadCursors

********************************************************************************************/

void BevelTool::RemoveCursors()
{
	// If we ever had a cursor
	if (pActiveCursor!=NULL)
	{
		// pop it off the stack
		CursorStack::GPop(CurrentCursorID);

		// and free up the cursors we allocated
		delete pNormalCursor;

		delete pMouseOverCursor;

		delete pSizeCursor;

		// and set them all to NULL
		pNormalCursor = NULL;
		pActiveCursor = NULL;
		pMouseOverCursor = NULL;
		pSizeCursor = NULL;
		CurrentCursorID = 0;
	}
}

/********************************************************************************************

>	void BevelTool::OnMouseMove(DocCoord Pos, Spread* pSpread, ClickModifiers ClickMods);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/3/99
	Inputs:		ID of the cursor you want to flip to
	Purpose:	Sorts out the movement cursor
	Errors:		can fail if the cursor cannot be created - the cursor code will fail.

********************************************************************************************/
void BevelTool::OnMouseMove(DocCoord Pos, Spread* pSpread, ClickModifiers ClickMods)
{
	// step through all the bevel controller nodes in the selection hit testing
	BOOL bFound = FALSE;

	// run through the blob list testing
	BevelBlob * pBlob = (BevelBlob *)m_BevelBlobList.GetHead();

	while (pBlob && !bFound)
	{
		if (pBlob->HitTest(Pos))
		{
			bFound = TRUE;
			TRACEUSER( "MarkH", _T("Hittesting!\n"));
		}

		pBlob = (BevelBlob *)m_BevelBlobList.GetNext(pBlob);
	}

	if (bFound)
	{
		ChangeCursor(pMouseOverCursor);

		// update the info bar
		DisplayStatusBarHelp(_R(IDS_BEVELDRAGBLOBHELP));

	}
	else
	{
		if (IsPointOverSelectionBlob(Pos, NULL))
		{
			ChangeCursor(pSizeCursor);
			DisplayStatusBarHelp(_R(IDS_BEVELDRAGSTATUSHELP));
		}
		else
		{		
			ChangeCursor(pNormalCursor);
			DisplayStatusBarHelp(_R(IDS_BEVELSTATUSHELP));
		}
	}
}

/********************************************************************************************

>	void BevelTool::DisplayStatusBarHelp(UINT32 StatusID)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	29/9/99
	Inputs:		StatusID = ID of status help string
	Outputs:	-
	Returns:	-
	Purpose:	Displays the given status help string in the status bar
	SeeAlso:	-

********************************************************************************************/

void BevelTool::DisplayStatusBarHelp(UINT32 StatusIDX)
{
	String_256 StatusMsg("");
	StatusMsg.Load(StatusIDX);
	GetApplication()->UpdateStatusBarText(&StatusMsg);
	BevelTool::StatusID = StatusIDX;
}

/********************************************************************************************

>	BOOL BevelTool::GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord DocPos, 
					ClickModifiers ClickMods);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	29/9/99
	Inputs:		StatusID = ID of status help string
	Outputs:	-
	Returns:	-
	Purpose:	Displays the given status help string in the status bar
	SeeAlso:	-

********************************************************************************************/
BOOL BevelTool::GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)
{
	ptext->Load(BevelTool::StatusID);
	return TRUE;
}

/********************************************************************************************

>	void BevelTool::ChangeCursor(Cursor* cursor)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/3/99
	Inputs:		ID of the cursor you want to flip to
	Purpose:	Changes to the specified cursor. Will only change the cursor if it isn't already
				this cursor, so it doesn't flicker.
	Errors:		can fail if the cursor cannot be created - the cursor code will fail.

********************************************************************************************/

void BevelTool::ChangeCursor(Cursor* pCursor)
{
	// only change if this cursor is different from the current cursor
	if ((pCursor!=pActiveCursor) && (pCursor!=NULL))
	{
		// set this cursor as the current cursor and immediately display it
		CursorStack::GSetTop(pCursor, CurrentCursorID);

		// remember this is our current cursor
		pActiveCursor = pCursor;

		DisplayStatusBarHelp(BevelTool::StatusID);
	}
}






/********************************************************************************************

>	void BevelTool::OnClick( DocCoord PointerPos, ClickType Click, 
								ClickModifiers ClickMods, Spread *pSpread )

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/2/99
	Inputs:		PointerPos - The Coords (in spread coords) of the point where the mouse 
				button was clicked
				Click -	Describes the type of click that was detected. 
				ClickMods -	Indicates which buttons caused the click and which modifers were
				pressed at the same time
				pSpread - the spread in which the click happened
	Returns:	TRUE if it handled the Click, FALSE otherwise
	Purpose:	Handle selection messages by the bevelling tool
	SeeAlso:	

********************************************************************************************/

void BevelTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )
{
	ERROR3IF_PF(pSpread==NULL,("pSpread is NULL"));

	if (ClickMods.Menu) return;							// Don't do anything if the user clicked the Menu button

//	NodeCompound * pCompound = NULL;

	DocRect BlobRect;

	switch (Click)
	{
		// if a drag was started, we alter or create a contour.
		case CLICKTYPE_DRAG:
		{	
			// run through my blob list testing the pointer position
			BOOL bDoingDrag = FALSE;
			BevelBlob* pBlob = (BevelBlob *) m_BevelBlobList.GetHead();

			while (pBlob && !bDoingDrag)
			{
				if (pBlob->HitTest(PointerPos))
				{
					bDoingDrag = TRUE;
					pBevelInfoBarOp->BeginDrag(pBlob);
				}

				pBlob = (BevelBlob *)m_BevelBlobList.GetNext(pBlob);
			}

			// if we're not doing a drag yet, then start a drag create.
			if (!bDoingDrag)
			{
				if (m_pDragBevelOp)
				{
					DocRect BlobRect;

					m_pDragBevelOp->DoDrag(pBevelInfoBarOp, PointerPos, 
						IsPointOverSelectionBlob(PointerPos, &BlobRect), &BlobRect);
				}
			}
		}
		break;

		case CLICKTYPE_SINGLE:
		{
			// check for contours existing
			List ContourList;
			BevelTools::BuildListOfSelectedNodes(&ContourList, CC_RUNTIME_CLASS(NodeContourController));

			if (!ContourList.IsEmpty())
			{
				ContourList.DeleteAll();

				DocRect BlobRect;

				// disable the drag
				if (IsPointOverSelectionBlob(PointerPos, &BlobRect))
				{
					DocView * pView = DocView::GetCurrent();

					if (pView)
						pView->EndDrag(NULL);				
					
					InformWarningContourExistsInSelection();			
				}
			}

			// call the base class ....
			
			DragTool::OnClick (PointerPos, Click, ClickMods, pSpread);
		}
		break;
		
		default:
			// call the base class ....
			
			DragTool::OnClick (PointerPos, Click, ClickMods, pSpread);
		break;
	}

}

/********************************************************************************************

>	void BevelTool::InformWarningContourExistsInSelection()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/2000
	Purpose:	Invokes the 'cant bevel contour' dialog

********************************************************************************************/
void BevelTool::InformWarningContourExistsInSelection()
{
	String_256 QueryString(_R(IDS_CANTBEVELCONTOUR));

	Error::SetError(0, QueryString, 0);				
	// The only way of bringing up a box with a string in it
	INT32 DlgResult = InformError(_R(IDS_CANTBEVELCONTOUR),
		_R(IDS_OK), _R(IDS_HELP));
				
	if (DlgResult == 2)
	{
		HelpUserTopic(_R(IDS_HELPPATH_Message__Contour_already_applied));
	}
}

/********************************************************************************************

>	void BevelTool::RenderBlob(BevelBlob * pBlob)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/94
	Inputs:		The blob to render
	Purpose:	Renders a single blob with the current spread

********************************************************************************************/
void BevelTool::RenderBlob(BevelBlob * pBlob)
{
	Spread * pSpread = Document::GetSelectedSpread();

	// get the clipping rect of the blob
	DocRect dr = pBlob->GetRect();

	// run through all the render regions on the stack rendering the blob
	RenderRegion * pRegion = DocView::RenderOnTop(NULL, pSpread, UnclippedEOR);

	while (pRegion)
	{
		pBlob->RenderBlob(pRegion);

		pRegion = DocView::GetNextOnTop(NULL);
	}
}


/********************************************************************************************

>	void BevelTool::RenderToolBlobs(Spread* pSpread, DocRect* pClipRect)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/94
	Inputs:		pSpread - The spread that the blob is to appear on
				pClipRect - Pointer to the rect that contains the blobs
	Purpose:	Renders the Tools Blobs. This will be used to draw paths as they are retro
				fitted live, as the accuracy bar is dragged!

				In case you were wondering, this is how bevel blobs are arranged:

					2	4	3

					6	*	7

					0	5	1

********************************************************************************************/

void BevelTool::RenderToolBlobs(Spread* pSpread, DocRect* pClipRect)
{
	// DoSelectBevelNodesFromChildren();

	if (m_bDisableBlobRendering)
		return;

	// have we zoomed ?
	DocView * pView = DocView::GetSelected(); //GetCurrent();

	if (pView)
	{
		if (m_LastZoom != pView->GetViewScale())
		{
			// ensure we recalculate the bounding rect
			SetupBlobList();
			m_LastZoom = pView->GetViewScale();
		}
	}

	// run through all the render regions and all blobs in the list
	// rendering

	// get the top render region for the blob rendering
	RenderRegion * pRegion = DocView::RenderOnTop(pClipRect, pSpread, UnclippedEOR);

	BevelBlob * pBlob = NULL;

	while (pRegion)
	{
		// go through all blobs in list
		pBlob = (BevelBlob *)m_BevelBlobList.GetHead();

		while (pBlob)
		{
			pBlob->RenderBlob(pRegion);

			pBlob = (BevelBlob *)m_BevelBlobList.GetNext(pBlob);
		}		

		// now do the selection blobs as well
		pRegion->SaveContext();
/*
 *	Karim 26/06/2000
 *	commented out - we're running a trial on some new Contour blobs :)
		pRegion->SetFillColour(COLOUR_RED);
		pRegion->SetLineColour(COLOUR_NONE);

		for (UINT32 i = 0 ; i < 8; i++)
			pRegion->DrawBlob(m_BlobPoints[i], BT_SELECTED);
 *
 */
		// left and right.
		pRegion->DrawBitmapBlob(m_BlobPoints[6], _R(IDBMP_CONTOUR_LEFTRIGHT));
		pRegion->DrawBitmapBlob(m_BlobPoints[7], _R(IDBMP_CONTOUR_LEFTRIGHT));

		// up and down.
		pRegion->DrawBitmapBlob(m_BlobPoints[4], _R(IDBMP_CONTOUR_UPDOWN));
		pRegion->DrawBitmapBlob(m_BlobPoints[5], _R(IDBMP_CONTOUR_UPDOWN));

		// acute accent (bl to tr).
		pRegion->DrawBitmapBlob(m_BlobPoints[0], _R(IDBMP_CONTOUR_ACUTE));
		pRegion->DrawBitmapBlob(m_BlobPoints[3], _R(IDBMP_CONTOUR_ACUTE));

		// grave accent (tl to br).
		pRegion->DrawBitmapBlob(m_BlobPoints[2], _R(IDBMP_CONTOUR_GRAVE));
		pRegion->DrawBitmapBlob(m_BlobPoints[1], _R(IDBMP_CONTOUR_GRAVE));

		pRegion->RestoreContext();		
		
		pRegion = DocView::GetNextOnTop(NULL);
	}
}

/********************************************************************************************

>	BOOL BevelTool::IsPointOverSelectionBlob(DocCoord &Point, DocRect * pRect)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/99
	Inputs:		Point - the point to test
	Purpose:	Returns TRUE if this point is over a selection (i.e. a drag create or
				drag change width blob)

********************************************************************************************/
BOOL BevelTool::IsPointOverSelectionBlob(DocCoord &Point, DocRect * pRect)
{
	// check to see if I'm over a blob
	BlobManager * pBlobMgr = GetApplication()->GetBlobManager();

	INT32 BlobSize = 0;

	// Karim 18/09/2000
	// The bevel blobs are about twice the size of normal blobs,
	// so I'm doubling the value of BlobSize when checking for them.
	if (pBlobMgr)
		BlobSize = pBlobMgr->GetBlobSize();
//		BlobSize = pBlobMgr->GetBlobSize()/2;

	for (INT32 i = 0; i < 8; i++)
	{
		DocRect dr(	m_BlobPoints[i].x - BlobSize,
					m_BlobPoints[i].y - BlobSize,
					m_BlobPoints[i].x + BlobSize,
					m_BlobPoints[i].y + BlobSize);

		if (dr.ContainsCoord(Point))
		{
			if (pRect)
				*pRect = dr;
			return TRUE;
		}
	}

	return FALSE;
}

/********************************************************************************************

>	BOOL BevelTool::SetupBlobList()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/3/99
	Inputs:		
	Purpose:	Sets up the blob list depending on the selection
	Returns:	FALSE for no selection, or no bevel attributes detected

********************************************************************************************/
BOOL BevelTool::SetupBlobList()
{
	// do the selection blobs first
	SetupSelectionBlobs();
	
	// clear our blob list
	m_BevelBlobList.DeleteAll();

	m_BlobListBounds.hi.x = 0;
	m_BlobListBounds.lo.x = 0;
	m_BlobListBounds.hi.y = 0;
	m_BlobListBounds.lo.y = 0;

	// kick the sel range
	// GetApplication()->UpdateSelection();
	
	BOOL bHandled = FALSE;

	BevelBlob * pBlob;

	List BevelNodeList;
	
	// make a list of all bevel ink nodes
	if (!BevelTools::BuildListOfSelectedNodes(&BevelNodeList, CC_RUNTIME_CLASS(NodeBevel)))
	{
		// failed
		return FALSE;
	}

	NodeListItem * pItem = (NodeListItem *)BevelNodeList.GetHead();
	Node * pNode = NULL;

//	BOOL bFound = FALSE;

	while (pItem)
	{
		pNode = pItem->pNode->FindParent();
		
		if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevelController)) 
			&& !pNode->IsNodeHidden() 
			&& !pNode->IsLocked() 
			&& pNode->IsRenderable())
		{
			// ok ! Now, run through our list determining if this light angle
			// matches with any others
			pBlob = (BevelBlob *)m_BevelBlobList.GetHead();

			bHandled = FALSE;

			while (pBlob && !bHandled)
			{
				// try adding the attribute to the list item
				if (pBlob->AddNode((NodeBevelController *)pNode))
				{
					bHandled = TRUE;

					// update the document rect
					if (m_BlobListBounds.Width() == 0 &&
						m_BlobListBounds.Height() == 0)
					{
						m_BlobListBounds = pBlob->GetRect();
					}
					else
					{
						m_BlobListBounds = m_BlobListBounds.Union(pBlob->GetRect());
					}
				}
				
				pBlob = (BevelBlob *)m_BevelBlobList.GetNext(pBlob);
			}

			// if the attribute hasn't been taken by any of the nodes in the list
			// then we need to create a new node
			if (!bHandled)
			{
				// create the bevel blob
				pBlob = new BevelBlob;

				if (!pBlob)
				{
					ERROR3("Cannot create bevel blob");
					BevelNodeList.DeleteAll();
					return FALSE;
				}

				if (!pBlob->AddNode((NodeBevelController *)pNode))
				{
					ERROR3("Cannot add attribute to bevel blob");
					BevelNodeList.DeleteAll();
					return FALSE;
				}

				// insert it into the list
				m_BevelBlobList.AddTail(pBlob);

				// update the document rect
				if (m_BlobListBounds.Width() == 0 &&
					m_BlobListBounds.Height() == 0)
				{
					m_BlobListBounds = pBlob->GetRect();
				}
				else
				{
					m_BlobListBounds = m_BlobListBounds.Union(pBlob->GetRect());
				}
			}
		}

		pItem = (NodeListItem *)BevelNodeList.GetNext(pItem);		
	}

	BevelNodeList.DeleteAll();

	return TRUE;
}

/********************************************************************************************

>	void BevelTool::SetupSelectionBlobs();

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/99
	Inputs:		
	Purpose:	Sets up the bevel tool blobs for the selection.

				Karim 28/07/2000
				Bevel tool blobs are laid out like so relative to the selection:

						2	4	3

						6	*	7

						0	5	1

				with corner blobs positioned 1 pixel out from the bounds rect
				and middle blobs positioned 2 pixels out from the bounds rect.

********************************************************************************************/
void BevelTool::SetupSelectionBlobs()
{
	// reset the blob positions.
	for (INT32 i = 0 ; i < 8; i++)
	{
		m_BlobPoints[i].x = 0;
		m_BlobPoints[i].y = 0;
	}

	// give up if we there is no selection or we can't get the info we need.
	if (GetApplication()->FindSelection() == NULL ||
		GetApplication()->FindSelection()->IsEmpty())
		return;

	BlobManager * pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr == NULL)
		return;

	// figure out the blob offsets, in millipoints.
	INT32 BlobGap = 0;
	DocView* pDocView = DocView::GetSelected();
	if (pDocView != NULL)
		BlobGap = pDocView->GetScaledPixelWidth().MakeLong();

	// ok, get the bounds of the selection, including bevels.
	SelRange Sel(*(GetApplication()->FindSelection()));
	DocRect drBounds;
	Node* pN = Sel.FindFirst();
	while (pN != NULL)
	{
		if (pN->IsBounded())
		{
			Node* pParent = pN->FindParent();
			while (pParent != NULL && pParent->IS_KIND_OF(NodeBevelController))
			{
				pN = pParent;
				pParent = pN->FindParent();
			}

			drBounds = drBounds.Union( ((NodeRenderableBounded*)pN)->GetBoundingRect() );
		}
		pN = Sel.FindNext(pN);
	}

	// lets set up those blobs!
	DocCoord dc;
	INT32 BlobSize = pBlobMgr->GetBlobSize();
	drBounds.Inflate(BlobGap + BlobSize);

	dc.x = drBounds.lo.x;
	dc.y = drBounds.lo.y;
	m_BlobPoints[0] = dc;

	dc.x = drBounds.hi.x;
	dc.y = drBounds.lo.y;
	m_BlobPoints[1] = dc;

	dc.x = drBounds.lo.x;
	dc.y = drBounds.hi.y;
	m_BlobPoints[2] = dc;

	dc.x = drBounds.hi.x;
	dc.y = drBounds.hi.y;
	m_BlobPoints[3] = dc;

	dc.x = (drBounds.lo.x + drBounds.hi.x) / 2;
	dc.y = drBounds.hi.y + BlobGap;
	m_BlobPoints[4] = dc;

	dc.x = (drBounds.lo.x + drBounds.hi.x) / 2;
	dc.y = drBounds.lo.y - BlobGap;
	m_BlobPoints[5] = dc;

	dc.x = drBounds.lo.x - BlobGap;
	dc.y = (drBounds.lo.y + drBounds.hi.y) / 2;
	m_BlobPoints[6] = dc;

	dc.x = drBounds.hi.x + BlobGap;
	dc.y = (drBounds.lo.y + drBounds.hi.y) / 2;
	m_BlobPoints[7] = dc;
}

/********************************************************************************************

>	void BevelTool::DoSelectBevelNodesFromChildren()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/99
	Inputs:		
	Purpose:	Selects all bevel controller nodes from the child nodes in the selection

********************************************************************************************/
void BevelTool::DoSelectBevelNodesFromChildren()
{
	// run through the selection selecting all nodes' parents which
	// are bevel controller nodes
	// do we have a bevel controller node in the selection ?
	// Obtain the current selections 
	Range Sel(*(GetApplication()->FindSelection()));
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);
	

	// flag to say whether we've changed the selection or not
	BOOL bSelChanged = FALSE;

	if (Sel.IsEmpty())
		return ;

	// search through until we find a path node
	Node *pNode = Sel.FindFirst(TRUE);

	if (!pNode)
	{
		pNode = Sel.FindFirst(FALSE);
	}

	if (!pNode)
		return ;

	Node * pOldNode = NULL;

//	BOOL bOK = FALSE;

//	NodeBevelController * pBevControl = NULL;
	Node * pParent;

	// this is the list of nodes to select
	List SelList;
	NodeListItem * pListItem = NULL;
	
	// run through all the nodes in the list finding out if any children of
	// bevel controller nodes are selected when the controller node itself isn't
	while (pNode)
	{
		pParent = pNode->FindParent();

		if (pParent)
		{
			if (pParent->IsKindOf(CC_RUNTIME_CLASS(NodeBevelController)))
			{
				// ensure the parent is selected
				if (!pParent->IsSelected())
				{
					pListItem = new NodeListItem;

					if (!pListItem)
					{
						ERROR3("Can't create list item");
						return;
					}

					pListItem->pNode = pParent;
					SelList.AddTail(pListItem);
					bSelChanged = TRUE;
				}
			}
		}

		pOldNode = pNode;
		pNode = Sel.FindNext(pNode, TRUE);

		if (!pNode)
		{
			pNode = Sel.FindNext(pOldNode, FALSE);
		}
	}

	BlobManager* BlobMgr = GetApplication()->GetBlobManager();

	Spread * pSpread = Document::GetSelectedSpread();

	DocRect dr;

	// update the selection in the app
	if (bSelChanged && BlobMgr && pSpread)
	{
		// first, render the blobs off
		dr = Sel.GetBoundingRect();
		BlobMgr->RenderOff(&dr, pSpread);

		// run through the list selecting all nodes
		pListItem = (NodeListItem *)SelList.GetHead();

		while (pListItem)
		{
			if (pListItem->pNode)
			{
				pListItem->pNode->SetSelected(TRUE);
			}

			pListItem = (NodeListItem *)SelList.GetNext(pListItem);
		}
		
		// update the selection
		GetApplication()->UpdateSelection();

		// update the blob list
		SetupBlobList();

		// render the blobs back on, bearing in mind the selection has changed
		Range NewSel(*(GetApplication()->FindSelection()));
		RangeControl rg = Sel.GetRangeControlFlags();
		rg.PromoteToParent = TRUE;
		Sel.Range::SetRangeControl(rg);
	
		dr = NewSel.GetBoundingRect();
		BlobMgr->RenderOn(&dr, pSpread);
	}

	SelList.DeleteAll();
}

/********************************************************************************************

>	void BevelTool::InvalidateToolBlobs()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/99
	Inputs:		
	Purpose:	Unrenders all blobs, sets up the blob list again and re-renders

********************************************************************************************/
void BevelTool::InvalidateToolBlobs()
{
	// render the blobs off first

	// get the blob manager
	BlobManager* BlobMgr = GetApplication()->GetBlobManager();
	Spread * pSpread = Document::GetSelectedSpread();

	if (!BlobMgr || !pSpread)
		return;

	// first, render the blobs off
	BlobMgr->RenderToolBlobsOff(this, pSpread, NULL);

	SetupBlobList();

	// render the blobs back on
	BlobMgr->RenderToolBlobsOn(this, pSpread, NULL);

	// force a redraw of the area
	/*
	Document * pDoc = Document::GetCurrent();
	DocView * pView = DocView::GetCurrent();

	if (pDoc && pView)
	{
		pDoc->ForceRedraw(pSpread, m_BlobListBounds);
	}
	*/
}

/********************************************************************************************

>	void BevelTool::ChangeBlobAngles(INT32 Angle)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/99
	Inputs:		The new angle
	Purpose:	Removes all blobs' angles and re-renders them with the new angle

********************************************************************************************/
void BevelTool::ChangeBlobAngles(INT32 Angle)
{
	if (m_BevelBlobList.IsEmpty())
		return;

	// get the blob manager
	BlobManager* BlobMgr = GetApplication()->GetBlobManager();
	Spread * pSpread = Document::GetSelectedSpread();

	if (!BlobMgr || !pSpread)
		return;

	// first, render the blobs off
	// BlobMgr->RenderToolBlobsOff(this, pSpread, &m_BlobListBounds);

	BevelBlob * pBlob = (BevelBlob *)m_BevelBlobList.GetHead();

	Document * pDoc = Document::GetCurrent();

	// set all the blob angles to the new value
	while (pBlob)
	{
		pBlob->SetAngle(Angle);

		pBlob->RenderNodes();

		if (pDoc)
			pDoc->ForceRedraw(pSpread, m_BlobListBounds);		
		
		pBlob = (BevelBlob *)m_BevelBlobList.GetNext(pBlob);
	}

	GetApplication()->ServiceRendering();

	// render the blobs back on
	// BlobMgr->RenderToolBlobsOn(this, pSpread, &m_BlobListBounds);
}

/********************************************************************************************

>	void BevelTool::ChangeBlobTilts(INT32 Tilt)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/2/2000
	Inputs:		The new angle
	Purpose:	Removes all blobs' angles and re-renders them with the new tilt

********************************************************************************************/
void BevelTool::ChangeBlobTilts(INT32 Tilt)
{
	if (m_BevelBlobList.IsEmpty())
		return;

	// get the blob manager
	BlobManager* BlobMgr = GetApplication()->GetBlobManager();
	Spread * pSpread = Document::GetSelectedSpread();

	if (!BlobMgr || !pSpread)
		return;

	// first, render the blobs off
	// BlobMgr->RenderToolBlobsOff(this, pSpread, &m_BlobListBounds);

	BevelBlob * pBlob = (BevelBlob *)m_BevelBlobList.GetHead();

	Document * pDoc = Document::GetCurrent();

	// set all the blob angles to the new value
	while (pBlob)
	{
		pBlob->SetTilt(Tilt);

		pBlob->RenderNodes();

		if (pDoc)
			pDoc->ForceRedraw(pSpread, m_BlobListBounds);		
		
		pBlob = (BevelBlob *)m_BevelBlobList.GetNext(pBlob);
	}

	GetApplication()->ServiceRendering();

	// render the blobs back on
	// BlobMgr->RenderToolBlobsOn(this, pSpread, &m_BlobListBounds);
}


/********************************************************************************************

>	void BevelTool::ChangeContrastAndReRender(INT32 NewContrast);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/99
	Inputs:		The new angle
	Purpose:	Removes all blobs' angles and re-renders them with the new angle

********************************************************************************************/
void BevelTool::ChangeContrastAndReRender(INT32 NewContrast)
{
	if (m_BevelBlobList.IsEmpty())
		return;

	// get the blob manager
	BlobManager* BlobMgr = GetApplication()->GetBlobManager();
	Spread * pSpread = Document::GetSelectedSpread();

	if (!BlobMgr || !pSpread)
		return;

	BevelBlob * pBlob = (BevelBlob *)m_BevelBlobList.GetHead();

	Document * pDoc = Document::GetCurrent();

	// set all the blob angles to the new value
	while (pBlob)
	{
		// change all the contrast settings for the nodes
		pBlob->ChangeContrast(NewContrast);

		if (pDoc)
		{
			pDoc->ForceRedraw(pSpread, pBlob->GetRect());
		}
		
		pBlob = (BevelBlob *)m_BevelBlobList.GetNext(pBlob);
	}

	GetApplication()->ServiceRendering();
}


/********************************************************************************************

>	BOOL BevelTool::AreManyBevelTypes()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/99
	Returns:	TRUE if there are many different types of bevel in the selection,
				FALSE if there is only one
	Purpose:	Queries the blob list to see if there are many different types of bevel
	SeeAlso:	

********************************************************************************************/

BOOL BevelTool::AreManyBevelTypes()
{
	if (m_BevelBlobList.IsEmpty())
		return FALSE;

	// run through the blob list, finding out if the selection contains bevelled
	// objects with diferent types
	BevelBlob * pItem = (BevelBlob *)m_BevelBlobList.GetHead();

	INT32 Type = -1;

	while (pItem)
	{
		NodeListItem * pNodeItem = (NodeListItem *)pItem->GetList()->GetHead();

		while (pNodeItem)
		{
			if (Type < 0)
			{
				Type = ((NodeBevelController *)pNodeItem->pNode)->m_BevelType;
			}
			else
			{
				if (Type != ((NodeBevelController *)pNodeItem->pNode)->m_BevelType)
				{
					return TRUE;
				}
			}

			pNodeItem = (NodeListItem *)pItem->GetList()->GetNext(pNodeItem);
		}

		pItem = (BevelBlob *)m_BevelBlobList.GetNext(pItem);
	}

	// test for a bevelled node & a non-bevelled node being selected
	Range Rng(*(GetApplication()->FindSelection()));
	RangeControl rg = Rng.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Rng.SetRangeControl(rg);

	List BevelList;
	
	Node * pNode = Rng.FindFirst(FALSE);

	// scan the selection range trying to find any nodes which aren't bevelled	
	while (pNode)
	{
		BevelList.DeleteAll();
		
		BevelTools::GetAllNodesUnderNode(pNode, &BevelList, CC_RUNTIME_CLASS(NodeBevel));
		
		if (BevelList.IsEmpty())
		{
			BevelList.DeleteAll();
			return TRUE;
		}
		
		pNode = Rng.FindNext(pNode, FALSE);
	}

	BevelList.DeleteAll();
	return FALSE;
}

/********************************************************************************************

>	BOOL BevelTool::AreManyBevelDirections()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/99
	Returns:	TRUE if there are many different directions of bevel in the selection,
				FALSE if there is only one
	Purpose:	Queries the blob list to see if there are many different directions of bevel
	SeeAlso:	

********************************************************************************************/

BOOL BevelTool::AreManyBevelDirections()
{
	if (m_BevelBlobList.IsEmpty())
		return FALSE;

	BevelBlob * pItem = (BevelBlob *)m_BevelBlobList.GetHead();

	BOOL bOuter = FALSE;
	BOOL bFirst = TRUE;

	while (pItem)
	{
		NodeListItem * pNodeItem = (NodeListItem *)pItem->GetList()->GetHead();

		while (pNodeItem)
		{
			if (bFirst)
			{
				bOuter = ((NodeBevelController *)pNodeItem->pNode)->m_bOuter;
				bFirst = FALSE;
			}
			else
			{
				if ((bOuter && !((NodeBevelController *)pNodeItem->pNode)->m_bOuter) ||
					(!bOuter && ((NodeBevelController *)pNodeItem->pNode)->m_bOuter))
				{
					return TRUE;
				}
			}

			pNodeItem = (NodeListItem *)pItem->GetList()->GetNext(pNodeItem);
		}

		pItem = (BevelBlob *)m_BevelBlobList.GetNext(pItem);
	}

	return FALSE;
}

/********************************************************************************************

>	BOOL BevelTool::AreManyBevelIndents()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/99
	Returns:	TRUE if there are many different directions of bevel in the selection,
				FALSE if there is only one
	Purpose:	Queries the blob list to see if there are many different directions of bevel
	SeeAlso:	

********************************************************************************************/

BOOL BevelTool::AreManyBevelIndents()
{
	if (m_BevelBlobList.IsEmpty())
		return FALSE;

	BevelBlob * pItem = (BevelBlob *)m_BevelBlobList.GetHead();

	MILLIPOINT Indent = 0;
	BOOL bFirst = TRUE;
	
	while (pItem)
	{
		NodeListItem * pNodeItem = (NodeListItem *)pItem->GetList()->GetHead();

		while (pNodeItem)
		{
			if (bFirst)
			{
				Indent = ((NodeBevelController *)pNodeItem->pNode)->m_Indent;
				bFirst = FALSE;
			}
			else
			{
				if (Indent != ((NodeBevelController *)pNodeItem->pNode)->m_Indent)
				{
					return TRUE;
				}
			}

			pNodeItem = (NodeListItem *)pItem->GetList()->GetNext(pNodeItem);
		}

		pItem = (BevelBlob *)m_BevelBlobList.GetNext(pItem);
	}

	return FALSE;
}


/********************************************************************************************

>	BOOL BevelTool::AreManyBevelContrasts()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/99
	Returns:	TRUE if there are many different c of bevel in the selection,
				FALSE if there is only one
	Purpose:	Queries the blob list to see if there are many different contrasts of bevel
	SeeAlso:	

********************************************************************************************/

BOOL BevelTool::AreManyBevelContrasts()
{
	if (m_BevelBlobList.IsEmpty())
		return FALSE;

	BevelBlob * pItem = (BevelBlob *)m_BevelBlobList.GetHead();

	INT32 Contrast = 0;
	BOOL bFirst = TRUE;
	
	while (pItem)
	{
		NodeListItem * pNodeItem = (NodeListItem *)pItem->GetList()->GetHead();

		while (pNodeItem)
		{
			if (bFirst)
			{
				Contrast = ((NodeBevelController *)pNodeItem->pNode)->m_Contrast;
				bFirst = FALSE;
			}
			else
			{
				if (Contrast != ((NodeBevelController *)pNodeItem->pNode)->m_Contrast)
				{
					return TRUE;
				}
			}

			pNodeItem = (NodeListItem *)pItem->GetList()->GetNext(pNodeItem);
		}

		pItem = (BevelBlob *)m_BevelBlobList.GetNext(pItem);
	}

	return FALSE;
}

/********************************************************************************************

>	BOOL BevelTool::AreManyBevelLightAngles()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/99
	Returns:	TRUE if there are many different c of bevel in the selection,
				FALSE if there is only one
	Purpose:	Queries the blob list to see if there are many different contrasts of bevel
	SeeAlso:	

********************************************************************************************/

BOOL BevelTool::AreManyBevelLightAngles()
{
	if (m_BevelBlobList.IsEmpty())
		return FALSE;

	BevelBlob * pItem = (BevelBlob *)m_BevelBlobList.GetHead();

	double LightAngle = 0;
	BOOL bFirst = TRUE;
	
	while (pItem)
	{
		NodeListItem * pNodeItem = (NodeListItem *)pItem->GetList()->GetHead();

		while (pNodeItem)
		{
			if (bFirst)
			{
				LightAngle = ((NodeBevelController *)pNodeItem->pNode)->m_LightAngle;
				bFirst = FALSE;
			}
			else
			{
				if (LightAngle != ((NodeBevelController *)pNodeItem->pNode)->m_LightAngle)
				{
					return TRUE;
				}
			}

			pNodeItem = (NodeListItem *)pItem->GetList()->GetNext(pNodeItem);
		}

		pItem = (BevelBlob *)m_BevelBlobList.GetNext(pItem);
	}

	return FALSE;
}

/********************************************************************************************

>	BOOL BevelTool::AreManyBevelLightTilts()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/99
	Returns:	TRUE if there are many different c of bevel in the selection,
				FALSE if there is only one
	Purpose:	Queries the blob list to see if there are many different contrasts of bevel
	SeeAlso:	

********************************************************************************************/

BOOL BevelTool::AreManyBevelLightTilts()
{
	if (m_BevelBlobList.IsEmpty())
		return FALSE;

	BevelBlob * pItem = (BevelBlob *)m_BevelBlobList.GetHead();

	double LightTilt = 0;
	BOOL bFirst = TRUE;
	
	while (pItem)
	{
		NodeListItem * pNodeItem = (NodeListItem *)pItem->GetList()->GetHead();

		while (pNodeItem)
		{
			if (bFirst)
			{
				LightTilt = ((NodeBevelController *)pNodeItem->pNode)->m_Tilt;
				bFirst = FALSE;
			}
			else
			{
				if (LightTilt != ((NodeBevelController *)pNodeItem->pNode)->m_Tilt)
				{
					return TRUE;
				}
			}

			pNodeItem = (NodeListItem *)pItem->GetList()->GetNext(pNodeItem);
		}

		pItem = (BevelBlob *)m_BevelBlobList.GetNext(pItem);
	}

	return FALSE;
}

/********************************************************************************************

>	BOOL BevelTool::DoDrag()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/99
	Inputs:		
	Purpose:	Starts a drag operation

********************************************************************************************/
/*
BOOL BevelTool::DoDrag()
{
	StartDrag(DRAGTYPE_AUTOSCROLL);

	return TRUE;
}
*/

/********************************************************************************************

>	static INT32 BevelTool::CalculateBevelWidth(DocRect &br, DocCoord &PointerPos)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/9/99
	Inputs:		-
	Outputs:	-
	Returns:	The width of the bevel
	Purpose:	Calculates the bevel width for a given node given a pointer position
	SeeAlso:	-

********************************************************************************************/

INT32 BevelTool::CalculateBevelWidth(DocRect &br, DocCoord &PointerPos)
{
	DocCoord Centre = br.Centre();

	INT32 Width = 0;

	INT32 OffsetX = 0;
	INT32 OffsetY = 0;

	double dOffsetX = 0;
	double dOffsetY = 0;

	// try to work out the width of the contour
	// first, are we dragging inside or outside the bounding rect ?
	if (!br.ContainsCoord(PointerPos))
	{
		// outside the bounding rect
		if (PointerPos.x < br.lo.x && PointerPos.y >= br.lo.y &&
			PointerPos.y <= br.hi.y)
		{
			Width = br.lo.x - PointerPos.x;
		}
		else if (PointerPos.x > br.hi.x && PointerPos.y >= br.lo.y &&
			PointerPos.y <= br.hi.y)
		{
			Width = PointerPos.x - br.hi.x;
		}
		else if (PointerPos.x >= br.lo.x && PointerPos.x <= br.hi.x &&
			PointerPos.y < br.lo.y)
		{
			Width = br.lo.y - PointerPos.y;
		}
		else if (PointerPos.x >= br.lo.x && PointerPos.x <= br.hi.x &&
			PointerPos.y > br.hi.y)
		{
			Width = PointerPos.y - br.hi.y;
		}
		else if (PointerPos.x < br.lo.x && PointerPos.y < br.lo.y)
		{
			OffsetX = br.lo.x - PointerPos.x;
			OffsetY = br.lo.y - PointerPos.y;
			
			if (OffsetX > OffsetY)
			{
				Width = OffsetX;
			}
			else
			{
				Width = OffsetY;
			}
		}
		else if (PointerPos.x > br.hi.x && PointerPos.y < br.lo.y)
		{
			OffsetX = PointerPos.x - br.hi.x ;
			OffsetY = br.lo.y - PointerPos.y;
			
			if (OffsetX > OffsetY)
			{
				Width = OffsetX;
			}
			else
			{
				Width = OffsetY;
			}
		}
		else if (PointerPos.x > br.hi.x && PointerPos.y > br.hi.y)
		{
			OffsetX = PointerPos.x - br.hi.x ;
			OffsetY = PointerPos.y - br.hi.y;
			
			if (OffsetX > OffsetY)
			{
				Width = OffsetX;
			}
			else
			{
				Width = OffsetY;
			}
		}
		else if (PointerPos.x < br.lo.x && PointerPos.y > br.hi.y)
		{
			OffsetX = br.lo.x - PointerPos.x ;
			OffsetY = PointerPos.y - br.hi.y;
			
			if (OffsetX > OffsetY)
			{
				Width = OffsetX;
			}
			else
			{
				Width = OffsetY;
			}
		}
		
		Width = -Width;
	}
	else
	{
		// inside the bounding rect then

		// work out which quadrant to access
		dOffsetX = ((double)(PointerPos.x - Centre.x));
		dOffsetY = ((double)(PointerPos.y - Centre.y));

		INT32 RWidth = br.Width()/2;
		INT32 RHeight = br.Height()/2;

		if (RWidth > RHeight)
		{
			INT32 DistX = RWidth - RHeight;

			if (PointerPos.x < Centre.x + DistX &&
				PointerPos.x > Centre.x - DistX)
			{
				if (dOffsetY > 0)
				{
					Width = br.hi.y - PointerPos.y;
				}
				else
				{
					Width = PointerPos.y - br.lo.y;
				}
			}
			else if (PointerPos.x > Centre.x + DistX)
			{
				dOffsetX -= (double)DistX;

				if (dOffsetY > 0)
				{
					if (dOffsetX > dOffsetY)
					{
						Width = br.hi.x - PointerPos.x;
					}
					else
					{
						Width = br.hi.y - PointerPos.y;
					}
				}
				else
				{
					if (dOffsetX > -dOffsetY)
					{
						Width = br.hi.x - PointerPos.x;
					}
					else
					{
						Width = PointerPos.y - br.lo.y;
					}
				}
			}
			else
			{
				dOffsetX += (double)DistX;
				dOffsetX = -dOffsetX;

				if (dOffsetY > 0)
				{
					if (dOffsetX > dOffsetY)
					{
						Width = PointerPos.x - br.lo.x;
					}
					else
					{
						Width = br.hi.y - PointerPos.y;
					}
				}
				else
				{
					if (dOffsetX > -dOffsetY)
					{
						Width = PointerPos.x - br.lo.x;
					}
					else
					{
						Width = PointerPos.y - br.lo.y;
					}
				}
			}
		}
		else
		{
			INT32 DistY = RHeight - RWidth;

			if (PointerPos.y < Centre.y + DistY &&
				PointerPos.y > Centre.y - DistY)
			{
				if (dOffsetX > 0)
				{
					Width = br.hi.x - PointerPos.x;
				}
				else
				{
					Width = PointerPos.x - br.lo.x;
				}
			}
			else if (PointerPos.y > Centre.y + DistY)
			{
				dOffsetY -= (double)DistY;

				if (dOffsetX > 0)
				{
					if (dOffsetY > dOffsetX)
					{
						Width = br.hi.y - PointerPos.y;
					}
					else
					{
						Width = br.hi.x - PointerPos.x;
					}
				}
				else
				{
					if (dOffsetY > -dOffsetX)
					{
						Width = br.hi.y - PointerPos.y;
					}
					else
					{
						Width = PointerPos.x - br.lo.x;
					}
				}
			}
			else
			{
				dOffsetY += (double)DistY;
				dOffsetY = -dOffsetY;

				if (dOffsetX > 0)
				{
					if (dOffsetY > dOffsetX)
					{
						Width = PointerPos.y - br.lo.y;
					}
					else
					{
						Width = br.hi.x - PointerPos.x;
					}
				}
				else
				{
					if (dOffsetY > -dOffsetX)
					{
						Width = PointerPos.y - br.lo.y;
					}
					else
					{
						Width = PointerPos.x - br.lo.x;
					}
				}
			}
		}
	}
	
	return Width;
}

/********************************************************************************************

>	void BevelTool::DragPointerMove(DocCoord PointerPos, ClickModifiers ClickMods,
		Spread * pSpread, BOOL bSolidDrag)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/99
	Inputs:		
	Purpose:	Handles the pointer moving inside of a drag

********************************************************************************************/
/*
void BevelTool::DragPointerMove(DocCoord PointerPos, ClickModifiers ClickMods,
		Spread * pSpread, BOOL bSolidDrag)
{
}
*/

/********************************************************************************************

>	void BevelTool::DragFinished(DocCoord PointerPos, ClickModifers ClickMods,
		Spread * pSpread, BOOL Success, BOOL bSolidDrag);


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/99
	Inputs:		
	Purpose:	Ends the drag

********************************************************************************************/
/*
void BevelTool::DragFinished(DocCoord PointerPos, ClickModifers ClickMods,
		Spread * pSpread, BOOL Success, BOOL bSolidDrag)
{
}
*/









/*
IMPLEMENT_SIMPLE_MODULE( BevelModule, MODULEID_BEVEL, BevelTool, 
							"Bevel Tool", "To bevel things", "DavidM" );
*/

//////////////////////////////////////////////////////////////////////////////////////////////
// Bevel blob implementation
// DMc 10/3/99

/********************************************************************************************

>	BevelBlob::BevelBlob()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/99
	Inputs:		
	Purpose:	Bevel blob rendering class constructor

********************************************************************************************/
BevelBlob::BevelBlob()
{
	m_bDragging = FALSE;
	m_BlobAngle = 0;
	m_BevelNodeList.DeleteAll();
}

/********************************************************************************************

>	BevelBlob::~BevelBlob()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/99
	Inputs:		
	Purpose:	Bevel blob destructor

********************************************************************************************/
BevelBlob::~BevelBlob()
{
	m_bDragging = FALSE;
	m_BlobAngle = 0;
	m_BevelNodeList.DeleteAll();
}

/********************************************************************************************

>	BOOL BevelBlob::AddNode(NodeBevelController * pAttr)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/99
	Inputs:		The bevel controller node to add to my list
	Purpose:	Adds the bevel controller node to my list
	Returns:	TRUE for node added to the list, FALSE if it wasn't (e.g. if the
				node added wasn't the same as the other attributes in my list

********************************************************************************************/
BOOL BevelBlob::AddNode(NodeBevelController * pNode)
{
	// continuity checks
	if (!pNode)
		return FALSE;

	if (!pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevelController)))
		return FALSE;
	
	NodeListItem * pListItem = NULL;
//	Node * pParent = NULL;

	AttrBevelLightAngle * pAttr = NULL;

	pNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBevelLightAngle), 
		(NodeAttribute **)(&pAttr));

	INT32 LightAngle = 0;

	if (pAttr)
	{
		LightAngle = (INT32)pAttr->Value.m_LightAngle;
	}
	else
	{
		LightAngle = (INT32)((NodeBevelController *)pNode)->m_LightAngle;
	}
	
	// is our list empty ?
	if (m_BevelNodeList.IsEmpty())
	{
		// set the bounding rect for this blob
		// get the bounding rect
		m_BevelNodeListRect = pNode->GetBlobBoundingRect();

		// then add the attribute to our list
		pListItem = new NodeListItem;
		if (!pListItem)
		{
			ERROR3("Can't create list item");
			return FALSE;
		}

		pListItem->pNode = pNode;

		// set up our angle
		m_BlobAngle = LightAngle;

		m_BevelNodeList.AddTail(pListItem);

		return TRUE;
	}

	// check the angle against mine
	if (LightAngle != m_BlobAngle)
	{
		// we're not dealing with it
		return FALSE;
	}
	
	// then add the attribute to our list
	pListItem = new NodeListItem;
	if (!pListItem)
	{
		ERROR3("Can't create list item");
		return FALSE;
	}

	pListItem->pNode = pNode;

	m_BevelNodeListRect = m_BevelNodeListRect.Union(pNode->GetBlobBoundingRect());

	m_BevelNodeList.AddTail(pListItem);

	return TRUE;
}

/********************************************************************************************

>	void BevelBlob::RenderBlob(RenderRegion * pRegion)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/99
	Inputs:		
	Purpose:	Renders the blobs

********************************************************************************************/
void BevelBlob::RenderBlob(RenderRegion * pRegion)
{
	// this is to prevent the clicking from accidentally rendering the blobs
	// twice
	pRegion->SaveContext();

	// calculate where to put the other blob
	pRegion->SetLineColour(COLOUR_NONE);
	pRegion->SetFillColour(COLOUR_BLACK);

	DocRect dr = m_BevelNodeListRect;

	DocCoord blob1((dr.hi.x + dr.lo.x) / 2, (dr.hi.y + dr.lo.y) / 2);
	DocCoord blob2;

//	double dWidth = dr.Width() / 2;
//	double dHeight = dr.Height() / 2;

	double angle = m_BlobAngle;
	TRACEUSER( "DavidM", _T("Blob angle %d\n"), m_BlobAngle);

	angle = (angle * 3.142) / 180.0;

	// make line 40 pixels INT32
	DocView * pView = DocView::GetSelected();

	if (!pView)
	{
		pRegion->RestoreContext();
		return;
	}

	double len = pView->GetScaledPixelWidth().MakeDouble() * 40.0;

	double dx = len * cos(angle);
	double dy = len * sin(angle);
	
	blob2.x = blob1.x - ((INT32)dx);
	blob2.y = blob1.y - ((INT32)dy);
	
	// render a blob in the centre
	// pRegion->DrawBlob(blob1, BT_SELECTED);

	m_BlobCentre = blob1;

	// create the arrow
	pRegion->SetLineColour(COLOUR_BLACK);

	pRegion->SetLineWidth((INT32)((pRegion->GetScaledPixelWidth()) * 2));
	
	// turn the capping to butt
	StartCapAttribute SCA;
	SCA.StartCap = LineCapButt;
	pRegion->SetStartCap(&SCA, FALSE);

	ArrowRec mArrow;
	mArrow.CreateStockArrow(SA_STRAIGHTARROW);
	mArrow.SetArrowSize(mArrow.GetArrowWidth() * 2);

	// draw the arrow head
	DocCoord ArrowBottom;

	pRegion->DrawLine(blob1, blob2);

	// draw the arrow head
	if (!pRegion->DrawArrowHead(mArrow, blob2, blob1))
		ERROR3("Cannot draw arrow");

	// draw the blob at the tip of the arrow head

	DocCoord blob3;


	len += (pView->GetScaledPixelWidth().MakeDouble()) * 15.0;
	
	dx = len * cos(angle);
	dy = len * sin(angle);
	
	blob3.x = blob1.x - ((INT32)dx);
	blob3.y = blob1.y - ((INT32)dy);

	pRegion->SetFillColour(COLOUR_RED);
	pRegion->SetLineColour(COLOUR_NONE);
	pRegion->DrawBlob(blob3, BT_SELECTED);	

	// get the blob's bounding rect and put it into our member variable
	if (GetApplication()->GetBlobManager())
	{
		GetApplication()->GetBlobManager()->GetBlobRect(blob3, &m_BlobDocRect);
	}

	pRegion->RestoreContext();
}

/********************************************************************************************

>	BOOL BevelBlob::IsBlobValid()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Checks all attributes to see if I'm still valid
	SeeAlso:	BevelTool::Init

********************************************************************************************/
BOOL BevelBlob::IsBlobValid()
{
	NodeListItem * pItem = (NodeListItem *)m_BevelNodeList.GetHead();

	if (pItem)
	{
		if (pItem->pNode)
		{
			if (pItem->pNode->IsNodeHidden() ||
				pItem->pNode->IsLocked() ||
				!pItem->pNode->IsRenderable())
			{
				return FALSE;
			}
		}

		pItem = (NodeListItem *)m_BevelNodeList.GetNext(pItem);
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL BevelBlob::HitTest(DocCoord dc)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		The doccoord to test (usually the pointer position)
	Returns:	TRUE for click on blob, FALSE otherwise
	Purpose:	Checks if the doccoord passed in was inside my blob
	
********************************************************************************************/
BOOL BevelBlob::HitTest(const DocCoord &dc)
{
	return m_BlobDocRect.ContainsCoord(dc);
}

/********************************************************************************************

>	void BevelBlob::CalculateAngle(const DocCoord &dc, BOOL bConstrain)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		The doc-coord to recalculate the angle
				bConstrain - whether to constrain the angle to levels of 45 degrees
	Returns:	=
	Purpose:	Calculates my angle so that the blob's direction arrow will be pointing
				at the given point
	
********************************************************************************************/
void BevelBlob::CalculateAngle(const DocCoord &dc, BOOL bConstrain)
{
	// calculate the vector
	double dx = dc.x - m_BlobCentre.x;
	double dy = dc.y - m_BlobCentre.y;

	// normalise
	double len = sqrt(dx*dx + dy*dy);

	// check for valid length
	if (len <= 0)
		return;

	dx /= len;
	dy /= len;

	// calculate the angle given the cosine
	double angle = acos(dx);

	angle *= ((double)180) / (3.142);

	// alter the range depending on y
	angle = 180.0 - angle;

	if (dy > 0)
	{
		angle = 360.0 - angle;
	}

	// constrain if necessary
	if (bConstrain)
	{
		// Now get the constrain angle from the options
		// Doubles actually need document based units to work.
		double ConstrainAngle = 0.0; 		// Angle stored in radians in the preference system
		INT32 ConstrainAngleInDegrees = 0;	// Angle displayed in degrees to the user
		BOOL ReadOk = Camelot.GetPrefValue(TEXT("Constraints"), TEXT("Constrain Angle"), &ConstrainAngle);
		// Set up the default option displayed
		if (ReadOk)
		{
			ConstrainAngleInDegrees = (INT32)((ConstrainAngle * 180.0/PI) + 0.5);	// Convert angle to degrees
			
			INT32 NewAngle = ((INT32)angle) / ConstrainAngleInDegrees;
			angle = NewAngle * ConstrainAngleInDegrees;
		}
	}		

	// ok, set my angle
	SetAngle((INT32)angle);
}

/********************************************************************************************

>	void BevelBlob::SetAngle(INT32 Angle, BOOL bSetList = FALSE)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		The angle to set and a flag to indicate whether I should change the light
				angles of the nodes in my list or not

	Returns:	=
	Purpose:	Sets the blob's angle
	
********************************************************************************************/
void BevelBlob::SetAngle(INT32 Angle, BOOL bSetList/* = FALSE*/)
{
	// set my internal angle
	m_BlobAngle = Angle;

	NodeListItem * pItem = NULL;

	// if we are to change the list, then do this
	if (bSetList)
	{
		pItem = (NodeListItem *)m_BevelNodeList.GetHead();

		while (pItem)
		{
			if (pItem->pNode)
			{
				if (pItem->pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevelController)))
				{
//					((NodeBevelController *)pItem->pNode)->m_LightAngle = Angle;
					((NodeBevelController *)pItem->pNode)->m_LightAngle = 360 - Angle;
					((NodeBevelController *)pItem->pNode)->ReleaseCached();
				}
			}

			pItem = (NodeListItem *)m_BevelNodeList.GetNext(pItem);
		}
	}
}

/********************************************************************************************

>	void BevelBlob::SetAngle(INT32 Angle, BOOL bSetList = FALSE)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		The angle to set and a flag to indicate whether I should change the light
				angles of the nodes in my list or not

	Returns:	=
	Purpose:	Sets the blob's angle
	
********************************************************************************************/
void BevelBlob::SetTilt(INT32 Angle, BOOL bSetList/* = FALSE*/)
{
	NodeListItem * pItem = NULL;

	NodeBevelController * pControl = NULL;
	NodeBevel			* pBevel	= NULL;

	// if we are to change the list, then do this
	if (bSetList)
	{
		pItem = (NodeListItem *)m_BevelNodeList.GetHead();

		while (pItem)
		{
			if (pItem->pNode)
			{
				if (pItem->pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevelController)))
				{
					pControl = ((NodeBevelController *)pItem->pNode);
					pControl->m_Tilt = Angle;
					
					pBevel = pControl->GetBevelNode();

					if (pBevel)
					{
						pBevel->m_Tilt = Angle;
					}
				}
				((NodeBevelController *)pItem->pNode)->ReleaseCached();
			}

			pItem = (NodeListItem *)m_BevelNodeList.GetNext(pItem);
		}
	}
}


/********************************************************************************************

>	void BevelBlobs::RenderNodes()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/99
	Inputs:		

	Returns:	=
	Purpose:	Renders the bevel nodes with my light angle
	
********************************************************************************************/
void BevelBlob::RenderNodes()
{
	TRACEUSER( "DavidM", _T("BevelBlob - render nodes\n"));
	
	NodeListItem * pItem = (NodeListItem *)m_BevelNodeList.GetHead();

	while (pItem)
	{
		((NodeBevelController *)pItem->pNode)->ReRenderBevelBitmap((double)m_BlobAngle);

		pItem = (NodeListItem *)m_BevelNodeList.GetNext(pItem);
	}
}

/********************************************************************************************

>	void BevelBlob::ChangeContrast(INT32 NewContrast)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/99
	Inputs:		

	Returns:	=
	Purpose:	Changes the contrast of all bevel nodes' attributes
	
********************************************************************************************/
void BevelBlob::ChangeContrast(INT32 NewContrast)
{
	NodeListItem * pItem = (NodeListItem *)m_BevelNodeList.GetHead();

	NodeBevelController * pControl = NULL;

//	INT32 Count = 0;

//	Document * pDoc = Document::GetCurrent();
	
	while (pItem)
	{
		pControl = (NodeBevelController *)pItem->pNode;
		pControl->m_Contrast = NewContrast;
		((NodeBevelController *)pItem->pNode)->ReleaseCached();

		pItem = (NodeListItem *)m_BevelNodeList.GetNext(pItem);
	}
}

BOOL BevelTool::AreToolBlobsRenderedOnSelection()
{
	return TRUE;
}

//////////////////////////////////////////////////////////
// Drag op for Bevelling - V similar to the one for contouring
//////////////////////////////////////////////////////////////////////////////////////////
//  OpDragBevel
//
// This operation is responsible for creating and editing



/********************************************************************************************

>	OpDragBevel::OpDragBevel()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Purpose:	Constructor. 

********************************************************************************************/

OpDragBevel::OpDragBevel()
{
	m_pBarOp = NULL;

	m_bHasDragged = FALSE;

	m_pPathList = NULL;
	m_NumPaths = 0;
	m_pSetList = NULL;
	m_NumSets = 0;
	m_pPathOuterList = NULL;
	m_JoinType = RoundJoin;
	m_pPathLineWidthList = NULL;
}

/********************************************************************************************

>	OpDragBevel::~OpDragBevel()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Purpose:	Destructor.

********************************************************************************************/

OpDragBevel::~OpDragBevel()
{
	if (m_pPathList)
	{
		delete [] m_pPathList;
	}	

	if (m_pSetList)
	{
		delete [] m_pSetList;
	}

	if (m_pPathOuterList)
	{
		delete [] m_pPathOuterList;
	}

	if (m_pPathLineWidthList)
	{
		delete [] m_pPathLineWidthList;
	}
}

/********************************************************************************************

>	BOOL OpDragBevel::SetupDragInfo()
    
	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/99
	Inputs:		
	Outputs:	TRUE for success
	Returns:	-
	Purpose:	Sets up the drag information necessary to render the drag blobs

********************************************************************************************/
BOOL OpDragBevel::SetupDragInfo()
{
	Range Sel((*GetApplication()->FindSelection()));

	if (Sel.IsEmpty())
		return FALSE;

	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.SetRangeControl(rg);
	
	// go through all nodes getting their inside bounding rects, and
	// building up the summed path
	CProfileBiasGain Profile;

	// find out how many paths we require

	Node * pNode = Sel.FindFirst();

	DocRect dr(0,0,0,0);

	// how many paths do we need ?
	m_NumPaths = 0;

	Node * pSubNode = NULL;

	// don't do needs parent nodes !!!
	// therefore, recurse through the subtree of every node in the selection
	// calling their can become a's individually
	m_NumSets = 0;

	// now, also test to see if there are any bevel nodes in the selection
	// if there are, ignore the non-bevelled nodes.
	BOOL bBevelNodesExist = FALSE;

	List BevelNodeList;
	BevelTools::BuildListOfSelectedNodes(&BevelNodeList, CC_RUNTIME_CLASS(NodeBevelController),TRUE);

	if (!BevelNodeList.IsEmpty())
	{
		bBevelNodesExist = TRUE;
	}

	BevelNodeList.DeleteAll();

	DocView * pView = DocView::GetCurrent();
	m_Flatness = 200.0;

	if (pView)
	{
		double ZoomFactor = pView->GetScaledPixelWidth().MakeDouble() / pView->GetPixelWidth().MakeDouble();
		m_Flatness *= ZoomFactor;
	}

	// we do a depth first search of the selection, calling DoBecomeA on the appropriate nodes
	// we do this because
	// a) we need to neglect all 'needs parent' nodes, which means calling doBecomeA on
	// just the top level selected nodes is insufficient
	// we also count how many 'sets' we need
	// a set defines a range in the path list (i.e. between path 2 and path 6) of paths
	// which need to be merged together before rendering
	// in building this set, we need to take into account when the nodes switch from one
	// contour controller node to another, as this obviously indicates the start of a new set
	// if any contour controller nodes exist, then we must ignore all nodes which are in the
	// selection but not part of a contour node. This is because in this case, when dragging,
	// no new contour nodes are created - the existing ones widths are changed

	Node* pLastController = NULL;
	Node* pParentController = NULL;
	AttrJoinType* pJoinType = NULL;
	AttrLineWidth* pLineWidth = NULL;

	BOOL bFoundJoinType = FALSE;
	UINT32 ObjectCount = 0;

	m_JoinType = RoundJoin;
	BecomeA MyTestBecomeA(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath));
	MyTestBecomeA.ResetCount();

	while (pNode)
	{
		pSubNode = pNode->FindFirstDepthFirst();

		// set up the parent controller from this node
		if (bBevelNodesExist)
		{
			pLastController = pSubNode->FindParent(CC_RUNTIME_CLASS(NodeBevelController));

			if(pLastController)
			{
				((NodeRenderableInk *)pLastController)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrJoinType), (NodeAttribute **)(&pJoinType));

				if (pJoinType)
				{
					bFoundJoinType = TRUE;
					m_JoinType = pJoinType->Value.JoinType;
				}
			}
		}
	
		while (pSubNode)
		{
			if(pSubNode->IsABevel())
			{
				((NodeRenderableInk *)pSubNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrJoinType),
					(NodeAttribute **)(&pJoinType));
				
				if (pJoinType)
				{
					bFoundJoinType = TRUE;
					m_JoinType = pJoinType->Value.JoinType;
				}
			}

			MyTestBecomeA.ResetCount();
			if (NodeCanBeDragBevelled(pSubNode, pNode, bBevelNodesExist) &&
				pSubNode->CanBecomeA(&MyTestBecomeA))
			{
				// test for whether we're just doing bevel nodes or all nodes
				if (!bBevelNodesExist || 
						pSubNode->FindParent(CC_RUNTIME_CLASS(NodeBevelController)) != NULL)
					m_NumPaths += MyTestBecomeA.GetCount();

				if (!bFoundJoinType)
				{
					((NodeRenderableInk *)pSubNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrJoinType),
						(NodeAttribute **)(&pJoinType));
					
					if (pJoinType)
					{
						bFoundJoinType = TRUE;
						m_JoinType = pJoinType->Value.JoinType;
					}
				}
				
				// check for the number of sets needing to be increased - i.e. if
				// the controller node of the new node is different to the last controller
				if (bBevelNodesExist)
				{
					pParentController = pSubNode->FindParent(CC_RUNTIME_CLASS(NodeBevelController));

					if (pParentController)
					{
						if (pParentController != pLastController &&
							pLastController != NULL)
						{
							m_NumSets ++;
						}

						pLastController = pParentController;
					}
				}
			}

			pSubNode = pSubNode->FindNextDepthFirst(pNode);
		}

		m_NumSets ++;

		pNode = Sel.FindNext(pNode);
	}

	// increase the number of sets by 1
	m_NumSets++;

	ERROR2IF(m_NumPaths == 0, FALSE, "Nothing to bevel");

	if (m_pPathList)
		delete [] m_pPathList;

	if (m_pSetList)
		delete [] m_pSetList;

	if (m_pPathOuterList)
		delete [] m_pPathOuterList;

	if (m_pPathLineWidthList)
		delete [] m_pPathLineWidthList;

	// each path has whether the object it came from was an inner or an outer bevel
	// or not
	ALLOC_WITH_FAIL(m_pPathOuterList, new BOOL[m_NumPaths], this);
	ALLOC_WITH_FAIL(m_pPathList, new Path[m_NumPaths], this);
	ALLOC_WITH_FAIL(m_pPathLineWidthList, new LineWidthAttribute*[m_NumPaths], this);
	ALLOC_WITH_FAIL(m_pSetList, new UINT32[m_NumSets], this);

	UINT32 i;
	for (i = 0; i < m_NumPaths; i++)
	{
		m_pPathList[i].Initialise();
		m_pPathOuterList[i] = TRUE;
		m_pPathLineWidthList[i] = NULL;
	}

	// Make sure we have a valid pOurDoc as Contouring moulds with bitmap fills go bang!
	pOurDoc = Document::GetCurrent();

	// sum all paths together in the range
	ContourDragBecomeA MyBecomeA(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath), this, FALSE,
		m_pPathList, m_NumPaths);

	pNode = Sel.FindFirst();

	// don't do needs parent nodes !!!
	// therefore, recurse through the subtree of every node in the selection
	// calling their do become a's individually

	NodeBevelController * pControl = NULL;

	UINT32 SetCount = 0;
	NodeAttribute* pAttr = NULL;
	BOOL IsVariableWidth = FALSE;
	Node* pNodePath = NULL;
	UINT32 StartCount = 0;
	UINT32 x = 0;

	while (pNode)
	{
		pSubNode = pNode->FindFirstDepthFirst();

		// set up the parent controller from this node
		if (bBevelNodesExist)
			pLastController = pSubNode->FindParent(CC_RUNTIME_CLASS(NodeBevelController));

		// start a new set
		m_pSetList[SetCount++] = MyBecomeA.GetCount();

		ERROR3IF(SetCount >= m_NumSets, "Number of sets doesn't match");		

		while (pSubNode)
		{
//			MyTestBecomeA.ResetCount();
			if (NodeCanBeDragBevelled(pSubNode, pNode, bBevelNodesExist) &&
				pSubNode->CanBecomeA(&MyTestBecomeA) &&
				pSubNode->IsAnObject())
			{
				// check for the number of sets needing to be increased - i.e. if
				// the controller node of the new node is different to the last controller
				if (bBevelNodesExist)
				{
					pParentController = pSubNode->FindParent(CC_RUNTIME_CLASS(NodeBevelController));

					if (pParentController)
					{
						if (pParentController != pLastController && pLastController != NULL)
							m_pSetList[SetCount++] = MyBecomeA.GetCount(); // start a new set

						pLastController = pParentController;
					}
				}

				// do the DoBecomeA
				if (!bBevelNodesExist || pSubNode->FindParent(CC_RUNTIME_CLASS(NodeBevelController)) != NULL)
				{
					// get the start path count
					StartCount = MyBecomeA.GetCount();
					
					// find out if we have a parent bevel controller node or not
					pControl = (NodeBevelController *)pSubNode->FindParent(CC_RUNTIME_CLASS(NodeBevelController));

					pLineWidth = NULL;
					((NodeRenderableInk *)pSubNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrLineWidth),(NodeAttribute**)(&pLineWidth));

					// do the become A
					if (!pSubNode->IsNodePath())
						pSubNode->DoBecomeA(&MyBecomeA);
					else
					{
						// make the node out of the path first
						pNodePath = ((NodePath *)pSubNode)->MakeNodePathFromAttributes(m_Flatness,NULL,FALSE,FALSE);

						if(pNodePath)
						{
							pNodePath->DoBecomeA(&MyBecomeA);
							delete pNodePath;
						}
					}
										
					if(((NodePath*)pSubNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrVariableWidth),&pAttr))
						IsVariableWidth = (pAttr && ((VariableWidthAttrValue*)pAttr->GetAttributeValue())->GetWidthFunction() != NULL);

					if (pLineWidth && !IsVariableWidth)
						m_pPathLineWidthList[ObjectCount] = &pLineWidth->Value;

					ObjectCount++;

					// find out if my parent controller is an inner or an outer,
					// and store the value for each path which has just been created
					if (pControl)
					{
						// find the applied join type attribute for this particular series of
						for (x = StartCount; x < MyBecomeA.GetCount(); x++)
						{
							if (!pControl->m_bOuter)
								m_pPathOuterList[x] = FALSE;
						}
					}
				}
			}

			pSubNode = pSubNode->FindNextDepthFirst(pNode);
		}

		pNode = Sel.FindNext(pNode);
	}

	// put the last set node in
	m_pSetList[SetCount] = MyBecomeA.GetCount();

	m_SelRect.MakeEmpty();

	// calculate the rect from the paths
	for (i = 0 ; i < m_NumPaths; i++)
	{
		if (m_pPathList[i].GetNumCoords() > 2)
		{
			m_pPathList[i].GetTrueBoundingRect(&dr);
			m_SelRect = m_SelRect.Union(dr);
		}
	}

	// reduce by the blob rect
	BlobManager * pBlobMgr = GetApplication()->GetBlobManager();

	if(pBlobMgr)
		m_SelRect.Inflate(pBlobMgr->GetBlobSize());

	return TRUE;
}



/********************************************************************************************

>	BOOL OpDragBevel::NodeCanBeDragBevelled(Node* pSubNode,
											Node* pNode,
											BOOL bBevelNodesExist)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/05/2000
	Inputs:		pSubNode	the node which we are considering.
				pNode		the node on which the drag is actually occurring.
							note that this should be an ancestor of pSubNode in the tree.
				bBevelNodesExist	TRUE if we're editting existing bevels,
									FALSE if we're creating new bevels.

	Returns:	TRUE if pSubNode should have its outline taken into account when the user
				creates or changes a bevel by dragging.
				FALSE if not.

	Purpose:	Helper function for SetupDragInfo(), which encapsulates a test for whether
				a node's outline should be considered when dragging bevels.
				There are a number of rules for exactly which nodes should be considered;
				examine the body of this function for details.
	Errors:		ERROR3 in DEBUG if any parameters are NULL.
				*Will* bomb out with AV's if NULL parameters are passed, so don't!
	See also:	SetupDragInfo()

********************************************************************************************/
BOOL OpDragBevel::NodeCanBeDragBevelled(Node* pSubNode,
											Node* pNode,
											BOOL bBevelNodesExist)
{
	// subnode must not require a parent node to exist.
	BOOL	ok = !pSubNode->NeedsParent(NULL);

	// subnode must be a NodeRenderableInk.
	if (ok) ok = pSubNode->IsAnObject();

	// subnode must not be any compound node other than the exceptions listed here.
	if (ok) ok = !pSubNode->IsCompound() || pSubNode->IsABaseTextClass() ||
					pSubNode->IS_KIND_OF(NodeMould) || pSubNode->IsANodeClipViewController();

	// subnode must not reside within a NodeMould.
	if (ok) ok = (pSubNode->FindParent(CC_RUNTIME_CLASS(NodeMould)) == NULL);

	// ClipView tests.
	if (ok)
	{
		// if bevels do exist, then subnode must not reside within a ClipView group
		// which has, or whose parents have, a bevel currently applied, and which is,
		// or lies beneath, the bevel's object node.
		if (bBevelNodesExist)
		{
			Node* pNCC = pSubNode->FindParent(CC_RUNTIME_CLASS(NodeBevelController));
			if (pNCC != NULL)
			{
				Node* pNodeTest = pNCC;
				while (pNodeTest != pNode && pNodeTest != NULL)
					pNodeTest = pNodeTest->FindParent();
				if (pNodeTest != NULL)
				{
					// pNCC is, or is a child of, pNode.
					// now we just need to find out whether pSubNode and pNCC
					// have a NCVC in between.
					ok = !pSubNode->IsFamily(CC_RUNTIME_CLASS(NodeClipViewController), pNCC);
				}
			}
		}

		// if no bevels exist yet, then subnode must not reside within a ClipView group
		// which either is, or lies beneath, the bevel's object node.
		else
			ok = !pSubNode->IsFamily(CC_RUNTIME_CLASS(NodeClipViewController), pNode);
	}

	return ok;

/*
 *	This comment holds the unexpanded version of the above tests,
 *	apart from the ClipView test for when bevels do exist.
 *
	return !pSubNode->NeedsParent(NULL) && 
			(!pSubNode->IsCompound() || pSubNode->IsABaseTextClass() || 
			pSubNode->IS_KIND_OF(NodeMould) || pSubNode->IsANodeClipViewController()) && 
			pSubNode->IsAnObject() &&
			pSubNode->FindParent(CC_RUNTIME_CLASS(NodeMould)) == NULL &&
			(bBevelNodesExist || 
			!pSubNode->IsFamily(CC_RUNTIME_CLASS(NodeClipViewController), pNode))
			;
*/
}



/********************************************************************************************

>	void OpDragBevel::AlterPointerPosToAccountForBlob(DocCoord * pPoint)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		The point to change
	Purpose:	Ensures that dragging back onto a blob returns you to the original
				contour value

********************************************************************************************/
void OpDragBevel::AlterPointerPosToAccountForBlob(DocCoord * pPoint)
{
	DocCoord PPos;

	DocCoord Offset;

	double Scale = 0;

	BlobManager * pBlobMgr = GetApplication()->GetBlobManager();

	double BlobSize = 0;

	if (pBlobMgr)
		BlobSize = pBlobMgr->GetBlobSize()/2;
	else
		return;
	
	if (m_BlobRect.IsValid())
	{
		DocCoord Centre = m_BlobRect.Centre();
			
		if (m_BlobRect.ContainsCoord(*pPoint))
		{
			
			pPoint->x = Centre.x;
			pPoint->y = Centre.y;
		}
		else
		{
			// move the pointer appropriately so that the blob is always
			// the same value
			
			// first find the point which is the intersection between the line from
			// the centre of the blob to the point on the outside of the blob
			PPos.x = pPoint->x - Centre.x;
			PPos.y = pPoint->y - Centre.y;

			if (abs(PPos.x) > abs(PPos.y))
			{
				Scale = ((double)abs(PPos.y)) / ((double)abs(PPos.x));
				Scale *= BlobSize;

				if (PPos.x > 0)
				{
					Offset.x = (INT32)BlobSize;
				}
				else
				{
					Offset.x = (INT32)-BlobSize;
				}

				if (PPos.y > 0)
				{
					Offset.y = (INT32)Scale;
				}
				else
				{
					Offset.y = (INT32)-Scale;
				}
			}
			else
			{
				Scale = ((double)PPos.x) / ((double)PPos.y);
				Scale *= BlobSize;

				if (PPos.y > 0)
				{
					Offset.y = (INT32)BlobSize;
				}
				else
				{
					Offset.y = (INT32)-BlobSize;
				}

				if (PPos.x > 0)
				{
					Offset.x = (INT32)Scale;
				}
				else
				{
					Offset.x = (INT32)-Scale;
				}
			}

			pPoint->x -= Offset.x;
			pPoint->y -= Offset.y;
		}
	}
}

/********************************************************************************************

>	BOOL OpDragBevel::DoDrag(BevelInfoBarOp * pBar, DocCoord &PointerPos,
								BOOL bDragOnBlob, DocRect *pBlobRect)
    
	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/99
	Inputs:		The info bar starting the drag, and the pointer position when the drag is 
				started.
				bDragOnBlob = TRUE if the drag started on a tool blob
	Outputs:	-
	Returns:	-
	Purpose:	This starts a drag that may lead to a contour.
				The DragFinished() method will do the hard work of contouring if it can be done.

********************************************************************************************/

BOOL OpDragBevel::DoDrag(BevelInfoBarOp * pBar, DocCoord &PointerPos, BOOL bDragOnBlob,
						 DocRect *pBlobRect)
{	
	// snap the pointer position
	DocView *pView = DocView::GetCurrent();
	Spread * pSpread = Document::GetSelectedSpread();

	if (pView && pSpread)
	{
		pView->Snap(pSpread, &PointerPos, FALSE, TRUE);
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////
	//
	// This added by Karim MacDonald 17/12/1999, to fix a bug, whereby starting a bevel
	// drag whilst a text char was sub-selected, then dragging with selector tool, caused
	// and error.
	//

	// if the drag was started on a blob, ensure any text in the selection is valid for this.
	if (bDragOnBlob)
		GetApplication()->FindSelection()->MakePartialSelectionWhole();

	// otherwise quit - can't do this drag unless it's from a blob.
	else
		return FALSE;
	//
	/////////////////////////////////////////////////////////////////////////////////////////

	// check whether the selection allows the drag
	// if so, just treat it like a non-blob drag
	/*
	OpCreateBevel * pCreateOp = NULL;
	ALLOC_WITH_FAIL(pCreateOp, new OpCreateBevel, this);

	ObjChangeFlags flgs(FALSE, FALSE, FALSE, FALSE);
	flgs.MultiReplaceNode = TRUE;
	ObjChangeParam OP(OBJCHANGE_STARTING, flgs, NULL, pCreateOp, OBJCHANGE_CALLEDBYOP);

	if (!GetApplication()->FindSelection()->AllowOp(&OP) && bDragOnBlob)
	{
		delete pCreateOp;
		return TRUE;
	}			

	delete pCreateOp;
	*/
	
	m_pBarOp = pBar;
	m_bHasDragged = FALSE;
	m_LastPointerPos = PointerPos;
	m_ThisPointerPos = PointerPos;	

	if (pBlobRect)
		m_BlobRect = *pBlobRect;
	else
		m_BlobRect = DocRect(0,0,0,0);

	m_bDragStartedOnBlob = bDragOnBlob;

	// if there's no selection then forget it !
	if (!GetApplication()->FindSelection())
		return FALSE;

	// decide what the start value for the drag will be
	List BevelList;
	BevelTools::BuildListOfSelectedNodes(&BevelList, CC_RUNTIME_CLASS(NodeBevelController));

	if (BevelList.IsEmpty())
	{
		m_OriginalWidth = 0;
	}
	else
	{
		NodeListItem * pItem = (NodeListItem *)BevelList.GetHead();

		m_OriginalWidth = ((NodeBevelController *)(pItem->pNode))->m_Indent;

		if (((NodeBevelController *)(pItem->pNode))->m_bOuter)
			m_OriginalWidth = -m_OriginalWidth;

		BevelList.DeleteAll();
	}

	// Tell the Dragging system that we need drags to happen
	StartDrag( DRAGTYPE_AUTOSCROLL );

	if (bDragOnBlob)
	{
		if (!SetupDragInfo())
		{
			EndDrag();
			return FALSE;
		}
		
		DocCoord BlobCentre(pBlobRect->Centre());

		m_LastPointerPos = BlobCentre;
		m_ThisPointerPos = BlobCentre;

		// calculate the width
		INT32 Width = ContourTool::CalculateContourWidth(m_SelRect, 
			BlobCentre);

		m_StartDragWidth = Width;
	}
	
	return TRUE;
}




/********************************************************************************************

>	void OpDragBevel::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
								   Spread* pSpread, BOOL bSolidDrag)
	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/99
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods  - Which key modifiers are being pressed
				pSpread    - The spread that the mouse pointer is over
	Purpose:	Takes the pointer position and calculates the new dragged outline of the EORd
				bounding box
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpDragBevel::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
								   Spread* pSpread, BOOL bSolidDrag)
{
	ERROR3IF(!m_pBarOp, "No info bar op");

	// snap the pointer position
	DocView *pView = DocView::GetCurrent();

	if (pView && pSpread)
	{
		pView->Snap(pSpread, &PointerPos, FALSE, TRUE);
	}

	if (PointerPos == m_LastPointerPos)
		return;

	if (!m_bDragStartedOnBlob)
	{
		m_bHasDragged = TRUE;
		return;
	}

	// render the drag blobs off first
	if (m_bHasDragged)
		RenderDragBlobs(m_SelRect, pSpread, bSolidDrag);

	m_bHasDragged = TRUE;
	m_LastPointerPos = PointerPos;
	m_ThisPointerPos = PointerPos;	

	BevelTool::DisplayStatusBarHelp(_R(IDS_BEVELDRAGSTATUSHELP));
	
	// calculate the contour width & set the infobar
	INT32 Width = (ContourTool::CalculateContourWidth(m_SelRect, PointerPos) - m_StartDragWidth) 
		+ m_OriginalWidth;

	RenderDragBlobs(m_SelRect, pSpread, bSolidDrag);

	if (Width < 0)
	{
		// outer contour
		m_pBarOp->SetBoolGadgetSelected(_R(IDC_BTN_BEVELOUTER), TRUE);
		m_pBarOp->SetBoolGadgetSelected(_R(IDC_BTN_BEVELINNER), FALSE);
	}
	else
	{
		// inner contour
		m_pBarOp->SetBoolGadgetSelected(_R(IDC_BTN_BEVELOUTER), FALSE);
		m_pBarOp->SetBoolGadgetSelected(_R(IDC_BTN_BEVELINNER), TRUE);
	}

	m_pBarOp->PaintGadgetNow(_R(IDC_BTN_BEVELINNER));
	m_pBarOp->PaintGadgetNow(_R(IDC_BTN_BEVELOUTER));

	m_pBarOp->m_Depth = abs(Width);
	m_pBarOp->SliderUpdate(FALSE, FALSE);
}



/********************************************************************************************

>	void OpDragBevel::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
								Spread* pSpread, BOOL Success, BOOL bSolidDrag)

    
	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickMods - the key modifiers being pressed
				pSpread - The spread that the drag finished on
				Success - TRUE if the drag was terminated properly, FALSE if it
				was ended with the escape key being pressed
	Purpose:	Ends the drag.
				Either creates a new grid or resizes GridClicked depending on the state of affairs
				when the drag started
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpDragBevel::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
								Spread* pSpread, BOOL Success, BOOL bSolidDrag)
{
	// snap the pointer position
	DocView *pView = DocView::GetCurrent();

	if (pView && pSpread)
	{
		pView->Snap(pSpread, &PointerPos, FALSE, TRUE);
	}

	if (Tool::GetCurrentID() != TOOLID_BEVELTOOL)
		return;

	//	Karim 11/04/2000
	//	Commented out - this test shouldn't be needed, as unless the current tool is derived
	//	from BevelTool, testing the tool's ID as above should always give the same result.
//	if (!Tool::GetCurrent()->IS_KIND_OF(BevelTool))
//		return;
	
	BevelTool * pTool = (BevelTool *)Tool::GetCurrent();
	
	// End the drag and the op
//	BlobManager * pBlobMgr = GetApplication()->GetBlobManager();

	EndDrag();

	// added 20/12/1999 by Karim MacDonald.
	// if the drag did not start on a blob, we quit now.
	if (!m_bDragStartedOnBlob)
		return;

	// calculate the contour width & set the infobar
	INT32 Width = (ContourTool::CalculateContourWidth(m_SelRect, PointerPos) - m_StartDragWidth) 
		+ m_OriginalWidth;

	// if we've not dragged, then do nothing
	if (!m_bHasDragged)
		return;

	if (m_pBarOp)
		m_pBarOp->UpdateState();

	m_bHasDragged = FALSE;

	if (!pSpread)
		pSpread = Document::GetSelectedSpread();

	// render the drag blobs off
	RenderDragBlobs(m_SelRect, pSpread, bSolidDrag);	

	// create the bevel, or change its width
	if (Success)
	{
		if (pTool)
		{
			pTool->RenderToolBlobs(pSpread, NULL);
			pTool->SetBlobRendering(FALSE);
		}
		
		// set up the bevel creation param
		BevelInfo Info;

		Info.m_bCreateNewBevels = FALSE;

		Info.m_Indent = abs(Width);
		Info.m_bBevelIndentChanged = TRUE;
		
		if (Width < 0)
		{
			Info.m_bOuter = TRUE;
		}
		else
		{
			Info.m_bOuter = FALSE;
		}

		Info.m_bBevelDirectionChanged = (m_OriginalWidth > 0 && Width < 0) || (m_OriginalWidth < 0 && Width > 0);

		// standard attributes
		Info.m_BevelType = 0;
		Info.m_Contrast = 50;
		Info.m_LightAngle = 300;

		OpDescriptor * pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpCreateBevel));
		
		if (pOpDesc)
		{
			pOpDesc->Invoke(&Info);
		}
		
		// put the tool blobs back on
		if (pTool)
		{
			pTool->SetBlobRendering(TRUE);
			pTool->SetupBlobList();
			pTool->RenderToolBlobs(pSpread, NULL);
		}
	}

	delete [] m_pPathList;
	m_pPathList = NULL;

	delete [] m_pSetList;
	m_pSetList = NULL;

	delete [] m_pPathOuterList;
	m_pPathOuterList = NULL;
}

/********************************************************************************************

>	void OpDragBevel::RenderDragBlobs(MILLIPOINT Width,Spread* pSpread,
									BOOL bKeepDirections)

	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com> - rewritten Dave Mc code 06/06/00
	Created:	11/10/94
	Inputs:		Width   - The width of the bevel to render the drag blobs of
				pSpread - The spread that the drawing will happen on
				bKeepDirections - Whether to maintian each set's direction (either inner
								  or outer) or to render the blobs with the signed value
								  passed in.
	Purpose:	Draws an EORed outline of distance Width away from the original objects!

********************************************************************************************/
void OpDragBevel::RenderDragBlobs(MILLIPOINT Width,Spread* pSpread, BOOL bKeepDirections)
{
	bool Outer = false;
	if(Width < 0)
		Outer = true;

	if (bKeepDirections)
		Width = abs(Width);

	if (m_pPathLineWidthList == NULL || m_pPathList == NULL || m_pSetList == NULL)
	{
		ERROR3("No list defined");
		return;
	}

	// Allocate Two Path Arrays to hold the outline information
	// SetPathList holds the all the paths inside compound nodes
	Path * pSetPathList = NULL;
	ALLOC_WITH_FAIL(pSetPathList, new Path[m_NumSets-1], this);

	// DrawPAthList holds the path that has been expanded by the width
	Path * pDrawPathList = NULL;
	ALLOC_WITH_FAIL(pDrawPathList, new Path[m_NumPaths], this);

	// Two local paths to hold intermediate path steps
	Path CopyPath;
	Path TempPath;
	CopyPath.Initialise();
	TempPath.Initialise();

	DocRect DrawBounds(0,0,0,0);
	DocRect dri(0,0,0,0);
	DocRect drj(0,0,0,0);
	UINT32 j = 0;
	INT32 PathWidth = 0;

	//  Get the current Join Style
	JoinStyles JoinS = (m_JoinType==MitreJoin) ? JOIN_MITER : (m_JoinType==RoundJoin) ? JOIN_ROUND : JOIN_BEVEL;

	INT32 CurrentWidth = 0;
	UINT32 i;
	// Loop trough the path list expanding them and saving the result into the DrawPathList
	for (i = 0 ; i < m_NumPaths; i++)
	{
		pDrawPathList[i].Initialise();
		PathWidth = Width;

		// We now require to add the linewidth to the bevel outline!
		if(m_pPathLineWidthList[i] != NULL)
		{
			CurrentWidth = m_pPathLineWidthList[i]->LineWidth;
			PathWidth -= (CurrentWidth >> 1);
			Outer |= (Width >= 0 && Width < (CurrentWidth >> 1));
		}

		// Make sure we use the correct function by checking to see if the path is closed or not!
		BOOL IsPathClosed = m_pPathList[i].IsClosed() && m_pPathList[i].IsFilled;
		m_pPathList[i].InitializeContourValues(abs(PathWidth)*2,JoinS,Outer,m_Flatness,IsPathClosed,IsPathClosed,CAPS_ROUND);
		m_pPathList[i].GetContourForStep(&pDrawPathList[i],1.0);
	}

	// merge all the paths in each set and keep an overall bounding rect going
	for (i = 0 ; i < m_NumSets - 1; i++)
	{
		pSetPathList[i].Initialise();
		pSetPathList[i].GetTrueBoundingRect(&dri);
		
		for (j = m_pSetList[i]; j < m_pSetList[i+1]; j++)
		{
			pDrawPathList[j].GetTrueBoundingRect(&drj);
			pSetPathList[i].MergeTwoPaths(pDrawPathList[j]);
		}

		if(m_NumSets > 1)
		{
			CopyPath.ClearPath(FALSE);
			CopyPath.MergeTwoPaths(pSetPathList[i]);
			pSetPathList[i].ClearPath(FALSE);
			TempPath.ClipPathToPath(CopyPath,&pSetPathList[i],7|(1<<4),50,m_Flatness,m_Flatness);
		}

		pSetPathList[i].IsFilled = TRUE;
		pSetPathList[i].IsStroked = FALSE;

		DrawBounds = DrawBounds.Union(dri);
	}

	// increase the bounds slightly - David code ?
	DrawBounds.Inflate(750);

	// Get the render region required
	RenderRegion* pRegion = DocView::RenderOnTop( NULL, pSpread, UnclippedEOR );

	while ( pRegion != NULL )
	{
		// Set the line colour and Draw the rect
		pRegion->SaveContext();

		pRegion->SetLineWidth(0);
		
		pRegion->SetWindingRule(PositiveWinding);
		
		pRegion->SetLineColour(COLOUR_RED);
		pRegion->SetFillColour(COLOUR_NONE);

		// draw all the paths in the current SetPathList
		for (i = 0 ; i < m_NumSets-1; i++)
		{
			pRegion->DrawPath(&(pSetPathList[i]));
		}
		
		// Get the Next render region
		pRegion->RestoreContext();
		pRegion = DocView::GetNextOnTop( NULL );
	}

	// tidy up and finish!
	delete [] pDrawPathList;
	delete [] pSetPathList;
}


/********************************************************************************************

>	void OpDragBevel::RenderDragBlobs(DocRect Rect,Spread* pSpread, BOOL bSolidDrag)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		Rect 	- The region that needs the blobs to be drawn
				pSpread - The spread that the drawing will happen on
	Purpose:	Draws an EORed rectangle defined by AnchorPoint and DragPoint

********************************************************************************************/

void OpDragBevel::RenderDragBlobs(DocRect Rect,Spread* pSpread, BOOL bSolidDrag)
{
	if (m_pPathList == NULL || m_NumPaths == 0)
	{
		ERROR3("Render drag blobs - no path list");
		return;
	}
	
	// set up the bounds
	INT32 Width = (ContourTool::CalculateContourWidth(m_SelRect, m_ThisPointerPos) - m_StartDragWidth) 
		+ m_OriginalWidth;

	RenderDragBlobs(Width, pSpread);	
}

/********************************************************************************************

>	BOOL OpDragBevel::Declare()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpDragBevel::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_BEVEL_TOOL),
								CC_RUNTIME_CLASS(OpDragBevel), 
								OPTOKEN_DRAGBEVEL,
								OpDragBevel::GetState,
								0,      	/* help ID */
								_R(IDBBL_NOOP),	/* bubble ID */
								0			/* bitmap ID */
								));
}


/********************************************************************************************

>	static OpState OpDragBevel::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the push tool
	Returns:	The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpDragBevel::GetState(String_256* Description, OpDescriptor*)
{
	OpState State;
	
	return State;
}

/********************************************************************************************

>	virtual void OpDragBevel::GetOpName(String_256* OpName) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		OpName = ptr to str to place op name in
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpDragBevel::GetOpName(String_256* OpName) 
{ 
	*OpName = String_256(_R(IDS_BEVELDRAGCREATEOP));
}  



#endif


