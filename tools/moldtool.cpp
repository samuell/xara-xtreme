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
// Implementation of the mould tool

/*
*/

#include "camtypes.h"
//#include "resource.h"
#include "oilfiles.h"
#include "csrstack.h"
//#include "viewrc.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "markn.h"
//#include "mike.h"
#include "moldtool.h"
#include "osrndrgn.h"
#include "nodeshap.h"
//#include "opdesc.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "paths.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "moldpers.h"
#include "moldenv.h"
#include "moldedit.h"
//#include "mouldbar.h"
#include "progress.h"
#include "blobs.h"
#include "ndmldpth.h"
#include "clipint.h"
#include "layer.h"
#include "objchge.h"
#include "keypress.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "attrmap.h"
#include "ophist.h"

//#include "will2.h"

//class NodeRenderableInk;

DECLARE_SOURCE( "$Revision$" );

CC_IMPLEMENT_MEMDUMP( MouldTool,Tool_v1 )
CC_IMPLEMENT_MEMDUMP( CreateMouldParam, OpParam )
CC_IMPLEMENT_DYNCREATE( MouldInfoBarOp,InformationBarOp )
CC_IMPLEMENT_DYNCREATE( OpCreateNewMould, OpMouldLibSel )
CC_IMPLEMENT_DYNCREATE( OpRectangularEnvelope, OpMouldLibSel )
CC_IMPLEMENT_DYNCREATE( OpRectangularPerspective, OpMouldLibSel )
CC_IMPLEMENT_DYNCREATE( OpRemoveMould, OpMouldLibSel )
CC_IMPLEMENT_DYNCREATE( OpCopyMouldShape, OpMouldLibSel )
CC_IMPLEMENT_DYNCREATE( OpPasteMouldShape, OpMouldLibSel )
CC_IMPLEMENT_DYNCREATE( OpPasteEnvelope, OpPasteMouldShape )
CC_IMPLEMENT_DYNCREATE( OpPastePerspective, OpPasteMouldShape )
CC_IMPLEMENT_DYNCREATE( OpToggleMouldGrid, Operation )
CC_IMPLEMENT_DYNCREATE( OpDetachMould, UndoableOperation )
CC_IMPLEMENT_DYNCREATE( OpRotateMould, OpMouldLib )
CC_IMPLEMENT_MEMDUMP( MouldBecomeA, BecomeA )

// Must come after the last CC_IMPLEMENT.. macro
#define new CAM_DEBUG_NEW     

// These are still char* while we wait for resource technology to be developed for modules
TCHAR* MouldTool::FamilyName	= _T("Mould Tools");
TCHAR* MouldTool::ToolName 	= _T("Mould Tool");
TCHAR* MouldTool::Purpose 	= _T("Mould manipulation");
TCHAR* MouldTool::Author 	= _T("Mike");

// Init those other useful static vars
BOOL			MouldTool::CurrentTool 			= FALSE;
MouldInfoBarOp*	MouldTool::pMouldInfoBarOp		= NULL;



/********************************************************************************************

>	MouldTool::MouldTool()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Purpose:	Default Constructor.
				Other initialisation is done in MouldTool::Init which is called by the Tool Manager
	SeeAlso:	MouldTool::Init

********************************************************************************************/

MouldTool::MouldTool()
{
	pcCurrentCursor = NULL;
}

/********************************************************************************************

>	MouldTool::~MouldTool()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Purpose:	Destructor (Virtual). Does nothing.

********************************************************************************************/

MouldTool::~MouldTool()
{
}


/********************************************************************************************

>	BOOL MouldTool::Init( INT32 Pass )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Returns:	FALSE if it does not want to be created, TRUE otherwise
	Purpose:	Used to check if the Tool was properly constructed
	SeeAlso:	MouldTool::MouldTool

********************************************************************************************/

BOOL MouldTool::Init()
{
	// Declare all your ops here and only succeed if all declarations succeed

	BOOL ok = TRUE;

	// register our mould tool operations
			ok = OpCreateNewMould::Init();
	if (ok) ok = OpRemoveMould::Init(); 
	if (ok) ok = OpCopyMouldShape::Init();
	if (ok) ok = OpPasteEnvelope::Init();
	if (ok) ok = OpPastePerspective::Init();
	if (ok) ok = OpToggleMouldGrid::Init();
	if (ok) ok = OpDetachMould::Init();
	if (ok) ok = OpRotateMould::Init();
	if (ok) ok = OpRectangularEnvelope::Init();
	if (ok) ok = OpRectangularPerspective::Init();

#if 0
	if (ok)
	{
		// This section reads in the infobar definition and creates an instance of
		// MouldInfoBarOp.  Also pMouldInfoBarOp, the ptr to the tool's infobar, is set up
		// after the infobar is successfully read and created.
		CCResTextFile 		file;				// Resource File
		MouldInfoBarOpCreate BarCreate;			// Object that creates MouldInfoBarOp objects

		 		ok = file.open(_R(IDM_MOULDTOOL_BAR), _R(IDT_INFO_BAR_RES));	// Open resource
		if (ok) ok = DialogBarOp::ReadBarsFromFile(file,BarCreate);		// Read and create info bar
		if (ok) file.close();									 		// Close resource

		ENSURE(ok,"Unable to load mouldbar.ini from resource\n"); 
	}

	if (ok)
	{
		// Info bar now exists.  Now get a pointer to it
		String_32 str = String_32(_R(IDS_MOLDTOOL_INFOBARNAME));
		DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(str);


				ok = (pDialogBarOp != NULL);
		if (ok) ok = pDialogBarOp->IsKindOf(CC_RUNTIME_CLASS(MouldInfoBarOp));
		if (ok) pMouldInfoBarOp = (MouldInfoBarOp*)pDialogBarOp;
 		if (ok) pMouldInfoBarOp->SetTool(this);				// Set a pointer from the op to this tool

		ENSURE(ok,"Error finding the mould tool info bar");
	}
#endif

	if (ok)
	{
		pMouldInfoBarOp = new MouldInfoBarOp();
		ok = (pMouldInfoBarOp != NULL);

 		if (ok) pMouldInfoBarOp->SetTool(this);				// Set a pointer from the op to this tool
	}

	return (ok);
}


/********************************************************************************************

>	void MouldTool::Describe(void *InfoPtr)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		InfoPtr - 	A pointer to a tool info block. It is passed cast to void* as
							the version of the tool is unknown at this point. Later versions 
							of the Tool class may have more items in this block, that this 
							tool will not use
	Outputs:	InfoPtr - 	The structure pointed to by InfoPtr will have had all the info
							that this version of the Tool knows about
	Purpose:	Allows the tool manager to extract information about the tool

********************************************************************************************/

void MouldTool::Describe(void *InfoPtr)
{
	// Cast structure into the latest one we understand.
	ToolInfo_v1 *Info = (ToolInfo_v1 *) InfoPtr;

	Info->InfoVersion = 1;
	
	Info->InterfaceVersion = GetToolInterfaceVersion();  // You should always have this line.
		
	// These are all arbitrary at present.
	Info->Version = 1;
	Info->ID      = GetID();
	Info->TextID  = _R(IDS_MOULD_TOOL);

	Info->Family  = FamilyName;
	Info->Name    = ToolName;
	Info->Purpose = Purpose;
	Info->Author  = Author;

	Info->BubbleID = _R(IDBBL_MOULD_TOOL);
}

/********************************************************************************************

>	virtual void MouldTool::SelectChange(BOOL isSelected)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		isSelected	- TRUE  = tool has been selected
							- FALSE = tool has been deselected
	Outputs:	-
	Returns:	-
	Purpose:	Starts up and closes down the mould tool
	Errors:		Debug warning if creating the cursor fails.
	SeeAlso:	-

********************************************************************************************/

void MouldTool::SelectChange(BOOL isSelected)
{
	if (isSelected)
	{
		// This tool has just been selected.  Create an appropriate cursor, and push it
		// onto the top of the cursor stack so it'll appear when the pointer moves into
		// our window.

		if (!CreateCursors()) return;

		// Push cursor but don't display now
		CurrentCursorID = CursorStack::GPush(pcNormalMouldCursor, FALSE);		
		pcCurrentCursor = pcNormalMouldCursor;

		// This tool is now the current one
		MouldTool::CurrentTool = TRUE;

		// Create and display the tool's info bar
		if (pMouldInfoBarOp != NULL)
			pMouldInfoBarOp->Create();

		// Which blobs do I want displayed
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
			pcCurrentCursor = NULL;
			CurrentCursorID = 0;
		}
		DestroyCursors();

		// Remove the info bar from view by deleting the actual underlying window
		if (pMouldInfoBarOp != NULL)
			pMouldInfoBarOp->Delete();

		// ensure any tool object blobs are removed.
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		if (BlobMgr != NULL)
		{
			BlobStyle bsRemoves;
			bsRemoves.ToolObject = TRUE;
			BlobMgr->RemoveInterest(bsRemoves);
		}

		// We're no longer the current tool
		MouldTool::CurrentTool = FALSE;
	}
}

/********************************************************************************************

>	BOOL MouldTool::CreateCursors()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if all the mould tool cursors have been successfully created
	Purpose:	Creates all the mould tool cursors
	SeeAlso:	-

********************************************************************************************/

BOOL MouldTool::CreateCursors()
{
	// This tool has just been selected.  Create the cursors.
	pcNormalMouldCursor = new Cursor(this, _R(IDC_MOULDDEFAULTCURSOR)); // _R(IDCSR_SEL_NORMAL)
		pcTransCoordCursor	= new Cursor(this, _R(IDC_MOVEMOULDCURSOR));
	pcTransOriginCursor = new Cursor(this, _R(IDCSR_SEL_GRADPOINT));

	if ( pcNormalMouldCursor==NULL ) goto bincursors;
	if ( pcTransCoordCursor==NULL) goto bincursors;
	if ( pcTransOriginCursor==NULL ) goto bincursors;
		
	if (!pcNormalMouldCursor->IsValid()) goto bincursors;
	if (!pcTransCoordCursor->IsValid()) goto bincursors;
	if (!pcTransOriginCursor->IsValid()) goto bincursors;

	return TRUE;

bincursors:
	DestroyCursors();
	return FALSE;
}

