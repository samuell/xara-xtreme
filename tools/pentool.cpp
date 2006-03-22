// $Id: pentool.cpp 662 2006-03-14 21:31:49Z alex $
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
// The 'pen' line and curve drawing tool
// Created by MIke on 19/9/94

/*
*/

#include "camtypes.h"
#include "binds.h"
//#include "resource.h"
//#include "barsdlgs.h" 
#include "stockcol.h"
#include "rndrgn.h"
#include "pentool.h"
#include "oilfiles.h"
//#include "viewrc.h"
//#include "mike.h"
#include "csrstack.h"
#include "blobs.h"
#include "app.h"
#include "docview.h"
//#include "markn.h"
#include "nodepath.h"
//#include "mainfrm.h"
#include "osrndrgn.h"
#include "penedit.h"
#include "ops.h"
#include "pathedit.h"
#include "cutop.h"
#include "opdesc.h"
#include "keypress.h"
//#include "jim.h"
#include "opbreak.h"
#include "vkextra.h"

//#include "will2.h"

// These are still char* while we wait for resource technology to be developed for modules
TCHAR* PenTool::FamilyName = _T("Drawing Tools");
TCHAR* PenTool::ToolName = _T("Pen Tool");
TCHAR* PenTool::Purpose = _T("To draw lines and curves");
TCHAR* PenTool::Author = _T("Mike");


CC_IMPLEMENT_MEMDUMP( PenTool, Tool_v1 )
CC_IMPLEMENT_DYNCREATE( PenToolInfoBarOp, InformationBarOp )

// Better memory tracking please
#define new CAM_DEBUG_NEW



/********************************************************************************************

>	PenTool::PenTool()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/9/94
	Purpose:	Dummy Constructor - It does nothing. All the real initialisation is done
				in PenTool::Init which is called by the Tool Manager
	SeeAlso:	PenTool::Init

********************************************************************************************/

PenTool::PenTool()
{
 	pcPenCursor = NULL;
 	pcPenAdjustCursor = NULL;
	pcPenReshapeCursor = NULL;
	pcMoveBezCursor = NULL;
	MyCurrentCursor = NULL;

	CurrPenState = IS_Undefined;
}




/********************************************************************************************

>	PenTool::~PenTool()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/9/94
	Purpose:	Destructor (Virtual). Does nothing.

********************************************************************************************/

PenTool::~PenTool()
{
	// Dummy destructor
}


/********************************************************************************************

>	BOOL PenTool::Init( INT32 Pass )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/9/94
	Returns:	FALSE if it does not want to be created, TRUE otherwise
	Purpose:	Used to check if the Tool was properly constructed
	SeeAlso:	PenTool::PenTool

********************************************************************************************/

BOOL PenTool::Init()
{

	BOOL 					ok;
	CCResTextFile 			file;				// Resource File
	PenToolInfoBarOpCreate	BarCreate;			// Object that creates PenInfoBarOp objects

	// This should be set to NULL by default. It will be set properly below, if
	// everthing is working as it should
	pPenInfoBarOp = NULL;

	// initially, no cursor
	pcPenCursor = 0;

	pPenInfoBarOp = new PenToolInfoBarOp();
	ok = (pPenInfoBarOp != NULL);
	if (ok) pPenInfoBarOp->pPenTool = this;				// Set a pointer from the op to this tool

#if 0
 		ok = file.open(_R(IDM_PENTOOL_BAR), _R(IDT_INFO_BAR_RES));			// Open resource
	if (ok) ok = DialogBarOp::ReadBarsFromFile(file,BarCreate);		// Read and create info bar
	if (ok) file.close();									 		// Close resource.

	ENSURE(ok,"Unable to load penbar.ini from resource\n"); 

	if (ok)
	{
		// Info bar now exists.  Now get a pointer to it
		String_32 str = String_32(_R(IDS_PENTOOL_INFOBARNAME));
		DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(str);

				ok = (pDialogBarOp != NULL);
		if (ok) ok = pDialogBarOp->IsKindOf(CC_RUNTIME_CLASS(PenToolInfoBarOp));
		if (ok) pPenInfoBarOp = (PenToolInfoBarOp*)pDialogBarOp;
		if (ok) pPenInfoBarOp->pPenTool = this;				// Set a pointer from the op to this tool

		ENSURE(ok,"Failed to create PENTOOL info bar");
	}
#endif

	if (ok) ok = EditPath.Initialise(12,24);				// create the edit path buffers please

	// Register our pen operation(s).
	if (ok)
	{
		ok = OpPenCreateInternal::Init();
		if (ok) ok = OpPenEditInternal::Init();
		if (ok) ok = OpPenCreatePath::Init();
		if (ok) ok = OpPenAddElement::Init();
		if (ok) ok = OpAddNewPath::Init();
		if (ok) ok = OpPenClosePath::Init();
	}

	return ok;

}


/********************************************************************************************

>	void PenTool::Describe(void *InfoPtr)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Inputs:		InfoPtr - A pointer to a tool info block. It is passed cast to void* as
				the version of the tool is unknown at this point. Later versions of the
				Tool class may have more items in this block, that this tool will not use
	Outputs:	InfoPtr - The structure pointed to by InfoPtr will have had all the info
				that this version of the Tool knows about
	Purpose:	Allows the tool manager to extract information about the tool

********************************************************************************************/

void PenTool::Describe(void *InfoPtr)
{
	// Cast structure into the latest one we understand.
	ToolInfo_v1 *Info = (ToolInfo_v1 *) InfoPtr;

	Info -> InfoVersion = 1;
	
	Info -> InterfaceVersion = GetToolInterfaceVersion();  // You should always have this line.
		
	// These are all arbitrary at present.
	Info -> Version = 1;
	Info -> ID      = GetID();
	Info -> TextID  = _R(IDS_PEN_TOOL);

	Info -> Family  = FamilyName;
	Info -> Name    = ToolName;
	Info -> Purpose = Purpose;
	Info -> Author  = Author;
	
	Info -> InfoBarDialog = _R(IDD_FREEHANDTOOL);

	Info -> BubbleID = _R(IDBBL_PEN_TOOL);

}


