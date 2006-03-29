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
// Implementation of the grid tool

/*
*/

#include "camtypes.h"
#include "app.h"
#include "range.h"
//#include "markn.h"
//#include "simon.h"
//#include "viewrc.h"
#include "errors.h"
#include "docview.h"
#include "cursor.h"
#include "rndrgn.h"
#include "csrstack.h"
#include "osrndrgn.h"
//#include "resource.h"
//#include "barsdlgs.h"
#include "paper.h"
#include "node.h"
#include "document.h"
#include "undoop.h"
#include "bars.h"
#include "chapter.h"
#include "spread.h"
#include "oilfiles.h"
#include "camelot.h"
#include "mainfrm.h"
#include "convert.h"
#include "units.h"
#include "msg.h"
#include "opdesc.h"
#include "cutop.h"
#include "progress.h"
#include "bubbleid.h"

#include "gridtool.h"

//#include "will2.h"

DECLARE_SOURCE( "$Revision: 662 $" );

CC_IMPLEMENT_DYNCREATE( OpGrid, 		 UndoableOperation )
CC_IMPLEMENT_DYNCREATE( OpGridNewResize, OpGrid )
CC_IMPLEMENT_DYNCREATE( OpGridSelection, OpGrid )
CC_IMPLEMENT_DYNCREATE( OpGridChange,    OpGrid )
CC_IMPLEMENT_DYNCREATE( OpGridDelete,    OpGrid )
CC_IMPLEMENT_DYNCREATE( OpGridCut,    	 OpGrid )
CC_IMPLEMENT_DYNCREATE( OpGridCopy,    	 OpGrid )
CC_IMPLEMENT_DYNCREATE( OpGridPaste,   	 OpGrid )
CC_IMPLEMENT_DYNCREATE( OpGridDuplicate, OpGrid )
CC_IMPLEMENT_DYNCREATE( GridInfoBarOp,   InformationBarOp )

static OpState NoClipboardOpGetState(String_256*, OpDescriptor*);

#define new CAM_DEBUG_NEW     

static struct
{
	GridType  	Type;
	UINT32 		IDS;
} GridTypeList[NUM_GRID_TYPES] = 
{
	{ RECTANGULAR,	_R(IDS_RECTANGULAR) },
	{ ISOMETRIC,	_R(IDS_ISOMETRIC) }
};

// These are still char* while we wait for resource technology to be developed for modules
char* GridTool::FamilyName	= "Grid Tools";
char* GridTool::ToolName 	= "Grid Tool";
char* GridTool::Purpose 	= "Grid manipulation";
char* GridTool::Author 		= "Mark Neves";

Spread*  		GridTool::SpreadClicked 		= NULL;
NodeGrid*		GridTool::LastGridClicked 		= NULL;
NodeGrid* 		GridTool::GridClicked 			= NULL;
GridBlobType 	GridTool::GridBlobClicked 		= NoGridBlob;
DocCoord 		GridTool::PointClicked;
//BOOL			GridTool::CurrentTool 			= FALSE;
UINT32			GridTool::LastStatusID			= 0;
GridInfoBarOp*	GridTool::pGridInfoBarOp		= NULL;
Document*		GridTool::pDocument				= NULL;

BOOL 			OpGridNewResize::StartADrag	= TRUE;
BOOL 			OpGridNewResize::EndTheDrag	= TRUE;

static MILLIPOINT abs(MILLIPOINT n) { if (n<0) return (0-n); else return (n);}
static MILLIPOINT sgn(MILLIPOINT n) { if (n<0) return (-1);  else return (1);}

CC_IMPLEMENT_MEMDUMP( GridTool, Tool_v1 )

/********************************************************************************************

>	GridTool::GridTool()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/94
	Purpose:	Constructor - does nothing.
				Other initialisation is done in GridTool::Init which is called by the Tool Manager
	SeeAlso:	GridTool::Init

********************************************************************************************/

GridTool::GridTool()
{
	pcCurrentCursor = NULL;
}

/********************************************************************************************

>	GridTool::~GridTool()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/94
	Purpose:	Destructor (Virtual). Does nothing.

********************************************************************************************/

GridTool::~GridTool()
{
	// Dummy destructor

//	if (IsUserName("MarkN")) TRACE( _T("~GridTool()\n"));

}



/********************************************************************************************

>	BOOL GridTool::Init( INT32 Pass )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/94
	Returns:	FALSE if it does not want to be created, TRUE otherwise
	Purpose:	Used to check if the Tool was properly constructed
	SeeAlso:	GridTool::GridTool

********************************************************************************************/

BOOL GridTool::Init()
{
//	if (IsUserName("MarkN")) TRACE( _T("\nGridTool::Init called\n"));
//	if (IsUserName("MarkN")) TRACE( _T("Creating grid tool info bar\n"));

	// Declare ops and only succeed if all declarations succeed
	BOOL ok = (	OpGridNewResize::Declare() && 
				OpGridSelection::Declare() && 
				OpGridChange::Declare());


	// This section reads in the infobar definition and creates an instance of
	// GridInfoBarOp.  Also pGridInfoBarOp, the ptr to my infobar, as set up
	// after the infobar is successfully read and created.
	if (ok)
	{
		CCResTextFile 		file;				// Resource File
		GridInfoBarOpCreate BarCreate;			// Object that creates GridInfoBarOp objects

		 		ok = file.open(_R(IDM_GRID_BAR), _R(IDT_INFO_BAR_RES));		// Open resource
		if (ok) ok = DialogBarOp::ReadBarsFromFile(file,BarCreate);	// Read and create info bar
		if (ok) file.close();									 	// Close resource

		ENSURE(ok,"Unable to load gridbar.ini from resource\n"); 

		if (ok)
		{
			// Info bar now exists.  Now get a pointer to it
			String_32 str = String_32(_R(IDS_GRIDTOOL_INFOBARNAME));
			DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(str);

					ok = (pDialogBarOp != NULL);
			if (ok) ok = pDialogBarOp->IsKindOf(CC_RUNTIME_CLASS(GridInfoBarOp));
			if (ok) pGridInfoBarOp = (GridInfoBarOp*)pDialogBarOp;

			ENSURE(ok,"Couldn't find grid tool info bar");
		}
	}

	return (ok);
}


/********************************************************************************************

>	void GridTool::Describe(void *InfoPtr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/94
	Inputs:		InfoPtr - 	A pointer to a tool info block. It is passed cast to void* as
							the version of the tool is unknown at this point. Later versions 
							of the Tool class may have more items in this block, that this 
							tool will not use
	Outputs:	InfoPtr - 	The structure pointed to by InfoPtr will have had all the info
							that this version of the Tool knows about
	Purpose:	Allows the tool manager to extract information about the tool

********************************************************************************************/

void GridTool::Describe(void *InfoPtr)
{
	// Cast structure into the latest one we understand.
	ToolInfo_v1 *Info = (ToolInfo_v1 *) InfoPtr;

	Info -> InfoVersion = 1;
	
	Info -> InterfaceVersion = GetToolInterfaceVersion();  // You should always have this line.
		
	// These are all arbitrary at present.
	Info -> Version = 1;
	Info -> ID      = GetID();
	Info -> TextID  = _R(IDS_GRID_TOOL);

	Info -> Family  = FamilyName;
	Info -> Name    = ToolName;
	Info -> Purpose = Purpose;
	Info -> Author  = Author;

	Info -> InfoBarDialog = _R(IDD_GRIDTOOL);
	Info -> BubbleID = _R(IDBBL_GRID_TOOLBOX);
}

UINT32 GridTool::GetID()
{
	return TOOLID_GRID;
}


// The Rectangle Tools EventHandlers



/********************************************************************************************

>	virtual void GridTool::SelectChange(BOOL isSelected)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/94
	Inputs:		isSelected	- TRUE  = tool has been selected
							- FALSE = tool has been deselected
	Outputs:	-
	Returns:	-
	Purpose:	Creates/destroys/pushes/pops the rectangle tool's cursor.
	Errors:		Debug warning if creating the cursor fails.
	SeeAlso:	-

********************************************************************************************/

void GridTool::SelectChange(BOOL isSelected)
{
	if (isSelected)
	{
		// Make sure my idea of the current document is correct
		GridTool::SetDoc(Document::GetCurrent());

		if (!CreateCursors()) return;

		CurrentCursorID = CursorStack::GPush(pcNormalGridCursor, FALSE);		// Push cursor but don't display now
		pcCurrentCursor = pcNormalGridCursor;

		// Make all grids visible
		if (DocView::GetSelected() != NULL)
		{
			if (!DocView::GetSelected()->GetShowGridState())
				NodeGrid::ForceRedrawAllGrids();	// Redraw all grids
			else
				RenderAllGridBlobs();				// Just EOR their blobs on
		}

		GridTool::FindCurrentSpread();
		//CurrentTool = TRUE;

		// Create and display the tool's info bar
		pGridInfoBarOp->Create();


		OpDescriptor* pOpDesc;

		// Find the pre-registered OpDescs that we're interested in and
		// alias them with an alternative op and GetState
		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_DELETE);
		if (pOpDesc != NULL) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpGridDelete),OpGridDelete::GetState);

		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CUT);
		if (pOpDesc != NULL) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpGridCut),NoClipboardOpGetState);

		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_COPY);
		if (pOpDesc != NULL) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpGridCopy),NoClipboardOpGetState);

		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_PASTE);
		if (pOpDesc != NULL) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpGridPaste),NoClipboardOpGetState);

		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_DUPLICATE);
		if (pOpDesc != NULL) pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpGridDuplicate),OpGridDuplicate::GetState);
	}
	else
	{
		// Deselection - destroy the tool's cursors, if they exist.
		if (pcCurrentCursor != NULL)
		{
			CursorStack::GPop(CurrentCursorID);
			delete pcNormalGridCursor;
			delete pcOverGridCursor;
			delete pcOverBlobCursorUD;
			delete pcOverBlobCursorLR;
			delete pcOverBlobCursorTL;
			delete pcOverBlobCursorTR;
			pcCurrentCursor = NULL;
			CurrentCursorID = 0;
		}
		// EOR off all grid blobs before exiting
		if (DocView::GetSelected() != NULL)
		{
			if (!DocView::GetSelected()->GetShowGridState())
				NodeGrid::ForceRedrawAllGrids();
			else
				RenderAllGridBlobs();
		}

		// Remove the info bar from view by deleting the actual underlying window
		pGridInfoBarOp->Delete();
		//CurrentTool = FALSE;
		SpreadClicked = NULL;
	}
}

/********************************************************************************************

>	static void GridTool::SetDoc(Document* pThisDoc)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/7/94
	Inputs:		pThisDoc = ptr to a document.  This should be the current doc.
	Outputs:	-
	Returns:	-
	Purpose:	Updates the grid tool's idea of what is the relevant doc.
				If the doc is different to the last one we had, then make sure the default
				units are correct for this doc, and init the controls in the info bar
				to take into account the new doc.
				
				This function relies on being called properly when the current doc changes.
				This is done via the grid tool infobar message handler
	SeeAlso:	-

********************************************************************************************/

void GridTool::SetDoc(Document* pThisDoc)
{
	if (GridTool::pDocument != pThisDoc)
	{
		GridTool::pDocument = pThisDoc;
		if (GridTool::pDocument != NULL)
			NodeGrid::SetDefaultUnits(GridTool::pDocument->GetDocUnitList()->GetPageUnits());
		else
			NodeGrid::SetDefaultUnits(NOTYPE);

		GridTool::FindCurrentSpread();
		pGridInfoBarOp->InitControls();
		pGridInfoBarOp->DisplayDefaultGridInfo();
	}
}


/********************************************************************************************

>	BOOL GridTool::CreateCursors()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if all the grid tool cursors have been successfully created
	Purpose:	Creates all the grid tool cursors
	SeeAlso:	-

********************************************************************************************/

BOOL GridTool::CreateCursors()
{
	// This tool has just been selected.  Create the cursors.
	pcNormalGridCursor 	= new Cursor(this, _R(IDC_GRIDTOOLCURSOR));
	pcOverGridCursor 	= new Cursor(this, _R(IDC_MOVEBEZIERCURSOR));
	pcOverBlobCursorUD 	= new Cursor(_R(IDC_SIZENS));
	pcOverBlobCursorLR 	= new Cursor(_R(IDC_SIZEWE));
	pcOverBlobCursorTL 	= new Cursor(_R(IDC_SIZENWSE));
	pcOverBlobCursorTR 	= new Cursor(_R(IDC_SIZENESW));

	if ( pcNormalGridCursor==NULL 		||	!pcNormalGridCursor->IsValid() 	||
	   	 pcOverGridCursor==NULL   		||	!pcOverGridCursor->IsValid()	||
	   	 pcOverBlobCursorUD==NULL   	||	!pcOverBlobCursorUD->IsValid()  ||
	   	 pcOverBlobCursorLR==NULL   	||	!pcOverBlobCursorLR->IsValid()  ||
	   	 pcOverBlobCursorTL==NULL   	||	!pcOverBlobCursorTL->IsValid()  ||
	   	 pcOverBlobCursorTR==NULL   	||	!pcOverBlobCursorTR->IsValid())
	{
		if (IsUserName("MarkN")) TRACE( _T("Couldn't create grid tool cursors!\n"));

		if (pcNormalGridCursor != NULL) delete pcNormalGridCursor;
		if (pcOverGridCursor   != NULL) delete pcOverGridCursor;
		if (pcOverBlobCursorUD != NULL) delete pcOverBlobCursorUD;
		if (pcOverBlobCursorLR != NULL) delete pcOverBlobCursorLR;
		if (pcOverBlobCursorTL != NULL) delete pcOverBlobCursorTL;
		if (pcOverBlobCursorTR != NULL) delete pcOverBlobCursorTR;
		return FALSE;
	}
	else
		return TRUE;
}

