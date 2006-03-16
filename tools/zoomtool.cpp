// $Id: zoomtool.cpp 662 2006-03-14 21:31:49Z alex $
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
// The Zoom Tool
/*
*/

#include "camtypes.h"

//#include "opdesc.h"
#include "docview.h"
//#include "stockcol.h"
//#include "lineattr.h"
#include "ensure.h"
#include "strings.h"
//#include "resource.h"
//#include "barsdlgs.h"
//#include "errors.h"
//#include "mario.h"
//#include "viewrc.h"
//#include "infobar.h"
//#include "basestr.h"
#include "csrstack.h"
//#include "progress.h"
#include "osrndrgn.h"
//#include "mainfrm.h"
#include "winrect.h"
#include "wrkrect.h"
//#include "oilfiles.h"
//#include "dlgmgr.h"
#include "spread.h"
//#include "app.h"
#include "range.h"
//#include "justin.h"
//#include "markn.h"
//#include "blobs.h"
#include "zoomtool.h"
#include "zoomops.h"
//#include "selector.h"		// for SelectorTool::InflateByBlobBorder

#include "module.h"
//#include "tool.h"
//#include "zoomres.h"
//#include "ezmodule.h"

//#include "will2.h"
//#include "hotlink.h"
//#include "userattr.h"
//#include "ralphdoc.h"

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_MEMDUMP(ZoomTool, Tool_v1)


// *******************************************************
// MIN and MAX Macros - These should not be here, I'm sure
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))
// *******************************************************



////////////////////////////////////////////////////////////////////////////////////////////
// class ZoomTool

TCHAR	   *ZoomTool::FamilyName = _T("View Tools");
TCHAR	   *ZoomTool::ToolName	= _T("Zoom Tool");
TCHAR	   *ZoomTool::Purpose	= _T("To zoom in on a document");
TCHAR	   *ZoomTool::Author	= _T("JustinF");
BOOL		ZoomTool::bIsActive	= FALSE;


/********************************************************************************************
>	ZoomTool::ZoomTool()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/93
	Purpose:	Dummy Constructor
********************************************************************************************/

ZoomTool::ZoomTool()
  : Inited(FALSE),
  	pcZoomCursor(NULL),
  	pcZoomOutCursor(NULL),
	pZoomInfoBarOp(NULL),
	psStatusText(NULL),
	bStatusTextValid(FALSE)
{
	// Empty.
}



/********************************************************************************************
>	virtual ZoomTool::~ZoomTool()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/93
	Purpose:	Destructor (Virtual)
********************************************************************************************/

ZoomTool::~ZoomTool()
{
	// Empty
}



/********************************************************************************************
>	virtual BOOL ZoomTool::Init()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/94
	Returns:	TRUE if the Zoom Tool initialised OK, FALSE otherwise.
	Purpose:	To Test if the tool had started up ok
	Errors:		Sends warning to debugging terminal if the info-bar can't be
				initialised.
********************************************************************************************/

BOOL ZoomTool::Init()
{
	// Don't initialise twice, dummy!
	if (Inited) return FALSE;		
	Inited = TRUE;

#if defined(EXCLUDE_FROM_RALPH)
	return OpZoom::Declare();
#else
	pZoomInfoBarOp = new ZoomInfoBarOp(_R(IDD_ZOOMINFO));
	BOOL ok=(pZoomInfoBarOp!=NULL) && OpZoom::Declare();
#if 0
	// Load in the info-bar stuff.
	CCResTextFile 		file;				// Resource File
	ZoomInfoBarOpCreate BarCreate;			// Object that creates ZoomInfoBarOp objects

	// Register the OpDescriptors and THEN open the bar resource file!
	BOOL	ok = OpZoom::Declare() && file.open(_R(IDM_ZOOM_BAR), _R(IDT_INFO_BAR_RES));
	if (ok) ok = DialogBarOp::ReadBarsFromFile(file, BarCreate);  // Read & create bar
	if (ok) file.close();									 	  // Close resource

	ENSURE(ok, "Unable to load ZOOMBAR.INI from resource\n"); 

	if (ok)
	{
		// Info bar now exists.  Now get a pointer to it.
		String_32 str(_R(IDS_ZOOMTOOL_INFOBARNAME));
		DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(str);

				ok = (pDialogBarOp != NULL);
		if (ok) ok = pDialogBarOp->IsKindOf(CC_RUNTIME_CLASS(ZoomInfoBarOp));
		if (ok) pZoomInfoBarOp = (ZoomInfoBarOp*) pDialogBarOp;

		ENSURE(ok, "Couldn't find zoom tool info bar");
	}
#endif
	return ok;
#endif
}