/********************************************************************************************

>	virtual void PenTool::SelectChange(BOOL isSelected)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the tool is selected or deselected.  Creates and pushes the
				tool's cursor; pops and destroys it.
	Errors:		Sends warning to debugging terminal if creating the cursor fails.
	SeeAlso:	-

********************************************************************************************/

void PenTool::SelectChange(BOOL isSelected)
{
	if (isSelected)
	{
		// This tool has just been selected.  Create an appropriate cursor, and push it
		// onto the top of the cursor stack so it'll appear when the pointer moves into
		// our window.

		MyCurrentCursor = NULL;

		if (!CreatePenCursors())
			return;

		CurrentCursorID = CursorStack::GPush(pcPenCursor, FALSE);		// Push, but don't display now
		MyCurrentCursor = pcPenCursor;
		
		// Create and display my info bar please
		if (pPenInfoBarOp != NULL)
			pPenInfoBarOp->Create();

			// Which blobs do I want displayed
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		if (BlobMgr != NULL)
		{
			// Decide which blobs we will display
			BlobStyle MyBlobs;
			MyBlobs.Pen = TRUE;

			// Tell the blob manager
			BlobMgr->ToolInterest(MyBlobs);
		}

		// Now set some local variables so we know what state we are
		// in to start with
		CurrPenState = IS_Undefined;

		// alias the delete function to delete points.
		// OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_DELETE);  
		// pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpDeletePoints),OpDeletePoints::GetState,0);  

		// remove the alias to the delete function
		// OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_DELETE);  
		// pOpDesc->RemoveAlias();
	}
	else
	{
		// Deselection - destroy the tool's current cursor, if there is one.
		if (MyCurrentCursor)
		{
			CursorStack::GPop(CurrentCursorID);
			MyCurrentCursor = NULL;
			CurrentCursorID = 0;
		}
		DeletePenCursors();

		// Hide and destroy my info bar please
		if (pPenInfoBarOp != NULL)
			pPenInfoBarOp->Delete();

		// ensure any tool object blobs are removed.
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		if (BlobMgr != NULL)
		{
			BlobStyle bsRemoves;
			bsRemoves.ToolObject = TRUE;
			BlobMgr->RemoveInterest(bsRemoves);
		}

		ClearInternalState();
	}
}



/********************************************************************************************

	BOOL PenTool::CreatePenCursors()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Pen tool cursor creation

********************************************************************************************/

BOOL PenTool::CreatePenCursors()
{
	pcPenAdjustCursor = new Cursor(this, _R(IDC_PENADJUSTCURSOR));
	if (!pcPenAdjustCursor || !pcPenAdjustCursor->IsValid())
	{
		DeletePenCursors();
		InformError( _R(IDS_OUT_OF_MEMORY), _R(IDS_OK) );
		return FALSE;										 
	}

	pcPenCursor = new Cursor(this, _R(IDC_NEWPATHCURSOR));
	if (!pcPenCursor || !pcPenCursor->IsValid())
	{
		DeletePenCursors();
		InformError( _R(IDS_OUT_OF_MEMORY), _R(IDS_OK) );
		return FALSE;
	}

	pcPenReshapeCursor = new Cursor(this, _R(IDC_RESHAPECURSOR));
	if (!pcPenReshapeCursor || !pcPenReshapeCursor->IsValid())
	{
		DeletePenCursors();
		InformError( _R(IDS_OUT_OF_MEMORY), _R(IDS_OK) );
		return FALSE;										 
	}

	pcMoveBezCursor = new Cursor(this, _R(IDC_MOVEBEZIERCURSOR));
	if (!pcMoveBezCursor || !pcMoveBezCursor->IsValid())
	{
		DeletePenCursors();
		InformError( _R(IDS_OUT_OF_MEMORY), _R(IDS_OK) );
		return FALSE;
	}

	return TRUE;
}


/********************************************************************************************

	BOOL PenTool::DeletePenCursors()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Pen tool cursor deletion

********************************************************************************************/

void PenTool::DeletePenCursors()
{
	if (pcPenCursor!=NULL)
	{
		delete pcPenCursor;
		pcPenCursor=NULL;
	}
	
	if (pcPenAdjustCursor!=NULL)
	{
		delete pcPenAdjustCursor;
		pcPenAdjustCursor=NULL;
	}

	if (pcPenReshapeCursor!=NULL)
	{
		delete pcPenReshapeCursor;
		pcPenReshapeCursor=NULL;
	}

	if (pcMoveBezCursor!=NULL)
	{
		delete pcMoveBezCursor;
		pcMoveBezCursor=NULL;
	}
}
		