/********************************************************************************************

>	static void GridTool::FindCurrentSpread()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Tries to find a spread that can be considered the current spread in
				relation to the grid tool
	SeeAlso:	-

********************************************************************************************/

void GridTool::FindCurrentSpread()
{
	Spread* pSpreadWithSelGrids   = NULL;
	Spread* pSpreadWithSelObjects = NULL;

	if (GridTool::GetDoc() != NULL)		// if there's a current doc, rock and roll
	{
		// Now we need to scan all spreads throughout this doc view's doc.

		Chapter* pChapter = Node::FindFirstChapter(GridTool::GetDoc());
	    while (pChapter != NULL && pSpreadWithSelGrids == NULL)
		{
			// For each chapter in the doc...
			Node* pNode = pChapter->FindFirstChild();
			while (pNode != NULL && pSpreadWithSelGrids == NULL)
			{
				// If the child node of the chapter is a spread...
				if (pNode->IsKindOf(CC_RUNTIME_CLASS(Spread)))
				{
					Spread* pSpread = (Spread*)pNode;
					if (HasSpreadSelGrids(pSpread))
						pSpreadWithSelGrids = pSpread;
				}
				pNode = pNode->FindNext();
			}
			pChapter = pChapter->FindNextChapter();
		}

		if (pSpreadWithSelGrids == NULL)
		{
			// Obtain the current selections
			Range Sel(*(GetApplication()->FindSelection()));
	
			// Find the first selected object.
			Node* FirstSelected = Sel.FindFirst();

			// If there isn't one, then there is no spread with sel objects
			// Otherwise get the selected object's parent spread
			if (FirstSelected != NULL)
				pSpreadWithSelObjects = (Spread*)FirstSelected->FindParent(CC_RUNTIME_CLASS(Spread));
	 	}
	}

	if (pSpreadWithSelGrids != NULL)
		SpreadClicked = pSpreadWithSelGrids;
	else
		SpreadClicked = pSpreadWithSelObjects;
}    


		
/********************************************************************************************

>	static BOOL GridTool::HasSpreadSelGrids(Spread* pSpread)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/94
	Inputs:		pSpread  = spread to look at
	Returns:	TRUE if spread contains at least one selected grid
	Purpose:	Tells you the 'selected grids' state of the given spread
	SeeAlso:	-

********************************************************************************************/

BOOL GridTool::HasSpreadSelGrids(Spread* pSpread)
{
	// scan the children of the spread for grids
	Node* pNode = pSpread->FindFirstChild();
	while (pNode != NULL)
	{
		if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeGrid)))
		{
			// we now have a ptr to a grid, so render those blobs
			NodeGrid* pGrid = (NodeGrid*)pNode;
			if (!pGrid->IsDisabled() && pGrid->IsGridSelected())
				return TRUE;
		}
		pNode = pNode->FindNext();
	}
	return FALSE;
}

/********************************************************************************************

>	void GridTool::UpdateInfoBar()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/2/94
	Purpose:	Update the GridTool's controls on the info bar.
	SeeAlso:	GridTool::HandleInfoBarMessage

********************************************************************************************/

//void GridTool::UpdateInfoBar()
//{ 
//	if (IsUserName("MarkN")) TRACE( _T("UpdateInfoBar\n"));
//}



/********************************************************************************************

>	void GridTool::HandleInfoBarMessage(CDlgMessage DlgMsg, CGadgetID Gadget)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/2/94
	Inputs:		DlgMsg - the dialog message type.
				Gadget - ID of the info bar gadget that caused the event.
	Purpose:	Handles UI events from the Zoom Tool's gadgets on the info bar.
	SeeAlso:	GridTool::UpdateInfoBar

********************************************************************************************/

//void GridTool::HandleInfoBarMessage(CDlgMessage DlgMsg, CGadgetID Gadget)
//{
//	if (IsUserName("MarkN")) TRACE( _T("DlgMsg = %ld , Gadget = %ld\n"),DlgMsg,Gadget);

/*	switch (Gadget)
	{

	}
*/
//}

/********************************************************************************************

>	void GridTool::RenderAllGridBlobs()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/94
	Inputs:		-
	Returns:	-
	Purpose:	Shows the grid blobs of all the grids in the current view
	SeeAlso:	

********************************************************************************************/

void GridTool::RenderAllGridBlobs()
{
	DocView* pDocView = DocView::GetCurrent();			// Get ptr to current doc view
	if (pDocView == NULL) return;						// If no current doc, return
    if (pDocView->GetDoc() == NULL) return;				// If no doc attached to current view, return
    Document* pDoc = pDocView->GetDoc();				// get ptr to the doc
	pDoc->SetCurrent();									// Make sure this doc becomes the current doc


	// Now we need to scan all spreads throughout the current doc view's doc.

	Chapter* pChapter = Node::FindFirstChapter(pDoc);
    while (pChapter != NULL)
	{
		// For each chapter in the doc...
		Node* pNode = pChapter->FindFirstChild();
		while (pNode != NULL)
		{
			// If the child node of the chapter is a spread...
			if (pNode->IsKindOf(CC_RUNTIME_CLASS(Spread)))
			{
				Spread* pSpread = (Spread*)pNode;

				DocRect BB = pSpread->GetPasteboardRect();
				pSpread->DocCoordToSpreadCoord(&BB);

				// BB is now the bounding rect of the spread
				// Now render all grids within this spread
				RenderGridBlobsInSpread(pDocView,pSpread,&BB);
			}
			pNode = pNode->FindNext();
		}
		pChapter = pChapter->FindNextChapter();
	}
}    
		
/********************************************************************************************

>	void GridTool::RenderGridBlobsInSpread(DocView *pDocView,Spread* pSpread,DocRect* pDocRect)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		pDocView = ptr to the doc view to render grid blobs in
				pSpread  = spread containing grids to be rendered
				pDocRect = bounding rect of spread defining which area of the spread view needs updating
	Returns:	-
	Purpose:	EORs the blobs of all the grids in the given spread viewed through the given doc view
	SeeAlso:	

********************************************************************************************/

void GridTool::RenderGridBlobsInSpread(DocView* pDocView,Spread* pSpread,DocRect* pDocRect)
{

	// Get a render region on the spread.  We need to render EORd stuff on top of the current view
	RenderRegion* pRender = pDocView->RenderOnTop(pDocRect,pSpread,UnclippedEOR);

	while (pRender != NULL)
	{
		// scan the children of the spread for grids
		Node* pNode = pSpread->FindFirstChild();
		while (pNode != NULL)
		{
			if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeGrid)))
			{
				// we now have a ptr to a grid, so render those blobs
				NodeGrid* pGrid = (NodeGrid*)pNode;
				RenderGridBlobs(pRender,pGrid);
				// if the grid is selected, show this by rendering the select blobs
				if (pGrid->IsGridSelected() && !pGrid->IsDefault())
					RenderGridSelectBlobs(pRender,pGrid);
			}
			pNode = pNode->FindNext();
		}

		// get the next render region
		pRender = pDocView->GetNextOnTop(pDocRect);
	}
}

/********************************************************************************************

>	void GridTool::RenderGridBlobs(RenderRegion* pRender,NodeGrid* pGrid)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		pRender = ptr to a render region
				pGrid	= ptr to the grid node
	Returns:	-
	Purpose:	Renders the blobs (i.e. the bounding rect) of the given grid.
	SeeAlso:	

********************************************************************************************/

void GridTool::RenderGridBlobs(RenderRegion* pRender,NodeGrid* pGrid)
{
	if (!pGrid->IsDefault())
	{
		DocRect Rect = pGrid->GetBoundingRect();

		pRender->SetLineColour(COLOUR_GRID);
		pRender->SetFillColour(COLOUR_TRANS);
		pRender->DrawRect(&Rect);
	}
}

/********************************************************************************************

>	void GridTool::RenderGridBlobs(NodeGrid* pGrid)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		pGrid = ptr to a grid
	Returns:	-
	Purpose:	EORs the bounding rectangle blob of the given grid.
				The routine works out what spread it belongs to and tries to render the blobs
				in the current doc view.
	SeeAlso:	

********************************************************************************************/

void GridTool::RenderGridBlobs(NodeGrid* pGrid)
{
	DocView* pDocView;
	Spread* pSpread;
	DocRect Rect;
	
	// Get the current doc view, the parent spread of the grid, and the spread bounds
	if (!GetGridBlobRenderVars(pGrid,&pDocView,&pSpread,&Rect))	return;

//	DocRect BB = pGrid->GetBoundingRect();   // get local copy of grid bounds

	// Start a 'render on top' loop so we can EOR our select blobs on
	RenderRegion* pRender = pDocView->RenderOnTop(&Rect,pSpread,UnclippedEOR);

	// Render grid EOR select blobs for each render region
	while (pRender != NULL)
	{
		RenderGridBlobs(pRender,pGrid);
		pRender = pDocView->GetNextOnTop(&Rect);
	}
}

/********************************************************************************************

>	void GridTool::RenderGridSelectBlobs(RenderRegion* pRender,NodeGrid* pGrid)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		pRender = ptr to a render region
				pGrid	= ptr to a grid node
	Returns:	-
	Purpose:	Shows the selection blobs of the given grid in the given render region
	SeeAlso:	

********************************************************************************************/

void GridTool::RenderGridSelectBlobs(RenderRegion* pRender,NodeGrid* pGrid)
{
	if (!pGrid->IsDefault())
	{
		// Make sure the line and fill colours are correct
	    pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

		for(INT32 i = BottomLeft;i < NumGridBlobTypes;i++)
			pRender->DrawBlob(pGrid->GetBlobCoord((GridBlobType)i),BT_UNSELECTED);
	}
}

/********************************************************************************************

>	void GridTool::RenderGridSelectBlobs(NodeGrid* pGrid)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		pGrid = ptr to a grid
	Returns:	-
	Purpose:	Shows the selection blobs of the given grid.
				The routine works out what spread it belongs to and tries to render the blobs
				in the current doc view.
	SeeAlso:	

********************************************************************************************/

void GridTool::RenderGridSelectBlobs(NodeGrid* pGrid)
{
	DocView* pDocView;
	Spread* pSpread;
	DocRect Rect;
	
	// Get the current doc view, the parent spread of the grid, and the spread bounds
	if (!GetGridBlobRenderVars(pGrid,&pDocView,&pSpread,&Rect))	return;

//	DocRect BB = pGrid->GetBoundingRect();   // get local copy of grid bounds

	// Start a 'render on top' loop so we can EOR our select blobs on
	RenderRegion* pRender = pDocView->RenderOnTop(&Rect,pSpread,UnclippedEOR);

	// Render grid EOR select blobs for each render region
	while (pRender != NULL)
	{
		RenderGridSelectBlobs(pRender,pGrid);
		pRender = pDocView->GetNextOnTop(&Rect);
	}
}

/********************************************************************************************

>	void GridTool::RenderAllGridBlobs(NodeGrid* pGrid)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		pGrid = ptr to a grid
	Returns:	-
	Purpose:	Renders the full compliment of blobs for the given grid.
				This means bounding rect AND select blobs are rendered.
				The routine works out what spread it belongs to and tries to render the blobs
				in the current doc view.
	SeeAlso:	

********************************************************************************************/

void GridTool::RenderAllGridBlobs(NodeGrid* pGrid)
{
	RenderGridBlobs(pGrid);
	RenderGridSelectBlobs(pGrid);
}



/********************************************************************************************

>	BOOL GridTool::GetGridBlobRenderVars(NodeGrid* pGrid,DocView** ppDocView,Spread** ppSpread,DocRect* pDocRect)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94

	Inputs:		pGrid 		= ptr to grid in question
				ppDocView 	= ptr to ptr to a DocView
				ppSpread	= ptr to ptr to a spread
				pDocRect	= ptr to the spread's bounding box

	Outputs:	Puts a ptr to the current doc view into *ppDocView
				Puts the ptr to the parent spread of the grid into *ppSpread
				Puts bounding DocRect of spread into *pDocRect

	Returns:	TRUE  - The vars are good, so go and render
				FALSE - Something was not found, so outputs are invalid

	Purpose:	Gets the vars required to render the blobs of a grid
	SeeAlso:	

********************************************************************************************/

