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
// Implementation of the text tool

/*
*/

#include "camtypes.h"
#include "texttool.h"	

// Resource files
//#include "markn.h"
//#include "markg.h"	
//#include "peter.h"	
//#include "resource.h"
//#include "viewrc.h"
//#include "will2.h"

// Code headers
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "blobs.h"
#include "csrstack.h"
#include "unicdman.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "keypress.h"
#include "nodetext.h"
#include "nodetxts.h"
#include "nodetxtl.h"
#include "objchge.h"
#include "oilfiles.h"
#include "opscale.h"
#include "opsquash.h"
//#include "selmsg.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "txtattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "textinfo.h"
#include "textops.h"
#include "textfuns.h"
#include "vkextra.h"
#include "nodetxts.h"
#include "nodemold.h"
#include "cutop.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "layer.h"
#include "nodepostpro.h"
#include "nodepath.h"
#include "usercord.h"
#include "camview.h"

DECLARE_SOURCE( "$Revision$" );

CC_IMPLEMENT_MEMDUMP(TextTool,Tool_v1)
CC_IMPLEMENT_DYNAMIC(TextToolBlobPosItem, ListItem);
CC_IMPLEMENT_DYNAMIC(TextToolBlobPosList, List);


// Must come after the last CC_IMPLEMENT.. macro
#define new CAM_DEBUG_NEW     

// These are still char* while we wait for resource technology to be developed for modules
TCHAR* TextTool::FamilyName	= _T("Text Tools");
TCHAR* TextTool::ToolName 	= _T("Text Tool");
TCHAR* TextTool::Purpose 	= _T("Text manipulation");
TCHAR* TextTool::Author 	= _T("TextToolTeam");

// Init those other useful static vars
BOOL			TextTool::CurrentTool 			= FALSE;
TextInfoBarOp*	TextTool::pTextInfoBarOp		= NULL;

// Preference variables
BOOL TextTool::UseDeadKeys = TRUE;

extern void Beep();


// These arrays consist of two UINT32 pairs, normal code followed by accented code.
// Terminate with 0.
static UINT32 GraveArray[] = {	0x0041, 0x00C0,	// A
								0x0045, 0x00C8,	// E
								0x0049, 0x00CC,	// I
								0x004F, 0x00D2,	// O
								0x0055, 0x00D9,	// U
								0x0061, 0x00E0,	// a
								0x0065, 0x00E8,	// e
								0x0069, 0x00EC,	// i
								0x006F, 0x00F2,	// o
								0x0075, 0x00F9,	// u
								0x0000 };
static UINT32 AcuteArray[] = {	0x0041, 0x00C1,	// A
								0x0045, 0x00C9,	// E
								0x0049, 0x00CD,	// I
								0x004F, 0x00D3,	// O
								0x0055, 0x00DA,	// U
								0x0059, 0x00DD,	// Y
								0x0061, 0x00E1,	// a
								0x0065, 0x00E9,	// e
								0x0069, 0x00ED,	// i
								0x006F, 0x00F3,	// o
								0x0075, 0x00FA,	// u
								0x0079, 0x00FD,	// y
								0x0000 };
static UINT32 HatArray[] = 	{	0x0041, 0x00C2,	// A
								0x0045, 0x00CA,	// E
								0x0049, 0x00CE,	// I
								0x004F, 0x00D4,	// O
								0x0053, 0x008A,	// S
								0x0055, 0x00DB,	// U
								0x0061, 0x00E2,	// a
								0x0065, 0x00EA,	// e
								0x0069, 0x00EE,	// i
								0x006F, 0x00F4,	// o
								0x0073, 0x009A,	// s
								0x0075, 0x00FB,	// u
								0x0000 };
static UINT32 TildeArray[] = {	0x0041, 0x00C3,	// A
								0x004E, 0x00D1,	// N
								0x004F, 0x00D5,	// O
								0x0061, 0x00E3,	// a
								0x006E, 0x00F1,	// n
								0x006F, 0x00F5,	// o
								0x0000 };
static UINT32 ColonArray[] = {	0x0041, 0x00C4,	// A
								0x0045, 0x00CB,	// E
								0x0049, 0x00CF,	// I
								0x004F, 0x00D6,	// O
								0x0055, 0x00DC,	// U
								0x0059, 0x009F,	// Y
								0x0061, 0x00E4,	// a
								0x0065, 0x00EB,	// e
								0x0069, 0x00EF,	// i
								0x006F, 0x00F6,	// o
								0x0075, 0x00FC,	// u
								0x0079, 0x00FF,	// y
								0x0000 };
static UINT32 AtArray[] = 	{	0x0041, 0x00C5,	// A
								0x0061, 0x00E5,	// a
								0x0043, 0x00A9,	// C
								0x0052, 0x00AE,	// R
								0x0063, 0x00A9,	// c
								0x0072, 0x00AE,	// r
								0x0000 };
static UINT32 CommaArray[] = 	{	0x0043, 0x00C7,	// C
								0x0063, 0x00E7,	// c
								0x0000 };
static UINT32 SlashArray[] = 	{	0x004F, 0x00D8,	// O
								0x0063, 0x00A2,	// c
								0x006F, 0x00F8,	// o
								0x0000 };



/********************************************************************************************

>	TextTool::TextTool()

	Author:		Mark_Goodall (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	Default Constructor.
				Other initialisation is done in TextTool::Init which is called by the Tool Manager
	SeeAlso:	TextTool::Init

********************************************************************************************/

TextTool::TextTool()
{
	pcCurrentCursor = NULL;
	IsBlankCursorUp = FALSE;
	UpdateAfterTyping = FALSE;

	PreviousDeadKey = NULL;
	GraveVirtKey = NULL;
	AcuteVirtKey = NULL;
	HatVirtKey = NULL;
	TildeVirtKey = NULL;
	ColonVirtKey = NULL;
	AtVirtKey = NULL;
	CommaVirtKey = NULL;
	SlashVirtKey = NULL;
	pLastFocusStory = NULL;
}

/********************************************************************************************

>	TextTool::~TextTool()

	Author:		Mark_Goodall (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	Destructor (Virtual). Does nothing.

********************************************************************************************/

TextTool::~TextTool()
{
	pTextInfoBarOp->pTextTool = NULL;
	TextInfoBarOp::DeInit(); 

	RemoveDeadKeys();
}



/********************************************************************************************

>	BOOL TextTool::Init( INT32 Pass )

	Author:		Mark_Goodall (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Returns:	FALSE if it does not want to be created, TRUE otherwise
	Purpose:	Used to check if the Tool was properly constructed
	SeeAlso:	TextTool::TextTool

********************************************************************************************/

BOOL TextTool::Init()
{
	// Declare all your ops here and only succeed if all declarations succeed

	BOOL ok = TRUE;

	// Register CreateTextObjectOp 
	ok = ok && OpCreateTextObject::Init(); 
	ok = ok && OpTextSelection::Init(); 
	ok = ok && OpDeleteTextStory::Init(); 
	ok = ok && OpReverseStoryPath::Init(); 
	ok = ok && OpApplyJustificationToStory::Init(); 
	ok = ok && OpTextPaste::Init();
	ok = ok && OpTogglePrintTextAsShapes::Init();
	ok = ok && OpDragStoryNonPathLeftIndent::Init();
	ok = ok && OpDragStoryNonPathRightIndent::Init();
	ok = ok && OpDragStoryPathLeftIndent::Init();
	ok = ok && OpDragStoryPathRightIndent::Init();
	ok = ok && OpAffectFontChange::Init();
	ok = ok && TextInfoBarOp::Init(); // static initialisation

	// This section reads in the infobar definition and creates an instance of
	// TextInfoBarOp.  Also pTextInfoBarOp, the ptr to the tool's infobar, is set up
	// after the infobar is successfully read and created.
	if (ok)
	{
#if 0
		CCResTextFile 		file;				// Resource File
		TextInfoBarOpCreate BarCreate;			// Object that creates TextInfoBarOp objects

		 		ok = file.open(_R(IDM_TEXT_BAR), _R(IDT_INFO_BAR_RES));		// Open resource
		if (ok) ok = DialogBarOp::ReadBarsFromFile(file,BarCreate);	// Read and create info bar
		if (ok) file.close();									 	// Close resource

		ENSURE(ok,"Unable to load Textbar.ini from resource\n"); 

		if (ok)
		{
			// Info bar now exists.  Now get a pointer to it
			String_32 str = String_32(_R(IDS_TEXTTOOL_INFOBARNAME));
			DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(str);

					ok = (pDialogBarOp != NULL);
			if (ok) ok = pDialogBarOp->IsKindOf(CC_RUNTIME_CLASS(TextInfoBarOp));
			if (ok) pTextInfoBarOp = (TextInfoBarOp*)pDialogBarOp;

			ENSURE(ok,"Error finding the Text tool info bar");
		}
#endif
		pTextInfoBarOp = new TextInfoBarOp();
		ok = (pTextInfoBarOp != NULL);
		ENSURE(ok,"Error finding the Text tool info bar");
	}

	// Read preference settings
	GetApplication()->DeclareSection(_T("TextTool"),1);
	GetApplication()->DeclarePref(_T("TextTool"),_T("UseDeadKeys"), &UseDeadKeys, 0, 1);

	// Read deadkey virtual key codes for the current driver
	if (ok)
		ok = RegisterDeadKeys();

	return (ok);
}



/********************************************************************************************

>	void TextTool::Describe(void *InfoPtr)

	Author:		Mark_Goodall (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		InfoPtr - 	A pointer to a tool info block. It is passed cast to void* as
							the version of the tool is unknown at this point. Later versions 
							of the Tool class may have more items in this block, that this 
							tool will not use
	Outputs:	InfoPtr - 	The structure pointed to by InfoPtr will have had all the info
							that this version of the Tool knows about
	Purpose:	Allows the tool manager to extract information about the tool

********************************************************************************************/

void TextTool::Describe(void *InfoPtr)
{
	// Cast structure into the latest one we understand.
	ToolInfo_v1 *Info = (ToolInfo_v1 *) InfoPtr;

	Info->InfoVersion = 1;
	
	Info->InterfaceVersion = GetToolInterfaceVersion();  // You should always have this line.
		
	// These are all arbitrary at present.
	Info->Version = 1;
	Info->ID      = GetID();
	Info->TextID  = _R(IDS_TEXT_TOOL);

	Info->Family  = FamilyName;
	Info->Name    = ToolName;
	Info->Purpose = Purpose;
	Info->Author  = Author;

	Info->BubbleID = _R(IDBBL_TEXT_TOOLBOX);
	// Text tool is associated with the BaseTextClass attribute group
	Info->CurrentAttributeGroup	= CC_RUNTIME_CLASS(BaseTextClass); 
}

/********************************************************************************************

>	virtual void TextTool::SelectChange(BOOL isSelected)

	Author:		Mark_Goodall (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		isSelected	- TRUE  = tool has been selected
							- FALSE = tool has been deselected
	Outputs:	-
	Returns:	-
	Purpose:	Starts up and closes down the Text tool
	Errors:		Debug warning if creating the cursor fails.
	SeeAlso:	-

********************************************************************************************/
void TextTool::SelectChange(BOOL isSelected)
{
	BOOL ok = TRUE;

	if (isSelected)
		ok = OnToolSelect();
	else
		ok = OnToolDeselect();

	if (!ok)
		InformError();
}



/********************************************************************************************

>	BOOL TextTool::CreateCursors()

	Author:		Mark_Goodall (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if all the Text tool cursors have been successfully created
	Purpose:	Creates all the Text tool cursors
	SeeAlso:	-

********************************************************************************************/