/********************************************************************************************

>	MsgResult PenToolInfoBarOp::Message(Msg* Message) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/94
	Inputs:		Message: The message to handle
	Outputs:	-
	Returns:	-
	Purpose:	Pen tool info bar dialog message handler
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult PenToolInfoBarOp::Message(Msg* Message) 
{
	if (MESSAGE_IS_A(Message,OpMsg)) 	// Check for undo/redo
	{
		OpMsg* pOpMsg = (OpMsg*)Message;

		if (pOpMsg->MsgType == OpMsg::AFTER_UNDO)
		{
			pPenTool->ClearInternalState();
		}

		if (pOpMsg->MsgType == OpMsg::END)
		{
			// an operation has ended, so lets check our own opstate
			
			if (IS_A(pOpMsg->pOp, OpPenCreateInternal) ||
				IS_A(pOpMsg->pOp, OpPenEditInternal))
				if (pPenTool->GetPenOpState() == OS_EditInternal)
				{
					if (!(pOpMsg->pOp->GetOpFlgs()).Failed)
						pPenTool->SetInternalState();
					pPenTool->ClearOp();
				}

			if (IS_A(pOpMsg->pOp, OpPenCreatePath))
				if (pPenTool->GetPenOpState() == OS_CreatePath)
					{
						if (!(pOpMsg->pOp->GetOpFlgs()).Failed)
							pPenTool->CreateNewPath();
						pPenTool->ClearPath();
						pPenTool->ClearOp();
					}

			if (IS_A(pOpMsg->pOp, OpPenAddElement))
				if (pPenTool->GetPenOpState() == OS_AddElement)
				{
					if (!(pOpMsg->pOp->GetOpFlgs()).Failed)
						pPenTool->AddElementToPath();
					pPenTool->ClearPath();
					pPenTool->ClearOp();
				}

			if (IS_A(pOpMsg->pOp, OpPenClosePath))
				if (pPenTool->GetPenOpState() == OS_ClosePath)
				{
					if (!(pOpMsg->pOp->GetOpFlgs()).Failed)
						pPenTool->CloseWithPath();
					pPenTool->ClearPath();
					pPenTool->ClearOp();
				}
		}
	}
	else if (MESSAGE_IS_A(Message,DocChangingMsg)) 	// Check for changes in the doc system
	{
		DocChangingMsg* pDocChangingMsg = (DocChangingMsg*)Message;
//		Document* pDoc = pDocChangingMsg->pChangingDoc;

		switch (pDocChangingMsg->State)
		{
			case DocChangingMsg::SELCHANGED:
			case DocChangingMsg::KILLED:
				pPenTool->ClearInternalState();
				break; 
			default:
				break;
		}
	}

	// Pass the message on
	return (DialogBarOp::Message(Message));
}


/********************************************************************************************

>	void PenTool::OnClick( DocCoord PointerPos, ClickType Click, 
						   ClickModifiers ClickMods, Spread *pSpread )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com> (via Rik)
	Created:	20/9/94
	Inputs:		PointerPos - The Coords (in spread coords) of the point where the mouse 
				button was clicked
				Click -	Describes the type of click that was detected. 
				ClickMods -	Indicates which buttons caused the click and which modifers were
				pressed at the same time
				pSpread - the spread in which the click happened
	Returns:	TRUE if it handled the Click, FALSE otherwise
	Purpose:	To handle a Mouse Click event for the Pen Tool. 
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers

********************************************************************************************/

void PenTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
					   Spread* pSpread )
{
	if (ClickMods.Menu) return;							// Don't do anything if the user clicked the Menu button

	// determine what to do with this click
	penclick WhatToDo = DetermineClickEffect(PointerPos, pSpread, &pNodePath);

	// if someones holding the adjust down and we're not on a point then clear the sel
	if (Click==CLICKTYPE_SINGLE && ClickMods.Adjust && WhatToDo!=PenOnPoint)
	{
		RemoveSelection(PointerPos,pSpread);
		ClearInternalState();	
		return;
	}

	switch (Click)
	{
		case CLICKTYPE_SINGLE:
			switch (WhatToDo)
			{
				case PenNewPath:
				{
					BOOL MakeInternal = TRUE;
					if ((CurrPenState == IS_MoveTo) || (CurrPenState == IS_DragTo))
					{
						// create a new path using the internal data points.
						// If LastDragTo==LastMoveTo, then the internal is a point,
						// else a curve control end exists.
						if (EditHandles.pHndSpread==pSpread)
						{
							MakeInternal = FALSE;
							OpPenCreatePath* pOpCreatePath = new OpPenCreatePath;
							if (pOpCreatePath!=NULL)
							{
								pOpCreatePath->DoPenCreatePath(&EditHandles, PointerPos, pSpread, &EditPath);
								CurrPenOpState = OS_CreatePath;
							}
						}
					}
					
					if (MakeInternal)
					{
						// First remove any selected points in the spread and create an
						// internal move or dragto
						RemoveSelection(PointerPos,pSpread);
						OpPenCreateInternal* pOpCreateInternal = new OpPenCreateInternal;
						if (pOpCreateInternal!=NULL)
						{
							pOpCreateInternal->DoPenCreateInternal(PointerPos, pSpread, &EditHandles);
							CurrPenOpState = OS_EditInternal; 
						}
					}
				}
				break;

				case PenClosePath:
					{
						NodeIndex = pNodePath->InkPath.GetPathPosition();
						OpPenClosePath* pOpPenClosePath = new OpPenClosePath;
						if (pOpPenClosePath!=NULL)
						{
							pOpPenClosePath->DoPenClosePath(pNodePath, NodeIndex, pSpread, &EditPath);
							CurrPenOpState = OS_ClosePath;
						}
					}
					break;

				case PenOnPoint:
					{
						// Ordinary click from OnPoint
						// Currently this does not work correctly because of eor'ing. The wrong
						// Eor blobs get put on by the node path click handler 
						RemoveSelection(PointerPos,pSpread,pNodePath);
						ClickOnEndPoint(Click, ClickMods, pSpread, pNodePath);
					}
					break;

				case PenAddSegment:
					{
						NodeIndex = pNodePath->InkPath.GetPathPosition();
						OpPenAddElement* pOpPenAddElement = new OpPenAddElement;
						if (pOpPenAddElement!=NULL)
						{
							pOpPenAddElement->DoPenAddElement(pNodePath, NodeIndex, PointerPos, pSpread, &EditPath);
							CurrPenOpState = OS_AddElement;
						}
					}
					break;

				case PenEditInternalMove:
					{
						OpPenEditInternal* pOpEditInternal = new OpPenEditInternal;
						if (pOpEditInternal!=NULL)
						{
							pOpEditInternal->DoPenEditInternal(&EditHandles);
							CurrPenOpState = OS_EditInternal;
						}
					}
					break;
			   
			   case PenReshapeLine:
			   		{
						double pdist;
						INT32 tempel;
						pNodePath->InkPath.SqrDistanceToPoint(PointerPos, &tempel, &pdist);
						INT32 PathPos = pNodePath->InkPath.GetPathPosition();

						OpReshapeOrAddPoint* pOpReshape = new OpReshapeOrAddPoint;
						if (pOpReshape!=NULL)
							pOpReshape->DoStartDragEdit(pNodePath, PointerPos, pSpread, PathPos, pdist);
						else
							InformError( _R(IDS_OUT_OF_MEMORY), _R(IDS_OK) );
					}
					break;

			}
			break;

		case CLICKTYPE_DOUBLE:
			break;

		case CLICKTYPE_DRAG:
			switch (WhatToDo)
			{
				case PenOnPoint:
					if (pNodePath)
						ClickOnEndPoint(Click, ClickMods, pSpread, pNodePath);
					break;

				default:
					break;
			}
			break;

		default:
			break;
	}

}




