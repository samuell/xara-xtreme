// $Id: blnktool.cpp 662 2006-03-14 21:31:49Z alex $
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
// Implementation of the blank tool

/*
*/

#include "camtypes.h"
#include "oilfiles.h"
#include "csrstack.h"
//#include "viewrc.h"
#include "app.h"
#include "docview.h"
//#include "markn.h"
#include "blobs.h"			// for blob rendering.
//#include "mike.h"			// <******* Remember to include your own personal .h file

#include "blnktool.h"		// <******* Remember to change to include the tool's header file

DECLARE_SOURCE( "$Revision: 662 $" );

CC_IMPLEMENT_MEMDUMP(BlankTool,Tool_v1)
CC_IMPLEMENT_DYNCREATE(BlankInfoBarOp,InformationBarOp)

// Must come after the last CC_IMPLEMENT.. macro
#define new CAM_DEBUG_NEW     

// These are still char* while we wait for resource technology to be developed for modules
char* BlankTool::FamilyName	= "Blank Tools";
char* BlankTool::ToolName 	= "Blank Tool";
char* BlankTool::Purpose 	= "Blank manipulation";
char* BlankTool::Author 	= "Buster Gonad";

// Init those other useful static vars
BOOL			BlankTool::CurrentTool 			= FALSE;
BlankInfoBarOp*	BlankTool::pBlankInfoBarOp		= NULL;

/********************************************************************************************

>	BlankTool::BlankTool()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	Default Constructor.
				Other initialisation is done in BlankTool::Init which is called by the Tool Manager
	SeeAlso:	BlankTool::Init

********************************************************************************************/

BlankTool::BlankTool()
{
	pcCurrentCursor = NULL;
}

/********************************************************************************************

>	BlankTool::~BlankTool()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	Destructor (Virtual). Does nothing.

********************************************************************************************/

BlankTool::~BlankTool()
{
}


/********************************************************************************************

>	BOOL BlankTool::Init( INT32 Pass )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Returns:	FALSE if it does not want to be created, TRUE otherwise
	Purpose:	Used to check if the Tool was properly constructed
	SeeAlso:	BlankTool::BlankTool

********************************************************************************************/

BOOL BlankTool::Init()
{
	// Declare all your ops here and only succeed if all declarations succeed

	BOOL ok = TRUE;

	// This section reads in the infobar definition and creates an instance of
	// BlankInfoBarOp.  Also pBlankInfoBarOp, the ptr to the tool's infobar, is set up
	// after the infobar is successfully read and created.
	if (ok)
	{
		CCResTextFile 		file;				// Resource File
		BlankInfoBarOpCreate BarCreate;			// Object that creates BlankInfoBarOp objects

		 		ok = file.open(_R(IDM_BLANK_BAR), _R(IDT_INFO_BAR_RES));		// Open resource
		if (ok) ok = DialogBarOp::ReadBarsFromFile(file,BarCreate);	// Read and create info bar
		if (ok) file.close();									 	// Close resource

		ENSURE(ok,"Unable to load blankbar.ini from resource\n"); 

		if (ok)
		{
			// Info bar now exists.  Now get a pointer to it
			String_32 str = String_32("Blank info bar");
			DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(str);

					ok = (pDialogBarOp != NULL);
			if (ok) ok = pDialogBarOp->IsKindOf(CC_RUNTIME_CLASS(BlankInfoBarOp));
			if (ok) pBlankInfoBarOp = (BlankInfoBarOp*)pDialogBarOp;

			ENSURE(ok,"Error finding the blank tool info bar");
		}
	}

	return (ok);
}


