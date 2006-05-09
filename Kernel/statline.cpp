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
// status line 

/*
*/

#include "camtypes.h" 
//#include "resource.h"	// for _R(IDC_SNAPPED) - 'snappped' mouse pointer
#include "csrstack.h"
#include "camelot.h"
#include "statline.h"
//#include "ctrlhelp.h"
#include "optsmsgs.h"
//#include "scrvw.h"
#include "docview.h"
#include "app.h"
#include "tool.h"
#include "errors.h"
//#include "cstatbar.h"
#include "ccolbar.h"
#include "document.h"
#include "basebar.h"
//#include "colpick.h"
//#include "rulers.h"
#include "spread.h"

#include "bars.h"
//#include "sgallery.h"

//#include "prevwdlg.h"

#include "colcontx.h"
#include "colormgr.h"
#include "opimgset.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNCREATE(StatusLine, StandardBar);

#define new CAM_DEBUG_NEW

// constants ...
const String_8 SelDescSuffix(_T(":: "));
const String_8 TruncChar(_T(";"));
const INT32      TextDelay  =1000;			// milisecond
const INT32      RenderDelay=200;			// milisecond

//void FixFPControlRegister();

String_256 StatusLine::m_strPrefix(_T(""));
BOOL StatusLine::restrictStatusLineFunctionsToColourPicker = FALSE;
BOOL StatusLine::DoControlHelp = TRUE;
StatusLine * StatusLine::s_pStatusLine = NULL;

// render pane animation (could be in statline.cpp but kept here next to pane defn)
static UINT32 RenderAnimation[] = { _R(IDB_SL_REND1), _R(IDB_SL_REND2), _R(IDB_SL_REND3) };

/*****************************************************************************
>	StatusLine::StatusLine()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Purpose:	Constructor for StatusLine
*****************************************************************************/

StatusLine::StatusLine()
{
	// ensure initial updates
	OldMemory=0;
	MousePosDP=2;
	TextTimer.Sample();
	RenderTimer.Sample();
	RenderAnimationState=0;
	MousePosNeedsUpdatingFlag=TRUE;
	MousePosPaneNeedsResizing=TRUE;
//	TransparencyNeedsUpdating=TRUE; 

	// create a 'snapped' mouse pointer
	pSnappedCursor = new Cursor(_R(IDCSR_SNAPPED));
	SnappedCursorID = 0;

	JobDescription=NULL;
	CurrentPercent=-1;
	ProgressShown=FALSE;
}


/*****************************************************************************
>	StatusLine::~StatusLine()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/10/95
	Purpose:	destructor for StatusLine
*****************************************************************************/

StatusLine::~StatusLine()
{
	if (pSnappedCursor!=NULL)
	{
		delete pSnappedCursor;
		pSnappedCursor=NULL;
	}
	if (this == s_pStatusLine)
	{
		// Only zap it out if there is a live status line. Status line objects
		// are temporarily created when invoking bar closure
		s_pStatusLine = NULL;
	}
}


/*****************************************************************************
>	BOOL StatusLine::OnIdle()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Purpose:	Idle event handler for status line
				Updates the text on the status bar if 'TextDelay' 
				has elapsed since it was last changed.
				Also updates the mouse position pane
	Returns:	FALSE if anything fails (see Errors)
				HOWEVER, the function will try to proceed
				to keep the status line as tidy as possible
	Errors:		pCCStatusBar==NULL,
				RefreshHelpText() fails,
				UpdateMousePos() fails,
				pCCStatusBar->UpdatePaneText() fails,
*****************************************************************************/

BOOL StatusLine::OnIdle()
{
	// If the current doc doesn't have a doc view (e.g. it's a clipboard doc), then there's no point
	// carrying on, because it's not a proper document, hence doen't require up-to-date status bar info.
	if (DocView::GetSelected() == NULL)
		return TRUE;

	BOOL ReturnValue=TRUE;			// set to FALSE if any section of code fails

#ifndef STANDALONE
	// if mouse position pane needs resizing, do it (ensuring it's enabled)
	if (MousePosPaneNeedsResizing)
	{
		MousePosPaneNeedsResizing=FALSE;
		Spread* pSpread=Document::GetSelectedSpread();
		if (pSpread)
		{
			if (!SetMousePosPaneWidth(pSpread))
				ReturnValue=FALSE;
PORTNOTE("StatusLine", "Removed use of CCStatusBar")
#ifndef EXCLUDE_FROM_XARALX
			if (pCCStatusBar->PaneState(_R(IDS_SL_MOUSEPOS),Enable)==Fail)
				ReturnValue=FALSE;
#endif
		}
	}
#endif

	// if sufficient time elapsed since status line last changed, refresh it
	if (TextTimer.Elapsed(TextDelay,TRUE))
		if (RefreshHelpText()==FALSE)
			ReturnValue=FALSE;

#ifdef _STATUSLINE_MEMORYPANE
	// establish if amount of memory changed BODGE
	MEMORYSTATUS MemState;
	MemState.dwLength=sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&MemState);
	INT32 Memory=MemState.dwAvailPhys;
	BOOL MemoryChanged=(OldMemory!=Memory);
	OldMemory=Memory;

	if (MemoryChanged)
	{
		// If this ever appears in retail builds it will have to be rewritten properly (it
		// isn't internationally portable at the moment).		
		String_256 StrBuf(TEXT(""));
		camSnprintf(StrBuf, 256, TEXT("%dK"), Memory / 1024);
		if (StrBuf.Length()>4)
			StrBuf.Insert(&String_8(TEXT(",")),StrBuf.Length()-4);
		BOOL ok = SetStringGadgetValue(_R(IDS_SL_MEMORY),StrBuf);
		if (ok)
			PaintGadgetNow(_R(IDS_SL_MEMORY))
		else
			ReturnValue=FALSE;
	}