BOOL TextTool::CreateCursors()
{
	// This tool has just been selected.  Create the cursors.
	pcNormalTextCursor = new Cursor(this, _R(IDC_TEXTTOOLBLOBCURSOR));
	pcBlobTextCursor = new Cursor(this, _R(IDC_TEXTTOOLCURSOR));
	pcIndentCursor = new Cursor(this, _R(IDC_TEXTTOOLINDENTCURSOR));
	pcBlankCursor = new Cursor(this, _R(IDC_TEXTTOOLBLANKCURSOR));

	if ( pcNormalTextCursor==NULL || !pcNormalTextCursor->IsValid() ||
		 pcBlobTextCursor==NULL || !pcBlobTextCursor->IsValid() ||
		 pcIndentCursor==NULL || !pcIndentCursor->IsValid() ||
		 pcBlankCursor==NULL || !pcBlankCursor->IsValid() )
	{
		DestroyCursors();
		return FALSE;
	}
	else
		return TRUE;
}



/********************************************************************************************

>	void TextTool::DestroyCursors()

	Author:		Mark_Goodall (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroys all the Text tool cursors
	SeeAlso:	-

********************************************************************************************/

void TextTool::DestroyCursors()
{
	if (pcNormalTextCursor != NULL) delete pcNormalTextCursor;
	if (pcBlobTextCursor != NULL) delete pcBlobTextCursor;
	if (pcIndentCursor != NULL) delete pcIndentCursor;
	if (pcBlankCursor != NULL) delete pcBlankCursor;
}




/********************************************************************************************

>	void TextTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )

	Author:		Mark_Goodall (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		PointerPos 	- 	The DocCoord of the point where the mouse button was clicked
				Click 		-	Describes the type of click that was detected. 
				ClickMods 	-	Indicates which buttons caused the click and which modifers were
								pressed at the same time
				pSpread 	- 	The spread in which the click happened
	Returns:	-
	Purpose:	To handle a Mouse Click event for the Text Tool.
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers; TextTool::HandleSingleClick;
				TextTool::HandleDoubleClick; TextTool::HandleDragClick

********************************************************************************************/
void TextTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,	Spread* pSpread )
{
	if (ClickMods.Menu) return;							// Don't do anything if the user clicked the Menu button

	if (IsBlankCursorUp)
		return;

	DocView* pDocView = DocView::GetSelected();
	if (pDocView == NULL)
	{
		ERROR3("No selected doc view!");
	}
	else
	{
		BOOL Success = TRUE;
	
		switch (Click)
		{
			case CLICKTYPE_SINGLE:
				if (pDocView->IsSingleClickReallyTriple())
					Success = HandleTripleClick(pSpread, PointerPos, ClickMods);
				else
					Success = HandleSingleClick(pSpread, PointerPos, ClickMods);
				break;
			case CLICKTYPE_DOUBLE:
				if (pDocView->IsSingleClickReallyQuad())
					Success = HandleQuadClick(pSpread, PointerPos, ClickMods);
				else
					Success = HandleDoubleClick(pSpread, PointerPos, ClickMods);
				break;
			case CLICKTYPE_DRAG:
				Success = HandleDragClick(pSpread, PointerPos, ClickMods);
				break;
			default:
				break;
		}

		if (!Success)
			InformError();
	}
}



/********************************************************************************************

>	void TextTool::HandleSingleClick(Spread* pSpread, DocCoord ClickPos, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/95
	Inputs:		pSpread - The spread in which the click occurred
				ClickPos - The DocCoord of the point where the mouse was clicked
				ClickMods - The state of the various modifiers at the time of the click
	Returns:	FALSE if an error occurs during processing
	Purpose:	To handle single click events in the text tool.
	SeeAlso:	TextTool::OnClick

********************************************************************************************/

BOOL TextTool::HandleSingleClick(Spread* pSpread, DocCoord ClickPos, ClickModifiers ClickMods)
{
	Node* pHitObject = NULL;
	TextStory* pTextStoryToSelect = NULL;

	// Is the click over an indent blob?
	BOOL BlobIsLeft = TRUE;
	TextStory* pFoundStory = NULL;
	if (!m_BlobPosList.FindBlobStory(pSpread, ClickPos, &BlobIsLeft, &pFoundStory))
		return FALSE;
	
	if (pFoundStory != NULL)
	{
		if (!m_BlobPosList.RenderAndRemove(pFoundStory))
			return FALSE;

		// Invoke a operation to drag the indent position
		OpDragStoryIndent* pOp = NULL;
		if (BlobIsLeft)
		{
			if (pFoundStory->GetTextPath() != NULL)
				pOp = new OpDragStoryPathLeftIndent;
			else
				pOp = new OpDragStoryNonPathLeftIndent;
		}
		else
		{
			if (pFoundStory->GetTextPath() != NULL)
				pOp = new OpDragStoryPathRightIndent;
			else
				pOp = new OpDragStoryNonPathRightIndent;
		}
		if (pOp==NULL)
			return FALSE;
		else
			return pOp->DoDrag(ClickPos, pSpread, ClickMods, pFoundStory);
	}
	else if ( IsPointNearUnselectedTextStory(pSpread, ClickPos, &pTextStoryToSelect, &pHitObject, FALSE) )
	{
		// Position the caret with a story, and drag selection
		OpTextSelection* pOp = new OpTextSelection;
		if (pOp == NULL)
			return FALSE;
		else
			return pOp->DoDrag(ClickPos, pSpread, ClickMods, pHitObject);
	}
	else
	{
		// Create a new text story
		OpCreateTextObject* pOpCreateTextObject = new OpCreateTextObject;
		if (pOpCreateTextObject == NULL)
			return FALSE;
		else
		{
			NodePath* pPath = NULL;
			IsPointNearNonStoryPath(pSpread, ClickPos, &pPath, FALSE);
			if (pPath != NULL)
				pOpCreateTextObject->DoImmediate(pSpread, ClickPos, pPath, ClickMods);
			else
				pOpCreateTextObject->DoDrag(pSpread, ClickPos, ClickMods);
		}
	}
	return TRUE;
}												



/********************************************************************************************

>	void TextTool::HandleDoubleClick(Spread* pSpread, DocCoord ClickPos, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/95
	Inputs:		pSpread - The spread in which the click occurred
				ClickPos - The DocCoord of the point where the mouse was clicked
				ClickMods - The state of the various modifiers at the time of the click
	Returns:	FALSE if an error occurs during processing
	Purpose:	To handle double click events in the text tool.
	SeeAlso:	TextTool::OnClick

********************************************************************************************/

BOOL TextTool::HandleDoubleClick(Spread* pSpread, DocCoord ClickPos, ClickModifiers ClickMods)
{
	BOOL Success = TRUE;
	// Select the word at the caret
	if ((TextStory::GetFocusStory() != NULL) && !ClickMods.Constrain && !ClickMods.Adjust)
	{
		OpTextCaret* pOp = new OpTextCaret();
		if (pOp != NULL)
			pOp->DoSelectWordAtCaret();
		else
			Success = FALSE;
	}
	return Success;
}



/********************************************************************************************
>	void TextTool::HandleTripleClick(Spread* pSpread, DocCoord ClickPos, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/95
	Inputs:		pSpread - The spread in which the click occurred
				ClickPos - The DocCoord of the point where the mouse was clicked
				ClickMods - The state of the various modifiers at the time of the click
	Returns:	FALSE if an error occurs during processing
	Purpose:	To handle triple click events in the text tool.
	SeeAlso:	TextTool::OnClick
********************************************************************************************/
BOOL TextTool::HandleTripleClick(Spread* pSpread, DocCoord ClickPos, ClickModifiers ClickMods)
{
	ERROR2IF(TextStory::GetFocusStory()==NULL, FALSE, "No focus story");
	ERROR2IF(TextStory::GetFocusStory()->GetCaret()==NULL, FALSE, "No caret in focus story");
	
	TextLine* pLine = (TextLine*)TextStory::GetFocusStory()->GetCaret()->FindParent(CC_RUNTIME_CLASS(TextLine));
	ERROR2IF(pLine==NULL, FALSE, "Caret didn't have a parent TextLine");

	OpTextSelection* pOp = new OpTextSelection();
	if (pOp != NULL)
		return pOp->DoSelectLineText();
	else
		return FALSE;
}



/********************************************************************************************
>	void TextTool::HandleQuadClick(Spread* pSpread, DocCoord ClickPos, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/95
	Inputs:		pSpread - The spread in which the click occurred
				ClickPos - The DocCoord of the point where the mouse was clicked
				ClickMods - The state of the various modifiers at the time of the click
	Returns:	FALSE if an error occurs during processing
	Purpose:	To handle quad click events in the text tool.
	SeeAlso:	TextTool::OnClick
********************************************************************************************/
BOOL TextTool::HandleQuadClick(Spread* pSpread, DocCoord ClickPos, ClickModifiers ClickMods)
{
	return TRUE;
}



/********************************************************************************************

>	void TextTool::HandleDragClick(Spread* pSpread, DocCoord ClickPos, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/95
	Inputs:		pSpread - The spread in which the click occurred
				ClickPos - The DocCoord of the point where the mouse was clicked
				ClickMods - The state of the various modifiers at the time of the click
	Returns:	FALSE if an error occurs during processing
	Purpose:	To handle drag events in the text tool.
	SeeAlso:	TextTool::OnClick

********************************************************************************************/

BOOL TextTool::HandleDragClick(Spread* pSpread, DocCoord ClickPos, ClickModifiers ClickMods)
{
	return TRUE;
}



/********************************************************************************************

>	void TextTool::IsPointNearUnselectedTextStory(	Spread* pSpread,
													DocCoord ClickPos,
													TextStory** pTS,
													Node** pHitObject,
													BOOL AllowInterrupts)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/02/95
	Inputs:		PointerPos 	- 	The DocCoord of the point where the mouse has moved to
				pSpread 	- 	The spread in which the move occurred
				AllowInterrupts - TRUE to stop the hit detection if the mouse moves
	Outputs:	pTS will point to the TextStory to select, or NULL
				pHitObject will point the the object under the pointer, or NULL
					(Set to NULL if not interested about objects)
	Returns:	TRUE if the pointer is over an text story.
				FALSE if not
	Purpose:	To see if a click at the current positon would select a TextStory
	SeeAlso:	TextTool::IsPointNearNonStoryPath

********************************************************************************************/

