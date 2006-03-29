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
// The Push Tool

/*
*/


#include "camtypes.h"

#include "strings.h"
#include "docview.h"
#include "ensure.h"
#include "toollist.h"
#include "tool.h"
//#include "oiltool.h"
//#include "viewrc.h"
//#include "resource.h"
#include "errors.h"
#include "pushtool.h"
//#include "mario.h"
#include "csrstack.h"
//#include "oilkeys.h"
//#include "justin.h"
//#include "markn.h"
#include "oilfiles.h"
#include "blobs.h"
#include "app.h"
#include "progress.h"
#include "zoomops.h"

//#include "will2.h"
//#include "hotlink.h"
#include "ralphdoc.h"
#include "userattr.h"
#include "bubbleid.h"
#include "brushmsg.h"

// These are still char* while we wait for resource technology to be developed for modules
TCHAR*	PushTool::FamilyName	= _T("View Tools");
TCHAR* 	PushTool::ToolName		= _T("Push Tool");
TCHAR*	PushTool::Purpose		= _T("To push the document around");
TCHAR*	PushTool::Author		= _T("Rik");

DECLARE_SOURCE("$Revision: 662 $");


CC_IMPLEMENT_MEMDUMP(PushTool, Tool_v1)


/********************************************************************************************

>	PushTool::PushTool()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Dummp Constructor - It does nothing. All the real initialisation is done
				in PushTool::Init which is called by the Tool Manager
	SeeAlso:	PushTool::Init

********************************************************************************************/

PushTool::PushTool()
  : pcPushCursor(NULL),
  	pPushInfoBarOp(NULL)
{
	// Dummy constructor
}





/********************************************************************************************

>	virtual PushTool::~PushTool()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Destructor. Does nothing.

********************************************************************************************/

PushTool::~PushTool()
{
	// Dummy destructor
}



/********************************************************************************************

>	virtual BOOL PushTool::Init()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Returns:	FALSE if it does not want to be created, TRUE otherwise
	Purpose:	Used to check if the Tool was properly constructed
	SeeAlso:	PushTool::PushTool

********************************************************************************************/

BOOL PushTool::Init()
{
/*	// Claim right-alt for our tool.
	ToolKeyClaimDetails Mods;
	Mods.Value = 0;
	Mods.Keys.Alternative2 = TRUE;
	ClaimToolKey((Tool*) this, Mods);
*/
	// Initially, no cursor.
	pcPushCursor = 0;

	// RALPH don't need no sissy infobar
#if defined(EXCLUDE_FROM_RALPH)
	return OpPush::Declare();
#else

	pPushInfoBarOp = new ZoomInfoBarOp(_R(IDD_PUSHINFO)); // Push tool uses a zoom info bar
	BOOL ok=(pPushInfoBarOp!=NULL);

#if 0
	// Load in the info-bar stuff.
	CCResTextFile 		file;				// Resource File
	ZoomInfoBarOpCreate BarCreate;			// Object that creates PushInfoBarOp objects

	BOOL	ok = file.open(_R(IDM_PUSH_BAR), _R(IDT_INFO_BAR_RES));		  // Open resource
	if (ok) ok = DialogBarOp::ReadBarsFromFile(file, BarCreate);  // Read & create bar
	if (ok) file.close();									 	  // Close resource

	ENSURE(ok,"Unable to load PUSHBAR.INI from resource\n"); 

	if (ok)
	{
			// Info bar now exists.  Now get a pointer to it.
		String_32 str(_R(IDS_PUSHTOOL_INFOBARNAME));
		DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(str);

				ok = (pDialogBarOp != NULL);
		if (ok) ok = pDialogBarOp->IsKindOf(CC_RUNTIME_CLASS(ZoomInfoBarOp));
		if (ok) pPushInfoBarOp = (ZoomInfoBarOp*) pDialogBarOp;

		ENSURE(ok,"Couldn't find push tool info bar");
	}
#endif

	// Register our push operation.
	return ok && OpPush::Declare();
#endif
}