/********************************************************************************************

>	void MouldTool::DestroyCursors()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroys all the mould tool cursors
	SeeAlso:	-

********************************************************************************************/

void MouldTool::DestroyCursors()
{
	if (pcNormalMouldCursor != NULL) delete pcNormalMouldCursor;
	if (pcTransCoordCursor != NULL) delete pcTransCoordCursor;
	if (pcTransOriginCursor != NULL) delete pcTransOriginCursor;

	pcNormalMouldCursor = NULL;
	pcTransCoordCursor = NULL;
	pcTransOriginCursor = NULL;
}



/********************************************************************************************

>	void MouldTool::ChooseCursor(INT32 ctype)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/02/95
	Inputs:		Mould cursor ID of the cursor you want to flip to
	Outputs:	-
	Returns:	-
	Purpose:	Returns a pointer to the correct cursor given a mould cursor type

********************************************************************************************/

Cursor* MouldTool::ChooseCursor(INT32 ctype)
{
	switch (ctype)
	{
		case 1:
			return pcNormalMouldCursor;
			break;
		case 2:
			return pcTransOriginCursor;
			break;
		case 3:
			return pcTransCoordCursor;
			break;
	}
	return pcNormalMouldCursor;
}


/********************************************************************************************

>	void MouldTool::ChangeCursor(Cursor* cursor)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		ID of the cursor you want to flip to
	Outputs:	-
	Returns:	-
	Purpose:	Changes to the specified cursor. Will only change the cursor if it isn't 
				already	this cursor, so it doesn't flicker.
	Errors:		can fail if the cursor cannot be created - the cursor code will fail.
	SeeAlso:	-

********************************************************************************************/

void MouldTool::ChangeCursor(Cursor* cursor)
{
	// only change if this cursor is different from the current cursor
	if (cursor != pcCurrentCursor)
	{
		// set this cursor as the current cursor and immediately display it
		CursorStack::GSetTop(cursor, CurrentCursorID);
		// remember this is our current cursor
		pcCurrentCursor = cursor;
	}
}




/********************************************************************************************

>	void MouldTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		PointerPos 	- 	The DocCoord of the point where the mouse button was clicked
				Click 		-	Describes the type of click that was detected. 
				ClickMods 	-	Indicates which buttons caused the click and which modifers were
								pressed at the same time
				pSpread 	- 	The spread in which the click happened
	Returns:	-
	Purpose:	To handle a Mouse Click event for the Mould Tool.
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers

********************************************************************************************/

void MouldTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )
{
	if (ClickMods.Menu) return;							// Don't do anything if the user clicked the Menu button

	// Now scan through the selected paths, and see if we've clicked on any moulds
	DocView* pDocView = DocView::GetCurrent();
	ENSURE( pDocView != NULL, "MouldTool::BroadcastClick() Can't find current DocView");
	if (pDocView==NULL)
		return;

	// Find the selected range of objects
	SelRange* Selected = GetApplication()->FindSelection();
	Node* pNode = Selected->FindFirst();
	Node* pNext;
	BOOL claimed = FALSE;

	while ((pNode != NULL) && (!claimed))
	{
		// Find the next selected node
		pNext = Selected->FindNext(pNode);
		if (pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeMould))
			claimed = ((NodeMould*)pNode)->OnClick(PointerPos, Click, ClickMods, pSpread);
		pNode = pNext;
	}

	if (!claimed)
	{
		// call the base class ....

		DragTool::OnClick (PointerPos, Click, ClickMods, pSpread);
	}
}



/********************************************************************************************

>	void MouldTool::OnMouseMove( DocCoord PointerPos,Spread* pSpread, ClickModifiers ClickMod )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		PointerPos 	- 	The DocCoord of the point where the mouse has moved to
				pSpread 	- 	The spread in which the move occurred
				ClickMods	- 	The state of the various modifiers at the time of the mouse move
	Returns:	TRUE if it handled the Click, FALSE otherwise
	Purpose:	To handle a Mouse Move event for the Mould Tool.
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers

********************************************************************************************/

void MouldTool::OnMouseMove(DocCoord PointerPos,Spread* pSpread,ClickModifiers ClickMods)
{
	// Now scan through the selected moulds, and pass the move on
	DocView* pDocView = DocView::GetSelected();
	ENSURE( pDocView != NULL, "MouldTool::OnMouseMove() Can't find selected DocView");
	if (pDocView==NULL)
		return;

	INT32 ctype,msgres;

	BOOL claimed = DetermineClickEffect(PointerPos, pSpread, ClickMods, &ctype, &msgres);

	if (!claimed)
	{
		ctype=1;
		msgres=1;
	}

	// update the cursor if someones changed it
	Cursor* pCursor = ChooseCursor(ctype);
	if (pCursor!=NULL)
		ChangeCursor(pCursor);

	String_256 StatusMsg("");
	GenerateStatusLineText(msgres, &StatusMsg);
	GetApplication()->UpdateStatusBarText(&StatusMsg);
}


/**************************************************************************************************************

> BOOL MouldTool::DetermineClickEffect(DocCoord PointerPos, Spread* pSpread,ClickModifiers ClickMods,  INT32* ctype, INT32* msgres)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/02/95
	Inputs:		PointerPos is the mouse position
				pSpread is a pointer to the spread containing the mouse position
	Outputs:	ctype  = an INT32 value indicating what resource cursor to use.
				msgres = an INT32 value indicating what resource message to use.
	Returns:	TRUE if
	Purpose:	Used when single clicking. This routine determines what effect a click will have.
				In this tool, clicking will add a segment to the end of a line, adjust the last element of
				a path or start a new path entirely.

***************************************************************************************************************/

BOOL MouldTool::DetermineClickEffect(DocCoord PointerPos,Spread* pSpread,ClickModifiers ClickMods, INT32* ctype, INT32* msgres)
{
	// Find the selected range of objects
	SelRange* Selected = GetApplication()->FindSelection();
	Node* pNode = Selected->FindFirst();
	Node* pNext;
	BOOL claimed = FALSE;

	// Set up a default value first
	(*msgres)=0;
		
	while ((pNode != NULL) && (!claimed))
	{
		// Find the next selected node
		pNext = Selected->FindNext(pNode);
		if (pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeMould))
			claimed = ((NodeMould*)pNode)->OnMouseMove(PointerPos, pSpread, ClickMods, ctype, msgres);
		pNode = pNext;
	}

	return claimed;
}




/********************************************************************************************

>	virtual BOOL MouldTool::GetStatusLineText(String_256* ptext,
											  Spread* pSpread,
											  DocCoord DocPos, 
											  ClickModifiers ClickMods)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/02/95
	Inputs:		pSpread   - pointer to spread under mouse (else NULL)
				DocPos    - position of mouse in doc (in spread coords)
				ClickMods - mouse click modifiers
	Outputs:	ptext - text for status line
	Returns:	TRUE if outputting valid text
	Purpose:	generate up-to-date text for the status line (called on idles)
	Errors:		ERROR1 if ptext is NULL

********************************************************************************************/

BOOL MouldTool::GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord coord, ClickModifiers mods)
{
	ERROR2IF(ptext==NULL,FALSE,"MouldTool::GetStatusLineText() passed a NULL text buffer");

	*ptext = "";

	// find what type of click we will generate
	INT32 ctype,msgres;
	DetermineClickEffect(coord, pSpread, mods, &ctype, &msgres);
	GenerateStatusLineText(msgres, ptext);

	return TRUE;
}




/********************************************************************************************

>	void MouldTool::GenerateStatusLineText(INT32 msgres, String_256* pStatusMsg)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/02/95
	Inputs:		msgres		= identifier of message resource
				pStatusMsg	= pointer to a 256 character text buffer
				
	Outputs:	pStatusMsg	- holds a text string
	Returns:	-
	Purpose:	Loads up an appropriate help string into the text buffer
				pointed to by pStatusMsg, using the msgres resource identifier
	Errors:		-

********************************************************************************************/

void MouldTool::GenerateStatusLineText(INT32 msgres, String_256* pStatusMsg)
{
	switch (msgres)
	{
		case 2:
			pStatusMsg->Load(_R(IDS_DRAGVPOINT), Tool::GetModuleID(GetID()));
			return;
			break;
		case 3:
			pStatusMsg->Load(_R(IDS_DRAGCOORD), Tool::GetModuleID(GetID()));
			return;
			break;

		default:
			pStatusMsg->Load(_R(IDS_MOULDDEFAULT), Tool::GetModuleID(GetID()));
			return;
			break;
	}
}






/********************************************************************************************

>	void MouldTool::RenderToolBlobs(Spread* pSpread,DocRect* pDocRect)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		pSpread  = ptr to a spread
				pDocRect = ptr to DocRect of spread to render in
	Returns:	-
	Purpose:	Handles the RenderToolBlobs method.
				Renders the tool's blobs into the current doc view.
	SeeAlso:	

********************************************************************************************/

void MouldTool::RenderToolBlobs(Spread* pSpread,DocRect* pDocRect)
{
	// Render into the selected doc view
	DocView* pDocView = DocView::GetSelected();
	if (pDocView != NULL)
	{
		// render you tool's blobs (if any) here
	}
}


/********************************************************************************************

>	static void MouldTool::DisplayStatusBarHelp(UINT32 StatusID)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		StatusID = ID of status help string
	Outputs:	-
	Returns:	-
	Purpose:	Displays the given status help string in the status bar
	SeeAlso:	-

********************************************************************************************/

void MouldTool::DisplayStatusBarHelp(UINT32 StatusID)
{
	String_256 StatusMsg("");
	StatusMsg.Load(StatusID);
	GetApplication()->UpdateStatusBarText(&StatusMsg);
}


/********************************************************************************************

>	void MouldTool::CreateEnvelope(EnvelopeType EnvType)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Purpose:	Calls the CREATENEWMOULD operation with a default rectangular envelope
				shape

********************************************************************************************/

