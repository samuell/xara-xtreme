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
// Implementation of the Regular Shape tool

/*
*/

#include "camtypes.h"
#include "regshape.h"	

#include "app.h"
#include "attrmgr.h"
#include "blobs.h"
#include "csrstack.h"
#include "document.h"
#include "docview.h"
#include "finfodlg.h"
//#include "justin.h"
//#include "markn.h"
#include "nodershp.h"
#include "oilfiles.h"
#include "opnudge.h"
#include "oprshape.h"
#include "optsmsgs.h"
//#include "peter.h"		
#include "rndrgn.h"
//#include "rshapbar.h"
//#include "simon.h"
#include "shapeops.h"
#include "spread.h"
//#include "viewrc.h"	 
//#include "will2.h"

DECLARE_SOURCE( "$Revision$" );

CC_IMPLEMENT_MEMDUMP(QuickShapeBase, Tool_v1)
CC_IMPLEMENT_MEMDUMP(QuickShapeTool, QuickShapeBase)
CC_IMPLEMENT_DYNCREATE(QuickShapeBaseInfoBarOp, InformationBarOp)	  

// Must come after the last CC_IMPLEMENT.. macro
#define new CAM_DEBUG_NEW     

TCHAR* QuickShapeTool::FamilyName	= _T("Shape Tools");
TCHAR* QuickShapeTool::ToolName		= _T("QuickShape Tool");
TCHAR* QuickShapeTool::Purpose		= _T("Regular Shape manipulation");
TCHAR* QuickShapeTool::Author		= _T("Peter Arnold");

INT32 QuickShapeTool::CreationMode = OpNewRegShape::RADIUS;
INT32 QuickShapeTool::NumSides = 6;
BOOL QuickShapeTool::CreatePolygons = TRUE;
BOOL QuickShapeTool::CreateStellated = FALSE;
BOOL QuickShapeTool::CreateCurved = FALSE;



/********************************************************************************************

>	QuickShapeBase::QuickShapeBase()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Purpose:	Default Constructor.
				Other initialisation is done in QuickShapeBase::Init which is called by the
				Tool Manager
	SeeAlso:	QuickShapeBase::Init

********************************************************************************************/

QuickShapeBase::QuickShapeBase()
{
	pcCurrentCursor = NULL;
	OldDetermineSpread = NULL;
	pQuickShapeBaseInfoBarOp = NULL;
	EditFieldMode = QuickShapeBase::EDITF_CENTRE;
}



/********************************************************************************************

>	QuickShapeBase::~QuickShapeBase()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Purpose:	Destructor (Virtual). Does nothing.

********************************************************************************************/

QuickShapeBase::~QuickShapeBase()
{
	// Set the infobar,s pointer to us to NULL as we are expiring
	pQuickShapeBaseInfoBarOp->pQuickShapeBase = NULL;
}



/********************************************************************************************

>	BOOL QuickShapeBase::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Returns:	FALSE if it does not want to be created, TRUE otherwise
	Purpose:	Used to finish construction and to check if the Tool was properly constructed.
	Errors:		ERROR2's if the resource file or the bar could not be found/opened/read
	SeeAlso:	QuickShapeBase::QuickShapeBase

********************************************************************************************/

BOOL QuickShapeBase::Init()
{
	// Declare all your ops here and only succeed if all declarations succeed

	BOOL ok = OpNewRegShape::Declare();

	// This section reads in the infobar definition and creates an instance of
	// QuickShapeBaseInfoBarOp.  Also pQuickShapeBaseInfoBarOp, the ptr to the tool's infobar, is set up
	// after the infobar is successfully read and created.
	if (ok)
	{
		pQuickShapeBaseInfoBarOp = new QuickShapeBaseInfoBarOp(this, _R(IDD_REGSHAPETOOLBAR));
		ok=(pQuickShapeBaseInfoBarOp!=NULL);
PORTNOTE("dialog", "Removed Bar reading")
#if 0
		CCResTextFile 			file;				// Resource File
		QuickShapeBaseInfoBarOpCreate BarCreate;			// Object that creates QuickShapeBaseInfoBarOp objects

		 		ok = file.open(_R(IDM_REGSHAPE_BAR), _R(IDT_INFO_BAR_RES));	// Open resource
		if (ok) ok = DialogBarOp::ReadBarsFromFile(file,BarCreate);	// Read and create info bar
		if (ok) file.close();									 	// Close resource

		ERROR2IF(!ok,FALSE,"Unable to load RegShBar.ini from resource"); 

		// Info bar now exists.  Now get a pointer to it
		String_32 str(_R(IDS_REGSHAPE_INFOBARNAME));
		DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(str);
		
		ERROR2IF(pDialogBarOp==NULL, FALSE, "infobar not found\n");

		ok = pDialogBarOp->IsKindOf(CC_RUNTIME_CLASS(QuickShapeBaseInfoBarOp));
		if (ok)
		{
			pQuickShapeBaseInfoBarOp = (QuickShapeBaseInfoBarOp*)pDialogBarOp;
			pQuickShapeBaseInfoBarOp->pQuickShapeBase = this;
		}
#endif
		ERROR2IF(!ok,FALSE,"Error finding the RegShape tool info bar");

	}

	return (ok);
}



/********************************************************************************************

>	virtual void QuickShapeBase::SelectChange(BOOL isSelected)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Inputs:		isSelected	- TRUE  = tool has been selected
							- FALSE = tool has been deselected
	Outputs:	-
	Returns:	-
	Purpose:	Starts up and closes down the RegShape tool
	SeeAlso:	-

********************************************************************************************/

void QuickShapeBase::SelectChange(BOOL isSelected)
{
	if (isSelected)
	{
		if (!CreateCursors()) return;
		CurrentCursorID = CursorStack::GPush(pcNormalCursor, FALSE);		// Push cursor but don't display now
		pcCurrentCursor = pcNormalCursor;

		// Create and display the tool's info bar
		pQuickShapeBaseInfoBarOp->Create();
		pQuickShapeBaseInfoBarOp->SetCreationMode(GetCreationMode());
		pQuickShapeBaseInfoBarOp->SetHelp();
		
		// Say which blobs to display
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		if (BlobMgr != NULL)
		{
			// Decide which blobs we will display
			BlobStyle MyBlobs;
			MyBlobs.Object = TRUE;

			// Tell the blob manager
			BlobMgr->ToolInterest(MyBlobs);
		}
	}
	else
	{
		// Deselection - destroy the tool's cursors, if they exist.
		if (pcCurrentCursor != NULL)
		{
			CursorStack::GPop(CurrentCursorID);
			CurrentCursorID = 0;
			pcCurrentCursor = NULL;
		}
		DestroyCursors();

		// Remove the info bar from view by deleting the actual underlying window
		pQuickShapeBaseInfoBarOp->Delete();
	}

		// ensure any tool object blobs are removed.
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		if (BlobMgr != NULL)
		{
			BlobStyle bsRemoves;
			bsRemoves.ToolObject = TRUE;
			BlobMgr->RemoveInterest(bsRemoves);
		}
}



/********************************************************************************************

>	BOOL QuickShapeBase::CreateCursors()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if all the RegShape tool cursors have been successfully created
	Purpose:	Creates all the RegShape tool cursors
	SeeAlso:	-

********************************************************************************************/

BOOL QuickShapeBase::CreateCursors()
{
	// This tool has just been selected.  Create the cursors.
	pcNormalCursor = new Cursor(this, GetCursorID());
	pcBlobCursor = new Cursor(this, _R(IDC_MOVEBEZIERCURSOR));
	pcEdgeCursor = new Cursor(this, _R(IDC_RESHAPECURSOR));

	if ( pcNormalCursor==NULL || !pcNormalCursor->IsValid() ||
		 pcEdgeCursor==NULL || !pcEdgeCursor->IsValid() ||
		 pcBlobCursor==NULL || !pcBlobCursor->IsValid()	)
	{
		DestroyCursors();
		return FALSE;
	}
	else
		return TRUE;
}



/********************************************************************************************

>	void QuickShapeBase::DestroyCursors()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroys all the RegShape tool cursors
	SeeAlso:	-

********************************************************************************************/

void QuickShapeBase::DestroyCursors()
{
	if (pcNormalCursor != NULL) delete pcNormalCursor;
	if (pcEdgeCursor != NULL) delete pcEdgeCursor;
	if (pcBlobCursor != NULL) delete pcBlobCursor;

	pcNormalCursor = NULL;
	pcBlobCursor = NULL;
	pcEdgeCursor = NULL;
}



/********************************************************************************************

>	void QuickShapeBase::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Inputs:		PointerPos 	- 	The DocCoord of the point where the mouse button was clicked
				Click 		-	Describes the type of click that was detected. 
				ClickMods 	-	Indicates which buttons caused the click and which modifers were
								pressed at the same time
				pSpread 	- 	The spread in which the click happened
	Returns:	-
	Purpose:	To handle a Mouse Click event for the RegShape Tool.
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers

********************************************************************************************/
void QuickShapeBase::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )
{
	if (ClickMods.Menu) return;							// Don't do anything if the user clicked the Menu button

	NodeRegularShape* HitShape = NULL;
	ShapeClickEffect TestEffect = DetermineClickEffect(PointerPos, pSpread, &HitShape);

	// Get the infobar into the right mode so immediate drags are shown
	EditModeIDS NewMode;
	switch (TestEffect)
	{
		case SHAPECLICK_CENTRE:
			NewMode = EDITF_CENTRE;
			break;
		case SHAPECLICK_PRIMARY:
			NewMode = EDITF_MAJOR;
			break;
		case SHAPECLICK_ELLIPSE:
		case SHAPECLICK_RECTANGLE:
			NewMode = EDITF_SIZE;
			break;
		case SHAPECLICK_STELLATION:
			NewMode = EDITF_STELLATION;
			break;
		case SHAPECLICK_PRIMARYCURVE:
		case SHAPECLICK_STELLATIONCURVE:
			NewMode = EDITF_CURVE;
			break;
		default:
			NewMode = EditFieldMode;
	}
	if (NewMode != EditFieldMode)
	{
		EditFieldMode = NewMode;
		GetInfoBar()->BuildEditMenu(TRUE);
	}

	// If we have found a shape then pass the click onto the shape for it to deal with
	if (HitShape != NULL)
	{
		if (DoesPolygonExist())
			HitShape->OnToolClick(PointerPos, Click, ClickMods,	pSpread);
		else
			HitShape->OnClick(PointerPos, Click, ClickMods,	pSpread);
	}
	else
	{
		// call the base class ....
		
		DragTool::OnClick (PointerPos, Click, ClickMods, pSpread);
	}

	// Alternativly, Deal with creating new shapes
	if ( (HitShape == NULL) && (Click == CLICKTYPE_DRAG) )
	{
		// Just what we wanted - Someone is dragging the mouse about
		// We need to make an operation to perform the drag with
		OpNewRegShape* pOpNewRegShape = new OpNewRegShape;
		if (pOpNewRegShape == NULL)
		{
			// Inform the person doing the clicking that life is not looking so good
			InformError();
		}
		else
		{
			// Put the edit field into size mode
			EditModeIDS NewMode = EDITF_MAJOR;
			if (IsEllipse() || IsRectangle())
				NewMode = EDITF_SIZE;
			if (NewMode != EditFieldMode)
			{
				EditFieldMode = NewMode;
				GetInfoBar()->BuildEditMenu(TRUE);
			}

			// Start the drag operation and pass in the Anchor
			INT32 NumberOfSides = GetNumSides();
			OpNewRegShape::CreateMode CreateMode = (OpNewRegShape::CreateMode)GetCreationMode();
			BOOL PolyGone = !GetPolygon();
			BOOL Stellation = GetStellation();
			BOOL Curvature = GetCurved();
			// Possibly use the current infobar settings
			if (AttributeManager::LastAttrAppliedBecomesCurrent && !IsEllipse())
			{
				if (!IsRectangle())
				{
					NumberOfSides = pQuickShapeBaseInfoBarOp->GetNumSides(FALSE);
					if (NumberOfSides < 3)
						NumberOfSides = GetNumSides();
					PolyGone = !pQuickShapeBaseInfoBarOp->IsPolygonCreate();
					Stellation = pQuickShapeBaseInfoBarOp->IsStellationCreate();
				}
				Curvature = pQuickShapeBaseInfoBarOp->IsCurvedCreate();
			}

			pOpNewRegShape->DoDrag( pSpread, PointerPos, NumberOfSides, CreateMode, PolyGone, Stellation, Curvature); 
			DialogBarOp::SetSystemStateChanged();
		}
	}
}



/********************************************************************************************

>	void QuickShapeBase::OnMouseMove( DocCoord PointerPos,Spread* pSpread, ClickModifiers ClickMod )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Inputs:		PointerPos 	- 	The DocCoord of the point where the mouse has moved to
				pSpread 	- 	The spread in which the move occurred
				ClickMods	- 	The state of the various modifiers at the time of the mouse move
	Returns:	TRUE if it handled the Click, FALSE otherwise
	Purpose:	To handle a Mouse Move event for the RegShape Tool.
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers

********************************************************************************************/
void QuickShapeBase::OnMouseMove(DocCoord PointerPos,Spread* pSpread,ClickModifiers ClickMods)
{
	// If we're over a blob then show the move blob cursor
	NodeRegularShape* HitShape;
	ShapeClickEffect TestEffect = DetermineClickEffect(PointerPos, pSpread, &HitShape);

	Cursor* DisplayCur = pcNormalCursor;
	if (TestEffect != SHAPECLICK_NONE)
		DisplayCur = pcBlobCursor;

	if ( ((TestEffect == SHAPECLICK_EDGE1) || (TestEffect == SHAPECLICK_EDGE2)) && !HitShape->IsCircular())
		DisplayCur = pcEdgeCursor;

	if ( (DisplayCur != pcCurrentCursor) && (DisplayCur != NULL) )
	{
		pcCurrentCursor = DisplayCur;
		CursorStack::GSetTop(pcCurrentCursor, CurrentCursorID);
	}

	// Display status line help text for the current position
	DisplayStatusBarHelp(PointerPos, pSpread, ClickMods);
}



/********************************************************************************************

>	void QuickShapeBase::DisplayStatusBarHelp(DocCoord DocPos, Spread* pSpread, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Inputs:		Point, Spread and click modifiers
	Outputs:	-
	Returns:	-
	Purpose:	Displays status help string for the given position in the status bar
	SeeAlso:	QuickShapeBase::GetCurrentStatusLineText

********************************************************************************************/

void QuickShapeBase::DisplayStatusBarHelp(DocCoord DocPos, Spread* pSpread, ClickModifiers ClickMods)
{
	String_256 StatusMsg("");

	GetCurrentStatusLineText(&StatusMsg, pSpread, DocPos, ClickMods);
	GetApplication()->UpdateStatusBarText(&StatusMsg);							 
}