/********************************************************************************************

>	void PushTool::Describe(void *InfoPtr)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		InfoPtr - A pointer to a tool info block. It is passed cast to void* as
				the version of the tool is unknown at this point. Later versions of the
				Tool class may have more items in this block, that this tool will not use
	Outputs:	InfoPtr - The structure pointed to by InfoPtr will have had all the info
				that this version of the Tool knows about
	Purpose:	Allows the tool manager to extract information about the tool

********************************************************************************************/

void PushTool::Describe(void *InfoPtr)
{
	// Cast structure into the latest one we understand.
	ToolInfo_v1 *Info = (ToolInfo_v1 *) InfoPtr;

	Info -> InfoVersion = 1;
	
	Info -> InterfaceVersion = GetToolInterfaceVersion();  // You should always have this line.
		
	// These are all arbitrary at present.
	Info -> Version = 1;
	Info -> ID      = GetID();
	Info -> TextID  = _R(IDS_PUSH_TOOL);

	Info -> Family  = FamilyName;
	Info -> Name    = ToolName;
	Info -> Purpose = Purpose;
	Info -> Author  = Author;
	
	Info -> InfoBarDialog = 0;
	Info->BubbleID = _R(IDBBL_PUSH_TOOLICON);
}



// The Push Tools EventHandlers


/********************************************************************************************

>	void PushTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
							Spread* pSpread )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		PointerPos - The Coords (in DocCoords) of the point where the mouse button
				was clicked
				Click -	Describes the type of click that was detected. 
				ClickMods -	Indicates which buttons caused the click and which modifers were
				pressed at the same time
				pSpread - Pointer to the spread that the click was on
	Returns:	TRUE if it handled the Click, FALSE otherwise
	Purpose:	To handle a Mouse Click event for the Push Tool. It starts up a Push 
				Operation.
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers

********************************************************************************************/

void PushTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )
{
	if (ClickMods.Menu) return;							// Don't do anything if the user clicked the Menu button

	// Make sure this click is one that we want
	if ( Click == CLICKTYPE_SINGLE )
	{

#ifdef RALPH
		//Graham 20/9/96: If we're in Ralph, clicking on a Hot Link activates it.
		//The one exception to this rule is if CTRL is pressed, in which case
		//the tool acts as normal.

		//So, is CTRL pressed?
		if (!(ClickMods.Constrain))
		{
			//No. Are we on a Hot Link? Let's try and find one.

			//First, find a Hot Link
	
			Node* pHotLinkNode= DocView::FindHotLinkNode(PointerPos);

			//If we've found one, go to that Hot Link
			if (pHotLinkNode)
			{
				DocView::GoToHotLink((AttrUser*) pHotLinkNode);
				return;
			}
		}

		//If CTRL was pressed, or if we weren't over a Hot Link, we treat
		//the click as a normal Push drag. Read on...

#endif //Ralph

		// Just what we wanted - Someone is dragging the mouse about
		// We need to make an operation to perform the drag with
		OpPush* pOpPush = new OpPush;
		if (pOpPush == NULL)
		{
			// Failed to get the memory to do the job
			// I guess we should just ignore the click, though the user is going to
			// wonder just what is happening!
//			TRACE( _T("The Push Operation could not start due to lack of memory\n") );

			// Inform the person doing the clicking that life is not looking so good
			InformError( _R(IDS_OUT_OF_MEMORY), _R(IDS_OK) );
		}
		else
		{
			// Start the drag operation and pass in the Anchor Point to the push operation
			pOpPush -> DoDrag( PointerPos );
		}
	}
}





/********************************************************************************************
>	virtual void PushTool::SelectChange(BOOL isSelected)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/93
	Inputs:		A flag which if TRUE means the tool has just been selected, if FALSE that
				the tool is being deselected.
	Outputs:	-
	Returns:	-
	Purpose:	When selected, creates a push-tool cursor object and pushes it on the
				cursor stack; when deselected pops the cursor from the stack and deletes
				it.
	Errors:		-
	SeeAlso:	Tool_v1::SelectChange
********************************************************************************************/