/**************************************************************************************************************

> penclick PenTool::DetermineClickEffect(DocCoord PointerPos, Spread* pSpread, NodePath** ReturnNode)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Inputs:		PointerPos is the mouse position
				pSpread is a pointer to the spread containing the mouse position
	Outputs:	ReturnNode returns a pointer to the node the mouse has clicked on
	Returns:	The effect of clicking - one of NewPath, AddSegment, OnPoint or EditInternalMove
	Purpose:	Used when single clicking. This routine determines what effect a click will have.
				In this tool, clicking will add a segment to the end of a line, adjust the last element of
				a path or start a new path entirely.

***************************************************************************************************************/

penclick PenTool::DetermineClickEffect(DocCoord PointerPos, Spread* pSpread, NodePath** ReturnNode )
{
	penclick WhatToDo = PenNewPath;	// tells me what effect the click would have
	NodePath* WhichNode = NULL;		// Saves me using a pointer to a pointer

	// Now scan through the selected paths, and see if we've clicked on any endpoints
	DocView* pDocView = DocView::GetSelected();
	ENSURE( pDocView != NULL, "PenTool::DetermineClickEffect: Can't find selected DocView");
	if (pDocView==NULL)
		return(PenNewPath);

	// Find the Rect round the mouse pos that counts as a hit
	DocRect BlobRect;
	OSRenderRegion::GetBlobRect(pDocView->GetViewScale(), PointerPos, BT_SELECTEDLARGEST, &BlobRect);

	// if we're in an internal state and someone has clicked on the moveto
	// again then go and edit it.
	if (CurrPenState == IS_MoveTo || CurrPenState == IS_DragTo)
	{
		if (BlobRect.ContainsCoord(EditHandles.HndClick))
			return (PenEditInternalMove);
	}


	// Find the selected range of objects
	SelRange* Selected = GetApplication()->FindSelection();
	Node* pNode = Selected->FindFirst();

	INT32 TotSelectedEndpoints = 0;		// Count selected points in paths
	BOOL MultiSegments = FALSE;

	if (pNode != NULL)
	{
		Spread* NodeSpread = pNode->FindParentSpread();
		if (NodeSpread == pSpread)
		{
			// On the same spread, so see if the pointer is over an endpoint
			while ((pNode!=NULL) && ((WhatToDo==PenNewPath) || (WhatToDo==PenAddSegment)))
			{
				if (pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodePath))
				{
					// Now we know it's a NodePath, get a pointer to the Path object within it, so
					// we can find any endpoints
					Path* ThisPath = &(((NodePath*)pNode)->InkPath);

					// count the number of selected points on the path (excluding control points)
					INT32 NumCoords = ThisPath->GetNumCoords();
					PathFlags* Flags = ThisPath->GetFlagArray();

					// Count selected points in this path
					INT32 NumSelectedEndpoints = 0;		
					INT32 lastsel = 0;
					for (INT32 i=0; i<NumCoords; i++)
					{
						if (Flags[i].IsSelected && Flags[i].IsEndPoint)
						{
							lastsel = i;
							NumSelectedEndpoints++;
						}
					}
					// Sum up
					TotSelectedEndpoints+=NumSelectedEndpoints;

					INT32 point;
					// First, see if the mouse is over a point.
					if (ThisPath->FindNearestPoint(PointerPos, POINTFLAG_ENDPOINTS | POINTFLAG_ENDSFIRST, &point))
					{
						WhichNode =	(NodePath*)pNode;
						WhatToDo = PenOnPoint;
						ThisPath->SetPathPosition(point);

						// if the mouse was over an open end and the other end of this sub
						// path is selected then we need to close the path
						// find the start and end of the subpath the point was found in
						INT32 start = point;
						INT32 end = point;
						ThisPath->FindStartOfSubPath(&start);
						ThisPath->FindEndElOfSubPath(&end);
						if (point==start || point==end)
						{
							INT32 other;
							(point==start) ? (other=end) : (other=start);
							if ((Flags[other].IsSelected) && (!Flags[point].IsSelected))
								WhatToDo = PenClosePath;
						}
						continue;
					}

					// if the mouse is close to an element, then it might be close
					// enough to reshape the line
					if (ThisPath->PointCloseToLine(PointerPos, &point))
					{
						WhichNode=(NodePath*)pNode;
						WhatToDo=PenReshapeLine;
						ThisPath->SetPathPosition(point);
						continue;
					}

					if (!MultiSegments && NumSelectedEndpoints==1)
					{
						INT32 start = lastsel;
						INT32 end = lastsel;
						ThisPath->FindStartOfSubPath(&start);
						ThisPath->FindEndElOfSubPath(&end);
						if (lastsel==start || lastsel==end)
						{
							if (WhatToDo==PenNewPath)
							{	
								WhichNode=(NodePath*)pNode;
								WhatToDo=PenAddSegment;
								ThisPath->SetPathPosition(lastsel);
							}
							else
							{
								// We have more than one path with a selected end point.
								// Adding elements is ambiguous so force a NewPath creation
								WhichNode=(NodePath*)pNode;
								WhatToDo=PenNewPath;
								MultiSegments=TRUE;
							}
						}
					}
				}
				// Now find the next selected node
				pNode = Selected->FindNext(pNode);
			}
		}
	}

	// WhatToDo tells us what the action will be
	// WhichNode points to the node we are dealing with
	// PathPosition is the index into that path of the element we are using

	*ReturnNode = WhichNode;
	return (WhatToDo);
}