/********************************************************************************************

>	virtual BOOL QuickShapeBase::GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		pSpread points to a spread
				DocPos points to a point in a document
				ClickMods are the current click modifiers
	Outputs:	Updates the string in ptext
	Returns:	TRUE
	Purpose:	Returns the current status line help string
	SeeAlso:	QuickShapeBase::GetCurrentStatusLineText

********************************************************************************************/
BOOL QuickShapeBase::GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)
{
	GetCurrentStatusLineText(ptext, pSpread, DocPos, ClickMods);
	return TRUE;
}



/********************************************************************************************

>	void QuickShapeBase::GetCurrentStatusLineText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		pSpread points to a spread
				DocPos points to a point in a document
				ClickMods are the current click modifiers
	Outputs:	Updates the string in ptext
	Returns:	-
	Purpose:	QuickShapeBase::GetStatusLineText

********************************************************************************************/
void QuickShapeBase::GetCurrentStatusLineText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)
{
	NodeRegularShape* pShape;
	switch (DetermineClickEffect(DocPos, pSpread, &pShape))
	{
		case SHAPECLICK_NONE:
			ExpandShapeString(ptext, _R(IDS_REGSHAPETOOL_CREATENEW), TRUE);
			break;
		case SHAPECLICK_RECTANGLE:
		case SHAPECLICK_PRIMARY:
		{
			String_32 shape;
			if (pShape->IsPrimaryCurvature() || pShape->IsStellationCurvature())
				shape.Load(_R(IDS_REGSHAPETOOL_RCURVE));
			else			
				shape.Load(_R(IDS_REGSHAPETOOL_ACURVE));
			ptext->MakeMsg(_R(IDS_REGSHAPETOOL_DRAGPVERTEX), (TCHAR*)shape);
			break;
		}
		case SHAPECLICK_ELLIPSE:
		{
			ptext->Load(_R(IDS_REGSHAPETOOL_DRAGELLIPSE));
			break;
		}
		case SHAPECLICK_STELLATION:
		{
			String_32 shape;
			if (pShape->IsPrimaryCurvature() || pShape->IsStellationCurvature())
				shape.Load(_R(IDS_REGSHAPETOOL_RCURVE));
			else			
				shape.Load(_R(IDS_REGSHAPETOOL_ACURVE));
			ptext->MakeMsg(_R(IDS_REGSHAPETOOL_DRAGSVERTEX), (TCHAR*)shape);
			break;
		}
		case SHAPECLICK_PRIMARYCURVE:
		case SHAPECLICK_STELLATIONCURVE:
			ptext->Load(_R(IDS_REGSHAPETOOL_DRAGCURVE));
			break;
		case SHAPECLICK_CENTRE:
		{
			String_32 shape;
			if (pShape->IsCircular())
				shape.Load(_R(IDS_REGSHAPETOOL_SPOLYGON));
			else			
				shape.Load(_R(IDS_REGSHAPETOOL_SELLIPSE));
			ptext->MakeMsg(_R(IDS_REGSHAPETOOL_DRAGCENTRE), (TCHAR*)shape);
			break;
		}
		case SHAPECLICK_EDGE1:
		case SHAPECLICK_EDGE2:
		{
			String_32 extra;
			INT32 ID;

			if (pShape->IsCircular())
				ID = _R(IDS_REGSHAPETOOL_DRAGEDGE1);
			else			
				ID = _R(IDS_REGSHAPETOOL_DRAGEDGE2);

			if (pShape->IsStellated())
				extra.Load(_R(IDS_REGSHAPETOOL_RSTELL));
			else			
				extra.Load(_R(IDS_REGSHAPETOOL_SSTELL));

			ptext->MakeMsg(ID, (TCHAR*)extra);
			break;
		}
		default:
			break;
	}
}



/*******************************************************************************************

>	void QuickShapeBase::ExpandShapeString(StringBase* pString, INT32 StringID, BOOL Singular)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/03/95
	Inputs:		pString - points to the string to store the expanded result in
				Singular - TRUE/FALSE for singular/plural
				StringID - the resource ID of the string to expand
	Outputs:	pString will be expanded to hold the name of the QuickShape being edited
	Returns:	-
	Purpose:	Insert the name of the QuickShape the tool is currently creating into
				position in the string
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void QuickShapeBase::ExpandShapeString(StringBase* pString, INT32 StringID, BOOL Singular)
{
	String_32 Name;
	if (Singular)
		Name.Load(GetShapeID());
	else
		Name.Load(GetShapesID());

	pString->MakeMsg(StringID, (TCHAR*)Name);
}



/*******************************************************************************************

> ShapeClickEffect QuickShapeBase::DetermineClickEffect(DocCoord PointerPos, Spread* pSpread, NodeRegularShape** ReturnShape = NULL)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		PointerPos is the mouse position
				pSpread is a pointer to the spread containing the mouse position
	Outputs:	ReturnShape returns a pointer to the shape the mouse would click on (NULL is
				returned if there is no excited shape)
	Returns:	The effect of clicking.
	Purpose:	Used when the cursor moves and when single clicking. This routine determines
				what effect a click will have by looking at the object under the pointer.
				The last position and result is cached so we can avoid excessive document scans
	Errors:		ERROR2 if there is no current DocView
	SeeAlso:	ShapeClickEffect

********************************************************************************************/

ShapeClickEffect QuickShapeBase::DetermineClickEffect(DocCoord PointerPos, Spread* pSpread, NodeRegularShape** ReturnShape)
{
//	// See if we can supply the result from the cache.
//	if ((OldDeterminePos == PointerPos) && (OldDetermineSpread == pSpread) && IsInterestingShape(OldDetermineShape))
//	{
//		*ReturnShape = OldDetermineShape;
//		return OldDetermineResult;
//	}

	OldDeterminePos = PointerPos;
	OldDetermineSpread = pSpread;
	ShapeClickEffect WhatToDo = SHAPECLICK_NONE;

	// Scan through the selected shapes, and see if any of them want the click
	DocRect BlobRect;
	DocView* pDocView = DocView::GetCurrent();
	ERROR2IF( pDocView==NULL, WhatToDo, "QuickShapeBase::DetermineClickEffect: Can't find current DocView");

	// Find the selected range of objects
	SelRange* Selected = GetApplication()->FindSelection();
	Node* pNode = Selected->FindFirst();

	while (pNode != NULL)
	{
		if (IS_A(pNode, NodeRegularShape) && IsInterestingShape((NodeRegularShape*)pNode))
		{
			NodeRegularShape* CurrentHit = (NodeRegularShape*) pNode;
			INT32 ClickPointNumber;
			WhatToDo = CurrentHit->DetermineClickEffect(&PointerPos, pSpread, &ClickPointNumber);

			// Ignore edge reforming if the tool shouldn't do it
			if ( ( (WhatToDo ==	SHAPECLICK_EDGE1) || (WhatToDo == SHAPECLICK_EDGE2) ) && !CanReformEdges())
				WhatToDo = SHAPECLICK_NONE; 

			if (WhatToDo != SHAPECLICK_NONE)
			{
				if (ReturnShape != NULL)
					*ReturnShape = CurrentHit;
				OldDetermineShape = CurrentHit;
				return (OldDetermineResult = WhatToDo);
			}
    	}

		// Now find the next selected node
		pNode = Selected->FindNext(pNode);
	}

	// Didn't find anything
	if (ReturnShape != NULL)
		*ReturnShape = NULL;
	OldDetermineShape = NULL;
	return (OldDetermineResult = SHAPECLICK_NONE);
}



/********************************************************************************************
>	MsgResult QuickShapeBaseInfoBarOp::Message(Msg* Message) 

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Inputs:		Msg = The message to handle
	Outputs:	-
	Returns:	-
	Purpose:	RegShape info bar message handler
	Errors:		-
	SeeAlso:	QuickShapeBaseInfoBarOp::DialogMessage
********************************************************************************************/
MsgResult QuickShapeBaseInfoBarOp::Message(Msg* Message) 
{
	BOOL UpdateInfo = FALSE;
	
	if (IS_OUR_DIALOG_MSG(Message))
	{
		MsgResult Result = DialogMessage((DialogMsg*)Message);
		if (Result != OK)
			return Result;
	}
	else if (MESSAGE_IS_A(Message,OpMsg)) 	// Update the bar after an operation has finished
	{
		if ( ((OpMsg*)Message)->MsgType == OpMsg::END) 
			UpdateInfo = TRUE;
	}
	else if (MESSAGE_IS_A(Message,UnitMsg)) 	// Update the bar as someone has changd a unit
	{
		UpdateInfo = TRUE;
	}
	else if (MESSAGE_IS_A(Message, OptionsChangingMsg))	// Or a units-changing message?
	{
		if (((OptionsChangingMsg*)Message)->State == OptionsChangingMsg::NEWUNITS)
			UpdateInfo = TRUE;
	}
	else if (MESSAGE_IS_A(Message,SelChangingMsg)) 	// Update the bar after the selection has changed
	{
		if ( ((SelChangingMsg*)Message)->State == SelChangingMsg::SELECTIONCHANGED) 
			UpdateInfo = TRUE;
	}
	else if (MESSAGE_IS_A(Message,DocChangingMsg)) 	// Update the bar after an operation has finished
	{
		if ( ((DocChangingMsg*)Message)->State == DocChangingMsg::SELCHANGED) 
			UpdateInfo = TRUE;
	}
	else if (MESSAGE_IS_A(Message,ShapeEditedMsg)) 	// Update the bar during shape drag
	{
		UpdateEditFields(((ShapeEditedMsg*)Message)->pShape, ((ShapeEditedMsg*)Message)->pSpread);
	}

	if (UpdateInfo) 
	{
		UpdateInfobar();
		BuildEditMenu();
	}

	// Pass the message on to our parent class
	return (InformationBarOp::Message(Message));
}    