#endif // _STATUSLINE_MEMORYPANE

// We don't need to know that the status line failed to update normally.
//	ENSURE(ReturnValue==TRUE, "StatusLine::OnIdle() failed!");

	return TRUE;
}


/*****************************************************************************
>	BOOL StatusLine::RefreshHelpText()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/95
	Purpose:	refresh the help text pane of the status line
	Returns:	FALSE if anything fails (see Errors)
	Errors:		CCamApp::GetMousePosAndWindowID() fails,
				pCCStatusBar->UpdatePaneText() fails,
				UpdateText() fails
*****************************************************************************/

BOOL StatusLine::RefreshHelpText()
{	
	// if drag op in progress, don't slow it down, just exit
	if (Operation::GetCurrentDragOp())
		return TRUE;

	BOOL ReturnValue=TRUE;
	BOOL PrefixSelDesc=STATUSLINE_SELDESC_DEFAULT;
	String_256 text(_T(""));
	BOOL TextValid=FALSE;

	// for new custom colour picker tool ....
	// (do NOT update status text if this tool is active!)

	if (IsRestrictedAccessToColourPicker () == TRUE)
	{
		return (TRUE);
	}

	if (!TextValid)
		TextValid=BaseBar::GetStatusLineText(&text);		// try bar drag
	if (TextValid)
		PrefixSelDesc=STATUSLINE_SELDESC_BARDRAG;

//	CWindowID	WinID=NULL;
	DocView*	pDocView=NULL;
	Spread*		pSpread=NULL;
	WinCoord	WndPos(0,0);
	DocCoord	DocPos(0,0);

	// if no valid text so far (and valid current doc else something deep down goes BANG)
	// get mouse pos (in DocCoords), handle of window it is over, DocView ptr, spread ptr
	if (TextValid!=TRUE && Document::GetCurrent()!=NULL)
	{
PORTNOTE("Statline", "Removed use of GetMousePosAndWindowID to abort snap processing")
#if !defined(EXCLUDE_FROM_XARALX)
		BOOL MouseOK=CCamApp::GetMousePosAndWindowID(&WinID,&WndPos);
		if (!MouseOK)
			return FALSE;

		if (WinID != NULL)
			pDocView=ScreenView::GetDocViewFromWindowID(WinID);
#endif

		if (pDocView != NULL)
		{
			OilCoord OilPos = WndPos.ToOil(pDocView, TRUE);
			pSpread = pDocView->OilToSpreadCoord(OilPos, &DocPos);
		}
	}

	// if no text so far, try drag ops
	if (!TextValid)
	{
		Operation* pDragOp=Operation::GetCurrentDragOp();
		if (pDragOp)
		{
			TextValid=pDragOp->GetStatusLineText(&text,pSpread,DocPos,ClickModifiers::GetClickModifiers());
			ERROR3IF(!TextValid,"StatusLine::RefreshHelpText() - pDragOp->GetStatusLineText() failed");
			PrefixSelDesc=STATUSLINE_SELDESC_DRAGOPS;
			// drag op in progress so we must have valid text (stops tool being interogated)
			if (!TextValid)
			{
				text="";	
				TextValid=TRUE;
			}
		}
	}

	// if no text so far, try other bits!
	if (!TextValid)
	{
		BOOL OverSelectedDoc=FALSE;
		if (pDocView)
		{
			if (pDocView==DocView::GetSelected())
			{
				OverSelectedDoc=TRUE;
				PrefixSelDesc=STATUSLINE_SELDESC_SELDOC;
				Tool* pTool=Tool::GetCurrent();
				ERROR3IF(pTool==NULL,"StatusLine::RefreshHelpText() - no selected tool");
				if (pTool)
					TextValid=pTool->GetStatusLineText(&text,pSpread,DocPos,ClickModifiers::GetClickModifiers());
				else
					ReturnValue=FALSE;
			}
			else
			{
				TextValid=text.Load(_R(IDS_CLICKHERETOSELECTDOC));						// over unselected doc
				if (!TextValid)
					ReturnValue=FALSE;
				PrefixSelDesc=STATUSLINE_SELDESC_OTHERDOC;
			}
		}
		else
		{
PORTNOTE("statline", "Removed use of ControlHelper")
#if !defined(EXCLUDE_FROM_XARALX)
			TextValid=ControlHelper::GetStatusLineText(&text,WinID);				// try buttons/bars
			if (TextValid)
				PrefixSelDesc=STATUSLINE_SELDESC_BUTTONS;
#endif

#ifndef STANDALONE
			if (!TextValid)
				TextValid=CColourBar::GetStatusLineText(&text);						// try ColourBar
			if (TextValid)
				PrefixSelDesc=STATUSLINE_SELDESC_COLBAR;

PORTNOTE("StatusLine", "Removed use of ColourPicker")
#ifndef EXCLUDE_FROM_XARALX
			if (!TextValid)
				TextValid=ColourPicker::GetStatusLineText(&text);					// try Colour Editor
#endif
			if (TextValid)
				PrefixSelDesc=STATUSLINE_SELDESC_COLBAR;
#endif

PORTNOTE("StatusLine", "Removed use of PreviewDlg")
#ifndef EXCLUDE_FROM_XARALX
			if (!TextValid)
				TextValid=PreviewDialog::GetStatusLineText(&text);					// try Preview Dialog
#endif
			if (TextValid)
				PrefixSelDesc=STATUSLINE_SELDESC_PREVIEWDLG;

// WEBSTER - markn 15/1/97
// No rulers in webster
PORTNOTE("StatusLine", "Removed use of Rulers")
#ifndef EXCLUDE_FROM_XARALX
#ifndef WEBSTER
			if (!TextValid)
			{
				DocView*   pDocView   = DocView::GetSelected();
				RulerPair* pRulerPair = NULL;
				if (pDocView!=NULL)
					pRulerPair = pDocView->GetpRulerPair();
				if (pRulerPair!=NULL)
					TextValid = pRulerPair->GetStatusLineText(&text,WndPos,WinID);	// try Rulers
			}
#endif // WEBSTER
#endif
			if (TextValid)
				PrefixSelDesc=STATUSLINE_SELDESC_STATBAR;

PORTNOTE("StatusLine", "Removed use of Galleries")
#ifndef EXCLUDE_FROM_XARALX
#ifndef EXCLUDE_GALS
			if (!TextValid && WinID!=NULL)
			{
				// Lets try the Galleries ....
				// ** This uses Naughty Oily stuff. (Bodge for the Viewer).
				CWindowID ParentWnd = ::GetParent(WinID);
				if (ParentWnd != NULL)
				{
					DialogBarOp* pBar = DialogBarOp::FindDialogBarOp((UINT32)ParentWnd);
					if (pBar != NULL && pBar->IS_KIND_OF(SuperGallery))
					{
						CRect TargetRect;
						::GetWindowRect(WinID, &TargetRect);

						// Get the screen DPI
						INT32 DPI = 96;
						HDC ScreenDC = CreateCompatibleDC(NULL);
						if (ScreenDC == NULL)
							ERROR2(FALSE, "Unable to create screen DC");
						DPI = GetDeviceCaps(ScreenDC, LOGPIXELSY);
						DeleteDC(ScreenDC);

						INT32 WindowHeight = TargetRect.Height();

						DocCoord KernelMousePos;
						KernelMousePos.x = ((WndPos.x) * 72000) / DPI;
						KernelMousePos.y = ((WindowHeight - WndPos.y) * 72000) / DPI;

						TextValid = ((SuperGallery*)pBar)->GetStatusLineHelp(&KernelMousePos, &text);
					}
				}
			}
#endif
#endif
		}

#ifndef STANDALONE
		// blank mouse pos if not over selected doc
		if (!OverSelectedDoc)
			if (UpdateMousePosAndSnap(NULL,NULL,pDocView,FALSE)==FALSE)
				ReturnValue=FALSE;
#endif
	}

	// actually update the text
	if (!TextValid)
		text=_T("");
	if (!UpdateText(&text,PrefixSelDesc))
		ReturnValue=FALSE;

	return ReturnValue;
}