/**************************************************************************************************************

>	void PenTool::ClickOnEndPoint( ClickType Click,
								   ClickModifiers ClickMods,
								   Spread *pSpread,
								   NodePath* pNodePath )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Inputs:		Click 		- Describes the type of click that was detected. 
				ClickMods 	- Indicates which buttons caused the click and which modifers were
							  pressed at the same time
				pSpread 	- the spread in which the click happened
				pNodePath	- The path on which the click happened (current pos set to the point clicked on)
	Outputs:	-
	Returns:	-
	Purpose:	An end point click has occured. This function acts on such a click by selecting the point
				or editing the point dependent on the click type
	
***************************************************************************************************************/

void PenTool::ClickOnEndPoint( ClickType Click, ClickModifiers ClickMods, Spread *pSpread, NodePath* pNodePath )
{
	Path* pPath = &(pNodePath->InkPath);

	DocCoord* Coords = pPath->GetCoordArray();
	PathFlags* Flags = pPath->GetFlagArray();
	PathVerb*  Verbs = pPath->GetVerbArray();
	INT32 NumCoords = pPath->GetNumCoords();

	// grab the index of the point actually clicked on!
	INT32 Pos = pPath->GetPathPosition();

	// Clicks on control points have no effect
	// but clicks on endpoints do have an effect

	switch (Click)
	{
		case CLICKTYPE_SINGLE:
		{
			if (Flags[Pos].IsEndPoint)
			{
				pPath->RenderPathPenBlobs(pSpread);

				BOOL CurSelState = Flags[Pos].IsSelected;
				BOOL NewSelState = TRUE;
				BOOL ClearOthers = FALSE;
				
				// if shift held down then toggle the point	whatever
				if (ClickMods.Adjust) 
					NewSelState = !CurSelState;

				// if the end point is not selected and we're not toggling
				if (!CurSelState && !ClickMods.Adjust)
					ClearOthers=TRUE;
					
				if (ClearOthers)
				{
					for (INT32 i=0; i<NumCoords; i++)
					{
						Flags[i].IsSelected = FALSE;
					}
				}

				// Now change the selection of this point
				Flags[Pos].IsSelected = NewSelState;

				if ((Pos>0) && (!Flags[Pos-1].IsEndPoint))
					Flags[Pos-1].IsSelected = NewSelState;

				if ((Pos+1<NumCoords) && (!Flags[Pos+1].IsEndPoint))
					Flags[Pos+1].IsSelected = NewSelState;

				// Check for this being the first element in a closed subpath
				// If this element is a moveto, and the end of the path has the
				// CLOSEFIGURE flag set, we should select the endpoint as well
				if (Verbs[Pos] == PT_MOVETO)
				{
					// This for loop will find either the end of the path, or the next moveto
					INT32 j;
					for (j=Pos+1;j<NumCoords && Verbs[j] != PT_MOVETO;j++);	// ; is intentional!
					j--;
					if (Verbs[j] & PT_CLOSEFIGURE)
					{
						//HandleBlobClick(Coords,Flags,j,NumCoords,TRUE);
						Flags[j].IsSelected = NewSelState;
						// If the previous point is a control point then deal with it
						if ((j>0) && (!Flags[j-1].IsEndPoint))
						{
							// Change the control point's selection state
							Flags[j-1].IsSelected = NewSelState;
						}
					}

				}

				pPath->RenderPathPenBlobs(pSpread);
			}
			break;
		}

		case CLICKTYPE_DOUBLE:
			break;

		case CLICKTYPE_DRAG:
		{
			if ((Flags[Pos].IsEndPoint) && (!ClickMods.Menu))
			{
				// Need to do a drag on the selected points, so we had better start an operation
				OpNodePathEditBlob* pOpNodePath = new OpNodePathEditBlob;
				if (pOpNodePath == NULL)
					InformError();
				else
					pOpNodePath->DoStartDragEdit(pNodePath, Coords[Pos], pSpread);
			}
		}
		break;

		default:
		{
		}
		break;
	}
}



/********************************************************************************************

>	virtual BOOL PenTool::OnKeyPress(KeyPress* pKeyPress)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/08/94
	Inputs:		pKeyPress - pointer to a keypress object
	Returns:	TRUE if it handled the keypress, FALSE otherwise
	Purpose:	To handle keypress events for the Pen Tool. If it is a keypress that it
				know about it starts up an appropiate operation.
				THIS IS BADLY WRONG AT THE MOMENT - WHEN THERE IS THE TECHNOLOGY FOR USER
				DEFINABLE KEY-SHORTCUTS THIS FUNCTION WILL NEED FIXING	

********************************************************************************************/

BOOL PenTool::OnKeyPress(KeyPress* pKeyPress)
{
	// We don't want to know about key release events
	if (pKeyPress->IsRelease())
		return FALSE;

	// If ESCAPE is pressed then clear the floating endpoint, but don't claim the keypress
	// so the selection is also cleared.
	if (*pKeyPress == KeyPress(CAMKEY(ESCAPE)))
	{
		if ((CurrPenState == IS_MoveTo) || (CurrPenState == IS_DragTo))
		{
			ClearInternalState();
			return TRUE;
		}
	}

	// break at points op key short cut.
	if (*pKeyPress == KeyPress(CAMKEY(B)))
	{
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpBreakAtPoints));
		String_256	UIDesc;
		if (pOpDesc != NULL)
		{
			OpState	State = OpBreakAtPoints::GetState(&UIDesc, pOpDesc);
			if (!State.Greyed)
			{
				pOpDesc->Invoke();
				return TRUE;
			}
		}
	} 

	// if delete pressed, try to delete the selected points in this path
	if ((pKeyPress->GetVirtKey() == CAMKEY(DELETE)) || (*pKeyPress == KeyPress(CAMKEY(BACK))) ) 
	{
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpDeletePoints));
		String_256 UIDesc;
		if (pOpDesc != NULL)
		{
			OpState	State = OpDeletePoints::GetState(&UIDesc, NULL);
			if (!State.Greyed)
			{
				pOpDesc->Invoke();
				return TRUE;
			}
		}
	} 

	// If we get this far then the keypress wasn't handled
	return FALSE;
}