/********************************************************************************************
>	virtual void ZoomTool::Describe(void* InfoPtr)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/93
	Inputs:		InfoPtr - Pointer to a block that will hold the name data
	Purpose:	The tool declares what versions of the tool module interface it
				can understand as well as its name, family etc
********************************************************************************************/

void ZoomTool::Describe(void* InfoPtr)
{
	// Cast structure into the latest one we understand.
	ToolInfo_v1* Info = (ToolInfo_v1*) InfoPtr;
	Info->InfoVersion = 1;
	
	// You should always have this line.
	Info->InterfaceVersion = GetToolInterfaceVersion();
		
	// These are all arbitrary at present.
	Info->Version = 1;
	Info->ID      = GetID();
	Info->TextID  = _R(IDS_ZOOM_TOOL);

	// Make a note of the tools description.
	Info->Family  = FamilyName;
	Info->Name    = ToolName;
	Info->Purpose = Purpose;
	Info->Author  = Author;
	
	// this tool uses the infobar, so supply a dialog id to put in the info bar.
	Info->InfoBarDialog = _R(IDD_ZOOMINFO);
	Info->BubbleID = _R(IDBBL_ZOOM_TOOLICON);
}



/********************************************************************************************
>	virtual UINT32 ZoomTool::GetID()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

UINT32 ZoomTool::GetID()
{
	return TOOLID_ZOOM;
}



/********************************************************************************************
>	virtual void ZoomTool::SelectChange(BOOL fIsSelected)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/93
	Inputs:		isSelected - TRUE if the tool is becoming selected
	Purpose:	Called when the tool is selected or deselected.  Creates and pushes the
				tool's cursor; pops and destroys it.
	Errors:		Sends warning to debugging terminal if creating the cursor fails.
	SeeAlso:	ZoomTool::IsSelectedTool
********************************************************************************************/