BOOL TextTool::IsPointNearUnselectedTextStory(Spread* pSpread, DocCoord ClickPos, TextStory** pTS, Node** pHitObject, BOOL AllowInterrupts)
{
	Node* pInterrupt;
	NodeRenderableInk* pObject = NULL;

	if (AllowInterrupts)
		pObject = NodeRenderableInk::FindSimpleAtPoint(pSpread, ClickPos, NULL, &pInterrupt);
	else
		pObject = NodeRenderableInk::FindSimpleAtPoint(pSpread, ClickPos);

	// The text tool has always ignored the compound node structure of the document
	// and allowed a text object to be selected inside any amount of nested nodes.
	// This is actually useful most of the time but we can't allow /anything/ to be
	// edited inside a locked effect because the changes will not be seen while
	// the locked effect's own bitmap is being rendered...
	NodeRenderableInk* pCompound = NodeRenderableInk::FindCompoundFromSimple(pObject);
	if (pCompound && pCompound->IsEffect() && ((NodeEffect*)pCompound)->IsLockedEffect())
		return FALSE;

	*pTS = NULL;
	if (pHitObject!=NULL)
		*pHitObject = NULL;

	// BODGE - lets assume the char is a child of the line of the story
	if (pObject != NULL)
	{
		if (pObject->IsAVisibleTextNode())
		{
			if (pHitObject!=NULL)
				*pHitObject = pObject;
			*pTS = (TextStory*)(pObject->FindParent()->FindParent());
			return TRUE;
		}
	}

	// We didn't find anything via hittesting.  Now scan the stories on this spread to see
	// if we are over blank areas of the story.
	Node* pNode = pSpread->FindFirstDepthFirst();
	while (pNode != NULL)
	{
		if (pNode->IsABaseTextClass() && IS_A(pNode, TextStory))
		{
			// Check to see if the point is within the bounds of the story
			TextStory* pStory = (TextStory*)pNode;
			DocRect StoryBounds = pStory->GetBoundingRect(TRUE);
			if (StoryBounds.ContainsCoord(ClickPos))
			{
				// Story must not be on a path and must be on an unlocked layer, and not in a mould
				Layer* pLayer = (Layer*)pStory->FindParent(CC_RUNTIME_CLASS(Layer));
				NodeMould* pMould = (NodeMould*)pStory->FindParent(CC_RUNTIME_CLASS(NodeMould));
				if (pLayer!=NULL && pMould==NULL && pStory->GetTextPath()==NULL && !pLayer->IsLocked())
				{
					// If the client wants the object then we have to do more work
					if (pHitObject!=NULL)
					{
						VisibleTextNode* pNearObject = NULL;
						BOOL ToLeft = FALSE;
						if (!pStory->GetCharacterAtPoint(TRUE, ClickPos, &pNearObject, &ToLeft))
						{
							InformError();
							return FALSE;
						}
						else
						{
							if (pNearObject != NULL)
							{
								*pHitObject = pNearObject;
								*pTS = pStory;
								return TRUE;
							}
						}
					}
					else
					{
						// Get untransformed story bounds and point
						DocRect UTStory = pStory->GetUTStoryBounds();
						DocCoord UTPoint = ClickPos;
						Matrix StoryMat = pStory->GetStoryMatrix();
						StoryMat=StoryMat.Inverse();
						StoryMat.transform(&UTPoint);

						if (UTStory.ContainsCoord(UTPoint))
						{
							*pTS=pStory;
							return TRUE;
						}
					}
				}
			}
		}
		pNode = pNode->FindNextDepthFirst(pSpread);
	}

	return FALSE;
}



/********************************************************************************************

>	void TextTool::IsPointNearNonStoryPath(	Spread* pSpread,
											DocCoord PointerPos,
											NodePath** pHitPath,
											BOOL AllowInterrupts)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/02/95
	Inputs:		pSpread 	- 	The spread in which the point is in
				PointerPos 	- 	The DocCoord of the point where the mouse has moved to
				AllowInterrupts - TRUE to stop the hit detection if the mouse moves
	Outputs:	pHitPath will point to the NodePath the pointer is over, or NULL
	Returns:	TRUE if the pointer is over an NodePath, FALSE if not
	Purpose:	To see if a click at the current positon should create a story on a path
	SeeAlso:	TextTool::IsPointNearUnselectedTextStory

********************************************************************************************/

BOOL TextTool::IsPointNearNonStoryPath(	Spread* pSpread, DocCoord PointerPos, NodePath** pHitPath, BOOL AllowInterrupts)
{
	Node* pInterrupt;
	NodeRenderableInk* pObject = NULL;
 	*pHitPath = NULL;

	if (AllowInterrupts)
		pObject = NodeRenderableInk::FindSimpleAtPoint(pSpread, PointerPos, NULL, &pInterrupt);
	else
		pObject = NodeRenderableInk::FindSimpleAtPoint(pSpread, PointerPos);

	// If its a NodePath that isn't already a story path
	if (pObject != NULL)
	{
		Node* pParent = pObject->FindParent();
		ObjChangeParam ChangeData(OBJCHANGE_STARTING, ObjChangeFlags(TRUE), pObject, NULL);
		if (IS_A(pObject, NodePath) && pObject->IsSelected() &&		/* It must be a selected path */
			(pParent != NULL) && !IS_A(pParent, TextStory) &&		/* It's not already in a story */
			pObject->AllowOp(&ChangeData, FALSE) )					/* And the path is happy with being deleted */
		{
			// Only allows clicks on the edge of the path (not the inside of filled shapes)
			NodePath* pPath = (NodePath*)pObject;
			INT32 PathSlot = 0;
			if (pPath->InkPath.PointCloseToLine(PointerPos, &PathSlot))
			{
				*pHitPath = (NodePath*)pObject;
				return TRUE;
			}
		}
	}

	return FALSE;
}																			      



/********************************************************************************************
>	void TextTool::OnMouseMove( DocCoord PointerPos,Spread* pSpread, ClickModifiers ClickMod )

	Author:		Mark_Goodall (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		PointerPos 	- 	The DocCoord of the point where the mouse has moved to
				pSpread 	- 	The spread in which the move occurred
				ClickMods	- 	The state of the various modifiers at the time of the mouse move
	Returns:	TRUE if it handled the Click, FALSE otherwise
	Purpose:	To handle a Mouse Move event for the Text Tool.
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers
********************************************************************************************/
void TextTool::OnMouseMove(DocCoord PointerPos, Spread* pSpread, ClickModifiers ClickMods)
{
	// Display status bar text for the current position
	DisplayStatusBarHelp(PointerPos, pSpread, ClickMods);
	IsBlankCursorUp = FALSE;

	// Cursor changing over characters
	NodePath* pPath = NULL;
	TextStory* pTextStoryToSelect = NULL;
	Cursor* DisplayCur = pcNormalTextCursor;

	// Are we over text to put a caret in OR near a path to click to start a story on?
	if ( IsPointNearUnselectedTextStory(pSpread, PointerPos, &pTextStoryToSelect, NULL, TRUE) ||
		IsPointNearNonStoryPath(pSpread, PointerPos, &pPath, TRUE) )
	{
		DisplayCur = pcBlobTextCursor;	
	}

	// See if we are near any story path indent blobs
	TextStory* pFoundStory = NULL;
	if (!m_BlobPosList.FindBlobStory(pSpread, PointerPos, NULL, &pFoundStory))
		InformError();
	else if (pFoundStory != NULL)
		DisplayCur = pcIndentCursor;

	// Display that cursor!
	if ( (DisplayCur != pcCurrentCursor) && (DisplayCur != NULL) )
	{
		pcCurrentCursor = DisplayCur;
		CursorStack::GSetTop(pcCurrentCursor, CurrentCursorID);
	}
}

void TextTool::GetRulerOrigin(Spread* pSpread, UserCoord *pOrigin)
{
	if (TextInfoBarOp::IsRulerOriginClaimed())
	{
		pOrigin->x = TextInfoBarOp::GetRulerOrigin();
	}
}

void TextTool::RenderRulerBlobs(RulerBase* pRuler, UserRect& UpdateRect, BOOL IsBackground)
{
	// we only draw onto the horizontal ruler and only if we have claimed it
	if (!TextInfoBarOp::IsRulerOriginClaimed()) return;

	// draw the highlighted background or the blobs
	if (IsBackground)
	{
		TextInfoBarOp::HighlightRulerSection(pRuler, UpdateRect);
	}
	else
	{
		TextInfoBarOp::RenderRulerBlobs(pRuler, UpdateRect);
	}
}

/********************************************************************************************

>   BOOL TextTool::OnRulerClick(UserCoord PointerPos, ClickType Click, ClickModifiers Mods,
								Spread* pSpread, RulerBase* pRuler)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	07/07/06
	Inputs:     PointerPos	- user coordinates of click on ruler (relative to origin set by tool)
				Click		- Type of click enum
				Mods		- Modifier flags struct
				pSpread		- pointer to spread upon which click occurred
				pRuler		- pointer to ruler which generated click
	Returns:    TRUE to claim the click
	Purpose:	Called when the user has clicked on the ruler and we are the current tool

********************************************************************************************/

BOOL TextTool::OnRulerClick(UserCoord PointerPos, ClickType Click, ClickModifiers Mods,
							Spread* pSpread, RulerBase* pRuler)
{
	if (!TextInfoBarOp::IsRulerOriginClaimed()) return FALSE;
	return TextInfoBarOp::OnRulerClick(PointerPos, Click, Mods, pSpread, pRuler);
}

/********************************************************************************************

>   BOOL TextTool::GetRulerStatusLineText(String_256* pText, UserCoord PointerPos,
										  Spread* pSpread, RulerBase* pRuler)

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	25/07/06
	Inputs:     PointerPos	- user coordinates of click on ruler (relative to origin set by tool)
				pSpread		- pointer to spread upon which click occurred
				pRuler		- pointer to ruler which generated click
	Outputs:    Status line text written to pText (if returning TRUE)
	Returns:    TRUE if the text has been set
	Purpose:    Allows the tool to set the status line text for the ruler

********************************************************************************************/

BOOL TextTool::GetRulerStatusLineText(String_256* pText, UserCoord PointerPos,
									  Spread* pSpread, RulerBase* pRuler)
{
	if (!TextInfoBarOp::IsRulerOriginClaimed()) return FALSE;
	return TextInfoBarOp::GetRulerStatusLineText(pText, PointerPos, pSpread, pRuler);
}

/********************************************************************************************

	>	BOOL TextTool::OnKeyPress(KeyPress* pKeyPress)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/02/95
	Inputs:		pKeyPress - pointer to a keypress object
	Returns:	TRUE if it handled the keypress, FALSE otherwise
	Purpose:	To handle a keypress event for the Text Tool.
	SeeAlso:	-

********************************************************************************************/