/********************************************************************************************

>	void PenTool::OnMouseMove(DocCoord coord, Spread* pSpread)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Inputs:		coord	 	-	The DocCoord of the point where the mouse has moved to
				pSpread 	- 	The spread in which the move occurred
				mods		-	which buttons/keys are down
	Outputs:	-
	Returns:	-
	Purpose:	This routine is called whenever the mouse moves while we're in the pen 
				tool. it sees what is under the pointer, and flips the cursor if clicking 
				will have a different effect. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void PenTool::OnMouseMove(DocCoord coord, Spread* pSpread, ClickModifiers mods)
{
	NodePath* pNodePath = NULL;
	Cursor* whichCursor = NULL;

	// find what type of click we will generate
	penclick WhatToDo = DetermineClickEffect(coord, pSpread, &pNodePath);

	String_256 StatusMsg("");
	GenerateStatusLineText(pNodePath, WhatToDo, &StatusMsg);
	GetApplication()->UpdateStatusBarText(&StatusMsg);

	switch (WhatToDo)
	{
		case PenNewPath:
		case PenAddSegment:
		case PenEditInternalMove: 
			whichCursor = pcPenCursor;
			break;
		case PenOnPoint:
			whichCursor = pcMoveBezCursor;
			break;
		case PenClosePath:
			whichCursor = pcPenAdjustCursor;
			break;
		case PenReshapeLine:
			whichCursor = pcPenReshapeCursor;
			break;
		default:
			whichCursor = pcPenCursor;
	}
 	ChangeCursor(whichCursor);
}


/********************************************************************************************

>	virtual BOOL PenTool::GetStatusLineText(String_256* ptext,
											Spread* pSpread,
											DocCoord DocPos, 
											ClickModifiers ClickMods);

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/01/95
	Inputs:		pSpread   - pointer to spread under mouse (else NULL)
				DocPos    - position of mouse in doc (in spread coords)
				ClickMods - mouse click modifiers
	Outputs:	ptext - text for status line
	Returns:	TRUE if outputting valid text
	Purpose:	generate up-to-date text for the status line (called on idles)
	Errors:		ERROR3 if ptext is NULL

********************************************************************************************/

BOOL PenTool::GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord coord, ClickModifiers mods)
{
	ERROR1IF(ptext==NULL,FALSE,"PenTool::GetStatusLineText() passed a NULL text buffer");

	*ptext = "";

	NodePath* pNodePath = NULL;

	// find what type of click we will generate
	penclick WhatToDo = DetermineClickEffect(coord, pSpread, &pNodePath);
	GenerateStatusLineText(pNodePath, WhatToDo, ptext);

	return TRUE;
}


/********************************************************************************************

>	void PenTool::GenerateStatusLineText(NodePath* pNodePath, 
										 penclick WhatToDo,
										 String_256* pStatusMsg)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/01/95
	Inputs:		pNodePath	= pointer to a node path the mouse is over
							  (can be NULL)
				WhatToDo	= what action to be taken 
							  (return param of DetermineClickEffect)
				pStatusMsg	= pointer to a 256 character text buffer
				
	Outputs:	pStatusMsg	- holds a text string
	Returns:	-
	Purpose:	Loads up an appropriate help string into the text buffer
				pointed to by pStatusMsg, using the nodepath and 'whattodo'
				information passed.
	Errors:		-

********************************************************************************************/

void PenTool::GenerateStatusLineText(NodePath* pNodePath, penclick WhatToDo, String_256* pStatusMsg)
{
	switch (WhatToDo)
	{
		case PenNewPath:
			switch(CurrPenState)
			{
				case IS_MoveTo:
					pStatusMsg->Load(_R(IDS_PENADDPOINT), Tool::GetModuleID(GetID()));	
					break;
				case IS_DragTo:
					pStatusMsg->Load(_R(IDS_PENADDCURVE), Tool::GetModuleID(GetID()));	
					break;
				default:
					pStatusMsg->Load(_R(IDS_PENNEWPATH), Tool::GetModuleID(GetID()));
					break;
			}
			break;

		case PenAddSegment:
			if (pNodePath != NULL)
			{
				PathVerb  Verb  = (pNodePath->InkPath).GetVerb();
				PathFlags Flags = (pNodePath->InkPath).GetFlags();
				switch (Verb)
				{
					case PT_BEZIERTO:
						if (Flags.IsRotate)
							pStatusMsg->Load(_R(IDS_PENADDCURVE), Tool::GetModuleID(GetID()));
						else
							pStatusMsg->Load(_R(IDS_PENADDPOINT), Tool::GetModuleID(GetID()));
						break;
					default:
						pStatusMsg->Load(_R(IDS_PENADDPOINT), Tool::GetModuleID(GetID()));
						break;
				}
			}
			else
				pStatusMsg->Load(_R(IDS_PENADDPOINT), Tool::GetModuleID(GetID()));
			break;

		case PenEditInternalMove: 
			pStatusMsg->Load(_R(IDS_PENEDITINT), Tool::GetModuleID(GetID()));
			break;
			
		case PenClosePath:
			pStatusMsg->Load(_R(IDS_PENCLOSEPATH), Tool::GetModuleID(GetID()));
			break;

		case PenReshapeLine:
			pStatusMsg->Load(_R(IDS_RESHAPE_LINE),Tool::GetModuleID(GetID()));
			break;

		case PenOnPoint:
			if (pNodePath != NULL)
			{
				PathFlags Flags = (pNodePath->InkPath).GetFlags();
				if (Flags.IsSelected)
					pStatusMsg->Load(_R(IDS_PENONSELPOINT), Tool::GetModuleID(GetID()));
				else
					pStatusMsg->Load(_R(IDS_PENONPOINT), Tool::GetModuleID(GetID()));
			}
			break;
			
		default:
			pStatusMsg->Load(_R(IDS_PENADDPOINT), Tool::GetModuleID(GetID()));
			break;
	}
}