void MouldTool::CreateEnvelope(EnvelopeType EnvType)
{
	// Make sure we have the operation ready
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CREATENEWMOULD);
	if (pOpDesc)
	{
		// Create a manifold to use
		Path* pDefShape = new Path;
		if (pDefShape!=NULL)
		{
			BOOL LockAspect = FALSE;

			if (pDefShape->Initialise(12,12))
			{
				BOOL ok = FALSE;

				switch (EnvType)
				{
					case ENV_DEFAULT:
						ok = EnvelopeShapes::Rectangular(pDefShape);
						break;

					case ENV_CIRCULAR:
						ok = EnvelopeShapes::Circular(pDefShape);
						LockAspect = TRUE;
						break;

					case ENV_ELLIPTICAL:
						ok = EnvelopeShapes::Circular(pDefShape);
						break;

					case ENV_CONCAVE:
						ok = EnvelopeShapes::Concave(pDefShape);
						break;

					case ENV_BANNER:
						ok = EnvelopeShapes::Banner(pDefShape);
						break;
				}

				if (ok)
				{
					// now build the correct mould parameter and invoke the operation				
					CreateMouldParam Param(pDefShape,MOULDSPACE_ENVELOPE,TRUE,LockAspect);
					pOpDesc->Invoke(&Param);
				}
			}
		}
		delete pDefShape;
	}
}

/********************************************************************************************

>	void MouldTool::CreateEnvelope2x2(EnvelopeType EnvType)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Purpose:	Calls the CREATENEWMOULD operation with a default rectangular envelope
				shape

********************************************************************************************/

void MouldTool::CreateEnvelope2x2(EnvelopeType EnvType)
{
	// Make sure we have the operation ready
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CREATENEWMOULD);
	if (pOpDesc)
	{
		// Create a manifold to use
		Path* pDefShape = new Path;
		if (pDefShape!=NULL)
		{
			BOOL LockAspect = FALSE;

			if (pDefShape->Initialise(12,12))
			{
				BOOL ok = FALSE;

				switch (EnvType)
				{
					case ENV_DEFAULT:
						ok = EnvelopeShapes::Rectangular2x2(pDefShape);
						break;

					default:
						break;
				}

				if (ok)
				{
					// now build the correct mould parameter and invoke the operation				
					CreateMouldParam Param(pDefShape,MOULDSPACE_ENVELOPE2X2,TRUE,LockAspect);
					pOpDesc->Invoke(&Param);
				}
			}
		}
		delete pDefShape;
	}
}


/********************************************************************************************

>	void MouldTool::CreatePerspective(PerspectiveType PerType)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Purpose:	Calls the CREATENEWMOULD operation with a default rectangular perspective
				shape

********************************************************************************************/

void MouldTool::CreatePerspective(PerspectiveType PerType)
{
	// Make sure we have the operation ready
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CREATENEWMOULD);
	if (pOpDesc)
	{
		// Create a manifold to use 
		Path* pDefShape = new Path;
		if (pDefShape!=NULL)
		{
			BOOL LockAspect = FALSE;

			if (pDefShape->Initialise(12,12))
			{
				BOOL ok = FALSE;

				switch (PerType)
				{
					case PER_DEFAULT:
						ok = PerspectiveShapes::Rectangular(pDefShape);
						break;
					case PER_LEFT:
						ok = PerspectiveShapes::LeftWall(pDefShape);
						break;
					case PER_RIGHT:
						ok = PerspectiveShapes::RightWall(pDefShape);
						break;
					case PER_FLOOR:
						ok = PerspectiveShapes::Floor(pDefShape);
						break;
					case PER_ROOF:
						ok = PerspectiveShapes::Ceiling(pDefShape);
						break;
				}

				if (ok)
				{
					// now build the correct mould parameter and invoke the operation				
					CreateMouldParam Param(pDefShape,MOULDSPACE_PERSPECTIVE,TRUE,LockAspect);
					pOpDesc->Invoke(&Param);
				}
			}
		}
		delete pDefShape;
	}
}



/********************************************************************************************

>	MouldInfoBarOp::MouldInfoBarOp()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/01/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the mould tool information bar operation.

********************************************************************************************/

MouldInfoBarOp::MouldInfoBarOp()
{
	DlgResID = _R(IDD_MOULDTOOLBAR);
	DetachState=FALSE;
}


/********************************************************************************************

>	void MouldInfoBarOp::SetTool(MouldTool* pTool)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		pTool, the pointer to the mould tool instance
	Outputs:	-
	Returns:	-
	Purpose:	Sets the mould info bar tool pointer private variable

********************************************************************************************/

void MouldInfoBarOp::SetTool(MouldTool* pTool)
{
	pMouldTool = pTool;
}


/********************************************************************************************

>	MsgResult MouldInfoBarOp::Message(Msg* Message) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Inputs:		Message = The message to handle
	Outputs:	-
	Returns:	-
	Purpose:	Mould info bar dialog message handler
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult MouldInfoBarOp::Message(Msg* Message) 
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		// Check if the message is a CANCEL
		switch (Msg->DlgMsg)
		{
			case DIM_CANCEL:
				Close(); // Close the dialog 
				break;

			case DIM_CREATE:
				// Initialise the infobar controls here
				// This is sent when you create the infobar in your tool startup code
				break;

			case DIM_LFT_BN_CLICKED:
			{
				if (pMouldTool !=NULL)
				{
					if (FALSE) {}
					else if (Msg->GadgetID == _R(IDC_BTN_DEFAULTENVELOPE))
					{
						pMouldTool->CreateEnvelope(ENV_DEFAULT);
						//pMouldTool->CreateEnvelope2x2(ENV_DEFAULT);
					}
					else if (Msg->GadgetID == _R(IDC_BTN_CIRCULARENVELOPE))
					{
						pMouldTool->CreateEnvelope(ENV_CIRCULAR);
					}
					else if (Msg->GadgetID == _R(IDC_BTN_ELLIPTICENVELOPE))
					{
						pMouldTool->CreateEnvelope(ENV_ELLIPTICAL);
					}
					else if (Msg->GadgetID == _R(IDC_BTN_CONCAVEENVELOPE))
					{
						pMouldTool->CreateEnvelope(ENV_CONCAVE);
					}
					else if (Msg->GadgetID == _R(IDC_BTN_BANNERENVELOPE))
					{
						pMouldTool->CreateEnvelope(ENV_BANNER);
					}
					else if (Msg->GadgetID == _R(IDC_BTN_DEFAULTPERSPECTIVE))
					{
						pMouldTool->CreatePerspective(PER_DEFAULT);
					}
					else if (Msg->GadgetID == _R(IDC_BTN_LEFTPERSPECTIVE))
					{
						pMouldTool->CreatePerspective(PER_LEFT);
					}
					else if (Msg->GadgetID == _R(IDC_BTN_RIGHTPERSPECTIVE))
					{
						pMouldTool->CreatePerspective(PER_RIGHT);
					}
					else if (Msg->GadgetID == _R(IDC_BTN_FLOORPERSPECTIVE))
					{
						pMouldTool->CreatePerspective(PER_FLOOR);
					}
					else if (Msg->GadgetID == _R(IDC_BTN_ROOFPERSPECTIVE))
					{
						pMouldTool->CreatePerspective(PER_ROOF);
					}
					else if (Msg->GadgetID ==_R(IDC_BTN_DETACHMOULD))
					{
						OpDetachMould* pOpDetachMould = new OpDetachMould;
						if (pOpDetachMould)
							pOpDetachMould->Do(NULL);
						SetDetachState(!DetachState);
					}
					
				}
			}
			break;

			case DIM_SELECTION_CHANGED:
			{
			}
			break;

			default:
				break;
		}
	}
	// Pass the message on to the immediate mould class
	return (DialogBarOp::Message(Message));
}    



/********************************************************************************************

>	virtual void MouldInfoBarOp::UpdateState()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/11/94
	Purpose:	Overrides the empty UpdateState function provided by InformationBarOp
				making a call to the function in DialogBarOp.

*********************************************************************************************/

void MouldInfoBarOp::UpdateState()
{
	// check that our tool is enabled and is current
	if (!pMouldTool) return;
	if (!pMouldTool->IsCurrent()) return;

	DialogBarOp::UpdateState();
	BOOL Selection = CheckSelection();
	UpdateDefEnvButton(Selection);
	UpdateDefPerButton(Selection);
	UpdateDetachButton(Selection);
}



/********************************************************************************************

>	void MouldInfoBarOp::UpdateDefEnvButton(BOOL Selection)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/11/94
	Inputs:		-
	Purpose:	Sets the grey and select status of the default envelope button
				on the infobar.

********************************************************************************************/

void MouldInfoBarOp::UpdateDefEnvButton(BOOL Selection)
{
	// Check for the existance of an info bar first
	if (WindowID)
	{
		if (!CanMouldSelection())
		{
			// if we can't mould the selection, then disable all gadgets
			Selection = FALSE;
		}
		
		EnableGadget(_R(IDC_BTN_DEFAULTENVELOPE), Selection);
		EnableGadget(_R(IDC_BTN_CIRCULARENVELOPE), Selection);
		EnableGadget(_R(IDC_BTN_ELLIPTICENVELOPE), Selection);
		EnableGadget(_R(IDC_BTN_CONCAVEENVELOPE), Selection);
		EnableGadget(_R(IDC_BTN_BANNERENVELOPE), Selection);

		// if we have NOT enabled any of the above, then we should NOT allow you to paste a mould either!
		// This is a BODGE - that works!  There is basically something wrong with the AllowOp mechanism,
		// and I do not have enough time to fix it ....

		if (!Selection)
		{
			EnableGadget(_R(IDC_BTN_PASTEENVELOPE), Selection);
		}
		else
		{
			String_256 Desc; 
			OpDescriptor* OpDesc = OpDescriptor::FindOpDescriptor (CC_RUNTIME_CLASS (OpPasteEnvelope));

			if (OpDesc != NULL)
			{
				OpState NewState = OpDesc->GetOpsState(&Desc);

				if (NewState.Greyed == FALSE)
				{
					EnableGadget(_R(IDC_BTN_PASTEENVELOPE), Selection);
				}
			}
		}
	}
}