BOOL GridTool::GetGridBlobRenderVars(NodeGrid* pGrid,DocView** ppDocView,Spread** ppSpread,DocRect* pDocRect)
{
	// Get current doc view.  If there isn't one, fail
	*ppDocView = DocView::GetCurrent();
	if (*ppDocView == NULL) return (FALSE);
	
	// Get parent spread. If there isn't one, fail
	*ppSpread = pGrid->FindParentSpread();
	if (*ppSpread == NULL) return (FALSE);

	// Get spread bounds and translate to spread coords
	*pDocRect = (*ppSpread)->GetPasteboardRect();
	(*ppSpread)->DocCoordToSpreadCoord(pDocRect);
	
	return (TRUE);
}




/********************************************************************************************

>	void GridTool::RenderToolBlobs(Spread* pSpread,DocRect* pDocRect)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		pSpread  = ptr to a spread
				pDocRect = ptr to DocRect of spread to render in
	Returns:	-
	Purpose:	Handles the RenderToolBlobs method.
				Renders the tool's blobs into the current doc view.
	SeeAlso:	

********************************************************************************************/

void GridTool::RenderToolBlobs(Spread* pSpread,DocRect* pDocRect)
{
//	if (IsUserName("MarkN")) TRACE( _T("GridTool::RenderToolBlobs\n"));

	// Render into the selected doc view
	DocView* pDocView = DocView::GetSelected();
	if (pDocView != NULL)
		RenderGridBlobsInSpread(pDocView,pSpread,pDocRect);
}

/********************************************************************************************

>	void GridTool::CurrentDocViewChange(Spread* pSpread,DocRect* pRect)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/94
	Inputs:		-
	Returns:	-
	Purpose:	To handle a CurrentDocViewChange event for the Grid Tool.
	SeeAlso:	-

********************************************************************************************/

//void GridTool::CurrentDocViewChange()
//{
//	if (IsUserName("MarkN")) TRACE( _T("GridTool::CurrentDocViewChange\n"));
//}

/********************************************************************************************

>	void GridTool::SelectedDocViewChange(Spread* pSpread,DocRect* pRect)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/94
	Inputs:		-
	Returns:	-
	Purpose:	To handle a SelectedDocViewChange event for the Grid Tool.
	SeeAlso:	-

********************************************************************************************/

//void GridTool::SelectedDocViewChange()
//{
//	if (IsUserName("MarkN")) TRACE( _T("GridTool::SelectedDocViewChange\n"));
//}

/********************************************************************************************

>	static void GridTool::ForceRedraw(Spread* pSpread,DocRect Rect)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/94
	Inputs:		pPSpread = ptr to a spread, dude!
				Rect - The region that needs the blobs to be drawn
	Purpose:	Forces a blob-inclusive redraw on the rect given in Spread.
				If the Rect is empty or invalid, no redraw is forced
				The redraw happens in the current view (if there is one).

********************************************************************************************/

void GridTool::ForceRedraw(Spread* pSpread,DocRect Rect)
{
	// Is it a wank spread? Then stick it where the sun don't shine!
	if (pSpread == NULL) return;

	// No selected view? Then no dice!
	DocView* pDocView = DocView::GetSelected();
	if (pDocView == NULL) return;

	// Bad rect?
	if (Rect.IsEmpty() || !Rect.IsValid()) return;

	DocRect BlobSize;	// Will old the size of a scaled blob

	// Add the size of a blob to the top corner of the bounding rectangle
	OSRenderRegion::GetBlobRect(pDocView->GetViewScale(), Rect.HighCorner(), 
								BT_SELECTEDLARGEST, &BlobSize);
	Rect.IncludePoint( BlobSize.HighCorner() );

	// add the size of a blob to the bottom corner of the bounding rectangle
	OSRenderRegion::GetBlobRect(pDocView->GetViewScale(), Rect.LowCorner(), 
								BT_SELECTEDLARGEST, &BlobSize);
	Rect.IncludePoint( BlobSize.LowCorner() );


	// Uncomment the next two lines for total spread area redraw
	//Rect = pSpread->GetPasteboardRect();
	//pSpread->DocCoordToSpreadCoord(&Rect);


	// redraw it 
	pDocView->ForceRedraw(pSpread,Rect);
}

/********************************************************************************************

>	static void GridTool::ForceRedraw(NodeGrid* pGrid)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/94
	Inputs:		pGrid = ptr to grid to force a redraw on
	Purpose:	Forces a blob-inclusive redraw on the the given grid.
				If the Rect is empty or invalid, no redraw is forced
				The redraw happens in the current view (if there is one).

********************************************************************************************/

void GridTool::ForceRedraw(NodeGrid* pGrid)
{
	ForceRedraw(pGrid->FindParentSpread(),pGrid->GetBlobBoundingRect());
}


/********************************************************************************************

>	BOOL GridTool::WhichGridClicked(Spread* pSpread,const DocCoord& PointerPos,NodeGrid** ppGrid)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		pSpread    = ptr to a spread
				PointerPos = pointer pos in spread coords
	Outputs:	*ppGrid	   - see 'Returns'
	Returns:	TRUE  - *ppGrid = ptr to the NodeGrid which contains the point
				FALSE - *ppGrid = NULL
	Purpose:	Scans the grids in the spread to see which one was clicked in (if any)
				Disabled grids are ignored.
	SeeAlso:	

********************************************************************************************/

BOOL GridTool::WhichGridClicked(Spread* pSpread,const DocCoord& PointerPos,NodeGrid** ppGrid)
{
	// Init reference vars
	*ppGrid = NULL;

	// Start from the last node in the spread, so top grid is checked before the one behind it, etc
	Node* pNode = pSpread->FindLastChild();
	while ((pNode != NULL) && (*ppGrid == NULL))
	{
		if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeGrid)))
		{
			NodeGrid* pGrid = (NodeGrid*)pNode;
			
			if (!pGrid->IsDisabled())
			{
				if (pGrid->GetBoundingRect().ContainsCoord(PointerPos))
					*ppGrid = pGrid;
			}
		}
		// Scan back along the sibling list
		pNode = pNode->FindPrevious();
	}
	// Return whether a grid was found or not
	return (*ppGrid != NULL);
}

/********************************************************************************************

>	BOOL GridTool::WhichGridBlobClicked(Spread* pSpread,const DocCoord& PointerPos,
										NodeGrid** ppGrid,GridBlobType* pGridBlob)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/94
	Inputs:		pSpread    = ptr to a spread
				PointerPos = pointer pos in spread coords
	Outputs:	*ppGrid	   - see 'Returns'
				*pGridBlob - see 'Returns'
	Returns:	TRUE  - *ppGrid    = ptr to the NodeGrid which contains the point
						*pGridBlob = the grid blob that was clicked
				FALSE - *ppGrid    = NULL
						*pGridBlob = NoGridBlob
	Purpose:	Scans the grids in the spread to see which grid select blob was clicked (if any)
	SeeAlso:	

********************************************************************************************/

BOOL GridTool::WhichGridBlobClicked(Spread* pSpread,const DocCoord& PointerPos,
										NodeGrid** ppGrid,GridBlobType* pGridBlob)
{
	// Init reference vars
	*ppGrid     = NULL;
	*pGridBlob = NoGridBlob;

	DocView *pDocView = DocView::GetCurrent();
	if (pDocView==NULL)	return FALSE;

	// Start from the last node in the spread, so top grid is checked before the one behind it, etc
	Node* pNode = pSpread->FindLastChild();
	while ((pNode != NULL) && (*ppGrid == NULL))
	{
		if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeGrid)))
		{
			NodeGrid* pGrid = (NodeGrid*)pNode;
			
			if (pGrid->IsGridSelected() && !pGrid->IsDisabled())
			{
				DocRect CornerBlob;
				DocCoord BlobCoord;

				for (INT32 i=BottomLeft;(i<NumGridBlobTypes) && (*ppGrid == NULL);i++)
				{
					BlobCoord = pGrid->GetBlobCoord((GridBlobType)i);
					OSRenderRegion::GetBlobRect(pDocView->GetViewScale(),BlobCoord,BT_SELECTEDLARGEST,&CornerBlob);

					// Check for collision with the control points
					if (CornerBlob.ContainsCoord(PointerPos))
					{
						*ppGrid = pGrid;
						*pGridBlob = (GridBlobType)i;
					}
				}
			}
		}
		// Scan back along the sibling list
		pNode = pNode->FindPrevious();
	}
	// Return whether a grid was found or not
	return (*ppGrid != NULL);
}

/********************************************************************************************

>	static void GridTool::ClearSelection(Spread* pSpread)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		pSpread = ptr to a spread (can cope with pSpread == NULL)
	Returns:	-
	Purpose:	Scans the grids in the spread and clears the selection bit.
				If the selection bit of a grid is set from TRUE to FALSE, the selection blobs
				of that grid are also updated
	SeeAlso:	

********************************************************************************************/

void GridTool::ClearSelection(Spread* pSpread)
{
	// A NULL spread is a possibility
	if (pSpread == NULL) return;

	Node* pNode = pSpread->FindLastChild();
	while (pNode != NULL)
	{
		if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeGrid)))
		{
			NodeGrid* pGrid = (NodeGrid*)pNode;
			
			if (pGrid->IsGridSelected())
			{
				RenderGridSelectBlobs(pGrid);
				pGrid->SetGridSelected(FALSE);
			}
		}
		pNode = pNode->FindPrevious();
	}
}

/********************************************************************************************

>	void GridTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/94
	Inputs:		PointerPos 	- 	The DocCoord of the point where the mouse button was clicked
				Click 		-	Describes the type of click that was detected. 
				ClickMods 	-	Indicates which buttons caused the click and which modifers were
								pressed at the same time
				pSpread 	- 	The spread in which the click happened
	Returns:	-
	Purpose:	To handle a Mouse Click event for the Grid Tool.
				It handles grid creation, selection and translation
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers

********************************************************************************************/

void GridTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )
{
	if (ClickMods.Menu) return;							// Don't do anything if the user clicked the Menu button

	ENSURE(pSpread != NULL,"GridTool::OnClick pSpread == NULL!!!!!");

	// Make sure this click is one that we want
	if ( Click == CLICKTYPE_SINGLE || Click == CLICKTYPE_DOUBLE)
	{
		// If click appears on a different spread than the last click, clear
		// the selection from the previous spread
		if (SpreadClicked != pSpread)	ClearSelection(SpreadClicked);

		if (SpreadClicked == NULL && pSpread != NULL)
			pGridInfoBarOp->EnableGadget(_R(IDC_MAKEGRID),TRUE);

		// make a note of the click pos and spread
		PointClicked  = PointerPos;
		SpreadClicked = pSpread;

		// Get the grid and blob clicked on...
		// The following two functions work out which grid was clicked on, and if a grid
		// was clicked on, which blob (if any) was clicked
		// NOTE : The result of these function calls are stored in GridClicked & GridBlobClicked

		if (!WhichGridBlobClicked(SpreadClicked,PointClicked,&GridClicked,&GridBlobClicked))
			WhichGridClicked(SpreadClicked,PointClicked,&GridClicked);


		// The following decides how the click will be processed

		if (GridClicked != NULL)
		{
			// Display grid info in the info bar
			ENSURE(pGridInfoBarOp != NULL,"GridTool::OnClick - pGridInfoBarOp == NULL!");
			pGridInfoBarOp->DisplayGridInfo(GridClicked);
	
			if (!GridClicked->IsDefault())
			{
				if (GridBlobClicked == NoGridBlob)
				{
					if (GridClicked->IsGridSelected() && !ClickMods.Adjust)
					{
						// A left click on a selected grid starts a drag on the selection
					}
					else
					{
						// if clicked with right mouse button, toggle grid's select state
						if (ClickMods.Adjust)
							GridClicked->SetGridSelected(!GridClicked->IsGridSelected());
						else
						{
							// Otherwise clear selection, then set clicked grid to be selected
							ClearSelection(SpreadClicked);
							GridClicked->SetGridSelected(TRUE);
						}
	
						// Render the grid select blobs to reflect the change in its select state
						RenderGridSelectBlobs(GridClicked);
						NodeGrid::RecalcNumSelectedGrids(SpreadClicked);
						DialogBarOp::SetSystemStateChanged();
					}
				}
			}
			else
			{
				ClearSelection(SpreadClicked);
				GridClicked->SetGridSelected(TRUE);
				GridBlobClicked = NoGridBlob;
				GridClicked 	= NULL;
				NodeGrid::RecalcNumSelectedGrids(SpreadClicked);
				DialogBarOp::SetSystemStateChanged();
			}
		}
		else
		{
			// if no grid clicked on, clear the selection
			ClearSelection(SpreadClicked);
			NodeGrid::RecalcNumSelectedGrids(SpreadClicked);
			DialogBarOp::SetSystemStateChanged();
		}

		// Make sure pointer shape and status bar text are correct
		OnMouseMove(PointerPos, pSpread, ClickMods);

		// Make sure the controls are shaded correctly
		pGridInfoBarOp->EnableControls();
	}

	if ( Click == CLICKTYPE_DRAG )
	{
		if ((GridClicked != NULL) && (GridBlobClicked == NoGridBlob))
		{
			// Is it a drag on a selected grid using the left mouse button?
			if (GridClicked->IsGridSelected() && !ClickMods.Adjust)
			{
				// Drag the selection around
				OpGridSelection* pOpGridSelection = new OpGridSelection;
				if (pOpGridSelection != NULL)
				{
					// Start the drag operation and pass in the Anchor Point to the push operation
					pOpGridSelection->DoDrag(SpreadClicked,PointClicked,GridClicked);
				}
			}
		}
		else
		{
			// Create/Resize a grid
			OpGridNewResize* pOpGridNewResize = new OpGridNewResize;
			if (pOpGridNewResize != NULL)
			{
				if (GridBlobClicked == NoGridBlob) 		// if no blob clicked, we're creating a new grid
					ClearSelection(SpreadClicked);		// so clear selection

				// Start the drag operation
				OpGridNewResize::StartADrag = TRUE;
				OpGridNewResize::EndTheDrag = TRUE;
				pOpGridNewResize->DoDrag(SpreadClicked,GridClicked,GridBlobClicked,PointClicked);
			}
		}
	}
}