void ZoomTool::SelectChange(BOOL fIsSelected)
{
	if (fIsSelected)
	{
		// This tool has just been selected.  Set the internal global flag so this
		// can be easily checked.
		bIsActive = TRUE;
//		TRACEUSER( "JustinF", _T("===================================== into ZOOM TOOL\n"));

		// Try to load our cursors.
		pcZoomCursor = new Cursor(this, _R(IDC_ZOOMTOOLCURSOR));
		pcZoomOutCursor = new Cursor(this, _R(IDC_ZOOMOUTTOOLCURSOR));

#ifdef RALPH
		//Graham 20/9/96. This cursor is needed to activate
		//HotLinks (TM) in Ralph (TM). The cursor is a static member of
		//the Cursor class and so we don't need to delete it.
		pcHotLinkCursor = Cursor::PointingHand;
#endif //Ralph

		if (pcZoomCursor == NULL || !pcZoomCursor->IsValid())
			return;

		// Push cursor, but don't display now.
		CurrentCursorID = CursorStack::GPush(pcZoomCursor, FALSE);
		
#if !defined(EXCLUDE_FROM_RALPH)
		// Show the info-bar.
		pZoomInfoBarOp->Create();
#endif

		// Allocate the string used for status-bar text, if necessary.
		// Graham 7/9/96: We now do this in Ralph too.

		if (psStatusText == NULL)
		{
			psStatusText = new String_256;
			#ifdef _DEBUG
				if (psStatusText == NULL)
					TRACE( _T("Couldn't allocate a String for zoom tool status bar text!\n") );
			#endif
			bStatusTextValid = FALSE;
		}


		// Which blobs do I want displayed?
		// Used to declare an interest in no blobs which meant that all blobs disappeared
		// when this tool is selected. Now try not to affect this by not declaring an
		// interest.
		// Changed by Neville 16/8/94
		//	BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		//	if ((BlobMgr != NULL) && (Document::GetCurrent() != NULL))
		//	{
		//		// Decide which blobs to display
		//		BlobStyle MyBlobs;
		//	
		//		// We want them all off, which is the default
		//		// tell the blob manager
		//		BlobMgr->ToolInterest(MyBlobs);
		//	}
	}
	else
	{
//		TRACEUSER( "JustinF", _T("------------------------------------- out of ZOOM TOOL\n"));
		
		// No longer the selected tool.
		bIsActive = FALSE;

		// Deselection - destroy the tool's cursor, if there is one.
		if (pcZoomCursor != NULL)
		{
			CursorStack::GPop(CurrentCursorID);
			delete pcZoomCursor;
			delete pcZoomOutCursor;
			CurrentCursorID = 0;
		}
#if !defined(EXCLUDE_FROM_RALPH)
		// Hide the info-bar.
		pZoomInfoBarOp->Delete();
#endif
	
		// Deallocate the status-bar text "mask".
		// Graham 7/9/96: And we now do this in Ralph too.
		delete psStatusText;
		psStatusText = NULL;

	}
}



/****************************************************************************

>	void ZoomTool::OnKeyPress(KeyPress* pKey)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/02/2004

	Inputs:		pKey		- pointer to a KeyPress
	Purpose:	

****************************************************************************/

BOOL ZoomTool::OnKeyPress(KeyPress* pKey)
{
	// Find the current state of the "click" keyboard modifiers...
	ClickModifiers ClickMods = ClickModifiers::GetClickModifiers();
	Cursor* pPointerShape = pcZoomCursor;

	// if the shift key is pressed...
	if (ClickMods.Adjust && !ClickMods.Constrain && !ClickMods.Alternative1)
	{
		pPointerShape = pcZoomOutCursor;
	}

	// only change if this cursor is different from the current cursor
	if (pPointerShape != MyCurrentCursor)
	{
		// set this cursor as the current cursor and immediately display it
		CursorStack::GSetTop(pPointerShape, CurrentCursorID);
		// remember this is our current cursor
		MyCurrentCursor = pPointerShape;
	}

	// return FALSE so that the key press gets passed on (e.g. Space needs to be passed
	// on so that the "swap to selector tool" works.
	return FALSE;
}

/********************************************************************************************
>	virtual void ZoomTool::OnClick(DocCoord dcMousePos, ClickType ctype,
						   		   ClickModifiers cmods, Spread* pMouseSpread)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Inputs:		dcMousePos 		- The Coords (in DocCoords) of the point where the mouse
							 	  button was clicked.
				ctype      		- Describes the type of click that was detected. 
				cmods  			- Indicates which buttons caused the click and which
							 	  modifers were pressed at the same time.
				pMouseSpread    - The spread that the click was on
	Returns:	TRUE if it handled the Click, FALSE otherwise
	Purpose:	Handle a mouse click and perform a zoom if needed. If we decided to
				do a zoom we first have to create an operation and a drag is started
				on it. If the user was just clicking on the window, then the drag
				will be very short lived and the operation will be able to detect
				that is was in reality just a click.  Once the drag has been started
				our part of the program is complete and it is up to the operation to
				actually change the zoom factor and get the	document to redraw.
********************************************************************************************/