/********************************************************************************************

>	BOOL MouldInfoBarOp::CanMouldSelection()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/99
	Inputs:		-
	Returns:	TRUE for the selection being able to be moulded, FALSE otherwise
	Purpose:	Tests the selection to see if all nodes can be moulded
				Mainly tests for NeedsParent() nodes - if any exist in the selection, then
				we can't mould !

********************************************************************************************/
BOOL MouldInfoBarOp::CanMouldSelection()
{
	Range * pRng = GetApplication()->FindSelection();

	Node * pParent = NULL;
	Node * pNode = pRng->FindFirst(FALSE);
	Node * pChild = NULL;

	while (pNode)
	{
		pParent = pNode->FindParent();

		if (pParent)
		{
			if (pNode->NeedsParent(pParent))
			{
				return FALSE;
			}
		}

		// check for any needs parent nodes in the node's child list
		pChild = pNode->FindFirstDepthFirst();

		while (pChild)
		{
			if (pChild->NeedsParent(NULL))
			{
				return FALSE;
			}

			pChild = pChild->FindNextDepthFirst(pNode);
		}

		pNode = pRng->FindNext(pNode, FALSE);
	}

	return TRUE;
}

/********************************************************************************************

>	void MouldInfoBarOp::UpdateDefPerButton()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/11/94
	Inputs:		-
	Purpose:	Sets the grey and select status of the default perspective button 
				on the infobar.

********************************************************************************************/

void MouldInfoBarOp::UpdateDefPerButton(BOOL Selection)
{
	// Check for the existance of an info bar first
	if (WindowID)
	{
		if (!CanMouldSelection())
		{
			// if we can't mould the selection, then disable all gadgets
			Selection = FALSE;
		}

		EnableGadget(_R(IDC_BTN_DEFAULTPERSPECTIVE), Selection);
		EnableGadget(_R(IDC_BTN_LEFTPERSPECTIVE), Selection);
		EnableGadget(_R(IDC_BTN_RIGHTPERSPECTIVE), Selection);
		EnableGadget(_R(IDC_BTN_FLOORPERSPECTIVE), Selection);
		EnableGadget(_R(IDC_BTN_ROOFPERSPECTIVE), Selection);

		// if we have NOT enabled any of the above, then we should NOT allow you to paste a mould either!
		// This is a BODGE - that works!  There is basically something wrong with the AllowOp mechanism,
		// and I do not have enough time to fix it ....

		if (!Selection)
		{
			EnableGadget(_R(IDC_BTN_PASTEPERSPECTIVE), Selection);
		}
		else
		{
			String_256 Desc; 
			OpDescriptor* OpDesc = OpDescriptor::FindOpDescriptor (CC_RUNTIME_CLASS (OpPastePerspective));

			if (OpDesc != NULL)
			{
				OpState NewState = OpDesc->GetOpsState(&Desc);

				if (NewState.Greyed == FALSE)
				{
					EnableGadget(_R(IDC_BTN_PASTEPERSPECTIVE), Selection);
				}
			}
		}
	}
}


/********************************************************************************************

>	void MouldInfoBarOp::UpdateDetachButton()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/03/95
	Inputs:		-
	Purpose:	Sets the grey and select status of the detach button on the infobar.

********************************************************************************************/

void MouldInfoBarOp::UpdateDetachButton(BOOL Selection)
{
	if (WindowID!=NULL)
	{
		if (!CanMouldSelection())
		{
			// if we can't mould the selection, then disable all gadgets
			Selection = FALSE;
		}

		EnableGadget(_R(IDC_BTN_DETACHMOULD), Selection);
		// if the button is enabled then set its state
		if (Selection)
		{		
			NodeMould* pLastMould;
			INT32 nmoulds = SelInfo(&pLastMould);

			// Set the detach button dependent on how many moulds are selected.
			BOOL Enabled = (nmoulds>0);
			BOOL PressedIn = ((nmoulds==1) && pLastMould->IsDetached());

			EnableGadget(_R(IDC_BTN_DETACHMOULD), Enabled);
			SetDetachState(PressedIn);
		}
	}
}

/********************************************************************************************

>	void MouldInfoBarOp::SetDetachState()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/03/95
	Inputs:		-
	Purpose:	Sets the detach button state. As we have no sensible button that clicks
				down then

********************************************************************************************/

void MouldInfoBarOp::SetDetachState(BOOL state)
{
	// Is it down or up?
	SetLongGadgetValue(_R(IDC_BTN_DETACHMOULD), state);
	DetachState = state;
}



/********************************************************************************************

>	INT32 MouldInfoBarOp::SelInfo(NodeMould** pMould)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/03/95
	Inputs:		pMould = a pointer to a mould object pointer to be used as a return parameter
	Outputs:	pMould = a pointer to the last selected mould object
	Returns:	The number of mould objects selected
	Purpose:	Find out how many moulds are selected and return a pointer to the last
				selected mould.

********************************************************************************************/

INT32 MouldInfoBarOp::SelInfo(NodeMould** pMould)
{
	SelRange* Selection = GetApplication()->FindSelection();
	Node* pNode = Selection->FindFirst();
	NodeMould* pLastMould=NULL;

	INT32 num = 0;
	while (pNode!=NULL)
	{
		if (IS_A(pNode, NodeMould) && pNode->IsSelected())
		{
			num++;
			pLastMould = (NodeMould*)pNode;
		}
		pNode = Selection->FindNext(pNode);
	}

	(*pMould)=pLastMould;
	return num;
}	 


/********************************************************************************************

>	void MouldInfoBarOp::CheckSelection()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/11/94
	Inputs:		-
	Returns:	TRUE if there are selected objects in the current document
				FALSE if nothing is selected
	Purpose:	Checks for selected objects in the current document and returns whether
				there are any or not

********************************************************************************************/

BOOL MouldInfoBarOp::CheckSelection()
{
	BOOL Enabled = FALSE;

/*
	if (Document::GetCurrent() != NULL)
	{
		SelRange* Selected = GetApplication()->FindSelection();
		Node* pNode = Selected->FindFirst();
		Enabled = (pNode!=NULL);
	}
*/

	// Get the selection and the number of selected objects
	SelRange* Selected = GetApplication()->FindSelection();
	INT32 Count = 0;
	if (Document::GetSelected() != NULL)
		Count = Selected->Count();

	// Are there any selected objects?
	if (Count > 0)
	{
		ObjChangeFlags cFlags;

		// If only one node selected, then it will just be replaced in the tree with another NodeRenderableInk node
		// Otherwise, the selected node could be moved, and if the node is a child of another node, effectively
		// delete a child node of a parent.
		/*
		if (Count == 1)
			cFlags.ReplaceNode = TRUE;
		else
			cFlags.DeleteNode  = TRUE;
		*/
		// **** Above code commented out due to imcomplete implementation
		//
		// For now, just pretend that the op is as severe as deleting to get the mould tool buttons to grey out
		// when the mould is selected inside the blend. (Haven't got time to do this properly at the moment)
		cFlags.MultiReplaceNode  = TRUE;

		ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);

		Node* pNode = Selected->FindFirst();
	
		Enabled = TRUE;
		while (pNode != NULL && Enabled)
		{
			Enabled = pNode->AllowOp(&ObjChange,FALSE);
			pNode = Selected->FindNext(pNode);
		}
	}

	return Enabled;
}



/********************************************************************************************

>	OpCreateNewMould::OpCreateNewMould() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/94
	Purpose:	OpCreateNewMould() constructor
	SeeAlso:	-

********************************************************************************************/

OpCreateNewMould::OpCreateNewMould()
{
	m_space = MOULDSPACE_UNDEFINED;
}




/********************************************************************************************

>	BOOL OpCreateNewMould::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpDeletePoints initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpCreateNewMould::Init()
{
	return (RegisterOpDescriptor(0,									// tool ID
 								_R(IDS_CREATENEWMOULD),					// string resource ID
								CC_RUNTIME_CLASS(OpCreateNewMould),	// runtime class for Op
 								OPTOKEN_CREATENEWMOULD,				// Ptr to token string
 								OpCreateNewMould::GetState,			// GetState function
 								0,									// help ID
 								_R(IDBBL_CREATENEWMOULD),				// bubble help ID
								0									// resource ID
								));

}               

  
/********************************************************************************************

>	void OpCreateNewMould::GetOpName(String_256* OpName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/94
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description
				appropriate to the type of attribute that the operation applies.
					
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpCreateNewMould::GetOpName(String_256* OpName)
{
	switch (m_space)
	{
		case MOULDSPACE_ENVELOPE:
			*OpName = String_256(_R(IDS_UNDO_ENVELOPE));
			break;
		case MOULDSPACE_ENVELOPE2X2:
			*OpName = String_256(_R(IDS_UNDO_ENVELOPE));
			break;
		case MOULDSPACE_PERSPECTIVE:
			*OpName = String_256(_R(IDS_UNDO_PERSPECTIVE));
			break;
		default:
			*OpName = String_256(_R(IDS_UNDO_CREATENEWMOULD));
			break;
	}
}


/********************************************************************************************

>	OpState	OpCreateNewMould::GetState(String_256*, OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpCreateNewMould
	Purpose:	For finding the OpCreateNewMould state. 

********************************************************************************************/

OpState	OpCreateNewMould::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
   	OpSt.Greyed = TRUE;

	// only ungrey if we have a selection
	SelRange* pSelRange = GetApplication()->FindSelection();
	if (pSelRange && pSelRange->FindFirst() != NULL)
		OpSt.Greyed=FALSE;

	return OpSt;   
}