/********************************************************************************************

>	void GridTool::OnMouseMove( DocCoord PointerPos,Spread* pSpread, ClickModifiers mods )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	
	Inputs:		PointerPos 	- 	The DocCoord of the point where the mouse has moved to
				pSpread 	- 	The spread in which the move occurred
				mods		-	which buttons/keys are down
	Returns:	TRUE if it handled the Click, FALSE otherwise
	Purpose:	To handle a Mouse Move event for the Grid Tool.
				It changes the cursor dependant on were it is in relation to grids in the spread
				The status bar is filled with a relevent piece of text
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers

********************************************************************************************/

void GridTool::OnMouseMove(DocCoord PointerPos,Spread* pSpread, ClickModifiers mods)
{
//	if (IsUserName("MarkN")) TRACE( _T("GridTool::OnMouseMove\n"));

	NodeGrid* 		pGrid;
	GridBlobType 	GridBlob;
	Cursor*			pCursor=NULL;
	UINT32			StatusID = _R(IDS_NOGRID);

	// Is pointer over a grid blob?
	if (WhichGridBlobClicked(pSpread,PointerPos,&pGrid,&GridBlob))
	{
		switch (GridBlob)
		{
			case BottomLeft		:	pCursor  = pcOverBlobCursorTR;
									StatusID = _R(IDS_BOTTOMLEFTHANDLE);
									break;
			case TopRight  		:	pCursor  = pcOverBlobCursorTR;
									StatusID = _R(IDS_TOPRIGHTHANDLE);
									break;
			case TopLeft		:	pCursor  = pcOverBlobCursorTL;
									StatusID = _R(IDS_TOPLEFTHANDLE);
									break;
			case BottomRight	:	pCursor  = pcOverBlobCursorTL;
									StatusID = _R(IDS_BOTTOMRIGHTHANDLE);
									break;
			case LeftMiddle		:	pCursor  = pcOverBlobCursorLR;
									StatusID = _R(IDS_LEFTHANDLE);
									break;
			case RightMiddle	:	pCursor  = pcOverBlobCursorLR;
	 								StatusID = _R(IDS_RIGHTHANDLE);
									break;
			case TopMiddle		:	pCursor  = pcOverBlobCursorUD;
									StatusID = _R(IDS_TOPHANDLE);
									break;
			case BottomMiddle	:	pCursor  = pcOverBlobCursorUD;
	 								StatusID = _R(IDS_BOTTOMHANDLE);
									break;
		}
	}
	else
	{
		// If not, see if it is over a grid
		if (WhichGridClicked(pSpread,PointerPos,&pGrid))
		{
			if (pGrid->IsDefault())
			{
				pCursor = pcNormalGridCursor;
				if (pGrid->IsGridSelected())
					StatusID = _R(IDS_SELDEFAULT);
				else
					StatusID = _R(IDS_UNSELDEFAULT);
			}
			else
			{
				pCursor = pcOverGridCursor;
				if (pGrid->IsGridSelected())
					StatusID = _R(IDS_SELGRID);
				else
					StatusID = _R(IDS_UNSELGRID);
			}
		}
		else
		{
			pCursor = pcNormalGridCursor;
			StatusID = _R(IDS_NOGRID);
		}
	}

	if (pCursor != pcCurrentCursor)
	{
		// We're using the wrong shape!! 
		pcCurrentCursor = pCursor;
		CursorStack::GSetTop(pcCurrentCursor, CurrentCursorID);
	}

	GridTool::DisplayStatusBarHelp(StatusID);
}

/********************************************************************************************

>	static void GridTool::ToggleDefaultGrid()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/94
	Inputs:		pSpread = spread in which to make the Default grid
	Outputs:	-
	Returns:	-
	Purpose:	Creates a grid covering the whole spread.  The grid is
				defined using the default parameters.
	SeeAlso:	-

********************************************************************************************/

void GridTool::ToggleDefaultGrid()
{
	if (SpreadClicked == NULL) return;

	// scan the children of the current spread for grids
	Node* pNode = SpreadClicked->FindFirstChild();
	while (pNode != NULL)
	{
		if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeGrid)))
		{
			// we now have a ptr to a grid, so render those blobs
			NodeGrid* pGrid = (NodeGrid*)pNode;
			if (pGrid->IsDefault())
			{
				if (pGrid->IsDisabled())
				{
					pGrid->SetDisabled(FALSE);
					ForceRedraw(pGrid);
					ClearSelection(SpreadClicked);
					pGrid->SetGridSelected(TRUE);
				}
				else
				{
					ForceRedraw(pGrid);
					pGrid->SetDisabled(TRUE);
				}
			}
		}
		pNode = pNode->FindNext();
	}
}

/********************************************************************************************

>	static void GridTool::DisplayStatusBarHelp(UINT32 StatusID)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/94
	Inputs:		StatusID = ID of status help string
	Outputs:	-
	Returns:	-
	Purpose:	Displays the given status help string in the status bar
	SeeAlso:	-

********************************************************************************************/

void GridTool::DisplayStatusBarHelp(UINT32 StatusID)
{
//	if (LastStatusID != StatusID)
//	{
		String_256 StatusMsg("");
		StatusMsg.Load(StatusID);
		GetApplication()->UpdateStatusBarText(&StatusMsg);
		LastStatusID = StatusID;
//	}
}


/********************************************************************************************

>	static void GridTool::DisplayGridInfo(NodeGrid* pGrid)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/94
	Inputs:		pGrid = ptr to grid to display in
	Outputs:	-
	Returns:	-
	Purpose:	Displays the given grid's info in the infobar
	SeeAlso:	-

********************************************************************************************/

void GridTool::DisplayGridInfo(NodeGrid* pGrid)
{
	pGridInfoBarOp->DisplayGridInfo(pGrid);
}


/********************************************************************************************

>	NodeGrid* OpGrid::DoDuplicateGrid(NodeGrid* pGrid)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/94
	Inputs:		pGrid,
				AttDir,
				pContextNode,
				pSrcSpread,
				pDestSpread,
				FRedraw,
				XDelta,
				YDelta
	Outputs:	-
	Returns:	ptr the new grid which is a duplicate of the given grid
				Returns NULL if unable to duplicate the node
	Purpose:	Duplicates the given grid and sticks it in a document node tree
				Should be called from an operation's Do function as undo information is
				generated.
	SeeAlso:	-

********************************************************************************************/