void ZoomTool::OnClick(DocCoord dcMousePos, ClickType ctype,
					   ClickModifiers cmods, Spread* pMouseSpread)
{
	// We only act on single clicks.
	if (ctype != CLICKTYPE_SINGLE) return;
	if (cmods.Menu) return;							// Don't do anything if the user clicked the Menu button

#ifdef RALPH
	//Graham 20/9/96: If we're in Ralph, clicking on a Hot Link activates it.
	//The one exception to this rule is if CTRL or SHIFT is pressed, in which case
	//the tool acts as normal.

	//That way CTRL acts as a "Don't activate Hot Links" key, and SHIFT-clicks
	//work as normal

	//So, is CTRL or SHIFT pressed?
	if (!(cmods.Constrain) && !(cmods.Adjust))
	{
		//No. Are we on a Hot Link? Let's try and find one.

		//First, find a Hot Link
	
		Node* pHotLinkNode= DocView::FindHotLinkNode(dcMousePos);

		//If we've found one, go to that Hot Link
		if (pHotLinkNode)
		{
			DocView::GoToHotLink((AttrUser*) pHotLinkNode);
			return;
		}
	}

	//If CTRL was pressed, or if we weren't over a Hot Link, we treat
	//the click as a normal Zoom click. Read on...

#endif //Ralph

	// Try to make a zoom operation.
	OpZoom* pZoomOp = new OpZoom;
	if (pZoomOp == NULL)
	{
		// We're out of memory, so we can't do the operation.
		InformError(_R(IDE_NOMORE_MEMORY));
		return;
	}

	// If the click is with the adjust button we zoom out, otherwise we always start
	// a drag with a non-adjust click.  If it turns out that the drag is insignificant
	// we will zoom in, ignoring the drag.
	if (cmods.Adjust)
	{
		DocView* pDocView = DocView::GetCurrent();
		ERROR3IF(pDocView == NULL, "Null current DocView in ZoomTool::OnClick");
		pZoomOp->ZoomOut(pDocView->GetClickWorkCoord());
	}
	else
	{
		pZoomOp->DoDrag(pMouseSpread, dcMousePos, cmods);
	}
}



/********************************************************************************************
>	virtual void ZoomTool::OnMouseMove(DocCoord dcPoint, Spread* pSpread, ClickModifiers mods)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Inputs:		(not used) ...except in Ralph (says Graham)
	Purpose:	immediately updates status line text
********************************************************************************************/

void ZoomTool::OnMouseMove(DocCoord dcPoint, Spread* pSpread, ClickModifiers mods)
{
#ifdef RALPH
	//Graham 20/9/96.
	//We need to check if the zoom tool's over a Hot Link. If it is, the cursor changes
	//to a pointing hand

	//The one exception is if CTRL's pressed, in which case the cursor is the 
	//normal Zoom cursor.

		//First, try and find a node with a Hot Link
	AttrUser* pHotLinkNode= (AttrUser*) DocView::FindHotLinkNode(dcPoint);

	if (pHotLinkNode && !(mods.Constrain) && !(mods.Adjust))
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

		//Now get the full status bar string and put the URL into it
		String_256 sZoomTool;

		sZoomTool.MakeMsg(_R(IDS_ZOOMTOOL_HOTLINK), (TCHAR*) strEmbedded);
		
		//And tell the Ralph document to show the string in the browser's status
		//bar
		ThisRalphDoc->SetStatusBar(&sZoomTool);
	}
	else
	{
		//No, the cursor is not over a Hot Link
		//Change the cursor to the standard selector-style "arrow"
		ChangeCursor(pcZoomCursor);

		//And get the status bar text
		ValidateStatusText();
		if (psStatusText) DocView::SetBrowserStatusBar(psStatusText);

	}
#else

	//If we're in Camelot rather than Ralph

	ValidateStatusText();

	if (psStatusText) GetApplication()->UpdateStatusBarText(psStatusText);
#endif //Ralph

}

/********************************************************************************************
>	void ZoomTool::ValidateStatusText();

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/95
	Outputs:	validates string pointed to by psStatusText
	Purpose:	Checks if the text in the status-bar needs updating, for example if a new
				zoom factor has been selected since it was last painted, and does what is
				required.

				Graham 7/9/96: We now get status line text in Ralph too.
********************************************************************************************/