BOOL TextTool::OnKeyPress(KeyPress* pKeyPress)
{
	TRACEUSER( "wuerthne", _T("TextTool::OnKeyPress, IsChar=%d IsRelease=%d VirtKey=%d\n"),
			   pKeyPress->IsChar(), pKeyPress->IsRelease(), pKeyPress->GetVirtKey() );

	// NB - each keypress casues three events:
	//      (1) a key down event (IsChar() = FALSE, IsRelease() = FALSE) - which may not have the
	//          correct Unicode char value because it has not been through the IM yet
	//      (2) a char event (IsChar() = TRUE, IsRelease() = FALSE) - with the correct Unicode
	//          char but with different values for GetVirtKey (e.g., Ctrl+char is reported as
	//          a ctrl code)
	//      (3) a release event (IsChar() = FALSE, IsRelease() = TRUE)

	// Our strategy is to use the key down event for shortcuts, to use the char events for
	// actual input and to ignore the release events. Care must be taken to swallow enough
	// events, e.g., though cursor movement is handled using the key down event, the
	// corresponding char event must be swallowed, too, otherwise the reported Unicode
	// character ends up being inserted in the text object. Finally, all non-char events
	// we do not swallow here will be processed by the hotkey system.

   	// Filter out all key release events 
   	if (pKeyPress->IsRelease())
	{
		TRACEUSER( "wuerthne", _T("Release\n") );

		if (GetApplication()->FindSelection()->IsSelRangeGagged())
		{
			GetApplication()->FindSelection()->SetGag(FALSE);
			GetApplication()->FindSelection()->Update(TRUE);
		}
		return FALSE;
	}

	// Deal with keypresses that don't dosen't depend on a focus story
	if (HandleSpecialStoryAndNonStoryKeys(pKeyPress))
	{
		TRACEUSER( "wuerthne", _T("SpecialStoryAndNonStoryKeys\n") );

		return TRUE;
	}

	// See if a caret has been selected without updating the focus story
	if (TextStory::GetFocusStory() == NULL)
	{
		SelRange* pSelection = GetApplication()->FindSelection();		
		Node* pNode = pSelection->FindFirst();
		while (pNode != NULL)
		{
			if (IS_A(pNode,CaretNode))
			{
				TextStory::SetFocusStory((TextStory*)pNode->FindParent(CC_RUNTIME_CLASS(TextStory)));
				break;
			}
				
			pNode = pSelection->FindNext(pNode);
		}
	}

	// We need a focus story do handle a keypress
	if (TextStory::GetFocusStory() == NULL) 
	{
		TRACEUSER( "wuerthne", _T("GetFocusStory\n") );

		return HandleSpecialNonStoryKeys(pKeyPress);
	}

	// Just to be safe let's see if the TextStory has a selected caret or selected region
	if ( !(TextStory::GetFocusStory()->GetCaret()->IsSelected()) &&
		 (TextStory::GetFocusStory()->GetSelectionEnd() == NULL) )
	{
		TextStory::SetFocusStory(NULL);
		return HandleSpecialNonStoryKeys(pKeyPress);
	}

	// Now clear out all selections from other stories/objects
	SelRange* Selection = GetApplication()->FindSelection();
	Node* pNode = Selection->FindFirst();
	while (pNode != NULL)
	{
 		if (pNode->IsAnObject())
 		{
	 		if (!pNode->IsABaseTextClass())
				((NodeRenderableInk*)pNode)->DeSelect(TRUE);
			else
			{
				if (!IS_A(pNode,TextStory))
				{
					TextStory* pParentStory = (TextStory*)pNode->FindParent(CC_RUNTIME_CLASS(TextStory));
					if (pParentStory != TextStory::GetFocusStory())
						((NodeRenderableInk*)pNode)->DeSelect(TRUE);
				}
			}
		}

		pNode = Selection->FindNext(pNode);
	}
   	
   	// First see if this is a special meaning key
	if (HandleSpecialStoryKeys(pKeyPress, TextStory::GetFocusStory(), TextStory::GetFocusStory()->GetCaret()))
	{
		TRACEUSER( "wuerthne", _T("HandleSpecialStoryKeys\n") );

		return TRUE;
	}

	// finally, only proper text input remaining, so we only want Char events
	if( !pKeyPress->IsChar() )
	{
		TRACEUSER("wuerthne", _T("not char"));

		// We own all keys that don't have constrain or alt pressed and aren't extended chars
		if( !pKeyPress->IsConstrain() && !pKeyPress->IsAlternative() && !pKeyPress->IsExtended() )
			return true;

		return pKeyPress->GetUnicode() == _T(' ');          // always claim the Space key (tool switch)
	}

	// temporary fix: do not allow function keys to insert funny Unicode characters
	// (e.g., F8 to select text tool inserts an s circumflex character
	if (pKeyPress->GetVirtKey() >= CAMKEY(F1) && pKeyPress->GetVirtKey() <= CAMKEY(F12))
		return FALSE;

	if ( (!pKeyPress->IsAlternative()) ||				    // Alt not down
		 (pKeyPress->IsAlternative() && pKeyPress->IsExtended()) || // Right alt down
		 (pKeyPress->IsAlternative() && pKeyPress->IsConstrain()) ) // Ctrl & left alt down
	{
		WCHAR UnicodeValue = pKeyPress->GetUnicode();
		TRACEUSER("wuerthne", _T("UnicodeValue from keypress event = %04x %d"), UnicodeValue, pKeyPress->IsExtended() );

		// Extanded keys shouldn't produce a character
		if (HandleDeadKeys(pKeyPress, &UnicodeValue) || pKeyPress->IsExtended())
			return TRUE;
		else
		{
			if ( (UnicodeValue>=32) && ((UnicodeValue < CAMELOT_UNICODE_BASE) || (UnicodeValue > CAMELOT_UNICODE_LAST)))
			{
#ifndef EXCLUDE_FROM_XARALX
				if ((UnicodeValue < 256) /*&& !TextManager::IsUnicodeCompleteOS()*/)
					UnicodeValue = UnicodeManager::MultiByteToUnicode(UnicodeValue);
#endif
				
				// Display a blank cursor (thus hiding the pointer), but only if the pointer
				// is over the current document window
				if (!IsBlankCursorUp)
				{	
					DocView* pDocView = DocView::GetSelected();
					if (pDocView != NULL)
					{
						OilCoord DummyPos;
						CCamView* pCCamView = pDocView->GetConnectionToOilView();
						if (pCCamView && pCCamView->GetCurrentMousePos(&DummyPos))
						{
							// mouse is over document window
							pcCurrentCursor = pcBlankCursor;
							CursorStack::GSetTop(pcCurrentCursor, CurrentCursorID);
							IsBlankCursorUp = TRUE;
						}		
					}
				}
				
				// Create EditTextOp
				OpTextFormat* pOp = new OpTextFormat();
				if (pOp != NULL)
				{
					TRACEUSER("jlh92", _T("inserting Unicode char %04x"), UnicodeValue);
					pOp->DoInsertChar(UnicodeValue, OpTextFormat::INSERT);
					UpdateAfterTyping = TRUE;
					return TRUE;
				} 
			}
			else
				TRACEUSER( "jlh92", _T("Rejected\n" ) );
		}
	}
	TRACEUSER("wuerthne", _T("TextTool::OnKeyPress returns FALSE"));
	return FALSE;
}



/********************************************************************************************
>	BOOL TextTool::HandleSpecialStoryAndNonStoryKeys(KeyPress* pKeyPress,)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/06/95
	Inputs:		pKeyPress - pointer to a keypress object
	Returns:	TRUE if it handled the keypress, FALSE otherwise
	Purpose:	To handle keypress events that wotk with and without a text focus story
	SeeAlso:	TextTool::HandleSpecialStoryKeys, TextTool::HandleSpecialNonStoryKeys
********************************************************************************************/
BOOL TextTool::HandleSpecialStoryAndNonStoryKeys(KeyPress* pKeyPress)
{
	ERROR3IF(pKeyPress == NULL, "KeyPress pointer was NULL");
	if (pKeyPress == NULL)
		return FALSE;

	if (pKeyPress->IsChar()) return FALSE;      // only use non-char events for hotkeys

	BOOL UsedTheKeypress = FALSE;
	BOOL Errored = FALSE;

	switch (pKeyPress->GetVirtKey())
	{
 		case CAMKEY(B):
			// Apply/Remove bold to/from the selection
			if (pKeyPress->IsConstrain() && !pKeyPress->IsAdjust() )
			{
				UsedTheKeypress = TRUE;
				Errored = !ApplyBold();
			}
			break;
 		case CAMKEY(I):
			// Apply/Remove italic to/from the selection
			if (pKeyPress->IsConstrain() && !pKeyPress->IsAdjust() )
			{
				UsedTheKeypress = TRUE;
				Errored = !ApplyItalic();
			}
			break;
	}

	if (Errored)
		InformError();
	TRACEUSER("wuerthne", _T("HandleSpecialStoryAndNonStoryKeys returns %d"), UsedTheKeypress);
	return UsedTheKeypress;
}



/********************************************************************************************
>	BOOL TextTool::HandleSpecialNonStoryKeys(KeyPress* pKeyPress,)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/08/95
	Inputs:		pKeyPress - pointer to a keypress object
	Returns:	TRUE if it used the keypress, FALSE otherwise
	Purpose:	To deal with keys presses that work without a focus story.
	SeeAlso:	TextTool::HandleSpecialStoryKeys, TextTool::HandleSpecialStoryAndNonStoryKeys
********************************************************************************************/
BOOL TextTool::HandleSpecialNonStoryKeys(KeyPress* pKeyPress)
{
	ERROR3IF(pKeyPress == NULL, "KeyPress pointer was NULL");
	if (pKeyPress == NULL)
		return FALSE;

	BOOL UsedTheKeypress = FALSE;

	switch (pKeyPress->GetVirtKey())
	{
 		case CAMKEY(UP):
 		case CAMKEY(DOWN):
 		case CAMKEY(LEFT):
 		case CAMKEY(RIGHT):
			UsedTheKeypress = TRUE;		// Just eat the keypress
			break;
	}
	TRACEUSER("wuerthne", _T("HandleSpecialNonStoryKeys returns %d"), UsedTheKeypress);
	return UsedTheKeypress;
}



/********************************************************************************************
>	BOOL TextTool::HandleSpecialStoryKeys(KeyPress* pKeyPress, TextStory* pStory, CaretNode* pCaret)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/06/95
	Inputs:		pKeyPress - pointer to a keypress object
				pStory - points to the story with the caret
				pCaret - points to the caret
	Returns:	TRUE if it handled the keypress, FALSE otherwise
	Purpose:	To handle special keypress events that apply at a story and/or caret
	SeeAlso:	TextTool::HandleSpecialNonStoryKeys
********************************************************************************************/