/********************************************************************************************
>	static void StatusLine::SetPrefix(const StringBase& str)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/10/96
	Inputs:		str		---		the string to display as the prefix on the status line
	Purpose:	Overrides the default behaviour of showing the selection state as
				the prefix on the status line.  SetDefaultPrefix restores the default
				behaviour of (usually) showing the selection state as the prefix.
	SeeAlso:	StatusLine::UpdateText; StatusLine::SetDefaultPrefix;
				StatusLine::GetPrefixWidth
********************************************************************************************/

void StatusLine::SetPrefix(const StringBase& str)
{
	m_strPrefix = str;
}


/********************************************************************************************
>	static void StatusLine::SetDefaultPrefix()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/10/96
	Purpose:	Undoes a call to SetPrefix, restoring the default behaviour of showing the
				selection state as a prefix on the status line.
	SeeAlso:	StatusLine::SetPrefix
********************************************************************************************/

void StatusLine::SetDefaultPrefix()
{
	m_strPrefix.Empty();
}


/********************************************************************************************
>	static INT32 StatusLine::GetPrefixWidth()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/10/96
	Returns:	The size of non-default prefix, in pixels.  If using the normal selection state
				prefix then always returns 0.
	SeeAlso:	StatusLine::SetPrefix
********************************************************************************************/

INT32 StatusLine::GetPrefixWidth()
{
PORTNOTE("StatusLine", "Removed use of CCStatusBar")
#ifndef EXCLUDE_FROM_XARALX
	CCStatusBar* pb = GetApplication()->GetpCCStatusBar();
	return pb && !m_strPrefix.IsEmpty() ? 4 + pb->GetTextWidth(&m_strPrefix, TRUE) : 0;
#else
	return 0;
#endif
}

/********************************************************************************************
>	BOOL StatusLine::RestrictStatusLineFunctionsToColourPicker (CWindowID colourPicker)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/99
	Inputs:		colourPicker - The HWND of the claimed colour picker control
	Returns:	TRUE - if successful
				FALSE - otherwise
	Purpose:	When the user is dragging a colour picker control, we need to restrict
				access to status line functionality to only the colour picker
	SeeAlso:	RestoreStatusLineFunctions
********************************************************************************************/

BOOL StatusLine::RestrictStatusLineFunctionsToColourPicker (CWindowID colourPicker)
{
	// firstly, lets check for obvious insanity

PORTNOTE("StatusLine", "Removed use of ColourPicker")
#ifndef EXCLUDE_FROM_XARALX
	String_256 ClassNameStr;  // The control type

	// Find out the class type of the gadget
	GetClassName (colourPicker, (TCHAR*) ClassNameStr, 255);
	
	if (ClassNameStr == String_8(TEXT("cc_colPicker")))
	{
		//if (restrictStatusLineFunctionsToColourPicker == FALSE)
		//{
			restrictStatusLineFunctionsToColourPicker = TRUE;
			return (TRUE);
		//}
	}

	ENSURE(FALSE, "RestrictStatusLineFunctionsToColourPicker called for invalid colour picker control");
#endif
	return (FALSE);
}



/********************************************************************************************
>	BOOL StatusLine::RestrictStatusLineFunctionsToColourPicker (CWindowID colourPicker)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/99
	Inputs:		colourPicker - The HWND of the claimed colour picker control
	Returns:	TRUE - if successful
				FALSE - otherwise
	SeeAlso:	RestoreStatusLineFunctions
********************************************************************************************/