/********************************************************************************************

>	void BlankTool::Describe(void *InfoPtr)

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

void BlankTool::Describe(void *InfoPtr)
{
	// Cast structure into the latest one we understand.
	ToolInfo_v1 *Info = (ToolInfo_v1 *) InfoPtr;

	Info->InfoVersion = 1;
	
	Info->InterfaceVersion = GetToolInterfaceVersion();  // You should always have this line.
		
	// These are all arbitrary at present.
	Info->Version = 1;
	Info->ID      = GetID();
	Info->TextID  = _R(IDS_BLANK_TOOL);

	Info->Family  = FamilyName;
	Info->Name    = ToolName;
	Info->Purpose = Purpose;
	Info->Author  = Author;

	Info->BubbleID = _R(IDBBL_BLANK_TOOLBOX);
}

/********************************************************************************************

>	virtual void BlankTool::SelectChange(BOOL isSelected)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		isSelected	- TRUE  = tool has been selected
							- FALSE = tool has been deselected
	Outputs:	-
	Returns:	-
	Purpose:	Starts up and closes down the blank tool
	Errors:		Debug warning if creating the cursor fails.
	SeeAlso:	-

********************************************************************************************/

void BlankTool::SelectChange(BOOL isSelected)
{
	if (isSelected)
	{
		if (!CreateCursors()) return;
		CurrentCursorID = CursorStack::GPush(pcNormalBlankCursor, FALSE);		// Push cursor but don't display now
		pcCurrentCursor = pcNormalBlankCursor;

		// This tool is now the current one
		CurrentTool = TRUE;

		// Create and display the tool's info bar
		pBlankInfoBarOp->Create();
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
		pBlankInfoBarOp->Delete();

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
	}
}

/********************************************************************************************

>	BOOL BlankTool::CreateCursors()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if all the blank tool cursors have been successfully created
	Purpose:	Creates all the blank tool cursors
	SeeAlso:	-

********************************************************************************************/

BOOL BlankTool::CreateCursors()
{
	// This tool has just been selected.  Create the cursors.
	pcNormalBlankCursor = new Cursor(this, _R(IDC_BLANKTOOLCURSOR));

	if ( pcNormalBlankCursor==NULL || !pcNormalBlankCursor->IsValid())
	{
		DestroyCursors();
		return FALSE;
	}
	else
		return TRUE;
}

/********************************************************************************************

>	void BlankTool::DestroyCursors()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroys all the blank tool cursors
	SeeAlso:	-

********************************************************************************************/

void BlankTool::DestroyCursors()
{
	if (pcNormalBlankCursor != NULL) delete pcNormalBlankCursor;
}

/********************************************************************************************

>	void BlankTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		PointerPos 	- 	The DocCoord of the point where the mouse button was clicked
				Click 		-	Describes the type of click that was detected. 
				ClickMods 	-	Indicates which buttons caused the click and which modifers were
								pressed at the same time
				pSpread 	- 	The spread in which the click happened
	Returns:	-
	Purpose:	To handle a Mouse Click event for the Blank Tool.
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers

********************************************************************************************/

void BlankTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )
{
	if (ClickMods.Menu) return;							// Don't do anything if the user clicked the Menu button

	// Make sure this click is one that we want
	if ( Click == CLICKTYPE_SINGLE || Click == CLICKTYPE_DOUBLE)
	{
	}

	if ( Click == CLICKTYPE_DRAG )
	{
	}
}

/********************************************************************************************

>	void BlankTool::OnMouseMove( DocCoord PointerPos,Spread* pSpread, ClickModifiers ClickMod )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		PointerPos 	- 	The DocCoord of the point where the mouse has moved to
				pSpread 	- 	The spread in which the move occurred
				ClickMods	- 	The state of the various modifiers at the time of the mouse move
	Returns:	TRUE if it handled the Click, FALSE otherwise
	Purpose:	To handle a Mouse Move event for the Blank Tool.
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers

********************************************************************************************/

void BlankTool::OnMouseMove(DocCoord PointerPos,Spread* pSpread,ClickModifiers ClickMods)
{
	// You can use this func to do things like change the pointer shape, or
	// alter the text in the status bar

	// Use this func to change your curor
	// 		CursorStack::GSetTop(Cursor* pCursor, CurrentCursorID)

	// Display status bar text for the current position
	DisplayStatusBarHelp(PointerPos, pSpread, ClickMods);
}