NodeGrid* OpGrid::DoDuplicateGrid(	NodeGrid* pGrid,
										AttachNodeDirection AttDir,
										Node* 	pContextNode,
										Spread* pSrcSpread,
										Spread* pDestSpread,
										BOOL	FRedraw,
										INT32	XDelta,
										INT32	YDelta)
{
	BOOL 		ok=FALSE;
	NodeGrid* 	pNewGrid;
	DocRect		OldRect,NewRect;

	OldRect = pGrid->GetBoundingRect();
	NewRect = OldRect;
	NewRect.Translate(XDelta,YDelta);

	switch (pGrid->GetGridType())
	{
		case RECTANGULAR 	: ALLOC_WITH_FAIL(pNewGrid,(new NodeGridRect()),this); break;
		case ISOMETRIC 		: ALLOC_WITH_FAIL(pNewGrid,(new NodeGridIso()), this); break;
	}

	ok = (pNewGrid != NULL);
	if (ok)	ok = DoInsertNewNode(pNewGrid,pContextNode,AttDir,TRUE);
	if (ok)	ok = DoInvalidateNodeRegion(pNewGrid, TRUE);
	if (ok)	ok = DoInvalidateNodeRegion(pGrid,    TRUE);

	if (ok)
	{
		pNewGrid->SetGridParams(pGrid->GetDivisions(),pGrid->GetSubdivisions(),pGrid->GetUnits());
		pNewGrid->SetGridSelected(pGrid->IsGridSelected());
		NodeGrid::RecalcNumSelectedGrids(pDestSpread);

		if (FRedraw)
		{
			GridTool::ForceRedraw(pSrcSpread, OldRect);
			GridTool::ForceRedraw(pDestSpread,NewRect);
		}

		if (!FRedraw) GridTool::RenderAllGridBlobs(pNewGrid);
		pNewGrid->SetBoundingRect(NewRect);
		if (!FRedraw) GridTool::RenderAllGridBlobs(pNewGrid);
	}

	if (!ok) 
		return (NULL); 
	else 
		return (pNewGrid);
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
//  The GridNewResize Operation
//
// This operation is responsible for creating and resizing grids.



/********************************************************************************************

>	OpGridNewResize::OpGridNewResize()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/94
	Purpose:	Constructor. 

********************************************************************************************/

OpGridNewResize::OpGridNewResize()
{
}

/********************************************************************************************

>	OpGridNewResize::~OpGridNewResize()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/94
	Purpose:	Destructor.

********************************************************************************************/

OpGridNewResize::~OpGridNewResize()
{
}


/********************************************************************************************

>	void OpGridNewResize::DoDrag(	Spread* pSpread,NodeGrid* pGrid,GridBlobType Blob,
									DocCoord PointClicked)
    
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/94
	Inputs:		pSpread			- The spread that the drag was started on
				pGrid			- The grid clicked on (or NULL if no grid clicked on)
				Blob			- The select blob clicked on (or NoGridBlob if no blob clicked)
				PointClicked 	- starting position of the drag
	Purpose:	Starts dragging from the coordinate passed in.
				This op either created a new grid on the page (if Blob == NoGridBlob) or resizes 
				pGrid using the Blob provided.

********************************************************************************************/

void OpGridNewResize::DoDrag(	Spread* pSpread,NodeGrid* pGrid,GridBlobType Blob,
								DocCoord PointClicked)
{
	DocView::SnapCurrent(pSpread,&PointClicked);

	// We haven't shown a grid in the infobar yet, so set these to NULL
	PreOpDisplayedGrid  = NULL;
	PostOpDisplayedGrid = NULL;

	SpreadClicked	= pSpread;
	GridClicked		= pGrid;
	GridBlobClicked	= Blob;

	if (GridBlobClicked == NoGridBlob)
		// Creating a new grid at this point, so drag and anchor points are the same
		DragPoint = AnchorPoint = PointClicked;
	else
	{
		// Here we are resizing the given grid using the given blob point
		ENSURE(GridClicked != NULL,"Oops! GridClicked == NULL && GridBlobClicked != NoGridBlob");

		// The following works out which points should be used as drag and anchor points
		// It assumes that the four corner points are defined in a specific way. If the GridBlobType
		// definition changes, then this code will have to change
		GridBlobType DragBlob   = (GridBlobType) ((INT32)GridBlobClicked % 4);
		GridBlobType AnchorBlob = (GridBlobType) (((INT32)DragBlob+2) % 4);

		// The anchor point is always the coord of the anchor blob
		AnchorPoint = GridClicked->GetBlobCoord(AnchorBlob);

		if (GridBlobClicked < LeftMiddle)
			// if we have clicked on a corner blob, the drag point is wherever the mouse is
			DragPoint = PointClicked;
		else
		{
			// Otherwise, the drag point has to be constrained either horizontally or vertically
			// depending on which mid point has been clicked
			DragPoint = GridClicked->GetBlobCoord(DragBlob);

			if (((INT32)GridBlobClicked % 2) == 0)
				DragPoint.x = PointClicked.x;
			else
				DragPoint.y = PointClicked.y;
		}
		RenderMyDragBlobs();
	}

	// And tell the Dragging system that we need drags to happen
	if (OpGridNewResize::StartADrag) StartDrag( DRAGTYPE_AUTOSCROLL );
}




/********************************************************************************************

>	void OpGridNewResize::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
								   Spread* pSpread, BOOL bSolidDrag)
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/94
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods  - Which key modifiers are being pressed
				pSpread    - The spread that the mouse pointer is over
	Purpose:	Takes the pointer position and calculates the new dragged outline of the EORd
				bounding box
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpGridNewResize::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
								   Spread* pSpread, BOOL bSolidDrag)
{
//	if (IsUserName("MarkN")) TRACE( _T("\n\nOpGridNewResize::DragPointerMove()\n"));

	// First Rub out the old box
	RenderMyDragBlobs();

	// Update the box and draw in the new one
	if (pSpread != SpreadClicked)
		PointerPos = MakeRelativeToSpread(SpreadClicked, pSpread, PointerPos);

	DocView::SnapCurrent(pSpread,&PointerPos);

	if (GridBlobClicked < LeftMiddle)
	// If dragging a corner blob, the drag point is the same as the mouse position
		DragPoint = PointerPos;
	else
	{
		// Otherwise, constrain the drag point depending on which mid line blob was clicked
		if (((INT32)GridBlobClicked % 2) == 0)
			DragPoint.x = PointerPos.x;
		else
			DragPoint.y = PointerPos.y;
	}

	// Render the new drag box
	RenderMyDragBlobs();
}



/********************************************************************************************

>	void OpGridNewResize::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
								Spread* pSpread, BOOL Success, BOOL bSolidDrag)

    
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/94
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

void OpGridNewResize::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
								Spread* pSpread, BOOL Success, BOOL bSolidDrag)
{
	BOOL ok = Success;			// The OK flag. If this is TRUE, it is safe to continue the operation

	// Put the hour glass up as we have to
	BeginSlowJob();

	if (ok) DocView::SnapCurrent(pSpread,&PointerPos);	// Snap the point

	// Check for an artificially created drag.  If it is artificial, then DragPointerMove
	// has never been called so update DragPoint to be PointerPos.
	if (!OpGridNewResize::EndTheDrag) DragPoint = PointerPos;

	// First Rub out the old box
	RenderMyDragBlobs();

	// Get a ptr to a doc view
	DocView* pDocView = DocView::GetSelected();			// Get ptr to selected doc view
	if (ok) ok = (pDocView != NULL);					// Check we've got a doc view

	UINT32 IDS = 0;

	// Has the user gone anywhere with the drag?
	if (ok) ok = (DragPoint != AnchorPoint);

	NodeGrid* pNewGrid = NULL;				// This will point to the new grid
	NodeGrid* pOldGrid = NULL;				// This will point to the grid being altered (if applicable)

	if (ok)
	{
//	    BOOL FRedraw = pDocView->GetShowGridState(); // FRedraw = TRUE if we need to cause redraws after change
	    BOOL FRedraw = TRUE;
		
		DocRect ScaledBlob;	// Will hold a scaled blob at coord (0,0)
		OSRenderRegion::GetBlobRect(pDocView->GetViewScale(), DocCoord(0,0), BT_SELECTEDLARGEST, &ScaledBlob);

		// This lump of code makes sure that the minimum grid size created is not less than the width of a blob
		MILLIPOINT dx = AnchorPoint.x - DragPoint.x;
		MILLIPOINT dy = AnchorPoint.y - DragPoint.y;
		if (abs(dx) < ScaledBlob.Width())  DragPoint.x = AnchorPoint.x + (ScaledBlob.Width() *sgn(dx));
		if (abs(dy) < ScaledBlob.Height()) DragPoint.y = AnchorPoint.y + (ScaledBlob.Height()*sgn(dy));


		// Build the rectangle of the drag box at the end of the drag
		DocRect Rect(	min(AnchorPoint.x, DragPoint.x),min(AnchorPoint.y, DragPoint.y),
						max(AnchorPoint.x, DragPoint.x),max(AnchorPoint.y, DragPoint.y) );
								  
	
		DocRect OldRect = DocRect(0,0,0,0);		// Old rectangle coords used when resizing GridClicked

		GridType 			CreateGridType;		// The type of grid we need to create
		Node* 				ContextNode;		// Context of the new grid's placing
		AttachNodeDirection AttachDir;			// How the ContextNode is used to place new grid

		if (GridBlobClicked == NoGridBlob) 		// No blob clicked, so we're creating a new grid
		{
			CreateGridType 	= NodeGrid::GetDefaultGridType();
			ContextNode		= SpreadClicked;
			AttachDir		= LASTCHILD;
			OpType			= NEW;
		}
		else									// Blob clicked, so new grid is edited form of old grid
		{
			pOldGrid 		= GridClicked;
			OldRect  		= pOldGrid->GetBoundingRect();
			GridTool::RenderAllGridBlobs(pOldGrid);

			CreateGridType 	= pOldGrid->GetGridType();
			ContextNode		= pOldGrid;
			AttachDir		= NEXT;
			OpType			= RESIZE;
		}

		switch (CreateGridType)					// Create the new grid of the right type
		{
			case RECTANGULAR 	: ALLOC_WITH_FAIL(pNewGrid,(new NodeGridRect()),this); break;
			case ISOMETRIC 		: ALLOC_WITH_FAIL(pNewGrid,(new NodeGridIso()), this); break;
		}

		ok = (pNewGrid != NULL);
		if (ok) 						ok = DoInsertNewNode(pNewGrid,ContextNode,AttachDir,TRUE);
		if (ok) 						ok = DoInvalidateNodeRegion(pNewGrid,TRUE);
		if (ok && (pOldGrid != NULL))	ok = DoInvalidateNodeRegion(pOldGrid,TRUE);
		if (ok && (pOldGrid != NULL)) 	ok = DoHideNode(pOldGrid,TRUE);

		if (ok)
		{
			// Initialise the new grid based on whether we have an old grid or not
			if (pOldGrid == NULL)
			{	
				pNewGrid->SetGridParams(NodeGrid::GetDefaultDivisions(),NodeGrid::GetDefaultSubdivisions(),NodeGrid::GetDefaultUnits());
//				pNewGrid->SetInitialBounds(Rect);
				pNewGrid->SetDefault(FALSE);
				pNewGrid->SetDisabled(FALSE);
				pNewGrid->SetGridSelected(TRUE);
			}
			else
			{
				pNewGrid->SetGridParams(pOldGrid->GetDivisions(),pOldGrid->GetSubdivisions(),pOldGrid->GetUnits());
//				pNewGrid->ChangeBounds(Rect);
				pNewGrid->SetGridSelected(pOldGrid->IsGridSelected());
			}

			NodeGrid::RecalcNumSelectedGrids(SpreadClicked);

			// Tell the NodeGrid of the new bounds
			pNewGrid->SetBoundingRect(Rect);

			// Redraw the old and new positions of the grid. 
			// GridTool::ForceRedraw will do nothing if Rect is empty or invalid

			if (FRedraw)
			{
				GridTool::ForceRedraw(SpreadClicked,Rect);
				GridTool::ForceRedraw(SpreadClicked,OldRect);
			}
			else
		    	GridTool::RenderAllGridBlobs(pNewGrid);

			// Show the new grid's info in the info bar
			GridInfoBarOp* pGridInfoBarOp = GridTool::GetGridInfoBarOp();
			if (pGridInfoBarOp != NULL)
			{
				PreOpDisplayedGrid = pGridInfoBarOp->GetLastGridDisplayed();
				pGridInfoBarOp->EnableControls();
				GridTool::DisplayGridInfo(pNewGrid);
				PostOpDisplayedGrid = pNewGrid;
			}
		}
	}

	if (!ok)
	{
		if (pNewGrid != NULL) delete pNewGrid;
		if (IDS > 0) InformError(IDS,_R(IDS_OK));
    	FailAndExecute();
	}

	// If it's a real drag event, end the drag.
	if (OpGridNewResize::EndTheDrag)
		EndDrag();

	// End the op
	End();
}



/********************************************************************************************

>	virtual void OpGridNewResize::RenderMyDragBlobs()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/94
	Inputs:		Rect 	- The region that needs the blobs to be drawn
				pSpread - The spread that the drawing will happen on
	Purpose:	Draws an EORed rectangle defined by AnchorPoint and DragPoint

********************************************************************************************/

void OpGridNewResize::RenderMyDragBlobs()
{
//	if (IsUserName("MarkN")) TRACE( _T("OpGridNewResize::RenderMyDragBlobs()\n"));

	DocRect Rect = DocRect(	min(AnchorPoint.x, DragPoint.x),min(AnchorPoint.y, DragPoint.y),
							max(AnchorPoint.x, DragPoint.x),max(AnchorPoint.y, DragPoint.y) );

	RenderDragBlobs(Rect,SpreadClicked, FALSE);		
}



/********************************************************************************************

>	void OpGridNewResize::RenderDragBlobs(DocRect Rect,Spread* pSpread, BOOL bSolidDrag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/94
	Inputs:		Rect 	- The region that needs the blobs to be drawn
				pSpread - The spread that the drawing will happen on
	Purpose:	Draws an EORed rectangle defined by AnchorPoint and DragPoint

********************************************************************************************/

void OpGridNewResize::RenderDragBlobs(DocRect Rect,Spread* pSpread, BOOL bSolidDrag)
{
//	if (IsUserName("MarkN")) TRACE( _T("OpGridNewResize::RenderDragBlobs()\n"));

	DocRect RubberBox = DocRect( min(AnchorPoint.x,DragPoint.x),min(AnchorPoint.y,DragPoint.y),
								 max(AnchorPoint.x,DragPoint.x),max(AnchorPoint.y,DragPoint.y) );
		
	RenderRegion* pRegion = DocView::RenderOnTop( &Rect, pSpread, UnclippedEOR );

	while ( pRegion != NULL )
	{
		// Set the line colour and Draw the rect
		pRegion -> SetLineColour( COLOUR_XORDRAG );

		// Draw the rectangle
		pRegion -> DrawDragRect( &RubberBox );

		// Get the Next render region
		pRegion = DocView::GetNextOnTop( &Rect );
	}
}



/********************************************************************************************

>	BOOL OpGridNewResize::Declare()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/94
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpGridNewResize::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_GRID_TOOL),
								CC_RUNTIME_CLASS(OpGridNewResize), 
								OPTOKEN_GRIDNEWRESIZE,
								OpGridNewResize::GetState,
								0,      	/* help ID */
								_R(IDBBL_NOOP),	/* bubble ID */
								0			/* bitmap ID */
								));
}


/********************************************************************************************

>	static OpState OpGridNewResize::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/94
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the push tool
	Returns:	The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpGridNewResize::GetState(String_256* Description, OpDescriptor*)
{
	OpState Grid;
	
	return Grid;
}

/********************************************************************************************

>	virtual void OpGridNewResize::GetOpName(String_256* OpName) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/94
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
					
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpGridNewResize::GetOpName(String_256* OpName) 
{ 
	switch (OpType)
	{
		case NEW:		*OpName = String_256(_R(IDS_GRID_UNDO_NEW));	break;
		case RESIZE:	*OpName = String_256(_R(IDS_GRID_UNDO_RESIZE));	break;
	}
}  


///////////////////////////////////////////////////////////////////////////////////////////
//  The Grid selection Operation



/********************************************************************************************

>	OpGridSelection::OpGridSelection()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/94
	Purpose:	Constructor. 

********************************************************************************************/

OpGridSelection::OpGridSelection()
{
}

/********************************************************************************************

>	OpGridSelection::~OpGridSelection()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/94
	Purpose:	Destructor.

********************************************************************************************/

OpGridSelection::~OpGridSelection()
{
}


/********************************************************************************************

>	DocRect OpGridSelection::GetSelectedGridBounds(Spread* pSpread)
    
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/94
	Inputs:		pSpread	- The spread containing the grid you're interested in
	Outputs:	-
	Returns:	The DocRect which is the union of the bounds of each of the selected grids.
	Purpose:	Goes through all the selected grids in the given spread, returning the union.
				Used to ensure the snapping of selected grids to grids works.

********************************************************************************************/

DocRect OpGridSelection::GetSelectedGridBounds(Spread* pSpread)
{
	DocRect rect;

	Node* pNode = pSpread->FindLastChild();
	Node* pPrevNode;
	while (pNode != NULL)
	{
		pPrevNode = pNode->FindPrevious();

		if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeGrid)))
		{
			NodeGrid* pGrid = (NodeGrid*)pNode;
			
			if (pGrid->IsGridSelected() && !pGrid->IsDefault())
				rect = rect.Union(pGrid->GetBoundingRect());
		}
		pNode = pPrevNode;
	}

	return (rect);
}

/********************************************************************************************

>	void OpGridSelection::DoDrag(Spread* pSpread,DocCoord PointClicked)
    
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/94
	Inputs:		pSpread			- The spread that the drag was started on
				PointClicked 	- starting position of the drag
	Purpose:	Starts dragging from the coordinate passed in.

********************************************************************************************/