/********************************************************************************************

> void OpCreateNewMould::DoWithParam(OpDescriptor* pOpDescr, OpParam* pOpParam)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	11/12/94
	Inputs:		pOpDescr = pointer to an OpDescriptor (unused)
				pOpParam = pointer to an OpParam
	Outputs:	-
	Returns:	-
	Purpose:	This operation will create a new mould object, and thus start off quite
				a complicated chain of events.
				First off, a mouldgroup node is created and all objects are moved into it as
				children, along with any inherited attributes.
				Secondly a mould object is created and initialise with the shape passed to
				this do function.
				The mouldgroup node is now added to the mould node as a first child.
				A moulder object is then created and added as the last child of the mould
				node. It is not added to the next object of the mouldgroup, as there may be
				more than one moulder object.
				The moulder object is then asked to initialise itself. In doing so it will
				scan the mould group children and ask them to make shapes of themselves.
				These shapes are stored in a list within the moulder class, (not in the tree). 
				Having constructed the moulder, a mould operation is triggered, whence all current
				moulder list objects are mangled by the mould shape class functions (held
				within the mould parent class). These mangled objects are added as children
				of the moulder object.
				Having done all this without running out of memory, the mouldgroup object is
				hidden.

	Errors:		failandexecute will be called if the operation fails in some way, most
				likely when no memory is available. 

********************************************************************************************/

void OpCreateNewMould::DoWithParam(OpDescriptor* pOpDescr, OpParam* pParam)
{
	ERROR3IF(pParam == NULL, "OpCreateNewMould::DoWithParam - NULL Param passed in");

	BeginSlowJob();

	// Initialise ourselves from the supplied parameters
	CreateMouldParam* CreateMould = (CreateMouldParam*)pParam;

	if (!CreateNewMould(CreateMould->ParamShape, 
						CreateMould->ParamSpace,
						CreateMould->FitSelection,
						CreateMould->LockAspect))
		FailAndExecute();

	End();
}




/********************************************************************************************

>	OpRemoveMould::OpRemoveMould() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Purpose:	Constructor for remove mould shape operation
	SeeAlso:	-

********************************************************************************************/

OpRemoveMould::OpRemoveMould()
{
	// Dummy constructor
}
  


/********************************************************************************************

>	BOOL OpRemoveMould::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	Initialise the remove mould operation so that it can be accessed from the
				interface.

	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpRemoveMould::Init()
{
	return (RegisterOpDescriptor(0,									// tool ID
 								_R(IDS_REMOVEMOULD),					// string resource ID
								CC_RUNTIME_CLASS(OpRemoveMould),	// runtime class for Op
 								OPTOKEN_REMOVEMOULD,				// Ptr to token string
 								OpRemoveMould::GetState,			// GetState function
 								0,									// help ID
 								_R(IDBBL_REMOVEMOULD),				// bubble help ID
 								_R(IDC_REMOVEMOULD),				// resource ID
								_R(IDC_REMOVEMOULD)					// control ID
								));
}               


/********************************************************************************************

>	OpState	OpRemoveMould::GetState(String_256*, OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpRemoveMould
	Purpose:	For finding the OpRemoveMould state. 

********************************************************************************************/

OpState	OpRemoveMould::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
	String_256 DisableReason; 
   	OpSt.Greyed = TRUE;
	
//	if (!IsAMouldSelected(NULL))
//		OpSt.Greyed=TRUE;

	ObjChangeFlags cFlags;
	cFlags.MultiReplaceNode = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);
	
	SelRange* pSelRange = GetApplication()->FindSelection();
	Node* pNode = pSelRange->FindFirst();

	while (pNode != NULL && OpSt.Greyed)
	{
		if (IS_A(pNode,NodeMould))
			OpSt.Greyed = !(pNode->AllowOp(&ObjChange,FALSE));

		pNode = pSelRange->FindNext(pNode);
	}

	return (OpSt);   
}



/********************************************************************************************

>	void OpRemoveMould::GetOpName(String_256* OpName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description
				appropriate to the type of attribute that the operation applies.

********************************************************************************************/

void OpRemoveMould::GetOpName(String_256* OpName)
{
	*OpName = String_256(_R(IDS_UNDO_REMOVEMOULD));
}






/********************************************************************************************

> void OpRemoveMould::Do(OpDescriptor* opDesc)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	18/01/95
	Inputs:		pointer to an OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		failandexecute will be called if the operation fails in some way, most
				likely when no memory is available. 

********************************************************************************************/

void OpRemoveMould::Do(OpDescriptor*)
{
	BeginSlowJob();

	// remember the selection before the operation
	if (DoStartSelOp(FALSE,FALSE))
	{
		// Obtain the current selections 
		SelRange* Selected = GetApplication()->FindSelection();
		if (Selected)
		{
			// compile a list of the selected objects
			DocRect Bounds = Selected->GetBoundingRect();
			List* pNodeList = Selected->MakeListOfNodes();

			// remove all mould objects in the list
			BOOL success = DoRemoveAllMoulds(pNodeList);

			// delete all elements in our compiled list
			while (!pNodeList->IsEmpty())
				delete (NodeListItem*)(pNodeList->RemoveHead());
			delete pNodeList;

			// update the selection
			if (success)
			{
				// Update all the changed parents.
				ObjChangeFlags cFlags;
				cFlags.MultiReplaceNode = TRUE;
				ObjChangeParam ObjChange(OBJCHANGE_FINISHED,cFlags,NULL,this);
				if (!UpdateChangedNodes(&ObjChange))
					FailAndExecute();					
			}
			else
				FailAndExecute();
		}
		else
			FailAndExecute();
	}
	else
		FailAndExecute();

	End();
}






/********************************************************************************************

>	OpCopyMouldShape::OpCopyMouldShape() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Purpose:	Constructor for copy mould shape operation
	SeeAlso:	-

********************************************************************************************/

OpCopyMouldShape::OpCopyMouldShape()
{
	// Dummy constructor
}
  


/********************************************************************************************

>	BOOL OpCopyMouldShape::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	Initialise the copy mould operation so that it can be accessed from the
				interface.

	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpCopyMouldShape::Init()
{
	return (RegisterOpDescriptor(0,									// tool ID
 								_R(IDS_COPYMOULD),						// string resource ID
								CC_RUNTIME_CLASS(OpCopyMouldShape),	// runtime class for Op
 								OPTOKEN_COPYMOULD,					// Ptr to token string
 								OpCopyMouldShape::GetState,			// GetState function
 								0,									// help ID
 								_R(IDBBL_COPYMOULD),					// bubble help ID
 								_R(IDD_MOULDTOOLBAR),					// resource ID
								_R(IDC_BTN_COPYMOULD),					// control ID
								SYSTEMBAR_EDIT,						// Group bar ID
								TRUE, 								// Receive messages
								FALSE,								// Smart
								FALSE,								// Clean
								0,									// One open Instance ID
								(GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION)
 								)); 

}               


/********************************************************************************************

>	OpState	OpCopyMouldShape::GetState(String_256*, OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpCopyMouldShape
	Purpose:	For finding the OpCopyMouldShape state. 

********************************************************************************************/

OpState	OpCopyMouldShape::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
	String_256 DisableReason; 
   	OpSt.Greyed = FALSE;

	if (!IsAMouldSelected(NULL))
		OpSt.Greyed=TRUE;

	return (OpSt);   
}



/********************************************************************************************

>	void OpCopyMouldShape::GetOpName(String_256* OpName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description
				appropriate to the type of attribute that the operation applies.

********************************************************************************************/

void OpCopyMouldShape::GetOpName(String_256* OpName)
{
	*OpName = String_256(_R(IDS_UNDO_COPYMOULD));
}






/********************************************************************************************

> void OpCopyMouldShape::Do(OpDescriptor* opDesc)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	18/01/95
	Inputs:		pointer to an OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Copies the mould node path to the clipboard so it can be used as a template
				object for further moulds.
	Errors:		failandexecute will be called if the operation fails in some way, most
				likely when no memory is available. 

********************************************************************************************/

void OpCopyMouldShape::Do(OpDescriptor*)
{
	NodeMould* pNodeMould;
	NodeMouldPath* pMouldPath;

	if (!DoStartSelOp(TRUE,TRUE))
		goto Failed;

	if (IsAMouldSelected(&pNodeMould))
	{
		// find the actual shape we need to copy.
		pMouldPath = pNodeMould->GetPathShape();
		if (pMouldPath==NULL)
			goto Failed;

		// RangeControl Cntrl = Selected + Unselected + don't cross layers  
		Range CopyRange(pMouldPath,pMouldPath,RangeControl(TRUE,TRUE,FALSE));
		if (!DoCopyNodesToClipboard(CopyRange))
			goto Failed;
	}
	End();
	return;

Failed:
	FailAndExecute();
	End();
	return;
}




/********************************************************************************************

>	OpPasteMouldShape::OpPasteMouldShape() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Purpose:	Constructor for paste mould shape operation
	SeeAlso:	-

********************************************************************************************/

OpPasteMouldShape::OpPasteMouldShape()
{
	// Dummy constructor
}
  
/********************************************************************************************

>	BOOL OpPasteMouldShape::FindPasteObject(Node** pReturnNode)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/02/95
	Inputs:		pReturnNode, a pointer to a pointer, which can be null if no return
				node is required.
	Outputs:	pReturnNode holds a pointer to the node to be pasted, if a suitable node
				has been found and the pointer pointer is none null on entry 
	Returns:	TRUE if a suitable paste object has been found
				FALSE if none
	Purpose:	This function scans for objects on the clipboard. It tries to find the
				first object which 
					Is a NodeRenderableInk
					Can become a nodepath
				If there is such an object the function will return true, and set the
				return pointer to point at the object if the return pointer exists ie is
				a none null receptor.

********************************************************************************************/

BOOL OpPasteMouldShape::FindPasteObject(Node** pReturnNode)
{
	BOOL found=FALSE;

	// find the first object
	Node* pNode = InternalClipboard::GetInsertionLayer()->FindFirstChild();
	while (pNode && !found)
	{
		// ok lets check what we've got here
		BecomeA TestBecomeAPath(BECOMEA_TEST, CC_RUNTIME_CLASS(NodePath));
		if ((pNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk))) &&
			(pNode->CanBecomeA(&TestBecomeAPath))
		   )
			found=TRUE;
		else
			pNode=pNode->FindNext();
	}

	if (found && pReturnNode)
		*pReturnNode=pNode;

	return found;
}