void PushTool::SelectChange(BOOL isSelected)
{
	if (isSelected)
	{
		// This tool has just been selected.  Create our cursor and push it on the cursor
		// stack.
		pcPushCursor = new Cursor(this, _R(IDC_PUSHTOOLCURSOR));

#ifdef RALPH
		//Graham 20/9/96. This cursor is needed to activate
		//HotLinks (TM) in Ralph (TM). The cursor is a static member of
		//the Cursor class and so we don't need to delete it.
		pcHotLinkCursor = Cursor::PointingHand;
#endif //Ralph

		if (!pcPushCursor)
		{
			TRACE( _T("Couldn't create push-tool cursor!\n"));
			return;
		}

		CurrentCursorID = CursorStack::GPush(pcPushCursor, FALSE);			// don't display just yet
#if !defined(EXCLUDE_FROM_RALPH)
		pPushInfoBarOp->Create();
#endif
		// good to see Neville's back into his old habits of leaving huge swathes of redundant code all over the place
		// Which blobs do I want displayed
		// Used to declare an interest in no blobs which meant that all blobs disappeared
		// when this tool is selected. Now try not to affect this by not declaring an
		// interest.
		// Changed by Neville 16/8/94
		//	BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		//	if ((BlobMgr != NULL) && (Document::GetCurrent() != NULL))
		//	{
		//	 	// Decide which blobs to display
		//		BlobStyle MyBlobs;
		//		// We want them all off, which is the default

		//		// tell the blob manager
		//		BlobMgr->ToolInterest(MyBlobs);
		//	}
	}
	else
	{
		// This tool is being deselected, so pop our cursor from the stack and deallocate it.
		if (pcPushCursor)
		{
			CursorStack::GPop(CurrentCursorID);
			delete pcPushCursor;
			CurrentCursorID = 0;
		}
#if !defined(EXCLUDE_FROM_RALPH)
			pPushInfoBarOp->Delete();
#endif

	}
}


/********************************************************************************************
>	BOOL PushTool::GetStatusLineText(String_256* pText, Spread*, DocCoord, ClickModifiers);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Inputs:		Spread*, DocCoords, ClickModifiers - unused
	Outputs:	pText -
	Returns:	TRUE if pText holds valid text
	Purpose:	Get status line help text for push tool when over doc
	Errors:		this==NULL
********************************************************************************************/

BOOL PushTool::GetStatusLineText(String_256* pText, Spread*, DocCoord, ClickModifiers)
{
	ERROR2IF(this==NULL, FALSE,"PushTool::GetStatusLineText() - this==NULL");
	ERROR2IF(pText==NULL,FALSE,"PushTool::GetStatusLineText() - pText==NULL");

	return pText->Load(_R(IDS_PUSHTOOL_STATUSHELP));
}


/********************************************************************************************
>	void PushTool::OnMouseMove(DocCoord, Spread*, ClickModifiers)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Inputs:		DocCoord, Spread* ClickModifiers - only used in Ralph
	Purpose:	immediately updates status line text

				In Ralph, updates status line text and changes the cursor.
********************************************************************************************/