void OpGridSelection::DoDrag(Spread* pSpread,DocCoord PointClicked,NodeGrid* GridClicked)
{
	// The last grid shown in the infobar before this op must be GridClicked
	PreOpDisplayedGrid  = GridClicked;
	// The dragged version of GridClicked is yet to be determined
	PostOpDisplayedGrid = NULL;

	SpreadClicked 	= pSpread;
	SpreadDrag		= pSpread;
	StartPoint		= PointClicked;
	DragPoint		= PointClicked;

	NodeGrid::ResetDocRectSnap();

	StartRect = GetSelectedGridBounds(pSpread);
	DragRect  = StartRect;
	//DocView::SnapCurrent(pSpread,&DragRect,StartPoint,DragPoint);

	RenderMyDragBlobs();

	// And tell the Dragging system that we need drags to happen
	StartDrag( DRAGTYPE_AUTOSCROLL );
}




/********************************************************************************************

>	void OpGridSelection::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
								   Spread* pSpread, BOOL bSolidDrag)
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/94
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods  - Which key modifiers are being pressed
				pSpread    - The spread that the mouse pointer is over
	Purpose:	Takes the pointer position of the corner of the rectangle being dragged
				and redisplays the outline rectangle
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpGridSelection::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
								   Spread* pSpread, BOOL bSolidDrag)
{
		// First Rub out the old boxes
		RenderMyDragBlobs();

		DocCoord PrevDragPoint = DragPoint;

		DragPoint  = PointerPos;
		SpreadDrag = pSpread;

		DragRect = StartRect;
		DragRect.Translate(DragPoint.x - StartPoint.x , DragPoint.y - StartPoint.y);
		DocView::SnapCurrent(pSpread,&DragRect,PrevDragPoint,DragPoint);

		RenderMyDragBlobs();
}



/********************************************************************************************

>	void OpGridSelection::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
								Spread* pSpread, BOOL Success, BOOL bSolidDrag)

    
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/94
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickMods - the key modifiers being pressed
				pSpread - The spread that the drag finished on
				Success - TRUE if the drag was terminated properly, FALSE if it
				was ended with the escape key being pressed
	Purpose:	Ends the drag and translates the selected grids. It also ends the
				operation.
				Assumes that DragPointerMove() has been called during the drag
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpGridSelection::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
								Spread* pSpread, BOOL Success, BOOL bSolidDrag)
{
 	BOOL ok = Success;			// The OK flag. If this is TRUE, it is safe to continue the operation

	// Put the hour glass up as we have to
	BeginSlowJob();

	INT32 XDelta = DragRect.lo.x - StartRect.lo.x;
	INT32 YDelta = DragRect.lo.y - StartRect.lo.y;

	BOOL FRedraw = FALSE;
	UINT32 SelCount = 0;
	UINT32 IDS = 0;

	DocView* pDocView = DocView::GetSelected();			// Get ptr to current doc view
	if (pDocView != NULL)
		FRedraw = pDocView->GetShowGridState();		
	
	FRedraw = TRUE;

	// First Rub out the old boxes
	RenderMyDragBlobs();

	Node* pNode = SpreadClicked->FindLastChild();
	Node* pPrevNode;
	while (pNode != NULL && ok)
	{
		pPrevNode = pNode->FindPrevious();

		if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeGrid)))
		{
			NodeGrid* pOldGrid = (NodeGrid*)pNode;
			
			if (pOldGrid->IsGridSelected() && !pOldGrid->IsDefault())
			{
				AttachNodeDirection AttDir;
				Node* 				pContextNode;
				NodeGrid* 			pNewGrid;
				Spread* 			pSrcSpread = SpreadClicked;
				Spread* 			pDestSpread;
				DocRect				OldRect,NewRect;

				SelCount++;

				if (SpreadClicked == SpreadDrag)
				{
					pContextNode = pOldGrid;
					AttDir		 = NEXT;
					pDestSpread	 = SpreadClicked;
				}
				else
				{
					pContextNode = SpreadDrag;
					AttDir		 = LASTCHILD;
					pDestSpread  = SpreadDrag;
				}

				pNewGrid = DoDuplicateGrid(	pOldGrid,AttDir,pContextNode,
											pSrcSpread,pDestSpread,FRedraw,
											XDelta,YDelta);
				ok = (pNewGrid != NULL);
				if (ok)	ok = DoHideNode(pOldGrid,TRUE);
				
				if (ok)
				{
					if (pOldGrid == PreOpDisplayedGrid)
					{
						GridTool::DisplayGridInfo(pNewGrid);
						PostOpDisplayedGrid = pNewGrid;
					}
				}
			}
		}
		pNode = pPrevNode;
	}

	if (!ok)
	{
		if (IDS > 0) InformError( IDS, _R(IDS_OK) );
		FailAndExecute();
	}

	Plural = (SelCount > 1);

	// End the Drag                             
	EndDrag();

	// End the op
	End();
}

/********************************************************************************************

>	virtual void OpGridSelection::RenderMyDragBlobs()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/94
	Inputs:		
	Purpose:	Draws an EORed rectangle for all selected grids in SpreadClicked

********************************************************************************************/

void OpGridSelection::RenderMyDragBlobs()
{
/*
	DocRect SpreadRect = SpreadClicked->GetPasteboardRect();
	SpreadClicked->DocCoordToSpreadCoord(&SpreadRect);

	RenderDragBlobs(SpreadRect,SpreadClicked);
*/
	DocRect SpreadRect = SpreadDrag->GetPasteboardRect();
	SpreadDrag->DocCoordToSpreadCoord(&SpreadRect);

	RenderDragBlobs(SpreadRect,SpreadDrag, FALSE);
}

/********************************************************************************************

>	virtual void OpGridSelection::RenderDragBlobs(DocRect Rect,Spread* pSpread, BOOL bSolidDrag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/94
	Inputs:		Rect = Rect to render drag blobs in
				pSpread = ptr to spread to render drag blobs in
	Purpose:	Draws an EORed rectangle for all selected grids using the given params

********************************************************************************************/

void OpGridSelection::RenderDragBlobs(DocRect Rect,Spread* pSpread, BOOL bSolidDrag)
{
	//MILLIPOINT	dx = DragPoint.x - StartPoint.x;
	//MILLIPOINT	dy = DragPoint.y - StartPoint.y;
	MILLIPOINT	dx = DragRect.lo.x - StartRect.lo.x;
	MILLIPOINT	dy = DragRect.lo.y - StartRect.lo.y;
	DocRect		GridRect;

	RenderRegion* pRegion = DocView::RenderOnTop( NULL/*&Rect*/, pSpread, UnclippedEOR );

	while ( pRegion != NULL )
	{
		// Set the line colour 
		pRegion -> SetLineColour( COLOUR_XORDRAG );

		Node* pNode = SpreadClicked->FindLastChild();
		while (pNode != NULL)
		{
			if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeGrid)))
			{
				NodeGrid* pGrid = (NodeGrid*)pNode;
			
				if (pGrid->IsGridSelected())
				{
					GridRect = pGrid->GetBoundingRect();
					GridRect.Translate(dx,dy);
					pRegion->DrawDragRect(&GridRect);
				}
			}
			pNode = pNode->FindPrevious();
		}

		// Get the Next render region
		pRegion = DocView::GetNextOnTop( &Rect );
	}
}



/********************************************************************************************

>	BOOL OpGridSelection::Declare()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/94
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpGridSelection::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_GRID_TOOL),
								CC_RUNTIME_CLASS(OpGridSelection), 
								OPTOKEN_GRIDSELECTION,
								OpGridSelection::GetState,
								0,			/* help ID */
								_R(IDBBL_NOOP),	/* bubble ID */
								0			/* bitmap ID */
								));
}


/********************************************************************************************

>	static OpState OpGridSelection::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/94
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the push tool
	Returns:	The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpGridSelection::GetState(String_256* Description, OpDescriptor*)
{
	OpState Grid;
	
	return Grid;
}

/********************************************************************************************

>	virtual void OpGridSelection::GetOpName(String_256* OpName) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/3/94
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
					
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpGridSelection::GetOpName(String_256* OpName) 
{ 
	if (Plural)
		*OpName = String_256(_R(IDS_GRID_UNDO_SELECTS));
	else
		*OpName = String_256(_R(IDS_GRID_UNDO_SELECT));
}  


//-----------------------------------------

/********************************************************************************************

>	OpGridChange::OpGridChange()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/94
	Purpose:	Constructor. 

********************************************************************************************/

OpGridChange::OpGridChange()
{
}

/********************************************************************************************

>	OpGridChange::~OpGridChange()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/94
	Purpose:	Destructor.

********************************************************************************************/

OpGridChange::~OpGridChange()
{
}


/********************************************************************************************

>	BOOL OpGridChange::Declare()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/94
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpGridChange::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_GRID_TOOL),
								CC_RUNTIME_CLASS(OpGridChange), 
								OPTOKEN_GRIDCHANGE,
								OpGridChange::GetState,
								0,			/* help ID */
								_R(IDBBL_NOOP),	/* bubble ID */
								0			/* bitmap ID */
								));
}


/********************************************************************************************

>	static OpState OpGridChange::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/94
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the push tool
	Returns:	The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpGridChange::GetState(String_256* Description, OpDescriptor*)
{
	OpState Grid;
	
	return Grid;
}

/********************************************************************************************

>	void OpGridChange::GetOpName(String_256* OpName) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/94
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
					
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpGridChange::GetOpName(String_256* OpName) 
{ 
	*OpName = String_256(ChangeGridUndoIDS);
}  


/********************************************************************************************

>	void OpGridChange::ChangeSelectedGrids(ChangeGrid* pChangeGrid)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/94
	Inputs:		pChGrid = ptr to a ChangeGrid class for changing the selected grids in some way
	Returns:	-
	Purpose:	Changes all the selected grids using the given ChangeGrid object
	SeeAlso:	

********************************************************************************************/

void OpGridChange::DoChangeSelectedGrids(ChangeGrid* pChangeGrid)
{
	// We haven't shown a grid in the infobar yet, so set these to NULL
	PreOpDisplayedGrid  = NULL;
	PostOpDisplayedGrid = NULL;

	Spread* SpreadClicked = GridTool::GetSpreadClicked();

	GridInfoBarOp* pGridInfoBarOp = GridTool::GetGridInfoBarOp();
	if (pGridInfoBarOp != NULL)
		PreOpDisplayedGrid = pGridInfoBarOp->GetLastGridDisplayed();

	BOOL    ok = (SpreadClicked != NULL);
	if (ok) ok = (pChangeGrid   != NULL);

	if (ok)
	{
		// scan the children of the spread for grids
		Node* pNode = SpreadClicked->FindFirstChild();
		while (pNode != NULL && ok)
		{
			Node* pNodeCurrent = pNode;
			pNode = pNode->FindNext();

			if (pNodeCurrent->IsKindOf(CC_RUNTIME_CLASS(NodeGrid)))
			{
				// we now have a ptr to a grid, so render those blobs
				NodeGrid* pGrid = (NodeGrid*)pNodeCurrent;

	 			if (pGrid->IsGridSelected())
				{
					GridTool::ForceRedraw(pGrid);
					ok = DoChangeGrid(pGrid,pChangeGrid);
				}
			}
		}
	}

	if (!ok)
		FailAndExecute();
	else
	{
		ChangeGridUndoIDS = pChangeGrid->GetUndoIDS();
		if (PostOpDisplayedGrid != NULL)
			GridTool::DisplayGridInfo(PostOpDisplayedGrid);
	}

	delete pChangeGrid;

	End();
}

/********************************************************************************************

>	BOOL OpGridChange::DoChangeGrid(NodeGrid* pOldGrid,ChangeGrid* pChangeGrid)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/94
	Inputs:		pOldGrid = ptr to a grid in the tree to change
				pChangeGrid = ptr to a grid changing object
	Returns:	-
	Purpose:	Changes the given grid to reflect the default grid values of the grid tool.
	SeeAlso:	

********************************************************************************************/
 
BOOL OpGridChange::DoChangeGrid(NodeGrid* pOldGrid,ChangeGrid* pChangeGrid)
{
	DocRect DummyRect;
	NodeGrid* pNewGrid;
	BOOL ok = TRUE;
	
	switch (pChangeGrid->GetGridType(pOldGrid))
	{
		case RECTANGULAR 	: ALLOC_WITH_FAIL(pNewGrid,(new NodeGridRect()),this); break;
		case ISOMETRIC 		: ALLOC_WITH_FAIL(pNewGrid,(new NodeGridIso()), this); break;
	}

	ok = (pNewGrid != NULL);
	if (ok) ok = DoInsertNewNode(pNewGrid,pOldGrid,NEXT,TRUE);
	if (ok)	ok = DoInvalidateNodeRegion(pOldGrid, TRUE);
	if (ok) ok = DoHideNode(pOldGrid,TRUE);

	if (ok)
	{
		pNewGrid->SetGridParams(pOldGrid->GetDivisions(),
								pOldGrid->GetSubdivisions(),
								pOldGrid->GetUnits());

		pChangeGrid->Change(pNewGrid);
		PostOpDisplayedGrid = pNewGrid;

		pNewGrid->SetBoundingRect(pOldGrid->GetBoundingRect());
		pNewGrid->SetGridSelected(pOldGrid->IsGridSelected());
		pNewGrid->SetDefault(pOldGrid->IsDefault());
	}

	return (ok);
}