BOOL TextTool::HandleSpecialStoryKeys(KeyPress* pKeyPress, TextStory* pStory, CaretNode* pCaret)
{
	TRACEUSER("wuerthne", _T("HandleSpecialStoryKeys %d"), pKeyPress->GetVirtKey());
	ERROR3IF(pKeyPress == NULL, "KeyPress pointer was NULL");
	ERROR3IF(pStory == NULL, "Story pointer was NULL");
	ERROR3IF(pCaret == NULL, "Caret pointer was NULL");
	if ((pKeyPress == NULL) || (pStory == NULL) || (pCaret == NULL))
		return FALSE;

	BOOL IsNonCharEvent = !pKeyPress->IsChar(); // only use non-char events for hotkeys, but eat char events matching our hotkeys
	BOOL UsedTheKeypress = FALSE;
	BOOL Errored = FALSE;

	switch (pKeyPress->GetVirtKey())
	{
		case CAMKEY(HOME):
	 		// Move caret to start of line
			if (IsNonCharEvent)
	 		{
				OpTextCaret* pOp = new OpTextCaret();
				if (pOp != NULL)
					pOp->DoMoveCaretHome(pKeyPress->IsAdjust(), pKeyPress->IsConstrain()); 
				else
					Errored = TRUE;
			}
			UsedTheKeypress = TRUE;
			break;
		case CAMKEY(END):
	 		// Move caret to end of line
			if (IsNonCharEvent)
	 		{
				OpTextCaret* pOp = new OpTextCaret();
				if (pOp != NULL)
					pOp->DoMoveCaretEnd(pKeyPress->IsAdjust(), pKeyPress->IsConstrain()); 
				else
					Errored = TRUE;
			}
			UsedTheKeypress = TRUE;
			break;
		case CAMKEY(BACK):
			// delete character before caret, or selected chars if either exists
			if (IsNonCharEvent && (pStory->GetSelectionEnd() || pCaret->FindPrevVTNInStory() != NULL))
	 		{
				OpTextFormat* pOp = new OpTextFormat();
				if (pOp != NULL)
					pOp->DoDeleteChar(OpTextFormat::BACKSPACE); 
				else
					Errored = TRUE;
			}
			UsedTheKeypress = TRUE;
			break;
 		case CAMKEY(DELETE):
			// delete character before caret, or selected chars if either exists
			if (IsNonCharEvent && (pStory->GetSelectionEnd() || pCaret->FindNextVTNInStory() != pStory->FindLastVTN()))
	 		{
				OpTextFormat* pOp = new OpTextFormat();
				if (pOp != NULL)
					pOp->DoDeleteChar(OpTextFormat::DEL); 
				else
					Errored = TRUE;
			}
			UsedTheKeypress = TRUE;
			break;
 		case CAMKEY(LEFT):
			// Move/Select caret left one word/character
			if (IsNonCharEvent)
			{
				TRACEUSER("wuerthne", _T("caret left"));
				OpTextCaret* pOp = new OpTextCaret();
				if (pOp != NULL)
					pOp->DoMoveCaretLeft(pKeyPress->IsAdjust(), pKeyPress->IsConstrain());
				else
					Errored = TRUE;
			}
			UsedTheKeypress = TRUE;
			break;
 		case CAMKEY(RIGHT):
			// Move/Select caret right one word/character
			if (IsNonCharEvent)
			{
				OpTextCaret* pOp = new OpTextCaret();
				if (pOp != NULL)
					pOp->DoMoveCaretRight(pKeyPress->IsAdjust(), pKeyPress->IsConstrain());
				else
					Errored = TRUE;
			}
			UsedTheKeypress = TRUE;
			break;
 		case CAMKEY(UP):
			// Move/Select caret up a line
			if (IsNonCharEvent)
			{
				OpTextCaret* pOp = new OpTextCaret();
				if (pOp != NULL)
					pOp->DoMoveCaretUp(pKeyPress->IsAdjust());
				else
					Errored = TRUE;
			}
			UsedTheKeypress = TRUE;
			break;
 		case CAMKEY(DOWN):
			// Move/Select caret down a line
			if (IsNonCharEvent)			
			{
				OpTextCaret* pOp = new OpTextCaret();
				if (pOp != NULL)
					pOp->DoMoveCaretDown(pKeyPress->IsAdjust());
				else
					Errored = TRUE;
			}
			UsedTheKeypress = TRUE;
			break;
 		case CAMKEY(W):
			// Swap case
			if (pKeyPress->IsConstrain() && !pKeyPress->IsAdjust() )
			{
				if (IsNonCharEvent && TextStory::GetFocusStory()->GetCaret()->FindNextTextCharInStory() != NULL)
				{
					OpTextFormat* pOp = new OpTextFormat();
					if (pOp != NULL)
					{
						pOp->DoSwapCase();
					}
					else
						Errored = TRUE;
				}
				UsedTheKeypress = TRUE;
			}
			break;
 		case CAMKEY(A):
			// Select all in the focus story, if there is one
			if (pKeyPress->IsConstrain() && !pKeyPress->IsAdjust())
			{
				if (IsNonCharEvent)
				{
					OpTextSelection* pOp = new OpTextSelection();
					if (pOp != NULL)
						Errored = !pOp->DoSelectAllText(TextStory::GetFocusStory());
					else
						Errored = TRUE;
				}
				UsedTheKeypress = TRUE;
			}
			break;

 		case CAMKEY(V):
			// Paste text from the clipboard into a selected text story
			if (pKeyPress->IsConstrain() && !pKeyPress->IsAdjust())
			{
				if (IsNonCharEvent)
				{
					// is there a focus story?
					OpTextPaste* pOp = new OpTextPaste();
					if (pOp != NULL)
						pOp->Do(NULL);
				}
				UsedTheKeypress = TRUE;
			}
			break;

 		case CAMKEY(L):
			// Select all text on the line with the caret
			if (pKeyPress->IsConstrain() && !pKeyPress->IsAdjust())
			{
				TextLine* pLine = (TextLine*)TextStory::GetFocusStory()->GetCaret()->FindParent(CC_RUNTIME_CLASS(TextLine));
				if (pLine != NULL)
				{
					if (IsNonCharEvent)
					{
						OpTextSelection* pOp = new OpTextSelection();
						if (pOp != NULL)
							Errored = !pOp->DoSelectLineText();
						else
							Errored = TRUE;
					}
					UsedTheKeypress = TRUE;
				}
			}
			break;
		case CAMKEY(EQUALS):
			// Increase kerning/tracking
			if (pKeyPress->IsConstrain() && !pKeyPress->IsAdjust() )
			{
				UsedTheKeypress = TRUE;
				Errored = IsNonCharEvent ? !IncreaseTrackKern() : FALSE;
			}
			break;
 		case CAMKEY(MINUS):
			// Decrease kerning/tracking
			if (pKeyPress->IsConstrain() && !pKeyPress->IsAdjust() )
			{
				UsedTheKeypress = TRUE;
				Errored = IsNonCharEvent ? !DecreaseTrackKern() : FALSE;
			}
			break;
		case CAMKEY(RETURN):
			// Create a new TextLine and move the caret to it.
			if (!pKeyPress->IsAlternative() && !pKeyPress->IsConstrain())
			{
				if (IsNonCharEvent)
				{
					OpTextFormat* pOp = new OpTextFormat();
					if (pOp != NULL)
						Errored = !pOp->DoReturn(OpTextFormat::INSERT);
					else
						Errored = TRUE;
				}
				UsedTheKeypress = TRUE;
			}
			break;
		case CAMKEY(TAB):
			if (!pKeyPress->IsAlternative() && !pKeyPress->IsConstrain())
			{
				if (IsNonCharEvent)
				{
					/* Create a tab node */
					OpTextFormat* pOp = new OpTextFormat();
					if (pOp != NULL)
						Errored = !pOp->DoTab();
					else
						Errored = TRUE;
				}
				UsedTheKeypress = TRUE;
			}
			break;
		case CAMKEY(ESCAPE):
			// Deselect the caret and select the text story
			if (!pKeyPress->IsAlternative() && !pKeyPress->IsConstrain())
			{
				if (IsNonCharEvent)
				{
					pCaret->DeSelect(TRUE);
					pStory->Select(TRUE);
					GetApplication()->FindSelection()->Update(TRUE);
				}
				UsedTheKeypress = TRUE;

				// Remove the focus story if it was empty
				Errored = IsNonCharEvent ? !OpDeleteTextStory::RemoveEmptyFocusStory() : FALSE;
			}
			break;
	}

	if (Errored)
		InformError();

	return UsedTheKeypress;
}



/********************************************************************************************

>	void TextTool::DisplayStatusBarHelp(DocCoord DocPos, Spread* pSpread, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		DocPos - the document coordinate of the point to display help on
				pSpread - pointer to the spread containing DocPos
				ClickMods - the current click modifiers
	Outputs:	-
	Returns:	-
	Purpose:	Displays status help string for the given position in the status bar.
	SeeAlso:	TextTool::GetCurrentStatusText

********************************************************************************************/

void TextTool::DisplayStatusBarHelp(DocCoord DocPos, Spread* pSpread, ClickModifiers ClickMods)
{
	String_256 StatusMsg("");

	// Get a string from the underlying help function and display it.
	GetCurrentStatusText(&StatusMsg, pSpread, DocPos, ClickMods);
	GetApplication()->UpdateStatusBarText(&StatusMsg);							 
}



/********************************************************************************************

>	virtual BOOL TextTool::GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		ptest - pointer to a string
				pSpread points to a spread
				DocPos points to a point in a document
				ClickMods are the current click modifiers
	Outputs:	Updates the string in ptext
	Returns:	TRUE if the string was updates (FALSE if not updated)
	Purpose:	Returns the current status line help string
	SeeAlso:	TextTool::GetCurrentStatusText, Tool_v1::GetStatusLineText

********************************************************************************************/

BOOL TextTool::GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)
{
	// We can call the underlying help function to get a string and return the result.
	GetCurrentStatusText(ptext, pSpread, DocPos, ClickMods);
	return TRUE;
}



/********************************************************************************************

>	void TextTool::GetCurrentStatusText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		pSpread points to a spread
				DocPos points to a point in a document
				ClickMods are the current click modifiers
	Outputs:	Updates the string in ptext
	Returns:	-
	Purpose:	Selects a suitable string for the status line based on the current location
				(as input via the parameters).
	SeeAlso:	TextTool::GetStatusLineText, TextTool::DisplayStatusBarHelp

********************************************************************************************/

void TextTool::GetCurrentStatusText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)
{
	// If nothing is detected, we are over the document
	INT32 IDToUse = _R(IDS_TEXTTOOL_OVERDOCUMENT);

	// Is the caret in the focus story selected (ie, can the user type?)
	if (TextStory::GetFocusStory()!=NULL && TextStory::GetFocusStory()->GetCaret()!=NULL
			&& TextStory::GetFocusStory()->GetCaret()->IsSelected())
	{
		IDToUse = _R(IDS_TEXTTOOL_OVERDOCUMENTC);
	}
	
	// Are we over a story or a path for a new story?
	TextStory* pTextStoryToSelect = NULL;
	if (IsPointNearUnselectedTextStory(pSpread, DocPos, &pTextStoryToSelect, NULL, TRUE))
	{
		if (pTextStoryToSelect != NULL)
			IDToUse = _R(IDS_TEXTTOOL_OVERTEXTSTORY);
	}
	else
	{
		NodePath* pPath = NULL;
		if (IsPointNearNonStoryPath(pSpread, DocPos, &pPath, TRUE))
			IDToUse = _R(IDS_TEXTTOOL_OVERNODEPATH);
	}

	// See if we are near any story path indent blobs
	BOOL LeftBlob = TRUE;
	TextStory* pFoundStory = NULL;
	if (!m_BlobPosList.FindBlobStory(pSpread, DocPos, &LeftBlob, &pFoundStory))
		InformError();
	else if (pFoundStory != NULL)
	{
		if (pFoundStory->GetTextPath() != NULL)
			IDToUse = _R(IDS_TEXTTOOL_DRAGPATHINDENT);
		else
			IDToUse = _R(IDS_TEXTTOOL_DRAGSTORYWIDTH);
	}

	ptext->Load(IDToUse);
}



/********************************************************************************************
>	BOOL TextTool::ApplyBold()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/04/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Applies or removes the bold attribute from the current selection or caret.

********************************************************************************************/
BOOL TextTool::ApplyBold()
{
	AttrTxtBold* ApplyBoldAttrib = new AttrTxtBold();
	BOOL Success = ApplyBoldAttrib != NULL;

	if (Success)
	{
		// See wether bold is currently on or off across the selection
		AttrTxtBold* CurrentBoldAttrib = NULL;
		SelRange::CommonAttribResult AttrRes = GetApplication()->FindSelection()->
					FindCommonAttribute(CC_RUNTIME_CLASS(AttrTxtBold), (NodeAttribute **)&CurrentBoldAttrib);
		if (AttrRes == SelRange::ATTR_COMMON)
			ApplyBoldAttrib->Value.BoldOn = !(CurrentBoldAttrib->Value.BoldOn);
		else
			ApplyBoldAttrib->Value.BoldOn = TRUE;

		// Apply the new bold attribute to the selection
		AttributeManager::AttributeSelected(ApplyBoldAttrib, NULL);
	}

	return Success;
}



/********************************************************************************************
>	BOOL TextTool::ApplyItalic()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/04/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Applies or removes the italic attribute from the current selection or caret.

********************************************************************************************/
BOOL TextTool::ApplyItalic()
{
	AttrTxtItalic* ApplyItalicAttrib = new AttrTxtItalic();
	BOOL Success = ApplyItalicAttrib != NULL;

	if (Success)
	{
		// See wether italic is currently on or off across the selection
		AttrTxtItalic* CurrentItalicAttrib = NULL;
		SelRange::CommonAttribResult AttrRes = GetApplication()->FindSelection()->
					FindCommonAttribute(CC_RUNTIME_CLASS(AttrTxtItalic), (NodeAttribute **)&CurrentItalicAttrib);
		if (AttrRes == SelRange::ATTR_COMMON)
			ApplyItalicAttrib->Value.ItalicOn = !(CurrentItalicAttrib->Value.ItalicOn);
		else
			ApplyItalicAttrib->Value.ItalicOn = TRUE;

		// Apply the new italic attribute to the selection
		AttributeManager::AttributeSelected(ApplyItalicAttrib, NULL);
	}

	return Success;
}