BOOL StatusLine::RestoreStatusLineFunctions (CWindowID colourPicker)
{
	// firstly, lets check for obvious insanity

PORTNOTE("StatusLine", "Removed use of ColourPicker")
#ifndef EXCLUDE_FROM_XARALX
	String_256 ClassNameStr;  // The control type

	// Find out the class type of the gadget
	GetClassName (colourPicker, (TCHAR*) ClassNameStr, 255);
	
	if (ClassNameStr == String_8(TEXT("cc_colPicker")))
	{
		//if (restrictStatusLineFunctionsToColourPicker == TRUE)
		//{
			restrictStatusLineFunctionsToColourPicker = FALSE;
			return (TRUE);
		//}
	}

	ENSURE(FALSE, "RestoreStatusLineFunctions called for invalid colour picker control");
#endif
	return (FALSE);
}


/*******************************************************************************
>	BOOL StatusLine::UpdateText(String_256* pText, BOOL PrefixSelDesc=TRUE)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Purpose:	update the status line with the given text,
				prefixed with the selection description (if valid to do so)
	Inputs:		pText         - pointer to text for status line
				PrefixSelDesc - prefix text with selection description if possible (if TRUE)
								(this parameter is ignored if the default prefix
								has been overridden by StatusLine::SetPrefix
	Returns:	FALSE if fails (see Erros)
	Errors:		pCCStatusBar==NULL
				pCCStatusBar->UpdatePaneText() fails,
				pCCStatusBar->GetPaneRect() fails,
				pCCStatusBar->GetTextWidth() fails,
				TruncateTextToWidth() fails
*********************************************************************************/

extern BOOL SafeAppendString(StringBase*, const StringBase&, BOOL = TRUE);

BOOL StatusLine::UpdateText(String_256* pText, BOOL PrefixSelDesc)
{
//	FixFPControlRegister();

#ifdef STANDALONE
	PrefixSelDesc = FALSE;	// Never show Selection Description in Viewer
#endif

	// create a string by concatenating sel desc and help text (as required)
	String_256 text("");
	SelRange* pSelection = 0;

	if (m_strPrefix.IsEmpty())
	{
		if (PrefixSelDesc)
		{
			// Show the selection state.

			// DMc 19/5/99
			// Alter the range control before we do the describe
			pSelection = GetApplication()->FindSelection();
			
			if (pSelection) text = pSelection->Describe(STATUS_BAR);
		}
	}
	else
	{
		// Show the caller-defined prefix.
		text = m_strPrefix;
	}

	if (text.Length()>0) SafeAppendString(&text, SelDescSuffix, FALSE);
	if (pText) SafeAppendString(&text, *pText, FALSE);

	// re-sample time and update status line
	TextTimer.Sample();
	
	if ((IsRestrictedAccessToColourPicker () == FALSE) && (DoControlHelp == TRUE))
	{
		return SetStatusText(text);
	}
	else
	{
		return (TRUE);
	}

	return FALSE;
}

BOOL StatusLine::UpdateTextForColourPicker(String_256* pText, BOOL PrefixSelDesc)
{
//	FixFPControlRegister();

#ifdef STANDALONE
	PrefixSelDesc = FALSE;	// Never show Selection Description in Viewer
#endif

	ERROR2IF(restrictStatusLineFunctionsToColourPicker==FALSE,FALSE,"StatusLine::UpdateColourPickerText() - require TRUE to use this function!");

	// create a string by concatenating sel desc and help text (as required)
	String_256 text("");
	SelRange* pSelection = 0;

	RangeControl rg;

	if (m_strPrefix.IsEmpty())
	{
		if (PrefixSelDesc)
		{
			// Show the selection state.

			// DMc 19/5/99
			// Alter the range control before we do the describe
			pSelection = GetApplication()->FindSelection();
			
			if (pSelection) text = pSelection->Describe(STATUS_BAR);
		}
	}
	else
	{
		// Show the caller-defined prefix.
		text = m_strPrefix;
	}

	if (text.Length()>0) SafeAppendString(&text, SelDescSuffix, FALSE);
	if (pText) SafeAppendString(&text, *pText, FALSE);

	// re-sample time and update status line
	TextTimer.Sample();
	
	return SetStatusText(text);
}


/*****************************************************************************
>	BOOL StatusLine::TruncateTextToWidth(String_256* pText, INT32 width)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/1/95
	Purpose:	if too long for the given width,
				truncate the given text at one of the specified delimiters
	Inputs:		pText - pointer to original text
				width - width to truncate text to
	Outputs:	pText - pointer to the (truncated) text
	Returns:	FALSE if anything fails (see Errors)
	Errors:		pCCStatusBar==NULL,
				pText==NULL,
				pCCStatusBar->GetTextWidth() fails,
*****************************************************************************/

BOOL StatusLine::TruncateTextToWidth(String_256* pText, INT32 width)
{
	ERROR2IF(       pText==NULL,FALSE,"StatusLine::TruncateTextToPane() - pText==NULL");

	// try truncating text at each successive point until it no longer fits the pane then
	// use the last one that fitted (if text null or no truncation points use whole text)
	INT32 LastTruncPoint=pText->Length();
	String_256 text("");
	
	INT32 TruncPoint=pText->Sub(SelDescSuffix);
	if (TruncPoint==-1)
		TruncPoint=pText->Length();
	else
		TruncPoint+=SelDescSuffix.Length();
	pText->Left(&text,TruncPoint);

PORTNOTE("StatusLine", "Removed use of CCStatusBar")
#ifndef EXCLUDE_FROM_XARALX
	INT32 TextWidth=0;
	while ((TextWidth=pCCStatusBar->GetTextWidth(&text))<=width)
	{
		ERROR2IF(TextWidth==-1,FALSE,"StatusLine::TruncateTextToPane() - pCCStatusBar->GetTextWidth() failed");
		LastTruncPoint=TruncPoint;
		if (LastTruncPoint==pText->Length())
			break;
		TruncPoint=pText->Sub(TruncChar,LastTruncPoint+1);
		if (TruncPoint==-1)
			TruncPoint=pText->Length();
		pText->Left(&text,TruncPoint);
	}
#endif

	pText->Left(pText,LastTruncPoint);

	return TRUE;
}