//----------------------------------------
//----------------------------------------
//----------------------------------------

/********************************************************************************************

>	void ChangeGridUnits::Change(NodeGrid* pGrid)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/94
	Inputs:		pGrid = ptr to a grid in the tree
	Returns:	-
	Purpose:	Changes the given grid's units to the default setting.
	SeeAlso:	

********************************************************************************************/

void ChangeGridUnits::Change(NodeGrid* pGrid)
{
	pGrid->SetGridParams(	pGrid->GetDivisions(),
							pGrid->GetSubdivisions(),
							NodeGrid::GetDefaultUnits());
}

GridType ChangeGridUnits::GetGridType(NodeGrid* pGrid) { return pGrid->GetGridType(); }

UINT32 ChangeGridUnits::GetUndoIDS() { return _R(IDS_GRID_UNDO_UNITS); }

/********************************************************************************************

>	void ChangeGridDivisions::Change(NodeGrid* pGrid)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/94
	Inputs:		pGrid = ptr to a grid in the tree
	Returns:	-
	Purpose:	Changes the given grid's divisions to the default setting.
	SeeAlso:	

********************************************************************************************/

void ChangeGridDivisions::Change(NodeGrid* pGrid)
{
	pGrid->SetGridParams(	NodeGrid::GetDefaultDivisions(),
							pGrid->GetSubdivisions(),
							pGrid->GetUnits());
}

GridType ChangeGridDivisions::GetGridType(NodeGrid* pGrid) { return pGrid->GetGridType(); }

UINT32 ChangeGridDivisions::GetUndoIDS() { return _R(IDS_GRID_UNDO_DIVS); }

/********************************************************************************************

>	void ChangeGridSubdivisions::Change(NodeGrid* pGrid)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/94
	Inputs:		pGrid = ptr to a grid in the tree
	Returns:	-
	Purpose:	Changes the given grid's subdivisions to the default setting.
	SeeAlso:	

********************************************************************************************/

void ChangeGridSubdivisions::Change(NodeGrid* pGrid)
{
	pGrid->SetGridParams(	pGrid->GetDivisions(),
							NodeGrid::GetDefaultSubdivisions(),
							pGrid->GetUnits());
}

GridType ChangeGridSubdivisions::GetGridType(NodeGrid* pGrid) { return pGrid->GetGridType(); }

UINT32 ChangeGridSubdivisions::GetUndoIDS() { return _R(IDS_GRID_UNDO_SUBDIVS); }

/********************************************************************************************

>	void ChangeGridType::Change(NodeGrid* pGrid)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/94
	Inputs:		pGrid = ptr to a grid in the tree
	Returns:	-
	Purpose:	This does nothing as changing the type of grid doesn't change the other params
	SeeAlso:	

********************************************************************************************/

void ChangeGridType::Change(NodeGrid* pGrid)
{
}

GridType ChangeGridType::GetGridType(NodeGrid* pGrid) { return NodeGrid::GetDefaultGridType(); }

UINT32 ChangeGridType::GetUndoIDS() { return _R(IDS_GRID_UNDO_TYPE); }

//-----------------------------------------




//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------

/********************************************************************************************

>	MsgResult GridInfoBarOp::Message(Msg* Message) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>  / Converted to new Message handler by SimonM
	Created:	28/4/94
	Inputs:		Message: The message to handle
	Outputs:	-
	Returns:	-
	Purpose:	Grid info bar dialog message handler
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult GridInfoBarOp::Message(Msg* Message) 
{
//	if (IsUserName("MarkN")) TRACE( _T("GridInfoBarOp::Message()\n"));

	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		// Check if the message is a CANCEL
		if (Msg->DlgMsg == DIM_CANCEL)
		{
			Close(); // Close the dialog 
		}
		else if (Msg->DlgMsg == DIM_CREATE)
		{
			// Initialise the controls 
			InitControls();
			DisplayDefaultGridInfo();
		}
		else
		{
			ChangeGrid* pChangeGrid = NULL;

			//if (IsUserName("MarkN")) TRACE( _T("Msg->GadgetID = %ld\n"),Msg->GadgetID);

			switch (Msg->GadgetID)
			{
				case _R(IDC_UNITS):
				{
					switch (Msg->DlgMsg)
					{
						case DIM_SELECTION_CHANGED :
						{
							WORD Index;
							GetValueIndex(_R(IDC_UNITS),&Index); 
							DocUnitList* pDocUnitList = DocUnitList::GetCurrentDocUnitList();
							if ((Index >= 0) && (Index < pDocUnitList->GetNumUnits()))
							{
								DocUnitList* pDocUnitList = DocUnitList::GetCurrentDocUnitList();
								Unit* pUnit = pDocUnitList->FindUnit(Index);
								NodeGrid::SetDefaultUnits(pUnit->GetUnitType());
								pChangeGrid = new ChangeGridUnits;
							}
						}
						break;
					}
		    	}
				break;

				case _R(IDC_GRIDTYPE):
				{
					switch (Msg->DlgMsg)
					{
						case DIM_SELECTION_CHANGED:
						{
							WORD Index;  
							GetValueIndex(_R(IDC_GRIDTYPE),&Index); 
							if ((Index >= 0) && (Index <= NUM_GRID_TYPES))
							{
								NodeGrid::SetDefaultGridType(GridTypeList[Index].Type);
								pChangeGrid = new ChangeGridType;
							}
						}
						break;
					}
				}
				break;

				case _R(IDC_DIVISIONS):
				{
					//if (IsUserName("MarkN")) TRACE( _T("_R(IDC_DIVISIONS) - Msg->DlgMsg = %ld\n"),Msg->DlgMsg);
					switch (Msg->DlgMsg)
					{
						case DIM_SELECTION_CHANGED:
						{
							WORD Index;  
							GetValueIndex(_R(IDC_DIVISIONS),&Index); 
							//if (IsUserName("MarkN")) TRACE( _T("Divisions Index = %ld\n"),Index);

							BOOL Valid=TRUE;
							double Div;

							if (Index < 0)
							{
								String_32 StrVal = GetStringGadgetValue(_R(IDC_DIVISIONS),&Valid); 
								_stscanf((TCHAR*)StrVal,"%NLf",&Div);
							}
							else
								Div = Index+1;

							if (Valid)
							{
								NodeGrid::SetDefaultDivisions(Div);
								pChangeGrid = new ChangeGridDivisions;
							}
						}
						break;											       
					}
				}
				break;

				case _R(IDC_SUBDIVISIONS):
				{
					//if (IsUserName("MarkN")) TRACE( _T("_R(IDC_SUBDIVISIONS) - Msg->DlgMsg = %ld\n"),Msg->DlgMsg);
					switch (Msg->DlgMsg)
					{
						case DIM_SELECTION_CHANGED:
						{
							WORD Index;  
							GetValueIndex(_R(IDC_SUBDIVISIONS),&Index); 
							//if (IsUserName("MarkN")) TRACE( _T("Divisions Index = %ld\n"),Index);

							BOOL Valid=TRUE;
							UINT32 Subdiv;

							if (Index < 0)
							{
								String_32 StrVal = GetStringGadgetValue(_R(IDC_SUBDIVISIONS),&Valid); 
								_stscanf((TCHAR*)StrVal,"%ld",&Subdiv);
							}
							else
								Subdiv = Index+1;

							if (Valid)
							{
								NodeGrid::SetDefaultSubdivisions(Subdiv);
								pChangeGrid = new ChangeGridSubdivisions;
							}
						}
						break;											       
					}
				}
				break;

				case _R(IDC_MAKEGRID) :
				{
					//if (IsUserName("MarkN")) TRACE( _T("_R(IDC_MAKEGRID) - Msg->DlgMsg = %ld\n"),Msg->DlgMsg);
					switch (Msg->DlgMsg)
					{
						case DIM_LFT_BN_CLICKED:
						{
							GridTool::ToggleDefaultGrid();
							// Make sure the controls are shaded correctly
							EnableControls();
						}
						break;											       
					}
				}
				break;
			}

			if (pChangeGrid != NULL)
			{
				OpGridChange* pOpGridChange = new OpGridChange;
				if (pOpGridChange != NULL)
					pOpGridChange->DoChangeSelectedGrids(pChangeGrid);
			}
		}
	}
	else if (MESSAGE_IS_A(Message,OpMsg)) 	// Check for undo/redo
	{
		OpMsg* pOpMsg = (OpMsg*)Message;

		if (pOpMsg->MsgType == OpMsg::AFTER_UNDO || pOpMsg->MsgType == OpMsg::AFTER_REDO)
		{
			if (pOpMsg->pOp != NULL)
			{
				NodeGrid* pNodeGrid = NULL;

				if (pOpMsg->pOp->IsKindOf(CC_RUNTIME_CLASS(OpGrid)))
				{
					NodeGrid::RecalcNumSelectedGrids(GridTool::GetSpreadClicked());

					GridInfoBarOp* pGridInfoBarOp = GridTool::GetGridInfoBarOp();
					if (pGridInfoBarOp != NULL)
						pGridInfoBarOp->EnableControls();

					OpGrid* pOpGrid = (OpGrid*)(pOpMsg->pOp);

					if (pOpMsg->MsgType == OpMsg::AFTER_UNDO)
						pNodeGrid = pOpGrid->GetPreOpDisplayedGrid();
					
					if (pOpMsg->MsgType == OpMsg::AFTER_REDO)
						pNodeGrid = pOpGrid->GetPostOpDisplayedGrid();

					if (pNodeGrid != NULL)
						DisplayGridInfo(pNodeGrid);
				}
			}
		}
	}
	else if (MESSAGE_IS_A(Message,UnitMsg)) 	// Check for changes in the units system
	{
		UnitMsg* pUnitMsg = (UnitMsg*)Message;
		BOOL UpdateInfoBar = FALSE;
		Unit* pThisUnit = NULL;

		switch (pUnitMsg->MsgType)
		{
			case UnitMsg::BEFOREDELETE:
			case UnitMsg::NEW:
			case UnitMsg::CHANGED:
				pThisUnit = pUnitMsg->pDocUnitList->FindUnit(pUnitMsg->ThisUnitType);
				break;
		}
				
		switch (pUnitMsg->MsgType)
		{
			case UnitMsg::BEFOREDELETE:
				if (NodeGrid::GetDefaultUnits() == pUnitMsg->ThisUnitType)
					NodeGrid::SetDefaultUnits(pThisUnit->GetBaseUnitType());
				break;

			case UnitMsg::NEW:
			case UnitMsg::CHANGED:
			case UnitMsg::AFTERDELETE:
				UpdateInfoBar = TRUE;
				break;
		}

		if (UpdateInfoBar)
		{
			InitControls();
			if (LastGridDisplayed == NULL)
				DisplayDefaultGridInfo();
			else
				DisplayGridInfo(LastGridDisplayed);
		}

		ProcessGridUnitMsg* pGridProc = new ProcessGridUnitMsg(pUnitMsg);
		if (pGridProc != NULL)
		{
			NodeGrid::ProcessAllGrids(pGridProc);
			delete pGridProc;
		}
	}
	else if (MESSAGE_IS_A(Message,DocChangingMsg)) 	// Check for changes in the units system
	{
/*
		DocChangingMsg* pDocChangingMsg = (DocChangingMsg*)Message;
		Document* pDoc = pDocChangingMsg->pChangingDoc;

		switch (pDocChangingMsg->State)
		{
			case DocChangingMsg::DocState::SELECTED:
				GridTool::SetDoc(pDoc);
				break;
				
			case DocChangingMsg::DocState::UNSELECTED:
			case DocChangingMsg::DocState::KILLED:
				if (GridTool::GetDoc() == pDoc)
					GridTool::SetDoc(NULL);
				break; 
		}
*/
		DocChangingMsg* pDocChangingMsg = (DocChangingMsg*)Message;

		switch (pDocChangingMsg->State)
		{
			case DocChangingMsg::DocState::SELCHANGED:
				GridTool::SetDoc(pDocChangingMsg->pNewDoc);
				break; 
		}
	}

	// Pass the message on
	return (InformationBarOp::Message(Message));
}    

/********************************************************************************************

>	void GridInfoBarOp::InitControls()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Inits all the controls in the info bar.
				Called immediately after the bar is created when the grid tool
				becomes the current tool.
	SeeAlso:	-

********************************************************************************************/