void PushTool::OnMouseMove(DocCoord dcPoint, Spread* pSpread, ClickModifiers mods)
{
#ifdef RALPH
	//Graham 20/9/96.
	//We need to check if the zoom tool's over a Hot Link. If it is, the cursor changes
	//to a pointing hand.

	//The one exception is if CTRL is pressed, in which case the cursor is the
	//normal Push tool cursor (Disabled at the moment)

	//First, try and find a node with a Hot Link

	AttrUser* pHotLinkNode= (AttrUser*) DocView::FindHotLinkNode(dcPoint);

	if (pHotLinkNode && !(mods.Constrain))
	{
		//Yes. So change the cursor to a pointing hand
		ChangeCursor(Cursor::PointingHand);

		//And now to update the status bar of the browser

		//So first get a pointer to the document
				
		//And a pointer to the Ralph document...
		Document* ThisDoc=Document::GetSelected();
		RalphDocument* ThisRalphDoc=ThisDoc->GetRalphDoc();

		ERROR3IF(ThisRalphDoc==NULL, "DocView::OnMouseMoveWithNoTool has no Ralph Doc");

		//Then get the base URL of this Ralph document as a Web Address
		WebAddress wBase(ThisRalphDoc->GetBaseURL());

		//And get the URL of this node as a Web Address
		WebAddress wEmbedded(pHotLinkNode->GetWebAddress());

		//This function makes wEmbedded into an absolute URL (if necessary)
		wEmbedded.Combine(wBase);

		//Make the Web Address into a normal string
		String_256 strEmbedded=wEmbedded.GetWebAddress();

		//And put that URL into the status bar string
		String_256 sPushTool;

		sPushTool.MakeMsg(_R(IDS_PUSHTOOL_HOTLINK), (TCHAR*) strEmbedded);
		
		//And tell the Ralph document to show the string in the browser's status
		//bar
		ThisRalphDoc->SetStatusBar(&sPushTool);

	}
	else
	{
		//No, the cursor is not over a Hot Link or CTRL is pressed
		//Change the cursor to the standard Push tool cursor
		//and display a standard message in the browser status bar
		ChangeCursor(pcPushCursor);

		String_256 StringToPass(_R(IDS_RALPH_PUSHTOOLSTR));

		DocView::SetBrowserStatusBar(&StringToPass);
	}

#endif //RALPH
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	if (this!=NULL)
	{
		String_256 text("");
		if (text.Load(_R(IDS_PUSHTOOL_STATUSHELP)))
			GetApplication()->UpdateStatusBarText(&text);
	}
	else
		ERROR3("PushTool::OnMouseMove() - this==NULL");
#endif
}


///////////////////////////////////////////////////////////////////////////////////////////
//  The Push Operation


CC_IMPLEMENT_DYNCREATE( OpPush, Operation )

/********************************************************************************************

>	OpPush::OpPush()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/93
	Purpose:	Constructor. This simply sets a few of the operation flags to say that
				it can not be added to the undo system and that it should be deleted
				when it ends.

********************************************************************************************/

OpPush::OpPush()
{
}



/********************************************************************************************

>	void OpPush::DoDrag( DocCoord Anchor )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		Anchor - The position of the mouse at the start of the Drag
	Purpose:	To start up the dragging of the document around. It records the
				starting point of the drag as this is used to calculate the offsets
				that are needed as the mouse moves about

********************************************************************************************/

void OpPush::DoDrag( DocCoord Anchor )
{
	// Find out the selected Document View
	DocView* pDocView = DocView::GetSelected();	
	ENSURE( pDocView != NULL, "OpPush::DoDrag - No selected DocView" );
	if (pDocView==NULL)
		return;

	// Get the mouse position in work coords
	AnchorPoint = pDocView -> GetClickWorkCoord();
	
	// and start a drag (pass in FALSE for KeepAccuracy, because we want this drag to
	// reflect the cursor position at all times - we're not really interested in the
	// intermediate mouse movements when there are lots of them).
	StartDrag( DRAGTYPE_NOSCROLL, NULL, NULL, FALSE );

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	String_256 text("");
	if (text.Load(_R(IDS_PUSHOP_STATUSHELP)))
		GetApplication()->UpdateStatusBarText(&text);
#endif
}