/*****************************************************************************
>	BOOL StatusLine::GetMousePosText(String_256 pText, DocCoord MousePos, Spread* pSpread, INT32 dp = -1)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Purpose:	get mouse position as a string
	Inputs:		MousePos - position of mouse in DocCoords
				pSpread  - pointer to spread mouse is over
	Outputs:	pText    - mouse position as text
	Returns:	FALSE if fails (see Errors)
	Errors:     pCCStatusBar==NULL
				pSpread==NULL
				Spread::SpreadCoordToText() fails
*****************************************************************************/

BOOL StatusLine::GetMousePosText(String_256* pText, DocCoord MousePos, Spread* pSpread, INT32 dp)
{
	ERROR2IF(       pText==NULL,FALSE,"StatusLine::GetMousePosText() - pText==NULL");
	ERROR2IF(     pSpread==NULL,FALSE,"StatusLine::GetMousePosText() - pSpread==NULL");

	if (dp == -1)
		dp = MousePosDP;

	String_256 yPos("");
	if (pSpread->SpreadCoordToText(pText,&yPos,MousePos,FALSE,TRUE,dp)==FALSE)
		return FALSE;

	*pText+=String_64(_R(IDS_K_STATLINE_POSITIONSEP));
	*pText+=yPos;
	return TRUE;
}


/*****************************************************************************
>	BOOL StatusLine::SetMousePosPaneWidth(Spread* pSpread)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/94
	Purpose:	Set the mouse position pane width to be large enough to
				hold any coord in the spread in the current preference units
				Also gets, and caches, the number of decimal places required to resolve 1 pixel
	Inputs:		pSpread  - pointer to spread mouse is over
	Returns:	FALSE if fails (see Errors)
	Errors:     pCCStatusBar==NULL
				pSpread==NULL
				Spread::GetDecimalPlacesForPixelResolution() fails
				GetMousePosText() fails
				pCCStatusBar->GetTextWidth() fails
				pCCStatusBar->SetPaneWidth() fails
*****************************************************************************/

BOOL StatusLine::SetMousePosPaneWidth(Spread* pSpread)
{	
	ERROR2IF(     pSpread==NULL,FALSE,"StatusLine::SetMousePosPaneWidth() - pSpread==NULL");

	// find/cache number of decimal places required to resolve a pixel with current units & scale
	INT32 dp=2;
	INT32 ReturnValue=TRUE;
	if (pSpread->GetDecimalPlacesForPixelResolution(DocView::GetSelected(), &dp))
		MousePosDP=dp;
	else
		ReturnValue=FALSE;

	// get dimensions of spread PasteRect converted (in DocCoords)
	DocRect  PasteRect=pSpread->GetPasteboardRect();
	DocCoord PasteSize(PasteRect.Width(),PasteRect.Height());

	// get min coord as text, then find it's width
	String_256 MousePosText("");
	if (GetMousePosText(&MousePosText,DocCoord(0,0),pSpread,0)==FALSE)
		return FALSE;

	String_256 DPSChar(_R(IDS_CONVERT_DP_CHAR));

	String_256 ZeroChar(_R(IDS_CONVERT_ZERO_CHAR));

	INT32 i;
	for (i=0;i<dp;i++)
	{
		MousePosText += ZeroChar;
		MousePosText += ZeroChar;

	}

	MousePosText += DPSChar;
	MousePosText += DPSChar;

PORTNOTE("StatusLine", "Removed use of CCStatusBar")
#ifndef EXCLUDE_FROM_XARALX
	INT32 MinCoordTextWidth=pCCStatusBar->GetTextWidth(&MousePosText);
#else
	INT32 MinCoordTextWidth=100;
#endif
	
	if (MinCoordTextWidth==-1)
		return FALSE;
    
	// get max coord as text, then find it's width
	if (GetMousePosText(&MousePosText,PasteSize,pSpread,0)==FALSE)
		return FALSE;

	for (i=0;i<dp;i++)
	{
		MousePosText += ZeroChar;
		MousePosText += ZeroChar;
	}

	MousePosText += DPSChar;
	MousePosText += DPSChar;
	
PORTNOTE("StatusLine", "Removed use of CCStatusBar")
#ifndef EXCLUDE_FROM_XARALX
	INT32 MaxCoordTextWidth=pCCStatusBar->GetTextWidth(&MousePosText) ; 
#else
	INT32 MaxCoordTextWidth=100;
#endif

	if (MaxCoordTextWidth==-1)
		return FALSE;
	
// set pane width
PORTNOTE("StatusLine", "Removed use of CCStatusBar")
#ifndef EXCLUDE_FROM_XARALX
	INT32 TextWidth = max(MaxCoordTextWidth,MinCoordTextWidth);
	if (pCCStatusBar->SetPaneWidth(TextWidth,_R(IDS_SL_MOUSEPOS))==FALSE)
		return FALSE;
#endif
	return ReturnValue;
}


/*****************************************************************************
>	MsgResult StatusLine::Message(Msg* pMsg)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/12/94
	Purpose:	handle system messages
	Inputs:		pMsg - pointer to message
	Returns:	OK, EAT_MSG, FAIL (see Errors)
*****************************************************************************/