/********************************************************************************************
>	MsgResult QuickShapeBaseInfoBarOp::DialogMessage(DialogMsg* Msg)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/95
	Inputs:		Msg = The message to handle
	Outputs:	-
	Returns:	Message Result
	Purpose:	RegShape info bar dialog message handler (deals with dialog specifc messages)
	Errors:		-
	SeeAlso:	QuickShapeBaseInfoBarOp::Message
********************************************************************************************/
MsgResult QuickShapeBaseInfoBarOp::DialogMessage(DialogMsg* Msg)
{
	Path NewPath;
	// Check if the message is a CANCEL
	if (Msg->DlgMsg == DIM_CANCEL)
	{
		Close(); // Close the dialog 
	}
	else if (Msg->DlgMsg == DIM_CREATE)
	{
		InitialiseInfobar();
	}
	else
	{
		BOOL	CauseShapeEdit = FALSE;
		EditRegularShapeParam ChangeData(NULL);
		if (pQuickShapeBase!=NULL)
			ChangeData.ShapesToAffect = (EditRegularShapeParam::AffectShape)pQuickShapeBase->GetShapesToAffect();

		if (Msg->GadgetID == _R(IDC_BTN_REGSHAPETOOL_RADIUS))
		{
			if (Msg->DlgMsg==DIM_LFT_BN_CLICKED && pQuickShapeBase!=NULL)
			{
				pQuickShapeBase->SetCreationMode(OpNewRegShape::RADIUS);
				SetCreationMode(OpNewRegShape::RADIUS);
			}
		}
		else if (Msg->GadgetID == _R(IDC_BTN_REGSHAPETOOL_DIAMETER))
		{
			if (Msg->DlgMsg==DIM_LFT_BN_CLICKED && pQuickShapeBase!=NULL)
			{
				pQuickShapeBase->SetCreationMode(OpNewRegShape::DIAMETER);
				SetCreationMode(OpNewRegShape::DIAMETER);
			}
		}
		else if (Msg->GadgetID == _R(IDC_BTN_REGSHAPETOOL_BOUNDBOX))
		{
			if (Msg->DlgMsg==DIM_LFT_BN_CLICKED && pQuickShapeBase!=NULL)
			{
				pQuickShapeBase->SetCreationMode(OpNewRegShape::BOUNDS);
				SetCreationMode(OpNewRegShape::BOUNDS);
			}
		}			
		else if (Msg->GadgetID == _R(IDC_BTN_REGSHAPETOOL_STELLATION))
		{
			if (Msg->DlgMsg==DIM_LFT_BN_CLICKED && pQuickShapeBase!=NULL)
			{
				BOOL NewStellSetting = IsStellationCreate();
				pQuickShapeBase->SetStellation(NewStellSetting);
				// Pop the button out as the bitmap buttons are stupid
				if (!NewStellSetting)
				{
					SetLongGadgetValue(_R(IDC_BTN_REGSHAPETOOL_STELLATION), FALSE);
				}
				if (ChangeMode)
				{
					// Set the operation parameters
					if (NewStellSetting)
						ChangeData.NewStellated = EditRegularShapeParam::CHANGE_SETTRUE;
					else
						ChangeData.NewStellated = EditRegularShapeParam::CHANGE_SETFALSE;
					CauseShapeEdit = TRUE;
				}
			}
		}
		else if (Msg->GadgetID ==_R(IDC_BTN_REGSHAPETOOL_CURVATURE))
		{
			if (Msg->DlgMsg==DIM_LFT_BN_CLICKED && pQuickShapeBase!=NULL)
			{
				BOOL NewCurveSetting = IsCurvedCreate();
				pQuickShapeBase->SetCurved(NewCurveSetting);
				// Pop the button out as the bitmap buttons are stupid
				if (!NewCurveSetting)
				{
					SetLongGadgetValue(_R(IDC_BTN_REGSHAPETOOL_CURVATURE), FALSE);
				}
				if (ChangeMode)
				{
					// Set the operation parameters
					if (NewCurveSetting)
					{
						ChangeData.NewPrimaryCurvature = EditRegularShapeParam::CHANGE_SETTRUE;
						ChangeData.NewStellationCurvature = EditRegularShapeParam::CHANGE_SETTRUE;
					}
					else
					{
						ChangeData.NewPrimaryCurvature = EditRegularShapeParam::CHANGE_SETFALSE;
						ChangeData.NewStellationCurvature = EditRegularShapeParam::CHANGE_SETFALSE;
					}
					CauseShapeEdit = TRUE;
				}
			}
		}
		else if (Msg->GadgetID == _R(IDC_BTN_REGSHAPETOOL_POLYGON))
		{
			if (Msg->DlgMsg==DIM_LFT_BN_CLICKED && pQuickShapeBase!=NULL)
			{
				pQuickShapeBase->SetPolygon(TRUE);
				if (ChangeMode)
				{
					// Set the operation parameters
					ChangeData.NewCircular = EditRegularShapeParam::CHANGE_SETFALSE;
					CauseShapeEdit = TRUE;
				}	
				else
				{
					SetPolygonCreate(TRUE);
				}
			}
		}
		else if (Msg->GadgetID == _R(IDC_BTN_REGSHAPETOOL_ELLIPSE))
		{
			if (Msg->DlgMsg==DIM_LFT_BN_CLICKED && pQuickShapeBase!=NULL)
			{
				pQuickShapeBase->SetPolygon(FALSE);
				if (ChangeMode)
				{
					// Set the operation parameters
					ChangeData.NewCircular = EditRegularShapeParam::CHANGE_SETTRUE;
					CauseShapeEdit = TRUE;
				}
				else
				{
					SetPolygonCreate(FALSE);
				}
			}
		}
		else if (Msg->GadgetID == _R(IDC_BTN_REGSHAPETOOL_LINES))
		{
				if (Msg->DlgMsg==DIM_LFT_BN_CLICKED)
				{
					NewPath.Initialise(4,1);
					NewPath.AddMoveTo(DocCoord(0,0));
					NewPath.AddLineTo(DocCoord(72000,0));
					ChangeData.NewEdgePath1 = &NewPath;
					ChangeData.NewEdgePath2 = &NewPath;
					CauseShapeEdit = TRUE;
				}
		}
		else if (Msg->GadgetID == _R(IDC_EDIT_REGSHAPETOOL_BUMPLEFT))
		{
			if (IsEnabled(_R(IDC_EDIT_REGSHAPETOOL_BUMPLEFT)))
			{
				if (Msg->DlgMsg==DIM_LFT_BN_CLICKED)
				{
					if (!BumpLeft())
						InformError();
				}
				else if (Msg->DlgMsg == DIM_LFT_BN_UP)
				{
					if (!EditCommit(TRUE))
						InformError();
					UpdateEditFields();
				}
			}
		}
		else if (Msg->GadgetID ==  _R(IDC_EDIT_REGSHAPETOOL_BUMPRIGHT))
		{
			if (IsEnabled(_R(IDC_EDIT_REGSHAPETOOL_BUMPRIGHT)))
			{
				if (Msg->DlgMsg==DIM_LFT_BN_CLICKED)
				{
					if (!BumpRight())
						InformError();
				}
				else if (Msg->DlgMsg == DIM_LFT_BN_UP)
				{
					if (!EditCommit(TRUE))
						InformError();
					UpdateEditFields();
				}
			}
		}
		else if (Msg->GadgetID == _R(IDC_EDIT_REGSHAPETOOL_BUMPUP))
		{
			if (IsEnabled(_R(IDC_EDIT_REGSHAPETOOL_BUMPUP)))
			{
				if (Msg->DlgMsg == DIM_LFT_BN_CLICKED)
				{
					if (!BumpUp())
						InformError();
				}
				else if (Msg->DlgMsg == DIM_LFT_BN_UP)
				{
					if (!EditCommit(FALSE))
						InformError();
					UpdateEditFields();
				}
			}
		}
		else if (Msg->GadgetID == _R(IDC_EDIT_REGSHAPETOOL_BUMPDOWN))
		{
			if (IsEnabled(_R(IDC_EDIT_REGSHAPETOOL_BUMPDOWN)))
			{
				if (Msg->DlgMsg == DIM_LFT_BN_CLICKED)
				{
					if (!BumpDown())
						InformError();
				}
				else if (Msg->DlgMsg == DIM_LFT_BN_UP)
				{
					if (!EditCommit(FALSE))
						InformError();
					UpdateEditFields();
				}
			}
		}
		// Number of sides edit/drop down box
		else if (Msg->GadgetID == _R(IDC_EDIT_REGSHAPETOOL_SIDES))
		{
			if (Msg->DlgMsg==DIM_SELECTION_CHANGED && pQuickShapeBase!=NULL)
			{
				INT32 NewNumSides = GetNumSides(TRUE);
				if (NewNumSides != -1)
				{
					pQuickShapeBase->SetNumSides(NewNumSides);
					if (ChangeMode)
					{
						// Set the operation parameters
						ChangeData.NewNumSides = NewNumSides;
						CauseShapeEdit = TRUE;
					}
				}
			}
			else if (Msg->DlgMsg== DIM_TEXT_CHANGED && pQuickShapeBase!=NULL)
			{
				BOOL Valid;
				String_256 string = GetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_SIDES),&Valid);
				if (!string.IsEmpty())
				{
					pQuickShapeBase->SetNumSides(GetLongGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_SIDES), 1, 99, 0, &Valid));
					if (!Valid)
					{
						InformError(_R(IDE_REGSHAPETOOL_SIDES));
						pQuickShapeBase->SetNumSides(3);
						UpdateInfobar();
					}
				}
			}
		}
		else if (Msg->GadgetID == _R(IDC_EDIT_REGSHAPETOOL_POS))
		{
			if (Msg->DlgMsg==DIM_SELECTION_CHANGED && pQuickShapeBase!=NULL)
			{
				WORD temp;
				GetValueIndex(_R(IDC_EDIT_REGSHAPETOOL_POS), &temp);
				pQuickShapeBase->EditFieldMode = pQuickShapeBase->EditFieldOptions[temp];
				UpdateEditFields();
			}
		}
		else if (Msg->GadgetID == _R(IDC_EDIT_REGSHAPETOOL_UPPER))
		{
			if (Msg->DlgMsg == DIM_SELECTION_CHANGED)
			{
				if (!EditCommit(TRUE))
					InformError();
				UpdateEditFields();
			}
		}	
		else if (Msg->GadgetID == _R(IDC_EDIT_REGSHAPETOOL_LOWER))
		{
			if (Msg->DlgMsg == DIM_SELECTION_CHANGED)
			{
				if (!EditCommit(FALSE))
					InformError();
				UpdateEditFields();
			}
		}

		// Invoke an operation to perform changes, if requested
		if (CauseShapeEdit)
		{
			OpDescriptor* Apple = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpEditRegularShape));
			if (Apple != NULL)
				Apple->Invoke(&ChangeData);
		}
	}

	return OK;
}