void GridInfoBarOp::InitControls()
{
	//if (!GridTool::IsCurrentTool()) return;
	if (Tool::GetCurrentID() != TOOLID_GRID) return;
	if (GridTool::GetDoc() == NULL) return;

	INT32 i;

	DeleteAllValues(_R(IDC_UNITS));
	DeleteAllValues(_R(IDC_GRIDTYPE));

	DocUnitList* pDocUnitList = GridTool::GetDoc()->GetDocUnitList();

	for (i=0; i < pDocUnitList->GetNumUnits(); i++)
	{
		Unit* pUnit   = pDocUnitList->FindUnit(i);
		String_32 Str = pUnit->GetToken();
		TCHAR* p = Str;
		String_256 Str256 = p;
		SetStringGadgetValue(_R(IDC_UNITS),&Str256,FALSE,i);
	}

	for (i=0; i<NUM_GRID_TYPES; i++)
	{
		String_32 Str = String_32(GridTypeList[i].IDS);
		SetStringGadgetValue(_R(IDC_GRIDTYPE),&Str,FALSE,i);  
	}

	INT32 l=1;
	for (i=0; i<10; i++,l++)
	{
		SetLongGadgetValue(_R(IDC_DIVISIONS),   l,FALSE,i);
		SetLongGadgetValue(_R(IDC_SUBDIVISIONS),l,FALSE,i);
	}

	EnableControls();
}           

/********************************************************************************************

>	void GridInfoBarOp::EnableControls()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Enables or disables the controls depending on the current context, i.e.
				the controls are disabled if there is no selection, etc.
	SeeAlso:	-

********************************************************************************************/

void GridInfoBarOp::EnableControls()
{
	//if (!GridTool::IsCurrentTool()) return;
	if (Tool::GetCurrentID() != TOOLID_GRID) return;

	Spread* pSpread = GridTool::GetSpreadClicked();
 	BOOL 	enable  = ((NodeGrid::GetNumSelectedGrids() > 0) && (pSpread != NULL));

	EnableGadget(_R(IDC_DIVISIONS),		enable);
	EnableGadget(_R(IDC_SUBDIVISIONS),	enable);
	EnableGadget(_R(IDC_UNITS),			enable);
	EnableGadget(_R(IDC_GRIDTYPE),		enable);

	EnableGadget(_R(IDC_MAKEGRID),pSpread != NULL); 
}           

/********************************************************************************************

>	void GridInfoBarOp::DisplayDefaultGridInfo()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/3/94
	Purpose:	Set the controls on the info bar to reflect the default grid tool values.
				Moved from GridTool to GridInfoBarOp on 28/4/94 by MarkN.
	SeeAlso:	

********************************************************************************************/

void GridInfoBarOp::DisplayDefaultGridInfo()
{
	DisplayGridInfo(	NodeGrid::GetDefaultDivisions(),
						NodeGrid::GetDefaultUnits(),
						NodeGrid::GetDefaultSubdivisions(),
						NodeGrid::GetDefaultGridType());

	LastGridDisplayed = NULL;
}


/********************************************************************************************

>	void GridInfoBarOp::DisplayGridInfo(NodeGrid* pGrid)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/3/94
	Inputs:		pGrid = ptr to grid who's info will be displayed in the infobar
	Purpose:	Set the GridTool's controls on the info bar.
				Moved from GridTool to GridInfoBarOp on 28/4/94 by MarkN.
	SeeAlso:	

********************************************************************************************/

void GridInfoBarOp::DisplayGridInfo(NodeGrid* pGrid)
{
	//if (!GridTool::IsCurrentTool()) return;
	if (Tool::GetCurrentID() != TOOLID_GRID) return;

	DisplayGridInfo(	pGrid->GetDivisions(),
						pGrid->GetUnits(),
						pGrid->GetSubdivisions(),
						pGrid->GetGridType());

	LastGridDisplayed = pGrid;

	//SetDimensionGadgetValue(_R(IDC_DIVISIONS),(MILLIPOINT)pGrid->GetMainStep(),pGrid);
}


/********************************************************************************************

>	void GridInfoBarOp::DisplayGridInfo(double Divisions, UnitType Units, UINT32 Subdivisions, GridType GType)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/3/94
	Inputs:		Divisions 		= Number of 'Units' between each main grid point
				Units			= The units used to calc main grid point spacing
				Subdivisions	= Number of sub-points between each main point
				GType			= Type of grid (See GridType for a list of these)
	Purpose:	Set the GridTool's controls on the info bar.
				Moved from GridTool to GridInfoBarOp on 28/4/94 by MarkN.
	SeeAlso:	

********************************************************************************************/

void GridInfoBarOp::DisplayGridInfo(double Divisions, UnitType Units, UINT32 Subdivisions, GridType GType)
{
	//if (!GridTool::IsCurrentTool()) return;
	if (Tool::GetCurrentID() != TOOLID_GRID) return;

	INT32 i;

	if (Units != NOTYPE)
	{
		DocUnitList* pDocUnitList = DocUnitList::GetCurrentDocUnitList();
		SetSelectedValueIndex(_R(IDC_UNITS),pDocUnitList->FindUnitIndex(Units));
	}

/*	if (pDocUnitList != NULL)
	{
		String_32 Str = pDocUnitList->GetToken(Units);
		TCHAR* p = Str;
		String_256 Str256 = p;
		SetStringGadgetValue(_R(IDC_UNITS),&Str256,FALSE,-1);
	}
*/
	for (i=0;i<NUM_GRID_TYPES;i++)
		if (GridTypeList[i].Type == GType)
        	SetSelectedValueIndex(_R(IDC_GRIDTYPE),i);

	String_16 StringVal;
	TCHAR CharString[20];

	_stprintf(CharString, "%.2g", (double) Divisions);
	StringVal = CharString;
	SetStringGadgetValue(_R(IDC_DIVISIONS), &StringVal, FALSE, -1);

	wsprintf(CharString, TEXT("%ld"), (INT32) Subdivisions);
	StringVal = CharString;
	SetStringGadgetValue(_R(IDC_SUBDIVISIONS), &StringVal, FALSE,-1);
}

//----------------------------------------------------
//----------------------------------------------------
//----------------------------------------------------
//----------------------------------------------------
//----------------------------------------------------

// ------------------------------------------------------------------------------------------
// OpGridDelete methods
            
/********************************************************************************************

>	static OpState	OpGridDelete::GetState(String_256*, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpGridDelete
	Purpose:	For finding OpGridDelete's state. 
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpGridDelete::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
	String_256 DisableReason; 

	// Ensure that a document exists
	if (GridTool::GetDoc() == NULL)
	{
		// There is no current document
		OpSt.Greyed = TRUE;
		// Load reason why operation is disabled
		DisableReason = String_256(_R(IDS_NO_DOC));
	   	*UIDescription = DisableReason;	     
	}
	else
	{
		Spread* pSpread = GridTool::GetSpreadClicked();
 		BOOL 	enable  = ((NodeGrid::GetNumNonDefaultSelectedGrids() > 0) && (pSpread != NULL));

		if (!enable)
		{
			OpSt.Greyed = TRUE; 
			// Load reason why operation is disabled
			DisableReason = String_256(_R(IDS_NO_OBJECTS_SELECTED));
	   		*UIDescription = DisableReason;	                                      
		}
	}

	return (OpSt);
}

/********************************************************************************************

>	void OpGridDelete::Do(OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the Delete operation. 
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
	
void OpGridDelete::Do(OpDescriptor*)
{   
	// We haven't shown a grid in the infobar yet, so set these to NULL
	PreOpDisplayedGrid  = NULL;
	PostOpDisplayedGrid = NULL;

	Spread* SpreadClicked = GridTool::GetSpreadClicked();

	GridInfoBarOp* pGridInfoBarOp = GridTool::GetGridInfoBarOp();
	if (pGridInfoBarOp != NULL)
		PreOpDisplayedGrid = pGridInfoBarOp->GetLastGridDisplayed();

	BOOL ok = (SpreadClicked != NULL);
	UINT32 SelCount=0;

	if (ok)
	{
		// scan the children of the spread for grids
		Node* pNode = SpreadClicked->FindFirstChild();
		while (pNode != NULL && ok)
		{
			Node* pNodeCurrent = pNode;
			pNode = pNode->FindNext();

			if (pNodeCurrent->IsKindOf(CC_RUNTIME_CLASS(NodeGrid)))
			{
				// we now have a ptr to a grid, so render those blobs
				NodeGrid* pGrid = (NodeGrid*)pNodeCurrent;

	 			if (pGrid->IsGridSelected() && !pGrid->IsDefault())
				{
					SelCount++;
					GridTool::ForceRedraw(pGrid);
							ok = DoInvalidateNodeRegion(pGrid, TRUE);
					if (ok) ok = DoHideNode(pGrid,TRUE);
				}
			}
		}
	}

	if (!ok)
		FailAndExecute();
	else
	{
		NodeGrid::RecalcNumSelectedGrids(SpreadClicked);
		pGridInfoBarOp->EnableControls();
	}

	Plural = (SelCount > 1);

	End();
}			

/********************************************************************************************

>	void OpGridDelete::GetOpName(String_256* OpName) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
					
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpGridDelete::GetOpName(String_256* OpName) 
{ 
	if (Plural)
		*OpName = String_256(_R(IDS_GRID_UNDO_DELETES));
	else
		*OpName = String_256(_R(IDS_GRID_UNDO_DELETE));
}  

// ------------------------------------------------------------------------------------------
// OpGridDuplicate methods
            
/********************************************************************************************

>	static OpState	OpGridDuplicate::GetState(String_256*, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpGridDuplicate
	Purpose:	For finding OpGridDuplicate's state. 
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpGridDuplicate::GetState(String_256* UIDescription, OpDescriptor*)
{
	return (OpGridDelete::GetState(UIDescription,NULL));
}

/********************************************************************************************

>	void OpGridDuplicate::Do(OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/94
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the Delete operation. 
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
	
void OpGridDuplicate::Do(OpDescriptor*)
{   
	// We haven't shown a grid in the infobar yet, so set these to NULL
	PreOpDisplayedGrid  = NULL;
	PostOpDisplayedGrid = NULL;

	Spread* SpreadClicked = GridTool::GetSpreadClicked();

	GridInfoBarOp* pGridInfoBarOp = GridTool::GetGridInfoBarOp();
	if (pGridInfoBarOp != NULL)
		PreOpDisplayedGrid = pGridInfoBarOp->GetLastGridDisplayed();

	BOOL ok = (SpreadClicked != NULL);
	UINT32 SelCount=0;

	if (ok)
	{
		// scan the children of the spread for grids
		Node* pNode = SpreadClicked->FindFirstChild();
		while (pNode != NULL && ok)
		{
			Node* pNodeCurrent = pNode;
			pNode = pNode->FindNext();

			if (pNodeCurrent->IsKindOf(CC_RUNTIME_CLASS(NodeGrid)))
			{
				// we now have a ptr to a grid, so render those blobs
				NodeGrid* pGrid = (NodeGrid*)pNodeCurrent;

	 			if (pGrid->IsGridSelected() && !pGrid->IsDefault())
				{
					SelCount++;

					NodeGrid* pNewGrid = DoDuplicateGrid(	pGrid,NEXT,pGrid,
															SpreadClicked,SpreadClicked,TRUE,
															28346,-28346);

					ok = (pNewGrid != NULL);

					if (ok)
					{
						pGrid->SetGridSelected(FALSE);
						if (pGrid == PreOpDisplayedGrid)
						{
							GridTool::DisplayGridInfo(pNewGrid);
							PostOpDisplayedGrid = pNewGrid;
						}
					}
				}
			}
		}
	}

	if (!ok)
		FailAndExecute();
	else
	{
		NodeGrid::RecalcNumSelectedGrids(SpreadClicked);
		pGridInfoBarOp->EnableControls();
	}

	Plural = (SelCount > 1);

	End();
}			

/********************************************************************************************

>	void OpGridDuplicate::GetOpName(String_256* OpName) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/94
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
					
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpGridDuplicate::GetOpName(String_256* OpName) 
{ 
	if (Plural)
		*OpName = String_256(_R(IDS_GRID_UNDO_DUPS));
	else
		*OpName = String_256(_R(IDS_GRID_UNDO_DUP));
}  

//--------------------------------------
//--------------------------------------
//--------------------------------------

/********************************************************************************************

>	static OpState NoClipboardOpGetState(String_256*, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/94
	Inputs:		-
	Outputs:	-
	Returns:	OpState.Greyed == TRUE, and a description of why is returned in the string
	Purpose:	General "There are no clipboard ops" get state function for aliased Cut, Copy
				and Paste operations.				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

static OpState NoClipboardOpGetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
	String_256 DisableReason; 

	// You cant have a small car that's good on the open road - You can with a Nissan
	// You cant cut a grid to the clipboard - Not even Nissan can help you there mate!
	OpSt.Greyed = TRUE;
	// Load reason why operation is disabled
	DisableReason = String_256(_R(IDS_NO_GRID_CLIPBOARD_OPS));
   	*UIDescription = DisableReason;	     

	return (OpSt);
}