MsgResult StatusLine::Message(Msg* pMsg)
{
//	FixFPControlRegister();
	ERROR2IF(        pMsg==NULL,FAIL,"StatusLine::Message() - pMsg==NULL");

	if (IS_OUR_DIALOG_MSG(pMsg))
	{
		DialogMsg* Msg = (DialogMsg*)pMsg;
	
		switch(Msg->DlgMsg)
		{
			case DIM_CREATE:
			{
				if (s_pStatusLine)
				{
					ERROR3("Two status lines - that isn't meant to happen");
					delete s_pStatusLine;
				}
				s_pStatusLine = this;
				SetGadgetBitmap(_R(IDB_SL_SNAP), _R(IDB_SL_SNAPN)); // turn snap off by default
			}

			default:
				break;

		}
		// Else fall through
	}



	// if pref units changed, or zoom changed flag mouse pos pane needs resizing
	if (MESSAGE_IS_A(pMsg,OptionsChangingMsg) /* && ((OptionsChangingMsg*)pMsg)->State==OptionsChangingMsg::NEWUNITS */
	||  MESSAGE_IS_A(pMsg,DocChangingMsg)        && ((DocChangingMsg*)pMsg)->State==DocChangingMsg::SELCHANGED
	||  MESSAGE_IS_A(pMsg,DocViewMsg)            && ((DocViewMsg*)pMsg)->State==DocViewMsg::SCALECHANGED)
		MousePosPaneNeedsResizing=TRUE;

	// if doc changing flag transparency needs updating & mouse pos pane needs resizing
	if (MESSAGE_IS_A(pMsg,DocChangingMsg) && ((DocChangingMsg*)pMsg)->State==DocChangingMsg::SELCHANGED)
	{
		//TransparencyNeedsUpdating=TRUE; 
		MousePosPaneNeedsResizing=TRUE;
	}

	if (MESSAGE_IS_A(pMsg, DocViewMsg) && ((DocViewMsg*)pMsg)->State==DocViewMsg::SELCHANGED)
	{
		CCamFrame* pMainFrame = GetMainFrame();
		if (pMainFrame)
		{
			DocView* pView = ((DocViewMsg*)pMsg)->pNewDocView;
			BOOL bState = TRUE;
			String_256 sPlateName;
			if (pView)
			{
				ColourPlateType PlateType = COLOURPLATE_NONE;
				ColourContext *cc = pView->GetColourContext(COLOURMODEL_RGBT);
				if (cc && cc->GetColourPlate())
				{
					PlateType = cc->GetColourPlate()->GetType();
				}

				switch (PlateType)
				{
				case COLOURPLATE_CYAN:
					sPlateName.Load(_R(IDS_PLATENAME_CYAN));
					break;

				case COLOURPLATE_MAGENTA:
					sPlateName.Load(_R(IDS_PLATENAME_MAGENTA));
					break;

				case COLOURPLATE_YELLOW:
					sPlateName.Load(_R(IDS_PLATENAME_YELLOW));
					break;

				case COLOURPLATE_KEY:
					sPlateName.Load(_R(IDS_PLATENAME_KEY));
					break;

				case COLOURPLATE_COMPOSITE:
					sPlateName.Load(_R(IDS_PLATENAME_COMPOSITE));
					break;

				case COLOURPLATE_SPOT:
					{
						IndexedColour* pSpotColour = cc->GetColourPlate()->GetSpotColour();
						if (pSpotColour)
							sPlateName = *(pSpotColour->GetName());
					}
					break;

				default:
					bState = FALSE;
					break;
				}
			}
			else
			{
				bState = FALSE;
			}
PORTNOTE("printing", "Removed SetPrinterColourStatus");
#ifndef EXCLUDE_FROM_XARALX
			pMainFrame->SetPrinterColourStatus(bState, &sPlateName);
#endif
		}

	}

	return StandardBar::Message(pMsg);
}