/********************************************************************************************

> void OpPasteMouldShape::Do(MouldSpace mSpace)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	18/01/95
	Inputs:		mSpace = the mould space to paste as
	Outputs:	-
	Returns:	-
	Purpose:	Take the contents of the clipboard and try to use whatever shape exists
				there as a mould shape. The mould space type is passed to this function	
				as a parameter. The function will find the first renderable object on the
				clipboard, try to make shapes of it and use the first shape returned as
				the mould space object. 
	Errors:		failandexecute will be called if the operation fails in some way, most
				likely when no memory is available. 

********************************************************************************************/

void OpPasteMouldShape::Do(MouldSpace mSpace)
{
	BeginSlowJob();

	Node* pPasteNode;
	BOOL Constrain=KeyPress::IsConstrainPressed();

	// Obtain the current selections 
	SelRange* Selected = GetApplication()->FindSelection();
	if (!Selected)
	{
		FailAndExecute(); End(); return;
	}

	// is there a suitable object on the clipboard?
	if (!FindPasteObject(&pPasteNode))
	{
		FailAndExecute(); End(); return;
	}

	// HERE

	// If so lets force the object to become a nodepath
	// Do this none undoably because we are doing things on the clipboard.
	// If we DoBecomeA passback on a perspectivised filled object, it hides
	// the original?
	MouldBecomeA ParamBecomeA(	BECOMEA_PASSBACK, 
								CC_RUNTIME_CLASS(NodePath),
								NULL);

	// Get pNode to generate its paths for us
	BOOL ok = pPasteNode->DoBecomeA(&ParamBecomeA);
	// whip the resulting mould path out of the do becomea param class
	NodePath* pNodePath = ParamBecomeA.GetMouldPath();
	if ((!ok) || pNodePath==NULL)
	{
		FailAndExecute(); End(); return;
	}

	// remember the selection before the operation
	if (!DoStartSelOp(TRUE,TRUE))
	{
		FailAndExecute(); End(); return;
	}

	ObjChangeFlags cFlags;
	cFlags.DeleteNode  = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

	if (!Selected->AllowOp(&ObjChange))
	{
		ERROR3("AllowOp() returned FALSE, i.e. op should have been greyed out");
		FailAndExecute(); End(); return;
	}

	// Trash the selection bounds
	Range Rnge = (*Selected);
	if (!DoInvalidateNodesRegions(Rnge,TRUE,TRUE))
	{
		FailAndExecute(); End(); return;
	}

	// Calculate the destination paste position
	DocRect Bounds 	= Selected->GetBoundingRect();
	DocRect sBounds = pNodePath->GetBoundingRect();

	Node* pNode = Selected->FindFirst();
	if (pNode==NULL)
	{
		FailAndExecute(); End(); return;
	}
	
	if (!Constrain)
	{
		// If we're not constraining, then we might have to force constrain
		// on to ensure the object pastes within the bounds of the spread.
		// We'll try to expand the pasteboard, and if that fails, we'll
		// turn constrain on
		
		// find the spread of the destination selected node (fix to Jason bug)
		Spread *pSpread = pNode->FindParentSpread();
		if (pSpread == NULL || !pSpread->ExpandPasteboardToInclude(sBounds))
			Constrain=TRUE;
	}

	if (Constrain)
	{
		// if 'control' constrain is pressed then paste the mould
		// shape centred over the selection otherwise leave it where
		// it is.
		INT32 tx = (Bounds.lo.x+Bounds.hi.x)/2 - (sBounds.lo.x+sBounds.hi.x)/2;
		INT32 ty = (Bounds.lo.y+Bounds.hi.y)/2 - (sBounds.lo.y+sBounds.hi.y)/2;
		Trans2DMatrix trans(tx,ty);
		pNodePath->Transform(trans);
	}

	// Grab the ink path from its node
	Path* pShape = &(pNodePath->InkPath);

	// using this list create or replace the mould bits
	ok = DoCreateOrReplaceMould(Selected, pShape, mSpace, TRUE, FALSE);
	if (!ok)
		FailAndExecute();

	// Update all the changed nodes, i.e. tell all the parents of the children that have been effected
	ObjChange.Define (OBJCHANGE_FINISHED,cFlags,NULL,this);
	UpdateChangedNodes(&ObjChange);

	End();
}


/********************************************************************************************

>	MouldBecomeA::~MouldBecomeA()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/09/96
	Purpose:	Destructor for the MouldBecomeA parameter
	SeeAlso:	-

********************************************************************************************/

MouldBecomeA::~MouldBecomeA()
{
	Destroy();
}


/***********************************************************************************************

>	virtual BOOL MouldBecomeA::PassBack(NodeRenderableInk* pNewNode, 
										NodeRenderableInk* pCreatedByNode,
										CCAttrMap* pAttrMap)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/02/95
	Inputs:		pNewNode   	   = ptr to Node that been generated by a node via its 
								 DoBecomeA() method
				pCreatedByNode = ptr to the node that created pNewNode
	Outputs:	-
	Returns:	TRUE if Successful, FALSE otherwise

	Purpose:	From the call     ProducerObj->DoBecomeA( MouldBecomeA(ObjectType) )
				this function being a member of MouldBecomeA will be passed objects of ObjectType
				created by ProducerObj.	We store the first object passed to us and ignore the
				rest as we really can only use one thanks very much. Most simple producers will
				probably return single objects anyway.
	
***********************************************************************************************/

BOOL MouldBecomeA::PassBack(NodeRenderableInk* pNewNode,
							NodeRenderableInk* pCreatedByNode, 
							CCAttrMap* pAttrMap)
{
	if ( (MouldBecomeA::pNodePath==NULL) && (pNewNode->IsKindOf(CC_RUNTIME_CLASS(NodePath))) )
		MouldBecomeA::pNodePath=(NodePath*)pNewNode;
	else
	{
		// remember on a pass back we own this object so we 
		// really need to delete it if we're not going to use it!
		pNewNode->CascadeDelete(); 
		delete pNewNode;
		pNewNode = NULL;
	}	

	return TRUE;
}

/********************************************************************************************

>	void MouldBecomeA::Destroy()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Purpose:	Destroy the contents of the mould become a object
	SeeAlso:	-

********************************************************************************************/

void MouldBecomeA::Destroy()
{
	// remember on a pass back we own this object so we 
	// really need to delete it if we're not going to use it!
	if (pNodePath)
	{
		pNodePath->CascadeDelete(); 
		delete pNodePath;
		pNodePath = NULL;
	}
}


/********************************************************************************************

>	OpPasteEnvelope::OpPasteEnvelope() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Purpose:	Constructor for paste envelope operation
	SeeAlso:	-

********************************************************************************************/

OpPasteEnvelope::OpPasteEnvelope()
{
	// Dummy constructor
}


/********************************************************************************************

>	BOOL OpPasteEnvelope::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	Initialise the paste mould operation so that it can be accessed from the
				interface.

	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpPasteEnvelope::Init()
{
	return (RegisterOpDescriptor(0,									// tool ID
 								_R(IDS_PASTEENVELOPE),					// string resource ID
								CC_RUNTIME_CLASS(OpPasteEnvelope),	// runtime class for Op
 								OPTOKEN_PASTEENVELOPE,				// Ptr to token string
 								OpPasteEnvelope::GetState,			// GetState function
 								0,									// help ID
 								_R(IDBBL_PASTEENVELOPE),				// bubble help ID
 								_R(IDD_MOULDTOOLBAR),					// resource ID
								_R(IDC_BTN_PASTEENVELOPE),				// control ID
								SYSTEMBAR_EDIT,						// Group bar ID
								TRUE, 								// Receive messages
								FALSE,								// Smart
								FALSE,								// Clean
								0,									// One open Instance ID
								(GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION)
 								)); 
}               


/********************************************************************************************

>	OpState	OpPasteEnvelope::GetState(String_256*, OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpPasteMouldShape
	Purpose:	For finding the OpPasteMouldShape state. 

********************************************************************************************/

OpState	OpPasteEnvelope::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
//	String_256 DisableReason; 
//	OpSt.Greyed=(!FindPasteObject(NULL));

	OpSt.Greyed = TRUE;

	if (FindPasteObject(NULL))
	{
		// Get the selection and the number of selected objects
		SelRange* Selected = GetApplication()->FindSelection();
		INT32 Count = Selected->Count();

		// Are there any selected objects?
		if (Count > 0)
		{
			ObjChangeFlags cFlags;
			cFlags.DeleteNode  = TRUE;
			OpPasteEnvelope* op = new OpPasteEnvelope ();		// need an op to do correct processing
			ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,op);

			Node* pNode = Selected->FindFirst();
			OpSt.Greyed = FALSE;
			while (pNode != NULL && !OpSt.Greyed)
			{
				OpSt.Greyed = !pNode->AllowOp(&ObjChange,FALSE);
				pNode = Selected->FindNext(pNode);
			}
			delete (op);
		}
	}

	return(OpSt);   
}




/********************************************************************************************

>	void OpPasteEnvelope::GetOpName(String_256* OpName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description
				appropriate to the type of attribute that the operation applies.

********************************************************************************************/

void OpPasteEnvelope::GetOpName(String_256* OpName)
{
	*OpName = String_256(_R(IDS_UNDO_PASTEENVELOPE));
}




/********************************************************************************************

> void OpPasteEnvelope::Do(OpDescriptor* opDesc)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	18/01/95
	Inputs:		pointer to an OpDescriptor (unused)
				pointer to a pastemould parameter block
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		failandexecute will be called if the operation fails in some way, most
				likely when no memory is available. 

********************************************************************************************/

void OpPasteEnvelope::Do(OpDescriptor* pOpDescr)
{
	// just call the parent class
	OpPasteMouldShape::Do(MOULDSPACE_ENVELOPE);
}




/********************************************************************************************

>	OpPastePerspective::OpPastePerspective() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Purpose:	Constructor for paste envelope operation
	SeeAlso:	-

********************************************************************************************/

OpPastePerspective::OpPastePerspective()
{
	// Dummy constructor
}