/********************************************************************************************

>	void BlankTool::RenderToolBlobs(Spread* pSpread,DocRect* pDocRect)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		pSpread  = ptr to a spread
				pDocRect = ptr to DocRect of spread to render in
	Returns:	-
	Purpose:	Handles the RenderToolBlobs method.
				Renders the tool's blobs into the selected doc view.
	SeeAlso:	

********************************************************************************************/

void BlankTool::RenderToolBlobs(Spread* pSpread,DocRect* pDocRect)
{
	// Render into the selected doc view
	DocView* pDocView = DocView::GetSelected();
	if (pDocView != NULL)
	{
		// render you tool's blobs (if any) here
	}
}

/********************************************************************************************

>	void BlankTool::DisplayStatusBarHelp(DocCoord DocPos, Spread* pSpread, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		DocPos - the document coordinate of the point to display help on
				pSpread - pointer to the spread containing DocPos
				ClickMods - the current click modifiers
	Outputs:	-
	Returns:	-
	Purpose:	Displays status help string for the given position in the status bar.
	SeeAlso:	BlankTool::GetCurrentStatusText

********************************************************************************************/

void BlankTool::DisplayStatusBarHelp(DocCoord DocPos, Spread* pSpread, ClickModifiers ClickMods)
{
	String_256 StatusMsg("");

	// Get a string from the underlying help function and display it.
	GetCurrentStatusText(&StatusMsg, pSpread, DocPos, ClickMods);
	GetApplication()->UpdateStatusBarText(&StatusMsg);							 
}

/********************************************************************************************

>	virtual BOOL BlankTool::GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		ptest - pointer to a string
				pSpread points to a spread
				DocPos points to a point in a document
				ClickMods are the current click modifiers
	Outputs:	Updates the string in ptext
	Returns:	TRUE if the string was updates (FALSE if not updated)
	Purpose:	Returns the current status line help string
	SeeAlso:	BlankTool::GetCurrentStatusText, Tool_v1::GetStatusLineText

********************************************************************************************/

BOOL BlankTool::GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)
{
	// We can call the underlying help function to get a string and return the result.
	GetCurrentStatusText(ptext, pSpread, DocPos, ClickMods);
	return TRUE;
}

/********************************************************************************************

>	void BlankTool::GetCurrentStatusText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		pSpread points to a spread
				DocPos points to a point in a document
				ClickMods are the current click modifiers
	Outputs:	Updates the string in ptext
	Returns:	-
	Purpose:	Selects a suitable string for the status line based on the current location
				(as input via the parameters).
	SeeAlso:	BlankTool::GetStatusLineText, BlankTool::DisplayStatusBarHelp

********************************************************************************************/

void BlankTool::GetCurrentStatusText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)
{
	// You must use the Spread, DocCoord and ClickModifiers to select a suitable string to be 
	// displaied in the status bar.  This is usually done via some sort of switch statement.
	// Having selected a suitable string you should do a statement like	
	//	ptext->Load(<#Insert your string ID here #>);

	// Delete this line when you display useful strings!
	ptext->Empty(); 
}

//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------

/********************************************************************************************

>	MsgResult BlankInfoBarOp::Message(Msg* Message) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		Message = The message to handle
	Outputs:	-
	Returns:	-
	Purpose:	Blank info bar dialog message handler
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult BlankInfoBarOp::Message(Msg* Message) 
{
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
			// Initialise the infobar controls here
			// This is sent when you create the infobar in your tool startup code
		}
		else
		{
			// Switch statement allows you to do things when receiving messages
			// for specific gadgets
			//
			// Just uncomment the next 3 lines to get started
			//
			//		switch (Msg->GadgetID)
			//		{
			//		}
		}
	}

	// Pass the message on to the immediate blank class
	return (InformationBarOp::Message(Message));
}    