/*******************************************************************************************************
>	BOOL StatusLine::UpdateMousePosAndSnap(DocCoord* pDocCoord, Spread* pSpread,
										   DocView* pDocView,   BOOL Snapped)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/1/95
	Inputs:		pDocoord - 
				pSpread  - 
				pDocView - 
				Snapped  - 
	Returns:	FALSE if anything fails (see Errors)
	Purpose:	Update mouse position pane, and snapped indicator
				If pDocCoord==NULL && pSpread==NULL && Snapped==FALSE, the mouse pos will be blanked
********************************************************************************************************/
BOOL StatusLine::UpdateMousePosAndSnap(DocCoord* pDocCoord, Spread* pSpread,
									   DocView* pDocView,   BOOL Snapped)
{
#ifndef STANDALONE

	if (MousePosPaneNeedsResizing)
		return TRUE;

	BOOL Blank = (pDocCoord==NULL && pSpread==NULL && Snapped==FALSE);
	if (!Blank)
	{
		ERROR2IF(     pSpread==NULL,FALSE,"StatusLine::UpdateMousePosAndSnap() - pSpread==NULL");
		ERROR2IF(   pDocCoord==NULL,FALSE,"StatusLine::UpdateMousePosAndSnap() - pDocCoord==NULL");
	}

	BOOL ReturnValue=TRUE;

	// update mouse position pane if over selected doc, else blank it
	String_256 MousePosText("");
	if (!Blank && GetMousePosText(&MousePosText,*pDocCoord,pSpread)==FALSE)
		ReturnValue=FALSE;
	ReturnValue &=SetStringGadgetValue(_R(IDC_SL_MOUSEPOS), MousePosText);
	PaintGadgetNow(_R(IDC_SL_MOUSEPOS));

// WEBSTER - markn 15/1/97
// No rulers in Webster
PORTNOTE("StatusLine", "Removed use of Rulers")
#ifndef EXCLUDE_FROM_XARALX
#ifndef WEBSTER
	// and update ruler mouse followers
	DocView* pView = DocView::GetSelected();
	if (pView!=NULL)
	{
		RulerPair* pRulerPair = pView->GetpRulerPair();
		if (pRulerPair!=NULL)
			ReturnValue = pRulerPair->UpdateMouseFollowers(pDocCoord);
	}
#endif // WEBSTER
#endif

//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	// if snap indicator not reflecting the desired state, update it.
	if (Snapped && (GetGadgetBitmap(_R(IDB_SL_SNAP)) != _R(IDB_SL_SNAP)))
	{
		SetGadgetBitmap(_R(IDB_SL_SNAP), _R(IDB_SL_SNAP));
		PaintGadgetNow(_R(IDB_SL_SNAP));
	}
	if (!Snapped && (GetGadgetBitmap(_R(IDB_SL_SNAP)) != _R(IDB_SL_SNAPN)))
	{
		SetGadgetBitmap(_R(IDB_SL_SNAP), _R(IDB_SL_SNAPN));
		PaintGadgetNow(_R(IDB_SL_SNAP));
	}
#endif	
	
	// if snap pointer not reflecting the desired state, update it.
	// (The snap pointer is only shown when snapped and during a 'SnappingDrag' drag)
	Operation* pDragOp = Operation::GetCurrentDragOp();
	if (Snapped && pDragOp!=NULL && pDragOp->SnappingDrag())
	{
		// Snapped and dragging so show pointer shape if not already shown
		if (SnappedCursorID==0)
		{
			SnappedCursorID = CursorStack::GPush(pSnappedCursor);
		}
	}
	else
	{
		// Not snapped or dragging so hide pointer shape if it was shown
		if (SnappedCursorID!=0)
		{
			CursorStack::GPop(SnappedCursorID);
			SnappedCursorID = 0;	// flag cursor no longer on stack
		}
	}

	// and mouse position no longer needs updating
	MousePosNeedsUpdatingFlag=FALSE;
	return ReturnValue;

#else
	return TRUE;
#endif
}



/*******************************************************************************************************
>	BOOL StatusLine::UpdateSolidDragIndicator(BOOL bSolidAllowed, BOOL bSolid)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/12/2003
	Inputs:		bSolid  - Is Solid mode turned on?
	Returns:	FALSE if anything fails (see Errors)
	Purpose:	Update mouse position pane, and snapped indicator
				If pDocCoord==NULL && pSpread==NULL && Snapped==FALSE, the mouse pos will be blanked
********************************************************************************************************/
BOOL StatusLine::UpdateSolidDragIndicator(BOOL bSolidAllowed, BOOL bSolid)
{
//	if (MousePosPaneNeedsResizing)
//		return TRUE;

	BOOL ReturnValue=TRUE;

	// if drag indicator not reflecting the desired state, update it.
PORTNOTE("StatusLine", "Removed use of CCStatusBar")
#ifndef EXCLUDE_FROM_XARALX
	FlagState DragState = pCCStatusBar->PaneState(_R(IDS_SL_SDRAG));
	if (bSolid && DragState==Disable)
	{
		if (pCCStatusBar->PaneState(_R(IDS_SL_SDRAG), Enable)==Fail)
			ReturnValue=FALSE;

		// force a redraw of the drag pane
		pCCStatusBar->UpdatePaneBitmap(_R(IDS_SL_SDRAG),0, TRUE);
	
	}
	if (!bSolid && DragState!=Disable)
	{
		if (pCCStatusBar->PaneState(_R(IDS_SL_SDRAG), Disable)==Fail)
			ReturnValue=FALSE;

		// force a redraw of the snap pane
		pCCStatusBar->UpdatePaneBitmap(_R(IDS_SL_SDRAG),0, TRUE);
	}
#endif
	
	return ReturnValue;
}


/*****************************************************************************
>	BOOL StatusLine::SetRenderIndicator(RenderState Action)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95
	Purpose:	Change state of the render indicator
	Inputs:		Action - Rendering	  display 'Rendering'    indicator	
						 Paused		  display 'Paused'       indicator	
						 NotRendering display 'NotRendering' indicator (reset animation)
						 Animate	  display char in animation 
									  (if sufficient time elapsed, advance animation)
	Returns:	FALSE if anything fails (see Errors)
	Errors:		pCCStatusBar==NULL,
				Action invalid,
				CStatusBar::UpdatePaneText() fails,
				StringBase::Load() fails
*****************************************************************************/

BOOL StatusLine::SetRenderIndicator(RenderState Action)
{
	INT32  RenderAnimationStates=sizeof(RenderAnimation)/sizeof(UINT32);
	UINT32 BitmapID=0;
	switch (Action)
	{
		case NotRendering:
			BitmapID=_R(IDB_SL_RENDN);
			RenderAnimationState=0;
			break;
		case Paused:
			BitmapID=_R(IDB_SL_RENDP);
			break;
		case Rendering:
			BitmapID=_R(IDB_SL_RENDR);
			RenderAnimationState=0;
			break;
		case Animate:
			if (RenderTimer.Elapsed(RenderDelay))
			{
				if (RenderAnimationState<0 || RenderAnimationState>=RenderAnimationStates)
					RenderAnimationState=0;
				BitmapID=RenderAnimation[RenderAnimationState++];
			}
			break;
		default: ERROR2(FALSE,"StatusLine::SetRenderIndicator() - Action invalid");
	}

	if (!BitmapID)
		return TRUE;

	RenderTimer.Sample();

	if (BitmapID == GetGadgetBitmap(_R(IDB_SL_RENDN)))
		return TRUE;
	SetGadgetBitmap(_R(IDB_SL_RENDN), BitmapID);
	PaintGadgetNow(_R(IDB_SL_RENDN));

	return TRUE;
}