/********************************************************************************************

>	void PenTool::OverPathEnd(DocCoord coord, Spread* pSpread, NodePath* pNodePath)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/94
	Inputs:		coordinate of mouse move, pointer to spread containing coord
	Outputs:	pointer to path mouse point is over. The current position is set to
				the end point on the path under the mouse.
	Returns:	BOOL - TRUE if the coord is over an open path end point
	Purpose:	This routine scans the specified spread for all path objects and checks
				whether the mouse coord is over an open end point. It can be used to work
				out whether to change the mouse shape or not.
				This routine should really exist in some other file, available to all tools
				but I dont know where to put it yet so it shall have to stay here for the
				mo.
	Errors:		-
	SeeAlso:	GetCoord() - this will return you the coord of the element under the mouse

********************************************************************************************/
/*
BOOL PenTool::OverPathEnd(DocCoord coord, Spread* pSpread, NodePath** pOverNode)
{
	// First, get a pointer to the DocView so we can do blob proximity checks
	DocView* pDocView = DocView::GetCurrent();
	ENSURE( pDocView != NULL, "OverPathEnd: Can't find current DocView");
	BOOL OverEndPoint = FALSE;

	if (pDocView==NULL)
		return OverEndPoint;

	// Find the Rect round the mouse pos that counts as a hit
	DocRect BlobRect;
	OSRenderRegion::GetBlobRect(pDocView->GetViewScale(), coord, BT_SELECTEDLARGEST, &BlobRect);

	// Find the selected range of objects
	SelRange* Selected = GetApplication()->FindSelection();
	Node* pNode = Selected->FindFirst();

	if (pNode != NULL)
	{
		Spread* NodeSpread = pNode->FindParentSpread();
		if (NodeSpread == pSpread)
		{
			// On the same spread, so see if the pointer is over an endpoint
			while ((pNode != NULL) && !OverEndPoint)
			{
				if (pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodePath))
				{
					// Now we know it's a NodePath, get a pointer to the Path object within it, 
					// so we can find any endpoints
					Path* ThisPath = &(((NodePath*)pNode)->InkPath);
					
					// See if it touches any point in this path
					INT32 CloseSlot = 0;
					if (ThisPath->IsNearOpenEnd(BlobRect, &CloseSlot))
					{
						// Set a few other details worth remembering
						ThisPath->SetPathPosition(CloseSlot);
						OverEndPoint = TRUE;
						*pOverNode = (NodePath*)pNode;
					}
            	}
				// Now find the next selected node
				pNode = Selected->FindNext(pNode);
			}
		}
	}
	return OverEndPoint;
}
*/


/********************************************************************************************

>	void PenTool::RemoveSelection(DocCoord PointerPos, Spread *pSpread, NodePath* pNodePath=NULL)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		PointerPos	= The position of the mouse click
				pSpread 	= The spread that the occured on
				pNodePath	= A pointer to a particular node path to leave out (defaults to
							  NULL)
	Purpose:

********************************************************************************************/

void PenTool::RemoveSelection( DocCoord PointerPos, Spread *pSpread, NodePath* pNodePath )
{																    
	// deselect any points within the spread the click occured over
	SelRange* Selected = GetApplication()->FindSelection();
	Node* pNode = Selected->FindFirst();
	if (pNode && pNode->FindParentSpread() == pSpread )
	{
		while (pNode)
		{
			if (pNode!=pNodePath)
			{
				if (pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodePath))
				{
					Path* ThisPath = &(((NodePath*)pNode)->InkPath);
					PathFlags* Flags = ThisPath->GetFlagArray();
					INT32 UsedSlots = ThisPath->GetNumCoords();

					BOOL selected = FALSE;
					for (INT32 i=0; i<UsedSlots; i++)
					{
						if (Flags[i].IsSelected)
						{
							if (!selected)
							{
								// right, red selection blobs off!
								ThisPath->RenderPathPenBlobs(pSpread);
								selected = TRUE;
							}
							Flags[i].IsSelected=FALSE;
						}
					}
					// now, black selection blobs back on!
					if (selected)
						ThisPath->RenderPathPenBlobs(pSpread);
				}
			}
			pNode = Selected->FindNext(pNode);
		}
	}
}




/********************************************************************************************

>	void PenTool::RenderToolBlobs(Spread* pSpread, DocRect* pClipRect)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Inputs:		pSpread - The spread that the blob is to appear on
				pClipRect - Pointer to the rect that contains the blobs (Can be NULL)
	Purpose:	The blobs this tool renders are
				(1) a moveto floating endpoint
				(2) a dragto floating endpoint
				These are usually rendered after the first click/drag on a document

********************************************************************************************/