void ZoomTool::ValidateStatusText()
{
	// If there is some status text to update ...
	if (psStatusText == NULL) return;

#ifndef RALPH
	// If the zoom factor has not changed since we last created the string ...
	// Graham 7/10/96: In Ralph, we assume the status bar text is always invalid
	if (bStatusTextValid) return;
#endif

	// Get the selected DocView, if any.
	DocView* pDocView = DocView::GetSelected();
	if (pDocView == NULL) return;			

	// Get the view's index into the zoom table, if initialised.
	INT32 nIndex = DocView::GetSelected()->GetZoomTableIndex();
	if (nIndex == cUninitZoomIndex) return;

	// Check if we have a 'fractional' zoom index that is equivalent to a preset
	// zoom factor.  If we have then convert it now to the equivalent preset, as
	// it makes the logic simpler, if slightly slower.
	INT32 nFracPercent = (pDocView->GetViewScale() * 100 + FIXED16_DBL(0.5)).MakeInt();
	if (nIndex == cFractionalZoomIndex)
	{
		// Convert the fractional index into a percentage.
		for (INT32 i = 0; i < cZoomTableSize; i++)
		{
			// If it's the same is the preset set it to the preset.
			if (nFracPercent == OpZoom::GetPresetZoomPercent(i))
			{
				pDocView->SetZoomTableIndex(nIndex = i);
				break;
			}
		}
	}	

	// Work out what percentages we get if we zoom in and zoom out from the current zoom
	// factor.  If we are at the maximum or minimum zoom we note this here as well.
	INT32 nInPercent, nOutPercent;
	if (nIndex != cFractionalZoomIndex)
	{
		// Handle a zoom factor that appears in the zoom table.
		if (nIndex == 0)
		{
			// Can't zoom in any further.  Use the special code for that, a 0% zoom.
			nInPercent  = 0;
			nOutPercent = OpZoom::GetPresetZoomPercent(1);
		}
		else if (nIndex == cZoomTableSize - 1)
		{
			// Can't zoom out any further.
			nInPercent  = OpZoom::GetPresetZoomPercent(cZoomTableSize - 2);
			nOutPercent = 0;
		}
		else
		{
			// The in/out factors are the next and previous presets.
			nInPercent	= OpZoom::GetPresetZoomPercent(nIndex - 1);
			nOutPercent = OpZoom::GetPresetZoomPercent(nIndex + 1);
		}
	}
	else
	{
		// Handle a zoom factor that doesn't appear in the zoom table.
		// Search the zoom factor table looking for two consequetive entries that
		// bracket the current percentage.
		INT32 i;
		for (i = 0; i < cZoomTableSize; i++)
			if (nFracPercent > OpZoom::GetPresetZoomPercent(i)) break;

		// Convert the found bounds into "in" and "out" percentages.
		if (i == 0)
		{
			// Can't zoom in any further, and more than the maximum preset.
			nInPercent  = 0;
			nOutPercent = OpZoom::GetPresetZoomPercent(0);
		}
		else if (i == cZoomTableSize)
		{
			// Can't zoom out any further, and less than the minimum preset.
			nInPercent  = OpZoom::GetPresetZoomPercent(cZoomTableSize - 1);
			nOutPercent = 0;
		}
		else
		{
			// Current zoom percentage lies within the bounds of the table.
			nInPercent  = OpZoom::GetPresetZoomPercent(i - 1);
			nOutPercent = OpZoom::GetPresetZoomPercent(i);
		}
	}

#ifdef RALPH
	//Graham 7/9/96: In Ralph, the status bar text only refers to the current zoom
	psStatusText->Empty();

	String_256 s;
	s.MakeMsg(_R(IDS_RALPH_ZOOMTOOLSTR), nFracPercent);
	*psStatusText+=s;

#else
	// (Re)create the status-bar text string from the two string-sub masks.
	psStatusText->Empty();
	if (nInPercent != 0) psStatusText->MakeMsg(_R(IDS_STATUS_ZOOM_IN_MASK), nInPercent);
	if (nOutPercent != 0)
	{
		String_256 s;
		s.MakeMsg(_R(IDS_STATUS_ZOOM_OUT_MASK), nOutPercent);
		*psStatusText += s;
	}

	// Append some text about dragging.
	*psStatusText += String_256(_R(IDS_STATUS_ZOOM_DRAG_TEXT));

	// Mark the status-bar text as now valid.
	bStatusTextValid = TRUE;
#endif //Ralph
}