/********************************************************************************************
>	BOOL StatusLine::ShowProgress (BOOL Show=TRUE, StringBase *JobDescrip = NULL)

	Author:		Alex Bligh
	Created:	09/05/2006
	Inputs:		BOOL Show : TRUE to show, else false
				JobDescription - A BRIEF string describing the job currently being undertaken.
						This will be displayed on the progress bar if possible
	Outputs:	-
	Returns:	TRUE if the initialisation of the progress bar was successful.
	Purpose:	Creates a window and associates it with this CProgressBar object. The window
				appears immediately over the status bar.
	Notes:		This currently assumes that it'll only ever be opened at the bottom of the
				Main Frame window.
	SeeAlso:	StatusLine::SetPercent; StatusLine::GetPercent
********************************************************************************************/

BOOL StatusLine::ShowProgress (BOOL Show, StringBase *JobDescrip)
{
	BOOL ShowChanged=(Show != ProgressShown);

	String_8 EmptyString(_T(""));

	// Make "NULL" mean "empty string"
	if (!JobDescrip)
		JobDescrip=&EmptyString;

	BOOL StringChanged=!JobDescription || (*JobDescription!=*JobDescrip);

	// Return if nothing changed
	if (!ShowChanged && !StringChanged)
		return TRUE;

	if (ShowChanged)
	{
		HideGadget(_R(IDC_SL_PROGRESSGAUGE), !Show);
		HideGadget(_R(IDC_SL_PROGRESSPERCENT), !Show);

		ProgressShown=Show;

		CurrentPercent=-1;		// Force a redraw
		SetPercent(0, TRUE, JobDescrip);	// Force redraw of window _including background_; always refresh string

		return TRUE;
	}

	return SetPercent(CurrentPercent, StringChanged, StringChanged?JobDescrip:NULL);
}


/********************************************************************************************
>	BOOL StatusLine::SetPercent(INT32 NewPercent,
									BOOL ClearBackground = FALSE, StringBase *JobDescrip = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/94
	Inputs:		NewPercent - the new percentage value to be displayed by the progress bar

				ClearBackground - Clears the entire bar background and redraws everything, rather
				than doing a (far more efficient) update of the bar/percentage alone. Use the default
				of FALSE unless absolutely necessary!

				JobDescrip - NULL, or a pointer to a new job description - pass NULL if this hasn't
				changed, as it makes a copy of the new string every time it is changed.	If this is
				non-NULL, then the ClearBackground flag will be forced to TRUE to draw the new text.

	Outputs:	-
	Returns:	TRUE if it successfully makes the change.
	Purpose:	Sets the currently displayed percentage of the progress bar. The bar
				will be immediately redrawn to reflect the new value. Values outside the
				range 0..99 (inclusive) are clipped to 0 or 99. No redraw will be done if
				NewPercent equals the currently displayed value.
	SeeAlso:	CProgressBar::GetPercent; CProgressBar::Create
********************************************************************************************/

BOOL StatusLine::SetPercent(INT32 NewPercent, BOOL ClearBackground /* =FALSE */,
								StringBase *JobDescrip /* =NULL */)
// The extra argument ClearBackground is not mentioned in the help, because it
// is used internally - When the window is first created, the background must be cleared,
// but on normal updates this causes horrible flicker, so is to be avoided.
{
	if (NewPercent < 0) NewPercent = 0;
	if (NewPercent > 99) NewPercent = 99;

	// If there's no change, don't bother updating
	if (JobDescrip == NULL && NewPercent == CurrentPercent)
		return(TRUE);

	CurrentPercent = NewPercent;

	String_64 PercentString;
	PercentString.MakePercent(CurrentPercent);
	SetStringGadgetValue(_R(IDC_SL_PROGRESSPERCENT), PercentString);
	SetLongGadgetValue(_R(IDC_SL_PROGRESSGAUGE), CurrentPercent);

	// If there is a new Job Description, change to use it
	if (JobDescrip != NULL)
	{
		delete JobDescription;
		JobDescription = new String_256(*JobDescrip);

		if (JobDescription)
		{
			SetStringGadgetValue(_R(IDC_SL_STATUSTEXT), ProgressShown?*JobDescription:StatusText);
		}
		else
		{
			SetStringGadgetValue(_R(IDC_SL_STATUSTEXT), ProgressShown?String_256(_T("")):StatusText);
		}

		InvalidateGadget(0, TRUE); // Repaint the whole bar
		PaintGadgetNow(0);
	}
	else
	{
		// Just repaint the gauge
		InvalidateGadget(_R(IDC_SL_PROGRESSGAUGE), ClearBackground);
		InvalidateGadget(_R(IDC_SL_PROGRESSPERCENT), ClearBackground);
		PaintGadgetNow(_R(IDC_SL_PROGRESSGAUGE));
		PaintGadgetNow(_R(IDC_SL_PROGRESSPERCENT));
	}

	return(TRUE);
}

/********************************************************************************************
>	BOOL StatusLine::SetStatusText(const String_256 &text)

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	09/05/2006
	Inputs:		text - the text to set it to

	Outputs:	-
	Returns:	TRUE if it successfully makes the change.
	Purpose:	Sets the status line text. Records it. Puts it in the gadget and invalidates
				it if the gadget is shown
	SeeAlso:	CProgressBar::GetPercent; CProgressBar::Create

********************************************************************************************/

BOOL StatusLine::SetStatusText(const String_256 &text)
{
	StatusText = text;
	if (!ProgressShown)
	{
		StatusText = text;
		BOOL ok=SetStringGadgetValue(_R(IDC_SL_STATUSTEXT), text);
		PaintGadgetNow(_R(IDC_SL_STATUSTEXT));
		return ok;
	}
	return TRUE;
}