/********************************************************************************************

>	BOOL OpPastePerspective::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	Initialise the paste mould operation so that it can be accessed from the
				interface.

	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpPastePerspective::Init()
{
	return (RegisterOpDescriptor(0,									// tool ID
 								_R(IDS_PASTEPERSPECTIVE),				// string resource ID
								CC_RUNTIME_CLASS(OpPastePerspective),	// runtime class for Op
 								OPTOKEN_PASTEPERSPECTIVE,			// Ptr to token string
 								OpPastePerspective::GetState,		// GetState function
 								0,									// help ID
 								_R(IDBBL_PASTEPERSPECTIVE),				// bubble help ID
 								_R(IDD_MOULDTOOLBAR),					// resource ID
								_R(IDC_BTN_PASTEPERSPECTIVE),				// control ID
								SYSTEMBAR_EDIT,						// Group bar ID
								TRUE, 								// Receive messages
								FALSE,								// Smart
								FALSE,								// Clean
								0,									// One open Instance ID
								(GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION)
 								)); 
}               


/********************************************************************************************

>	OpState	OpPastePerspective::GetState(String_256*, OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpPasteMouldShape
	Purpose:	For finding the OpPasteMouldShape state. 

********************************************************************************************/

OpState	OpPastePerspective::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
//	String_256 DisableReason; 
//	OpSt.Greyed=(!FindPasteObject(NULL));

	OpSt.Greyed = TRUE;

	if (FindPasteObject(NULL))
	{
		// Get the selection and the number of selected objects
		SelRange* Selected = GetApplication()->FindSelection();
		INT32 Count = Selected->Count();

		// Are there any selected objects?
		if (Count > 0)
		{
			ObjChangeFlags cFlags;
			cFlags.DeleteNode  = TRUE;
			OpPasteEnvelope* op = new OpPasteEnvelope ();		// need an op to do correct processing
			ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,op);

			Node* pNode = Selected->FindFirst();
			OpSt.Greyed = FALSE;
			while (pNode != NULL && !OpSt.Greyed)
			{
				OpSt.Greyed = !pNode->AllowOp(&ObjChange,FALSE);
				pNode = Selected->FindNext(pNode);
			}
			delete (op);
		}
	}

	return(OpSt);   
}




/********************************************************************************************

>	void OpPastePerspective::GetOpName(String_256* OpName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description
				appropriate to the type of attribute that the operation applies.

********************************************************************************************/

void OpPastePerspective::GetOpName(String_256* OpName)
{
	*OpName = String_256(_R(IDS_UNDO_PASTEPERSPECTIVE));
}




/********************************************************************************************

> void OpPastePerspective::Do(OpDescriptor* opDesc)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	18/01/95
	Inputs:		pointer to an OpDescriptor (unused)
				pointer to a pastemould parameter block
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		failandexecute will be called if the operation fails in some way, most
				likely when no memory is available. 

********************************************************************************************/

void OpPastePerspective::Do(OpDescriptor* pOpDescr)
{
	// just call the parent class
	OpPasteMouldShape::Do(MOULDSPACE_PERSPECTIVE);
}





/********************************************************************************************

>	OpToggleMouldGrid::OpToggleMouldGrid() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Purpose:	Constructor for toggle mould grid operation
	SeeAlso:	-

********************************************************************************************/

OpToggleMouldGrid::OpToggleMouldGrid()
{
	// Dummy constructor
}
  


/********************************************************************************************

>	BOOL OpToggleMouldGrid::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	Initialise the toggle mould grid operation so that it can be accessed from the
				interface.

	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpToggleMouldGrid::Init()
{
	return (RegisterOpDescriptor(0,										// tool ID
 								_R(IDS_TOGGLEMOULDGRID),					// string resource ID
								CC_RUNTIME_CLASS(OpToggleMouldGrid),	// runtime class for Op
 								OPTOKEN_TOGGLEMOULDGRID,				// Ptr to token string
 								OpToggleMouldGrid::GetState,			// GetState function
 								0,										// help ID
 								_R(IDBBL_TOGGLEMOULDGRID),					// bubble help ID
 								_R(IDD_MOULDTOOLBAR),						// resource ID
								_R(IDC_BTN_TOGGLEMOULDGRID),				// control ID
								SYSTEMBAR_EDIT,							// Group bar ID
								TRUE, 									// Receive messages
								FALSE,									// Smart
								FALSE,									// Clean
								0,										// One open Instance ID
								(GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION)
 								)); 
}               


/********************************************************************************************

>	OpState	OpToggleMouldGrid::GetState(String_256*, OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpToggleMouldGrid
	Purpose:	For finding the OpToggleMouldGrid state. 

********************************************************************************************/

OpState	OpToggleMouldGrid::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
	String_256 DisableReason; 
   	OpSt.Greyed = FALSE;

/*
	if (Document::GetCurrent() != NULL)
	{
		SelRange* Selected = GetApplication()->FindSelection();
		if (Selected)
			if (!Selected->FindObject(CC_RUNTIME_CLASS(NodeMould), NULL))
				OpSt.Greyed = TRUE;
	}
*/

	BOOL Sel = FALSE;
	if (Document::GetCurrent() != NULL)
	{
		SelRange* Selected = GetApplication()->FindSelection();
		if (Selected)
		{
			Node* pNode = Selected->FindFirst();
			while ((pNode!=NULL) && (!Sel))
			{
				if (pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeMould))
					Sel=TRUE;

				pNode = Selected->FindNext(pNode);
			}
		}
	}

	if (!Sel)
		OpSt.Greyed = TRUE;

	return (OpSt);   
}


/********************************************************************************************

> void OpToggleMouldGrid::Do(OpDescriptor* opDesc)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	18/01/95
	Inputs:		pointer to an OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		failandexecute will be called if the operation fails in some way, most
				likely when no memory is available. 

********************************************************************************************/

void OpToggleMouldGrid::Do(OpDescriptor*)
{
	// Get a snapshot of the selection
	SelRange* Selected = GetApplication()->FindSelection();
	Node* pNode = Selected->FindFirst();
	while (pNode)
	{
		if (pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeMould))
		{
			NodeMould* pMould = (NodeMould*)pNode;
			MouldGeometry* pGeometry = pMould->GetGeometry();
			if (pGeometry!=NULL)
			{
				Spread* pSpread = pNode->FindParentSpread();
				if (pSpread)
					pGeometry->ToggleControlBlobs(pSpread);
			}
		}
		pNode = Selected->FindNext(pNode);
	}
	End();
}






/********************************************************************************************

>	OpDetachMould::OpDetachMould() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/03/95
	Purpose:	Constructor for Detach mould operation
	SeeAlso:	-

********************************************************************************************/

OpDetachMould::OpDetachMould()
{
	// Dummy constructor
}
  


/********************************************************************************************

>	BOOL OpDetachMould::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/03/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	Initialise the detach mould operation so that it can be accessed from the
				interface.

	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpDetachMould::Init()
{
	return (RegisterOpDescriptor(0,									// tool ID
 								_R(IDS_DETACHMOULD),					// string resource ID
								CC_RUNTIME_CLASS(OpDetachMould),	// runtime class for Op
 								OPTOKEN_DETACHMOULD,				// Ptr to token string
 								OpDetachMould::GetState,			// GetState function
 								0,									// help ID
 								_R(IDBBL_DETACHMOULD),					// bubble help ID
								0
								));
}               


/********************************************************************************************

>	OpState	OpDetachMould::GetState(String_256*, OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/03/95
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpDetachMould
	Purpose:	For finding the OpDetachMould state. 

********************************************************************************************/

OpState	OpDetachMould::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
	String_256 DisableReason; 

	NodeMould* pLastMould;
	INT32 nmoulds = MouldInfoBarOp::SelInfo(&pLastMould);

	// Set the detach operation tick and grey fields correctly
	OpSt.Greyed = (nmoulds==0);
	OpSt.Ticked = ((nmoulds==1) && pLastMould->IsDetached());

	return (OpSt);   
}

/********************************************************************************************

>	void OpDetachMould::GetOpName(String_256* OpName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description
				appropriate to the type of attribute that the operation applies.

********************************************************************************************/

void OpDetachMould::GetOpName(String_256* OpName)
{
	*OpName = String_256(_R(IDS_UNDO_DETACHMOULD));
}



/********************************************************************************************

> void OpDetachMould::Do(OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	20/03/95
	Inputs:		pointer to an OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		failandexecute will be called if the operation fails in some way, most
				likely when no memory is available. 

********************************************************************************************/

void OpDetachMould::Do(OpDescriptor*)
{
	BeginSlowJob();

	// Note, I only spent the whole afternoon trying to work out why DoStartSelOp(XX,YY)
	// Was completely screwing up the selection here. Anyway, the solution was to
	// derive this op from UndoableOperation, not SelOperation which solved the problem
	// immediately. The morel is, if the selection is not changing, dont use SelOperation!
	// + It stops lots of flashing of blobs too.

	// Scan the selection
	SelRange* Selected = GetApplication()->FindSelection();
	Node* pNode = Selected->FindFirst();
	while (pNode)
	{
		if (pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeMould))
		{
			NodeMould* pMould = (NodeMould*)pNode;

			SaveDetachAction* pSaveDetAct;
			ALLOC_WITH_FAIL(pSaveDetAct, (new SaveDetachAction), this);
			if (pSaveDetAct==NULL)
			{
				FailAndExecute(); End(); return;
			}
			
			if ((pSaveDetAct->Save(this, pMould))==AC_FAIL)
			{
				FailAndExecute(); End(); return;
			}

			// toggle the detach type
			pMould->ToggleDetachFlag();

			if (!pMould->IsDetached())
			{
				if (!DoInvalidateNodeRegion(pMould,TRUE))
				{
					FailAndExecute(); End(); return;
				}

				if (pMould->StartSaveContext(this,REC_REBUILD)!=CC_OK)
				{
					FailAndExecute(); End(); return;
				}

				if (pMould->RemouldAll(this)!=CC_OK)
				{
					FailAndExecute(); End(); return;
				}

				if (pMould->EndSaveContext(this,REC_REBUILD)!=CC_OK)
				{
					FailAndExecute(); End(); return;
				}
			}
		}
		pNode = Selected->FindNext(pNode);
	}

	End();
}