void PenTool::RenderToolBlobs(Spread* pSpread, DocRect* pClipRect)
{
	// Can only draw the path if there is a path to draw
	ERROR3IF(pSpread==NULL,"PenTool::RenderToolBlobs() called with a null spread");

	RenderRegion* pRegion;
	DocCoord LastMove = EditHandles.HndClick;
	DocCoord LastDrag = EditHandles.HndDrag;

	switch (CurrPenState)
	{
		case IS_MoveTo:
			pRegion = DocView::RenderOnTop(pClipRect, pSpread, ClippedEOR);
			while (pRegion)
			{
				// Draw a moveto blob
				pRegion->SetLineColour(COLOUR_BEZIERBLOB);
				pRegion->SetFillColour(COLOUR_TRANS);
				pRegion->DrawBlob(LastMove,BT_SELECTED);
				
				// Get the next region in the list
				pRegion = DocView::GetNextOnTop(pClipRect);
			}
			break;

 		case IS_DragTo:
			{
				pRegion = DocView::RenderOnTop(pClipRect, pSpread, ClippedEOR);
				DocCoord OtherDrag;
				OtherDrag.x = LastMove.x - (LastDrag.x - LastMove.x);
				OtherDrag.y = LastMove.y - (LastDrag.y - LastMove.y);
	
				while (pRegion)
				{
					// Draw three end blobs and an eor'd dotted line
					pRegion->SetLineColour(COLOUR_BEZIERLINE);
					pRegion->DrawLine(OtherDrag,LastDrag);
					pRegion->SetLineColour(COLOUR_BEZIERBLOB);
					pRegion->SetFillColour(COLOUR_TRANS);
					pRegion->DrawBlob(LastMove,BT_SELECTED);
					pRegion->SetFillColour(COLOUR_UNSELECTEDBLOB);
					pRegion->SetLineColour(COLOUR_TRANS);
					pRegion->DrawBlob(LastDrag,BT_UNSELECTED);
					pRegion->DrawBlob(OtherDrag,BT_UNSELECTED);
	
					// Get the next region in the list
					pRegion = DocView::GetNextOnTop(pClipRect);
				}
				break;
			}
		default:
			break;
	}	
}





/********************************************************************************************

>	void PenTool::ChangeCursor(Cursor* cursor)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Inputs:		ID of the cursor you want to flip to
	Outputs:	-
	Returns:	-
	Purpose:	Changes to the specified cursor. Will only change the cursor if it isn't 
				already	this cursor, so it doesn't flicker.
	Errors:		can fail if the cursor cannot be created - the cursor code will fail.
	SeeAlso:	-

********************************************************************************************/

void PenTool::ChangeCursor(Cursor* cursor)
{
	// only change if this cursor is different from the current cursor
	if (cursor != MyCurrentCursor)
	{
		// set this cursor as the current cursor and immediately display it
		CursorStack::GSetTop(cursor, CurrentCursorID);
		// remember this is our current cursor
		MyCurrentCursor = cursor;
	}
}




/********************************************************************************************

>	void PenTool::SetInternalState()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function is called from an operation which tells the tool that there
				should be a virtual moveto coordinate and dragto coordinate set within the
				tool. If the dragto is equal to the moveto, then an internal move should be
				created, rather than an internal drag.

********************************************************************************************/

void PenTool::SetInternalState()
{
	// Check that the handles are actually defined.
	if (EditHandles.pHndSpread==NULL)
		return;

	if (EditHandles.HndClick == EditHandles.HndDrag)
		CurrPenState = IS_MoveTo;
	else
		CurrPenState = IS_DragTo;

	// Now display the blobs
	BlobManager* BlobMgr = GetApplication()->GetBlobManager();
	ENSURE(BlobMgr!=NULL, "Blob Manager was not there");
	BlobMgr->RenderToolBlobsOn(this, EditHandles.pHndSpread, NULL);

}

/********************************************************************************************

>	void PenTool::ClearInternalState()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Clears the internal condition.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void PenTool::ClearInternalState()
{
	if (CurrPenState == IS_MoveTo || CurrPenState == IS_DragTo)
	{
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		ENSURE(BlobMgr!=NULL, "Blob Manager was not there");
		BlobMgr->RenderToolBlobsOff(this, EditHandles.pHndSpread,NULL);
	}
	CurrPenState = IS_Undefined;
}




/********************************************************************************************

>	void PenTool::ClearPath()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Throw away the internal data of the pen path
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void PenTool::ClearPath()
{
	// call the path clearing function!

	ClearInternalState();
	EditPath.ClearPath();
}

/********************************************************************************************

>	void PenTool::ClearOp()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Inputs:		-
	Purpose:	Throw away the internal op state we've been holding

********************************************************************************************/

void PenTool::ClearOp()
{
	CurrPenOpState = OS_Undefined;
}


/********************************************************************************************

>	penopstate PenTool::GetPenOpState()	const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Inputs:		-
	Purpose:	return the operation state the pen tool thinks its in.

********************************************************************************************/

penopstate PenTool::GetPenOpState()	const
{
	return (CurrPenOpState);
}


/********************************************************************************************

>	void PenTool::CreateNewPath()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Once a drag has finished we need to build a new path with the data
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void PenTool::CreateNewPath()
{
	// Ok, the last drag has altered our internal drag path. We now
	// have a path ready to add to the tree, so we need to get on with it!
	OpAddNewPath* pOpAddNewPath = new OpAddNewPath;
	pOpAddNewPath->DoAddNewPath(&EditPath, EditHandles.pHndSpread);
}



/********************************************************************************************

>	void PenTool::AddElementToPath()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Once a drag has finished we need to add the newly edited element to the
				specified path
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void PenTool::AddElementToPath()
{
	// Right, the last drag has created a new path section for me
	// I need to add it in to the specified nodepath object

	OpAddPathToPath* pOpAddPathToPath = new OpAddPathToPath;
	pOpAddPathToPath->DoAddPathToPath(pNodePath, &EditPath, NodeIndex);
}


/********************************************************************************************

>	void PenTool::CloseWithPath()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Once a drag has finished we need to add the newly edited element to the
				specified path
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void PenTool::CloseWithPath()
{
	// Right, the last drag has closed a path.
	// I need to add it in to the specified nodepath object

	OpClosePathWithPath* pOpClosePathWithPath = new OpClosePathWithPath;
	pOpClosePathWithPath->DoClosePathWithPath(pNodePath, &EditPath, NodeIndex);
}