/********************************************************************************************
>	BOOL TextTool::IncreaseTrackKern()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/04/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	If there is a text selection then the tracking applied to it is increased by
				10/1000's of an em
				If there is no selection then a kern up of 50/1000 is inserted	

********************************************************************************************/
BOOL TextTool::IncreaseTrackKern()
{
	// Get pointers to focus story and its caret
	TextStory* pStory = TextStory::GetFocusStory();
	ERROR3IF(pStory == NULL, "IncreaseTrackKern called without a focus story!");
	BOOL Success = TRUE;

	if (pStory != NULL)
	{
		if (pStory->GetSelectionEnd() == NULL)
			Success = CommonApplyKern(50);
		else
			Success = CommonApplyTracking(10);
	}

	return Success;
}



/********************************************************************************************
>	BOOL TextTool::DecreaseTrackKern()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/04/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	If there is a text selection then the tracking applied to it is decreased by
				10/1000's of an em
				If there is no selection then a kern down of 50/1000 is inserted	

********************************************************************************************/
BOOL TextTool::DecreaseTrackKern()
{
	// Get pointers to focus story and its caret
	TextStory* pStory = TextStory::GetFocusStory();
	ERROR3IF(pStory == NULL, "DecreaseTrackKern called without a focus story!");
	BOOL Success = TRUE;

	if (pStory != NULL)
	{
		if (pStory->GetSelectionEnd() == NULL)
			Success = CommonApplyKern(-50);
		else
			Success = CommonApplyTracking(-10);
	}

	return Success;
}



/********************************************************************************************
>	BOOL TextTool::CommonApplyTracking(MILLIPOINT ChangeAmmount)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/04/95
	Inputs:		ChangeAmmount - ammount to increase/decrease the tracking by
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Applys/increases/descreases the tracking across the selected characters	

********************************************************************************************/
BOOL TextTool::CommonApplyTracking(MILLIPOINT ChangeAmmount)
{
	// Create a new attribute to apply (will replace any existing)
	AttrTxtTracking* ApplyTrackingAttrib = new AttrTxtTracking();
	BOOL Success = (ApplyTrackingAttrib != NULL);

	if (Success)
	{
		// Get the current tracking ammount across the selection
		AttrTxtTracking* CurrentTrackingAttrib = NULL;
		SelRange::CommonAttribResult AttrRes = GetApplication()->FindSelection()->
					FindCommonAttribute(CC_RUNTIME_CLASS(AttrTxtTracking), (NodeAttribute **)&CurrentTrackingAttrib);
		if (AttrRes == SelRange::ATTR_COMMON)
			ApplyTrackingAttrib->Value.Tracking = CurrentTrackingAttrib->Value.Tracking + ChangeAmmount;
		else
			ApplyTrackingAttrib->Value.Tracking = ChangeAmmount;

		// Apply the increased attribute to the selection
		AttributeManager::AttributeSelected(ApplyTrackingAttrib, NULL);
	}

	return Success;
}



/********************************************************************************************
>	BOOL TextTool::CommonApplyKern(MILLIPOINT ChangeAmmount)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/04/95
	Inputs:		ChangeAmmount - ammount to increase/decrease the kern at the caret by
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Applys/increases/descreases the horizontal kern at the caret
********************************************************************************************/

BOOL TextTool::CommonApplyKern(MILLIPOINT ChangeAmmount)
{
	MILLIPOINT NewKernVal = ChangeAmmount;

	// Get the current kern ammount - check the node to the left of the caret
	TextStory* pStory = TextStory::GetFocusStory();
	CaretNode* pCaret = NULL;
	if (pStory != NULL)
		pCaret = pStory->GetCaret();
	if (pCaret != NULL)
	{
		VisibleTextNode* LastNode = pCaret->FindPrevAbstractTextCharInStory();
		if ((LastNode != NULL) && IS_A(LastNode, KernCode))
	 	   	NewKernVal += (((KernCode*)LastNode)->GetValue()).x;
	}

	// Invoke an operation to apply the kern
	OpDescriptor* OpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpTextKern));
	if (OpDesc != NULL)
	{
		OpParam param(NewKernVal,0);
		OpDesc->Invoke(&param);
	}

	return TRUE;
}



/********************************************************************************************
>	BOOL TextTool::OnIdle()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/08/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for more/no more idles needed
	Purpose:	Called on idle events.  Used to update the infobar kern field after typing
********************************************************************************************/

BOOL TextTool::OnIdle()
{
	if (UpdateAfterTyping)
	{
		if (!pTextInfoBarOp->UpdateFieldsAfterTyping())
			InformError();
		UpdateAfterTyping = FALSE;
	}

	return FALSE;
}



/********************************************************************************************
>	static BOOL TextTool::DelayingUpdate()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/9/96
	Returns:	TRUE if the text tool is saving up any updates to do when it is idle, FALSE
				if it isn't or if the text tool is not the current tool.
	Purpose:	Let's the caller know if the text-tool has finished mucking about with
				the document's modified flag.
	SeeAlso:	CCamSrvrItem::QueryTypeAhead
********************************************************************************************/

BOOL TextTool::DelayingUpdate()
{
	// Is the text tool the current tool?
	Tool* pTool = Tool::GetCurrent();
	if (!pTool || pTool->GetID() != TOOLID_TEXT) return FALSE;

	// Is it saving up some updates?
	return ((TextTool*) pTool)->UpdateAfterTyping;
}



/********************************************************************************************
>	BOOL TextTool::RegisterDeadKeys()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Registers all the dead keys virtual codes with the keypress system
	SeeAlso:	TextTool::RemoveDeadKeys
********************************************************************************************/
BOOL TextTool::RegisterDeadKeys()
{
	BOOL ok = TRUE;
	
	if (UseDeadKeys)
	{
		if (ok)
			ok = RegisterAdditionalVirtKey(&GraveVirtKey, (TCHAR)('`'));
		if (ok)
			ok = RegisterAdditionalVirtKey(&AcuteVirtKey, (TCHAR)0x27);	// Single Quote
		if (ok)
			ok = RegisterAdditionalVirtKey(&TildeVirtKey, (TCHAR)('~'));
		if (ok)
			ok = RegisterAdditionalVirtKey(&HatVirtKey, (TCHAR)('^'));
		if (ok)
			ok = RegisterAdditionalVirtKey(&ColonVirtKey, (TCHAR)(':'));
		if (ok)
			ok = RegisterAdditionalVirtKey(&AtVirtKey, (TCHAR)('@'));
		if (ok)
			ok = RegisterAdditionalVirtKey(&CommaVirtKey, (TCHAR)(','));
		if (ok)
			ok = RegisterAdditionalVirtKey(&SlashVirtKey, (TCHAR)('/'));
	}

	return ok;
}



/********************************************************************************************
>	BOOL TextTool::RemoveDeadKeys()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE
	Purpose:	Deregisters all the dead keys
	SeeAlso:	TextTool::RegisterDeadKeys
********************************************************************************************/
BOOL TextTool::RemoveDeadKeys()
{
	if (GraveVirtKey != NULL)
	{
		KeyPress::RemoveVirtualKeyCode(GraveVirtKey->GetVirtKey(), GraveVirtKey->GetUnicode());
		delete GraveVirtKey;
	}
	if (AcuteVirtKey != NULL)
	{
		KeyPress::RemoveVirtualKeyCode(AcuteVirtKey->GetVirtKey(), AcuteVirtKey->GetUnicode());
		delete AcuteVirtKey;
	}
	if (TildeVirtKey != NULL)
	{
		KeyPress::RemoveVirtualKeyCode(TildeVirtKey->GetVirtKey(), TildeVirtKey->GetUnicode());
		delete TildeVirtKey;
	}
	if (HatVirtKey != NULL)
	{
		KeyPress::RemoveVirtualKeyCode(HatVirtKey->GetVirtKey(), HatVirtKey->GetUnicode());
		delete HatVirtKey;
	}
	if (ColonVirtKey != NULL)
	{
		KeyPress::RemoveVirtualKeyCode(ColonVirtKey->GetVirtKey(), ColonVirtKey->GetUnicode());
		delete ColonVirtKey;
	}
	if (AtVirtKey != NULL)
	{
		KeyPress::RemoveVirtualKeyCode(AtVirtKey->GetVirtKey(), AtVirtKey->GetUnicode());
		delete AtVirtKey;
	}
	if (CommaVirtKey != NULL)
	{
		KeyPress::RemoveVirtualKeyCode(CommaVirtKey->GetVirtKey(), CommaVirtKey->GetUnicode());
		delete CommaVirtKey;
	}
	if (SlashVirtKey != NULL)
	{
		KeyPress::RemoveVirtualKeyCode(SlashVirtKey->GetVirtKey(), SlashVirtKey->GetUnicode());
		delete SlashVirtKey;
	}

	return TRUE;
}



/********************************************************************************************

>	BOOL TextTool::RegisterAdditionalVirtKey(KeyPress** pKey, TCHAR ch)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Inputs:		pKey - pointer to a KeyPress object
				ch - Unicode value of the required Virtual Key
	Outputs:	pKey is the KeyPress object representing the virtual key
	Purpose:	Registers a requirement for a virtual key message with the KeyPress system

********************************************************************************************/

BOOL TextTool::RegisterAdditionalVirtKey(KeyPress** pKey, TCHAR ch)
{
	BOOL ok = TRUE;

	*pKey = KeyPress::ConstructVK(ch);
	ok = (*pKey != NULL);
	if (ok)
		ok = KeyPress::AddVirtualKeyCode((*pKey)->GetVirtKey(), (*pKey)->GetUnicode());

	return ok;
}