/*****************************************************************************
>	virtual BOOL ZoomTool::GetStatusLineText(String_256* ptext, Spread* pSpread,
											 DocCoord DocPos, ClickModifiers ClickMods);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Inputs:		pSpread   - pointer to spread under mouse (else NULL)
				DocPos    - position of mouse in doc (in spread coords)
				ClickMods - mouse click modifiers
	Outputs:	ptext - text for status line
	Returns:	TRUE if outputting valid text
	Purpose:	generate up-to-date text for the status line (called on idles)
*****************************************************************************/

BOOL ZoomTool::GetStatusLineText(String_256* ptext, Spread*, DocCoord, ClickModifiers)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	ERROR3IF(ptext==NULL,"ZoomTool::GetStatusLineText() - ptext passed as null");

	ValidateStatusText();
	if (psStatusText==NULL)	return FALSE;

	*ptext=*psStatusText;
	return TRUE;
#else
	return FALSE;
#endif
}


/********************************************************************************************
>	static BOOL ZoomTool::IsSelectedTool()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the zoom tool is the currently active ("selected") tool.
	Purpose:	
	Errors:		-
	SeeAlso:	ZoomTool::SelectChange
********************************************************************************************/

BOOL ZoomTool::IsSelectedTool()
{
	return bIsActive;
}



/********************************************************************************************
>	static void ZoomTool::InvalidateStatusText()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Checks if the zoom tool is the currently active ("selected") tool, and if
				it is then "invalidates" the status-bar text, forcing it to be updated on
				receipt of the next call to OnMouseMove
	Errors:		-
	SeeAlso:	ZoomTool::OnMouseMove
********************************************************************************************/

void ZoomTool::InvalidateStatusText()
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// If we are the current tool and have a status-bar text string, then "invalidate" it.
	if (IsSelectedTool())
	{
		ZoomTool* pTool = (ZoomTool*) Tool::GetCurrent();
		ENSURE(pTool != NULL, "Null current Tool* in ZoomTool::InvalidateStatusText");
		if (pTool->psStatusText != NULL) pTool->bStatusTextValid = FALSE;
	}
#endif
}

#ifdef RALPH
/********************************************************************************************

>	void ZoomTool::ChangeCursor(Cursor* cursor)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>, from Jim's code
	Created:	20/9/96
	Inputs:		ID of the cursor you want to flip to
	Outputs:	-
	Returns:	-
	Purpose:	We only need this in Ralph (at present), where the zoom tool cursor
				sometimes changes to a pointing hand, which is used to activate Hot Links.
	
				Changes to the specified cursor. Will only change the cursor if it isn't already
				this cursor, so it doesn't flicker.

	Errors:		can fail if the cursor cannot be created - the cursor code will fail.
	SeeAlso:	-

********************************************************************************************/

void ZoomTool::ChangeCursor(Cursor* cursor)
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

>	void ZoomTool::OnKeyPress(KeyPress* pKey)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/9/96
	Inputs:		Keypress object
	Outputs:	-
	Returns:	-
	Purpose:	When a key is pressed, we call the Mouse Move function to update the
				cursor and status bar text.


********************************************************************************************/
/*
BOOL ZoomTool::OnKeyPress(KeyPress* pKey)
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

// These lines make this a stand-alone DLL when required.
/*
IMPLEMENT_SIMPLE_MODULE(ZoomModule,
						MODULEID_ZOOM,
						ZoomTool,
						"Zoom Tool",
						"To scale the view",
						"Camelot team");
*/