/********************************************************************************************

>	BOOL QuickShapeBaseInfoBarOp::InitialiseInfobar()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if all went OK, otherwise FALSE
	Purpose:	To initialise the regular shape tool's infobar.  In particular it sets up the 
				number of sides listbox, and puts the buttons into a sensible default state.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL QuickShapeBaseInfoBarOp::InitialiseInfobar()
{
	if (WindowID==NULL || pQuickShapeBase==NULL)
		return TRUE;

	// build the list for the number of sides menu
	if (pQuickShapeBase->DoesNumSidesExist())
	{
		String_32 Str;
		for (INT32 i = 3; i <= 10; i++)
		{
			Str._MakeMsg(_T("#1%d"),i);
			SetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_SIDES),&Str);
		}
		SetComboListLength(_R(IDC_EDIT_REGSHAPETOOL_SIDES)); 
		SetNumSides(pQuickShapeBase->GetNumSides());
		PaintGadgetNow(_R(IDC_EDIT_REGSHAPETOOL_SIDES)); 
	}
	
	PreviousCurvature = FALSE;  
	PreviousStellation = FALSE;  
	PreviousCircular = FALSE;  
	BuildEditMenu(TRUE);

	UpdateInfobar();

	return TRUE;
}



/********************************************************************************************

>	void QuickShapeBaseInfoBarOp::BuildEditMenu(BOOL UpdateImmediate = FALSE)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/95
	Inputs:		UpdateImmediate TRUE if the menu should be forced to redraw
	Outputs:	-
	Returns:	-
	Purpose:	Builds the list-box menu for the the edit fields.  The contents of the menu
				are dependent on the currenty selected shapes (whether or not thay have curvature)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void QuickShapeBaseInfoBarOp::BuildEditMenu(BOOL UpdateImmediate)
{
	if (WindowID==NULL || pQuickShapeBase==NULL)
		return;

	BOOL HasCurvature = FALSE;
	BOOL HasStellation = FALSE;
	BOOL JustCircular = TRUE;

	// scan the selected shapes to see if any have curvature
	SelRange*	Selection = GetApplication()->FindSelection();
	Node*	pNode = Selection->FindFirst();

	while ((pNode != NULL) && !(HasCurvature && HasStellation))
	{
		if (IS_A(pNode, NodeRegularShape) && pQuickShapeBase->IsInterestingShape((NodeRegularShape*)pNode))
		{
			NodeRegularShape*	pShape = (NodeRegularShape*)pNode;

			if (pShape->IsPrimaryCurvature() ||	pShape->IsStellationCurvature())
				HasCurvature = TRUE;

			if (pShape->IsStellated())
				HasStellation = TRUE;

			if (!pShape->IsCircular())
				JustCircular = FALSE;
		}

		pNode = Selection->FindNext(pNode);
	}

	if (JustCircular)
	{
		HasCurvature = HasStellation = FALSE;
	}

	// Re-build the drop down menu
	if ((PreviousCurvature != HasCurvature) || (PreviousStellation != HasStellation) ||
									(PreviousCircular != JustCircular) || UpdateImmediate) 
	{
		PreviousCircular = JustCircular;
		PreviousCurvature = HasCurvature;
		PreviousStellation = HasStellation;
		String_32 Str;
		const INT32 MenuEntries = 4;
		BOOL ResPresent[MenuEntries] = {TRUE, TRUE, HasStellation, HasCurvature};
		UINT32 ResIDs[MenuEntries] = {_R(IDS_REGSHAPETOOL_MENUCENTRE), _R(IDS_REGSHAPETOOL_MENUMAJOR),
									_R(IDS_REGSHAPETOOL_MENUSTELL), _R(IDS_REGSHAPETOOL_MENUCURVE)};
		QuickShapeBase::EditModeIDS OptionIDs[MenuEntries] = {QuickShapeBase::EDITF_CENTRE, QuickShapeBase::EDITF_MAJOR, QuickShapeBase::EDITF_STELLATION, QuickShapeBase::EDITF_CURVE};

		// By hiding and showing the gadget it forces closed an open drop-down list
		HideGadget(_R(IDC_EDIT_REGSHAPETOOL_POS), TRUE);
		DeleteAllValues(_R(IDC_EDIT_REGSHAPETOOL_POS));
		HideGadget(_R(IDC_EDIT_REGSHAPETOOL_POS), FALSE);

		// Fill in the standard menu options
		INT32 CurrentCount = 0;
		for (INT32 i = 0; i < MenuEntries; i++)
		{
			if (ResPresent[i])
			{
				// Get the string and set the option name to it
				UINT32 StringResource = ResIDs[i];
				if ((i == 1) && (pQuickShapeBase->IsEllipse() || pQuickShapeBase->IsRectangle()))
				{
					StringResource = _R(IDS_REGSHAPETOOL_MENUSIZE);
					pQuickShapeBase->EditFieldOptions[CurrentCount++] = QuickShapeBase::EDITF_SIZE;
				}
				else
					pQuickShapeBase->EditFieldOptions[CurrentCount++] = OptionIDs[i];
					
				Str.MakeMsg(StringResource);
				SetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_POS),&Str);
			}
		}

		// Ellipse and Rectangle tools have special rotate option
		if (pQuickShapeBase->IsEllipse() || pQuickShapeBase->IsRectangle())
		{
			Str.MakeMsg(_R(IDS_REGSHAPETOOL_MENUROT));
			pQuickShapeBase->EditFieldOptions[CurrentCount++] = QuickShapeBase::EDITF_ROT;
			SetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_POS),&Str);
		}
		SetComboListLength(_R(IDC_EDIT_REGSHAPETOOL_POS));

		// Ensure that the selected mode is valid for this particular tool
		if ((!HasCurvature && (pQuickShapeBase->EditFieldMode == QuickShapeBase::EDITF_CURVE)) ||
		 	(!HasStellation && (pQuickShapeBase->EditFieldMode == QuickShapeBase::EDITF_STELLATION)) )
		{
			pQuickShapeBase->EditFieldMode = QuickShapeBase::EDITF_CENTRE;
		}
		if (!pQuickShapeBase->IsEllipse() && !pQuickShapeBase->IsRectangle() 
							&& ((pQuickShapeBase->EditFieldMode == QuickShapeBase::EDITF_SIZE)
							|| (pQuickShapeBase->EditFieldMode == QuickShapeBase::EDITF_ROT)))
		{
			pQuickShapeBase->EditFieldMode = QuickShapeBase::EDITF_MAJOR;
		}
		INT32 SelectedItem = (INT32)pQuickShapeBase->EditFieldMode;
		if (SelectedItem == 3)
		{
			if (!HasStellation)
				SelectedItem = 2;
		}
		if (SelectedItem == (INT32)(QuickShapeBase::EDITF_SIZE))
			SelectedItem = 1;
		if (SelectedItem == (INT32)(QuickShapeBase::EDITF_ROT))
		{
			if (HasCurvature)
				SelectedItem = 3;
			else
				SelectedItem = 2;
		}
		SetSelectedValueIndex(_R(IDC_EDIT_REGSHAPETOOL_POS),SelectedItem);
		PaintGadgetNow(_R(IDC_EDIT_REGSHAPETOOL_POS));
	}

	UpdateEditFields();
}



/********************************************************************************************

>	void QuickShapeBaseInfoBarOp::UpdateEditFields(NodeRegularShape* ShapeToShow = NULL, Spread* ShapeSpread = NULL)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		ShapeToShow - the shape to show in the edit fields (NULL to scan the doc)
				ShapeSpread - the spread containing ShapeToShow
	Outputs:	-
	Returns:	-
	Purpose:	Updates the edit fields.  The contents depend on the currently selected
				menu item.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void QuickShapeBaseInfoBarOp::UpdateEditFields(NodeRegularShape* ShapeToShow, Spread* ShapeSpread)
{
	if (WindowID==NULL || pQuickShapeBase==NULL)
		return;

	BOOL NoneSelected = TRUE;
	BOOL MultipleCentres = FALSE;
	DocCoord Centre(-1,-1);
	BOOL MultipleMajorRadius = FALSE;
	BOOL MultipleMinorRadius = FALSE;
	BOOL MultipleMajorAngle = FALSE;
	double MajorRadius = -1.0;
	double MinorRadius = -1.0;
	double MajorAngle = -1.0;
	BOOL MultipleStellationRadius = FALSE;
	BOOL MultipleStellationOffset = FALSE;
	double StellationRadius = -1.0;
	double StellationOffset = -1000.0;
	UINT32 PreviousNumSides = 0;
	BOOL MultiplePrimaryCurvature = FALSE;
	BOOL MultipleStellationCurvature = FALSE;
	double PrimaryCurvature = -1.0;
	double StellationCurvature = -1.0;
	BOOL FoundStellatedCurve = FALSE;

	// scan the selected shapes for things to display in the edit fields
	SelRange*	Selection = GetApplication()->FindSelection();
	Node*	pNode = Selection->FindFirst();

	if (ShapeToShow != NULL)
		pNode = ShapeToShow;

	while (pNode != NULL)
	{
		if (IS_A(pNode, NodeRegularShape) && pQuickShapeBase->IsInterestingShape((NodeRegularShape*)pNode))
		{
			NodeRegularShape*	pShape = (NodeRegularShape*)pNode;
			if (ShapeToShow == NULL)
				CurrentSpread = pShape->FindParentSpread();
			else
				CurrentSpread = ShapeSpread;

			NoneSelected = FALSE;

			// Test centres
			if (!MultipleCentres)
			{
				if (Centre == DocCoord(-1,-1))
				{
					Centre = pShape->GetCentrePoint();
				}
				else
				{
					if (Centre != pShape->GetCentrePoint())
						MultipleCentres = TRUE;
				}
			}

			// Test major axes radi
			if (!MultipleMajorRadius)
			{
				double Length = pShape->GetCentrePoint().Distance(pShape->GetMajorAxes());
				Length = Length*2;	// Make it the diameter, not the radius
				if (MajorRadius == -1.0)
				{
					MajorRadius = Length;
				}
				else
				{
					if (Length != MajorRadius)
						MultipleMajorRadius = TRUE;
				}
			}	

			// Test minor axes radi
			if (!MultipleMinorRadius)
			{
				double Length = pShape->GetCentrePoint().Distance(pShape->GetMinorAxes());
				Length = Length*2;	// Make it the diameter, not the radius
				if (MinorRadius == -1.0)
				{
					MinorRadius = Length;
				}
				else
				{
					if (Length != MinorRadius)
						MultipleMinorRadius = TRUE;
				}
			}	

			// Test major axes angle
			if (!MultipleMajorAngle)
			{
				double Angle = pShape->GetRotationAngle();

				if (MajorAngle == -1.0)
				{
					MajorAngle = Angle;
				}
				else
				{
					// we are subject to NASTY rounding errors here !!!!
					// previously this said *1000; but this was too accurate and would incorrectly
					// state many when on-screen the shapes all appear to be (e.g.  45 degrees).
					// the problem is to do with the way that GetRotationAngle () does its stuff
					if ((INT32)(Angle*100) != (INT32)(MajorAngle*100))
						MultipleMajorAngle = TRUE;
				}
			}	

			if (!pShape->IsCircular())
			{
				// Test stellation radi
				if (!MultipleStellationRadius)
				{
					double Length = pShape->GetCentrePoint().Distance(pShape->GetMajorAxes());
					Length = Length * pShape->GetStellRadiusToPrimary();
					if (StellationRadius == -1.0)
					{
						StellationRadius = Length;
					}
					else
					{
						if (Length != StellationRadius)
							MultipleStellationRadius = TRUE;
					}
				}	

				// Test stellation offset angle ratio
				if (!MultipleStellationOffset)
				{
					double Offset = pShape->GetStellationRatio();

					if (StellationOffset == -1000.0)
					{
						StellationOffset = Offset;
						PreviousNumSides = pShape->GetNumSides();
					}
					else
					{
						if (((INT32)(Offset*1000) != (INT32)(StellationOffset*1000)) || 
											(PreviousNumSides != pShape->GetNumSides()))
							MultipleStellationOffset = TRUE;
					}
				}	

				// Test Primary Curvature
				if (!MultiplePrimaryCurvature)
				{
					if (PrimaryCurvature == -1.0)
					{
						PrimaryCurvature = pShape->GetPrimaryCurveToPrimary();
					}
					else
					{
						if (PrimaryCurvature != pShape->GetPrimaryCurveToPrimary())
							MultiplePrimaryCurvature = TRUE;
					}
				}

				// Test Stellation Curvature
				if (pShape->IsStellated() && pShape->IsStellationCurvature())
					FoundStellatedCurve= TRUE;
				if (!MultipleStellationCurvature)
				{
					if (StellationCurvature == -1.0)
					{
						StellationCurvature = pShape->GetStellCurveToStell();
					}
					else
					{
						if (StellationCurvature != pShape->GetStellCurveToStell())
							MultipleStellationCurvature = TRUE;
					}
				}
			}
		}

		if (ShapeToShow == NULL)
			pNode = Selection->FindNext(pNode);
		else
			pNode = NULL;
	}

	String_32 Str;

	// Now update the edit fields
	BOOL EnableArray[6] = {TRUE, TRUE, TRUE, TRUE, TRUE, TRUE};
	UINT32 loop;
	BOOL Blank = FALSE;
	BOOL MakeUpperMany = FALSE;
	BOOL MakeLowerMany = FALSE;
	BOOL FastUpdate = (ShapeToShow != NULL);
	if ((NoneSelected) || (Document::GetSelected() == NULL) || (Document::GetCurrent() == NULL))
		Blank = TRUE;
	else
	{
		switch (pQuickShapeBase->EditFieldMode)
		{
			case QuickShapeBase::EDITF_CENTRE:
				if (MultipleCentres)
				{
					MakeLowerMany = MakeUpperMany = TRUE;
				}
				else
				{
					if(!SetEditPosition(_R(IDC_EDIT_REGSHAPETOOL_UPPER), _R(IDC_EDIT_REGSHAPETOOL_LOWER), 
																	Centre, CurrentSpread, FastUpdate))
					{
						Blank = TRUE;
					}
				}
				break;
			case QuickShapeBase::EDITF_MAJOR:
				if (MultipleMajorRadius)
				{
					MakeUpperMany = TRUE;
				}
				else
				{
					Blank = !SetEdit(_R(IDC_EDIT_REGSHAPETOOL_UPPER), (INT32)MajorRadius, FastUpdate);
				}
				if (!Blank)
				{
					if (MultipleMajorAngle)
					{
						MakeLowerMany = TRUE;
					}
					else
					{
						// convert MajorAngle into the display value
						MajorAngle = MajorAngle*(180/PI) - 90;
						if (MajorAngle >= 0)
							MajorAngle -= 360;
						MajorAngle = -MajorAngle;
						if (MajorAngle >= 360.0)
							MajorAngle = 0.0;
						
						Blank = !SetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), MajorAngle);
						if (FastUpdate)
							PaintGadgetNow(_R(IDC_EDIT_REGSHAPETOOL_LOWER));
					}
				}
				break;
			case QuickShapeBase::EDITF_STELLATION:
				if (MultipleStellationRadius)
				{
					MakeUpperMany = TRUE;
				}
				else
				{
					Blank = !SetEdit(_R(IDC_EDIT_REGSHAPETOOL_UPPER), (INT32)StellationRadius, FastUpdate);
				}
				if (!Blank)
				{
					if (MultipleStellationOffset)
					{
						MakeLowerMany = TRUE;
					}
					else
					{
						ERROR3IF(PreviousNumSides == 0, "Trying to update stellation with no stellated sides!");
						// convert StellationOffset into the display value
						if (PreviousNumSides != 0)
						{
							StellationOffset = StellationOffset*(360/PreviousNumSides);
							Blank = !SetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), StellationOffset);
							if (FastUpdate)
								PaintGadgetNow(_R(IDC_EDIT_REGSHAPETOOL_LOWER));
						}
					}
				}
				break;
			case QuickShapeBase::EDITF_CURVE:
				if (MultiplePrimaryCurvature)
				{
					MakeUpperMany = TRUE;
				}
				else
				{
					Blank = !SetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_UPPER), PrimaryCurvature);
					if (FastUpdate)
						PaintGadgetNow(_R(IDC_EDIT_REGSHAPETOOL_UPPER));
				}
				if (!Blank)
				{
					if (MultipleStellationCurvature)
					{
						MakeLowerMany = TRUE;
					}
					else
					{
						Blank = !SetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), StellationCurvature);
						if (FastUpdate)
							PaintGadgetNow(_R(IDC_EDIT_REGSHAPETOOL_LOWER));
					}
				}
				if (!FoundStellatedCurve)
				{
					EnableArray[1] = FALSE;
					EnableArray[4] = FALSE;
					EnableArray[5] = FALSE;
				}
				break;

			case QuickShapeBase::EDITF_SIZE:
				// Width in the upper field
				if (MultipleMinorRadius)
					MakeUpperMany = TRUE;
				else
				{
					if (pQuickShapeBase->IsRectangle())
						MinorRadius = MinorRadius * cos (PI/4);
					Blank = !SetEdit(_R(IDC_EDIT_REGSHAPETOOL_UPPER), (INT32)MinorRadius, FastUpdate);
				}

				// Height in the lower field
				if (MultipleMajorRadius || Blank)
					MakeLowerMany = TRUE;
				else
				{
					if (pQuickShapeBase->IsRectangle())
						MajorRadius = MajorRadius * cos (PI/4);
					Blank = !SetEdit(_R(IDC_EDIT_REGSHAPETOOL_LOWER), (INT32)MajorRadius, FastUpdate);
				}
				break;
	
			case QuickShapeBase::EDITF_ROT:
				if (MultipleMajorAngle)
					MakeUpperMany = TRUE;
				else
				{
					// convert MajorAngle into the display value
					MajorAngle = MajorAngle*(180/PI) - 90;
					if (MajorAngle >= 0)
						MajorAngle -= 360;
					MajorAngle = -MajorAngle;
					if (MajorAngle >= 360.0)
						MajorAngle = 0.0;
					
					Blank = !SetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_UPPER), MajorAngle);
					Str.Empty();
					SetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER),&Str);
					if (FastUpdate)
					{
						PaintGadgetNow(_R(IDC_EDIT_REGSHAPETOOL_UPPER));
						PaintGadgetNow(_R(IDC_EDIT_REGSHAPETOOL_LOWER));
					}
				}
				EnableArray[1] = FALSE;
				EnableArray[4] = FALSE;
				EnableArray[5] = FALSE;
				break;

			default:
				ERROR3("Unknown edit field mode found when updating");
				Blank = TRUE;
				break;
		}
	}

	if (MakeUpperMany)
	{
		Str.MakeMsg(_R(IDS_MANY));
		SetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_UPPER),&Str);
	}

	if (MakeLowerMany)
	{
		Str.MakeMsg(_R(IDS_MANY));
		SetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER),&Str);
	}

	if (Blank)
	{
		Str.Empty();
		SetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_UPPER),&Str);
		SetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER),&Str);
		for (loop = 0; loop < 6; EnableArray[loop++] = FALSE);
	}

 	if (ShapeToShow == NULL)
 	{
	 	EnableGadget(_R(IDC_EDIT_REGSHAPETOOL_UPPER), EnableArray[0]);
		EnableGadget(_R(IDC_EDIT_REGSHAPETOOL_LOWER), EnableArray[1]);	 
		EnableGadget(_R(IDC_EDIT_REGSHAPETOOL_BUMPLEFT), EnableArray[2]); 
		EnableGadget(_R(IDC_EDIT_REGSHAPETOOL_BUMPRIGHT), EnableArray[3]);
		EnableGadget(_R(IDC_EDIT_REGSHAPETOOL_BUMPUP), EnableArray[4]);	 
		EnableGadget(_R(IDC_EDIT_REGSHAPETOOL_BUMPDOWN), EnableArray[5]); 
	}

	SetHelp();
}


/********************************************************************************************
>	BOOL QuickShapeBaseInfoBarOp::IsEnabled(CGadgetID gid)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/10/2005
	Inputs:		gid				gadget identifier
	Outputs:	-
	Returns:	TRUE if the specified gadget is enabled
				FALSE if not, if it's "greyed out"
	Purpose:	Test whether control is available to the user or not
	Errors:		-
	SeeAlso:	SelectorInfoBarOp::SetEdit
	Notes:		Note that this function ignores ShapeToShow because the gadgets are only
				enabled or disable in the fucntion above when ShapeToShow is NULL

********************************************************************************************/

BOOL QuickShapeBaseInfoBarOp::IsEnabled(CGadgetID gid)
{
	SelRange*	Selection = GetApplication()->FindSelection();
	Node*	pNode = Selection->FindFirst();

	if( _R(IDC_EDIT_REGSHAPETOOL_BUMPLEFT) == gid ||
		_R(IDC_EDIT_REGSHAPETOOL_BUMPRIGHT) == gid ||
		_R(IDC_EDIT_REGSHAPETOOL_BUMPUP) == gid ||
		_R(IDC_EDIT_REGSHAPETOOL_BUMPDOWN) )
	{
		while (pNode != NULL)
		{
			if (IS_A(pNode, NodeRegularShape) && pQuickShapeBase->IsInterestingShape((NodeRegularShape*)pNode))
			{
				NodeRegularShape*	pShape = (NodeRegularShape*)pNode;
				CurrentSpread = pShape->FindParentSpread();
				return TRUE;
			}

			pNode = Selection->FindNext(pNode);
		}
	}
	else
		ERROR3("IsEnabled behaviour not defined for this gadget\n");

	return FALSE;
}




/********************************************************************************************
>	BOOL QuickShapeBaseInfoBarOp::SetEdit(CGadgetID gid, INT32 nValue, BOOL PaintNow = FALSE)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/1/95
	Inputs:		gid				gadget identifier
				nValue			the value to set in the edit field (in millipoints
				PaintNow - TRUE if the edit filed should be redrawn immediatly.
	Outputs:	-
	Returns:	TRUE if everything suceeded, FALSE if an error occured
	Purpose:	Converts the given value to the appropriate units, as contained in the
				spread, converts it to text and writes in into the edit-field.
	Errors:		-
	SeeAlso:	SelectorInfoBarOp::SetEdit
	
********************************************************************************************/

BOOL QuickShapeBaseInfoBarOp::SetEdit(CGadgetID gid, INT32 nValue, BOOL PaintNow)
{
	if (CurrentSpread != NULL)
	{
		// Get the dimension scaling object (units) associated with the given spread
		// and convert to its units.
		DimScale* pDimScale = DimScale::GetPtrDimScale((Node*) CurrentSpread);
		ERROR2IF (pDimScale == NULL, FALSE, "Null DimScale* in QuickShapeBaseInfoBarOp::SetEdit");
	
		// Convert to units & text and display.
		String_256 str;
		pDimScale->ConvertToUnits((INT32) nValue, &str);
		if (!UpdateStringGadgetValue(gid, &str))
			PaintNow = FALSE;
	}
	else
	{
		// No unit conversion possible, just convert to text.
		SetLongGadgetValue(gid, nValue);
	}
	if (PaintNow)
		PaintGadgetNow(gid);
	return TRUE;
}



/********************************************************************************************
>	BOOL QuickShapeBaseInfoBarOp::SetEditPosition(CGadgetID gidX, CGadgetID gidY, DocCoord loc, Spread* pSpread, BOOL PaintNow)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/95
	Inputs:		gidX - gadget identifier for X field
				gidY - gadget identifier for Y field
				DocCoord - location to show
				pSpread - Spread containing the location
				PaintNow - TRUE if the edit fields should be redrawn immediatly.
	Outputs:	-
	Returns:	TRUE if everything suceeded, FALSE if an error occured
	Purpose:	Converts the given coordinate to the appropriate units, as contained in the
				spread, converts it to text and writes in into the edit-field.
	Errors:		-
	SeeAlso:	SelectorInfoBarOp::SetEdit
	
********************************************************************************************/