/********************************************************************************************
>	BOOL TextTool::HandleDeadKeys(KeyPress* pKeyPress, WCHAR* pNewUnicode)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/95
	Inputs:		pKeyPress - points to the keypress object
	Outputs:	pNewUnicode contains the unicode value of the keypress, possibly changed to
				an accented version of the pressed key.
	Returns:	TRUE if keypress used, FALSE if not
	Purpose:	Flags 'dead-keys'.  Attempts to apply an accent onto the next character.
********************************************************************************************/
BOOL TextTool::HandleDeadKeys(KeyPress* pKeyPress, WCHAR* pNewUnicode)
{
	BOOL UsedKey = FALSE;
	*pNewUnicode = pKeyPress->GetUnicode();
//	WCHAR OldUnicode = *pNewUnicode;

	if (!UseDeadKeys)
		return FALSE;

	// Is this a dead key?
	if ( TestForDeadKey(pKeyPress, GraveVirtKey)	||
		 TestForDeadKey(pKeyPress, AcuteVirtKey)	||
		 TestForDeadKey(pKeyPress, HatVirtKey)		||
		 TestForDeadKey(pKeyPress, TildeVirtKey)	||
		 TestForDeadKey(pKeyPress, ColonVirtKey) 	||
		 TestForDeadKey(pKeyPress, AtVirtKey) 		||
		 TestForDeadKey(pKeyPress, SlashVirtKey) 	||
		 TestForDeadKey(pKeyPress, CommaVirtKey) )
	{
		UsedKey = TRUE;
	}

	// This isn't a dead key, was the previous?
	if (!UsedKey && (PreviousDeadKey != NULL) && IsUsableUnicode(pKeyPress->GetUnicode()) )
	{
		// Get a pointer to the appropiate accent map
		UINT32* pMapArrays = NULL;
		if (PreviousDeadKey == GraveVirtKey)
			pMapArrays = GraveArray;
		else if (PreviousDeadKey == AcuteVirtKey)
			pMapArrays = AcuteArray;
		else if (PreviousDeadKey == HatVirtKey)
			pMapArrays = HatArray;
		else if (PreviousDeadKey == TildeVirtKey)
			pMapArrays = TildeArray;
		else if (PreviousDeadKey == ColonVirtKey)
			pMapArrays = ColonArray;
		else if (PreviousDeadKey == AtVirtKey)
			pMapArrays = AtArray;
		else if (PreviousDeadKey == SlashVirtKey)
			pMapArrays = SlashArray;
		else if (PreviousDeadKey == CommaVirtKey)
			pMapArrays = CommaArray;
		else
		{
			ERROR3("Unknown dead key");
			pMapArrays = NULL;
		}

		// See if the pressed key maps onto an accented version
		if (pMapArrays != NULL)
		{
			BOOL Found = FALSE;
			while (!Found && (*pMapArrays != 0))
			{
				if (*pMapArrays == (UINT32)(*pNewUnicode))
				{									  
					*pNewUnicode = *(pMapArrays+1);
					Found = TRUE;
				}
				pMapArrays += 2;
			}
		}

		PreviousDeadKey = 0;
		UsedKey = FALSE;
	}

	return UsedKey;
}



/********************************************************************************************
>	BOOL TextTool::TestForDeadKey(KeyPress* pKeyPress, KeyPress* pDeadKeyPress)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Inputs:		pKeyPress - points to the current keypress object
				pDeadKeyPress - points to a know dead key to test against
	Outputs:	
	Returns:	TRUE if keypress used, FALSE if not
	Purpose:	Flags 'dead-keys'.  Attempts to apply an accent onto the next character.
********************************************************************************************/
BOOL TextTool::TestForDeadKey(KeyPress* pKeyPress, KeyPress* pDeadKeyPress)
{
	if (	pDeadKeyPress != NULL && 
			pDeadKeyPress->GetVirtKey() != CAMKEY(CC_NONE) &&
			!pDeadKeyPress->IsConstrain() &&
			pKeyPress->IsConstrain() &&
			pDeadKeyPress->IsAdjust() == pKeyPress->IsAdjust() &&
			pDeadKeyPress->IsAlternative() == pKeyPress->IsAlternative() &&
			pDeadKeyPress->GetVirtKey() == pKeyPress->GetVirtKey()
		)
	{
		PreviousDeadKey = pDeadKeyPress;
		return TRUE;
	}
	else
		return FALSE;
}



/********************************************************************************************
>	BOOL TextTool::IsUsableUnicode(WCHAR CodeValue)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/95
	Inputs:		CodeValue - a number representing a unicode character
	Outputs:	-
	Returns:	TRUE if the text tool accepts the character, FALSE if it dosen't
	Purpose:	Central place to filter out unicode greater than 255.
********************************************************************************************/
BOOL TextTool::IsUsableUnicode(WCHAR CodeValue)
{
	return ( ((CodeValue>=32) && (CodeValue<=126)) || ((CodeValue>=128) && (CodeValue<=255)) );
}



/********************************************************************************************
>	void TextTool::LocaleChanged()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the machines locale changes.  Re-registers the dead keys as their
				virtual key code may have changed.
********************************************************************************************/
void TextTool::LocaleChanged()
{
	RemoveDeadKeys();
	if (!RegisterDeadKeys())
		InformError();
}



/********************************************************************************************
>	BOOL TextTool::OnToolSelect()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if an error occured, TRUE if tool selected ok
	Purpose:	Called to do processing required when text tool is selected
********************************************************************************************/
BOOL TextTool::OnToolSelect()
{
	BOOL ok = TRUE;
	
	// Create the tools cursors
	ok = CreateCursors();
	if (ok)
	{
		CurrentCursorID = CursorStack::GPush(pcNormalTextCursor, FALSE);
		pcCurrentCursor = pcNormalTextCursor;
		IsBlankCursorUp = FALSE;
	}
	else
		pcCurrentCursor = NULL;

	// This tool is now the current one
	CurrentTool = TRUE;
	UpdateAfterTyping = FALSE;
	PreviousDeadKey = 0;

	// Say which blobs should be displayed
	BlobManager* BlobMgr = GetApplication()->GetBlobManager();
	if (BlobMgr != NULL)
	{
		// Decide which blobs we will display
		BlobStyle MyBlobs;
		MyBlobs.Object = TRUE;
		
		// Tell the blob manager
		BlobMgr->ToolInterest(MyBlobs);
	}

	// Render on the tool blobs
	if (ok)
		ok = m_BlobPosList.BuildFromSelection();
	if (ok && m_BlobPosList.GetBlobSpread()!=NULL)
		BlobMgr->RenderToolBlobsOn(this, m_BlobPosList.GetBlobSpread(), NULL);
	
	// Create and display the tool's info bar
	if (ok)
		ok = pTextInfoBarOp->Create();
	pTextInfoBarOp->pTextTool = this;

	// Alias the paste operation
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_PASTE);  
	pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpTextPaste),NULL,0);

	// Ensure the dead key mappings are correct
	LocaleChanged();

	// Give the input focus to the story with a subselection, as long as it is the only sub-selected story
	SelRange* pSelRange = GetApplication()->FindSelection();
	Node* pSel = pSelRange->FindFirst();
//	BOOL SelectedVTN = FALSE;
	TextStory* pSelStory = NULL;
	while (pSel != NULL)
	{
		// if it's a text object get the story it is in
		TextStory* pParentStory = NULL;
		if (pSel->IsABaseTextClass())
		{
			if (IS_A(pSel,TextStory))
				pParentStory = (TextStory*)pSel;
			else				
				pParentStory = (TextStory*)pSel->FindParent(CC_RUNTIME_CLASS(TextStory));
		}

		if (pSelStory==NULL)
		{
			// Set the selected story pointer
			if (pSel->IsAVisibleTextNode())
				pSelStory = pParentStory;
		}
		else
		{
			// See if we have found two selected stories
			if (pSelStory!=pParentStory)
			{
				pSelStory = NULL;
				break;
			}
		}

		pSel = pSelRange->FindNext(pSel);
	}
	if (pSelStory != NULL)
		TextStory::SetFocusStory(pSelStory);

	// See if there is just one selected TextStory and give it the focus
	// if it had the focus when we left the tool last time.  Also selected the caret within it 
	if ((pLastFocusStory!= NULL) && (pSelStory==NULL) && (GetApplication()->FindSelection()->Count()==1))
	{
		Node* pSelected = GetApplication()->FindSelection()->FindFirst();
		if (IS_A(pSelected,TextStory) && (pSelected == pLastFocusStory))
		{
			TextStory* pTS = (TextStory*)pSelected;
			CaretNode* pCaret = pTS->GetCaret();
			if (pCaret != NULL)
			{
				pTS->DeSelect(TRUE);
				pCaret->Select(TRUE);
				TextStory::SetFocusStory(pTS);
				GetApplication()->FindSelection()->Update(TRUE);
			}
		}
	}

	return ok;
}



/********************************************************************************************
>	BOOL TextTool::OnToolDeselect()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if an error occured, TRUE if tool deselected ok
	Purpose:	Called to do processing required when text tool is deselected
********************************************************************************************/
BOOL TextTool::OnToolDeselect()
{
	BOOL ok = TRUE;

	// Remove the info bar from view by deleting the actual underlying window
	pTextInfoBarOp->Delete();	

	// Destroy the tool's cursors, if they exist, having stopped displaying them
	if (pcCurrentCursor != NULL)
	{
		CursorStack::GPop(CurrentCursorID);
		pcCurrentCursor = NULL;
		CurrentCursorID = 0;
	}
	DestroyCursors();

	// If a caret is selected alone then select its parent text story instead
	GetApplication()->FindSelection()->SetGag(FALSE);
	TextStory* pTS = TextStory::GetFocusStory();
	if (pTS != NULL)
	{
		// Move the selection up
		// If the caret is selected and there is no other selection
		CaretNode* pCaret = pTS->GetCaret();
		VisibleTextNode* pSel = pTS->GetSelectionEnd();
		if (pCaret->IsSelected() && pSel==NULL)
		{
			pCaret->DeSelect(TRUE);
			pTS->Select(TRUE);
			GetApplication()->FindSelection()->Update(TRUE);
		}

		ok = OpDeleteTextStory::RemoveEmptyFocusStory();
	}

	// render off the tool blobs
	if (m_BlobPosList.GetBlobSpread()!=NULL)
		m_BlobPosList.RenderAndRemoveAll();

	// ensure any tool object blobs are removed.
	BlobManager* BlobMgr = GetApplication()->GetBlobManager();
	if (BlobMgr != NULL)
	{
		BlobStyle bsRemoves;
		bsRemoves.ToolObject = TRUE;
		BlobMgr->RemoveInterest(bsRemoves);
	}

	// Clear the focus, remembering what it was for possible later reselection
	pLastFocusStory = TextStory::GetFocusStory();
	TextStory::SetFocusStory(NULL);

	// make sure the ruler is redrawn if we had claimed it
	TextInfoBarOp::ReleaseRuler();
	CurrentTool = FALSE;

	return ok;
}



/********************************************************************************************
>	void TextTool::RenderToolBlobs(Spread* pSpread, DocRect* pClipRect)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/2/96
	Inputs:		pSpread - the spread to render blobs on
				pClipRect - blob rendering clipping rectangle, NULL for no clipping
	Outputs:	-
	Returns:	-
	Purpose:	Called to render the tool blobs for the text tool.
********************************************************************************************/
void TextTool::RenderToolBlobs(Spread* pSpread, DocRect* pClipRect)
{
	m_BlobPosList.RenderAllBlobs(pSpread, pClipRect);

// ARGH! We may need to do this! Phil (15/12/2003)
// See old implementation of RenderToolBlobsOff below.
//	if (!bRenderingOn)
//		m_BlobPosList.DeleteAll();
}



/********************************************************************************************
>	BOOL TextTool::SelectionHasChanged()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/2/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if all ok, FALSE if error occured
	Purpose:	Called when the selection has changed.  Ensures tool blobs are up-to-date
********************************************************************************************/
BOOL TextTool::SelectionHasChanged()
{
	BOOL ok = TRUE;
	
	// Render the old blobs off
	if (ok)
		ok = m_BlobPosList.RenderAllBlobs();

	// Reconstruct the list of blobs
	if (ok)
		ok = m_BlobPosList.BuildFromSelection();

	// Render the new blobs on
	if (ok)
		ok = m_BlobPosList.RenderAllBlobs();

	return ok;
}



/********************************************************************************************
>	void TextTool::RenderToolBlobsOff(Spread* pSpread, DocRect* Rect)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/96
	Inputs:		pSpread - pointer to a spread to render onto
				Rect - render clipping rectangle
	Outputs:	-
	Returns:	-
	Purpose:	Renders off tool blobs.
********************************************************************************************/
//void TextTool::RenderToolBlobsOff(Spread* pSpread, DocRect* Rect)
//{
//	m_BlobPosList.RenderAndRemoveAll();
//}