/********************************************************************************************

>	OpRotateMould::OpRotateMould() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/95
	Purpose:	Constructor for Detach mould operation
	SeeAlso:	-

********************************************************************************************/

OpRotateMould::OpRotateMould()
{
	// Dummy constructor
}
  


/********************************************************************************************

>	BOOL OpRotateMould::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/03/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	Initialise the rotate mould operation so that it can be accessed from the
				interface.
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.

********************************************************************************************/

BOOL OpRotateMould::Init()
{
	return (RegisterOpDescriptor(0,									// tool ID
 								_R(IDS_ROTATEMOULD),					// string resource ID
								CC_RUNTIME_CLASS(OpRotateMould),	// runtime class for Op
 								OPTOKEN_ROTATEMOULD,				// Ptr to token string
 								OpRotateMould::GetState,			// GetState function
 								0,									// help ID
 								_R(IDBBL_ROTATEMOULD),					// bubble help ID
 								_R(IDD_MOULDTOOLBAR),					// resource ID
								_R(IDC_BTN_ROTATEMOULD),				// control ID
								SYSTEMBAR_EDIT,						// Group bar ID
								TRUE, 								// Receive messages
								FALSE,								// Smart
								FALSE,								// Clean
								0,									// One open Instance ID
								(GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION)
 								)); 
			 
}               


/********************************************************************************************

>	OpState	OpRotateMould::GetState(String_256*, OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/03/95
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpRotateMould
	Purpose:	For finding the OpRotateMould state. 

********************************************************************************************/

OpState	OpRotateMould::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
	String_256 DisableReason; 
   	OpSt.Greyed = FALSE;

	BOOL Sel = FALSE;
	if (Document::GetCurrent() != NULL)
	{
		SelRange* Selected = GetApplication()->FindSelection();
		if (Selected)
		{
			Node* pNode = Selected->FindFirst();
			while ((pNode!=NULL) && (!Sel))
			{
				// only rotate mould objects if they're not detached.
				if (pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeMould))
				{
					if ( !((NodeMould*)pNode)->IsDetached() )
						Sel=TRUE;
				}
				pNode = Selected->FindNext(pNode);
			}
		}
	}

	if (!Sel)
		OpSt.Greyed = TRUE;

	return (OpSt);   
}

/********************************************************************************************

>	void OpRotateMould::GetOpName(String_256* OpName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description
				appropriate to the type of attribute that the operation applies.

********************************************************************************************/

void OpRotateMould::GetOpName(String_256* OpName)
{
	*OpName = String_256(_R(IDS_UNDO_ROTATEMOULD));
}

/********************************************************************************************

> void OpRotateMould::Do(OpDescriptor* opDesc)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	18/01/95
	Inputs:		pointer to an OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Scan through the selection finding mould objects. Each mould will have
				its contents remoulded, having has its mould path rotated around
				logically by one element. The result will be to spin the orientation
				of the moulded objects around.
	Errors:		failandexecute will be called if the operation fails in some way, most
				likely when no memory is available. 

********************************************************************************************/

void OpRotateMould::Do(OpDescriptor*)
{
	BeginSlowJob();
	SelRange* Selected = GetApplication()->FindSelection();
	Node* pNode = Selected->FindFirst();
	while (pNode)
	{
		if (pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeMould))
		{
			NodeMould* pMould = (NodeMould*)pNode;

			if (!DoInvalidateNodeRegion(pMould,TRUE))
			{
				FailAndExecute(); End(); return;
			}

			if (!DoRotateMould(pMould))
			{
				FailAndExecute(); End(); return;
			}
		}
		pNode = Selected->FindNext(pNode);
	}
	End();
}





/********************************************************************************************

>	OpRectangularEnvelope::OpRectangularEnvelope() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/95
	Purpose:	Constructor for Detach mould operation
	SeeAlso:	-

********************************************************************************************/

OpRectangularEnvelope::OpRectangularEnvelope()
{
	// Dummy constructor
}
  


/********************************************************************************************

>	BOOL OpRectangularEnvelope::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/03/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	Initialise the rotate mould operation so that it can be accessed from the
				interface.
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.

********************************************************************************************/

BOOL OpRectangularEnvelope::Init()
{
	return (RegisterOpDescriptor(0,										// tool ID
 								_R(IDS_RECTANGULARENVELOPE),				// string resource ID
								CC_RUNTIME_CLASS(OpRectangularEnvelope),// runtime class for Op
 								OPTOKEN_RECTANGULARENVELOPE,			// Ptr to token string
 								OpRectangularEnvelope::GetState,		// GetState function
 								0,										// help ID
								_R(IDBBL_RECTANGULARENVELOPE)
 								)); 
}               


/********************************************************************************************

>	OpState	OpRectangularEnvelope::GetState(String_256*, OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/03/95
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpRotateMould
	Purpose:	For finding the OpRotateMould state. 

********************************************************************************************/

OpState	OpRectangularEnvelope::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
	String_256 DisableReason; 
   	OpSt.Greyed = TRUE;
	
	ObjChangeFlags cFlags;
	cFlags.MultiReplaceNode = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);
	
	SelRange* pSelRange = GetApplication()->FindSelection();
	Node* pNode = pSelRange->FindFirst();

	while (pNode != NULL && OpSt.Greyed)
	{
		OpSt.Greyed = !(pNode->AllowOp(&ObjChange,FALSE));
		pNode = pSelRange->FindNext(pNode);
	}

	return (OpSt);   
}

/********************************************************************************************

>	void OpRectangularEnvelope::GetOpName(String_256* OpName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description
				appropriate to the type of attribute that the operation applies.

********************************************************************************************/

void OpRectangularEnvelope::GetOpName(String_256* OpName)
{
	*OpName = String_256(_R(IDS_UNDO_RECTANGULARENVELOPE));
}

/********************************************************************************************

> void OpRectangularEnvelope::Do(OpDescriptor* opDesc)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	18/01/95
	Inputs:		pointer to an OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Scan through the selection finding mould objects. Each mould will have
				its contents remoulded, having has its mould path rotated around
				logically by one element. The result will be to spin the orientation
				of the moulded objects around.
	Errors:		failandexecute will be called if the operation fails in some way, most
				likely when no memory is available. 

********************************************************************************************/

void OpRectangularEnvelope::Do(OpDescriptor*)
{
	BeginSlowJob();

	// Assume faied until proven otherwise.
	BOOL Failed=TRUE;

	// Create a manifold to use 
	Path* pDefShape = new Path;
	if (pDefShape!=NULL)
	{
		if (pDefShape->Initialise(12,12))
		{
			if (EnvelopeShapes::Rectangular(pDefShape))
			{
				Failed=!CreateNewMould(pDefShape, MOULDSPACE_ENVELOPE, TRUE, FALSE);
			}
		}
		// remove the shape once used
		delete pDefShape;
	}

	if (Failed)
		FailAndExecute();

	End();
}





/********************************************************************************************

>	OpRectangularPerspective::OpRectangularPerspective() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/95
	Purpose:	Constructor for Detach mould operation
	SeeAlso:	-

********************************************************************************************/

OpRectangularPerspective::OpRectangularPerspective()
{
	// Dummy constructor
}
  


/********************************************************************************************

>	BOOL OpRectangularPerspective::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/03/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	Initialise the rotate mould operation so that it can be accessed from the
				interface.
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.

********************************************************************************************/

BOOL OpRectangularPerspective::Init()
{
	return (RegisterOpDescriptor(0,											// tool ID
 								_R(IDS_RECTANGULARPERSPECTIVE),					// string resource ID
								CC_RUNTIME_CLASS(OpRectangularPerspective),	// runtime class for Op
 								OPTOKEN_RECTANGULARPERSPECTIVE,				// Ptr to token string
 								OpRectangularPerspective::GetState,			// GetState function
 								0,											// help ID
								_R(IDBBL_RECTANGULARPERSPECTIVE)
 								)); 
}               


/********************************************************************************************

>	OpState	OpRectangularPerspective::GetState(String_256*, OpDescriptor*)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/03/95
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpRotateMould
	Purpose:	For finding the OpRotateMould state. 

********************************************************************************************/

OpState	OpRectangularPerspective::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
	String_256 DisableReason; 
   	OpSt.Greyed = TRUE;
	
	ObjChangeFlags cFlags;
	cFlags.MultiReplaceNode = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);
	
	SelRange* pSelRange = GetApplication()->FindSelection();
	Node* pNode = pSelRange->FindFirst();

	while (pNode != NULL && OpSt.Greyed)
	{
		OpSt.Greyed = !(pNode->AllowOp(&ObjChange,FALSE));
		pNode = pSelRange->FindNext(pNode);
	}

	return (OpSt);   
}

/********************************************************************************************

>	void OpRectangularPerspective::GetOpName(String_256* OpName)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/01/95
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description
				appropriate to the type of attribute that the operation applies.

********************************************************************************************/

void OpRectangularPerspective::GetOpName(String_256* OpName)
{
	*OpName = String_256(_R(IDS_UNDO_RECTANGULARPERSPECTIVE));
}

/********************************************************************************************

> void OpRectangularPerspective::Do(OpDescriptor* opDesc)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	18/01/95
	Inputs:		pointer to an OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Scan through the selection finding mould objects. Each mould will have
				its contents remoulded, having has its mould path rotated around
				logically by one element. The result will be to spin the orientation
				of the moulded objects around.
	Errors:		failandexecute will be called if the operation fails in some way, most
				likely when no memory is available. 

********************************************************************************************/

void OpRectangularPerspective::Do(OpDescriptor*)
{
	BeginSlowJob();

	// Assume faied until proven otherwise.
	BOOL Failed=TRUE;

	// Create a manifold to use 
	Path* pDefShape = new Path;
	if (pDefShape!=NULL)
	{
		if (pDefShape->Initialise(12,12))
		{
			if (PerspectiveShapes::Rectangular(pDefShape))
			{
				Failed=!CreateNewMould(pDefShape, MOULDSPACE_PERSPECTIVE, TRUE, FALSE);
			}
		}
		// remove the shape once used
		delete pDefShape;
	}

	if (Failed)
		FailAndExecute();

	End();
}