BOOL QuickShapeBaseInfoBarOp::SetEditPosition(CGadgetID gidX, CGadgetID gidY, DocCoord loc, Spread* pSpread, BOOL PaintNow)
{
 	ERROR2IF(pSpread == FALSE, FALSE, "Current spread was NULL");

	String_256	XText;
	String_256	YText;

	pSpread->SpreadCoordToText(&XText, &YText, loc);
	
	BOOL UpdatedX = UpdateStringGadgetValue(gidX, &XText);
	BOOL UpdatedY = UpdateStringGadgetValue(gidY, &YText);

	if (PaintNow && UpdatedX)
		PaintGadgetNow(gidX);
	if (PaintNow && UpdatedY)
		PaintGadgetNow(gidY);

	return TRUE;
}



/********************************************************************************************
>	BOOL QuickShapeBaseInfoBarOp::EditCommit(BOOL UpperCommit)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		UpperCommit is TRUE if the upper edit field has been commited, FALSE for the
				lower edit field
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Called when the user has committed a value in either of the edit fields
	Errors:		-
	SeeAlso:	-
	
********************************************************************************************/
BOOL QuickShapeBaseInfoBarOp::EditCommit(BOOL UpperCommit)
{
	ERROR2IF(pQuickShapeBase==NULL, FALSE, "NULL tool pointer");


	EditRegularShapeParam ChangeData(NULL);
	ChangeData.ShapesToAffect = (EditRegularShapeParam::AffectShape)pQuickShapeBase->GetShapesToAffect();
		
	// Put changes into ChangeData depending on selected edit
	switch (pQuickShapeBase->EditFieldMode)
	{
		case QuickShapeBase::EDITF_CENTRE:
			if (!EditCommitCentre(UpperCommit, &ChangeData))
				return FALSE;
			break;
		case QuickShapeBase::EDITF_MAJOR:
			if (!EditCommitSizeAndRotation(UpperCommit, &ChangeData))
				return FALSE;
			break;
		case QuickShapeBase::EDITF_STELLATION:
			if (!EditCommitStellation(UpperCommit, &ChangeData))
				return FALSE;
			break;
		case QuickShapeBase::EDITF_CURVE:
			if (!EditCommitCurvature(UpperCommit, &ChangeData))
				return FALSE;
			break;
		case QuickShapeBase::EDITF_SIZE:
			if (!EditCommitWidthAndHeight(UpperCommit, &ChangeData))
				return FALSE;
			break;
		case QuickShapeBase::EDITF_ROT:
			if (!EditCommitRotation(UpperCommit, &ChangeData))
				return FALSE;
			break;
		default:
			ERROR3("Unknown edit field mode found when committing");
	} 

	// Invoke an operation to perform the changes 
	OpDescriptor* Apple = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpEditRegularShape));
	ERROR2IF(Apple == NULL, FALSE, "Couldn't find OpEditRegularShape's OpDescriptor");
	Apple->Invoke(&ChangeData);

	return TRUE;
}



/********************************************************************************************

>	BOOL QuickShapeBaseInfoBarOp::GetFieldCoord(DocCoord* pCoord,pSpread)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		pCoord - pointer to a DocCoord to return the results in
				pSpread - the spread the object is on.
	Outputs:	pCoord contains the coordinate shown in the edit field
	Returns:	TRUE for success/ FALSE for validation error (pCoord is undefined)
	Purpose:	Reads the text in the edit fields as coordinate values
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL QuickShapeBaseInfoBarOp::GetFieldCoord(DocCoord* pCoord, Spread* pSpread)
{
	if (pSpread==NULL)
		return FALSE;

	BOOL Valid;
	String_64	strX;
	String_64	strY;
	String_16 ManyField;
	ManyField.Load(_R(IDS_MANY));

	// Get the text strings from the edit fields
	strX = GetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_UPPER), &Valid);
	if (strX == ManyField)
		Valid = FALSE;
	if (Valid) 
		strY = GetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), &Valid);
	if (strY == ManyField)
		Valid = FALSE;

	if (Valid)
		Valid = pSpread->TextToSpreadCoord(pCoord, &strX, &strY);
		
	if (!Valid)
		Error::SetError(_R(IDE_INVALID_CENTRE));
	
	return Valid;
}		



/********************************************************************************************

>	void QuickShapeBaseInfoBarOp::SetToolMode()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets the text in the infobars Add/Change field.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void QuickShapeBaseInfoBarOp::SetToolMode()
{
	if (pQuickShapeBase!=NULL)
	{
		String_16	Text;
		
		if (AreShapesSelected())
			Text.Load(_R(IDS_BEZTOOL_CHANGE),Tool::GetModuleID(pQuickShapeBase->GetID()));
		else
			Text.Load(_R(IDS_BEZTOOL_NEW),Tool::GetModuleID(pQuickShapeBase->GetID()));
		SetStringGadgetValue(_R(IDC_REGSHAPETOOL_ADDCHANGE), &Text);
	}
}



/********************************************************************************************

>	INT32 QuickShapeBaseInfoBarOp::GetCreationMode()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		-
	Outputs:	-
	Returns:	The creation mode that the info bar is in; either RADIUS, DIAMETER or BOUNDS
	Purpose:	To read which of the creation modes is currently selected.
	Errors:		ERROR3 if no buttons are selected (RADIUS is selected and returned)
	SeeAlso:	QuickShapeBaseInfoBarOp::SetCreationMode

********************************************************************************************/
INT32 QuickShapeBaseInfoBarOp::GetCreationMode()
{
	// Is the radius button selected?
	if (GetLongGadgetValue(_R(IDC_BTN_REGSHAPETOOL_RADIUS), 0, 1))
	{
		return (OpNewRegShape::RADIUS); 
	}
	// Is the diameter button selected?
	if (GetLongGadgetValue(_R(IDC_BTN_REGSHAPETOOL_DIAMETER), 0, 1))
	{
		return (OpNewRegShape::DIAMETER); 
	}
	// Is the bounds button selected?
	if (GetLongGadgetValue(_R(IDC_BTN_REGSHAPETOOL_BOUNDBOX), 0, 1))
	{
		return (OpNewRegShape::BOUNDS); 
	}
	// To get here no button was selected - in debug builds do an ERROR3
	// In retail builds select the radius button and return that	  
	ERROR3("No creation mode button was selected!");
	SetCreationMode(OpNewRegShape::RADIUS);
	return (OpNewRegShape::RADIUS); 					 
}						 




/********************************************************************************************

>	void QuickShapeBaseInfoBarOp::SetCreationMode(INT32 NewMode)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		NewMode - the creation mode to put the infobar into.  This should be either
				RADIUS, DIAMETER or BOUNDS
	Outputs:	-
	Returns:	-
	Purpose:	To set the creation mode the infobar is in (ie it sets the three buttons).
	Errors:		ERROR3 if NewMode is not one of the three recognised states.
	SeeAlso:	QuickShapeBaseInfoBarOp::GetCreationMode

********************************************************************************************/
void QuickShapeBaseInfoBarOp::SetCreationMode(INT32 NewMode)
{
	BOOL	SelectRadius = FALSE;
	BOOL	SelectDiameter = FALSE;
	BOOL	SelectBounds = FALSE;

	// Now make one of the above TRUE depending on NewMode
	switch (NewMode)
	{
		case OpNewRegShape::RADIUS:
			SelectRadius = TRUE;
			break;
		case OpNewRegShape::DIAMETER:
			SelectDiameter = TRUE;
			break;
		case OpNewRegShape::BOUNDS:
			SelectBounds = TRUE;
			break;
		default:
			ERROR3("Invalid creation mode");
			SelectRadius = TRUE;
			break;
	}

	// Now set the buttons
	if (WindowID != NULL)
	{
		SetLongGadgetValue(_R(IDC_BTN_REGSHAPETOOL_RADIUS), SelectRadius);
		SetLongGadgetValue(_R(IDC_BTN_REGSHAPETOOL_DIAMETER), SelectDiameter);
		SetLongGadgetValue(_R(IDC_BTN_REGSHAPETOOL_BOUNDBOX), SelectBounds);
	}
}



/********************************************************************************************

>	BOOL QuickShapeBaseInfoBarOp::IsPolygonCreate()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the Create Polygons button is selected, FALSE if the Create Ellipses
				button is selected.
	Purpose:	To read wether ploygons or ellipses should be created.
	Errors:		-
	SeeAlso:	QuickShapeBaseInfoBarOp::SetPolygonCreate()

********************************************************************************************/
BOOL QuickShapeBaseInfoBarOp::IsPolygonCreate()
{
	if (pQuickShapeBase!=NULL && pQuickShapeBase->DoesPolygonExist())
		return (GetLongGadgetValue(_R(IDC_BTN_REGSHAPETOOL_POLYGON), 0, 1)) ;
	else
		return FALSE;
}



/********************************************************************************************

>	void QuickShapeBaseInfoBarOp::SetPolygonCreate(BOOL PolygonMode)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		PolygonMode	- TRUE if polygon mode should be selected, FALSE if ellipse
				mode shuld be selected.
	Outputs:	-
	Returns:	-
	Purpose:	To set the selection state of the polygon/ellipse buttons.  If ellipse mode
				then the curve and stellate buttons are greyed.
	Errors:		-
	SeeAlso:	QuickShapeBaseInfoBarOp::IsPolygonCreate

********************************************************************************************/
void QuickShapeBaseInfoBarOp::SetPolygonCreate(BOOL PolygonMode)
{
	if (pQuickShapeBase!=NULL && pQuickShapeBase->DoesReformSidesExist() && pQuickShapeBase->DoesNumSidesExist())
	{
		SetLongGadgetValue(_R(IDC_BTN_REGSHAPETOOL_POLYGON), PolygonMode!=0);
		SetLongGadgetValue(_R(IDC_BTN_REGSHAPETOOL_ELLIPSE), PolygonMode==0);
		EnableGadget(_R(IDC_BTN_REGSHAPETOOL_POLYGON), !pQuickShapeBase->ForcePolygonToGrey());
		EnableGadget(_R(IDC_BTN_REGSHAPETOOL_ELLIPSE), !pQuickShapeBase->ForceEllipseToGrey());
		if (PolygonMode)
		{
			EnableGadget(_R(IDC_BTN_REGSHAPETOOL_STELLATION), !pQuickShapeBase->ForceStellationToGrey());
			EnableGadget(_R(IDC_BTN_REGSHAPETOOL_CURVATURE), !pQuickShapeBase->ForceCurvatureToGrey());
			EnableGadget(_R(IDC_EDIT_REGSHAPETOOL_SIDES), !pQuickShapeBase->ForceNumSidesToGrey());
		}
		else
		{
			EnableGadget(_R(IDC_BTN_REGSHAPETOOL_STELLATION), FALSE);
			EnableGadget(_R(IDC_BTN_REGSHAPETOOL_CURVATURE), FALSE);
			EnableGadget(_R(IDC_EDIT_REGSHAPETOOL_SIDES), !pQuickShapeBase->ForceNumSidesToGrey());
		}
	}
}



/********************************************************************************************

>	BOOL QuickShapeBaseInfoBarOp::IsStellationCreate()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the stellation button is selected, FALSE if it isn't
	Purpose:	To get the selection state of the stellation button.
	Errors:		-
	SeeAlso:	QuickShapeBaseInfoBarOp::SetStellationCreate

********************************************************************************************/
BOOL QuickShapeBaseInfoBarOp::IsStellationCreate()
{
	if (pQuickShapeBase!= NULL && pQuickShapeBase->DoesStellationExist())
		return (GetLongGadgetValue(_R(IDC_BTN_REGSHAPETOOL_STELLATION), 0, 1)) ;
	else
		return FALSE;
}



/********************************************************************************************

>	void QuickShapeBaseInfoBarOp::SetStellationCreate(BOOL StellationCreate)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		StellationCreate - TRUE if the button should be selected, FALSE for deselection
	Outputs:	-
	Returns:	-
	Purpose:	To set the selection state of the stellation button.
	Errors:		-
	SeeAlso:	BOOL QuickShapeBaseInfoBarOp::IsStellationCreate

********************************************************************************************/
void QuickShapeBaseInfoBarOp::SetStellationCreate(BOOL StellationCreate)
{
	if (pQuickShapeBase!=NULL && pQuickShapeBase->DoesStellationExist())
	{
		if (pQuickShapeBase->ForceStellationToGrey())
			StellationCreate = FALSE;
		SetLongGadgetValue(_R(IDC_BTN_REGSHAPETOOL_STELLATION), StellationCreate!=0);
	}
}



/********************************************************************************************

>	BOOL QuickShapeBaseInfoBarOp::IsCurvedCreate()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the rounded edges button is selected, FALSE if it isn't
	Purpose:	To get the selection state of the rounded edges button.
	Errors:		-
	SeeAlso:	QuickShapeBaseInfoBarOp::SetCurvedCreate

********************************************************************************************/
BOOL QuickShapeBaseInfoBarOp::IsCurvedCreate()
{
	if (pQuickShapeBase!=NULL && pQuickShapeBase->DoesCurvatureExist())
		return (GetLongGadgetValue(_R(IDC_BTN_REGSHAPETOOL_CURVATURE), 0, 1)) ;
	else
		return FALSE;
}



/********************************************************************************************

>	void QuickShapeBaseInfoBarOp::SetCurvedCreate(BOOL CurvedCreate)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		CurvedCreate - TRUE if the button should be selected, FALSE for deselection
	Outputs:	-
	Returns:	-
	Purpose:	To set the selection state of the rounded corners button.
	Errors:		-
	SeeAlso:	BOOL QuickShapeBaseInfoBarOp::IsCurvedCreate

********************************************************************************************/
void QuickShapeBaseInfoBarOp::SetCurvedCreate(BOOL CurvedCreate)
{
	if (pQuickShapeBase!=NULL && pQuickShapeBase->DoesCurvatureExist())
	{
		if (pQuickShapeBase->ForceCurvatureToGrey())
			CurvedCreate = FALSE;
		SetLongGadgetValue(_R(IDC_BTN_REGSHAPETOOL_CURVATURE), CurvedCreate!=0);
	}
}



/********************************************************************************************

>	INT32 QuickShapeBaseInfoBarOp::GetNumSides(BOOL Error)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		Error - TRUE means if the field is invalid then show an error to the user.
	Outputs:	-
	Returns:	The number of sides displaied in the infobar edit field.
	Purpose:	To get the number of sides in the edit field.
	Errors:		If the number in the edit field is invalid then the user is informed and -1
				is returned (the edit field is forced to 3).  Valid numbers of sides are
				between 3 and 99 inclusive.
	SeeAlso:	QuickShapeBaseInfoBarOp::SetNumSides

********************************************************************************************/
INT32 QuickShapeBaseInfoBarOp::GetNumSides(BOOL	Error)
{
	if (pQuickShapeBase!=NULL && pQuickShapeBase->DoesNumSidesExist())
	{
		BOOL Valid;
		INT32 Result;
	
		if (Error)
			Result = GetLongGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_SIDES), 3, 99, _R(IDE_REGSHAPETOOL_SIDES), &Valid);
		else
			Result = GetLongGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_SIDES), 3, 99, 0, &Valid);

		if (Valid)
			return Result;
		else
		{
		 	SetNumSides(3);
			return -1;
		}
	}
	else
		return 3;
}