/********************************************************************************************
>	TextToolBlobPosItem::TextToolBlobPosItem(DocCoord Left, DocCoord Right, BOOL Connected, TextStory* pStory)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/2/96
	Inputs:		Left - the left hand blobs position
				Right - the right hand blobs position
				Connected - TRUE if the two blobs are to be rendered with a line between them
				pStory - the story that this blob is being rendered for
	Outputs:	-
	Returns:	-
	Purpose:	Constructor - sets member variables to the parameters.
********************************************************************************************/
TextToolBlobPosItem::TextToolBlobPosItem(DocCoord Left, DocCoord Right, BOOL Connected, TextStory* pStory)
{
	ERROR3IF(pStory==NULL, "NULL story pointer");
	
	m_LeftBlobPos = Left;
	m_RightBlobPos = Right;
	m_pTextStory = pStory;
	m_BlobsConnected = Connected;
}



/********************************************************************************************
>	TextToolBlobPosItem::~TextToolBlobPosItem()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/2/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destructor
********************************************************************************************/
TextToolBlobPosItem::~TextToolBlobPosItem()
{

}






/********************************************************************************************
>	TextToolBlobPosList::TextToolBlobPosList()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/2/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor - sets member variables to defaults
********************************************************************************************/
TextToolBlobPosList::TextToolBlobPosList()
{
	m_pSelSpread = NULL;
}



/********************************************************************************************
>	TextToolBlobPosList::~TextToolBlobPosList()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/2/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destructor
********************************************************************************************/
TextToolBlobPosList::~TextToolBlobPosList()
{
	DeleteAll();
}



/********************************************************************************************
>	BOOL TextToolBlobPosList::BuildFromSelection()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/2/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE for success, FALSE if an error occured
	Purpose:	Builds a list of text tool blob positions based on the current selection.
********************************************************************************************/
BOOL TextToolBlobPosList::BuildFromSelection()
{
	// Delete what we currently have
	DeleteAll();
	m_pSelSpread = NULL;

	// Scan the selection for selected text stories & text stories with a sub-selection
	SelRange* pSel = GetApplication()->FindSelection();
	Node* pCurrent = pSel->FindFirst();
	TextStory* pLastSelStory = NULL;
	while (pCurrent!=NULL)
	{
		// Are we interested in this item?
		if (pCurrent->IsABaseTextClass())
		{
			TextStory* pCurrentStory = ((BaseTextClass*)pCurrent)->FindParentStory();

			if (pCurrentStory!=NULL && pCurrentStory!=pLastSelStory && pCurrentStory->IsWordWrapping())
			{
				DocCoord LeftBlob;
				DocCoord RightBlob;

				if (pCurrentStory->GetTextPath() != NULL)
				{
					LeftBlob = pCurrentStory->GetLeftIndentPos();
					RightBlob = pCurrentStory->GetRightIndentPos();
				}
				else
				{
					const Matrix* pStoryMat = pCurrentStory->GetpStoryMatrix();
					LeftBlob = DocCoord(0,0);
					RightBlob = DocCoord(pCurrentStory->GetStoryWidth(),0);
					pStoryMat->transform(&LeftBlob);
					pStoryMat->transform(&RightBlob);
				}

				Spread* pSpread = pCurrentStory->FindParentSpread();

				// Add blob to list
				ERROR3IF(pSpread==NULL, "Story didn't have a parent spread!");
				if (pSpread!=NULL)
				{
					// Construct a list item
					TextToolBlobPosItem* pNewItem = new TextToolBlobPosItem(LeftBlob, RightBlob, (pCurrentStory->GetTextPath()==NULL), pCurrentStory);
					if (pNewItem==NULL)
						return FALSE;

					// Add it to the list
					AddTail(pNewItem);

					// Update spread pointer
					ERROR3IF(m_pSelSpread!=NULL && m_pSelSpread!=pSpread, "Selection crosses spreads!  Text tool blobs wont work!");
					m_pSelSpread = pSpread;
				}

				pLastSelStory = pCurrentStory;
			}
		}

		pCurrent = pSel->FindNext(pCurrent);
	}

	return TRUE;
}



/********************************************************************************************
>	BOOL TextToolBlobPosList::RenderAllBlobs(Spread* pSpread = NULL, DocRect* pClipRect = NULL)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/2/96
	Inputs:		pSpread - spread to render blobs for, NULL for all spreads
				pClipRect - rendering clipping rectangle, NULL for no clipping
	Outputs:	-
	Returns:	TRUE for success, FALSE if an error occured
	Purpose:	Renders all the blobs currently in the list (optionally for a given spread)
********************************************************************************************/
BOOL TextToolBlobPosList::RenderAllBlobs(Spread* pSpread, DocRect* pClipRect)
{
	TextToolBlobPosItem* pItem = (TextToolBlobPosItem*) GetHead();

	// If no spread specified then use current, only render onto current spread
	if (pSpread==NULL)
		pSpread = m_pSelSpread;
	if (m_pSelSpread!=pSpread)
		return TRUE;

	while (pItem!=NULL)
	{
		RenderBlobPair(pSpread, pClipRect, pItem);

		pItem = (TextToolBlobPosItem*) GetNext(pItem);
	}

	return TRUE;
}



/********************************************************************************************
>	static void TextToolBlobPosList::RenderBlobPair(Spread* pSpread, DocRect* pClipRect, TextToolBlobPosItem* pPair)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/8/96
	Inputs:		pSpread - spread to render blobs for
				pClipRect - rendering clipping rectangle
				pPair - the TextToolBlobPosItem describing the blob pair
	Outputs:	-
	Returns:	TRUE for success, FALSE if an error occured
	Purpose:	Renders a pair of blobs (left, right and connecting line)
********************************************************************************************/
void TextToolBlobPosList::RenderBlobPair(Spread* pSpread, DocRect* pClipRect, TextToolBlobPosItem* pPair)
{
	ERROR3IF(pPair==NULL, "NULL pointer");

	if (pPair!=NULL)
	{
		// This code is very similar to RenderBlob but avoids having to create 3 times as
		// the number of RenderOnTop loops.
		RenderRegion* pRegion = DocView::RenderOnTop(pClipRect, pSpread, ClippedEOR);
		while (pRegion != NULL)
		{
			// Draw the two blobs
			pRegion->SetLineColour(COLOUR_NONE);
			pRegion->SetFillColour(COLOUR_BEZIERBLOB);
			pRegion->DrawBlob(pPair->GetLeftBlobPos(), BT_UNSELECTED);
			pRegion->DrawBlob(pPair->GetRightBlobPos(), BT_UNSELECTED);

			// Draw the connecting line
			if (pPair->IsConnected())
			{
				pRegion->SetLineColour(COLOUR_BEZIERBLOB);
				pRegion->DrawLine(pPair->GetLeftBlobPos(), pPair->GetRightBlobPos());
			}

			// Get the next region in the list
			pRegion = DocView::GetNextOnTop(pClipRect);
		}
	}
}



/********************************************************************************************
>	static void TextToolBlobPosList::RenderBlob(Spread* pSpread, DocRect* pClipRect, DocCoord BlobPos)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/96
	Inputs:		pSpread - spread to render blobs for
				pClipRect - rendering clipping rectangle
				BlobPos - the position to render the blob
	Outputs:	-
	Returns:	TRUE for success, FALSE if an error occured
	Purpose:	Renders one text origin blob
********************************************************************************************/
void TextToolBlobPosList::RenderBlob(Spread* pSpread, DocRect* pClipRect, DocCoord BlobPos)
{
	RenderRegion* pRegion = DocView::RenderOnTop(pClipRect, pSpread, ClippedEOR);
	while (pRegion != NULL)
	{
		// Draw a Cross Hair
		pRegion->SetLineColour(COLOUR_NONE);
		pRegion->SetFillColour(COLOUR_BEZIERBLOB);
		pRegion->DrawBlob(BlobPos, BT_UNSELECTED);

		// Get the next region in the list
		pRegion = DocView::GetNextOnTop(pClipRect);
	}
}



/********************************************************************************************
>	BOOL TextToolBlobPosList::FindBlobStory(Spread* pSpread, DocCoord PointerPos, BOOL* pLeft, TextStory** pStory)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/96
	Inputs:		pSpread - spread PointerPos is on
				PointerPos - the coord to check
	Outputs:	pLeft - TRUE if point over left blob, FALSE if over right blob (Can be NULL)
				pStory - pointer to found story, NULL if no story
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Finds the textstory that the mouse is over.
********************************************************************************************/
BOOL TextToolBlobPosList::FindBlobStory(Spread* pSpread, DocCoord PointerPos, BOOL* pLeft, TextStory** pStory)
{
	// Is the spread the same as our current one?
	ERROR2IF(pSpread!=m_pSelSpread && m_pSelSpread!=NULL, FALSE, "Different spreads");
	ERROR2IF(pStory==NULL, FALSE, "NULL output parameter");

	// Work backwards through our blob list, thus in hit test order.
	TextToolBlobPosItem* pItem = (TextToolBlobPosItem*) GetTail();
	while (pItem!=NULL)
	{
		// Get a rectangle surrounding the left blob
		DocRect BlobBounds;
		DocCoord BlobPos = pItem->GetLeftBlobPos();
		GetApplication()->GetBlobManager()->GetBlobRect(BlobPos, &BlobBounds);
		if (BlobBounds.ContainsCoord(PointerPos))
		{
			if (pLeft!=NULL)
				*pLeft = TRUE;
			*pStory = pItem->GetBlobStory();

			return TRUE;
		}

		// Test the right blob
		BlobPos = pItem->GetRightBlobPos();
		GetApplication()->GetBlobManager()->GetBlobRect(BlobPos, &BlobBounds);
		if (BlobBounds.ContainsCoord(PointerPos))
		{
			if (pLeft!=NULL)
				*pLeft = FALSE;
			*pStory = pItem->GetBlobStory();

			return TRUE;
		}

		pItem = (TextToolBlobPosItem*) GetPrev(pItem);
	}

	return TRUE;
}



/********************************************************************************************
>	BOOL TextToolBlobPosList::RenderAndRemove(TextStory* pStory)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/96
	Inputs:		pStory - pointer to a text story
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	If the story is in the list of blobs then it's blob is rendered (off) and the 
				blob is removed from the blob list.
********************************************************************************************/
BOOL TextToolBlobPosList::RenderAndRemove(TextStory* pStory)
{
	ERROR2IF(pStory==NULL, FALSE, "NULL story pointer");

	// Work through our list to find the story
	TextToolBlobPosItem* pItem = (TextToolBlobPosItem*) GetHead();
	while (pItem!=NULL)
	{
		if (pItem->GetBlobStory() == pStory)
		{
			// Render the blob pair off
			RenderBlobPair(m_pSelSpread, NULL, pItem);

			// Remove it from the list
			RemoveItem(pItem);
			delete pItem;
			
			return TRUE;
		}
		else
			pItem = (TextToolBlobPosItem*) GetNext(pItem);
	}

	ERROR3("Story not found");

	return TRUE;
}



/********************************************************************************************
>	void TextToolBlobPosList::RenderAndRemoveAll()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	renders off all the blobs and removes them from the list.
********************************************************************************************/
void TextToolBlobPosList::RenderAndRemoveAll()
{
	// Work through our list to find the story
	TextToolBlobPosItem* pItem = (TextToolBlobPosItem*) RemoveHead();
	while (pItem!=NULL)
	{
		// Render the blob pair off
		RenderBlobPair(m_pSelSpread, NULL, pItem);

		// Delte the item now we've finshed with it.
		delete pItem;
			
		pItem = (TextToolBlobPosItem*) RemoveHead();
	}
}