/********************************************************************************************

>	void OpPush::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, BOOL bSolidDrag)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods - Which key modifiers are being pressed
	Purpose:	This function will change the scroll offsets of the current view
				to track the mouse moving about. This will give the effect of sliding the
				document around with the mouse
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpPush::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread *, BOOL bSolidDrag)
{
	DocView* pDocView = DocView::GetSelected();
	ENSURE( pDocView != NULL, "OpPush::DragPointerMove - DocView was NULL" );
	if (pDocView==NULL)
		return;

	// Declare a few WorkCoords we will need
	WorkCoord ScrollOffsets;
	WorkCoord Change;	
	WorkCoord MoveClick = pDocView->GetClickWorkCoord();
	
	// How Much has it changed
	Change.x = AnchorPoint.x - MoveClick.x;
	Change.y = AnchorPoint.y - MoveClick.y;
	
	// Find the Scroll Offsets and change them
	ScrollOffsets = pDocView->GetScrollOffsets(); 
	ScrollOffsets.x += Change.x;
	ScrollOffsets.y += Change.y;
	
	// Make sure the ScrollOffsets are valid and then set them
	if ( ScrollOffsets.x < 0 ) ScrollOffsets.x = 0;
	if ( ScrollOffsets.y > 0 ) ScrollOffsets.y = 0;
	pDocView->SetScrollOffsets( ScrollOffsets, TRUE );
}



/********************************************************************************************

>	void OpPush::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, BOOL Success, BOOL bSolidDrag)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickMods - the key modifiers being pressed
				Success - TRUE if the drag was terminated properly, FALSE if it
				was ended with the escape key being pressed
	Purpose:	Finishes the scrolling around the document and ends the drag operation
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpPush::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, Spread *, BOOL Success, BOOL bSolidDrag)
{
	// Put up the hourglass as we have to
	BeginSlowJob();

	// End the Drag
	EndDrag();
	
	// If the drag failed, then fail the operation
	if (!Success) FailAndExecute();

	// End the operation
	End();

	// Send a message saying that the screen has changed, so that the brush tool can update
	BROADCAST_TO_ALL(ScreenChangeMsg());

}


/********************************************************************************************

>	BOOL OpPush::Declare()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpPush::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_PUSH_TOOL),
								CC_RUNTIME_CLASS(OpPush), 
								OPTOKEN_PUSH,
								OpPush::GetState,
								0,	/* help ID */
								_R(IDBBL_PUSHTOOLOP),
								0	/* bitmap ID */));

}


/********************************************************************************************

>	OpState OpPush::GetState(String_256* Description, OpDescriptor*)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the push tool
	Returns:	The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpPush::GetState(String_256*, OpDescriptor*)
{
	OpState Blobby;
	return Blobby;
}



/********************************************************************************************
>	BOOL OpPush::GetStatusLineText(String_256* pText, Spread*, DocCoord, ClickModifiers);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Inputs:		Spread*, DocCoords, ClickModifiers - unused
	Outputs:	pText -
	Returns:	TRUE if pText holds valid text
	Purpose:	Get status line help text for push tool when dragging
	Errors:		this==NULL
********************************************************************************************/

BOOL OpPush::GetStatusLineText(String_256* pText, Spread*, DocCoord, ClickModifiers)
{
	ERROR2IF(this==NULL,FALSE,"OpPush::GetStatusLineText() - this==NULL");

	return pText->Load(_R(IDS_PUSHOP_STATUSHELP));
}

#ifdef RALPH
/********************************************************************************************

>	void ZoomTool::ChangeCursor(Cursor* cursor)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>, from Jim's code
	Created:	20/9/96
	Inputs:		ID of the cursor you want to flip to
	Outputs:	-
	Returns:	-
	Purpose:	We only need this in Ralph (at present), where the push tool cursor
				sometimes changes to a pointing hand, which is used to activate Hot Links.
	
				Changes to the specified cursor. Will only change the cursor if it isn't already
				this cursor, so it doesn't flicker.


	Errors:		can fail if the cursor cannot be created - the cursor code will fail.
	SeeAlso:	-

********************************************************************************************/

void PushTool::ChangeCursor(Cursor* cursor)
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

>	void PushTool::OnKeyPress(KeyPress* pKey)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/96
	Inputs:		Keypress object
	Outputs:	-
	Returns:	-
	Purpose:	When a key is pressed, we call the Mouse Move function to update the
				cursor and status bar text.

				Disabled at the moment, but if Ralph ever handles Key Presses this should
				go in.


********************************************************************************************/
/*
BOOL PushTool::OnKeyPress(KeyPress* pKey)
{
	//First get the current click modifiers, pointer position and spread
	ClickModifiers ClickMods = ClickModifiers::GetClickModifiers();

	Spread*  pSpread;
	DocCoord dcMousePos;
	if (DocView::GetCurrentMousePos(&pSpread, &dcMousePos))
		OnMouseMove(dcMousePos, pSpread, ClickMods);
	
	return TRUE;
}
*/
#endif //Ralph