/********************************************************************************************

>	void QuickShapeBaseInfoBarOp::SetNumSides(INT32 NumberSides)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		NumberSides - the number to display	in the edit field
	Outputs:	-
	Returns:	-
	Purpose:	To initialise the regular shape tool's infobar.  In particular it sets up the 
				number of sides listbox.  A value of -1 means clear the field.
	Errors:		ERROR3's if NumberSides is less than three or greater than the maximum
	SeeAlso:	QuickShapeBaseInfoBarOp::GetNumSides

********************************************************************************************/
void QuickShapeBaseInfoBarOp::SetNumSides(INT32 NumberSides)
{
	if (pQuickShapeBase!=NULL && pQuickShapeBase->DoesNumSidesExist())
	{
		ERROR3IF(NumberSides > (INT32)REGULARSHAPE_MAXSIDES, "Tried to display more than the max number of sides");

		// If the number is -1 then clear the field.
		if (NumberSides == -1)
		{
			SetSelectedValueIndex(_R(IDC_EDIT_REGSHAPETOOL_SIDES), -1);
		}
		else
		{
			SetLongGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_SIDES), NumberSides, FALSE, -1);

			// If the number is between 3 and 10 then select the menu item
			if ((NumberSides >= 3) && (NumberSides <= 10))
				SetSelectedValueIndex(_R(IDC_EDIT_REGSHAPETOOL_SIDES), NumberSides-3);
		}
	}
}



/********************************************************************************************

>	BOOL QuickShapeBaseInfoBarOp::SetBarFromSelection()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the bar was setup (FALSE if it wasn't (ie Add mode should be selected))
	Purpose:	Scans the selected items.  If any Regular shapes are found then the infobar
				is setup using their parameters.
	Errors:		-
	SeeAlso:	QuickShapeBaseInfoBarOp::SetCreationMode, QuickShapeBaseInfoBarOp::SetPolygonCreate, 
				QuickShapeBaseInfoBarOp::SetStellationCreate, QuickShapeBaseInfoBarOp::SetCurvedCreate,
				QuickShapeBaseInfoBarOp::SetNumSides

********************************************************************************************/

BOOL QuickShapeBaseInfoBarOp::SetBarFromSelection()
{
	ERROR2IF(pQuickShapeBase==NULL, FALSE, "NULL tool pointer");

	SelRange*	Selection = GetApplication()->FindSelection();
	Node*	pNode = Selection->FindFirst();

	BOOL	FoundPolygon = FALSE;
	BOOL	FoundEllipse = FALSE;
	BOOL	FoundShape = FALSE;
	BOOL	FoundStellated = FALSE;
	BOOL	ShouldStellate = FALSE;
	BOOL	FoundCurved = FALSE;
	BOOL	ShouldCurve = FALSE;
	INT32		NumSides = 0;
	BOOL	FoundMultipleSides = FALSE;
	BOOL	FoundReformed = FALSE;
	
	while (pNode != NULL)
	{
		if (IS_A(pNode, NodeRegularShape) && pQuickShapeBase->IsInterestingShape((NodeRegularShape*)pNode))
		{
			NodeRegularShape*	pShape = (NodeRegularShape*)pNode;
			
			// If we haven't found a shape so far then set our var
			if (NumSides == 0)
				NumSides = pShape->GetNumSides();
			else
			{
				if (!FoundMultipleSides && (NumSides != (INT32)pShape->GetNumSides()))
				{
					FoundMultipleSides = TRUE; 
					NumSides = -1;
				}
			}

			// Now test for stellation
			if (!FoundStellated)
			{
				FoundStellated = TRUE;
				ShouldStellate = pShape->IsStellated();
			}
			else
			{
				if (ShouldStellate != pShape->IsStellated())
					ShouldStellate = FALSE;
			}

			// Now test for curvation
			if (!FoundCurved)
			{
				FoundCurved = TRUE;
				ShouldCurve = pShape->IsPrimaryCurvature();
			}
			else
			{
				if (ShouldCurve != pShape->IsPrimaryCurvature())
					ShouldCurve = FALSE;
			}

			// Now test for polygon/ellipse
			FoundShape = TRUE;
			FoundPolygon = FoundPolygon || !pShape->IsCircular();
			FoundEllipse = FoundEllipse || pShape->IsCircular();

			// Finally check the path edges
			FoundReformed = FoundReformed || pShape->IsReformed();
		}

		pNode = Selection->FindNext(pNode);
	}

	if (pQuickShapeBase->DoesReformSidesExist())
		EnableGadget(_R(IDC_BTN_REGSHAPETOOL_LINES), FoundReformed);
	if (FoundShape)
	{
		// If we have found polygons and ellipses then pop the buttons out
		// but allow editing of the number of sides
		if (FoundPolygon && FoundEllipse)
		{
			if (pQuickShapeBase->DoesPolygonExist())
				SetLongGadgetValue(_R(IDC_BTN_REGSHAPETOOL_POLYGON), FALSE);
			if (pQuickShapeBase->DoesEllipseExist())
				SetLongGadgetValue(_R(IDC_BTN_REGSHAPETOOL_ELLIPSE), FALSE);
			if (pQuickShapeBase->DoesNumSidesExist())
				EnableGadget(_R(IDC_EDIT_REGSHAPETOOL_SIDES), TRUE);
		}
		else
		{																		 
			SetPolygonCreate(FoundPolygon);
		}
		SetNumSides(NumSides);
		SetStellationCreate(ShouldStellate);
		SetCurvedCreate(ShouldCurve);
		pQuickShapeBase->SetNumSides(NumSides);
		pQuickShapeBase->SetPolygon(FoundPolygon);
		pQuickShapeBase->SetStellation(ShouldStellate);
		pQuickShapeBase->SetCurved(ShouldCurve);
	}
	else
	{
		SetNumSides(pQuickShapeBase->GetNumSides());
	}

	return FoundShape;
}



/********************************************************************************************

>	BOOL QuickShapeBaseInfoBarOp::AreShapesSelected()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if there are selected NodeRegularShapes in the document, FALSE if there arn't
	Purpose:	To see if there are any regular shapes in the current selection.  Also sets the
				member variable ChangeMode to the return value.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL QuickShapeBaseInfoBarOp::AreShapesSelected()
{
	ERROR2IF(pQuickShapeBase==NULL, FALSE, "NULL tool pointer");

	SelRange*	Selection = GetApplication()->FindSelection();
	Node*	pNode = Selection->FindFirst();
	
	while (pNode != NULL)
	{
		if (IS_A(pNode, NodeRegularShape) && pQuickShapeBase->IsInterestingShape((NodeRegularShape*)pNode))
		{
			ChangeMode = TRUE;
			return TRUE;
		}
		pNode = Selection->FindNext(pNode);
	}

	ChangeMode = FALSE;
	return FALSE;
}



/********************************************************************************************

>	void QuickShapeBaseInfoBarOp::UpdateInfobar()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	To ensure the infobar is up-to-date.  Called on initial creation and selection
				changes.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void QuickShapeBaseInfoBarOp::UpdateInfobar()
{
	if (WindowID!=NULL && pQuickShapeBase!=NULL)
	{
		SetToolMode();
		if (!SetBarFromSelection())
		{
			SetPolygonCreate(pQuickShapeBase->GetPolygon());
			SetStellationCreate(pQuickShapeBase->GetStellation());
			SetCurvedCreate(pQuickShapeBase->GetCurved());
			SetNumSides(pQuickShapeBase->GetNumSides());
		}
	}
}



/********************************************************************************************

>	BOOL QuickShapeBaseInfoBarOp::BumpLeft()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Called when the user clicks on the bump left button.  Does different things
				depeding on what the edit fields are displaying
	Errors:		-
	SeeAlso:	QuickShapeBaseInfoBarOp::BumpRight, QuickShapeBaseInfoBarOp::BumpUp, QuickShapeBaseInfoBarOp::BumpDown

********************************************************************************************/
BOOL QuickShapeBaseInfoBarOp::BumpLeft()
{
	ERROR2IF(pQuickShapeBase==NULL, FALSE, "NULL tool pointer");

	// Get the current edit field
	BOOL Valid = TRUE;
	ERROR2IF(CurrentSpread == NULL, FALSE, "BumpLeft found CurrentSpread was NULL");	
	DimScale* pDimScale = DimScale::GetPtrDimScale((Node*) CurrentSpread);
	ERROR2IF(pDimScale == NULL, FALSE, "NULL DimScalePtr");
	String_256 FieldContents = GetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_UPPER), &Valid);

	// Bump it
	if (Valid)
	{
		switch (pQuickShapeBase->EditFieldMode)
		{
			// bump centre left, bump down major length, bump down size, bump down stellation
			case QuickShapeBase::EDITF_CENTRE:
			case QuickShapeBase::EDITF_MAJOR:
			case QuickShapeBase::EDITF_SIZE:
			case QuickShapeBase::EDITF_STELLATION:
			{
				MILLIPOINT CurrentPos = 0;
				Valid = pDimScale->ConvertToMillipoints(FieldContents, &CurrentPos);
				if (Valid)
				{
					CurrentPos -= OpNudge::GetNudgeStep();
					if ((pQuickShapeBase->EditFieldMode == QuickShapeBase::EDITF_MAJOR) || (pQuickShapeBase->EditFieldMode == QuickShapeBase::EDITF_SIZE))
						{
							if (CurrentPos>100)
								CurrentPos=100;
						}
					if (pQuickShapeBase->EditFieldMode == QuickShapeBase::EDITF_STELLATION)
						{
							if (CurrentPos>0)
								CurrentPos=0;
						}
					SetEdit(_R(IDC_EDIT_REGSHAPETOOL_UPPER), CurrentPos, TRUE);
				}
			}
			break;
			
			case QuickShapeBase::EDITF_CURVE:
			{
				double CurrentCurve = GetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_UPPER), 0, 100, 0, &Valid);
				if (Valid)
				{
					CurrentCurve -= .05;
					if (CurrentCurve < 0)
						CurrentCurve = 0;
					SetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_UPPER), CurrentCurve);
				}
			}
			break;

			// Rotate anti-clockwise by 5 degrees
			case QuickShapeBase::EDITF_ROT:
			{
				double CurrentRot = GetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_UPPER), -360, 360, 0, &Valid);
				if (Valid)
				{
					CurrentRot = RestrictAngleZero360(CurrentRot - 5);
					SetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_UPPER), CurrentRot);
				}
			}
			break;

			default:
				ERROR2(FALSE, "Unknown edit field mode found when bumping left");
		} 
		PaintGadgetNow(_R(IDC_EDIT_REGSHAPETOOL_UPPER));
	}
	
	return TRUE;
}



/********************************************************************************************

>	BOOL QuickShapeBaseInfoBarOp::BumpRight()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Called when the user clicks on the bump right button.  Does different things
				depeding on what the edit fields are displaying
	Errors:		-
	SeeAlso:	QuickShapeBaseInfoBarOp::BumpLeft, QuickShapeBaseInfoBarOp::BumpUp, QuickShapeBaseInfoBarOp::BumpDown

********************************************************************************************/
BOOL QuickShapeBaseInfoBarOp::BumpRight()
{
	ERROR2IF(pQuickShapeBase==NULL, FALSE, "NULL tool pointer");

	// Get the current edit field
	BOOL Valid = TRUE;
	ERROR2IF(CurrentSpread == NULL, FALSE, "BumpRight found CurrentSpread was NULL");	
	DimScale* pDimScale = DimScale::GetPtrDimScale((Node*) CurrentSpread);
	ERROR2IF(pDimScale == NULL, FALSE, "NULL DimScalePtr");
	String_256 FieldContents = GetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_UPPER), &Valid);

	// Bump it
	if (Valid)
	{
		switch (pQuickShapeBase->EditFieldMode)
		{
			// bump centre right, bump up major length, bump up size, bump up stellation
			case QuickShapeBase::EDITF_CENTRE:
			case QuickShapeBase::EDITF_MAJOR:
			case QuickShapeBase::EDITF_SIZE:
			case QuickShapeBase::EDITF_STELLATION:
			{
				MILLIPOINT CurrentPos = 0;
				Valid = pDimScale->ConvertToMillipoints(FieldContents, &CurrentPos);
				if (Valid)
				{
					CurrentPos += OpNudge::GetNudgeStep();
					SetEdit(_R(IDC_EDIT_REGSHAPETOOL_UPPER), CurrentPos, TRUE);
				}
			}
			break;
			
			case QuickShapeBase::EDITF_CURVE:
			{
				double CurrentCurve = GetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_UPPER), 0, 100, 0, &Valid);
				if (Valid)
				{
					CurrentCurve += .05;
					SetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_UPPER), CurrentCurve);
				}
			}
			break;

			// Rotate clockwise by 5 degrees
			case QuickShapeBase::EDITF_ROT:
			{
				double CurrentRot = GetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_UPPER), -360, 360, 0, &Valid);
				if (Valid)
				{
					CurrentRot = RestrictAngleZero360(CurrentRot + 5);
					SetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_UPPER), CurrentRot);
				}
			}
			break;

			default:
				ERROR2(FALSE, "Unknown edit field mode found when bumping right");
		} 
		PaintGadgetNow(_R(IDC_EDIT_REGSHAPETOOL_UPPER));
	}
	
	return TRUE;
}



/********************************************************************************************

>	BOOL QuickShapeBaseInfoBarOp::BumpUp()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Called when the user clicks on the bump up button.  Does different things
				depeding on what the edit fields are displaying
	Errors:		-
	SeeAlso:	QuickShapeBaseInfoBarOp::BumpLeft, QuickShapeBaseInfoBarOp::BumpRight, QuickShapeBaseInfoBarOp::BumpDown

********************************************************************************************/
BOOL QuickShapeBaseInfoBarOp::BumpUp()
{
	ERROR2IF(pQuickShapeBase==NULL, FALSE, "NULL tool pointer");

	// Get the current edit field
	BOOL Valid = TRUE;
	ERROR2IF(CurrentSpread == NULL, FALSE, "BumpRight found CurrentSpread was NULL");	
	DimScale* pDimScale = DimScale::GetPtrDimScale((Node*) CurrentSpread);
	ERROR2IF(pDimScale == NULL, FALSE, "NULL DimScalePtr");
	String_256 FieldContents = GetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), &Valid);

	// Bump it
	if (Valid)
	{
		switch (pQuickShapeBase->EditFieldMode)
		{
			// bump centre up
			case QuickShapeBase::EDITF_CENTRE:
			{
				MILLIPOINT CurrentPos = 0;
				Valid = pDimScale->ConvertToMillipoints(FieldContents, &CurrentPos);
				if (Valid)
				{
					CurrentPos -= OpNudge::GetNudgeStep();
					SetEdit(_R(IDC_EDIT_REGSHAPETOOL_LOWER), CurrentPos, TRUE);
				}
			}
			break;
			
			case QuickShapeBase::EDITF_SIZE:
			{
				MILLIPOINT CurrentPos = 0;
				Valid = pDimScale->ConvertToMillipoints(FieldContents, &CurrentPos);
				if (Valid)
				{
					CurrentPos -= OpNudge::GetNudgeStep();
					if (CurrentPos < 0)
						CurrentPos = 0;
					SetEdit(_R(IDC_EDIT_REGSHAPETOOL_LOWER), CurrentPos, TRUE);
				}
			}
			break;

			// Rotate anti-clockwise by 5 degrees
			case QuickShapeBase::EDITF_MAJOR:
			{
				double CurrentRot = GetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), -360, 360, 0, &Valid);
				if (Valid)
				{
					CurrentRot = RestrictAngleZero360(CurrentRot - 5);
					SetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), CurrentRot);
				}
			}
			break;

			// Rotate stellation offset anti-clockwise by 5 degrees
			case QuickShapeBase::EDITF_STELLATION:
			{
				double CurrentRot = GetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), -180, 180, 0, &Valid);
				if (Valid)
				{
					CurrentRot = RestrictAngleMinus180(CurrentRot - 5);
					SetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), CurrentRot);
				}
			}
			break;

			case QuickShapeBase::EDITF_CURVE:
			{
				double CurrentCurve = GetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), 0, 100, 0, &Valid);
				if (Valid)
				{
					CurrentCurve -= .05;
					if (CurrentCurve < 0)
						CurrentCurve = 0;
					SetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), CurrentCurve);
				}
			}
			break;

			default:
				ERROR2(FALSE, "Unknown edit field mode found when bumping up");
		} 
	}
	
	PaintGadgetNow(_R(IDC_EDIT_REGSHAPETOOL_LOWER));
	return TRUE;
}



/********************************************************************************************

>	BOOL QuickShapeBaseInfoBarOp::BumpDown()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Called when the user clicks on the bump down button.  Does different things
				depeding on what the edit fields are displaying
	Errors:		-
	SeeAlso:	QuickShapeBaseInfoBarOp::BumpLeft, QuickShapeBaseInfoBarOp::BumpRight, QuickShapeBaseInfoBarOp::BumpUp

********************************************************************************************/
BOOL QuickShapeBaseInfoBarOp::BumpDown()
{
	ERROR2IF(pQuickShapeBase==NULL, FALSE, "NULL tool pointer");

	// Get the current edit field
	BOOL Valid = TRUE;
	ERROR2IF(CurrentSpread == NULL, FALSE, "BumpRight found CurrentSpread was NULL");	
	DimScale* pDimScale = DimScale::GetPtrDimScale((Node*) CurrentSpread);
	ERROR2IF(pDimScale == NULL, FALSE, "NULL DimScalePtr");
	String_256 FieldContents = GetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), &Valid);

	// Bump it
	if (Valid)
	{
		switch (pQuickShapeBase->EditFieldMode)
		{
			// bump centre down, and size down
			case QuickShapeBase::EDITF_CENTRE:
			{
				MILLIPOINT CurrentPos = 0;
				Valid = pDimScale->ConvertToMillipoints(FieldContents, &CurrentPos);
				if (Valid)
				{
					CurrentPos += OpNudge::GetNudgeStep();
					SetEdit(_R(IDC_EDIT_REGSHAPETOOL_LOWER), CurrentPos, TRUE);
				}
			}
			break;
			
			case QuickShapeBase::EDITF_SIZE:
			{
				MILLIPOINT CurrentPos = 0;
				Valid = pDimScale->ConvertToMillipoints(FieldContents, &CurrentPos);
				if (Valid)
				{
					CurrentPos += OpNudge::GetNudgeStep();
					SetEdit(_R(IDC_EDIT_REGSHAPETOOL_LOWER), CurrentPos, TRUE);
				}
			}
			break;

			// Rotate clockwise by 5 degrees
			case QuickShapeBase::EDITF_MAJOR:
			{
				double CurrentRot = GetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), -360, 360, 0, &Valid);
				if (Valid)
				{
					CurrentRot = RestrictAngleZero360(CurrentRot + 5);
					SetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), CurrentRot);
				}
			}
			break;

			// Rotate stellation offset clockwise by 5 degrees
			case QuickShapeBase::EDITF_STELLATION:
			{
				double CurrentRot = GetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), -180, 180, 0, &Valid);
				if (Valid)
				{
					CurrentRot = RestrictAngleMinus180(CurrentRot + 5);
					SetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), CurrentRot);
				}
			}
			break;

			case QuickShapeBase::EDITF_CURVE:
			{
				double CurrentCurve = GetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), 0, 100, 0, &Valid);
				if (Valid)
				{
					CurrentCurve += .05;
					if (CurrentCurve < 0)
						CurrentCurve = 0;
					SetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), CurrentCurve);
				}
			}
			break;

			default:
				ERROR2(FALSE, "Unknown edit field mode found when bumping down");
		} 
	}
	
	PaintGadgetNow(_R(IDC_EDIT_REGSHAPETOOL_LOWER));
	return TRUE;
}



/********************************************************************************************
>	double QuickShapeBaseInfoBarOp::RestrictAngleZero360(double Current)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	32/8/95
	Inputs:		Current - an angle
	Outputs:	-
	Returns:	Current restricted between 0 and 360.  eg 372 becomes 12
	Purpose:	Restricts an angle for display purposes
	Errors:		-
	SeeAlso:	QuickShapeBaseInfoBarOp::RestrictAngleMinus180
********************************************************************************************/
double QuickShapeBaseInfoBarOp::RestrictAngleZero360(double Current)
{
	while (Current >= 360.0)
		Current -= 360;
	while (Current < 0)
		Current += 360;

	return Current;
}



/********************************************************************************************
>	double QuickShapeBaseInfoBarOp::RestrictAngleMinus180(double Current)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	32/8/95
	Inputs:		Current - an angle
	Outputs:	-
	Returns:	Current restricted between -180 and 180.  eg 192 becomes -168
	Purpose:	Restricts an angle for display purposes
	Errors:		-
	SeeAlso:	QuickShapeBaseInfoBarOp::RestrictAngleZero360
********************************************************************************************/
double QuickShapeBaseInfoBarOp::RestrictAngleMinus180(double Current)
{
	while (Current <= -180.0)
		Current += 360;
	while (Current > 180.0)
		Current -= 360;

	return Current;
}



/********************************************************************************************
>	void QuickShapeBaseInfoBarOp::SetHelp()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets the bubble and status line help on the edit fields depending on the current
				menu setting. 
	Errors:		-
	SeeAlso:	-
********************************************************************************************/
void QuickShapeBaseInfoBarOp::SetHelp()
{
	if (pQuickShapeBase!=NULL)
	{
		switch (pQuickShapeBase->EditFieldMode)
		{
			case QuickShapeBase::EDITF_CENTRE:
				SetGadgetHelp(_R(IDC_EDIT_REGSHAPETOOL_UPPER), _R(IDBBL_REGSHAPETOOL_CENTREX), _R(IDS_REGSHAPETOOL_CENTREX)); 
				SetGadgetHelp(_R(IDC_EDIT_REGSHAPETOOL_LOWER), _R(IDBBL_REGSHAPETOOL_CENTREY), _R(IDS_REGSHAPETOOL_CENTREY)); 
				break;
			case QuickShapeBase::EDITF_MAJOR:
				SetGadgetHelp(_R(IDC_EDIT_REGSHAPETOOL_UPPER), _R(IDBBL_REGSHAPETOOL_SIZE), _R(IDS_REGSHAPETOOL_SIZE)); 
				SetGadgetHelp(_R(IDC_EDIT_REGSHAPETOOL_LOWER), _R(IDBBL_REGSHAPETOOL_ROTATION), _R(IDS_REGSHAPETOOL_ROTATION)); 
				break;
			case QuickShapeBase::EDITF_STELLATION:
				SetGadgetHelp(_R(IDC_EDIT_REGSHAPETOOL_UPPER), _R(IDBBL_REGSHAPETOOL_STELLSIZE), _R(IDS_REGSHAPETOOL_STELLSIZE)); 
				SetGadgetHelp(_R(IDC_EDIT_REGSHAPETOOL_LOWER), _R(IDBBL_REGSHAPETOOL_STELLOFF), _R(IDS_REGSHAPETOOL_STELLOFF)); 
				break;
			case QuickShapeBase::EDITF_CURVE:
				SetGadgetHelp(_R(IDC_EDIT_REGSHAPETOOL_UPPER), _R(IDBBL_REGSHAPETOOL_PRIMER), _R(IDS_REGSHAPETOOL_PRIMER)); 
				SetGadgetHelp(_R(IDC_EDIT_REGSHAPETOOL_LOWER), _R(IDBBL_REGSHAPETOOL_STELLR), _R(IDS_REGSHAPETOOL_STELLR)); 
				break;
			case QuickShapeBase::EDITF_SIZE:
				SetGadgetHelp(_R(IDC_EDIT_REGSHAPETOOL_UPPER), _R(IDBBL_REGSHAPE_WIDTH), _R(IDS_REGSHAPE_WIDTH)); 
				SetGadgetHelp(_R(IDC_EDIT_REGSHAPETOOL_LOWER), _R(IDBBL_REGSHAPE_HEIGHT), _R(IDS_REGSHAPE_HEIGHT)); 
				break;
			case QuickShapeBase::EDITF_ROT:
				SetGadgetHelp(_R(IDC_EDIT_REGSHAPETOOL_UPPER), _R(IDBBL_REGSHAPETOOL_ROTATION), _R(IDS_REGSHAPETOOL_ROTATION)); 
				SetGadgetHelp(_R(IDC_EDIT_REGSHAPETOOL_LOWER), 0, 0); 
				break;
			default:
				ERROR3("Unknown edit field mode found when setting help");
		} 
	}
}



/*********************************************************************************************

	Preference:	CreationMode
	Section:	RegularShapeTool
	Range:		0, 1 or 2
	Purpose:	The way shapes are created by dragging.
	SeeAlso:	-

**********************************************************************************************/ 
/*********************************************************************************************

	Preference:	NumSides
	Section:	RegularShapeTool
	Range:		3 to 99 inclusive
	Purpose:	The number of sides to create shapes with
	SeeAlso:	-

**********************************************************************************************/ 
/*********************************************************************************************

	Preference:	CreatePolygons
	Section:	RegularShapeTool
	Range:		TRUE or FALSE
	Purpose:	Whether to create polygons (TRUE) or ellipses (FALSE)
	SeeAlso:	-

**********************************************************************************************/ 
/*********************************************************************************************

	Preference:	CreateStellated
	Section:	RegularShapeTool
	Range:		TRUE or FALSE
	Purpose:	Whether to create stellated shapes or not
	SeeAlso:	-

**********************************************************************************************/ 
/*********************************************************************************************

	Preference:	CreateCurved
	Section:	RegularShapeTool
	Range:		TRUE or FALSE
	Purpose:	Whether to create shapes with rounded corners
	SeeAlso:	-

**********************************************************************************************/ 



/********************************************************************************************

>	QuickShapeTool::QuickShapeTool()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the QuickShape tool
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
QuickShapeTool::QuickShapeTool() : QuickShapeBase()
{

}



/********************************************************************************************

>	QuickShapeTool::~QuickShapeTool()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	QuickShapes tools destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
QuickShapeTool::~QuickShapeTool()
{

}



/********************************************************************************************

>	BOOL QuickShapeTool::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/03/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Called to initialise the QuickShape tool.  Also initialases the base class
	Errors:		-
	SeeAlso:	QuickShapeBase::Init

********************************************************************************************/
BOOL QuickShapeTool::Init()
{
	// Rread in the preference settings from the file
	GetApplication()->DeclareSection(_T("RegularShapeTool"),5);
	GetApplication()->DeclarePref(_T("RegularShapeTool"),_T("CreationType"), &CreationMode, OpNewRegShape::RADIUS, OpNewRegShape::BOUNDS);
	GetApplication()->DeclarePref(_T("RegularShapeTool"),_T("NumberSides"), &NumSides, 3, 99);
	GetApplication()->DeclarePref(_T("RegularShapeTool"),_T("CreatePolygons"), &CreatePolygons, 0, 1);
	GetApplication()->DeclarePref(_T("RegularShapeTool"),_T("CreateStellated"), &CreateStellated, 0, 1);
	GetApplication()->DeclarePref(_T("RegularShapeTool"),_T("CreateCurved"), &CreateCurved, 0, 1);

	// Initialise the base class first
	return QuickShapeBase::Init();
}



/********************************************************************************************

>	void QuickShapeTool::Describe(void *InfoPtr)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Inputs:		InfoPtr - 	A pointer to a tool info block.
	Outputs:	InfoPtr - 	The structure pointed to by InfoPtr will have had all the info
							that this version of the Tool knows about
	Purpose:	Allows the tool manager to extract information about the tool

********************************************************************************************/

void QuickShapeTool::Describe(void *InfoPtr)
{
	// Cast structure into the latest one we understand.
	ToolInfo_v1 *Info = (ToolInfo_v1 *) InfoPtr;

	Info->InfoVersion = 1;
	
	Info->InterfaceVersion = GetToolInterfaceVersion();
		
	Info->Version = 1;
	Info->ID      = GetID();
	Info->TextID  = _R(IDS_REGSHAPE_TOOL);

	Info->Family  = FamilyName;
	Info->Name    = ToolName;
	Info->Purpose = Purpose;
	Info->Author  = Author;

	Info->BubbleID = _R(IDBBL_REGSHAPE_TOOLBOX);
}


void QuickShapeTool::SetNumSides(INT32 NewValue)
{
	if (!pQuickShapeBaseInfoBarOp->AreShapesSelected())
		NumSides = NewValue;
}

void QuickShapeTool::SetCreationMode(INT32 NewValue)
{
	CreationMode = NewValue;
}

void QuickShapeTool::SetPolygon(BOOL NewValue)
{
	if (!pQuickShapeBaseInfoBarOp->AreShapesSelected())
		CreatePolygons = NewValue;
}

void QuickShapeTool::SetStellation(BOOL NewValue)
{
	if (!pQuickShapeBaseInfoBarOp->AreShapesSelected())
		CreateStellated = NewValue;
}

void QuickShapeTool::SetCurved(BOOL NewValue)
{
	if (!pQuickShapeBaseInfoBarOp->AreShapesSelected())
		CreateCurved = NewValue;
}



/********************************************************************************************

>	virtual BOOL QuickShapeTool::IsInterestingShape(NodeRegularShape* pShape)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/03/95
	Inputs:		pShape - points to a regular shape in the document
	Outputs:	-
	Returns:	TRUE if the shape is one processed by this tool
	Purpose:	Always returns TRUE as the QuickShape tool deals with all shapes

********************************************************************************************/
BOOL QuickShapeTool::IsInterestingShape(NodeRegularShape* pShape)
{
	return TRUE;
}



/********************************************************************************************
>	BOOL QuickShapeBaseInfoBarOp::EditCommitCentre(BOOL UpperCommit, EditRegularShapeParam* ChangeData)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/1/96
	Inputs:		UpperCommit - TRUE if the upper edit field was commited; FALSE for the lower one
				ChangeData - the changes data
	Outputs:	Changes to be made are output into ChangesData
	Returns:	TRUE if all ok, FALSE if an error occured
	Purpose:	Does the processing involved when the user changes a vales in the edit fields
				when in centre mode.
********************************************************************************************/
BOOL QuickShapeBaseInfoBarOp::EditCommitCentre(BOOL UpperCommit, EditRegularShapeParam* ChangeData)
{
	// The edit fields define a coordinate to move the centre points of all selected shapes to
	DocCoord NewCentre;
	if (!GetFieldCoord(&NewCentre, CurrentSpread) )
	{
		Error::SetError(_R(IDE_REGSHAPE_CENTREOFFSPREAD));
		return FALSE;
	}

	// Check the centre is on the spread
	BOOL OnSpread = TRUE;
	if (CurrentSpread != NULL)
	{
		DocRect SpreadBounds = CurrentSpread->GetPasteboardRect(FALSE);
		SpreadBounds = SpreadBounds.ToSpread(CurrentSpread, DocView::GetSelected());

		OnSpread = SpreadBounds.ContainsCoord(NewCentre);
	}

	if (OnSpread)
	{
		ChangeData->TranslateTo = NewCentre;
		return TRUE;
	}
	else
	{
		Error::SetError(_R(IDE_REGSHAPE_CENTREOFFSPREAD));
		return FALSE;
	}
}



/********************************************************************************************
>	BOOL QuickShapeBaseInfoBarOp::EditCommitSizeAndRotation(BOOL UpperCommit, EditRegularShapeParam* ChangeData)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/1/96
	Inputs:		UpperCommit - TRUE if the upper edit field was commited; FALSE for the lower one
				ChangeData - the changes data
	Outputs:	Changes to be made are output into ChangesData
	Returns:	TRUE if all ok, FALSE if an error occured
	Purpose:	Does the processing involved when the user changes a vales in the edit fields
				when in size and rotation mode.
********************************************************************************************/
BOOL QuickShapeBaseInfoBarOp::EditCommitSizeAndRotation(BOOL UpperCommit, EditRegularShapeParam* ChangeData)
{
	// Get the length string from the upper edit field, checking that it is not the "many" string
	String_32 EditFieldString;
	String_16 ManyField(_R(IDS_MANY));
	BOOL Many = FALSE;
	BOOL Valid = TRUE;
	EditFieldString = GetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_UPPER), &Valid);
	if (ManyField == EditFieldString)
		Many = TRUE;
	
	// Convert the length string to millipoints
	DimScale* pDimScale = DimScale::GetPtrDimScale((Node*) CurrentSpread);
	ERROR2IF (pDimScale == NULL, FALSE, "Null DimScale* in QuickShapeBaseInfoBarOp::GetFieldCoord");
	INT32 Length = 0;
	if (Valid && !Many)
		Valid = pDimScale->ConvertToMillipoints(EditFieldString, &Length); 

	if (!Valid && UpperCommit)
	{
		Error::SetError(_R(IDE_INVALID_RADIUS));
		return FALSE;
	}
	else
	{
		if (Valid && !Many)
		{
			// Check the new length is less than the diagonal of the spread
			INT32 Diagonal = 0;
			if (CurrentSpread != NULL)
			{
				DocRect SpreadBounds = CurrentSpread->GetPasteboardRect(FALSE);
				SpreadBounds = SpreadBounds.ToSpread(CurrentSpread, DocView::GetSelected());
				Diagonal = (INT32)sqrt((double)SpreadBounds.Width()*(double)SpreadBounds.Width() + (double)SpreadBounds.Height()*(double)SpreadBounds.Height());
			}

			if (Length/2 == 0)
			{
				Error::SetError(_R(IDS_REGSHAPETOOL_TOOSMALL));
				return FALSE;
			}
			else if ((CurrentSpread != NULL) && (Length > Diagonal))
			{
				Error::SetError(_R(IDS_REGSHAPETOOL_TOOLARGE));
				return FALSE;
			}
			else
				ChangeData->SetRadiusLength = Length/2;
		}
	}

	// Get the rotation value from the upper edit field, checking that it is not the "many" string
	Many = (ManyField == GetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), &Valid));
	double Angle = GetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), -360.0, 360.0, 0, &Valid);

	if (!Valid && !UpperCommit)
	{
		Error::SetError(_R(IDE_INVALID_ANGLE));
		return FALSE;
	}
	else
	{
		// convert Angle back from the display value
		if (Valid && !Many)
		{
			Angle = -Angle;
			if (Angle > 0)
				Angle -= 360;
			ChangeData->RotateTo = Angle*(PI/180) + (PI/2);
		}
	}

	return TRUE;
}



/********************************************************************************************
>	BOOL QuickShapeBaseInfoBarOp::EditCommitStellation(BOOL UpperCommit, EditRegularShapeParam* ChangeData)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/1/96
	Inputs:		UpperCommit - TRUE if the upper edit field was commited; FALSE for the lower one
				ChangeData - the changes data
	Outputs:	Changes to be made are output into ChangesData
	Returns:	TRUE if all ok, FALSE if an error occured
	Purpose:	Does the processing involved when the user changes a vales in the edit fields
				when in stellation mode.
********************************************************************************************/
BOOL QuickShapeBaseInfoBarOp::EditCommitStellation(BOOL UpperCommit, EditRegularShapeParam* ChangeData)
{
	// The upper field is the length to make the stellation radi of all selected shapes
	String_32 EditFieldString;
	String_16 ManyField(_R(IDS_MANY));
	BOOL Many = FALSE;
	BOOL Valid = TRUE;
	EditFieldString = GetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_UPPER), &Valid);
	if (ManyField == EditFieldString)
		Many = TRUE;

	// Convert string in to millipoints
	DimScale* pDimScale = DimScale::GetPtrDimScale((Node*) CurrentSpread);
	ERROR2IF (pDimScale == NULL, FALSE, "Null DimScale*");
	INT32 Length = 0;
	if (Valid)
		Valid = pDimScale->ConvertToMillipoints(EditFieldString, &Length); 

	if (!Valid && UpperCommit)
	{
		Error::SetError(_R(IDE_INVALID_RADIUS));
		return FALSE;
	}
	else
	{
		if (Valid && !Many)
			ChangeData->SetStellationLength = Length;
	}

	// The lower field is the rotation angle to set on all the shapes
	Many = (ManyField == GetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), &Valid));
	double Angle = GetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), -180.0, 180.0, 0, &Valid);

	if (!Valid && !UpperCommit)
	{
		Error::SetError(_R(IDE_INVALID_STELLATIONOFFSET));
		return FALSE;
	}
	else
	{
		// convert Angle back from the display value
		if (Valid && !Many)
			ChangeData->SetStellationOffset = float(Angle);
	}

	return TRUE;
}



/********************************************************************************************
>	BOOL QuickShapeBaseInfoBarOp::EditCommitCurvature(BOOL UpperCommit, EditRegularShapeParam* ChangeData)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/1/96
	Inputs:		UpperCommit - TRUE if the upper edit field was commited; FALSE for the lower one
				ChangeData - the changes data
	Outputs:	Changes to be made are output into ChangesData
	Returns:	TRUE if all ok, FALSE if an error occured
	Purpose:	Does the processing involved when the user changes a vales in the edit fields
				when in curvature mode.
********************************************************************************************/
BOOL QuickShapeBaseInfoBarOp::EditCommitCurvature(BOOL UpperCommit, EditRegularShapeParam* ChangeData)
{
	// The upper field is the primary curvature ratio
	String_16 ManyField(_R(IDS_MANY));
	BOOL Valid = TRUE;
	BOOL Many = FALSE;
	if (ManyField == GetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_UPPER), &Valid))
		Many = TRUE;
	double NewRatio = GetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_UPPER), 0.0, 100.0, 0, &Valid);

	if (!Valid && UpperCommit)
	{
		Error::SetError(_R(IDE_INVALID_CURVATURE));
		return FALSE;
	}
	else
	{
		if (Valid && !Many)
			ChangeData->NewPrimaryCurveToPrimary = float(NewRatio);
	}

	// The lower field is the stellation curvature ratio
	Many = (ManyField == GetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), &Valid));
	NewRatio = GetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), 0.0, 100.0, 0, &Valid);

	if (!Valid && !UpperCommit)
	{
		Error::SetError(_R(IDE_INVALID_CURVATURE));
		return FALSE;
	}
	else
	{
		if (Valid && !Many)
			ChangeData->NewStellCurveToStell = float(NewRatio);
	}

	return TRUE;
}



/********************************************************************************************
>	BOOL QuickShapeBaseInfoBarOp::EditCommitWidthAndHeight(BOOL UpperCommit, EditRegularShapeParam* ChangeData)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/1/96
	Inputs:		UpperCommit - TRUE if the upper edit field was commited; FALSE for the lower one
				ChangeData - the changes data
	Outputs:	Changes to be made are output into ChangesData
	Returns:	TRUE if all ok, FALSE if an error occured
	Purpose:	Does the processing involved when the user changes a vales in the edit fields
				when in width and height mode.
********************************************************************************************/
BOOL QuickShapeBaseInfoBarOp::EditCommitWidthAndHeight(BOOL UpperCommit, EditRegularShapeParam* ChangeData)
{
	ERROR2IF(pQuickShapeBase==NULL, FALSE, "NULL tool pointer");

	// The upper field is the length to make the minor axis of all selected shapes
	String_32 EditFieldString;
	String_16 ManyField(_R(IDS_MANY));
	BOOL Many = FALSE;
	BOOL Valid = TRUE;
	EditFieldString = GetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_UPPER), &Valid);
	if (ManyField == EditFieldString)
		Many = TRUE;
	
	// Convert string to millipoints
	DimScale* pDimScale = DimScale::GetPtrDimScale((Node*) CurrentSpread);
	ERROR2IF (pDimScale == NULL, FALSE, "Null DimScale* in QuickShapeBaseInfoBarOp::GetFieldCoord");
	INT32 Length = 0;
	if (Valid && !Many)
		Valid = pDimScale->ConvertToMillipoints(EditFieldString, &Length); 

	if (!Valid && UpperCommit)
	{
		Error::SetError(_R(IDE_INVALID_RADIUS));
		return FALSE;
	}
	else
	{
		if (Valid && !Many)
		{
			if (pQuickShapeBase->IsRectangle())
				Length = (INT32)(Length / cos (PI/4));

			// Check the new length is less than the diagonal of the spread
			INT32 Diagonal = 0;
			if (CurrentSpread != NULL)
			{
				DocRect SpreadBounds = CurrentSpread->GetPasteboardRect(FALSE);
				SpreadBounds = SpreadBounds.ToSpread(CurrentSpread, DocView::GetSelected());
				Diagonal = (INT32)sqrt((double)SpreadBounds.Width()*(double)SpreadBounds.Width() + (double)SpreadBounds.Height()*(double)SpreadBounds.Height());
			}

			if (Length/2 == 0)
			{
				Error::SetError(_R(IDS_REGSHAPETOOL_TOOSMALL));
				return FALSE;
			}
			else if (Length > Diagonal)
			{
				Error::SetError(_R(IDS_REGSHAPETOOL_TOOLARGE));
				return FALSE;
			}
			else
				ChangeData->SetMinorAxisLength = Length/2;
		}
	}

	// The lower field is the length to make the major axis of all selected shapes
	EditFieldString = GetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_LOWER), &Valid);
	if (ManyField == EditFieldString)
		Many = TRUE;
	
	if (Valid && !Many)
		Valid = pDimScale->ConvertToMillipoints(EditFieldString, &Length); 

	if (!Valid && !UpperCommit)
	{
		Error::SetError(_R(IDE_INVALID_RADIUS));
		return FALSE;
	}
	else
	{
		if (Valid && !Many)
		{
			if (pQuickShapeBase->IsRectangle())
				Length = (INT32)(Length / cos (PI/4));

			// Check the new length is less than the diagonal of the spread
			INT32 Diagonal = 0;
			if (CurrentSpread != NULL)
			{
				DocRect SpreadBounds = CurrentSpread->GetPasteboardRect(FALSE);
				SpreadBounds = SpreadBounds.ToSpread(CurrentSpread, DocView::GetSelected());
				Diagonal = (INT32)sqrt((double)SpreadBounds.Width()*(double)SpreadBounds.Width() + (double)SpreadBounds.Height()*(double)SpreadBounds.Height());
			}

			if (Length/2 == 0)
			{
				Error::SetError(_R(IDS_REGSHAPETOOL_TOOSMALL));
				return FALSE;
			}
			else if (Length > Diagonal)
			{
				Error::SetError(_R(IDS_REGSHAPETOOL_TOOLARGE));
				return FALSE;
			}
			else
				ChangeData->SetMajorAxisLength = Length/2;
		}
	}

	return TRUE;
}


/********************************************************************************************
>	BOOL QuickShapeBaseInfoBarOp::EditCommitRotation(BOOL UpperCommit, EditRegularShapeParam* ChangeData)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/1/96
	Inputs:		UpperCommit - TRUE if the upper edit field was commited; FALSE for the lower one
				ChangeData - the changes data
	Outputs:	Changes to be made are output into ChangesData
	Returns:	TRUE if all ok, FALSE if an error occured
	Purpose:	Does the processing involved when the user changes a vales in the edit fields
				when in rotation mode.
********************************************************************************************/
BOOL QuickShapeBaseInfoBarOp::EditCommitRotation(BOOL UpperCommit, EditRegularShapeParam* ChangeData)
{
	// The lower field is the rotation angle to set on all the shapes
	String_16 ManyField(_R(IDS_MANY));
	BOOL Valid = TRUE;
	BOOL Many = (ManyField == GetStringGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_UPPER), &Valid));
	double Angle = GetDoubleGadgetValue(_R(IDC_EDIT_REGSHAPETOOL_UPPER), -360.0, 360.0, 0, &Valid);

	if (!Valid && UpperCommit)
	{
		Error::SetError(_R(IDE_INVALID_ANGLE));
		return FALSE;
	}
	else
	{
		// convert Angle back from the display value
		if (Valid && !Many)
		{
			Angle = -Angle;
			if (Angle > 0)
				Angle -= 360;
			ChangeData->RotateTo = Angle*(PI/180) + (PI/2);
		}
	}

	return TRUE;
}
